#pragma once
#include "Cells.h"

namespace Schematics {

struct SchematicTop;
struct CpuBus;

//-----------------------------------------------------------------------------

struct VramBus {
  void tock(SchematicTop& top);
  void dump(Dumper& d, const SchematicTop& top) const;

  int get_map_x(const SchematicTop& top) const;
  int get_map_y(const SchematicTop& top) const;

  int get_bus_addr() const {
    return pack_p(!VRAM_BUS_A00n.tp(), !VRAM_BUS_A01n.tp(), !VRAM_BUS_A02n.tp(), !VRAM_BUS_A03n.tp(),
                  !VRAM_BUS_A04n.tp(), !VRAM_BUS_A05n.tp(), !VRAM_BUS_A06n.tp(), !VRAM_BUS_A07n.tp(),
                  !VRAM_BUS_A08n.tp(), !VRAM_BUS_A09n.tp(), !VRAM_BUS_A10n.tp(), !VRAM_BUS_A11n.tp(),
                  !VRAM_BUS_A12n.tp(), 0, 0, 0);
  }

  int get_bus_data() const {
    return pack_p(VRAM_BUS_D0p.tp(), VRAM_BUS_D1p.tp(), VRAM_BUS_D2p.tp(), VRAM_BUS_D3p.tp(),
                  VRAM_BUS_D4p.tp(), VRAM_BUS_D5p.tp(), VRAM_BUS_D6p.tp(), VRAM_BUS_D7p.tp());
  }

  uint16_t get_pin_addr() const {
    return (uint16_t)pack_p(!VRAM_PIN_A00n.tp(), !VRAM_PIN_A01n.tp(), !VRAM_PIN_A02n.tp(), !VRAM_PIN_A03n.tp(),
                            !VRAM_PIN_A04n.tp(), !VRAM_PIN_A05n.tp(), !VRAM_PIN_A06n.tp(), !VRAM_PIN_A07n.tp(),
                            !VRAM_PIN_A08n.tp(), !VRAM_PIN_A09n.tp(), !VRAM_PIN_A10n.tp(), !VRAM_PIN_A11n.tp(),
                            !VRAM_PIN_A12n.tp(), 0, 0, 0);
  }

  uint8_t get_pin_data_out() const {
    return (uint8_t)pack_p(!VRAM_PIN_D0n_A.tp(), !VRAM_PIN_D1n_A.tp(), !VRAM_PIN_D2n_A.tp(), !VRAM_PIN_D3n_A.tp(),
                           !VRAM_PIN_D4n_A.tp(), !VRAM_PIN_D5n_A.tp(), !VRAM_PIN_D6n_A.tp(), !VRAM_PIN_D7n_A.tp());
  }

  void set_pin_data_in(uint8_t data) {
    VRAM_PIN_D0n_C = !(data & 0x01);
    VRAM_PIN_D1n_C = !(data & 0x02);
    VRAM_PIN_D2n_C = !(data & 0x04);
    VRAM_PIN_D3n_C = !(data & 0x08);
    VRAM_PIN_D4n_C = !(data & 0x10);
    VRAM_PIN_D5n_C = !(data & 0x20);
    VRAM_PIN_D6n_C = !(data & 0x40);
    VRAM_PIN_D7n_C = !(data & 0x80);
  }

  void set_pin_data_z() {
    VRAM_PIN_D0n_C = DELTA_TRIZ;
    VRAM_PIN_D1n_C = DELTA_TRIZ;
    VRAM_PIN_D2n_C = DELTA_TRIZ;
    VRAM_PIN_D3n_C = DELTA_TRIZ;
    VRAM_PIN_D4n_C = DELTA_TRIZ;
    VRAM_PIN_D5n_C = DELTA_TRIZ;
    VRAM_PIN_D6n_C = DELTA_TRIZ;
    VRAM_PIN_D7n_C = DELTA_TRIZ;
  }

  //-----------------------------------------------------------------------------

  /*p32.LEGU*/ RegQN LEGU_TILE_DA0n = REG_D0C0; // def holds inverted pix
  /*p32.NUDU*/ RegQN NUDU_TILE_DA1n = REG_D0C0;
  /*p32.MUKU*/ RegQN MUKU_TILE_DA2n = REG_D0C0;
  /*p32.LUZO*/ RegQN LUZO_TILE_DA3n = REG_D0C0;
  /*p32.MEGU*/ RegQN MEGU_TILE_DA4n = REG_D0C0;
  /*p32.MYJY*/ RegQN MYJY_TILE_DA5n = REG_D0C0;
  /*p32.NASA*/ RegQN NASA_TILE_DA6n = REG_D0C0;
  /*p32.NEFO*/ RegQN NEFO_TILE_DA7n = REG_D0C0; // color wrong on die

  /*p32.RAWU*/ RegQN RAWU_TILE_DB0n = REG_D0C0; // def holds inverted pix, also holds tile index during fetch
  /*p32.POZO*/ RegQN POZO_TILE_DB1n = REG_D0C0;
  /*p32.PYZO*/ RegQN PYZO_TILE_DB2n = REG_D0C0; 
  /*p32.POXA*/ RegQN POXA_TILE_DB3n = REG_D0C0; 
  /*p32.PULO*/ RegQN PULO_TILE_DB4n = REG_D0C0; 
  /*p32.POJU*/ RegQN POJU_TILE_DB5n = REG_D0C0; 
  /*p32.POWY*/ RegQN POWY_TILE_DB6n = REG_D0C0; 
  /*p32.PYJU*/ RegQN PYJU_TILE_DB7n = REG_D0C0;

  /*p33.PEFO*/ RegQN PEFO_SPRITE_DB0n = REG_D0C0; // def holds inverted pix
  /*p33.ROKA*/ RegQN ROKA_SPRITE_DB1n = REG_D0C0;
  /*p33.MYTU*/ RegQN MYTU_SPRITE_DB2n = REG_D0C0;
  /*p33.RAMU*/ RegQN RAMU_SPRITE_DB3n = REG_D0C0;
  /*p33.SELE*/ RegQN SELE_SPRITE_DB4n = REG_D0C0;
  /*p33.SUTO*/ RegQN SUTO_SPRITE_DB5n = REG_D0C0;
  /*p33.RAMA*/ RegQN RAMA_SPRITE_DB6n = REG_D0C0;
  /*p33.RYDU*/ RegQN RYDU_SPRITE_DB7n = REG_D0C0;

  /*p33.REWO*/ RegQN REWO_SPRITE_DA0n = REG_D0C0; // def holds inverted pix
  /*p33.PEBA*/ RegQN PEBA_SPRITE_DA1n = REG_D0C0;
  /*p33.MOFO*/ RegQN MOFO_SPRITE_DA2n = REG_D0C0;
  /*p33.PUDU*/ RegQN PUDU_SPRITE_DA3n = REG_D0C0;
  /*p33.SAJA*/ RegQN SAJA_SPRITE_DA4n = REG_D0C0;
  /*p33.SUNY*/ RegQN SUNY_SPRITE_DA5n = REG_D0C0;
  /*p33.SEMO*/ RegQN SEMO_SPRITE_DA6n = REG_D0C0;
  /*p33.SEGA*/ RegQN SEGA_SPRITE_DA7n = REG_D0C0;

  //----------------------------------------
  
  // VRAM_BUS_D* must _not_ be inverting, see CBD->VBD->VPD chain

  Tri VRAM_BUS_D0p = TRI_D0NP;
  Tri VRAM_BUS_D1p = TRI_D0NP;
  Tri VRAM_BUS_D2p = TRI_D0NP;
  Tri VRAM_BUS_D3p = TRI_D0NP;
  Tri VRAM_BUS_D4p = TRI_D0NP;
  Tri VRAM_BUS_D5p = TRI_D0NP;
  Tri VRAM_BUS_D6p = TRI_D0NP;
  Tri VRAM_BUS_D7p = TRI_D0NP;

  Tri VRAM_BUS_A00n = TRI_D0NP;
  Tri VRAM_BUS_A01n = TRI_D0NP;
  Tri VRAM_BUS_A02n = TRI_D0NP;
  Tri VRAM_BUS_A03n = TRI_D0NP;
  Tri VRAM_BUS_A04n = TRI_D0NP;
  Tri VRAM_BUS_A05n = TRI_D0NP;
  Tri VRAM_BUS_A06n = TRI_D0NP;
  Tri VRAM_BUS_A07n = TRI_D0NP;
  Tri VRAM_BUS_A08n = TRI_D0NP;
  Tri VRAM_BUS_A09n = TRI_D0NP;
  Tri VRAM_BUS_A10n = TRI_D0NP;
  Tri VRAM_BUS_A11n = TRI_D0NP;
  Tri VRAM_BUS_A12n = TRI_D0NP;

  //----------------------------------------
  // VRAM bus

  Tri _VRAM_PIN_CS_A = TRI_D0NP;   // PIN_43 <- P25.SOKY
  Tri _VRAM_PIN_CS_C = TRI_D0NP;   // PIN_43 -> P25.TEFY
  Tri _VRAM_PIN_CS_D = TRI_D0NP;   // PIN_43 <- P25.SETY

  Tri _VRAM_PIN_OE_A = TRI_D0NP;   // PIN_45 <- P25.REFO
  Tri _VRAM_PIN_OE_C = TRI_D0NP;   // PIN_45 -> P25.TAVY
  Tri _VRAM_PIN_OE_D = TRI_D0NP;   // PIN_45 <- P25.SAHA

  Tri _VRAM_PIN_WR_A = TRI_D0NP;   // PIN_49 <- P25.SYSY
  Tri _VRAM_PIN_WR_C = TRI_D0NP;   // PIN_49 -> P25.SUDO
  Tri _VRAM_PIN_WR_D = TRI_D0NP;   // PIN_49 <- P25.RAGU

  Tri VRAM_PIN_A00n  = TRI_D0NP;  // PIN_34 <- P04.ECAL
  Tri VRAM_PIN_A01n  = TRI_D0NP;  // PIN_35 <- P04.EGEZ
  Tri VRAM_PIN_A02n  = TRI_D0NP;  // PIN_36 <- P04.FUHE
  Tri VRAM_PIN_A03n  = TRI_D0NP;  // PIN_37 <- P04.FYZY
  Tri VRAM_PIN_A04n  = TRI_D0NP;  // PIN_38 <- P04.DAMU
  Tri VRAM_PIN_A05n  = TRI_D0NP;  // PIN_39 <- P04.DAVA
  Tri VRAM_PIN_A06n  = TRI_D0NP;  // PIN_40 <- P04.ETEG
  Tri VRAM_PIN_A07n  = TRI_D0NP;  // PIN_41 <- P04.EREW
  Tri VRAM_PIN_A08n  = TRI_D0NP;  // PIN_48 <- P04.EVAX
  Tri VRAM_PIN_A09n  = TRI_D0NP;  // PIN_47 <- P04.DUVE
  Tri VRAM_PIN_A10n  = TRI_D0NP;  // PIN_44 <- P04.ERAF
  Tri VRAM_PIN_A11n  = TRI_D0NP;  // PIN_46 <- P04.FUSY
  Tri VRAM_PIN_A12n  = TRI_D0NP;  // PIN_42 <- P04.EXYF

  Tri VRAM_PIN_D0n_A = TRI_D0NP;    // PIN_33 <- P25.REGE
  Tri VRAM_PIN_D1n_A = TRI_D0NP;    // PIN_31 <- P25.RYKY
  Tri VRAM_PIN_D2n_A = TRI_D0NP;    // PIN_30 <- P25.RAZO
  Tri VRAM_PIN_D3n_A = TRI_D0NP;    // PIN_29 <- P25.RADA
  Tri VRAM_PIN_D4n_A = TRI_D0NP;    // PIN_28 <- P25.RYRO
  Tri VRAM_PIN_D5n_A = TRI_D0NP;    // PIN_27 <- P25.REVU
  Tri VRAM_PIN_D6n_A = TRI_D0NP;    // PIN_26 <- P25.REKU
  Tri VRAM_PIN_D7n_A = TRI_D0NP;    // PIN_25 <- P25.RYZE

  Tri VRAM_PIN_D0_B = TRI_D0NP;    // PIN_33 <- P25.ROFA
  Tri VRAM_PIN_D1_B = TRI_D0NP;    // PIN_31 <- P25.ROFA
  Tri VRAM_PIN_D2_B = TRI_D0NP;    // PIN_30 <- P25.ROFA
  Tri VRAM_PIN_D3_B = TRI_D0NP;    // PIN_29 <- P25.ROFA
  Tri VRAM_PIN_D4_B = TRI_D0NP;    // PIN_28 <- P25.ROFA
  Tri VRAM_PIN_D5_B = TRI_D0NP;    // PIN_27 <- P25.ROFA
  Tri VRAM_PIN_D6_B = TRI_D0NP;    // PIN_26 <- P25.ROFA
  Tri VRAM_PIN_D7_B = TRI_D0NP;    // PIN_25 <- P25.ROFA

  Tri VRAM_PIN_D0n_C = TRI_D0PU;    // PIN_33 -> P25.RODY
  Tri VRAM_PIN_D1n_C = TRI_D0PU;    // PIN_31 -> P25.REBA
  Tri VRAM_PIN_D2n_C = TRI_D0PU;    // PIN_30 -> P25.RYDO
  Tri VRAM_PIN_D3n_C = TRI_D0PU;    // PIN_29 -> P25.REMO
  Tri VRAM_PIN_D4n_C = TRI_D0PU;    // PIN_28 -> P25.ROCE
  Tri VRAM_PIN_D5n_C = TRI_D0PU;    // PIN_27 -> P25.ROPU
  Tri VRAM_PIN_D6n_C = TRI_D0PU;    // PIN_26 -> P25.RETA
  Tri VRAM_PIN_D7n_C = TRI_D0PU;    // PIN_25 -> P25.RAKU

  Tri VRAM_PIN_D0n_D = TRI_D0NP;    // PIN_33 <- P25.RURA
  Tri VRAM_PIN_D1n_D = TRI_D0NP;    // PIN_31 <- P25.RULY
  Tri VRAM_PIN_D2n_D = TRI_D0NP;    // PIN_30 <- P25.RARE
  Tri VRAM_PIN_D3n_D = TRI_D0NP;    // PIN_29 <- P25.RODU
  Tri VRAM_PIN_D4n_D = TRI_D0NP;    // PIN_28 <- P25.RUBE
  Tri VRAM_PIN_D5n_D = TRI_D0NP;    // PIN_27 <- P25.RUMU
  Tri VRAM_PIN_D6n_D = TRI_D0NP;    // PIN_26 <- P25.RYTY
  Tri VRAM_PIN_D7n_D = TRI_D0NP;    // PIN_25 <- P25.RADY

  //----------------------------------------
  // Signals for debugging

  /*p26.FAFO*/ Sig FAFO_TILE_Y0S; 
  /*p26.FAFO*/ Sig FAFO_TILE_Y0C; 
  /*p26.EMUX*/ Sig EMUX_TILE_Y1S; 
  /*p26.EMUX*/ Sig EMUX_TILE_Y1C; 
  /*p26.ECAB*/ Sig ECAB_TILE_Y2S; 
  /*p26.ECAB*/ Sig ECAB_TILE_Y2C; 
  /*p26.ETAM*/ Sig ETAM_MAP_Y0S;
  /*p26.ETAM*/ Sig ETAM_MAP_Y0C;
  /*p26.DOTO*/ Sig DOTO_MAP_Y1S;
  /*p26.DOTO*/ Sig DOTO_MAP_Y1C;
  /*p26.DABA*/ Sig DABA_MAP_Y2S;
  /*p26.DABA*/ Sig DABA_MAP_Y2C;
  /*p26.EFYK*/ Sig EFYK_MAP_Y3S;
  /*p26.EFYK*/ Sig EFYK_MAP_Y3C;
  /*p26.EJOK*/ Sig EJOK_MAP_Y4S;
  /*p26.EJOK*/ Sig EJOK_MAP_Y4C;

  /*p26.ATAD*/ Sig ATAD_TILE_X0S; 
  /*p26.ATAD*/ Sig ATAD_TILE_X0C; 
  /*p26.BEHU*/ Sig BEHU_TILE_X1S; 
  /*p26.BEHU*/ Sig BEHU_TILE_X1C; 
  /*p26.APYH*/ Sig APYH_TILE_X2S; 
  /*p26.APYH*/ Sig APYH_TILE_X2C; 
  /*p26.BABE*/ Sig BABE_MAP_X0S;
  /*p26.BABE*/ Sig BABE_MAP_X0C;
  /*p26.ABOD*/ Sig ABOD_MAP_X1S;
  /*p26.ABOD*/ Sig ABOD_MAP_X1C;
  /*p26.BEWY*/ Sig BEWY_MAP_X2S;
  /*p26.BEWY*/ Sig BEWY_MAP_X2C;
  /*p26.BYCA*/ Sig BYCA_MAP_X3S;
  /*p26.BYCA*/ Sig BYCA_MAP_X3C;
  /*p26.ACUL*/ Sig ACUL_MAP_X4S;
  /*p26.ACUL*/ Sig ACUL_MAP_X4C;

  /*p25.XEDU*/ Sig XEDU_CPU_VRAM_RDn;
  /*p04.AHOC*/ Sig AHOC_DMA_VRAM_RDn;
  /*p29.ABON*/ Sig ABON_SPR_VRM_RDn;
  /*p26.BAFY*/ Sig BAFY_BG_MAP_READn;
  /*p25.WUKO*/ Sig WUKO_WIN_MAP_READn;

  /*p27.NETA*/ Sig NETA_TILE_READp;
  /*p26.ASUL*/ Sig ASUL_TILE_READp;
  /*p26.BEJE*/ Sig BEJE_BGD_TILE_READn;
  /*p25.XUCY*/ Sig XUCY_WIN_TILE_READn;
  /*p25.VUZA*/ Sig VUZA_TILE_BANKp;
};

//-----------------------------------------------------------------------------

}; // namespace Schematics