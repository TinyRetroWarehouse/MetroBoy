#include "Sch_Merged.h"

#include "Sch_Common.h"
#include "Sch_Pins.h"
#include "TestGB.h"
#include "Constants.h"

#pragma warning(disable:4100) // unreferenced formal parameter

using namespace Schematics;

/*
0b000xxxxx_xxxxxxxx - 0x0000 to 0x1FFF (cart rom)
0b001xxxxx_xxxxxxxx - 0x2000 to 0x3FFF (cart rom)
0b010xxxxx_xxxxxxxx - 0x4000 to 0x5FFF (cart rom banked)
0b011xxxxx_xxxxxxxx - 0x6000 to 0x7FFF (cart rom banked)
0b100xxxxx_xxxxxxxx - 0x8000 to 0x9FFF (vram)
0b101xxxxx_xxxxxxxx - 0xA000 to 0xBFFF (cart ram)

0b110xxxxx_xxxxxxxx - 0xC000 to 0xDFFF (internal ram)
0b111xxxxx_xxxxxxxx - 0xE000 to 0xFFFF (echo ram, oam, high ram, io)

0b11111110_00000000 - 0xFE00 - OAM begin
0b11111110_10011111 - 0xFE9F - OAM end

0b11111111_00000000 - 0xFF00 - IO begin
0b11111111_01111111 - 0xFF7F - IO end

0b11111111_10000000 - 0xFF80 - Zeropage begin
0b11111111_11111110 - 0xFFFE - Zeropage end
0b11111111_11111111 - 0xFFFF - Interrupt enable
*/

TestGB gb;

//-----------------------------------------------------------------------------

ResetSignals ResetSignals::get(const TestGB& test_gb) {
  /*p01.AVOR*/ wire AVOR_RSTp = or (test_gb.rst_reg.RESET_REGp.q(), test_gb.rst_reg.ASOL_RST_LATCHp.q());
  /*p23.XONA*/ wire XONA_LCDC_EN = test_gb.cfg_reg.XONA_LCDC_EN.q();

  /*p01.ALUR*/ wire ALUR_RSTn = not(AVOR_RSTp);   // this goes all over the place
  /*p01.DULA*/ wire DULA_RSTp = not(ALUR_RSTn);
  /*p01.CUNU*/ wire CUNU_RSTn = not(DULA_RSTp);
  /*p01.XORE*/ wire XORE_RSTp = not(CUNU_RSTn);
  /*p01.XEBE*/ wire XEBE_RSTn = not(XORE_RSTp);
  /*p01.WESY*/ wire WESY_RSTn = not(XORE_RSTp);

  /*p01.XODO*/ wire XODO_VID_RSTp = nand(XEBE_RSTn, XONA_LCDC_EN);
  /*p01.XAPO*/ wire XAPO_VID_RSTn = not(XODO_VID_RSTp);
  /*p01.PYRY*/ wire PYRY_VID_RSTp = not(XAPO_VID_RSTn);
  /*p01.TOFU*/ wire TOFU_VID_RSTp = not(XAPO_VID_RSTn);
  /*p01.LYHA*/ wire LYHA_VID_RSTp = not(XAPO_VID_RSTn);
  /*p01.LYFE*/ wire LYFE_VID_RSTn = not(LYHA_VID_RSTp);
  /*p01.ATAR*/ wire ATAR_VID_RSTp = not(XAPO_VID_RSTn);
  /*p01.ABEZ*/ wire ABEZ_VID_RSTn = not(ATAR_VID_RSTp);

  return {
    .ALUR_RSTn = ALUR_RSTn,
    .DULA_RSTp = DULA_RSTp,
    .CUNU_RSTn = CUNU_RSTn,
    .XORE_RSTp = XORE_RSTp,
    .XEBE_RSTn = XEBE_RSTn,
    .WESY_RSTn = WESY_RSTn,

    .XODO_VID_RSTp = XODO_VID_RSTp,
    .XAPO_VID_RSTn = XAPO_VID_RSTn,
    .PYRY_VID_RSTp = PYRY_VID_RSTp,
    .TOFU_VID_RSTp = TOFU_VID_RSTp,
    .LYHA_VID_RSTp = LYHA_VID_RSTp,
    .LYFE_VID_RSTn = LYFE_VID_RSTn,
    .ATAR_VID_RSTp = ATAR_VID_RSTp,
    .ABEZ_VID_RSTn = ABEZ_VID_RSTn,
  };
}

//-----------------------------------------------------------------------------

AddressSignals AddressSignals::get(const CpuPins& cpu_pins) {
  /*p03.TOVY*/ wire TOVY_A00n = not(cpu_pins.A00);
  /*p08.TOLA*/ wire TOLA_A01n = not(cpu_pins.A01);
  /*p06.SEFY*/ wire SEFY_A02n = not(cpu_pins.A02);
  /*p07.TONA*/ wire TONA_A08n = not(cpu_pins.A08);
  /*p07.TUNA*/ wire TUNA_CPU_ADDR_0000_FDFF = nand(cpu_pins.A15, cpu_pins.A14, cpu_pins.A13, cpu_pins.A12, cpu_pins.A11, cpu_pins.A10, cpu_pins.A09);
  /*p06.SARE*/ wire SARE_XX00_XX07p = nor(cpu_pins.A07, cpu_pins.A06, cpu_pins.A05, cpu_pins.A04, cpu_pins.A03);
  /*p07.SYKE*/ wire SYKE_FFXXp = nor(TUNA_CPU_ADDR_0000_FDFF, TONA_A08n);

  return {
    /*p03.TOVY*/ .TOVY_A00n               = TOVY_A00n,
    /*p08.TOLA*/ .TOLA_A01n               = TOLA_A01n,
    /*p06.SEFY*/ .SEFY_A02n               = SEFY_A02n,
    /*p07.TONA*/ .TONA_A08n               = TONA_A08n,
    /*p07.TUNA*/ .TUNA_CPU_ADDR_0000_FDFF = TUNA_CPU_ADDR_0000_FDFF,
    /*p06.SARE*/ .SARE_XX00_XX07p         = SARE_XX00_XX07p, 
    /*p07.SYKE*/ .SYKE_FFXXp              = SYKE_FFXXp,         

  };
}

//-----------------------------------------------------------------------------

DebugSignals DebugSignals::get(const TestGB& test_gb) {
  /*p07.UBET*/ wire UBET_T1n = not(test_gb.sys_pins.T1);
  /*p07.UVAR*/ wire UVAR_T2n = not(test_gb.sys_pins.T2);
  /*p07.UMUT*/ wire UMUT_MODE_DBG1 = and (test_gb.sys_pins.T1, UVAR_T2n);
  /*p07.UNOR*/ wire UNOR_MODE_DBG2n = and (test_gb.sys_pins.T2, UBET_T1n);
  /*p08.TOVA*/ wire TOVA_MODE_DBG2p = not(UNOR_MODE_DBG2n);
  /*p07.UPOJ*/ wire UPOJ_MODE_PRODn = nand(UBET_T1n, UVAR_T2n, test_gb.sys_pins.RST);
  /*p08.RYCA*/ wire RYCA_MODE_DBG2p = not(UNOR_MODE_DBG2n);


  return {
    UBET_T1n,
    UVAR_T2n,
    UMUT_MODE_DBG1,
    UNOR_MODE_DBG2n,
    TOVA_MODE_DBG2p,
    UPOJ_MODE_PRODn,
    RYCA_MODE_DBG2p,
  };
}

//-----------------------------------------------------------------------------

#if 0

//Die trace:
ABOL = not(CLKREQ)
ATEZ = not(CLKIN_A)

ADAR = not(ADYK.qn)
ATYP = not(AFUR.q);
AROV = not(APUK.qn);
AFEP = not(ALEF.qn);

AFAS = nor(ADAR, ATYP)
AREV = nand(AFAS, CPU_RAW_WR)
APOV = not(AREV) // 4 rung inverter


APAP = not(CPU_ADDR_VALID)
AWOD = nor(UNOR, APAP)
ABUZ = not(AWOD)

AGUT = ? ? ? (AROV, AJAX) - gate unused ?
APAP = not(CPU_ADDR_VALID)
AJAX = not(ATYP)
ALUR = not(AVOR) // 3 rung inverter
AVOR = or (AFER, ASOL)

AFER = reg, gap above it or something.starting at the first connected rung

// Because this has BOGA and BOMA both as inputs and two UPOJ inputs, maybe
// it's supposed to be a clock-crossing synchronizer?

AFER00 << UPOJ
AFER01 << ASOL
AFER02 nc
AFER03 << BOGA
AFER04 nc
AFER05 nc
AFER06 << BOMA
AFER06 << UPOJ
AFER07 nc
AFER08 nc
AFER09 >> nc     // QN
AFER10 >> AVOR   // Q

ASOL = nor_latch(<< AFAR, nc, >> AVOR, nc, nc, << PIN_RST) - output inverted

AFAR = nor(PIN_RST, ALYP)
ALYP = not(TABA)
ADAR = not(ADYK)


// Hax

/*p01.ADYK*/ clk_reg.ADYK_PHAZ_xxxxEFGH
/*p01.AFUR*/ clk_reg.AFUR_PHAZ_xBCDExxx

ABOL = not(CLKREQ)
ATEZ = not(CLKIN_A)
APOV = and(ADYK_xxxxEFGH, AFUR_xBCDExxx, CPU_RAW_WR) // cpu write is _only_ on E?

ABUZ = or(UNOR, !CPU_ADDR_VALID)

AGUT = ? ? ? (AROV, AJAX) - gate unused ?
AJAX = AFUR
ALUR = nor(AFER_RST, ASOL_RST_LATCH)



#endif

void TestGB::tick_everything() {
  auto clk_sig = ClockSignals::get(*this);

  //----------------------------------------
  // sch_system

  //----------------------------------------
  // sch_clocks

  {
    wire FROM_CPU5 = gb.cpu_pins.FROM_CPU5;
    /*p04.MAKA*/ FROM_CPU5_SYNC.set(clk_sig.ZEME_AxCxExGx, CUNU_RSTn(), FROM_CPU5);
  }


#if 0
  // if rung 6 of AFUR/ALEF/APUK/ADYK is QN and not Q...

  ADYK = !APUK
  APUK = !ALEF
  ALEF = !AFUR
  AFUR = ADYK

  ADYK APUK ALEF AFUR
  0    0    0    0
  1    1    1    0
  0    0    1    1
  1    0    0    0
  1    1    1    0

  // yeah doesn't work or make sense.
#endif


  {
    // wave ram write clock
    wave_pins.BORY_ABxxxxxH.set(clk_sig.BORY_ABxxxxxH);
  }

  //----------------------------------------
  // sch_resets

  {

  }


  {
    // Latch w/ arms on the ground side, output on the top rung - nor latch with inverted output
    // TUBO00 << cpu_pins.CLKREQ
    // TUBO01 nc
    // TUBO02 >> UNUT 
    // TUBO03 == nc
    // TUBO04 nc
    // TUBO05 << UPYF
    /*p01.UCOB*/ wire UCOB_CLKBAD = not(sys_pins.CLK_GOOD);
    /*p01.UPYF*/ wire UPYF = or (sys_pins.RST, UCOB_CLKBAD);
    /*p01.TUBO*/ rst_reg.TUBO_CLKREQn_LATCH.nor_latch(cpu_pins.CLKREQ, UPYF);
  }

  //----------------------------------------
  // sch_clocks

  {
  }

  {
    /*p07.UBET*/ wire UBET_T1n       = not(sys_pins.T1);
    /*p07.UNOR*/ wire UNOR_MODE_DBG2 = and(sys_pins.T2, UBET_T1n);
    /*p25.SYCY*/ wire SYCY_DBG_CLOCK = not(UNOR_MODE_DBG2);
    /*p25.SOTO*/ bus_reg.SOTO_DBG.set(SYCY_DBG_CLOCK, CUNU_RSTn(), !bus_reg.SOTO_DBG);
  }

  //----------------------------------------

  clk_reg.tick(*this);
  dma_reg.tick(*this);
  tick_timer();
  ser_reg.tick(*this);
  tick_joypad();
  tick_ppu();
  sst_reg.tick(*this);
  tick_lcd();

  tick_pixpipe();
  tick_cpu();
  tick_vram_addr();

  tick_bootrom();
  tick_cart_data();
  tick_cart_pins();

  tick_oam();
}

//-----------------------------------------------------------------------------
