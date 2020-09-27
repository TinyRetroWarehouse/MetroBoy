.include "header.inc"

main:
  ld a, 0

  ldh (DIV), a
  set_ie_timer

  ld a, $FC
  ldh (TIMA), a

  ld a, %00000101
  ldh (TAC), a

  ei

  xor a
.repeat 11
  inc a
.endr
  test_fail

.org TIMER_INT_VECTOR
  test_finish_a 11
