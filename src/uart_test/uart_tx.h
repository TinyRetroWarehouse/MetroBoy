#pragma once
#include "metron.h"

//==============================================================================

template<int cycles_per_bit = 4>
struct uart_tx {




  //----------------------------------------

  logic<8> i_data;
  bit i_req;

  bit o_serial;
  bit o_cts;
  bit o_idle;

  /*verilator public_module*/

  //----------------------------------------
  // 1 start bit, 8 data bits, 1 stop bit, 7 additional stop bits to guarantee
  // that recevier can resync between messages

  static const uint32_t extra_stop_bits = 7;

  static const uint32_t cycle_bits = clog2(cycles_per_bit);
  static const uint32_t cycle_max  = cycles_per_bit - 1;

  static const uint32_t cursor_bits = clog2(10 + extra_stop_bits);
  static const uint32_t cursor_max  = 10 + extra_stop_bits - 1;

  logic<cycle_bits> cycle;
  logic<cursor_bits> cursor;
  logic<9> buffer;

  //----------------------------------------

  void tick(bool rst_n) {
    o_serial = buffer & 1;
    o_cts    = ((cursor == extra_stop_bits) && (cycle == 0)) || (cursor < extra_stop_bits);
    o_idle   = (cursor == 0) && (cycle == 0);
  }

  //----------------------------------------

  void tock(bool rst_n) {
    if (!rst_n) {
      cycle  = 0;
      cursor = 0;
      buffer = 0x1FF;
    } else {
      if (cursor <= extra_stop_bits && cycle == 0 && i_req) {
        // Transmit start
        cycle  = cycle_max;
        cursor = cursor_max;
        buffer = i_data << 1;
      } else if (cycle != 0) {
        // Bit delay
        cycle  = cycle - 1;
        cursor = cursor;
        buffer = buffer;
      } else if (cursor != 0) {
        // Bit delay done, switch to next bit.
        cycle  = cycle_max;
        cursor = cursor - 1;
        buffer = (buffer >> 1) | 0x100;
      }
    }
  }
};

//==============================================================================
