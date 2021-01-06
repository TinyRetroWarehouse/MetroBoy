#include "GateBoyLib/GateBoyJoypad.h"

#include "GateBoyLib/GateBoyCpuBus.h"
#include "GateBoyLib/GateBoyResetDebug.h"
#include "GateBoyLib/GateBoyClocks.h"

// JOYP should read as 0xCF at reset? So the RegQPs reset to 1 and the RegQNs reset to 0?
// That also means that _both_ P14 and P15 are selected at reset :/

//------------------------------------------------------------------------------------------------------------------------

void GateBoyJoypad::read(GateBoyCpuBus& cpu_bus) {
  /* p10.ACAT*/ wire _ACAT_FF00_RDp =  and4(cpu_bus.TEDO_CPU_RDp.qp_new(), cpu_bus.ANAP_FF_0xx00000(), cpu_bus.AKUG_A06n(), cpu_bus.BYKO_A05n());

  /* p05.BYZO*/ wire _BYZO_FF00_RDn = not1(_ACAT_FF00_RDp);
  /* p05.KEVU*/ KEVU_JOYP_L0n.tp_latchn(_BYZO_FF00_RDn, PIN_67_JOY_P10.int_qp_new());
  /* p05.KAPA*/ KAPA_JOYP_L1n.tp_latchn(_BYZO_FF00_RDn, PIN_66_JOY_P11.int_qp_new());
  /* p05.KEJA*/ KEJA_JOYP_L2n.tp_latchn(_BYZO_FF00_RDn, PIN_65_JOY_P12.int_qp_new());
  /* p05.KOLO*/ KOLO_JOYP_L3n.tp_latchn(_BYZO_FF00_RDn, PIN_64_JOY_P13.int_qp_new());

  /* p05.KEMA*/ cpu_bus.BUS_CPU_D00p.tri6_nn(_BYZO_FF00_RDn, KEVU_JOYP_L0n.qp_new());
  /* p05.KURO*/ cpu_bus.BUS_CPU_D01p.tri6_nn(_BYZO_FF00_RDn, KAPA_JOYP_L1n.qp_new());
  /* p05.KUVE*/ cpu_bus.BUS_CPU_D02p.tri6_nn(_BYZO_FF00_RDn, KEJA_JOYP_L2n.qp_new());
  /* p05.JEKU*/ cpu_bus.BUS_CPU_D03p.tri6_nn(_BYZO_FF00_RDn, KOLO_JOYP_L3n.qp_new());
  /* p05.KOCE*/ cpu_bus.BUS_CPU_D04p.tri6_nn(_BYZO_FF00_RDn, KELY_JOYP_UDLRp.qn_new());
  /* p05.CUDY*/ cpu_bus.BUS_CPU_D05p.tri6_nn(_BYZO_FF00_RDn, COFY_JOYP_ABCSp.qn_new());
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoyJoypad::write_sync(GateBoyResetDebug& rst, GateBoyCpuBus& cpu_bus) {
  /* p10.ATOZ*/ wire _ATOZ_FF00_WRn = nand4(cpu_bus.TAPU_CPU_WRp.qp_new(), cpu_bus.ANAP_FF_0xx00000(), cpu_bus.AKUG_A06n(), cpu_bus.BYKO_A05n());
  ///* p05.JUTE*/ JUTE_DBG_D0    .dff17(_ATOZ_FF00_WRn, rst.ALUR_SYS_RSTn(), cpu_bus.BUS_CPU_D[0].qp_old());
  ///* p05.KECY*/ KECY_DBG_D1    .dff17(_ATOZ_FF00_WRn, rst.ALUR_SYS_RSTn(), cpu_bus.BUS_CPU_D[1].qp_old());
  ///* p05.JALE*/ JALE_DBG_D2    .dff17(_ATOZ_FF00_WRn, rst.ALUR_SYS_RSTn(), cpu_bus.BUS_CPU_D[2].qp_old());
  ///* p05.KYME*/ KYME_DBG_D3    .dff17(_ATOZ_FF00_WRn, rst.ALUR_SYS_RSTn(), cpu_bus.BUS_CPU_D[3].qp_old());
  /* p05.KELY*/ KELY_JOYP_UDLRp.dff17(_ATOZ_FF00_WRn, rst.ALUR_SYS_RSTn(), cpu_bus.BUS_CPU_D04p.qp_old());
  /* p05.COFY*/ COFY_JOYP_ABCSp.dff17(_ATOZ_FF00_WRn, rst.ALUR_SYS_RSTn(), cpu_bus.BUS_CPU_D05p.qp_old());
  ///* p05.KUKO*/ KUKO_DBG_D6    .dff17(_ATOZ_FF00_WRn, rst.ALUR_SYS_RSTn(), cpu_bus.BUS_CPU_D[6].qp_old());
  ///* p05.KERU*/ KERU_DBG_D7    .dff17(_ATOZ_FF00_WRn, rst.ALUR_SYS_RSTn(), cpu_bus.BUS_CPU_D[7].qp_old());
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoyJoypad::tock2(GateBoyResetDebug& rst, GateBoyClock& clk, uint8_t sys_buttons)
{
  //wire PIN_58_VCC = 1;
  wire PIN_32_GND = 0;

  PIN_67_JOY_P10.reset_for_pass();
  PIN_66_JOY_P11.reset_for_pass();
  PIN_65_JOY_P12.reset_for_pass();
  PIN_64_JOY_P13.reset_for_pass();
  PIN_63_JOY_P14.reset_for_pass();
  PIN_62_JOY_P15.reset_for_pass();

  /* p07.BURO*/ wire _BURO_FF60_D0p = not1(PIN_32_GND); // FIXME hacking out debug stuff
  /* p05.KURA*/ wire _KURA_FF60_D0n = not1(_BURO_FF60_D0p);

  /*
  // lcd ribbon voltages after bootrom
  04 5 left & b
  05 0 diodes 1&2
  06 5 down & start
  07 5 up & select
  08 5 right & a
  09 0 diodes 3 & 4
  */

  /* p05.KARU*/ wire _KARU = or2(KELY_JOYP_UDLRp.qn_new(), _KURA_FF60_D0n);
  /* p05.CELA*/ wire _CELA = or2(COFY_JOYP_ABCSp.qn_new(), _KURA_FF60_D0n);

  /*PIN_63*/ PIN_63_JOY_P14.pin_out_hilo(_KARU, KELY_JOYP_UDLRp.qn_new());
  /*PIN_62*/ PIN_62_JOY_P15.pin_out_hilo(_CELA, COFY_JOYP_ABCSp.qn_new());

  // FIXME hacking in a pullup here
  PIN_63_JOY_P14.state |= BIT_PULLUP;
  PIN_62_JOY_P15.state |= BIT_PULLUP;

  if (PIN_63_JOY_P14.ext_qp_new()) {
    PIN_67_JOY_P10.pin_in_dp(bit(~sys_buttons, 0));
    PIN_66_JOY_P11.pin_in_dp(bit(~sys_buttons, 1));
    PIN_65_JOY_P12.pin_in_dp(bit(~sys_buttons, 2));
    PIN_64_JOY_P13.pin_in_dp(bit(~sys_buttons, 3));
  }
  else if (PIN_62_JOY_P15.ext_qp_new()) {
    PIN_67_JOY_P10.pin_in_dp(bit(~sys_buttons, 4));
    PIN_66_JOY_P11.pin_in_dp(bit(~sys_buttons, 5));
    PIN_65_JOY_P12.pin_in_dp(bit(~sys_buttons, 6));
    PIN_64_JOY_P13.pin_in_dp(bit(~sys_buttons, 7));
  }
  else {
    PIN_67_JOY_P10.pin_in_dp(1);
    PIN_66_JOY_P11.pin_in_dp(1);
    PIN_65_JOY_P12.pin_in_dp(1);
    PIN_64_JOY_P13.pin_in_dp(1);
  }

  /* p02.KERY*/ wire _KERY_ANY_BUTTONp = or4(PIN_64_JOY_P13.int_qp_new(), PIN_65_JOY_P12.int_qp_new(), PIN_66_JOY_P11.int_qp_new(), PIN_67_JOY_P10.int_qp_new());

  /* p02.AWOB*/ AWOB_WAKE_CPU.tp_latchn(clk.BOGA_Axxxxxxx(), _KERY_ANY_BUTTONp);
  /*SIG_CPU_WAKE*/ SIG_CPU_WAKE.set(AWOB_WAKE_CPU.qp_new());

  /* p02.APUG*/ APUG_JP_GLITCH3.dff17(clk.BOGA_Axxxxxxx(), rst.ALUR_SYS_RSTn(), AGEM_JP_GLITCH2.qp_old());
  /* p02.AGEM*/ AGEM_JP_GLITCH2.dff17(clk.BOGA_Axxxxxxx(), rst.ALUR_SYS_RSTn(), ACEF_JP_GLITCH1.qp_old());
  /* p02.ACEF*/ ACEF_JP_GLITCH1.dff17(clk.BOGA_Axxxxxxx(), rst.ALUR_SYS_RSTn(), BATU_JP_GLITCH0.qp_old());
  /* p02.BATU*/ BATU_JP_GLITCH0.dff17(clk.BOGA_Axxxxxxx(), rst.ALUR_SYS_RSTn(), _KERY_ANY_BUTTONp);
}

//------------------------------------------------------------------------------------------------------------------------
