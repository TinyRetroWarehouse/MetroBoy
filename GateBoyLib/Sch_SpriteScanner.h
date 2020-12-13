#pragma once
#include "GateBoyLib/Gates.h"

//-----------------------------------------------------------------------------

struct SpriteScanner {
  /*p28.BESU*/ NorLatch BESU_SCANNINGp_evn; // Axxxxxxx
  /*p29.CENO*/ DFF17 CENO_SCANNINGp_evn;    // xxxxExxx
  /*p29.BYBA*/ DFF17 BYBA_SCAN_DONE_Ap_evn; // Axxxxxxx
  /*p29.DOBA*/ DFF17 DOBA_SCAN_DONE_Bp_xxx; // ABxxxxxx Cleared on A, set on B

  /*p28.YFEL*/ DFF17 YFEL_SCAN0_evn;        // AxxxExxx
  /*p28.WEWY*/ DFF17 WEWY_SCAN1_evn;        // Axxxxxxx
  /*p28.GOSO*/ DFF17 GOSO_SCAN2_evn;        // Axxxxxxx
  /*p28.ELYN*/ DFF17 ELYN_SCAN3_evn;        // Axxxxxxx
  /*p28.FAHA*/ DFF17 FAHA_SCAN4_evn;        // Axxxxxxx
  /*p28.FONY*/ DFF17 FONY_SCAN5_evn;        // Axxxxxxx
};

//-----------------------------------------------------------------------------
