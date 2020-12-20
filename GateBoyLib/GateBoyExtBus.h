#pragma once
#include "GateBoyLib/Gates.h"

struct GateBoyDMA;
struct GateBoyResetDebug;

//------------------------------------------------------------------------------------------------------------------------

struct ExtDataLatch {
  void reset_to_cart() {
    SOMA_EXT_DATA_LATCH_D0n.reset_to_cart(REG_D1C0);
    RONY_EXT_DATA_LATCH_D1n.reset_to_cart(REG_D1C0);
    RAXY_EXT_DATA_LATCH_D2n.reset_to_cart(REG_D1C0);
    SELO_EXT_DATA_LATCH_D3n.reset_to_cart(REG_D1C0);
    SODY_EXT_DATA_LATCH_D4n.reset_to_cart(REG_D1C0);
    SAGO_EXT_DATA_LATCH_D5n.reset_to_cart(REG_D1C0);
    RUPA_EXT_DATA_LATCH_D6n.reset_to_cart(REG_D1C0);
    SAZY_EXT_DATA_LATCH_D7n.reset_to_cart(REG_D1C0);
  }

  /*p08.SOMA*/ TpLatch SOMA_EXT_DATA_LATCH_D0n; // AxCxExxx
  /*p08.RONY*/ TpLatch RONY_EXT_DATA_LATCH_D1n; // AxCxExxx
  /*p08.RAXY*/ TpLatch RAXY_EXT_DATA_LATCH_D2n; // AxCxExxx
  /*p08.SELO*/ TpLatch SELO_EXT_DATA_LATCH_D3n; // AxCxExxx
  /*p08.SODY*/ TpLatch SODY_EXT_DATA_LATCH_D4n; // AxCxExxx
  /*p08.SAGO*/ TpLatch SAGO_EXT_DATA_LATCH_D5n; // AxCxExxx
  /*p08.RUPA*/ TpLatch RUPA_EXT_DATA_LATCH_D6n; // AxCxExxx
  /*p08.SAZY*/ TpLatch SAZY_EXT_DATA_LATCH_D7n; // AxCxExxx
};

//------------------------------------------------------------------------------------------------------------------------

struct ExtAddrLatch {
  void reset_to_cart() {
    ALOR_EXT_ADDR_LATCH_00p.reset_to_cart(REG_D0C0);
    APUR_EXT_ADDR_LATCH_01p.reset_to_cart(REG_D0C0);
    ALYR_EXT_ADDR_LATCH_02p.reset_to_cart(REG_D0C0);
    ARET_EXT_ADDR_LATCH_03p.reset_to_cart(REG_D0C0);
    AVYS_EXT_ADDR_LATCH_04p.reset_to_cart(REG_D0C0);
    ATEV_EXT_ADDR_LATCH_05p.reset_to_cart(REG_D0C0);
    AROS_EXT_ADDR_LATCH_06p.reset_to_cart(REG_D0C0);
    ARYM_EXT_ADDR_LATCH_07p.reset_to_cart(REG_D0C0);
    LUNO_EXT_ADDR_LATCH_08p.reset_to_cart(REG_D1C0);
    LYSA_EXT_ADDR_LATCH_09p.reset_to_cart(REG_D0C0);
    PATE_EXT_ADDR_LATCH_10p.reset_to_cart(REG_D0C0);
    LUMY_EXT_ADDR_LATCH_11p.reset_to_cart(REG_D0C0);
    LOBU_EXT_ADDR_LATCH_12p.reset_to_cart(REG_D0C0);
    LONU_EXT_ADDR_LATCH_13p.reset_to_cart(REG_D0C0);
    NYRE_EXT_ADDR_LATCH_14p.reset_to_cart(REG_D0C0);
  }

  void copy_cpu_addr_to_addr_latch(const GateBoyResetDebug& rst, Signal BUS_CPU_A[16], wire TEXO_ADDR_VRAMn_ext);

  /*p08.ALOR*/ TpLatch ALOR_EXT_ADDR_LATCH_00p; // xBxxxxxx
  /*p08.APUR*/ TpLatch APUR_EXT_ADDR_LATCH_01p; // xBxxxxxx
  /*p08.ALYR*/ TpLatch ALYR_EXT_ADDR_LATCH_02p; // xBxxxxxx
  /*p08.ARET*/ TpLatch ARET_EXT_ADDR_LATCH_03p; // xBxxxxxx
  /*p08.AVYS*/ TpLatch AVYS_EXT_ADDR_LATCH_04p; // xBxxxxxx
  /*p08.ATEV*/ TpLatch ATEV_EXT_ADDR_LATCH_05p; // xBxxxxxx
  /*p08.AROS*/ TpLatch AROS_EXT_ADDR_LATCH_06p; // xBxxxxxx
  /*p08.ARYM*/ TpLatch ARYM_EXT_ADDR_LATCH_07p; // xBxxxxxx
  /*p08.LUNO*/ TpLatch LUNO_EXT_ADDR_LATCH_08p; // ABxxxxxx
  /*p08.LYSA*/ TpLatch LYSA_EXT_ADDR_LATCH_09p; // ABxxxxxx
  /*p08.PATE*/ TpLatch PATE_EXT_ADDR_LATCH_10p; // ABxxxxxx
  /*p08.LUMY*/ TpLatch LUMY_EXT_ADDR_LATCH_11p; // ABxxxxxx
  /*p08.LOBU*/ TpLatch LOBU_EXT_ADDR_LATCH_12p; // ABxxxxxx
  /*p08.LONU*/ TpLatch LONU_EXT_ADDR_LATCH_13p; // ABxxxxxx
  /*p08.NYRE*/ TpLatch NYRE_EXT_ADDR_LATCH_14p; // ABxxxxxx
};

//------------------------------------------------------------------------------------------------------------------------

struct GateBoyExtBus {
  void reset_to_cart() {
    ext_addr_latch.reset_to_cart();
    ext_data_latch.reset_to_cart();

    PIN01_ADDR[ 0].reset_to_cart(REG_D1C0);
    PIN01_ADDR[ 1].reset_to_cart(REG_D0C0);
    PIN01_ADDR[ 2].reset_to_cart(REG_D1C0);
    PIN01_ADDR[ 3].reset_to_cart(REG_D1C0);
    PIN01_ADDR[ 4].reset_to_cart(REG_D0C0);
    PIN01_ADDR[ 5].reset_to_cart(REG_D0C0);
    PIN01_ADDR[ 6].reset_to_cart(REG_D1C0);
    PIN01_ADDR[ 7].reset_to_cart(REG_D0C0);
    PIN01_ADDR[ 8].reset_to_cart(REG_D0C0);
    PIN01_ADDR[ 9].reset_to_cart(REG_D0C0);
    PIN01_ADDR[10].reset_to_cart(REG_D0C0);
    PIN01_ADDR[11].reset_to_cart(REG_D0C0);
    PIN01_ADDR[12].reset_to_cart(REG_D0C0);
    PIN01_ADDR[13].reset_to_cart(REG_D0C0);
    PIN01_ADDR[14].reset_to_cart(REG_D0C0);
    PIN01_ADDR[15].reset_to_cart(REG_D1C0);

    PIN17_DATA[0].reset_to_cart(REG_D0C0);
    PIN17_DATA[1].reset_to_cart(REG_D1C0);
    PIN17_DATA[2].reset_to_cart(REG_D0C0);
    PIN17_DATA[3].reset_to_cart(REG_D1C0);
    PIN17_DATA[4].reset_to_cart(REG_D0C0);
    PIN17_DATA[5].reset_to_cart(REG_D0C0);
    PIN17_DATA[6].reset_to_cart(REG_D0C0);
    PIN17_DATA[7].reset_to_cart(REG_D0C0);
  }

  void copy_addr_latch_to_pins(
    const GateBoyResetDebug& rst,
    const GateBoyCpuBus& cpu_bus,
    const GateBoyDMA& dma,
    wire ABUZ_EXT_RAM_CS_CLK
  );

  void copy_pins_to_data_latch(const GateBoyCpuBus& cpu_bus);
  void copy_data_latch_to_cpu_bus(GateBoyCpuBus& cpu_bus);
  void copy_cpu_data_to_pins(const GateBoyResetDebug& rst, const GateBoyCpuBus& cpu_bus);

  void set_control_pins(
    const GateBoyResetDebug& rst,
    const GateBoyCpuBus& cpu_bus,
    const GateBoyDMA& dma,
    wire ABUZ_EXT_RAM_CS_CLK);

  void copy_cpu_addr_to_addr_latch(const GateBoyResetDebug& rst, const GateBoyCpuBus& cpu_bus);
  void read_ext_to_pins (const uint8_t* cart_buf, const uint8_t* cart_ram, const uint8_t* ext_ram);
  void write_pins_to_ext(uint8_t* cart_ram, uint8_t* ext_ram);

  void dump(Dumper& d) {
    d.dump_bitp   ("PIN80_CSn  : ", PIN80_CSn.state);
    d.dump_bitp   ("PIN79_RDn  : ", PIN79_RDn.state);
    d.dump_bitp   ("PIN78_WRn  : ", PIN78_WRn.state);
    d.dump_slice2p("ADDR LATCH : ", &ext_addr_latch.ALOR_EXT_ADDR_LATCH_00p, 15);
    d.dump_slice2n("DATA LATCH : ", &ext_data_latch.SOMA_EXT_DATA_LATCH_D0n, 8);
    d.dump_slice2p("PIN01_ADDR : ", PIN01_ADDR, 16);
    d.dump_slice2p("PIN17_DATA : ", PIN17_DATA, 8);
  }

  ExtDataLatch ext_data_latch;
  ExtAddrLatch ext_addr_latch;
  PinOut PIN80_CSn;      // CS changes on phase C if addr in [A000,FDFF]
  PinOut PIN79_RDn;      // RDn idles low, goes high on phase B for an external write
  PinOut PIN78_WRn;      // WRn idles high, goes low during EFG if there's a write
  PinOut PIN01_ADDR[16]; // Pins 01 - 16
  PinIO  PIN17_DATA[8];  // Pins 17 - 24

  // pin17 a << RUXA
  // pin17 b << LULA
  // pin17 c >> TOVO,RALO,SOMA
  // pin17 d << RUNE
};

//------------------------------------------------------------------------------------------------------------------------
