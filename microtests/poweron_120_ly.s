.include "header.inc"

.define DELAY  120
.define ADDR   $FF44
.define RESULT 1

main:
  nops DELAY
  ld a, (ADDR)
  test_finish_a RESULT
