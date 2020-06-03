#pragma once
#include "Types.h"

//-----------------------------------------------------------------------------

struct Z80 {
  void    reset(uint16_t new_pc);

  uint8_t get_int_ack() const { return int_ack; }
  void    get_bus_req(Req& r) const;

  void    tick_a(const uint8_t imask_, const uint8_t intf_, const Ack& ack);
  void    tock_b(const uint8_t imask_, const uint8_t intf_, const Ack& ack);

  void    tick_c(const uint8_t imask_, const uint8_t intf_, const Ack& ack);
  void    tock_d(const uint8_t imask_, const uint8_t intf_, const Ack& ack);

  void    tick_e(const uint8_t imask_, const uint8_t intf_, const Ack& ack);
  void    tock_f(const uint8_t imask_, const uint8_t intf_, const Ack& ack);

  void    tick_g(const uint8_t imask_, const uint8_t intf_, const Ack& ack);
  void    tock_h(const uint8_t imask_, const uint8_t intf_, const Ack& ack);

  void    dump(std::string& d);

  uint16_t get_op_addr() const { return op_addr; }
  uint8_t  get_a()  const { return a; }
  uint8_t  get_op() const { return op; }
  int      get_state() const { return state; }
  

//private:
  int      state = 0, state_ = 0;
  uint16_t op_addr = 0;
  bool     unhalt = 0;
  uint8_t  op = 0;
  uint8_t  cb = 0;
  bool     ime = 0, ime_delay = 0;

  uint8_t  in = 0;
  uint8_t  out = 0;

  Req      bus_req;
  Ack      bus_ack;

  uint8_t alu_x = 0;
  uint8_t alu_y = 0;
  uint8_t alu_o = 0;
  uint8_t alu_f = 0;

  uint8_t inc_x = 0;
  uint8_t inc_y = 0;
  uint8_t inc_o = 0;
  uint8_t inc_c = 0;

  uint8_t inc(uint8_t ix, uint8_t iy) {
    inc_x = ix;
    inc_y = iy;
    inc_o = inc_x + inc_y;
    inc_c = (inc_o == 0x00);
    return inc_o;
  }

  uint8_t dec(uint8_t ix, uint8_t iy) {
    inc_x = ix;
    inc_y = iy;
    inc_o = inc_x - inc_y;
    inc_c = (inc_o == 0xFF);
    return inc_o;
  }

  uint8_t  int_ack = 0;

#pragma warning(push)
#pragma warning(disable : 4201)
#pragma warning(disable : 26495)

  // left "data" bus              low side     high side
  union { uint16_t bc = 0;     struct { uint8_t   c; uint8_t   b; }; };
  union { uint16_t de = 0;     struct { uint8_t   e; uint8_t   d; }; };
  union { uint16_t hl = 0;     struct { uint8_t   l; uint8_t   h; }; };
  union { uint16_t af = 0;     struct { uint8_t   f; uint8_t   a; }; };
                           
  // right "addr" bus      
  union { uint16_t xy = 0;     struct { uint8_t   y; uint8_t   x; }; };
  union { uint16_t pc = 0;     struct { uint8_t pcl; uint8_t pch; }; };
  union { uint16_t sp = 0;     struct { uint8_t spl; uint8_t sph; }; };
  union { uint16_t ad = 0;     struct { uint8_t adl; uint8_t adh; }; };

#pragma warning(pop)

  void set_addr(uint16_t new_addr, int new_write);
  uint8_t& reg(int mux);

  void     set_f(uint8_t mask);
  uint8_t  alu_cb(int op, uint8_t flags);
  uint8_t  alu(int op, uint8_t flags);
  uint8_t  rlu(int op, uint8_t flags);
  uint8_t  daa(uint8_t x, uint8_t f);
};

//-----------------------------------------------------------------------------
