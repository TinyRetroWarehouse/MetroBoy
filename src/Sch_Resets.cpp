#include "Sch_Resets.h"
#include "Sch_Top.h"

#define FAST_BOOT

using namespace Schematics;

//-----------------------------------------------------------------------------

void ResetRegisters::tick(SchematicTop& /*top*/) {
}

//-----------------------------------------------------------------------------

void ResetRegisters::tock(SchematicTop& top) {
  wire SYS_PIN_CLK_A = top.SYS_PIN_CLK_A;
  wire SYS_PIN_RSTp = top.SYS_PIN_RSTp;
  wire CPU_PIN_READYp = top.CPU_PIN_READYp;

  /*p01.UCOB*/ wire UCOB = not(SYS_PIN_CLK_A);
  /*p01.UPYF*/ wire UPYF = or(SYS_PIN_RSTp, UCOB);

  // Are we _sure_ this is a nor latch?
  /*p01.TUBO*/ _TUBO_CPU_READYn.nor_latch(UPYF, CPU_PIN_READYp);

#ifdef FAST_BOOT
  // Just wait until DIV = 4 instead of DIV = 32768
  /*p01.UNUT*/ wire _UNUT_POR_TRIGn = and (_TUBO_CPU_READYn.q(), top.UNER_DIV_02());
#else
  /*p01.UNUT*/ wire _UNUT_POR_TRIGn = and (_TUBO_CPU_READYn.q(), top.UPOF_DIV_15());
#endif
  /*p01.TABA*/ wire _TABA_POR_TRIGn = or(top.UNOR_MODE_DBG2p(), top.UMUT_MODE_DBG1p(), _UNUT_POR_TRIGn);
  /*p01.ALYP*/ wire _ALYP_RSTn = not(_TABA_POR_TRIGn);
  /*p01.AFAR*/ wire _AFAR_RST  = nor(_ALYP_RSTn, top.SYS_PIN_RSTp);

  /*p01.ASOL*/ ASOL_POR_DONEn.nor_latch(top.SYS_PIN_RSTp, _AFAR_RST); // Schematic wrong, this is a latch.
  /*p01.AFER*/ AFER_SYS_RSTp.set(top.BOGA_xBCDEFGH(), top.BOMA_Axxxxxxx(), top.UPOJ_MODE_PRODn(), ASOL_POR_DONEn.q());

  top.CPU_PIN_STARTp.set(_TABA_POR_TRIGn);
  top.CPU_PIN_SYS_RSTp.set(AFER_SYS_RSTp.q());
}

//-----------------------------------------------------------------------------

SignalHash ResetRegisters::commit() {
  SignalHash hash;

  /*p01.TUBO*/ hash << _TUBO_CPU_READYn.commit();
  /*p01.ASOL*/ hash << ASOL_POR_DONEn.commit(); // Schematic wrong, this is a latch.
  /*p01.AFER*/ hash << AFER_SYS_RSTp.commit();
  return hash;
}

//-----------------------------------------------------------------------------
