#pragma once
#include "GateBoyLib/Gates.h"

namespace Schematics {

struct SchematicTop;
struct CpuBus;

//-----------------------------------------------------------------------------

struct Bootrom {
  void reset_cart() {
    BOOT_BITn.reset(REG_D1C1);
  }

  void reset_boot() {
    BOOT_BITn.reset(REG_D0C0);
  }

  // Starts 0, set to 1 by bootrom which blocks reading 0x0000-0x00FF.
  // In run mode, BOOT_BITn must _not_ be reset.
  /*p07.TEPU*/ DFF17 BOOT_BITn;
};

//-----------------------------------------------------------------------------

}; // namespace Schematics