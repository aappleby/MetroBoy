; ld happens before interrupt and after if change, di not hit

.include "header.inc"

main:
  reset_lcd_for_test

  long_delay 114 * 143

  set_stat_int_oam
  set_ie_stat
  clear_if
  ei

  xor a
  ld hl, $FF0F
  nops 92

  ld a, (hl)
  di
  test_fail

.org STAT_INT_VECTOR
  test_finish_a $E2