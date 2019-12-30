#pragma once
#include "Sch_Common.h"

namespace Schematics {

//-----------------------------------------------------------------------------

struct ResetSignals1 {

  void tick_slow(const SystemSignals& sys_sig,
                 const ClockSignals1& clk_sig1,
                 const ResetRegisters& rst_reg);
  void tick_fast(const SystemSignals& sys_sig,
                 const ClockSignals1& clk_sig1,
                 const ResetRegisters& rst_reg);

  void reset() {
    RESET_CLK = false;

    SYS_RESETn = true;
    SYS_RESET = false;
    CUNU_RESETn = true;

    WESY_RESET = true;
    WALU_RESET = true;
    XARE_RESET = true;
    SOTO_RESET = true;
  };

  /*p01.BOMA*/ bool RESET_CLK; // _____fgh -> PORTD_07
  /*p01.ALUR*/ bool SYS_RESETn;
  /*p01.DULA*/ bool SYS_RESET;

  /*p01.CUNU*/ bool CUNU_RESETn;

  /*p01.WESY*/ bool WESY_RESET;
  /*p01.WALU*/ bool WALU_RESET;
  /*p01.XARE*/ bool XARE_RESET;
  /*p01.SOTO*/ bool SOTO_RESET;
};

//-----------------------------------------------------------------------------

struct ResetSignals2 {

  void tick_slow(const SystemSignals& sys_sig,
                 const ResetSignals1& rst_sig1,
                 const ResetRegisters& rst_reg);
  void tick_fast(const SystemSignals& sys_sig,
                 const ResetSignals1& rst_sig1,
                 const ResetRegisters& rst_reg);

  void reset() {
    VID_RESETn = true;
    VID_RESET3 = false;
    VID_RESET4 = false;
    VID_RESET5 = false;
    VID_RESET6 = false;
    VID_RESETn3 = true;
  };

  /*p01.XAPO*/ bool VID_RESETn;
  /*p01.TOFU*/ bool VID_RESET3;
  /*p01.PYRY*/ bool VID_RESET4;
  /*p01.ROSY*/ bool VID_RESET5;
  /*p01.ATAR*/ bool VID_RESET6;
  /*p01.ABEZ*/ bool VID_RESETn3;
};

//-----------------------------------------------------------------------------

struct ResetRegisters {

  void tock_slow(const SystemSignals& sys_sig,
                 const ClockSignals1& clk_sig1,
                 const ResetRegisters& rst_reg);
  void tock_fast(const SystemSignals& sys_sig,
                 const ClockSignals1& clk_sig1,
                 const ResetRegisters& rst_reg);

  void reset() {
    WAITING_FOR_CLKREQ = true;
    RESET_REG.val = false;
    RESET_REG.clk = false;
  };

  static void check_match(const ResetRegisters& a, const ResetRegisters& b) {
    check(a.WAITING_FOR_CLKREQ == b.WAITING_FOR_CLKREQ);
    check(a.RESET_REG.val == b.RESET_REG.val);
  }

  /*p01.TUBO*/ bool WAITING_FOR_CLKREQ;
  /*p01.AFER*/ Reg  RESET_REG;
};

//-----------------------------------------------------------------------------

};