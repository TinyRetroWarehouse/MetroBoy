#include "Sch_Window.h"
#include "Sch_Top.h"

using namespace Schematics;

//------------------------------------------------------------------------------

void WindowRegisters::tick(SchematicTop& top) {

  /*p01.ANOS*/ wire ANOS_AxCxExGx = not(top.SYS_PIN_CLK_B);
  /*p01.ATAL*/ wire ATAL_xBxDxFxH = not(ANOS_AxCxExGx);
  /*p01.AZOF*/ wire AZOF_AxCxExGx = not(ATAL_xBxDxFxH);
  /*p01.ZAXY*/ wire ZAXY_xBxDxFxH = not(AZOF_AxCxExGx);
  /*p01.ZEME*/ wire ZEME_AxCxExGx = not(ZAXY_xBxDxFxH);
  /*p01.ALET*/ wire ALET_xBxDxFxH = not(ZEME_AxCxExGx);
  /*p27.MEHE*/ wire MEHE_AxCxExGx = not(ALET_xBxDxFxH);

  /*p21.TALU*/ wire TALU_xBCDExxx = not(top.VENA_xBCDExxx());

  /*p28.ATEJ*/ wire ATEJ_VID_LINE_TRIG_d4p = not(top.BYHA_VID_LINE_TRIG_d4());
  /*p27.XAHY*/ wire _XAHY_VID_LINE_TRIG_d4n = not(ATEJ_VID_LINE_TRIG_d4p);
  /*p27.XOFO*/ pwire _XOFO_WIN_RSTp = nand(top.WYMO_LCDC_WINEN, _XAHY_VID_LINE_TRIG_d4n, top.XAPO_VID_RSTn());

  /*p21.PARU*/ pwire PARU_VBLANKp = not(!top.POPU_VBLANK_d4());
  /*p01.PYRY*/ pwire _PYRY_VID_RSTp = not(top.XAPO_VID_RSTn());
  /*p27.REPU*/ pwire _REPU_VBLANK_RSTp = or(PARU_VBLANKp, _PYRY_VID_RSTp);
  /*p27.SYNY*/ nwire _SYNY_VBLANK_RSTn = not(_REPU_VBLANK_RSTp);

  {
    /*p27.SARY*/ _SARY_WY_MATCH.set(TALU_xBCDExxx, top.XAPO_VID_RSTn(), ROGE_WY_MATCHp(top));
    /*p27.REJO*/ _REJO_WY_MATCH_LATCH.nor_latch((pwire)_SARY_WY_MATCH, _REPU_VBLANK_RSTp);
  }

  {
    // This trigger fires on the pixel _at_ WX
    /*p27.ROCO*/ pwire _ROCO_CLKPIPEp = not(top.SEGU_CLKPIPEn());
    /*p27.PYCO*/ _PYCO_WX_MATCH_A.set(_ROCO_CLKPIPEp, top.XAPO_VID_RSTn(), NUKO_WX_MATCHp(top));
    /*p27.NUNU*/ _NUNU_WX_MATCH_B.set(MEHE_AxCxExGx, top.XAPO_VID_RSTn(), _PYCO_WX_MATCH_A);

    /*p27.PYNU*/ PYNU_WIN_MODE_A.nor_latch((pwire)_NUNU_WX_MATCH_B, _XOFO_WIN_RSTp);
    /*p27.NOPA*/ _NOPA_WIN_MODE_B.set(ALET_xBxDxFxH, top.XAPO_VID_RSTn(), PYNU_WIN_MODE_A);
    /*p27.NUNY*/ NUNY_WX_MATCHpe = and (PYNU_WIN_MODE_A, !_NOPA_WIN_MODE_B);
  }

  {
    // This trigger fires on the pixel _after_ WX. Not sure what the fine count is about.
    /*p27.PANY*/ wire _PANY_WX_MATCHn = nor(NUKO_WX_MATCHp(top), top.ROZE_FINE_COUNT_7n());
    /*p27.RYFA*/ _RYFA_WX_MATCHn_A.set(top.SEGU_CLKPIPEn().as_pwire(), top.XYMU_RENDERINGp().as_nwire(), _PANY_WX_MATCHn);
    /*p27.RENE*/ _RENE_WX_MATCHn_B.set(ALET_xBxDxFxH,                  top.XYMU_RENDERINGp().as_nwire(), _RYFA_WX_MATCHn_A);
    /*p27.SEKO*/ SEKO_WX_MATCHne = nor(!_RYFA_WX_MATCHn_A, _RENE_WX_MATCHn_B);
  }

  {
    // PUKU/RYDY form a NOR latch. WIN_MODE_TRIG is SET, (VID_RESET | BFETCH_DONE_SYNC_DELAY) is RESET.
    ///*p27.PUKU*/ PUKU = nor(RYDY, WIN_MODE_TRIG);
    ///*p27.RYDY*/ RYDY = nor(PUKU, rst_reg.VID_RESET4, BFETCH_DONE_SYNC_DELAY);

    /*p27.RYDY*/ RYDY_WIN_FIRST_TILE_A.nor_latch((pwire)NUNY_WX_MATCHpe, _PYRY_VID_RSTp || top.PORY_TILE_FETCH_DONE_Bp());
    /*p27.SOVY*/ _SOVY_WIN_FIRST_TILE_B.set(ALET_xBxDxFxH, top.XAPO_VID_RSTn(), RYDY_WIN_FIRST_TILE_A);
    /*p27.SYLO*/ wire SYLO_WIN_FIRST_TILE_An = not(RYDY_WIN_FIRST_TILE_A);
    /*p27.TUXY*/ wire TUXY_WIN_FIRST_TILE_NE = nand(SYLO_WIN_FIRST_TILE_An, _SOVY_WIN_FIRST_TILE_B);
    /*p27.SUZU*/ SUZU_WIN_FIRST_TILEne = not(TUXY_WIN_FIRST_TILE_NE);
  }

  // window x coordinate
  {
    // something weird here, PORE doesn't look like a clock

    /*p27.NOCU*/ wire NOCU_WIN_MODEn = not(PYNU_WIN_MODE_A.q());
    /*p27.PORE*/ wire PORE_WIN_MODEp = not(NOCU_WIN_MODEn);
    /*p27.VETU*/ wire VETU_WIN_MAP_CLK = and (top.TEVO_FINE_RSTp(), PORE_WIN_MODEp);
    /*p27.XACO*/ wire XACO_WIN_RSTn = not(_XOFO_WIN_RSTp);

    /*p27.WYKA*/ WYKA_WIN_X3.set(VETU_WIN_MAP_CLK, XACO_WIN_RSTn, !WYKA_WIN_X3);
    /*p27.WODY*/ WODY_WIN_X4.set(!WYKA_WIN_X3,     XACO_WIN_RSTn, !WODY_WIN_X4);
    /*p27.WOBO*/ WOBO_WIN_X5.set(!WODY_WIN_X4,     XACO_WIN_RSTn, !WOBO_WIN_X5);
    /*p27.WYKO*/ WYKO_WIN_X6.set(!WOBO_WIN_X5,     XACO_WIN_RSTn, !WYKO_WIN_X6);
    /*p27.XOLO*/ XOLO_WIN_X7.set(!WYKO_WIN_X6,     XACO_WIN_RSTn, !XOLO_WIN_X7);
  }

  // window y coordinate
  // every time we leave win mode we increment win_y
  {
    /*p27.NOCU*/ wire NOCU_WIN_MODEn = not(PYNU_WIN_MODE_A.q());
    /*p27.PORE*/ wire PORE_WIN_MODEp = not(NOCU_WIN_MODEn);
    /*p27.WAZY*/ wire WAZY_WIN_Y_CLK = not(PORE_WIN_MODEp);

    /*p27.VYNO*/ VYNO_WIN_Y0.set(WAZY_WIN_Y_CLK, _SYNY_VBLANK_RSTn, !VYNO_WIN_Y0);
    /*p27.VUJO*/ VUJO_WIN_Y1.set(!VYNO_WIN_Y0,   _SYNY_VBLANK_RSTn, !VUJO_WIN_Y1);
    /*p27.VYMU*/ VYMU_WIN_Y2.set(!VUJO_WIN_Y1,   _SYNY_VBLANK_RSTn, !VYMU_WIN_Y2);
    /*p27.TUFU*/ TUFU_WIN_Y3.set(!VYMU_WIN_Y2,   _SYNY_VBLANK_RSTn, !TUFU_WIN_Y3);
    /*p27.TAXA*/ TAXA_WIN_Y4.set(!TUFU_WIN_Y3,   _SYNY_VBLANK_RSTn, !TAXA_WIN_Y4);
    /*p27.TOZO*/ TOZO_WIN_Y5.set(!TAXA_WIN_Y4,   _SYNY_VBLANK_RSTn, !TOZO_WIN_Y5);
    /*p27.TATE*/ TATE_WIN_Y6.set(!TOZO_WIN_Y5,   _SYNY_VBLANK_RSTn, !TATE_WIN_Y6);
    /*p27.TEKE*/ TEKE_WIN_Y7.set(!TATE_WIN_Y6,   _SYNY_VBLANK_RSTn, !TEKE_WIN_Y7);
  }

  // FF4A
  {
    /*p22.XOLA*/ wire XOLA_A00n = not(top.CPU_PIN_A00);
    /*p22.XENO*/ wire XENO_A01n = not(top.CPU_PIN_A01);
    /*p22.XUSY*/ wire XUSY_A02n = not(top.CPU_PIN_A02);
    /*p22.XERA*/ wire XERA_A03n = not(top.CPU_PIN_A03);

    /*p22.WESA*/ wire WESA_A01p = not(XENO_A01n);
    /*p22.WEPO*/ wire WEPO_A03p = not(XERA_A03n);

    /*p22.WYVO*/ wire FF4An = nand(top.WERO_FF4Xp(), XOLA_A00n, WESA_A01p, XUSY_A02n, WEPO_A03p);
    /*p22.VYGA*/ wire FF4A = not(FF4An);

    /*p07.TEDO*/ wire TEDO_CPU_RD = not(top.UJYV_CPU_RDn());
    /*p07.AJAS*/ wire AJAS_CPU_RD = not(TEDO_CPU_RD);
    /*p07.ASOT*/ wire ASOT_CPU_RD = not(AJAS_CPU_RD);
    /*p23.WAXU*/ wire FF4A_RD = and (ASOT_CPU_RD, FF4A);
    /*p23.VOMY*/ wire FF4A_RDn = not(FF4A_RD);

    /*p07.TAPU*/ wire TAPU_CPU_WR_xxxxxFGH = not(top.UBAL_CPU_WRp_ABCDExxx());
    /*p07.DYKY*/ wire DYKY_CPU_WR_ABCDExxx = not(TAPU_CPU_WR_xxxxxFGH);
    /*p07.CUPA*/ wire CUPA_CPU_WR_xxxxxFGH = not(DYKY_CPU_WR_ABCDExxx);
    /*p23.WEKO*/ wire WEKO_FF4A_WR = and (CUPA_CPU_WR_xxxxxFGH, FF4A);
    /*p23.VEFU*/ wire VEFU_FF4A_WRn = not(WEKO_FF4A_WR);

    /*p01.ALUR*/ wire ALUR_RSTn = not(top.AVOR_RSTp());
    /*p01.DULA*/ wire DULA_RSTp = not(ALUR_RSTn);
    /*p01.CUNU*/ wire CUNU_RSTn = not(DULA_RSTp);
    /*p01.XORE*/ wire XORE_RSTp = not(CUNU_RSTn);
    /*p01.WALU*/ wire WALU_RSTn = not(XORE_RSTp);
    /*p23.NESO*/ NESO_WY0.set(VEFU_FF4A_WRn, !VEFU_FF4A_WRn, WALU_RSTn, top.CPU_TRI_D0);
    /*p23.NYRO*/ NYRO_WY1.set(VEFU_FF4A_WRn, !VEFU_FF4A_WRn, WALU_RSTn, top.CPU_TRI_D1);
    /*p23.NAGA*/ NAGA_WY2.set(VEFU_FF4A_WRn, !VEFU_FF4A_WRn, WALU_RSTn, top.CPU_TRI_D2);
    /*p23.MELA*/ MELA_WY3.set(VEFU_FF4A_WRn, !VEFU_FF4A_WRn, WALU_RSTn, top.CPU_TRI_D3);
    /*p23.NULO*/ NULO_WY4.set(VEFU_FF4A_WRn, !VEFU_FF4A_WRn, WALU_RSTn, top.CPU_TRI_D4);
    /*p23.NENE*/ NENE_WY5.set(VEFU_FF4A_WRn, !VEFU_FF4A_WRn, WALU_RSTn, top.CPU_TRI_D5);
    /*p23.NUKA*/ NUKA_WY6.set(VEFU_FF4A_WRn, !VEFU_FF4A_WRn, WALU_RSTn, top.CPU_TRI_D6);
    /*p23.NAFU*/ NAFU_WY7.set(VEFU_FF4A_WRn, !VEFU_FF4A_WRn, WALU_RSTn, top.CPU_TRI_D7);


    /*p23.PUNU*/ top.CPU_TRI_D0.set_tribuf(!FF4A_RDn, NESO_WY0.q());
    /*p23.PODA*/ top.CPU_TRI_D1.set_tribuf(!FF4A_RDn, NYRO_WY1.q());
    /*p23.PYGU*/ top.CPU_TRI_D2.set_tribuf(!FF4A_RDn, NAGA_WY2.q());
    /*p23.LOKA*/ top.CPU_TRI_D3.set_tribuf(!FF4A_RDn, MELA_WY3.q());
    /*p23.MEGA*/ top.CPU_TRI_D4.set_tribuf(!FF4A_RDn, NULO_WY4.q());
    /*p23.PELA*/ top.CPU_TRI_D5.set_tribuf(!FF4A_RDn, NENE_WY5.q());
    /*p23.POLO*/ top.CPU_TRI_D6.set_tribuf(!FF4A_RDn, NUKA_WY6.q());
    /*p23.MERA*/ top.CPU_TRI_D7.set_tribuf(!FF4A_RDn, NAFU_WY7.q());
  }

  // FF4B
  {
    /*p22.XOLA*/ wire XOLA_A00n = not(top.CPU_PIN_A00);
    /*p22.XENO*/ wire XENO_A01n = not(top.CPU_PIN_A01);
    /*p22.XUSY*/ wire XUSY_A02n = not(top.CPU_PIN_A02);
    /*p22.XERA*/ wire XERA_A03n = not(top.CPU_PIN_A03);

    /*p22.WADO*/ wire WADO_A00p = not(XOLA_A00n);
    /*p22.WESA*/ wire WESA_A01p = not(XENO_A01n);
    /*p22.WEPO*/ wire WEPO_A03p = not(XERA_A03n);

    /*p22.WAGE*/ wire FF4Bn = nand(top.WERO_FF4Xp(), WADO_A00p, WESA_A01p, XUSY_A02n, WEPO_A03p);
    /*p22.VUMY*/ wire FF4Bp = not(FF4Bn);

    /*p07.TEDO*/ wire TEDO_CPU_RD = not(top.UJYV_CPU_RDn());
    /*p07.AJAS*/ wire AJAS_CPU_RD = not(TEDO_CPU_RD);
    /*p07.ASOT*/ wire ASOT_CPU_RD = not(AJAS_CPU_RD);
    /*p23.WYZE*/ pwire FF4B_RDp = and (ASOT_CPU_RD, FF4Bp);
    /*p23.VYCU*/ nwire FF4B_RDn = not(FF4B_RDp);

    /*p07.TAPU*/ wire TAPU_CPU_WR_xxxxxFGH = not(top.UBAL_CPU_WRp_ABCDExxx());
    /*p07.DYKY*/ wire DYKY_CPU_WR_ABCDExxx = not(TAPU_CPU_WR_xxxxxFGH);
    /*p07.CUPA*/ wire CUPA_CPU_WR_xxxxxFGH = not(DYKY_CPU_WR_ABCDExxx);
    /*p23.WUZA*/ wire WUZA_FF4B_WR = and (CUPA_CPU_WR_xxxxxFGH, FF4Bp);
    /*p23.VOXU*/ wire VOXU_FF4B_WRn = not(WUZA_FF4B_WR);

    /*p01.ALUR*/ wire ALUR_RSTn = not(top.AVOR_RSTp());
    /*p01.DULA*/ wire DULA_RSTp = not(ALUR_RSTn);
    /*p01.CUNU*/ wire CUNU_RSTn = not(DULA_RSTp);
    /*p01.XORE*/ wire XORE_RSTp = not(CUNU_RSTn);
    /*p01.WALU*/ wire WALU_RSTn = not(XORE_RSTp);
    /*p23.MYPA*/ MYPA_WX0.set(VOXU_FF4B_WRn, !VOXU_FF4B_WRn, WALU_RSTn, top.CPU_TRI_D0);
    /*p23.NOFE*/ NOFE_WX1.set(VOXU_FF4B_WRn, !VOXU_FF4B_WRn, WALU_RSTn, top.CPU_TRI_D1);
    /*p23.NOKE*/ NOKE_WX2.set(VOXU_FF4B_WRn, !VOXU_FF4B_WRn, WALU_RSTn, top.CPU_TRI_D2);
    /*p23.MEBY*/ MEBY_WX3.set(VOXU_FF4B_WRn, !VOXU_FF4B_WRn, WALU_RSTn, top.CPU_TRI_D3);
    /*p23.MYPU*/ MYPU_WX4.set(VOXU_FF4B_WRn, !VOXU_FF4B_WRn, WALU_RSTn, top.CPU_TRI_D4);
    /*p23.MYCE*/ MYCE_WX5.set(VOXU_FF4B_WRn, !VOXU_FF4B_WRn, WALU_RSTn, top.CPU_TRI_D5);
    /*p23.MUVO*/ MUVO_WX6.set(VOXU_FF4B_WRn, !VOXU_FF4B_WRn, WALU_RSTn, top.CPU_TRI_D6);
    /*p23.NUKU*/ NUKU_WX7.set(VOXU_FF4B_WRn, !VOXU_FF4B_WRn, WALU_RSTn, top.CPU_TRI_D7);


    /*p23.LOVA*/ top.CPU_TRI_D0.set_tribuf_6n(FF4B_RDn, MYPA_WX0.q());
    /*p23.MUKA*/ top.CPU_TRI_D1.set_tribuf_6n(FF4B_RDn, NOFE_WX1.q());
    /*p23.MOKO*/ top.CPU_TRI_D2.set_tribuf_6n(FF4B_RDn, NOKE_WX2.q());
    /*p23.LOLE*/ top.CPU_TRI_D3.set_tribuf_6n(FF4B_RDn, MEBY_WX3.q());
    /*p23.MELE*/ top.CPU_TRI_D4.set_tribuf_6n(FF4B_RDn, MYPU_WX4.q());
    /*p23.MUFE*/ top.CPU_TRI_D5.set_tribuf_6n(FF4B_RDn, MYCE_WX5.q());
    /*p23.MULY*/ top.CPU_TRI_D6.set_tribuf_6n(FF4B_RDn, MUVO_WX6.q());
    /*p23.MARA*/ top.CPU_TRI_D7.set_tribuf_6n(FF4B_RDn, NUKU_WX7.q());
  }
}

//------------------------------------------------------------------------------

wire WindowRegisters::ROGE_WY_MATCHp(const SchematicTop& top) const {
  /*p27.NAZE*/ wire _WY_MATCH0 = xnor(top.MUWY_Y0(), NESO_WY0);
  /*p27.PEBO*/ wire _WY_MATCH1 = xnor(top.MYRO_Y1(), NYRO_WY1);
  /*p27.POMO*/ wire _WY_MATCH2 = xnor(top.LEXA_Y2(), NAGA_WY2);
  /*p27.NEVU*/ wire _WY_MATCH3 = xnor(top.LYDO_Y3(), MELA_WY3);
  /*p27.NOJO*/ wire _WY_MATCH4 = xnor(top.LOVU_Y4(), NULO_WY4);
  /*p27.PAGA*/ wire _WY_MATCH5 = xnor(top.LEMA_Y5(), NENE_WY5);
  /*p27.PEZO*/ wire _WY_MATCH6 = xnor(top.MATO_Y6(), NUKA_WY6);
  /*p27.NUPA*/ wire _WY_MATCH7 = xnor(top.LAFO_Y7(), NAFU_WY7);

  /*p27.PALO*/ wire _WY_MATCH_HIn  = nand(top.WYMO_LCDC_WINEN, _WY_MATCH4, _WY_MATCH5, _WY_MATCH6, _WY_MATCH7);
  /*p27.NELE*/ wire _WY_MATCH_HI   = not(_WY_MATCH_HIn);
  /*p27.PAFU*/ wire _WY_MATCHn     = nand(_WY_MATCH_HI, _WY_MATCH0, _WY_MATCH1, _WY_MATCH2, _WY_MATCH3);
  /*p27.ROGE*/ wire ROGE_WY_MATCHp = not(_WY_MATCHn);
  return ROGE_WY_MATCHp;
}

wire WindowRegisters::NUKO_WX_MATCHp(const SchematicTop& top) const {
  /*p27.MYLO*/ wire _WX_MATCH0 = xnor(top.XEHO_X0(), MYPA_WX0);
  /*p27.PUWU*/ wire _WX_MATCH1 = xnor(top.SAVY_X1(), NOFE_WX1);
  /*p27.PUHO*/ wire _WX_MATCH2 = xnor(top.XODU_X2(), NOKE_WX2);
  /*p27.NYTU*/ wire _WX_MATCH3 = xnor(top.XYDO_X3(), MEBY_WX3);
  /*p27.NEZO*/ wire _WX_MATCH4 = xnor(top.TUHU_X4(), MYPU_WX4);
  /*p27.NORY*/ wire _WX_MATCH5 = xnor(top.TUKY_X5(), MYCE_WX5);
  /*p27.NONO*/ wire _WX_MATCH6 = xnor(top.TAKO_X6(), MUVO_WX6);
  /*p27.PASE*/ wire _WX_MATCH7 = xnor(top.SYBE_X7(), NUKU_WX7);

  /*p27.PUKY*/ wire _WX_MATCH_HIn  = nand(_REJO_WY_MATCH_LATCH, _WX_MATCH4, _WX_MATCH5, _WX_MATCH6, _WX_MATCH7);
  /*p27.NUFA*/ wire _WX_MATCH_HI   = not (_WX_MATCH_HIn);
  /*p27.NOGY*/ wire _WX_MATCHn     = nand(_WX_MATCH_HI, _WX_MATCH0, _WX_MATCH1, _WX_MATCH2, _WX_MATCH3);
  /*p27.NUKO*/ wire NUKO_WX_MATCHp = not(_WX_MATCHn);
  return NUKO_WX_MATCHp;
}

//------------------------------------------------------------------------------

SignalHash WindowRegisters::commit() {
  SignalHash hash;
  /*p27.SARY*/ hash << _SARY_WY_MATCH.commit_reg();
  /*p27.RYFA*/ hash << _RYFA_WX_MATCHn_A.commit_reg();
  /*p27.RENE*/ hash << _RENE_WX_MATCHn_B.commit_reg();
  /*p27.PYCO*/ hash << _PYCO_WX_MATCH_A.commit_reg();
  /*p27.NUNU*/ hash << _NUNU_WX_MATCH_B.commit_reg();
  /*p27.REJO*/ hash << _REJO_WY_MATCH_LATCH.commit_latch();
  /*p27.NOPA*/ hash << _NOPA_WIN_MODE_B.commit_reg();
  /*p27.SOVY*/ hash << _SOVY_WIN_FIRST_TILE_B.commit_reg();
  /*p27.PYNU*/ hash << PYNU_WIN_MODE_A.commit_latch();
  /*p27.RYDY*/ hash << RYDY_WIN_FIRST_TILE_A.commit_latch();

  /*p27.WYKA*/ hash << WYKA_WIN_X3.commit_reg();
  /*p27.WODY*/ hash << WODY_WIN_X4.commit_reg();
  /*p27.WOBO*/ hash << WOBO_WIN_X5.commit_reg();
  /*p27.WYKO*/ hash << WYKO_WIN_X6.commit_reg();
  /*p27.XOLO*/ hash << XOLO_WIN_X7.commit_reg();
  /*p27.VYNO*/ hash << VYNO_WIN_Y0.commit_reg();
  /*p27.VUJO*/ hash << VUJO_WIN_Y1.commit_reg();
  /*p27.VYMU*/ hash << VYMU_WIN_Y2.commit_reg();
  /*p27.TUFU*/ hash << TUFU_WIN_Y3.commit_reg();
  /*p27.TAXA*/ hash << TAXA_WIN_Y4.commit_reg();
  /*p27.TOZO*/ hash << TOZO_WIN_Y5.commit_reg();
  /*p27.TATE*/ hash << TATE_WIN_Y6.commit_reg();
  /*p27.TEKE*/ hash << TEKE_WIN_Y7.commit_reg();

  /*p23.MYPA*/ hash << MYPA_WX0.commit_reg();
  /*p23.NOFE*/ hash << NOFE_WX1.commit_reg();
  /*p23.NOKE*/ hash << NOKE_WX2.commit_reg();
  /*p23.MEBY*/ hash << MEBY_WX3.commit_reg();
  /*p23.MYPU*/ hash << MYPU_WX4.commit_reg();
  /*p23.MYCE*/ hash << MYCE_WX5.commit_reg();
  /*p23.MUVO*/ hash << MUVO_WX6.commit_reg();
  /*p23.NUKU*/ hash << NUKU_WX7.commit_reg();

  /*p23.NESO*/ hash << NESO_WY0.commit_reg();
  /*p23.NYRO*/ hash << NYRO_WY1.commit_reg();
  /*p23.NAGA*/ hash << NAGA_WY2.commit_reg();
  /*p23.MELA*/ hash << MELA_WY3.commit_reg();
  /*p23.NULO*/ hash << NULO_WY4.commit_reg();
  /*p23.NENE*/ hash << NENE_WY5.commit_reg();
  /*p23.NUKA*/ hash << NUKA_WY6.commit_reg();
  /*p23.NAFU*/ hash << NAFU_WY7.commit_reg();



  return hash;
}

//------------------------------------------------------------------------------


// Die trace:
// VETU = and(TEVO, PORE);
// XACO = not(XOFO)
// WYKA =

// WYKA01 <> WYKA12
// WYKA02 << VETU
// WYKA03 <> WYKA09
// WYKA04 nc
// WYKA05 nc
// WYKA06 << XACO
// WYKA07 << WYKA16
// WYKA08 nc
// WYKA09 <> WYKA03
// WYKA10 nc
// WYKA11 nc
// WYKA12 <> WYKA01
// WYKA13 << XACO
// WYKA14 nc
// WYKA15 nc
// WYKA16 >> WYKA07, WODY02
// WYKA17 >> XEJA
