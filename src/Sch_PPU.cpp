#include "Sch_PPU.h"

#include "TestGB.h"

using namespace Schematics;

// Die trace
// NUNY04 = and(PYNU04, NOPA16)
// NYFO02 = not(NUNY04)
// MOSU03 = not(NYFO02) // 3 rung inverter
// ABAF02 = not(CATU17)

// BYHA = weirdgate
// BYHA01 << ANEL17
// BYHA02 << ABAF02
// BYHA03 xx CATU17 (crossover?)
// BYHA04 << ABEZ02
// BYHA05 >> ATEJ01

// ATEJ03 = not(BYHA05)
// ANOM03 = nor(ATEJ03, ATAR02)
// BALU02 = not(ANOM03)
// BEBU05 = or(DOBA17, BALU02, BYBA16)
// AVAP03 = not(BEBU05)
// NYXU04 = nor(AVAP03, MOSU03, TEVO05)

// Die trace:
// SAKY = nor(TULY17, VONU17)
// TEPA = not(XYMU)
// TYSO = or(SAKY, TEPA)
// TEXY = not(TYSO)

// WUSA arms on the ground side, nor latch
// WUSA00 << XAJO03
// WUSA01 nc
// WUSA02 >> nc
// WUSA03 >> TOBA00
// WUSA04 nc 
// WUSA05 << WEGO03

// When XAJO03 goes high, WUSA03 goes high.
// When WEGO03 goes high, WUSA03 goes low.

// XEHO01 <> XEHO12
// XEHO02 << SACU04
// XEHO03 <> XEHO09
// XEHO04 nc
// XEHO05 nc
// XEHO06 << XYMU03
// XEHO07 << XEHO16 // so it's a counter
// XEHO08 nc
// XEHO09 <> XEHO03
// XEHO10 nc
// XEHO11 ?? RUMA01
// XEHO12 <> XEHO01
// XEHO13 << XYMU03
// XEHO14 nc
// XEHO15 nc
// XEHO16
// XEHO17 >> XAJO01

// ROXY = NOR latch
// ROXY00 << PAHA
// ROXY01 nc
// ROXY02 >> nc
// ROXY03 >> RONE00
// ROXY04 nc
// ROXY05 << POVA

// If PAHA goes high, ROXY03 goes high.
// If POVA goes high, ROXY03 goes low

// LURY01 << LOVY15 (next to bottom rung)
// LURY02 << XYMU02
// LURY03 nc
// LURY04 >> LONY01

// LONY has "arms" on the VCC side - different from the other latches?
// Probably a NAND latch instead of NOR
// LONY01 << LURY03
// LONY02 nc
// LONY03 == nc
// LONY04 >> LUSU01, MYMA01
// LONY05 nc
// LONY06 << NYXU03

// if LURY goes low, LONY goes low
// if NYXU goes low, LONY goes high

// NOR latch
// POKY00 << PYGO
// POKY01 nc
// POKY02 >> nc
// POKY03 >> ROMO00, others
// POKY04 nc
// POKY05 << LOBY01

// If PYGO goes high, POKY03 goes high
// if LOBY goes high, POKY03 goes low.

// XYMU has "arms" on the ground side
// XYMU00 << WEGO03
// XYMU01 nc
// XYMU02 >> bunch of stuff
// XYMU03 >> nc
// XYMU04 nc
// XYMU05 << AVAP02

// if AVAP02 goes high, XYMU02 goes high.
// if WEGO03 goes high, XYMU02 goes low.

// TAKA has arms on the VCC side - nand latch
// TAKA01 << VEKU02
// TAKA02 nc
// TAKA03 >> nc
// TAKA04 >> SOWO00
// TAKA05 nc
// TAKA06 << SECA03
// if SECA03 goes low, TAKA04 goes high
// if VEKU02 goes low, TAKA04 goes low

//------------------------------------------------------------------------------

PpuSignals PpuRegisters::sig(const TestGB& gb) const {
  PpuSignals ppu_sig;

  auto win_sig = gb.win_reg.sig(gb);

  auto sst_sig = gb.sst_reg.sig(gb, XYMO_LCDC_SPSIZE);
  auto clk_sig = gb.clk_reg.sig(gb);
  auto rst_sig = gb.rst_reg.sig(gb);
  auto dma_sig = gb.dma_reg.sig(gb);
  auto lcd_sig = gb.lcd_reg.sig(gb);
  auto cpu_sig = gb.cpu_bus.sig(gb);
  auto oam_sig = gb.oam_bus.sig();

  auto dbg_sig = gb.dbg_reg.sig(gb);
  auto& vram_pins = gb.vram_pins;

  auto& tile_fetcher = gb.tile_fetcher;
  auto tile_fetcher_sig = tile_fetcher.sig(gb);

  //----------

  ppu_sig.POKY_AFTER_PORCH_LATCHp = POKY_AFTER_PORCH_LATCHp;
  ppu_sig.XYMU_RENDERINGp = XYMU_RENDERINGp;
  ppu_sig.XEHO_X0 = XEHO_X0;
  ppu_sig.SAVY_X1 = SAVY_X1;
  ppu_sig.XODU_X2 = XODU_X2;
  ppu_sig.XYDO_X3 = XYDO_X3;
  ppu_sig.TUHU_X4 = TUHU_X4;
  ppu_sig.TUKY_X5 = TUKY_X5;
  ppu_sig.TAKO_X6 = TAKO_X6;
  ppu_sig.SYBE_X7 = SYBE_X7;

  ppu_sig.TYFO_SFETCH_S0_D1 = TYFO_SFETCH_S0_D1;
  ppu_sig.LONY_BG_READ_VRAM_LATCHp = LONY_BG_READ_VRAM_LATCHp;
  ppu_sig.PORY_BFETCH_DONE_SYNC_DELAY = tile_fetcher.PORY_BFETCH_DONE_SYNC_DELAY;

  {
    /*p21.XUGU*/ wire _XUGU_X_167n = nand(XEHO_X0.q(), SAVY_X1.q(), XODU_X2.q(), TUKY_X5.q(), SYBE_X7.q()); // 128 + 32 + 4 + 2 + 1 = 167
    /*p21.XANO*/ wire _XANO_X_167 = not(_XUGU_X_167n);
    /*p21.XENA*/ wire _XENA_STORE_MATCHn = not(sst_sig.FEPO_STORE_MATCHp);
    /*p21.WODU*/ ppu_sig.WODU_RENDER_DONEp = and (_XENA_STORE_MATCHn, _XANO_X_167);
  }

  {
    /*p27.ROMO*/ wire _ROMO_AFTER_PORCHn = not(POKY_AFTER_PORCH_LATCHp);
    /*p27.SUVU*/ wire _SUVU_PORCH_ENDn = nand(XYMU_RENDERINGp, _ROMO_AFTER_PORCHn, tile_fetcher.NYKA_BFETCH_DONE_SYNC, tile_fetcher.PORY_BFETCH_DONE_SYNC_DELAY);
    /*p27.TAVE*/ ppu_sig.TAVE_PORCH_ENDp = not(_SUVU_PORCH_ENDn);
  }

#if HAX
  /*p27.TAVE*/ wire _TAVE_PORCH_ENDp = and(XYMU_RENDERINGp, !POKY_AFTER_PORCH_LATCHp, NYKA_BFETCH_DONE_SYNC, PORY_BFETCH_DONE_SYNC_DELAY;
#endif

  /*p27.TEVO*/ ppu_sig.TEVO_CLK_STOPn = nor(win_sig.SEKO_WIN_TRIGGER, win_sig.SUZU, ppu_sig.TAVE_PORCH_ENDp);

  /*p27.NYXU*/ ppu_sig.NYXU_BFETCH_RSTn = nor(sst_sig.AVAP_SCAN_DONE_d0_TRIGp, win_sig.MOSU_WIN_MODE_TRIGp, ppu_sig.TEVO_CLK_STOPn);

  /*p28.ACYL*/ ppu_sig.ACYL_PPU_USE_OAM1p = and (dma_sig.BOGE_DMA_RUNNINGn, sst_sig.BESU_SCANNINGp);

  // So we only read a sprite if both those regs are... low? what is rung 17's polarity?

  {
    /*p29.SAKY*/ wire _SAKY = nor(TULY_SFETCH_S1.q(), VONU_SFETCH_S1_D4.q());
    /*p29.TEPA*/ ppu_sig.TEPA_RENDERINGn = not(XYMU_RENDERINGp);
    /*p29.TYSO*/ wire _TYSO_SPRITE_READn = or(_SAKY, ppu_sig.TEPA_RENDERINGn);
    /*p29.TEXY*/ ppu_sig.TEXY_SPRITE_READp = not(_TYSO_SPRITE_READn);
  }

  {
    /*p29.TYNO*/ wire _TYNO = nand(TOXE_SFETCH_S0.q(), SEBA_SFETCH_S1_D5.q(), VONU_SFETCH_S1_D4.q());
    /*p29.VUSA*/ wire _VUSA = or(!TYFO_SFETCH_S0_D1.q(), _TYNO);
    /*p29.WUTY*/ ppu_sig.WUTY_SPRITE_DONE = not(_VUSA);
  }

  {
    /*p24.VYBO*/ wire _VYBO_PIX_CLK_AxCxExGx = nor(sst_sig.FEPO_STORE_MATCHp, ppu_sig.WODU_RENDER_DONEp, clk_sig.MYVO_AxCxExGx);
    /*p24.TYFA*/ wire _TYFA_AxCxExGx = and (win_sig.SOCY_WIN_HITn, POKY_AFTER_PORCH_LATCHp, _VYBO_PIX_CLK_AxCxExGx);
    /*p24.SEGU*/ ppu_sig.SEGU_xBxDxFxH = not(_TYFA_AxCxExGx);
    /*p24.ROXO*/ ppu_sig.ROXO_AxCxExGx = not(ppu_sig.SEGU_xBxDxFxH);
    /*p27.ROCO*/ ppu_sig.ROCO_AxCxExGx = not(ppu_sig.SEGU_xBxDxFxH);
    /*p24.SACU*/ ppu_sig.SACU_CLKPIPE_AxCxExGx = nor(ppu_sig.SEGU_xBxDxFxH, ROXY_FINE_MATCH_LATCHn);
  }

  /*p24.LOBY*/ ppu_sig.LOBY_RENDERINGn = not(XYMU_RENDERINGp.q());
  /*p25.ROPY*/ ppu_sig.ROPY_RENDERINGn = not(XYMU_RENDERINGp.q());

  /*p27.ROZE*/ ppu_sig.ROZE_FINE_COUNT_STOPn = nand(RYKU_FINE_CNT0, ROGA_FINE_CNT1, RUBU_FINE_CNT2);

  {
    /*p25.TUCA*/ wire _TUCA_CPU_VRAM_RD = and (cpu_sig.SOSE_8000_9FFFp, dbg_sig.ABUZ);
    /*p25.TEFY*/ wire _TEFY_MCSn_Cn = not(vram_pins.PIN_MCSn_C);
    /*p25.TOLE*/ wire _TOLE_VRAM_RD = mux2_p(_TEFY_MCSn_Cn, _TUCA_CPU_VRAM_RD, dbg_sig.TUTO_DBG_VRAMp);
    /*p25.SERE*/ ppu_sig.SERE_VRAM_RD = and (_TOLE_VRAM_RD, ppu_sig.ROPY_RENDERINGn);
  }

  {
    /*p29.TYTU*/ ppu_sig.TYTU_SFETCH_S0_D0n = not(TOXE_SFETCH_S0.q());
    /*p29.TACU*/ ppu_sig.TACU_SPR_SEQ_5_TRIG = nand(TYFO_SFETCH_S0_D1.q(), ppu_sig.TYTU_SFETCH_S0_D0n);
  }

  {
    /*p21.TAPA*/ wire TAPA_INT_OAM = and (lcd_sig.TOLU_VBLANKn, lcd_sig.SELA_NEW_LINE_d0p);
    /*p21.TARU*/ wire TARU_INT_HBL = and (lcd_sig.TOLU_VBLANKn, ppu_sig.WODU_RENDER_DONEp);
    /*p21.SUKO*/ wire SUKO_INT_STATb = amux4(RUGU_INT_LYC_EN, lcd_sig.ROPO_LY_MATCH_SYNCp,
                                             REFE_INT_OAM_EN, TAPA_INT_OAM,
                                             RUFO_INT_VBL_EN, lcd_sig.PARU_VBLANKp, // polarity?
                                             ROXE_INT_HBL_EN, TARU_INT_HBL);
    /*p21.TUVA*/ wire TUVA_INT_STATn = not(SUKO_INT_STATb);
    /*p21.VOTY*/ ppu_sig.VOTY_INT_STATp = not(TUVA_INT_STATn);
  }

  /*p29.TUVO*/ ppu_sig.TUVO_PPU_OAM_RDp = nor(ppu_sig.TEPA_RENDERINGn, TULY_SFETCH_S1.q(), TESE_SFETCH_S2.q());

  {
    /*p29.SYCU*/ wire SYCU = nor(ppu_sig.TYTU_SFETCH_S0_D0n, ppu_sig.LOBY_RENDERINGn, TYFO_SFETCH_S0_D1);
    /*p29.TOPU*/ ppu_sig.TOPU_LATCH_SPPIXA = and (TULY_SFETCH_S1, SYCU);
    /*p29.RACA*/ ppu_sig.RACA_LATCH_SPPIXB = and (VONU_SFETCH_S1_D4, SYCU);
    /*p29.XONO*/ ppu_sig.XONO_FLIP_X = and (oam_sig.BAXO_SPRITE_X5, ppu_sig.TEXY_SPRITE_READp);
  }

  {
    /*p23.VYXE*/ ppu_sig.VYXE_LCDC_BGEN    = VYXE_LCDC_BGEN;
    /*p23.XYLO*/ ppu_sig.XYLO_LCDC_SPEN    = XYLO_LCDC_SPEN;
    /*p23.XYMO*/ ppu_sig.XYMO_LCDC_SPSIZE  = XYMO_LCDC_SPSIZE;
    /*p23.XAFO*/ ppu_sig.XAFO_LCDC_BGMAP   = XAFO_LCDC_BGMAP;
    /*p23.WEXU*/ ppu_sig.WEXU_LCDC_BGTILE  = WEXU_LCDC_BGTILE;
    /*p23.WYMO*/ ppu_sig.WYMO_LCDC_WINEN   = WYMO_LCDC_WINEN;
    /*p23.WOKY*/ ppu_sig.WOKY_LCDC_WINMAP  = WOKY_LCDC_WINMAP;
    /*p23.XONA*/ ppu_sig.XONA_LCDC_EN      = XONA_LCDC_EN;
  }

  return ppu_sig;
}

//------------------------------------------------------------------------------

void PpuRegisters::tick(TestGB& gb) {
  auto cpu_sig = gb.cpu_bus.sig(gb);
  auto clk_sig = gb.clk_reg.sig(gb);
  auto rst_sig = gb.rst_reg.sig(gb);
  auto win_sig = gb.win_reg.sig(gb);
  auto lcd_sig = gb.lcd_reg.sig(gb);
  auto dbg_sig = gb.dbg_reg.sig(gb);
  auto sst_sig = gb.sst_reg.sig(gb, XYMO_LCDC_SPSIZE);
  auto ppu_sig = sig(gb);
  auto scr_sig = gb.scr_reg.sig(gb);
  auto oam_sig = gb.oam_bus.sig();
  auto tile_fetch_sig = gb.tile_fetcher.sig(gb);

  auto& cpu_bus = gb.cpu_bus;
  auto& vram_bus = gb.vram_bus;
  
  auto& tile_fetcher = gb.tile_fetcher;
  auto tile_fetcher_sig = tile_fetcher.sig(gb);

  wire P10_B = 0;

  //----------

  // The first tile generated is thrown away. I'm calling that section of rendering the front porch.

  {
    /*p24.PYGO*/ PYGO_TILE_DONE.set(clk_sig.ALET_xBxDxFxH, XYMU_RENDERINGp, tile_fetcher.PORY_BFETCH_DONE_SYNC_DELAY);
  }

  {
    /*p24.POKY*/ POKY_AFTER_PORCH_LATCHp.nor_latch(PYGO_TILE_DONE, ppu_sig.LOBY_RENDERINGn);
  }


  {
    /*p21.WEGO*/ wire _WEGO_RST_LATCH = or(rst_sig.TOFU_VID_RSTp, VOGA_RENDER_DONE_SYNC);
    /*p27.POVA*/ wire _POVA_FINE_MATCH_SETp = and (PUXA_FINE_MATCH_SYNC1, !NYZE_FINE_MATCH_SYNC2);

    /*p21.XYMU*/ XYMU_RENDERINGp.nor_latch(sst_sig.AVAP_SCAN_DONE_d0_TRIGp, _WEGO_RST_LATCH);

    {
      // XAJO04 = and(XEHO17, XYDO17);
      /*p21.XAJO*/ wire _XAJO_X_009 = and (XEHO_X0.q(), XYDO_X3.q());
      /*p21.WUSA*/ WUSA_CPEN_LATCH.nor_latch(_XAJO_X_009, _WEGO_RST_LATCH);
      /*p21.TOBA*/ wire _TOBA = and (ppu_sig.SACU_CLKPIPE_AxCxExGx, WUSA_CPEN_LATCH);
      /*p21.SEMU*/ wire _SEMU_LCD_CPn = or(_TOBA, _POVA_FINE_MATCH_SETp);
      /*p21.RYPO*/ wire _RYPO_LCD_CP = not(_SEMU_LCD_CPn);
      CP.set(_RYPO_LCD_CP);
    }

    {
      /*p27.PAHA*/ wire _PAHA_FINE_MATCH_RSTp = not(XYMU_RENDERINGp);
      /*p27.ROXY*/ ROXY_FINE_MATCH_LATCHn.nor_latch(_PAHA_FINE_MATCH_RSTp, _POVA_FINE_MATCH_SETp);
    }

    {
      /*p27.PECU*/ wire _PECU_FINE_CLK = nand(ppu_sig.ROXO_AxCxExGx, ppu_sig.ROZE_FINE_COUNT_STOPn);
      /*p27.PASO*/ wire _PASO_FINE_RST = nor(ppu_sig.TEVO_CLK_STOPn, ppu_sig.ROPY_RENDERINGn);
      /*p27.RYKU*/ RYKU_FINE_CNT0.set(_PECU_FINE_CLK, _PASO_FINE_RST, !RYKU_FINE_CNT0);
      /*p27.ROGA*/ ROGA_FINE_CNT1.set(!RYKU_FINE_CNT0, _PASO_FINE_RST, !ROGA_FINE_CNT1);
      /*p27.RUBU*/ RUBU_FINE_CNT2.set(!ROGA_FINE_CNT1, _PASO_FINE_RST, !RUBU_FINE_CNT2);
    }

    {
      /*p27.SUHA*/ wire _SUHA_FINE_MATCH0 = xnor(scr_sig.DATY_SCX0, RYKU_FINE_CNT0); // Arms on the ground side, XNOR
      /*p27.SYBY*/ wire _SYBY_FINE_MATCH1 = xnor(scr_sig.DUZU_SCX1, ROGA_FINE_CNT1);
      /*p27.SOZU*/ wire _SOZU_FINE_MATCH2 = xnor(scr_sig.CYXU_SCX2, RUBU_FINE_CNT2);

      /*p27.RONE*/ wire _RONE_FINE_MATCHn = nand(ROXY_FINE_MATCH_LATCHn, _SUHA_FINE_MATCH0, _SYBY_FINE_MATCH1, _SOZU_FINE_MATCH2);
      /*p27.POHU*/ wire _POHU_FINE_MATCH = not(_RONE_FINE_MATCHn);
      /*p27.MOXE*/ wire _MOXE_AxCxExGx = not(clk_sig.ALET_xBxDxFxH);
      /*p27.PUXA*/ PUXA_FINE_MATCH_SYNC1.set(ppu_sig.ROXO_AxCxExGx, XYMU_RENDERINGp, _POHU_FINE_MATCH);
      /*p27.NYZE*/ NYZE_FINE_MATCH_SYNC2.set(_MOXE_AxCxExGx, XYMU_RENDERINGp, PUXA_FINE_MATCH_SYNC1);
    }
  }

  //----------


  {
    // this seems reasonable. BG_READ_VRAM_LATCHp is bracketed by BFETCH_RSTn (start) and
    // vid_reg.LOVY_BG_SEQ5_SYNC (stop).

    /*p27.LURY*/ wire _LURY_BG_READ_VRAM_LATCH_RSTn = and(!LOVY_BG_SEQ5_SYNC, XYMU_RENDERINGp);
    /*p27.LONY*/ LONY_BG_READ_VRAM_LATCHp.nand_latch(ppu_sig.NYXU_BFETCH_RSTn, _LURY_BG_READ_VRAM_LATCH_RSTn);
  }


  {
    /*p27.MOCE*/ wire _MOCE_BFETCH_DONEn = nand(tile_fetcher.LAXU_BFETCH_S0, tile_fetcher.NYVA_BFETCH_S2, ppu_sig.NYXU_BFETCH_RSTn);
    /*p27.LYRY*/ wire _LYRY_BFETCH_DONEp = not(_MOCE_BFETCH_DONEn);
    /*p27.LOVY*/ LOVY_BG_SEQ5_SYNC.set(clk_sig.MYVO_AxCxExGx, ppu_sig.NYXU_BFETCH_RSTn, _LYRY_BFETCH_DONEp);

    /*p24.NAFY*/ wire _NAFY_RENDERING_AND_NOT_WIN_TRIG = nor(win_sig.MOSU_WIN_MODE_TRIGp, ppu_sig.LOBY_RENDERINGn);

    /*p27.LYZU*/ tile_fetcher.LYZU_BFETCH_S0_DELAY.set       (clk_sig.ALET_xBxDxFxH, ppu_sig.XYMU_RENDERINGp,          tile_fetcher.LAXU_BFETCH_S0);
    /*p24.NYKA*/ tile_fetcher.NYKA_BFETCH_DONE_SYNC.set      (clk_sig.ALET_xBxDxFxH, _NAFY_RENDERING_AND_NOT_WIN_TRIG, _LYRY_BFETCH_DONEp);
    /*p24.PORY*/ tile_fetcher.PORY_BFETCH_DONE_SYNC_DELAY.set(clk_sig.MYVO_AxCxExGx, _NAFY_RENDERING_AND_NOT_WIN_TRIG, tile_fetcher.NYKA_BFETCH_DONE_SYNC);
  }


  //----------------------------------------
  // So this is def the chunk that watches FEPO_STORE_MATCHp and triggers a sprite fetch...

  // Maybe we should annotate phase starting with the phase 0 = FEPO_MATCH_SYNC goes high?

  {
    /*p27.MOCE*/ wire _MOCE_BFETCH_DONEn = nand(tile_fetcher.LAXU_BFETCH_S0, tile_fetcher.NYVA_BFETCH_S2, ppu_sig.NYXU_BFETCH_RSTn);
    /*p27.LYRY*/ wire _LYRY_BFETCH_DONEp = not(_MOCE_BFETCH_DONEn);
    /*p27.SOWO*/ wire _SOWO_SPRITE_FETCH_LATCHn = not(TAKA_SFETCH_RUN_LATCH);
    /*p27.TEKY*/ wire _TEKY_SPRITE_FETCH = and (sst_sig.FEPO_STORE_MATCHp, win_sig.TUKU_WIN_HITn, _LYRY_BFETCH_DONEp, _SOWO_SPRITE_FETCH_LATCHn);
    /*p27.SOBU*/ SOBU_SPRITE_FETCH_SYNC1.set(clk_sig.TAVA_xBxDxFxH, dbg_sig.VYPO_P10_Bn, _TEKY_SPRITE_FETCH);
    /*p27.SUDA*/ SUDA_SPRITE_FETCH_SYNC2.set(clk_sig.LAPE_AxCxExGx, dbg_sig.VYPO_P10_Bn, SOBU_SPRITE_FETCH_SYNC1);

    /*p27.RYCE*/ wire _RYCE_SPRITE_FETCH_TRIG = and (SOBU_SPRITE_FETCH_SYNC1, !SUDA_SPRITE_FETCH_SYNC2);

    // byha here seems wrong polarity
    /*p27.SECA*/ wire _SECA_SFETCH_SETn = nor(_RYCE_SPRITE_FETCH_TRIG, rst_sig.ROSY_VID_RSTp, lcd_sig.BYHA_VID_LINE_TRIG_d4n); // def nor

    /*p27.VEKU*/ wire _VEKU_SFETCH_RSTn = nor(ppu_sig.WUTY_SPRITE_DONE, ppu_sig.TAVE_PORCH_ENDp); // def nor

    /*p29.TAME*/ wire _TAME_SFETCH_101n = nand(TESE_SFETCH_S2, TOXE_SFETCH_S0);
    /*p29.TOMA*/ wire _TOMA_xBxDxFxH = nand(clk_sig.LAPE_AxCxExGx, _TAME_SFETCH_101n);

    /*p27.TAKA*/ TAKA_SFETCH_RUN_LATCH.nand_latch(_SECA_SFETCH_SETn, _VEKU_SFETCH_RSTn);
    /*p29.TOXE*/ TOXE_SFETCH_S0.set(_TOMA_xBxDxFxH,  _SECA_SFETCH_SETn, !TOXE_SFETCH_S0);
    /*p29.TULY*/ TULY_SFETCH_S1.set(!TOXE_SFETCH_S0, _SECA_SFETCH_SETn, !TULY_SFETCH_S1);
    /*p29.TESE*/ TESE_SFETCH_S2.set(!TULY_SFETCH_S1, _SECA_SFETCH_SETn, !TESE_SFETCH_S2);

    /*p29.TYFO*/ TYFO_SFETCH_S0_D1.set(clk_sig.LAPE_AxCxExGx, dbg_sig.VYPO_P10_Bn, TOXE_SFETCH_S0);
    /*p29.TOBU*/ TOBU_SFETCH_S1_D2.set(clk_sig.TAVA_xBxDxFxH, XYMU_RENDERINGp, TULY_SFETCH_S1);    // note input is seq 1 not 2

    /*p29.VONU*/ VONU_SFETCH_S1_D4.set(clk_sig.TAVA_xBxDxFxH, XYMU_RENDERINGp, TOBU_SFETCH_S1_D2);
    /*p29.SEBA*/ SEBA_SFETCH_S1_D5.set(clk_sig.LAPE_AxCxExGx, XYMU_RENDERINGp, VONU_SFETCH_S1_D4); // is this clock wrong?
  }


  //----------

  {
    // vid x position, has carry lookahead because this increments every tcycle
    /*p21.RYBO*/ wire RYBO = xor(XEHO_X0, SAVY_X1);
    /*p21.XUKE*/ wire XUKE = and(XEHO_X0, SAVY_X1);

    /*p21.XYLE*/ wire XYLE = and(XODU_X2, XUKE);
    /*p21.XEGY*/ wire XEGY = xor(XODU_X2, XUKE);
    /*p21.XORA*/ wire XORA = xor(XYDO_X3, XYLE);

    /*p21.SAKE*/ wire SAKE = xor(TUHU_X4, TUKY_X5);
    /*p21.TYBA*/ wire TYBA = and(TUHU_X4, TUKY_X5);
    /*p21.SURY*/ wire SURY = and(TAKO_X6, TYBA);
    /*p21.TYGE*/ wire TYGE = xor(TAKO_X6, TYBA);
    /*p21.ROKU*/ wire ROKU = xor(SYBE_X7, SURY);

    /*p21.TADY*/ wire TADY_X_RSTn = nor(lcd_sig.BYHA_VID_LINE_TRIG_d4n, rst_sig.TOFU_VID_RSTp);

    /*p24.TOCA*/ wire TOCA_CLKPIPE_HI = not(XYDO_X3);

    /*p21.XEHO*/ XEHO_X0.set(ppu_sig.SACU_CLKPIPE_AxCxExGx, TADY_X_RSTn, !XEHO_X0);
    /*p21.SAVY*/ SAVY_X1.set(ppu_sig.SACU_CLKPIPE_AxCxExGx, TADY_X_RSTn, RYBO);
    /*p21.XODU*/ XODU_X2.set(ppu_sig.SACU_CLKPIPE_AxCxExGx, TADY_X_RSTn, XEGY);
    /*p21.XYDO*/ XYDO_X3.set(ppu_sig.SACU_CLKPIPE_AxCxExGx, TADY_X_RSTn, XORA);
    /*p21.TUHU*/ TUHU_X4.set(TOCA_CLKPIPE_HI, TADY_X_RSTn, !TUHU_X4);
    /*p21.TUKY*/ TUKY_X5.set(TOCA_CLKPIPE_HI, TADY_X_RSTn, SAKE);
    /*p21.TAKO*/ TAKO_X6.set(TOCA_CLKPIPE_HI, TADY_X_RSTn, TYGE);
    /*p21.SYBE*/ SYBE_X7.set(TOCA_CLKPIPE_HI, TADY_X_RSTn, ROKU);
  }

  {
    /*p21.TADY*/ wire TADY_X_RST = nor(lcd_sig.BYHA_VID_LINE_TRIG_d4n, rst_sig.TOFU_VID_RSTp);
    // having this reset connected to both RENDER_DONE_SYNC and x seems odd
    /*p21.VOGA*/ VOGA_RENDER_DONE_SYNC.set(clk_sig.ALET_xBxDxFxH, TADY_X_RST, ppu_sig.WODU_RENDER_DONEp);
  }

  {
    // LCD horizontal sync pin latch
    // if AVAP goes high, POFY goes high.
    // if PAHO or TOFU go high, POFY goes low.

    /*p24.PAHO*/ PAHO_X_8_SYNC.set(ppu_sig.ROXO_AxCxExGx, ppu_sig.XYMU_RENDERINGp, XYDO_X3);
    /*p24.RUJU*/ POFY_ST_LATCH.nor_latch(sst_sig.AVAP_SCAN_DONE_d0_TRIGp, PAHO_X_8_SYNC || rst_sig.TOFU_VID_RSTp);
    /*p24.RUZE*/ wire _RUZE_PIN_ST = not(POFY_ST_LATCH);
    ST.set(_RUZE_PIN_ST);
  }

  /*p27.LUSU*/ wire LUSU_BGW_VRAM_RDn = not(LONY_BG_READ_VRAM_LATCHp.q());
  /*p27.LENA*/ wire LENA_BGW_VRAM_RD = not(LUSU_BGW_VRAM_RDn);

  {
    /*p27.NOGU*/ wire _NOGU_FETCH_01p = nand(tile_fetcher_sig.NAKO_FETCH_S1n, tile_fetcher_sig.NOFU_FETCH_S2n);
    /*p27.NENY*/ wire _NENY_FETCH_01n = not(_NOGU_FETCH_01p);
    /*p26.AXAD*/ wire _AXAD_WIN_MODEn = not(win_sig.PORE_WIN_MODE);

    {
      /*p27.POTU*/ wire _POTU_BGW_FETCH_01 = and (LENA_BGW_VRAM_RD, _NENY_FETCH_01n);

      // Background map read
      /*p26.ACEN*/ wire _ACEN_BG_MAP_READp = and (_POTU_BGW_FETCH_01, _AXAD_WIN_MODEn);
      /*p26.BAFY*/ wire _BAFY_BG_MAP_READn = not(_ACEN_BG_MAP_READp);
      /*p26.AXEP*/ vram_bus.TRI_A00.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.BABE_MAP_X0S);
      /*p26.AFEB*/ vram_bus.TRI_A01.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.ABOD_MAP_X1S);
      /*p26.ALEL*/ vram_bus.TRI_A02.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.BEWY_MAP_X2S);
      /*p26.COLY*/ vram_bus.TRI_A03.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.BYCA_MAP_X3S);
      /*p26.AJAN*/ vram_bus.TRI_A04.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.ACUL_MAP_X4S);
      /*p26.DUHO*/ vram_bus.TRI_A05.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.ETAM_MAP_Y0S);
      /*p26.CASE*/ vram_bus.TRI_A06.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.DOTO_MAP_Y1S);
      /*p26.CYPO*/ vram_bus.TRI_A07.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.DABA_MAP_Y2S);
      /*p26.CETA*/ vram_bus.TRI_A08.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.EFYK_MAP_Y3S);
      /*p26.DAFE*/ vram_bus.TRI_A09.set_tribuf(_BAFY_BG_MAP_READn, scr_sig.EJOK_MAP_Y4S);
      /*p26.AMUV*/ vram_bus.TRI_A10.set_tribuf(_BAFY_BG_MAP_READn, XAFO_LCDC_BGMAP);
      /*p26.COVE*/ vram_bus.TRI_A11.set_tribuf(_BAFY_BG_MAP_READn, dbg_sig.VYPO_P10_Bn);
      /*p26.COXO*/ vram_bus.TRI_A12.set_tribuf(_BAFY_BG_MAP_READn, dbg_sig.VYPO_P10_Bn);

      // Window map read
      /*p25.XEZE*/ wire _XEZE_WIN_MAP_READp = and (_POTU_BGW_FETCH_01, win_sig.PORE_WIN_MODE);
      /*p25.WUKO*/ wire _WUKO_WIN_MAP_READn = not(_XEZE_WIN_MAP_READp);
      /*p27.XEJA*/ vram_bus.TRI_A00.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_X3);
      /*p27.XAMO*/ vram_bus.TRI_A01.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_X4);
      /*p27.XAHE*/ vram_bus.TRI_A02.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_X5);
      /*p27.XULO*/ vram_bus.TRI_A03.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_X6);
      /*p27.WUJU*/ vram_bus.TRI_A04.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_X7);
      /*p27.VYTO*/ vram_bus.TRI_A05.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_Y3);
      /*p27.VEHA*/ vram_bus.TRI_A06.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_Y4);
      /*p27.VACE*/ vram_bus.TRI_A07.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_Y5);
      /*p27.VOVO*/ vram_bus.TRI_A08.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_Y6);
      /*p27.VULO*/ vram_bus.TRI_A09.set_tribuf(_WUKO_WIN_MAP_READn, win_sig.WIN_Y7);
      /*p27.VEVY*/ vram_bus.TRI_A10.set_tribuf(_WUKO_WIN_MAP_READn, WOKY_LCDC_WINMAP);
      /*p27.VEZA*/ vram_bus.TRI_A11.set_tribuf(_WUKO_WIN_MAP_READn, dbg_sig.VYPO_P10_Bn);
      /*p27.VOGU*/ vram_bus.TRI_A12.set_tribuf(_WUKO_WIN_MAP_READn, dbg_sig.VYPO_P10_Bn);
    }

    {
      // Background/window tile read
      /*p27.XUHA*/ wire _XUHA_FETCH_S2p = not(tile_fetcher_sig.NOFU_FETCH_S2n);
      /*p27.NETA*/ wire _NETA_TILE_READn = and (LENA_BGW_VRAM_RD, _NENY_FETCH_01n);
      /*p26.ASUL*/ wire _ASUL_TILE_READp = and (_NETA_TILE_READn, _AXAD_WIN_MODEn);
      /*p26.BEJE*/ wire _BEJE_TILE_READn = not(_ASUL_TILE_READp);
      /*p25.XUCY*/ wire _XUCY_TILE_READn = nand(_NETA_TILE_READn, win_sig.PORE_WIN_MODE);

      /*p26.ASUM*/ vram_bus.TRI_A00.set_tribuf(_BEJE_TILE_READn, _XUHA_FETCH_S2p);
      /*p26.EVAD*/ vram_bus.TRI_A01.set_tribuf(_BEJE_TILE_READn, scr_sig.FAFO_TILE_Y0S);
      /*p26.DAHU*/ vram_bus.TRI_A02.set_tribuf(_BEJE_TILE_READn, scr_sig.EMUX_TILE_Y1S);
      /*p26.DODE*/ vram_bus.TRI_A03.set_tribuf(_BEJE_TILE_READn, scr_sig.ECAB_TILE_Y2S);

      /*p25.XONU*/ vram_bus.TRI_A00.set_tribuf(_XUCY_TILE_READn, _XUHA_FETCH_S2p);
      /*p25.WUDO*/ vram_bus.TRI_A01.set_tribuf(_XUCY_TILE_READn, win_sig.WIN_Y0);
      /*p25.WAWE*/ vram_bus.TRI_A02.set_tribuf(_XUCY_TILE_READn, win_sig.WIN_Y1);
      /*p25.WOLU*/ vram_bus.TRI_A03.set_tribuf(_XUCY_TILE_READn, win_sig.WIN_Y2);

      /*p25.VAPY*/ vram_bus.TRI_A04.set_tribuf(_NETA_TILE_READn, tile_fetch_sig.BG_PIX_B0);
      /*p25.SEZU*/ vram_bus.TRI_A05.set_tribuf(_NETA_TILE_READn, tile_fetch_sig.BG_PIX_B1);
      /*p25.VEJY*/ vram_bus.TRI_A06.set_tribuf(_NETA_TILE_READn, tile_fetch_sig.BG_PIX_B2);
      /*p25.RUSA*/ vram_bus.TRI_A07.set_tribuf(_NETA_TILE_READn, tile_fetch_sig.BG_PIX_B3);
      /*p25.ROHA*/ vram_bus.TRI_A08.set_tribuf(_NETA_TILE_READn, tile_fetch_sig.BG_PIX_B4);
      /*p25.RESO*/ vram_bus.TRI_A09.set_tribuf(_NETA_TILE_READn, tile_fetch_sig.BG_PIX_B5);
      /*p25.SUVO*/ vram_bus.TRI_A10.set_tribuf(_NETA_TILE_READn, tile_fetch_sig.BG_PIX_B6);
      /*p25.TOBO*/ vram_bus.TRI_A11.set_tribuf(_NETA_TILE_READn, tile_fetch_sig.BG_PIX_B7);

      /*p25.VUZA*/ wire _VUZA_TILE_BANKp = nor(WEXU_LCDC_BGTILE, tile_fetch_sig.BG_PIX_B7); // register reused
      /*p25.VURY*/ vram_bus.TRI_A12.set_tribuf(_NETA_TILE_READn, _VUZA_TILE_BANKp);
    }
  }

  {
    /*p29.FUFO*/ wire _FUFO_LCDC_SPSIZEn = not(ppu_sig.XYMO_LCDC_SPSIZE);
    /*p29.WUKY*/ wire _WUKY_FLIP_Y = not(oam_sig.YZOS_SPRITE_X6);

    /*p29.XUQU*/ wire _XUQU_SPRITE_AB = not(!VONU_SFETCH_S1_D4.q());
    /*p29.CYVU*/ wire _CYVU_SPRITE_Y0 = xor (_WUKY_FLIP_Y, sst_sig.CUCU_TS_LINE_1);
    /*p29.BORE*/ wire _BORE_SPRITE_Y1 = xor (_WUKY_FLIP_Y, sst_sig.CUCA_TS_LINE_2);
    /*p29.BUVY*/ wire _BUVY_SPRITE_Y2 = xor (_WUKY_FLIP_Y, sst_sig.CEGA_TS_LINE_3);

    /*p29.WAGO*/ wire _WAGO = xor (_WUKY_FLIP_Y, sst_sig.WENU_TS_LINE_0);
    /*p29.GEJY*/ wire _GEJY_SPRITE_Y3 = amux2(_FUFO_LCDC_SPSIZEn, !oam_sig.XUSO_SPRITE_Y0, ppu_sig.XYMO_LCDC_SPSIZE, _WAGO);

    /*p29.ABON*/ wire ABON_SPRITE_READn = not(ppu_sig.TEXY_SPRITE_READp);

    /*p29.ABEM*/ vram_bus.TRI_A00.set_tribuf(ABON_SPRITE_READn, _XUQU_SPRITE_AB);
    /*p29.BAXE*/ vram_bus.TRI_A01.set_tribuf(ABON_SPRITE_READn, _CYVU_SPRITE_Y0);
    /*p29.ARAS*/ vram_bus.TRI_A02.set_tribuf(ABON_SPRITE_READn, _BORE_SPRITE_Y1);
    /*p29.AGAG*/ vram_bus.TRI_A03.set_tribuf(ABON_SPRITE_READn, _BUVY_SPRITE_Y2);
    /*p29.FAMU*/ vram_bus.TRI_A04.set_tribuf(ABON_SPRITE_READn, _GEJY_SPRITE_Y3);
    /*p29.FUGY*/ vram_bus.TRI_A05.set_tribuf(ABON_SPRITE_READn, oam_sig.XEGU_SPRITE_Y1);
    /*p29.GAVO*/ vram_bus.TRI_A06.set_tribuf(ABON_SPRITE_READn, oam_sig.YJEX_SPRITE_Y2);
    /*p29.WYGA*/ vram_bus.TRI_A07.set_tribuf(ABON_SPRITE_READn, oam_sig.XYJU_SPRITE_Y3);
    /*p29.WUNE*/ vram_bus.TRI_A08.set_tribuf(ABON_SPRITE_READn, oam_sig.YBOG_SPRITE_Y4);
    /*p29.GOTU*/ vram_bus.TRI_A09.set_tribuf(ABON_SPRITE_READn, oam_sig.WYSO_SPRITE_Y5);
    /*p29.GEGU*/ vram_bus.TRI_A10.set_tribuf(ABON_SPRITE_READn, oam_sig.XOTE_SPRITE_Y6);
    /*p29.XEHE*/ vram_bus.TRI_A11.set_tribuf(ABON_SPRITE_READn, oam_sig.YZAB_SPRITE_Y7);
    /*p29.DYSO*/ vram_bus.TRI_A12.set_tribuf(ABON_SPRITE_READn, P10_B);   // sprites always in low half of tile store
  }

  //----------------------------------------
  // Config registers

  // FF40 LCDC
  {
    /*p22.WORU*/ wire _WORU_FF40n = nand(cpu_sig.WERO_FF40_FF4Fp, cpu_sig.XOLA_A00n, cpu_sig.XENO_A01n, cpu_sig.XUSY_A02n, cpu_sig.XERA_A03n);
    /*p22.VOCA*/ wire _VOCA_FF40p = not(_WORU_FF40n);

    /*p23.VYRE*/ wire _VYRE_FF40_RDp = and (_VOCA_FF40p, cpu_sig.ASOT_CPU_RD);
    /*p23.WYCE*/ wire _WYCE_FF40_RDn = not(_VYRE_FF40_RDp);

    /*p23.WARU*/ wire _WARU_FF40_WRp = and (_VOCA_FF40p, cpu_sig.CUPA_CPU_WR_xxxxxFGH);
    /*p23.XUBO*/ wire _XUBO_FF40_WRn = not(_WARU_FF40_WRp);

    /*p23.WYPO*/ cpu_bus.TRI_D0.set_tribuf(!_WYCE_FF40_RDn, VYXE_LCDC_BGEN);
    /*p23.XERO*/ cpu_bus.TRI_D1.set_tribuf(!_WYCE_FF40_RDn, XYLO_LCDC_SPEN);
    /*p23.WYJU*/ cpu_bus.TRI_D2.set_tribuf(!_WYCE_FF40_RDn, XYMO_LCDC_SPSIZE);
    /*p23.WUKA*/ cpu_bus.TRI_D3.set_tribuf(!_WYCE_FF40_RDn, XAFO_LCDC_BGMAP);
    /*p23.VOKE*/ cpu_bus.TRI_D4.set_tribuf(!_WYCE_FF40_RDn, WEXU_LCDC_BGTILE);
    /*p23.VATO*/ cpu_bus.TRI_D5.set_tribuf(!_WYCE_FF40_RDn, WYMO_LCDC_WINEN);
    /*p23.VAHA*/ cpu_bus.TRI_D6.set_tribuf(!_WYCE_FF40_RDn, WOKY_LCDC_WINMAP);
    /*p23.XEBU*/ cpu_bus.TRI_D7.set_tribuf(!_WYCE_FF40_RDn, XONA_LCDC_EN);

    /*p01.XARE*/ wire _XARE_RESETn = not(rst_sig.XORE_RSTp);
    /*p23.VYXE*/ VYXE_LCDC_BGEN   .set(_XUBO_FF40_WRn, _XARE_RESETn, cpu_bus.TRI_D0);
    /*p23.XYLO*/ XYLO_LCDC_SPEN   .set(_XUBO_FF40_WRn, _XARE_RESETn, cpu_bus.TRI_D1);
    /*p23.XYMO*/ XYMO_LCDC_SPSIZE .set(_XUBO_FF40_WRn, _XARE_RESETn, cpu_bus.TRI_D2);
    /*p23.XAFO*/ XAFO_LCDC_BGMAP  .set(_XUBO_FF40_WRn, _XARE_RESETn, cpu_bus.TRI_D3);
    /*p23.WEXU*/ WEXU_LCDC_BGTILE .set(_XUBO_FF40_WRn, _XARE_RESETn, cpu_bus.TRI_D4);
    /*p23.WYMO*/ WYMO_LCDC_WINEN  .set(_XUBO_FF40_WRn, _XARE_RESETn, cpu_bus.TRI_D5);
    /*p23.WOKY*/ WOKY_LCDC_WINMAP .set(_XUBO_FF40_WRn, _XARE_RESETn, cpu_bus.TRI_D6);
    /*p23.XONA*/ XONA_LCDC_EN     .set(_XUBO_FF40_WRn, _XARE_RESETn, cpu_bus.TRI_D7);
  }


  // FF41 STAT
  {
    // stat read
    // hblank  = stat 0 = 00
    // vblank  = stat 1 = 01
    // oamscan = stat 2 = 10
    // render  = stat 3 = 11

    // RUPO arms on ground side, nor latch
    // RUPO00 << ROPO16
    // RUPO01 nc
    // RUPO02 >> SEGO03
    // RUPO03 >> nc
    // RUPO04 nc
    // RUPO05 << PAGO03

    // when PAGO03 goes high, RUPO02 goes high
    // when ROPO16 goes high, RUPO02 goes low.

    /*p22.WOFA*/ wire WOFA_FF41n = nand(cpu_sig.WERO_FF40_FF4Fp, cpu_sig.WADO_A00p, cpu_sig.XENO_A01n, cpu_sig.XUSY_A02n, cpu_sig.XERA_A03n);
    /*p22.VARY*/ wire VARY_FF41p = not(WOFA_FF41n);

    /*p21.TOBE*/ wire TOBE_FF41_RDa = and (cpu_sig.ASOT_CPU_RD, VARY_FF41p); // die AND
    /*p21.VAVE*/ wire VAVE_FF41_RDb = not(TOBE_FF41_RDa); // die INV

    /*p21.SEPA*/ wire SEPA_FF41_WRp = and (cpu_sig.CUPA_CPU_WR_xxxxxFGH, VARY_FF41p);
    /*p21.RYVE*/ wire RYVE_FF41_WRn = not(SEPA_FF41_WRp);

    /*p21.RYJU*/ wire RYJU_FF41_WRn = not(SEPA_FF41_WRp);
    /*p21.PAGO*/ wire PAGO_LYC_MATCH_RST = nor(rst_sig.WESY_RSTn, RYJU_FF41_WRn);  // schematic wrong, this is NOR
    /*p21.RUPO*/ RUPO_LYC_MATCH_LATCHn.nor_latch(PAGO_LYC_MATCH_RST, lcd_sig.ROPO_LY_MATCH_SYNCp);

    /*p21.ROXE*/ ROXE_INT_HBL_EN.set(RYVE_FF41_WRn, rst_sig.WESY_RSTn, cpu_bus.TRI_D0);
    /*p21.RUFO*/ RUFO_INT_VBL_EN.set(RYVE_FF41_WRn, rst_sig.WESY_RSTn, cpu_bus.TRI_D1);
    /*p21.REFE*/ REFE_INT_OAM_EN.set(RYVE_FF41_WRn, rst_sig.WESY_RSTn, cpu_bus.TRI_D2);
    /*p21.RUGU*/ RUGU_INT_LYC_EN.set(RYVE_FF41_WRn, rst_sig.WESY_RSTn, cpu_bus.TRI_D3);

    /*p21.PARU*/ wire PARU_IN_VBLANK = not(!lcd_sig.POPU_VBLANK_d4);
    /*p21.XATY*/ wire XATY_STAT_MODE1n = nor(XYMU_RENDERINGp, ppu_sig.ACYL_PPU_USE_OAM1p); // die NOR
    /*p21.SADU*/ wire SADU_STAT_MODE0n = nor(XYMU_RENDERINGp, PARU_IN_VBLANK); // die NOR

    // OK, these tribufs are _slightly_ different - compare SEGO and SASY, second rung.
    /*p21.TEBY*/ cpu_bus.TRI_D0.set_tribuf(TOBE_FF41_RDa, not(SADU_STAT_MODE0n));
    /*p21.WUGA*/ cpu_bus.TRI_D1.set_tribuf(TOBE_FF41_RDa, not(XATY_STAT_MODE1n));
    /*p21.SEGO*/ cpu_bus.TRI_D2.set_tribuf(TOBE_FF41_RDa, not(RUPO_LYC_MATCH_LATCHn.q()));
    /*p21.PUZO*/ cpu_bus.TRI_D3.set_tribuf(not(VAVE_FF41_RDb), ROXE_INT_HBL_EN.q());
    /*p21.POFO*/ cpu_bus.TRI_D4.set_tribuf(not(VAVE_FF41_RDb), RUFO_INT_VBL_EN.q());
    /*p21.SASY*/ cpu_bus.TRI_D5.set_tribuf(not(VAVE_FF41_RDb), REFE_INT_OAM_EN.q());
    /*p21.POTE*/ cpu_bus.TRI_D6.set_tribuf(not(VAVE_FF41_RDb), RUGU_INT_LYC_EN.q());
  }
}

//------------------------------------------------------------------------------

bool PpuRegisters::commit() {
  bool changed = false;
  /*p29.TOXE*/ changed |= TOXE_SFETCH_S0.commit_reg();
  /*p29.TULY*/ changed |= TULY_SFETCH_S1.commit_reg();
  /*p29.TESE*/ changed |= TESE_SFETCH_S2.commit_reg();
  /*p29.TOBU*/ changed |= TOBU_SFETCH_S1_D2.commit_reg();
  /*p29.VONU*/ changed |= VONU_SFETCH_S1_D4.commit_reg();
  /*p29.SEBA*/ changed |= SEBA_SFETCH_S1_D5.commit_reg();
  /*p29.TYFO*/ changed |= TYFO_SFETCH_S0_D1.commit_reg();

  /*p??.ROXY*/ changed |= ROXY_FINE_MATCH_LATCHn.commit_latch();
  /*p??.PUXA*/ changed |= PUXA_FINE_MATCH_SYNC1.commit_reg();
  /*p27.NYZE*/ changed |= NYZE_FINE_MATCH_SYNC2.commit_reg();
  /*p27.RYKU*/ changed |= RYKU_FINE_CNT0.commit_reg();
  /*p27.ROGA*/ changed |= ROGA_FINE_CNT1.commit_reg();
  /*p27.RUBU*/ changed |= RUBU_FINE_CNT2.commit_reg();
  /*p21.XEHO*/ changed |= XEHO_X0.commit_reg();
  /*p21.SAVY*/ changed |= SAVY_X1.commit_reg();
  /*p21.XODU*/ changed |= XODU_X2.commit_reg();
  /*p21.XYDO*/ changed |= XYDO_X3.commit_reg();
  /*p21.TUHU*/ changed |= TUHU_X4.commit_reg();
  /*p21.TUKY*/ changed |= TUKY_X5.commit_reg();
  /*p21.TAKO*/ changed |= TAKO_X6.commit_reg();
  /*p21.SYBE*/ changed |= SYBE_X7.commit_reg();
  /*p21.XYMU*/ changed |= XYMU_RENDERINGp.commit_latch();
  /*p21.VOGA*/ changed |= VOGA_RENDER_DONE_SYNC.commit_reg();
  /*p27.LONY*/ changed |= LONY_BG_READ_VRAM_LATCHp.commit_latch();
  /*p27.LOVY*/ changed |= LOVY_BG_SEQ5_SYNC.commit_reg();
  /*p24.PYGO*/ changed |= PYGO_TILE_DONE.commit_reg();
  /*p24.POKY*/ changed |= POKY_AFTER_PORCH_LATCHp.commit_latch();

  /*p27.TAKA*/ changed |= TAKA_SFETCH_RUN_LATCH.commit_latch();
  /*p27.SOBU*/ changed |= SOBU_SPRITE_FETCH_SYNC1.commit_reg();
  /*p27.SUDA*/ changed |= SUDA_SPRITE_FETCH_SYNC2.commit_reg();
  /*p21.RUPO*/ changed |= RUPO_LYC_MATCH_LATCHn.commit_latch();


  /*p24.PAHO*/ changed |= PAHO_X_8_SYNC.commit_reg();
  /*p24.RUJU*/ changed |= POFY_ST_LATCH.commit_latch(); // nor latch with p24.RUJU, p24.POME
  /*p21.WUSA*/ changed |= WUSA_CPEN_LATCH.commit_latch();
  /* PIN_53 */ changed |= CP.commit_pinout();
  /* PIN_54 */ changed |= ST.commit_pinout();

  // FF40 - LCDC
  /*p23.VYXE*/ changed |= VYXE_LCDC_BGEN.commit_reg();
  /*p23.XYLO*/ changed |= XYLO_LCDC_SPEN.commit_reg();
  /*p23.XYMO*/ changed |= XYMO_LCDC_SPSIZE.commit_reg();
  /*p23.XAFO*/ changed |= XAFO_LCDC_BGMAP.commit_reg();
  /*p23.WEXU*/ changed |= WEXU_LCDC_BGTILE.commit_reg();
  /*p23.WYMO*/ changed |= WYMO_LCDC_WINEN.commit_reg();
  /*p23.WOKY*/ changed |= WOKY_LCDC_WINMAP.commit_reg();
  /*p23.XONA*/ changed |= XONA_LCDC_EN.commit_reg();

  // FF41 - STAT
  /*p21.ROXE*/ changed |= ROXE_INT_HBL_EN.commit_reg();
  /*p21.RUFO*/ changed |= RUFO_INT_VBL_EN.commit_reg();
  /*p21.REFE*/ changed |= REFE_INT_OAM_EN.commit_reg();
  /*p21.RUGU*/ changed |= RUGU_INT_LYC_EN.commit_reg();

  return changed;
}

//------------------------------------------------------------------------------

//dump_long(text_painter, "PAHO_X_8_SYNC ", PAHO_X_8_SYNC.a);
//dump_long(text_painter, "WUSA_CPEN_LATCH    ", WUSA_CPEN_LATCH.a);
//dump_long(text_painter, "POFY_ST_LATCH ", POFY_ST_LATCH.a);
    //CP.dump(text_painter, "CP  ");
    //ST.dump(text_painter, "ST  ");

#if 0

void dump_regs(TextPainter& text_painter) {
  text_painter.dprintf("----- VID_REG -----\n");

  text_painter.dprintf("PIX X      %d\n", pack(XEHO_X0.q(), SAVY_X1.q(), XODU_X2.q(), XYDO_X3.q(), TUHU_X4.q(), TUKY_X5.q(), TAKO_X6.q(), SYBE_X7.q()));
  //text_painter.dprintf("WIN MAP X  %d\n", pack(WIN_X3.q(), WIN_X4.q(), WIN_X5.q(), WIN_X6.q(), WIN_X7.q()));
  //text_painter.dprintf("WIN MAP Y  %d\n", pack(WIN_Y3.q(), WIN_Y4.q(), WIN_Y5.q(), WIN_Y6.q(), WIN_Y7.q()));
  //text_painter.dprintf("WIN TILE Y %d\n", pack(WIN_Y0.q(), WIN_Y1.q(), WIN_Y2.q()));
  text_painter.dprintf("FINE_CNT   %d\n", pack(RYKU_FINE_CNT0.q(), ROGA_FINE_CNT1.q(), RUBU_FINE_CNT2.q()));

  ROXY_FINE_MATCH_LATCHn.dump(text_painter, "ROXY_FINE_MATCH_LATCHn          ");
  PUXA_FINE_MATCH_SYNC1.dump(text_painter, "PUXA_FINE_MATCH_SYNC1         ");
  NYZE_FINE_MATCH_SYNC2.dump(text_painter, "NYZE_FINE_MATCH_SYNC2         ");
  XYMU_RENDERINGp.dump(text_painter, "RENDERING_LATCH     ");
  VOGA_RENDER_DONE_SYNC.dump(text_painter, "VOGA_RENDER_DONE_SYNC         ");
  ROXE_INT_HBL_EN.dump(text_painter, "ROXE_INT_HBL_EN               ");
  RUFO_INT_VBL_EN.dump(text_painter, "RUFO_INT_VBL_EN               ");
  REFE_INT_OAM_EN.dump(text_painter, "REFE_INT_OAM_EN               ");
  RUGU_INT_LYC_EN.dump(text_painter, "RUGU_INT_LYC_EN               ");
  LONY_BG_READ_VRAM_LATCHp.dump(text_painter, "BG_READ_VRAM_LATCH      ");
  LAXU_BFETCH_S0.dump(text_painter, "LAXU_BFETCH_S0          ");
  MESU_BFETCH_S1.dump(text_painter, "MESU_BFETCH_S1          ");
  NYVA_BFETCH_S2.dump(text_painter, "NYVA_BFETCH_S2          ");
  LOVY_BG_SEQ5_SYNC.dump(text_painter, "BG_SEQ5_SYNC             ");
  NYKA_BFETCH_DONE_SYNC.dump(text_painter, "NYKA_BFETCH_DONE_SYNC                 ");
  PORY_BFETCH_DONE_SYNC_DELAY.dump(text_painter, "PORY_BFETCH_DONE_SYNC_DELAY                 ");
  LYZU_BFETCH_S0_DELAY.dump(text_painter, "LYZU_BFETCH_S0_DELAY    ");
  PYGO_TILE_DONE.dump(text_painter, "PYGO_TILE_DONE           ");
  POKY_AFTER_PORCH_LATCHp.dump(text_painter, "POKY_AFTER_PORCH_LATCHp  ");
  TAKA_SFETCH_RUN_LATCH.dump(text_painter, "TAKA_SFETCH_RUN_LATCH      ");
  SOBU_SPRITE_FETCH_SYNC1.dump(text_painter, "SOBU_SPRITE_FETCH_SYNC1      ");
  SUDA_SPRITE_FETCH_SYNC2.dump(text_painter, "SUDA_SPRITE_FETCH_SYNC2      ");
  text_painter.newline();
}

void dump_regs(TextPainter& text_painter) {
  text_painter.dprintf("----- SPR_REG -----\n");

  TOXE_SFETCH_S0.dump(text_painter, "TOXE_SFETCH_S0    ");
  TULY_SFETCH_S1.dump(text_painter, "TULY_SFETCH_S1    ");
  TESE_SFETCH_S2.dump(text_painter, "TESE_SFETCH_S2    ");
  TOBU_SFETCH_S1_D2.dump(text_painter, "TOBU_SFETCH_S1_D2  ");
  VONU_SFETCH_S1_D4.dump(text_painter, "VONU_SFETCH_S1_D4 ");
  SEBA_SFETCH_S1_D5.dump(text_painter, "SEBA_SFETCH_S1_D5 ");
  TYFO_SFETCH_S0_D1.dump(text_painter, "TYFO_SFETCH_S0_D1     ");
  //CENO_SCANNINGp.dump(text_painter, "CENO_SCANNINGp");

  //text_painter.dprintf("SCAN    %d\n", scan());
  //SCAN_DONE_d4.dump(text_painter, "SCAN_DONE_d4 ");
  //SCAN_DONE_d5.dump(text_painter, "SCAN_DONE_d5 ");

  //text_painter.dprintf("SPR_IDX %d\n", spr_idx());
  //text_painter.dprintf("TS_IDX  %d\n", ts_idx());
  //text_painter.dprintf("TS_LINE %d\n", ts_line());
  text_painter.newline();
}

/*
void dump_regs(TextPainter& text_painter) {
text_painter.dprintf(" ----- PPU CFG -----\n");
dump_long(text_painter, "VYXE_LCDC_BGEN   ", VYXE_LCDC_BGEN.a  );
dump_long(text_painter, "XYLO_LCDC_SPEN   ", XYLO_LCDC_SPEN.a  );   
dump_long(text_painter, "XYMO_LCDC_SPSIZE ", XYMO_LCDC_SPSIZE.a);
dump_long(text_painter, "XAFO_LCDC_BGMAP  ", XAFO_LCDC_BGMAP.a );
dump_long(text_painter, "WEXU_LCDC_BGTILE ", WEXU_LCDC_BGTILE.a);
dump_long(text_painter, "WYMO_LCDC_WINEN  ", WYMO_LCDC_WINEN.a );
dump_long(text_painter, "WOKY_LCDC_WINMAP ", WOKY_LCDC_WINMAP.a);
dump_long(text_painter, "XONA_LCDC_EN     ", XONA_LCDC_EN.a    );

//dump(text_painter,      "LYC         ", LYC0,  LYC1,  LYC2,  LYC3,  LYC4,  LYC5,  LYC6,  LYC7);
//dump(text_painter,      "BGP         ", BGP0,  BGP1,  BGP2,  BGP3,  BGP4,  BGP5,  BGP6,  BGP7);
//dump(text_painter,      "OBP0        ", OBP00, OBP01, OBP02, OBP03, OBP04, OBP05, OBP06, OBP07);
//dump(text_painter,      "OBP1        ", OBP10, OBP11, OBP12, OBP13, OBP14, OBP15, OBP16, OBP17);

text_painter.newline();
}

int get_lcdc() const {
return pack(VYXE_LCDC_BGEN.q(),
XYLO_LCDC_SPEN.q(),
XYMO_LCDC_SPSIZE.q(),
XAFO_LCDC_BGMAP.q(),
WEXU_LCDC_BGTILE.q(),
WYMO_LCDC_WINEN.q(),
WOKY_LCDC_WINMAP.q(),
XONA_LCDC_EN.q());
}
*/


#endif