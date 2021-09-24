#include "GateBoyLib/LogicBoy.h"
#include "GateBoyLib/GateBoy.h"

FieldInfo LogicBoy::fields[] = {
  DECLARE_FIELD(LogicBoy, lb_state),
  DECLARE_FIELD(LogicBoy, gb_state),
  DECLARE_FIELD(LogicBoy, cpu),
  DECLARE_FIELD(LogicBoy, mem),
  DECLARE_FIELD(LogicBoy, sys),
  DECLARE_FIELD(LogicBoy, pins),
  DECLARE_FIELD(LogicBoy, probes),
};

#define gb_state FORBIDDEN

//-----------------------------------------------------------------------------

GBResult LogicBoy::reset_to_bootrom(const blob& cart_blob)
{
  lb_state.reset_to_bootrom();
  cpu.reset_to_bootrom();
  mem.reset_to_bootrom();
  sys.reset_to_bootrom();
  pins.reset_to_bootrom();
  probes.reset_to_bootrom();
  return GBResult::ok();
}

//-----------------------------------------------------------------------------

GBResult LogicBoy::reset_to_cart(const blob& cart_blob) {
  lb_state.reset_to_cart();
  cpu.reset_to_cart();
  mem.reset_to_cart();
  sys.reset_to_cart();
  pins.reset_to_cart();
  probes.reset_to_cart();
  return GBResult::ok();
}

//-----------------------------------------------------------------------------

GBResult LogicBoy::peek(int addr) const {
  if (addr >= 0x8000 && addr <= 0x9FFF) { return GBResult(mem.vid_ram[addr - 0x8000]);  }
  if (addr >= 0xA000 && addr <= 0xBFFF) { return GBResult(mem.cart_ram[addr - 0xA000]); }
  if (addr >= 0xC000 && addr <= 0xDFFF) { return GBResult(mem.int_ram[addr - 0xC000]);  }
  if (addr >= 0xE000 && addr <= 0xFDFF) { return GBResult(mem.int_ram[addr - 0xE000]);  }
  if (addr >= 0xFE00 && addr <= 0xFEFF) { return GBResult(mem.oam_ram[addr - 0xFE00]);  }
  if (addr >= 0xFF80 && addr <= 0xFFFE) { return GBResult(mem.zero_ram[addr - 0xFF80]); }
  return lb_state.peek(addr);
}

GBResult LogicBoy::poke(int addr, uint8_t data_in) {
  if (addr >= 0x8000 && addr <= 0x9FFF) { mem.vid_ram[addr - 0x8000] = data_in;  return GBResult::ok(); }
  if (addr >= 0xA000 && addr <= 0xBFFF) { mem.cart_ram[addr - 0xA000] = data_in; return GBResult::ok(); }
  if (addr >= 0xC000 && addr <= 0xDFFF) { mem.int_ram[addr - 0xC000] = data_in;  return GBResult::ok(); }
  if (addr >= 0xE000 && addr <= 0xFDFF) { mem.int_ram[addr - 0xE000] = data_in;  return GBResult::ok(); }
  if (addr >= 0xFE00 && addr <= 0xFEFF) { mem.oam_ram[addr - 0xFE00] = data_in;  return GBResult::ok(); }
  if (addr >= 0xFF80 && addr <= 0xFFFE) { mem.zero_ram[addr - 0xFF80] = data_in; return GBResult::ok(); }
  return lb_state.poke(addr, data_in);
}

//-----------------------------------------------------------------------------

GBResult LogicBoy::dbg_req(uint16_t addr, uint8_t data, bool write) {
  CHECK_P((sys.gb_phase_total & 7) == 0);

  cpu.bus_req_new.addr = addr;
  cpu.bus_req_new.data = data;
  cpu.bus_req_new.read = !write;
  cpu.bus_req_new.write = write;

  return GBResult::ok();
}

//-----------------------------------------------------------------------------

GBResult LogicBoy::dbg_read(const blob& cart_blob, int addr) {
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

//------------------------------------------------------------------------------

GBResult LogicBoy::dbg_write(const blob& cart_blob, int addr, uint8_t data_in) {
  CHECK_P((sys.gb_phase_total & 7) == 0);

  Req old_req = cpu.bus_req_new;
  bool old_cpu_en = sys.cpu_en;
  sys.cpu_en = false;

  dbg_req((uint16_t)addr, data_in, 1);
  GBResult res = run_phases(cart_blob, 8);

  cpu.bus_req_new = old_req;
  sys.cpu_en = old_cpu_en;
  return res;
}

//------------------------------------------------------------------------------------------------------------------------

GBResult LogicBoy::run_phases(const blob& cart_blob, int phase_count) {
  GBResult res = GBResult::ok();
  for (int i = 0; i < phase_count; i++) {
    res &= next_phase(cart_blob);
  }
  return res;
}

GBResult LogicBoy::next_phase(const blob& cart_blob) {
  sys.gb_phase_total++;
  tock_cpu();
  tock_logic(cart_blob);
  return GBResult::ok();
}

//------------------------------------------------------------------------------------------------------------------------

#define PHASE_A_NEW (phase_new == 0)
#define PHASE_B_NEW (phase_new == 1)
#define PHASE_C_NEW (phase_new == 2)
#define PHASE_D_NEW (phase_new == 3)
#define PHASE_E_NEW (phase_new == 4)
#define PHASE_F_NEW (phase_new == 5)
#define PHASE_G_NEW (phase_new == 6)
#define PHASE_H_NEW (phase_new == 7)

void LogicBoy::tock_cpu() {
  auto phase_old = (sys.gb_phase_total - 1) & 7;
  auto phase_new = (sys.gb_phase_total - 0) & 7;

  cpu.cpu_data_latch &= lb_state.cpu_dbus;
  cpu.imask_latch = lb_state.reg_ie;

  if (PHASE_A_NEW) {
    if (cpu.core.op == 0x76 && (cpu.imask_latch & cpu.intf_halt_latch)) cpu.core.state_ = 0;
    cpu.intf_halt_latch = 0;
  }

  // +ha -ab -bc -cd -de -ef -fg -gh
  if (PHASE_A_NEW) {
    // this one latches funny, some hardware bug
    if (get_bit(lb_state.reg_if, 2)) cpu.intf_halt_latch |= INT_TIMER_MASK;
  }

  // -ha +ab -bc
  if (PHASE_B_NEW) {
    if (sys.cpu_en) {
      cpu.core.tock_ab(cpu.imask_latch, cpu.intf_latch, cpu.cpu_data_latch);
    }
  }

  if (PHASE_B_NEW) {
    if (sys.cpu_en) {
      cpu.bus_req_new.addr = cpu.core._bus_addr;
      cpu.bus_req_new.data = cpu.core._bus_data;
      cpu.bus_req_new.read = cpu.core._bus_read;
      cpu.bus_req_new.write = cpu.core._bus_write;
    }
  }

  // -bc +cd +de -ef -fg -gh -ha -ab
  if (PHASE_E_NEW) {
    if (get_bit(lb_state.reg_if, 0)) cpu.intf_halt_latch |= INT_VBLANK_MASK;
    if (get_bit(lb_state.reg_if, 1)) cpu.intf_halt_latch |= INT_STAT_MASK;
    if (get_bit(lb_state.reg_if, 3)) cpu.intf_halt_latch |= INT_SERIAL_MASK;
    if (get_bit(lb_state.reg_if, 4)) cpu.intf_halt_latch |= INT_JOYPAD_MASK;
  }

  // -ha -ab -bc -cd -de -ef +fg +gh
  if (PHASE_H_NEW) {
    cpu.cpu_data_latch = 0xFF;
  }

  // +ha -ab -bc -cd -de -ef -fg +gh
  if (PHASE_H_NEW) {
    cpu.intf_latch = lb_state.reg_if;
  }
}

//-----------------------------------------------------------------------------

#define DELTA_AB   (phase_old == 0)
#define DELTA_BC   (phase_old == 1)
#define DELTA_CD   (phase_old == 2)
#define DELTA_DE   (phase_old == 3)
#define DELTA_EF   (phase_old == 4)
#define DELTA_FG   (phase_old == 5)
#define DELTA_GH   (phase_old == 6)
#define DELTA_HA   (phase_old == 7)

#define DELTA_EVEN  ((phase_old & 1) == 0)
#define DELTA_ODD  ((phase_old & 1) == 1)

void LogicBoy::tock_logic(const blob& cart_blob) {
  LogicBoyState  state_old = lb_state;
  LogicBoyState& state_new = lb_state;

  auto phase_total_old = sys.gb_phase_total - 1;
  auto phase_total_new = sys.gb_phase_total - 0;

  auto phase_old = (sys.gb_phase_total - 1) & 7;
  auto phase_new = (sys.gb_phase_total - 0) & 7;

  //----------------------------------------

  // Data has to be driven on EFGH or we fail the wave tests
  state_new.cpu_dbus = (DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH) && cpu.bus_req_new.write ? cpu.bus_req_new.data_lo : 0xFF; // must be pulled up or we fail regression
  state_new.cpu_abus = (DELTA_HA ? cpu.bus_req_new.addr & 0x00FF : cpu.bus_req_new.addr);

  const bool req_addr_vram = (cpu.bus_req_new.addr >= 0x8000) && (cpu.bus_req_new.addr <= 0x9FFF);
  const bool req_addr_ram  = (cpu.bus_req_new.addr >= 0xA000) && (cpu.bus_req_new.addr <= 0xFDFF);
  const bool req_addr_oam  = (cpu.bus_req_new.addr >= 0xFE00) && (cpu.bus_req_new.addr <= 0xFEFF);
  const bool req_addr_hi   = (cpu.bus_req_new.addr >= 0xFE00);
  const bool req_addr_lo   = (cpu.bus_req_new.addr <= 0x00FF);

  const bool cpu_addr_oam_old  = (state_old.cpu_abus >= 0xFE00) && (state_old.cpu_abus <= 0xFEFF);

  const bool cpu_addr_vram_new = req_addr_vram && !DELTA_HA;
  const bool cpu_addr_ram_new  = req_addr_ram && !DELTA_HA;
  const bool cpu_addr_oam_new  = req_addr_oam && !DELTA_HA;

  const bool cpu_rd = cpu.bus_req_new.read  && !DELTA_HA;
  const bool cpu_wr = cpu.bus_req_new.write && !DELTA_HA;

  const auto cpu_addr_new = state_new.cpu_abus;

  //----------------------------------------
  // DIV

  const uint16_t div_old = state_old.reg_div;

  if (DELTA_HA) {
    state_new.reg_div = state_new.reg_div + 1;
  }

  if (cpu_wr && (DELTA_DE || DELTA_EF || DELTA_FG)) {
    if (cpu_addr_new == 0xFF04) state_new.reg_div = 0;
  }

  const uint16_t div_new = state_new.reg_div;

  //----------------------------------------

  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF00) {
    set_bit(state_new.reg_joy, 0, get_bit(state_old.cpu_dbus, 4));
    set_bit(state_new.reg_joy, 1, get_bit(state_old.cpu_dbus, 5));
  }
  // LCDC write has to be near the top as it controls the video reset signal
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF40) state_new.reg_lcdc = ~state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF45) state_new.reg_lyc  = ~state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF06) state_new.reg_tma  =  state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF07) state_new.reg_tac  =  state_old.cpu_dbus & 0b111;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF42) state_new.reg_scy  = ~state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF43) state_new.reg_scx  = ~state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF47) state_new.reg_bgp  = ~state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF48) state_new.reg_obp0 = ~state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF49) state_new.reg_obp1 = ~state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF4A) state_new.reg_wy   = ~state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF4B) state_new.reg_wx   = ~state_old.cpu_dbus;
  if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF50) state_new.cpu_signals.TEPU_BOOT_BITn.state = get_bit(state_old.cpu_dbus, 0) || state_old.cpu_signals.TEPU_BOOT_BITn;

  if (cpu_wr && DELTA_GH && (cpu_addr_new >= 0xFF80) && (cpu_addr_new <= 0xFFFE)) mem.zero_ram[cpu_addr_new & 0x007F] = state_old.cpu_dbus;

  const bool req_addr_boot = (cpu.bus_req_new.addr >= 0x0000) && (cpu.bus_req_new.addr <= 0x00FF) && !state_new.cpu_signals.TEPU_BOOT_BITn;

  const bool ext_addr_new = (cpu.bus_req_new.read || cpu.bus_req_new.write) &&
                            (( DELTA_HA && !req_addr_hi && !req_addr_boot && !req_addr_vram) ||
                            (!DELTA_HA && !req_addr_hi && !req_addr_boot));

  bool cpu_addr_bootrom_new = req_addr_lo && !state_new.cpu_signals.TEPU_BOOT_BITn;

  const bool vid_rst_old  =  get_bit(state_old.reg_lcdc, 7);

  const bool bgw_en_new   = !get_bit(state_new.reg_lcdc, 0);
  const bool spr_en_new   = !get_bit(state_new.reg_lcdc, 1);
  const bool spr_size_new =  get_bit(state_new.reg_lcdc, 2);
  const bool bg_map_new   =  get_bit(state_new.reg_lcdc, 3);
  const bool bgw_tile_new =  get_bit(state_new.reg_lcdc, 4);
  const bool win_en_new   = !get_bit(state_new.reg_lcdc, 5);
  const bool win_map_new  =  get_bit(state_new.reg_lcdc, 6);
  const bool vid_rst_new  =  get_bit(state_new.reg_lcdc, 7);


  //----------------------------------------
  // LX, LY, lcd flags

  const bool vblank_old = state_old.lcd.POPU_VBLANKp_odd.state;
  const uint8_t ly_old = state_old.reg_ly;

  if (DELTA_BC) {
    if (!state_new.lcd.NYPE_LINE_ENDp_odd && state_new.lcd.RUTU_LINE_ENDp_odd) {
      state_new.lcd.POPU_VBLANKp_odd.state = state_new.reg_ly >= 144;
      state_new.lcd.MYTA_FRAME_ENDp_odd.state = state_new.reg_ly == 153;
      if (state_new.reg_ly == 153) state_new.reg_ly = 0;
    }
    state_new.lcd.NYPE_LINE_ENDp_odd.state = state_new.lcd.RUTU_LINE_ENDp_odd.state;
    state_new.reg_lx++;
    if (state_new.lcd.RUTU_LINE_ENDp_odd) state_new.reg_lx = 0;
  }

  if (DELTA_FG) {
    if (!state_new.lcd.MYTA_FRAME_ENDp_odd && !state_old.lcd.RUTU_LINE_ENDp_odd && (state_new.reg_lx == 113)) {
      state_new.reg_ly++;
    }
    state_new.lcd.RUTU_LINE_ENDp_odd.state = (state_new.reg_lx == 113);
    if (state_new.reg_lx == 113) state_new.reg_lx = 0;
  }

  if (vid_rst_new) {
    state_new.lcd.NYPE_LINE_ENDp_odd.state = 0;
    state_new.lcd.RUTU_LINE_ENDp_odd.state = 0;
    state_new.lcd.POPU_VBLANKp_odd.state = 0;
    state_new.lcd.MYTA_FRAME_ENDp_odd.state = 0;
    state_new.reg_lx = 0;
    state_new.reg_ly = 0;
  }

  const bool vblank_new = state_new.lcd.POPU_VBLANKp_odd.state;
  const uint8_t ly_new = state_new.reg_ly;

  //----------------------------------------
  // Line reset trigger

  bool line_rst_old = (state_old.lcd.CATU_x113p_odd && !state_old.lcd.ANEL_x113p_odd);

  if (vid_rst_new) {
    state_new.lcd.ANEL_x113p_odd.state = 0;
    state_new.lcd.CATU_x113p_odd.state = 0;
  }
  else {
    if (DELTA_HA || DELTA_DE) {
      state_new.lcd.CATU_x113p_odd.state = state_new.lcd.RUTU_LINE_ENDp_odd && (ly_new < 144);
    }

    if (DELTA_BC || DELTA_FG) {
      state_new.lcd.ANEL_x113p_odd = state_new.lcd.CATU_x113p_odd;
    }
  }

  bool line_rst_new_odd = (state_new.lcd.CATU_x113p_odd && !state_new.lcd.ANEL_x113p_odd);

  //----------------------------------------
  // Joypad

  // FIXME what if both scan bits are set?

  if (DELTA_HA) {
    uint8_t button_mask = 0b00000000;
    if (!get_bit(state_new.reg_joy, 0)) button_mask |= 0b00001111;
    if (!get_bit(state_new.reg_joy, 1)) button_mask |= 0b11110000;

    state_new.int_ctrl.AWOB_WAKE_CPU.state  = !(sys.buttons & button_mask);
    state_new.int_ctrl.SIG_CPU_WAKE.state   = !(sys.buttons & button_mask);
    state_new.joy_int.APUG_JP_GLITCH3       = state_new.joy_int.AGEM_JP_GLITCH2;
    state_new.joy_int.AGEM_JP_GLITCH2       = state_new.joy_int.ACEF_JP_GLITCH1;
    state_new.joy_int.ACEF_JP_GLITCH1       = state_new.joy_int.BATU_JP_GLITCH0;
    state_new.joy_int.BATU_JP_GLITCH0.state = !(sys.buttons & button_mask);
  }
  else if (!cpu_rd) {
    uint8_t button_mask = 0b00000000;
    if (!get_bit(state_new.reg_joy, 0)) button_mask |= 0b00001111;
    if (!get_bit(state_new.reg_joy, 1)) button_mask |= 0b11110000;
    state_new.joy_latch = (((sys.buttons & button_mask) >> 0) | ((sys.buttons & button_mask) >> 4)) & 0b1111;
  }

  //----------------------------------------
  //tock_serial_logic();
  //tock_timer_logic();

  //----------------------------------------
  // Timer

  bool SOGU_TIMA_CLKn_old = 0;
  bool SOGU_TIMA_CLKn_new = 0;

  if ((state_old.reg_tac & 0b111) == 4) SOGU_TIMA_CLKn_old = (div_old >> 7) & 1;
  if ((state_old.reg_tac & 0b111) == 5) SOGU_TIMA_CLKn_old = (div_old >> 1) & 1;
  if ((state_old.reg_tac & 0b111) == 6) SOGU_TIMA_CLKn_old = (div_old >> 3) & 1;
  if ((state_old.reg_tac & 0b111) == 7) SOGU_TIMA_CLKn_old = (div_old >> 5) & 1;

  if ((state_new.reg_tac & 0b111) == 4) SOGU_TIMA_CLKn_new = (div_new >> 7) & 1;
  if ((state_new.reg_tac & 0b111) == 5) SOGU_TIMA_CLKn_new = (div_new >> 1) & 1;
  if ((state_new.reg_tac & 0b111) == 6) SOGU_TIMA_CLKn_new = (div_new >> 3) & 1;
  if ((state_new.reg_tac & 0b111) == 7) SOGU_TIMA_CLKn_new = (div_new >> 5) & 1;

  if (DELTA_HA) {
    state_new.int_ctrl.MOBA_TIMER_OVERFLOWp.state = !get_bit(state_old.reg_tima, 7) && state_old.int_ctrl.NYDU_TIMA7p_DELAY;
    state_new.int_ctrl.NYDU_TIMA7p_DELAY.state = get_bit(state_old.reg_tima, 7);
  }

  if (SOGU_TIMA_CLKn_old && !SOGU_TIMA_CLKn_new) {
    state_new.reg_tima = state_new.reg_tima + 1;
  }

  if ((DELTA_DE || DELTA_EF || DELTA_FG) && cpu_wr && !cpu_rd && cpu_addr_new == 0xFF05) {
    state_new.int_ctrl.NYDU_TIMA7p_DELAY.state = 0;
    state_new.reg_tima = state_new.cpu_dbus;
  }
  else if (state_new.int_ctrl.MOBA_TIMER_OVERFLOWp) {
    state_new.int_ctrl.NYDU_TIMA7p_DELAY.state = 0;
    state_new.reg_tima = state_new.reg_tma;
  }

  //----------------------------------------
  // bootrom read

  if (cpu_addr_new <= 0x00FF) {
    if (cpu_rd && !state_new.cpu_signals.TEPU_BOOT_BITn) {
      state_new.cpu_dbus = DMG_ROM_blob[cpu_addr_new & 0xFF];
    }
  }

  // boot bit read

  if (cpu_rd && cpu_addr_new == 0xFF50) {
    state_new.cpu_dbus &= ~1;
    state_new.cpu_dbus |= state_new.cpu_signals.TEPU_BOOT_BITn.state;
  }

  //----------------------------------------
  // DMA
  // this block is all sealed up
  // +dma_ctrl +reg_dma +dma_lo

  const auto dma_running_old = state_old.MATU_DMA_RUNNINGp;

  {
    auto& ctrl = state_new.dma_ctrl;
    auto& dma_lo = state_new.dma_lo;
    auto& dma_hi = state_new.reg_dma;

    if (cpu_wr && DELTA_GH && cpu_addr_new == 0xFF46) dma_hi  = ~state_old.cpu_dbus;
    if (cpu_rd && cpu_addr_new == 0xFF46) state_new.cpu_dbus = ~dma_hi;

    if (DELTA_HA) {
      ctrl.LUVY_DMA_TRIG_d0_odd.state = ctrl.LYXE_DMA_LATCHp;
      if (ctrl.LOKY_DMA_LATCHp_odd && !ctrl.LENE_DMA_TRIG_d4_odd) dma_lo++;
      state_new.MATU_DMA_RUNNINGp = ctrl.LOKY_DMA_LATCHp_odd;
    }
    else if (DELTA_DE) {
      if (cpu_wr && cpu_addr_new == 0xFF46) {
        ctrl.LYXE_DMA_LATCHp.state = 1;
      }

      if (state_old.dma_lo == 159) {
        ctrl.MYTE_DMA_DONE_odd.state = 1;
        ctrl.LARA_DMA_LATCHn_odd = 1;
        ctrl.LOKY_DMA_LATCHp_odd = 0;
      }

      ctrl.LENE_DMA_TRIG_d4_odd = state_old.dma_ctrl.LUVY_DMA_TRIG_d0_odd;

      if (ctrl.LUVY_DMA_TRIG_d0_odd) {
        ctrl.MYTE_DMA_DONE_odd.state = 0;
        ctrl.LYXE_DMA_LATCHp.state = 0;
        ctrl.LARA_DMA_LATCHn_odd = 0;
        ctrl.LOKY_DMA_LATCHp_odd = 1;
        dma_lo = 0;
      }
    }
  }
    
  const auto dma_running_new = state_new.MATU_DMA_RUNNINGp;
  const auto dma_addr_new = ((state_new.reg_dma ^ 0xFF) << 8) | state_new.dma_lo;
  const bool dma_addr_vram_new = (dma_addr_new >= 0x8000) && (dma_addr_new <= 0x9FFF);

  //----------------------------------------

  const bool scan_done_trig_old = state_old.sprite_scanner.BYBA_SCAN_DONEp_odd && !state_old.sprite_scanner.DOBA_SCAN_DONEp_evn;
  const bool hblank_old = (!state_old.FEPO_STORE_MATCHp_odd && (state_old.pix_count == 167));
  const bool pause_pipe_old = state_old.win_ctrl.RYDY_WIN_HITp_odd || !state_old.tfetch_control.POKY_PRELOAD_LATCHp_evn || state_old.FEPO_STORE_MATCHp_odd || hblank_old;

  const bool rendering_old = !state_old.XYMU_RENDERINGn;

  const bool win_mode_trig_old_odd   = state_old.win_ctrl.PYNU_WIN_MODE_Ap_odd && !state_old.win_ctrl.NOPA_WIN_MODE_Bp_evn;
  const bool win_fetch_trig_old_evn  = state_old.win_ctrl.RYFA_WIN_FETCHn_A_evn && !state_old.win_ctrl.RENE_WIN_FETCHn_B_evn;
  const bool win_hit_trig_old_vn    = state_old.win_ctrl.SOVY_WIN_HITp_evn && !state_old.win_ctrl.RYDY_WIN_HITp_odd;
  const bool tile_fetch_trig_old = rendering_old && !state_old.tfetch_control.POKY_PRELOAD_LATCHp_evn && state_old.tfetch_control.NYKA_FETCH_DONEp_evn && state_old.tfetch_control.PORY_FETCH_DONEp_odd;


  const bool fetch_rst_old = scan_done_trig_old || win_mode_trig_old_odd || win_fetch_trig_old_evn || win_hit_trig_old_vn || tile_fetch_trig_old;

  //----------------------------------------
  // Sprite scanner

  if (vid_rst_new || line_rst_new_odd) {
    state_new.scan_counter = 0;
    state_new.sprite_counter = 0;
    state_new.sprite_store_flags = 0;

    state_new.sprite_scanner.DOBA_SCAN_DONEp_evn.state = 0;
    state_new.sprite_scanner.BYBA_SCAN_DONEp_odd.state = 0;
    state_new.sprite_scanner.BESU_SCAN_DONEn_odd.state = line_rst_new_odd && !vid_rst_new; // wat?

    state_new.store_x0 = 0xFF;
    state_new.store_x1 = 0xFF;
    state_new.store_x2 = 0xFF;
    state_new.store_x3 = 0xFF;
    state_new.store_x4 = 0xFF;
    state_new.store_x5 = 0xFF;
    state_new.store_x6 = 0xFF;
    state_new.store_x7 = 0xFF;
    state_new.store_x8 = 0xFF;
    state_new.store_x9 = 0xFF;

    if (vid_rst_new) {
      state_new.sprite_scanner.CENO_SCAN_DONEn_odd.state = 0;
      state_new.sprite_scanner.DEZY_INC_COUNTn_odd.state = 0;
    }
  }
  else {
    int ly = (int)ly_new;
    int sy = (int)state_new.oam_temp_a - 16;
    int sprite_height = spr_size_new ? 8 : 16;
    const bool sprite_hit = ly >= sy && ly < sy + sprite_height && state_new.sprite_scanner.CENO_SCAN_DONEn_odd;

    if (DELTA_HA || DELTA_DE) {
      state_new.sprite_scanner.CENO_SCAN_DONEn_odd.state = state_new.sprite_scanner.BESU_SCAN_DONEn_odd;
      if (state_new.scan_counter == 39) {
        state_new.sprite_scanner.BYBA_SCAN_DONEp_odd.state = 1;
        state_new.sprite_scanner.BESU_SCAN_DONEn_odd.state = 0;
      }
      else {
        state_new.scan_counter++;
      }
      if (!state_new.sprite_scanner.DEZY_INC_COUNTn_odd && state_new.sprite_counter < 10) state_new.sprite_counter++;
      state_new.sprite_scanner.DEZY_INC_COUNTn_odd.state = 1;
    }
    if (DELTA_AB || DELTA_EF) {
      state_new.sprite_scanner.DOBA_SCAN_DONEp_evn = state_new.sprite_scanner.BYBA_SCAN_DONEp_odd;

      if (sprite_hit) {
        state_new.sprite_store_flags = (1 << state_new.sprite_counter);
        (&state_new.store_i0)[state_new.sprite_counter] = state_new.sprite_ibus ^ 0b111111;
        (&state_new.store_l0)[state_new.sprite_counter] = state_new.sprite_lbus ^ 0b1111;
      }
      else {
        state_new.sprite_store_flags = 0;
      }
    }
    if (DELTA_BC || DELTA_FG) {
      if (sprite_hit) {
        state_new.sprite_scanner.DEZY_INC_COUNTn_odd.state = 0;
      }
    }
    if (DELTA_CD || DELTA_GH) {
      state_new.sprite_scanner.DOBA_SCAN_DONEp_evn = state_new.sprite_scanner.BYBA_SCAN_DONEp_odd;
      if (state_new.sprite_store_flags) {
        (&state_new.store_x0)[state_new.sprite_counter] = state_new.oam_temp_b;
      }
      state_new.sprite_store_flags = 0;
    }
  }

  const bool scan_done_trig_new = state_new.sprite_scanner.BYBA_SCAN_DONEp_odd && !state_new.sprite_scanner.DOBA_SCAN_DONEp_evn;

  //----------------------------------------

  if (DELTA_EVEN) {
    if (hblank_old) state_new.XYMU_RENDERINGn = 1;
  }
  if (scan_done_trig_new) {
    state_new.XYMU_RENDERINGn = 0;
  }

  if (vid_rst_new || line_rst_new_odd) {
    state_new.XYMU_RENDERINGn = 1;
  }

  const bool rendering_new = !state_new.XYMU_RENDERINGn;

  //----------------------------------------
  // Sprite fetch state counter

  const bool tfetch_count_max_old = state_old.tfetch_counter == 5;
  const bool sfetch_trig_old = state_old.sfetch_control.SOBU_SFETCH_REQp_evn && !state_old.sfetch_control.SUDA_SFETCH_REQp_odd;

  if (DELTA_EVEN) {
    state_new.sfetch_control.SOBU_SFETCH_REQp_evn.state =
        state_old.FEPO_STORE_MATCHp_odd &&
       !state_old.win_ctrl.RYDY_WIN_HITp_odd &&
       !fetch_rst_old &&
        tfetch_count_max_old &&
       !state_old.sfetch_control.TAKA_SFETCH_RUNNINGp_evn;

    state_new.sfetch_control.VONU_SFETCH_S1p_D4_evn = state_old.sfetch_control.TOBU_SFETCH_S1p_D2_evn;
    state_new.sfetch_control.TOBU_SFETCH_S1p_D2_evn.state = get_bit(state_old.sfetch_counter_evn, 1);

    if (state_new.sfetch_control.SOBU_SFETCH_REQp_evn && !state_new.sfetch_control.SUDA_SFETCH_REQp_odd) {
      state_new.sfetch_counter_evn = 0;
      state_new.sfetch_control.TAKA_SFETCH_RUNNINGp_evn.state = 1;
    }
    else {
      if (state_new.sfetch_counter_evn != 5) state_new.sfetch_counter_evn++;
    }
  }

  if (DELTA_ODD) {
    state_new.sfetch_control.SUDA_SFETCH_REQp_odd   = state_old.sfetch_control.SOBU_SFETCH_REQp_evn;
    state_new.sfetch_control.TYFO_SFETCH_S0p_D1_odd.state = get_bit(state_old.sfetch_counter_evn, 0);
    state_new.sfetch_control.SEBA_SFETCH_S1p_D5_odd = state_new.sfetch_control.VONU_SFETCH_S1p_D4_evn;
  }

  if (vid_rst_new || line_rst_new_odd) {
    state_new.sfetch_counter_evn = 0;
    state_new.sfetch_control.TAKA_SFETCH_RUNNINGp_evn.state = 1;
    state_new.sfetch_control.TOBU_SFETCH_S1p_D2_evn.state = 0;
    state_new.sfetch_control.VONU_SFETCH_S1p_D4_evn.state = 0;
    state_new.sfetch_control.SEBA_SFETCH_S1p_D5_odd.state = 0;
  }

  //if (line_rst_new) {
  //  state_new.sfetch_counter = 0;
  //  state_new.sfetch_control.TAKA_SFETCH_RUNNINGp.state = 1;
  //}

  //----------------------------------------

  const bool wuty_sfetch_done_old =
        !vid_rst_old &&
        rendering_old &&
        state_old.sfetch_control.TYFO_SFETCH_S0p_D1_odd &&
        get_bit(state_old.sfetch_counter_evn, 0) &&
        state_old.sfetch_control.SEBA_SFETCH_S1p_D5_odd &&
        state_old.sfetch_control.VONU_SFETCH_S1p_D4_evn;


  const bool wuty_sfetch_done_new =
        !vid_rst_new &&
        rendering_new &&
        state_new.sfetch_control.TYFO_SFETCH_S0p_D1_odd &&
        get_bit(state_new.sfetch_counter_evn, 0) &&
        state_new.sfetch_control.SEBA_SFETCH_S1p_D5_odd &&
        state_new.sfetch_control.VONU_SFETCH_S1p_D4_evn;

  //----------------------------------------

  if (!vid_rst_new) {
    if (!rendering_new) {
    }
    else {
      if (!wuty_sfetch_done_old && wuty_sfetch_done_new) {
        state_new.sprite_reset_flags = state_old.sprite_match_flags;
      }
    }
  }




  if (vid_rst_new || line_rst_new_odd) {
    state_new.sprite_reset_flags = 0;
  }
  else {
    int sprite_reset_index = 32 - __lzcnt(state_new.sprite_reset_flags - 1);
    if (sprite_reset_index != 32) (&state_new.store_x0)[sprite_reset_index] = 0xFF;
  }

  //----------------------------------------
  // Tile fetch sequencer
  // fetch_rst_new below will reset LOVY, LONY, and the counter

  const uint8_t bfetch_phase_old = pack(
    !(state_old.tfetch_control.LYZU_BFETCH_S0p_D1.state ^ get_bit(state_old.tfetch_counter, 0)),
    get_bit(state_old.tfetch_counter, 0),
    get_bit(state_old.tfetch_counter, 1),
    get_bit(state_old.tfetch_counter, 2));


  if (DELTA_EVEN) {
    state_new.tfetch_control.PYGO_FETCH_DONEp_evn = state_new.tfetch_control.PORY_FETCH_DONEp_odd;
    state_new.tfetch_control.NYKA_FETCH_DONEp_evn.state = (!fetch_rst_old && tfetch_count_max_old);
    state_new.tfetch_control.LYZU_BFETCH_S0p_D1.state = get_bit(state_new.tfetch_counter, 0);

    if (state_new.tfetch_control.PYGO_FETCH_DONEp_evn) {
      state_new.tfetch_control.POKY_PRELOAD_LATCHp_evn.state = 1;
    }
  }

  if (DELTA_ODD) {
    state_new.tfetch_control.PORY_FETCH_DONEp_odd = state_new.tfetch_control.NYKA_FETCH_DONEp_evn;
    if (state_new.tfetch_counter < 5) state_new.tfetch_counter++;
    state_new.tfetch_control.LOVY_FETCH_DONEp.state = (!fetch_rst_old && tfetch_count_max_old);

    if (state_new.tfetch_control.LOVY_FETCH_DONEp) {
      state_new.tfetch_control.LONY_FETCHINGp.state = 0;
    }
  }

  if (vid_rst_new || !rendering_new) {
    state_new.tfetch_control.PYGO_FETCH_DONEp_evn.state = 0;
    state_new.tfetch_control.PORY_FETCH_DONEp_odd.state = 0;
    state_new.tfetch_control.NYKA_FETCH_DONEp_evn.state = 0;
    state_new.tfetch_control.POKY_PRELOAD_LATCHp_evn.state = 0;
  }

  if (!rendering_new) {
    state_new.tfetch_control.LYZU_BFETCH_S0p_D1.state = 0;
    state_new.tfetch_control.LONY_FETCHINGp.state = 0;
  }

  // FIXME PORY hasn't had its reset applied yet
  const bool tile_fetch_trig_new = rendering_new && !state_new.tfetch_control.POKY_PRELOAD_LATCHp_evn && state_new.tfetch_control.NYKA_FETCH_DONEp_evn && state_new.tfetch_control.PORY_FETCH_DONEp_odd;
  const bool tfetch_count_max_new = get_bit(state_new.tfetch_counter, 0) && get_bit(state_new.tfetch_counter, 2);

  //----------------------------------------

  if (!vid_rst_new) {
    if (!rendering_new) {
      state_new.sfetch_control.TOBU_SFETCH_S1p_D2_evn.state = 0;
      state_new.sfetch_control.VONU_SFETCH_S1p_D4_evn.state = 0;
      state_new.sfetch_control.SEBA_SFETCH_S1p_D5_odd.state = 0;
    }
    else {
      if (tile_fetch_trig_new) {
        state_new.sfetch_control.TAKA_SFETCH_RUNNINGp_evn.state = 0;
      }

      if (state_new.tfetch_control.PYGO_FETCH_DONEp_evn) {
      }

      if (wuty_sfetch_done_new) {
        state_new.sfetch_control.TAKA_SFETCH_RUNNINGp_evn.state = 0;
      }
    }
  }

  bool sfetching_old = !vid_rst_old && rendering_new && ((get_bit(state_old.sfetch_counter_evn, 1) || state_old.sfetch_control.VONU_SFETCH_S1p_D4_evn));

  const uint8_t sfetch_phase_old = pack(
    !(state_old.sfetch_control.TYFO_SFETCH_S0p_D1_odd.state ^ get_bit(state_old.sfetch_counter_evn, 0)),
    get_bit(state_old.sfetch_counter_evn, 0),
    get_bit(state_old.sfetch_counter_evn, 1),
    get_bit(state_old.sfetch_counter_evn, 2));

  const uint8_t sfetch_phase_new = pack(
    !(state_new.sfetch_control.TYFO_SFETCH_S0p_D1_odd.state ^ get_bit(state_new.sfetch_counter_evn, 0)),
    get_bit(state_new.sfetch_counter_evn, 0),
    get_bit(state_new.sfetch_counter_evn, 1),
    get_bit(state_new.sfetch_counter_evn, 2));


  //----------------------------------------
  // OAM latch from last cycle gets moved into temp registers.

  {
    wire XYSO_xBCDxFGH_old = !vid_rst_old && gen_clk(phase_old, 0b01110111);
    wire XYSO_xBCDxFGH_new = !vid_rst_new && gen_clk(phase_new, 0b01110111);

    wire CUFE_OAM_CLKp_old       = !((cpu_addr_oam_old || dma_running_old) && (DELTA_EF || DELTA_FG || DELTA_GH || DELTA_HA));
    wire acyl_old                = (!dma_running_old && state_old.sprite_scanner.BESU_SCAN_DONEn_odd.state && !vid_rst_old);
    wire AVER_AxxxExxx_old       = !(acyl_old && XYSO_xBCDxFGH_old);
    wire TYTU_SFETCH_S0n_old     = !get_bit(state_old.sfetch_counter_evn, 0);
    wire TACU_SPR_SEQ_5_TRIG_old = !(state_old.sfetch_control.TYFO_SFETCH_S0p_D1_odd.state && TYTU_SFETCH_S0n_old);
    wire TUVO_PPU_OAM_RDp_old    = !(!rendering_old || get_bit(state_old.sfetch_counter_evn, 1) || get_bit(state_old.sfetch_counter_evn, 2));
    wire VAPE_OAM_CLKENn_old     = (TUVO_PPU_OAM_RDp_old && TACU_SPR_SEQ_5_TRIG_old);
    wire XUJY_OAM_CLKENp_old     = !VAPE_OAM_CLKENn_old;
    wire BYCU_OAM_CLKp_old       = !(AVER_AxxxExxx_old && XUJY_OAM_CLKENp_old && CUFE_OAM_CLKp_old);

    wire CUFE_OAM_CLKp_new       = !((cpu_addr_oam_new || dma_running_new) && (DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH));
    wire acyl_new                = (!dma_running_new && state_new.sprite_scanner.BESU_SCAN_DONEn_odd.state && !vid_rst_new);
    wire AVER_AxxxExxx_new       = !(acyl_new && XYSO_xBCDxFGH_new);
    wire TYTU_SFETCH_S0n_new     = !get_bit(state_new.sfetch_counter_evn, 0);
    wire TACU_SPR_SEQ_5_TRIG_new = !(state_new.sfetch_control.TYFO_SFETCH_S0p_D1_odd.state && TYTU_SFETCH_S0n_new);
    wire TUVO_PPU_OAM_RDp_new    = !(!rendering_new || get_bit(state_new.sfetch_counter_evn, 1) || get_bit(state_new.sfetch_counter_evn, 2));
    wire VAPE_OAM_CLKENn_new     = (TUVO_PPU_OAM_RDp_new && TACU_SPR_SEQ_5_TRIG_new);
    wire XUJY_OAM_CLKENp_new     = !VAPE_OAM_CLKENn_new;
    wire BYCU_OAM_CLKp_new       = !(AVER_AxxxExxx_new && XUJY_OAM_CLKENp_new &&  CUFE_OAM_CLKp_new);

    if (bit(BYCU_OAM_CLKp_old) && !bit(BYCU_OAM_CLKp_new)) {
      state_new.oam_temp_a = ~state_new.oam_latch_a;
      state_new.oam_temp_b = ~state_new.oam_latch_b;
    }
  }

  //----------------------------------------
  // This bit of sprite bus drive has to come after oam_temp_a is set.

  if (!vid_rst_new) {
    if (DELTA_HA || DELTA_DE) {
      state_new.sprite_index = (state_new.oam_abus >> 2) ^ 0b111111;
    }
  }

  if (!rendering_new) {
    state_new.sprite_ibus = state_new.sprite_index;
    state_new.sprite_lbus = (~ly_new + state_new.oam_temp_a) & 0b00001111;
    state_new.sprite_match_flags = 0;
  }

  else if (rendering_new && state_new.sprite_scanner.CENO_SCAN_DONEn_odd) {
    state_new.sprite_ibus = state_new.sprite_index;
    state_new.sprite_lbus = 0b00001111;
  }





  //----------------------------------------
  // I'm not entirely certain GateBoy has this right either...

  const bool nuko_wx_match_old = (uint8_t(~state_old.reg_wx) == state_old.pix_count) && state_old.win_ctrl.REJO_WY_MATCH_LATCHp;

  if (vid_rst_new) {
    state_new.win_ctrl.PYCO_WIN_MATCHp_evn.state = 0;
    state_new.win_ctrl.NUNU_WIN_MATCHp_odd.state = 0;
    state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd.state = 0;
    state_new.win_ctrl.NOPA_WIN_MODE_Bp_evn.state = 0;
    state_new.win_ctrl.RYDY_WIN_HITp_odd = 0;
    state_new.win_ctrl.PUKU_WIN_HITn_odd = 1;
  }
  else {

    if (DELTA_EVEN) {
      //wire pause_pipe_old = (state_old.win_ctrl.RYDY_WIN_HITp || !state_old.tfetch_control.POKY_PRELOAD_LATCHp || state_old.FEPO_STORE_MATCHp || hblank_old);
      if (!pause_pipe_old) state_new.win_ctrl.PYCO_WIN_MATCHp_evn.state = nuko_wx_match_old;
    }

    if (DELTA_ODD) {
      state_new.win_ctrl.NUNU_WIN_MATCHp_odd.state = state_old.win_ctrl.PYCO_WIN_MATCHp_evn.state;
      if (state_new.win_ctrl.NUNU_WIN_MATCHp_odd.state) state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd.state = 1;
    }

    if (line_rst_new_odd)                                 state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd.state = 0;
    if (!win_en_new)                                  state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd.state = 0;

    if (DELTA_EVEN) {
      state_new.win_ctrl.NOPA_WIN_MODE_Bp_evn.state = state_old.win_ctrl.PYNU_WIN_MODE_Ap_odd;
      if (bit(state_new.tfetch_control.PORY_FETCH_DONEp_odd.state)) {
        state_new.win_ctrl.RYDY_WIN_HITp_odd = 0;
        state_new.win_ctrl.PUKU_WIN_HITn_odd = 1;
      }
    }

    if (DELTA_ODD) {
      if (state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd && !state_new.win_ctrl.NOPA_WIN_MODE_Bp_evn) {
        state_new.tfetch_control.PORY_FETCH_DONEp_odd.state = 0;
        state_new.win_ctrl.RYDY_WIN_HITp_odd = 1;
        state_new.win_ctrl.PUKU_WIN_HITn_odd = 0;
      }
    }
  }

  const bool win_mode_trig_new_odd = state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd && !state_new.win_ctrl.NOPA_WIN_MODE_Bp_evn;
  if (win_mode_trig_new_odd) state_new.tfetch_control.NYKA_FETCH_DONEp_evn.state = 0;














  //==========

  // FIXME note this still has old fepo and old hblank...?
  wire pause_pipe_new = (state_new.win_ctrl.RYDY_WIN_HITp_odd || !state_new.tfetch_control.POKY_PRELOAD_LATCHp_evn || state_old.FEPO_STORE_MATCHp_odd || hblank_old);

  if (DELTA_EVEN) {
    if (!pause_pipe_new) {
      state_new.fine_scroll.PUXA_SCX_FINE_MATCH_evn.state = state_old.fine_scroll.ROXY_FINE_SCROLL_DONEn && (((state_old.reg_scx & 0b111) ^ 0b111) == state_old.fine_count);
    }
  }

  if (DELTA_ODD) {
    state_new.fine_scroll.NYZE_SCX_FINE_MATCH_odd = state_new.fine_scroll.PUXA_SCX_FINE_MATCH_evn;
  }

  if (!rendering_new) {
    state_new.fine_scroll.ROXY_FINE_SCROLL_DONEn.state = 1;
    state_new.fine_scroll.NYZE_SCX_FINE_MATCH_odd.state = 0;
    state_new.fine_scroll.PUXA_SCX_FINE_MATCH_evn.state = 0;
  }

  if (state_new.fine_scroll.PUXA_SCX_FINE_MATCH_evn && !state_new.fine_scroll.NYZE_SCX_FINE_MATCH_odd) {
    state_new.fine_scroll.ROXY_FINE_SCROLL_DONEn.state = 0;
  }



  //----------------------------------------
  // Window stuff

  if (DELTA_EVEN) {
    state_new.win_ctrl.SOVY_WIN_HITp_evn.state = state_old.win_ctrl.RYDY_WIN_HITp_odd;
    state_new.win_ctrl.RENE_WIN_FETCHn_B_evn   = state_new.win_ctrl.RYFA_WIN_FETCHn_A_evn;
  }

  if (DELTA_ODD) {
    if (!pause_pipe_old && !state_old.fine_scroll.ROXY_FINE_SCROLL_DONEn) {
      state_new.win_ctrl.RYFA_WIN_FETCHn_A_evn.state = !nuko_wx_match_old && state_new.fine_count == 7;
    }
  }


  if (DELTA_BC) {
    if (win_en_new) {
      state_new.win_ctrl.SARY_WY_MATCHp_odd.state = ly_new == uint8_t(~state_new.reg_wy);
      if (state_new.win_ctrl.SARY_WY_MATCHp_odd) {
        state_new.win_ctrl.REJO_WY_MATCH_LATCHp.state = 1;
      }
    }
    else {
      state_new.win_ctrl.SARY_WY_MATCHp_odd.state = 0;
    }
  }

  if (vblank_new) state_new.win_ctrl.REJO_WY_MATCH_LATCHp.state = 0;

  if (vid_rst_new) {
    state_new.win_ctrl.SOVY_WIN_HITp_evn.state = 0;
    state_new.win_ctrl.SARY_WY_MATCHp_odd.state = 0;
    state_new.win_ctrl.REJO_WY_MATCH_LATCHp.state = 0;
  }

  if (!rendering_new) {
    state_new.win_ctrl.RENE_WIN_FETCHn_B_evn.state = 0;
    state_new.win_ctrl.RYFA_WIN_FETCHn_A_evn.state = 0;
  }



  const bool win_fetch_trig_new = state_new.win_ctrl.RYFA_WIN_FETCHn_A_evn && !state_new.win_ctrl.RENE_WIN_FETCHn_B_evn;
  const bool win_hit_trig_new   = state_new.win_ctrl.SOVY_WIN_HITp_evn && !state_new.win_ctrl.RYDY_WIN_HITp_odd;

  //----------------------------------------

  const wire fetch_rst_new = scan_done_trig_new || win_mode_trig_new_odd || win_fetch_trig_new || win_hit_trig_new || tile_fetch_trig_new;

  if (fetch_rst_new) {
    state_new.tfetch_counter = 0;
    state_new.tfetch_control.LOVY_FETCH_DONEp.state = 0;
    state_new.tfetch_control.LONY_FETCHINGp.state = 1;
  }



  //----------------------------------------
  // Pixel counter

  if (vid_rst_new || line_rst_new_odd) {
    state_new.pix_count = 0;
  }
  else if (DELTA_HA || DELTA_BC || DELTA_DE || DELTA_FG) {
    if (!pause_pipe_old && !state_old.fine_scroll.ROXY_FINE_SCROLL_DONEn) {
      state_new.pix_count = state_new.pix_count + 1;
    }
  }

  const auto pix_count_new = state_new.pix_count;

  //----------------------------------------

  if (rendering_new && !state_new.sprite_scanner.CENO_SCAN_DONEn_odd) {
    auto& s = state_new;
    auto& sf = s.sprite_match_flags;
    auto& si = s.sprite_ibus;
    auto& sl = s.sprite_lbus;

    sf = 0;
    si = 0x3F;
    sl = 0x0F;

    if (spr_en_new) {
      s.FEPO_STORE_MATCHp_odd = 0;
      if      (pix_count_new == s.store_x0) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x001;  si = s.store_i0 ^ 0x3F; sl = s.store_l0 ^ 0x0F; }
      else if (pix_count_new == s.store_x1) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x002;  si = s.store_i1 ^ 0x3F; sl = s.store_l1 ^ 0x0F; }
      else if (pix_count_new == s.store_x2) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x004;  si = s.store_i2 ^ 0x3F; sl = s.store_l2 ^ 0x0F; }
      else if (pix_count_new == s.store_x3) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x008;  si = s.store_i3 ^ 0x3F; sl = s.store_l3 ^ 0x0F; }
      else if (pix_count_new == s.store_x4) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x010;  si = s.store_i4 ^ 0x3F; sl = s.store_l4 ^ 0x0F; }
      else if (pix_count_new == s.store_x5) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x020;  si = s.store_i5 ^ 0x3F; sl = s.store_l5 ^ 0x0F; }
      else if (pix_count_new == s.store_x6) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x040;  si = s.store_i6 ^ 0x3F; sl = s.store_l6 ^ 0x0F; }
      else if (pix_count_new == s.store_x7) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x080;  si = s.store_i7 ^ 0x3F; sl = s.store_l7 ^ 0x0F; }
      else if (pix_count_new == s.store_x8) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x100;  si = s.store_i8 ^ 0x3F; sl = s.store_l8 ^ 0x0F; }
      else if (pix_count_new == s.store_x9) { s.FEPO_STORE_MATCHp_odd = 1; sf = 0x200;  si = s.store_i9 ^ 0x3F; sl = s.store_l9 ^ 0x0F; }
    }
  }

  if (rendering_new && !state_new.FEPO_STORE_MATCHp_odd) {
    const auto pack_ydiff = ~ly_new + state_new.oam_temp_a;
    state_new.sprite_lbus = pack_ydiff & 0b00001111;
  }

  //----------------------------------------

  if (!rendering_new) {
    state_new.lcd.PAHO_X8_SYNC.state = 0;
  }

  if (DELTA_AB || DELTA_CD || DELTA_EF || DELTA_GH) {
    if (rendering_new && !pause_pipe_old) {
      state_new.lcd.PAHO_X8_SYNC.state = get_bit(state_old.pix_count, 3);
    }
  }


  //----------------------------------------
  // Vram to tile temp

  if (rendering_old /* must be old */) {
    // These ffs are weird because they latches on phase change _or_ if rendering stops in the middle of a fetch
    // Good example of gate-level behavior that doesn't matter
    const uint8_t bfetch_phase_new = pack(
      !(state_new.tfetch_control.LYZU_BFETCH_S0p_D1.state ^ get_bit(state_new.tfetch_counter, 0)),
      get_bit(state_new.tfetch_counter, 0),
      get_bit(state_new.tfetch_counter, 1),
      get_bit(state_new.tfetch_counter, 2)
    );

    if ((bfetch_phase_old == 6) && (bfetch_phase_new == 7 || !rendering_new)) {
      state_new.tile_temp_a = ~state_new.vram_dbus;
    }

    if ((bfetch_phase_old == 2) && (bfetch_phase_new == 3 || !rendering_new)) {
      state_new.tile_temp_b = state_new.vram_dbus;
    }

    if ((bfetch_phase_old == 10) && (bfetch_phase_new == 11 || !rendering_new)) {
      state_new.tile_temp_b = state_new.vram_dbus;
    }
  }

  //----------------------------------------
  // Fine match counter

  if (DELTA_HA || DELTA_BC || DELTA_DE || DELTA_FG) {
    if (!pause_pipe_old) {
      if (state_new.fine_count < 7) state_new.fine_count++;
    }
  }

  if (!rendering_new || vid_rst_new || line_rst_new_odd || win_fetch_trig_new || win_hit_trig_new || tile_fetch_trig_new) {
    state_new.fine_count = 0;
  }

  //----------------------------------------
  // Win map x counter

  if ((win_fetch_trig_new || win_hit_trig_new || tile_fetch_trig_new) && state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd) {
    state_new.win_x.map++;
  }

  if (vid_rst_new || line_rst_new_odd || !win_en_new) {
    state_new.win_x.map = 0;
  }

  //----------------------------------------
  // Win map y counter - increments when we _leave_ window mode.

  if (state_old.win_ctrl.PYNU_WIN_MODE_Ap_odd && !state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd) {
    uint8_t win_y_old = state_old.win_y.tile | (state_old.win_y.map << 3);
    uint8_t win_y_new = win_y_old + 1;

    state_new.win_y.tile = win_y_new & 0b111;
    state_new.win_y.map  = win_y_new >> 3;
  }

  if (vid_rst_new || vblank_new) {
    state_new.win_y.tile = 0;
    state_new.win_y.map = 0;
  }

  //----------------------------------------
  // Pixel pipes

  if (DELTA_HA || DELTA_BC || DELTA_DE || DELTA_FG) {
    if (!pause_pipe_old && !state_old.fine_scroll.ROXY_FINE_SCROLL_DONEn) {
      state_new.spr_pipe_a = (state_new.spr_pipe_a << 1) | 0;
      state_new.spr_pipe_b = (state_new.spr_pipe_b << 1) | 0;
      state_new.bgw_pipe_a = (state_new.bgw_pipe_a << 1) | 0;
      state_new.bgw_pipe_b = (state_new.bgw_pipe_b << 1) | 0;
      state_new.mask_pipe  = (state_new.mask_pipe  << 1) | 1;
      state_new.pal_pipe   = (state_new.pal_pipe   << 1) | 0;
    }
  }

  uint8_t sprite_pix = state_old.vram_dbus;
  if (rendering_new) {

    if (get_bit(state_old.oam_temp_b, 5) && sfetching_old) {
      sprite_pix = bit_reverse(state_old.vram_dbus);
    }

    if ((sfetch_phase_old == 5) && (sfetch_phase_new == 6)) {
      state_new.sprite_pix_a = ~sprite_pix;
    }

    if ((sfetch_phase_old == 9) && (sfetch_phase_new == 10)) {
      state_new.sprite_pix_b = ~sprite_pix;
    }
  }

  if (wuty_sfetch_done_new) {
    uint8_t sprite_mask = state_new.spr_pipe_b | state_new.spr_pipe_a;
    state_new.spr_pipe_a = (state_new.spr_pipe_a & sprite_mask) | (~state_new.sprite_pix_a & ~sprite_mask);
    state_new.spr_pipe_b = (state_new.spr_pipe_b & sprite_mask) | (~state_new.sprite_pix_b & ~sprite_mask);
    state_new.mask_pipe  = get_bit(state_new.oam_temp_b, 7) ? state_new.mask_pipe | ~sprite_mask : state_new.mask_pipe & sprite_mask;
    state_new.pal_pipe   = get_bit(state_new.oam_temp_b, 4) ? state_new.pal_pipe  | ~sprite_mask : state_new.pal_pipe  & sprite_mask;
  }

  if (bit(fetch_rst_new)) {
    state_new.bgw_pipe_a = ~state_new.tile_temp_a;
    state_new.bgw_pipe_b =  state_new.tile_temp_b;
  }

  uint8_t bgw_pix_a = get_bit(state_new.bgw_pipe_a, 7) && bgw_en_new;
  uint8_t bgw_pix_b = get_bit(state_new.bgw_pipe_b, 7) && bgw_en_new;
  uint8_t spr_pix_a = get_bit(state_new.spr_pipe_a, 7) && spr_en_new;
  uint8_t spr_pix_b = get_bit(state_new.spr_pipe_b, 7) && spr_en_new;
  uint8_t mask_pix  = get_bit(state_new.mask_pipe,  7);
  uint8_t pal_pix   = get_bit(state_new.pal_pipe,   7);

  uint8_t spr_col = pack(spr_pix_a, spr_pix_b);
  uint8_t bgw_col = pack(bgw_pix_a, bgw_pix_b);
    
  uint8_t pal = 0;
  uint8_t col = 0;

  if (spr_col == 0 || (mask_pix & (bgw_col != 0))) {
    pal = state_new.reg_bgp ^ 0xFF;
    col = bgw_col;
  }
  else if (pal_pix) {
    pal = state_new.reg_obp1 ^ 0xFF;
    col = spr_col;
  }
  else {
    pal = state_new.reg_obp0 ^ 0xFF;
    col = spr_col;
  }

  uint8_t REMY_LD0n = (pal >> (col * 2 + 0)) & 1;
  uint8_t RAVO_LD1n = (pal >> (col * 2 + 1)) & 1;

  //----------------------------------------
  // LCD pins

  // the "avap_scan_done_new && state_new.lcd.PAHO_X_8_SYNC" latch branch is never hit, would probably cause
  // latch oscillation or something
  if (vid_rst_new) {
    state_new.lcd.POME_X8_LATCH = 1;
  }
  else if (scan_done_trig_new) {
    state_new.lcd.POME_X8_LATCH = 0;
  }
  else if (state_new.lcd.PAHO_X8_SYNC) {
    state_new.lcd.POME_X8_LATCH = 1;
  }

  //----------------------------------------
  // Audio

  //tock_spu_logic();

  //----------------------------------------
  // Memory buses

  uint8_t pins_ctrl_csn_new = 1;
  uint8_t pins_ctrl_rdn_new = 1;
  uint8_t pins_ctrl_wrn_new = 1;

  uint8_t pins_abus_lo = 0xFF;
  uint8_t pins_abus_hi = 0xFF;

  if (ext_addr_new && !cpu_addr_vram_new) {
    state_new.ext_addr_latch = cpu_addr_new & 0x7FFF;
  }

  if (dma_running_new && !dma_addr_vram_new) {
    pins_ctrl_csn_new = !!(dma_addr_new & 0x8000);
    pins_abus_lo = (~dma_addr_new >> 0) & 0xFF;
    pins_abus_hi = (~dma_addr_new >> 8) & 0xFF;
  }
  else {
    pins_ctrl_csn_new = gen_clk_new(phase_total_old, 0b00111111) && ext_addr_new && cpu_addr_ram_new;
    pins_abus_lo = ~((state_new.ext_addr_latch >> 0) & 0xFF);
    pins_abus_hi = ~((state_new.ext_addr_latch >> 8) & 0x7F);
  }

  pins_ctrl_rdn_new = 1;
  pins_ctrl_wrn_new = 0;
  if (!(dma_running_new && dma_addr_vram_new) && ext_addr_new && cpu_wr) {
    pins_ctrl_rdn_new = cpu_addr_vram_new;
    pins_ctrl_wrn_new = gen_clk_new(phase_total_old, 0b00001110) && !cpu_addr_vram_new;
  }

  pins_abus_hi &= 0b01111111;
  if (dma_running_new && !dma_addr_vram_new) {
    pins_abus_hi |= (!!((~dma_addr_new >> 8) & 0b10000000)) << 7;
  }
  else if (!state_new.cpu_signals.TEPU_BOOT_BITn && cpu_addr_new <= 0x00FF) {
  }
  else {
    uint8_t bit = gen_clk_new(phase_total_old, 0b00111111) && ext_addr_new && !get_bit(cpu_addr_new, 15);
    pins_abus_hi |= bit << 7;
  }

  uint8_t pins_dbus = 0;
  
  if (cpu_wr && !req_addr_hi && !cpu_addr_bootrom_new && !cpu_addr_vram_new) {
    pins_dbus = ~state_new.cpu_dbus;
  }

  //----------------------------------------
  // Ext read

  if (pins_ctrl_rdn_new) {
    const uint16_t ext_addr_lo = pins_abus_lo ^ 0xFF;
    const uint16_t ext_addr_hi = pins_abus_hi ^ 0xFF;
    const uint16_t ext_addr = (ext_addr_lo << 0) | (ext_addr_hi << 8);

    const auto rom_addr_mask = cart_rom_addr_mask(cart_blob);
    const auto ram_addr_mask = cart_ram_addr_mask(cart_blob);

    const int region = ext_addr >> 13;
    uint8_t data_in = 0x00;

    bool ext_read_en = false;

    if (cart_has_mbc1(cart_blob)) {

      const bool mbc1_ram_en = state_new.ext_mbc.MBC1_RAM_EN;
      const bool mbc1_mode = state_new.ext_mbc.MBC1_MODE;

      const uint32_t mbc1_rom0_bank = mbc1_mode ? bit_pack(&state_new.ext_mbc.MBC1_BANK5, 2) : 0;
      const uint32_t mbc1_rom0_addr = ((ext_addr & 0x3FFF) | (mbc1_rom0_bank << 19)) & rom_addr_mask;

      uint32_t mbc1_rom1_bank = bit_pack(&state_new.ext_mbc.MBC1_BANK0, 7);
      if ((mbc1_rom1_bank & 0x1F) == 0) mbc1_rom1_bank |= 1;
      const uint32_t mbc1_rom1_addr = ((ext_addr & 0x3FFF) | (mbc1_rom1_bank << 14)) & rom_addr_mask;

      const uint32_t mbc1_ram_bank = mbc1_mode ? bit_pack(&state_new.ext_mbc.MBC1_BANK5, 2) : 0;
      const uint32_t mbc1_ram_addr = ((ext_addr & 0x1FFF) | (mbc1_ram_bank << 13)) & ram_addr_mask;

      switch (region) {
      case 0: ext_read_en = true; data_in = cart_blob[mbc1_rom0_addr]; break;
      case 1: ext_read_en = true; data_in = cart_blob[mbc1_rom0_addr]; break;
      case 2: ext_read_en = true; data_in = cart_blob[mbc1_rom1_addr]; break;
      case 3: ext_read_en = true; data_in = cart_blob[mbc1_rom1_addr]; break;
      case 4: data_in = 0x00; break;
      case 5: ext_read_en = true; data_in = mbc1_ram_en ? mem.cart_ram[mbc1_ram_addr] : 0xFF; break;
      case 6: ext_read_en = true; data_in = mem.int_ram[ext_addr & 0x1FFF]; break;
      case 7: ext_read_en = true; data_in = mem.int_ram[ext_addr & 0x1FFF]; break;
      }
    }
    else {
      switch (region) {
      case 0: ext_read_en = true; data_in = cart_blob[ext_addr & rom_addr_mask]; break;
      case 1: ext_read_en = true; data_in = cart_blob[ext_addr & rom_addr_mask]; break;
      case 2: ext_read_en = true; data_in = cart_blob[ext_addr & rom_addr_mask]; break;
      case 3: ext_read_en = true; data_in = cart_blob[ext_addr & rom_addr_mask]; break;
      case 4: data_in = 0x00; break;
      case 5: data_in = 0x00; break;
      case 6: ext_read_en = true; data_in = mem.int_ram[ext_addr & 0x1FFF]; break;
      case 7: ext_read_en = true; data_in = mem.int_ram[ext_addr & 0x1FFF]; break;
      }
    }

    if (ext_read_en) {
      pins_dbus = ~data_in;
    }
  }

  //----------------------------------------
  // Ext write

  {
    const uint16_t ext_addr_lo = pins_abus_lo ^ 0xFF;
    const uint16_t ext_addr_hi = pins_abus_hi ^ 0xFF;
    const uint16_t ext_addr = (ext_addr_lo << 0) | (ext_addr_hi << 8);

    const auto region = ext_addr >> 13;
    const uint8_t data_out = ~pins_dbus;
    const bool mbc1_ram_en = state_new.ext_mbc.MBC1_RAM_EN;
    const bool mbc1_mode = state_new.ext_mbc.MBC1_MODE;

    const auto mbc1_ram_bank = mbc1_mode ? bit_pack(&state_new.ext_mbc.MBC1_BANK5, 2) : 0;
    const auto mbc1_ram_addr = ((ext_addr & 0x1FFF) | (mbc1_ram_bank << 13)) & cart_ram_addr_mask(cart_blob);

    if (pins_ctrl_wrn_new && cart_has_mbc1(cart_blob)) {
      switch (region) {
      case 0: state_new.ext_mbc.MBC1_RAM_EN = (data_out & 0x0F) == 0x0A; break;
      case 1: bit_unpack(&state_new.ext_mbc.MBC1_BANK0, 5, data_out); break;
      case 2: bit_unpack(&state_new.ext_mbc.MBC1_BANK5, 2, data_out); break;
      case 3: state_new.ext_mbc.MBC1_MODE = (data_out & 1); break;
      case 4: break;
      case 5: if (cart_has_ram(cart_blob) && mbc1_ram_en) mem.cart_ram[mbc1_ram_addr & cart_ram_addr_mask(cart_blob)] = (uint8_t)data_out; break;
      case 6: mem.int_ram[ext_addr & 0x1FFF] = (uint8_t)data_out; break;
      case 7: mem.int_ram[ext_addr & 0x1FFF] = (uint8_t)data_out; break;
      }
    }

    if (pins_ctrl_wrn_new && !cart_has_mbc1(cart_blob)) {
      switch (region) {
      case 0: break;
      case 1: break;
      case 2: break;
      case 3: break;
      case 4: break;
      case 5: if (cart_has_ram(cart_blob)) mem.cart_ram[ext_addr & cart_ram_addr_mask(cart_blob)] = (uint8_t)data_out; break;
      case 6: mem.int_ram[ext_addr & 0x1FFF] = (uint8_t)data_out;break;
      case 7: mem.int_ram[ext_addr & 0x1FFF] = (uint8_t)data_out;break;
      }
    }
  }

  //----------------------------------------
  // VRAM bus

  if (cpu_rd &&
     !(req_addr_vram && DELTA_HA) &&
     !req_addr_hi &&
     !cpu_addr_bootrom_new &&
     !cpu_addr_vram_new &&
     (DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH))
  {
    state_new.cpu_dbus = ~state_new.ext_data_latch;
  }
  else {
    state_new.ext_data_latch = pins_dbus;
  }

  state_new.vram_abus = VRAM_ADDR_MASK;
  state_new.vram_dbus = 0xFF;

  // CPU vram read address

  if (!(dma_running_new && dma_addr_vram_new) && !rendering_new) {
    state_new.vram_abus = (~cpu_addr_new) & VRAM_ADDR_MASK;
  }

  // DMA vram read address

  if ((dma_running_new && dma_addr_vram_new)) {
    state_new.vram_abus = ~dma_addr_new & VRAM_ADDR_MASK;
  }

  //--------------------------------------------

  if (state_new.tfetch_control.LONY_FETCHINGp) {
    const auto px  = state_new.pix_count;
    const auto scx = ~state_new.reg_scx;
    const auto scy = ~state_new.reg_scy;

    const auto sum_x = px + scx;
    const auto sum_y = ly_new + scy;

    //--------------------------------------------
    // BG map read address


    if (get_bit(state_new.tfetch_counter, 1) || get_bit(state_new.tfetch_counter, 2)) {

      const auto hilo = get_bit(state_new.tfetch_counter, 2);
      const auto tile_y = (state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd ? state_new.win_y.tile : (sum_y & 0b111));
      const auto map_y = state_new.tile_temp_b;
      const auto map = !get_bit(state_new.tile_temp_b, 7) && bgw_tile_new;

      uint32_t addr  = 0;
      bit_cat(addr,  0,  0, hilo);
      bit_cat(addr,  1,  3, tile_y);
      bit_cat(addr,  4, 11, map_y);
      bit_cat(addr, 12, 12, map);
      
      state_new.vram_abus = uint16_t(addr ^ VRAM_ADDR_MASK);
    }
    else {
      if (state_new.win_ctrl.PYNU_WIN_MODE_Ap_odd) {
        uint32_t addr = 0;
        bit_cat(addr,  0,  4, ~state_new.win_x.map);
        bit_cat(addr,  5,  9, ~state_new.win_y.map);
        bit_cat(addr, 10, 10, win_map_new);
        state_new.vram_abus = uint16_t(addr);
      }
      else {
        uint32_t addr = 0;
        bit_cat(addr,  0,  4, (px + scx) >> 3);
        bit_cat(addr,  5,  9, (ly_new + scy) >> 3);
        bit_cat(addr, 10, 10, !bg_map_new);
        bit_cat(addr, 11, 11, 1);
        bit_cat(addr, 12, 12, 1);

        state_new.vram_abus = uint16_t(addr ^ VRAM_ADDR_MASK);
      }

    }
  }

  //--------------------------------------------
  // Sprite read address

  const bool sfetching_new = (!vid_rst_new && rendering_new && ((get_bit(state_new.sfetch_counter_evn, 1) || state_new.sfetch_control.VONU_SFETCH_S1p_D4_evn)));

  if (sfetching_new) {
    const bool hilo = state_new.sfetch_control.VONU_SFETCH_S1p_D4_evn;
    const auto line = state_new.sprite_lbus ^ (get_bit(state_new.oam_temp_b, 6) ? 0b0000 : 0b1111);
    const auto tile = state_new.oam_temp_a;

    uint32_t addr = 0;
    bit_cat(addr,  0,  0, hilo);
    if (spr_size_new) {
      bit_cat(addr,  1,  3, line);
      bit_cat(addr,  4, 11, tile);
    }
    else {
      bit_cat(addr,  1,  4, line);
      bit_cat(addr,  5, 11, tile >> 1);
    }
    state_new.vram_abus = uint16_t(addr ^ VRAM_ADDR_MASK);
  }

  //--------------------------------------------
  // Vram address pin driver

  state_new.vram_dbus = 0xFF;

  uint8_t pins_vram_dbus = 0xFF;

  uint8_t pins_vram_ctrl_csn_new = 1;
  uint8_t pins_vram_ctrl_oen_new = 1;
  uint8_t pins_vram_ctrl_wrn_new = 1;

  if (dma_running_new && dma_addr_vram_new) {
    pins_vram_ctrl_csn_new = 1;
    pins_vram_ctrl_oen_new = 1;
  }
  else if (rendering_new) {
    pins_vram_ctrl_csn_new = state_new.tfetch_control.LONY_FETCHINGp || sfetching_new;
    pins_vram_ctrl_oen_new = state_new.tfetch_control.LONY_FETCHINGp || (sfetching_new && (!state_new.sfetch_control.TYFO_SFETCH_S0p_D1_odd || get_bit(state_new.sfetch_counter_evn, 0)));
  }
  else if (ext_addr_new) {
    pins_vram_ctrl_csn_new = (cpu_addr_vram_new && gen_clk_new(phase_total_old, 0b00111111) && 1);
    pins_vram_ctrl_oen_new = !cpu_addr_vram_new || !cpu.bus_req_new.write || DELTA_HA;
  }
  else {
    pins_vram_ctrl_csn_new = 0;
    pins_vram_ctrl_oen_new = !cpu_addr_vram_new || !cpu.bus_req_new.write || DELTA_HA;;
  }


  if (rendering_new) {
    pins_vram_ctrl_wrn_new = 0;
  }
  else if (ext_addr_new) {
    pins_vram_ctrl_wrn_new = cpu_addr_vram_new && gen_clk_new(phase_total_old, 0b00001110) && cpu_wr && 1;
  }
  else {
    pins_vram_ctrl_wrn_new = cpu_addr_vram_new && gen_clk_new(phase_total_old, 0b00001110) && cpu_wr && 0;
  }


  if (rendering_new) {
    if (pins_vram_ctrl_oen_new) {
      state_new.vram_dbus = mem.vid_ram[state_new.vram_abus ^ 0x1FFF];
    }
    pins_vram_dbus = ~state_new.vram_dbus;
  }
  else if (ext_addr_new) {
    if ((gen_clk_new(phase_total_old, 0b00111111) && 1) && cpu_addr_vram_new && cpu_wr) {
      state_new.vram_dbus = state_new.cpu_dbus;
    }

    uint8_t vdata = 0xFF;

    if (pins_vram_ctrl_oen_new) vdata = mem.vid_ram[state_new.vram_abus ^ 0x1FFF];

    pins_vram_dbus = pins_vram_ctrl_oen_new ? ~vdata : 0;

    if (cpu_addr_vram_new && gen_clk_new(phase_total_old, 0b00111111) && cpu_wr) {
      pins_vram_dbus = ~state_new.vram_dbus;
    }
    else {
      state_new.vram_dbus = ~pins_vram_dbus;
    }

    if (pins_vram_ctrl_wrn_new) mem.vid_ram[state_new.vram_abus ^ 0x1FFF] = ~pins_vram_dbus;

    if (cpu_addr_vram_new && gen_clk_new(phase_total_old, 0b00111111) && (cpu.bus_req_new.read && !DELTA_HA) && ((DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH) && cpu.bus_req_new.read)) {
      state_new.cpu_dbus = state_new.vram_dbus;
    }
  }
  else {
    uint8_t vdata = 0xFF;

    if (pins_vram_ctrl_oen_new) vdata = mem.vid_ram[state_new.vram_abus ^ 0x1FFF];

    pins_vram_dbus = pins_vram_ctrl_oen_new ? ~vdata : 0;

    state_new.vram_dbus = ~pins_vram_dbus;

    if (pins_vram_ctrl_wrn_new) mem.vid_ram[state_new.vram_abus ^ 0x1FFF] = ~pins_vram_dbus;
  }

  //----------------------------------------
  // CPU read registers
  
  // This has to be before OAM, because the OAM dbus mirrors CPU dbus if it's not doing other things

  // But ZRAM can't be before OAM because ZRAM _doesn't_ show up on the oam bus? That seems wrong...

  if ((cpu.bus_req_new.read && !DELTA_HA)) {
    if (cpu_addr_new == 0xFF00) {
      set_bit(state_new.cpu_dbus, 0, !get_bit(state_new.joy_latch, 0));
      set_bit(state_new.cpu_dbus, 1, !get_bit(state_new.joy_latch, 1));
      set_bit(state_new.cpu_dbus, 2, !get_bit(state_new.joy_latch, 2));
      set_bit(state_new.cpu_dbus, 3, !get_bit(state_new.joy_latch, 3));
      set_bit(state_new.cpu_dbus, 4,  get_bit(state_new.reg_joy, 0));
      set_bit(state_new.cpu_dbus, 5,  get_bit(state_new.reg_joy, 1));
    }
    if (cpu_addr_new == 0xFF04) state_new.cpu_dbus =  (uint8_t)(state_new.reg_div >> 6);
    if (cpu_addr_new == 0xFF05) state_new.cpu_dbus =  state_new.reg_tima;
    if (cpu_addr_new == 0xFF06) state_new.cpu_dbus =  state_new.reg_tma;
    if (cpu_addr_new == 0xFF07) state_new.cpu_dbus =  (state_new.reg_tac & 0b111) | 0b11111000;
    if (cpu_addr_new == 0xFF40) state_new.cpu_dbus = ~state_new.reg_lcdc;
    if (cpu_addr_new == 0xFF42) state_new.cpu_dbus = ~state_new.reg_scy;
    if (cpu_addr_new == 0xFF43) state_new.cpu_dbus = ~state_new.reg_scx;
    if (cpu_addr_new == 0xFF44) state_new.cpu_dbus =  state_new.reg_ly;
    if (cpu_addr_new == 0xFF45) state_new.cpu_dbus = ~state_old.reg_lyc;
    if (cpu_addr_new == 0xFF47) state_new.cpu_dbus = ~state_new.reg_bgp;
    if (cpu_addr_new == 0xFF48) state_new.cpu_dbus = ~state_new.reg_obp0;
    if (cpu_addr_new == 0xFF49) state_new.cpu_dbus = ~state_new.reg_obp1;
    if (cpu_addr_new == 0xFF4A) state_new.cpu_dbus = ~state_new.reg_wy;
    if (cpu_addr_new == 0xFF4B) state_new.cpu_dbus = ~state_new.reg_wx;
  }

  //----------------------------------------
  // oam

  // this is weird, why is it always 0 when not in reset?
  state_new.oam_ctrl.MAKA_LATCH_EXTp.state = 0;

  // OAM bus has to be pulled up or we fail regression
  state_new.oam_abus = 0xFF;
  state_new.oam_dbus_a = 0xFF;
  state_new.oam_dbus_b = 0xFF;

  if (gen_clk_new(phase_total_old, 0b11110000)) state_new.oam_ctrl.WUJE_CPU_OAM_WRn.state = 1;
  if (cpu_addr_oam_new && cpu_wr && gen_clk_new(phase_total_old, 0b00001110)) state_new.oam_ctrl.WUJE_CPU_OAM_WRn.state = 0;

  //----------
  // OAM bus and control signals.
  // The inclusion of cpu_addr_oam_new in the SCANNING and RENDERING branches is probably a hardware bug.

  if (dma_running_new) {
    state_new.oam_abus = (uint8_t)~state_new.dma_lo;

    if (DELTA_HA || DELTA_AB || DELTA_BC || DELTA_CD) {
      state_new.oam_ctrl.SIG_OAM_CLKn.state  = 1;
      state_new.oam_ctrl.SIG_OAM_WRn_A.state = 1;
      state_new.oam_ctrl.SIG_OAM_WRn_B.state = 1;
      state_new.oam_ctrl.SIG_OAM_OEn.state   = 1;
    }
    else {
      state_new.oam_ctrl.SIG_OAM_CLKn.state  = 0;
      state_new.oam_ctrl.SIG_OAM_WRn_A.state = !get_bit(state_new.oam_abus, 0);
      state_new.oam_ctrl.SIG_OAM_WRn_B.state =  get_bit(state_new.oam_abus, 0);
      state_new.oam_ctrl.SIG_OAM_OEn.state   = 1;
    }

    state_new.oam_dbus_a = dma_addr_vram_new ? ~state_new.vram_dbus : pins_dbus;
    state_new.oam_dbus_b = dma_addr_vram_new ? ~state_new.vram_dbus : pins_dbus;
  }
  else if (state_new.sprite_scanner.BESU_SCAN_DONEn_odd && !vid_rst_new) {
    state_new.oam_abus = (uint8_t)~((state_new.scan_counter << 2) | 0b00);
    state_new.oam_ctrl.SIG_OAM_CLKn.state  = 0;
    state_new.oam_ctrl.SIG_OAM_WRn_A.state = 1;
    state_new.oam_ctrl.SIG_OAM_WRn_B.state = 1;
    state_new.oam_ctrl.SIG_OAM_OEn.state   = 0;

    if (DELTA_GH || DELTA_HA || DELTA_CD || DELTA_DE) {
      state_new.oam_ctrl.SIG_OAM_CLKn.state  = (DELTA_DE && !cpu_addr_oam_new) || DELTA_HA;
      state_new.oam_ctrl.SIG_OAM_OEn.state   = !(cpu_addr_oam_new && (cpu.bus_req_new.read && !DELTA_HA) && !((DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH) && cpu.bus_req_new.read));
    }

    if (!state_new.oam_ctrl.SIG_OAM_OEn) {
      state_new.oam_dbus_a = ~mem.oam_ram[(state_new.oam_abus ^ 0xFF) & ~1];
      state_new.oam_dbus_b = ~mem.oam_ram[(state_new.oam_abus ^ 0xFF) |  1];
    }
  }
  else if (rendering_new) {
    const auto sfetch_oam_clk_new = (get_bit(state_new.sfetch_counter_evn, 1) || get_bit(state_new.sfetch_counter_evn, 2) || (state_new.sfetch_control.TYFO_SFETCH_S0p_D1_odd && !get_bit(state_new.sfetch_counter_evn, 0)));
    const auto sfetch_oam_oen_new = (get_bit(state_new.sfetch_counter_evn, 1) || get_bit(state_new.sfetch_counter_evn, 2) || !state_new.sfetch_control.TYFO_SFETCH_S0p_D1_odd);

    state_new.oam_abus = (uint8_t)~((state_new.sprite_ibus  << 2) | 0b11);
    state_new.oam_ctrl.SIG_OAM_CLKn.state  = sfetch_oam_clk_new && (!cpu_addr_oam_new || gen_clk_new(phase_total_old, 0b11110000));
    state_new.oam_ctrl.SIG_OAM_WRn_A.state = 1;
    state_new.oam_ctrl.SIG_OAM_WRn_B.state = 1;
    state_new.oam_ctrl.SIG_OAM_OEn.state   = sfetch_oam_oen_new && !(cpu_addr_oam_new && (cpu.bus_req_new.read && !DELTA_HA) && !((DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH) && cpu.bus_req_new.read));

    if (!sfetch_oam_oen_new) {
      state_new.oam_dbus_a = ~mem.oam_ram[(state_new.oam_abus ^ 0xFF) & ~1];
      state_new.oam_dbus_b = ~mem.oam_ram[(state_new.oam_abus ^ 0xFF) |  1];
    }
    else if (cpu_addr_oam_new && (cpu.bus_req_new.read && !DELTA_HA) && !((DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH) && cpu.bus_req_new.read)) {
      state_new.oam_dbus_a = ~mem.oam_ram[(state_new.oam_abus ^ 0xFF) & ~1];
      state_new.oam_dbus_b = ~mem.oam_ram[(state_new.oam_abus ^ 0xFF) |  1];
    }
  }
  else if (cpu_addr_oam_new) {
    state_new.oam_abus = (uint8_t)~cpu_addr_new;
    state_new.oam_ctrl.SIG_OAM_CLKn.state  = DELTA_HA || DELTA_AB || DELTA_BC || DELTA_CD;
    state_new.oam_ctrl.SIG_OAM_WRn_A.state = 1;
    state_new.oam_ctrl.SIG_OAM_WRn_B.state = 1;
    state_new.oam_ctrl.SIG_OAM_OEn.state   = !((cpu.bus_req_new.read && !DELTA_HA) && !((DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH) && cpu.bus_req_new.read));

    if (DELTA_DE || DELTA_EF || DELTA_FG) {
      state_new.oam_ctrl.SIG_OAM_WRn_A.state = !cpu_wr || !get_bit(state_new.oam_abus, 0);
      state_new.oam_ctrl.SIG_OAM_WRn_B.state = !cpu_wr ||  get_bit(state_new.oam_abus, 0);
    }

    if ((cpu.bus_req_new.read && !DELTA_HA)) {
      if (((DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH) && cpu.bus_req_new.read)) {
        state_new.cpu_dbus = get_bit(state_new.oam_abus, 0) ? ~state_new.oam_latch_a : ~state_new.oam_latch_b;
      }
      else {
        state_new.oam_dbus_a = ~mem.oam_ram[(state_new.oam_abus ^ 0xFF) & ~1];
        state_new.oam_dbus_b = ~mem.oam_ram[(state_new.oam_abus ^ 0xFF) |  1];
      }
    }

    wire XUPA_CPU_OAM_WRp  = !state_new.oam_ctrl.WUJE_CPU_OAM_WRn.state;
    wire AJUJ_OAM_BUSYn    = !(state_new.sprite_scanner.BESU_SCAN_DONEn_odd && !vid_rst_new) && !rendering_new;
    wire APAG_CBD_TO_OBDp  = (XUPA_CPU_OAM_WRp && AJUJ_OAM_BUSYn);
    if (APAG_CBD_TO_OBDp) {
      state_new.oam_dbus_a = ~state_new.cpu_dbus;
      state_new.oam_dbus_b = ~state_new.cpu_dbus;
    }
  }
  else {
    state_new.oam_ctrl.SIG_OAM_CLKn.state  = 1;
    state_new.oam_ctrl.SIG_OAM_WRn_A.state = 1;
    state_new.oam_ctrl.SIG_OAM_WRn_B.state = 1;
    state_new.oam_ctrl.SIG_OAM_OEn.state   = 1;
    state_new.oam_abus = (uint8_t)~cpu_addr_new;
    state_new.oam_dbus_a = ~state_new.cpu_dbus;
    state_new.oam_dbus_b = ~state_new.cpu_dbus;

    wire APAG_CBD_TO_OBDp  = !(state_new.sprite_scanner.BESU_SCAN_DONEn_odd && !vid_rst_new) && !rendering_new;
    if (APAG_CBD_TO_OBDp) {
      state_new.oam_dbus_a = ~state_new.cpu_dbus;
      state_new.oam_dbus_b = ~state_new.cpu_dbus;
    }
  }

  // OAM latch stores the contents of the oam dbus

  if ((!dma_running_new && state_new.sprite_scanner.BESU_SCAN_DONEn_odd && !vid_rst_new)) {
    if (DELTA_AB || DELTA_BC || DELTA_EF || DELTA_FG) {
      state_new.oam_latch_a = state_new.oam_dbus_a;
      state_new.oam_latch_b = state_new.oam_dbus_b;
    }
  }
  else if (rendering_new) {
    if (!(get_bit(state_new.sfetch_counter_evn, 1) || get_bit(state_new.sfetch_counter_evn, 2) || !state_new.sfetch_control.TYFO_SFETCH_S0p_D1_odd)) {
      state_new.oam_latch_a = state_new.oam_dbus_a;
      state_new.oam_latch_b = state_new.oam_dbus_b;
    }
  }
  else {
    if (cpu_addr_oam_new && (cpu.bus_req_new.read && !DELTA_HA) && !((DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH) && cpu.bus_req_new.read)) {
      state_new.oam_latch_a = state_new.oam_dbus_a;
      state_new.oam_latch_b = state_new.oam_dbus_b;
    }
  }

  // Actual OAM write
  if (state_old.oam_ctrl.SIG_OAM_CLKn && !state_new.oam_ctrl.SIG_OAM_CLKn) {
    uint8_t oam_addr_new = uint8_t(~state_new.oam_abus) >> 1;
    if (!state_new.oam_ctrl.SIG_OAM_WRn_A) mem.oam_ram[(oam_addr_new << 1) + 0] = ~state_new.oam_dbus_a;
    if (!state_new.oam_ctrl.SIG_OAM_WRn_B) mem.oam_ram[(oam_addr_new << 1) + 1] = ~state_new.oam_dbus_b;
  }
 
  //----------------------------------------
  // zram

  if ((cpu.bus_req_new.read && !DELTA_HA)) {
    if ((cpu_addr_new >= 0xFF80) && (cpu_addr_new <= 0xFFFE)) state_new.cpu_dbus = mem.zero_ram[cpu_addr_new & 0x007F];
  }

  //----------------------------------------
  // And finally, interrupts.

  auto pack_cpu_dbus_old = state_old.cpu_dbus;
  auto pack_cpu_dbus_new = state_new.cpu_dbus;
  auto pack_ie = state_new.reg_ie;
  auto pack_if = state_new.reg_if;
  auto pack_stat = state_new.reg_stat;

  if (!vid_rst_new && DELTA_BC) {
    state_new.int_ctrl.ROPO_LY_MATCH_SYNCp.state = ly_old == (state_old.reg_lyc ^ 0xFF);
  }

  // FIXME this seems slightly wrong...
  if (cpu_wr && gen_clk_new(phase_total_old, 0b00001110) && cpu_addr_new == 0xFF41) {
  }
  else {
    state_new.int_ctrl.RUPO_LYC_MATCHn.state = 1;
  }

  // but the "reset" arm of the latch overrides the "set" arm, so it doesn't completely break?
  if (state_new.int_ctrl.ROPO_LY_MATCH_SYNCp) {
    state_new.int_ctrl.RUPO_LYC_MATCHn.state = 0;
  }

  if (cpu_addr_new == 0xFFFF && cpu_wr && gen_clk_new(phase_total_old, 0b00000001)) {
    pack_ie = pack_cpu_dbus_old;
  }

  if (cpu_addr_new == 0xFF41 && cpu_wr && gen_clk_new(phase_total_old, 0b00000001)) {
    pack_stat = (~pack_cpu_dbus_old >> 3) & 0b00001111;
  }

  if (cpu_addr_new == 0xFF41 && (cpu.bus_req_new.read && !DELTA_HA)) {
    uint8_t stat = 0x80;

    stat |= (rendering_new || vblank_new) << 0;
    stat |= (rendering_new || (!dma_running_new && state_new.sprite_scanner.BESU_SCAN_DONEn_odd && !vid_rst_new)) << 1;
    stat |= (!state_new.int_ctrl.RUPO_LYC_MATCHn) << 2;
    stat |= (pack_stat ^ 0b1111) << 3;

    pack_cpu_dbus_new = stat;
  }

  bool int_stat_old = 0;
  if (!get_bit(state_old.reg_stat, 0) && hblank_old && !vblank_old) int_stat_old = 1;
  if (!get_bit(state_old.reg_stat, 1) && vblank_old) int_stat_old = 1;
  if (!get_bit(state_old.reg_stat, 2) && !vblank_old && state_old.lcd.RUTU_LINE_ENDp_odd) int_stat_old = 1;
  if (!get_bit(state_old.reg_stat, 3) && state_old.int_ctrl.ROPO_LY_MATCH_SYNCp) int_stat_old = 1;

  const bool int_lcd_old = vblank_old;
  const bool int_joy_old = !state_old.joy_int.APUG_JP_GLITCH3 || !state_old.joy_int.BATU_JP_GLITCH0;
  const bool int_tim_old = state_old.int_ctrl.MOBA_TIMER_OVERFLOWp;
  //const bool int_ser_old = serial.CALY_SER_CNT3;
  const bool int_ser_old = 0;

  bool hblank_new = !state_old.FEPO_STORE_MATCHp_odd && (state_new.pix_count & 167) == 167;

  bool int_stat_new = 0;
  if (!get_bit(pack_stat, 0) && hblank_new && !vblank_new) int_stat_new = 1;
  if (!get_bit(pack_stat, 1) && vblank_new) int_stat_new = 1;
  if (!get_bit(pack_stat, 2) && !vblank_new && state_new.lcd.RUTU_LINE_ENDp_odd) int_stat_new = 1;
  if (!get_bit(pack_stat, 3) && state_new.int_ctrl.ROPO_LY_MATCH_SYNCp) int_stat_new = 1;

  const wire int_lcd_new = vblank_new;
  const wire int_joy_new = !state_new.joy_int.APUG_JP_GLITCH3 || !state_new.joy_int.BATU_JP_GLITCH0;
  const wire int_tim_new = state_new.int_ctrl.MOBA_TIMER_OVERFLOWp;
  //const wire int_ser = state_new.serial.CALY_SER_CNT3;
  const wire int_ser_new = 0;

  if (!int_lcd_old  && int_lcd_new)  pack_if |= (1 << 0);
  if (!int_stat_old && int_stat_new) pack_if |= (1 << 1);
  if (!int_tim_old  && int_tim_new)  pack_if |= (1 << 2);
  if (!int_ser_old  && int_ser_new)  pack_if |= (1 << 3);
  if (!int_joy_old  && int_joy_new)  pack_if |= (1 << 4);

  // note this is an async set so it doesn't happen on the GH clock edge like other writes
  if (cpu_wr && gen_clk_new(phase_total_old, 0b00001110)) {
    if (cpu_addr_new == 0xFF0F) pack_if = pack_cpu_dbus_new;
  }

  pack_if &= ~cpu.core.int_ack;

  if ((cpu.bus_req_new.read && !DELTA_HA)) {
    if (cpu_addr_new == 0xFFFF) pack_cpu_dbus_new = pack_ie | 0b11100000;
    if (cpu_addr_new == 0xFF0F) state_new.int_latch = (uint8_t)pack_if;
    if (cpu_addr_new == 0xFF0F) pack_cpu_dbus_new = pack_if | 0b11100000;
  }


  state_new.cpu_dbus = (uint8_t)pack_cpu_dbus_new;
  state_new.cpu_int = (uint8_t)pack_if;
  state_new.reg_ie = (uint8_t)pack_ie;
  state_new.reg_if = (uint8_t)pack_if;
  state_new.reg_stat = (uint8_t)pack_stat;




  int lcd_x = lb_state.pix_count - 8;
  int lcd_y = ly_new;

  if (lcd_y >= 0 && lcd_y < 144 && lcd_x >= 0 && lcd_x < 160) {
    wire p0 = !REMY_LD0n;
    wire p1 = !RAVO_LD1n;
    auto new_pix = p0 + p1 * 2;

    mem.framebuffer[lcd_x + lcd_y * 160] = uint8_t(3 - new_pix);
  }
















































  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  //===========================================================================
  // These are all dead (unused) signals that are only needed for regression tests

  if (!config_fastmode) {

    if (DELTA_AB || DELTA_CD || DELTA_EF || DELTA_GH) state_new.VOGA_HBLANKp = hblank_old;
    if (vid_rst_new || line_rst_new_odd) state_new.VOGA_HBLANKp = 0;

    state_new.cpu_signals.SIG_IN_CPU_EXT_BUSp.state = ext_addr_new;
    state_new.cpu_signals.SIG_IN_CPU_DBUS_FREE.state = ((DELTA_DE || DELTA_EF || DELTA_FG || DELTA_GH) && cpu.bus_req_new.read);
    state_new.cpu_signals.SIG_IN_CPU_WRp.state = cpu_wr;
    state_new.cpu_signals.SIG_IN_CPU_RDp.state = cpu_rd;
    if (!vid_rst_new) {
      if (DELTA_FG) {
        state_new.lcd.SYGU_LINE_STROBE.state = (state_old.reg_lx == 0) || (state_old.reg_lx == 7) || (state_old.reg_lx == 45) || (state_old.reg_lx == 83);
      }

      if (state_old.lcd.RUTU_LINE_ENDp_odd && !state_new.lcd.RUTU_LINE_ENDp_odd) state_new.lcd.LUCA_LINE_EVENp.state = !state_new.lcd.LUCA_LINE_EVENp;
      if (!vblank_old && vblank_new) state_new.lcd.NAPO_FRAME_EVENp.state = !state_new.lcd.NAPO_FRAME_EVENp;

      if (state_old.lcd.NYPE_LINE_ENDp_odd && !state_new.lcd.NYPE_LINE_ENDp_odd) {
        state_new.lcd.MEDA_VSYNC_OUTn.state = state_new.reg_ly == 0;
      }

      if (get_bit(state_new.pix_count, 0) && get_bit(state_new.pix_count, 3)) state_new.lcd.WUSA_LCD_CLOCK_GATE.state = 1;
      if (state_new.VOGA_HBLANKp) state_new.lcd.WUSA_LCD_CLOCK_GATE.state = 0;
    }
    if (vid_rst_new) {
      state_new.lcd.SYGU_LINE_STROBE.state = 0;
      state_new.lcd.LUCA_LINE_EVENp.state = 0;
      state_new.lcd.NAPO_FRAME_EVENp.state = 0;
      state_new.lcd.MEDA_VSYNC_OUTn.state = 0;
      state_new.lcd.WUSA_LCD_CLOCK_GATE.state = 0;
    }

    bit_unpack(pins.abus_lo, pins_abus_lo);
    bit_unpack(pins.abus_hi, pins_abus_hi);

    pins.ctrl.PIN_78_WRn.state = pins_ctrl_wrn_new;
    pins.ctrl.PIN_79_RDn.state = pins_ctrl_rdn_new;
    pins.ctrl.PIN_80_CSn.state = pins_ctrl_csn_new;

    pins.vram_ctrl.PIN_43_VRAM_CSn.state = pins_vram_ctrl_csn_new;
    pins.vram_ctrl.PIN_45_VRAM_OEn.state = pins_vram_ctrl_oen_new;
    pins.vram_ctrl.PIN_49_VRAM_WRn.state = pins_vram_ctrl_wrn_new;

    if (!vid_rst_new) {
      const bool SACU_CLKPIPE_new = DELTA_HA || DELTA_BC || DELTA_DE || DELTA_FG || pause_pipe_new || state_new.fine_scroll.ROXY_FINE_SCROLL_DONEn;

      pins.lcd.PIN_50_LCD_DATA1.state = RAVO_LD1n;
      pins.lcd.PIN_51_LCD_DATA0.state = REMY_LD0n;
      pins.lcd.PIN_52_LCD_CNTRL.state = !state_new.lcd.SYGU_LINE_STROBE && !state_new.lcd.RUTU_LINE_ENDp_odd;
      pins.lcd.PIN_53_LCD_CLOCK.state = (!state_new.lcd.WUSA_LCD_CLOCK_GATE || !SACU_CLKPIPE_new) && (!state_new.fine_scroll.PUXA_SCX_FINE_MATCH_evn || state_new.fine_scroll.NYZE_SCX_FINE_MATCH_odd);
      pins.lcd.PIN_54_LCD_HSYNC.state = state_new.lcd.POME_X8_LATCH;
      pins.lcd.PIN_55_LCD_LATCH.state = !state_new.lcd.RUTU_LINE_ENDp_odd;
      pins.lcd.PIN_56_LCD_FLIPS.state = state_new.lcd.NAPO_FRAME_EVENp ^ state_new.lcd.LUCA_LINE_EVENp;
      pins.lcd.PIN_57_LCD_VSYNC.state = !state_new.lcd.MEDA_VSYNC_OUTn;
    }
    else {
      pins.lcd.PIN_50_LCD_DATA1.state = RAVO_LD1n;
      pins.lcd.PIN_51_LCD_DATA0.state = REMY_LD0n;
      pins.lcd.PIN_52_LCD_CNTRL.state = 1;
      pins.lcd.PIN_53_LCD_CLOCK.state = 1;
      pins.lcd.PIN_54_LCD_HSYNC.state = 1;
      pins.lcd.PIN_55_LCD_LATCH.state = !get_bit(state_new.reg_div, 6);
      pins.lcd.PIN_56_LCD_FLIPS.state = !get_bit(state_new.reg_div, 7);
      pins.lcd.PIN_57_LCD_VSYNC.state = 1;
    }


    state_new.lcd.RUJU = state_new.lcd.POME_X8_LATCH;
    state_new.lcd.POFY = !state_new.lcd.POME_X8_LATCH;
    state_new.lcd.REMY_LD0n = REMY_LD0n;
    state_new.lcd.RAVO_LD1n = RAVO_LD1n;
    state_new.sprite_scanner.AVAP_SCAN_DONE_tp_odd = !vid_rst_new && !line_rst_new_odd && scan_done_trig_new;
    state_new.tfetch_control.LYRY_BFETCH_DONEp = !fetch_rst_new && get_bit(state_new.tfetch_counter, 0) && get_bit(state_new.tfetch_counter, 2);
    state_new.win_ctrl.NUKO_WX_MATCHp = (uint8_t(~state_new.reg_wx) == state_new.pix_count) && state_new.win_ctrl.REJO_WY_MATCH_LATCHp;
    state_new.sfetch_control.WUTY_SFETCH_DONE_TRIGp = wuty_sfetch_done_new;
    state_new.sfetch_control.TEXY_SFETCHINGp_evn = sfetching_new;
    state_new.WODU_HBLANKp = hblank_new;
    state_new.ACYL_SCANNINGp = (!dma_running_new && state_new.sprite_scanner.BESU_SCAN_DONEn_odd && !vid_rst_new);
    state_new.sprite_scanner.FETO_SCAN_DONEp = (state_new.scan_counter == 39) && !vid_rst_new;
    state_new.SATO_BOOT_BITn = get_bit(state_new.cpu_dbus, 0) || state_new.cpu_signals.TEPU_BOOT_BITn;
    state_new.ATEJ_LINE_RSTp = line_rst_new_odd || vid_rst_new;
    state_new.cpu_signals.SIG_CPU_BOOTp.state = 0;
    state_new.cpu_signals.SIG_BOOT_CSp.state = 0;

    if (cpu_addr_new <= 0x00FF) {

      state_new.cpu_signals.SIG_CPU_BOOTp.state = !state_new.cpu_signals.TEPU_BOOT_BITn;

      if ((cpu.bus_req_new.read && !DELTA_HA) && !state_new.cpu_signals.TEPU_BOOT_BITn) {
        state_new.cpu_signals.SIG_BOOT_CSp.state = 1;
      }
    }

    bit_unpack(pins.vram_dbus, pins_vram_dbus);
    bit_unpack(pins.vram_abus, state_new.vram_abus);

    bit_unpack(pins.dbus, pins_dbus);

    state_new.win_ctrl.ROGE_WY_MATCHp = (state_new.reg_ly == uint8_t(~state_new.reg_wy)) && win_en_new;

    pins.joy.PIN_63_JOY_P14.state = !get_bit(state_new.reg_joy, 0);
    pins.joy.PIN_62_JOY_P15.state = !get_bit(state_new.reg_joy, 1);

    pins.joy.PIN_67_JOY_P10.state = get_bit(state_new.joy_latch, 0);
    pins.joy.PIN_66_JOY_P11.state = get_bit(state_new.joy_latch, 1);
    pins.joy.PIN_65_JOY_P12.state = get_bit(state_new.joy_latch, 2);
    pins.joy.PIN_64_JOY_P13.state = get_bit(state_new.joy_latch, 3);
    

    if (get_bit(state_new.oam_temp_b, 5) && sfetching_old) {
      state_new.flipped_sprite = bit_reverse(state_new.vram_dbus);
    }
    else {
      state_new.flipped_sprite = state_new.vram_dbus;
    }

    state_new.oam_ctrl.old_oam_clk = !state_new.oam_ctrl.SIG_OAM_CLKn; // Vestige of gate mode
    state_new.zram_bus.clk_old = gen_clk_new(phase_total_old, 0b00001110) && cpu_wr;

    state_new.cpu_ack = cpu.core.int_ack;

    pins.sys.PIN_74_CLK.CLK.state = gen_clk_new(phase_total_old, 0b10101010); // dead signal
    pins.sys.PIN_74_CLK.CLKGOOD.state = 1; // dead signal

    pins.sys.PIN_71_RST = 0; // dead signal
    pins.sys.PIN_76_T2 = 0; // dead signal
    pins.sys.PIN_77_T1 = 0; // dead signal

    state_new.sys_clk.SIG_CPU_CLKREQ.state = 1; // dead signal

    state_new.cpu_signals.SIG_CPU_ADDR_HIp.state = cpu_addr_new >= 0xFE00 && cpu_addr_new <= 0xFFFF; // dead signal
    state_new.cpu_signals.SIG_CPU_UNOR_DBG.state = 0; // dead signal
    state_new.cpu_signals.SIG_CPU_UMUT_DBG.state = 0; // dead signal

    pins.sys.PIN_73_CLK_DRIVE.state = pins.sys.PIN_74_CLK.CLK; // dead signal
    state_new.sys_clk.AVET_DEGLITCH.state = pins.sys.PIN_74_CLK.CLK; // dead signal
    state_new.sys_clk.ANOS_DEGLITCH.state = !pins.sys.PIN_74_CLK.CLK; // dead signal

    state_new.sys_clk.AFUR_xxxxEFGH.state = gen_clk_new(phase_total_old, 0b00001111); // dead signal
    state_new.sys_clk.ALEF_AxxxxFGH.state = gen_clk_new(phase_total_old, 0b10000111); // dead signal
    state_new.sys_clk.APUK_ABxxxxGH.state = gen_clk_new(phase_total_old, 0b11000011); // dead signal
    state_new.sys_clk.ADYK_ABCxxxxH.state = gen_clk_new(phase_total_old, 0b11100001); // dead signal

    pins.sys.PIN_75_CLK_OUT.state = gen_clk_new(phase_total_old, 0b00001111); // dead signal

    state_new.sys_clk.SIG_CPU_BOWA_Axxxxxxx.state = gen_clk_new(phase_total_old, 0b10000000); // dead signal
    state_new.sys_clk.SIG_CPU_BEDO_xBCDEFGH.state = gen_clk_new(phase_total_old, 0b01111111); // dead signal
    state_new.sys_clk.SIG_CPU_BEKO_ABCDxxxx.state = gen_clk_new(phase_total_old, 0b11110000); // dead signal
    state_new.sys_clk.SIG_CPU_BUDE_xxxxEFGH.state = gen_clk_new(phase_total_old, 0b00001111); // dead signal
    state_new.sys_clk.SIG_CPU_BOLO_ABCDEFxx.state = gen_clk_new(phase_total_old, 0b11111100); // dead signal
    state_new.sys_clk.SIG_CPU_BUKE_AxxxxxGH.state = gen_clk_new(phase_total_old, 0b10000011); // dead signal
    state_new.sys_clk.SIG_CPU_BOMA_xBCDEFGH.state = gen_clk_new(phase_total_old, 0b01111111); // dead signal
    state_new.sys_clk.SIG_CPU_BOGA_Axxxxxxx.state = gen_clk_new(phase_total_old, 0b10000000); // dead signal

    state_new.cpu_signals.TEDO_CPU_RDp.state = (cpu.bus_req_new.read && !DELTA_HA); // dead signal
    state_new.cpu_signals.APOV_CPU_WRp = gen_clk_new(phase_total_old, 0b00001110) && cpu_wr; // dead signal
    state_new.cpu_signals.TAPU_CPU_WRp = gen_clk_new(phase_total_old, 0b00001110) && cpu_wr; // dead signal
    state_new.cpu_signals.ABUZ_EXT_RAM_CS_CLK = (gen_clk_new(phase_total_old, 0b00111111) && state_new.cpu_signals.SIG_IN_CPU_EXT_BUSp); // dead signal

    state_new.sys_rst.AFER_SYS_RSTp = 0; // dead signal
    state_new.sys_rst.TUBO_WAITINGp = 0; // dead signal
    state_new.sys_rst.ASOL_POR_DONEn = 0; // dead signal
    state_new.sys_rst.SIG_CPU_EXT_CLKGOOD = 1; // dead signal
    state_new.sys_rst.SIG_CPU_EXT_RESETp = 0; // dead signal
    state_new.sys_rst.SIG_CPU_STARTp = 0; // dead signal
    state_new.sys_rst.SIG_CPU_INT_RESETp = 0; // dead signal
    state_new.sys_rst.SOTO_DBG_VRAMp = 0; // dead signal

    state_new.sys_clk.WOSU_AxxDExxH.state = !vid_rst_new && gen_clk_new(phase_total_old, 0b10011001); // dead signal
    state_new.sys_clk.WUVU_ABxxEFxx.state = !vid_rst_new && gen_clk_new(phase_total_old, 0b11001100); // dead signal
    state_new.sys_clk.VENA_xxCDEFxx.state = !vid_rst_new && gen_clk_new(phase_total_old, 0b00111100); // dead signal
  }
}
