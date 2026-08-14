/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exclusive clean-room public register / descriptor names for
 * user/drivers/rtl8168_udx — RTL8168/8111 class (PCI 10ec:8168).
 *
 * Sources of *names* and coarse offsets: Realtek public datasheets
 * (e.g. RTL8111B/RTL8168B Registers DataSheet 1.0, Track ID
 * JATR-1076-21, 26 Jan 2006 — FreeBSD public mirror) and related
 * PCI BAR inventories. Not a silicon certification matrix.
 *
 * CLEAN-ROOM: do not paste Linux r8169.c, Realtek GPL blobs, or
 * kernel/drv/rtl8168.c. Soft residual only -- never product TX/RX claim.
 *
 * Product path (Dual DoD B direction — Linux-shaped NIC via UDX):
 *   Userspace UDX+ABI+DDI host for 10ec:8168; caps for MMIO/IRQ/DMA.
 *   Freestanding kernel/drv/rtl8168.c is SKIP by default
 *   (GJ_RTL8168_PROBE=0) -- not product. Soft!=product. G-AC-1.
 *   Dual DoD B = OPEN (wire / lab IP / stack / sshd not closed here).
 *
 * Product residual (C2; Dual DoD B OPEN; stamp-free bar v2026.08.04.75):
 *   product=UDX+sshd+stack — public reg names only; never program silicon.
 *   Chain residual: this host (rtl8168_udx) → netstackd → sshd :22.
 *   Soft never_tx_rx / never_program residual != Dual DoD B wire close.
 * greppable: rtl8168_udx_regs: soft product residual product=UDX+sshd+stack
 * greppable: Soft!=product Dual DoD B OPEN product=UDX+sshd+stack
 *
 * BAR honesty (soft):
 *   Silicon-common primary MMIO is often BAR2.
 *   Freestanding DDI bind (GJ_SYS_DDI) currently grants/maps BAR0.
 *   Soft probe prefers BAR2, falls back to BAR0 then first MEM BAR.
 *   Prefer != product wire; Soft!=product.
 *
 * C2 regs residual (public names only):
 *   Prefer Table-1 MAC tags + published bit symbols from JATR-1076-21.
 *   Long-form residual names (ChipCmd/IntrMask/TxConfig/…) keep
 *   short public aliases (CR/IMR/TCR/…). Non-Table-1 slots are
 *   residual-reserved honesty only — not product program targets.
 *   Soft residual never programs silicon. Soft!=product. G-AC-1.
 */
#pragma once

/* Public PCI identity (PCI-SIG vendor 0x10ec, device 0x8168). */
#define RTL8168_VEND           0x10ecu
#define RTL8168_DEV            0x8168u

/* Host soft inject / freestanding G752-class defaults (not product AC). */
#define RTL8168_IRQ_SOFT       12
#define RTL8168_BAR_MMIO       2       /* silicon-common MMIO BAR index */
#define RTL8168_BAR_DDI        0       /* freestanding DDI MAP_BAR grant */
#define RTL8168_BAR_LEN        0x1000ull
#define RTL8168_SOFT_BUS       3u
#define RTL8168_SOFT_SLOT      0u
#define RTL8168_SOFT_FUNC      0u

/*
 * Soft residual honesty constants (names only; never a version stamp).
 * greppable: Soft!=product · Dual DoD B OPEN · G-AC-1 · never_program
 * greppable: rtl8168_udx_regs: soft product residual product=UDX+sshd+stack
 */
#define RTL_SOFT_NEVER_PROGRAM     1u /* never write TNPDS/RDSAR/THPDS */
#define RTL_SOFT_NEVER_TE_RE       1u /* never arm ChipCmd TE|RE */
#define RTL_SOFT_NEVER_PHYAR_RW    1u /* never PHYAR R/W cycles */
#define RTL_SOFT_NEVER_TPPOLL      1u /* never TPPoll kick */
#define RTL_SOFT_NEVER_TX_RX       1u /* never product wire I/O */
#define RTL_SOFT_DUAL_DOD_B_OPEN   1u /* Dual DoD B product path OPEN */
#define RTL_SOFT_NE_PRODUCT        1u /* Soft!=product */
#define RTL_SOFT_G_AC_1_HOLD       1u /* no .ko product AC */
/*
 * Product residual seed (Soft!=product; Dual DoD B OPEN; stamp-free).
 * product=UDX+sshd+stack — reg catalog residual for NIC → stack → sshd.
 * Never programs silicon; never claims wire / host banner close.
 * Bar honesty v2026.08.04.75 stamp-free. NEVER bump GJ_IMAGE_VERSION.
 */
#define RTL_SOFT_PRODUCT_UDX_SSHD_STACK  1u
#define RTL_SOFT_PRODUCT_CHAIN           "rtl8168_udx>netstackd>sshd"
#define RTL_SOFT_PRODUCT_SSHD_PORT       22u
#define RTL_SOFT_PRODUCT_STACK_HOST      1u /* netstackd residual seed */

/*
 * Coarse MMIO map — public Realtek MAC register tags (8111B/8168B class).
 * Soft residual RO-snaps most; only IntrStatus/IntrMask are host-demo RW.
 * Offsets / tags from RTL8111B/RTL8168B Registers DataSheet 1.0 Table 1
 * unless noted (not GPL sources).
 *
 * Public datasheet tags (Table 1): IDR / MAR / DTCCR / TNPDS / THPDS /
 * CR / TPPoll / IMR / ISR / TCR / RCR / TCTR / 9346CR / CONFIGx /
 * TimerInt / PHYAR / PHYStatus / Wakeup* / CRC* / RMS / C+CR / RDSAR /
 * MTPS. Soft residual keeps long-form names + short public aliases.
 */
#define RTL_REG_IDR0           0x00ull /* public IDR0 — ID Register 0 (4-byte access) */
#define RTL_REG_IDR1           0x01ull /* public IDR1 */
#define RTL_REG_IDR2           0x02ull /* public IDR2 */
#define RTL_REG_IDR3           0x03ull /* public IDR3 */
#define RTL_REG_IDR4           0x04ull /* public IDR4 */
#define RTL_REG_IDR5           0x05ull /* public IDR5 */
/*
 * MAR public byte tags (Table 1): MAR7@0x08 … MAR0@0x0F (4-byte access).
 * Dword residual view: MAR0 dword = bytes 08h-0Bh, MAR4 dword = 0Ch-0Fh.
 */
#define RTL_REG_MAR0           0x08ull /* MAR dword0 residual (public MAR7..MAR4) */
#define RTL_REG_MAR4           0x0Cull /* MAR dword1 residual (public MAR3..MAR0) */
#define RTL_REG_MAR7_B         0x08ull /* public MAR7 byte tag */
#define RTL_REG_MAR0_B         0x0Full /* public MAR0 byte tag */
#define RTL_REG_DTCCR          0x10ull /* public DTCCR Dump Tally Counter Command */
#define RTL_REG_DTCCR_HI       0x14ull /* DTCCR high dword (bits 63:32) */
#define RTL_REG_TNPDS          0x20ull /* public TNPDS Tx normal-priority desc start */
#define RTL_REG_TNPDS_HI       0x24ull /* TNPDS high dword */
#define RTL_REG_THPDS          0x28ull /* public THPDS Tx high-priority desc start */
#define RTL_REG_THPDS_HI       0x2Cull /* THPDS high dword */
#define RTL_REG_CHIPCMD        0x37ull /* long-form residual — public tag CR */
#define RTL_REG_CR             RTL_REG_CHIPCMD /* public CR Command Register */
#define RTL_REG_TPPOLL         0x38ull /* long-form residual — public tag TPPoll */
#define RTL_REG_TPPOLL_TAG     RTL_REG_TPPOLL  /* public TPPoll */
#define RTL_REG_INTR_MASK      0x3Cull /* long-form residual — public tag IMR */
#define RTL_REG_IMR            RTL_REG_INTR_MASK /* public IMR Interrupt Mask */
#define RTL_REG_INTR_STATUS    0x3Eull /* long-form residual — public tag ISR */
#define RTL_REG_ISR            RTL_REG_INTR_STATUS /* public ISR Interrupt Status */
#define RTL_REG_TXCFG          0x40ull /* long-form residual — public tag TCR */
#define RTL_REG_TCR            RTL_REG_TXCFG /* public TCR Transmit Configuration */
#define RTL_REG_RXCFG          0x44ull /* long-form residual — public tag RCR */
#define RTL_REG_RCR            RTL_REG_RXCFG /* public RCR Receive Configuration */
#define RTL_REG_TCTR           0x48ull /* public TCTR Timer CounT (unit 8ns @ 125MHz) */
/*
 * Table-1 reserved residual slots (NOT public 8168B tags).
 * Offsets kept for residual map honesty / host-scan skip only.
 * Soft residual never programs these; prefer public tags above.
 */
#define RTL_REG_RSVD_4C        0x4Cull /* Table-1 reserved 004Ch-004Fh */
/*
 * MPC residual-compat alias (non-Table-1 on 8111B/8168B). Older 8139-class
 * maps used 0x4C as Missed Packet Counter; public 8168B MissPkt lives in
 * DTCCR tally dump (RTL_TALLY_MISSPKT). thr densify may RO-snap 0x4C as
 * residual honesty only — never claim public RxMissed/MPC product path.
 */
#define RTL_REG_MPC            RTL_REG_RSVD_4C /* residual-compat alias (non-Table-1) */
#define RTL_REG_9346CR         0x50ull /* public 9346CR 93C46/93C56 Command */
#define RTL_REG_CONFIG0        0x51ull /* public CONFIG0 */
#define RTL_REG_CONFIG1        0x52ull /* public CONFIG1 */
#define RTL_REG_CONFIG2        0x53ull /* public CONFIG2 */
#define RTL_REG_CONFIG3        0x54ull /* public CONFIG3 */
#define RTL_REG_CONFIG4        0x55ull /* public CONFIG4 */
#define RTL_REG_CONFIG5        0x56ull /* public CONFIG5 */
#define RTL_REG_TIMERINT       0x58ull /* public TimerInt Timer Interrupt Register */
#define RTL_REG_RSVD_5C        0x5Cull /* Table-1 reserved 005Ch-005Fh */
#define RTL_REG_MULINT         RTL_REG_RSVD_5C /* residual-compat alias (non-Table-1) */
#define RTL_REG_PHYAR          0x60ull /* public PHYAR PHY Access — soft never R/W */
#define RTL_REG_RSVD_64        0x64ull /* Table-1 reserved 0064h-006Bh */
#define RTL_REG_RSVD_68        0x68ull /* Table-1 reserved (pair with RSVD_64) */
#define RTL_REG_TBICSR         RTL_REG_RSVD_64 /* residual-compat alias (non-Table-1) */
#define RTL_REG_TBI_ANAR       RTL_REG_RSVD_68 /* residual-compat alias (non-Table-1) */
#define RTL_REG_PHYSTATUS      0x6Cull /* public PHYStatus (byte, soft RO) */

/*
 * Power-management wakeup frame / CRC map (public Table 1; soft never RW).
 * Wakeup0/1 are 64-bit; Wakeup2..4 are 128-bit (LD + HD).
 */
#define RTL_REG_WAKEUP0        0x84ull /* public Wakeup0 */
#define RTL_REG_WAKEUP1        0x8Cull /* public Wakeup1 */
#define RTL_REG_WAKEUP2LD      0x94ull /* public Wakeup2LD */
#define RTL_REG_WAKEUP2HD      0x9Cull /* public Wakeup2HD */
#define RTL_REG_WAKEUP3LD      0xA4ull /* public Wakeup3LD */
#define RTL_REG_WAKEUP3HD      0xACull /* public Wakeup3HD */
#define RTL_REG_WAKEUP4LD      0xB4ull /* public Wakeup4LD */
#define RTL_REG_WAKEUP4HD      0xBCull /* public Wakeup4HD */
#define RTL_REG_CRC0           0xC4ull /* public CRC0 */
#define RTL_REG_CRC1           0xC6ull /* public CRC1 */
#define RTL_REG_CRC2           0xC8ull /* public CRC2 */
#define RTL_REG_CRC3           0xCAull /* public CRC3 */
#define RTL_REG_CRC4           0xCCull /* public CRC4 */

#define RTL_REG_RMS            0xDAull /* public RMS Rx packet Maximum Size */
#define RTL_REG_CPLUSCMD       0xE0ull /* long-form residual — public tag C+CR */
#define RTL_REG_CPLUSCR        RTL_REG_CPLUSCMD /* public C+CR C+ Command */
#define RTL_REG_RDSAR          0xE4ull /* public RDSAR Rx descriptor start */
#define RTL_REG_RDSAR_HI       0xE8ull /* RDSAR high dword (public RDSA 63:32) */
#define RTL_REG_RDSA           RTL_REG_RDSAR /* public field name RDSA */
#define RTL_REG_MTPS           0xECull /* public MTPS Max Transmit Packet Size */
#define RTL_REG_ETTHR          RTL_REG_MTPS /* residual-compat alias (non-Table-1 Early Tx thr) */

/* Public field masks (datasheet; soft never programs unless noted). */
#define RTL_RMS_MASK           0x3fffu    /* RMS bits 13:0 */
#define RTL_MTPS_MASK          0x3fu      /* MTPS bits 5:0 */
#define RTL_MTPS_UNIT_BYTES    128u       /* MTPS unit = 128 bytes */

/* Public alignment spirit (datasheet program notes; soft never enforces). */
#define RTL_ALIGN_TNPDS        256u /* TNPDS/THPDS/RDSAR 256-byte align */
#define RTL_ALIGN_DTCCR        64u  /* DTCCR dump buffer 64-byte align */
#define RTL_ALIGN_RX_BUF       8u   /* Rx buffer 8-byte align residual spirit */

/* Soft interrupt reason bits (public IMR/ISR symbols; host demo uses ROK|TOK). */
#define RTL_ISR_ROK            0x0001u /* public ROK */
#define RTL_ISR_RER            0x0002u /* public RER */
#define RTL_ISR_TOK            0x0004u /* public TOK */
#define RTL_ISR_TER            0x0008u /* public TER */
#define RTL_ISR_RDU            0x0010u /* public RDU */
#define RTL_ISR_LINKCHG        0x0020u /* public LinkChg (primary) */
#define RTL_ISR_PUN_LINKCHG    RTL_ISR_LINKCHG /* residual-compat alias (non-public bit name) */
#define RTL_ISR_FOVW           0x0040u /* public FOVW */
#define RTL_ISR_TDU            0x0080u /* public TDU */
#define RTL_ISR_SWINT          0x0100u /* public SWInt */
#define RTL_ISR_FEMP           0x0200u /* public FEmp */
#define RTL_ISR_TIMEOUT        0x4000u /* public TimeOut */
#define RTL_SOFT_IRQ_BITS      (RTL_ISR_ROK | RTL_ISR_TOK)
/*
 * Product thr-poll IntrMask residual (public IMR bits; not MSI-X).
 * Soft host demo arms ROK|TOK only; product path arms common status so
 * ISR/IntrStatus update under TE|RE for thr-ack dig. Soft!=product.
 */
#define RTL_PRODUCT_IRQ_BITS   (RTL_ISR_ROK | RTL_ISR_RER | RTL_ISR_TOK | \
                                RTL_ISR_TER | RTL_ISR_RDU | RTL_ISR_FOVW | \
                                RTL_ISR_TDU | RTL_ISR_LINKCHG)

/* CR soft bits (public RST/RE/TE; soft residual never arms TE/RE/RST). */
#define RTL_CMD_TE             0x04u /* public TE Transmitter Enable — OPEN */
#define RTL_CMD_RE             0x08u /* public RE Receiver Enable — OPEN */
#define RTL_CMD_RST            0x10u /* public RST software reset — never issued */
#define RTL_CMD_TE_RE          (RTL_CMD_TE | RTL_CMD_RE) /* product arm pair OPEN */
/* CR bits 1:0 reserved on 8111B/8168B Table 3 — residual-compat only. */
#define RTL_CMD_BUFE           0x01u /* residual-compat (non-Table-1 Rx buffer empty spirit) */

/* TPPoll soft bits (public HPQ / NPQ / FSWInt; soft never kicks). */
#define RTL_TPPOLL_FSWINT      0x01u /* public FSWInt Forced Software Interrupt */
#define RTL_TPPOLL_NPQ         0x40u /* public NPQ Normal Priority Queue poll */
#define RTL_TPPOLL_HPQ         0x80u /* public HPQ High Priority Queue poll */

/* PHYStatus soft observe bits (public PHYStatus Table 17; soft RO only). */
#define RTL_PHYST_FULLDUP      0x01u /* public FullDup */
#define RTL_PHYST_LINKOK       0x02u /* public LinkSts (Link Ok) */
#define RTL_PHYST_LINKSTS      RTL_PHYST_LINKOK /* public LinkSts alias */
#define RTL_PHYST_SPEED10      0x04u /* public 10M */
#define RTL_PHYST_SPEED100     0x08u /* public 100M */
#define RTL_PHYST_SPEED1000    0x10u /* public 1000MF */
#define RTL_PHYST_RXFLOW       0x20u /* public RxFlow */
#define RTL_PHYST_TXFLOW       0x40u /* public TxFlow */

/* PHYAR soft bits (public PHY Access Table 16; soft never R/W). */
#define RTL_PHYAR_DATA_MASK    0x0000ffffu /* public Data15-0 */
#define RTL_PHYAR_REG_SHIFT    16u
#define RTL_PHYAR_REG_MASK     0x001f0000u /* public RegAddr 4-0 */
#define RTL_PHYAR_FLAG         0x80000000u /* public Flag bit 31 */

/* C+CR / CPlusCmd soft bits (public C+CR Table 19). */
#define RTL_CPLUS_PCIMULRW     0x0008u /* public PCIMulRW — multi beat PCI DMA */
#define RTL_CPLUS_RXCHKSUM     0x0020u /* public RxChkSum */
#define RTL_CPLUS_RXVLAN       0x0040u /* public RxVLAN */

/* CONFIG1 soft observe bits (public CONFIG1 Table 11; soft RO residual). */
#define RTL_CFG1_PMEN          0x01u /* public PMEn (RO; always 1 after autoload) */
#define RTL_CFG1_VPD           0x02u /* public VPD (RO) */
#define RTL_CFG1_IOMAP         0x04u /* public IOMAP (RO; always 1) */
#define RTL_CFG1_MEMMAP        0x08u /* public MEMMAP (RO; always 1) */

/* CONFIG3 soft bits (public CONFIG3 Table 13; soft never programs). */
#define RTL_CFG3_MAGIC         0x20u /* public Magic Packet enable */
#define RTL_CFG3_VPDSEL        0x40u /* public VPDSel */

/* CONFIG5 soft bits (public CONFIG5 Table 15; soft never programs). */
#define RTL_CFG5_UWF           0x10u /* public UWF Unicast Wakeup Frame */
#define RTL_CFG5_MWF           0x20u /* public MWF Multicast Wakeup Frame */
#define RTL_CFG5_BWF           0x40u /* public BWF Broadcast Wakeup Frame */

/* Receive Configuration accept bits (public RCR Table 8; soft never programs). */
#define RTL_RCR_AAP            0x00000001u /* public AAP */
#define RTL_RCR_APM            0x00000002u /* public APM */
#define RTL_RCR_AM             0x00000004u /* public AM */
#define RTL_RCR_AB             0x00000008u /* public AB */
#define RTL_RCR_AR             0x00000010u /* public AR */
#define RTL_RCR_AER            0x00000020u /* public AER */
#define RTL_RCR_9356SEL        0x00000040u /* public 9356SEL (RO) */
#define RTL_RCR_MXDMA_SHIFT    8u
#define RTL_RCR_MXDMA_MASK     0x00000700u /* public MXDMA2-0 Max Rx DMA burst */
#define RTL_RCR_RXFTH_SHIFT    13u
#define RTL_RCR_RXFTH_MASK     0x0000e000u /* public RXFTH2-0 Rx FIFO threshold */

/* Transmit Configuration bits (public TCR Table 7; soft observe / never program). */
#define RTL_TCR_MXDMA_SHIFT    8u
#define RTL_TCR_MXDMA_MASK     0x00000700u /* public MXDMA2-0 Max Tx DMA burst */
#define RTL_TCR_TX_NOCRC       0x00010000u /* public TX_NOCRC */
#define RTL_TCR_LBK_SHIFT      17u
#define RTL_TCR_LBK_MASK       0x00060000u /* public LBK1-0 digital loopback */
#define RTL_TCR_IFG_MASK       0x03080000u /* public IFG2 + IFG1-0 field spirit */
#define RTL_TCR_HWVERID_MASK   0x7c800000u /* public HWVERID field spirit */

/* 9346CR operating mode bits (public EEM1-0 Table 9; soft never programs). */
#define RTL_9346_EEDO          0x01u /* public EEDO */
#define RTL_9346_EEDI          0x02u /* public EEDI */
#define RTL_9346_EESK          0x04u /* public EESK */
#define RTL_9346_EECS          0x08u /* public EECS */
#define RTL_9346_EEM_SHIFT     6u
#define RTL_9346_EEM_MASK      0xc0u /* public EEM1-0 */
#define RTL_9346_EEM_NORMAL    0x00u /* network/host mode */
#define RTL_9346_EEM_AUTOLOAD  0x40u /* auto-load from EEPROM */
#define RTL_9346_EEM_PROGRAM   0x80u /* 93C46 programming */
#define RTL_9346_EEM_CFGWRITE  0xc0u /* Config register write enable */

/* DTCCR dump command + layout (public Table 2; soft never dump). */
#define RTL_DTCCR_CMD          (1ull << 3) /* public Cmd bit 3 — start dump */
#define RTL_TALLY_TXOK         0u   /* public TxOk 64-bit */
#define RTL_TALLY_RXOK         8u   /* public RxOk 64-bit */
#define RTL_TALLY_TXER         16u  /* public TxER 64-bit */
#define RTL_TALLY_RXER         24u  /* public RxEr 32-bit */
#define RTL_TALLY_MISSPKT      28u  /* public MissPkt 16-bit */
#define RTL_TALLY_FAE          30u  /* public FAE 16-bit */
#define RTL_TALLY_TX1COL       32u  /* public Tx1Col 32-bit */
#define RTL_TALLY_TXMCOL       36u  /* public TxMCol 32-bit */
#define RTL_TALLY_RXOKPHY      40u  /* public RxOkPhy 64-bit */
#define RTL_TALLY_RXOKBRD      48u  /* public RxOkBrd 64-bit */
#define RTL_TALLY_RXOKMUL      56u  /* public RxOkMul 32-bit */
#define RTL_TALLY_TXABT        60u  /* public TxAbt 16-bit */
#define RTL_TALLY_TXUNDRN      62u  /* public TxUndrn 16-bit */
#define RTL_TALLY_BYTES        64u  /* public dump span (64-byte align) */

/*
 * Soft descriptor residual (public Own/EOR/FS/LS layout spirit).
 * 16-byte slots = 4 consecutive double-words (datasheet §6.1). Soft residual
 * zeros rings and marks EOR on last slot only — Own stays host (0).
 * TNPDS / RDSAR / THPDS are never written (product path OPEN / Dual DoD B).
 * Public Tx/Rx command bits (Tables 49–58) listed for residual catalogs only.
 *
 * Public RX descriptor (RTL8111B/8168B-class; opts1/opts2/addr):
 *   opts1 bit31 OWN  — host sets 1 to hand to NIC; NIC clears when done
 *   opts1 bit30 EOR  — end of ring; product sets only on last RX slot
 *   opts1 bit29 FS / bit28 LS — first/last segment (complete = both)
 *   opts1 bit21 RES  — Rx Error Summary (when OWN=0; skip inject)
 *   opts1 bits13:0   — Buffer_Size when OWN=1 (host fill; multiple of 8,
 *                      public max 0x1FF8); Frame_Length when OWN=0 (NIC
 *                      writeback; length includes 4-byte FCS/CRC)
 *   opts2            — VLAN/tag residual; product zeros (no TAGC/TAVA)
 *   addr lo/hi       — 64-bit buffer bus address
 */
#define RTL_DESC_BYTES         16u
#define RTL_DESC_RING_MAX      1024u      /* public max consecutive descriptors */
#define RTL_DESC_OWN           (1u << 31) /* public OWN — device owns when set */
#define RTL_DESC_EOR           (1u << 30) /* public EOR end of ring */
#define RTL_DESC_FS            (1u << 29) /* public FS first segment */
#define RTL_DESC_LS            (1u << 28) /* public LS last segment */
#define RTL_DESC_LGSEN         (1u << 27) /* public LGSEN Large Send (Tx) */
#define RTL_DESC_MSS_SHIFT     16u
#define RTL_DESC_MSS_MASK      0x07ff0000u /* public Large-Send MSS bits 26:16 */
#define RTL_DESC_IPCS          (1u << 18) /* public IPCS IP checksum (Tx) */
#define RTL_DESC_UDPCS         (1u << 17) /* public UDPCS UDP checksum (Tx) */
#define RTL_DESC_TCPCS         (1u << 16) /* public TCPCS TCP checksum (Tx) */
#define RTL_DESC_LEN_MASK      0x3fffu    /* soft residual len spirit (Rx buf 13:0) */
#define RTL_DESC_TX_LEN_MASK   0xffffu    /* public Tx Frame_Length 15:0 */
#define RTL_DESC_RX_BUF_MASK   0x3fffu    /* public Rx Buffer_Size 13:0 (OWN=1) */
#define RTL_DESC_RX_FRAME_MASK 0x3fffu    /* public Rx Frame_Length 13:0 (OWN=0) */
#define RTL_DESC_RX_RES        (1u << 21) /* public RES Rx Error Summary */
#define RTL_DESC_RX_BUF_MAX    0x1ff8u    /* public Buffer_Size max (×8) */
#define RTL_DESC_RX_FCS_BYTES  4u         /* public Frame_Length includes FCS */
#define RTL_DESC_TAGC          (1u << 17) /* public TAGC Tx VLAN Tag Control (opts2) */
#define RTL_DESC_TAVA          (1u << 16) /* public TAVA Rx Tag Available (opts2) */

/* Soft residual ring geometry (CPU DMA only; not silicon program). */
#define RTL_SOFT_TX_SLOTS      16u
#define RTL_SOFT_RX_SLOTS      16u
#define RTL_SOFT_RING_BYTES \
    ((RTL_SOFT_TX_SLOTS + RTL_SOFT_RX_SLOTS) * RTL_DESC_BYTES)
#define RTL8168_RING_BYTES     4096u /* coherent alloc size (room for residual) */

/*
 * Product RX buffer geometry (public Buffer_Size honesty).
 * FORCE32 per-slot pages are 4 KiB page-aligned (udx fs_dma slab).
 * Buffer_Size in Own opts1 must be multiple of 8, ≤ RTL_DESC_RX_BUF_MAX,
 * and ≤ page. Glass v0.1.99: page_align PASS but Own still stuck + FOVW
 * with Buffer_Size=4KiB; use standard eth span 1536 (0x600) — public
 * regular-frame size; page remains 4 KiB DMA alloc.
 * Soft!=product Dual DoD B OPEN.
 */
#define RTL_SOFT_RX_PAGE_BYTES 4096u
#define RTL_SOFT_RX_BUF_BYTES  0x0600u /* 1536 — eth+FCS headroom; ×8 */

/*
 * Public IEEE/GMII MDIO register addresses (datasheet Table 22 PHY
 * Register Definitions). Soft residual never issues PHYAR cycles; names only.
 */
#define RTL_MDIO_BMCR          0x00u /* public BMCR Basic Mode Control */
#define RTL_MDIO_BMSR          0x01u /* public BMSR Basic Mode Status */
#define RTL_MDIO_PHYAD1        0x02u /* public PHYAD1 PHY Identifier 1 */
#define RTL_MDIO_PHYAD2        0x03u /* public PHYAD2 PHY Identifier 2 */
#define RTL_MDIO_PHYID1        RTL_MDIO_PHYAD1 /* residual-compat alias */
#define RTL_MDIO_PHYID2        RTL_MDIO_PHYAD2 /* residual-compat alias */
#define RTL_MDIO_ANAR          0x04u /* public ANAR */
#define RTL_MDIO_ANLPAR        0x05u /* public ANLPAR */
#define RTL_MDIO_ANER          0x06u /* public ANER */
#define RTL_MDIO_ANNPTR        0x07u /* public ANNPTR Next Page TX */
#define RTL_MDIO_ANNPRR        0x08u /* public ANNPRR / ANNRPR Next Page RX */
#define RTL_MDIO_GBCR          0x09u /* public GBCR 1000Base-T Control */
#define RTL_MDIO_GBSR          0x0Au /* public GBSR 1000Base-T Status */
#define RTL_MDIO_GBESR         0x0Fu /* public GBESR 1000Base-T Extended Status */

/* Public BMCR bits (Table 23). Soft residual catalogs only; product path may PHYAR. */
#define RTL_BMCR_RESET         0x8000u /* public Reset */
#define RTL_BMCR_LOOPBACK      0x4000u /* public Loopback */
#define RTL_BMCR_ANE           0x1000u /* public ANE Auto-Negotiation Enable */
#define RTL_BMCR_PWD           0x0800u /* public PWD Power Down */
#define RTL_BMCR_ISOLATE       0x0400u /* public Isolate */
#define RTL_BMCR_RESTART_AN    0x0200u /* public Restart_AN */
#define RTL_BMCR_DUPLEX        0x0100u /* public Duplex */

/* Public BMSR bits (Table 24 spirit). */
#define RTL_BMSR_LINK          0x0004u /* public Link Status (latched) */
#define RTL_BMSR_ANEG_COMPLETE 0x0020u /* public Auto-Negotiation Complete */

/*
 * Public GBCR 1000Base-T Control (IEEE 802.3 / Table 22 spirit).
 * Windows control on same cable/switch proves partner can link; advertise
 * 1000Full so G752-class copper is not stuck half-advertised. Soft!=product.
 */
#define RTL_GBCR_1000FULL      0x0200u /* public 1000BASE-T Full Duplex */
#define RTL_GBCR_1000HALF      0x0100u /* public 1000BASE-T Half Duplex */

/*
 * Soft probe progress stages (lamps only; never product TX/RX):
 *   1 = BAR map ok
 *   2 = soft reg/cfg snapshot
 *   3 = soft ring residual laid out (CPU DMA only)
 *   4 = IRQ + IntrMask soft armed
 *   5 = host IRQ/work path (host demo)
 * Soft reclaim / TE-RE / TNPDS program remain OPEN beyond stage 5.
 */
#define RTL_SOFT_ST_MAP        1u
#define RTL_SOFT_ST_REGS       2u
#define RTL_SOFT_ST_RING       3u
#define RTL_SOFT_ST_IRQ        4u
#define RTL_SOFT_ST_WORK       5u

/* Soft BAR prefer codes for honesty lamps (not product claim). */
#define RTL_SOFT_BAR_SRC_BAR2  0u /* preferred silicon-common */
#define RTL_SOFT_BAR_SRC_BAR0  1u /* DDI grant / fallback */
#define RTL_SOFT_BAR_SRC_SCAN  2u /* first non-empty MEM BAR scan */

/*
 * Soft wire ownership honesty (bind != wire):
 *   Freestanding lab path owns live wire interim (Dual DoD B / lab eth).
 *   This UDX host is soft residual only — never claims product wire.
 *   bind_by_id / host inject != freestanding TX/RX ownership transfer.
 */
#define RTL_SOFT_WIRE_OWNER_FS   1u /* freestanding owns wire interim */
#define RTL_SOFT_WIRE_OWNER_UDX  0u /* product UDX NIC host — OPEN */
#define RTL_SOFT_BIND_NE_WIRE    1u /* bind PASS != wire handoff */

/*
 * Soft open/MAP residual path bits (rollup honesty; not product CNode).
 * Tracked in soft state; greppable via soft open path / soft map path.
 */
#define RTL_SOFT_PATH_OPEN     1u
#define RTL_SOFT_PATH_MAP      2u
#define RTL_SOFT_PATH_RING     4u
#define RTL_SOFT_PATH_VERIFY   8u
#define RTL_SOFT_PATH_IRQ      16u
#define RTL_SOFT_PATH_PROD_CAT 32u /* product residual catalog walked (lamps only) */
/* C2 product-direction residual deepen (catalog only; Soft!=product). */
#define RTL_SOFT_PATH_PROG     64u   /* 64-bit desc-base program residual */
#define RTL_SOFT_PATH_PHY_RES  128u  /* PHY/link residual catalog */
#define RTL_SOFT_PATH_NAPI_RES 256u  /* NAPI/reclaim residual catalog */
#define RTL_SOFT_PATH_TXRX_RES 512u  /* TX/RX + TPPOLL + net residual */
#define RTL_SOFT_PATH_C2       1024u /* C2 product path honesty walked */
#define RTL_SOFT_PATH_CAPS     2048u /* MMIO/IRQ/DMA caps residual catalog */
#define RTL_SOFT_PATH_IRQ_RES  4096u /* soft IRQ residual catalog (once) */
#define RTL_SOFT_PATH_WORK_RES 8192u /* soft work residual catalog (once) */
#define RTL_SOFT_PATH_WIRE_HO  16384u /* wire handoff residual catalog */
#define RTL_SOFT_PATH_FUNC     32768u /* functional residual catalog walked */

/*
 * Soft functional residual (Dual DoD B product direction when DDI caps live):
 *   Ordered open → MAP → ring → IRQ → work → wire-handoff catalog.
 *   Densifies the live soft path toward product UDX wire ownership.
 *   Soft residual never programs silicon; freestanding owns wire interim.
 * greppable: rtl8168_udx: soft functional residual / soft functional step=
 */
#define RTL_SOFT_FUNC_STEP_OPEN   1u /* pci_enable + regions (soft open) */
#define RTL_SOFT_FUNC_STEP_MAP    2u /* BAR ioremap / soft MAP */
#define RTL_SOFT_FUNC_STEP_RING   3u /* CPU DMA ring residual + verify */
#define RTL_SOFT_FUNC_STEP_IRQ    4u /* request_irq + IntrMask soft */
#define RTL_SOFT_FUNC_STEP_WORK   5u /* ISR → work → reclaim catalog */
#define RTL_SOFT_FUNC_STEP_WIRE   6u /* wire handoff residual (names only) */
#define RTL_SOFT_FUNC_STEP_COUNT  6u

/*
 * Soft caps residual (product path over UDX/DDI):
 *   Product direction mints MMIO_FRAME / IRQ Notification / DMA window
 *   into the host CNode via DDI. Soft residual catalogs only; mint=OPEN.
 *   Soft host inject / ioremap / request_irq / dma_alloc != product caps.
 *   product_mint=0 always in this residual (no CNode mint).
 * greppable: rtl8168_udx: soft caps residual / soft caps step=
 */
#define RTL_SOFT_CAP_STEP_MMIO  1u /* MMIO_FRAME / BAR map via UDX/DDI */
#define RTL_SOFT_CAP_STEP_IRQ   2u /* IRQ -> Notification bind residual */
#define RTL_SOFT_CAP_STEP_DMA   3u /* DMA window / coherent ring residual */
#define RTL_SOFT_CAP_STEP_COUNT 3u

/*
 * Product model honesty (names only; never a version stamp).
 * product = userspace Linux-shaped host over hot+cold ABI + DDI/UDX.
 * Freestanding kernel/drv/rtl8168.c default SKIP (GJ_RTL8168_PROBE=0).
 * Soft residual != product AC. G-AC-1: no Linux .ko product.
 */
#define RTL_SOFT_PRODUCT_UDX_ABI     1u /* product path = UDX+ABI+DDI */
#define RTL_SOFT_FS_PROBE_SKIP       1u /* freestanding rtl product SKIP */
#define RTL_SOFT_G_AC_1              1u /* no .ko product AC */

/*
 * Soft product residual catalog steps (order only; never executed).
 * Product UDX NIC host direction — Dual DoD B userspace over DDI/UDX.
 * Freestanding lab still owns live wire interim (DoD B eth). Soft!=product.
 * Steps name public RTL register map entries (datasheet-class only):
 * TNPDS / RDSAR / CR(TE|RE) / IMR / ISR / TCR / RCR / RMS / PHYAR /
 * PHYStatus / C+CR / TPPoll / THPDS. Long-form residual names alias these.
 *
 *   1 TNPDS program (TX normal desc base 64-bit; + TCR observe)
 *   2 RDSAR program (RX desc base 64-bit; + RCR / RMS observe)
 *   3 CR TE|RE arm (+ IMR / ISR observe)
 *   4 PHY / link (PHYAR / PHYStatus product path)
 *   5 NAPI-style reclaim / poll (+ C+CR observe)
 *   6 TX/RX datapath + TPPoll + netstack attach
 */
#define RTL_SOFT_PROD_STEP_TNPDS  1u
#define RTL_SOFT_PROD_STEP_RDSAR  2u
#define RTL_SOFT_PROD_STEP_TE_RE  3u
#define RTL_SOFT_PROD_STEP_PHY    4u
#define RTL_SOFT_PROD_STEP_NAPI   5u
#define RTL_SOFT_PROD_STEP_TXRX   6u
#define RTL_SOFT_PROD_STEP_COUNT  6u

/*
 * Soft program residual (C2 deepen under prod steps 1–2).
 * 64-bit TNPDS/RDSAR write *order* catalog only — never_program=1.
 * Public desc-base program spirit: low dword then high dword.
 */
#define RTL_SOFT_PROG_STEP_TNPDS_LO 1u
#define RTL_SOFT_PROG_STEP_TNPDS_HI 2u
#define RTL_SOFT_PROG_STEP_RDSAR_LO 3u
#define RTL_SOFT_PROG_STEP_RDSAR_HI 4u
#define RTL_SOFT_PROG_STEP_COUNT    4u

/*
 * Soft PHY residual (C2 deepen under prod step 4).
 * PHYStatus RO bits + PHYAR name + link residual — never PHYAR R/W.
 */
#define RTL_SOFT_PHY_STEP_STATUS 1u
#define RTL_SOFT_PHY_STEP_PHYAR  2u
#define RTL_SOFT_PHY_STEP_LINK   3u
#define RTL_SOFT_PHY_STEP_COUNT  3u

/*
 * Soft NAPI residual (C2 deepen under prod step 5).
 * Budget / TX done / RX refill catalog — never Own handoff / poll.
 */
#define RTL_SOFT_NAPI_STEP_BUDGET  1u
#define RTL_SOFT_NAPI_STEP_TX_DONE 2u
#define RTL_SOFT_NAPI_STEP_RX_FILL 3u
#define RTL_SOFT_NAPI_STEP_COUNT   3u

/*
 * Soft TX/RX residual (C2 deepen under prod step 6).
 * FS/LS desc spirit / TPPOLL kick / net door attach — never wire I/O.
 */
#define RTL_SOFT_TXRX_STEP_FSLS   1u
#define RTL_SOFT_TXRX_STEP_TPPOLL 2u
#define RTL_SOFT_TXRX_STEP_NET    3u
#define RTL_SOFT_TXRX_STEP_COUNT  3u

/* Soft NAPI residual budget (catalog constant only; never polled). */
#define RTL_SOFT_NAPI_BUDGET      64u

/*
 * Soft wire-handoff residual (Dual DoD B product direction via UDX — OPEN).
 * Ordered *names* of next program steps for product wire ownership.
 * Soft residual never executes these writes; freestanding owns wire interim.
 * Public register tags only (TNPDS/RDSAR/CR/IMR/ISR/PHYAR/TPPoll/…).
 * greppable: rtl8168_udx: soft wire handoff / soft wire step=
 *
 *   1 product caps mint (MMIO_FRAME / IRQ notify / DMA window)
 *   2 ring residual ready (Own=0 EOR last — soft may already hold)
 *   3 TNPDS program (TX normal desc base 64-bit)
 *   4 RDSAR program (RX desc base 64-bit)
 *   5 CR TE|RE arm (public TE|RE)
 *   6 PHY / link (PHYStatus / PHYAR product path)
 *   7 NAPI-style reclaim / poll
 *   8 TX/RX + TPPoll + netstack attach
 *   9 wire_owner flip freestanding → UDX (product handoff OPEN)
 */
#define RTL_SOFT_WIRE_STEP_CAPS   1u
#define RTL_SOFT_WIRE_STEP_RING   2u
#define RTL_SOFT_WIRE_STEP_TNPDS  3u
#define RTL_SOFT_WIRE_STEP_RDSAR  4u
#define RTL_SOFT_WIRE_STEP_TE_RE  5u
#define RTL_SOFT_WIRE_STEP_PHY    6u
#define RTL_SOFT_WIRE_STEP_NAPI   7u
#define RTL_SOFT_WIRE_STEP_TXRX   8u
#define RTL_SOFT_WIRE_STEP_OWNER  9u
#define RTL_SOFT_WIRE_STEP_COUNT  9u

/* Soft descriptor residual view (host CPU endian; not a silicon dump). */
struct rtl8168_soft_desc {
    u32 u32Opts1;  /* Own | EOR | FS | LS | len… */
    u32 u32Opts2;
    u32 u32AddrLo;
    u32 u32AddrHi;
};
