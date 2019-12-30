#include "Sch_Resets.h"

#include "Sch_Debug.h"
#include "Sch_Pins.h"
#include "Sch_Clocks.h"

#pragma warning(disable:4458)

namespace Schematics {

//-----------------------------------------------------------------------------

void ResetSignals1::tick_slow(const SystemSignals& sys_sig,
                              const ClockSignals1& clk_sig1,
                              const ResetRegisters& rst_reg) {

  /*p01.UPYF*/ wire UPYF = or(sys_sig.RST, sys_sig.CLK_BAD1);
  /*p01.TUBO*/ bool WAITING_FOR_CLKREQ = rst_reg.WAITING_FOR_CLKREQ;
  /*p01.TUBO*/ if (!sys_sig.CPUCLK_REQn) WAITING_FOR_CLKREQ = 0;
  /*p01.TUBO*/ if (!UPYF)                WAITING_FOR_CLKREQ = 1;

  /*p01.UNUT*/ wire TIMEOUT     = and(WAITING_FOR_CLKREQ, sys_sig.DIV_15);
  /*p01.TABA*/ wire CPU_RESET   = or(sys_sig.MODE_DBG2, sys_sig.MODE_DBG1, TIMEOUT);
  /*p01.ALYP*/ wire CPU_RESETn  = not(CPU_RESET);
  /*p01.AFAR*/ wire AFAR        = nor(CPU_RESETn, sys_sig.RST);
  /*p01.ASOL*/ wire RESET_IN    = or (AFAR, sys_sig.RST);

  /*p01.AVOR*/ wire AVOR_RESET  = or(rst_reg.RESET_REG, RESET_IN);
  /*p01.ALUR*/ wire SYS_RESETn  = not(AVOR_RESET);   // this goes all over the place
  /*p01.DULA*/ wire SYS_RESET   = not(SYS_RESETn);
  /*p01.SOTO*/ wire SOTO_RESET  = not(SYS_RESET);
  /*p01.CUNU*/ wire CUNU_RESETn = not(SYS_RESET);
  /*p01.XORE*/ wire XORE_RESET  = not(CUNU_RESETn);
  /*p01.BOMA*/ wire RESET_CLK    = not(clk_sig1.BOGA_xBCDEFGH);
  /*p01.WESY*/ wire WESY_RESET  = not(XORE_RESET);
  /*p01.WALU*/ wire WALU_RESET  = not(XORE_RESET);
  /*p01.XARE*/ wire XARE_RESET  = not(XORE_RESET);

  ResetSignals1& next = *this;

  /*p01.BOMA*/ next.RESET_CLK   = RESET_CLK;
  /*p01.ALUR*/ next.SYS_RESETn  = SYS_RESETn;
  /*p01.DULA*/ next.SYS_RESET   = SYS_RESET;
  /*p01.CUNU*/ next.CUNU_RESETn = CUNU_RESETn;
  /*p01.WESY*/ next.WESY_RESET  = WESY_RESET;
  /*p01.WALU*/ next.WALU_RESET  = WALU_RESET;
  /*p01.XARE*/ next.XARE_RESET  = XARE_RESET;
  /*p01.SOTO*/ next.SOTO_RESET  = SOTO_RESET;
}

//----------------------------------------

void ResetSignals1::tick_fast(const SystemSignals& sys_sig,
                              const ClockSignals1& clk_sig1,
                              const ResetRegisters& rst_reg) {
  RESET_CLK = not(clk_sig1.BOGA_xBCDEFGH);

  // polarity here seems weird
  /*p01.UPYF*/ wire UPYF = or(sys_sig.RST, sys_sig.CLK_BAD1);
  /*p01.TUBO*/ bool WAITING_FOR_CLKREQ = rst_reg.WAITING_FOR_CLKREQ;
  /*p01.TUBO*/ if (!sys_sig.CPUCLK_REQn) WAITING_FOR_CLKREQ = 0;
  /*p01.TUBO*/ if (!UPYF)                WAITING_FOR_CLKREQ = 1;

  bool TIMEOUT = and(WAITING_FOR_CLKREQ, sys_sig.DIV_15);

  bool RESET = TIMEOUT || rst_reg.RESET_REG || sys_sig.RST || sys_sig.MODE_DBG1 || sys_sig.MODE_DBG2;

  ResetSignals1& next = *this;

  next.SYS_RESETn  = !RESET;
  next.SYS_RESET   =  RESET;
  next.CUNU_RESETn = !RESET;
  next.WESY_RESET  = !RESET;
  next.WALU_RESET  = !RESET;
  next.XARE_RESET  = !RESET;
  next.SOTO_RESET  = !RESET;
}

//-----------------------------------------------------------------------------

void ResetSignals2::tick_slow(const SystemSignals& sys_sig,
                              const ResetSignals1& rst_sig1,
                              const ResetRegisters& /*rst_reg*/) {
  /*p01.XORE*/ wire XORE_RESET  = not(rst_sig1.CUNU_RESETn);
  /*p01.XEBE*/ wire XEBE_RESET  = not(XORE_RESET);
  /*p01.XODO*/ wire XODO_RESET  = nand(XEBE_RESET, sys_sig.LCDC_EN);
  /*p01.XAPO*/ wire VID_RESETn  = not(XODO_RESET);
  /*p01.TOFU*/ wire VID_RESET3  = not(VID_RESETn);
  /*p01.PYRY*/ wire VID_RESET4  = not(VID_RESETn);
  /*p01.ROSY*/ wire VID_RESET5  = not(VID_RESETn);
  /*p01.ATAR*/ wire VID_RESET6  = not(VID_RESETn);
  /*p01.ABEZ*/ wire VID_RESETn3 = not(VID_RESET6);

  ResetSignals2& next = *this;

  /*p01.XAPO*/ next.VID_RESETn  = VID_RESETn;
  /*p01.TOFU*/ next.VID_RESET3  = VID_RESET3;
  /*p01.PYRY*/ next.VID_RESET4  = VID_RESET4;
  /*p01.ROSY*/ next.VID_RESET5  = VID_RESET5;
  /*p01.ATAR*/ next.VID_RESET6  = VID_RESET6;
  /*p01.ABEZ*/ next.VID_RESETn3 = VID_RESETn3;
}

//----------------------------------------

void ResetSignals2::tick_fast(const SystemSignals& sys_sig,
                              const ResetSignals1& rst_sig1,
                              const ResetRegisters& /*rst_reg*/) {
  wire VID_RESET  = nand(rst_sig1.SYS_RESETn, sys_sig.LCDC_EN);

  ResetSignals2& next = *this;

  next.VID_RESETn  = !VID_RESET;
  next.VID_RESET3  =  VID_RESET;
  next.VID_RESET4  =  VID_RESET;
  next.VID_RESET5  =  VID_RESET;
  next.VID_RESET6  =  VID_RESET;
  next.VID_RESETn3 = !VID_RESET;
}

//-----------------------------------------------------------------------------

void ResetRegisters::tock_slow(const SystemSignals& sys_sig,
                               const ClockSignals1& clk_sig1,
                               const ResetRegisters& rst_reg) {
  ResetRegisters& next = *this;

  /*p01.UPYF*/ bool UPYF = or(sys_sig.RST, sys_sig.CLK_BAD1);
  /*p01.TUBO*/ bool WAITING_FOR_CLKREQ2 = !UPYF ? 1 : !sys_sig.CPUCLK_REQn ? 0 : rst_reg.WAITING_FOR_CLKREQ;
  /*p01.BOMA*/ bool RESET_CLK   = not(clk_sig1.BOGA_xBCDEFGH);
  /*p01.UNUT*/ bool TIMEOUT     = and(WAITING_FOR_CLKREQ2, sys_sig.DIV_15);
  /*p01.TABA*/ bool CPU_RESET   = or(sys_sig.MODE_DBG2, sys_sig.MODE_DBG1, TIMEOUT);
  /*p01.ALYP*/ bool CPU_RESETn  = not(CPU_RESET);
  /*p01.ASOL*/ bool RESET_IN    = or (/*p01.AFAR*/ nor(CPU_RESETn, sys_sig.RST), sys_sig.RST);

  /*p01.TUBO*/ next.WAITING_FOR_CLKREQ = WAITING_FOR_CLKREQ2;
  /*p01.AFER*/ next.RESET_REG.tock(RESET_CLK, sys_sig.MODE_PROD, RESET_IN);
}

//----------------------------------------

void ResetRegisters::tock_fast(const SystemSignals& sys_sig,
                               const ClockSignals1& clk_sig1,
                               const ResetRegisters& /*rst_reg*/) {
  ResetRegisters& next = *this;

  if (sys_sig.CPUCLK_REQ) next.WAITING_FOR_CLKREQ = 0;
  if (!sys_sig.RST && !sys_sig.CLK_BAD1) next.WAITING_FOR_CLKREQ = 1;
  bool TIMEOUT = and(next.WAITING_FOR_CLKREQ, sys_sig.DIV_15);
  
  bool RESET     = TIMEOUT || sys_sig.RST || sys_sig.MODE_DBG2 || sys_sig.MODE_DBG1;
  bool RESET_CLK = not(clk_sig1.BOGA_xBCDEFGH);

  next.RESET_REG.tock(RESET_CLK, sys_sig.MODE_PROD, RESET);
}

//-----------------------------------------------------------------------------

};