#include "Z80.h"
#include "Constants.h"
#include <assert.h>

//-----------------------------------------------------------------------------

#define OP_QUAD       ((op >> 6) & 3)
#define OP_ROW        ((op >> 3) & 7)
#define OP_COL        ((op >> 0) & 7)

#define NOP           (op == 0x00)
#define STOP          (op == 0x10)

#define STM_A8_A      (op == 0xE0)
#define STM_C_A       (op == 0xE2)
#define STM_A16_SP    (op == 0x08)
#define STM_A16_A     (op == 0xEA)

#define ADD_SP_R8     (op == 0xE8)
#define LD_HL_SP_R8   (op == 0xF8)

#define LDM_A_A16     (op == 0xFA)
#define LDM_A_C       (op == 0xF2)
#define LDM_A_A8      (op == 0xF0)

#define JP_A16        (op == 0xC3)
#define JR_R8         (op == 0x18)
#define JP_HL         (op == 0xE9)
#define CALL_A16      (op == 0xCD)
#define RET           (op == 0xC9)
#define RETI          (op == 0xD9)
#define DI            (op == 0xF3)
#define EI            (op == 0xFB)
#define HALT          (op == 0x76)
#define LD_SP_HL      (op == 0xF9)

#define LD_BC_D16     (op == 0b00000001)
#define LD_DE_D16     (op == 0b00010001)
#define LD_HL_D16     (op == 0b00100001)
#define LD_SP_D16     (op == 0b00110001)
#define LD_RR_D16     ((op & 0b11001111) == 0b00000001)

#define ADD_HL_BC     (op == 0b00001001)
#define ADD_HL_DE     (op == 0b00011001)
#define ADD_HL_HL     (op == 0b00101001)
#define ADD_HL_SP     (op == 0b00111001)
#define ADD_HL_RR     ((op & 0b11001111) == 0b00001001)

#define STM_BC_A      (op == 0x02)
#define STM_DE_A      (op == 0x12)
#define STM_HLP_A     (op == 0x22)
#define STM_HLM_A     (op == 0x32)

#define LDM_A_BC      (op == 0x0A)
#define LDM_A_DE      (op == 0x1A)
#define LDM_A_HLP     (op == 0x2A)
#define LDM_A_HLM     (op == 0x3A)

#define INC_BC        (op == 0b00000011)
#define INC_DE        (op == 0b00010011)
#define INC_HL        (op == 0b00100011)
#define INC_SP        (op == 0b00110011)

#define DEC_BC        (op == 0x0B)
#define DEC_DE        (op == 0x1B)
#define DEC_HL        (op == 0x2B)
#define DEC_SP        (op == 0x3B)

#define INC_AT_HL     (op == 0x34)
#define DEC_AT_HL     (op == 0x35)
#define INC_R         ((op & 0b11000111) == 0b00000100 && !INC_AT_HL)
#define DEC_R         ((op & 0b11000111) == 0b00000101 && !DEC_AT_HL)

#define STM_HL_A      (op == 0x77)
#define STM_HL_D8     (op == 0x36)
#define LD_R_D8       ((op & 0b11000111) == 0b00000110 && !STM_HL_D8)

#define RLU_A         ((op & 0b11000111) == 0b00000111)

#define RLC_A         (op == 0x07)
#define RRC_A         (op == 0x0F)
#define RL_A          (op == 0x17)
#define RR_A          (op == 0x1F)
#define DAA           (op == 0x27)
#define CPL           (op == 0x2F)
#define SCF           (op == 0x37)
#define CCF           (op == 0x3F)

#define LDM_B_HL      (op == 0x46)
#define LDM_C_HL      (op == 0x4E)
#define LDM_D_HL      (op == 0x56)
#define LDM_E_HL      (op == 0x5E)
#define LDM_H_HL      (op == 0x66)
#define LDM_L_HL      (op == 0x6E)
#define LDM_A_HL      (op == 0x7E)

#define LDM_R_HL      ((op & 0b11000111) == 0b01000110 && !HALT)
#define STM_HL_R      ((op & 0b11111000) == 0b01110000 && !HALT)
#define MV_R_R        ((op & 0b11000000) == 0b01000000 && (op & 0b00000111) != 0b00000110 && (op & 0b00111000) != 0b00110000)

#define JR_CC_R8      ((op & 0b11100111) == 0b00100000)
#define RET_CC        ((op & 0b11100111) == 0b11000000)
#define JP_CC_A16     ((op & 0b11100111) == 0b11000010)
#define CALL_CC_A16   ((op & 0b11100111) == 0b11000100)

#define PUSH_BC       (op == 0b11000101)
#define PUSH_DE       (op == 0b11010101)
#define PUSH_HL       (op == 0b11100101)
#define PUSH_AF       (op == 0b11110101)
#define PUSH_RR       ((op & 0b11001111) == 0b11000101)

#define POP_BC        (op == 0b11000001)
#define POP_DE        (op == 0b11010001)
#define POP_HL        (op == 0b11100001)
#define POP_AF        (op == 0b11110001)
#define POP_RR        ((op & 0b11001111) == 0b11000001)

#define PREFIX_CB     (op == 0xCB)
#define CB_QUAD       ((cb >> 6) & 3)
#define CB_ROW        ((cb >> 3) & 7)
#define CB_COL        ((cb >> 0) & 7)
#define OP_CB_R       (PREFIX_CB && CB_COL != 6)
#define OP_CB_HL      (PREFIX_CB && CB_COL == 6)

#define ALU_A_R       ((OP_QUAD == 2) && (OP_COL != 6))
#define ALU_A_HL      ((OP_QUAD == 2) && (OP_COL == 6))
#define ALU_A_D8      ((OP_QUAD == 3) && (OP_COL == 6))
#define RST_NN        ((OP_QUAD == 3) && (OP_COL == 7))

// fake opcode to indicate interrupt handling
#define INT           (op == 0xF4)

uint8_t  sxt(uint8_t x) { return x & 0x80 ? 0xFF : 0x00; }

constexpr uint8_t F_CARRY = 0x10;
constexpr uint8_t F_HALF_CARRY = 0x20;
constexpr uint8_t F_NEGATIVE = 0x40;
constexpr uint8_t F_ZERO = 0x80;

//-----------------------------------------------------------------------------

void Z80::reset(uint16_t new_pc) {
  *this = Z80();

  if (new_pc == 0x100) {
    unhalt = 0;
    ime = ime_delay = 0;

    state = 0;
             
    int_ack = 0;

    pc = 0x100;
    op_addr = pc;
    op = 0;
    cb = 0;
    ad = pc;
    in = 0;
    state = 0;

    bc = 0x0013;
    de = 0x00D8;
    hl = 0x014D;
    af = 0x01B0;
    sp = 0xFFFE;
    xy = 0x0000;
  }
  else {
    unhalt = 0;
    ime = ime_delay = 0;

    state = 0;
             
    int_ack = 0;

    pc = 0xFFFF;
    op_addr = pc;
    op = 0;
    cb = 0;
    ad = pc;
    state = 0;

    bc = 0x0000;
    de = 0x0000;
    hl = 0x0000;
    af = 0x0000;
    sp = 0x0000;
    xy = 0x0000;
  }

  set_addr(ad, 0);

  in = 0;
}

//-----------------------------------------------------------------------------

void Z80::get_bus_req(Req& r) const {
  r = bus_req;
}

uint8_t lo(uint16_t x) { return uint8_t(x >> 0); }
uint8_t hi(uint16_t x) { return uint8_t(x >> 8); }

void Z80::set_addr(uint16_t new_addr, int new_write) {
  ad = new_addr;

  if (LDM_A_A8 && state == 1) { adh = 0xFF; }
  if (STM_A8_A && state == 1) { adh = 0xFF; }
  if (LDM_A_C  && state == 0) { adh = 0xFF; }
  if (STM_C_A  && state == 0) { adh = 0xFF; }

  bus_req.addr  = ad;
  bus_req.data  = uint16_t(new_write ? out : 0);
  bus_req.read  = (bool)!new_write;
  bus_req.write = (bool)new_write;
}

#define R_ROW reg(OP_ROW)
#define R_COL reg(OP_COL)

#define DBUS_BUSY
#define DBUS_IDLE
#define ABUS_BUSY
#define ABUS_IDLE

#pragma warning(disable:4189)

//-----------------------------------------------------------------------------
// Do the meat of executing the instruction
// pc update _must_ happen in tcycle 0 of state 0, because if an interrupt fires it should _not_ happen.
// if it always takes a bus move to get something into the incrementer, we can't increment the address in time

//-----------------------------------------------------------------------------

void Z80::tick_a(const uint8_t imask_, const uint8_t intf_, const Ack& ack) {
  state = state_;
  ime = ime_delay;

  if (state == 0) {
    op_addr = ack.addr;
    op = in;     
    int_ack = 0;

    if ((imask_ & intf_) && ime) {
      op = 0xF4; // fake opcode
      ime = false;
      ime_delay = false;
    }
  }
}

void Z80::tock_b(const uint8_t imask_, const uint8_t intf_, const Ack& ack) {
#ifdef FUZZ_TEST
  uint8_t z;
  {
    static uint32_t r = 1;
    r *= 0x1234567;
    r ^= r >> 16;
    z = uint8_t(r);
  }
#else
  const uint8_t z = 0;
#endif

  //--------------------------------------------------------------------------------

  if (INT) {

    if (state == 0) {
      set_addr(sp, 0);
      state_ = 1;
    }

    if (state == 1) {
      spl = dec(spl, 1);
      out = pch;
      sph = dec(sph, inc_c);
      set_addr(sp, 1);
      state_ = 2;
    }
    
    if (state == 2) {
      spl = dec(spl, 1);
      out = pcl;
      sph = dec(sph, inc_c);
      set_addr(sp, 1);
      state_ = 3;
    }

    if (state == 3) {
      x = 0;
      set_addr(xy, 0);
      state_ = 4;
    }

    if (state == 4) {
      if      (imask_ & intf_ & INT_JOYPAD_MASK) { int_ack = INT_JOYPAD_MASK; }
      else if (imask_ & intf_ & INT_SERIAL_MASK) { int_ack = INT_SERIAL_MASK; }
      else if (imask_ & intf_ & INT_TIMER_MASK)  { int_ack = INT_TIMER_MASK; }
      else if (imask_ & intf_ & INT_STAT_MASK)   { int_ack = INT_STAT_MASK; }
      else if (imask_ & intf_ & INT_VBLANK_MASK) { int_ack = INT_VBLANK_MASK; }
      else                                     { int_ack = 0; }

      uint8_t int_addr = 0;
      if      (imask_ & intf_ & INT_JOYPAD_MASK) { int_addr = 0x60; }
      else if (imask_ & intf_ & INT_SERIAL_MASK) { int_addr = 0x58; }
      else if (imask_ & intf_ & INT_TIMER_MASK)  { int_addr = 0x50; }
      else if (imask_ & intf_ & INT_STAT_MASK)   { int_addr = 0x48; }
      else if (imask_ & intf_ & INT_VBLANK_MASK) { int_addr = 0x40; }
      else                                     { int_addr = 0x00; }

      y = int_addr;
      set_addr(xy, 0);
      state_ = 0;
    }

  }                                                                                                                                                                                                                             
  else if (HALT) {                                                                                                                                                                                                              
    bool no_halt = ((imask_ & intf_) && !ime);                                                                                                                                                                                    
    if (HALT && state == 0) unhalt = 0;                                                                                                                                                                                         

    if (state == 0 && HALT)                   /**/ {                      pcl = adl = inc(pcl, 1);  /**/                              pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = !no_halt; }
    if (state == 1 && HALT)                   /**/ {                                                /**/                                                           set_addr(pc, 0); /**/                                                  state_ = !unhalt; }
  }                                                                                                                                                                                                                             
  else if (PREFIX_CB) {                                                                                                                                                                                                         
    if (state == 1) cb = in;                                                                                                                                                                                                    
    uint8_t mask = cb_flag_mask[CB_QUAD];                                                                                                                                                                                       

    if (state == 0)                           /**/ {                      pcl = adl = inc(pcl, 1);  /**/                              pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                              state_ = 1; }
    if (state == 1 && OP_CB_R)                /**/ { alu_x = reg(CB_COL); pcl = adl = inc(pcl, 1);  /**/ reg(CB_COL) = alu_cb(cb, f); pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                 set_f(mask); state_ = 0; }
    if (state == 1 && OP_CB_HL)               /**/ {                      pcl = adl = inc(pcl, 1);  /**/                              pch = adh = inc(pch, inc_c); set_addr(hl, 0); /**/                                                              state_ = 2; }
    if (state == 2)                           /**/ { alu_x = in;                                    /**/ out = alu_cb(cb, f);                                      set_addr(hl, 1); /**/                                                 set_f(mask); state_ = 3; }
    if (state == 3)                           /**/ {                                                /**/                                                           set_addr(pc, 0); /**/                                                              state_ = 0; }
  }                                                                                                                                                                                                    
  else {                                                                                                                                                                                               
    bool branch = false;                                                                                                                                                                               
    switch (OP_ROW & 3) {                                                                                                                                                                              
    case 0: branch = !(f & F_ZERO); break;                                                                                                                                                             
    case 1: branch =  (f & F_ZERO); break;                                                                                                                                                             
    case 2: branch = !(f & F_CARRY); break;                                                                                                                                                            
    case 3: branch =  (f & F_CARRY); break;                                                                                                                                                            
    }                                                                                                                                                                                                  

    uint8_t bus = 0xDD;

    // IN OUT BC DE HL AF XY | PC SP AD

    // misc

    if (state == 0 && NOP)                    /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 0 && STOP)                   /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 0 && DI)                     /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 0 && EI)                     /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 0 && MV_R_R)                 /**/ { R_ROW = R_COL;    pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                              
    if (state == 0 && LD_SP_HL)               /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LD_SP_HL)               /**/ { DBUS_BUSY;        spl = l;                  /**/ DBUS_BUSY;       sph = h;                     set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                              
    if (state == 0 && LD_R_D8)                /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LD_R_D8)                /**/ { R_ROW = in;       pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                              
    // 8-bit alu                                                                                                                                                                                              
                                                                                                                                                                                                              
    if (state == 0 && ALU_A_R)                /**/ { alu_x = a;        pcl = adl = inc(pcl, 1);  /**/ alu_y = R_COL;   pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ a = alu(OP_ROW, f);                 set_f(0xF0); state_ = 0; }
    if (state == 0 && INC_R)                  /**/ { alu_x = R_ROW;    pcl = adl = inc(pcl, 1);  /**/ alu_y = 1;       pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ R_ROW = alu(0, f);                  set_f(0xE0); state_ = 0; }
    if (state == 0 && DEC_R)                  /**/ { alu_x = R_ROW;    pcl = adl = inc(pcl, 1);  /**/ alu_y = 1;       pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ R_ROW = alu(2, f);                  set_f(0xE0); state_ = 0; }
    if (state == 0 && RLC_A)                  /**/ { alu_x = a;        pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ a = rlu(OP_ROW, f);                 set_f(0xF0); state_ = 0; }
    if (state == 0 && RRC_A)                  /**/ { alu_x = a;        pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ a = rlu(OP_ROW, f);                 set_f(0xF0); state_ = 0; }
    if (state == 0 && RL_A)                   /**/ { alu_x = a;        pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ a = rlu(OP_ROW, f);                 set_f(0xF0); state_ = 0; }
    if (state == 0 && RR_A)                   /**/ { alu_x = a;        pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ a = rlu(OP_ROW, f);                 set_f(0xF0); state_ = 0; }
    if (state == 0 && DAA)                    /**/ { alu_x = a;        pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ a = rlu(OP_ROW, f);                 set_f(0xB0); state_ = 0; }
    if (state == 0 && CPL)                    /**/ { alu_x = a;        pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ a = rlu(OP_ROW, f);                 set_f(0x60); state_ = 0; }
    if (state == 0 && SCF)                    /**/ { alu_x = a;        pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ a = rlu(OP_ROW, f);                 set_f(0x70); state_ = 0; }
    if (state == 0 && CCF)                    /**/ { alu_x = a;        pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ a = rlu(OP_ROW, f);                 set_f(0x70); state_ = 0; }
                                                                                                                                                                                                              
    if (state == 0 && ALU_A_D8)               /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 1 && ALU_A_D8)               /**/ { alu_y = in;       pcl = adl = inc(pcl, 1);  /**/ alu_x = a;       pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/ a = alu(OP_ROW, f);                 set_f(0xF0); state_ = 0; }
                                                                                                                                                                                                              
    if (state == 0 && ALU_A_HL)               /**/ { DBUS_BUSY;        adl = l;                  /**/ DBUS_BUSY;       adh = h;                     set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 1 && ALU_A_HL)               /**/ { alu_y = in;       pcl = adl = inc(pcl, 1);  /**/ alu_x = a;       pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/ a = alu(OP_ROW, f);                 set_f(0xF0); state_ = 0; }
                                                                      

    if (state == 0 && INC_AT_HL)              /**/ { DBUS_BUSY;        adl = l;                  /**/ DBUS_BUSY;       adh = h;                     set_addr(ad, 0); /**/ alu_y = 1;                                       state_ = 1; }
    if (state == 0 && DEC_AT_HL)              /**/ { DBUS_BUSY;        adl = l;                  /**/ DBUS_BUSY;       adh = h;                     set_addr(ad, 0); /**/ alu_y = 1;                                       state_ = 1; }
    if (state == 1 && INC_AT_HL)              /**/ { alu_x = in;                                 /**/ out = alu(0, f);                              set_addr(ad, 1); /**/                                     set_f(0xE0); state_ = 2; }
    if (state == 1 && DEC_AT_HL)              /**/ { alu_x = in;                                 /**/ out = alu(2, f);                              set_addr(ad, 1); /**/                                     set_f(0xE0); state_ = 2; }
    if (state == 2 && INC_AT_HL)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 2 && DEC_AT_HL)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                       
    // 16-bit alu                                                                                                      
                                                                                                                       
    if (state == 0 && ADD_SP_R8)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 1 && ADD_SP_R8)              /**/ { alu_x = in;                                 /**/ alu_y = spl;     ABUS_BUSY;                   set_addr(ad, 0); /**/ DBUS_BUSY;         spl = alu(0, f); set_f(0xF0); state_ = 2; }
    if (state == 2 && ADD_SP_R8)              /**/ { alu_x = sxt(in);                            /**/ alu_y = sph;     ABUS_BUSY;                   set_addr(ad, 0); /**/ DBUS_BUSY;         sph = alu(1, f);              state_ = 3; }
    if (state == 3 && ADD_SP_R8)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                      
    if (state == 0 && LD_HL_SP_R8)            /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LD_HL_SP_R8)            /**/ { alu_x = in;                                 /**/ alu_y = spl;     ABUS_BUSY;                   set_addr(ad, 0); /**/ l = alu(0, f);                      set_f(0xF0); state_ = 2; }
    if (state == 2 && LD_HL_SP_R8)            /**/ { alu_x = sxt(in);                            /**/ alu_y = sph;     ABUS_BUSY;                   set_addr(ad, 0); /**/ h = alu(1, f);                                   state_ = 3; }
    if (state == 3 && LD_HL_SP_R8)            /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                     
    if (state == 0 && INC_BC)                 /**/ {                           c = inc(  c, 1);  /**/                          b = inc(  b, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 0 && DEC_BC)                 /**/ {                           c = dec(  c, 1);  /**/                          b = dec(  b, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 0 && INC_DE)                 /**/ {                           e = inc(  e, 1);  /**/                          d = inc(  d, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 0 && DEC_DE)                 /**/ {                           e = dec(  e, 1);  /**/                          d = dec(  d, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 0 && INC_HL)                 /**/ {                           l = inc(  l, 1);  /**/                          h = inc(  h, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 0 && DEC_HL)                 /**/ {                           l = dec(  l, 1);  /**/                          h = dec(  h, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 0 && INC_SP)                 /**/ {                         spl = inc(spl, 1);  /**/                        sph = inc(sph, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 0 && DEC_SP)                 /**/ {                         spl = dec(spl, 1);  /**/                        sph = dec(sph, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }

    if (state == 1 && INC_BC)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 1 && DEC_BC)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 1 && INC_DE)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 1 && DEC_DE)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 1 && INC_HL)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 1 && DEC_HL)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 1 && INC_SP)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    if (state == 1 && DEC_SP)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                       
    // load/store                                                                                                      
                                                                                                                       
    if (state == 0 && STM_A16_SP)             /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/ out = spl;         ABUS_BUSY;                    state_ = 1; }
    if (state == 1 && STM_A16_SP)             /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/ x   = sph;         ABUS_BUSY;                    state_ = 2; }
    if (state == 2 && STM_A16_SP)             /**/ { DBUS_BUSY;              adl = y;            /**/ DBUS_BUSY;             adh = in;              set_addr(ad, 1); /**/                                                  state_ = 3; }
    if (state == 3 && STM_A16_SP)             /**/ { out = x;                adl = inc(adl, 1);  /**/                        adh = inc(adh, inc_c); set_addr(ad, 1); /**/                                                  state_ = 4; }
    if (state == 4 && STM_A16_SP)             /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                       
    if (state == 0 && STM_A16_A)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/ out = a;                                         state_ = 1; }
    if (state == 1 && STM_A16_A)              /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 2; }
    if (state == 2 && STM_A16_A)              /**/ { DBUS_BUSY;              adl = y;            /**/ DBUS_BUSY;             adh = in;              set_addr(ad, 1); /**/                                                  state_ = 3; }
    if (state == 3 && STM_A16_A)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                       
    // so xy has to be on the left side?
    // no i think it's on the right side w/ the new bus model
                                                                                                                       
    if (state == 0 && LDM_A_A16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LDM_A_A16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/ DBUS_BUSY;         y = in;                       state_ = 2; }
    if (state == 2 && LDM_A_A16)              /**/ { DBUS_BUSY;              adl = y;            /**/ DBUS_BUSY;             adh = in;              set_addr(ad, 0); /**/                                                  state_ = 3; }
    if (state == 3 && LDM_A_A16)              /**/ { a = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }

    // indirect load/store

    if (state == 0 && LDM_A_BC)               /**/ { DBUS_BUSY;        adl = c;                  /**/ DBUS_BUSY;       adh = b;                     set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LDM_A_BC)               /**/ { a = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && LDM_A_DE)               /**/ { DBUS_BUSY;        adl = e;                  /**/ DBUS_BUSY;       adh = d;                     set_addr(ad, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LDM_A_DE)               /**/ { a = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
    
    // hmmmmmmm ok this is problematic. we need two moves to get bc into ad, and one to get a into out? but we only have two cycles to do it... unless every instruction leaves out in a?

    if (state == 0 && STM_BC_A)               /**/ { out = a;          adl = c;                  /**/ adh = b;                                      set_addr(ad, 1); /**/                                                  state_ = 1; }
    if (state == 1 && STM_BC_A)               /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && STM_DE_A)               /**/ { out = a;          adl = e;                  /**/ adh = d;                                      set_addr(ad, 1); /**/                                                  state_ = 1; }
    if (state == 1 && STM_DE_A)               /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(ad, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && LDM_A_HLP)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(hl, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LDM_A_HLP)              /**/ { a = in;                   l = inc(  l, 1);  /**/                          h = inc(  h, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; }

    // somewhat same thing here. how do we increment/decrement hl and get the result back to hl in time and also have time to increment pc?
    // maybe xy is on the right side?

    if (state == 0 && LDM_A_HLM) {
      adl = y = l;
      adh = x = h;
      set_addr(ad, 0);
      l = dec(y, 1);
      state_ = 1;
    }
    if (state == 1 && LDM_A_HLM) {
      a = in; pcl = adl = inc(pcl, 1);
              pch = adh = inc(pch, inc_c);
      set_addr(ad, 0);
      h = dec(x, inc_c);
      state_ = 0;
    }
                                                                                                                                                                                                                          
    if (state == 0 && STM_HLP_A)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/ out = a;        pch = adh = inc(pch, inc_c); set_addr(hl, 1); /**/                                                  state_ = 1; }
    if (state == 1 && STM_HLP_A)              /**/ {                           l = inc(  l, 1);  /**/                         h = inc(  h, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && STM_HLM_A) {
      out = a;
      adl = y = l;
      adh = x = h;
      set_addr(ad, 1);
      l = dec(y, 1);
      state_ = 1;
    }
    if (state == 1 && STM_HLM_A) {
      pcl = adl = inc(pcl, 1);
      pch = adh = inc(pch, inc_c);
      set_addr(ad, 0);
      h = dec(x, inc_c);
      state_ = 0;
    }
                                                                                                                                                                                                                          
    if (state == 0 && LDM_R_HL)               /**/ {                   pcl = adl = inc(pcl, 1);  /**/                 pch = adh = inc(pch, inc_c); set_addr(hl, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LDM_R_HL)               /**/ {                                             /**/                                              set_addr(pc, 0); /**/ reg(OP_ROW) = in;                                state_ = 0; }

    if (state == 0 && STM_HL_R) {
      ad = abus = dbus = hl;
      out = dbl = reg(OP_COL);
      set_addr(ad, 1);
      state_ = 1;
    }
    if (state == 1 && STM_HL_R) {
      pcl = adl = inc(pcl, 1);
      pch = adh = inc(pch, inc_c);
      set_addr(pc, 0);
      state_ = 0;
    }
                                                                                                                                                                                                                          
    if (state == 0 && STM_HL_D8)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && STM_HL_D8)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(hl, 1); /**/                                                  state_ = 2; }
    if (state == 2 && STM_HL_D8)              /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    // zero-page load/store                                                                                                                                                                                               
                                                                                                                                                                                                                          
    if (state == 0 && LDM_A_A8)               /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LDM_A_A8)               /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(xy, 0); /**/                                                  state_ = 2; }
    if (state == 2 && LDM_A_A8)               /**/ { a = in;                                     /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && STM_A8_A)               /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && STM_A8_A)               /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/ out = a;         pch = adh = inc(pch, inc_c); set_addr(xy, 1); /**/                                                  state_ = 2; }
    if (state == 2 && STM_A8_A)               /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && LDM_A_C)                /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(bc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LDM_A_C)                /**/ { a = in;                                     /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && STM_C_A)                /**/ {                   pcl = adl = inc(pcl, 1);  /**/ out = a;         pch = adh = inc(pch, inc_c); set_addr(bc, 1); /**/                                                  state_ = 1; }
    if (state == 1 && STM_C_A)                /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    // push / pop                                                                                                                                                                                                         
                                                                                                                                                                                                                          
    if (state == 0 && PUSH_BC)                /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 0 && PUSH_DE)                /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 0 && PUSH_HL)                /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 0 && PUSH_AF)                /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 0 && POP_BC)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 0 && POP_DE)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 0 && POP_HL)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 0 && POP_AF)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 1 && PUSH_BC)                /**/ {                   spl = adl = dec(spl, 1);  /**/ out = b;         sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 2; }
    if (state == 1 && PUSH_DE)                /**/ {                   spl = adl = dec(spl, 1);  /**/ out = d;         sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 2; }
    if (state == 1 && PUSH_HL)                /**/ {                   spl = adl = dec(spl, 1);  /**/ out = h;         sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 2; }
    if (state == 1 && PUSH_AF)                /**/ {                   spl = adl = dec(spl, 1);  /**/ out = a;         sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 2; }
    if (state == 2 && PUSH_BC)                /**/ {                   spl = adl = dec(spl, 1);  /**/ out = c;         sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 3; }
    if (state == 2 && PUSH_DE)                /**/ {                   spl = adl = dec(spl, 1);  /**/ out = e;         sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 3; }
    if (state == 2 && PUSH_HL)                /**/ {                   spl = adl = dec(spl, 1);  /**/ out = l;         sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 3; }
    if (state == 2 && PUSH_AF)                /**/ {                   spl = adl = dec(spl, 1);  /**/ out = f;         sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 3; }
    if (state == 3 && PUSH_BC)                /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; }
    if (state == 3 && PUSH_DE)                /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; }
    if (state == 3 && PUSH_HL)                /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; }
    if (state == 3 && PUSH_AF)                /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 1 && POP_BC)                 /**/ { c = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(sp, 0); /**/                                                  state_ = 2; }
    if (state == 1 && POP_DE)                 /**/ { e = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(sp, 0); /**/                                                  state_ = 2; }
    if (state == 1 && POP_HL)                 /**/ { l = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(sp, 0); /**/                                                  state_ = 2; }
    if (state == 1 && POP_AF)                 /**/ { f = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(sp, 0); /**/                                                  state_ = 2; }
    if (state == 2 && POP_BC)                 /**/ { b = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; }
    if (state == 2 && POP_DE)                 /**/ { d = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; }
    if (state == 2 && POP_HL)                 /**/ { h = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; }
    if (state == 2 && POP_AF)                 /**/ { a = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    // 16-bit stuff                                                                                                                                                                                                       
                                                                                                                                                                                                                          
    if (state == 0 && LD_BC_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 0 && LD_DE_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 0 && LD_HL_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LD_BC_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ c = in;                                          state_ = 2; }
    if (state == 1 && LD_DE_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ e = in;                                          state_ = 2; }
    if (state == 1 && LD_HL_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ l = in;                                          state_ = 2; }
    if (state == 2 && LD_BC_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ b = in;                                          state_ = 0; }
    if (state == 2 && LD_DE_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ d = in;                                          state_ = 0; }
    if (state == 2 && LD_HL_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ h = in;                                          state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && LD_SP_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && LD_SP_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ spl = in;                                        state_ = 2; }
    if (state == 2 && LD_SP_D16)              /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ sph = in;                                        state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && ADD_HL_BC)              /**/ { alu_x = c;        pcl = adl = inc(pcl, 1);  /**/ alu_y = l;       pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ l = alu(0, f);                                   state_ = 1; }
    if (state == 0 && ADD_HL_DE)              /**/ { alu_x = e;        pcl = adl = inc(pcl, 1);  /**/ alu_y = l;       pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ l = alu(0, f);                                   state_ = 1; }
    if (state == 0 && ADD_HL_HL)              /**/ { alu_x = l;        pcl = adl = inc(pcl, 1);  /**/ alu_y = l;       pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ l = alu(0, f);                                   state_ = 1; }
    if (state == 0 && ADD_HL_SP)              /**/ { alu_x = spl;      pcl = adl = inc(pcl, 1);  /**/ alu_y = l;       pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ l = alu(0, f);                                   state_ = 1; }
    if (state == 1 && ADD_HL_BC)              /**/ { alu_x = b;                                  /**/ alu_y = h;                                    set_addr(pc, 0); /**/ h = alu(1, alu_f);  set_f(0x70);                 state_ = 0; }
    if (state == 1 && ADD_HL_DE)              /**/ { alu_x = d;                                  /**/ alu_y = h;                                    set_addr(pc, 0); /**/ h = alu(1, alu_f);  set_f(0x70);                 state_ = 0; }
    if (state == 1 && ADD_HL_HL)              /**/ { alu_x = h;                                  /**/ alu_y = h;                                    set_addr(pc, 0); /**/ h = alu(1, alu_f);  set_f(0x70);                 state_ = 0; }
    if (state == 1 && ADD_HL_SP)              /**/ { alu_x = sph;                                /**/ alu_y = h;                                    set_addr(pc, 0); /**/ h = alu(1, alu_f);  set_f(0x70);                 state_ = 0; }
                                                                                                                                                                                                                          
    // conditional branches                                                                                                                                                                                               
                                                                                                                                                                                                                          
    if (state == 0 && JR_CC_R8    &&  branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ alu_x = pcl + 1;                                 state_ = 1; }
    if (state == 1 && JR_CC_R8    &&  branch) /**/ { alu_y = in;                                 /**/ y = alu(0, f);                                set_addr(pc, 0); /**/ alu_x = pch;                                     state_ = 2; }
    if (state == 2 && JR_CC_R8    &&  branch) /**/ { alu_y = sxt(in);  pcl = adl = inc(pcl, 1);  /**/ x = alu(1, alu_f); pch = adh = inc(pch, inc_c); set_addr(xy, 0); /**/                                                state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && JR_CC_R8    && !branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ alu_x = pcl + 1;                                 state_ = 1; }
    if (state == 1 && JR_CC_R8    && !branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && JR_R8)                  /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/ alu_x = pcl + 1;                                 state_ = 1; }
    if (state == 1 && JR_R8)                  /**/ { alu_y = in;                                 /**/ y = alu(0, f);                                set_addr(pc, 0); /**/ alu_x = pch;                                     state_ = 2; }                                                         
    if (state == 2 && JR_R8)                  /**/ { alu_y = sxt(in);  pcl = adl = inc(pcl, 1);  /**/ x = alu(1, alu_f); pch = adh = inc(pch, inc_c); set_addr(xy, 0); /**/                                                state_ = 0; }
                                                                                                                                                                                                                          
                                                                                                                                                                                                                          
    if (state == 0 && JP_CC_A16   &&  branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 0 && JP_CC_A16   && !branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && JP_CC_A16   &&  branch) /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 2; }
    if (state == 1 && JP_CC_A16   && !branch) /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 2; }
    if (state == 2 && JP_CC_A16   &&  branch) /**/ { x = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 3; }
    if (state == 2 && JP_CC_A16   && !branch) /**/ { x = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; out = a; }
    if (state == 3 && JP_CC_A16   &&  branch) /**/ {                   pcl = y;                  /**/                                    pch = x;   set_addr(pc, 0); /**/                                                  state_ = 0; out = a; }
                                                                                                                                                                                                                          
    if (state == 0 && JP_A16)                 /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && JP_A16)                 /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 2; }
    if (state == 2 && JP_A16)                 /**/ { x = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 3; }
    if (state == 3 && JP_A16)                 /**/ {                   pcl = y;                  /**/                                    pch = x;   set_addr(pc, 0); /**/                                                  state_ = 0; /*out = a;*/ } // why does this break things?
                                                                                                                                                                                                                          
    if (state == 0 && CALL_CC_A16 &&  branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && CALL_CC_A16 &&  branch) /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 2; }
    if (state == 2 && CALL_CC_A16 &&  branch) /**/ { x = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/ out = pch;                                       state_ = 3; }
    if (state == 3 && CALL_CC_A16 &&  branch) /**/ {                   spl = adl = dec(spl, 1);  /**/                  sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/ out = pcl;                                       state_ = 4; }
    if (state == 4 && CALL_CC_A16 &&  branch) /**/ {                   spl = adl = dec(spl, 1);  /**/                  sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 5; }
    if (state == 5 && CALL_CC_A16 &&  branch) /**/ {                   pcl = y;                  /**/                                    pch = x;   set_addr(pc, 0); /**/                                                  state_ = 0; }
                                                                                                                                                                                                                          
    if (state == 0 && CALL_CC_A16 && !branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && CALL_CC_A16 && !branch) /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 2; }
    if (state == 2 && CALL_CC_A16 && !branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; out = a; }
                                                                                                                                                                                                                          
    // ok we need to latch out on set_addr                                                                                                                                                                                
                                                                                                                                                                                                                          
    if (state == 0 && CALL_A16)               /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 1 && CALL_A16)               /**/ { y = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 2; }
    if (state == 2 && CALL_A16)               /**/ { x = in;           pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/ out = pch;                                       state_ = 3; }
    if (state == 3 && CALL_A16)               /**/ {                   spl = adl = dec(spl, 1);  /**/                  sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/ out = pcl;                                       state_ = 4; }
    if (state == 4 && CALL_A16)               /**/ {                   spl = adl = dec(spl, 1);  /**/                  sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 5; }
    if (state == 5 && CALL_A16)               /**/ {                                             /**/                                               set_addr(xy, 0); /**/                                                  state_ = 0; out = a; }
                                                                                                                                                                                                                          
    if (state == 0 && RET_CC      &&  branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 0 && RET_CC      && !branch) /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(pc, 0); /**/                                                  state_ = 1; }
    if (state == 0 && RETI)                   /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 1 && RET_CC      &&  branch) /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 2; }
    if (state == 1 && RET_CC      && !branch) /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; out = a; }
    if (state == 1 && RETI)                   /**/ { pcl = in;         spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(sp, 0); /**/                                                  state_ = 2; }
    if (state == 2 && RET_CC      &&  branch) /**/ {                                             /**/                                               set_addr(sp, 0); /**/                                                  state_ = 3; }
    if (state == 2 && RETI)                   /**/ { pch = in;         spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c);                  /**/                                                  state_ = 3; }
    if (state == 3 && RET_CC      &&  branch) /**/ { y = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(sp, 0); /**/                                                  state_ = 4; }
    if (state == 3 && RETI)                   /**/ {                                             /**/                                               set_addr(pc, 0); /**/                                                  state_ = 0; out = a; }
    if (state == 4 && RET_CC      &&  branch) /**/ { x = in;           spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(xy, 0); /**/                                                  state_ = 0; out = a; }
                                                                                                                                                                                                                          
    if (state == 0 && RET)                    /**/ {                   pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 1 && RET)                    /**/ { pcl = in;         spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(sp, 0); /**/                                                  state_ = 2; }
    if (state == 2 && RET)                    /**/ { pch = in;                                   /**/                                                                /**/                                                  state_ = 3; }
    if (state == 3 && RET)                    /**/ {                   spl = adl = inc(spl, 1);  /**/                  sph = adh = inc(sph, inc_c); set_addr(pc, 0); /**/                                                  state_ = 0; out = a; }
                                                                                                                                                                                                                          
    // branches                                                                                                                                                                                                           
                                                                                                                                                                                                                          
    if (state == 0 && JP_HL)                  /**/ {                   pcl = l;                  /**/                                    pch = h;   set_addr(pc, 0); /**/                                                  state_ = 0; out = a; }
                                                                                                                                                                                                                          
    if (state == 0 && RST_NN)                 /**/ { alu_y = 0x38;     pcl = adl = inc(pcl, 1);  /**/                  pch = adh = inc(pch, inc_c); set_addr(sp, 0); /**/                                                  state_ = 1; }
    if (state == 1 && RST_NN)                 /**/ {                   spl = adl = dec(spl, 1);  /**/ out = pch;       sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/                                                  state_ = 2; }
    if (state == 2 && RST_NN)                 /**/ {                   spl = adl = dec(spl, 1);  /**/ out = pcl;       sph = adh = dec(sph, inc_c); set_addr(sp, 1); /**/ x = 0x00;                                        state_ = 3; }
    if (state == 3 && RST_NN)                 /**/ { alu_x = op;                                 /**/ y = alu(4, f);                                set_addr(xy, 0); /**/                                                  state_ = 0; out = a; }
  }

  f &= 0xF0;

  if (RETI && state_ == 0) {ime = true;       ime_delay = true;}
  if (DI)                  {ime = false;      ime_delay = false;}
  if (EI)                  {ime = ime_delay;  ime_delay = true;}

  if (state_ == 0) {
    pc = ad;
  }
}

//-----------------------------------------------------------------------------

void Z80::tick_c(const uint8_t imask_, const uint8_t intf_, const Ack& ack) {
  // Z80 idle this cycle
}

void Z80::tock_d(const uint8_t imask_, const uint8_t intf_, const Ack& ack) {
  // Z80 idle this cycle
}

//-----------------------------------------------------------------------------

void Z80::tick_e(const uint8_t imask_, const uint8_t intf_, const Ack& ack) {
}

void Z80::tock_f(const uint8_t imask_, const uint8_t intf_, const Ack& ack) {
  // "first" execution cycle
  bus_ack = ack;
  in = (uint8_t)ack.data;
}

//-----------------------------------------------------------------------------

void Z80::tick_g(const uint8_t imask_, const uint8_t intf_, const Ack& ack) {
}

void Z80::tock_h(const uint8_t imask_, const uint8_t intf_, const Ack& ack) {
  // "second" execution cycle
}

//-----------------------------------------------------------------------------

uint8_t& Z80::reg(int mux) {
  switch(mux) {
  case 0: return b;
  case 1: return c;
  case 2: return d;
  case 3: return e;
  case 4: return h;
  case 5: return l;
  case 6: return in;     
  case 7: return a;
  }
  static uint8_t fail = 0;
  return fail;
}

//-----------------------------------------------------------------------------

void Z80::set_f(uint8_t mask) {
  f = (f & ~mask) | (alu_f & mask);
  if (ADD_SP_R8)   { f &= ~(F_ZERO | F_NEGATIVE); }
  if (LD_HL_SP_R8) { f &= ~(F_ZERO | F_NEGATIVE); }
}

//-----------------------------------------------------------------------------
#pragma warning(disable : 4458)

uint8_t Z80::alu(int op, uint8_t flags) {
  alu_f = flags;
  const uint8_t x = alu_x;
  const uint8_t y = alu_y;

  uint16_t d1 = 0, d2 = 0;
  const bool c = alu_f & F_CARRY;

  switch(op) {
  case 0: d1 = (x & 0xF) + (y & 0xF);     d2 = x + y;     break;
  case 1: d1 = (x & 0xF) + (y & 0xF) + c; d2 = x + y + c; break;
  case 2: d1 = (x & 0xF) - (y & 0xF);     d2 = x - y;     break;
  case 3: d1 = (x & 0xF) - (y & 0xF) - c; d2 = x - y - c; break;
  case 4: d1 = 0xFFF;                     d2 = x & y;     break;
  case 5: d1 = 0x000;                     d2 = x ^ y;     break;
  case 6: d1 = 0x000;                     d2 = x | y;     break;
  case 7: d1 = (x & 0xF) - (y & 0xF);     d2 = x - y;     break;
  }

  alu_o = (uint8_t)d2;
  alu_f = (op == 2 || op == 3 || op == 7) ? F_NEGATIVE : 0;

  if (d1 & 0x010) alu_f |= F_HALF_CARRY;
  if (d2 & 0x100) alu_f |= F_CARRY;
  if (alu_o == 0x000) alu_f |= F_ZERO;
  if (op == 7)  alu_o = x;

  return alu_o;
}

//-----------------------------------------------------------------------------
// The logic is more annoying, but this can be implemented as two 4-bit additions

uint8_t Z80::daa(uint8_t x, uint8_t f) {
  bool c = f & F_CARRY;
  bool h = f & F_HALF_CARRY;
  bool n = f & F_NEGATIVE;

  // low nibble
  uint8_t lo = (x >> 0) & 0xF;
  bool o = lo > 9;
  uint8_t d = 0;
  if (+h || +o) d = 0x6;
  if (+h && +n) d = 0xA;
  if (!h && +n) d = 0x0;
  lo += d;

  // high nibble
  uint8_t hi = (x >> 4) & 0xF;
  o = (lo >> 4) ? (hi > 8) : (hi > 9);
  d = 0;
  if (+n && (!h && +c)) d = 0xA;
  if (+n && (+h && !c)) d = 0xF;
  if (+n && (+h && +c)) d = 0x9;
  if (!n && (+o || +c)) d = 0x6;
  hi += d + (lo >> 4);

  // output
  alu_o = uint8_t((hi << 4) | (lo & 0xF));
  alu_f = 0;
  if (c) alu_f |= F_CARRY;
  if ((hi >> 4) && !n) alu_f |= F_CARRY;
  if (!alu_o) alu_f |= F_ZERO;
  return alu_o;
}

//-----------------------------------------------------------------------------

uint8_t Z80::rlu(int op, uint8_t flags) {
  alu_f = flags;
  const uint8_t x = alu_x;
  const uint8_t f = alu_f;

  switch (op) {
  case 0:
    alu_o = (x << 1) | (x >> 7);
    alu_f = (x >> 7) ? F_CARRY : 0;
    alu_f &= ~F_ZERO;
    break;
  case 1:
    alu_o = (x >> 1) | (x << 7);
    alu_f = (x & 1) ? F_CARRY : 0;
    alu_f &= ~F_ZERO;
    break;
  case 2:
    alu_o = (x << 1) | (f & F_CARRY ? 1 : 0);
    alu_f = (x >> 7) ? F_CARRY : 0;
    alu_f &= ~F_ZERO;
    break;
  case 3:
    alu_o = (x >> 1) | ((f & F_CARRY ? 1 : 0) << 7);
    alu_f = (x & 1) ? F_CARRY : 0;
    alu_f &= ~F_ZERO;
    break;
  case 4:
    daa(x, f);
    if (!alu_o) alu_f |= F_ZERO;
    break;
  case 5:
    alu_o = ~x;
    alu_f = f | 0x60;
    if (!alu_o) alu_f |= F_ZERO;
    break;
  case 6:
    alu_o = x;
    alu_f = (f & 0x80) | 0x10;
    if (!alu_o) alu_f |= F_ZERO;
    break;
  case 7:
    alu_o = x;
    alu_f = (f & 0x90) ^ 0x10;
    if (!alu_o) alu_f |= F_ZERO;
    break;
  }

  return alu_o;
}

//-----------------------------------------------------------------------------

uint8_t Z80::alu_cb(int op, uint8_t flags) {
  alu_f = flags;
  const uint8_t x = alu_x;
  const uint8_t quad = ((op >> 6) & 3);
  const uint8_t row = ((op >> 3) & 7);
  const bool bit_mux = (x >> row) & 1;

  switch (quad) {
  case 0:
    switch (row) {
    case 0:
      alu_o = (x << 1) | (x >> 7);
      alu_f = (x >> 7) ? F_CARRY : 0;
      break;
    case 1:
      alu_o = (x >> 1) | (x << 7);
      alu_f = (x & 1) ? F_CARRY : 0;
      break;
    case 2:
      alu_o = (x << 1) | (f & F_CARRY ? 1 : 0);
      alu_f = (x >> 7) ? F_CARRY : 0;
      break;
    case 3:
      alu_o = (x >> 1) | ((f & F_CARRY ? 1 : 0) << 7);
      alu_f = (x & 1) ? F_CARRY : 0;
      break;
    case 4:
      alu_o = (x << 1) & 0xFF;
      alu_f = 0;
      if (x >> 7)     alu_f |= F_CARRY;
      break;
    case 5:
      alu_o = ((x >> 1) | (x & 0x80)) & 0xFF;
      alu_f = 0;
      if (x & 1)      alu_f |= F_CARRY;
      break;
    case 6:
      alu_o = ((x << 4) | (x >> 4)) & 0xFF;
      alu_f = 0;
      break;
    case 7:
      alu_o = (x >> 1) & 0xFF;
      alu_f = 0;
      if (x & 1)      alu_f |= F_CARRY;
      break;
    }
    if (alu_o == 0) alu_f |= F_ZERO;
    break;
  case 1:
    alu_o = x;
    alu_f = (f & 0x10) | 0x20;
    if (!bit_mux) alu_f |= F_ZERO;
    break;
  case 2:
    alu_o = x & (~(1 << row));
    alu_f = f;
    break;
  case 3:
    alu_o = x | (1 << row);
    alu_f = f;
    break;
  }

  return alu_o;
}

//-----------------------------------------------------------------------------

#pragma warning(disable:4458)

void Z80::dump(std::string& o) {
  sprintf(o, "state       %d\n", state);
  sprintf(o, "\n");

  sprintf(o, "op_addr     0x%04x\n", op_addr);
  sprintf(o, "OP          0x%02x @ %d\n", op, state);
  sprintf(o, "CB          0x%02x\n", cb);
  sprintf(o, "\n");

  sprintf(o, "bus req     "); print_req(o, bus_req);
  sprintf(o, "bus ack     "); print_ack(o, bus_ack);
  sprintf(o, "\n");

  sprintf(o, "PC          0x%04x 0x%02x 0x%02x\n", pc, pcl, pch);
  sprintf(o, "SP          0x%04x 0x%02x 0x%02x\n", sp, sph, spl);
  sprintf(o, "XY          0x%04x 0x%02x 0x%02x\n", xy, x, y);
  sprintf(o, "BC          0x%04x 0x%02x 0x%02x\n", bc, b, c);
  sprintf(o, "DE          0x%04x 0x%02x 0x%02x\n", de, d, e);
  sprintf(o, "HL          0x%04x 0x%02x 0x%02x\n", hl, h, l);
  sprintf(o, "AF          0x%04x 0x%02x 0x%02x\n", af, a, f);
  sprintf(o, "alu_o       0x%02x\n", alu_o);
  sprintf(o, "alu_f       0x%02x\n", alu_f);
  sprintf(o, "\n");

  sprintf(o, "IME         %d\n", ime);
  sprintf(o, "IME_        %d\n", ime_delay);
  sprintf(o, "interrupt   %d\n", INT);
  sprintf(o, "int_ack     0x%02x\n", int_ack);
}

//-----------------------------------------------------------------------------
