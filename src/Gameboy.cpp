#include "Gameboy.h"

#include "Assembler.h"
#include "Constants.h"

#include <assert.h>
#include <imgui.h>

extern uint8_t rom_buf[];
extern const uint8_t DMG_ROM_bin[];

//-----------------------------------------------------------------------------

void Gameboy::reset(size_t new_rom_size, uint16_t new_pc) {
  *this = {};

  z80.reset(new_pc);
  cart.reset(new_rom_size);
  ppu.reset(new_pc == 0);
  oam.reset();
  spu.reset();
  timer.reset();
  vram.reset();
  iram.reset();
  joypad.reset();
  serial.reset();
  zram.reset();
  boot.reset(new_pc);

  phase = -2;
  trace_val = 0;

  intf = 0xE1;
  imask = 0x00;
  ack = {0};

  sentinel = 0xDEADBEEF;

  z80.get_bus_req(cpu_req);
  ebus_req = cpu_req;
}

void Gameboy::reset(uint16_t new_pc) {
  reset(cart.get_rom_size(), new_pc);
}

//-----------------------------------------------------------------------------

//#pragma warning(push)
#pragma warning(disable:4458)

void Gameboy::ibus_req2(Req ibus_req) {
  ack = {0};
  bool hit = (ibus_req.addr == ADDR_IF) || (ibus_req.addr == ADDR_IE);

  if (!hit) {
    return;
  }
  else if (ibus_req.write) {
    if (ibus_req.addr == ADDR_IF) intf  = (uint8_t)ibus_req.data | 0b11100000;
    if (ibus_req.addr == ADDR_IE) imask = (uint8_t)ibus_req.data;
    ack = {
      .addr  = ibus_req.addr,
      .data  = ibus_req.data,
      .read  = 0,
      .write = 1,
    };
  }
  else if (ibus_req.read) {
    uint8_t data = 0;
    if (ibus_req.addr == ADDR_IF) data = 0b11100000 | intf;
    if (ibus_req.addr == ADDR_IE) data = imask;
    ack = {
      .addr = ibus_req.addr,
      .data  = data,
      .read  = 1,
      .write = 0,
    };
  }
  else {
    assert(false);
  }
}

void Gameboy::ibus_ack2(Ack& ibus_ack) const {
  ibus_ack.addr  += ack.addr;
  ibus_ack.data  += ack.data;
  ibus_ack.read  += ack.read;
  ibus_ack.write += ack.write;
}

//#pragma warning(pop)

//-----------------------------------------------------------------------------

/*
void print_ack(std::string& d, const char* name, const Ack& ack) {
  sprintf(d, "%-8s %d 0x%04x 0x%02x %c %c\n",
          name,
          ack.phase,
          ack.addr,
          ack.read ? 'R' : '-',
          ack.write ? 'W' : '-');
}
*/

//-----------------------------------------------------------------------------

void Gameboy::tick2() {
  phase++;
  int64_t tcycle = (int64_t)floor(double(phase) / 2);
  int64_t tphase = tcycle & 3;

  //-----------------------------------
  // interrupts are partially asynchronous

  intf &= ~z80.get_int_ack();
  if (imask & 0x01) z80.unhalt |= ppu.vblank1;
  if (imask & 0x02) z80.unhalt |= ppu.stat2;
  if (imask & 0x04) z80.unhalt |= timer.get_interrupt();
  if (imask & 0x10) z80.unhalt |= joypad.get() != 0xFF;

  if (ppu.vblank1)             intf |= INT_VBLANK_MASK;
  if (ppu.stat1)               intf |= INT_STAT_MASK;
  if (timer.get_interrupt())   intf |= INT_TIMER_MASK;
  if (joypad.get() != 0xFF)    intf |= INT_JOYPAD_MASK;

  //-----------------------------------
  // gather reqs

  if (tphase == 2) {
    z80.get_bus_req(cpu_req);
  }

  dma.get_ebus_req(dma_ebus_req);
  dma.get_vbus_req(dma_vbus_req);
  dma.get_obus_req(dma_obus_req);

  ppu.get_vbus_req(ppu_vbus_req);
  ppu.get_obus_req(ppu_obus_req);

  //-----------------------------------
  // prioritize reqs

  bool cpu_has_vbus_req = cpu_req.addr >= ADDR_VRAM_BEGIN && cpu_req.addr <= ADDR_VRAM_END;
  bool cpu_has_obus_req = cpu_req.addr >= ADDR_OAM_BEGIN  && cpu_req.addr <= ADDR_OAM_END;
  bool cpu_has_ibus_req = cpu_req.addr >= ADDR_IOBUS_BEGIN;
  bool cpu_has_ebus_req = !cpu_has_vbus_req && !cpu_has_obus_req && !cpu_has_ibus_req;

  // ibus
  if (cpu_has_ibus_req) {
    ibus_req = cpu_req;
  }
  else {
    ibus_req = {};
  }

  // ebus
  if (dma_ebus_req.read) {
    ebus_req = dma_ebus_req;
  }
  else if (cpu_has_ebus_req) {
    ebus_req = cpu_req;
  }
  else {
    ebus_req = {};
  }

  // vbus
  if (dma_vbus_req.read) {
    vbus_req = dma_vbus_req;
  }
  else if (ppu_vbus_req.read) {
    vbus_req = ppu_vbus_req;
  }
  else if (cpu_has_vbus_req) {
    vbus_req = cpu_req;
  }
  else {
    vbus_req = {};
  }

  // obus
  if (dma_obus_req.write) {
    obus_req = dma_obus_req;
  }
  else if (ppu_obus_req.read) {
    obus_req = ppu_obus_req;
  }
  else if (cpu_has_obus_req) {
    obus_req = cpu_req;
  }
  else {
    obus_req = {};
  }

  //-----------------------------------
  // gather acks

  if (tphase == 0) {
    ibus_ack = {0};
    ebus_ack = {0};
    vbus_ack = {0};
    obus_ack = {0};

    if (ibus_req.read || ibus_req.write) {
      this->ibus_ack2(ibus_ack);
      ppu.   ibus_ack(ibus_ack);
      serial.ibus_ack(ibus_ack);
      joypad.ibus_ack(ibus_ack);
      zram.  ibus_ack(ibus_ack);
      timer. ibus_ack(ibus_ack);
      spu.   ibus_ack(ibus_ack);
      dma.   ibus_ack(ibus_ack);
      boot.  ibus_ack(ibus_ack);
    }

    if (ebus_req.read || ebus_req.write) {
      cart.ebus_ack(ebus_ack);
      iram.ebus_ack(ebus_ack);
    }

    if (vbus_req.read || vbus_req.write) {
      vram.vbus_ack(vbus_ack);
    }

    if (obus_req.read || obus_req.write) {
      oam.obus_ack(obus_ack);
    }

    if (ibus_ack.read  > 1) __debugbreak();
    if (ibus_ack.write > 1) __debugbreak();
    if (ebus_ack.read  > 1) __debugbreak();
    if (ebus_ack.write > 1) __debugbreak();
    if (vbus_ack.read  > 1) __debugbreak();
    if (vbus_ack.write > 1) __debugbreak();
    if (obus_ack.read  > 1) __debugbreak();
    if (obus_ack.write > 1) __debugbreak();

    if (cpu_has_ibus_req) cpu_ack = ibus_ack;
    if (cpu_has_ebus_req) cpu_ack = ebus_ack;
    if (cpu_has_vbus_req) cpu_ack = vbus_ack;
    if (cpu_has_obus_req) cpu_ack = obus_ack;
  }

  /*
  if (cpu_has_ibus_req) {
  cpu_ack = ibus_ack;
  }

  if (dma.has_ebus_req()) {
  dma.on_ebus_ack(ebus_ack);
  }
  else if (cpu_has_ebus_req) {
  cpu_ack = ebus_ack;
  }

  if (dma.has_vbus_req()) {
  dma.on_vbus_ack(vbus_ack);
  }
  else if (ppu.has_vbus_req()) {
  ppu.on_vbus_ack(vbus_ack);
  }
  else if (cpu_has_vbus_req) {
  cpu_ack = vbus_ack;
  }

  if (dma.has_obus_req()) {
  dma.on_obus_ack(obus_ack);
  }
  else if (ppu.has_obus_req()) {
  ppu.on_obus_ack(obus_ack);
  }
  else if (cpu_has_obus_req) {
  cpu_ack = obus_ack;
  }
  */

  if (tphase == 0) {
    z80.tick_t0(imask, intf, cpu_ack);
  }
  else if (tphase == 1) {
    z80.tick_t1();
  }
  else if (tphase == 2) {
    z80.tick_t2();
  }
  else if (tphase == 3) {
    z80.tick_t3();
  }

  //-----------------------------------
}

//-----------------------------------------------------------------------------

void Gameboy::tock2() {
  phase++;
  int64_t tcycle = (int64_t)floor(double(phase) / 2);
  int64_t tphase = tcycle & 3;

  //-----------------------------------

  if (tphase == 0) {
    z80.tock_t0(imask, intf);
    timer.tock_t30();
  }

  //-----------------------------------

  if (tphase == 1) {
    z80.tock_t1(imask, intf);
  }

  //-----------------------------------

  if (tphase == 2) {
    z80.tock_t2(imask, intf);
  }

  //-----------------------------------
  // dispatch reqs

  if (tphase == 3) {
    if (ibus_req.read || ibus_req.write) {
      this->ibus_req2(ibus_req);
      ppu.   ibus_req(ibus_req);
      serial.ibus_req(ibus_req);
      joypad.ibus_req(ibus_req);
      zram.  ibus_req(ibus_req);
      timer. ibus_req(ibus_req);
      spu.   ibus_req(ibus_req);
      dma.   ibus_req(ibus_req);
      boot.  ibus_req(ibus_req);
    }

    if (ebus_req.read || ebus_req.write) {
      cart.ebus_req(ebus_req);
      iram.ebus_req(ebus_req);
    }

    if (vbus_req.read || vbus_req.write) {
      vram.vbus_req(vbus_req);
    }

    if (obus_req.read || obus_req.write) {
      oam.obus_req(obus_req);
    }

    z80.tock_t3(imask, intf);
  }

  //-----------------------------------
  // Z80 bus mux & tock

  //-----------------------------------
  // Peripheral bus mux & tocks

  //ppu  .tock(int(tcycle_new));
  //spu  .tock(int(tcycle_new));
  //dma  .tock(int(tcycle_new));

  //----------

  /*
  gb_to_host.x       = ppu.pix_count;
  gb_to_host.y       = ppu.line;
  gb_to_host.counter = ppu.counter;
  gb_to_host.pix     = ppu.pix_out;
  gb_to_host.pix_oe  = ppu.pix_oe;
  gb_to_host.out_r   = spu.get_r();
  gb_to_host.out_l   = spu.get_l();
  gb_to_host.trace   = vbus_req.addr;
  gb_to_host.trace   = 0;
  */
}

//-----------------------------------------------------------------------------

void Gameboy::dump_cpu(std::string& d) {
  sprintf(d, "\002------------- CPU --------------\001\n");
  z80.dump(d);
}

void Gameboy::dump_bus(std::string& d) {
  sprintf(d, "\002------------- BUS --------------\001\n");

  const char* phases[] = {
    "A_______",
    "_B______",
    "__C_____",
    "___D____",
    "____E___",
    "_____F__",
    "______G_",
    "_______H",
  };

  sprintf(d, "phase %s\n", phases[phase & 7]);
  sprintf(d, "tphase %lld\n", phase);
  sprintf(d, "tcycle %lld\n", (int64_t)floor(double(phase) / 2));
  sprintf(d, "mcycle %lld\n", (int64_t)floor(double(phase) / 8));
  sprintf(d, "bgb cycle      0x%08x\n", ((phase / 2) * 8) + 0x00B2D5E6);
  sprintf(d, "imask  %s\n", byte_to_bits(imask));
  sprintf(d, "intf   %s\n", byte_to_bits(intf));
  sprintf(d, "boot   %d\n", boot.disable_bootrom);
  sprintf(d, "\n");

  {
    bool cpu_has_ibus_req = cpu_req.addr >= ADDR_IOBUS_BEGIN;
    bool cpu_has_vbus_req = cpu_req.addr >= ADDR_VRAM_BEGIN && cpu_req.addr <= ADDR_VRAM_END;
    bool cpu_has_obus_req = cpu_req.addr >= ADDR_OAM_BEGIN  && cpu_req.addr <= ADDR_OAM_END;
    bool cpu_has_ebus_req = !cpu_has_vbus_req && !cpu_has_obus_req && !cpu_has_ibus_req;

    char cpu_dir = cpu_req.write ? 'W' : cpu_req.read ? 'R' : '?';

    int cpu_to_ibus = cpu_has_ibus_req ? cpu_dir : ' ';
    int cpu_to_ebus = cpu_has_ebus_req ? cpu_dir : ' ';
    int cpu_to_vbus = cpu_has_vbus_req ? cpu_dir : ' ';
    int cpu_to_obus = cpu_has_obus_req ? cpu_dir : ' ';

    int dma_to_ebus = dma_ebus_req.read  ? 'R' : ' ';
    int dma_to_vbus = dma_vbus_req.read  ? 'R' : ' ';
    int dma_to_obus = dma_obus_req.write ? 'W' : ' ';

    int ppu_to_vbus = ppu_vbus_req.read ? 'R' : ' ';
    int ppu_to_obus = ppu_obus_req.read ? 'O' : ' ';

    if (dma_ebus_req.read) {
      cpu_to_ebus = tolower(cpu_to_ebus);
    }

    /*
    if (dma_ebus_req.read) {
    ebus_req = dma_ebus_req;
    }
    else if (cpu_has_ebus_req) {
    ebus_req = cpu_req;
    }

    if (dma_vbus_req.read) {
    vbus_req = dma_vbus_req;
    }
    else if (ppu_vbus_req.read) {
    vbus_req = ppu_vbus_req;
    }
    else if (cpu_has_vbus_req) {
    vbus_req = cpu_req;
    }

    if (dma_obus_req.write) {
    obus_req = dma_obus_req;
    }
    else if (ppu_obus_req.read) {
    obus_req = ppu_obus_req;
    }
    else if (cpu_has_obus_req) {
    obus_req = cpu_req;
    }
    */

    sprintf(d, "     |cpu|dma|ppu|\n");
    sprintf(d, " ibus|  %c|  %c|  %c|\n", cpu_to_ibus, ' ', ' ');
    sprintf(d, " ebus|  %c|  %c|  %c|\n", cpu_to_ebus, dma_to_ebus, ' ');
    sprintf(d, " vbus|  %c|  %c|  %c|\n", cpu_to_vbus, dma_to_vbus, ppu_to_vbus);
    sprintf(d, " obus|  %c|  %c|  %c|\n", cpu_to_obus, dma_to_obus, ppu_to_obus);
    sprintf(d, "\n");
  }

  sprintf(d,   "---CPU:\n");
  print_req(d, "cpu_req  ", cpu_req);
  print_ack(d, "cpu_ack  ", cpu_ack);

  sprintf(d,   "---DMA to EBUS:\n");
  print_req(d, "cpu_req  ", dma_ebus_req);
  print_ack(d, "cpu_ack  ", dma_ebus_ack);

  sprintf(d,   "---DMA to VBUS:\n");
  print_req(d, "cpu_req  ", dma_vbus_req);
  print_ack(d, "cpu_ack  ", dma_vbus_ack);

  sprintf(d,   "---DMA to OBUS:\n");
  print_req(d, "cpu_req  ", dma_obus_req);
  print_ack(d, "cpu_ack  ", dma_obus_ack);

  sprintf(d,   "---PPU to VBUS:\n");
  print_req(d, "cpu_req  ", ppu_vbus_req);
  print_ack(d, "cpu_ack  ", ppu_vbus_ack);

  sprintf(d,   "---PPU to OBUS:\n");
  print_req(d, "cpu_req  ", ppu_obus_req);
  print_ack(d, "cpu_ack  ", ppu_obus_ack);


  sprintf(d,   "---IBUS:\n");
  print_req(d, "ibus_req ", ibus_req);
  print_ack(d, "ibus_ack ", ibus_ack);

  sprintf(d,   "---EBUS:\n");
  print_req(d, "ebus_req ", ebus_req);
  print_ack(d, "ebus_ack ", ebus_ack);

  sprintf(d,   "---VBUS:\n");
  print_req(d, "vbus_req ", vbus_req);
  print_ack(d, "vbus_ack ", vbus_ack);

  sprintf(d,   "---OBUS:\n");
  print_req(d, "obus_req ", obus_req);
  print_ack(d, "obus_ack ", obus_ack);
  sprintf(d,   "\n");
}

void Gameboy::dump_zram(std::string& d) {
  zram.dump(d);
}

void Gameboy::dump_timer(std::string& d) {
  timer.dump(d);
}

void Gameboy::dump_cart(std::string& d) {
  cart.dump(d);
}

void Gameboy::dump_oam(std::string& d) {
  oam.dump(d);
}

void Gameboy::dump_joypad(std::string& d) {
  joypad.dump(d);
}

void Gameboy::dump_serial(std::string& d) {
  serial.dump(d);
}

void Gameboy::dump_disasm(std::string& d) {
  sprintf(d, "\002--------------DISASM-----------\001\n");

  uint16_t pc = z80.get_op_addr();
  const uint8_t* segment;

  if (ADDR_IRAM_BEGIN <= pc && pc <= ADDR_IRAM_END) {
    segment = iram.get() + (pc - ADDR_IRAM_BEGIN);
  }
  else if (ADDR_ZEROPAGE_BEGIN <= pc && pc <= ADDR_ZEROPAGE_END) {
    segment = zram.get() + (pc - ADDR_ZEROPAGE_BEGIN);
  }
  else if (ADDR_OAM_BEGIN <= pc && pc <= ADDR_OAM_END) {
    segment = oam.get() + (pc - ADDR_OAM_BEGIN);
  }
  else {
    segment = cart.get_flat_ptr(pc);
  }

  if (segment) {
    Assembler a;
    a.disassemble(segment, 65536, pc, 30, d, false);
    sprintf(d, "\n");
  }
  else {
    sprintf(d, "(bad pc)\n");
  }
}

//-----------------------------------------------------------------------------