#include "GateBoyLib/Sch_DMA.h"

#include "GateBoyLib/Sch_Top.h"

using namespace Schematics;

//------------------------------------------------------------------------------

void DmaRegisters::dump(Dumper& d) const {
  d("\002===== DMA Reg =====\001\n");

  int dma_addr_hi = pack_p(!NAFA_DMA_A08n.qp08(), !PYNE_DMA_A09n.qp08(), !PARA_DMA_A10n.qp08(), !NYDO_DMA_A11n.qp08(),
                           !NYGY_DMA_A12n.qp08(), !PULA_DMA_A13n.qp08(), !POKU_DMA_A14n.qp08(), !MARU_DMA_A15n.qp08());

  int dma_addr_lo = pack_p(NAKY_DMA_A00p.qp17(), PYRO_DMA_A01p.qp17(), NEFY_DMA_A02p.qp17(), MUTY_DMA_A03p.qp17(),
                           NYKO_DMA_A04p.qp17(), PYLO_DMA_A05p.qp17(), NUTO_DMA_A06p.qp17(), MUGU_DMA_A07p.qp17());

  d("DMA Addr 0x%02x:%02x\n",dma_addr_hi, dma_addr_lo);
  d("MATU_DMA_RUNNINGp   %d\n", MATU_DMA_RUNNINGp.qp17());
  d("LUMA_DMA_READ_CARTp %d\n", LUMA_DMA_READ_CARTp.qp());
  d("LUFA_DMA_VRM_RDp    %d\n", LUFA_DMA_READ_VRAMp.qp());
  d("LYXE_DMA_LATCHn     %d\n", LYXE_DMA_LATCHp);
  d("MYTE_DMA_DONE       %d\n", !MYTE_DMA_DONE.qn16());
  d("LUVY_DMA_TRIG_d0    %d\n",  LUVY_DMA_TRIG_d0.qp17());
  d("LENE_DMA_TRIG_d4    %d\n", !LENE_DMA_TRIG_d4.qn16());
  d("LOKY_DMA_LATCHp     %d\n",  LOKY_DMA_LATCHp.qp03());
  d("\n");
}

//------------------------------------------------------------------------------

void DmaRegisters::tick() {
  /*#p04.LEBU*/ wire LEBU_DMA_ADDR_A15n  = not1(MARU_DMA_A15n.qn07());
  /*#p04.MUDA*/ wire MUDA_DMA_SRC_VRAMp  = nor3(PULA_DMA_A13n.qn07(), POKU_DMA_A14n.qn07(), LEBU_DMA_ADDR_A15n);
  /* p04.LOGO*/ wire LOGO_DMA_VRAMn      = not1(MUDA_DMA_SRC_VRAMp);
  /* p04.MORY*/ wire MORY_DMA_READ_CARTn = nand2(MATU_DMA_RUNNINGp.qp17(), LOGO_DMA_VRAMn);
  /* p04.LUMA*/ LUMA_DMA_READ_CARTp      = not1(MORY_DMA_READ_CARTn);

  /* p04.MUHO*/ wire MUHO_DMA_VRAM_RDn = nand2(MATU_DMA_RUNNINGp.qp17(), MUDA_DMA_SRC_VRAMp);
  /* p04.LUFA*/ LUFA_DMA_READ_VRAMp    = not1(MUHO_DMA_VRAM_RDn);

  /*#p28.BOGE*/ BOGE_DMA_RUNNINGn = not1(MATU_DMA_RUNNINGp.qp17());
}

//------------------------------------------------------------------------------

void DmaRegisters::tock(const SchematicTop& top, CpuBus& cpu_bus) {
  /* p01.UVYT*/ wire UVYT_ABCDxxxx = not1(top.clk_reg.BUDE_xxxxEFGH);
  /* p04.MOPA*/ wire MOPA_xxxxEFGH = not1(UVYT_ABCDxxxx);

  {
    /*#p22.WATE*/ wire WATE_FF46n    = nand5(top.cpu_bus.WERO_FF4Xp(), top.cpu_bus.XOLA_A00n(), top.cpu_bus.WESA_A01p(), top.cpu_bus.WALO_A02p(), top.cpu_bus.XERA_A03n());
    /*#p22.XEDA*/ wire XEDA_FF46p    = not1(WATE_FF46n);
    /*#p04.LAVY*/ wire LAVY_FF46_WRp = and2(top.CUPA_CPU_WRp_xxxxEFGx, XEDA_FF46p);
    /*#p04.LOKO*/ wire LOKO_DMA_RSTp = nand2(LENE_DMA_TRIG_d4.qn16(), top.clk_reg.CUNU_SYS_RSTn);
    /*#p04.LYXE*/ LYXE_DMA_LATCHp.nor_latch(LAVY_FF46_WRp, LOKO_DMA_RSTp);

    /*#p04.LUPA*/ wire LUPA_DMA_TRIG = nor2(LAVY_FF46_WRp, LYXE_DMA_LATCHp.qn03());
    /*#p04.LUVY*/ LUVY_DMA_TRIG_d0  .dff17(UVYT_ABCDxxxx, top.clk_reg.CUNU_SYS_RSTn, LUPA_DMA_TRIG);
    /*#p04.LENE*/ LENE_DMA_TRIG_d4  .dff17(MOPA_xxxxEFGH, top.clk_reg.CUNU_SYS_RSTn, LUVY_DMA_TRIG_d0.qp17());

    /*#p04.LOKY*/ LOKY_DMA_LATCHp.nand_latch(LENE_DMA_TRIG_d4.qn16(), and2(MYTE_DMA_DONE.qn16(), top.clk_reg.CUNU_SYS_RSTn));
    /*#p04.MATU*/ MATU_DMA_RUNNINGp .dff17(UVYT_ABCDxxxx, top.clk_reg.CUNU_SYS_RSTn, LOKY_DMA_LATCHp.qp03());
  }

  {
    // 128+16+8+4+2+1 = 159, this must be "dma done"

    /*#p04.LOKO*/ wire LOKO_DMA_RSTp  = nand2(LENE_DMA_TRIG_d4.qn16(), top.clk_reg.CUNU_SYS_RSTn);
    /*#p04.LAPA*/ wire LAPA_DMA_RSTn  = not1(LOKO_DMA_RSTp);
    /*#p04.NAVO*/ wire NAVO_DMA_DONEn = nand6(NAKY_DMA_A00p.qp17(), PYRO_DMA_A01p.qp17(), NEFY_DMA_A02p.qp17(), MUTY_DMA_A03p.qp17(), NYKO_DMA_A04p.qp17(), MUGU_DMA_A07p.qp17());
    /*#p04.NOLO*/ wire NOLO_DMA_DONEp = not1(NAVO_DMA_DONEn);
    /*#p04.MYTE*/ MYTE_DMA_DONE      .dff17(MOPA_xxxxEFGH, LAPA_DMA_RSTn, NOLO_DMA_DONEp);
  }

  {
    /*#p04.LOKO*/ wire LOKO_DMA_RSTp = nand2(LENE_DMA_TRIG_d4.qn16(), top.clk_reg.CUNU_SYS_RSTn);
    /*#p04.LAPA*/ wire LAPA_DMA_RSTn = not1(LOKO_DMA_RSTp);
    /*#p04.META*/ wire META_DMA_CLKp = and2(UVYT_ABCDxxxx, LOKY_DMA_LATCHp.qp03());

    /*#p04.NAKY*/ NAKY_DMA_A00p.dff17(META_DMA_CLKp,       LAPA_DMA_RSTn, NAKY_DMA_A00p.qn16());
    /*#p04.PYRO*/ PYRO_DMA_A01p.dff17(NAKY_DMA_A00p.qn16(),  LAPA_DMA_RSTn, PYRO_DMA_A01p.qn16());
    /* p04.NEFY*/ NEFY_DMA_A02p.dff17(PYRO_DMA_A01p.qn16(),  LAPA_DMA_RSTn, NEFY_DMA_A02p.qn16());
    /* p04.MUTY*/ MUTY_DMA_A03p.dff17(NEFY_DMA_A02p.qn16(),  LAPA_DMA_RSTn, MUTY_DMA_A03p.qn16());
    /* p04.NYKO*/ NYKO_DMA_A04p.dff17(MUTY_DMA_A03p.qn16(),  LAPA_DMA_RSTn, NYKO_DMA_A04p.qn16());
    /* p04.PYLO*/ PYLO_DMA_A05p.dff17(NYKO_DMA_A04p.qn16(),  LAPA_DMA_RSTn, PYLO_DMA_A05p.qn16());
    /* p04.NUTO*/ NUTO_DMA_A06p.dff17(PYLO_DMA_A05p.qn16(),  LAPA_DMA_RSTn, NUTO_DMA_A06p.qn16());
    /* p04.MUGU*/ MUGU_DMA_A07p.dff17(NUTO_DMA_A06p.qn16(),  LAPA_DMA_RSTn, MUGU_DMA_A07p.qn16());
  }

  // FF46 DMA
  {
    /*#p22.WATE*/ wire WATE_FF46n    = nand5(top.cpu_bus.WERO_FF4Xp(), top.cpu_bus.XOLA_A00n(), top.cpu_bus.WESA_A01p(), top.cpu_bus.WALO_A02p(), top.cpu_bus.XERA_A03n());
    /*#p22.XEDA*/ wire XEDA_FF46p    = not1(WATE_FF46n);
    /*#p04.LAVY*/ wire LAVY_FF46_WRp = and2(top.CUPA_CPU_WRp_xxxxEFGx, XEDA_FF46p);
    /*#p04.LORU*/ wire LORU_FF46_WRn = not1(LAVY_FF46_WRp);
    ///*#p??.PYSU*/ wire PYSU_FF46_WRp = not1(LORU_FF46_WRn);

    /*#p04.NAFA*/ NAFA_DMA_A08n.dff8p(LORU_FF46_WRn, top.cpu_bus.BUS_CPU_D0p.qp()); // BOTH OUTPUTS USED
    /* p04.PYNE*/ PYNE_DMA_A09n.dff8p(LORU_FF46_WRn, top.cpu_bus.BUS_CPU_D1p.qp());
    /* p04.PARA*/ PARA_DMA_A10n.dff8p(LORU_FF46_WRn, top.cpu_bus.BUS_CPU_D2p.qp());
    /* p04.NYDO*/ NYDO_DMA_A11n.dff8p(LORU_FF46_WRn, top.cpu_bus.BUS_CPU_D3p.qp());
    /* p04.NYGY*/ NYGY_DMA_A12n.dff8p(LORU_FF46_WRn, top.cpu_bus.BUS_CPU_D4p.qp());
    /* p04.PULA*/ PULA_DMA_A13n.dff8p(LORU_FF46_WRn, top.cpu_bus.BUS_CPU_D5p.qp());
    /* p04.POKU*/ POKU_DMA_A14n.dff8p(LORU_FF46_WRn, top.cpu_bus.BUS_CPU_D6p.qp());
    /* p04.MARU*/ MARU_DMA_A15n.dff8p(LORU_FF46_WRn, top.cpu_bus.BUS_CPU_D7p.qp());

    /*p 07.AJAS*/ wire AJAS_CPU_RDn  = not1(top.TEDO_CPU_RDp);
    /*p 07.ASOT*/ wire ASOT_CPU_RDp  = not1(AJAS_CPU_RDn);
    /*#p04.MOLU*/ wire MOLU_FF46_RDp = and2(XEDA_FF46p, ASOT_CPU_RDp);
    /*#p04.NYGO*/ wire NYGO_FF46_RDn = not1(MOLU_FF46_RDp);
    /*#p04.PUSY*/ wire PUSY_FF46_RDp = not1(NYGO_FF46_RDn);

    /*#p04.POLY*/ cpu_bus.BUS_CPU_D0p.tri_6pn(PUSY_FF46_RDp, NAFA_DMA_A08n.qp08());
    /* p04.ROFO*/ cpu_bus.BUS_CPU_D1p.tri_6pn(PUSY_FF46_RDp, PYNE_DMA_A09n.qp08());
    /* p04.REMA*/ cpu_bus.BUS_CPU_D2p.tri_6pn(PUSY_FF46_RDp, PARA_DMA_A10n.qp08());
    /* p04.PANE*/ cpu_bus.BUS_CPU_D3p.tri_6pn(PUSY_FF46_RDp, NYDO_DMA_A11n.qp08());
    /* p04.PARE*/ cpu_bus.BUS_CPU_D4p.tri_6pn(PUSY_FF46_RDp, NYGY_DMA_A12n.qp08());
    /* p04.RALY*/ cpu_bus.BUS_CPU_D5p.tri_6pn(PUSY_FF46_RDp, PULA_DMA_A13n.qp08());
    /* p04.RESU*/ cpu_bus.BUS_CPU_D6p.tri_6pn(PUSY_FF46_RDp, POKU_DMA_A14n.qp08());
    /* p04.NUVY*/ cpu_bus.BUS_CPU_D7p.tri_6pn(PUSY_FF46_RDp, MARU_DMA_A15n.qp08());
  }
}

//-----------------------------------------------------------------------------
