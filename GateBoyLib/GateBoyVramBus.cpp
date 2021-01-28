#include "GateBoyLib/GateBoyVramBus.h"

#include "GateBoyLib/GateBoy.h"

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::cpu_addr_to_vram_addr() {
  // CPU to VRAM address driver
  /*#p25.XANE*/ wire _XANE_VRAM_LOCKn = nor2(LUFA_DMA_VRAMp(), XYMU_RENDERINGn.qn_new());
  /* p25.XEDU*/ wire _XEDU_VRAM_LOCKp = not1(_XANE_VRAM_LOCKn);
  /* p25.XAKY_CA00_TO_VA00*/ new_bus.BUS_VRAM_A00n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A00p.qp_new());
  /* p25.XUXU_CA01_TO_VA01*/ new_bus.BUS_VRAM_A01n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A01p.qp_new());
  /* p25.XYNE_CA02_TO_VA02*/ new_bus.BUS_VRAM_A02n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A02p.qp_new());
  /* p25.XODY_CA03_TO_VA03*/ new_bus.BUS_VRAM_A03n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A03p.qp_new());
  /* p25.XECA_CA04_TO_VA04*/ new_bus.BUS_VRAM_A04n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A04p.qp_new());
  /* p25.XOBA_CA05_TO_VA05*/ new_bus.BUS_VRAM_A05n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A05p.qp_new());
  /* p25.XOPO_CA06_TO_VA06*/ new_bus.BUS_VRAM_A06n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A06p.qp_new());
  /* p25.XYBO_CA07_TO_VA07*/ new_bus.BUS_VRAM_A07n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A07p.qp_new());
  /* p25.RYSU_CA08_TO_VA08*/ new_bus.BUS_VRAM_A08n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A08p.qp_new());
  /* p25.RESE_CA09_TO_VA09*/ new_bus.BUS_VRAM_A09n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A09p.qp_new());
  /* p25.RUSE_CA10_TO_VA10*/ new_bus.BUS_VRAM_A10n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A10p.qp_new());
  /* p25.RYNA_CA11_TO_VA11*/ new_bus.BUS_VRAM_A11n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A11p.qp_new());
  /* p25.RUMO_CA12_TO_VA12*/ new_bus.BUS_VRAM_A12n.tri6_nn(_XEDU_VRAM_LOCKp, new_bus.BUS_CPU_A12p.qp_new());
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::dma_addr_to_vram_addr() {
  /* p04.AHOC*/ wire _AHOC_DMA_VRAMn_new = not1(LUFA_DMA_VRAMp());
  /* p04.ECAL_DA00_TO_VA00*/ new_bus.BUS_VRAM_A00n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.NAKY_DMA_A00p.qp_new());
  /* p04.EGEZ_DA01_TO_VA01*/ new_bus.BUS_VRAM_A01n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.PYRO_DMA_A01p.qp_new());
  /* p04.FUHE_DA02_TO_VA02*/ new_bus.BUS_VRAM_A02n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.NEFY_DMA_A02p.qp_new());
  /* p04.FYZY_DA03_TO_VA03*/ new_bus.BUS_VRAM_A03n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.MUTY_DMA_A03p.qp_new());
  /* p04.DAMU_DA04_TO_VA04*/ new_bus.BUS_VRAM_A04n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.NYKO_DMA_A04p.qp_new());
  /* p04.DAVA_DA05_TO_VA05*/ new_bus.BUS_VRAM_A05n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.PYLO_DMA_A05p.qp_new());
  /* p04.ETEG_DA06_TO_VA06*/ new_bus.BUS_VRAM_A06n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.NUTO_DMA_A06p.qp_new());
  /*#p04.EREW_DA07_TO_VA07*/ new_bus.BUS_VRAM_A07n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.MUGU_DMA_A07p.qp_new());
  /*#p04.EVAX_DA08_TO_VA08*/ new_bus.BUS_VRAM_A08n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.NAFA_DMA_A08n.qn_new());
  /* p04.DUVE_DA09_TO_VA09*/ new_bus.BUS_VRAM_A09n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.PYNE_DMA_A09n.qn_new());
  /* p04.ERAF_DA10_TO_VA10*/ new_bus.BUS_VRAM_A10n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.PARA_DMA_A10n.qn_new());
  /* p04.FUSY_DA11_TO_VA11*/ new_bus.BUS_VRAM_A11n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.NYDO_DMA_A11n.qn_new());
  /* p04.EXYF_DA12_TO_VA12*/ new_bus.BUS_VRAM_A12n.tri6_nn(_AHOC_DMA_VRAMn_new, dma.NYGY_DMA_A12n.qn_new());
}

//------------------------------------------------------------------------------------------------------------------------
// BG map read address

void GateBoy::scroll_to_vram_addr(BGScrollX scroll_x, BGScrollY scroll_y) {
  /* p27.POTU*/ wire POTU_BGW_MAP_READp  = and2(LENA_BFETCHINGp(), NENY_BFETCH_01n());
  /* p26.ACEN*/ wire _ACEN_BG_MAP_READp = and2(POTU_BGW_MAP_READp, AXAD_WIN_MODEn());
  /* p26.BAFY*/ wire _BAFY_BG_MAP_READn = not1(_ACEN_BG_MAP_READp);
  /* p26.AXEP_MX00_TO_VA00*/ new_bus.BUS_VRAM_A00n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_x.BABE_MAP_X0.sum);
  /* p26.AFEB_MX01_TO_VA01*/ new_bus.BUS_VRAM_A01n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_x.ABOD_MAP_X1.sum);
  /* p26.ALEL_MX02_TO_VA02*/ new_bus.BUS_VRAM_A02n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_x.BEWY_MAP_X2.sum);
  /* p26.COLY_MX03_TO_VA03*/ new_bus.BUS_VRAM_A03n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_x.BYCA_MAP_X3.sum);
  /* p26.AJAN_MX04_TO_VA04*/ new_bus.BUS_VRAM_A04n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_x.ACUL_MAP_X4.sum);
  /* p26.DUHO_MY00_TO_VA05*/ new_bus.BUS_VRAM_A05n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_y.ETAM_MAP_Y0.sum);
  /* p26.CASE_MY01_TO_VA06*/ new_bus.BUS_VRAM_A06n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_y.DOTO_MAP_Y1.sum);
  /* p26.CYPO_MY02_TO_VA07*/ new_bus.BUS_VRAM_A07n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_y.DABA_MAP_Y2.sum);
  /* p26.CETA_MY03_TO_VA08*/ new_bus.BUS_VRAM_A08n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_y.EFYK_MAP_Y3.sum);
  /* p26.DAFE_MY04_TO_VA09*/ new_bus.BUS_VRAM_A09n.tri6_nn(_BAFY_BG_MAP_READn,  scroll_y.EJOK_MAP_Y4.sum);
  /*#p26.AMUV_BMAP_TO_VA10*/ new_bus.BUS_VRAM_A10n.tri6_nn(_BAFY_BG_MAP_READn, reg_lcdc.XAFO_LCDC_BGMAPn.qn_new());
  /* p26.COVE_BMAP_TO_VA11*/ new_bus.BUS_VRAM_A11n.tri6_nn(_BAFY_BG_MAP_READn, SIG_VCC.qp_new());
  /* p26.COXO_BMAP_TO_VA12*/ new_bus.BUS_VRAM_A12n.tri6_nn(_BAFY_BG_MAP_READn, SIG_VCC.qp_new());
}

//------------------------------------------------------------------------------------------------------------------------
// Win map read address


void GateBoy::win_to_vram_addr() {
  /* p27.POTU*/ wire POTU_BGW_MAP_READp  = and2(LENA_BFETCHINGp(), NENY_BFETCH_01n());
  /*#p25.XEZE*/ wire _XEZE_WIN_MAP_READp = and2(POTU_BGW_MAP_READp, PORE_WIN_MODEp());
  /*#p25.WUKO*/ wire _WUKO_WIN_MAP_READn = not1(_XEZE_WIN_MAP_READp);
  /*#p27.XEJA_WX03_TO_VA00*/ new_bus.BUS_VRAM_A00n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_x.WYKA_WIN_X3.qp_new());
  /* p27.XAMO_WX04_TO_VA01*/ new_bus.BUS_VRAM_A01n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_x.WODY_WIN_X4.qp_new());
  /* p27.XAHE_WX05_TO_VA02*/ new_bus.BUS_VRAM_A02n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_x.WOBO_WIN_X5.qp_new());
  /* p27.XULO_WX06_TO_VA03*/ new_bus.BUS_VRAM_A03n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_x.WYKO_WIN_X6.qp_new());
  /* p27.WUJU_WX07_TO_VA04*/ new_bus.BUS_VRAM_A04n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_x.XOLO_WIN_X7.qp_new());
  /*#p27.VYTO_WY03_TO_VA05*/ new_bus.BUS_VRAM_A05n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_y.TUFU_WIN_Y3.qp_new());
  /* p27.VEHA_WY04_TO_VA06*/ new_bus.BUS_VRAM_A06n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_y.TAXA_WIN_Y4.qp_new());
  /* p27.VACE_WY05_TO_VA07*/ new_bus.BUS_VRAM_A07n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_y.TOZO_WIN_Y5.qp_new());
  /* p27.VOVO_WY06_TO_VA08*/ new_bus.BUS_VRAM_A08n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_y.TATE_WIN_Y6.qp_new());
  /* p27.VULO_WY07_TO_VA09*/ new_bus.BUS_VRAM_A09n.tri6_nn(_WUKO_WIN_MAP_READn, win_map_y.TEKE_WIN_Y7.qp_new());
  /*#p27.VEVY_WMAP_TO_VA10*/ new_bus.BUS_VRAM_A10n.tri6_nn(_WUKO_WIN_MAP_READn, reg_lcdc.WOKY_LCDC_WINMAPn.qn_new());
  /* p27.VEZA_WMAP_TO_VA11*/ new_bus.BUS_VRAM_A11n.tri6_nn(_WUKO_WIN_MAP_READn, SIG_VCC.qp_new());
  /* p27.VOGU_WMAP_TO_VA12*/ new_bus.BUS_VRAM_A12n.tri6_nn(_WUKO_WIN_MAP_READn, SIG_VCC.qp_new());
}

//------------------------------------------------------------------------------------------------------------------------
// BG/Win tile read address

void GateBoy::tile_to_vram_addr(const BGScrollY scroll_y) {

  /* p27.NETA*/ wire NETA_BGW_TILE_READp = and2(LENA_BFETCHINGp(), NOGU_BFETCH_01p());

  /* p26.ASUL*/ wire _ASUL_BG_TILE_READp =  and2(NETA_BGW_TILE_READp, AXAD_WIN_MODEn());
  /* p26.BEJE*/ wire _BEJE_BG_TILE_READn = not1(_ASUL_BG_TILE_READp);
  /* p25.XUCY*/ wire _XUCY_WIN_TILE_READn = nand2(NETA_BGW_TILE_READp, PORE_WIN_MODEp());

  /*#p27.XUHA*/ wire XUHA_FETCH_HILOp = not1(NOFU_BFETCH_S2n());

  /*#p26.ASUM_HILO_TO_VA00*/ new_bus.BUS_VRAM_A00n.tri6_nn(_BEJE_BG_TILE_READn, XUHA_FETCH_HILOp);
  /* p26.EVAD_TLY0_TO_VA01*/ new_bus.BUS_VRAM_A01n.tri6_nn(_BEJE_BG_TILE_READn, scroll_y.FAFO_TILE_Y0.sum);
  /* p26.DAHU_TLY1_TO_VA02*/ new_bus.BUS_VRAM_A02n.tri6_nn(_BEJE_BG_TILE_READn, scroll_y.EMUX_TILE_Y1.sum);
  /* p26.DODE_TLY2_TO_VA03*/ new_bus.BUS_VRAM_A03n.tri6_nn(_BEJE_BG_TILE_READn, scroll_y.ECAB_TILE_Y2.sum);

  /*#p25.XONU_HILO_TO_VA00*/ new_bus.BUS_VRAM_A00n.tri6_nn(_XUCY_WIN_TILE_READn, XUHA_FETCH_HILOp);
  /*#p25.WUDO_WNY0_TO_VA01*/ new_bus.BUS_VRAM_A01n.tri6_nn(_XUCY_WIN_TILE_READn, win_map_y.VYNO_WIN_Y0.qp_new());
  /*#p25.WAWE_WNY1_TO_VA02*/ new_bus.BUS_VRAM_A02n.tri6_nn(_XUCY_WIN_TILE_READn, win_map_y.VUJO_WIN_Y1.qp_new());
  /*#p25.WOLU_WNY2_TO_VA03*/ new_bus.BUS_VRAM_A03n.tri6_nn(_XUCY_WIN_TILE_READn, win_map_y.VYMU_WIN_Y2.qp_new());

  /*#p25.VAPY_TEMP_TO_VA04*/ new_bus.BUS_VRAM_A04n.tri6_pn(NETA_BGW_TILE_READp, tile_temp_b.RAWU_TILE_DB0p.qp_new());
  /*#p25.SEZU_TEMP_TO_VA05*/ new_bus.BUS_VRAM_A05n.tri6_pn(NETA_BGW_TILE_READp, tile_temp_b.POZO_TILE_DB1p.qp_new());
  /*#p25.VEJY_TEMP_TO_VA06*/ new_bus.BUS_VRAM_A06n.tri6_pn(NETA_BGW_TILE_READp, tile_temp_b.PYZO_TILE_DB2p.qp_new());
  /*#p25.RUSA_TEMP_TO_VA07*/ new_bus.BUS_VRAM_A07n.tri6_pn(NETA_BGW_TILE_READp, tile_temp_b.POXA_TILE_DB3p.qp_new());
  /*#p25.ROHA_TEMP_TO_VA08*/ new_bus.BUS_VRAM_A08n.tri6_pn(NETA_BGW_TILE_READp, tile_temp_b.PULO_TILE_DB4p.qp_new());
  /*#p25.RESO_TEMP_TO_VA09*/ new_bus.BUS_VRAM_A09n.tri6_pn(NETA_BGW_TILE_READp, tile_temp_b.POJU_TILE_DB5p.qp_new());
  /*#p25.SUVO_TEMP_TO_VA10*/ new_bus.BUS_VRAM_A10n.tri6_pn(NETA_BGW_TILE_READp, tile_temp_b.POWY_TILE_DB6p.qp_new());
  /*#p25.TOBO_TEMP_TO_VA11*/ new_bus.BUS_VRAM_A11n.tri6_pn(NETA_BGW_TILE_READp, tile_temp_b.PYJU_TILE_DB7p.qp_new());

  /*#p25.VUZA*/ wire _VUZA_TILE_BANKp = nor2(tile_temp_b.PYJU_TILE_DB7p.qp_new(), reg_lcdc.WEXU_LCDC_BGTILEn.qn_new());
  /*#p25.VURY_BANK_TO_VA12*/ new_bus.BUS_VRAM_A12n.tri6_pn(NETA_BGW_TILE_READp, _VUZA_TILE_BANKp);
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::sprite_to_vram_addr() {
  /*#p29.WUKY*/ wire _WUKY_FLIP_Yp = not1(oam_temp_b.YZOS_OAM_DB6p.qp_new());
  /*#p29.FUFO*/ wire _FUFO_LCDC_SPSIZEn = not1(reg_lcdc.XYMO_LCDC_SPSIZEn.qn_new());

  /*#p29.CYVU*/ wire _CYVU_L0 = xor2(_WUKY_FLIP_Yp, new_bus.BUS_SPR_L0.qp_new());
  /*#p29.BORE*/ wire _BORE_L1 = xor2(_WUKY_FLIP_Yp, new_bus.BUS_SPR_L1.qp_new());
  /*#p29.BUVY*/ wire _BUVY_L2 = xor2(_WUKY_FLIP_Yp, new_bus.BUS_SPR_L2.qp_new());
  /*#p29.WAGO*/ wire _WAGO_L3 = xor2(_WUKY_FLIP_Yp, new_bus.BUS_SPR_L3.qp_new());
  /*#p29.GEJY*/ wire _GEJY_L3 = amux2(oam_temp_a.XUSO_OAM_DA0p.qp_new(), _FUFO_LCDC_SPSIZEn, reg_lcdc.XYMO_LCDC_SPSIZEn.qn_new(), _WAGO_L3);

  /* p29.TEPA*/ wire _TEPA_RENDERINGp = not1(XYMU_RENDERINGn.qn_new());
  /* p29.TYSO*/ wire _TYSO_SFETCHINGn = or2(SAKY_SFETCHn(), _TEPA_RENDERINGp); // def or
  /* p29.TEXY*/ wire _TEXY_SFETCHINGp = not1(_TYSO_SFETCHINGn);
  /* p29.ABON*/ wire _ABON_SFETCHINGn = not1(_TEXY_SFETCHINGp);
  /* p29.ABEM_HILO_TO_VA00*/ new_bus.BUS_VRAM_A00n.tri6_nn(_ABON_SFETCHINGn, XUQU_SPRITE_AB());
  /* p29.BAXE_SPL0_TO_VA01*/ new_bus.BUS_VRAM_A01n.tri6_nn(_ABON_SFETCHINGn, _CYVU_L0);
  /* p29.ARAS_SPL1_TO_VA02*/ new_bus.BUS_VRAM_A02n.tri6_nn(_ABON_SFETCHINGn, _BORE_L1);
  /* p29.AGAG_SPL2_TO_VA03*/ new_bus.BUS_VRAM_A03n.tri6_nn(_ABON_SFETCHINGn, _BUVY_L2);
  /* p29.FAMU_SPL3_TO_VA04*/ new_bus.BUS_VRAM_A04n.tri6_nn(_ABON_SFETCHINGn, _GEJY_L3);
  /*#p29.FUGY_ODA1_TO_VA05*/ new_bus.BUS_VRAM_A05n.tri6_nn(_ABON_SFETCHINGn, oam_temp_a.XEGU_OAM_DA1p.qp_new());
  /* p29.GAVO_ODA2_TO_VA06*/ new_bus.BUS_VRAM_A06n.tri6_nn(_ABON_SFETCHINGn, oam_temp_a.YJEX_OAM_DA2p.qp_new());
  /* p29.WYGA_ODA3_TO_VA07*/ new_bus.BUS_VRAM_A07n.tri6_nn(_ABON_SFETCHINGn, oam_temp_a.XYJU_OAM_DA3p.qp_new());
  /* p29.WUNE_ODA4_TO_VA08*/ new_bus.BUS_VRAM_A08n.tri6_nn(_ABON_SFETCHINGn, oam_temp_a.YBOG_OAM_DA4p.qp_new());
  /* p29.GOTU_ODA5_TO_VA09*/ new_bus.BUS_VRAM_A09n.tri6_nn(_ABON_SFETCHINGn, oam_temp_a.WYSO_OAM_DA5p.qp_new());
  /* p29.GEGU_ODA6_TO_VA10*/ new_bus.BUS_VRAM_A10n.tri6_nn(_ABON_SFETCHINGn, oam_temp_a.XOTE_OAM_DA6p.qp_new());
  /* p29.XEHE_ODA7_TO_VA11*/ new_bus.BUS_VRAM_A11n.tri6_nn(_ABON_SFETCHINGn, oam_temp_a.YZAB_OAM_DA7p.qp_new());
  /* p29.DYSO_BANK_TO_VA12*/ new_bus.BUS_VRAM_A12n.tri6_nn(_ABON_SFETCHINGn, SIG_GND.qp_new());   // sprites always in low half of tile store
}

//------------------------------------------------------------------------------------------------------------------------
// PIN_VRAM_A driver

void GateBoy::vram_addr_to_pins() {
  /* p25.MYFU*/ wire _MYFUp = not1(new_bus.BUS_VRAM_A00n.qp_new());
  /* p25.MASA*/ wire _MASAp = not1(new_bus.BUS_VRAM_A01n.qp_new());
  /* p25.MYRE*/ wire _MYREp = not1(new_bus.BUS_VRAM_A02n.qp_new());
  /* p25.MAVU*/ wire _MAVUp = not1(new_bus.BUS_VRAM_A03n.qp_new());
  /* p25.MEPA*/ wire _MEPAp = not1(new_bus.BUS_VRAM_A04n.qp_new());
  /* p25.MYSA*/ wire _MYSAp = not1(new_bus.BUS_VRAM_A05n.qp_new());
  /* p25.MEWY*/ wire _MEWYp = not1(new_bus.BUS_VRAM_A06n.qp_new());
  /* p25.MUME*/ wire _MUMEp = not1(new_bus.BUS_VRAM_A07n.qp_new());
  /* p25.VOVA*/ wire _VOVAp = not1(new_bus.BUS_VRAM_A08n.qp_new());
  /* p25.VODE*/ wire _VODEp = not1(new_bus.BUS_VRAM_A09n.qp_new());
  /* p25.RUKY*/ wire _RUKYp = not1(new_bus.BUS_VRAM_A10n.qp_new());
  /* p25.RUMA*/ wire _RUMAp = not1(new_bus.BUS_VRAM_A11n.qp_new());
  /* p25.REHO*/ wire _REHOp = not1(new_bus.BUS_VRAM_A12n.qp_new());

  /* p25.LEXE*/ wire _LEXEn = not1(_MYFUp);
  /* p25.LOZU*/ wire _LOZUn = not1(_MASAp);
  /* p25.LACA*/ wire _LACAn = not1(_MYREp);
  /* p25.LUVO*/ wire _LUVOn = not1(_MAVUp);
  /* p25.LOLY*/ wire _LOLYn = not1(_MEPAp);
  /* p25.LALO*/ wire _LALOn = not1(_MYSAp);
  /* p25.LEFA*/ wire _LEFAn = not1(_MEWYp);
  /* p25.LUBY*/ wire _LUBYn = not1(_MUMEp);
  /* p25.TUJY*/ wire _TUJYn = not1(_VOVAp);
  /* p25.TAGO*/ wire _TAGOn = not1(_VODEp);
  /* p25.NUVA*/ wire _NUVAn = not1(_RUKYp);
  /* p25.PEDU*/ wire _PEDUn = not1(_RUMAp);
  /* p25.PONY*/ wire _PONYn = not1(_REHOp);

  /*PIN_34*/ vram_bus.PIN_34_VRAM_A00.pin_out_dp(_LEXEn);
  /*PIN_35*/ vram_bus.PIN_35_VRAM_A01.pin_out_dp(_LOZUn);
  /*PIN_36*/ vram_bus.PIN_36_VRAM_A02.pin_out_dp(_LACAn);
  /*PIN_37*/ vram_bus.PIN_37_VRAM_A03.pin_out_dp(_LUVOn);
  /*PIN_38*/ vram_bus.PIN_38_VRAM_A04.pin_out_dp(_LOLYn);
  /*PIN_39*/ vram_bus.PIN_39_VRAM_A05.pin_out_dp(_LALOn);
  /*PIN_40*/ vram_bus.PIN_40_VRAM_A06.pin_out_dp(_LEFAn);
  /*PIN_41*/ vram_bus.PIN_41_VRAM_A07.pin_out_dp(_LUBYn);
  /*PIN_48*/ vram_bus.PIN_48_VRAM_A08.pin_out_dp(_TUJYn);
  /*PIN_47*/ vram_bus.PIN_47_VRAM_A09.pin_out_dp(_TAGOn);
  /*PIN_44*/ vram_bus.PIN_44_VRAM_A10.pin_out_dp(_NUVAn);
  /*PIN_46*/ vram_bus.PIN_46_VRAM_A11.pin_out_dp(_PEDUn);
  /*PIN_42*/ vram_bus.PIN_42_VRAM_A12.pin_out_dp(_PONYn);
}

//------------------------------------------------------------------------------------------------------------------------
// PIN_VRAM_D driver

void GateBoy::cpu_data_to_vram_bus_data() {
  /*#p25.RUVY*/ wire _RUVY_CPU_VRAM_WRp = not1(SALE_CPU_VRAM_WRn());
  /*#p25.SAZO*/ wire _SAZO_CBD_TO_VPDp = and2(SERE_CPU_VRAM_RDp(), _RUVY_CPU_VRAM_WRp);

  /*#p25.RYJE*/ wire _RYJE_CBD_TO_VPDn = not1(_SAZO_CBD_TO_VPDp);
  /*#p25.REVO*/ wire _REVO_CBD_TO_VPDp = not1(_RYJE_CBD_TO_VPDn);
  /* p25.ROCY*/ wire _ROCY_CBD_TO_VPDp = and2(_REVO_CBD_TO_VPDp, _SAZO_CBD_TO_VPDp);
  /* p25.RAHU*/ wire _RAHU_CBD_TO_VPDn = not1(_ROCY_CBD_TO_VPDp);

  /* p25.TEME_CD0_TO_VD0*/ new_bus.BUS_VRAM_D00p.tri10_np(_RAHU_CBD_TO_VPDn, new_bus.BUS_CPU_D00p.qp_new());
  /* p25.TEWU_CD1_TO_VD1*/ new_bus.BUS_VRAM_D01p.tri10_np(_RAHU_CBD_TO_VPDn, new_bus.BUS_CPU_D01p.qp_new());
  /*#p25.TYGO_CD2_TO_VD2*/ new_bus.BUS_VRAM_D02p.tri10_np(_RAHU_CBD_TO_VPDn, new_bus.BUS_CPU_D02p.qp_new());
  /* p25.SOTE_CD3_TO_VD3*/ new_bus.BUS_VRAM_D03p.tri10_np(_RAHU_CBD_TO_VPDn, new_bus.BUS_CPU_D03p.qp_new());
  /* p25.SEKE_CD4_TO_VD4*/ new_bus.BUS_VRAM_D04p.tri10_np(_RAHU_CBD_TO_VPDn, new_bus.BUS_CPU_D04p.qp_new());
  /* p25.RUJO_CD5_TO_VD5*/ new_bus.BUS_VRAM_D05p.tri10_np(_RAHU_CBD_TO_VPDn, new_bus.BUS_CPU_D05p.qp_new());
  /* p25.TOFA_CD6_TO_VD6*/ new_bus.BUS_VRAM_D06p.tri10_np(_RAHU_CBD_TO_VPDn, new_bus.BUS_CPU_D06p.qp_new());
  /* p25.SUZA_CD7_TO_VD7*/ new_bus.BUS_VRAM_D07p.tri10_np(_RAHU_CBD_TO_VPDn, new_bus.BUS_CPU_D07p.qp_new());
}

void GateBoy::vram_bus_data_to_pins() {
  /*#p25.RUVY*/ wire _RUVY_CPU_VRAM_WRp = not1(SALE_CPU_VRAM_WRn());
  /*#p25.SAZO*/ wire _SAZO_CBD_TO_VPDp = and2(SERE_CPU_VRAM_RDp(), _RUVY_CPU_VRAM_WRp);

  /*#p25.RYJE*/ wire _RYJE_CBD_TO_VPDn = not1(_SAZO_CBD_TO_VPDp);
  /*#p25.REVO*/ wire _REVO_CBD_TO_VPDp = not1(_RYJE_CBD_TO_VPDn);

  /* p25.ROCY*/ wire _ROCY_CBD_TO_VPDp = and2(_REVO_CBD_TO_VPDp, _SAZO_CBD_TO_VPDp);
  /*#p25.RELA*/ wire _RELA_CBD_TO_VPDp =  or2(_REVO_CBD_TO_VPDp, _SAZO_CBD_TO_VPDp);

  /* p25.RAHU*/ wire _RAHU_CBD_TO_VPDn = not1(_ROCY_CBD_TO_VPDp);
  /*#p25.RENA*/ wire _RENA_CBD_TO_VPDn = not1(_RELA_CBD_TO_VPDp);

  /*#p25.ROVE*/ wire _ROVE_CBD_TO_VPDp = not1(_RAHU_CBD_TO_VPDn);
  /*#p25.ROFA*/ wire _ROFA_CBD_TO_VPDp = not1(_RENA_CBD_TO_VPDn);

  /*#p25.SEFA*/ wire _SEFA_D0p = and2(new_bus.BUS_VRAM_D00p.qp_new(), _ROVE_CBD_TO_VPDp);
  /* p25.SOGO*/ wire _SOGO_D1p = and2(new_bus.BUS_VRAM_D01p.qp_new(), _ROVE_CBD_TO_VPDp);
  /* p25.SEFU*/ wire _SEFU_D2p = and2(new_bus.BUS_VRAM_D02p.qp_new(), _ROVE_CBD_TO_VPDp);
  /* p25.SUNA*/ wire _SUNA_D3p = and2(new_bus.BUS_VRAM_D03p.qp_new(), _ROVE_CBD_TO_VPDp);
  /* p25.SUMO*/ wire _SUMO_D4p = and2(new_bus.BUS_VRAM_D04p.qp_new(), _ROVE_CBD_TO_VPDp);
  /* p25.SAZU*/ wire _SAZU_D5p = and2(new_bus.BUS_VRAM_D05p.qp_new(), _ROVE_CBD_TO_VPDp);
  /* p25.SAMO*/ wire _SAMO_D6p = and2(new_bus.BUS_VRAM_D06p.qp_new(), _ROVE_CBD_TO_VPDp);
  /* p25.SUKE*/ wire _SUKE_D7p = and2(new_bus.BUS_VRAM_D07p.qp_new(), _ROVE_CBD_TO_VPDp);

  /*#p25.SYNU*/ wire _SYNU_D0p = or2(_RAHU_CBD_TO_VPDn, new_bus.BUS_VRAM_D00p.qp_new());
  /* p25.SYMA*/ wire _SYMA_D1p = or2(_RAHU_CBD_TO_VPDn, new_bus.BUS_VRAM_D01p.qp_new());
  /* p25.ROKO*/ wire _ROKO_D2p = or2(_RAHU_CBD_TO_VPDn, new_bus.BUS_VRAM_D02p.qp_new());
  /* p25.SYBU*/ wire _SYBU_D3p = or2(_RAHU_CBD_TO_VPDn, new_bus.BUS_VRAM_D03p.qp_new());
  /* p25.SAKO*/ wire _SAKO_D4p = or2(_RAHU_CBD_TO_VPDn, new_bus.BUS_VRAM_D04p.qp_new());
  /* p25.SEJY*/ wire _SEJY_D5p = or2(_RAHU_CBD_TO_VPDn, new_bus.BUS_VRAM_D05p.qp_new());
  /* p25.SEDO*/ wire _SEDO_D6p = or2(_RAHU_CBD_TO_VPDn, new_bus.BUS_VRAM_D06p.qp_new());
  /* p25.SAWU*/ wire _SAWU_D7p = or2(_RAHU_CBD_TO_VPDn, new_bus.BUS_VRAM_D07p.qp_new());

  /*#p25.REGE*/ wire _REGE_D0n = not1(_SEFA_D0p);
  /* p25.RYKY*/ wire _RYKY_D1n = not1(_SOGO_D1p);
  /* p25.RAZO*/ wire _RAZO_D2n = not1(_SEFU_D2p);
  /* p25.RADA*/ wire _RADA_D3n = not1(_SUNA_D3p);
  /* p25.RYRO*/ wire _RYRO_D4n = not1(_SUMO_D4p);
  /* p25.REVU*/ wire _REVU_D5n = not1(_SAZU_D5p);
  /* p25.REKU*/ wire _REKU_D6n = not1(_SAMO_D6p);
  /* p25.RYZE*/ wire _RYZE_D7n = not1(_SUKE_D7p);

  /*#p25.RURA*/ wire _RURA_D0n = not1(_SYNU_D0p);
  /* p25.RULY*/ wire _RULY_D1n = not1(_SYMA_D1p);
  /* p25.RARE*/ wire _RARE_D2n = not1(_ROKO_D2p);
  /* p25.RODU*/ wire _RODU_D3n = not1(_SYBU_D3p);
  /* p25.RUBE*/ wire _RUBE_D4n = not1(_SAKO_D4p);
  /* p25.RUMU*/ wire _RUMU_D5n = not1(_SEJY_D5p);
  /* p25.RYTY*/ wire _RYTY_D6n = not1(_SEDO_D6p);
  /* p25.RADY*/ wire _RADY_D7n = not1(_SAWU_D7p);

  /*PIN_33*/ vram_bus.PIN_33_VRAM_D00.pin_io_out_pull_hilo(_ROFA_CBD_TO_VPDp, _REGE_D0n, _RURA_D0n);
  /*PIN_31*/ vram_bus.PIN_31_VRAM_D01.pin_io_out_pull_hilo(_ROFA_CBD_TO_VPDp, _RYKY_D1n, _RULY_D1n);
  /*PIN_30*/ vram_bus.PIN_30_VRAM_D02.pin_io_out_pull_hilo(_ROFA_CBD_TO_VPDp, _RAZO_D2n, _RARE_D2n);
  /*PIN_29*/ vram_bus.PIN_29_VRAM_D03.pin_io_out_pull_hilo(_ROFA_CBD_TO_VPDp, _RADA_D3n, _RODU_D3n);
  /*PIN_28*/ vram_bus.PIN_28_VRAM_D04.pin_io_out_pull_hilo(_ROFA_CBD_TO_VPDp, _RYRO_D4n, _RUBE_D4n);
  /*PIN_27*/ vram_bus.PIN_27_VRAM_D05.pin_io_out_pull_hilo(_ROFA_CBD_TO_VPDp, _REVU_D5n, _RUMU_D5n);
  /*PIN_26*/ vram_bus.PIN_26_VRAM_D06.pin_io_out_pull_hilo(_ROFA_CBD_TO_VPDp, _REKU_D6n, _RYTY_D6n);
  /*PIN_25*/ vram_bus.PIN_25_VRAM_D07.pin_io_out_pull_hilo(_ROFA_CBD_TO_VPDp, _RYZE_D7n, _RADY_D7n);
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::set_vram_pin_cs() {
  /*#p25.SUTU*/ wire _SUTU_MCSn = nor4(LENA_BFETCHINGp(), LUFA_DMA_VRAMp(), TEXY_SFETCHINGp(), SERE_CPU_VRAM_RDp());

  /*#p25.RACO*/ wire _RACO_DBG_VRAMn = not1(TUTO_VRAM_DBGp());
  /* p25.TODE*/ wire _TODE_MCSn_A = and2(_SUTU_MCSn, _RACO_DBG_VRAMn);
  /* p25.SEWO*/ wire _SEWO_MCSn_D =  or2(_SUTU_MCSn, TUTO_VRAM_DBGp());
  /* p25.SOKY*/ wire _SOKY_MCSp_A = not1(_TODE_MCSn_A);
  /* p25.SETY*/ wire _SETY_MCSp_D = not1(_SEWO_MCSn_D);

  /*PIN_43*/ vram_bus.PIN_43_VRAM_CSn.pin_out_hilo(_SOKY_MCSp_A, _SETY_MCSp_D); // FIXME not actually using this pin, but we should
}

void GateBoy::set_vram_pin_wr() {
  ///* p25.SUDO*/ wire _SUDO_MWRp = not1(/*vram_bus.PIN_VRAM_WRn.qn_new()*/ 1); // Ignoring debug stuff for now
  /* p25.SUDO*/ wire _SUDO_MWRp = not1(SIG_VCC.qp_new()); // Ignoring debug stuff for now
  /* p25.TYJY*/ wire _TYJY_VRAM_WRp = mux2p(TUTO_VRAM_DBGp(), _SUDO_MWRp, TUJA_CPU_VRAM_WRp());
  /* p25.SOHY*/ wire _SOHY_MWRn   = nand2(_TYJY_VRAM_WRp, SERE_CPU_VRAM_RDp());

  /*#p25.RACO*/ wire _RACO_DBG_VRAMn = not1(TUTO_VRAM_DBGp());
  /* p25.TAXY*/ wire _TAXY_MWRn_A = and2(_SOHY_MWRn, _RACO_DBG_VRAMn);
  /* p25.SOFY*/ wire _SOFY_MWRn_D =  or2(_SOHY_MWRn, TUTO_VRAM_DBGp());
  /* p25.SYSY*/ wire _SYSY_MWRp_A = not1(_TAXY_MWRn_A);
  /* p25.RAGU*/ wire _RAGU_MWRp_D = not1(_SOFY_MWRn_D);

  /*PIN_49*/ vram_bus.PIN_49_VRAM_WRn.pin_out_hilo(_SYSY_MWRp_A, _RAGU_MWRp_D);
}

void GateBoy::set_vram_pin_oe() {
  /*#p25.XANE*/ wire _XANE_VRAM_LOCKn = nor2(LUFA_DMA_VRAMp(), XYMU_RENDERINGn.qn_new());
  /* p25.RYLU*/ wire _RYLU_CPU_VRAM_RDn = nand2(SALE_CPU_VRAM_WRn(), _XANE_VRAM_LOCKn);
  /* p25.RAWA*/ wire _RAWA_SPR_VRAM_RDn = not1(SOHO_SPR_VRAM_RDp());
  /* p27.MYMA*/ wire _MYMA_BGW_VRAM_RDn = not1(tile_fetcher.LONY_FETCHINGp.qp_new());
  /* p25.APAM*/ wire _APAM_DMA_VRAMn    = not1(LUFA_DMA_VRAMp());

  /* p25.RACU*/ wire _RACU_MOEn   = and4(_RYLU_CPU_VRAM_RDn, _RAWA_SPR_VRAM_RDn, _MYMA_BGW_VRAM_RDn, _APAM_DMA_VRAMn); // def and

  /*#p25.RACO*/ wire _RACO_DBG_VRAMn = not1(TUTO_VRAM_DBGp());
  /* p25.SEMA*/ wire _SEMA_MOEn_A = and2(_RACU_MOEn, _RACO_DBG_VRAMn);
  /* p25.RUTE*/ wire _RUTE_MOEn_D =  or2(_RACU_MOEn, TUTO_VRAM_DBGp()); // schematic wrong, second input is RACU
  /* p25.REFO*/ wire _REFO_MOEn_A = not1(_SEMA_MOEn_A);
  /* p25.SAHA*/ wire _SAHA_MOEn_D = not1(_RUTE_MOEn_D);
  /*PIN_45*/ vram_bus.PIN_45_VRAM_OEn.pin_out_hilo(_REFO_MOEn_A, _SAHA_MOEn_D);
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::read_vram() {
  uint16_t addr = (uint16_t)BitBase::pack_ext_new(13, &vram_bus.PIN_34_VRAM_A00);
  uint8_t data = 0xFF;

  if (bit(~vram_bus.PIN_45_VRAM_OEn.qp_ext_new())) {
    data = vid_ram[addr];
  }

  // not tagging these, we'll leave only the internal connections in plait.
  vram_bus.PIN_33_VRAM_D00.pin_io_in_oedp(~vram_bus.PIN_45_VRAM_OEn.qp_ext_new(), bit(data, 0));
  vram_bus.PIN_31_VRAM_D01.pin_io_in_oedp(~vram_bus.PIN_45_VRAM_OEn.qp_ext_new(), bit(data, 1));
  vram_bus.PIN_30_VRAM_D02.pin_io_in_oedp(~vram_bus.PIN_45_VRAM_OEn.qp_ext_new(), bit(data, 2));
  vram_bus.PIN_29_VRAM_D03.pin_io_in_oedp(~vram_bus.PIN_45_VRAM_OEn.qp_ext_new(), bit(data, 3));
  vram_bus.PIN_28_VRAM_D04.pin_io_in_oedp(~vram_bus.PIN_45_VRAM_OEn.qp_ext_new(), bit(data, 4));
  vram_bus.PIN_27_VRAM_D05.pin_io_in_oedp(~vram_bus.PIN_45_VRAM_OEn.qp_ext_new(), bit(data, 5));
  vram_bus.PIN_26_VRAM_D06.pin_io_in_oedp(~vram_bus.PIN_45_VRAM_OEn.qp_ext_new(), bit(data, 6));
  vram_bus.PIN_25_VRAM_D07.pin_io_in_oedp(~vram_bus.PIN_45_VRAM_OEn.qp_ext_new(), bit(data, 7));
}

void GateBoy::write_vram() {
  uint16_t addr= (uint16_t)BitBase::pack_ext_new(13, &vram_bus.PIN_34_VRAM_A00);
  uint8_t data = (uint8_t)BitBase::pack_ext_new(8, &vram_bus.PIN_33_VRAM_D00);

  if (bit(~vram_bus.PIN_49_VRAM_WRn.qp_ext_new())) {
    vid_ram[addr] = data;
  }
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::vram_pins_to_data_bus() {
  /*#p25.RUVY*/ wire _RUVY_CPU_VRAM_WRp = not1(SALE_CPU_VRAM_WRn());
  /*#p25.SAZO*/ wire _SAZO_CBD_TO_VPDp = and2(SERE_CPU_VRAM_RDp(), _RUVY_CPU_VRAM_WRp);

  /*#p25.RYJE*/ wire _RYJE_CBD_TO_VPDn = not1(_SAZO_CBD_TO_VPDp);
  /*#p25.REVO*/ wire _REVO_CBD_TO_VPDp = not1(_RYJE_CBD_TO_VPDn);
  /*#p25.RELA*/ wire _RELA_CBD_TO_VPDp =  or2(_REVO_CBD_TO_VPDp, _SAZO_CBD_TO_VPDp);
  /*#p25.RENA*/ wire _RENA_CBD_TO_VPDn = not1(_RELA_CBD_TO_VPDp);

  /* p25.RODY_VP0_TO_VD0*/ new_bus.BUS_VRAM_D00p.tri6_pn(_RENA_CBD_TO_VPDn, vram_bus.PIN_33_VRAM_D00.qp_new());
  /* p25.REBA_VP1_TO_VD1*/ new_bus.BUS_VRAM_D01p.tri6_pn(_RENA_CBD_TO_VPDn, vram_bus.PIN_31_VRAM_D01.qp_new());
  /* p25.RYDO_VP2_TO_VD2*/ new_bus.BUS_VRAM_D02p.tri6_pn(_RENA_CBD_TO_VPDn, vram_bus.PIN_30_VRAM_D02.qp_new());
  /* p25.REMO_VP3_TO_VD3*/ new_bus.BUS_VRAM_D03p.tri6_pn(_RENA_CBD_TO_VPDn, vram_bus.PIN_29_VRAM_D03.qp_new());
  /* p25.ROCE_VP4_TO_VD4*/ new_bus.BUS_VRAM_D04p.tri6_pn(_RENA_CBD_TO_VPDn, vram_bus.PIN_28_VRAM_D04.qp_new());
  /* p25.ROPU_VP5_TO_VD5*/ new_bus.BUS_VRAM_D05p.tri6_pn(_RENA_CBD_TO_VPDn, vram_bus.PIN_27_VRAM_D05.qp_new());
  /* p25.RETA_VP6_TO_VD6*/ new_bus.BUS_VRAM_D06p.tri6_pn(_RENA_CBD_TO_VPDn, vram_bus.PIN_26_VRAM_D06.qp_new());
  /* p25.RAKU_VP7_TO_VD7*/ new_bus.BUS_VRAM_D07p.tri6_pn(_RENA_CBD_TO_VPDn, vram_bus.PIN_25_VRAM_D07.qp_new());
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::vram_data_bus_to_cpu_bus() {
  /* p25.RERY*/ wire _RERY_VBUS_D0n = not1(new_bus.BUS_VRAM_D00p.qp_new());
  /* p25.RUNA*/ wire _RUNA_VBUS_D1n = not1(new_bus.BUS_VRAM_D01p.qp_new());
  /* p25.RONA*/ wire _RONA_VBUS_D2n = not1(new_bus.BUS_VRAM_D02p.qp_new());
  /* p25.RUNO*/ wire _RUNO_VBUS_D3n = not1(new_bus.BUS_VRAM_D03p.qp_new());
  /* p25.SANA*/ wire _SANA_VBUS_D4n = not1(new_bus.BUS_VRAM_D04p.qp_new());
  /* p25.RORO*/ wire _RORO_VBUS_D5n = not1(new_bus.BUS_VRAM_D05p.qp_new());
  /* p25.RABO*/ wire _RABO_VBUS_D6n = not1(new_bus.BUS_VRAM_D06p.qp_new());
  /* p25.SAME*/ wire _SAME_VBUS_D7n = not1(new_bus.BUS_VRAM_D07p.qp_new());

  /* p25.TYVY*/ wire _TYVY_VBD_TO_CBDn = nand2(SERE_CPU_VRAM_RDp(), LEKO_CPU_RDp());
  /* p25.SEBY*/ wire _SEBY_VBD_TO_CBDp = not1(_TYVY_VBD_TO_CBDn);

  /*#p25.RUGA_VD0_TO_CD0*/ new_bus.BUS_CPU_D00p.tri6_pn(_SEBY_VBD_TO_CBDp, _RERY_VBUS_D0n);
  /* p25.ROTA_VD1_TO_CD1*/ new_bus.BUS_CPU_D01p.tri6_pn(_SEBY_VBD_TO_CBDp, _RUNA_VBUS_D1n);
  /* p25.RYBU_VD2_TO_CD2*/ new_bus.BUS_CPU_D02p.tri6_pn(_SEBY_VBD_TO_CBDp, _RONA_VBUS_D2n);
  /* p25.RAJU_VD3_TO_CD3*/ new_bus.BUS_CPU_D03p.tri6_pn(_SEBY_VBD_TO_CBDp, _RUNO_VBUS_D3n);
  /* p25.TYJA_VD4_TO_CD4*/ new_bus.BUS_CPU_D04p.tri6_pn(_SEBY_VBD_TO_CBDp, _SANA_VBUS_D4n);
  /* p25.REXU_VD5_TO_CD5*/ new_bus.BUS_CPU_D05p.tri6_pn(_SEBY_VBD_TO_CBDp, _RORO_VBUS_D5n);
  /* p25.RUPY_VD6_TO_CD6*/ new_bus.BUS_CPU_D06p.tri6_pn(_SEBY_VBD_TO_CBDp, _RABO_VBUS_D6n);
  /* p25.TOKU_VD7_TO_CD7*/ new_bus.BUS_CPU_D07p.tri6_pn(_SEBY_VBD_TO_CBDp, _SAME_VBUS_D7n);
}

//------------------------------------------------------------------------------------------------------------------------
