#pragma once
#include "GateBoyLib/Gates.h"

//-----------------------------------------------------------------------------

struct SpriteFetchCounter {
  void reset_to_poweron() {
    TOXE_SFETCH_S0p.state = 0b00011000;
    TULY_SFETCH_S1p.state = 0b00011000;
    TESE_SFETCH_S2p.state = 0b00011000;
  }

  void reset_to_bootrom() {
    TOXE_SFETCH_S0p.state = 0b00011000;
    TULY_SFETCH_S1p.state = 0b00011010;
    TESE_SFETCH_S2p.state = 0b00011010;
  }

  void reset_to_cart() {
    TOXE_SFETCH_S0p.state = 0b00011011;
    TULY_SFETCH_S1p.state = 0b00011000;
    TESE_SFETCH_S2p.state = 0b00011011;
  }

  /*#p29.TOXE*/ DFF17 TOXE_SFETCH_S0p;          // ABxDxFxH Beginning of line, reset on A. During line, ticks and resets on odd.
  /*#p29.TULY*/ DFF17 TULY_SFETCH_S1p;          // ABxDxFxH Beginning of line, reset on A. During line, ticks and resets on odd.
  /*#p29.TESE*/ DFF17 TESE_SFETCH_S2p;          // ABxDxFxH Beginning of line, reset on A. During line, ticks and resets on odd.
};

struct SpriteFetchControl {
  void reset_to_poweron() {
    WUTY_SFETCH_DONE_TRIGp.state = 0b00011000;
    TEXY_SFETCHINGp.state        = 0b00011000;
    TAKA_SFETCH_RUNNINGp.state   = 0b00011000;
    SOBU_SFETCH_REQp.state       = 0b00011000;
    SUDA_SFETCH_REQp.state       = 0b00011000;
    TYFO_SFETCH_S0p_D1.state     = 0b00011000;
    TOBU_SFETCH_S1p_D2.state     = 0b00011000;
    VONU_SFETCH_S1p_D4.state     = 0b00011000;
    SEBA_SFETCH_S1p_D5.state     = 0b00011000;
  }

  void reset_to_bootrom() {
    WUTY_SFETCH_DONE_TRIGp.state = 0b00011000;
    TEXY_SFETCHINGp.state        = 0b00011000;
    TAKA_SFETCH_RUNNINGp.state   = 0b00011001;
    SOBU_SFETCH_REQp.state       = 0b00011000;
    SUDA_SFETCH_REQp.state       = 0b00011010;
    TYFO_SFETCH_S0p_D1.state     = 0b00011010;
    TOBU_SFETCH_S1p_D2.state     = 0b00011000;
    VONU_SFETCH_S1p_D4.state     = 0b00011000;
    SEBA_SFETCH_S1p_D5.state     = 0b00011010;
  }

  void reset_to_cart() {
    WUTY_SFETCH_DONE_TRIGp.state = 0b00011000;
    TEXY_SFETCHINGp.state        = 0b00011000;
    TAKA_SFETCH_RUNNINGp.state   = 0b00011000;
    SOBU_SFETCH_REQp.state       = 0b00011000;
    SUDA_SFETCH_REQp.state       = 0b00011010;
    TYFO_SFETCH_S0p_D1.state     = 0b00011011;
    TOBU_SFETCH_S1p_D2.state     = 0b00011000;
    VONU_SFETCH_S1p_D4.state     = 0b00011000;
    SEBA_SFETCH_S1p_D5.state     = 0b00011010;
  }

  /*_p29.WUTY*/ Gate WUTY_SFETCH_DONE_TRIGp; // new used
  /*_p29.TEXY*/ Gate TEXY_SFETCHINGp; // new used
  /*_p27.TAKA*/ NandLatch TAKA_SFETCH_RUNNINGp; // ABCDEFGH Set on odd, cleared on even
  /*_p27.SOBU*/ DFF17 SOBU_SFETCH_REQp;         // xBxDxFxH
  /*_p27.SUDA*/ DFF17 SUDA_SFETCH_REQp;         // AxBxExGx

  /*#p29.TYFO*/ DFF17 TYFO_SFETCH_S0p_D1;       // AxCxExGx
  /*#p29.TOBU*/ DFF17 TOBU_SFETCH_S1p_D2;       // xBxDxFxH
  /*#p29.VONU*/ DFF17 VONU_SFETCH_S1p_D4;       // xBxDxFxH
  /*#p29.SEBA*/ DFF17 SEBA_SFETCH_S1p_D5;       // AxCxExGx
};

//-----------------------------------------------------------------------------
