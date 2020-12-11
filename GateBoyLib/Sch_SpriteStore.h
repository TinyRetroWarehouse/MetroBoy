#pragma once
#include "GateBoyLib/Gates.h"

//-----------------------------------------------------------------------------

struct SpriteStore {
  void reset_cart() {
    DEZY_COUNT_CLKp_evn.reset(REG_D1C1);
    BESE_SPRITE_COUNT0_evn.reset(REG_D0C1);
    CUXY_SPRITE_COUNT1_evn.reset(REG_D0C1);
    BEGO_SPRITE_COUNT2_evn.reset(REG_D0C1);
    DYBE_SPRITE_COUNT3_evn.reset(REG_D0C1);

    EBOJ_STORE0_RSTp_evn.reset(REG_D0C0);
    CEDY_STORE1_RSTp_evn.reset(REG_D0C0);
    EGAV_STORE2_RSTp_evn.reset(REG_D0C0);
    GOTA_STORE3_RSTp_evn.reset(REG_D0C0);
    XUDY_STORE4_RSTp_evn.reset(REG_D0C0);
    WAFY_STORE5_RSTp_evn.reset(REG_D0C0);
    WOMY_STORE6_RSTp_evn.reset(REG_D0C0);
    WAPO_STORE7_RSTp_evn.reset(REG_D0C0);
    EXUQ_STORE8_RSTp_evn.reset(REG_D0C0);
    FONO_STORE9_RSTp_evn.reset(REG_D0C0);

    XADU_SPRITE_IDX0p_evn.reset(REG_D0C1);
    XEDY_SPRITE_IDX1p_evn.reset(REG_D0C1);
    ZUZE_SPRITE_IDX2p_evn.reset(REG_D1C1);
    XOBE_SPRITE_IDX3p_evn.reset(REG_D0C1);
    YDUF_SPRITE_IDX4p_evn.reset(REG_D1C1);
    XECU_SPRITE_IDX5p_evn.reset(REG_D0C1);



    YGUS_STORE0_I0n_odd.reset(REG_D0C0);
    YSOK_STORE0_I1n_odd.reset(REG_D0C0);
    YZEP_STORE0_I2n_odd.reset(REG_D0C0);
    WYTE_STORE0_I3n_odd.reset(REG_D0C0);
    ZONY_STORE0_I4n_odd.reset(REG_D0C0);
    YWAK_STORE0_I5n_odd.reset(REG_D0C0);

    GYHO_STORE0_L0n_odd.reset(REG_D0C0);
    CUFO_STORE0_L1n_odd.reset(REG_D0C0);
    BOZU_STORE0_L2n_odd.reset(REG_D0C0);
    FYHY_STORE0_L3n_odd.reset(REG_D0C0);

    XEPE_STORE0_X0p_odd.reset(REG_D1C1);
    YLAH_STORE0_X1p_odd.reset(REG_D1C1);
    ZOLA_STORE0_X2p_odd.reset(REG_D1C1);
    ZULU_STORE0_X3p_odd.reset(REG_D1C1);
    WELO_STORE0_X4p_odd.reset(REG_D1C1);
    XUNY_STORE0_X5p_odd.reset(REG_D1C1);
    WOTE_STORE0_X6p_odd.reset(REG_D1C1);
    XAKO_STORE0_X7p_odd.reset(REG_D1C1);



    CADU_STORE1_I0n_odd.reset(REG_D0C0);
    CEBO_STORE1_I1n_odd.reset(REG_D0C0);
    CUFA_STORE1_I2n_odd.reset(REG_D0C0);
    COMA_STORE1_I3n_odd.reset(REG_D0C0);
    CUZA_STORE1_I4n_odd.reset(REG_D0C0);
    CAJY_STORE1_I5n_odd.reset(REG_D0C0);

    AMES_STORE1_L0n_odd.reset(REG_D0C0);
    AROF_STORE1_L1n_odd.reset(REG_D0C0);
    ABOP_STORE1_L2n_odd.reset(REG_D0C0);
    ABUG_STORE1_L3n_odd.reset(REG_D0C0);

    DANY_STORE1_X0p_odd.reset(REG_D1C1);
    DUKO_STORE1_X1p_odd.reset(REG_D1C1);
    DESU_STORE1_X2p_odd.reset(REG_D1C1);
    DAZO_STORE1_X3p_odd.reset(REG_D1C1);
    DAKE_STORE1_X4p_odd.reset(REG_D1C1);
    CESO_STORE1_X5p_odd.reset(REG_D1C1);
    DYFU_STORE1_X6p_odd.reset(REG_D1C1);
    CUSY_STORE1_X7p_odd.reset(REG_D1C1);



    BUHE_STORE2_I0n_odd.reset(REG_D0C0);
    BYHU_STORE2_I1n_odd.reset(REG_D0C0);
    BECA_STORE2_I2n_odd.reset(REG_D0C0);
    BULU_STORE2_I3n_odd.reset(REG_D0C0);
    BUNA_STORE2_I4n_odd.reset(REG_D0C0);
    BOXA_STORE2_I5n_odd.reset(REG_D0C0);

    YLOV_STORE2_L0n_odd.reset(REG_D0C0);
    XOSY_STORE2_L1n_odd.reset(REG_D0C0);
    XAZY_STORE2_L2n_odd.reset(REG_D0C0);
    YKUK_STORE2_L3n_odd.reset(REG_D0C0);

    FOKA_STORE2_X0p_odd.reset(REG_D1C1);
    FYTY_STORE2_X1p_odd.reset(REG_D1C1);
    FUBY_STORE2_X2p_odd.reset(REG_D1C1);
    GOXU_STORE2_X3p_odd.reset(REG_D1C1);
    DUHY_STORE2_X4p_odd.reset(REG_D1C1);
    EJUF_STORE2_X5p_odd.reset(REG_D1C1);
    ENOR_STORE2_X6p_odd.reset(REG_D1C1);
    DEPY_STORE2_X7p_odd.reset(REG_D1C1);



    DEVY_STORE3_I0n_odd.reset(REG_D0C0);
    DESE_STORE3_I1n_odd.reset(REG_D0C0);
    DUNY_STORE3_I2n_odd.reset(REG_D0C0);
    DUHA_STORE3_I3n_odd.reset(REG_D0C0);
    DEBA_STORE3_I4n_odd.reset(REG_D0C0);
    DAFU_STORE3_I5n_odd.reset(REG_D0C0);

    ZURO_STORE3_L0n_odd.reset(REG_D0C0);
    ZYLU_STORE3_L1n_odd.reset(REG_D0C0);
    ZENE_STORE3_L2n_odd.reset(REG_D0C0);
    ZURY_STORE3_L3n_odd.reset(REG_D0C0);

    XOLY_STORE3_X0p_odd.reset(REG_D1C1);
    XYBA_STORE3_X1p_odd.reset(REG_D1C1);
    XABE_STORE3_X2p_odd.reset(REG_D1C1);
    XEKA_STORE3_X3p_odd.reset(REG_D1C1);
    XOMY_STORE3_X4p_odd.reset(REG_D1C1);
    WUHA_STORE3_X5p_odd.reset(REG_D1C1);
    WYNA_STORE3_X6p_odd.reset(REG_D1C1);
    WECO_STORE3_X7p_odd.reset(REG_D1C1);



    XAVE_STORE4_I0n_odd.reset(REG_D0C0);
    XEFE_STORE4_I1n_odd.reset(REG_D0C0);
    WANU_STORE4_I2n_odd.reset(REG_D0C0);
    XABO_STORE4_I3n_odd.reset(REG_D0C0);
    XEGE_STORE4_I4n_odd.reset(REG_D0C0);
    XYNU_STORE4_I5n_odd.reset(REG_D0C0);

    CAPO_STORE4_L0n_odd.reset(REG_D0C0);
    CAJU_STORE4_L1n_odd.reset(REG_D0C0);
    CONO_STORE4_L2n_odd.reset(REG_D0C0);
    CUMU_STORE4_L3n_odd.reset(REG_D0C0);

    WEDU_STORE4_X0p_odd.reset(REG_D1C1);
    YGAJ_STORE4_X1p_odd.reset(REG_D1C1);
    ZYJO_STORE4_X2p_odd.reset(REG_D1C1);
    XURY_STORE4_X3p_odd.reset(REG_D1C1);
    YBED_STORE4_X4p_odd.reset(REG_D1C1);
    ZALA_STORE4_X5p_odd.reset(REG_D1C1);
    WYDE_STORE4_X6p_odd.reset(REG_D1C1);
    XEPA_STORE4_X7p_odd.reset(REG_D1C1);



    EKOP_STORE5_I0n_odd.reset(REG_D0C0);
    ETYM_STORE5_I1n_odd.reset(REG_D0C0);
    GORU_STORE5_I2n_odd.reset(REG_D0C0);
    EBEX_STORE5_I3n_odd.reset(REG_D0C0);
    ETAV_STORE5_I4n_odd.reset(REG_D0C0);
    EKAP_STORE5_I5n_odd.reset(REG_D0C0);

    ACEP_STORE5_L0n_odd.reset(REG_D0C0);
    ABEG_STORE5_L1n_odd.reset(REG_D0C0);
    ABUX_STORE5_L2n_odd.reset(REG_D0C0);
    ANED_STORE5_L3n_odd.reset(REG_D0C0);

    FUSA_STORE5_X0p_odd.reset(REG_D1C1);
    FAXA_STORE5_X1p_odd.reset(REG_D1C1);
    FOZY_STORE5_X2p_odd.reset(REG_D1C1);
    FESY_STORE5_X3p_odd.reset(REG_D1C1);
    CYWE_STORE5_X4p_odd.reset(REG_D1C1);
    DYBY_STORE5_X5p_odd.reset(REG_D1C1);
    DURY_STORE5_X6p_odd.reset(REG_D1C1);
    CUVY_STORE5_X7p_odd.reset(REG_D1C1);



    GABO_STORE6_I0n_odd.reset(REG_D0C0);
    GACY_STORE6_I1n_odd.reset(REG_D0C0);
    FOGO_STORE6_I2n_odd.reset(REG_D0C0);
    GOHU_STORE6_I3n_odd.reset(REG_D0C0);
    FOXY_STORE6_I4n_odd.reset(REG_D0C0);
    GECU_STORE6_I5n_odd.reset(REG_D0C0);

    ZUMY_STORE6_L0n_odd.reset(REG_D0C0);
    ZAFU_STORE6_L1n_odd.reset(REG_D0C0);
    ZEXO_STORE6_L2n_odd.reset(REG_D0C0);
    ZUBE_STORE6_L3n_odd.reset(REG_D0C0);

    YCOL_STORE6_X0p_odd.reset(REG_D1C1);
    YRAC_STORE6_X1p_odd.reset(REG_D1C1);
    YMEM_STORE6_X2p_odd.reset(REG_D1C1);
    YVAG_STORE6_X3p_odd.reset(REG_D1C1);
    ZOLY_STORE6_X4p_odd.reset(REG_D1C1);
    ZOGO_STORE6_X5p_odd.reset(REG_D1C1);
    ZECU_STORE6_X6p_odd.reset(REG_D1C1);
    ZESA_STORE6_X7p_odd.reset(REG_D1C1);



    GULE_STORE7_I0n_odd.reset(REG_D0C0);
    GYNO_STORE7_I1n_odd.reset(REG_D0C0);
    FEFA_STORE7_I2n_odd.reset(REG_D0C0);
    FYSU_STORE7_I3n_odd.reset(REG_D0C0);
    GESY_STORE7_I4n_odd.reset(REG_D0C0);
    FUZO_STORE7_I5n_odd.reset(REG_D0C0);

    XYNA_STORE7_L0n_odd.reset(REG_D0C0);
    YGUM_STORE7_L1n_odd.reset(REG_D0C0);
    XAKU_STORE7_L2n_odd.reset(REG_D0C0);
    XYGO_STORE7_L3n_odd.reset(REG_D0C0);

    ERAZ_STORE7_X0p_odd.reset(REG_D1C1);
    EPUM_STORE7_X1p_odd.reset(REG_D1C1);
    EROL_STORE7_X2p_odd.reset(REG_D1C1);
    EHYN_STORE7_X3p_odd.reset(REG_D1C1);
    FAZU_STORE7_X4p_odd.reset(REG_D1C1);
    FAXE_STORE7_X5p_odd.reset(REG_D1C1);
    EXUK_STORE7_X6p_odd.reset(REG_D1C1);
    FEDE_STORE7_X7p_odd.reset(REG_D1C1);



    AXUV_STORE8_I0n_odd.reset(REG_D0C0);
    BADA_STORE8_I1n_odd.reset(REG_D0C0);
    APEV_STORE8_I2n_odd.reset(REG_D0C0);
    BADO_STORE8_I3n_odd.reset(REG_D0C0);
    BEXY_STORE8_I4n_odd.reset(REG_D0C0);
    BYHE_STORE8_I5n_odd.reset(REG_D0C0);

    AZAP_STORE8_L0n_odd.reset(REG_D0C0);
    AFYX_STORE8_L1n_odd.reset(REG_D0C0);
    AFUT_STORE8_L2n_odd.reset(REG_D0C0);
    AFYM_STORE8_L3n_odd.reset(REG_D0C0);

    EZUF_STORE8_X0p_odd.reset(REG_D1C1);
    ENAD_STORE8_X1p_odd.reset(REG_D1C1);
    EBOW_STORE8_X2p_odd.reset(REG_D1C1);
    FYCA_STORE8_X3p_odd.reset(REG_D1C1);
    GAVY_STORE8_X4p_odd.reset(REG_D1C1);
    GYPU_STORE8_X5p_odd.reset(REG_D1C1);
    GADY_STORE8_X6p_odd.reset(REG_D1C1);
    GAZA_STORE8_X7p_odd.reset(REG_D1C1);



    YBER_STORE9_I0n_odd.reset(REG_D0C0);
    YZOR_STORE9_I1n_odd.reset(REG_D0C0);
    XYFE_STORE9_I2n_odd.reset(REG_D0C0);
    XOTU_STORE9_I3n_odd.reset(REG_D0C0);
    XUTE_STORE9_I4n_odd.reset(REG_D0C0);
    XUFO_STORE9_I5n_odd.reset(REG_D0C0);

    CANA_STORE9_L0n_odd.reset(REG_D0C0);
    FOFO_STORE9_L1n_odd.reset(REG_D0C0);
    DYSY_STORE9_L2n_odd.reset(REG_D0C0);
    DEWU_STORE9_L3n_odd.reset(REG_D0C0);

    XUVY_STORE9_X0p_odd.reset(REG_D1C1);
    XERE_STORE9_X1p_odd.reset(REG_D1C1);
    XUZO_STORE9_X2p_odd.reset(REG_D1C1);
    XEXA_STORE9_X3p_odd.reset(REG_D1C1);
    YPOD_STORE9_X4p_odd.reset(REG_D1C1);
    YROP_STORE9_X5p_odd.reset(REG_D1C1);
    YNEP_STORE9_X6p_odd.reset(REG_D1C1);
    YZOF_STORE9_X7p_odd.reset(REG_D1C1);
  }

  //----------------------------------------
  // Active sprite counter, 10 match signals, 10 reset signals

  /*p29.DEZY*/ DFF17 DEZY_COUNT_CLKp_evn;    // AxCxExGx
  /*p29.BESE*/ DFF17 BESE_SPRITE_COUNT0_evn; // AxxxExxx
  /*p29.CUXY*/ DFF17 CUXY_SPRITE_COUNT1_evn; // AxxxExxx
  /*p29.BEGO*/ DFF17 BEGO_SPRITE_COUNT2_evn; // AxxxExxx
  /*p29.DYBE*/ DFF17 DYBE_SPRITE_COUNT3_evn; // AxxxExxx

  /*p29.EBOJ*/ DFF17 EBOJ_STORE0_RSTp_evn;   // AxCxExGx
  /*p29.CEDY*/ DFF17 CEDY_STORE1_RSTp_evn;   // AxCxExGx
  /*p29.EGAV*/ DFF17 EGAV_STORE2_RSTp_evn;   // AxCxExGx
  /*p29.GOTA*/ DFF17 GOTA_STORE3_RSTp_evn;   // AxCxExGx
  /*p29.XUDY*/ DFF17 XUDY_STORE4_RSTp_evn;   // AxCxExGx
  /*p29.WAFY*/ DFF17 WAFY_STORE5_RSTp_evn;   // AxCxExGx
  /*p29.WOMY*/ DFF17 WOMY_STORE6_RSTp_evn;   // AxCxExGx
  /*p29.WAPO*/ DFF17 WAPO_STORE7_RSTp_evn;   // AxCxExGx
  /*p29.EXUQ*/ DFF17 EXUQ_STORE8_RSTp_evn;   // AxCxExGx
  /*p29.FONO*/ DFF17 FONO_STORE9_RSTp_evn;   // AxCxExGx

  //----------------------------------------
  // Sprite index latch selects which store to write

  /*p30.XADU*/ DFF13 XADU_SPRITE_IDX0p_evn; // AxxxExxx
  /*p30.XEDY*/ DFF13 XEDY_SPRITE_IDX1p_evn; // AxxxExxx
  /*p30.ZUZE*/ DFF13 ZUZE_SPRITE_IDX2p_evn; // AxxxExxx
  /*p30.XOBE*/ DFF13 XOBE_SPRITE_IDX3p_evn; // AxxxExxx
  /*p30.YDUF*/ DFF13 YDUF_SPRITE_IDX4p_evn; // AxxxExxx
  /*p30.XECU*/ DFF13 XECU_SPRITE_IDX5p_evn; // AxxxExxx

  //----------------------------------------
  // All 10 of the sprite stores

  // STORE_I - xBxxxFxx
  // STORE_L - xBxxxFxx
  // STORE_X - During scan, xxxDxxxH. During line, AxCxExGx

  /*p30.YGUS*/ DFF8n YGUS_STORE0_I0n_odd;
  /*p30.YSOK*/ DFF8n YSOK_STORE0_I1n_odd;
  /*p30.YZEP*/ DFF8n YZEP_STORE0_I2n_odd;
  /*p30.WYTE*/ DFF8n WYTE_STORE0_I3n_odd;
  /*p30.ZONY*/ DFF8n ZONY_STORE0_I4n_odd;
  /*p30.YWAK*/ DFF8n YWAK_STORE0_I5n_odd;

  /*p30.GYHO*/ DFF8n GYHO_STORE0_L0n_odd;
  /*p30.CUFO*/ DFF8n CUFO_STORE0_L1n_odd;
  /*p30.BOZU*/ DFF8n BOZU_STORE0_L2n_odd;
  /*p30.FYHY*/ DFF8n FYHY_STORE0_L3n_odd;

  /*p31.XEPE*/ DFF9 XEPE_STORE0_X0p_odd;
  /*p31.YLAH*/ DFF9 YLAH_STORE0_X1p_odd;
  /*p31.ZOLA*/ DFF9 ZOLA_STORE0_X2p_odd;
  /*p31.ZULU*/ DFF9 ZULU_STORE0_X3p_odd;
  /*p31.WELO*/ DFF9 WELO_STORE0_X4p_odd;
  /*p31.XUNY*/ DFF9 XUNY_STORE0_X5p_odd;
  /*p31.WOTE*/ DFF9 WOTE_STORE0_X6p_odd;
  /*p31.XAKO*/ DFF9 XAKO_STORE0_X7p_odd;

  //----------

  /*p30.CADU*/ DFF8n CADU_STORE1_I0n_odd;
  /*p30.CEBO*/ DFF8n CEBO_STORE1_I1n_odd;
  /*p30.CUFA*/ DFF8n CUFA_STORE1_I2n_odd;
  /*p30.COMA*/ DFF8n COMA_STORE1_I3n_odd;
  /*p30.CUZA*/ DFF8n CUZA_STORE1_I4n_odd;
  /*p30.CAJY*/ DFF8n CAJY_STORE1_I5n_odd;

  /*p30.AMES*/ DFF8n AMES_STORE1_L0n_odd;
  /*p30.AROF*/ DFF8n AROF_STORE1_L1n_odd;
  /*p30.ABOP*/ DFF8n ABOP_STORE1_L2n_odd;
  /*p30.ABUG*/ DFF8n ABUG_STORE1_L3n_odd;

  /*p31.DANY*/ DFF9 DANY_STORE1_X0p_odd;
  /*p31.DUKO*/ DFF9 DUKO_STORE1_X1p_odd;
  /*p31.DESU*/ DFF9 DESU_STORE1_X2p_odd;
  /*p31.DAZO*/ DFF9 DAZO_STORE1_X3p_odd;
  /*p31.DAKE*/ DFF9 DAKE_STORE1_X4p_odd;
  /*p31.CESO*/ DFF9 CESO_STORE1_X5p_odd;
  /*p31.DYFU*/ DFF9 DYFU_STORE1_X6p_odd;
  /*p31.CUSY*/ DFF9 CUSY_STORE1_X7p_odd;

  //----------

  /*p30.BUHE*/ DFF8n BUHE_STORE2_I0n_odd;
  /*p30.BYHU*/ DFF8n BYHU_STORE2_I1n_odd;
  /*p30.BECA*/ DFF8n BECA_STORE2_I2n_odd;
  /*p30.BULU*/ DFF8n BULU_STORE2_I3n_odd;
  /*p30.BUNA*/ DFF8n BUNA_STORE2_I4n_odd;
  /*p30.BOXA*/ DFF8n BOXA_STORE2_I5n_odd;

  /*p30.YLOV*/ DFF8n YLOV_STORE2_L0n_odd;
  /*p30.XOSY*/ DFF8n XOSY_STORE2_L1n_odd;
  /*p30.XAZY*/ DFF8n XAZY_STORE2_L2n_odd;
  /*p30.YKUK*/ DFF8n YKUK_STORE2_L3n_odd;

  /*p31.FOKA*/ DFF9 FOKA_STORE2_X0p_odd;
  /*p31.FYTY*/ DFF9 FYTY_STORE2_X1p_odd;
  /*p31.FUBY*/ DFF9 FUBY_STORE2_X2p_odd;
  /*p31.GOXU*/ DFF9 GOXU_STORE2_X3p_odd;
  /*p31.DUHY*/ DFF9 DUHY_STORE2_X4p_odd;
  /*p31.EJUF*/ DFF9 EJUF_STORE2_X5p_odd;
  /*p31.ENOR*/ DFF9 ENOR_STORE2_X6p_odd;
  /*p31.DEPY*/ DFF9 DEPY_STORE2_X7p_odd;

  //----------

  /*p30.DEVY*/ DFF8n DEVY_STORE3_I0n_odd;
  /*p30.DESE*/ DFF8n DESE_STORE3_I1n_odd;
  /*p30.DUNY*/ DFF8n DUNY_STORE3_I2n_odd;
  /*p30.DUHA*/ DFF8n DUHA_STORE3_I3n_odd;
  /*p30.DEBA*/ DFF8n DEBA_STORE3_I4n_odd;
  /*p30.DAFU*/ DFF8n DAFU_STORE3_I5n_odd;

  /*p30.ZURO*/ DFF8n ZURO_STORE3_L0n_odd;
  /*p30.ZYLU*/ DFF8n ZYLU_STORE3_L1n_odd;
  /*p30.ZENE*/ DFF8n ZENE_STORE3_L2n_odd;
  /*p30.ZURY*/ DFF8n ZURY_STORE3_L3n_odd;

  /*p31.XOLY*/ DFF9 XOLY_STORE3_X0p_odd;
  /*p31.XYBA*/ DFF9 XYBA_STORE3_X1p_odd;
  /*p31.XABE*/ DFF9 XABE_STORE3_X2p_odd;
  /*p31.XEKA*/ DFF9 XEKA_STORE3_X3p_odd;
  /*p31.XOMY*/ DFF9 XOMY_STORE3_X4p_odd;
  /*p31.WUHA*/ DFF9 WUHA_STORE3_X5p_odd;
  /*p31.WYNA*/ DFF9 WYNA_STORE3_X6p_odd;
  /*p31.WECO*/ DFF9 WECO_STORE3_X7p_odd;

  //----------

  /*p30.XAVE*/ DFF8n XAVE_STORE4_I0n_odd;
  /*p30.XEFE*/ DFF8n XEFE_STORE4_I1n_odd;
  /*p30.WANU*/ DFF8n WANU_STORE4_I2n_odd;
  /*p30.XABO*/ DFF8n XABO_STORE4_I3n_odd;
  /*p30.XEGE*/ DFF8n XEGE_STORE4_I4n_odd;
  /*p30.XYNU*/ DFF8n XYNU_STORE4_I5n_odd;

  /*p30.CAPO*/ DFF8n CAPO_STORE4_L0n_odd;
  /*p30.CAJU*/ DFF8n CAJU_STORE4_L1n_odd;
  /*p30.CONO*/ DFF8n CONO_STORE4_L2n_odd;
  /*p30.CUMU*/ DFF8n CUMU_STORE4_L3n_odd;

  /*p31.WEDU*/ DFF9 WEDU_STORE4_X0p_odd;
  /*p31.YGAJ*/ DFF9 YGAJ_STORE4_X1p_odd;
  /*p31.ZYJO*/ DFF9 ZYJO_STORE4_X2p_odd;
  /*p31.XURY*/ DFF9 XURY_STORE4_X3p_odd;
  /*p31.YBED*/ DFF9 YBED_STORE4_X4p_odd;
  /*p31.ZALA*/ DFF9 ZALA_STORE4_X5p_odd;
  /*p31.WYDE*/ DFF9 WYDE_STORE4_X6p_odd;
  /*p31.XEPA*/ DFF9 XEPA_STORE4_X7p_odd;

  //----------

  /*p30.EKOP*/ DFF8n EKOP_STORE5_I0n_odd;
  /*p30.ETYM*/ DFF8n ETYM_STORE5_I1n_odd;
  /*p30.GORU*/ DFF8n GORU_STORE5_I2n_odd;
  /*p30.EBEX*/ DFF8n EBEX_STORE5_I3n_odd;
  /*p30.ETAV*/ DFF8n ETAV_STORE5_I4n_odd;
  /*p30.EKAP*/ DFF8n EKAP_STORE5_I5n_odd;

  /*p30.ACEP*/ DFF8n ACEP_STORE5_L0n_odd;
  /*p30.ABEG*/ DFF8n ABEG_STORE5_L1n_odd;
  /*p30.ABUX*/ DFF8n ABUX_STORE5_L2n_odd;
  /*p30.ANED*/ DFF8n ANED_STORE5_L3n_odd;

  /*p31.FUSA*/ DFF9 FUSA_STORE5_X0p_odd;
  /*p31.FAXA*/ DFF9 FAXA_STORE5_X1p_odd;
  /*p31.FOZY*/ DFF9 FOZY_STORE5_X2p_odd;
  /*p31.FESY*/ DFF9 FESY_STORE5_X3p_odd;
  /*p31.CYWE*/ DFF9 CYWE_STORE5_X4p_odd;
  /*p31.DYBY*/ DFF9 DYBY_STORE5_X5p_odd;
  /*p31.DURY*/ DFF9 DURY_STORE5_X6p_odd;
  /*p31.CUVY*/ DFF9 CUVY_STORE5_X7p_odd;

  //----------

  /*p30.GABO*/ DFF8n GABO_STORE6_I0n_odd;
  /*p30.GACY*/ DFF8n GACY_STORE6_I1n_odd;
  /*p30.FOGO*/ DFF8n FOGO_STORE6_I2n_odd;
  /*p30.GOHU*/ DFF8n GOHU_STORE6_I3n_odd;
  /*p30.FOXY*/ DFF8n FOXY_STORE6_I4n_odd;
  /*p30.GECU*/ DFF8n GECU_STORE6_I5n_odd;

  /*p30.ZUMY*/ DFF8n ZUMY_STORE6_L0n_odd;
  /*p30.ZAFU*/ DFF8n ZAFU_STORE6_L1n_odd;
  /*p30.ZEXO*/ DFF8n ZEXO_STORE6_L2n_odd;
  /*p30.ZUBE*/ DFF8n ZUBE_STORE6_L3n_odd;

  /*p31.YCOL*/ DFF9 YCOL_STORE6_X0p_odd;
  /*p31.YRAC*/ DFF9 YRAC_STORE6_X1p_odd;
  /*p31.YMEM*/ DFF9 YMEM_STORE6_X2p_odd;
  /*p31.YVAG*/ DFF9 YVAG_STORE6_X3p_odd;
  /*p31.ZOLY*/ DFF9 ZOLY_STORE6_X4p_odd;
  /*p31.ZOGO*/ DFF9 ZOGO_STORE6_X5p_odd;
  /*p31.ZECU*/ DFF9 ZECU_STORE6_X6p_odd;
  /*p31.ZESA*/ DFF9 ZESA_STORE6_X7p_odd;

  //----------

  /*p30.GULE*/ DFF8n GULE_STORE7_I0n_odd;
  /*p30.GYNO*/ DFF8n GYNO_STORE7_I1n_odd;
  /*p30.FEFA*/ DFF8n FEFA_STORE7_I2n_odd;
  /*p30.FYSU*/ DFF8n FYSU_STORE7_I3n_odd;
  /*p30.GESY*/ DFF8n GESY_STORE7_I4n_odd;
  /*p30.FUZO*/ DFF8n FUZO_STORE7_I5n_odd;

  /*p30.XYNA*/ DFF8n XYNA_STORE7_L0n_odd;
  /*p30.YGUM*/ DFF8n YGUM_STORE7_L1n_odd;
  /*p30.XAKU*/ DFF8n XAKU_STORE7_L2n_odd;
  /*p30.XYGO*/ DFF8n XYGO_STORE7_L3n_odd;

  /*p31.ERAZ*/ DFF9 ERAZ_STORE7_X0p_odd;
  /*p31.EPUM*/ DFF9 EPUM_STORE7_X1p_odd;
  /*p31.EROL*/ DFF9 EROL_STORE7_X2p_odd;
  /*p31.EHYN*/ DFF9 EHYN_STORE7_X3p_odd;
  /*p31.FAZU*/ DFF9 FAZU_STORE7_X4p_odd;
  /*p31.FAXE*/ DFF9 FAXE_STORE7_X5p_odd;
  /*p31.EXUK*/ DFF9 EXUK_STORE7_X6p_odd;
  /*p31.FEDE*/ DFF9 FEDE_STORE7_X7p_odd;

  //----------

  /*p30.AXUV*/ DFF8n AXUV_STORE8_I0n_odd;
  /*p30.BADA*/ DFF8n BADA_STORE8_I1n_odd;
  /*p30.APEV*/ DFF8n APEV_STORE8_I2n_odd;
  /*p30.BADO*/ DFF8n BADO_STORE8_I3n_odd;
  /*p30.BEXY*/ DFF8n BEXY_STORE8_I4n_odd;
  /*p30.BYHE*/ DFF8n BYHE_STORE8_I5n_odd;

  /*p30.AZAP*/ DFF8n AZAP_STORE8_L0n_odd;
  /*p30.AFYX*/ DFF8n AFYX_STORE8_L1n_odd;
  /*p30.AFUT*/ DFF8n AFUT_STORE8_L2n_odd;
  /*p30.AFYM*/ DFF8n AFYM_STORE8_L3n_odd;

  /*p31.EZUF*/ DFF9 EZUF_STORE8_X0p_odd;
  /*p31.ENAD*/ DFF9 ENAD_STORE8_X1p_odd;
  /*p31.EBOW*/ DFF9 EBOW_STORE8_X2p_odd;
  /*p31.FYCA*/ DFF9 FYCA_STORE8_X3p_odd;
  /*p31.GAVY*/ DFF9 GAVY_STORE8_X4p_odd;
  /*p31.GYPU*/ DFF9 GYPU_STORE8_X5p_odd;
  /*p31.GADY*/ DFF9 GADY_STORE8_X6p_odd;
  /*p31.GAZA*/ DFF9 GAZA_STORE8_X7p_odd;

  //----------

  /*p30.YBER*/ DFF8n YBER_STORE9_I0n_odd;
  /*p30.YZOR*/ DFF8n YZOR_STORE9_I1n_odd;
  /*p30.XYFE*/ DFF8n XYFE_STORE9_I2n_odd;
  /*p30.XOTU*/ DFF8n XOTU_STORE9_I3n_odd;
  /*p30.XUTE*/ DFF8n XUTE_STORE9_I4n_odd;
  /*p30.XUFO*/ DFF8n XUFO_STORE9_I5n_odd;

  /*p30.CANA*/ DFF8n CANA_STORE9_L0n_odd;
  /*p30.FOFO*/ DFF8n FOFO_STORE9_L1n_odd;
  /*p30.DYSY*/ DFF8n DYSY_STORE9_L2n_odd;
  /*p30.DEWU*/ DFF8n DEWU_STORE9_L3n_odd;

  /*p31.XUVY*/ DFF9 XUVY_STORE9_X0p_odd;
  /*p31.XERE*/ DFF9 XERE_STORE9_X1p_odd;
  /*p31.XUZO*/ DFF9 XUZO_STORE9_X2p_odd;
  /*p31.XEXA*/ DFF9 XEXA_STORE9_X3p_odd;
  /*p31.YPOD*/ DFF9 YPOD_STORE9_X4p_odd;
  /*p31.YROP*/ DFF9 YROP_STORE9_X5p_odd;
  /*p31.YNEP*/ DFF9 YNEP_STORE9_X6p_odd;
  /*p31.YZOF*/ DFF9 YZOF_STORE9_X7p_odd;
};

//-----------------------------------------------------------------------------
