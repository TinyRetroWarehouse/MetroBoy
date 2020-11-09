#include "GateBoyApp/GateBoyApp.h"

#include "CoreLib/Constants.h"
#include "CoreLib/Debug.h" // for StringDumper

#include "AppLib/AppHost.h"
#include "AppLib/GLBase.h"

#define SDL_MAIN_HANDLED
#ifdef _MSC_VER
#include "SDL/include/SDL.h"
#else
#include <SDL2/SDL.h>
#endif

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  App* app = new GateBoyApp();
  AppHost* app_host = new AppHost(app);
  int ret = app_host->app_main(argc, argv);
  delete app;
  return ret;
}

//-----------------------------------------------------------------------------

void GateBoyApp::app_init() {
  printf("GateBoyApp::app_init()\n");

  grid_painter.init();
  text_painter.init();
  dump_painter.init();
  gb_blitter.init();
  blitter.init();

  trace_tex = create_texture_u32(912, 154);
  ram_tex = create_texture_u8(256, 256);
  overlay_tex = create_texture_u32(160, 144);
  keyboard_state = SDL_GetKeyboardState(nullptr);


  //gb_thread.init();

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

  //load_flat_dump("roms/LinksAwakening_dog.dump");
  //gb_thread.gb->sys_cpu_en = false;

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

  //load_rom("microtests/build/dmg/oam_read_l0_d.gb");

  gb_thread.start();
}

//-----------------------------------------------------------------------------

void GateBoyApp::app_close() {
  gb_thread.stop();
}

//------------------------------------------------------------------------------

void GateBoyApp::load_raw_dump() {
  printf("Loading raw dump from %s\n", "gateboy.raw.dump");
  gb_thread.gb.reset_states();
  gb_thread.gb->load_dump("gateboy.raw.dump");
  gb_thread.gb->set_rom(gb_thread.rom_buf.data(), gb_thread.rom_buf.size());
}

void GateBoyApp::save_raw_dump() {
  printf("Saving raw dump to %s\n", "gateboy.raw.dump");
  gb_thread.gb->save_dump("gateboy.raw.dump");
}

//------------------------------------------------------------------------------
// Load a standard GB rom

void GateBoyApp::load_rom(const char* filename) {
  printf("Loading %s\n", filename);
  gb_thread.rom_buf = load_blob(filename);

  gb_thread.gb.reset_states();
  gb_thread.gb->reset_cart();
  gb_thread.gb->set_rom(gb_thread.rom_buf.data(), gb_thread.rom_buf.size());
  gb_thread.gb->phase_total = 0;
  gb_thread.gb->pass_count = 0;
  gb_thread.gb->pass_total = 0;

  printf("Loaded %zd bytes from rom %s\n", gb_thread.rom_buf.size(), filename);
}


//-----------------------------------------------------------------------------
// Load a flat (just raw contents of memory addresses, no individual regs) dump
// and copy it into the various regs and memory chunks.

void GateBoyApp::load_flat_dump(const char* filename) {
  gb_thread.rom_buf = load_blob(filename);

  gb_thread.gb.reset_states();
  gb_thread.gb->reset_cart();
  gb_thread.gb->set_rom(gb_thread.rom_buf.data(), gb_thread.rom_buf.size());

  memcpy(gb_thread.gb->vid_ram,  gb_thread.rom_buf.data() + 0x8000, 8192);
  memcpy(gb_thread.gb->cart_ram, gb_thread.rom_buf.data() + 0xA000, 8192);
  memcpy(gb_thread.gb->ext_ram,  gb_thread.rom_buf.data() + 0xC000, 8192);
  memcpy(gb_thread.gb->oam_ram,  gb_thread.rom_buf.data() + 0xFE00, 256);
  memcpy(gb_thread.gb->zero_ram, gb_thread.rom_buf.data() + 0xFF80, 128);

  gb_thread.gb->dbg_write(ADDR_BGP,  gb_thread.rom_buf[ADDR_BGP]);
  gb_thread.gb->dbg_write(ADDR_OBP0, gb_thread.rom_buf[ADDR_OBP0]);
  gb_thread.gb->dbg_write(ADDR_OBP1, gb_thread.rom_buf[ADDR_OBP1]);
  gb_thread.gb->dbg_write(ADDR_SCY,  gb_thread.rom_buf[ADDR_SCY]);
  gb_thread.gb->dbg_write(ADDR_SCX,  gb_thread.rom_buf[ADDR_SCX]);
  gb_thread.gb->dbg_write(ADDR_WY,   gb_thread.rom_buf[ADDR_WY]);
  gb_thread.gb->dbg_write(ADDR_WX,   gb_thread.rom_buf[ADDR_WX]);

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

  gb_thread.gb->dbg_write(ADDR_LCDC, gb_thread.rom_buf[ADDR_LCDC]);
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
    case SDLK_r:    gb_thread.reset();          break;
    case SDLK_d:    show_diff   = !show_diff;   break;
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

void dump_cpu_bus(Dumper& d, const CpuBus& cpu_bus) {
  d("\002===== CPU Bus =====\001\n");
  d("PIN_CPU_BOOTp     %c\n", cpu_bus.PIN_CPU_BOOTp.c());
  d("PIN_CPU_ADDR_HIp  %c\n", cpu_bus.PIN_CPU_ADDR_HIp.c());
  d("PIN_CPU_RDp       %c\n", cpu_bus.PIN_CPU_RDp.c());
  d("PIN_CPU_WRp       %c\n", cpu_bus.PIN_CPU_WRp.c());
  d("PIN_CPU_ADDR_EXT  %c\n", cpu_bus.PIN_CPU_EXT_BUSp.c());
  d("PIN_CPU_LATCH_EXT %c\n", cpu_bus.PIN_CPU_LATCH_EXT.c());

  /*
  d("BUS ADDR  0x%04x  %c%c%c%c%c%c%c%c:%c%c%c%c%c%c%c%c\n",
    get_bus_addr(),
    BUS_CPU_A15.c(), BUS_CPU_A14.c(), BUS_CPU_A13.c(), BUS_CPU_A12.c(),
    BUS_CPU_A11.c(), BUS_CPU_A10.c(), BUS_CPU_A09.c(), BUS_CPU_A08.c(),
    BUS_CPU_A07.c(), BUS_CPU_A06.c(), BUS_CPU_A05.c(), BUS_CPU_A04.c(),
    BUS_CPU_A03.c(), BUS_CPU_A02.c(), BUS_CPU_A01.c(), BUS_CPU_A00.c());

  d("BUS DATA  0x%02x   %c%c%c%c%c%c%c%c\n",
    get_bus_data(),
    BUS_CPU_D7p.c(), BUS_CPU_D6p.c(), BUS_CPU_D5p.c(), BUS_CPU_D4p.c(),
    BUS_CPU_D3p.c(), BUS_CPU_D2p.c(), BUS_CPU_D1p.c(), BUS_CPU_D0p.c());
  */

  d("\n");

  d("PIN STARTp        %d\n", cpu_bus.PIN_CPU_STARTp.qp());
  d("PIN SYS_RSTp      %d\n", cpu_bus.PIN_CPU_SYS_RSTp.qp());
  d("PIN EXT_RST       %d\n", cpu_bus.PIN_CPU_EXT_RST.qp());
  d("PIN UNOR_DBG      %d\n", cpu_bus.PIN_CPU_UNOR_DBG.qp());
  d("PIN UMUT_DBG      %d\n", cpu_bus.PIN_CPU_UMUT_DBG.qp());
  d("PIN EXT_CLKGOOD   %d\n", cpu_bus.PIN_CPU_EXT_CLKGOOD.qp());
  d("PIN BOWA_xBCDEFGH %d\n", cpu_bus.PIN_CPU_BOWA_Axxxxxxx.qp());
  d("PIN BEDO_Axxxxxxx %d\n", cpu_bus.PIN_CPU_BEDO_xBCDEFGH.qp());
  d("PIN BEKO_ABCDxxxx %d\n", cpu_bus.PIN_CPU_BEKO_ABCDxxxx.qp());
  d("PIN BUDE_xxxxEFGH %d\n", cpu_bus.PIN_CPU_BUDE_xxxxEFGH.qp());
  d("PIN BOLO_ABCDEFxx %d\n", cpu_bus.PIN_CPU_BOLO_ABCDEFxx.qp());
  d("PIN BUKE_AxxxxxGH %d\n", cpu_bus.PIN_CPU_BUKE_AxxxxxGH.qp());
  d("PIN BOMA_Axxxxxxx %d\n", cpu_bus.PIN_CPU_BOMA_xBCDEFGH.qp());
  d("PIN BOGA_xBCDEFGH %d\n", cpu_bus.PIN_CPU_BOGA_Axxxxxxx.qp());
  d("\n");
}

void dump_ext_bus(Dumper& d, const ExtBus& ext_bus) {
  d("\002===== Ext Bus =====\001\n");
  d("PIN CLK    : %c\n", ext_bus.PIN_EXT_CLK.c());
  d("PIN RDn    : %c\n", ext_bus.PIN_EXT_RDn.c());
  d("PIN WRn    : %c\n", ext_bus.PIN_EXT_WRn.c());
  d("PIN CSn    : %c\n", ext_bus.PIN_EXT_CSn.c());
  d("\n");

  d("PIN ADDR   : %c%c%c%c%c%c%c%c:%c%c%c%c%c%c%c%c\n",
    ext_bus.PIN_EXT_A15p.c(), ext_bus.PIN_EXT_A14p.c(), ext_bus.PIN_EXT_A13p.c(), ext_bus.PIN_EXT_A12p.c(),
    ext_bus.PIN_EXT_A11p.c(), ext_bus.PIN_EXT_A10p.c(), ext_bus.PIN_EXT_A09p.c(), ext_bus.PIN_EXT_A08p.c(),
    ext_bus.PIN_EXT_A07p.c(), ext_bus.PIN_EXT_A06p.c(), ext_bus.PIN_EXT_A05p.c(), ext_bus.PIN_EXT_A04p.c(),
    ext_bus.PIN_EXT_A03p.c(), ext_bus.PIN_EXT_A02p.c(), ext_bus.PIN_EXT_A01p.c(), ext_bus.PIN_EXT_A00p.c());
  d("PIN DATA   : %c%c%c%c%c%c%c%c\n",
    ext_bus.PIN_EXT_D07p.c(), ext_bus.PIN_EXT_D06p.c(), ext_bus.PIN_EXT_D05p.c(), ext_bus.PIN_EXT_D04p.c(),
    ext_bus.PIN_EXT_D03p.c(), ext_bus.PIN_EXT_D02p.c(), ext_bus.PIN_EXT_D01p.c(), ext_bus.PIN_EXT_D00p.c());
  d("\n");

  d("ADDR LATCH : _%c%c%c%c%c%c%c:%c%c%c%c%c%c%c%c\n",
    ext_bus.NYRE_EXT_ADDR_LATCH_14p.c(), ext_bus.LONU_EXT_ADDR_LATCH_13p.c(), ext_bus.LOBU_EXT_ADDR_LATCH_12p.c(), ext_bus.LUMY_EXT_ADDR_LATCH_11p.c(),
    ext_bus.PATE_EXT_ADDR_LATCH_10p.c(), ext_bus.LYSA_EXT_ADDR_LATCH_09p.c(), ext_bus.LUNO_EXT_ADDR_LATCH_08p.c(), ext_bus.ARYM_EXT_ADDR_LATCH_07p.c(),
    ext_bus.AROS_EXT_ADDR_LATCH_06p.c(), ext_bus.ATEV_EXT_ADDR_LATCH_05p.c(), ext_bus.AVYS_EXT_ADDR_LATCH_04p.c(), ext_bus.ARET_EXT_ADDR_LATCH_03p.c(),
    ext_bus.ALYR_EXT_ADDR_LATCH_02p.c(), ext_bus.APUR_EXT_ADDR_LATCH_01p.c(), ext_bus.ALOR_EXT_ADDR_LATCH_00p.c());
  d("DATA LATCH : %c%c%c%c%c%c%c%c\n",
    ext_bus.SAZY_EXT_DATA_LATCH_D7n.c(), ext_bus.RUPA_EXT_DATA_LATCH_D6n.c(), ext_bus.SAGO_EXT_DATA_LATCH_D5n.c(), ext_bus.SODY_EXT_DATA_LATCH_D4n.c(),
    ext_bus.SELO_EXT_DATA_LATCH_D3n.c(), ext_bus.RAXY_EXT_DATA_LATCH_D2n.c(), ext_bus.RONY_EXT_DATA_LATCH_D1n.c(), ext_bus.SOMA_EXT_DATA_LATCH_D0n.c());
  d("\n");
}

void dump_vram_bus(Dumper& d, const VramBus& vram_bus) {
  d("\002===== VRAM Bus =====\001\n");

  uint16_t bus_addr = (uint16_t)pack_p(!vram_bus.BUS_VRAM_A00n.qp(), !vram_bus.BUS_VRAM_A01n.qp(), !vram_bus.BUS_VRAM_A02n.qp(), !vram_bus.BUS_VRAM_A03n.qp(),
                                       !vram_bus.BUS_VRAM_A04n.qp(), !vram_bus.BUS_VRAM_A05n.qp(), !vram_bus.BUS_VRAM_A06n.qp(), !vram_bus.BUS_VRAM_A07n.qp(),
                                       !vram_bus.BUS_VRAM_A08n.qp(), !vram_bus.BUS_VRAM_A09n.qp(), !vram_bus.BUS_VRAM_A10n.qp(), !vram_bus.BUS_VRAM_A11n.qp(),
                                       !vram_bus.BUS_VRAM_A12n.qp(), 0, 0, 0);

  uint8_t bus_data = (uint8_t)pack_p(vram_bus.BUS_VRAM_D0p.qp(), vram_bus.BUS_VRAM_D1p.qp(), vram_bus.BUS_VRAM_D2p.qp(), vram_bus.BUS_VRAM_D3p.qp(),
                                     vram_bus.BUS_VRAM_D4p.qp(), vram_bus.BUS_VRAM_D5p.qp(), vram_bus.BUS_VRAM_D6p.qp(), vram_bus.BUS_VRAM_D7p.qp());

  d("VRAM BUS ADDR : %04x %c%c%c%c%c:%c%c%c%c%c%c%c%c\n",
    bus_addr | 0x8000,
    vram_bus.BUS_VRAM_A12n.c(), vram_bus.BUS_VRAM_A11n.c(), vram_bus.BUS_VRAM_A10n.c(), vram_bus.BUS_VRAM_A09n.c(),
    vram_bus.BUS_VRAM_A08n.c(), vram_bus.BUS_VRAM_A07n.c(), vram_bus.BUS_VRAM_A06n.c(), vram_bus.BUS_VRAM_A05n.c(),
    vram_bus.BUS_VRAM_A04n.c(), vram_bus.BUS_VRAM_A03n.c(), vram_bus.BUS_VRAM_A02n.c(), vram_bus.BUS_VRAM_A01n.c(),
    vram_bus.BUS_VRAM_A00n.c());
  d("VRAM BUS DATA : %02x %c%c%c%c%c%c%c%c\n",
    bus_data,
    vram_bus.BUS_VRAM_D7p.c(), vram_bus.BUS_VRAM_D6p.c(), vram_bus.BUS_VRAM_D5p.c(), vram_bus.BUS_VRAM_D4p.c(),
    vram_bus.BUS_VRAM_D3p.c(), vram_bus.BUS_VRAM_D2p.c(), vram_bus.BUS_VRAM_D1p.c(), vram_bus.BUS_VRAM_D0p.c());
  d("\n");

  d("VRAM PIN MCSn : %c\n", vram_bus.PIN_VRAM_CSn.c());
  d("VRAM PIN MOEn : %c\n", vram_bus.PIN_VRAM_OEn.c());
  d("VRAM PIN MWRn : %c\n", vram_bus.PIN_VRAM_WRn.c());

  uint16_t pin_addr = (uint16_t)pack_p(vram_bus.PIN_VRAM_A00p.qp(), vram_bus.PIN_VRAM_A01p.qp(), vram_bus.PIN_VRAM_A02p.qp(), vram_bus.PIN_VRAM_A03p.qp(),
                                       vram_bus.PIN_VRAM_A04p.qp(), vram_bus.PIN_VRAM_A05p.qp(), vram_bus.PIN_VRAM_A06p.qp(), vram_bus.PIN_VRAM_A07p.qp(),
                                       vram_bus.PIN_VRAM_A08p.qp(), vram_bus.PIN_VRAM_A09p.qp(), vram_bus.PIN_VRAM_A10p.qp(), vram_bus.PIN_VRAM_A11p.qp(),
                                       vram_bus.PIN_VRAM_A12p.qp(), 0, 0, 0);
  uint8_t pin_data = (uint8_t)pack_p(vram_bus.PIN_VRAM_D07p.c(), vram_bus.PIN_VRAM_D06p.c(), vram_bus.PIN_VRAM_D05p.c(), vram_bus.PIN_VRAM_D04p.c(),
                                     vram_bus.PIN_VRAM_D03p.c(), vram_bus.PIN_VRAM_D02p.c(), vram_bus.PIN_VRAM_D01p.c(), vram_bus.PIN_VRAM_D00p.c());


  d("VRAM PIN ADDR : 0x%04x\n", pin_addr | 0x8000);
  d("VRAM PIN DATA : %02x %c%c%c%c%c%c%c%c\n",
    pin_data,
    vram_bus.PIN_VRAM_D07p.c(), vram_bus.PIN_VRAM_D06p.c(), vram_bus.PIN_VRAM_D05p.c(), vram_bus.PIN_VRAM_D04p.c(),
    vram_bus.PIN_VRAM_D03p.c(), vram_bus.PIN_VRAM_D02p.c(), vram_bus.PIN_VRAM_D01p.c(), vram_bus.PIN_VRAM_D00p.c());
  d("\n");

  int TILE_DA = pack_p(vram_bus.LEGU_TILE_DA0n.qn07(), vram_bus.NUDU_TILE_DA1n.qn07(), vram_bus.MUKU_TILE_DA2n.qn07(), vram_bus.LUZO_TILE_DA3n.qn07(),
                       vram_bus.MEGU_TILE_DA4n.qn07(), vram_bus.MYJY_TILE_DA5n.qn07(), vram_bus.NASA_TILE_DA6n.qn07(), vram_bus.NEFO_TILE_DA7n.qn07());
  int TILE_DB = pack_p(vram_bus.RAWU_TILE_DB0p.q11p(), vram_bus.POZO_TILE_DB1p.q11p(), vram_bus.PYZO_TILE_DB2p.q11p(), vram_bus.POXA_TILE_DB3p.q11p(),
                       vram_bus.PULO_TILE_DB4p.q11p(), vram_bus.POJU_TILE_DB5p.q11p(), vram_bus.POWY_TILE_DB6p.q11p(), vram_bus.PYJU_TILE_DB7p.q11p());
  int SPRITE_DA = pack_p(vram_bus.PEFO_SPRITE_DB0n.qn07(), vram_bus.ROKA_SPRITE_DB1n.qn07(), vram_bus.MYTU_SPRITE_DB2n.qn07(), vram_bus.RAMU_SPRITE_DB3n.qn07(),
                         vram_bus.SELE_SPRITE_DB4n.qn07(), vram_bus.SUTO_SPRITE_DB5n.qn07(), vram_bus.RAMA_SPRITE_DB6n.qn07(), vram_bus.RYDU_SPRITE_DB7n.qn07());
  int SPRITE_DB = pack_p(vram_bus.REWO_SPRITE_DA0n.qn07(), vram_bus.PEBA_SPRITE_DA1n.qn07(), vram_bus.MOFO_SPRITE_DA2n.qn07(), vram_bus.PUDU_SPRITE_DA3n.qn07(),
                         vram_bus.SAJA_SPRITE_DA4n.qn07(), vram_bus.SUNY_SPRITE_DA5n.qn07(), vram_bus.SEMO_SPRITE_DA6n.qn07(), vram_bus.SEGA_SPRITE_DA7n.qn07());

  d("TILE_DA       : 0x%02x\n", TILE_DA);
  d("TILE_DB       : 0x%02x\n", TILE_DB);
  d("SPRITE_DA     : 0x%02x\n", SPRITE_DA);
  d("SPRITE_DB     : 0x%02x\n", SPRITE_DB);
  d("\n");
}

void dump_oam_bus(Dumper& d, const OamBus& oam_bus) {
  d("\002===== OAM Bus =====\001\n");

  d("PIN_OAM_CLK    %c\n", oam_bus.PIN_OAM_CLK.c());
  d("PIN_OAM_OE     %c\n", oam_bus.PIN_OAM_OEn.c());
  d("PIN_OAM_WR_A   %c\n", oam_bus.PIN_OAM_WR_A.c());
  d("PIN_OAM_WR_B   %c\n", oam_bus.PIN_OAM_WR_B.c());

  int oam_tri_addr = pack_p(oam_bus.BUS_OAM_A0n.qn(), oam_bus.BUS_OAM_A1n.qn(), oam_bus.BUS_OAM_A2n.qn(), oam_bus.BUS_OAM_A3n.qn(),
                            oam_bus.BUS_OAM_A4n.qn(), oam_bus.BUS_OAM_A5n.qn(), oam_bus.BUS_OAM_A6n.qn(), oam_bus.BUS_OAM_A7n.qn());

  d("OAM TRI ADDR   %03d %02x %c%c%c%c%c%c%c%c\n",
    oam_tri_addr,
    oam_tri_addr,
    oam_bus.BUS_OAM_A7n.cn(), oam_bus.BUS_OAM_A6n.cn(), oam_bus.BUS_OAM_A5n.cn(), oam_bus.BUS_OAM_A4n.cn(),
    oam_bus.BUS_OAM_A3n.cn(), oam_bus.BUS_OAM_A2n.cn(), oam_bus.BUS_OAM_A1n.cn(), oam_bus.BUS_OAM_A0n.cn());

  int oam_latch_data_a = pack_p(oam_bus.YDYV_OAM_LATCH_DA0n.c(), oam_bus.YCEB_OAM_LATCH_DA1n.c(), oam_bus.ZUCA_OAM_LATCH_DA2n.c(), oam_bus.WONE_OAM_LATCH_DA3n.c(),
                                oam_bus.ZAXE_OAM_LATCH_DA4n.c(), oam_bus.XAFU_OAM_LATCH_DA5n.c(), oam_bus.YSES_OAM_LATCH_DA6n.c(), oam_bus.ZECA_OAM_LATCH_DA7n.c());

  int oam_latch_data_b = pack_p(oam_bus.XYKY_OAM_LATCH_DB0n.c(), oam_bus.YRUM_OAM_LATCH_DB1n.c(), oam_bus.YSEX_OAM_LATCH_DB2n.c(), oam_bus.YVEL_OAM_LATCH_DB3n.c(),
                                oam_bus.WYNO_OAM_LATCH_DB4n.c(), oam_bus.CYRA_OAM_LATCH_DB5n.c(), oam_bus.ZUVE_OAM_LATCH_DB6n.c(), oam_bus.ECED_OAM_LATCH_DB7n.c());

  /*
  d("OAM BUS ADDR   %03d %02x -%c%c%c%c%c%c%c\n",
    get_oam_pin_addr(),
    get_oam_pin_addr(),
    BUS_OAM_A7n.cn(), BUS_OAM_A6n.cn(), BUS_OAM_A5n.cn(), BUS_OAM_A4n.cn(),
    BUS_OAM_A3n.cn(), BUS_OAM_A2n.cn(), BUS_OAM_A1n.cn());

  d("OAM BUS DATA A %03d %02x %c%c%c%c%c%c%c%c\n",
    get_oam_pin_data_a(),
    get_oam_pin_data_a(),
    BUS_OAM_DA7n.c(), BUS_OAM_DA6n.c(), BUS_OAM_DA5n.c(), BUS_OAM_DA4n.c(),
    BUS_OAM_DA3n.c(), BUS_OAM_DA2n.c(), BUS_OAM_DA1n.c(), BUS_OAM_DA0n.c());

  d("OAM BUS DATA B %03d %02x %c%c%c%c%c%c%c%c\n",
    get_oam_pin_data_b(),
    get_oam_pin_data_b(),
    BUS_OAM_DB7n.c(), BUS_OAM_DB6n.c(), BUS_OAM_DB5n.c(), BUS_OAM_DB4n.c(),
    BUS_OAM_DB3n.c(), BUS_OAM_DB2n.c(), BUS_OAM_DB1n.c(), BUS_OAM_DB0n.c());
  */

  d("MAKA_HOLD_MEMp   %c\n", oam_bus.MAKA_HOLD_MEMp.c());
  d("WUJE_CPU_OAM_WRn %c\n", oam_bus.WUJE_CPU_OAM_WRn.c());

  d("OAM LATCH A    %03d %02x %c%c%c%c%c%c%c%c\n",
    oam_latch_data_a,
    oam_latch_data_a,
    oam_bus.ZECA_OAM_LATCH_DA7n.c(), oam_bus.YSES_OAM_LATCH_DA6n.c(), oam_bus.XAFU_OAM_LATCH_DA5n.c(), oam_bus.ZAXE_OAM_LATCH_DA4n.c(),
    oam_bus.WONE_OAM_LATCH_DA3n.c(), oam_bus.ZUCA_OAM_LATCH_DA2n.c(), oam_bus.YCEB_OAM_LATCH_DA1n.c(), oam_bus.YDYV_OAM_LATCH_DA0n.c());

  d("OAM LATCH B    %03d %02x %c%c%c%c%c%c%c%c\n",
    oam_latch_data_b,
    oam_latch_data_b,
    oam_bus.ECED_OAM_LATCH_DB7n.c(), oam_bus.ZUVE_OAM_LATCH_DB6n.c(), oam_bus.CYRA_OAM_LATCH_DB5n.c(), oam_bus.WYNO_OAM_LATCH_DB4n.c(),
    oam_bus.YVEL_OAM_LATCH_DB3n.c(), oam_bus.YSEX_OAM_LATCH_DB2n.c(), oam_bus.YRUM_OAM_LATCH_DB1n.c(), oam_bus.XYKY_OAM_LATCH_DB0n.c());

  int oam_temp_a = pack_p(oam_bus.XUSO_OAM_DA0p.qp08(), oam_bus.XEGU_OAM_DA1p.qp08(), oam_bus.YJEX_OAM_DA2p.qp08(), oam_bus.XYJU_OAM_DA3p.qp08(),
                          oam_bus.YBOG_OAM_DA4p.qp08(), oam_bus.WYSO_OAM_DA5p.qp08(), oam_bus.XOTE_OAM_DA6p.qp08(), oam_bus.YZAB_OAM_DA7p.qp08());

  int oam_temp_b = pack_p(oam_bus.YLOR_OAM_DB0p.qp08(), oam_bus.ZYTY_OAM_DB1p.qp08(), oam_bus.ZYVE_OAM_DB2p.qp08(), oam_bus.ZEZY_OAM_DB3p.qp08(),
                          oam_bus.GOMO_OAM_DB4p.qp08(), oam_bus.BAXO_OAM_DB5p.qp08(), oam_bus.YZOS_OAM_DB6p.qp08(), oam_bus.DEPO_OAM_DB7p.qp08());


  d("OAM TEMP A     %03d %02x %c%c%c%c%c%c%c%c\n",
    oam_temp_a,
    oam_temp_a,
    oam_bus.YZAB_OAM_DA7p.c(), oam_bus.XOTE_OAM_DA6p.c(), oam_bus.WYSO_OAM_DA5p.c(), oam_bus.YBOG_OAM_DA4p.c(),
    oam_bus.XYJU_OAM_DA3p.c(), oam_bus.YJEX_OAM_DA2p.c(), oam_bus.XEGU_OAM_DA1p.c(), oam_bus.XUSO_OAM_DA0p.c());

  d("OAM TEMP B     %03d %2x %c%c%c%c%c%c%c%c\n",
    oam_temp_b,
    oam_temp_b,
    oam_bus.DEPO_OAM_DB7p.c(), oam_bus.YZOS_OAM_DB6p.c(), oam_bus.BAXO_OAM_DB5p.c(), oam_bus.GOMO_OAM_DB4p.c(),
    oam_bus.ZEZY_OAM_DB3p.c(), oam_bus.ZYVE_OAM_DB2p.c(), oam_bus.ZYTY_OAM_DB1p.c(), oam_bus.YLOR_OAM_DB0p.c());


  d("\n");
}

//-----------------------------------------------------------------------------

void GateBoyApp::app_render_frame(Viewport view) {
  gb_thread.pause();

  grid_painter.render(view);

  const auto gb = gb_thread.gb.state();

  uint8_t* framebuffer = gb->framebuffer;
  uint8_t* vid_ram = gb->vid_ram;
  int fb_x = gb->screen_x;
  int fb_y = gb->screen_y;
  int64_t phase_total = gb->phase_total;
  bool sim_stable = gb->sim_stable;

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
  d("Phase count %d\n",      gb->phase_total);
  d("Pass count  %d\n",      gb->pass_count);
  d("Pass total  %d\n",      gb->pass_total);
  d("Pass avg    %4.2f\n",   float(gb->pass_total) / float(gb->phase_total));
  d("Pass hash   %016llx\n", gb->pass_hash);
  d("Total hash  %016llx\n", gb->total_hash);
  d("BGB cycle   0x%08x\n",  (gb->phase_total / 4) - 0x10000);
  d("Sim clock   %f\n",      double(gb->phase_total) / (4194304.0 * 2));

  d("Commands left %d\n",    uint8_t(gb_thread.cursor_w - gb_thread.cursor_r));
  d("Steps left    %d\n",    gb_thread.command.count);
  d("Sim time      %f\n",    gb->sim_time);
  d("Waiting       %d\n",    gb_thread.sig_waiting.load());

  double phase_rate = (gb->phase_total - gb_thread.old_phase_total) / (gb->sim_time - gb_thread.old_sim_time);

  if (gb->sim_time == gb_thread.old_sim_time) {
    phase_rate = 0;
  }

  gb_thread.phase_rate_smooth = (gb_thread.phase_rate_smooth * 0.99) + (phase_rate * 0.01);

  d("Phase rate    %f\n",      gb_thread.phase_rate_smooth);
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
  d("screen_x       %3d\n", gb->screen_x);
  d("screen_y       %3d\n", gb->screen_y);
  d("lcd_data_latch %3d\n", gb->lcd_data_latch);
  d("lcd_pix_lo     %c\n",  gb->lcd_pix_lo.c());
  d("lcd_pix_hi     %c\n",  gb->lcd_pix_hi.c());

  d("lcd_pipe_lo    %c%c%c%c%c%c%c%c\n",
    gb->lcd_pipe_lo[0].c(), gb->lcd_pipe_lo[1].c(), gb->lcd_pipe_lo[2].c(), gb->lcd_pipe_lo[3].c(),
    gb->lcd_pipe_lo[4].c(), gb->lcd_pipe_lo[5].c(), gb->lcd_pipe_lo[6].c(), gb->lcd_pipe_lo[7].c());
  d("lcd_pipe_hi    %c%c%c%c%c%c%c%c\n",
    gb->lcd_pipe_hi[0].c(), gb->lcd_pipe_hi[1].c(), gb->lcd_pipe_hi[2].c(), gb->lcd_pipe_hi[3].c(),
    gb->lcd_pipe_hi[4].c(), gb->lcd_pipe_hi[5].c(), gb->lcd_pipe_hi[6].c(), gb->lcd_pipe_hi[7].c());

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

  d("\002===== Timer =====\001\n");
  d("DIV    : 0x%04x %d\n", gb->tim_reg.get_div(),    gb->tim_reg.get_div());
  d("TIMA A : 0x%02x %d\n", gb->tim_reg.get_tima_a(), gb->tim_reg.get_tima_a());
  d("TIMA B : 0x%02x %d\n", gb->tim_reg.get_tima_b(), gb->tim_reg.get_tima_b());
  d("TMA    : 0x%02x %d\n", gb->tim_reg.get_tma(),    gb->tim_reg.get_tma());
  d("TAC    : 0x%02x %d\n", gb->tim_reg.get_tac(),    gb->tim_reg.get_tac());
  d("NYDU_TIMA_D7_DELAY   %c\n", gb->tim_reg.NYDU_TIMA_D7_DELAY.c());
  d("MOBA_TIMER_OVERFLOWp %c\n", gb->tim_reg.MOBA_TIMER_OVERFLOWp.c());
  d("\n");

  d("\002===== Ints =====\001\n");
  d("IE_D0              %c\n", gb->IE_D0.c());
  d("IE_D1              %c\n", gb->IE_D1.c());
  d("IE_D2              %c\n", gb->IE_D2.c());
  d("IE_D3              %c\n", gb->IE_D3.c());
  d("IE_D4              %c\n", gb->IE_D4.c());
  d("\n");

  d("LOPE_FF0F_0        %c\n", gb->int_reg.LOPE_FF0F_D0p.c());
  d("LALU_FF0F_1        %c\n", gb->int_reg.LALU_FF0F_D1p.c());
  d("NYBO_FF0F_2        %c\n", gb->int_reg.NYBO_FF0F_D2p.c());
  d("UBUL_FF0F_3        %c\n", gb->int_reg.UBUL_FF0F_D3p.c());
  d("ULAK_FF0F_4        %c\n", gb->int_reg.ULAK_FF0F_D4p.c());
  d("\n");
  d("MATY_FF0F_L0p      %c\n", gb->int_reg.MATY_FF0F_L0p.c());
  d("MOPO_FF0F_L1p      %c\n", gb->int_reg.MOPO_FF0F_L1p.c());
  d("PAVY_FF0F_L2p      %c\n", gb->int_reg.PAVY_FF0F_L2p.c());
  d("NEJY_FF0F_L3p      %c\n", gb->int_reg.NEJY_FF0F_L3p.c());
  d("NUTY_FF0F_L4p      %c\n", gb->int_reg.NUTY_FF0F_L4p.c());
  d("\n");
  d("PIN_CPU_INT_VBLANK %c\n", gb->int_reg.PIN_CPU_INT_VBLANK.c());
  d("PIN_CPU_INT_STAT   %c\n", gb->int_reg.PIN_CPU_INT_STAT.c());
  d("PIN_CPU_INT_TIMER  %c\n", gb->int_reg.PIN_CPU_INT_TIMER.c());
  d("PIN_CPU_INT_SERIAL %c\n", gb->int_reg.PIN_CPU_INT_SERIAL.c());
  d("PIN_CPU_INT_JOYPAD %c\n", gb->int_reg.PIN_CPU_INT_JOYPAD.c());
  d("\n");
  d("PIN_CPU_ACK_VBLANK %c\n", gb->int_reg.PIN_CPU_ACK_VBLANK.c());
  d("PIN_CPU_ACK_STAT   %c\n", gb->int_reg.PIN_CPU_ACK_STAT.c());
  d("PIN_CPU_ACK_TIMER  %c\n", gb->int_reg.PIN_CPU_ACK_TIMER.c());
  d("PIN_CPU_ACK_SERIAL %c\n", gb->int_reg.PIN_CPU_ACK_SERIAL.c());
  d("PIN_CPU_ACK_JOYPAD %c\n", gb->int_reg.PIN_CPU_ACK_JOYPAD.c());
  d("\n");

  text_painter.render(view, d.s.c_str(), cursor, 0);
  cursor += 224 - 32;
  d.clear();

  //----------------------------------------

  d("\002===== Clocks =====\001\n");
  d("PHASE %c%c%c%c\n", gb->clk_reg.AFUR_xxxxEFGH.c(), gb->clk_reg.ALEF_AxxxxFGH.c(), gb->clk_reg.APUK_ABxxxxGH.c(), gb->clk_reg.ADYK_ABCxxxxH.c());
  d("\n");
  d("AFUR_xxxxEFGH %c\n", gb->clk_reg.AFUR_xxxxEFGH.c());
  d("ALEF_AxxxxFGH %c\n", gb->clk_reg.ALEF_AxxxxFGH.c());
  d("APUK_ABxxxxGH %c\n", gb->clk_reg.APUK_ABxxxxGH.c());
  d("ADYK_ABCxxxxH %c\n", gb->clk_reg.ADYK_ABCxxxxH.c());
  d("VENA_ABCDxxxx %c\n", gb->clk_reg.VENA_xxCDEFxx.c());
  d("WOSU_xBCxxFGx %c\n", gb->clk_reg.WOSU_AxxDExxH.c());
  d("WUVU_xxCDxxGH %c\n", gb->clk_reg.WUVU_ABxxEFxx.c());
  d("\n");

  d("\002===== Reset =====\001\n");
  d("TUBO_WAITINGp  %c\n", gb->clk_reg.TUBO_WAITINGp.c());
  d("ASOL_POR_DONEn %c\n", gb->clk_reg.ASOL_POR_DONEn.c());
  d("AFER_SYS_RSTp  %c\n", gb->clk_reg.AFER_SYS_RSTp.c());
  d("\n");

  d("\002===== Joypad =====\001\n");
  d("PIN_JOY_P10      %c\n", gb->joypad.PIN_JOY_P10.c());
  d("PIN_JOY_P11      %c\n", gb->joypad.PIN_JOY_P11.c());
  d("PIN_JOY_P12      %c\n", gb->joypad.PIN_JOY_P12.c());
  d("PIN_JOY_P13      %c\n", gb->joypad.PIN_JOY_P13.c());
  d("PIN_JOY_P14      %c\n", gb->joypad.PIN_JOY_P14.c());
  d("PIN_JOY_P15      %c\n", gb->joypad.PIN_JOY_P15.c());
  d("PIN_CPU_WAKE     %c\n", gb->joypad.PIN_CPU_WAKE .c());
  d("AWOB_WAKE_CPU    %c\n", gb->joypad.AWOB_WAKE_CPU.c());
  d("BATU_JP_GLITCH0  %c\n", gb->joypad.BATU_JP_GLITCH0.c());
  d("ACEF_JP_GLITCH1  %c\n", gb->joypad.ACEF_JP_GLITCH1.c());
  d("AGEM_JP_GLITCH2  %c\n", gb->joypad.AGEM_JP_GLITCH2.c());
  d("APUG_JP_GLITCH3  %c\n", gb->joypad.APUG_JP_GLITCH3.c());
  d("JUTE_JOYP_RA     %c\n", gb->joypad.JUTE_JOYP_RA.c());
  d("KECY_JOYP_LB     %c\n", gb->joypad.KECY_JOYP_LB.c());
  d("JALE_JOYP_UC     %c\n", gb->joypad.JALE_JOYP_UC.c());
  d("KYME_JOYP_DS     %c\n", gb->joypad.KYME_JOYP_DS.c());
  d("KELY_JOYP_UDLR   %c\n", gb->joypad.KELY_JOYP_UDLR.c());
  d("COFY_JOYP_ABCS   %c\n", gb->joypad.COFY_JOYP_ABCS.c());
  d("KUKO_DBG_FF00_D6 %c\n", gb->joypad.KUKO_DBG_FF00_D6.c());
  d("KERU_DBG_FF00_D7 %c\n", gb->joypad.KERU_DBG_FF00_D7.c());
  d("KEVU_JOYP_L0     %c\n", gb->joypad.KEVU_JOYP_L0.c());
  d("KAPA_JOYP_L1     %c\n", gb->joypad.KAPA_JOYP_L1.c());
  d("KEJA_JOYP_L2     %c\n", gb->joypad.KEJA_JOYP_L2.c());
  d("KOLO_JOYP_L3     %c\n", gb->joypad.KOLO_JOYP_L3.c());
  d("\n");

  d("\002===== Serial =====\001\n");
  d("XFER_START  %c\n", gb->ser_reg.ETAF_XFER_START.c());
  d("XFER_DIR    %c\n", gb->ser_reg.CULY_XFER_DIR.c());
  d("SER_CLK     %c\n", gb->ser_reg.COTY_SER_CLK.c());
  d("SER_CNT     %d\n", pack_p(!gb->ser_reg.CAFA_SER_CNT0.qn16(), !gb->ser_reg.CYLO_SER_CNT1.qn16(), !gb->ser_reg.CYDE_SER_CNT2.qn16(), 0));
  d("SER_DATA    0x%02x\n", gb->ser_reg.get_data());
  d("SER_OUT     %c\n", gb->ser_reg.ELYS_SER_OUT.c());
  d("SCK         %c\n", gb->ser_reg.PIN_SCK.c());
  d("SIN         %c\n", gb->ser_reg.PIN_SIN.c());
  d("SOUT        %c\n", gb->ser_reg.PIN_SOUT.c());
  d("_CALY_SER_INTp %c\n", gb->ser_reg.CALY_INT_SERp.c());
  d("\n");

  text_painter.render(view, d.s.c_str(), cursor, 0);
  cursor += 224 - 64;
  d.clear();

  //----------------------------------------

  dump_cpu_bus(d, gb->cpu_bus);
  dump_ext_bus(d, gb->ext_bus);
  dump_vram_bus(d, gb->vram_bus);
  dump_oam_bus(d, gb->oam_bus);


  d("\002===== DMA Reg =====\001\n");
  const auto& dma_reg = gb->dma_reg;

  int dma_addr_hi = pack_p(!dma_reg.NAFA_DMA_A08n.qp08(), !dma_reg.PYNE_DMA_A09n.qp08(), !dma_reg.PARA_DMA_A10n.qp08(), !dma_reg.NYDO_DMA_A11n.qp08(),
                           !dma_reg.NYGY_DMA_A12n.qp08(), !dma_reg.PULA_DMA_A13n.qp08(), !dma_reg.POKU_DMA_A14n.qp08(), !dma_reg.MARU_DMA_A15n.qp08());

  int dma_addr_lo = pack_p(dma_reg.NAKY_DMA_A00p.qp17(), dma_reg.PYRO_DMA_A01p.qp17(), dma_reg.NEFY_DMA_A02p.qp17(), dma_reg.MUTY_DMA_A03p.qp17(),
                           dma_reg.NYKO_DMA_A04p.qp17(), dma_reg.PYLO_DMA_A05p.qp17(), dma_reg.NUTO_DMA_A06p.qp17(), dma_reg.MUGU_DMA_A07p.qp17());

  d("DMA Addr 0x%02x:%02x\n",dma_addr_hi, dma_addr_lo);
  d("MATU_DMA_RUNNINGp   %d\n",  dma_reg.MATU_DMA_RUNNINGp.qp17());
  d("LYXE_DMA_LATCHn     %d\n",  dma_reg.LYXE_DMA_LATCHp);
  d("MYTE_DMA_DONE       %d\n", !dma_reg.MYTE_DMA_DONE.qn16());
  d("LUVY_DMA_TRIG_d0    %d\n",  dma_reg.LUVY_DMA_TRIG_d0.qp17());
  d("LENE_DMA_TRIG_d4    %d\n", !dma_reg.LENE_DMA_TRIG_d4.qn16());
  d("LOKY_DMA_LATCHp     %d\n",  dma_reg.LOKY_DMA_LATCHp.qp03());
  d("\n");

  text_painter.render(view, d.s.c_str(), cursor, 0);
  cursor += 224;
  d.clear();

  //----------------------------------------

  d("\002===== LCD =====\001\n");
  d("PIX COUNT : %03d\n", gb->pix_pipe.get_pix_count());
  d("LCD X     : %03d\n", gb->lcd_reg.get_lx());
  d("LCD Y     : %03d\n", gb->lcd_reg.get_ly());
  d("LYC       : %03d\n", gb->lcd_reg.get_lyc());
  d("\n");

  d("PIN_LCD_CLOCK   : "); gb->PIN_LCD_CLOCK.dump(d); d("\n");
  d("PIN_LCD_HSYNC   : "); gb->PIN_LCD_HSYNC.dump(d); d("\n");
  d("PIN_LCD_VSYNC   : "); gb->PIN_LCD_VSYNC.dump(d); d("\n");
  d("PIN_LCD_DATA1   : "); gb->PIN_LCD_DATA1.dump(d); d("\n");
  d("PIN_LCD_DATA0   : "); gb->PIN_LCD_DATA0.dump(d); d("\n");
  d("PIN_LCD_CNTRL   : "); gb->PIN_LCD_CNTRL.dump(d); d("\n");
  d("PIN_LCD_DATALCH : "); gb->PIN_LCD_LATCH.dump(d); d("\n");
  d("PIN_LCD_ALTSIGL   : "); gb->PIN_LCD_FLIPS.dump(d); d("\n");
  d("\n");

  d("CATU_LINE_P000      %c\n", gb->lcd_reg.CATU_LINE_P000.c());
  d("NYPE_LINE_P002      %c\n", gb->lcd_reg.NYPE_LINE_P002.c());
  d("ANEL_LINE_P002      %c\n", gb->lcd_reg.ANEL_LINE_P002.c());
  d("RUTU_LINE_P910      %c\n", gb->lcd_reg.RUTU_LINE_P910.c());
  d("MYTA_LINE_153p      %c\n", gb->lcd_reg.MYTA_LINE_153p     .c());
  d("POPU_IN_VBLANKp     %c\n", gb->lcd_reg.POPU_IN_VBLANKp    .c());
  d("ROPO_LY_MATCH_SYNCp %c\n", gb->lcd_reg.ROPO_LY_MATCH_SYNCp.c());
  d("\n");

  d("\002===== Pix Pipe =====\001\n");

  d.dump_reg("PIX COUNT",
    gb->pix_pipe.XEHO_X0p.qp17(), gb->pix_pipe.SAVY_X1p.qp17(), gb->pix_pipe.XODU_X2p.qp17(), gb->pix_pipe.XYDO_X3p.qp17(),
    gb->pix_pipe.TUHU_X4p.qp17(), gb->pix_pipe.TUKY_X5p.qp17(), gb->pix_pipe.TAKO_X6p.qp17(), gb->pix_pipe.SYBE_X7p.qp17());


  d.dump_reg("FF40 LCDC",
    gb->pix_pipe.VYXE_LCDC_BGENn.qn08(),
    gb->pix_pipe.XYLO_LCDC_SPENn.qn08(),
    gb->pix_pipe.XYMO_LCDC_SPSIZEn.qn08(),
    gb->pix_pipe.XAFO_LCDC_BGMAPn.qn08(),
    gb->pix_pipe.WEXU_LCDC_BGTILEn.qn08(),
    gb->pix_pipe.WYMO_LCDC_WINENn.qn08(),
    gb->pix_pipe.WOKY_LCDC_WINMAPn.qn08(),
    gb->pix_pipe.XONA_LCDC_LCDENn.qn08());

  // FIXME plumb sadu/xaty in here somehow
  d.dump_reg("FF41 STAT",
    0, //!gb->pix_pipe.SADU_STAT_MODE0n,
    0, //!gb->pix_pipe.XATY_STAT_MODE1n,
    gb->pix_pipe.RUPO_LYC_MATCH_LATCHn.qn03(),
    gb->pix_pipe.ROXE_STAT_HBI_ENn.qn08(),
    gb->pix_pipe.RUFO_STAT_VBI_ENn.qn08(),
    gb->pix_pipe.REFE_STAT_OAI_ENn.qn08(),
    gb->pix_pipe.RUGU_STAT_LYI_ENn.qn08(),
    1
  );

  d.dump_reg("FF42 SCY",
    gb->pix_pipe.GAVE_SCY0n.qn08(),
    gb->pix_pipe.FYMO_SCY1n.qn08(),
    gb->pix_pipe.FEZU_SCY2n.qn08(),
    gb->pix_pipe.FUJO_SCY3n.qn08(),
    gb->pix_pipe.DEDE_SCY4n.qn08(),
    gb->pix_pipe.FOTY_SCY5n.qn08(),
    gb->pix_pipe.FOHA_SCY6n.qn08(),
    gb->pix_pipe.FUNY_SCY7n.qn08()
  );

  d.dump_reg("FF43 SCX",
    gb->pix_pipe.DATY_SCX0n.qn08(),
    gb->pix_pipe.DUZU_SCX1n.qn08(),
    gb->pix_pipe.CYXU_SCX2n.qn08(),
    gb->pix_pipe.GUBO_SCX3n.qn08(),
    gb->pix_pipe.BEMY_SCX4n.qn08(),
    gb->pix_pipe.CUZY_SCX5n.qn08(),
    gb->pix_pipe.CABU_SCX6n.qn08(),
    gb->pix_pipe.BAKE_SCX7n.qn08()
  );

  d.dump_reg("FF47 BGP",
    gb->pix_pipe.PAVO_BGP_D0n.qn07(),
    gb->pix_pipe.NUSY_BGP_D1n.qn07(),
    gb->pix_pipe.PYLU_BGP_D2n.qn07(),
    gb->pix_pipe.MAXY_BGP_D3n.qn07(),
    gb->pix_pipe.MUKE_BGP_D4n.qn07(),
    gb->pix_pipe.MORU_BGP_D5n.qn07(),
    gb->pix_pipe.MOGY_BGP_D6n.qn07(),
    gb->pix_pipe.MENA_BGP_D7n.qn07()
  );

  d.dump_reg("FF48 OBP0",
    gb->pix_pipe.XUFU_OBP0_D0n.qn07(),
    gb->pix_pipe.XUKY_OBP0_D1n.qn07(),
    gb->pix_pipe.XOVA_OBP0_D2n.qn07(),
    gb->pix_pipe.XALO_OBP0_D3n.qn07(),
    gb->pix_pipe.XERU_OBP0_D4n.qn07(),
    gb->pix_pipe.XYZE_OBP0_D5n.qn07(),
    gb->pix_pipe.XUPO_OBP0_D6n.qn07(),
    gb->pix_pipe.XANA_OBP0_D7n.qn07()
  );

  d.dump_reg("FF49 OBP1",
    gb->pix_pipe.MOXY_OBP1_D0n.qn07(),
    gb->pix_pipe.LAWO_OBP1_D1n.qn07(),
    gb->pix_pipe.MOSA_OBP1_D2n.qn07(),
    gb->pix_pipe.LOSE_OBP1_D3n.qn07(),
    gb->pix_pipe.LUNE_OBP1_D4n.qn07(),
    gb->pix_pipe.LUGU_OBP1_D5n.qn07(),
    gb->pix_pipe.LEPU_OBP1_D6n.qn07(),
    gb->pix_pipe.LUXO_OBP1_D7n.qn07()
  );

  d.dump_reg("FF4A WY",
    gb->pix_pipe.NESO_WY0n.qn08(),
    gb->pix_pipe.NYRO_WY1n.qn08(),
    gb->pix_pipe.NAGA_WY2n.qn08(),
    gb->pix_pipe.MELA_WY3n.qn08(),
    gb->pix_pipe.NULO_WY4n.qn08(),
    gb->pix_pipe.NENE_WY5n.qn08(),
    gb->pix_pipe.NUKA_WY6n.qn08(),
    gb->pix_pipe.NAFU_WY7n.qn08()
  );

  d.dump_reg("FF4B WX",
    gb->pix_pipe.MYPA_WX0n.qn08(),
    gb->pix_pipe.NOFE_WX1n.qn08(),
    gb->pix_pipe.NOKE_WX2n.qn08(),
    gb->pix_pipe.MEBY_WX3n.qn08(),
    gb->pix_pipe.MYPU_WX4n.qn08(),
    gb->pix_pipe.MYCE_WX5n.qn08(),
    gb->pix_pipe.MUVO_WX6n.qn08(),
    gb->pix_pipe.NUKU_WX7n.qn08()
  );

  d.dump_reg("BG_PIPE_A",
    gb->pix_pipe.MYDE_BG_PIPE_A0.qp16(), gb->pix_pipe.NOZO_BG_PIPE_A1.qp16(), gb->pix_pipe.MOJU_BG_PIPE_A2.qp16(), gb->pix_pipe.MACU_BG_PIPE_A3.qp16(),
    gb->pix_pipe.NEPO_BG_PIPE_A4.qp16(), gb->pix_pipe.MODU_BG_PIPE_A5.qp16(), gb->pix_pipe.NEDA_BG_PIPE_A6.qp16(), gb->pix_pipe.PYBO_BG_PIPE_A7.qp16());

  d.dump_reg("BG_PIPE_B",
    gb->pix_pipe.TOMY_BG_PIPE_B0.qp16(), gb->pix_pipe.TACA_BG_PIPE_B1.qp16(), gb->pix_pipe.SADY_BG_PIPE_B2.qp16(), gb->pix_pipe.RYSA_BG_PIPE_B3.qp16(),
    gb->pix_pipe.SOBO_BG_PIPE_B4.qp16(), gb->pix_pipe.SETU_BG_PIPE_B5.qp16(), gb->pix_pipe.RALU_BG_PIPE_B6.qp16(), gb->pix_pipe.SOHU_BG_PIPE_B7.qp16());

  d.dump_reg("SPR_PIPE_A",
    gb->pix_pipe.NYLU_SPR_PIPE_B0.qp16(), gb->pix_pipe.PEFU_SPR_PIPE_B1.qp16(), gb->pix_pipe.NATY_SPR_PIPE_B2.qp16(), gb->pix_pipe.PYJO_SPR_PIPE_B3.qp16(),
    gb->pix_pipe.VARE_SPR_PIPE_B4.qp16(), gb->pix_pipe.WEBA_SPR_PIPE_B5.qp16(), gb->pix_pipe.VANU_SPR_PIPE_B6.qp16(), gb->pix_pipe.VUPY_SPR_PIPE_B7.qp16());

  d.dump_reg("SPR_PIPE_B",
    gb->pix_pipe.NURO_SPR_PIPE_A0.qp16(), gb->pix_pipe.MASO_SPR_PIPE_A1.qp16(), gb->pix_pipe.LEFE_SPR_PIPE_A2.qp16(), gb->pix_pipe.LESU_SPR_PIPE_A3.qp16(),
    gb->pix_pipe.WYHO_SPR_PIPE_A4.qp16(), gb->pix_pipe.WORA_SPR_PIPE_A5.qp16(), gb->pix_pipe.VAFO_SPR_PIPE_A6.qp16(), gb->pix_pipe.WUFY_SPR_PIPE_A7.qp16());

  d.dump_reg("PAL_PIPE",
    gb->pix_pipe.RUGO_PAL_PIPE_0.qp16(), gb->pix_pipe.SATA_PAL_PIPE_1.qp16(), gb->pix_pipe.ROSA_PAL_PIPE_2.qp16(), gb->pix_pipe.SOMY_PAL_PIPE_3.qp16(),
    gb->pix_pipe.PALU_PAL_PIPE_4.qp16(), gb->pix_pipe.NUKE_PAL_PIPE_5.qp16(), gb->pix_pipe.MODA_PAL_PIPE_6.qp16(), gb->pix_pipe.LYME_PAL_PIPE_7.qp16()
  );

  d.dump_reg("MASK_PIPE",
    gb->pix_pipe.VEZO_MASK_PIPE_0.qp16(), gb->pix_pipe.WURU_MASK_PIPE_1.qp16(), gb->pix_pipe.VOSA_MASK_PIPE_2.qp16(), gb->pix_pipe.WYFU_MASK_PIPE_3.qp16(),
    gb->pix_pipe.XETE_MASK_PIPE_4.qp16(), gb->pix_pipe.WODA_MASK_PIPE_5.qp16(), gb->pix_pipe.VUMO_MASK_PIPE_6.qp16(), gb->pix_pipe.VAVA_MASK_PIPE_7.qp16()
  );

  d.dump_reg("WIN X",
    0,
    0,
    0,
    gb->pix_pipe.WYKA_WIN_X3.qp17(),
    gb->pix_pipe.WODY_WIN_X4.qp17(),
    gb->pix_pipe.WOBO_WIN_X5.qp17(),
    gb->pix_pipe.WYKO_WIN_X6.qp17(),
    gb->pix_pipe.XOLO_WIN_X7.qp17()
  );

  d.dump_reg("WIN Y",
    gb->pix_pipe.VYNO_WIN_Y0.qp17(),
    gb->pix_pipe.VUJO_WIN_Y1.qp17(),
    gb->pix_pipe.VYMU_WIN_Y2.qp17(),
    gb->pix_pipe.TUFU_WIN_Y3.qp17(),
    gb->pix_pipe.TAXA_WIN_Y4.qp17(),
    gb->pix_pipe.TOZO_WIN_Y5.qp17(),
    gb->pix_pipe.TATE_WIN_Y6.qp17(),
    gb->pix_pipe.TEKE_WIN_Y7.qp17()
  );

  d("\n");

  d("ROXY_FINE_SCROLL_DONEn %c\n", gb->pix_pipe.ROXY_SCX_FINE_MATCH_LATCHn.c());
  d("RYKU_FINE_CNT0         %c\n", gb->pix_pipe.RYKU_FINE_CNT0.c());
  d("ROGA_FINE_CNT1         %c\n", gb->pix_pipe.ROGA_FINE_CNT1.c());
  d("RUBU_FINE_CNT2         %c\n", gb->pix_pipe.RUBU_FINE_CNT2.c());
  d("XYMU_RENDERINGp        %c\n", gb->pix_pipe.XYMU_RENDERINGn.c());
  d("RUPO_LYC_MATCH_LATCHn  %c\n", gb->pix_pipe.RUPO_LYC_MATCH_LATCHn.c());
  d("WUSA_LCD_CLOCK_GATE    %c\n", gb->pix_pipe.WUSA_LCD_CLOCK_GATE.c());
  d("VOGA_RENDER_DONE_SYNC  %c\n", gb->pix_pipe.VOGA_HBLANKp.c());
  d("PUXA_FINE_MATCH_A      %c\n", gb->pix_pipe.PUXA_SCX_FINE_MATCH_A.c());
  d("NYZE_FINE_MATCH_B      %c\n", gb->pix_pipe.NYZE_SCX_FINE_MATCH_B.c());
  d("PAHO_X_8_SYNC          %c\n", gb->pix_pipe.PAHO_X_8_SYNC.c());
  d("POFY_HSYNCp            %c\n", gb->pix_pipe.POFY.c());
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

  text_painter.render(view, d.s.c_str(), cursor, 0);
  cursor += 224;
  d.clear();

  //----------------------------------------

  d("\002===== SpriteFetch =====\001\n");
  d("SOBU_SFETCH_REQp     %c\n", gb->sprite_fetcher.SOBU_SFETCH_REQp    .c());
  d("SUDA_SFETCH_REQp     %c\n", gb->sprite_fetcher.SUDA_SFETCH_REQp    .c());
  d("\n");
  d("TAKA_SFETCH_RUNNINGp %c\n", gb->sprite_fetcher.TAKA_SFETCH_RUNNINGp.c());
  d("\n");
  d("TOXE_SFETCH_S0       %c\n", gb->sprite_fetcher.TOXE_SFETCH_S0      .c());
  d("TYFO_SFETCH_S0_D1    %c\n", gb->sprite_fetcher.TYFO_SFETCH_S0_D1   .c());
  d("\n");
  d("TULY_SFETCH_S1       %c\n", gb->sprite_fetcher.TULY_SFETCH_S1      .c());
  d("TOBU_SFETCH_S1_D2    %c\n", gb->sprite_fetcher.TOBU_SFETCH_S1_D2   .c());
  d("VONU_SFETCH_S1_D4    %c\n", gb->sprite_fetcher.VONU_SFETCH_S1_D4   .c());
  d("SEBA_SFETCH_S1_D5    %c\n", gb->sprite_fetcher.SEBA_SFETCH_S1_D5   .c());
  d("\n");
  d("TESE_SFETCH_S2       %c\n", gb->sprite_fetcher.TESE_SFETCH_S2      .c());
  d("\n");

  d("\002===== SpriteScan =====\001\n");

  d("SCAN INDEX       %02d\n", pack_p(gb->sprite_scanner.YFEL_SCAN0.qp17(), gb->sprite_scanner.WEWY_SCAN1.qp17(), gb->sprite_scanner.GOSO_SCAN2.qp17(), gb->sprite_scanner.ELYN_SCAN3.qp17(),
                                      gb->sprite_scanner.FAHA_SCAN4.qp17(), gb->sprite_scanner.FONY_SCAN5.qp17(), 0, 0));
  d("SPRITE INDEX     %02d\n", pack_p(gb->sprite_scanner.XADU_SPRITE_IDX0p.qn12(), gb->sprite_scanner.XEDY_SPRITE_IDX1p.qn12(), gb->sprite_scanner.ZUZE_SPRITE_IDX2p.qn12(), gb->sprite_scanner.XOBE_SPRITE_IDX3p.qn12(),
                                      gb->sprite_scanner.YDUF_SPRITE_IDX4p.qn12(), gb->sprite_scanner.XECU_SPRITE_IDX5p.qn12(), 0, 0));

  d("BESU_SCANNINGp   %c\n", gb->sprite_scanner.BESU_SCANNINGp  .c());
  d("CENO_SCANNINGp   %c\n", gb->sprite_scanner.CENO_SCANNINGp  .c());
  d("BYBA_SCAN_DONE_A %c\n", gb->sprite_scanner.BYBA_SCAN_DONE_A.c());
  d("DOBA_SCAN_DONE_B %c\n", gb->sprite_scanner.DOBA_SCAN_DONE_B.c());
  d("\n");
  d("LCD Y      %03d\n", gb->lcd_reg.get_ly());

  d("\n");


  d("\002===== SpriteStore =====\001\n");
  d("DEZY_STORE_ENn %c\n", gb->sprite_store.DEZY_STORE_ENn.c());

  d("SPRITE COUNT %02d\n", pack_p(gb->sprite_store.BESE_SPRITE_COUNT0.qp17(), gb->sprite_store.CUXY_SPRITE_COUNT1.qp17(), gb->sprite_store.BEGO_SPRITE_COUNT2.qp17(), gb->sprite_store.DYBE_SPRITE_COUNT3.qp17()));

  int spr_tri_idx = pack_p(gb->sprite_store.SPR_TRI_I0p.qp(), gb->sprite_store.SPR_TRI_I1p.qp(), gb->sprite_store.SPR_TRI_I2p.qp(), gb->sprite_store.SPR_TRI_I3p.qp(), gb->sprite_store.SPR_TRI_I4p.qp(), gb->sprite_store.SPR_TRI_I5p.qp(), 0, 0);
  d("SPR_TRI_IDX  = %2d %c%c%c%c%c%c\n",
    spr_tri_idx,
    gb->sprite_store.SPR_TRI_I5p.c(), gb->sprite_store.SPR_TRI_I4p.c(), gb->sprite_store.SPR_TRI_I3p.c(), gb->sprite_store.SPR_TRI_I2p.c(),
    gb->sprite_store.SPR_TRI_I1p.c(), gb->sprite_store.SPR_TRI_I0p.c());

  int spr_tri_line = pack_p(gb->sprite_store.SPR_TRI_L0.qp(), gb->sprite_store.SPR_TRI_L1.qp(), gb->sprite_store.SPR_TRI_L2.qp(), gb->sprite_store.SPR_TRI_L3.qp());
  d("SPR_TRI_LINE = %2d %c%c%c%c\n",
    spr_tri_line,
    gb->sprite_store.SPR_TRI_L0.c(), gb->sprite_store.SPR_TRI_L1.c(), gb->sprite_store.SPR_TRI_L2.c(), gb->sprite_store.SPR_TRI_L3.c());

  d("STORE0 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.EBOJ_STORE0_RSTp.qp17(),
    pack_n(gb->sprite_store.YGUS_STORE0_I0n.qp08(), gb->sprite_store.YSOK_STORE0_I1n.qp08(), gb->sprite_store.YZEP_STORE0_I2n.qp08(), gb->sprite_store.WYTE_STORE0_I3n.qp08(), gb->sprite_store.ZONY_STORE0_I4n.qp08(), gb->sprite_store.YWAK_STORE0_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.GYHO_STORE0_L0n.qp08(), gb->sprite_store.CUFO_STORE0_L1n.qp08(), gb->sprite_store.BOZU_STORE0_L2n.qp08(), gb->sprite_store.FYHY_STORE0_L3n.qp08()),
    pack_n(gb->sprite_store.XEPE_STORE0_X0p.qn08(), gb->sprite_store.YLAH_STORE0_X1p.qn08(), gb->sprite_store.ZOLA_STORE0_X2p.qn08(), gb->sprite_store.ZULU_STORE0_X3p.qn08(), gb->sprite_store.WELO_STORE0_X4p.qn08(), gb->sprite_store.XUNY_STORE0_X5p.qn08(), gb->sprite_store.WOTE_STORE0_X6p.qn08(), gb->sprite_store.XAKO_STORE0_X7p.qn08())
  );

  d("STORE1 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.CEDY_STORE1_RSTp.qp17(),
    pack_n(gb->sprite_store.CADU_STORE1_I0n.qp08(), gb->sprite_store.CEBO_STORE1_I1n.qp08(), gb->sprite_store.CUFA_STORE1_I2n.qp08(), gb->sprite_store.COMA_STORE1_I3n.qp08(), gb->sprite_store.CUZA_STORE1_I4n.qp08(), gb->sprite_store.CAJY_STORE1_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.AMES_STORE1_L0n.qp08(), gb->sprite_store.AROF_STORE1_L1n.qp08(), gb->sprite_store.ABOP_STORE1_L2n.qp08(), gb->sprite_store.ABUG_STORE1_L3n.qp08()),
    pack_n(gb->sprite_store.DANY_STORE1_X0p.qn08(), gb->sprite_store.DUKO_STORE1_X1p.qn08(), gb->sprite_store.DESU_STORE1_X2p.qn08(), gb->sprite_store.DAZO_STORE1_X3p.qn08(), gb->sprite_store.DAKE_STORE1_X4p.qn08(), gb->sprite_store.CESO_STORE1_X5p.qn08(), gb->sprite_store.DYFU_STORE1_X6p.qn08(), gb->sprite_store.CUSY_STORE1_X7p.qn08())
  );

  d("STORE2 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.EGAV_STORE2_RSTp.qp17(),
    pack_n(gb->sprite_store.BUHE_STORE2_I0n.qp08(), gb->sprite_store.BYHU_STORE2_I1n.qp08(), gb->sprite_store.BECA_STORE2_I2n.qp08(), gb->sprite_store.BULU_STORE2_I3n.qp08(), gb->sprite_store.BUNA_STORE2_I4n.qp08(), gb->sprite_store.BOXA_STORE2_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.YLOV_STORE2_L0n.qp08(), gb->sprite_store.XOSY_STORE2_L1n.qp08(), gb->sprite_store.XAZY_STORE2_L2n.qp08(), gb->sprite_store.YKUK_STORE2_L3n.qp08()),
    pack_n(gb->sprite_store.FOKA_STORE2_X0p.qn08(), gb->sprite_store.FYTY_STORE2_X1p.qn08(), gb->sprite_store.FUBY_STORE2_X2p.qn08(), gb->sprite_store.GOXU_STORE2_X3p.qn08(), gb->sprite_store.DUHY_STORE2_X4p.qn08(), gb->sprite_store.EJUF_STORE2_X5p.qn08(), gb->sprite_store.ENOR_STORE2_X6p.qn08(), gb->sprite_store.DEPY_STORE2_X7p.qn08())
  );

  d("STORE3 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.GOTA_STORE3_RSTp.qp17(),
    pack_n(gb->sprite_store.DEVY_STORE3_I0n.qp08(), gb->sprite_store.DESE_STORE3_I1n.qp08(), gb->sprite_store.DUNY_STORE3_I2n.qp08(), gb->sprite_store.DUHA_STORE3_I3n.qp08(), gb->sprite_store.DEBA_STORE3_I4n.qp08(), gb->sprite_store.DAFU_STORE3_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.ZURO_STORE3_L0n.qp08(), gb->sprite_store.ZYLU_STORE3_L1n.qp08(), gb->sprite_store.ZENE_STORE3_L2n.qp08(), gb->sprite_store.ZURY_STORE3_L3n.qp08()),
    pack_n(gb->sprite_store.XOLY_STORE3_X0p.qn08(), gb->sprite_store.XYBA_STORE3_X1p.qn08(), gb->sprite_store.XABE_STORE3_X2p.qn08(), gb->sprite_store.XEKA_STORE3_X3p.qn08(), gb->sprite_store.XOMY_STORE3_X4p.qn08(), gb->sprite_store.WUHA_STORE3_X5p.qn08(), gb->sprite_store.WYNA_STORE3_X6p.qn08(), gb->sprite_store.WECO_STORE3_X7p.qn08())
  );

  d("STORE4 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.XUDY_STORE4_RSTp.qp17(),
    pack_n(gb->sprite_store.XAVE_STORE4_I0n.qp08(), gb->sprite_store.XEFE_STORE4_I1n.qp08(), gb->sprite_store.WANU_STORE4_I2n.qp08(), gb->sprite_store.XABO_STORE4_I3n.qp08(), gb->sprite_store.XEGE_STORE4_I4n.qp08(), gb->sprite_store.XYNU_STORE4_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.CAPO_STORE4_L0n.qp08(), gb->sprite_store.CAJU_STORE4_L1n.qp08(), gb->sprite_store.CONO_STORE4_L2n.qp08(), gb->sprite_store.CUMU_STORE4_L3n.qp08()),
    pack_n(gb->sprite_store.WEDU_STORE4_X0p.qn08(), gb->sprite_store.YGAJ_STORE4_X1p.qn08(), gb->sprite_store.ZYJO_STORE4_X2p.qn08(), gb->sprite_store.XURY_STORE4_X3p.qn08(), gb->sprite_store.YBED_STORE4_X4p.qn08(), gb->sprite_store.ZALA_STORE4_X5p.qn08(), gb->sprite_store.WYDE_STORE4_X6p.qn08(), gb->sprite_store.XEPA_STORE4_X7p.qn08())
  );

  d("STORE5 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.WAFY_STORE5_RSTp.qp17(),
    pack_n(gb->sprite_store.EKOP_STORE5_I0n.qp08(), gb->sprite_store.ETYM_STORE5_I1n.qp08(), gb->sprite_store.GORU_STORE5_I2n.qp08(), gb->sprite_store.EBEX_STORE5_I3n.qp08(), gb->sprite_store.ETAV_STORE5_I4n.qp08(), gb->sprite_store.EKAP_STORE5_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.ACEP_STORE5_L0n.qp08(), gb->sprite_store.ABEG_STORE5_L1n.qp08(), gb->sprite_store.ABUX_STORE5_L2n.qp08(), gb->sprite_store.ANED_STORE5_L3n.qp08()),
    pack_n(gb->sprite_store.FUSA_STORE5_X0p.qn08(), gb->sprite_store.FAXA_STORE5_X1p.qn08(), gb->sprite_store.FOZY_STORE5_X2p.qn08(), gb->sprite_store.FESY_STORE5_X3p.qn08(), gb->sprite_store.CYWE_STORE5_X4p.qn08(), gb->sprite_store.DYBY_STORE5_X5p.qn08(), gb->sprite_store.DURY_STORE5_X6p.qn08(), gb->sprite_store.CUVY_STORE5_X7p.qn08())
  );

  d("STORE6 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.WOMY_STORE6_RSTp.qp17(),
    pack_n(gb->sprite_store.GABO_STORE6_I0n.qp08(), gb->sprite_store.GACY_STORE6_I1n.qp08(), gb->sprite_store.FOGO_STORE6_I2n.qp08(), gb->sprite_store.GOHU_STORE6_I3n.qp08(), gb->sprite_store.FOXY_STORE6_I4n.qp08(), gb->sprite_store.GECU_STORE6_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.ZUMY_STORE6_L0n.qp08(), gb->sprite_store.ZAFU_STORE6_L1n.qp08(), gb->sprite_store.ZEXO_STORE6_L2n.qp08(), gb->sprite_store.ZUBE_STORE6_L3n.qp08()),
    pack_n(gb->sprite_store.YCOL_STORE6_X0p.qn08(), gb->sprite_store.YRAC_STORE6_X1p.qn08(), gb->sprite_store.YMEM_STORE6_X2p.qn08(), gb->sprite_store.YVAG_STORE6_X3p.qn08(), gb->sprite_store.ZOLY_STORE6_X4p.qn08(), gb->sprite_store.ZOGO_STORE6_X5p.qn08(), gb->sprite_store.ZECU_STORE6_X6p.qn08(), gb->sprite_store.ZESA_STORE6_X7p.qn08())
  );

  d("STORE7 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.WAPO_STORE7_RSTp.qp17(),
    pack_n(gb->sprite_store.GULE_STORE7_I0n.qp08(), gb->sprite_store.GYNO_STORE7_I1n.qp08(), gb->sprite_store.FEFA_STORE7_I2n.qp08(), gb->sprite_store.FYSU_STORE7_I3n.qp08(), gb->sprite_store.GESY_STORE7_I4n.qp08(), gb->sprite_store.FUZO_STORE7_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.XYNA_STORE7_L0n.qp08(), gb->sprite_store.YGUM_STORE7_L1n.qp08(), gb->sprite_store.XAKU_STORE7_L2n.qp08(), gb->sprite_store.XYGO_STORE7_L3n.qp08()),
    pack_n(gb->sprite_store.ERAZ_STORE7_X0p.qn08(), gb->sprite_store.EPUM_STORE7_X1p.qn08(), gb->sprite_store.EROL_STORE7_X2p.qn08(), gb->sprite_store.EHYN_STORE7_X3p.qn08(), gb->sprite_store.FAZU_STORE7_X4p.qn08(), gb->sprite_store.FAXE_STORE7_X5p.qn08(), gb->sprite_store.EXUK_STORE7_X6p.qn08(), gb->sprite_store.FEDE_STORE7_X7p.qn08())
  );

  d("STORE8 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.EXUQ_STORE8_RSTp.qp17(),
    pack_n(gb->sprite_store.AXUV_STORE8_I0n.qp08(), gb->sprite_store.BADA_STORE8_I1n.qp08(), gb->sprite_store.APEV_STORE8_I2n.qp08(), gb->sprite_store.BADO_STORE8_I3n.qp08(), gb->sprite_store.BEXY_STORE8_I4n.qp08(), gb->sprite_store.BYHE_STORE8_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.AZAP_STORE8_L0n.qp08(), gb->sprite_store.AFYX_STORE8_L1n.qp08(), gb->sprite_store.AFUT_STORE8_L2n.qp08(), gb->sprite_store.AFYM_STORE8_L3n.qp08()),
    pack_n(gb->sprite_store.EZUF_STORE8_X0p.qn08(), gb->sprite_store.ENAD_STORE8_X1p.qn08(), gb->sprite_store.EBOW_STORE8_X2p.qn08(), gb->sprite_store.FYCA_STORE8_X3p.qn08(), gb->sprite_store.GAVY_STORE8_X4p.qn08(), gb->sprite_store.GYPU_STORE8_X5p.qn08(), gb->sprite_store.GADY_STORE8_X6p.qn08(), gb->sprite_store.GAZA_STORE8_X7p.qn08())
  );

  d("STORE9 R%d I%02d L%02d X%03d\n",
    gb->sprite_store.FONO_STORE9_RSTp.qp17(),
    pack_n(gb->sprite_store.YBER_STORE9_I0n.qp08(), gb->sprite_store.YZOR_STORE9_I1n.qp08(), gb->sprite_store.XYFE_STORE9_I2n.qp08(), gb->sprite_store.XOTU_STORE9_I3n.qp08(), gb->sprite_store.XUTE_STORE9_I4n.qp08(), gb->sprite_store.XUFO_STORE9_I5n.qp08(), 1, 1),
    pack_n(gb->sprite_store.CANA_STORE9_L0n.qp08(), gb->sprite_store.FOFO_STORE9_L1n.qp08(), gb->sprite_store.DYSY_STORE9_L2n.qp08(), gb->sprite_store.DEWU_STORE9_L3n.qp08()),
    pack_n(gb->sprite_store.XUVY_STORE9_X0p.qn08(), gb->sprite_store.XERE_STORE9_X1p.qn08(), gb->sprite_store.XUZO_STORE9_X2p.qn08(), gb->sprite_store.XEXA_STORE9_X3p.qn08(), gb->sprite_store.YPOD_STORE9_X4p.qn08(), gb->sprite_store.YROP_STORE9_X5p.qn08(), gb->sprite_store.YNEP_STORE9_X6p.qn08(), gb->sprite_store.YZOF_STORE9_X7p.qn08())
  );

  d("\n");


  d("\002=====TileFetcher=====\001\n");
  d("LAXU_BFETCH_S0           %c\n", gb->tile_fetcher.LAXU_BFETCH_S0.c());
  d("MESU_BFETCH_S1           %c\n", gb->tile_fetcher.MESU_BFETCH_S1.c());
  d("NYVA_BFETCH_S2           %c\n", gb->tile_fetcher.NYVA_BFETCH_S2.c());
  d("LYZU_BFETCH_S0_D1        %c\n", gb->tile_fetcher.LYZU_BFETCH_S0_D1.c());
  d("\n");
  d("NYKA_FETCH_DONE_P11      %c\n", gb->tile_fetcher.NYKA_FETCH_DONE_P11.c());
  d("PORY_FETCH_DONE_P12      %c\n", gb->tile_fetcher.PORY_FETCH_DONE_P12.c());
  d("PYGO_FETCH_DONE_P13      %c\n", gb->tile_fetcher.PYGO_FETCH_DONE_P13.c());
  d("POKY_PRELOAD_DONEp       %c\n", gb->tile_fetcher.POKY_PRELOAD_LATCHp.c());
  d("\n");
  d("LONY_FETCH_RUNNINGp      %c\n", gb->tile_fetcher.LONY_BG_FETCH_RUNNINGp.c()); // 1 for phases 0-11, 0 for 12-15
  d("LOVY_FETCH_DONEp         %c\n", gb->tile_fetcher.LOVY_BG_FETCH_DONEp.c());    // 0 for phases 0-11, 1 for 12-15
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

    if (!gb->BOOT_BITn.qp17()) {
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
  if (fb_y >= 0 && fb_y < 144 && fb_x >= 0 && fb_x < 160) {
    memset(overlay, 0, sizeof(overlay));

    for (int x = 0; x < fb_x; x++) {
      uint8_t p0 = gb->lcd_pipe_lo[159 - fb_x + x + 1].qp();
      uint8_t p1 = gb->lcd_pipe_hi[159 - fb_x + x + 1].qp();

      int r = (3 - (p0 + p1 * 2)) * 30 + 50;
      int g = (3 - (p0 + p1 * 2)) * 30 + 50;
      int b = (3 - (p0 + p1 * 2)) * 30 + 30;

      overlay[x + fb_y * 160] = 0xFF000000 | (b << 16) | (g << 8) | (r << 0);
    }
    {
      uint8_t p0 = gb->lcd_pix_lo.qp04();
      uint8_t p1 = gb->lcd_pix_hi.qp04();

      int c = (3 - (p0 + p1 * 2)) * 85;

      overlay[fb_x + fb_y * 160] = 0xFF000000 | (c << 16) | (c << 8) | (c << 0);
    }

    update_texture_u32(overlay_tex, 160, 144, overlay);
    blitter.blit(view, overlay_tex, gb_x, gb_y, 160 * 2, 144 * 2);
  }

  // Status bar under screen

  //double phases_per_frame = 114 * 154 * 60 * 8;
  //double sim_ratio = sim_rate / phases_per_frame;
  double sim_ratio = 0.0;
  double sim_time_smooth = 0.0;

  d("%s %s Sim clock %8.3f %s %c %s\n",
    runmode_names[runmode],
    stepmode_names[stepmode],
    double(phase_total) / (4194304.0 * 2),
    phase_names[phase_total & 7],
    sim_stable ? ' ' : '*',
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
