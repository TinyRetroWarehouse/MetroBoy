#include "GateBoyLib/GateBoyZramBus.h"

#include "GateBoyLib/GateBoy.h"

// ZRAM control signals are
// clk_reg.SIG_CPU_BUKE_AxxxxxGH
// TEDO_CPU_RDp();
// TAPU_CPU_WRp_xxxxEFGx()
// SYKE_FF00_FFFFp
// and there's somes gates WUTA/WOLY/WALE that do the check for FFXX && !FFFF

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::tock_zram_gates(GateBoyReg& reg_old)
{
  auto addr = bit_pack(reg.cpu_abus);
  wire CSp = (addr >= 0xFF80) && (addr <= 0xFFFE);

  if (bit(reg.zram_bus.clk_old.out_old() & ~reg.cpu_signals.TAPU_CPU_WRp.out_new() & CSp)) {
    mem.zero_ram[addr & 0x007F] = (uint8_t)bit_pack(reg_old.cpu_dbus);
  }
  reg.zram_bus.clk_old = reg.cpu_signals.TAPU_CPU_WRp.out_new();

  uint8_t data = mem.zero_ram[addr & 0x007F];

  triwire tri0 = tri_pp(CSp && bit(reg.cpu_signals.TEDO_CPU_RDp.out_new()), get_bit(data, 0));
  triwire tri1 = tri_pp(CSp && bit(reg.cpu_signals.TEDO_CPU_RDp.out_new()), get_bit(data, 1));
  triwire tri2 = tri_pp(CSp && bit(reg.cpu_signals.TEDO_CPU_RDp.out_new()), get_bit(data, 2));
  triwire tri3 = tri_pp(CSp && bit(reg.cpu_signals.TEDO_CPU_RDp.out_new()), get_bit(data, 3));
  triwire tri4 = tri_pp(CSp && bit(reg.cpu_signals.TEDO_CPU_RDp.out_new()), get_bit(data, 4));
  triwire tri5 = tri_pp(CSp && bit(reg.cpu_signals.TEDO_CPU_RDp.out_new()), get_bit(data, 5));
  triwire tri6 = tri_pp(CSp && bit(reg.cpu_signals.TEDO_CPU_RDp.out_new()), get_bit(data, 6));
  triwire tri7 = tri_pp(CSp && bit(reg.cpu_signals.TEDO_CPU_RDp.out_new()), get_bit(data, 7));

  reg.cpu_dbus.BUS_CPU_D00p.tri_bus(tri0);
  reg.cpu_dbus.BUS_CPU_D01p.tri_bus(tri1);
  reg.cpu_dbus.BUS_CPU_D02p.tri_bus(tri2);
  reg.cpu_dbus.BUS_CPU_D03p.tri_bus(tri3);
  reg.cpu_dbus.BUS_CPU_D04p.tri_bus(tri4);
  reg.cpu_dbus.BUS_CPU_D05p.tri_bus(tri5);
  reg.cpu_dbus.BUS_CPU_D06p.tri_bus(tri6);
  reg.cpu_dbus.BUS_CPU_D07p.tri_bus(tri7);
}

//------------------------------------------------------------------------------------------------------------------------
