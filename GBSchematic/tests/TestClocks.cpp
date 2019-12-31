#include "TestGB.h"

using namespace Schematics;

void plot(int x, int y, bool v) {
  printf("\033[%d;%dH%c", y, x, v ? '#' : '.');
}

void plot2(int x, int y, int v) {
  printf("\033[%d;%dH%d", y, x, v);
}


void printAt(int x, int y, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  printf("\033[%d;%dH", y, x);
  vprintf(format, args);
  va_end(args);
}

void labels() {
  int line = 3;
  printAt(0, line++, "PHASE");
  printAt(0, line++, "CLK");
  line++;

  printAt(0, line++, "ARYS_AxCxExGx");
  printAt(0, line++, "ANOS_AxCxExGx");
  printAt(0, line++, "ATAL_AxCxExGx");
  printAt(0, line++, "ZAXY_AxCxExGx");
  printAt(0, line++, "ALET_AxCxExGx");
  printAt(0, line++, "XYVA_AxCxExGx");
  printAt(0, line++, "TAVA_AxCxExGx");
  printAt(0, line++, "XYFY_AxCxExGx");

  printAt(0, line++, "AVET_xBxDxFxH");
  printAt(0, line++, "AZOF_xBxDxFxH");
  printAt(0, line++, "ZEME_xBxDxFxH");
  printAt(0, line++, "MYVO_xBxDxFxH");
  printAt(0, line++, "XOTA_xBxDxFxH");
  printAt(0, line++, "MOXE_xBxDxFxH");
  printAt(0, line++, "MEHE_xBxDxFxH");
  printAt(0, line++, "LAPE_xBxDxFxH");
  line++;

  printAt(0, line++, "PHAZ_ABCDxxxx");
  printAt(0, line++, "PHAZ_xBCDExxx");
  printAt(0, line++, "PHAZ_xxCDEFxx");
  printAt(0, line++, "PHAZ_xxxDEFGx");
  printAt(0, line++, "AFEP_AxxxxFGH");
  printAt(0, line++, "ATYP_ABCDxxxx");
  printAt(0, line++, "ADAR_ABCxxxxH");
  printAt(0, line++, "AROV_xxCDEFxx");
  printAt(0, line++, "AFAS_xxxxEFGx");
  line++;

  printAt(0, line++, "NULE_xxxxEFGH");
  printAt(0, line++, "BYRY_ABCDxxxx");
  printAt(0, line++, "BUDE_xxxxEFGH");
  printAt(0, line++, "DOVA_ABCDxxxx");
  printAt(0, line++, "UVYT_ABCDxxxx");
  printAt(0, line++, "BEKO_ABCDxxxx");
  printAt(0, line++, "MOPA_xxxxEFGH");
  line++;

  printAt(0, line++, "BAPY_xxxxxxGH");
  printAt(0, line++, "BERU_ABCDEFxx");
  printAt(0, line++, "BUFA_xxxxxxGH");
  printAt(0, line++, "BOLO_ABCDEFxx");
  printAt(0, line++, "BEJA_xxxxEFGH");
  printAt(0, line++, "BANE_ABCDxxxx");
  printAt(0, line++, "BELO_xxxxEFGH");
  printAt(0, line++, "BAZE_ABCDxxxx");
  line++;

  printAt(0, line++, "BUTO_xBCDEFGH");
  printAt(0, line++, "BELE_Axxxxxxx");
  printAt(0, line++, "BYJU_xBCDEFGH");
  printAt(0, line++, "BALY_Axxxxxxx");
  printAt(0, line++, "BOGA_xBCDEFGH");
  printAt(0, line++, "BUVU_Axxxxxxx");
  printAt(0, line++, "BYXO_xBCDEFGH");
  printAt(0, line++, "BEDO_Axxxxxxx");
  printAt(0, line++, "BOWA_xBCDEFGH");
  line++;

  printAt(0, line++, "WUVU_AxxDExxH");
  printAt(0, line++, "XUPY_xBCxxFGx");
  printAt(0, line++, "AWOH_AxxDExxH");
  printAt(0, line++, "VENA_xBCDExxx");
  printAt(0, line++, "TALU_xBCDExxx");
  printAt(0, line++, "SONO_AxxxxFGH");
  printAt(0, line++, "WOSU_xxCDxxGH");
  printAt(0, line++, "XOCE_ABxxEFxx");
  line++;
}

void dump(int x, SystemRegisters& sys_reg, ClockSignals1& clk_sig1, ClockSignals2& clk_sig2) {
  int line = 3;
  plot2(x, line++, sys_reg.phaseC());
  plot(x, line++, sys_reg.clk());
  line++;

  plot(x, line++, clk_sig1.ARYS_AxCxExGx);
  plot(x, line++, clk_sig1.ANOS_AxCxExGx);
  plot(x, line++, clk_sig1.ATAL_AxCxExGx);
  plot(x, line++, clk_sig1.ZAXY_AxCxExGx);
  plot(x, line++, clk_sig1.ALET_AxCxExGx);
  plot(x, line++, clk_sig1.XYVA_AxCxExGx);
  plot(x, line++, clk_sig1.TAVA_AxCxExGx);
  plot(x, line++, clk_sig1.XYFY_AxCxExGx);
  plot(x, line++, clk_sig1.AVET_xBxDxFxH);
  plot(x, line++, clk_sig1.AZOF_xBxDxFxH);
  plot(x, line++, clk_sig1.ZEME_xBxDxFxH);
  plot(x, line++, clk_sig1.MYVO_xBxDxFxH);
  plot(x, line++, clk_sig1.XOTA_xBxDxFxH);
  plot(x, line++, clk_sig1.MOXE_xBxDxFxH);
  plot(x, line++, clk_sig1.MEHE_xBxDxFxH);
  plot(x, line++, clk_sig1.LAPE_xBxDxFxH);
  line++;

  plot(x, line++, clk_sig1.PHAZ_ABCDxxxx);
  plot(x, line++, clk_sig1.PHAZ_xBCDExxx);
  plot(x, line++, clk_sig1.PHAZ_xxCDEFxx);
  plot(x, line++, clk_sig1.PHAZ_xxxDEFGx);
  plot(x, line++, clk_sig1.AFEP_AxxxxFGH);
  plot(x, line++, clk_sig1.ATYP_ABCDxxxx);
  plot(x, line++, clk_sig1.ADAR_ABCxxxxH);
  plot(x, line++, clk_sig1.AROV_xxCDEFxx);
  plot(x, line++, clk_sig1.AFAS_xxxxEFGx);
  line++;

  plot(x, line++, clk_sig1.NULE_xxxxEFGH);
  plot(x, line++, clk_sig1.BYRY_ABCDxxxx);
  plot(x, line++, clk_sig1.BUDE_xxxxEFGH);
  plot(x, line++, clk_sig1.DOVA_ABCDxxxx);
  plot(x, line++, clk_sig1.UVYT_ABCDxxxx);
  plot(x, line++, clk_sig1.BEKO_ABCDxxxx);
  plot(x, line++, clk_sig1.MOPA_xxxxEFGH);
  line++;

  plot(x, line++, clk_sig1.BAPY_xxxxxxGH);
  plot(x, line++, clk_sig1.BERU_ABCDEFxx);
  plot(x, line++, clk_sig1.BUFA_xxxxxxGH);
  plot(x, line++, clk_sig1.BOLO_ABCDEFxx);
  plot(x, line++, clk_sig1.BEJA_xxxxEFGH);
  plot(x, line++, clk_sig1.BANE_ABCDxxxx);
  plot(x, line++, clk_sig1.BELO_xxxxEFGH);
  plot(x, line++, clk_sig1.BAZE_ABCDxxxx);
  line++;

  plot(x, line++, clk_sig1.BUTO_xBCDEFGH);
  plot(x, line++, clk_sig1.BELE_Axxxxxxx);
  plot(x, line++, clk_sig1.BYJU_xBCDEFGH);
  plot(x, line++, clk_sig1.BALY_Axxxxxxx);
  plot(x, line++, clk_sig1.BOGA_xBCDEFGH);
  plot(x, line++, clk_sig1.BUVU_Axxxxxxx);
  plot(x, line++, clk_sig1.BYXO_xBCDEFGH);
  plot(x, line++, clk_sig1.BEDO_Axxxxxxx);
  plot(x, line++, clk_sig1.BOWA_xBCDEFGH);
  line++;

  plot(x, line++, clk_sig2.WUVU_AxxDExxH);
  plot(x, line++, clk_sig2.XUPY_xBCxxFGx);
  plot(x, line++, clk_sig2.AWOH_AxxDExxH);
  plot(x, line++, clk_sig2.VENA_xBCDExxx);
  plot(x, line++, clk_sig2.TALU_xBCDExxx);
  plot(x, line++, clk_sig2.SONO_AxxxxFGH);
  plot(x, line++, clk_sig2.WOSU_xxCDxxGH);
  plot(x, line++, clk_sig2.XOCE_ABxxEFxx);
  line++;
}

static int cursor = 20;

void TestClocks() {
  printf("TestClocks: ");
  printf("\033[?6l");
  labels();

  TestGB gb;
  gb.reset();

  for (int phase = 0; phase < 8; phase++) {
    gb.sim_slow(1);

    ClockSignals1 clk_sig1 = ClockSignals1::tick_slow(gb.sys_reg, gb.clk_reg1);
    ResetSignals1 rst_sig1 = ResetSignals1::tick_fast(gb.sys_reg, gb.rst_reg);
    ResetSignals2 rst_sig2 = ResetSignals2::tick_fast(gb.sys_reg, rst_sig1);
    ClockSignals2 clk_sig2 = ClockSignals2::tick_fast(gb.sys_reg, rst_sig2, gb.clk_reg2);

    dump(cursor++, gb.sys_reg, clk_sig1, clk_sig2);
  }

  /*
  bool CLK_GOOD = false;
  bool CPUCLK_REQ = false;
  bool MODE_PROD = false;
  bool VID_RESETn = 0;

  //----------

  CLK_GOOD = false;
  CPUCLK_REQ = false;
  MODE_PROD = false;
  VID_RESETn = 0;

  printAt(cursor, 1, "rst");
  sim(clk, CLK_GOOD, CPUCLK_REQ, MODE_PROD, VID_RESETn);
  cursor++;

  //----------

  MODE_PROD = true;
  printAt(cursor, 1, "prod");
  sim(clk, CLK_GOOD, CPUCLK_REQ, MODE_PROD, VID_RESETn);
  cursor++;

  //----------

  CLK_GOOD = true;
  printAt(cursor, 1, "clk_good");
  sim(clk, CLK_GOOD, CPUCLK_REQ, MODE_PROD, VID_RESETn);
  cursor++;

  //----------

  CPUCLK_REQ = true;
  printAt(cursor, 1, "clkreq");
  sim(clk, CLK_GOOD, CPUCLK_REQ, MODE_PROD, VID_RESETn);
  cursor++;

  //----------

  VID_RESETn = true;
  printAt(cursor, 1, "vid_rst");
  sim(clk, CLK_GOOD, CPUCLK_REQ, MODE_PROD, VID_RESETn);
  cursor++;
  */

  //printAt(0, 70, "done");

  printf("\n");
  printf("pass\n");
}