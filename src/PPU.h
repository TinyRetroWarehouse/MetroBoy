#pragma once
#include <stdint.h>
#include "OAM.h"
#include "VRAM.h"
#include "Constants.h"

//-----------------------------------------------------------------------------

struct PPU {
  void reset(bool run_bootrom, int new_model);
  PpuOut tick(int tphase) const;
  void tock(int tphase, CpuBus cpu_bus, VRAM::Out vram_out, OAM::Out oam_out);

  void dump(std::string& out);

  uint8_t get_stat()       const { return stat; }

  void dump_tiles(uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* vram) const;
  void draw_sprite(OAM& oam, uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* vram, int sprite_index) const;
  void draw_map(uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* map, const uint8_t* tiles, uint8_t map_flip) const;
  void draw_bg_map(uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* vram) const;
  void draw_wm_map(uint32_t* framebuffer, int stride, int sx, int sy, int scale, const uint8_t* vram) const;

  bool get_old_stat_int1() const { return old_stat_int2; }
  bool get_old_stat_int2() const { return old_stat_int2; }

private:

  void tock_lcdoff(int tphase, CpuBus cpu_bus, VRAM::Out vram_out, OAM::Out oam_in);
  void emit_pixel(int tphase);
  void merge_tile(int tphase);

  void bus_read_early(uint16_t cpu_addr);
  void bus_read_late(uint16_t cpu_addr);
  void bus_write_early(uint16_t cpu_addr, uint8_t cpu_data);
  void bus_write_late(uint16_t cpu_addr, uint8_t cpu_data);

  PpuOut out;

  int model;

  uint8_t stat_int1;
  uint8_t stat_int2;

  uint8_t bus_out;
  bool bus_oe;

  uint8_t pix_out;
  bool pix_oe;

  bool vram_lock;
  uint16_t vram_addr;

  bool oam_lock;

  //----------
  // Registers

  uint8_t lcdc; // FF40
  uint8_t stat; // FF41
  uint8_t scy;  // FF42
  uint8_t scx;  // FF43
  uint8_t ly;   // FF44
  uint8_t lyc;  // FF45
  uint8_t dma;  // FF46
  uint8_t bgp;  // FF47
  uint8_t obp0; // FF48
  uint8_t obp1; // FF49
  uint8_t wy;   // FF4A
  uint8_t wx;   // FF4B

  uint8_t bgp_early;

  uint8_t palettes[4];


  uint8_t scx_latch;
  uint8_t win_y_latch;
  uint8_t win_y_counter;

  //----------
  // Timers and states

  int state;

  uint16_t counter;
  uint16_t counter_delay1;
  uint16_t counter_delay2;
  uint16_t counter_delay3;

  uint8_t line;
  uint8_t line_delay1;
  uint8_t line_delay2;
  uint8_t line_delay3;

  bool frame_start;
  bool frame_done;
  int frame_count;

  uint8_t hblank_delay2;

  int compare_line;

  bool fire_int_stat1;
  bool fire_int_stat2;
  bool fire_int_vblank1;
  bool fire_int_vblank2;

  bool old_stat_int1;
  bool old_stat_int2;

  //----------
  // Sprites

  uint8_t sprite_count;
  int8_t sprite_index;
  int sprite_hit;
  uint8_t sprite_x[10]; // 80 bits
  uint8_t sprite_y[10]; // 80 bits
  uint8_t sprite_i[10]; // 60 bits?

  uint8_t spriteY;
  uint8_t spriteX;
  uint8_t spriteP;
  uint8_t spriteF;

  uint8_t sprite_lo;
  uint8_t sprite_hi;

  //----------
  // Vram Fetcher

  enum FetchType {
    FETCH_NONE,
    FETCH_BACKGROUND,
    FETCH_WINDOW,
    FETCH_SPRITE,
  };

  enum FetchState {
    FETCH_IDLE,
    FETCH_MAP,
    FETCH_LO,
    FETCH_HI,
  };

  FetchType fetch_type;
  FetchState fetch_state;
  bool fetch_delay;

  bool in_window_old;
  bool in_window_new;
  bool in_window_new_early;
  bool in_window_late;

  bool window_retrigger_old;
  bool window_retrigger_new;

  //----------
  // Pixel pipe

  uint8_t map_x;

  uint8_t tile_map;
  uint8_t tile_lo;
  uint8_t tile_hi;
  bool    tile_latched;

  int pix_count2;
  int pix_discard_scx;
  int pix_discard_pad;
  uint8_t pipe_count;

  uint8_t bg_pix_lo;
  uint8_t bg_pix_hi;
  uint8_t bg_pal_lo;
  uint8_t bg_pal_hi;

  uint8_t ob_pix_lo;
  uint8_t ob_pix_hi;
  uint8_t ob_pal_lo;
  uint8_t ob_pal_hi;
};

//-----------------------------------------------------------------------------