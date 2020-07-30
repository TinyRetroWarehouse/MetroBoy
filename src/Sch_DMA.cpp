#include "Sch_DMA.h"

#include "Sch_Top.h"

using namespace Schematics;

#if 0

// LAVY00 << CUPA04
// LAVY01 << XEDA02
// LAVY02 nc
// LAVY03 >> LORU00

// LYXE00 << LAVY03
// LYXE01 nc
// LYXE02 >> LUPA01
// LYXE03 >> nc
// LYXE04 nc
// LYXE05 << LOKO02

// LAVY = and2(FF46, CUPA)
// NAVO = nand6(dma addr)
// NOLO = not(NAVO)
// LOKO = nand2(CUNU, !LENE) // check neg output?
// LUPA = nor2(LAVY, LYXE)
// LUVY = state
// MATU = state
// LENE = state
// MYTE_DMA_DONE = state
// LYXE = latch(LOKO, LAVY)
// LARA = nand3(LOKY, CUNU, !MYTE_DMA_DONE)
// LOKY = nand2(LARA, !LENE)
// META = and2(UVYT, LOKY)
// LORU = not(LAVY)

// LYXE00 << LAVY03
// LYXE01 nc
// LYXE02 >> LUPA01
// LYXE03 >> nc
// LYXE04 nc
// LYXE05 << LOKO02

// LUPA00 << LAVY
// LUPA01 << LYXE
// LUPA02 >> LUVY

// LOGO = not(MUDA);
// MORY = nand(MATU17, LOGO) MATU17 must be Q
// LUMA = not(MORY);

// Die trace:
// LEBU = not(MARU06)
// MUDA = nor(PULA06, POKU06, LEBU);

// if rung 6 of MARU/PULA/POKU was QN:
// MUDA = and(A13, A14, !A15);
// would select last quarter of ROM, which doesn't make sense
// so rung 6 of MARU must be Q.

// schematic incorrect.
// lyxe - weird gate - lavy, loko
// lupa - nor - lavy, lyxe

//----------------------------------------

// Arms on ground, NOR latch
// LYXE00 << LAVY03
// LYXE01 nc
// LYXE02 >> LUPA01
// LYXE03 >> nc
// LYXE04 nc
// LYXE05 << LOKO02

// if LOKO02 goes high, LYXE02 goes high.
// if LAVY03 goes high, LYXE02 goes low.

#endif

//------------------------------------------------------------------------------

void DmaRegisters::tick(const SchematicTop& /*top*/) {
}

//------------------------------------------------------------------------------

void DmaRegisters::tock(const SchematicTop& top, CpuBus& cpu_bus) {
  
  /*p22.WATE*/ wire _WATE_FF46n = nand(top.cpu_bus.WERO_FF4Xp(), top.cpu_bus.XOLA_A00n(), top.cpu_bus.WESA_A01p(), top.cpu_bus.WALO_A02p(), top.cpu_bus.XERA_A03n());
  /*p22.XEDA*/ wire _XEDA_FF46p = not(_WATE_FF46n);
  /*p04.MOLU*/ wire _MOLU_FF46_RDp = and(_XEDA_FF46p, top.ASOT_CPU_RDp());
  /*p04.LAVY*/ wire _LAVY_FF46_WRp = and(_XEDA_FF46p, top.CUPA_CPU_WRp_xxxxEFGx());
  /*p04.LOKO*/ wire _LOKO_DMA_RSTp = nand(top.clk_reg.CUNU_SYS_RSTn(), !_LENE_DMA_TRIG_d4.q());

  {
    /*p04.LYXE*/ _LYXE_DMA_LATCHn = nor_latch(_LOKO_DMA_RSTp, _LAVY_FF46_WRp);
    /*p04.LUPA*/ wire _LUPA_DMA_TRIG = nor(_LAVY_FF46_WRp, _LYXE_DMA_LATCHn);
    /*p04.LUVY*/ _LUVY_DMA_TRIG_d0 = dff17(top.clk_reg.UVYT_ABCDxxxx(), top.clk_reg.CUNU_SYS_RSTn(), _LUPA_DMA_TRIG);
    /*p04.LENE*/ _LENE_DMA_TRIG_d4 = dff17(top.clk_reg.MOPA_xxxxEFGH(), top.clk_reg.CUNU_SYS_RSTn(), _LUVY_DMA_TRIG_d0.q());
  }

  {
    // NAND latch
    /*p04.LOKY*/ _LOKY_DMA_LATCHp = nand_latch(
      _LENE_DMA_TRIG_d4.qn(),
      and(top.clk_reg.CUNU_SYS_RSTn(), _MYTE_DMA_DONE.qn())
    ); 

    /*p04.MATU*/ _MATU_DMA_RUNNINGp = dff17(top.clk_reg.UVYT_ABCDxxxx(), top.clk_reg.CUNU_SYS_RSTn(), _LOKY_DMA_LATCHp);
  }

  {
    /*p04.LAPA*/ wire _LAPA_DMA_RSTn = not(_LOKO_DMA_RSTp);
    /*p04.NAVO*/ wire _NAVO_DMA_DONEn = nand(NAKY_DMA_A00.q(), PYRO_DMA_A01.q(), NEFY_DMA_A02.q(), MUTY_DMA_A03.q(), NYKO_DMA_A04.q(), MUGU_DMA_A07.q()); // 128+16+8+4+2+1 = 159, this must be "dma done"
    /*p04.NOLO*/ wire _NOLO_DMA_DONEp = not(_NAVO_DMA_DONEn);

    /*p04.MYTE*/ _MYTE_DMA_DONE = dff17(top.clk_reg.MOPA_xxxxEFGH(), _LAPA_DMA_RSTn, _NOLO_DMA_DONEp);

    /*p04.META*/ wire _META_DMA_CLKp = and(top.clk_reg.UVYT_ABCDxxxx(), _LOKY_DMA_LATCHp);
    /*p04.NAKY*/ NAKY_DMA_A00 = dff17(_META_DMA_CLKp, _LAPA_DMA_RSTn, !NAKY_DMA_A00.q());
    /*p04.PYRO*/ PYRO_DMA_A01 = dff17(NAKY_DMA_A00.qn(),  _LAPA_DMA_RSTn, PYRO_DMA_A01.qn());
    /*p04.NEFY*/ NEFY_DMA_A02 = dff17(PYRO_DMA_A01.qn(),  _LAPA_DMA_RSTn, NEFY_DMA_A02.qn());
    /*p04.MUTY*/ MUTY_DMA_A03 = dff17(NEFY_DMA_A02.qn(),  _LAPA_DMA_RSTn, MUTY_DMA_A03.qn());
    /*p04.NYKO*/ NYKO_DMA_A04 = dff17(MUTY_DMA_A03.qn(),  _LAPA_DMA_RSTn, NYKO_DMA_A04.qn());
    /*p04.PYLO*/ PYLO_DMA_A05 = dff17(NYKO_DMA_A04.qn(),  _LAPA_DMA_RSTn, PYLO_DMA_A05.qn());
    /*p04.NUTO*/ NUTO_DMA_A06 = dff17(PYLO_DMA_A05.qn(),  _LAPA_DMA_RSTn, NUTO_DMA_A06.qn());
    /*p04.MUGU*/ MUGU_DMA_A07 = dff17(NUTO_DMA_A06.qn(),  _LAPA_DMA_RSTn, MUGU_DMA_A07.qn());
  }

  // FF46 DMA
  {
    /*p04.NYGO*/ wire _NYGO_FF46_RDn = not(_MOLU_FF46_RDp);
    /*p04.PUSY*/ wire _PUSY_FF46_RDp = not(_NYGO_FF46_RDn);
    /*p04.POLY*/ cpu_bus.CPU_BUS_D0 = tribuf_6p(_PUSY_FF46_RDp, NAFA_DMA_A08.q());
    /*p04.ROFO*/ cpu_bus.CPU_BUS_D1 = tribuf_6p(_PUSY_FF46_RDp, PYNE_DMA_A09.q());
    /*p04.REMA*/ cpu_bus.CPU_BUS_D2 = tribuf_6p(_PUSY_FF46_RDp, PARA_DMA_A10.q());
    /*p04.PANE*/ cpu_bus.CPU_BUS_D3 = tribuf_6p(_PUSY_FF46_RDp, NYDO_DMA_A11.q());
    /*p04.PARE*/ cpu_bus.CPU_BUS_D4 = tribuf_6p(_PUSY_FF46_RDp, NYGY_DMA_A12.q());
    /*p04.RALY*/ cpu_bus.CPU_BUS_D5 = tribuf_6p(_PUSY_FF46_RDp, PULA_DMA_A13.q());
    /*p04.RESU*/ cpu_bus.CPU_BUS_D6 = tribuf_6p(_PUSY_FF46_RDp, POKU_DMA_A14.q());
    /*p04.NUVY*/ cpu_bus.CPU_BUS_D7 = tribuf_6p(_PUSY_FF46_RDp, MARU_DMA_A15.q());

    /*p04.LORU*/ wire _LORU_FF46_WRn = not(_LAVY_FF46_WRp);
    /*p??.PYSU*/ wire _PYSU_FF46_WRp = not(_LORU_FF46_WRn); // not on schematic

    /*p04.NAFA*/ NAFA_DMA_A08 = dff8(_LORU_FF46_WRn, _PYSU_FF46_WRp, top.cpu_bus.CPU_BUS_D0);
    /*p04.PYNE*/ PYNE_DMA_A09 = dff8(_LORU_FF46_WRn, _PYSU_FF46_WRp, top.cpu_bus.CPU_BUS_D1);
    /*p04.PARA*/ PARA_DMA_A10 = dff8(_LORU_FF46_WRn, _PYSU_FF46_WRp, top.cpu_bus.CPU_BUS_D2);
    /*p04.NYDO*/ NYDO_DMA_A11 = dff8(_LORU_FF46_WRn, _PYSU_FF46_WRp, top.cpu_bus.CPU_BUS_D3);
    /*p04.NYGY*/ NYGY_DMA_A12 = dff8(_LORU_FF46_WRn, _PYSU_FF46_WRp, top.cpu_bus.CPU_BUS_D4);
    /*p04.PULA*/ PULA_DMA_A13 = dff8(_LORU_FF46_WRn, _PYSU_FF46_WRp, top.cpu_bus.CPU_BUS_D5);
    /*p04.POKU*/ POKU_DMA_A14 = dff8(_LORU_FF46_WRn, _PYSU_FF46_WRp, top.cpu_bus.CPU_BUS_D6);
    /*p04.MARU*/ MARU_DMA_A15 = dff8(_LORU_FF46_WRn, _PYSU_FF46_WRp, top.cpu_bus.CPU_BUS_D7);
  }
}

//-----------------------------------------------------------------------------
