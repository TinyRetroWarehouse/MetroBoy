#include "GateBoyLib/GateBoy.h"
#include <memory.h>
#include <stdio.h>

#include "CoreLib/Constants.h"
#include "CoreLib/Tests.h"
#include "GateBoyLib/Probe.h"
#include "GateBoyLib/GateBoyState.h"

//-----------------------------------------------------------------------------

GBResult GateBoy::reset_to_poweron(const blob& cart_blob) {
  gb_state.reset_to_poweron();
  cpu.reset_to_poweron();
  mem.reset_to_poweron();
  sys.reset_to_poweron();
  pins.reset_to_poweron();
  probes.reset_to_poweron();
  return GBResult::ok();
}

//-----------------------------------------------------------------------------

GBResult GateBoy::run_poweron_reset(const blob& cart_blob, bool fastboot) {
  sys.fastboot = fastboot;

  //----------------------------------------
  // Update the sim without ticking the clock to to settle initial reset signals.

  sys.rst = 1;

  tock_cpu();
  tock_gates(cart_blob);
  gb_state.commit();
  pins.commit();

  //----------------------------------------
  // Release reset, start clock, and sync with phase

  sys.rst = 0;
  sys.clk_en = 1;
  sys.clk_good = 1;
  run_phases(cart_blob, 2);

  CHECK_N(bit(gb_state.sys_clk.AFUR_xxxxEFGH.qp_old()));
  CHECK_P(bit(gb_state.sys_clk.ALEF_AxxxxFGH.qp_old()));
  CHECK_P(bit(gb_state.sys_clk.APUK_ABxxxxGH.qp_old()));
  CHECK_P(bit(gb_state.sys_clk.ADYK_ABCxxxxH.qp_old()));

  sys.gb_phase_total = 0;
  
  //----------------------------------------
  // Wait for SIG_CPU_START

  while(bit(~gb_state.sys_rst.SIG_CPU_STARTp.out_old())) {
    run_phases(cart_blob, 8);
  }

  //----------------------------------------
  // Delay to sync up with expected div value

  run_phases(cart_blob, 16);

  //----------------------------------------
  // Fetch the first instruction in the bootrom

  dbg_read(cart_blob, 0x0000);

  //----------------------------------------
  // We're ready to go, release the CPU so it can start running the bootrom.

  sys.clk_req = 1;
  sys.cpu_en = true;

  if (fastboot) {
    gb_state.reg_div.TERO_DIV03p.state = 0b00011010;
    gb_state.reg_div.UNYK_DIV04p.state = 0b00011010;
    gb_state.reg_div.UPOF_DIV15p.state = 0b00011011;
  }
  return GBResult::ok();
}

//-----------------------------------------------------------------------------

GBResult GateBoy::reset_to_bootrom(const blob& cart_blob) {
  gb_state.reset_to_bootrom();
  cpu.reset_to_bootrom();
  mem.reset_to_bootrom();
  sys.reset_to_bootrom();
  pins.reset_to_bootrom();
  probes.reset_to_bootrom();
  return GBResult::ok();
}

//-----------------------------------------------------------------------------

GBResult GateBoy::reset_to_cart(const blob& cart_blob) {
  reset_to_bootrom(cart_blob);
  gb_state.reset_to_cart();
  cpu.reset_to_cart();
  mem.reset_to_cart();
  sys.reset_to_cart();
  pins.reset_to_cart();
  probes.reset_to_cart();
  return GBResult::ok();
}

//-----------------------------------------------------------------------------

GBResult GateBoy::peek(int addr) const {
  //if (addr >= 0x0000 && addr <= 0x7FFF) { return cart_blob.data()[addr - 0x0000]; }
  if (addr >= 0x8000 && addr <= 0x9FFF) { return GBResult(mem.vid_ram[addr - 0x8000]);  }
  if (addr >= 0xA000 && addr <= 0xBFFF) { return GBResult(mem.cart_ram[addr - 0xA000]); }
  if (addr >= 0xC000 && addr <= 0xDFFF) { return GBResult(mem.int_ram[addr - 0xC000]);  }
  if (addr >= 0xE000 && addr <= 0xFDFF) { return GBResult(mem.int_ram[addr - 0xE000]);  }
  if (addr >= 0xFE00 && addr <= 0xFEFF) { return GBResult(mem.oam_ram[addr - 0xFE00]);  }
  if (addr >= 0xFF80 && addr <= 0xFFFE) { return GBResult(mem.zero_ram[addr - 0xFF80]); }
  return gb_state.peek(addr);
}

GBResult GateBoy::poke(int addr, uint8_t data_in) {
  //if (addr >= 0x0000 && addr <= 0x7FFF) { cart_blob.data()[addr - 0x0000] = data_in; return data_in; }
  if (addr >= 0x8000 && addr <= 0x9FFF) { mem.vid_ram[addr - 0x8000] = data_in;  return GBResult::ok(); }
  if (addr >= 0xA000 && addr <= 0xBFFF) { mem.cart_ram[addr - 0xA000] = data_in; return GBResult::ok(); }
  if (addr >= 0xC000 && addr <= 0xDFFF) { mem.int_ram[addr - 0xC000] = data_in;  return GBResult::ok(); }
  if (addr >= 0xE000 && addr <= 0xFDFF) { mem.int_ram[addr - 0xE000] = data_in;  return GBResult::ok(); }
  if (addr >= 0xFE00 && addr <= 0xFEFF) { mem.oam_ram[addr - 0xFE00] = data_in;  return GBResult::ok(); }
  if (addr >= 0xFF80 && addr <= 0xFFFE) { mem.zero_ram[addr - 0xFF80] = data_in; return GBResult::ok(); }
  return gb_state.poke(addr, data_in);
}

//-----------------------------------------------------------------------------

GBResult GateBoy::dbg_req(uint16_t addr, uint8_t data, bool write) {
  cpu.bus_req_new.addr = addr;
  cpu.bus_req_new.data = data;
  cpu.bus_req_new.read = !write;
  cpu.bus_req_new.write = write;
  return GBResult::ok();
}

//-----------------------------------------------------------------------------

GBResult GateBoy::dbg_read(const blob& cart_blob, int addr) {
  CHECK_P((sys.gb_phase_total & 7) == 0);

  Req old_req = cpu.bus_req_new;
  bool old_cpu_en = sys.cpu_en;
  sys.cpu_en = false;

  dbg_req((uint16_t)addr, 0, 0);
  run_phases(cart_blob, 8);

  cpu.bus_req_new = old_req;
  sys.cpu_en = old_cpu_en;

  return GBResult(cpu.cpu_data_latch);
}

//-----------------------------------------------------------------------------

GBResult GateBoy::dbg_write(const blob& cart_blob, int addr, uint8_t data) {
  CHECK_P((sys.gb_phase_total & 7) == 0);

  Req old_req = cpu.bus_req_new;
  bool old_cpu_en = sys.cpu_en;
  sys.cpu_en = false;

  dbg_req((uint16_t)addr, data, 1);
  run_phases(cart_blob, 8);

  cpu.bus_req_new = old_req;
  sys.cpu_en = old_cpu_en;
  return GBResult::ok();
}

//-----------------------------------------------------------------------------

GBResult GateBoy::run_phases(const blob& cart_blob, int phase_count) {
  GBResult res = GBResult::ok();
  for (int i = 0; i < phase_count; i++) {
    res &= next_phase(cart_blob);
  }
  return res;
}

//-----------------------------------------------------------------------------

GBResult GateBoy::next_phase(const blob& cart_blob) {
  probes.begin_pass((sys.gb_phase_total + 1) & 7);
  sys.gb_phase_total++;

  tock_cpu();

  tock_gates(cart_blob);
  gb_state.commit();
  pins.commit();

  if (config_idempotence) {
    auto gb_state_old = gb_state;

    tock_gates(cart_blob);
    gb_state.commit();
    pins.commit();

    if (gb_state.diff(gb_state_old, 0xFF)) {
      LOG_R("idempotence fail!\n");
      debugbreak();
    }
  }

  update_framebuffer();
  probes.end_pass();
  return GBResult::ok();
}

//#pragma warning(disable:4127) // conditional expression is constant

//-----------------------------------------------------------------------------

void GateBoy::update_framebuffer() {
  int lcd_x = bit_pack(gb_state.pix_count) - 8;
  int lcd_y = bit_pack(gb_state.reg_ly);
  int DATA0 = pins.lcd.PIN_51_LCD_DATA0.qp_ext_old();
  int DATA1 = pins.lcd.PIN_50_LCD_DATA1.qp_ext_old();

  if (lcd_y >= 0 && lcd_y < 144 && lcd_x >= 0 && lcd_x < 160) {
    wire p0 = bit(DATA0);
    wire p1 = bit(DATA1);
    auto new_pix = p0 + p1 * 2;

    mem.framebuffer[lcd_x + lcd_y * 160] = uint8_t(3 - new_pix);
  }

#if 0
  if (bit(~lcd.old_lcd_clock.qp_old()) && lcd.PIN_53_LCD_CLOCK.qp_new()) {
    gb_screen_x++;
  }
  if (lcd.PIN_54_LCD_HSYNC.qp_new() || lcd.PIN_55_LCD_LATCH.qp_new()) {
    gb_screen_x = 0;
  }

  if (bit(~lcd.old_lcd_latch.qp_old()) && lcd.PIN_55_LCD_LATCH.qp_new()) {
    if (gb_screen_y < 144) {
      for (int x = 0; x < 159; x++) {
        uint8_t p0 = lcd.lcd_pipe_lo[x + 1].qp_new();
        uint8_t p1 = lcd.lcd_pipe_hi[x + 1].qp_new();
        framebuffer[x + gb_screen_y * 160] = p0 + p1 * 2;
      }
      {
        uint8_t p0 = lcd.lcd_pix_lo.qp_new();
        uint8_t p1 = lcd.lcd_pix_hi.qp_new();
        framebuffer[159 + gb_screen_y * 160] = p0 + p1 * 2;
      }
    }

    if (lcd.PIN_57_LCD_VSYNC.qp_new()) {
      gb_screen_y = 0;
    }
    else {
      gb_screen_y++;
    }
  }

  lcd.old_lcd_clock.set_new(lcd.PIN_53_LCD_CLOCK.qp_new());
  lcd.old_lcd_latch.set_new(lcd.PIN_55_LCD_LATCH.qp_new());
#endif
}

//-----------------------------------------------------------------------------

// AB CD EF GH
#define DELTA_EVEN_new  ((sys.gb_phase_total & 1) == 1)
// HA BC DE FG 
#define DELTA_ODD_new  ((sys.gb_phase_total & 1) == 0)

#define DELTA_AB_new   ((sys.gb_phase_total & 7) == 1)
#define DELTA_BC_new   ((sys.gb_phase_total & 7) == 2)
#define DELTA_CD_new   ((sys.gb_phase_total & 7) == 3)
#define DELTA_DE_new   ((sys.gb_phase_total & 7) == 4)
#define DELTA_EF_new   ((sys.gb_phase_total & 7) == 5)
#define DELTA_FG_new   ((sys.gb_phase_total & 7) == 6)
#define DELTA_GH_new   ((sys.gb_phase_total & 7) == 7)
#define DELTA_HA_new   ((sys.gb_phase_total & 7) == 0)

#define CHECK_ODD(A)  CHECK_P(DELTA_ODD_new  || ((reg_old.A.state & 1) == (reg_new.A.state & 1)))
#define CHECK_EVEN(A) CHECK_P(DELTA_EVEN_new || ((reg_old.A.state & 1) == (reg_new.A.state & 1)))

void GateBoy::tock_cpu() {
  cpu.cpu_data_latch &= (uint8_t)bit_pack(gb_state.cpu_dbus);
  cpu.imask_latch = (uint8_t)bit_pack(gb_state.reg_ie);

  if (DELTA_HA_new) {
    if (cpu.core.op == 0x76 && (cpu.imask_latch & cpu.intf_halt_latch)) cpu.core.state_ = 0;
    cpu.intf_halt_latch = 0;
  }

  // +ha -ab -bc -cd -de -ef -fg -gh
  if (DELTA_HA_new) {
    // this one latches funny, some hardware bug
    if (bit(gb_state.reg_if.NYBO_FF0F_D2p.state)) cpu.intf_halt_latch |= INT_TIMER_MASK;
  }

  // -ha +ab -bc
  if (DELTA_AB_new) {
    if (sys.cpu_en) {
      cpu.core.tock_ab(cpu.imask_latch, cpu.intf_latch, cpu.cpu_data_latch);
    }
  }

  if (DELTA_AB_new) {
    if (sys.cpu_en) {
      cpu.bus_req_new.addr = cpu.core._bus_addr;
      cpu.bus_req_new.data = cpu.core._bus_data;
      cpu.bus_req_new.read = cpu.core._bus_read;
      cpu.bus_req_new.write = cpu.core._bus_write;
    }
  }

  // -bc +cd +de -ef -fg -gh -ha -ab
  if (DELTA_DE_new) {
    if (bit(gb_state.reg_if.LOPE_FF0F_D0p.state)) cpu.intf_halt_latch |= INT_VBLANK_MASK;
    if (bit(gb_state.reg_if.LALU_FF0F_D1p.state)) cpu.intf_halt_latch |= INT_STAT_MASK;
    if (bit(gb_state.reg_if.UBUL_FF0F_D3p.state)) cpu.intf_halt_latch |= INT_SERIAL_MASK;
    if (bit(gb_state.reg_if.ULAK_FF0F_D4p.state)) cpu.intf_halt_latch |= INT_JOYPAD_MASK;
  }

  // -ha -ab -bc -cd -de -ef +fg +gh
  if (DELTA_GH_new) {
    cpu.cpu_data_latch = 0xFF;
  }

  // +ha -ab -bc -cd -de -ef -fg +gh
  if (DELTA_GH_new) {
    cpu.intf_latch = (uint8_t)bit_pack(gb_state.reg_if);
  }
}


//-----------------------------------------------------------------------------

void GateBoy::tock_gates(const blob& cart_blob) {
  const GateBoyState  reg_old = gb_state;
  GateBoyState& reg_new = gb_state;

  //----------

  /*#p27.SYLO*/ wire SYLO_WIN_HITn_odd_old = not1(reg_old.win_ctrl.RYDY_WIN_HITp_odd.out_old());
  /*#p24.TOMU*/ wire TOMU_WIN_HITp_odd_old = not1(SYLO_WIN_HITn_odd_old);

  //----------

  wire EXT_vcc = 1;
  wire EXT_gnd = 0;

  /*_SIG_VCC*/ gb_state.SIG_VCC.sig_in(EXT_vcc);
  /*_SIG_GND*/ gb_state.SIG_GND.sig_in(EXT_gnd);

  //----------------------------------------

  memset(&gb_state.cpu_abus, BIT_NEW | BIT_PULLED | 1, sizeof(gb_state.cpu_abus));
  memset(&gb_state.cpu_dbus, BIT_NEW | BIT_PULLED | 1, sizeof(gb_state.cpu_dbus));

  probe_wire(0, "DELTA_HA",  DELTA_HA_new);

  //----------------------------------------

  {
    bool EXT_cpu_latch_ext;

    if (DELTA_DE_new || DELTA_EF_new || DELTA_FG_new || DELTA_GH_new) {
      // Data has to be driven on EFGH or we fail the wave tests
      gb_state.cpu_dbus.set_data(cpu.bus_req_new.write, cpu.bus_req_new.data_lo);
      EXT_cpu_latch_ext = cpu.bus_req_new.read;
    }
    else {
      gb_state.cpu_dbus.set_data(false, 0);
      EXT_cpu_latch_ext = 0;
    }
    /*_SIG_IN_CPU_LATCH_EXT*/ gb_state.cpu_signals.SIG_IN_CPU_DBUS_FREE.sig_in(EXT_cpu_latch_ext);

    bool EXT_addr_new = (cpu.bus_req_new.read || cpu.bus_req_new.write);
    bool in_bootrom = bit(~reg_old.cpu_signals.TEPU_BOOT_BITn.qp_old());
    bool addr_boot = (cpu.bus_req_new.addr <= 0x00FF) && in_bootrom;
    bool addr_vram = (cpu.bus_req_new.addr >= 0x8000) && (cpu.bus_req_new.addr <= 0x9FFF);
    bool addr_high = (cpu.bus_req_new.addr >= 0xFE00);

    bool EXT_cpu_rd;
    bool EXT_cpu_wr;

    if (DELTA_HA_new) {
      EXT_cpu_rd = 0;
      EXT_cpu_wr = 0;
      gb_state.cpu_abus.set_addr(cpu.bus_req_new.addr & 0x00FF);

      if (addr_high) EXT_addr_new = false;
      if (addr_boot) EXT_addr_new = false;
      if (addr_vram) EXT_addr_new = false;
    }
    else {
      EXT_cpu_rd = cpu.bus_req_new.read;
      EXT_cpu_wr = cpu.bus_req_new.write;
      gb_state.cpu_abus.set_addr(cpu.bus_req_new.addr);

      if (addr_high) EXT_addr_new = false;
      if (addr_boot) EXT_addr_new = false;
    }

    /*_SIG_IN_CPU_RDp*/ gb_state.cpu_signals.SIG_IN_CPU_RDp.sig_in(EXT_cpu_rd);
    /*_SIG_IN_CPU_WRp*/ gb_state.cpu_signals.SIG_IN_CPU_WRp.sig_in(EXT_cpu_wr);
    /*_SIG_IN_CPU_EXT_BUSp*/ gb_state.cpu_signals.SIG_IN_CPU_EXT_BUSp.sig_in(EXT_addr_new);
  }

  //----------------------------------------

  {
    wire EXT_sys_clkreq = bit(sys.clk_req);
    wire EXT_sys_rst = bit(~sys.rst);
    wire EXT_sys_t2 = bit(~sys.t2);
    wire EXT_sys_t1 = bit(~sys.t1);

    wire EXT_clkin = (sys.gb_phase_total & 1) && sys.clk_en;
    wire EXT_clkgood = bit(~sys.clk_good);

    /*_PIN_74*/ pins.sys.PIN_74_CLK.pin_clk(EXT_clkin, EXT_clkgood);
    /*_PIN_71*/ pins.sys.PIN_71_RST.pin_in(EXT_sys_rst);
    /*_PIN_76*/ pins.sys.PIN_76_T2.pin_in(EXT_sys_t2);
    /*_PIN_77*/ pins.sys.PIN_77_T1.pin_in(EXT_sys_t1);

    wire EXT_ack_vblank = get_bit(cpu.core.int_ack, BIT_VBLANK);
    wire EXT_ack_stat = get_bit(cpu.core.int_ack, BIT_STAT);
    wire EXT_ack_timer = get_bit(cpu.core.int_ack, BIT_TIMER);
    wire EXT_ack_serial = get_bit(cpu.core.int_ack, BIT_SERIAL);
    wire EXT_ack_joypad = get_bit(cpu.core.int_ack, BIT_JOYPAD);

    /*_SIG_CPU_ACK_VBLANK*/ gb_state.cpu_ack.SIG_CPU_ACK_VBLANK.sig_in(EXT_ack_vblank);
    /*_SIG_CPU_ACK_STAT  */ gb_state.cpu_ack.SIG_CPU_ACK_STAT.sig_in(EXT_ack_stat);
    /*_SIG_CPU_ACK_TIMER */ gb_state.cpu_ack.SIG_CPU_ACK_TIMER.sig_in(EXT_ack_timer);
    /*_SIG_CPU_ACK_SERIAL*/ gb_state.cpu_ack.SIG_CPU_ACK_SERIAL.sig_in(EXT_ack_serial);
    /*_SIG_CPU_ACK_JOYPAD*/ gb_state.cpu_ack.SIG_CPU_ACK_JOYPAD.sig_in(EXT_ack_joypad);

    /*_SIG_CPU_CLKREQ*/ gb_state.sys_clk.SIG_CPU_CLKREQ.sig_in(EXT_sys_clkreq);

    /*_SIG_CPU_ADDR_HIp*/ gb_state.cpu_signals.SIG_CPU_ADDR_HIp.sig_out(gb_state.cpu_abus.SYRO_FE00_FFFF());
    /*_SIG_CPU_UNOR_DBG*/ gb_state.cpu_signals.SIG_CPU_UNOR_DBG.sig_out(pins.sys.UNOR_MODE_DBG2p());
    /*_SIG_CPU_UMUT_DBG*/ gb_state.cpu_signals.SIG_CPU_UMUT_DBG.sig_out(pins.sys.UMUT_MODE_DBG1p());
  }

  //----------------------------------------
  // Sys clock signals

  tock_clocks_gates();

  /*_p07.UJYV*/ wire UJYV_CPU_RDn = not1(gb_state.cpu_signals.SIG_IN_CPU_RDp.out_new());
  /*_p07.TEDO*/ gb_state.cpu_signals.TEDO_CPU_RDp <<= not1(UJYV_CPU_RDn);

  /*#p01.AFAS*/ wire AFAS_xxxxEFGx = nor2(gb_state.sys_clk.ADAR_ABCxxxxH(), gb_state.sys_clk.ATYP_ABCDxxxx());
  /*_p01.AREV*/ wire AREV_CPU_WRn = nand2(gb_state.cpu_signals.SIG_IN_CPU_WRp.out_new(), AFAS_xxxxEFGx);
  /*_p01.APOV*/ gb_state.cpu_signals.APOV_CPU_WRp <<= not1(AREV_CPU_WRn);

  /*_p07.UBAL*/ wire UBAL_CPU_WRn = not1(gb_state.cpu_signals.APOV_CPU_WRp.out_new());
  /*_p07.TAPU*/ gb_state.cpu_signals.TAPU_CPU_WRp <<= not1(UBAL_CPU_WRn); // xxxxEFGx

  /*#p01.AGUT*/ wire AGUT_xxCDEFGH = or_and3(gb_state.sys_clk.AROV_xxCDEFxx(), gb_state.sys_clk.AJAX_xxxxEFGH(), gb_state.cpu_signals.SIG_IN_CPU_EXT_BUSp.out_new());
  /*#p01.AWOD*/ wire AWOD_ABxxxxxx = nor2(pins.sys.UNOR_MODE_DBG2p(), AGUT_xxCDEFGH);
  /*#p01.ABUZ*/ gb_state.cpu_signals.ABUZ_EXT_RAM_CS_CLK <<= not1(AWOD_ABxxxxxx);

  tock_div_gates();
  tock_reset_gates(bit(sys.fastboot) ? gb_state.reg_div.TERO_DIV03p : gb_state.reg_div.UPOF_DIV15p);
  tock_lcdc_gates(reg_old); // LCDC has to be near the top as it controls the video reset signal
  tock_vid_clocks_gates();
  tock_lyc_gates(reg_old);
  tock_lcd_gates();
  tock_joypad_gates(reg_old);
  //tock_serial_gates();
  tock_timer_gates(reg_old);
  tock_bootrom_gates();
  tock_dma_gates(reg_old);

  //----------------------------------------


  /*#p21.PARU*/ wire PARU_VBLANKp_odd = not1(gb_state.lcd.POPU_VBLANKp_odd.qn_new());
  /*_p27.REPU*/ wire REPU_VBLANKp_odd = or2(PARU_VBLANKp_odd, PYRY_VID_RSTp());

  /*_p21.TADY*/ wire TADY_LINE_RSTn_odd = nor2(gb_state.ATEJ_LINE_RSTp_odd.out_new(), TOFU_VID_RSTp());
  /*#p28.ANOM*/ wire ANOM_LINE_RSTn_odd = nor2(gb_state.ATEJ_LINE_RSTp_odd.out_new(), ATAR_VID_RSTp());
  /*#p29.BALU*/ wire BALU_LINE_RSTp_odd = not1(ANOM_LINE_RSTn_odd);
  /*#p29.BAGY*/ wire BAGY_LINE_RSTn_odd = not1(BALU_LINE_RSTp_odd);
  /*_p27.XAHY*/ wire XAHY_LINE_RSTn_odd = not1(gb_state.ATEJ_LINE_RSTp_odd.out_new());

  //----------------------------------------

  //----------------------------------------
  // Sprite scanner

  {
    /*#p29.BYBA*/ gb_state.sprite_scanner.BYBA_SCAN_DONEp_odd.dff17(gb_state.sys_clk.XUPY_ABxxEFxx(), BAGY_LINE_RSTn_odd, reg_old.sprite_scanner.FETO_SCAN_DONEp.out_old());
    /*#p29.DOBA*/ gb_state.sprite_scanner.DOBA_SCAN_DONEp_evn.dff17(gb_state.sys_clk.ALET_evn(),      BAGY_LINE_RSTn_odd, reg_old.sprite_scanner.BYBA_SCAN_DONEp_odd.qp_old());
    /*#p29.BEBU*/ wire BEBU_SCAN_DONE_tn_odd = or3(gb_state.sprite_scanner.DOBA_SCAN_DONEp_evn.qp_new(), BALU_LINE_RSTp_odd, gb_state.sprite_scanner.BYBA_SCAN_DONEp_odd.qn_new());
    /*#p29.AVAP*/ gb_state.sprite_scanner.AVAP_SCAN_DONE_tp_odd <<= not1(BEBU_SCAN_DONE_tn_odd);

    /*#p28.ASEN*/ wire ASEN_SCAN_DONE_tp_odd = or2(ATAR_VID_RSTp(), gb_state.sprite_scanner.AVAP_SCAN_DONE_tp_odd.out_new());
    /*#p28.BESU*/ gb_state.sprite_scanner.BESU_SCAN_DONEn_odd.nor_latch(gb_state.lcd.CATU_x113p_odd.qp_new(), ASEN_SCAN_DONE_tp_odd);
    /*#p29.CENO*/ gb_state.sprite_scanner.CENO_SCAN_DONEn_odd.dff17(gb_state.sys_clk.XUPY_ABxxEFxx(), ABEZ_VID_RSTn(), reg_old.sprite_scanner.BESU_SCAN_DONEn_odd.qp_old());

    /*#p28.BOGE*/ wire BOGE_DMA_RUNNINGn_odd = not1(gb_state.MATU_DMA_RUNNINGp_odd.qp_new());
    /*#p28.ACYL*/ gb_state.ACYL_SCANNINGp_odd <<= and2(BOGE_DMA_RUNNINGn_odd, gb_state.sprite_scanner.BESU_SCAN_DONEn_odd.qp_new());

    // Sprite scanner grabs the sprite index off the _old_ oam address bus
    /*_p28.YFOT*/ wire YFOT_OAM_A2p_old = not1(reg_old.oam_abus.BUS_OAM_A02n.out_old());
    /*_p28.YFOC*/ wire YFOC_OAM_A3p_old = not1(reg_old.oam_abus.BUS_OAM_A03n.out_old());
    /*_p28.YVOM*/ wire YVOM_OAM_A4p_old = not1(reg_old.oam_abus.BUS_OAM_A04n.out_old());
    /*_p28.YMEV*/ wire YMEV_OAM_A5p_old = not1(reg_old.oam_abus.BUS_OAM_A05n.out_old());
    /*_p28.XEMU*/ wire XEMU_OAM_A6p_old = not1(reg_old.oam_abus.BUS_OAM_A06n.out_old());
    /*_p28.YZET*/ wire YZET_OAM_A7p_old = not1(reg_old.oam_abus.BUS_OAM_A07n.out_old());
    /*_p30.XADU*/ gb_state.sprite_index.XADU_SPRITE_IDX0p_odd.dff13(gb_state.sys_clk.WUDA_xxCDxxGH(), gb_state.SIG_VCC.out_new(), YFOT_OAM_A2p_old);
    /*_p30.XEDY*/ gb_state.sprite_index.XEDY_SPRITE_IDX1p_odd.dff13(gb_state.sys_clk.WUDA_xxCDxxGH(), gb_state.SIG_VCC.out_new(), YFOC_OAM_A3p_old);
    /*_p30.ZUZE*/ gb_state.sprite_index.ZUZE_SPRITE_IDX2p_odd.dff13(gb_state.sys_clk.WUDA_xxCDxxGH(), gb_state.SIG_VCC.out_new(), YVOM_OAM_A4p_old);
    /*_p30.XOBE*/ gb_state.sprite_index.XOBE_SPRITE_IDX3p_odd.dff13(gb_state.sys_clk.WUDA_xxCDxxGH(), gb_state.SIG_VCC.out_new(), YMEV_OAM_A5p_old);
    /*_p30.YDUF*/ gb_state.sprite_index.YDUF_SPRITE_IDX4p_odd.dff13(gb_state.sys_clk.WUDA_xxCDxxGH(), gb_state.SIG_VCC.out_new(), XEMU_OAM_A6p_old);
    /*_p30.XECU*/ gb_state.sprite_index.XECU_SPRITE_IDX5p_odd.dff13(gb_state.sys_clk.WUDA_xxCDxxGH(), gb_state.SIG_VCC.out_new(), YZET_OAM_A7p_old);

    // FIXME is this OK without the second assignment of FETO?

    for (int feedback = 0; feedback < 2; feedback++) {
      // 32 + 4 + 2 + 1 = 39
      /*#p28.FETO*/ gb_state.sprite_scanner.FETO_SCAN_DONEp <<= and4(gb_state.scan_counter.YFEL_SCAN0_odd.qp_mid(), gb_state.scan_counter.WEWY_SCAN1_odd.qp_mid(), gb_state.scan_counter.GOSO_SCAN2_odd.qp_mid(), gb_state.scan_counter.FONY_SCAN5_odd.qp_mid());

      /*#p28.GAVA*/ wire GAVA_SCAN_CLOCKp_odd = or2(gb_state.sprite_scanner.FETO_SCAN_DONEp.out_new(), gb_state.sys_clk.XUPY_ABxxEFxx());

      /*#p28.YFEL*/ gb_state.scan_counter.YFEL_SCAN0_odd.dff17_any(GAVA_SCAN_CLOCKp_odd,                      ANOM_LINE_RSTn_odd, gb_state.scan_counter.YFEL_SCAN0_odd.qn_any());
      /*_p28.WEWY*/ gb_state.scan_counter.WEWY_SCAN1_odd.dff17_any(gb_state.scan_counter.YFEL_SCAN0_odd.qn_any(), ANOM_LINE_RSTn_odd, gb_state.scan_counter.WEWY_SCAN1_odd.qn_any());
      /*_p28.GOSO*/ gb_state.scan_counter.GOSO_SCAN2_odd.dff17_any(gb_state.scan_counter.WEWY_SCAN1_odd.qn_any(), ANOM_LINE_RSTn_odd, gb_state.scan_counter.GOSO_SCAN2_odd.qn_any());
      /*_p28.ELYN*/ gb_state.scan_counter.ELYN_SCAN3_odd.dff17_any(gb_state.scan_counter.GOSO_SCAN2_odd.qn_any(), ANOM_LINE_RSTn_odd, gb_state.scan_counter.ELYN_SCAN3_odd.qn_any());
      /*_p28.FAHA*/ gb_state.scan_counter.FAHA_SCAN4_odd.dff17_any(gb_state.scan_counter.ELYN_SCAN3_odd.qn_any(), ANOM_LINE_RSTn_odd, gb_state.scan_counter.FAHA_SCAN4_odd.qn_any());
      /*_p28.FONY*/ gb_state.scan_counter.FONY_SCAN5_odd.dff17_any(gb_state.scan_counter.FAHA_SCAN4_odd.qn_any(), ANOM_LINE_RSTn_odd, gb_state.scan_counter.FONY_SCAN5_odd.qn_any());
    }

    ///*#p28.FETO*/ sprite_scanner.FETO_SCAN_DONEp = and4(sprite_scanner.YFEL_SCAN0.qp_new(), sprite_scanner.WEWY_SCAN1.qp_new(), sprite_scanner.GOSO_SCAN2.qp_new(), sprite_scanner.FONY_SCAN5.qp_new());
  }


  //----------------------------------------
  // Global rendering flag 'XYMU'

  /*#p21.VOGA*/ gb_state.VOGA_HBLANKp_evn.dff17(gb_state.sys_clk.ALET_evn(), TADY_LINE_RSTn_odd, reg_old.WODU_HBLANKp_odd.out_old());
  /*#p21.WEGO*/ wire WEGO_HBLANKp_evn = or2(TOFU_VID_RSTp(), gb_state.VOGA_HBLANKp_evn.qp_new());
  /*#p21.XYMU*/ gb_state.XYMU_RENDERINGn.nor_latch(WEGO_HBLANKp_evn, gb_state.sprite_scanner.AVAP_SCAN_DONE_tp_odd.out_new());
  /*_p24.LOBY*/ wire LOBY_RENDERINGn = not1(gb_state.XYMU_RENDERINGn.qn_new());
  /*#p27.PAHA*/ wire PAHA_RENDERINGn = not1(gb_state.XYMU_RENDERINGn.qn_new());
  /*_p29.TEPA*/ wire TEPA_RENDERINGn = not1(gb_state.XYMU_RENDERINGn.qn_new());

  //----------------------------------------
  // Sprite fetch sequencer

  {
    /*_p27.TUKU*/ wire TUKU_WIN_HITn_odd_old = not1(TOMU_WIN_HITp_odd_old);
    /*_p27.SOWO*/ wire SOWO_SFETCH_RUNNINGn_evn_old = not1(reg_old.sfetch_control.TAKA_SFETCH_RUNNINGp_evn.qp_old());
    /*_p27.TEKY*/ wire TEKY_SFETCH_REQp_odd_old = and4(reg_old.FEPO_STORE_MATCHp_odd.out_old(), TUKU_WIN_HITn_odd_old, reg_old.tfetch_control.LYRY_BFETCH_DONEp_odd.out_old(), SOWO_SFETCH_RUNNINGn_evn_old);

    probe_wire(16, "TEKY_OLD",  TEKY_SFETCH_REQp_odd_old);

    /*_p27.SOBU*/ gb_state.sfetch_control.SOBU_SFETCH_REQp_evn.dff17(gb_state.sys_clk.TAVA_evn(), gb_state.SIG_VCC.out_new(), TEKY_SFETCH_REQp_odd_old);
    /*_p27.SUDA*/ gb_state.sfetch_control.SUDA_SFETCH_REQp_odd.dff17(gb_state.sys_clk.LAPE_odd(), gb_state.SIG_VCC.out_new(), reg_old.sfetch_control.SOBU_SFETCH_REQp_evn.qp_old());
    probe_wire(18, "SOBU",  gb_state.sfetch_control.SOBU_SFETCH_REQp_evn.state & 1);
    probe_wire(19, "SUDA",  gb_state.sfetch_control.SUDA_SFETCH_REQp_odd.state & 1);

    /*#p29.TYFO*/ gb_state.sfetch_control.TYFO_SFETCH_S0p_D1_odd.dff17(gb_state.sys_clk.LAPE_odd(), gb_state.SIG_VCC.out_new(),        reg_old.sfetch_counter_evn.TOXE_SFETCH_S0p_evn.qp_old());
    /*#p29.TOBU*/ gb_state.sfetch_control.TOBU_SFETCH_S1p_D2_evn.dff17(gb_state.sys_clk.TAVA_evn(), gb_state.XYMU_RENDERINGn.qn_new(), reg_old.sfetch_counter_evn.TULY_SFETCH_S1p_evn.qp_old());
    /*#p29.VONU*/ gb_state.sfetch_control.VONU_SFETCH_S1p_D4_evn.dff17(gb_state.sys_clk.TAVA_evn(), gb_state.XYMU_RENDERINGn.qn_new(), reg_old.sfetch_control.TOBU_SFETCH_S1p_D2_evn.qp_old());
    /*#p29.SEBA*/ gb_state.sfetch_control.SEBA_SFETCH_S1p_D5_odd.dff17(gb_state.sys_clk.LAPE_odd(), gb_state.XYMU_RENDERINGn.qn_new(), reg_old.sfetch_control.VONU_SFETCH_S1p_D4_evn.qp_old());

    /*_p27.RYCE*/ wire RYCE_SFETCH_TRIGp = and2(gb_state.sfetch_control.SOBU_SFETCH_REQp_evn.qp_new(), gb_state.sfetch_control.SUDA_SFETCH_REQp_odd.qn_new());
    /*#p27.SECA*/ wire SECA_SFETCH_STARTn_evn = nor3(RYCE_SFETCH_TRIGp, ROSY_VID_RSTp(), gb_state.ATEJ_LINE_RSTp_odd.out_new());

    // Feedback loop
    for (int feedback = 0; feedback < 2; feedback++) {
      /*#p29.TAME*/ wire TAME_SFETCH_CLK_GATE = nand2(gb_state.sfetch_counter_evn.TESE_SFETCH_S2p_evn.qp_any(), gb_state.sfetch_counter_evn.TOXE_SFETCH_S0p_evn.qp_any());
      /*#p29.TOMA*/ wire TOMA_SFETCH_evn = nand2(gb_state.sys_clk.LAPE_odd(), TAME_SFETCH_CLK_GATE);
      /*#p29.TOXE*/ gb_state.sfetch_counter_evn.TOXE_SFETCH_S0p_evn.dff17_any(TOMA_SFETCH_evn,                                      SECA_SFETCH_STARTn_evn, gb_state.sfetch_counter_evn.TOXE_SFETCH_S0p_evn.qn_any());
      /*#p29.TULY*/ gb_state.sfetch_counter_evn.TULY_SFETCH_S1p_evn.dff17_any(gb_state.sfetch_counter_evn.TOXE_SFETCH_S0p_evn.qn_any(), SECA_SFETCH_STARTn_evn, gb_state.sfetch_counter_evn.TULY_SFETCH_S1p_evn.qn_any());
      /*#p29.TESE*/ gb_state.sfetch_counter_evn.TESE_SFETCH_S2p_evn.dff17_any(gb_state.sfetch_counter_evn.TULY_SFETCH_S1p_evn.qn_any(), SECA_SFETCH_STARTn_evn, gb_state.sfetch_counter_evn.TESE_SFETCH_S2p_evn.qn_any());
    }

    /*_p29.SAKY*/ wire SAKY_SFETCHn_evn = nor2(gb_state.sfetch_counter_evn.TULY_SFETCH_S1p_evn.qp_new(), gb_state.sfetch_control.VONU_SFETCH_S1p_D4_evn.qp_new());
    /*_p29.TYSO*/ wire TYSO_SFETCHINGn_evn = or2(SAKY_SFETCHn_evn, TEPA_RENDERINGn); // def or
    /*_p29.TEXY*/ gb_state.sfetch_control.TEXY_SFETCHINGp_evn <<= not1(TYSO_SFETCHINGn_evn);

    /*_p29.TYNO*/ wire TYNO = nand3(gb_state.sfetch_counter_evn.TOXE_SFETCH_S0p_evn.qp_new(), gb_state.sfetch_control.SEBA_SFETCH_S1p_D5_odd.qp_new(), gb_state.sfetch_control.VONU_SFETCH_S1p_D4_evn.qp_new());
    /*_p29.VUSA*/ wire VUSA_SPRITE_DONEn = or2(gb_state.sfetch_control.TYFO_SFETCH_S0p_D1_odd.qn_new(), TYNO);
    /*_p29.WUTY*/ gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp <<= not1(VUSA_SPRITE_DONEn);
  }

  /*_p27.RYCE*/ wire RYCE_SFETCH_TRIGp_evn = and2(gb_state.sfetch_control.SOBU_SFETCH_REQp_evn.qp_new(), gb_state.sfetch_control.SUDA_SFETCH_REQp_odd.qn_new());
  /*#p27.SECA*/ wire SECA_SFETCH_STARTn_evn = nor3(RYCE_SFETCH_TRIGp_evn, ROSY_VID_RSTp(), gb_state.ATEJ_LINE_RSTp_odd.out_new());

  //----------------------------------------
  // Window state has some interaction with the tile fetcher here.




  


















  // There's what looks like a combinatorial loop here, but it isn't actually a loop because half of it happens on odd phases and half happens on even phases.

  /*_p27.NUNU*/ reg_new.win_ctrl.NUNU_WIN_MATCHp_odd.dff17 (reg_new.sys_clk.MEHE_odd(), XAPO_VID_RSTn(), reg_old.win_ctrl.PYCO_WIN_MATCHp_evn.qp_old());
  /*_p27.NOPA*/ reg_new.win_ctrl.NOPA_WIN_MODE_Bp_evn.dff17(reg_new.sys_clk.ALET_evn(), XAPO_VID_RSTn(), reg_old.win_ctrl.PYNU_WIN_MODE_Ap_odd.qp_old());

  /*#p27.XOFO*/ wire XOFO_WIN_RSTp = nand3(reg_new.reg_lcdc.WYMO_LCDC_WINENn.qn_new(), XAHY_LINE_RSTn_odd, XAPO_VID_RSTn());
  /*_p27.PYNU*/ reg_new.win_ctrl.PYNU_WIN_MODE_Ap_odd.nor_latch(reg_new.win_ctrl.NUNU_WIN_MATCHp_odd.qp_new(), XOFO_WIN_RSTp);

  /*#p27.NUNY*/ wire NUNY_WIN_MODE_tp_odd = and2(gb_state.win_ctrl.PYNU_WIN_MODE_Ap_odd.qp_new(), gb_state.win_ctrl.NOPA_WIN_MODE_Bp_evn.qn_new());
  /*_p27.NYFO*/ wire NYFO_WIN_MODE_tn_odd = not1(NUNY_WIN_MODE_tp_odd);
  /*_p27.MOSU*/ wire MOSU_WIN_MODE_tp_odd = not1(NYFO_WIN_MODE_tn_odd);
  /*_p24.NAFY*/ wire NAFY_WIN_MODE_tn_odd = nor2(MOSU_WIN_MODE_tp_odd, LOBY_RENDERINGn);

  /*_p24.PYGO*/ gb_state.tfetch_control.PYGO_FETCH_DONEp_evn.dff17(reg_new.sys_clk.ALET_evn(), reg_new.XYMU_RENDERINGn.qn_new(),  reg_old.tfetch_control.PORY_FETCH_DONEp_odd.qp_old());
  /*_p24.PORY*/ gb_state.tfetch_control.PORY_FETCH_DONEp_odd.dff17(reg_new.sys_clk.MYVO_odd(), NAFY_WIN_MODE_tn_odd,              reg_old.tfetch_control.NYKA_FETCH_DONEp_evn.qp_old());
  /*_p24.NYKA*/ gb_state.tfetch_control.NYKA_FETCH_DONEp_evn.dff17(reg_new.sys_clk.ALET_evn(), NAFY_WIN_MODE_tn_odd,              reg_old.tfetch_control.LYRY_BFETCH_DONEp_odd.out_old());

  /*#p27.NUNY*/ wire NUNY_WIN_MODE_TRIGp_odd = and2(reg_new.win_ctrl.PYNU_WIN_MODE_Ap_odd.qp_new(), reg_new.win_ctrl.NOPA_WIN_MODE_Bp_evn.qn_new());
  /*_p27.RYDY*/ reg_new.win_ctrl.RYDY_WIN_HITp_odd <<= nor3(reg_new.win_ctrl.PUKU_WIN_HITn_odd.out_mid(), reg_new.tfetch_control.PORY_FETCH_DONEp_odd.qp_new(), PYRY_VID_RSTp());
  /*_p27.PUKU*/ reg_new.win_ctrl.PUKU_WIN_HITn_odd <<= nor2(reg_new.win_ctrl.RYDY_WIN_HITp_odd.out_mid(), NUNY_WIN_MODE_TRIGp_odd);
  /*_p27.RYDY*/ reg_new.win_ctrl.RYDY_WIN_HITp_odd <<= nor3(reg_new.win_ctrl.PUKU_WIN_HITn_odd.out_mid(), reg_new.tfetch_control.PORY_FETCH_DONEp_odd.qp_new(), PYRY_VID_RSTp());
  /*_p27.PUKU*/ reg_new.win_ctrl.PUKU_WIN_HITn_odd <<= nor2(reg_new.win_ctrl.RYDY_WIN_HITp_odd.out_mid(), NUNY_WIN_MODE_TRIGp_odd);















  /*_p24.POKY*/ gb_state.tfetch_control.POKY_PRELOAD_LATCHp_evn.nor_latch(gb_state.tfetch_control.PYGO_FETCH_DONEp_evn.qp_new(), LOBY_RENDERINGn);


  /*_p27.SOVY*/ gb_state.win_ctrl.SOVY_WIN_HITp_evn.dff17(gb_state.sys_clk.ALET_evn(), XAPO_VID_RSTn(), reg_old.win_ctrl.RYDY_WIN_HITp_odd.out_old());

  wire pause_pipe_new1 = reg_new.win_ctrl.RYDY_WIN_HITp_odd.state || !reg_new.tfetch_control.POKY_PRELOAD_LATCHp_evn.state || reg_old.FEPO_STORE_MATCHp_odd.state || (!reg_old.FEPO_STORE_MATCHp_odd.state && (bit_pack(reg_old.pix_count) == 167));

  wire pause_pipe_new2 = reg_new.win_ctrl.RYDY_WIN_HITp_odd.out_new() || !reg_new.tfetch_control.POKY_PRELOAD_LATCHp_evn.qp_new() || reg_old.FEPO_STORE_MATCHp_odd.out_old() || reg_old.WODU_HBLANKp_odd.out_old();

  if (bit(pause_pipe_new1) != bit(pause_pipe_new2)) {
    debugbreak();
  }


  /*#p27.SYLO*/ wire SYLO_WIN_HITn_odd_new = not1(gb_state.win_ctrl.RYDY_WIN_HITp_odd.out_new());
  /*#p24.TOMU*/ wire TOMU_WIN_HITp_odd_new = not1(SYLO_WIN_HITn_odd_new);
  /*#p24.SOCY*/ wire SOCY_WIN_HITn_odd_new = not1(TOMU_WIN_HITp_odd_new);

  // CLKPIPE is an even clock, it can only go high on even deltas. FEPO/WODU/SOCY are odd signals, they stay constant during even deltas.
  // SO, it is guaranteed safe to use the old values of FEPO/WODU/SOCY to compute CLKPIPE
  /*#p24.VYBO*/ wire VYBO_CLKPIPE_evn_new = nor3(reg_old.FEPO_STORE_MATCHp_odd.out_old(), reg_old.WODU_HBLANKp_odd.out_old(), reg_new.sys_clk.MYVO_odd());
  /*#p24.TYFA*/ wire TYFA_CLKPIPE_evn_new = and3(SOCY_WIN_HITn_odd_new, reg_new.tfetch_control.POKY_PRELOAD_LATCHp_evn.qp_new(), VYBO_CLKPIPE_evn_new);
  /*#p24.SEGU*/ wire SEGU_CLKPIPE_odd_new = not1(TYFA_CLKPIPE_evn_new);
  /*_p27.ROCO*/ wire ROCO_CLKPIPE_evn_new = not1(SEGU_CLKPIPE_odd_new);
  /*#p24.ROXO*/ wire ROXO_CLKPIPE_evn_new = not1(SEGU_CLKPIPE_odd_new);


  /*_p27.ROMO*/ wire ROMO_PRELOAD_DONEn_evn = not1(gb_state.tfetch_control.POKY_PRELOAD_LATCHp_evn.qp_new());
  /*_p27.SUVU*/ wire SUVU_PRELOAD_DONE_TRIGn = nand4(
    gb_state.XYMU_RENDERINGn.qn_new(),
    ROMO_PRELOAD_DONEn_evn,
    gb_state.tfetch_control.NYKA_FETCH_DONEp_evn.qp_new(),
    gb_state.tfetch_control.PORY_FETCH_DONEp_odd.qp_new());
  /*_p27.TAVE*/ wire TAVE_PRELOAD_DONE_TRIGp = not1(SUVU_PRELOAD_DONE_TRIGn);

  /*_p27.VEKU*/ wire VEKU_SFETCH_ENDn = nor2(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), TAVE_PRELOAD_DONE_TRIGp); // def nor
  /*_p27.TAKA*/ gb_state.sfetch_control.TAKA_SFETCH_RUNNINGp_evn.nand_latch(SECA_SFETCH_STARTn_evn, VEKU_SFETCH_ENDn);

  //----------------------------------------
  // OAM temp register B stores sprite X coordinate during scan, so we need a copy of it for later.

  auto oam_temp_b_old = reg_old.oam_temp_b;

  //----------------------------------------
  // OAM latch from last cycle gets moved into temp registers.

  {
    /*_p25.CUFE*/ wire CUFE_OAM_CLKp = not_or_and3(gb_state.cpu_abus.SARO_ADDR_OAMp(), gb_state.MATU_DMA_RUNNINGp_odd.qp_new(), gb_state.sys_clk.MOPA_xxxxEFGH()); // CUFE looks like BYHA minus an inverter
    /*_p25.AVER*/ wire AVER_AxxxExxx = nand2(gb_state.ACYL_SCANNINGp_odd.out_new(), gb_state.sys_clk.XYSO_xBCDxFGH());
    /*_p29.TYTU*/ wire TYTU_SFETCH_S0n = not1(gb_state.sfetch_counter_evn.TOXE_SFETCH_S0p_evn.qp_new());
    /*_p29.TACU*/ wire TACU_SPR_SEQ_5_TRIG = nand2(gb_state.sfetch_control.TYFO_SFETCH_S0p_D1_odd.qp_new(), TYTU_SFETCH_S0n);
    /*_p29.TUVO*/ wire TUVO_PPU_OAM_RDp = nor3(TEPA_RENDERINGn, gb_state.sfetch_counter_evn.TULY_SFETCH_S1p_evn.qp_new(), gb_state.sfetch_counter_evn.TESE_SFETCH_S2p_evn.qp_new());
    /*_p25.VAPE*/ wire VAPE_OAM_CLKENn = and2(TUVO_PPU_OAM_RDp, TACU_SPR_SEQ_5_TRIG);
    /*_p25.XUJY*/ wire XUJY_OAM_CLKENp = not1(VAPE_OAM_CLKENn);
    /*_p25.BYCU*/ wire BYCU_OAM_CLKp = nand3(AVER_AxxxExxx, XUJY_OAM_CLKENp, CUFE_OAM_CLKp);
    /*_p25.COTA*/ wire COTA_OAM_CLKn = not1(BYCU_OAM_CLKp);
    oam_latch_to_temp_a_gates(COTA_OAM_CLKn, gb_state.oam_latch_a, gb_state.oam_temp_a);
    oam_latch_to_temp_b_gates(COTA_OAM_CLKn, gb_state.oam_latch_b, gb_state.oam_temp_b);
  }

  //----------------------------------------
  // Sprite scanner triggers the sprite store clock, increments the sprite counter, and puts the sprite in the sprite store if it overlaps the current LCD Y coordinate.

  SpriteDeltaY sprite_delta_y = sub_sprite_y_gates();

  {
    /*_p29.GACE*/ wire GACE_SPRITE_DELTA4 = not1(sprite_delta_y.GOPU_YDIFF4.sum);
    /*_p29.GUVU*/ wire GUVU_SPRITE_DELTA5 = not1(sprite_delta_y.FUWA_YDIFF5.sum);
    /*_p29.GYDA*/ wire GYDA_SPRITE_DELTA6 = not1(sprite_delta_y.GOJU_YDIFF6.sum);
    /*_p29.GEWY*/ wire GEWY_SPRITE_DELTA7 = not1(sprite_delta_y.WUHU_YDIFF7.sum);

    /*#p29.GOVU*/ wire GOVU_SPSIZE_MATCH = or2(gb_state.reg_lcdc.XYMO_LCDC_SPSIZEn.qn_new(), sprite_delta_y.GYKY_YDIFF3.sum);
    /*_p29.WOTA*/ wire WOTA_SCAN_MATCH_Yn = nand6(GACE_SPRITE_DELTA4, GUVU_SPRITE_DELTA5, GYDA_SPRITE_DELTA6, GEWY_SPRITE_DELTA7, sprite_delta_y.WUHU_YDIFF7.carry, GOVU_SPSIZE_MATCH);
    /*_p29.GESE*/ wire GESE_SCAN_MATCH_Yp = not1(WOTA_SCAN_MATCH_Yn);
    /*_p29.CEHA*/ wire CEHA_SCANNINGp_odd = not1(gb_state.sprite_scanner.CENO_SCAN_DONEn_odd.qn_new());
    /*_p29.CARE*/ wire CARE_COUNT_CLKn_evn = and3(gb_state.sys_clk.XOCE_xBCxxFGx(), CEHA_SCANNINGp_odd, GESE_SCAN_MATCH_Yp); // Dots on VCC, this is AND. Die shot and schematic wrong.
    /*_p29.DYTY*/ wire DYTY_COUNT_CLKp_evn = not1(CARE_COUNT_CLKn_evn);
    /*_p29.DEZY*/ gb_state.sprite_scanner.DEZY_INC_COUNTn_odd.dff17_any(gb_state.sys_clk.ZEME_ODD(), XAPO_VID_RSTn(), DYTY_COUNT_CLKp_evn);

    // There's a feedback loop here, but we don't actually need to loop - BAKY holds the clock line high once the sprite store is full, so doing a second logic pass
    // doesn't actually change any of the dffs.
    /*#p29.BAKY*/ wire BAKY_SPRITES_FULL_new = and2(gb_state.sprite_counter.CUXY_SPRITE_COUNT1_odd.qp_any(), gb_state.sprite_counter.DYBE_SPRITE_COUNT3_odd.qp_any());
    /*#p29.CAKE*/ wire CAKE_COUNT_CLKp_new = or2(BAKY_SPRITES_FULL_new, gb_state.sprite_scanner.DEZY_INC_COUNTn_odd.qp_any());
    /*#p28.AZYB*/ wire AZYB_LINE_TRIGn = not1(gb_state.ATEJ_LINE_RSTp_odd.out_new());
    /*_p29.BESE*/ gb_state.sprite_counter.BESE_SPRITE_COUNT0_odd.dff17_any(CAKE_COUNT_CLKp_new, AZYB_LINE_TRIGn, gb_state.sprite_counter.BESE_SPRITE_COUNT0_odd.qn_any());
    /*_p29.CUXY*/ gb_state.sprite_counter.CUXY_SPRITE_COUNT1_odd.dff17_any(gb_state.sprite_counter.BESE_SPRITE_COUNT0_odd.qn_any(), AZYB_LINE_TRIGn, gb_state.sprite_counter.CUXY_SPRITE_COUNT1_odd.qn_any());
    /*_p29.BEGO*/ gb_state.sprite_counter.BEGO_SPRITE_COUNT2_odd.dff17_any(gb_state.sprite_counter.CUXY_SPRITE_COUNT1_odd.qn_any(), AZYB_LINE_TRIGn, gb_state.sprite_counter.BEGO_SPRITE_COUNT2_odd.qn_any());
    /*_p29.DYBE*/ gb_state.sprite_counter.DYBE_SPRITE_COUNT3_odd.dff17_any(gb_state.sprite_counter.BEGO_SPRITE_COUNT2_odd.qn_any(), AZYB_LINE_TRIGn, gb_state.sprite_counter.DYBE_SPRITE_COUNT3_odd.qn_any());

    /*_p28.ABAK*/ wire ABAK_LINE_RSTp = or2(gb_state.ATEJ_LINE_RSTp_odd.out_new(), AMYG_VID_RSTp());
    /*_p28.BYVA*/ wire BYVA_LINE_RSTn = not1(ABAK_LINE_RSTp);

    //update_sprite_reset_flags(sfetch_control.WUTY_SFETCH_DONE_TRIGp, BYVA_LINE_RSTn, sprite_match_flags, sprite_reset_flags);

    /*_p29.EBOJ*/ gb_state.sprite_reset_flags_odd.EBOJ_STORE0_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.GUVA_SPRITE0_GETp.out_old());
    /*_p29.CEDY*/ gb_state.sprite_reset_flags_odd.CEDY_STORE1_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.ENUT_SPRITE1_GETp.out_old());
    /*_p29.EGAV*/ gb_state.sprite_reset_flags_odd.EGAV_STORE2_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.EMOL_SPRITE2_GETp.out_old());
    /*_p29.GOTA*/ gb_state.sprite_reset_flags_odd.GOTA_STORE3_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.GYFY_SPRITE3_GETp.out_old());
    /*_p29.XUDY*/ gb_state.sprite_reset_flags_odd.XUDY_STORE4_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.GONO_SPRITE4_GETp.out_old());
    /*_p29.WAFY*/ gb_state.sprite_reset_flags_odd.WAFY_STORE5_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.GEGA_SPRITE5_GETp.out_old());
    /*_p29.WOMY*/ gb_state.sprite_reset_flags_odd.WOMY_STORE6_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.XOJA_SPRITE6_GETp.out_old());
    /*_p29.WAPO*/ gb_state.sprite_reset_flags_odd.WAPO_STORE7_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.GUTU_SPRITE7_GETp.out_old());
    /*_p29.EXUQ*/ gb_state.sprite_reset_flags_odd.EXUQ_STORE8_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.FOXA_SPRITE8_GETp.out_old());
    /*_p29.FONO*/ gb_state.sprite_reset_flags_odd.FONO_STORE9_RSTp.dff17(gb_state.sfetch_control.WUTY_SFETCH_DONE_TRIGp.out_new(), BYVA_LINE_RSTn, reg_old.sprite_match_flags_odd.GUZE_SPRITE9_GETp.out_old());

    SpriteStoreFlags sprite_store_flags_old = reg_old.sprite_store_flags_evn;

    update_sprite_store_flags_gates(gb_state.sprite_counter, DYTY_COUNT_CLKp_evn, gb_state.sprite_store_flags_evn);
    store_sprite_gates(sprite_store_flags_old, gb_state.sprite_store_flags_evn, gb_state.sprite_reset_flags_odd, BYVA_LINE_RSTn, oam_temp_b_old);
  }

  //----------------------------------------
  // Fine scroll match, sprite store match, clock pipe, and pixel counter are intertwined here.

  auto XYDO_PX3p_old = reg_old.pix_count.XYDO_PX3p_odd;

  // Weird feedback loop here - fine scroll and pix count affect store match, which affects pipe clock, which affects fine scroll and pix count.
  // Implementing the feedback doesn't seem to change any test results, but... paranoia...

  //for (int feedback = 0; feedback < 2; feedback++) {
  {
    /*_p27.SUHA*/ wire SUHA_SCX_FINE_MATCHp_old_odd = xnor2(reg_old.reg_scx.DATY_SCX0n.qn_old(), reg_old.fine_count_odd.RYKU_FINE_CNT0_odd.qp_old());
    /*_p27.SYBY*/ wire SYBY_SCX_FINE_MATCHp_old_odd = xnor2(reg_old.reg_scx.DUZU_SCX1n.qn_old(), reg_old.fine_count_odd.ROGA_FINE_CNT1_odd.qp_old());
    /*_p27.SOZU*/ wire SOZU_SCX_FINE_MATCHp_old_odd = xnor2(reg_old.reg_scx.CYXU_SCX2n.qn_old(), reg_old.fine_count_odd.RUBU_FINE_CNT2_odd.qp_old());
    /*#p27.RONE*/ wire RONE_SCX_FINE_MATCHn_old_odd = nand4(gb_state.fine_scroll.ROXY_FINE_SCROLL_DONEn_evn.qp_any(), SUHA_SCX_FINE_MATCHp_old_odd, SYBY_SCX_FINE_MATCHp_old_odd, SOZU_SCX_FINE_MATCHp_old_odd);
    /*#p27.POHU*/ wire POHU_SCX_FINE_MATCHp_old_odd = not1(RONE_SCX_FINE_MATCHn_old_odd);

    /*_p29.CEHA*/ wire CEHA_SCANNINGp_odd  = not1(gb_state.sprite_scanner.CENO_SCAN_DONEn_odd.qn_new());
    /*#p29.BYJO*/ wire BYJO_SCANNINGn_odd  = not1(CEHA_SCANNINGp_odd);
    /*#p29.AZEM*/ wire AZEM_RENDERINGp_odd = and2(gb_state.XYMU_RENDERINGn.qn_new(), BYJO_SCANNINGn_odd);
    /*#p29.AROR*/ wire AROR_MATCH_ENp_odd  = and2(AZEM_RENDERINGp_odd, gb_state.reg_lcdc.XYLO_LCDC_SPENn.qn_new());

    /*#p27.PUXA*/ reg_new.fine_scroll.PUXA_SCX_FINE_MATCH_evn.dff17_any(ROXO_CLKPIPE_evn_new,        reg_new.XYMU_RENDERINGn.qn_new(), POHU_SCX_FINE_MATCHp_old_odd);
    /*#p27.NYZE*/ reg_new.fine_scroll.NYZE_SCX_FINE_MATCH_odd.dff17_any(reg_new.sys_clk.MOXE_ODD(), reg_new.XYMU_RENDERINGn.qn_new(), reg_old.fine_scroll.PUXA_SCX_FINE_MATCH_evn.qp_old());

    /*#p27.POVA*/ wire POVA_FINE_MATCH_TRIGp_evn = and2(reg_new.fine_scroll.PUXA_SCX_FINE_MATCH_evn.qp_any(), reg_new.fine_scroll.NYZE_SCX_FINE_MATCH_odd.qn_any());
    /*#p27.ROXY*/ reg_new.fine_scroll.ROXY_FINE_SCROLL_DONEn_evn.nor_latch(PAHA_RENDERINGn, POVA_FINE_MATCH_TRIGp_evn);

    // SACU high on ACEG, gated high

    /*#p24.SACU*/ wire SACU_CLKPIPE_odd_new = or2(SEGU_CLKPIPE_odd_new, reg_new.fine_scroll.ROXY_FINE_SCROLL_DONEn_evn.qp_any());

    // Pixel counter, has carry lookahead because this can increment every tcycle
    /*_p21.RYBO*/ wire RYBO_old = xor2(reg_old.pix_count.XEHO_PX0p_odd.qp_any(), reg_old.pix_count.SAVY_PX1p_odd.qp_any()); // XOR layout 1, feet facing gnd, this should def be regular xor
    /*_p21.XUKE*/ wire XUKE_old = and2(reg_old.pix_count.XEHO_PX0p_odd.qp_any(), reg_old.pix_count.SAVY_PX1p_odd.qp_any());
    /*_p21.XYLE*/ wire XYLE_old = and2(reg_old.pix_count.XODU_PX2p_odd.qp_any(), XUKE_old);
    /*_p21.XEGY*/ wire XEGY_old = xor2(reg_old.pix_count.XODU_PX2p_odd.qp_any(), XUKE_old); // feet facing gnd
    /*_p21.XORA*/ wire XORA_old = xor2(reg_old.pix_count.XYDO_PX3p_odd.qp_any(), XYLE_old); // feet facing gnd

    /*_p21.XEHO*/ reg_new.pix_count.XEHO_PX0p_odd.dff17_any(SACU_CLKPIPE_odd_new, TADY_LINE_RSTn_odd, reg_new.pix_count.XEHO_PX0p_odd.qn_any());
    /*_p21.SAVY*/ reg_new.pix_count.SAVY_PX1p_odd.dff17_any(SACU_CLKPIPE_odd_new, TADY_LINE_RSTn_odd, RYBO_old);
    /*_p21.XODU*/ reg_new.pix_count.XODU_PX2p_odd.dff17_any(SACU_CLKPIPE_odd_new, TADY_LINE_RSTn_odd, XEGY_old);
    /*_p21.XYDO*/ reg_new.pix_count.XYDO_PX3p_odd.dff17_any(SACU_CLKPIPE_odd_new, TADY_LINE_RSTn_odd, XORA_old);

    probe_wire(25, "SACU", SACU_CLKPIPE_odd_new);

    /*_p24.TOCA*/ wire TOCA_new = not1(reg_new.pix_count.XYDO_PX3p_odd.qp_any());
    /*_p21.SAKE*/ wire SAKE_old = xor2(reg_old.pix_count.TUHU_PX4p_odd.qp_any(), reg_old.pix_count.TUKY_PX5p_odd.qp_any());
    /*_p21.TYBA*/ wire TYBA_old = and2(reg_old.pix_count.TUHU_PX4p_odd.qp_any(), reg_old.pix_count.TUKY_PX5p_odd.qp_any());
    /*_p21.SURY*/ wire SURY_old = and2(reg_old.pix_count.TAKO_PX6p_odd.qp_any(), TYBA_old);
    /*_p21.TYGE*/ wire TYGE_old = xor2(reg_old.pix_count.TAKO_PX6p_odd.qp_any(), TYBA_old);
    /*_p21.ROKU*/ wire ROKU_old = xor2(reg_old.pix_count.SYBE_PX7p_odd.qp_any(), SURY_old);

    /*_p21.TUHU*/ reg_new.pix_count.TUHU_PX4p_odd.dff17_any(TOCA_new, TADY_LINE_RSTn_odd, reg_new.pix_count.TUHU_PX4p_odd.qn_any());
    /*_p21.TUKY*/ reg_new.pix_count.TUKY_PX5p_odd.dff17_any(TOCA_new, TADY_LINE_RSTn_odd, SAKE_old);
    /*_p21.TAKO*/ reg_new.pix_count.TAKO_PX6p_odd.dff17_any(TOCA_new, TADY_LINE_RSTn_odd, TYGE_old);
    /*_p21.SYBE*/ reg_new.pix_count.SYBE_PX7p_odd.dff17_any(TOCA_new, TADY_LINE_RSTn_odd, ROKU_old);
   
    get_sprite_match_flags_gates(AROR_MATCH_ENp_odd, reg_new.sprite_match_flags_odd); // this changes FEPO
  }

  //=================
  //=================
  //=================
  //=================

  // Pix counter triggers HBLANK if there's no sprite store match and enables the pixel pipe clocks for later
  {
    /*#p21.XENA*/ wire XENA_STORE_MATCHn_odd = not1(reg_new.FEPO_STORE_MATCHp_odd.out_new());
    /*#p21.XUGU*/ wire XUGU_PX167n_odd = nand5(
      reg_new.pix_count.XEHO_PX0p_odd.qp_new(),
      reg_new.pix_count.SAVY_PX1p_odd.qp_new(),
      reg_new.pix_count.XODU_PX2p_odd.qp_new(),
      reg_new.pix_count.TUKY_PX5p_odd.qp_new(),
      reg_new.pix_count.SYBE_PX7p_odd.qp_new()); // 128 + 32 + 4 + 2 + 1 = 167
    /*#p21.XANO*/ wire XANO_PX167p_odd = not1(XUGU_PX167n_odd);
    /*#p21.WODU*/ gb_state.WODU_HBLANKp_odd <<= and2(XENA_STORE_MATCHn_odd, XANO_PX167p_odd); // WODU goes high on odd, cleared on H
  }

  probe_wire(20, "MYVO", gb_state.sys_clk.MYVO_odd());

  /*_p27.PYCO*/ gb_state.win_ctrl.PYCO_WIN_MATCHp_evn.dff17(ROCO_CLKPIPE_evn_new, XAPO_VID_RSTn(), reg_old.win_ctrl.NUKO_WX_MATCHp_odd.out_old());


  //=================
  //=================
  //=================
  //=================




  probe_wire(21, "ROCO", ROCO_CLKPIPE_evn_new);
  /*#p24.SACU*/ wire SACU_CLKPIPE_odd_new = or2(SEGU_CLKPIPE_odd_new, gb_state.fine_scroll.ROXY_FINE_SCROLL_DONEn_evn.qp_new());

  /*_p24.PAHO*/ gb_state.lcd.PAHO_X8_SYNC.dff17(ROXO_CLKPIPE_evn_new, gb_state.XYMU_RENDERINGn.qn_new(), XYDO_PX3p_old.qp_old());

  //----------------------------------------

  memset(&gb_state.sprite_ibus, BIT_NEW | BIT_PULLED | 1, sizeof(gb_state.sprite_ibus));
  memset(&gb_state.sprite_lbus, BIT_NEW | BIT_PULLED | 1, sizeof(gb_state.sprite_lbus));

  sprite_match_to_bus_gates(gb_state.sprite_match_flags_odd);
  sprite_scan_to_bus_gates(sprite_delta_y, gb_state.XYMU_RENDERINGn, gb_state.FEPO_STORE_MATCHp_odd);

  //----------------------------------------
  // WY/WX/window match


  tock_window_gates(reg_old, SEGU_CLKPIPE_odd_new, REPU_VBLANKp_odd);

  //----------------------------------------
  // Tile fetch sequencer

  /*_p27.LYZU*/ gb_state.tfetch_control.LYZU_BFETCH_S0p_D1.dff17(gb_state.sys_clk.ALET_evn(), gb_state.XYMU_RENDERINGn.qn_new(), reg_old.tfetch_counter.LAXU_BFETCH_S0p_odd.qp_old());

  /*_p27.SEKO*/ wire SEKO_WIN_FETCH_TRIGp_evn = nor2(gb_state.win_ctrl.RYFA_WIN_FETCHn_A_evn.qn_new(), gb_state.win_ctrl.RENE_WIN_FETCHn_B_evn.qp_new());
  /*_p27.TUXY*/ wire TUXY_WIN_FIRST_TILEne = nand2(SYLO_WIN_HITn_odd_new, gb_state.win_ctrl.SOVY_WIN_HITp_evn.qp_new());
  /*_p27.SUZU*/ wire SUZU_WIN_FIRST_TILEne = not1(TUXY_WIN_FIRST_TILEne);
  /*_p27.TEVO*/ wire TEVO_WIN_FETCH_TRIGp = or3(SEKO_WIN_FETCH_TRIGp_evn, SUZU_WIN_FIRST_TILEne, TAVE_PRELOAD_DONE_TRIGp); // Schematic wrong, this is OR
  /*#p27.NUNY*/ wire NUNY_WIN_MODE_TRIGp = and2(gb_state.win_ctrl.PYNU_WIN_MODE_Ap_odd.qp_new(), gb_state.win_ctrl.NOPA_WIN_MODE_Bp_evn.qn_new());
  /*_p27.NYFO*/ wire NYFO_WIN_MODE_TRIGn = not1(NUNY_WIN_MODE_TRIGp);
  /*_p27.MOSU*/ wire MOSU_WIN_MODE_TRIGp = not1(NYFO_WIN_MODE_TRIGn);
  /*_p27.NYXU*/ wire NYXU_BFETCH_RSTn = nor3(gb_state.sprite_scanner.AVAP_SCAN_DONE_tp_odd.out_new(), MOSU_WIN_MODE_TRIGp, TEVO_WIN_FETCH_TRIGp);

  for (int feedback = 0; feedback < 2; feedback++) {
    /*_p27.MOCE*/ wire MOCE_BFETCH_DONEn = nand3(gb_state.tfetch_counter.LAXU_BFETCH_S0p_odd.qp_any(), gb_state.tfetch_counter.NYVA_BFETCH_S2p_odd.qp_any(), NYXU_BFETCH_RSTn);
    /*_p27.LEBO*/ wire LEBO_ODD = nand2(gb_state.sys_clk.ALET_evn(), MOCE_BFETCH_DONEn);

    /*_p27.LAXU*/ gb_state.tfetch_counter.LAXU_BFETCH_S0p_odd.dff17_any(LEBO_ODD,                                             NYXU_BFETCH_RSTn, gb_state.tfetch_counter.LAXU_BFETCH_S0p_odd.qn_any());
    /*_p27.MESU*/ gb_state.tfetch_counter.MESU_BFETCH_S1p_odd.dff17_any(gb_state.tfetch_counter.LAXU_BFETCH_S0p_odd.qn_any(), NYXU_BFETCH_RSTn, gb_state.tfetch_counter.MESU_BFETCH_S1p_odd.qn_any());
    /*_p27.NYVA*/ gb_state.tfetch_counter.NYVA_BFETCH_S2p_odd.dff17_any(gb_state.tfetch_counter.MESU_BFETCH_S1p_odd.qn_any(), NYXU_BFETCH_RSTn, gb_state.tfetch_counter.NYVA_BFETCH_S2p_odd.qn_any());
  }

  /*_p27.LOVY*/ gb_state.tfetch_control.LOVY_FETCH_DONEp.dff17(gb_state.sys_clk.MYVO_odd(), NYXU_BFETCH_RSTn, reg_old.tfetch_control.LYRY_BFETCH_DONEp_odd.out_old());

  /*_p27.MOCE*/ wire MOCE_BFETCH_DONEn = nand3(gb_state.tfetch_counter.LAXU_BFETCH_S0p_odd.qp_new(), gb_state.tfetch_counter.NYVA_BFETCH_S2p_odd.qp_new(), NYXU_BFETCH_RSTn);
  /*_p27.LYRY*/ gb_state.tfetch_control.LYRY_BFETCH_DONEp_odd <<= not1(MOCE_BFETCH_DONEn);

  /*_p27.LURY*/ wire LURY_BG_FETCH_DONEn = and2(gb_state.tfetch_control.LOVY_FETCH_DONEp.qn_new(), gb_state.XYMU_RENDERINGn.qn_new());
  /*_p27.LONY*/ gb_state.tfetch_control.LONY_FETCHINGp.nand_latch(NYXU_BFETCH_RSTn, LURY_BG_FETCH_DONEn);


  //----------------------------------------
  // Fine match counter

  /*#p27.PASO*/ wire PASO_FINE_RST = nor2(PAHA_RENDERINGn, TEVO_WIN_FETCH_TRIGp);

  /*#p27.ROZE*/ wire ROZE_FINE_COUNT_7n_odd = nand3(reg_new.fine_count_odd.RUBU_FINE_CNT2_odd.qp_any(), reg_new.fine_count_odd.ROGA_FINE_CNT1_odd.qp_any(), reg_new.fine_count_odd.RYKU_FINE_CNT0_odd.qp_any());
  /*#p27.PECU*/ wire PECU_FINE_CLK_odd = nand2(ROXO_CLKPIPE_evn_new, ROZE_FINE_COUNT_7n_odd);
  
  /*#p27.RYKU*/ reg_new.fine_count_odd.RYKU_FINE_CNT0_odd.dff17_any(PECU_FINE_CLK_odd,                               PASO_FINE_RST, reg_new.fine_count_odd.RYKU_FINE_CNT0_odd.qn_any());
  /*#p27.ROGA*/ reg_new.fine_count_odd.ROGA_FINE_CNT1_odd.dff17_any(reg_new.fine_count_odd.RYKU_FINE_CNT0_odd.qn_any(), PASO_FINE_RST, reg_new.fine_count_odd.ROGA_FINE_CNT1_odd.qn_any());
  /*#p27.RUBU*/ reg_new.fine_count_odd.RUBU_FINE_CNT2_odd.dff17_any(reg_new.fine_count_odd.ROGA_FINE_CNT1_odd.qn_any(), PASO_FINE_RST, reg_new.fine_count_odd.RUBU_FINE_CNT2_odd.qn_any());

  ROZE_FINE_COUNT_7n_odd = nand3(reg_new.fine_count_odd.RUBU_FINE_CNT2_odd.qp_any(), reg_new.fine_count_odd.ROGA_FINE_CNT1_odd.qp_any(), reg_new.fine_count_odd.RYKU_FINE_CNT0_odd.qp_any());
  PECU_FINE_CLK_odd = nand2(ROXO_CLKPIPE_evn_new, ROZE_FINE_COUNT_7n_odd);
  
  /*#p27.RYKU*/ reg_new.fine_count_odd.RYKU_FINE_CNT0_odd.dff17_any(PECU_FINE_CLK_odd,                               PASO_FINE_RST, reg_new.fine_count_odd.RYKU_FINE_CNT0_odd.qn_any());
  /*#p27.ROGA*/ reg_new.fine_count_odd.ROGA_FINE_CNT1_odd.dff17_any(reg_new.fine_count_odd.RYKU_FINE_CNT0_odd.qn_any(), PASO_FINE_RST, reg_new.fine_count_odd.ROGA_FINE_CNT1_odd.qn_any());
  /*#p27.RUBU*/ reg_new.fine_count_odd.RUBU_FINE_CNT2_odd.dff17_any(reg_new.fine_count_odd.ROGA_FINE_CNT1_odd.qn_any(), PASO_FINE_RST, reg_new.fine_count_odd.RUBU_FINE_CNT2_odd.qn_any());


  {
    ///*#p27.SYLO*/ wire SYLO_WIN_HITn_odd_new = not1(reg_new.win_ctrl.RYDY_WIN_HITp_odd.out_new());
    ///*#p24.TOMU*/ wire TOMU_WIN_HITp_odd_new = not1(SYLO_WIN_HITn_odd_new);
    ///*_p27.TUKU*/ wire TUKU_WIN_HITn_odd_new = not1(TOMU_WIN_HITp_odd_new);
    ///*_p27.SOWO*/ wire SOWO_SFETCH_RUNNINGn_evn_new = not1(reg_new.sfetch_control.TAKA_SFETCH_RUNNINGp_evn.qp_new());
    ///*_p27.TEKY*/ wire TEKY_SFETCH_REQp_odd_new = and4(reg_new.FEPO_STORE_MATCHp_odd.out_new(), TUKU_WIN_HITn_odd_new, reg_new.tfetch_control.LYRY_BFETCH_DONEp_odd.out_new(), SOWO_SFETCH_RUNNINGn_evn_new);
    //probe_wire(17, "TEKY_NEW",  TEKY_SFETCH_REQp_odd_new);
  }

  //----------------------------------------
  // PPU / LCD output

  tock_pix_pipes_gates(reg_old, SACU_CLKPIPE_odd_new, NYXU_BFETCH_RSTn);
  set_lcd_pins_gates(SACU_CLKPIPE_odd_new);

  //----------------------------------------
  // Audio

  //tock_spu_gates();

  //----------------------------------------
  // Memory buses

  tock_ext_gates(cart_blob);
  tock_vram_bus_gates(reg_old, TEVO_WIN_FETCH_TRIGp);
  tock_oam_bus_gates();
  tock_zram_gates(reg_old);

  //----------------------------------------
  // And finally, interrupts.

  tock_interrupts_gates(reg_old);
}

//-----------------------------------------------------------------------------
