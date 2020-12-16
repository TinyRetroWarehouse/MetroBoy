#include "GateBoyLib/GateBoyLCD.h"

#include "GateBoyLib/GateBoyResetDebug.h"
#include "GateBoyLib/GateBoyClocks.h"
#include "GateBoyLib/GateBoyCpuBus.h"
#include "GateBoyLib/GateBoyTimer.h"
#include "GateBoyLib/GateBoyPixPipe.h"

//------------------------------------------------------------------------------------------------------------------------

void RegLX::tock(GateBoyResetDebug& rst, GateBoyClock& clk) {
  /*#p21.SANU*/ wire _SANU_x113p_old = and4(TYRY_LX6p.qp_old(), TAHA_LX5p.qp_old(), SUDE_LX4p.qp_old(), SAXO_LX0p.qp_old()); // 113 = 64 + 32 + 16 + 1, schematic is wrong
  /*#p21.NYPE*/ _NYPE_x113p.dff17(clk.TALU_xxCDEFxx(), rst.LYFE_VID_RSTn(), _RUTU_x113p.qp_old());
  /*#p21.RUTU*/ _RUTU_x113p.dff17(clk.SONO_ABxxxxGH(), rst.LYFE_VID_RSTn(), _SANU_x113p_old);

  /*#p21.MUDE*/ wire _MUDE_X_RSTn = nor2(_RUTU_x113p.qp_new(), rst.LYHA_VID_RSTp());
  /*#p21.SAXO*/ SAXO_LX0p.dff17(clk.TALU_xxCDEFxx(), _MUDE_X_RSTn, SAXO_LX0p.qn());
  /*#p21.TYPO*/ TYPO_LX1p.dff17(SAXO_LX0p.qn_new(),  _MUDE_X_RSTn, TYPO_LX1p.qn());
  /*#p21.VYZO*/ VYZO_LX2p.dff17(TYPO_LX1p.qn_new(),  _MUDE_X_RSTn, VYZO_LX2p.qn());
  /*#p21.TELU*/ TELU_LX3p.dff17(VYZO_LX2p.qn_new(),  _MUDE_X_RSTn, TELU_LX3p.qn());
  /*#p21.SUDE*/ SUDE_LX4p.dff17(TELU_LX3p.qn_new(),  _MUDE_X_RSTn, SUDE_LX4p.qn());
  /*#p21.TAHA*/ TAHA_LX5p.dff17(SUDE_LX4p.qn_new(),  _MUDE_X_RSTn, TAHA_LX5p.qn());
  /*#p21.TYRY*/ TYRY_LX6p.dff17(TAHA_LX5p.qn_new(),  _MUDE_X_RSTn, TYRY_LX6p.qn());
}

wire RegLX::TEGY_STROBE() const {
  /* p21.SAXO*/ wire _SAXO_LX0p = SAXO_LX0p.qp_new();
  /* p21.TYPO*/ wire _TYPO_LX1p = TYPO_LX1p.qp_new();
  /* p21.VYZO*/ wire _VYZO_LX2p = VYZO_LX2p.qp_new();
  /* p21.TELU*/ wire _TELU_LX3p = TELU_LX3p.qp_new();
  /* p21.SUDE*/ wire _SUDE_LX4p = SUDE_LX4p.qp_new();
  /* p21.TAHA*/ wire _TAHA_LX5p = TAHA_LX5p.qp_new();
  /* p21.TYRY*/ wire _TYRY_LX6p = TYRY_LX6p.qp_new();

  /*#p21.TOCU*/ wire _TOCU_LX0n = not1(_SAXO_LX0p);
  /*#p21.VEPE*/ wire _VEPE_LX1n = not1(_TYPO_LX1p);
  /* p21.VUTY*/ wire _VUTY_LX2n = not1(_VYZO_LX2p);
  /* p21.VATE*/ wire _VATE_LX3n = not1(_TELU_LX3p);
  /* p21.TUDA*/ wire _TUDA_LX4n = not1(_SUDE_LX4p);
  /* p21.TAFY*/ wire _TAFY_LX5n = not1(_TAHA_LX5p);
  /* p21.TUJU*/ wire _TUJU_LX6n = not1(_TYRY_LX6p);

  /* p21.VOKU*/ wire _VOKU_LX000n = nand7(_TUJU_LX6n, _TAFY_LX5n, _TUDA_LX4n, _VATE_LX3n, _VUTY_LX2n, _VEPE_LX1n, _TOCU_LX0n); // 0000000 == 0
  /* p21.TOZU*/ wire _TOZU_LX007n = nand7(_TUJU_LX6n, _TAFY_LX5n, _TUDA_LX4n, _VATE_LX3n, _VYZO_LX2p, _TYPO_LX1p, _SAXO_LX0p); // 0000111 == 7
  /* p21.TECE*/ wire _TECE_LX045n = nand7(_TUJU_LX6n, _TAHA_LX5p, _TUDA_LX4n, _TELU_LX3p, _VYZO_LX2p, _VEPE_LX1n, _SAXO_LX0p); // 0101101 == 45
  /*#p21.TEBO*/ wire _TEBO_LX083n = nand7(_TYRY_LX6p, _TAFY_LX5n, _SUDE_LX4p, _VATE_LX3n, _VUTY_LX2n, _TYPO_LX1p, _SAXO_LX0p); // 1010011 == 83
  /*#p21.TEGY*/ wire _TEGY_STROBE = nand4(_VOKU_LX000n, _TOZU_LX007n, _TECE_LX045n, _TEBO_LX083n);

  return _TEGY_STROBE;
}

//------------------------------------------------------------------------------------------------------------------------

void RegLY::tock(GateBoyResetDebug& rst, GateBoyCpuBus& cpu_bus, RegLX& reg_lx)
{
  /*#p21.NOKO*/ wire _NOKO_y153p_old = and4(LAFO_LY7p.qp_old(), LOVU_LY4p.qp_old(), LYDO_LY3p.qp_old(), MUWY_LY0p.qp_old()); // Schematic wrong: NOKO = and2(V7, V4, V3, V0) = 128 + 16 + 8 + 1 = 153
  /*#p21.MYTA*/ MYTA_y153p.dff17(reg_lx.NYPE_x113p(), rst.LYFE_VID_RSTn(), _NOKO_y153p_old);

  /*#p21.LAMA*/ wire _LAMA_Y_RSTn = nor2(MYTA_y153p.qp_new(), rst.LYHA_VID_RSTp());
  /*#p21.MUWY*/ MUWY_LY0p.dff17(reg_lx.RUTU_x113p(), _LAMA_Y_RSTn, MUWY_LY0p.qn());
  /*#p21.MYRO*/ MYRO_LY1p.dff17(MUWY_LY0p.qn_new(),  _LAMA_Y_RSTn, MYRO_LY1p.qn());
  /*#p21.LEXA*/ LEXA_LY2p.dff17(MYRO_LY1p.qn_new(),  _LAMA_Y_RSTn, LEXA_LY2p.qn());
  /*#p21.LYDO*/ LYDO_LY3p.dff17(LEXA_LY2p.qn_new(),  _LAMA_Y_RSTn, LYDO_LY3p.qn());
  /*#p21.LOVU*/ LOVU_LY4p.dff17(LYDO_LY3p.qn_new(),  _LAMA_Y_RSTn, LOVU_LY4p.qn());
  /*#p21.LEMA*/ LEMA_LY5p.dff17(LOVU_LY4p.qn_new(),  _LAMA_Y_RSTn, LEMA_LY5p.qn());
  /*#p21.MATO*/ MATO_LY6p.dff17(LEMA_LY5p.qn_new(),  _LAMA_Y_RSTn, MATO_LY6p.qn());
  /*#p21.LAFO*/ LAFO_LY7p.dff17(MATO_LY6p.qn_new(),  _LAMA_Y_RSTn, LAFO_LY7p.qn());

  /* p23.WAFU*/ wire _WAFU_FF44_RDp = and2(cpu_bus.ASOT_CPU_RDp(), cpu_bus.XOGY_FF44p());
  /* p23.VARO*/ wire _VARO_FF44_RDn = not1(_WAFU_FF44_RDp);

  /*#p23.WURY*/ wire _WURY_LY0n = not1(MUWY_LY0p.qp_new());
  /* p23.XEPO*/ wire _XEPO_LY1n = not1(MYRO_LY1p.qp_new());
  /* p23.MYFA*/ wire _MYFA_LY2n = not1(LEXA_LY2p.qp_new());
  /* p23.XUHY*/ wire _XUHY_LY3n = not1(LYDO_LY3p.qp_new());
  /* p23.WATA*/ wire _WATA_LY4n = not1(LOVU_LY4p.qp_new());
  /* p23.XAGA*/ wire _XAGA_LY5n = not1(LEMA_LY5p.qp_new());
  /* p23.XUCE*/ wire _XUCE_LY6n = not1(MATO_LY6p.qp_new());
  /* p23.XOWO*/ wire _XOWO_LY7n = not1(LAFO_LY7p.qp_new());

  /* p23.VEGA*/ cpu_bus.BUS_CPU_D_out[0].tri6_nn(_VARO_FF44_RDn, _WURY_LY0n);
  /* p23.WUVA*/ cpu_bus.BUS_CPU_D_out[1].tri6_nn(_VARO_FF44_RDn, _XEPO_LY1n);
  /* p23.LYCO*/ cpu_bus.BUS_CPU_D_out[2].tri6_nn(_VARO_FF44_RDn, _MYFA_LY2n);
  /* p23.WOJY*/ cpu_bus.BUS_CPU_D_out[3].tri6_nn(_VARO_FF44_RDn, _XUHY_LY3n);
  /* p23.VYNE*/ cpu_bus.BUS_CPU_D_out[4].tri6_nn(_VARO_FF44_RDn, _WATA_LY4n);
  /* p23.WAMA*/ cpu_bus.BUS_CPU_D_out[5].tri6_nn(_VARO_FF44_RDn, _XAGA_LY5n);
  /* p23.WAVO*/ cpu_bus.BUS_CPU_D_out[6].tri6_nn(_VARO_FF44_RDn, _XUCE_LY6n);
  /* p23.WEZE*/ cpu_bus.BUS_CPU_D_out[7].tri6_nn(_VARO_FF44_RDn, _XOWO_LY7n);
}

//------------------------------------------------------------------------------------------------------------------------

void RegLCDC::tock(GateBoyResetDebug& rst, GateBoyCpuBus& cpu_bus)
{
  /* p23.WARU*/ wire _WARU_FF40_WRp = and2(cpu_bus.CUPA_CPU_WRp(), cpu_bus.VOCA_FF40p());
  /* p23.XUBO*/ wire _XUBO_FF40_WRn = not1(_WARU_FF40_WRp);
  /*#p23.VYXE*/ VYXE_LCDC_BGENn  .dff9(_XUBO_FF40_WRn, rst.XARE_SYS_RSTn(), cpu_bus.BUS_CPU_D[0].qp());
  /* p23.XYLO*/ XYLO_LCDC_SPENn  .dff9(_XUBO_FF40_WRn, rst.XARE_SYS_RSTn(), cpu_bus.BUS_CPU_D[1].qp());
  /* p23.XYMO*/ XYMO_LCDC_SPSIZEn.dff9(_XUBO_FF40_WRn, rst.XARE_SYS_RSTn(), cpu_bus.BUS_CPU_D[2].qp());
  /* p23.XAFO*/ XAFO_LCDC_BGMAPn .dff9(_XUBO_FF40_WRn, rst.XARE_SYS_RSTn(), cpu_bus.BUS_CPU_D[3].qp());
  /* p23.WEXU*/ WEXU_LCDC_BGTILEn.dff9(_XUBO_FF40_WRn, rst.XARE_SYS_RSTn(), cpu_bus.BUS_CPU_D[4].qp());
  /* p23.WYMO*/ WYMO_LCDC_WINENn .dff9(_XUBO_FF40_WRn, rst.XARE_SYS_RSTn(), cpu_bus.BUS_CPU_D[5].qp());
  /* p23.WOKY*/ WOKY_LCDC_WINMAPn.dff9(_XUBO_FF40_WRn, rst.XARE_SYS_RSTn(), cpu_bus.BUS_CPU_D[6].qp());
  /* p23.WOKY*/ XONA_LCDC_LCDENn. dff9(_XUBO_FF40_WRn, rst.XARE_SYS_RSTn(), cpu_bus.BUS_CPU_D[7].qp());

  /* p23.VYRE*/ wire _VYRE_FF40_RDp = and2(cpu_bus.ASOT_CPU_RDp(), cpu_bus.VOCA_FF40p());
  /* p23.WYCE*/ wire _WYCE_FF40_RDn = not1(_VYRE_FF40_RDp);
  /*#p23.WYPO*/ cpu_bus.BUS_CPU_D_out[0].tri6_nn(_WYCE_FF40_RDn, VYXE_LCDC_BGENn.qp_new());
  /*#p23.XERO*/ cpu_bus.BUS_CPU_D_out[1].tri6_nn(_WYCE_FF40_RDn, XYLO_LCDC_SPENn.qp_new());
  /* p23.WYJU*/ cpu_bus.BUS_CPU_D_out[2].tri6_nn(_WYCE_FF40_RDn, XYMO_LCDC_SPSIZEn.qp_new());
  /* p23.WUKA*/ cpu_bus.BUS_CPU_D_out[3].tri6_nn(_WYCE_FF40_RDn, XAFO_LCDC_BGMAPn.qp_new());
  /* p23.VOKE*/ cpu_bus.BUS_CPU_D_out[4].tri6_nn(_WYCE_FF40_RDn, WEXU_LCDC_BGTILEn.qp_new());
  /* p23.VATO*/ cpu_bus.BUS_CPU_D_out[5].tri6_nn(_WYCE_FF40_RDn, WYMO_LCDC_WINENn.qp_new());
  /*#p23.VAHA*/ cpu_bus.BUS_CPU_D_out[6].tri6_nn(_WYCE_FF40_RDn, WOKY_LCDC_WINMAPn.qp_new());
  /*#p23.VAHA*/ cpu_bus.BUS_CPU_D_out[7].tri6_nn(_WYCE_FF40_RDn, XONA_LCDC_LCDENn.qp_new());
}

//------------------------------------------------------------------------------------------------------------------------

void RegLYC::tock(GateBoyResetDebug& rst, GateBoyClock& clk, GateBoyCpuBus& cpu_bus, const RegLY& reg_ly)
{
  // LYC matcher
  /* p21.RYME*/ wire _RYME_LY_MATCH0n_old = xor2(reg_ly.MUWY_LY0p.qp_old(), SYRY_LYC0n.qn_old());
  /* p21.TYDE*/ wire _TYDE_LY_MATCH1n_old = xor2(reg_ly.MYRO_LY1p.qp_old(), VUCE_LYC1n.qn_old());
  /* p21.REDA*/ wire _REDA_LY_MATCH2n_old = xor2(reg_ly.LEXA_LY2p.qp_old(), SEDY_LYC2n.qn_old());
  /* p21.RASY*/ wire _RASY_LY_MATCH3n_old = xor2(reg_ly.LYDO_LY3p.qp_old(), SALO_LYC3n.qn_old());
  /* p21.TYKU*/ wire _TYKU_LY_MATCH4n_old = xor2(reg_ly.LOVU_LY4p.qp_old(), SOTA_LYC4n.qn_old());
  /* p21.TUCY*/ wire _TUCY_LY_MATCH5n_old = xor2(reg_ly.LEMA_LY5p.qp_old(), VAFA_LYC5n.qn_old());
  /* p21.TERY*/ wire _TERY_LY_MATCH6n_old = xor2(reg_ly.MATO_LY6p.qp_old(), VEVO_LYC6n.qn_old());
  /* p21.SYFU*/ wire _SYFU_LY_MATCH7n_old = xor2(reg_ly.LAFO_LY7p.qp_old(), RAHA_LYC7n.qn_old());

  /*#p21.SOVU*/ wire _SOVU_LY_MATCHA_old = nor4 (_SYFU_LY_MATCH7n_old, _TERY_LY_MATCH6n_old, _TUCY_LY_MATCH5n_old, _TYKU_LY_MATCH4n_old);
  /*#p21.SUBO*/ wire _SUBO_LY_MATCHB_old = nor4 (_RASY_LY_MATCH3n_old, _REDA_LY_MATCH2n_old, _TYDE_LY_MATCH1n_old, _RYME_LY_MATCH0n_old);
  /*#p21.RAPE*/ wire _RAPE_LY_MATCHn_old = nand2(_SOVU_LY_MATCHA_old,  _SUBO_LY_MATCHB_old);
  /*#p21.PALY*/ wire _PALY_LY_MATCHa_old = not1 (_RAPE_LY_MATCHn_old);

  /*#p21.ROPO*/ ROPO_LY_MATCH_SYNCp.dff17(clk.TALU_xxCDEFxx(), rst.WESY_SYS_RSTn(), _PALY_LY_MATCHa_old);

  /* p23.XUFA*/ wire _XUFA_FF45_WRn = and2(cpu_bus.CUPA_CPU_WRp(), cpu_bus.XAYU_FF45p());
  /* p23.WANE*/ wire _WANE_FF45_WRp = not1(_XUFA_FF45_WRn);
  /* p23.SYRY*/ SYRY_LYC0n.dff9(_WANE_FF45_WRp, rst.WESY_SYS_RSTn(), cpu_bus.BUS_CPU_D[0].qp());
  /* p23.VUCE*/ VUCE_LYC1n.dff9(_WANE_FF45_WRp, rst.WESY_SYS_RSTn(), cpu_bus.BUS_CPU_D[1].qp());
  /* p23.SEDY*/ SEDY_LYC2n.dff9(_WANE_FF45_WRp, rst.WESY_SYS_RSTn(), cpu_bus.BUS_CPU_D[2].qp());
  /* p23.SALO*/ SALO_LYC3n.dff9(_WANE_FF45_WRp, rst.WESY_SYS_RSTn(), cpu_bus.BUS_CPU_D[3].qp());
  /* p23.SOTA*/ SOTA_LYC4n.dff9(_WANE_FF45_WRp, rst.WESY_SYS_RSTn(), cpu_bus.BUS_CPU_D[4].qp());
  /* p23.VAFA*/ VAFA_LYC5n.dff9(_WANE_FF45_WRp, rst.WESY_SYS_RSTn(), cpu_bus.BUS_CPU_D[5].qp());
  /* p23.VEVO*/ VEVO_LYC6n.dff9(_WANE_FF45_WRp, rst.WESY_SYS_RSTn(), cpu_bus.BUS_CPU_D[6].qp());
  /* p23.RAHA*/ RAHA_LYC7n.dff9(_WANE_FF45_WRp, rst.WESY_SYS_RSTn(), cpu_bus.BUS_CPU_D[7].qp());

  /* p23.XYLY*/ wire _XYLY_FF45_RDp = and2(cpu_bus.ASOT_CPU_RDp(), cpu_bus.XAYU_FF45p());
  /* p23.WEKU*/ wire _WEKU_FF45_RDn = not1(_XYLY_FF45_RDp);
  /*#p23.RETU*/ cpu_bus.BUS_CPU_D_out[0].tri6_nn(_WEKU_FF45_RDn, SYRY_LYC0n.qp_new());
  /* p23.VOJO*/ cpu_bus.BUS_CPU_D_out[1].tri6_nn(_WEKU_FF45_RDn, VUCE_LYC1n.qp_new());
  /* p23.RAZU*/ cpu_bus.BUS_CPU_D_out[2].tri6_nn(_WEKU_FF45_RDn, SEDY_LYC2n.qp_new());
  /* p23.REDY*/ cpu_bus.BUS_CPU_D_out[3].tri6_nn(_WEKU_FF45_RDn, SALO_LYC3n.qp_new());
  /* p23.RACE*/ cpu_bus.BUS_CPU_D_out[4].tri6_nn(_WEKU_FF45_RDn, SOTA_LYC4n.qp_new());
  /*#p23.VAZU*/ cpu_bus.BUS_CPU_D_out[5].tri6_nn(_WEKU_FF45_RDn, VAFA_LYC5n.qp_new());
  /* p23.VAFE*/ cpu_bus.BUS_CPU_D_out[6].tri6_nn(_WEKU_FF45_RDn, VEVO_LYC6n.qp_new());
  /* p23.PUFY*/ cpu_bus.BUS_CPU_D_out[7].tri6_nn(_WEKU_FF45_RDn, RAHA_LYC7n.qp_new());
}

//------------------------------------------------------------------------------------------------------------------------

wire GateBoyLCD::ATEJ_LINE_RSTp() const {
  /* p28.ABAF*/ wire _ABAF_LINE_P000n = not1(CATU_LINE_P000p.qp());
  /* p28.BYHA*/ wire _BYHA_LINE_TRIGn = or_and3(ANEL_LINE_P002p.qp(), _ABAF_LINE_P000n, ABEZ_VID_RSTn(_XODO_VID_RSTp.qp())); // so if this is or_and, BYHA should go low on 910 and 911
  /* p28.ATEJ*/ wire _ATEJ_LINE_RSTp = not1(_BYHA_LINE_TRIGn);
  return _ATEJ_LINE_RSTp;
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoyLCD::tock(
  GateBoyResetDebug& rst,
  GateBoyClock& clk,
  const RegLX& reg_lx,
  wire XYVO_y144p_old,
  wire RUTU_x113p_old)
{
  _XODO_VID_RSTp = rst.XODO_VID_RSTp();

  /*#p21.POPU*/ POPU_VBLANKp.dff17(reg_lx.NYPE_x113p(), rst.LYFE_VID_RSTn(), XYVO_y144p_old);

  /*#p21.PURE*/ wire _PURE_LINE_ENDn_old = not1(RUTU_x113p_old);
  /*#p21.SELA*/ wire _SELA_LINE_P908p_old = not1(_PURE_LINE_ENDn_old);
  /*#p29.ALES*/ wire _ALES_y144n_old = not1(XYVO_y144p_old);
  /*#p29.ABOV*/ wire _ABOV_LINE_P908p_old = and2(_SELA_LINE_P908p_old, _ALES_y144n_old);
  /*#p28.ANEL*/ ANEL_LINE_P002p.dff17(clk.AWOH_xxCDxxGH(), rst.ABEZ_VID_RSTn(),  CATU_LINE_P000p.qp_old());
  /*#p29.CATU*/ CATU_LINE_P000p.dff17(clk.XUPY_ABxxEFxx(), rst.ABEZ_VID_RSTn(), _ABOV_LINE_P908p_old);
}

//--------------------------------------------------------------------------------

void GateBoyLCD::set_pin_ctrl(GateBoyResetDebug& rst, GateBoyClock& clk, const RegLX& reg_lx) {
  /*#p21.SYGU*/ SYGU_LINE_STROBE.dff17(clk.SONO_ABxxxxGH(), rst.LYFE_VID_RSTn(), reg_lx.TEGY_STROBE());
  /*#p21.RYNO*/ wire _RYNO = or2(SYGU_LINE_STROBE.qp_new(), reg_lx.RUTU_x113p());
  /*#p21.POGU*/ wire _POGU = not1(_RYNO);
  PIN_LCD_CNTRL.pin_out(_POGU, _POGU);
}

//--------------------------------------------------------------------------------
// if LCDC_ENn, PIN_LCD_FLIPS = 4k div clock. Otherwise PIN_LCD_FLIPS = xor(LINE_evn,FRAME_evn)

void GateBoyLCD::set_pin_flip(GateBoyResetDebug& rst, const RegLX& reg_lx, wire TULU_DIV07p, wire XONA_LCDC_LCDENp) {
  /*#p24.LUCA*/ LUCA_LINE_EVENp .dff17(reg_lx.LOFU_x113n(),   rst.LYFE_VID_RSTn(), LUCA_LINE_EVENp.qn());
  /*#p21.NAPO*/ NAPO_FRAME_EVENp.dff17(POPU_VBLANKp.qp_new(), rst.LYFE_VID_RSTn(), NAPO_FRAME_EVENp.qn());

  /*#p24.MAGU*/ wire _MAGU = xor2(NAPO_FRAME_EVENp.qp_new(), LUCA_LINE_EVENp.qn_new());
  /*#p24.MECO*/ wire _MECO = not1(_MAGU);
  /*#p24.KEBO*/ wire _KEBO = not1(_MECO);
  /* p01.UREK*/ wire _UREK_DIV07n = not1(TULU_DIV07p);
  /*#p24.USEC*/ wire _USEC_DIV07p = not1(_UREK_DIV07n);
  /*#p24.KEDY*/ wire _KEDY_LCDC_ENn = not1(XONA_LCDC_LCDENp);
  /*#p24.KUPA*/ wire _KUPA = amux2(XONA_LCDC_LCDENp, _KEBO, _KEDY_LCDC_ENn, _USEC_DIV07p);
  /*#p24.KOFO*/ wire _KOFO = not1(_KUPA);
  PIN_LCD_FLIPS.pin_out(_KOFO, _KOFO);
}

//--------------------------------------------------------------------------------

void GateBoyLCD::set_pin_vsync(GateBoyResetDebug& rst, const RegLX& reg_lx, const RegLY& reg_ly) {
  /*#p24.MEDA*/ MEDA_VSYNC_OUTn.dff17(reg_lx.NYPE_x113n(), rst.LYFE_VID_RSTn(), reg_ly.NERU_y000p());
  /*#p24.MURE*/ wire _MURE_VSYNC = not1(MEDA_VSYNC_OUTn.qp_new());
  PIN_LCD_VSYNC.pin_out(_MURE_VSYNC, _MURE_VSYNC);
}

//--------------------------------------------------------------------------------
// FIXME inversion
// I don't know why ROXO has to be inverted here but it extends HSYNC by one phase, which
// seems to be correct and makes it match the trace. With that change, HSYNC is 30 phases.
// Is it possible that it should be 29 phases and it only looks like 30 phases because of gate delay?
// That would be a loooot of gate delay.
// Could we possibly be incrementing X3p one phase early?

void GateBoyLCD::set_pin_hsync(GateBoyResetDebug& rst, wire TYFA_CLKPIPE_odd, wire XYMU_RENDERINGp, wire XYDO_PX3p, wire AVAP_SCAN_DONE_TRIGp) {
  /*#p24.SEGU*/ wire _SEGU_CLKPIPE_evn = not1(TYFA_CLKPIPE_odd);
  /*#p24.ROXO*/ wire _ROXO_CLKPIPE_odd = not1(_SEGU_CLKPIPE_evn);
  /* p24.PAHO*/ PAHO_X_8_SYNC.dff17(!_ROXO_CLKPIPE_odd, XYMU_RENDERINGp, XYDO_PX3p);

  // LCD horizontal sync pin latch
  /*#p24.POME*/ POME.set(nor2(AVAP_SCAN_DONE_TRIGp, POFY.qp_old()));
  /*#p24.RUJU*/ RUJU.set(or3(PAHO_X_8_SYNC.qp_new(), rst.TOFU_VID_RSTp(), POME.qp_new()));
  /*#p24.POFY*/ POFY.set(not1(RUJU.qp_new()));
  /*#p24.POME*/ POME.set(nor2(AVAP_SCAN_DONE_TRIGp, POFY.qp_new()));
  /*#p24.RUJU*/ RUJU.set(or3(PAHO_X_8_SYNC.qp_new(), rst.TOFU_VID_RSTp(), POME.qp_new()));
  /*#p24.POFY*/ POFY.set(not1(RUJU.qp_new()));

  /*#p24.RUZE*/ wire _RUZE_HSYNCn = not1(POFY.qp_new());
  PIN_LCD_HSYNC.pin_out(_RUZE_HSYNCn, _RUZE_HSYNCn);
}

//--------------------------------------------------------------------------------

void GateBoyLCD::set_pin_data(wire REMY_LD0n, wire RAVO_LD1n) {
  PIN_LCD_DATA0.pin_out(REMY_LD0n, REMY_LD0n);
  PIN_LCD_DATA1.pin_out(RAVO_LD1n, RAVO_LD1n);
}

void GateBoyLCD::set_pin_latch(GateBoyDiv& div, RegLX& reg_lx, RegLCDC& reg_lcdc) {
  /* p01.UMEK*/ wire _UMEK_DIV06n = not1(div.UGOT_DIV06p.qp_new());
  /*#p24.KASA*/ wire _KASA_LINE_ENDp = not1(reg_lx.PURE_LINE_ENDn());
  /*#p24.UMOB*/ wire _UMOB_DIV_06p = not1(_UMEK_DIV06n);
  /*#p24.KEDY*/ wire _KEDY_LCDC_ENn = not1(reg_lcdc.XONA_LCDC_LCDENn.qn_new());
  /*#p24.KAHE*/ wire _KAHE_LINE_ENDp = amux2(reg_lcdc.XONA_LCDC_LCDENn.qn_new(), _KASA_LINE_ENDp, _KEDY_LCDC_ENn, _UMOB_DIV_06p);
  /*#p24.KYMO*/ wire _KYMO_LINE_ENDn = not1(_KAHE_LINE_ENDp);
  PIN_LCD_LATCH.pin_out(_KYMO_LINE_ENDn, _KYMO_LINE_ENDn);
}

void GateBoyLCD::set_pin_clock(PixCount& pix_count, FineScroll& fine_scroll, wire WEGO_HBLANKp, wire SACU_CLKPIPE_evn) {
  /*#p21.XAJO*/ wire _XAJO_X_009p = and2(pix_count.XEHO_PX0p.qp_new(), pix_count.XYDO_PX3p.qp_new());
  /*#p21.WUSA*/ WUSA_LCD_CLOCK_GATE.nor_latch(_XAJO_X_009p, WEGO_HBLANKp);
  /*#p21.TOBA*/ wire _TOBA_LCD_CLOCK = and2(WUSA_LCD_CLOCK_GATE.qp_new(), SACU_CLKPIPE_evn);
  /*#p21.SEMU*/ wire _SEMU_LCD_CLOCK = or2(_TOBA_LCD_CLOCK, fine_scroll.POVA_FINE_MATCH_TRIGp());
  /*#p21.RYPO*/ wire _RYPO_LCD_CLOCK = not1(_SEMU_LCD_CLOCK);
  PIN_LCD_CLOCK.pin_out(_RYPO_LCD_CLOCK, _RYPO_LCD_CLOCK);
}

//------------------------------------------------------------------------------------------------------------------------