#include "MetroBoy.h"
#include "Audio.h"
#include <assert.h>

extern const int op_sizes[];

//=============================================================================

void MetroBoy::load_rom(const char* filename, bool run_bootrom) {
  load_array(filename, rom);
  current->gb.set_rom(rom.data(), rom.size());
  current->gb.reset(run_bootrom ? 0x0000 : 0x0100);
}

//-----------------------------------------------------------------------------

void MetroBoy::reset(uint16_t new_pc) {
  clear();
  memset(tracebuffer, 0, sizeof(tracebuffer));
  current->gb.reset(new_pc);
}

//-----------------------------------------------------------------------------

void MetroBoy::run_fast(uint8_t buttons, int mcycles) {
  clear();

  auto& gb = current->gb;

  gb.set_joypad(~buttons);

  gb.sync_to_mcycle();
  for (int cycle = 0; cycle < mcycles; cycle++) {
    gb.mcycle();
  }
}

//-------------------------------------

void MetroBoy::run_vsync(uint8_t buttons) {
  clear();

#if 0
  current->gb.set_joypad(~buttons);

  gb.sync_to_mcycle();

  while(gb_out.y != 144) {
    gb.mcycle();
  }

  audio_begin();

  for (int i = 0; i < 154 * 114; i++) {
    gb.mcycle();
    audio_post(current->gb.get_host_data().out_l,
               current->gb.get_host_data().out_r);
  }

  audio_end();
#endif
}

//-------------------------------------

void MetroBoy::run_to(uint16_t breakpoint) {
  clear();

#if 0
  gb.sync_to_mcycle();
  while (1) {
    uint16_t op_addr = current->gb.get_cpu().get_op_addr();
    if (op_addr == breakpoint) break;
    gb.mcycle();
  }
#endif
}

//-----------------------------------------------------------------------------

void MetroBoy::step_frame() {
#if 0
  push_frame();
  
  gb.sync_to_mcycle();

  do {
    mcycle();
  } while (gb_out.y == 144);

  do {
    mcycle();
  } while (gb_out.y != 144);
#endif
}

void MetroBoy::step_line() {
#if 0
  push_line();

  gb.sync_to_mcycle();

  int line = gb_out.y;
  do {
    gb.mcycle();
  } while (gb_out.y == line);
#endif
}

void MetroBoy::step_phase() {
  push_cycle();
  current->gb.halfcycle();
}

void MetroBoy::step_over() {
#if 0
  push_cycle();

  int op_addr = current->gb.get_cpu().get_op_addr();
  int op = rom_buf[op_addr];
  int op_size = op_sizes[op];
  if (op == 0xcb) op_size = 2;

  int next_op_addr = op_addr + op_size;

  gb.sync_to_mcycle();
  int i = 0;
  for (; i < 1000000; i++) {
    if (current->gb.get_cpu().get_op_addr() == next_op_addr) {
      // step succeeded
      return;
    }
    gb.mcycle();
  }

  // step failed
  pop_cycle();
  return;
#endif
}

//-----------------------------------------------------------------------------

/*
void MetroBoy::halfcycle() {
  if ((current->gb.phase1 & 1) == 1) {
  }
  else {
    //gb_out = current->gb.get_host_data();

    //tracebuffer[gb_out.y * 456 + gb_out.counter] = gb_out.trace;

    tracebuffer[tracecursor++] = gb_out.trace;
    if (tracecursor == 456 * 154) tracecursor = 0;

    if (gb_out.pix_oe) {
      int x = gb_out.x - 1;
      int y = gb_out.y;

      if (x >= 0 && x < 160 && y >= 0 && y < 144) {
        current->fb[x + y * 160] = gb_out.pix;
      }
    }

    current->gb.check_sentinel();
  }
}
*/

//-----------------------------------------------------------------------------
