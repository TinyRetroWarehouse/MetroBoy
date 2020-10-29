#include "GateBoyThread.h"

#include "CoreLib/Constants.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>

//-----------------------------------------------------------------------------

void GateBoyThread::init() {
  printf("GateBoyThread::init()\n");

  load_rom("roms/tetris.gb");

  // regenerate post-bootrom dump
#if 0
  gb->reset_boot();
  rom_buf = load_blob("roms/tetris.gb");
  gb->set_rom(rom_buf.data(), rom_buf.size());
  gb->run_reset_sequence();

  for (int i = 0; i < 8192; i++) {
    gb->vid_ram[i] = (uint8_t)rand();
  }
#endif

#if 0
  // run tiny app
  if (1) {
    std::string app;
    app += "0150:\n";

    app += "ld a, $00\n";
    app += "ldh ($40), a\n";
    app += "ld a, $73\n";
    app += "ld hl, $8000\n";
    app += "ld (hl), a\n";
    app += "ld hl, $809F\n";
    app += "ld (hl), a\n";

    app += "ld hl, $FF80\n";
    app += "ld a, $E0\n"; app += "ld (hl+), a\n";
    app += "ld a, $46\n"; app += "ld (hl+), a\n";
    app += "ld a, $3E\n"; app += "ld (hl+), a\n";
    app += "ld a, $28\n"; app += "ld (hl+), a\n";
    app += "ld a, $3D\n"; app += "ld (hl+), a\n";
    app += "ld a, $20\n"; app += "ld (hl+), a\n";
    app += "ld a, $FD\n"; app += "ld (hl+), a\n";
    app += "ld a, $C9\n"; app += "ld (hl+), a\n";

    app += "ld a, $80\n";
    app += "call $ff80\n";

    app += "ld a, $00\n";
    app += "ld hl, $8000\n";
    app += "add (hl)\n";
    app += "jr -2\n";

    Assembler as;
    as.assemble(app.c_str());
    blob rom = as.link();

    gb->reset_post_bootrom(rom.data(), rom.size());
  }
#endif

  load_flat_dump("roms/LinksAwakening_dog.dump");
  gb->sys_cpu_en = false;

  /*

  {
    memset(gb->oam_ram, 0, 160);
    memset(gb->vid_ram, 0, 8192);
    uint8_t* cursor = gb->vid_ram;
    for (int i = 0; i < 384; i++) {
      *cursor++ = 0b11111111;
      *cursor++ = 0b11111111;
      *cursor++ = 0b10101011;
      *cursor++ = 0b10000001;
      *cursor++ = 0b10101011;
      *cursor++ = 0b10000001;
      *cursor++ = 0b10101011;
      *cursor++ = 0b10000001;
      *cursor++ = 0b10101011;
      *cursor++ = 0b10000001;f
      *cursor++ = 0b10101011;
      *cursor++ = 0b10000001;
      *cursor++ = 0b10101011;
      *cursor++ = 0b10000001;
      *cursor++ = 0b11111111;
      *cursor++ = 0b11111111;
    }
  }
  */

  /*
  for (int i = 0; i < 2048; i += 2) {
    gb->vid_ram[i + 0] = 0xFF;
    gb->vid_ram[i + 1] = 0x00;
  }
  memset(&gb->vid_ram[1024 * 2], 0x00, 1024 * 4);

  for (int i = 0; i < 160; i+= 4) {
    gb->oam_ram[i + 0] = 0xFF;
    gb->oam_ram[i + 1] = 0xFF;
  }

  gb->oam_ram[0] = 17;
  gb->oam_ram[1] = 8;
  */


  //gb->top.pix_pipe.set_wx(7);
  //gb->top.pix_pipe.set_wy(16);

  // run rom

  //load_rom   ("roms/mealybug/m3_lcdc_win_en_change_multiple_wx.gb");
  //load_golden("roms/mealybug/m3_lcdc_win_en_change_multiple_wx.bmp");

  //load_rom("microtests/build/dmg/oam_read_l0_d.gb");
}

//-----------------------------------------------------------------------------

void GateBoyThread::reset() {
  printf("Resetting sim\n");
  gb.reset_states();
  gb->reset_cart();
  gb->set_rom(rom_buf.data(), rom_buf.size());
}

//------------------------------------------------------------------------------

void GateBoyThread::thread_main() {
  std::unique_lock<std::mutex> lock(mut, std::defer_lock);

  while(!sig_exit) {
    // Lock and wait until we're unpaused and we have a job in the queue.
    lock.lock();
    while (sig_pause || (command.count == 0 && (cursor_r == cursor_w))) {
      sig_waiting = true;
      cv_thread_pause.notify_one();
      cv_thread_resume.wait(lock);
    }
    sig_waiting = false;

    // Grab the next job off the queue.
    if (command.count == 0 && (cursor_r != cursor_w)) {
      command = ring[cursor_r++];
      if (command.op == CMD_StepPhase) gb.push();
    }

    // Unlock and do the job if we have one.
    lock.unlock();
    switch(command.op) {
    case CMD_Exit:      sig_exit = true;  break;
    case CMD_StepPhase: run_step_phase(); break;
    case CMD_StepPass:  run_step_pass();  break;
    case CMD_StepBack:  run_step_back();  break;
    default:
      printf("BAD COMMAND\n");
      command.count = 0;
      break;
    }
  }
}

//----------------------------------------

void GateBoyThread::pause() {
  if (!pause_count++) {
    std::unique_lock<std::mutex> lock(mut);
    sig_pause = true;
    while (!sig_waiting) cv_thread_pause.wait(lock);
  }
}

//----------------------------------------

void GateBoyThread::resume() {
  if (pause_count && !--pause_count) {
    std::unique_lock<std::mutex> lock(mut);
    sig_pause = false;
    cv_thread_resume.notify_one();
  }
}

//----------------------------------------

void GateBoyThread::post_work(Command c) {
  std::unique_lock<std::mutex> lock(mut);
  if (uint8_t(cursor_w + 1) != cursor_r) {
    ring[cursor_w++] = c;
  }
  cv_thread_resume.notify_one();
}

//----------------------------------------

void GateBoyThread::clear_work() {
  pause();
  cursor_r = 0;
  cursor_w = 0;
  command.count = 0;
  resume();
}

//------------------------------------------------------------------------------

void GateBoyThread::run_step_phase() {
  double time_begin = timestamp();

  for(;command.count && !sig_pause; command.count--) {
    gb->next_phase();
  }

  double time_end = timestamp();
  gb->sim_time += (time_end - time_begin);
}

//----------------------------------------

void GateBoyThread::run_step_pass() {
  for(;command.count && !sig_pause; command.count--) {
    gb->next_pass();
  }
}

//----------------------------------------

void GateBoyThread::run_step_back() {
  for(;command.count && !sig_pause; command.count--) {
    printf("pop\n");
    gb.pop();
  }
}

//------------------------------------------------------------------------------

void GateBoyThread::start() {
  if (main) return;
  main = new std::thread([this] { thread_main(); });
}

//----------------------------------------

void GateBoyThread::stop() {
  if (!main) return;
  clear_work();
  post_work({CMD_Exit, 1});
  while (pause_count) resume();
  main->join();
  delete main;
}

//----------------------------------------

void GateBoyThread::step_phase(int steps) {
  post_work({CMD_StepPhase, steps});
}

void GateBoyThread::step_pass(int steps) {
  post_work({CMD_StepPass, steps});
}

void GateBoyThread::step_back(int steps) {
  post_work({CMD_StepBack, steps});
}

//------------------------------------------------------------------------------

void GateBoyThread::step_sim() {
  //printf("%d %d\n", sem_run.count.load(), sem_stop.count.load());
}

//------------------------------------------------------------------------------

void GateBoyThread::load_raw_dump() {
  printf("Loading raw dump from %s\n", "gateboy.raw.dump");
  gb.reset_states();
  gb->load_dump("gateboy.raw.dump");
  gb->set_rom(rom_buf.data(), rom_buf.size());
}

void GateBoyThread::save_raw_dump() {
  printf("Saving raw dump to %s\n", "gateboy.raw.dump");
  gb->save_dump("gateboy.raw.dump");
}

//------------------------------------------------------------------------------
// Load a standard GB rom

void GateBoyThread::load_rom(const char* filename) {
  printf("Loading %s\n", filename);
  rom_buf = load_blob(filename);

  gb.reset_states();
  gb->reset_cart();
  gb->set_rom(rom_buf.data(), rom_buf.size());
  gb->phase_total = 0;
  gb->pass_count = 0;
  gb->pass_total = 0;

  printf("Loaded %zd bytes from rom %s\n", rom_buf.size(), filename);
}


//-----------------------------------------------------------------------------
// Load a flat (just raw contents of memory addresses, no individual regs) dump
// and copy it into the various regs and memory chunks.

void GateBoyThread::load_flat_dump(const char* filename) {
  rom_buf = load_blob(filename);

  gb.reset_states();
  gb->reset_cart();
  gb->set_rom(rom_buf.data(), rom_buf.size());

  memcpy(gb->vid_ram,  rom_buf.data() + 0x8000, 8192);
  memcpy(gb->cart_ram, rom_buf.data() + 0xA000, 8192);
  memcpy(gb->ext_ram,  rom_buf.data() + 0xC000, 8192);
  memcpy(gb->oam_ram,  rom_buf.data() + 0xFE00, 256);
  memcpy(gb->zero_ram, rom_buf.data() + 0xFF80, 128);

  gb->dbg_write(ADDR_BGP,  rom_buf[ADDR_BGP]);
  gb->dbg_write(ADDR_OBP0, rom_buf[ADDR_OBP0]);
  gb->dbg_write(ADDR_OBP1, rom_buf[ADDR_OBP1]);
  gb->dbg_write(ADDR_SCY,  rom_buf[ADDR_SCY]);
  gb->dbg_write(ADDR_SCX,  rom_buf[ADDR_SCX]);
  gb->dbg_write(ADDR_WY,   rom_buf[ADDR_WY]);
  gb->dbg_write(ADDR_WX,   rom_buf[ADDR_WX]);

  // Bit 7 - LCD Display Enable             (0=Off, 1=On)
  // Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
  // Bit 5 - Window Display Enable          (0=Off, 1=On)
  // Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
  // Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
  // Bit 2 - OBJ (Sprite) Size              (0=8x8, 1=8x16)
  // Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
  // Bit 0 - BG Display (for CGB see below) (0=Off, 1=On)

  // #define FLAG_BG_ON        0x01
  // #define FLAG_OBJ_ON       0x02
  // #define FLAG_TALL_SPRITES 0x04
  // #define FLAG_BG_MAP_1     0x08
  // #define FLAG_TILE_0       0x10
  // #define FLAG_WIN_ON       0x20
  // #define FLAG_WIN_MAP_1    0x40
  // #define FLAG_LCD_ON       0x80

  gb->dbg_write(ADDR_LCDC, rom_buf[ADDR_LCDC]);
}

//------------------------------------------------------------------------------

void GateBoyThread::dump1(Dumper& d) {
  const auto& top = gb->top;

  d("\002===== Top =====\001\n");

  const char* phases[] = {
    "\002A_______\001",
    "\003_B______\001",
    "\002__C_____\001",
    "\003___D____\001",
    "\002____E___\001",
    "\003_____F__\001",
    "\002______G_\001",
    "\003_______H\001",
  };

  d("phase %s\n", phases[gb->phase_total & 7]);

  d("State count %d\n", gb.state_count());
  size_t state_size = gb.state_size_bytes();
  if (state_size < 1024 * 1024) {
    d("State size  %d K\n", state_size / 1024);
  }
  else {
    d("State size  %d M\n", state_size / (1024 * 1024));
  }
  d("Phase count %d\n",      gb->phase_total);
  d("Pass count  %d\n",      gb->pass_count);
  d("Pass total  %d\n",      gb->pass_total);
  d("Pass avg    %4.2f\n",   float(gb->pass_total) / float(gb->phase_total));
  d("Pass hash   %016llx\n", gb->pass_hash);
  d("Total hash  %016llx\n", gb->total_hash);
  d("BGB cycle   0x%08x\n",  (gb->phase_total / 4) - 0x10000);
  d("Sim clock   %f\n",      double(gb->phase_total) / (4194304.0 * 2));

  d("Commands left %d\n",    uint8_t(cursor_w - cursor_r));
  d("Steps left    %d\n",    command.count);
  d("Sim time      %f\n",    gb->sim_time);
  d("Waiting       %d\n",    sig_waiting.load());

  double phase_rate = (gb->phase_total - old_phase_total) / (gb->sim_time - old_sim_time);

  if (gb->sim_time == old_sim_time) {
    phase_rate = 0;
  }

  phase_rate_smooth = (phase_rate_smooth * 0.99) + (phase_rate * 0.01);

  d("Phase rate    %f\n",      phase_rate_smooth);
  old_phase_total = gb->phase_total;
  old_sim_time = gb->sim_time;

  /*
  d("Pass count  %lld\n",    pass_count);
  d("Pass rate   %f\n",      double(pass_count) / sim_time);
  d("Phase count %lld\n",    phase_count);
  d("Phase rate  %f\n",      double(phase_count) / sim_time);
  */

  d("\n");
  d("dbg_req ");
  dump_req(d, gb->dbg_req);
  d("cpu_req ");
  dump_req(d, gb->cpu_req);
  d("bus_req ");
  dump_req(d, gb->bus_req);
  d("cpu_data_latch %d 0x%02x\n", gb->cpu_data_latch, gb->cpu_data_latch);
  d("\n");

  gb->dump(d);
  d("\n");

  gb->cpu.dump(d);
  gb->top.tim_reg.dump(d);

  d("\002===== Ints =====\001\n");
  d("IE_D0              %c\n", top.IE_D0.c());
  d("IE_D1              %c\n", top.IE_D1.c());
  d("IE_D2              %c\n", top.IE_D2.c());
  d("IE_D3              %c\n", top.IE_D3.c());
  d("IE_D4              %c\n", top.IE_D4.c());
  d("\n");

  d("LOPE_FF0F_0        %c\n", top.int_reg.LOPE_FF0F_D0p.c());
  d("LALU_FF0F_1        %c\n", top.int_reg.LALU_FF0F_D1p.c());
  d("NYBO_FF0F_2        %c\n", top.int_reg.NYBO_FF0F_D2p.c());
  d("UBUL_FF0F_3        %c\n", top.int_reg.UBUL_FF0F_D3p.c());
  d("ULAK_FF0F_4        %c\n", top.int_reg.ULAK_FF0F_D4p.c());
  d("\n");
  d("MATY_FF0F_L0p      %c\n", top.int_reg.MATY_FF0F_L0p.c());
  d("MOPO_FF0F_L1p      %c\n", top.int_reg.MOPO_FF0F_L1p.c());
  d("PAVY_FF0F_L2p      %c\n", top.int_reg.PAVY_FF0F_L2p.c());
  d("NEJY_FF0F_L3p      %c\n", top.int_reg.NEJY_FF0F_L3p.c());
  d("NUTY_FF0F_L4p      %c\n", top.int_reg.NUTY_FF0F_L4p.c());
  d("\n");
  d("PIN_CPU_INT_VBLANK %c\n", top.int_reg.PIN_CPU_INT_VBLANK.c());
  d("PIN_CPU_INT_STAT   %c\n", top.int_reg.PIN_CPU_INT_STAT.c());
  d("PIN_CPU_INT_TIMER  %c\n", top.int_reg.PIN_CPU_INT_TIMER.c());
  d("PIN_CPU_INT_SERIAL %c\n", top.int_reg.PIN_CPU_INT_SERIAL.c());
  d("PIN_CPU_INT_JOYPAD %c\n", top.int_reg.PIN_CPU_INT_JOYPAD.c());
  d("\n");
  d("PIN_CPU_ACK_VBLANK %c\n", top.int_reg.PIN_CPU_ACK_VBLANK.c());
  d("PIN_CPU_ACK_STAT   %c\n", top.int_reg.PIN_CPU_ACK_STAT.c());
  d("PIN_CPU_ACK_TIMER  %c\n", top.int_reg.PIN_CPU_ACK_TIMER.c());
  d("PIN_CPU_ACK_SERIAL %c\n", top.int_reg.PIN_CPU_ACK_SERIAL.c());
  d("PIN_CPU_ACK_JOYPAD %c\n", top.int_reg.PIN_CPU_ACK_JOYPAD.c());
  d("\n");
}

//------------------------------------------------------------------------------

void GateBoyThread::dump2(Dumper& d) {
  const auto& top = gb->top;

  gb->top.clk_reg.dump(d);

  d("\002===== Joypad =====\001\n");
  d("PIN_JOY_P10      %c\n", top.joypad.PIN_JOY_P10.c());
  d("PIN_JOY_P11      %c\n", top.joypad.PIN_JOY_P11.c());
  d("PIN_JOY_P12      %c\n", top.joypad.PIN_JOY_P12.c());
  d("PIN_JOY_P13      %c\n", top.joypad.PIN_JOY_P13.c());
  d("PIN_JOY_P14      %c\n", top.joypad.PIN_JOY_P14.c());
  d("PIN_JOY_P15      %c\n", top.joypad.PIN_JOY_P15.c());
  d("PIN_CPU_WAKE     %c\n", top.joypad.PIN_CPU_WAKE .c());
  d("ASOK_INT_JOYp    %c\n", top.joypad.ASOK_INT_JOYp.c());
  d("AWOB_WAKE_CPU    %c\n", top.joypad.AWOB_WAKE_CPU.c());
  d("BATU_JP_GLITCH0  %c\n", top.joypad.BATU_JP_GLITCH0.c());
  d("ACEF_JP_GLITCH1  %c\n", top.joypad.ACEF_JP_GLITCH1.c());
  d("AGEM_JP_GLITCH2  %c\n", top.joypad.AGEM_JP_GLITCH2.c());
  d("APUG_JP_GLITCH3  %c\n", top.joypad.APUG_JP_GLITCH3.c());
  d("JUTE_JOYP_RA     %c\n", top.joypad.JUTE_JOYP_RA.c());
  d("KECY_JOYP_LB     %c\n", top.joypad.KECY_JOYP_LB.c());
  d("JALE_JOYP_UC     %c\n", top.joypad.JALE_JOYP_UC.c());
  d("KYME_JOYP_DS     %c\n", top.joypad.KYME_JOYP_DS.c());
  d("KELY_JOYP_UDLR   %c\n", top.joypad.KELY_JOYP_UDLR.c());
  d("COFY_JOYP_ABCS   %c\n", top.joypad.COFY_JOYP_ABCS.c());
  d("KUKO_DBG_FF00_D6 %c\n", top.joypad.KUKO_DBG_FF00_D6.c());
  d("KERU_DBG_FF00_D7 %c\n", top.joypad.KERU_DBG_FF00_D7.c());
  d("KEVU_JOYP_L0     %c\n", top.joypad.KEVU_JOYP_L0.c());
  d("KAPA_JOYP_L1     %c\n", top.joypad.KAPA_JOYP_L1.c());
  d("KEJA_JOYP_L2     %c\n", top.joypad.KEJA_JOYP_L2.c());
  d("KOLO_JOYP_L3     %c\n", top.joypad.KOLO_JOYP_L3.c());
  d("\n");

  gb->top.ser_reg.dump(d);
}

//------------------------------------------------------------------------------

void GateBoyThread::dump3(Dumper& d) {
  gb->top.cpu_bus.dump(d);
  gb->top.ext_bus.dump(d);
  gb->top.vram_bus.dump(d, gb->top);
  gb->top.oam_bus.dump(d);
  gb->top.dma_reg.dump(d);
}

//------------------------------------------------------------------------------

void GateBoyThread::dump4(Dumper& d) {
  const auto& top = gb->top;

  d("\002===== LCD =====\001\n");
  d("PIX COUNT : %03d\n", top.pix_pipe.get_pix_count());
  d("LCD X     : %03d\n", top.lcd_reg.get_lx());
  d("LCD Y     : %03d\n", top.lcd_reg.get_ly());
  d("LYC       : %03d\n", top.lcd_reg.get_lyc());
  d("\n");

  d("PIN_LCD_CLOCK   : "); top.PIN_LCD_CLOCK.dump(d); d("\n");
  d("PIN_LCD_HSYNC   : "); top.PIN_LCD_HSYNC.dump(d); d("\n");
  d("PIN_LCD_VSYNC   : "); top.PIN_LCD_VSYNC.dump(d); d("\n");
  d("PIN_LCD_DATA1   : "); top.PIN_LCD_DATA1.dump(d); d("\n");
  d("PIN_LCD_DATA0   : "); top.PIN_LCD_DATA0.dump(d); d("\n");
  d("PIN_LCD_CNTRL   : "); top.PIN_LCD_CNTRL.dump(d); d("\n");
  d("PIN_LCD_DATALCH : "); top.PIN_LCD_LATCH.dump(d); d("\n");
  d("PIN_LCD_ALTSIGL   : "); top.PIN_LCD_FLIPS.dump(d); d("\n");
  d("\n");

  d("CATU_LINE_P000      %c\n", top.lcd_reg.CATU_LINE_P000.c());
  d("NYPE_LINE_P002      %c\n", top.lcd_reg.NYPE_LINE_P002.c());
  d("ANEL_LINE_P002      %c\n", top.lcd_reg.ANEL_LINE_P002.c());
  d("RUTU_LINE_P910      %c\n", top.lcd_reg.RUTU_LINE_P910.c());
  d("ATEJ_LINE_TRIGp     %c\n", top.lcd_reg.ATEJ_LINE_TRIGp.c());
  d("MYTA_LINE_153p      %c\n", top.lcd_reg.MYTA_LINE_153p     .c());
  d("POPU_IN_VBLANKp     %c\n", top.lcd_reg.POPU_IN_VBLANKp    .c());
  d("ROPO_LY_MATCH_SYNCp %c\n", top.lcd_reg.ROPO_LY_MATCH_SYNCp.c());
  d("PARU_VBLANKp_d4     %c\n", top.lcd_reg.PARU_VBLANKp_d4.c());
  d("VYPU_INT_VBLANKp    %c\n", top.lcd_reg.VYPU_INT_VBLANKp.c());
  d("\n");

  d("\002===== Pix Pipe =====\001\n");

  d.dump_reg("PIX COUNT",
    top.pix_pipe.XEHO_X0p.qp17(), top.pix_pipe.SAVY_X1p.qp17(), top.pix_pipe.XODU_X2p.qp17(), top.pix_pipe.XYDO_X3p.qp17(),
    top.pix_pipe.TUHU_X4p.qp17(), top.pix_pipe.TUKY_X5p.qp17(), top.pix_pipe.TAKO_X6p.qp17(), top.pix_pipe.SYBE_X7p.qp17());


  d.dump_reg("FF40 LCDC",
    top.pix_pipe.VYXE_LCDC_BGENn.qn08(),
    top.pix_pipe.XYLO_LCDC_SPENn.qn08(),
    top.pix_pipe.XYMO_LCDC_SPSIZEn.qn08(),
    top.pix_pipe.XAFO_LCDC_BGMAPn.qn08(),
    top.pix_pipe.WEXU_LCDC_BGTILEn.qn08(),
    top.pix_pipe.WYMO_LCDC_WINENn.qn08(),
    top.pix_pipe.WOKY_LCDC_WINMAPn.qn08(),
    top.pix_pipe.XONA_LCDC_LCDENn.qn08());

  d.dump_reg("FF41 STAT",
    !top.pix_pipe.SADU_STAT_MODE0n,
    !top.pix_pipe.XATY_STAT_MODE1n,
    top.pix_pipe.RUPO_LYC_MATCH_LATCHn.qn03(),
    top.pix_pipe.ROXE_STAT_HBI_ENn.qn08(),
    top.pix_pipe.RUFO_STAT_VBI_ENn.qn08(),
    top.pix_pipe.REFE_STAT_OAI_ENn.qn08(),
    top.pix_pipe.RUGU_STAT_LYI_ENn.qn08(),
    1
  );

  d.dump_reg("FF42 SCY",
    top.pix_pipe.GAVE_SCY0n.qn08(),
    top.pix_pipe.FYMO_SCY1n.qn08(),
    top.pix_pipe.FEZU_SCY2n.qn08(),
    top.pix_pipe.FUJO_SCY3n.qn08(),
    top.pix_pipe.DEDE_SCY4n.qn08(),
    top.pix_pipe.FOTY_SCY5n.qn08(),
    top.pix_pipe.FOHA_SCY6n.qn08(),
    top.pix_pipe.FUNY_SCY7n.qn08()
  );

  d.dump_reg("FF43 SCX",
    top.pix_pipe.DATY_SCX0n.qn08(),
    top.pix_pipe.DUZU_SCX1n.qn08(),
    top.pix_pipe.CYXU_SCX2n.qn08(),
    top.pix_pipe.GUBO_SCX3n.qn08(),
    top.pix_pipe.BEMY_SCX4n.qn08(),
    top.pix_pipe.CUZY_SCX5n.qn08(),
    top.pix_pipe.CABU_SCX6n.qn08(),
    top.pix_pipe.BAKE_SCX7n.qn08()
  );

  d.dump_reg("FF47 BGP",
    top.pix_pipe.PAVO_BGP_D0n.qn07(),
    top.pix_pipe.NUSY_BGP_D1n.qn07(),
    top.pix_pipe.PYLU_BGP_D2n.qn07(),
    top.pix_pipe.MAXY_BGP_D3n.qn07(),
    top.pix_pipe.MUKE_BGP_D4n.qn07(),
    top.pix_pipe.MORU_BGP_D5n.qn07(),
    top.pix_pipe.MOGY_BGP_D6n.qn07(),
    top.pix_pipe.MENA_BGP_D7n.qn07()
  );

  d.dump_reg("FF48 OBP0",
    top.pix_pipe.XUFU_OBP0_D0n.qn07(),
    top.pix_pipe.XUKY_OBP0_D1n.qn07(),
    top.pix_pipe.XOVA_OBP0_D2n.qn07(),
    top.pix_pipe.XALO_OBP0_D3n.qn07(),
    top.pix_pipe.XERU_OBP0_D4n.qn07(),
    top.pix_pipe.XYZE_OBP0_D5n.qn07(),
    top.pix_pipe.XUPO_OBP0_D6n.qn07(),
    top.pix_pipe.XANA_OBP0_D7n.qn07()
  );

  d.dump_reg("FF49 OBP1",
    top.pix_pipe.MOXY_OBP1_D0n.qn07(),
    top.pix_pipe.LAWO_OBP1_D1n.qn07(),
    top.pix_pipe.MOSA_OBP1_D2n.qn07(),
    top.pix_pipe.LOSE_OBP1_D3n.qn07(),
    top.pix_pipe.LUNE_OBP1_D4n.qn07(),
    top.pix_pipe.LUGU_OBP1_D5n.qn07(),
    top.pix_pipe.LEPU_OBP1_D6n.qn07(),
    top.pix_pipe.LUXO_OBP1_D7n.qn07()
  );

  d.dump_reg("FF4A WY",
    top.pix_pipe.NESO_WY0n.qn08(),
    top.pix_pipe.NYRO_WY1n.qn08(),
    top.pix_pipe.NAGA_WY2n.qn08(),
    top.pix_pipe.MELA_WY3n.qn08(),
    top.pix_pipe.NULO_WY4n.qn08(),
    top.pix_pipe.NENE_WY5n.qn08(),
    top.pix_pipe.NUKA_WY6n.qn08(),
    top.pix_pipe.NAFU_WY7n.qn08()
  );

  d.dump_reg("FF4B WX",
    top.pix_pipe.MYPA_WX0n.qn08(),
    top.pix_pipe.NOFE_WX1n.qn08(),
    top.pix_pipe.NOKE_WX2n.qn08(),
    top.pix_pipe.MEBY_WX3n.qn08(),
    top.pix_pipe.MYPU_WX4n.qn08(),
    top.pix_pipe.MYCE_WX5n.qn08(),
    top.pix_pipe.MUVO_WX6n.qn08(),
    top.pix_pipe.NUKU_WX7n.qn08()
  );

  d.dump_reg("BG_PIPE_A",
    top.pix_pipe.MYDE_BG_PIPE_A0.qp16(), top.pix_pipe.NOZO_BG_PIPE_A1.qp16(), top.pix_pipe.MOJU_BG_PIPE_A2.qp16(), top.pix_pipe.MACU_BG_PIPE_A3.qp16(),
    top.pix_pipe.NEPO_BG_PIPE_A4.qp16(), top.pix_pipe.MODU_BG_PIPE_A5.qp16(), top.pix_pipe.NEDA_BG_PIPE_A6.qp16(), top.pix_pipe.PYBO_BG_PIPE_A7.qp16());

  d.dump_reg("BG_PIPE_B",
    top.pix_pipe.TOMY_BG_PIPE_B0.qp16(), top.pix_pipe.TACA_BG_PIPE_B1.qp16(), top.pix_pipe.SADY_BG_PIPE_B2.qp16(), top.pix_pipe.RYSA_BG_PIPE_B3.qp16(),
    top.pix_pipe.SOBO_BG_PIPE_B4.qp16(), top.pix_pipe.SETU_BG_PIPE_B5.qp16(), top.pix_pipe.RALU_BG_PIPE_B6.qp16(), top.pix_pipe.SOHU_BG_PIPE_B7.qp16());

  d.dump_reg("SPR_PIPE_A",
    top.pix_pipe.NYLU_SPR_PIPE_B0.qp16(), top.pix_pipe.PEFU_SPR_PIPE_B1.qp16(), top.pix_pipe.NATY_SPR_PIPE_B2.qp16(), top.pix_pipe.PYJO_SPR_PIPE_B3.qp16(),
    top.pix_pipe.VARE_SPR_PIPE_B4.qp16(), top.pix_pipe.WEBA_SPR_PIPE_B5.qp16(), top.pix_pipe.VANU_SPR_PIPE_B6.qp16(), top.pix_pipe.VUPY_SPR_PIPE_B7.qp16());

  d.dump_reg("SPR_PIPE_B",
    top.pix_pipe.NURO_SPR_PIPE_A0.qp16(), top.pix_pipe.MASO_SPR_PIPE_A1.qp16(), top.pix_pipe.LEFE_SPR_PIPE_A2.qp16(), top.pix_pipe.LESU_SPR_PIPE_A3.qp16(),
    top.pix_pipe.WYHO_SPR_PIPE_A4.qp16(), top.pix_pipe.WORA_SPR_PIPE_A5.qp16(), top.pix_pipe.VAFO_SPR_PIPE_A6.qp16(), top.pix_pipe.WUFY_SPR_PIPE_A7.qp16());

  d.dump_reg("PAL_PIPE",
    top.pix_pipe.RUGO_PAL_PIPE_0.qp16(), top.pix_pipe.SATA_PAL_PIPE_1.qp16(), top.pix_pipe.ROSA_PAL_PIPE_2.qp16(), top.pix_pipe.SOMY_PAL_PIPE_3.qp16(),
    top.pix_pipe.PALU_PAL_PIPE_4.qp16(), top.pix_pipe.NUKE_PAL_PIPE_5.qp16(), top.pix_pipe.MODA_PAL_PIPE_6.qp16(), top.pix_pipe.LYME_PAL_PIPE_7.qp16()
  );

  d.dump_reg("MASK_PIPE",
    top.pix_pipe.VEZO_MASK_PIPE_0.qp16(), top.pix_pipe.WURU_MASK_PIPE_1.qp16(), top.pix_pipe.VOSA_MASK_PIPE_2.qp16(), top.pix_pipe.WYFU_MASK_PIPE_3.qp16(),
    top.pix_pipe.XETE_MASK_PIPE_4.qp16(), top.pix_pipe.WODA_MASK_PIPE_5.qp16(), top.pix_pipe.VUMO_MASK_PIPE_6.qp16(), top.pix_pipe.VAVA_MASK_PIPE_7.qp16()
  );

  d.dump_reg("WIN X",
    0,
    0,
    0,
    top.pix_pipe.WYKA_WIN_X3.qp17(),
    top.pix_pipe.WODY_WIN_X4.qp17(),
    top.pix_pipe.WOBO_WIN_X5.qp17(),
    top.pix_pipe.WYKO_WIN_X6.qp17(),
    top.pix_pipe.XOLO_WIN_X7.qp17()
  );

  d.dump_reg("WIN Y",
    top.pix_pipe.VYNO_WIN_Y0.qp17(),
    top.pix_pipe.VUJO_WIN_Y1.qp17(),
    top.pix_pipe.VYMU_WIN_Y2.qp17(),
    top.pix_pipe.TUFU_WIN_Y3.qp17(),
    top.pix_pipe.TAXA_WIN_Y4.qp17(),
    top.pix_pipe.TOZO_WIN_Y5.qp17(),
    top.pix_pipe.TATE_WIN_Y6.qp17(),
    top.pix_pipe.TEKE_WIN_Y7.qp17()
  );

  d("\n");

  d("ROXY_FINE_SCROLL_DONEn %c\n", top.pix_pipe.ROXY_SCX_FINE_MATCH_LATCHn.c());
  d("RYKU_FINE_CNT0         %c\n", top.pix_pipe.RYKU_FINE_CNT0.c());
  d("ROGA_FINE_CNT1         %c\n", top.pix_pipe.ROGA_FINE_CNT1.c());
  d("RUBU_FINE_CNT2         %c\n", top.pix_pipe.RUBU_FINE_CNT2.c());
  d("XYMU_RENDERINGp        %c\n", top.pix_pipe.XYMU_RENDERINGn.c());
  d("RUPO_LYC_MATCH_LATCHn  %c\n", top.pix_pipe.RUPO_LYC_MATCH_LATCHn.c());
  d("WUSA_LCD_CLOCK_GATE    %c\n", top.pix_pipe.WUSA_LCD_CLOCK_GATE.c());
  d("VOGA_RENDER_DONE_SYNC  %c\n", top.pix_pipe.VOGA_HBLANKp.c());
  d("PUXA_FINE_MATCH_A      %c\n", top.pix_pipe.PUXA_SCX_FINE_MATCH_A.c());
  d("NYZE_FINE_MATCH_B      %c\n", top.pix_pipe.NYZE_SCX_FINE_MATCH_B.c());
  d("PAHO_X_8_SYNC          %c\n", top.pix_pipe.PAHO_X_8_SYNC.c());
  d("POFY_HSYNCp            %c\n", top.pix_pipe.POFY.c());
  d("VOTY_INT_STATp         %c\n", top.pix_pipe.VOTY_INT_STATp.c());
  d("\n");


  /*
  d("\002===== Window =====\001\n");

  d("PYNU_WIN_MODE_A       : %c\n", PYNU_WIN_MODE_A.c());
  d("RYDY_WIN_FIRST_TILE_A : %c\n", RYDY_WIN_FIRST_TILE_A.c());
  d("NOPA_WIN_MODE_B       : %c\n", NOPA_WIN_MODE_B.c());
  d("SOVY_WIN_FIRST_TILE_B : %c\n", SOVY_WIN_FIRST_TILE_B.c());
  d("REJO_WY_MATCH_LATCH   : %c\n", REJO_WY_MATCH_LATCH.c());
  d("SARY_WY_MATCH         : %c\n", SARY_WY_MATCH.c());
  d("RYFA_FETCHn_A         : %c\n", RYFA_FETCHn_A.c());
  d("RENE_FETCHn_B         : %c\n", RENE_FETCHn_B.c());
  d("PYCO_WX_MATCH_A       : %c\n", PYCO_WX_MATCH_A.c());
  d("NUNU_WX_MATCH_B       : %c\n", NUNU_WX_MATCH_B.c());
  d("\n");
  */
}

//------------------------------------------------------------------------------

void GateBoyThread::dump5(Dumper& d) {
  gb->top.sprite_fetcher.dump(d);
  gb->top.sprite_scanner.dump(d, gb->top);
  gb->top.sprite_store.dump(d, gb->top);
  gb->top.tile_fetcher.dump(d, gb->top);
}

//------------------------------------------------------------------------------

void GateBoyThread::dump6(Dumper& d) {
  d("\002===== Disasm =====\001\n");
  {
    uint16_t pc = gb->cpu.op_addr;
    const uint8_t* code = nullptr;
    uint16_t code_size = 0;
    uint16_t code_base = 0;

    if (!gb->top.bootrom.BOOT_BITn.qp17()) {
      code = DMG_ROM_bin;
      code_size = 256;
      code_base = ADDR_BOOT_ROM_BEGIN;
    }
    else if (pc >= 0x0000 && pc <= 0x7FFF) {
      // FIXME needs to account for mbc1 mem mapping
      code = gb->rom_buf;
      code_size = 32768;
      code_base = ADDR_CART_ROM_BEGIN;
    }
    else if (pc >= 0xFF80 && pc <= 0xFFFE) {
      code = gb->zero_ram;
      code_size = 127;
      code_base = ADDR_ZEROPAGE_BEGIN;
    }

    assembler.disassemble(code, code_size, code_base, pc, 34, d, /*collapse_nops*/ false);
  }
  d("\n");

  d("\002===== OAM =====\001\n");
  for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 16; x++) {
      d("%02x ", gb->oam_ram[x + y * 16]);
    }
    d("\n");
  }
  d("\n");
  d("\002===== ZRAM =====\001\n");
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 16; x++) {
      d("%02x ", gb->zero_ram[x + y * 16]);
    }
    d("\n");
  }
  d("\n");
}

//------------------------------------------------------------------------------
