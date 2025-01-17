#pragma once
#include "CoreLib/Types.h"

//-----------------------------------------------------------------------------

#pragma pack(push, 1)

struct CpuState {
  void cpu_bus_pass(uint16_t addr) {
    bus_addr  = addr;
    bus_data  = 0;
    bus_read  = 0;
    bus_write = 0;
  }

  void cpu_bus_read(uint16_t addr)  {
    bus_addr  = addr;
    bus_data  = 0;
    bus_read  = 1;
    bus_write = 0;
  }

  void cpu_bus_write(uint16_t addr, uint8_t data) {
    bus_addr  = addr;
    bus_data  = data;
    bus_read  = 0;
    bus_write = 1;
  }

  void cpu_bus_done(uint16_t addr) {
    bus_addr  = addr;
    bus_data  = 0;
    bus_read  = 1;
    bus_write = 0;
  }

  uint16_t bus_addr;
  uint8_t  bus_data;
  uint8_t  bus_read;
  uint8_t  bus_write;

  uint16_t op_addr;
  uint8_t  op_prev;
  uint8_t  op_next;
  uint8_t  op_cb;
  int      op_state;
  uint8_t  data_in;

  bool     ime, ime_delay;
  uint8_t  int_addr;
  uint8_t  int_ack;

  uint8_t  alu_f;
  uint8_t  alu_o;

#pragma warning(push)
#pragma warning(disable : 4201)
#pragma warning(disable : 26495)

  // right "addr" bus           low side     high side
  union { uint16_t pc; struct { uint8_t pcl; uint8_t pch; }; };
  union { uint16_t sp; struct { uint8_t spl; uint8_t sph; }; };
  union { uint16_t xy; struct { uint8_t xyl; uint8_t xyh; }; };

  // left "data" bus            low side     high side
  union { uint16_t bc; struct { uint8_t   c; uint8_t   b; }; };
  union { uint16_t de; struct { uint8_t   e; uint8_t   d; }; };
  union { uint16_t hl; struct { uint8_t   l; uint8_t   h; }; };
  union { uint16_t af; struct { uint8_t   f; uint8_t   a; }; };

#pragma warning(pop)
};

//-----------------------------------------------------------------------------

struct MetroBoyCPU {
public:

  static FieldInfo fields[];


  void reset_to_bootrom();
  void reset_to_cart();
  void dump(Dumper& d) const;

  /*
  void latch_op(uint8_t _op) {
    if (op_state == 0) {
      op_addr = _bus_addr;
      op_next = _op;
    }
  }

  void check_int(uint8_t _imask, uint8_t _intf) {
    if (op_state == 0) {
      if ((_imask & _intf) && ime) {
        op_next = 0xF4; // fake opcode
        ime = false;
        ime_delay = false;
      }
    }
    int_ack = 0;
    ime = ime_delay; // must be after int check, before op execution
  }
  */

  void execute(uint8_t _imask, uint8_t _intf) {
    if      (reg.op_next == 0xF4) execute_int(_imask, _intf);   // INT
    else if (reg.op_next == 0x76) execute_halt(_imask, _intf);  // HALT
    else if (reg.op_next == 0xCB) execute_cb();                 // CB
    else                            execute_op();
  }

  /*
  void update_halt(uint8_t _imask, uint8_t _intf_halt_latch) {
    if (op_next == 0x76 && (_imask & _intf_halt_latch)) op_state_ = 0;
  }

  void latch_bus_data(uint8_t _data) {
    if (_bus_read) in = _data;
    op_state = op_state_;
  }
  */

  //----------------------------------------

  uint8_t  get_int_ack() const { return reg.int_ack; }
  uint8_t  get_op()      const { return reg.op_next; }
  uint16_t get_op_addr() const { return reg.op_addr; }
  uint8_t  get_a()       const { return reg.a; }
  uint16_t get_sp()      const { return reg.sp; }

  Req get_bus_req() const {
    Req r;
    r.addr  = reg.bus_addr;
    r.data  = reg.bus_data;
    r.read  = reg.bus_read;
    r.write = reg.bus_write;
    return r;
  }

  uint64_t get_hash() const {
    uint64_t hash = HASH_INIT;
    hash = mix(hash ^ reg.pc);
    hash = mix(hash ^ reg.sp);
    hash = mix(hash ^ reg.xy);
    hash = mix(hash ^ reg.bc);
    hash = mix(hash ^ reg.de);
    hash = mix(hash ^ reg.hl);
    hash = mix(hash ^ reg.af);
    hash = mix(hash ^ (uint8_t)reg.ime);
    hash = mix(hash ^ reg.bus_addr);
    hash = mix(hash ^ reg.bus_data);
    hash = mix(hash ^ (uint8_t)reg.bus_read);
    hash = mix(hash ^ (uint8_t)reg.bus_write);
    return hash;
  }

  /*
  uint16_t get_bus_addr()  const { return _bus_addr; }
  uint8_t  get_bus_data()  const { return _bus_data; }
  uint8_t  get_bus_read()  const { return _bus_read; }
  uint8_t  get_bus_write() const { return _bus_write; }
  */

  //----------------------------------------

//private:

  void execute_int(uint8_t imask_, uint8_t intf_);
  void execute_halt(uint8_t imask_, uint8_t intf_);
  void execute_cb();
  void execute_op();


  uint8_t get_reg(int mux);
  void    set_reg(int mux, uint8_t data);
  void    set_f(uint8_t mask);
  uint8_t alu(uint8_t x, uint8_t y, int op, uint8_t flags);
  uint8_t rlu(uint8_t x, int op, uint8_t flags);
  uint8_t daa(uint8_t x, uint8_t f);
  uint8_t alu_cb(uint8_t arg1, int op, uint8_t flags);

  //----------------------------------------

  CpuState reg;
};
#pragma pack(pop)

//-----------------------------------------------------------------------------
