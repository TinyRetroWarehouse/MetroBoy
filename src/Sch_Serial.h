#pragma once
#include "Sch_Common.h"

namespace Schematics {

struct SchematicTop;

//-----------------------------------------------------------------------------

struct SerialRegisters {

  void tick(SchematicTop& gb);
  SignalHash commit();

  uint8_t ser_cnt() { return (uint8_t)pack(SER_CNT0.q(), SER_CNT1.q(), SER_CNT2.q(), CALY_INT_SERIALp.q()); }
  uint8_t ser_data() { return (uint8_t)pack(SER_DATA0.q(), SER_DATA1.q(), SER_DATA2.q(), SER_DATA3.q(), SER_DATA4.q(), SER_DATA5.q(), SER_DATA6.q(), SER_DATA7.q()); }

  void dump_regs(TextPainter& text_painter);
  void dump_pins(TextPainter& text_painter);

private:
  friend SchematicTop;

  /*p06.ETAF*/ Reg17 XFER_START;
  /*p06.CULY*/ Reg17 XFER_DIR;

  /*p06.COTY*/ Reg17 SER_CLK;

  /*p06.CAFA*/ Reg17 SER_CNT0;
  /*p06.CYLO*/ Reg17 SER_CNT1;
  /*p06.CYDE*/ Reg17 SER_CNT2;
  /*p06.CALY*/ Reg17 CALY_INT_SERIALp;

  /*p06.CUBA*/ Reg22 SER_DATA0;
  /*p06.DEGU*/ Reg22 SER_DATA1;
  /*p06.DYRA*/ Reg22 SER_DATA2;
  /*p06.DOJO*/ Reg22 SER_DATA3;
  /*p06.DOVU*/ Reg22 SER_DATA4;
  /*p06.EJAB*/ Reg22 SER_DATA5;
  /*p06.EROD*/ Reg22 SER_DATA6;
  /*p06.EDER*/ Reg22 SER_DATA7;

  /*p06.ELYS*/ Reg17 SER_OUT;

  //----------
  // Serial pins

  /* PIN_68 */ PinOut SCK_A;   // <- P06.KEXU
  /* PIN_68 */ PinOut SCK_B;   // <- P06.CULY
  /* PIN_68 */ PinIn  SCK_C;   // -> P06.CAVE
  /* PIN_68 */ PinOut SCK_D;   // <- P06.KUJO

  /* PIN_69 */ PinOut SIN_A;   // nc?
  /* PIN_69 */ PinOut SIN_B;   // nc?
  /* PIN_69 */ PinIn  SIN_C;   // -> P06.CAGE
  /* PIN_69 */ PinOut SIN_D;   // nc?

  /* PIN_70 */ PinOut SOUT;    // <- P05.KENA
};

//-----------------------------------------------------------------------------

}; // namespace Schematics