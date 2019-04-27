.include "header.inc"

main:
  reset_lcd_for_test

  ld a, $FF
  ldh (LYC), a

  set_stat_int_lyc
  set_ie_stat
  clear_if
  ei

  nops 88 + 114
  ld a, 2
  ldh (LYC), a

  di
  test_fail

.org STAT_INT_VECTOR
  test_pass
