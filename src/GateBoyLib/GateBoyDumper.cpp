#include "GateBoyLib/GateBoyDumper.h"
#include "GateBoyLib/GateBoy.h"
#include "GateBoyLib/Gates.h"

#include "GateBoyLib/Utils.h"

char cp_int(uint8_t state) {
  if (state & BIT_DRIVEN) return bit0(state) ? '1' : '0';
  if (state & BIT_PULLED) return bit0(state) ? '^' : 'v';
  return 'X';
}

void dump_slice_int(Dumper& d, const char* tag, const BitBase* bits, int bit_count) {
  d(tag);
  for (int i = bit_count - 1; i >= 0; i--) {
    d.add_char(cp_int(bits[i].state));
  }
  d.add_char('\n');
}

/*
void dump_slicen(Dumper& d, const char* tag, const BitBase* bits, int bit_count) {
  d(tag);
  for (int i = bit_count - 1; i >= 0; i--) {
    d.add_char(bits[i].cn());
  }
  d.add_char('\n');
}
*/

void GateBoyDumper::dump_pins(const GateBoyPins& p, Dumper& d) {
  d.dump_slice2n("PIN_01_ADDR      : ", &p.abus_lo, 16);
  d.dump_slice2n("PIN_17_DATA      : ", &p.dbus, 8);
  d.dump_bitn   ("PIN_80_CSn       : ", p.ctrl.PIN_80_CSn.state);
  d.dump_bitn   ("PIN_79_RDn       : ", p.ctrl.PIN_79_RDn.state);
  d.dump_bitn   ("PIN_78_WRn       : ", p.ctrl.PIN_78_WRn.state);

  d.dump_bitp   ("PIN_73_CLK_DRIVE : ", p.sys.PIN_73_CLK_DRIVE.state);
  d.dump_bitp   ("PIN_74_CLK       : ", p.sys.PIN_74_CLK.CLK.state);
  d.dump_bitp   ("PIN_74_CLKGOOD   : ", p.sys.PIN_74_CLK.CLKGOOD.state);
  d.dump_bitp   ("PIN_75_CLK_OUT   : ", p.sys.PIN_75_CLK_OUT.state);

  d.dump_bitp   ("PIN_71_RST       : ", p.sys.PIN_71_RST.state);
  d.dump_bitp   ("PIN_77_T1        : ", p.sys.PIN_77_T1.state);
  d.dump_bitp   ("PIN_76_T2        : ", p.sys.PIN_76_T2.state);

  d.dump_bitp   ("PIN_43_VRAM_CSn  : ", p.vram_ctrl.PIN_43_VRAM_CSn.state);
  d.dump_bitp   ("PIN_45_VRAM_OEn  : ", p.vram_ctrl.PIN_45_VRAM_OEn.state);
  d.dump_bitp   ("PIN_49_VRAM_WRn  : ", p.vram_ctrl.PIN_49_VRAM_WRn.state);
  d.dump_slice2p("PIN_34_VRAM_ADDR : ", &p.vram_abus.PIN_34_VRAM_A00, 13);
  d.dump_slice2p("PIN_25_VRAM_DATA : ", &p.vram_dbus.PIN_33_VRAM_D00, 8);
  d.dump_bitp   ("PIN_67_JOY_P10   : ", p.joy.PIN_67_JOY_P10.state);
  d.dump_bitp   ("PIN_66_JOY_P11   : ", p.joy.PIN_66_JOY_P11.state);
  d.dump_bitp   ("PIN_65_JOY_P12   : ", p.joy.PIN_65_JOY_P12.state);
  d.dump_bitp   ("PIN_64_JOY_P13   : ", p.joy.PIN_64_JOY_P13.state);
  d.dump_bitp   ("PIN_63_JOY_P14   : ", p.joy.PIN_63_JOY_P14.state);
  d.dump_bitp   ("PIN_62_JOY_P15   : ", p.joy.PIN_62_JOY_P15.state);
  d.dump_bitp   ("PIN_50_LCD_DATA1 : ", p.lcd.PIN_50_LCD_DATA1.state);
  d.dump_bitp   ("PIN_51_LCD_DATA0 : ", p.lcd.PIN_51_LCD_DATA0.state);
  d.dump_bitp   ("PIN_54_LCD_HSYNC : ", p.lcd.PIN_54_LCD_HSYNC.state);
  d.dump_bitp   ("PIN_56_LCD_FLIPS : ", p.lcd.PIN_56_LCD_FLIPS.state);
  d.dump_bitp   ("PIN_52_LCD_CNTRL : ", p.lcd.PIN_52_LCD_CNTRL.state);
  d.dump_bitp   ("PIN_55_LCD_LATCH : ", p.lcd.PIN_55_LCD_LATCH.state);
  d.dump_bitp   ("PIN_53_LCD_CLOCK : ", p.lcd.PIN_53_LCD_CLOCK.state);
  d.dump_bitp   ("PIN_57_LCD_VSYNC : ", p.lcd.PIN_57_LCD_VSYNC.state);
}


void GateBoyDumper::dump_sys(const GateBoySys& s, Dumper& d) {
  const char* phases[] = {
    "\002A_______\001",
    "\003_B______\001",
    "\002__C_____\001",
    "\003___D____\001",
    "\002____E___\001",
    "\003_____F__\001",
    "\002______G_\001",
    "\003_______H\001",
  };

#if 1
  d             ("phase_total  : %lld\n",    s.gb_phase_total);
  d             ("phase        : %s\n", phases[s.gb_phase_total & 7]);
  d             ("sys_rst      : %d\n", s.rst);
  d             ("sys_t1       : %d\n", s.t1);
  d             ("sys_t2       : %d\n", s.t2);
  d             ("sys_clken    : %d\n", s.clk_en);
  d             ("sys_clkgood  : %d\n", s.clk_good);
  d             ("sys_cpuready : %d\n", s.clk_req);
  d             ("sys_cpu_en   : %d\n", s.cpu_en);
  d             ("sys_fastboot : %d\n", s.fastboot);
  d             ("sys_buttons  : %d\n", s.buttons);
  //d.dump_req    ("bus_req_new  : ", gb.cpu.bus_req_new);
  //d.dump_bytep  ("data_latch   : ", gb.cpu.cpu_data_latch);
  d("\n");
  //d             ("gb_screen_x  : %d\n", gb_screen_x);
  //d             ("gb_screen_y  : %d\n", gb_screen_y);
  //d.dump_bitp   ("lcd_pix_lo   : ", lcd.lcd_pix_lo.state);
  //d.dump_bitp   ("lcd_pix_hi   : ", lcd.lcd_pix_hi.state);
  //d.dump_slice2p("lcd_pipe_lo  : ", lcd.lcd_pipe_lo, 8);
  //d.dump_slice2p("lcd_pipe_hi  : ", lcd.lcd_pipe_hi, 8);
  //d("\n");
  //d             ("sim_time     : %f\n",      gb.sys.sim_time);
#endif
}

void GateBoyDumper::dump_tile_fetcher(const GateBoyState& s, Dumper& d) {
  d.dump_bitp   ("LAXU_BFETCH_S0p     : ", s.tfetch_counter.LAXU_BFETCH_S0p_odd.state);
  d.dump_bitp   ("MESU_BFETCH_S1p     : ", s.tfetch_counter.MESU_BFETCH_S1p_odd.state);
  d.dump_bitp   ("NYVA_BFETCH_S2p     : ", s.tfetch_counter.NYVA_BFETCH_S2p_odd.state);
  d("\n");
  d.dump_bitp   ("LYRY_BFETCH_DONEp   : ", s.tfetch_control.LYRY_BFETCH_DONEp_odd.state);
  d.dump_bitp   ("POKY_PRELOAD_LATCHp : ", s.tfetch_control.POKY_PRELOAD_LATCHp_evn.state);
  d.dump_bitp   ("LONY_FETCHINGp      : ", s.tfetch_control.LONY_TFETCHINGp.state);
  d.dump_bitp   ("LOVY_FETCH_DONEp    : ", s.tfetch_control.LOVY_TFETCH_DONEp.state);
  d.dump_bitp   ("NYKA_FETCH_DONEp    : ", s.tfetch_control.NYKA_FETCH_DONEp_evn.state);
  d.dump_bitp   ("PORY_FETCH_DONEp    : ", s.tfetch_control.PORY_FETCH_DONEp_odd.state);
  d.dump_bitp   ("PYGO_FETCH_DONEp    : ", s.tfetch_control.PYGO_FETCH_DONEp_evn.state);
  d.dump_bitp   ("LYZU_BFETCH_S0p_D1  : ", s.tfetch_control.LYZU_BFETCH_S0p_D1.state);
  d("\n");
  d.dump_slice2n("Temp A : ", &s.tile_temp_a.LEGU_TILE_DA0n, 8);
  d.dump_slice2p("Temp B : ", &s.tile_temp_b.RAWU_TILE_DB0p, 8);
}

void GateBoyDumper::dump_clocks(const GateBoyState& s, Dumper& d) {
  d.dump_bitp("AFUR_xxxxEFGHp : ", s.sys_clk.AFUR_xxxxEFGH.state);
  d.dump_bitp("ALEF_AxxxxFGHp : ", s.sys_clk.ALEF_AxxxxFGH.state);
  d.dump_bitp("APUK_ABxxxxGHp : ", s.sys_clk.APUK_ABxxxxGH.state);
  d.dump_bitp("ADYK_ABCxxxxHp : ", s.sys_clk.ADYK_ABCxxxxH.state);
  d("\n");
  d.dump_bitp("WUVU_ABxxEFxxp : ", s.sys_clk.WUVU_ABxxEFxx.state);
  d.dump_bitp("VENA_xxCDEFxxp : ", s.sys_clk.VENA_xxCDEFxx.state);
  d.dump_bitp("WOSU_AxxDExxHp : ", s.sys_clk.WOSU_AxxDExxH.state);
}

void GateBoyDumper::dump_interrupts(const GateBoyState& s, Dumper& d) {
  d.dump_slice2p("FF0F IF : ", &s.reg_if, 5);
  d.dump_slice2p("FFFF IE : ", &s.reg_ie, 5);
  d.dump_slice2p("LATCH   : ", &s.int_latch, 5);
  d.dump_slice2p("CPU_INT : ", &s.cpu_int, 5);
  d.dump_slice2p("CPU_ACK : ", &s.cpu_ack, 5);
  d("\n");
  d.dump_bitp   ("ROPO_LY_MATCH   : ", s.int_ctrl.ROPO_LY_MATCH_SYNCp.state);
  d.dump_bitp   ("RUPO_LYC_MATCHn : ", s.int_ctrl.RUPO_LYC_MATCHn.state);
  d("\n");
  d.dump_bitp   ("NYDU_TIMA7p_DELAY    : ", s.int_ctrl.NYDU_TIMA7p_DELAY.state);
  d.dump_bitp   ("MOBA_TIMER_OVERFLOWp : ", s.int_ctrl.MOBA_TIMER_OVERFLOWp.state);
}

void GateBoyDumper::dump_joypad(const GateBoyState& s, Dumper& d) {
  d.dump_bitp("AWOB_WAKE_CPU   : ", s.int_ctrl.AWOB_WAKE_CPU.state);
  d.dump_bitp("SIG_CPU_WAKE    : ", s.int_ctrl.SIG_CPU_WAKE.state);
  d("\n");
  d("\n");
  d.dump_bitp("KEVU_JOYP_L0n   : ", s.joy_latch.KEVU_JOYP_L0n.state);
  d.dump_bitp("KAPA_JOYP_L1n   : ", s.joy_latch.KAPA_JOYP_L1n.state);
  d.dump_bitp("KEJA_JOYP_L2n   : ", s.joy_latch.KEJA_JOYP_L2n.state);
  d.dump_bitp("KOLO_JOYP_L3n   : ", s.joy_latch.KOLO_JOYP_L3n.state);
  d("\n");
  d.dump_bitp("BATU_JP_GLITCH0 : ", s.joy_int.BATU_JP_GLITCH0.state);
  d.dump_bitp("ACEF_JP_GLITCH1 : ", s.joy_int.ACEF_JP_GLITCH1.state);
  d.dump_bitp("AGEM_JP_GLITCH2 : ", s.joy_int.AGEM_JP_GLITCH2.state);
  d.dump_bitp("APUG_JP_GLITCH3 : ", s.joy_int.APUG_JP_GLITCH3.state);
  d("\n");
  //d.dump_bitp("JUTE_DBG_D0     : ", joy.JUTE_DBG_D0.state);
  //d.dump_bitp("KECY_DBG_D1     : ", joy.KECY_DBG_D1.state);
  //d.dump_bitp("JALE_DBG_D2     : ", joy.JALE_DBG_D2.state);
  //d.dump_bitp("KYME_DBG_D3     : ", joy.KYME_DBG_D3.state);
  d.dump_bitp("KELY_JOYP_UDLR  : ", s.reg_joy.KELY_JOYP_UDLRp.state);
  d.dump_bitp("COFY_JOYP_ABCS  : ", s.reg_joy.COFY_JOYP_ABCSp.state);
  //d.dump_bitp("KUKO_DBG_D6     : ", joy.KUKO_DBG_D6.state);
  //d.dump_bitp("KERU_DBG_D7     : ", joy.KERU_DBG_D7.state);
}

void GateBoyDumper::dump_lcd(const GateBoyState& s, Dumper& d) {
  d.dump_bitp   ("CATU : ", s.lcd.CATU_LINE_ENDp_odd.state);
  d.dump_bitp   ("ANEL : ", s.lcd.ANEL_LINE_ENDp_odd.state);
  d.dump_bitp   ("POPU : ", s.lcd.POPU_VBLANKp_odd.state);
  d.dump_bitp   ("MYTA : ", s.lcd.MYTA_FRAME_ENDp_odd.state);
  d.dump_bitp   ("RUTU : ", s.lcd.RUTU_LINE_ENDp_odd.state);
  d.dump_bitp   ("NYPE : ", s.lcd.NYPE_LINE_ENDp_odd.state);
  d("\n");
  d.dump_bitp   ("SYGU_LINE_STROBE    : ", s.lcd.SYGU_LINE_STROBE.state);
  d.dump_bitn   ("MEDA_VSYNC_OUTn     : ", s.lcd.MEDA_VSYNC_OUTn.state);
  d.dump_bitp   ("LUCA_LINE_EVENp     : ", s.lcd.LUCA_LINE_EVENp.state);
  d.dump_bitp   ("NAPO_FRAME_EVENp    : ", s.lcd.NAPO_FRAME_EVENp.state);
  d.dump_bitp   ("RUJU                : ", s.lcd.RUJU.state);
  d.dump_bitp   ("POFY                : ", s.lcd.POFY.state);
  d.dump_bitp   ("POME                : ", s.lcd.POME_X8_LATCH.state);
  d.dump_bitp   ("PAHO_X_8_SYNC       : ", s.lcd.PAHO_X8_SYNC.state);
  d.dump_bitp   ("WUSA_LCD_CLOCK_GATE : ", s.lcd.WUSA_LCD_CLOCK_GATE.state);
  d("\n");
  d("\n");
  d.dump_slice2p("LX              : ", &s.reg_lx.SAXO_LX0p_odd.state,  7);
  d.dump_slice2p("FF44 LY         : ", &s.reg_ly.MUWY_LY0p_odd.state,  8);
  d.dump_slice2n("FF45 LYC        : ", &s.reg_lyc.SYRY_LYC0n.state, 8);
}

void GateBoyDumper::dump_oam_bus(const GateBoyState& s, Dumper& d) {
  d.dump_bitp   ("MAKA_LATCH_EXTp  : ", s.oam_ctrl.MAKA_LATCH_EXTp.state);
  d.dump_bitp   ("WUJE_CPU_OAM_WRn : ", s.oam_ctrl.WUJE_CPU_OAM_WRn.state);
  d.dump_bitp   ("SIG_OAM_CLKn     : ", s.oam_ctrl.SIG_OAM_CLKn.state);
  d.dump_bitp   ("SIG_OAM_WRn_A    : ", s.oam_ctrl.SIG_OAM_WRn_A.state);
  d.dump_bitp   ("SIG_OAM_WRn_B    : ", s.oam_ctrl.SIG_OAM_WRn_B.state);
  d.dump_bitp   ("SIG_OAM_OEn      : ", s.oam_ctrl.SIG_OAM_OEn.state);

  d.dump_slice2n("BUS_OAM_An  : ", &s.oam_abus.BUS_OAM_A00n, 8);
  d.dump_slice2n("BUS_OAM_DAn : ", &s.oam_dbus_a.BUS_OAM_DA00n, 8);
  d.dump_slice2n("BUS_OAM_DBn : ", &s.oam_dbus_b.BUS_OAM_DB00n, 8);
  d.dump_slice2n("OAM LATCH A : ", &s.oam_latch_a.YDYV_OAM_LATCH_DA0n, 8);
  d.dump_slice2n("OAM LATCH B : ", &s.oam_latch_b.XYKY_OAM_LATCH_DB0n, 8);
  d.dump_slice2p("OAM TEMP A  : ", &s.oam_temp_a.XUSO_OAM_DA0p, 8);
  d.dump_slice2p("OAM TEMP B  : ", &s.oam_temp_b.YLOR_OAM_DB0p, 8);

}

void GateBoyDumper::dump_sprite_store(const GateBoyState& s, Dumper& d) {

  //SpriteMatchFlags sprite_match_flags;
  //SpriteResetFlags sprite_reset_flags;
  //SpriteStoreFlags sprite_store_flags;
  d.dump_flags  ("SPRITE MATCH   : ", &s.sprite_match_flags_odd, sizeof(s.sprite_match_flags_odd));
  d.dump_flags  ("SPRITE RESET   : ", &s.sprite_reset_flags_odd, sizeof(s.sprite_reset_flags_odd));
  d.dump_flags  ("SPRITE STORE   : ", &s.sprite_store_flags_evn, sizeof(s.sprite_store_flags_evn));
  d.dump_slice2p("SPRITE INDEX   : ", &s.sprite_index.XADU_SPRITE_IDX0p_odd.state, 6);
  d.dump_slice2p("SPRITE COUNT   : ", &s.sprite_counter.BESE_SPRITE_COUNT0_odd, 4);
  d("\n");
  d("STORE0 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i0), bit_pack_inv(s.store_l0), bit_pack_inv(s.store_x0));
  d("STORE1 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i1), bit_pack_inv(s.store_l1), bit_pack_inv(s.store_x1));
  d("STORE2 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i2), bit_pack_inv(s.store_l2), bit_pack_inv(s.store_x2));
  d("STORE3 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i3), bit_pack_inv(s.store_l3), bit_pack_inv(s.store_x3));
  d("STORE4 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i4), bit_pack_inv(s.store_l4), bit_pack_inv(s.store_x4));
  d("STORE5 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i5), bit_pack_inv(s.store_l5), bit_pack_inv(s.store_x5));
  d("STORE6 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i6), bit_pack_inv(s.store_l6), bit_pack_inv(s.store_x6));
  d("STORE7 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i7), bit_pack_inv(s.store_l7), bit_pack_inv(s.store_x7));
  d("STORE8 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i8), bit_pack_inv(s.store_l8), bit_pack_inv(s.store_x8));
  d("STORE9 I%02d L%02d X%03d\n", bit_pack_inv(s.store_i9), bit_pack_inv(s.store_l9), bit_pack_inv(s.store_x9));
}

void GateBoyDumper::dump_mbc1(const GateBoyState& s, Dumper& d) {
  d.dump_bitp   ("MBC1_MODE    : ",  s.ext_mbc.MBC1_MODE.state);
  d.dump_bitp   ("MBC1_RAM_EN  : ",  s.ext_mbc.MBC1_RAM_EN.state);
  d.dump_slice2p("MBC1_BANK    : ", &s.ext_mbc.MBC1_BANK0, 7);
  d.dump_slice2p("MBC1_BANK_LO : ", &s.ext_mbc.MBC1_BANK0, 5);
  d.dump_slice2p("MBC1_BANK_HI : ", &s.ext_mbc.MBC1_BANK5, 2);
}

void GateBoyDumper::dump_cpu_bus(const GateBoyState& s, Dumper& d) {
  d.dump_bitp   ("SIG_CPU_RDp       : ", s.cpu_signals.SIG_IN_CPU_RDp.state);
  d.dump_bitp   ("SIG_CPU_WRp       : ", s.cpu_signals.SIG_IN_CPU_WRp.state);
  d.dump_bitp   ("SIG_CPU_UNOR_DBG  : ", s.cpu_signals.SIG_CPU_UNOR_DBG.state);
  d.dump_bitp   ("SIG_CPU_ADDR_HIp  : ", s.cpu_signals.SIG_CPU_ADDR_HIp.state);
  d.dump_bitp   ("SIG_CPU_UMUT_DBG  : ", s.cpu_signals.SIG_CPU_UMUT_DBG.state);
  d.dump_bitp   ("SIG_CPU_EXT_BUSp  : ", s.cpu_signals.SIG_IN_CPU_EXT_BUSp.state);
  //d.dump_bitp   ("SIG_CPU_6         : ", SIG_CPU_6.state);
  d.dump_bitp   ("SIG_CPU_LATCH_EXT : ", s.cpu_signals.SIG_IN_CPU_DBUS_FREE.state);
  d.dump_bitp   ("BOOT_BITn         : ", s.cpu_signals.TEPU_BOOT_BITn.state);
  d.dump_bitp   ("SIG_CPU_BOOTp     : ", s.cpu_signals.SIG_CPU_BOOTp.state);
  d.dump_bitp   ("TEDO_CPU_RDp      : ", s.cpu_signals.TEDO_CPU_RDp.state);
  d.dump_bitp   ("APOV_CPU_WRp      : ", s.cpu_signals.APOV_CPU_WRp.state);
  d.dump_bitp   ("TAPU_CPU_WRp      : ", s.cpu_signals.TAPU_CPU_WRp.state);
  d.dump_slice2p("BUS_CPU_A : ",     (BitBase*)&s.cpu_abus.BUS_CPU_A00p, 16);
  d.dump_slice2p("BUS_CPU_D : ",     (BitBase*)&s.cpu_dbus.BUS_CPU_D00p, 8);
  //dump_slice_int(d,  "BUS_CPU_D : ", (BitBase*)&s.cpu_dbus.BUS_CPU_D00p, 8);
}

void GateBoyDumper::dump_dma(const GateBoyState& s, Dumper& d) {
  d.dump_slice2p("DMA_A_LOW  : ", &s.dma_lo.NAKY_DMA_A00p_odd, 8);
  d.dump_slice2n("DMA_A_HIGH : ", &s.reg_dma.NAFA_DMA_A08n, 8);
  d             ("DMA Addr   : 0x%02x:%02x\n", bit_pack_inv(s.reg_dma), bit_pack(s.dma_lo));
  d.dump_bitp   ("MATU_DMA_RUNNINGp : ", s.MATU_DMA_RUNNINGp_odd.state);
  d.dump_bitp   ("LYXE_DMA_LATCHp   : ", s.dma_ctrl.LYXE_DMA_LATCHp  .state);
  d.dump_bitp   ("MYTE_DMA_DONE     : ", s.dma_ctrl.MYTE_DMA_DONE_odd    .state);
  d.dump_bitp   ("LUVY_DMA_TRIG_d0  : ", s.dma_ctrl.LUVY_DMA_TRIG_d0_odd .state);
  d.dump_bitp   ("LENE_DMA_TRIG_d4  : ", s.dma_ctrl.LENE_DMA_TRIG_d4_odd .state);
  d.dump_bitp   ("LOKY_DMA_LATCHp   : ", s.dma_ctrl.LOKY_DMA_LATCHp_odd  .state);
}


void GateBoyDumper::dump_ext_bus(const GateBoyState& s, Dumper& d) {
  /*
  struct GateBoyMBC {
  Gate MBC1_RAM_EN;
  Gate MBC1_MODE;

  Gate MBC1_BANK0;
  Gate MBC1_BANK1;
  Gate MBC1_BANK2;
  Gate MBC1_BANK3;
  Gate MBC1_BANK4;
  Gate MBC1_BANK5;
  Gate MBC1_BANK6;
  };
  */

  d.dump_slice2p("ADDR LATCH  : ", &s.ext_addr_latch.ALOR_EXT_ADDR_LATCH_00p, 15);
  d.dump_slice2n("DATA LATCH  : ", &s.ext_data_latch.SOMA_EXT_DATA_LATCH_D0n, 8);
  d.dump_bitp   ("MBC1 RAM EN : ", s.ext_mbc.MBC1_RAM_EN.state);
  d.dump_bitp   ("MBC1 MODE   : ", s.ext_mbc.MBC1_MODE.state);
  d.dump_slice2p("MBC1 BANK   : ", &s.ext_mbc.MBC1_BANK0, 6);
}

void GateBoyDumper::dump_vram_bus(const GateBoyState& s, Dumper& d) {
  d.dump_slice2n("BUS_VRAM_An      : ", &s.vram_abus.lo.BUS_VRAM_A00n, 13);
  d.dump_slice2p("BUS_VRAM_Dp      : ", &s.vram_dbus.BUS_VRAM_D00p, 8);
}

void GateBoyDumper::dump_sprite_fetcher(const GateBoyState& s, Dumper& d) {
  d.dump_bitp   ("TOXE_SFETCH_S0       : ", s.sfetch_counter_evn.TOXE_SFETCH_S0p_evn     .state);
  d.dump_bitp   ("TULY_SFETCH_S1       : ", s.sfetch_counter_evn.TULY_SFETCH_S1p_evn     .state);
  d.dump_bitp   ("TESE_SFETCH_S2       : ", s.sfetch_counter_evn.TESE_SFETCH_S2p_evn     .state);
  d.dump_bitp   ("TAKA_SFETCH_RUNNINGp : ", s.sfetch_control.TAKA_SFETCH_RUNNINGp_evn.state);
  d.dump_bitp   ("SOBU_SFETCH_REQp     : ", s.sfetch_control.SOBU_SFETCH_REQp_evn    .state);
  d.dump_bitp   ("SUDA_SFETCH_REQp     : ", s.sfetch_control.SUDA_SFETCH_REQp_odd    .state);
  d.dump_bitp   ("TYFO_SFETCH_S0_D1    : ", s.sfetch_control.TYFO_SFETCH_S0p_D1_odd  .state);
  d.dump_bitp   ("TOBU_SFETCH_S1_D2    : ", s.sfetch_control.TOBU_SFETCH_S1p_D2_evn  .state);
  d.dump_bitp   ("VONU_SFETCH_S1_D4    : ", s.sfetch_control.VONU_SFETCH_S1p_D4_evn  .state);
  d.dump_bitp   ("SEBA_SFETCH_S1_D5    : ", s.sfetch_control.SEBA_SFETCH_S1p_D5_odd  .state);
  d.dump_slice2n("Temp A : ", &s.sprite_pix_a.REWO_SPRITE_DA0n, 8);
  d.dump_slice2n("Temp B : ", &s.sprite_pix_b.PEFO_SPRITE_DB0n, 8);
}

void GateBoyDumper::dump_timer(const GateBoyState& s, Dumper& d) {
  d.dump_slice2p("DIV16 : ", &s.reg_div.UKUP_DIV00p, 16);
  d.dump_slice2p("FF04 DIV  : ", &s.reg_div.UGOT_DIV06p, 8);
  d.dump_slice2p("FF05 TIMA : ", &s.reg_tima, 8);
  d.dump_slice2p("FF06 TMA  : ", &s.reg_tma, 8);
  d.dump_slice2p("FF07 TAC  : ", &s.reg_tac, 3);
}

void GateBoyDumper::dump_resets(const GateBoyState& s, Dumper& d) {
  d.dump_bitp("TUBO_WAITINGp  : ", s.sys_rst.TUBO_WAITINGp.state);
  d.dump_bitn("ASOL_POR_DONEn : ", s.sys_rst.ASOL_POR_DONEn.state);
  d.dump_bitp("AFER_SYS_RSTp  : ", s.sys_rst.AFER_SYS_RSTp.state);
  d.dump_bitp("SOTO_DBG_VRAMp : ", s.sys_rst.SOTO_DBG_VRAMp.state);

  d.dump_bitp("SIG_CPU_EXT_CLKGOOD : ", s.sys_rst.SIG_CPU_EXT_CLKGOOD.state);
  d.dump_bitp("SIG_CPU_EXT_RESETp  : ", s.sys_rst.SIG_CPU_EXT_RESETp .state);
  d.dump_bitp("SIG_CPU_STARTp      : ", s.sys_rst.SIG_CPU_STARTp     .state);
  d.dump_bitp("SIG_CPU_INT_RESETp  : ", s.sys_rst.SIG_CPU_INT_RESETp .state);
}

void GateBoyDumper::dump_sprite_scanner(const GateBoyState& s, Dumper& d) {
  d.dump_slice2p("SCAN INDEX        : ", &s.scan_counter.YFEL_SCAN0_odd, 6);
  d.dump_bitp   ("BESU_SCANNINGp    : ", s.sprite_scanner.BESU_SCAN_DONEn_odd.state);
  d.dump_bitp   ("CENO_SCANNINGp    : ", s.sprite_scanner.CENO_SCAN_DONEn_odd.state);
  d.dump_bitp   ("DEZY_COUNT_CLKp   : ", s.sprite_scanner.DEZY_INC_COUNTn_odd.state);
  d.dump_bitp   ("BYBA_SCAN_DONE_Ap : ", s.sprite_scanner.BYBA_SCAN_DONEp_odd.state);
  d.dump_bitp   ("DOBA_SCAN_DONE_Bp : ", s.sprite_scanner.DOBA_SCAN_DONEp_evn.state);
  d.dump_bitp   ("AVAP_SCAN_DONE_Tp : ", s.sprite_scanner.AVAP_SCAN_DONE_tp_odd.state);
}

void GateBoyDumper::dump_serial(const GateBoyState& s, Dumper& d) {
#if 0
  d.dump_bitp   ("ETAF_SER_RUNNING : ", serial.ETAF_SER_RUN.state);
  d.dump_bitp   ("CULY_XFER_DIR    : ", serial.CULY_SER_DIR.state);
  d.dump_bitp   ("COTY_SER_CLK     : ", serial.COTY_SER_CLK.state);
  d.dump_bitp   ("ELYS_SER_OUT     : ", serial.ELYS_SER_OUT.state);
  d.dump_slice2p("CAFA_SER_CNT     : ", &serial.CAFA_SER_CNT0, 4);
  d.dump_slice2p("CUBA_SER_DATA    : ", &serial.CUBA_SER_DATA0, 8);
#endif
}

void GateBoyDumper::dump_ppu(const GateBoyState& s, Dumper& d) {
  d.dump_slice2n("FF40 LCDC  : ", &s.reg_lcdc.VYXE_LCDC_BGENn, 8);
  
  {
    wire PARU_VBLANKp_odd = not1(s.lcd.POPU_VBLANKp_odd.qn_old());
    wire SADU_STAT_MODE0n = nor2(s.XYMU_RENDERING_LATCHn.qn_old(), PARU_VBLANKp_odd);
    wire XATY_STAT_MODE1n = nor2(s.ACYL_SCANNINGp_odd.out_old(), s.XYMU_RENDERING_LATCHn.qn_old());
    wire STAT[8] = {
      SADU_STAT_MODE0n,
      XATY_STAT_MODE1n,
      s.int_ctrl.RUPO_LYC_MATCHn.qp_old(),
      s.reg_stat.ROXE_STAT_HBI_ENn.qp_old(),
      s.reg_stat.RUFO_STAT_VBI_ENn.qp_old(),
      s.reg_stat.REFE_STAT_OAI_ENn.qp_old(),
      s.reg_stat.RUGU_STAT_LYI_ENn.qp_old(),
      0,
    };
  
    d.dump_slice2n("FF41 STAT  : ", STAT, 8);
  }

  d.dump_slice2n("FF42 SCY   : ", &s.reg_scy.GAVE_SCY0n, 8);
  d.dump_slice2n("FF43 SCX   : ", &s.reg_scx.DATY_SCX0n, 8);
  d.dump_slice2n("FF47 BGP   : ", &s.reg_bgp.PAVO_BGP_D0n, 8);
  d.dump_slice2n("FF48 OBP0  : ", &s.reg_obp0.XUFU_OBP0_D0n, 8);
  d.dump_slice2n("FF49 OBP1  : ", &s.reg_obp1.MOXY_OBP1_D0n, 8);
  d.dump_slice2n("FF4A WY    : ", &s.reg_wy.NESO_WY0n, 8);
  d.dump_slice2n("FF4B WX    : ", &s.reg_wx.MYPA_WX0n, 8);
  d.dump_slice2p("WIN MAP X  : ", &s.win_x.map.WYKA_WIN_MAP_X0, 5);
  d.dump_slice2p("WIN MAP Y  : ", &s.win_y.map.TUFU_WIN_MAP_Y0, 5);
  d.dump_slice2p("WIN TILE Y : ", &s.win_y.tile.VYNO_WIN_TILE_Y0, 3);
  d("\n");
  d.dump_slice2p("PIX COUNT  : ", &s.pix_count.XEHO_PX0p_odd, 8);
  d.dump_slice2p("BG PIPE A  : ", &s.bgw_pipe_a, 8);
  d.dump_slice2p("BG PIPE B  : ", &s.bgw_pipe_b, 8);
  d.dump_slice2p("SPR PIPE A : ", &s.spr_pipe_a, 8);
  d.dump_slice2p("SPR PIPE B : ", &s.spr_pipe_b, 8);
  d.dump_slice2p("PAL PIPE   : ", &s.pal_pipe, 8);
  d.dump_slice2p("MASK PIPE  : ", &s.mask_pipe, 8);
  d.dump_bitn   ("REMY_LD0n  : ", s.lcd.REMY_LD0n.state);
  d.dump_bitn   ("RAVO_LD1n  : ", s.lcd.RAVO_LD1n.state);
  d("\n");
  d.dump_bitp("XYMU_RENDERINGn        : ", s.XYMU_RENDERING_LATCHn.state);
  d.dump_bitp("PYNU_WIN_MODE_Ap       : ", s.win_ctrl.PYNU_WIN_MODE_Ap_odd.state);
  d.dump_bitp("PUKU_WIN_HITn          : ", s.win_ctrl.PUKU_WIN_HITn_odd.state);
  d.dump_bitp("RYDY_WIN_HITp          : ", s.win_ctrl.RYDY_WIN_HITp_odd.state);
  d.dump_bitp("SOVY_WIN_FIRST_TILE_B  : ", s.win_ctrl.SOVY_WIN_HITp_evn.state);
  d.dump_bitp("NOPA_WIN_MODE_B        : ", s.win_ctrl.NOPA_WIN_MODE_Bp_evn.state);
  d.dump_bitp("PYCO_WX_MATCH_A        : ", s.win_ctrl.PYCO_WIN_MATCHp_evn.state);
  d.dump_bitp("NUNU_WX_MATCH_B        : ", s.win_ctrl.NUNU_WIN_MATCHp_odd.state);
  d.dump_bitp("REJO_WY_MATCH_LATCH    : ", s.win_ctrl.REJO_WY_MATCH_LATCHp_odd.state);
  d.dump_bitp("SARY_WY_MATCH          : ", s.win_ctrl.SARY_WY_MATCHp_odd.state);
  d.dump_bitp("RYFA_FETCHn_A          : ", s.win_ctrl.RYFA_WIN_FETCHn_A_evn.state);
  d.dump_bitp("RENE_FETCHn_B          : ", s.win_ctrl.RENE_WIN_FETCHn_B_evn.state);
  d.dump_bitp("RYKU_FINE_CNT0         : ", s.fine_count_odd.RYKU_FINE_CNT0_odd.state);
  d.dump_bitp("ROGA_FINE_CNT1         : ", s.fine_count_odd.ROGA_FINE_CNT1_odd.state);
  d.dump_bitp("RUBU_FINE_CNT2         : ", s.fine_count_odd.RUBU_FINE_CNT2_odd.state);
  d.dump_bitp("PUXA_FINE_MATCH_A      : ", s.fine_scroll.PUXA_SCX_FINE_MATCH_evn.state);
  d.dump_bitp("NYZE_FINE_MATCH_B      : ", s.fine_scroll.NYZE_SCX_FINE_MATCH_odd.state);
  d.dump_bitp("ROXY_FINE_SCROLL_DONEn : ", s.fine_scroll.ROXY_FINE_SCROLL_DONEn_evn.state);

  d.dump_bitp("WODU_HBLANKp_odd       : ", s.WODU_HBLANK_GATEp_odd.state);
  d.dump_bitp("VOGA_HBLANKp_evn       : ", s.VOGA_HBLANKp_evn.state);
  d("\n");
}

void GateBoyDumper::dump_spu(const GateBoyState& s, Dumper& d) {
  (void)d;
  //d.dump_bitp   ("HADA_ALL_SOUND_ONp     : ", reg_NR52.HADA_ALL_SOUND_ONp.state);
  //d.dump_slice2p("NR50 : ", &reg_NR50.APEG_VOL_L0, 8);
}