#include "Sch_Clocks.h"
#include "Sch_Top.h"

using namespace Schematics;

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

//-----------------------------------------------------------------------------

void ClockRegisters::tick(SchematicTop& top) {
  ///*p01.ARYS*/ wire ARYS_xBxDxFxH = not(ext_pins.CLK); // ignoring the deglitcher here
  ///*p01.AVET*/ wire AVET_AxCxExGx = ext_pins.CLK;

  {
    /*p01.ANOS*/ wire ANOS_AxCxExGx = not(top.SYS_PIN_CLK_xBxDxFxH);
    /*p01.ATAL*/ wire ATAL_xBxDxFxH = not(ANOS_AxCxExGx);
    /*p01.AFUR*/ AFUR_xBCDExxx.set_duo(ATAL_xBxDxFxH, top.UPOJ_MODE_PROD(), !ADYK_xxxxEFGH);
    /*p01.ALEF*/ ALEF_xxCDEFxx.set_duo(ATAL_xBxDxFxH, top.UPOJ_MODE_PROD(),  AFUR_xBCDExxx);
    /*p01.APUK*/ APUK_xxxDEFGx.set_duo(ATAL_xBxDxFxH, top.UPOJ_MODE_PROD(),  ALEF_xxCDEFxx);
    /*p01.ADYK*/ ADYK_xxxxEFGH.set_duo(ATAL_xBxDxFxH, top.UPOJ_MODE_PROD(),  APUK_xxxDEFGx);
  }

  {
    /*p01.ANOS*/ wire ANOS_AxCxExGx = not(top.SYS_PIN_CLK_xBxDxFxH);
    /*p01.ATAL*/ wire ATAL_xBxDxFxH = not(ANOS_AxCxExGx);
    /*p01.AZOF*/ wire AZOF_AxCxExGx = not(ATAL_xBxDxFxH);
    /*p01.ZAXY*/ wire ZAXY_xBxDxFxH = not(AZOF_AxCxExGx);
    /*p01.ZEME*/ wire ZEME_AxCxExGx = not(ZAXY_xBxDxFxH);
    /*p29.XYVA*/ wire XYVA_AxCxExGx = not(ZEME_AxCxExGx);
    /*p29.XOTA*/ wire XOTA_xBxDxFxH = not(XYVA_AxCxExGx);
    /*p29.XYFY*/ wire XYFY_AxCxExGx = not(XOTA_xBxDxFxH);

    /*p29.WUVU*/ WUVU_AxxDExxH.set(XOTA_xBxDxFxH,  top.XAPO_VID_RSTn(), !WUVU_AxxDExxH);
    /*p21.VENA*/ VENA_xBCDExxx.set(!WUVU_AxxDExxH, top.XAPO_VID_RSTn(), !VENA_xBCDExxx);
    /*p29.WOSU*/ WOSU_xxCDxxGH.set(XYFY_AxCxExGx,  top.XAPO_VID_RSTn(), !WUVU_AxxDExxH);
  }

  //cpu_pins.CLK_GOOD.set(sys_pins.CLK_GOOD);

  // phases checked and ok
  // cpu_pins.BEDO_xBxxxxxx.set(clk_reg.BEDO_xBxxxxxx);
  // cpu_pins.BOWA_AxCDEFGH.set(clk_reg.BOWA_AxCDEFGH);
  // cpu_pins.BEKO_xBCDExxx.set(clk_reg.BEKO_xBCDExxx);
  // cpu_pins.BUDE_AxxxxFGH.set(clk_reg.BUDE_AxxxxFGH);
  // cpu_pins.BOLO_xBCDEFGx.set(clk_reg.BOLO_xBCDEFGx);
  // cpu_pins.BUKE_ABxxxxxH.set(clk_reg.BUKE_ABxxxxxH);
  // cpu_pins.BOGA_AxCDEFGH.set(clk_reg.BOGA_AxCDEFGH);
  // cpu_pins.BOMA_xBxxxxxx.set(clk_reg.BOMA_xBxxxxxx);
}

//-----------------------------------------------------------------------------

SignalHash ClockRegisters::commit() {
  SignalHash hash;

  hash << AFUR_xBCDExxx.commit_duo();
  hash << ALEF_xxCDEFxx.commit_duo();
  hash << APUK_xxxDEFGx.commit_duo();
  hash << ADYK_xxxxEFGH.commit_duo();

  hash << WUVU_AxxDExxH.commit_reg();
  hash << VENA_xBCDExxx.commit_reg();
  hash << WOSU_xxCDxxGH.commit_reg();
  return hash;
}

//-----------------------------------------------------------------------------