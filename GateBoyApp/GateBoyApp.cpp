#include "GateBoyApp/GateBoyApp.h"

#include "CoreLib/Constants.h"
#include "CoreLib/Debug.h" // for StringDumper
#include "CoreLib/Tests.h"

#include "AppLib/AppHost.h"
#include "AppLib/GLBase.h"

#define SDL_MAIN_HANDLED
#ifdef _MSC_VER
#include "SDL/include/SDL.h"
#else
#include <SDL2/SDL.h>
#endif

#include <windows.h>

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  SetPriorityClass(GetCurrentProcess(), 0x00000080);

  App* app = new GateBoyApp();
  AppHost* app_host = new AppHost(app);
  int ret = app_host->app_main(argc, argv);
  delete app;
  return ret;
}

//-----------------------------------------------------------------------------

void GateBoyApp::app_init() {
  LOG_G("GateBoyApp::app_init()\n");
  LOG_SCOPE_INDENT();

  grid_painter.init();
  text_painter.init();
  dump_painter.init();
  gb_blitter.init();
  blitter.init();

  trace_tex = create_texture_u32(912, 154);
  ram_tex = create_texture_u8(256, 256);
  overlay_tex = create_texture_u32(160, 144);
  keyboard_state = SDL_GetKeyboardState(nullptr);

#if 0
  // regenerate post-bootrom dump
  gb_thread.reset_boot(DMG_ROM_blob, load_blob("roms/tetris.gb"));
  for (int i = 0; i < 8192; i++) {
    gb_thread.gb->vid_ram[i] = (uint8_t)rand();
  }
#endif


#if 0
  // run tiny app
  if (1) {
    /*
    std::string app = R"(
    0150:
      ld a, $00
      ldh ($40), a
      ld a, $73
      ld hl, $8000
      ld (hl), a
      ld hl, $809F
      ld (hl), a

      ld hl, $FF80
      ld a, $E0
      ld (hl+), a
      ld a, $46
      ld (hl+), a
      ld a, $3E
      ld (hl+), a
      ld a, $28
      ld (hl+), a
      ld a, $3D
      ld (hl+), a
      ld a, $20
      ld (hl+), a
      ld a, $FD
      ld (hl+), a
      ld a, $C9
      ld (hl+), a

      ld a, $80
      call $ff80

      ld a, $00
      ld hl, $8000
      add (hl)
      jr -2
    )";
    */

    std::string app = R"(
    0150:
      ld a, ($FF40)
      ld a, ($FF40)
      ld a, ($FF40)
      ld a, ($FF40)
      ld a, ($FF40)
      ld a, ($FF40)
      ld a, ($FF40)
      jp $0150
    )";

    Assembler as;
    as.assemble(app.c_str());
    blob rom = as.link();

    gb_thread.reset_cart(DMG_ROM_blob, rom);
  }
#endif

#if 1
  load_flat_dump("roms/LinksAwakening_dog.dump");
  gb_thread.gb->sys_cpu_en = false;
  gb_thread.gb->phase_total = 0;

  gb_thread.gb->dbg_write(ADDR_WY, 113);
  gb_thread.gb->dbg_write(ADDR_WX, 13 + 7);

  //gb_thread.gb->dbg_write(ADDR_SCX, 3);

#endif

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


  //gb->pix_pipe.set_wx(7);
  //gb->pix_pipe.set_wy(16);

  // run rom

  //load_rom   ("roms/mealybug/m3_lcdc_win_en_change_multiple_wx.gb");
  //load_golden("roms/mealybug/m3_lcdc_win_en_change_multiple_wx.bmp");

  //load_rom("microtests/build/dmg/win10_scx3_a.gb");

  LOG_G("GateBoyApp::app_init() done\n");
  gb_thread.start();
}

//-----------------------------------------------------------------------------

void GateBoyApp::app_close() {
  gb_thread.stop();
}

//------------------------------------------------------------------------------

void GateBoyApp::load_raw_dump() {
  printf("Loading raw dump from %s\n", "gateboy.raw.dump");
  gb_thread.gb->load_dump("gateboy.raw.dump");
  gb_thread.set_cart(DMG_ROM_blob, gb_thread.cart);
}

void GateBoyApp::save_raw_dump() {
  printf("Saving raw dump to %s\n", "gateboy.raw.dump");
  gb_thread.gb->save_dump("gateboy.raw.dump");
}

//------------------------------------------------------------------------------
// Load a standard GB rom

void GateBoyApp::load_rom(const char* filename) {
  printf("Loading %s\n", filename);

  gb_thread.reset_cart(DMG_ROM_blob, load_blob(filename));

  printf("Loaded %zd bytes from rom %s\n", gb_thread.cart.size(), filename);
}


//-----------------------------------------------------------------------------
// Load a flat (just raw contents of memory addresses, no individual regs) dump
// and copy it into the various regs and memory chunks.

void GateBoyApp::load_flat_dump(const char* filename) {

  gb_thread.reset_cart(DMG_ROM_blob, load_blob(filename));

  memcpy(gb_thread.gb->vid_ram,  gb_thread.cart.data() + 0x8000, 8192);
  memcpy(gb_thread.gb->cart_ram, gb_thread.cart.data() + 0xA000, 8192);
  memcpy(gb_thread.gb->ext_ram,  gb_thread.cart.data() + 0xC000, 8192);
  memcpy(gb_thread.gb->oam_ram,  gb_thread.cart.data() + 0xFE00, 256);
  memcpy(gb_thread.gb->zero_ram, gb_thread.cart.data() + 0xFF80, 128);

  gb_thread.gb->dbg_write(ADDR_BGP,  gb_thread.cart[ADDR_BGP]);
  gb_thread.gb->dbg_write(ADDR_OBP0, gb_thread.cart[ADDR_OBP0]);
  gb_thread.gb->dbg_write(ADDR_OBP1, gb_thread.cart[ADDR_OBP1]);
  gb_thread.gb->dbg_write(ADDR_SCY,  gb_thread.cart[ADDR_SCY]);
  gb_thread.gb->dbg_write(ADDR_SCX,  gb_thread.cart[ADDR_SCX]);
  gb_thread.gb->dbg_write(ADDR_WY,   gb_thread.cart[ADDR_WY]);
  gb_thread.gb->dbg_write(ADDR_WX,   gb_thread.cart[ADDR_WX]);

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

  gb_thread.gb->dbg_write(ADDR_LCDC, gb_thread.cart[ADDR_LCDC]);
}


//-----------------------------------------------------------------------------

void GateBoyApp::app_update(double /*delta*/) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {

    if (event.type == SDL_KEYDOWN)
    switch (event.key.keysym.sym) {

    case SDLK_SPACE: { gb_thread.sig_pause ? gb_thread.resume() : gb_thread.pause(); break; }

    case SDLK_f: {
      gb_thread.clear_work();
      if (runmode != RUN_FAST) {
        gb_thread.step_phase(1024 * 1024 * 1024);
        gb_thread.resume();
        runmode = RUN_FAST;
      }
      break;
    }
    case SDLK_v: {
      gb_thread.clear_work();
      runmode = RUN_SYNC;
      break;
    }
    case SDLK_s: {
      gb_thread.clear_work();
      runmode = RUN_STEP;
      break;
    }

    case SDLK_F1:   load_raw_dump();            break;
    case SDLK_F4:   save_raw_dump();            break;
    case SDLK_r:    gb_thread.reset_cart(gb_thread.boot, gb_thread.cart);          break;
    //case SDLK_d:    show_diff   = !show_diff;   break;
    case SDLK_g:    show_golden = !show_golden; break;
    case SDLK_o:    draw_passes = !draw_passes; break;
    case SDLK_UP:   stepmode = STEP_PHASE;      break;
    case SDLK_DOWN: stepmode = STEP_PASS;       break;
    //case SDLK_c:  { gb_thread.toggle_cpu(); break; }

    case SDLK_LEFT:   {
      if (runmode == RUN_STEP) {
        if (keyboard_state[SDL_SCANCODE_LCTRL]) {
          gb_thread.step_back(8);
        } else {
          gb_thread.step_back(1);
        }
      }
      break;
    }

    case SDLK_RIGHT:  {
      if (runmode == RUN_STEP) {
        if (stepmode == STEP_PHASE) {
          if (keyboard_state[SDL_SCANCODE_LCTRL] && keyboard_state[SDL_SCANCODE_LALT]) {
            gb_thread.step_phase(114 * 8 * 8);
          }
          else if (keyboard_state[SDL_SCANCODE_LALT]) {
            gb_thread.step_phase(114 * 8);
          } else if (keyboard_state[SDL_SCANCODE_LCTRL]) {
            gb_thread.step_phase(8);
          } else {
            gb_thread.step_phase(1);
          }
        }
        else {
          gb_thread.step_pass(1);
        }
      }
      break;
    }
    }

    if (event.type == SDL_DROPFILE) {
      load_rom(event.drop.file);
      SDL_free(event.drop.file);
    }
  }
}

//-----------------------------------------------------------------------------

#pragma warning(disable:4189)

void GateBoyApp::app_render_frame(Viewport view) {
  gb_thread.pause();

  grid_painter.render(view);

  const auto gb = gb_thread.gb.state();

  uint8_t* framebuffer = gb->framebuffer;
  uint8_t* vid_ram = gb->vid_ram;
  int64_t phase_total = gb->phase_total;

  StringDumper d;
  float cursor = 0;

  //----------------------------------------

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

  d("State count %d\n", gb_thread.gb.state_count());
  size_t state_size = gb_thread.gb.state_size_bytes();
  if (state_size < 1024 * 1024) {
    d("State size  %d K\n", state_size / 1024);
  }
  else {
    d("State size  %d M\n", state_size / (1024 * 1024));
  }
  d("BGB cycle   0x%08x\n",  (gb->phase_total / 4) - 0x10000);
  d("Sim clock   %f\n",      double(gb->phase_total) / (4194304.0 * 2));

  d("Commands left %d\n",    uint8_t(gb_thread.cursor_w - gb_thread.cursor_r));
  d("Steps left    %d\n",    gb_thread.command.count);
  d("Waiting       %d\n",    gb_thread.sig_waiting.load());

  double phase_rate = (gb->phase_total - gb_thread.old_phase_total) / (gb->sim_time - gb_thread.old_sim_time);

  if (gb->sim_time == gb_thread.old_sim_time) {
    phase_rate = 0;
  }

  gb_thread.phase_rate_smooth = (gb_thread.phase_rate_smooth * 0.99) + (phase_rate * 0.01);

  d("Phase rate    %f\n",      gb_thread.phase_rate_smooth);
  d("Sim fps       %f\n",      60.0 * gb_thread.phase_rate_smooth / PHASES_PER_SECOND);

  gb_thread.old_phase_total = gb->phase_total;
  gb_thread.old_sim_time = gb->sim_time;

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

  d("\002===== GateBoy =====\001\n");
  d("sys_rst        %d\n",      gb->sys_rst);
  d("sys_t1         %d\n",      gb->sys_t1);
  d("sys_t2         %d\n",      gb->sys_t2);
  d("sys_clken      %d\n",      gb->sys_clken);
  d("sys_clkgood    %d\n",      gb->sys_clkgood);
  d("sys_cpuready   %d\n",      gb->sys_clkreq);
  d("sys_cpu_en     %d\n",      gb->sys_cpu_en);
  d("sys_fastboot   %d\n",      gb->sys_fastboot);
  d("sys_buttons    %d\n",      gb->sys_buttons);
  d("\n");

  d("gb_screen_x    %d\n",      gb->gb_screen_x);
  d("gb_screen_y    %d\n",      gb->gb_screen_y);
  d("lcd_data_latch %d\n", gb->lcd_data_latch);
  d.dump_bitp("lcd_pix_lo ",  gb->lcd.lcd_pix_lo.state);
  d.dump_bitp("lcd_pix_hi ",  gb->lcd.lcd_pix_hi.state);

  d.dump_slice2p("lcd_pipe_lo", gb->lcd.lcd_pipe_lo, 8);
  d.dump_slice2p("lcd_pipe_hi", gb->lcd.lcd_pipe_hi, 8);
  d("\n");

  d("sim_time    %f\n",      gb->sim_time);
  d("phase_total %d\n",      gb->phase_total);
  d("pass_hash   %016llx\n", gb->phase_hash);
  d("total_hash  %016llx\n", gb->cumulative_hash);
  d("\n");

  d("\002===== CPU =====\001\n");
  d("state    %d\n", gb->cpu.state);
  d("state_   %d\n", gb->cpu.state_);
  d("op addr  0x%04x\n", gb->cpu.op_addr);
  d("opcode   0x%02x\n", gb->cpu.op);
  d("opname   '%s' @ %d\n", op_strings2[gb->cpu.op], gb->cpu.state);
  d("CB       0x%02x\n", gb->cpu.cb);
  d("in       0x%02x\n", gb->cpu.in);
  d("out      0x%02x\n", gb->cpu.out);
  d("\n");
  d("bus req   ");
  dump_req(d, gb->cpu.bus_req);
  d("PC        0x%04x 0x%02x 0x%02x\n", gb->cpu.pc, gb->cpu.pcl, gb->cpu.pch);
  d("SP        0x%04x 0x%02x 0x%02x\n", gb->cpu.sp, gb->cpu.sph, gb->cpu.spl);
  d("XY        0x%04x 0x%02x 0x%02x\n", gb->cpu.xy, gb->cpu.xyh, gb->cpu.xyl);
  d("BC        0x%04x 0x%02x 0x%02x\n", gb->cpu.bc, gb->cpu.b,   gb->cpu.c);
  d("DE        0x%04x 0x%02x 0x%02x\n", gb->cpu.de, gb->cpu.d,   gb->cpu.e);
  d("HL        0x%04x 0x%02x 0x%02x\n", gb->cpu.hl, gb->cpu.h,   gb->cpu.l);
  d("AF        0x%04x 0x%02x 0x%02x\n", gb->cpu.af, gb->cpu.a,   gb->cpu.f);
  d("alu_f     0x%02x\n", gb->cpu.alu_f);
  d("\n");
  d("IME       %d\n", gb->cpu.ime);
  d("IME_      %d\n", gb->cpu.ime_delay);
  d("interrupt %d\n", gb->cpu.op == 0xF4);
  d("int_ack   0x%02x\n", gb->cpu.int_ack);
  d("\n");

  d("\002===== Ints =====\001\n");
  d.dump_bitp("IE_D0        ", gb->interrupts.IE_D0.state);
  d.dump_bitp("IE_D1        ", gb->interrupts.IE_D1.state);
  d.dump_bitp("IE_D2        ", gb->interrupts.IE_D2.state);
  d.dump_bitp("IE_D3        ", gb->interrupts.IE_D3.state);
  d.dump_bitp("IE_D4        ", gb->interrupts.IE_D4.state);
  d("\n");
  d.dump_bitp("LOPE_FF0F_0  ", gb->interrupts.LOPE_FF0F_D0p.state);
  d.dump_bitp("LALU_FF0F_1  ", gb->interrupts.LALU_FF0F_D1p.state);
  d.dump_bitp("NYBO_FF0F_2  ", gb->interrupts.NYBO_FF0F_D2p.state);
  d.dump_bitp("UBUL_FF0F_3  ", gb->interrupts.UBUL_FF0F_D3p.state);
  d.dump_bitp("ULAK_FF0F_4  ", gb->interrupts.ULAK_FF0F_D4p.state);
  d("\n");
  d.dump_bitp("MATY_FF0F_L0p", gb->interrupts.MATY_FF0F_L0p.state);
  d.dump_bitp("MOPO_FF0F_L1p", gb->interrupts.MOPO_FF0F_L1p.state);
  d.dump_bitp("PAVY_FF0F_L2p", gb->interrupts.PAVY_FF0F_L2p.state);
  d.dump_bitp("NEJY_FF0F_L3p", gb->interrupts.NEJY_FF0F_L3p.state);
  d.dump_bitp("NUTY_FF0F_L4p", gb->interrupts.NUTY_FF0F_L4p.state);
  d("\n");

  text_painter.render(view, d.s.c_str(), cursor, 0);
  cursor += 224 - 32;
  d.clear();

  //----------------------------------------

  d("\002===== Clocks =====\001\n");
  d("PHASE %d%d%d%d\n",
    gb->pclk.AFUR_xxxxEFGHp.qp_old(),
    gb->pclk.ALEF_AxxxxFGHp.qp_old(),
    gb->pclk.APUK_ABxxxxGHp.qp_old(),
    gb->pclk.ADYK_ABCxxxxHp.qp_old());
  d("\n");
  d.dump_bitp("TUBO_WAITINGp ", gb->rstdbg.TUBO_WAITINGp.state);
  d.dump_bitn("ASOL_POR_DONEn", gb->rstdbg.ASOL_POR_DONEn.state);
  d.dump_bitp("AFER_SYS_RSTp ", gb->rstdbg.AFER_SYS_RSTp_evn.state);
  d("\n");
  d.dump_bitp("AFUR_xxxxEFGHp", gb->pclk.AFUR_xxxxEFGHp.state);
  d.dump_bitp("ALEF_AxxxxFGHp", gb->pclk.ALEF_AxxxxFGHp.state);
  d.dump_bitp("APUK_ABxxxxGHp", gb->pclk.APUK_ABxxxxGHp.state);
  d.dump_bitp("ADYK_ABCxxxxHp", gb->pclk.ADYK_ABCxxxxHp.state);
  d("\n");
  d.dump_bitp("WUVU_ABxxEFxxp", gb->vclk.WUVU_ABxxEFxx.state);
  d.dump_bitp("VENA_xxCDEFxxp", gb->vclk.VENA_xxCDEFxx.state);
  d.dump_bitp("WOSU_AxxDExxHp", gb->vclk.WOSU_AxxDExxH.state);
  d("\n");

  d("\002===== Timer =====\001\n");
  d.dump_bitp("NYDU_TIMA7p_DELAY  ", gb->timer.NYDU_TIMA7p_DELAY_evn.state);
  d.dump_bitp("MOBA_TIMER_OVERFLOWp", gb->timer.MOBA_TIMER_OVERFLOWp_evn.state);
  d.dump_slice2p("DIV ", &gb->div.UKUP_DIV00p_evn, 16);
  d.dump_slice2p("TIMA", &gb->timer.REGA_TIMA0p_evn, 8);
  d.dump_slice2p("TMA ", &gb->timer.SABU_TMA0p_h, 8);
  d.dump_slice2p("TAC ", &gb->timer.SOPU_TAC0p_h, 3);
  d("\n");

  d("\002===== Joypad =====\001\n");
  d.dump_bitp("AWOB_WAKE_CPU  ", gb->joypad.AWOB_WAKE_CPU.state);
  d("\n");
  d.dump_bitp("BATU_JP_GLITCH0", gb->joypad.BATU_JP_GLITCH0.state);
  d.dump_bitp("ACEF_JP_GLITCH1", gb->joypad.ACEF_JP_GLITCH1.state);
  d.dump_bitp("AGEM_JP_GLITCH2", gb->joypad.AGEM_JP_GLITCH2.state);
  d.dump_bitp("APUG_JP_GLITCH3", gb->joypad.APUG_JP_GLITCH3.state);
  d("\n");
  d.dump_bitp("JUTE_JOYP_RA   ", gb->joypad.JUTE_JOYP_RA.state);
  d.dump_bitp("KECY_JOYP_LB   ", gb->joypad.KECY_JOYP_LB.state);
  d.dump_bitp("JALE_JOYP_UC   ", gb->joypad.JALE_JOYP_UC.state);
  d.dump_bitp("KYME_JOYP_DS   ", gb->joypad.KYME_JOYP_DS.state);
  d.dump_bitp("KELY_JOYP_UDLR ", gb->joypad.KELY_JOYP_UDLRp.state);
  d.dump_bitp("COFY_JOYP_ABCS ", gb->joypad.COFY_JOYP_ABCSp.state);
  d("\n");
  d.dump_bitn("KUKO_DBG_FF00_D", gb->joypad.KUKO_DBG_FF00_D6n.state);
  d.dump_bitn("KERU_DBG_FF00_D", gb->joypad.KERU_DBG_FF00_D7n.state);
  d("\n");
  d.dump_bitn("KEVU_JOYP_L0   ", gb->joypad.KEVU_JOYP_L0n.state);
  d.dump_bitn("KAPA_JOYP_L1   ", gb->joypad.KAPA_JOYP_L1n.state);
  d.dump_bitn("KEJA_JOYP_L2   ", gb->joypad.KEJA_JOYP_L2n.state);
  d.dump_bitn("KOLO_JOYP_L3   ", gb->joypad.KOLO_JOYP_L3n.state);
  d("\n");

  d("\002===== Serial =====\001\n");
  d.dump_bitp   ("ETAF_SER_RUNNING", gb->serial.ETAF_SER_RUNNING.state);
  d.dump_bitp   ("CULY_XFER_DIR   ", gb->serial.CULY_XFER_DIR.state);
  d.dump_bitp   ("COTY_SER_CLK    ", gb->serial.COTY_SER_CLK.state);
  d.dump_bitp   ("ELYS_SER_OUT    ", gb->serial.ELYS_SER_OUT.state);
  d.dump_slice2p("CAFA_SER_CNT    ", &gb->serial.CAFA_SER_CNT0, 4);
  d.dump_slice2p("CUBA_SER_DATA   ", &gb->serial.CUBA_SER_DATA0, 8);
  d("\n");

  text_painter.render(view, d.s.c_str(), cursor, 0);
  cursor += 224 - 64;
  d.clear();

  //----------------------------------------

  d("\002===== Buses =====\001\n");
  d.dump_slice2p("BUS_CPU_D_out    ", &gb->BUS_CPU_D_out, 8);
  d.dump_bitp   ("MAKA_HOLD_MEMp   ",  gb->oam_bus.MAKA_LATCH_EXTp_evn.state);
  d.dump_bitp   ("WUJE_CPU_OAM_WRn ",  gb->oam_bus.WUJE_CPU_OAM_WRn_evn.state);
  d.dump_slice2p("EXT_ADDR         ", &gb->ext_addr_latch.ALOR_EXT_ADDR_LATCH_00p, 15);
  d.dump_slice2n("EXT_DATA         ", &gb->ext_data_latch.SOMA_EXT_DATA_LATCH_D0n, 8);
  d.dump_slice2n("SPRITE TEMP A    ", &gb->sprite_temp_a.REWO_SPRITE_DA0n, 8);
  d.dump_slice2n("SPRITE TEMP B    ", &gb->sprite_temp_b.PEFO_SPRITE_DB0n, 8);
  d.dump_slice2n("OAM LATCH A      ", &gb->oam_latch_a.YDYV_OAM_LATCH_DA0n_odd, 8);
  d.dump_slice2n("OAM LATCH B      ", &gb->oam_latch_b.XYKY_OAM_LATCH_DB0n_odd, 8);
  d.dump_slice2p("OAM TEMP A       ", &gb->oam_temp_a.XUSO_OAM_DA0p_evn, 8);
  d.dump_slice2p("OAM TEMP B       ", &gb->oam_temp_b.YLOR_OAM_DB0p_evn, 8);
  d("\n");

  d("\002===== DMA Reg =====\001\n");
  d("DMA Addr 0x%02x:%02x\n", pack_u8n(8, &gb->dma.NAFA_DMA_A08n_h), pack_u8p(8, &gb->dma.NAKY_DMA_A00p_evn));
  d("\n");
  d.dump_bitp("MATU_DMA_RUNNINGp", gb->dma.MATU_DMA_RUNNINGp_evn.state);
  d.dump_bitp("LYXE_DMA_LATCHp  ", gb->dma.LYXE_DMA_LATCHp_evn  .state);
  d.dump_bitp("MYTE_DMA_DONE    ", gb->dma.MYTE_DMA_DONE_evn    .state);
  d.dump_bitp("LUVY_DMA_TRIG_d0 ", gb->dma.LUVY_DMA_TRIG_d0_evn .state);
  d.dump_bitp("LENE_DMA_TRIG_d4 ", gb->dma.LENE_DMA_TRIG_d4_evn .state);
  d.dump_bitp("LOKY_DMA_LATCHp  ", gb->dma.LOKY_DMA_LATCHp_evn  .state);
  d("\n");
  d.dump_slice2p("DMA_A_LOW ", &gb->dma.NAKY_DMA_A00p_evn, 8);
  d.dump_slice2n("DMA_A_HIGH", &gb->dma.NAFA_DMA_A08n_h, 8);
  d("\n");

  d("\002===== LCD =====\001\n");
  d("LX        : %03d\n", gb->reg_lx.get());
  d("LY        : %03d\n", gb->reg_ly.get());
  d("LYC       : %03d\n", gb->reg_lyc.get());
  d.dump_bitp("lcd_pix_lo", gb->lcd.lcd_pix_lo.state);
  d.dump_bitp("lcd_pix_lo", gb->lcd.lcd_pix_hi.state);

  d("\n");

  d.dump_bitp("LX NYPE_LINE_P002p ", gb->reg_lx.NYPE_x113p_c.state);
  d.dump_bitp("LX RUTU_LINE_P910p ", gb->reg_lx.RUTU_x113p_g.state);

  d.dump_bitp("LY MYTA_y153p      ", gb->reg_ly.MYTA_y153p_evn.state);
  d.dump_bitp("ROPO_LY_MATCH_SYNCp", gb->reg_lyc.ROPO_LY_MATCH_SYNCp_c.state);

  d.dump_bitp("POPU_VBLANKp       ", gb->lcd.POPU_VBLANKp_evn.state);
  d.dump_bitp("SYGU_LINE_STROBE   ", gb->lcd.SYGU_LINE_STROBE_evn.state);
  d.dump_bitn("MEDA_VSYNC_OUTn    ", gb->lcd.MEDA_VSYNC_OUTn_evn.state);
  d.dump_bitp("LUCA_LINE_EVENp    ", gb->lcd.LUCA_LINE_EVENp_evn.state);
  d.dump_bitp("NAPO_FRAME_EVENp   ", gb->lcd.NAPO_FRAME_EVENp_evn.state);
  d.dump_bitp("CATU_LINE_P000p    ", gb->lcd.CATU_LINE_P000p_a.state);
  d.dump_bitp("ANEL_LINE_P002p    ", gb->lcd.ANEL_LINE_P002p_c.state);
  d("\n");
  d.dump_slice2p("LX  ", &gb->reg_lx.SAXO_LX0p_evn.state,  7);
  d.dump_slice2p("LY  ", &gb->reg_ly.MUWY_LY0p_evn.state,  8);
  d.dump_slice2n("LYC ", &gb->reg_lyc.SYRY_LYC0n.state, 8);
  d.dump_bitp("WUSA_LCD_CLOCK_GATE   ", gb->lcd.WUSA_LCD_CLOCK_GATE.state);
  d.dump_bitp("PAHO_X_8_SYNC         ", gb->lcd.PAHO_X_8_SYNC_odd.state);
  d.dump_bitp("RUJU                  ", gb->lcd.RUJU.state);
  d.dump_bitp("POFY                  ", gb->lcd.POFY.state);
  d.dump_bitp("POME                  ", gb->lcd.POME.state);
  d("\n");

  text_painter.render(view, d.s.c_str(), cursor, 0);
  cursor += 240;
  d.clear();

  //----------------------------------------

  d("\002===== Pix Pipe =====\001\n");
  d("PIX COUNT  0x%02x\n", pack_u8p(8, &gb->pix_count.XEHO_PX0p));
  d("\n");
  d.dump_bitp("XYMU_RENDERINGn       ", gb->ppu_reg.XYMU_RENDERINGn.state);
  d.dump_bitp("PYNU_WIN_MODE_Ap      ", gb->win_reg.PYNU_WIN_MODE_Ap_evn.state);
  d.dump_bitp("PUKU_WIN_HITn         ", gb->win_reg.PUKU_WIN_HITn_evn.state);
  d.dump_bitp("RYDY_WIN_HITp         ", gb->win_reg.RYDY_WIN_HITp_evn.state);
  d.dump_bitp("SOVY_WIN_FIRST_TILE_B ", gb->win_reg.SOVY_WIN_HITp.state);
  d.dump_bitp("NOPA_WIN_MODE_B       ", gb->win_reg.NOPA_WIN_MODE_Bp_odd.state);
  d.dump_bitp("PYCO_WX_MATCH_A       ", gb->win_reg.PYCO_WIN_MATCHp_odd.state);
  d.dump_bitp("NUNU_WX_MATCH_B       ", gb->win_reg.NUNU_WIN_MATCHp_evn.state);
  d.dump_bitp("REJO_WY_MATCH_LATCH   ", gb->win_reg.REJO_WY_MATCH_LATCHp_evn.state);
  d.dump_bitp("SARY_WY_MATCH         ", gb->win_reg.SARY_WY_MATCHp_evn.state);
  d.dump_bitp("RYFA_FETCHn_A         ", gb->win_reg.RYFA_WIN_FETCHn_A_evn.state);
  d.dump_bitp("RENE_FETCHn_B         ", gb->win_reg.RENE_WIN_FETCHn_B_odd.state);

  d.dump_bitp("RYKU_FINE_CNT0        ", gb->fine_scroll.RYKU_FINE_CNT0.state);
  d.dump_bitp("ROGA_FINE_CNT1        ", gb->fine_scroll.ROGA_FINE_CNT1.state);
  d.dump_bitp("RUBU_FINE_CNT2        ", gb->fine_scroll.RUBU_FINE_CNT2.state);
  d.dump_bitp("PUXA_FINE_MATCH_A     ", gb->fine_scroll.PUXA_SCX_FINE_MATCH_A.state);
  d.dump_bitp("NYZE_FINE_MATCH_B     ", gb->fine_scroll.NYZE_SCX_FINE_MATCH_B.state);
  d.dump_bitp("ROXY_FINE_SCROLL_DONEn", gb->fine_scroll.ROXY_FINE_SCROLL_DONEn.state);

  d.dump_bitp("RUPO_LYC_MATCH_LATCHn ", gb->reg_stat.RUPO_STAT_LYC_MATCHn_evn.state);
  d.dump_bitp("VOGA_HBLANKp          ", gb->ppu_reg.VOGA_HBLANKp_xxx.state);
  d("\n");
  d.dump_slice2p("PIX COUNT ", &gb->pix_count.XEHO_PX0p, 8);
  d.dump_slice2p("BG PIPE A ", &gb->pix_pipes.MYDE_BGW_PIPE_A0_evn, 8);
  d.dump_slice2p("BG PIPE B ", &gb->pix_pipes.TOMY_BGW_PIPE_B0_evn, 8);
  d.dump_slice2p("SPR PIPE A", &gb->pix_pipes.NURO_SPR_PIPE_A0_evn, 8);
  d.dump_slice2p("SPR PIPE B", &gb->pix_pipes.NYLU_SPR_PIPE_B0_evn, 8);
  d.dump_slice2p("PAL PIPE  ", &gb->pix_pipes.RUGO_PAL_PIPE_D0_evn, 8);
  d.dump_slice2p("MASK PIPE ", &gb->pix_pipes.VEZO_MASK_PIPE_0_evn, 8);
  d("\n");
  d.dump_slice2n("FF40 LCDC ", &gb->reg_lcdc.VYXE_LCDC_BGENn_h, 8);
  d.dump_slice2n("FF41 STAT ", &gb->reg_stat.ROXE_STAT_HBI_ENn_h, 4);
  d.dump_slice2n("FF47 BGP  ", &gb->reg_bgp. PAVO_BGP_D0n_h, 8);
  d.dump_slice2n("FF48 OBP0 ", &gb->reg_obp0.XUFU_OBP0_D0n_h, 8);
  d.dump_slice2n("FF49 OBP1 ", &gb->reg_obp1.MOXY_OBP1_D0n_h, 8);
  d.dump_slice2n("FF4A WY   ", &gb->reg_wy.  NESO_WY0n_h, 8);
  d.dump_slice2n("FF4B WX   ", &gb->reg_wx.  MYPA_WX0n_h, 8);

  text_painter.render(view, d.s.c_str(), cursor, 0);
  cursor += 224;
  d.clear();

  //----------------------------------------

  d("\002===== Tile Fetch =====\001\n");
  d.dump_bitp   ("POKY_PRELOAD_LATCHp", gb->tile_fetcher.POKY_PRELOAD_LATCHp_odd.state);
  d.dump_bitp   ("LONY_FETCHINGp     ", gb->tile_fetcher.LONY_FETCHINGp_xxx.state);
  d.dump_bitp   ("LOVY_FETCH_DONEp   ", gb->tile_fetcher.LOVY_FETCH_DONEp_evn.state);
  d.dump_bitp   ("NYKA_FETCH_DONEp   ", gb->tile_fetcher.NYKA_FETCH_DONEp_xxx.state);
  d.dump_bitp   ("PORY_FETCH_DONEp   ", gb->tile_fetcher.PORY_FETCH_DONEp_xxx.state);
  d.dump_bitp   ("PYGO_FETCH_DONEp   ", gb->tile_fetcher.PYGO_FETCH_DONEp_odd.state);
  d("\n");
  d.dump_bitp   ("LAXU_BFETCH_S0p    ", gb->tile_fetcher.LAXU_BFETCH_S0p_evn.state);
  d.dump_bitp   ("MESU_BFETCH_S1p    ", gb->tile_fetcher.MESU_BFETCH_S1p_evn.state);
  d.dump_bitp   ("NYVA_BFETCH_S2p    ", gb->tile_fetcher.NYVA_BFETCH_S2p_evn.state);
  d.dump_bitp   ("LYZU_BFETCH_S0p_D1 ", gb->tile_fetcher.LYZU_BFETCH_S0p_D1_odd.state);
  //d.dump_bitn   ("NYXU_BFETCH_RSTp   ", gb->NYXU_BFETCH_RSTn.state);
  d("\n");
  d.dump_slice2p("WIN MAP X ", &gb->win_map_x.WYKA_WIN_X3_evn, 5);
  d.dump_slice2p("WIN Y     ", &gb->win_line_y.VYNO_WIN_Y0_evn, 8);
  d("\n");
  d.dump_slice2n("FF42 SCY  ", &gb->reg_scy.GAVE_SCY0n_h, 8);
  d.dump_slice2n("FF43 SCX  ", &gb->reg_scx.DATY_SCX0n_h, 8);
  d("\n");
  d.dump_slice2n("TEMP A    ", &gb->tile_temp_a.LEGU_TILE_DA0n_odd, 8);
  d.dump_slice2p("TEMP B    ", &gb->tile_temp_b.RAWU_TILE_DB0p_odd, 8);
  d("\n");

  d("\002===== Sprite Fetch =====\001\n");
  d.dump_bitp("TAKA_SFETCH_RUNNINGp", gb->sprite_fetcher.TAKA_SFETCH_RUNNINGp_xxx.state);
  d.dump_bitp("SOBU_SFETCH_REQp    ", gb->sprite_fetcher.SOBU_SFETCH_REQp_odd    .state);
  d.dump_bitp("SUDA_SFETCH_REQp    ", gb->sprite_fetcher.SUDA_SFETCH_REQp_evn    .state);
  d.dump_bitp("TOXE_SFETCH_S0      ", gb->sprite_fetcher.TOXE_SFETCH_S0p_odd     .state);
  d.dump_bitp("TULY_SFETCH_S1      ", gb->sprite_fetcher.TULY_SFETCH_S1p_odd     .state);
  d.dump_bitp("TESE_SFETCH_S2      ", gb->sprite_fetcher.TESE_SFETCH_S2p_odd     .state);
  d.dump_bitp("TYFO_SFETCH_S0_D1   ", gb->sprite_fetcher.TYFO_SFETCH_S0p_D1_evn  .state);
  d.dump_bitp("TOBU_SFETCH_S1_D2   ", gb->sprite_fetcher.TOBU_SFETCH_S1p_D2_odd  .state);
  d.dump_bitp("VONU_SFETCH_S1_D4   ", gb->sprite_fetcher.VONU_SFETCH_S1p_D4_odd  .state);
  d.dump_bitp("SEBA_SFETCH_S1_D5   ", gb->sprite_fetcher.SEBA_SFETCH_S1p_D5_evn  .state);
  //d.dump_bitp("WUTY_SFETCH_DONEp   ", gb->WUTY_SFETCH_DONEp                  .state);
  d("\n");

  d("\002===== Sprite Scan =====\001\n");
  d("SCAN INDEX        : %02d\n", pack_u8p(6, &gb->scan_counter.YFEL_SCAN0_evn));
  d("\n");
  d.dump_bitp("BESU_SCANNINGp   ", gb->sprite_scanner.BESU_SCANNINGp_evn.state);
  d.dump_bitp("CENO_SCANNINGp   ", gb->sprite_scanner.CENO_SCANNINGp_evn.state);
  d.dump_bitp("BYBA_SCAN_DONE_Ap", gb->sprite_scanner.BYBA_SCAN_DONE_Ap_evn.state);
  d.dump_bitp("DOBA_SCAN_DONE_Bp", gb->sprite_scanner.DOBA_SCAN_DONE_Bp_xxx.state);
  d("\n");
  d.dump_bitp("YFEL_SCAN0       ", gb->scan_counter.YFEL_SCAN0_evn.state);
  d.dump_bitp("WEWY_SCAN1       ", gb->scan_counter.WEWY_SCAN1_evn.state);
  d.dump_bitp("GOSO_SCAN2       ", gb->scan_counter.GOSO_SCAN2_evn.state);
  d.dump_bitp("ELYN_SCAN3       ", gb->scan_counter.ELYN_SCAN3_evn.state);
  d.dump_bitp("FAHA_SCAN4       ", gb->scan_counter.FAHA_SCAN4_evn.state);
  d.dump_bitp("FONY_SCAN5       ", gb->scan_counter.FONY_SCAN5_evn.state);
  d("\n");

  const auto& ss = gb->sprite_store;
  d("\002===== Sprite Store =====\001\n");
  d.dump_bitp   ("DEZY_STORE_ENn", gb->sprite_counter.DEZY_COUNT_CLKp_evn.state);
  d("SPRITE INDEX      : %02d\n", pack_u8p(6, &gb->sprite_store.XADU_SPRITE_IDX0p_evn));
  d.dump_slice2p("SPRITE COUNT", &gb->sprite_counter.BESE_SPRITE_COUNT0_evn, 4);
  d("\n");
  d.dump_bitp("XADU_SPRITE_IDX0p", gb->sprite_store.XADU_SPRITE_IDX0p_evn.state);
  d.dump_bitp("XEDY_SPRITE_IDX1p", gb->sprite_store.XEDY_SPRITE_IDX1p_evn.state);
  d.dump_bitp("ZUZE_SPRITE_IDX2p", gb->sprite_store.ZUZE_SPRITE_IDX2p_evn.state);
  d.dump_bitp("XOBE_SPRITE_IDX3p", gb->sprite_store.XOBE_SPRITE_IDX3p_evn.state);
  d.dump_bitp("YDUF_SPRITE_IDX4p", gb->sprite_store.YDUF_SPRITE_IDX4p_evn.state);
  d.dump_bitp("XECU_SPRITE_IDX5p", gb->sprite_store.XECU_SPRITE_IDX5p_evn.state);

  d("STORE0 R%d I%02d L%02d X%03d\n", ss.EBOJ_STORE0_RSTp_evn.qp_old(), pack_u8n(6, &ss.YGUS_STORE0_I0n_odd), pack_u8n(4, &ss.GYHO_STORE0_L0n_odd), pack_u8n(8, &ss.XEPE_STORE0_X0p_odd));
  d("STORE1 R%d I%02d L%02d X%03d\n", ss.CEDY_STORE1_RSTp_evn.qp_old(), pack_u8n(6, &ss.CADU_STORE1_I0n_odd), pack_u8n(4, &ss.AMES_STORE1_L0n_odd), pack_u8n(8, &ss.DANY_STORE1_X0p_odd));
  d("STORE2 R%d I%02d L%02d X%03d\n", ss.EGAV_STORE2_RSTp_evn.qp_old(), pack_u8n(6, &ss.BUHE_STORE2_I0n_odd), pack_u8n(4, &ss.YLOV_STORE2_L0n_odd), pack_u8n(8, &ss.FOKA_STORE2_X0p_odd));
  d("STORE3 R%d I%02d L%02d X%03d\n", ss.GOTA_STORE3_RSTp_evn.qp_old(), pack_u8n(6, &ss.DEVY_STORE3_I0n_odd), pack_u8n(4, &ss.ZURO_STORE3_L0n_odd), pack_u8n(8, &ss.XOLY_STORE3_X0p_odd));
  d("STORE4 R%d I%02d L%02d X%03d\n", ss.XUDY_STORE4_RSTp_evn.qp_old(), pack_u8n(6, &ss.XAVE_STORE4_I0n_odd), pack_u8n(4, &ss.CAPO_STORE4_L0n_odd), pack_u8n(8, &ss.WEDU_STORE4_X0p_odd));
  d("STORE5 R%d I%02d L%02d X%03d\n", ss.WAFY_STORE5_RSTp_evn.qp_old(), pack_u8n(6, &ss.EKOP_STORE5_I0n_odd), pack_u8n(4, &ss.ACEP_STORE5_L0n_odd), pack_u8n(8, &ss.FUSA_STORE5_X0p_odd));
  d("STORE6 R%d I%02d L%02d X%03d\n", ss.WOMY_STORE6_RSTp_evn.qp_old(), pack_u8n(6, &ss.GABO_STORE6_I0n_odd), pack_u8n(4, &ss.ZUMY_STORE6_L0n_odd), pack_u8n(8, &ss.YCOL_STORE6_X0p_odd));
  d("STORE7 R%d I%02d L%02d X%03d\n", ss.WAPO_STORE7_RSTp_evn.qp_old(), pack_u8n(6, &ss.GULE_STORE7_I0n_odd), pack_u8n(4, &ss.XYNA_STORE7_L0n_odd), pack_u8n(8, &ss.ERAZ_STORE7_X0p_odd));
  d("STORE8 R%d I%02d L%02d X%03d\n", ss.EXUQ_STORE8_RSTp_evn.qp_old(), pack_u8n(6, &ss.AXUV_STORE8_I0n_odd), pack_u8n(4, &ss.AZAP_STORE8_L0n_odd), pack_u8n(8, &ss.EZUF_STORE8_X0p_odd));
  d("STORE9 R%d I%02d L%02d X%03d\n", ss.FONO_STORE9_RSTp_evn.qp_old(), pack_u8n(6, &ss.YBER_STORE9_I0n_odd), pack_u8n(4, &ss.CANA_STORE9_L0n_odd), pack_u8n(8, &ss.XUVY_STORE9_X0p_odd));
  d("\n");

  text_painter.render(view, d.s.c_str(), cursor, 0);
  cursor += 224 - 32;
  d.clear();

  //----------------------------------------

  d("\002===== Disasm =====\001\n");
  {
    uint16_t pc = gb->cpu.op_addr;
    const uint8_t* code = nullptr;
    uint16_t code_size = 0;
    uint16_t code_base = 0;

    if (!gb->bootrom.BOOT_BITn_h.qp_old()) {
      code = gb_thread.boot.data();
      code_size = 256;
      code_base = ADDR_BOOT_ROM_BEGIN;
    }
    else if (pc >= 0x0000 && pc <= 0x7FFF) {
      // FIXME needs to account for mbc1 mem mapping
      code = gb_thread.cart.data();
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
  text_painter.render(view, d.s.c_str(), cursor, 0);
  d.clear();

  //----------------------------------------

  //update_texture_u32(trace_tex, 912, 154, trace);
  //blitter.blit(view, trace_tex, 0, 0, 912, 154);

  // Draw flat memory view
  /*
  {
    //printf("rom_buf.data() %p\n", rom_buf.data());
    //printf("gb->rom_buf %p\n", gb->rom_buf);

    update_texture_u8(ram_tex, 0x00, 0x00, 256, 128, gb->rom_buf);
    update_texture_u8(ram_tex, 0x00, 0x80, 256,  32, gb->vid_ram);
    update_texture_u8(ram_tex, 0x00, 0xA0, 256,  32, gb->cart_ram);
    update_texture_u8(ram_tex, 0x00, 0xC0, 256,  32, gb->ext_ram);
    update_texture_u8(ram_tex, 0x00, 0xFE, 256,   1, gb->oam_ram);
    update_texture_u8(ram_tex, 0x80, 0xFF, 128,   1, gb->zero_ram);
    blitter.blit_mono(view, ram_tex, 256, 256,
                      0, 0, 256, 256,
                      960 + 96 - 64, 640 + 96, 256, 256);
  }
  */

  // Draw screen and vid ram contents

  int gb_x = 1216;
  int gb_y = 32;

  if (has_golden && show_diff) {
    gb_blitter.blit_diff(view, gb_x, gb_y,  2, framebuffer, golden_u8);
  } else if (show_golden) {
    gb_blitter.blit_screen(view, gb_x, gb_y,  2, golden_u8);
  } else {
    gb_blitter.blit_screen(view, gb_x, gb_y,  2, framebuffer);
  }

  gb_blitter.blit_tiles (view, 1632, 32,  1, vid_ram);
  gb_blitter.blit_map   (view, 1344, 448, 1, vid_ram, 0, 0);
  gb_blitter.blit_map   (view, 1632, 448, 1, vid_ram, 0, 1);
  gb_blitter.blit_map   (view, 1344, 736, 1, vid_ram, 1, 0);
  gb_blitter.blit_map   (view, 1632, 736, 1, vid_ram, 1, 1);

  // Draw screen overlay
  {
    int fb_x = gb->gb_screen_x;
    int fb_y = gb->gb_screen_y;
    if (fb_y >= 0 && fb_y < 144 && fb_x >= 0 && fb_x < 160) {
      memset(overlay, 0, sizeof(overlay));

      for (int x = 0; x < fb_x; x++) {
        uint8_t p0 = gb->lcd.lcd_pipe_lo[159 - fb_x + x + 1].qp_old();
        uint8_t p1 = gb->lcd.lcd_pipe_hi[159 - fb_x + x + 1].qp_old();

        int r = (3 - (p0 + p1 * 2)) * 30 + 50;
        int g = (3 - (p0 + p1 * 2)) * 30 + 50;
        int b = (3 - (p0 + p1 * 2)) * 30 + 30;

        overlay[x + fb_y * 160] = 0xFF000000 | (b << 16) | (g << 8) | (r << 0);
      }
      {
        uint8_t p0 = gb->lcd.lcd_pix_lo.qp_old();
        uint8_t p1 = gb->lcd.lcd_pix_hi.qp_old();

        int c = (3 - (p0 + p1 * 2)) * 85;

        overlay[fb_x + fb_y * 160] = 0xFF000000 | (c << 16) | (c << 8) | (c << 0);
      }

      update_texture_u32(overlay_tex, 160, 144, overlay);
      blitter.blit(view, overlay_tex, gb_x, gb_y, 160 * 2, 144 * 2);
    }
  }

  // Status bar under screen

  //double phases_per_frame = 114 * 154 * 60 * 8;
  //double sim_ratio = sim_rate / phases_per_frame;
  double sim_ratio = 0.0;
  double sim_time_smooth = 0.0;

  d("%s %s Sim clock %8.3f %s %s\n",
    runmode_names[runmode],
    stepmode_names[stepmode],
    double(phase_total) / (4194304.0 * 2),
    phase_names[phase_total & 7],
    show_golden ? "GOLDEN IMAGE " : "");
  d("Sim time %f, sim ratio %f, frame time %f\n", sim_time_smooth, sim_ratio, frame_time_smooth);
  text_painter.render(view, d.s, gb_x, gb_y + 144 * 2);
  d.clear();

  // Probe dump

  gb->probes.dump(d, draw_passes);
  text_painter.render(view, d.s, 640 - 64, 640 + 128);
  d.clear();

  frame_count++;
  gb_thread.resume();
}

//------------------------------------------------------------------------------

void GateBoyApp::load_golden(const char* filename) {
  SDL_Surface* golden_surface = SDL_LoadBMP(filename);

  if (!golden_surface) {
    printf("Failed to load golden %s\n", filename);
    memset(golden_u8, 0, 160 * 144);
    return;
  }

  if (golden_surface && golden_surface->format->format == SDL_PIXELFORMAT_INDEX8) {
    printf("Loaded i8 golden %s\n", filename);
    uint8_t* src = (uint8_t*)golden_surface->pixels;
    uint32_t* pal = (uint32_t*)golden_surface->format->palette->colors;
    for (int y = 0; y < 144; y++) {
      for (int x = 0; x < 160; x++) {
        uint8_t a = pal[src[x + y * 160]] & 0xFF;

        if (a < 40) a = 3;
        else if (a < 128) a = 2;
        else if (a < 210) a = 1;
        else a = 0;

        golden_u8[x + y * 160] = a;
      }
    }
  }

  else if (golden_surface && golden_surface->format->format == SDL_PIXELFORMAT_BGR24) {
    printf("Loaded bgr24 golden %s\n", filename);
    uint8_t* src = (uint8_t*)golden_surface->pixels;
    (void)src;
    for (int y = 0; y < 144; y++) {
      for (int x = 0; x < 160; x++) {
        uint8_t a = src[x * 3 + y * golden_surface->pitch];

        if (a < 40) a = 3;
        else if (a < 128) a = 2;
        else if (a < 210) a = 1;
        else a = 0;

        golden_u8[x + y * 160] = a;
      }
    }
  }

  has_golden = true;
  show_diff = true;
}

//-----------------------------------------------------------------------------
