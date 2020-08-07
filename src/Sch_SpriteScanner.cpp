#include "Sch_SpriteScanner.h"
#include "Sch_Top.h"

using namespace Schematics;

void SpriteScanner::dump(Dumper& d, const SchematicTop& top) const {
  d("----------SpriteScan ---------\n");

  /*p29.BEBU*/ wire _BEBU_SCAN_DONE_TRIGn = or3(_BALU_LINE_RSTp, DOBA_SCAN_DONE_B.qp(), BYBA_SCAN_DONE_A.qn());
  /*p29.AVAP*/ wire AVAP_RENDER_START_TRIGp = not1(_BEBU_SCAN_DONE_TRIGn);
  wire CATU_VID_LINE_ENDp = top.lcd_reg.CATU_VID_LINE_ENDp();
  /*p28.ASEN*/ wire _ASEN_SCAN_DONE_PE = or2(top.clk_reg.ATAR_VID_RSTp(), AVAP_RENDER_START_TRIGp);

  d("_BALU_LINE_RSTp         %d\n", _BALU_LINE_RSTp.as_wire());
  d("_BEBU_SCAN_DONE_TRIGn   %d\n", _BEBU_SCAN_DONE_TRIGn);
  d("AVAP_RENDER_START_TRIGp %d\n", AVAP_RENDER_START_TRIGp);
  d("CATU_VID_LINE_ENDp      %d\n", CATU_VID_LINE_ENDp);
  d("_ASEN_SCAN_DONE_PE      %d\n", _ASEN_SCAN_DONE_PE);

  d("BESU_SCANNINGp   %c\n", BESU_SCANNINGp  .c());
  d("CENO_SCANNINGp   %c\n", CENO_SCANNINGp  .c());
  d("BYBA_SCAN_DONE_A %c\n", BYBA_SCAN_DONE_A.c());
  d("DOBA_SCAN_DONE_B %c\n", DOBA_SCAN_DONE_B.c());

  /*p28.FETO*/ wire _FETO_SCAN_DONE_d0 = and4(_YFEL_SCAN0.qp(), _WEWY_SCAN1.qp(), _GOSO_SCAN2.qp(), _FONY_SCAN5.qp()); // die AND. 32 + 4 + 2 + 1 = 39
  /*p28.GAVA*/ wire _GAVA_SCAN_CLK = or2(_FETO_SCAN_DONE_d0,   top.clk_reg.XUPY_xxCDxxGH());
  /*p28.ANOM*/ wire ANOM_LINE_RSTn = nor2(top.lcd_reg.ATEJ_VID_LINE_TRIGp(), top.clk_reg.ATAR_VID_RSTp());
  d("_GAVA_SCAN_CLK %d\n", _GAVA_SCAN_CLK);
  d("ANOM_LINE_RSTn %d\n", ANOM_LINE_RSTn);
  d("SCAN INDEX        %02d\n", 
    pack_p(
      _YFEL_SCAN0.qp(),
      _WEWY_SCAN1.qp(),
      _GOSO_SCAN2.qp(),
      _ELYN_SCAN3.qp(),
      _FAHA_SCAN4.qp(),
      _FONY_SCAN5.qp(),
      0,
      0
    )
  );

  int y_diff = pack_p(_ERUC_YDIFF_S0, _ENEF_YDIFF_S1, _FECO_YDIFF_S2, _GYKY_YDIFF_S3,
                      _GOPU_YDIFF_S4, _FUWA_YDIFF_S5, _GOJU_YDIFF_S6, _WUHU_YDIFF_S7);

  uint8_t lcd_y  = top.lcd_reg.get_y();

  d("LCD Y    %03d\n", lcd_y);
  d("Y DIFF   %03d\n", y_diff);

  d("_GACE_SPRITE_DELTA4      %d\n", (wire)_GACE_SPRITE_DELTA4);
  d("_GUVU_SPRITE_DELTA5      %d\n", (wire)_GUVU_SPRITE_DELTA5);
  d("_GYDA_SPRITE_DELTA6      %d\n", (wire)_GYDA_SPRITE_DELTA6);
  d("_GEWY_SPRITE_DELTA7      %d\n", (wire)_GEWY_SPRITE_DELTA7);
  d("_GOVU_SPSIZE_MATCH       %d\n", (wire)_GOVU_SPSIZE_MATCH );
  d("_WOTA_SCAN_MATCH_Yn      %d\n", (wire)_WOTA_SCAN_MATCH_Yn);
  d("_GESE_SCAN_MATCH_Y       %d\n", (wire)_GESE_SCAN_MATCH_Y );
  d("_CARE_STORE_ENp_ABxxEFxx %d\n", (wire)_CARE_STORE_ENp_ABxxEFxx);
  d("\n");
}


//------------------------------------------------------------------------------

void SpriteScanner::tick(const SchematicTop& top) {
  wire GND = 0;

  {
    _XYMU_RENDERINGp = top.pix_pipe.XYMU_RENDERINGp();

    /*p28.ANOM*/ wire ANOM_LINE_RSTn = nor2(top.lcd_reg.ATEJ_VID_LINE_TRIGp(), top.clk_reg.ATAR_VID_RSTp());

    /*p29.BALU*/ _BALU_LINE_RSTp = not1(ANOM_LINE_RSTn);
    /*p29.BAGY*/ _BAGY_LINE_RSTn = not1(_BALU_LINE_RSTp);
  }

  {
    /*p29.EBOS*/ wire _EBOS_Y0n = not1(top.lcd_reg.MUWY_Y0.qp());
    /*p29.DASA*/ wire _DASA_Y1n = not1(top.lcd_reg.MYRO_Y1.qp());
    /*p29.FUKY*/ wire _FUKY_Y2n = not1(top.lcd_reg.LEXA_Y2.qp());
    /*p29.FUVE*/ wire _FUVE_Y3n = not1(top.lcd_reg.LYDO_Y3.qp());
    /*p29.FEPU*/ wire _FEPU_Y4n = not1(top.lcd_reg.LOVU_Y4.qp());
    /*p29.FOFA*/ wire _FOFA_Y5n = not1(top.lcd_reg.LEMA_Y5.qp());
    /*p29.FEMO*/ wire _FEMO_Y6n = not1(top.lcd_reg.MATO_Y6.qp());
    /*p29.GUSU*/ wire _GUSU_Y7n = not1(top.lcd_reg.LAFO_Y7.qp());

    /*p29.ERUC*/ _ERUC_YDIFF_S0 = add_s(_EBOS_Y0n, top.oam_bus.XUSO_OAM_DA0.qp(), GND);
    /*p29.ERUC*/ _ERUC_YDIFF_C0 = add_c(_EBOS_Y0n, top.oam_bus.XUSO_OAM_DA0.qp(), GND);
    /*p29.ENEF*/ _ENEF_YDIFF_S1 = add_s(_DASA_Y1n, top.oam_bus.XEGU_OAM_DA1.qp(), _ERUC_YDIFF_C0);
    /*p29.ENEF*/ _ENEF_YDIFF_C1 = add_c(_DASA_Y1n, top.oam_bus.XEGU_OAM_DA1.qp(), _ERUC_YDIFF_C0);
    /*p29.FECO*/ _FECO_YDIFF_S2 = add_s(_FUKY_Y2n, top.oam_bus.YJEX_OAM_DA2.qp(), _ENEF_YDIFF_C1);
    /*p29.FECO*/ _FECO_YDIFF_C2 = add_c(_FUKY_Y2n, top.oam_bus.YJEX_OAM_DA2.qp(), _ENEF_YDIFF_C1);
    /*p29.GYKY*/ _GYKY_YDIFF_S3 = add_s(_FUVE_Y3n, top.oam_bus.XYJU_OAM_DA3.qp(), _FECO_YDIFF_C2);
    /*p29.GYKY*/ _GYKY_YDIFF_C3 = add_c(_FUVE_Y3n, top.oam_bus.XYJU_OAM_DA3.qp(), _FECO_YDIFF_C2);
    /*p29.GOPU*/ _GOPU_YDIFF_S4 = add_s(_FEPU_Y4n, top.oam_bus.YBOG_OAM_DA4.qp(), _GYKY_YDIFF_C3);
    /*p29.GOPU*/ _GOPU_YDIFF_C4 = add_c(_FEPU_Y4n, top.oam_bus.YBOG_OAM_DA4.qp(), _GYKY_YDIFF_C3);
    /*p29.FUWA*/ _FUWA_YDIFF_S5 = add_s(_FOFA_Y5n, top.oam_bus.WYSO_OAM_DA5.qp(), _GOPU_YDIFF_C4);
    /*p29.FUWA*/ _FUWA_YDIFF_C5 = add_c(_FOFA_Y5n, top.oam_bus.WYSO_OAM_DA5.qp(), _GOPU_YDIFF_C4);
    /*p29.GOJU*/ _GOJU_YDIFF_S6 = add_s(_FEMO_Y6n, top.oam_bus.XOTE_OAM_DA6.qp(), _FUWA_YDIFF_C5);
    /*p29.GOJU*/ _GOJU_YDIFF_C6 = add_c(_FEMO_Y6n, top.oam_bus.XOTE_OAM_DA6.qp(), _FUWA_YDIFF_C5);
    /*p29.WUHU*/ _WUHU_YDIFF_S7 = add_s(_GUSU_Y7n, top.oam_bus.YZAB_OAM_DA7.qp(), _GOJU_YDIFF_C6);
    /*p29.WUHU*/ _WUHU_YDIFF_C7 = add_c(_GUSU_Y7n, top.oam_bus.YZAB_OAM_DA7.qp(), _GOJU_YDIFF_C6);
  }

  {
    /*p29.GACE*/ _GACE_SPRITE_DELTA4 = not1(_GOPU_YDIFF_S4);
    /*p29.GUVU*/ _GUVU_SPRITE_DELTA5 = not1(_FUWA_YDIFF_S5);
    /*p29.GYDA*/ _GYDA_SPRITE_DELTA6 = not1(_GOJU_YDIFF_S6);
    /*p29.GEWY*/ _GEWY_SPRITE_DELTA7 = not1(_WUHU_YDIFF_S7);

    /*p29.GOVU*/ _GOVU_SPSIZE_MATCH = or2(_GYKY_YDIFF_S3, top.pix_pipe.XYMO_LCDC_SPSIZE.qp());
    /*p29.WOTA*/ _WOTA_SCAN_MATCH_Yn = nand6(_GACE_SPRITE_DELTA4, _GUVU_SPRITE_DELTA5, _GYDA_SPRITE_DELTA6, _GEWY_SPRITE_DELTA7, _WUHU_YDIFF_C7, _GOVU_SPSIZE_MATCH);
    /*p29.GESE*/ _GESE_SCAN_MATCH_Y = not1(_WOTA_SCAN_MATCH_Yn);
    /*p29.CARE*/ _CARE_STORE_ENp_ABxxEFxx = and3(top.clk_reg.XOCE_AxxDExxH(), top.sprite_scanner.CEHA_SCANNINGp(), _GESE_SCAN_MATCH_Y); // Dots on VCC, this is AND. Die shot and schematic wrong.
  }
}

//------------------------------------------------------------------------------

void SpriteScanner::tock(const SchematicTop& top) {

  /*p28.FETO*/ wire _FETO_SCAN_DONE_d0 = and4(_YFEL_SCAN0.qp(), _WEWY_SCAN1.qp(), _GOSO_SCAN2.qp(), _FONY_SCAN5.qp()); // die AND. 32 + 4 + 2 + 1 = 39

  //----------------------------------------
  // Sprite scan trigger & reset. Why it resets both before and after the scan I do not know.

  {
    /*p29.BYBA*/ BYBA_SCAN_DONE_A = dff17_AB(top.clk_reg.XUPY_xxCDxxGH(), _BAGY_LINE_RSTn, _FETO_SCAN_DONE_d0);
    /*p29.DOBA*/ DOBA_SCAN_DONE_B = dff17_B (top.clk_reg.ALET_xBxDxFxH(), _BAGY_LINE_RSTn, BYBA_SCAN_DONE_A.qp());

    /*p29.BEBU*/ wire _BEBU_SCAN_DONE_TRIGn = or3(_BALU_LINE_RSTp, DOBA_SCAN_DONE_B.qp(), BYBA_SCAN_DONE_A.qn());
    /*p29.AVAP*/ wire AVAP_RENDER_START_TRIGp = not1(_BEBU_SCAN_DONE_TRIGn);

    /*p28.ASEN*/ wire _ASEN_SCAN_DONE_PE = or2(top.clk_reg.ATAR_VID_RSTp(), AVAP_RENDER_START_TRIGp);
    /*p28.BESU*/ BESU_SCANNINGp = nor_latch(top.lcd_reg.CATU_VID_LINE_ENDp(), _ASEN_SCAN_DONE_PE);
    /*p29.CENO*/ CENO_SCANNINGp = dff17_A(top.clk_reg.XUPY_xxCDxxGH(), top.clk_reg.ABEZ_VID_RSTn(), BESU_SCANNINGp.qp());
  }

  //----------------------------------------
  // Sprite scan counter
  // Sprite scan takes 160 phases, 4 phases per sprite.

  {
    /*p28.GAVA*/ wire _GAVA_SCAN_CLK = or2(_FETO_SCAN_DONE_d0,   top.clk_reg.XUPY_xxCDxxGH());
    /*p28.ANOM*/ wire ANOM_LINE_RSTn = nor2(top.lcd_reg.ATEJ_VID_LINE_TRIGp(), top.clk_reg.ATAR_VID_RSTp());

    /*p28.YFEL*/ _YFEL_SCAN0 = dff17_AB(_GAVA_SCAN_CLK,   ANOM_LINE_RSTn, _YFEL_SCAN0.qn());
    /*p28.WEWY*/ _WEWY_SCAN1 = dff17_AB(_YFEL_SCAN0.qn(), ANOM_LINE_RSTn, _WEWY_SCAN1.qn());
    /*p28.GOSO*/ _GOSO_SCAN2 = dff17_AB(_WEWY_SCAN1.qn(), ANOM_LINE_RSTn, _GOSO_SCAN2.qn());
    /*p28.ELYN*/ _ELYN_SCAN3 = dff17_AB(_GOSO_SCAN2.qn(), ANOM_LINE_RSTn, _ELYN_SCAN3.qn());
    /*p28.FAHA*/ _FAHA_SCAN4 = dff17_AB(_ELYN_SCAN3.qn(), ANOM_LINE_RSTn, _FAHA_SCAN4.qn());
    /*p28.FONY*/ _FONY_SCAN5 = dff17_AB(_FAHA_SCAN4.qn(), ANOM_LINE_RSTn, _FONY_SCAN5.qn());
  }
}

//------------------------------------------------------------------------------
