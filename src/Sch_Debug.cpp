#include "Sch_Debug.h"

#include "Sch_Top.h"

using namespace Schematics;

//-----------------------------------------------------------------------------

void DebugRegisters::tick(const SchematicTop& top) {
  /*p25.SYCY*/ nwire SYCY_DBG_CLOCKn = not(top.UNOR_MODE_DBG2p());
  /*p01.ALUR*/ nwire ALUR_RSTn = not(top.AVOR_RSTp());
  /*p01.DULA*/ pwire DULA_RSTp = not(ALUR_RSTn);
  /*p01.CUNU*/ nwire CUNU_RSTn = not(DULA_RSTp);
  /*p25.SOTO*/ SOTO_DBG.set(SYCY_DBG_CLOCKn.as_pwire(), CUNU_RSTn, !SOTO_DBG);

  //cpu_pins.UMUT_MODE_DBG1.set(dbg_sig.UMUT_MODE_DBG1);
  //cpu_pins.UNOR_MODE_DBG2.set(dbg_sig.UNOR_MODE_DBG2n);
  //cpu_pins.UPOJ_MODE_PRODn = UPOJ_MODE_PRODn;
  //cpu_pins.TOVA_MODE_DBG2n = TOVA_MODE_DBG2n;
  //cpu_pins.RYCA_MODE_DBG2n = RYCA_MODE_DBG2n;
}

//-----------------------------------------------------------------------------

SignalHash DebugRegisters::commit() {
  SignalHash hash;

  /*p25.SOTO*/ hash << SOTO_DBG.commit_reg();
  ///*p07.BURO*/ hash << BURO_FF60_0.commit_reg();
  ///*p07.AMUT*/ hash << AMUT_FF60_1.commit_reg();
  return hash;
}

//-----------------------------------------------------------------------------