; pass - dmg

.include "header.inc"

.define DELAY  175
.define ADDR   $9FFF
.define RESULT $91

main:
  ld hl, ADDR

  ld a, $0
  ldh (LCDC), a

  ld a, $F0
  ld (hl), a

  ld a, $91
  ldh (LCDC), a

  nops DELAY
  ld (hl), a

- ldh a, (STAT)
  and 3
  jr nz, -

  ld a, (hl)
  test_finish_a RESULT