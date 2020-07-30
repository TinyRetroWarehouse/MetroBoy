#pragma once
#include "Cells.h"

namespace Schematics {

struct SchematicTop;

//-----------------------------------------------------------------------------

struct SpriteStore {
  void tick(const SchematicTop& top);
  void tock(const SchematicTop& top);

  /*p29.FEPO*/ Sig FEPO_STORE_MATCHp;
  /*p21.XENA*/ wire XENA_STORE_MATCHn() const { return not(FEPO_STORE_MATCHp); }

  Tri SPR_TRI_INDX_0 = TRI_HZPU;
  Tri SPR_TRI_INDX_1 = TRI_HZPU;
  Tri SPR_TRI_INDX_2 = TRI_HZPU;
  Tri SPR_TRI_INDX_3 = TRI_HZPU;
  Tri SPR_TRI_INDX_4 = TRI_HZPU;
  Tri SPR_TRI_INDX_5 = TRI_HZPU;

  Tri SPR_TRI_LINE_0 = TRI_HZPU;
  Tri SPR_TRI_LINE_1 = TRI_HZPU;
  Tri SPR_TRI_LINE_2 = TRI_HZPU;
  Tri SPR_TRI_LINE_3 = TRI_HZPU;

  void dump(Dumper& d) {
    d("----------SpriteStore---------\n");
    d("DEZY_STORE_ENn %c\n", DEZY_STORE_ENn.c());

    d("SPRITE INDEX %02d\n", 
      pack(0, 0, XECU_SPRITE_INDX5.q(), YDUF_SPRITE_INDX4.q(),
           XOBE_SPRITE_INDX3.q(), ZUZE_SPRITE_INDX2.q(), XEDY_SPRITE_INDX1.q(), XADU_SPRITE_INDX0.q()));

    d("STORE MATCH %c%c%c%c%c%c%c%c%c%c\n",
      STORE0_MATCHn.c(), STORE1_MATCHn.c(), STORE2_MATCHn.c(), STORE3_MATCHn.c(), STORE4_MATCHn.c(),
      STORE5_MATCHn.c(), STORE6_MATCHn.c(), STORE7_MATCHn.c(), STORE8_MATCHn.c(), STORE9_MATCHn.c());

    d("SPRITE COUNT %02d\n", pack(SPRITE_COUNT3.q(), SPRITE_COUNT2.q(), SPRITE_COUNT1.q(), SPRITE_COUNT0.q()));
    d("\n");
  }

private:
  friend struct SchematicTop;

  /*p30.XADU*/ Reg XADU_SPRITE_INDX0 = REG_D0C0;
  /*p30.XEDY*/ Reg XEDY_SPRITE_INDX1 = REG_D0C0;
  /*p30.ZUZE*/ Reg ZUZE_SPRITE_INDX2 = REG_D0C0;
  /*p30.XOBE*/ Reg XOBE_SPRITE_INDX3 = REG_D0C0;
  /*p30.YDUF*/ Reg YDUF_SPRITE_INDX4 = REG_D0C0;
  /*p30.XECU*/ Reg XECU_SPRITE_INDX5 = REG_D0C0;

  /*p29.YDUG*/ Sig STORE0_MATCHn;
  /*p29.DYDU*/ Sig STORE1_MATCHn;
  /*p29.DEGO*/ Sig STORE2_MATCHn;
  /*p29.YLOZ*/ Sig STORE3_MATCHn;
  /*p29.XAGE*/ Sig STORE4_MATCHn;
  /*p29.EGOM*/ Sig STORE5_MATCHn;
  /*p29.YBEZ*/ Sig STORE6_MATCHn;
  /*p29.DYKA*/ Sig STORE7_MATCHn;
  /*p29.EFYL*/ Sig STORE8_MATCHn;
  /*p29.YGEM*/ Sig STORE9_MATCHn;

  /*p29.DEZY*/ Reg DEZY_STORE_ENn = REG_D0C0;
  /*p29.BESE*/ Reg SPRITE_COUNT0 = REG_D0C0;
  /*p29.CUXY*/ Reg SPRITE_COUNT1 = REG_D0C0;
  /*p29.BEGO*/ Reg SPRITE_COUNT2 = REG_D0C0;
  /*p29.DYBE*/ Reg SPRITE_COUNT3 = REG_D0C0;

  //----------

  /*p29.EBOJ*/ Reg EBOJ_STORE0_RSTp = REG_D0C0;
  /*p30.YGUS*/ Reg YGUS_STORE0_INDX5 = REG_D0C0;
  /*p30.YSOK*/ Reg YSOK_STORE0_INDX4 = REG_D0C0;
  /*p30.YZEP*/ Reg YZEP_STORE0_INDX3 = REG_D0C0;
  /*p30.WYTE*/ Reg WYTE_STORE0_INDX2 = REG_D0C0;
  /*p30.ZONY*/ Reg ZONY_STORE0_INDX1 = REG_D0C0;
  /*p30.YWAK*/ Reg YWAK_STORE0_INDX0 = REG_D0C0;
  /*p30.FYHY*/ Reg FYHY_STORE0_LINE0 = REG_D0C0;
  /*p30.GYHO*/ Reg GYHO_STORE0_LINE1 = REG_D0C0;
  /*p30.BOZU*/ Reg BOZU_STORE0_LINE2 = REG_D0C0;
  /*p30.CUFO*/ Reg CUFO_STORE0_LINE3 = REG_D0C0;
  /*p31.XEPE*/ Reg XEPE_STORE0_X0 = REG_D0C0;
  /*p31.YLAH*/ Reg YLAH_STORE0_X1 = REG_D0C0;
  /*p31.ZOLA*/ Reg ZOLA_STORE0_X2 = REG_D0C0;
  /*p31.ZULU*/ Reg ZULU_STORE0_X3 = REG_D0C0;
  /*p31.WELO*/ Reg WELO_STORE0_X4 = REG_D0C0;
  /*p31.XUNY*/ Reg XUNY_STORE0_X5 = REG_D0C0;
  /*p31.WOTE*/ Reg WOTE_STORE0_X6 = REG_D0C0;
  /*p31.XAKO*/ Reg XAKO_STORE0_X7 = REG_D0C0;

  //----------

  /*p29.CEDY*/ Reg CEDY_STORE1_RSTp = REG_D0C0;
  /*p30.CAJY*/ Reg STORE1_INDX0 = REG_D0C0;
  /*p30.CUZA*/ Reg STORE1_INDX1 = REG_D0C0;
  /*p30.COMA*/ Reg STORE1_INDX2 = REG_D0C0;
  /*p30.CUFA*/ Reg STORE1_INDX3 = REG_D0C0;
  /*p30.CEBO*/ Reg STORE1_INDX4 = REG_D0C0;
  /*p30.CADU*/ Reg STORE1_INDX5 = REG_D0C0;
  /*p30.ABUG*/ Reg STORE1_LINE0 = REG_D0C0;
  /*p30.AMES*/ Reg STORE1_LINE1 = REG_D0C0;
  /*p30.ABOP*/ Reg STORE1_LINE2 = REG_D0C0;
  /*p30.AROF*/ Reg STORE1_LINE3 = REG_D0C0;
  /*p31.DANY*/ Reg STORE1_X0 = REG_D0C0;
  /*p31.DUKO*/ Reg STORE1_X1 = REG_D0C0;
  /*p31.DESU*/ Reg STORE1_X2 = REG_D0C0;
  /*p31.DAZO*/ Reg STORE1_X3 = REG_D0C0;
  /*p31.DAKE*/ Reg STORE1_X4 = REG_D0C0;
  /*p31.CESO*/ Reg STORE1_X5 = REG_D0C0;
  /*p31.DYFU*/ Reg STORE1_X6 = REG_D0C0;
  /*p31.CUSY*/ Reg STORE1_X7 = REG_D0C0;

  /*p29.EGAV*/ Reg EGAV_STORE2_RSTp = REG_D0C0;
  /*p30.BOXA*/ Reg STORE2_INDX0 = REG_D0C0;
  /*p30.BUNA*/ Reg STORE2_INDX1 = REG_D0C0;
  /*p30.BULU*/ Reg STORE2_INDX2 = REG_D0C0;
  /*p30.BECA*/ Reg STORE2_INDX3 = REG_D0C0;
  /*p30.BYHU*/ Reg STORE2_INDX4 = REG_D0C0;
  /*p30.BUHE*/ Reg STORE2_INDX5 = REG_D0C0;
  /*p30.YKUK*/ Reg STORE2_LINE0 = REG_D0C0;
  /*p30.YLOV*/ Reg STORE2_LINE1 = REG_D0C0;
  /*p30.XAZY*/ Reg STORE2_LINE2 = REG_D0C0;
  /*p30.XOSY*/ Reg STORE2_LINE3 = REG_D0C0;
  /*p31.FOKA*/ Reg STORE2_X0 = REG_D0C0;
  /*p31.FYTY*/ Reg STORE2_X1 = REG_D0C0;
  /*p31.FUBY*/ Reg STORE2_X2 = REG_D0C0;
  /*p31.GOXU*/ Reg STORE2_X3 = REG_D0C0;
  /*p31.DUHY*/ Reg STORE2_X4 = REG_D0C0;
  /*p31.EJUF*/ Reg STORE2_X5 = REG_D0C0;
  /*p31.ENOR*/ Reg STORE2_X6 = REG_D0C0;
  /*p31.DEPY*/ Reg STORE2_X7 = REG_D0C0;

  /*p29.GOTA*/ Reg GOTA_STORE3_RSTp = REG_D0C0;
  /*p30.DAFU*/ Reg STORE3_INDX0 = REG_D0C0;
  /*p30.DEBA*/ Reg STORE3_INDX1 = REG_D0C0;
  /*p30.DUHA*/ Reg STORE3_INDX2 = REG_D0C0;
  /*p30.DUNY*/ Reg STORE3_INDX3 = REG_D0C0;
  /*p30.DESE*/ Reg STORE3_INDX4 = REG_D0C0;
  /*p30.DEVY*/ Reg STORE3_INDX5 = REG_D0C0;
  /*p30.ZURY*/ Reg STORE3_LINE0 = REG_D0C0;
  /*p30.ZURO*/ Reg STORE3_LINE1 = REG_D0C0;
  /*p30.ZENE*/ Reg STORE3_LINE2 = REG_D0C0;
  /*p30.ZYLU*/ Reg STORE3_LINE4 = REG_D0C0;
  /*p31.XOLY*/ Reg STORE3_X0 = REG_D0C0;
  /*p31.XYBA*/ Reg STORE3_X1 = REG_D0C0;
  /*p31.XABE*/ Reg STORE3_X2 = REG_D0C0;
  /*p31.XEKA*/ Reg STORE3_X3 = REG_D0C0;
  /*p31.XOMY*/ Reg STORE3_X4 = REG_D0C0;
  /*p31.WUHA*/ Reg STORE3_X5 = REG_D0C0;
  /*p31.WYNA*/ Reg STORE3_X6 = REG_D0C0;
  /*p31.WECO*/ Reg STORE3_X7 = REG_D0C0;

  /*p29.XUDY*/ Reg XUDY_STORE4_RSTp = REG_D0C0;
  /*p30.XYNU*/ Reg XYNU = REG_D0C0;
  /*p30.XEGE*/ Reg XEGE = REG_D0C0;
  /*p30.XABO*/ Reg XABO = REG_D0C0;
  /*p30.WANU*/ Reg WANU = REG_D0C0;
  /*p30.XEFE*/ Reg XEFE = REG_D0C0;
  /*p30.XAVE*/ Reg XAVE = REG_D0C0;
  /*p30.CUMU*/ Reg CUMU = REG_D0C0;
  /*p30.CAPO*/ Reg CAPO = REG_D0C0;
  /*p30.CONO*/ Reg CONO = REG_D0C0;
  /*p30.CAJU*/ Reg CAJU = REG_D0C0;
  /*p31.WEDU*/ Reg STORE4_X0 = REG_D0C0;
  /*p31.YGAJ*/ Reg STORE4_X1 = REG_D0C0;
  /*p31.ZYJO*/ Reg STORE4_X2 = REG_D0C0;
  /*p31.XURY*/ Reg STORE4_X3 = REG_D0C0;
  /*p31.YBED*/ Reg STORE4_X4 = REG_D0C0;
  /*p31.ZALA*/ Reg STORE4_X5 = REG_D0C0;
  /*p31.WYDE*/ Reg STORE4_X6 = REG_D0C0;
  /*p31.XEPA*/ Reg STORE4_X7 = REG_D0C0;

  // store 5
  /*p29.WAFY*/ Reg WAFY_STORE5_RSTp = REG_D0C0;
  /*p30.EKAP*/ Reg EKAP = REG_D0C0;
  /*p30.ETAV*/ Reg ETAV = REG_D0C0;
  /*p30.EBEX*/ Reg EBEX = REG_D0C0;
  /*p30.GORU*/ Reg GORU = REG_D0C0;
  /*p30.ETYM*/ Reg ETYM = REG_D0C0;
  /*p30.EKOP*/ Reg EKOP = REG_D0C0;
  /*p30.ANED*/ Reg ANED = REG_D0C0;
  /*p30.ACEP*/ Reg ACEP = REG_D0C0;
  /*p30.ABUX*/ Reg ABUX = REG_D0C0;
  /*p30.ABEG*/ Reg ABEG = REG_D0C0;
  /*p31.FUSA*/ Reg STORE5_X0 = REG_D0C0;
  /*p31.FAXA*/ Reg STORE5_X1 = REG_D0C0;
  /*p31.FOZY*/ Reg STORE5_X2 = REG_D0C0;
  /*p31.FESY*/ Reg STORE5_X3 = REG_D0C0;
  /*p31.CYWE*/ Reg STORE5_X4 = REG_D0C0;
  /*p31.DYBY*/ Reg STORE5_X5 = REG_D0C0;
  /*p31.DURY*/ Reg STORE5_X6 = REG_D0C0;
  /*p31.CUVY*/ Reg STORE5_X7 = REG_D0C0;

  // sprite store 6
  /*p29.WOMY*/ Reg WOMY_STORE6_RSTp = REG_D0C0;
  /*p30.GECU*/ Reg GECU = REG_D0C0;
  /*p30.FOXY*/ Reg FOXY = REG_D0C0;
  /*p30.GOHU*/ Reg GOHU = REG_D0C0;
  /*p30.FOGO*/ Reg FOGO = REG_D0C0;
  /*p30.GACY*/ Reg GACY = REG_D0C0;
  /*p30.GABO*/ Reg GABO = REG_D0C0;
  /*p30.ZUBE*/ Reg ZUBE = REG_D0C0;
  /*p30.ZUMY*/ Reg ZUMY = REG_D0C0;
  /*p30.ZEXO*/ Reg ZEXO = REG_D0C0;
  /*p30.ZAFU*/ Reg ZAFU = REG_D0C0;
  /*p31.YCOL*/ Reg STORE6_X0 = REG_D0C0;
  /*p31.YRAC*/ Reg STORE6_X1 = REG_D0C0;
  /*p31.YMEM*/ Reg STORE6_X2 = REG_D0C0;
  /*p31.YVAG*/ Reg STORE6_X3 = REG_D0C0;
  /*p31.ZOLY*/ Reg STORE6_X4 = REG_D0C0;
  /*p31.ZOGO*/ Reg STORE6_X5 = REG_D0C0;
  /*p31.ZECU*/ Reg STORE6_X6 = REG_D0C0;
  /*p31.ZESA*/ Reg STORE6_X7 = REG_D0C0;

  // sprite store 7
  /*p29.WAPO*/ Reg WAPO_STORE7_RSTp = REG_D0C0;
  /*p30.FUZO*/ Reg FUZO = REG_D0C0;
  /*p30.GESY*/ Reg GESY = REG_D0C0;
  /*p30.FYSU*/ Reg FYSU = REG_D0C0;
  /*p30.FEFA*/ Reg FEFA = REG_D0C0;
  /*p30.GYNO*/ Reg GYNO = REG_D0C0;
  /*p30.GULE*/ Reg GULE = REG_D0C0;
  /*p30.XYGO*/ Reg XYGO = REG_D0C0;
  /*p30.XYNA*/ Reg XYNA = REG_D0C0;
  /*p30.XAKU*/ Reg XAKU = REG_D0C0;
  /*p30.YGUM*/ Reg YGUM = REG_D0C0;
  /*p31.ERAZ*/ Reg STORE7_X0 = REG_D0C0;
  /*p31.EPUM*/ Reg STORE7_X1 = REG_D0C0;
  /*p31.EROL*/ Reg STORE7_X2 = REG_D0C0;
  /*p31.EHYN*/ Reg STORE7_X3 = REG_D0C0;
  /*p31.FAZU*/ Reg STORE7_X4 = REG_D0C0;
  /*p31.FAXE*/ Reg STORE7_X5 = REG_D0C0;
  /*p31.EXUK*/ Reg STORE7_X6 = REG_D0C0;
  /*p31.FEDE*/ Reg STORE7_X7 = REG_D0C0;

  // sprite store 8
  /*p29.EXUQ*/ Reg EXUQ_STORE8_RSTp = REG_D0C0;
  /*p30.AXUV*/ Reg STORE8_INDX5 = REG_D0C0;
  /*p30.BADA*/ Reg STORE8_INDX4 = REG_D0C0;
  /*p30.APEV*/ Reg STORE8_INDX3 = REG_D0C0;
  /*p30.BADO*/ Reg STORE8_INDX2 = REG_D0C0;
  /*p30.BEXY*/ Reg STORE8_INDX1 = REG_D0C0;
  /*p30.BYHE*/ Reg STORE8_INDX0 = REG_D0C0;
  /*p30.AFYM*/ Reg STORE8_LINE0 = REG_D0C0;
  /*p30.AZAP*/ Reg STORE8_LINE1 = REG_D0C0;
  /*p30.AFUT*/ Reg STORE8_LINE2 = REG_D0C0;
  /*p30.AFYX*/ Reg STORE8_LINE3 = REG_D0C0;
  /*p31.GAVY*/ Reg STORE8_X4 = REG_D0C0;
  /*p31.GYPU*/ Reg STORE8_X5 = REG_D0C0;
  /*p31.GADY*/ Reg STORE8_X6 = REG_D0C0;
  /*p31.GAZA*/ Reg STORE8_X7 = REG_D0C0;
  /*p31.EZUF*/ Reg STORE8_X0 = REG_D0C0;
  /*p31.ENAD*/ Reg STORE8_X1 = REG_D0C0;
  /*p31.EBOW*/ Reg STORE8_X2 = REG_D0C0;
  /*p31.FYCA*/ Reg STORE8_X3 = REG_D0C0;

  // sprite store 9
  /*p29.FONO*/ Reg FONO_STORE9_RSTp = REG_D0C0;
  /*p30.XUFO*/ Reg STORE9_INDX0 = REG_D0C0;
  /*p30.XUTE*/ Reg STORE9_INDX1 = REG_D0C0;
  /*p30.XOTU*/ Reg STORE9_INDX2 = REG_D0C0;
  /*p30.XYFE*/ Reg STORE9_INDX3 = REG_D0C0;
  /*p30.YZOR*/ Reg STORE9_INDX4 = REG_D0C0;
  /*p30.YBER*/ Reg STORE9_INDX5 = REG_D0C0;
  /*p30.DEWU*/ Reg STORE9_LINE0 = REG_D0C0;
  /*p30.CANA*/ Reg STORE9_LINE1 = REG_D0C0;
  /*p30.DYSY*/ Reg STORE9_LINE2 = REG_D0C0;
  /*p30.FOFO*/ Reg STORE9_LINE3 = REG_D0C0;
  /*p31.XUVY*/ Reg STORE9_X0 = REG_D0C0;
  /*p31.XERE*/ Reg STORE9_X1 = REG_D0C0;
  /*p31.XUZO*/ Reg STORE9_X2 = REG_D0C0;
  /*p31.XEXA*/ Reg STORE9_X3 = REG_D0C0;
  /*p31.YPOD*/ Reg STORE9_X4 = REG_D0C0;
  /*p31.YROP*/ Reg STORE9_X5 = REG_D0C0;
  /*p31.YNEP*/ Reg STORE9_X6 = REG_D0C0;
  /*p31.YZOF*/ Reg STORE9_X7 = REG_D0C0;
};

//-----------------------------------------------------------------------------

}; // namespace Schematics