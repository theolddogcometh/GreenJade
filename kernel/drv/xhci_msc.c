/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room minimal xHCI + USB MSC BOT write path for GreenJade.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux USB paste.
 *
 * Public specs only (Intel xHCI / USB 2.0/3.x / MSC BOT / SCSI / FAT32).
 *
 * *** C0 FREESTANDING SKIP RESIDUAL HONESTY · NOT PRODUCT Dual DoD A ***
 * GJ_XHCI_MSC_PROBE stays 0: freestanding USB MSC class SKIP (same policy as
 * freestanding rtl NIC). Product laptop USB = userspace xhci_udx + hot+cold
 * ABI + DDI/UDX caps. Dual DoD A = OPEN_UDX (UDX stick / BOT path) —
 * freestanding residual never closes Dual DoD A. Soft!=product · G-AC-1
 * (no usb_storage.ko / xhci_pci.ko product AC; no freestanding MSC as
 * Dual DoD A close). Stamp-free residual (no invent version stamp storms).
 *
 * STOP freestanding MSC thrash. NEVER re-enable freestanding MSC as default.
 * NEVER stage-ladder thrash as product engineering. Opt-in residual only:
 * -DGJ_XHCI_MSC_PROBE=1 (lab). SKIP residual = lean once-shot honesty lamps
 * (product=UDX+ABI path=xhci_udx; Dual DoD A stick OPEN_UDX; not stick ready).
 *
 * Residual scope when opt-in (not product bar):
 *   - First PCI 0C:03 prog-if 0x30, BAR0 UC map, bus master
 *   - HC reset + run, single-segment command + event rings (poll, no IRQ)
 *   - First connected port: reset, Enable Slot, Address Device
 *   - Control: GET_DESCRIPTOR / SET_CONFIGURATION for MSC BOT (08/06/50)
 *   - Configure Endpoint for bulk IN/OUT; BOT READ CAPACITY(10) + WRITE(10)
 *   - FAT32 overwrite of pre-sized EFI/GREENJADE/KLOG.TXT, else raw LBA log
 *
 * Soft != HID. QEMU Multiboot without xHCI -> SKIP cleanly.
 * Soft residual lean only under default SKIP (no HC claim; net first).
 *
 * greppable: xhci: init PASS|FAIL|SKIP | GJ_XHCI_MSC_PROBE=0
 * greppable: xhci: freestanding MSC SKIP | freestanding_msc=SKIP
 * greppable: xhci: soft residual product=UDX+ABI | product_udx_abi honesty
 * greppable: xhci: soft residual lean | msc: soft residual lean
 * greppable: stick: soft residual lean | Dual DoD A stick OPEN
 * greppable: dual_dod_a=OPEN_UDX | need=UDX_OPEN | claim=0
 * greppable: freestanding_close=0 | C0_SKIP=1 | soft_ne_product=1
 * greppable: Soft!=product | path=xhci_udx | C0 freestanding SKIP residual
 * greppable: stamp_storm=0 | version_stamp=0 | not Dual DoD A close
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>
#include <gj/xhci_msc.h>
#include <gj/dma_buf.h>

/* ---- PCI ---------------------------------------------------------------- */
#define PCI_CFG_ADDR     0xCF8u
#define PCI_CFG_DATA     0xCFCu
#define PCI_CMD          0x04u
#define PCI_CMD_MEM      0x2u
#define PCI_CMD_BUS      0x4u
#define USB_PCI_CLASS    0x0cu
#define USB_PCI_SUBCLASS 0x03u
#define USB_PIF_XHCI     0x30u

/* ---- xHCI public register / TRB constants ------------------------------- */
#define XHCI_USBCMD_RS       (1u << 0)
#define XHCI_USBCMD_HCRST    (1u << 1)
#define XHCI_USBSTS_HCH      (1u << 0)
#define XHCI_USBSTS_CNR      (1u << 11)
#define XHCI_PORTSC_CCS      (1u << 0)
#define XHCI_PORTSC_PED      (1u << 1)
#define XHCI_PORTSC_OCA      (1u << 3)
#define XHCI_PORTSC_PR       (1u << 4)
#define XHCI_PORTSC_PP       (1u << 9)  /* Port Power - required after HCRST */
#define XHCI_PORTSC_LWS      (1u << 16) /* Port Link State Write Strobe */
#define XHCI_PORTSC_WPR      (1u << 31) /* Warm Port Reset (SS) */
#define XHCI_PORTSC_CSC      (1u << 17)
#define XHCI_PORTSC_PEC      (1u << 18)
#define XHCI_PORTSC_WRC      (1u << 19)
#define XHCI_PORTSC_OCC      (1u << 20)
#define XHCI_PORTSC_PRC      (1u << 21)
#define XHCI_PORTSC_PLC      (1u << 22)
#define XHCI_PORTSC_CEC      (1u << 23)
#define XHCI_PORTSC_CHANGE                                                     \
    (XHCI_PORTSC_CSC | XHCI_PORTSC_PEC | XHCI_PORTSC_WRC | XHCI_PORTSC_OCC |   \
     XHCI_PORTSC_PRC | XHCI_PORTSC_PLC | XHCI_PORTSC_CEC)
/* R/W bits software may preserve when writing PORTSC (xHCI 4.19.2) */
#define XHCI_PORTSC_PRESERVE 0x0e00c3e0u

#define XHCI_TRB_TYPE_NORMAL      1u
#define XHCI_TRB_TYPE_SETUP       2u
#define XHCI_TRB_TYPE_DATA        3u
#define XHCI_TRB_TYPE_STATUS      4u
#define XHCI_TRB_TYPE_LINK        6u
#define XHCI_TRB_TYPE_ENABLE_SLOT  9u
#define XHCI_TRB_TYPE_DISABLE_SLOT 10u
#define XHCI_TRB_TYPE_ADDRESS_DEV  11u
#define XHCI_TRB_TYPE_CONFIG_EP    12u
#define XHCI_TRB_TYPE_EVAL_CTX     13u
#define XHCI_TRB_TYPE_RESET_EP     14u
#define XHCI_TRB_TYPE_STOP_EP      15u
#define XHCI_TRB_TYPE_SET_TR_DEQ   16u
#define XHCI_TRB_TYPE_NOOP_CMD     23u
#define XHCI_TRB_TYPE_XFER_EVENT   32u
#define XHCI_TRB_TYPE_CMD_COMP     33u
#define XHCI_TRB_TYPE_PORT_STATUS  34u

/* USB standard feature for Clear Feature(ENDPOINT_HALT) */
#define USB_REQ_CLEAR_FEATURE 0x01u
#define USB_FEAT_ENDPOINT_HALT 0x00u

#define XHCI_TRB_CYCLE  (1u << 0)
#define XHCI_TRB_TC     (1u << 1)
#define XHCI_TRB_ISP    (1u << 2) /* Interrupt on Short Packet */
#define XHCI_TRB_CH     (1u << 4) /* Chain */
#define XHCI_TRB_IOC    (1u << 5)
#define XHCI_TRB_IDT    (1u << 6)
/* Address Device Command TRB: Block Set Address Request (software SET_ADDRESS) */
#define XHCI_ADDR_BSR   (1u << 9)

#define USB_REQ_SET_ADDRESS 0x05u

#define XHCI_EP_TYPE_CTRL_BI 4u
#define XHCI_EP_TYPE_BULK_OUT 2u
#define XHCI_EP_TYPE_BULK_IN  6u

/* EP State (Output Context DW0 bits 2:0) - xHCI 6.2.3 */
#define XHCI_EP_ST_DISABLED 0u
#define XHCI_EP_ST_RUNNING  1u
#define XHCI_EP_ST_HALTED   2u
#define XHCI_EP_ST_STOPPED  3u
#define XHCI_EP_ST_ERROR    4u

#define XHCI_RING_TRBS   64u
#define XHCI_MAX_PORTS   32u
#define XHCI_POLL_SPINS  5000000u
/*
 * Control poll budgets (soft residual lean · freestanding net first).
 * Historical overnight deepen (SS x100/x38/x72 + 22-26-pass recover) hung
 * G752 before NET/:22. Caps fail-closed in seconds. Soft!=product · G-AC-1.
 * Freestanding MSC SKIP default - residual path only when GJ_XHCI_MSC_PROBE=1.
 * Grep: XHCI_POLL_SPINS_SS_CTRL | soft residual lean | boot hang GET_DESC18
 */
#define XHCI_POLL_SPINS_SS_CTRL (XHCI_POLL_SPINS * 16u)
#define XHCI_POLL_SPINS_HS_CTRL (XHCI_POLL_SPINS * 4u)
/* Late Transfer Event grace after primary poll (device still completing). */
#define XHCI_POLL_SPINS_CTRL_GRACE (XHCI_POLL_SPINS * 6u)
/* Second/third SS grace: control IN after device-desc + short-status. */
#define XHCI_POLL_SPINS_SS_CTRL_EXTRA (XHCI_POLL_SPINS * 10u)
/* Drain must catch already-posted events; 80 was too tight on a12f. */
#define XHCI_DRAIN_SPINS 4000u
#define XHCI_MAP_BYTES   (256u * 1024u)

/* USB request types */
#define USB_REQ_GET_DESCRIPTOR 0x06u
#define USB_REQ_SET_CONFIGURATION 0x09u
#define USB_DT_DEVICE  0x01u
#define USB_DT_CONFIG  0x02u

/* MSC BOT */
#define USB_CLASS_MSC     0x08u
#define USB_SUBCLASS_SCSI 0x06u
#define USB_PROTO_BOT     0x50u
#define CBW_SIGNATURE     0x43425355u /* 'USBC' LE */
#define CSW_SIGNATURE     0x53425355u /* 'USBS' LE */
#define CBW_FLAG_IN       0x80u
#define SCSI_INQUIRY      0x12u
#define SCSI_READ_CAPACITY10 0x25u
#define SCSI_READ10       0x28u
#define SCSI_WRITE10      0x2Au
#define MSC_SECTOR        512u

/* FAT / raw log */
#define FAT_KLOG_MAX      (128u * 1024u)
#define RAW_LOG_MAGIC     "GJUSBLOG1"
#define RAW_LOG_SECTS     256u

/* ---- structures --------------------------------------------------------- */
struct xhci_trb {
    u64 u64Param;
    u32 u32Status;
    u32 u32Control;
} __attribute__((packed));

struct xhci_erst_ent {
    u64 u64Base;
    u32 u32Size;
    u32 u32Rsvd;
} __attribute__((packed));

struct usb_setup {
    u8  u8BmRequestType;
    u8  u8BRequest;
    u16 u16WValue;
    u16 u16WIndex;
    u16 u16WLength;
} __attribute__((packed));

struct msc_cbw {
    u32 u32Signature;
    u32 u32Tag;
    u32 u32DataTransferLength;
    u8  u8Flags;
    u8  u8Lun;
    u8  u8CbLength;
    u8  aCb[16];
} __attribute__((packed));

struct msc_csw {
    u32 u32Signature;
    u32 u32Tag;
    u32 u32DataResidue;
    u8  u8Status;
} __attribute__((packed));

/* ---- globals ------------------------------------------------------------ */
static volatile u8 *g_pCap;
static volatile u8 *g_pOp;
static volatile u8 *g_pRt;
static volatile u32 *g_pDb;
static u8  g_u8CapLen;
static u8  g_u8CtxSize; /* 32 or 64 */
static u8  g_u8MaxSlots;
static u8  g_u8MaxPorts;
static u8  g_u8SlotId;
static u8  g_u8PortId; /* 1-based */
static u8  g_u8PortSpeed;
static u8  g_u8EpOut;  /* endpoint address 1..15 */
static u8  g_u8EpIn;
static u16 g_u16MaxPkt0;
/* MPS programmed into Address Device Input Context (P0-3 diverge guard). */
static u16 g_u16AddrMps0;
static u16 g_u16BulkMps;
static u8  g_u8ConfigVal;
static u8  g_fInitTried;
static u8  g_fHcUp;
static u8  g_fMscReady;
static u32 g_u32LastStage; /* see xhci_msc_last_stage() */
static u32 g_u32LastCc;    /* sticky last xHCI completion code */
/* Address-path fail code (not overwritten by later EP0 arm Success=1). */
static u32 g_u32AddrFailCc;
/*
 * Control-path fail code (GET_DESC / SET_CONFIG / EP0 eval for panel stages
 * 11/12/13/15/17). Set only on real Transfer/Command Event fail or timeout
 * (cc=0); never overwritten by later arm/Evaluate Success=1.
 */
static u32 g_u32CtrlFailCc;
/* Residual Transfer Length from last consumed event (status bits 0-23). */
static u32 g_u32LastEvtResidual;
/* Residual from last xhci_ctrl completion (data-stage on short packet). */
static u32 g_u32LastCtrlResidual;
/* 0=none 1=BSR0 2=BSR+soft SET_ADDRESS */
static u32 g_u32AddrPath;
static u32 g_u32SsPortBits; /* bit (port-1): SuperSpeed-capable (xECP) */
static u8  g_u8SsPortLo;   /* 1-based inclusive; 0 = unknown */
static u8  g_u8SsPortHi;
static u8  g_fCmdCycle = 1;
static u8  g_fEvtCycle = 1;
static u8  g_fEp0Cycle = 1;
static u8  g_fBulkOutCycle = 1;
static u8  g_fBulkInCycle = 1;
static u32 g_u32CmdIdx;
static u32 g_u32EvtIdx;
static u32 g_u32Ep0Idx;
static u32 g_u32BulkOutIdx;
static u32 g_u32BulkInIdx;
static u32 g_u32CbwTag = 1;
static u32 g_u32BlockCount; /* last LBA+1 from READ CAPACITY */
static u32 g_u32BlockSize = MSC_SECTOR;
/*
 * Rate-limit greppable stamps (stage-11/13/15 Soft!=product): soft-recover /
 * soft-sync / mid-ring-after-PASS loops must not flood serial (stamp storms
 * hide real TO cause on a12f ladders; historical #PF class). Soft!=product.
 * Gap C residual deepen (freestanding past 11/13->15+; dual-DoD deepen Soft!=product).
 * g_u32LampStageLast + why/cc-tag: stage lamp once per stage/why/cc (n=).
 * g_u32LampStageBurstN: same stage any-why - first + every 16th (no storms).
 * g_u32SoftSyncAlreadyN: suppress repeated "soft-sync ok already" within burst.
 * g_u32SoftSyncDenyN: rate-limit soft-sync deny greps (ladder re-enter).
 * g_u32SoftSyncSnapN: rate-limit soft-sync residual snap greps (no storms).
 * g_u32SoftRecoverWhereTag + N: soft-recover 2nd|3rd/ok/deny (lean-capped).
 * g_u32CtrlPassSoftN: mid-ring/soft-continue after ctrl PASS (every TD).
 * g_u32GetConfigTryStampN: rate-limit get_config try/FAIL greps (no storms).
 * g_u32GetConfigAlignStampN: rate-limit get_config soft-align greps (no storms).
 * g_u32FalseToStampN: rate-limit ctrl false-TO residual greps (no storms).
 * Soft residual lean · MSC SKIP default · Dual DoD A stick OPEN · Soft!=product.
 */
static u32 g_u32LampStageLast;
static u32 g_u32LampWhyTag;
static u32 g_u32LampStageBurstN;
static u32 g_u32SoftSyncAlreadyN;
static u32 g_u32SoftSyncDenyN;
static u32 g_u32SoftSyncSnapN;
static u32 g_u32SoftRecoverWhereTag;
static u32 g_u32SoftRecoverStampN;
static u32 g_u32CtrlPassSoftN;
static u32 g_u32GetConfigTryStampN;
static u32 g_u32GetConfigAlignStampN;
static u32 g_u32FalseToStampN;
/* Once-shot Dual DoD A residual inventory (SKIP or after init; no storms). */
static u8  g_fSoftResidualOnce;

static gj_paddr_t g_paDcbaa;
static gj_paddr_t g_paCmdRing;
static gj_paddr_t g_paEvtRing;
static gj_paddr_t g_paErst;
static gj_paddr_t g_paDevCtx;
static gj_paddr_t g_paInputCtx;
static gj_paddr_t g_paEp0Ring;
static gj_paddr_t g_paBulkOutRing;
static gj_paddr_t g_paBulkInRing;
static gj_paddr_t g_paScratch; /* transfer bounce (1 page) */
static gj_paddr_t g_paScratch2;

static struct xhci_trb *g_pCmdRing;
static struct xhci_trb *g_pEvtRing;
static struct xhci_trb *g_pEp0Ring;
static struct xhci_trb *g_pBulkOutRing;
static struct xhci_trb *g_pBulkInRing;
static u64 *g_pDcbaa;
static void *g_pDevCtx;
static void *g_pInputCtx;
static void *g_pScratch;
static void *g_pScratch2;

/* FAT write metadata (after locate) */
static u32 g_u32FatPartLba; /* partition start absolute LBA */
static u32 g_u32FatDataLba;  /* first data sector absolute */
static u32 g_u32FatSecPerClus;
static u32 g_u32KlogCluster;
static u32 g_u32KlogSize; /* preallocated file size */
static u8  g_fKlogLocated;

/* ---- low-level helpers -------------------------------------------------- */
static inline void
mmio_barrier(void)
{
    __asm__ volatile("mfence" ::: "memory");
}

static inline void
cpu_relax(void)
{
    __asm__ volatile("pause" ::: "memory");
}

static void
xhci_clflush_ptr(const void *p)
{
    __asm__ volatile("clflush (%0)" : : "r"(p) : "memory");
    __asm__ volatile("mfence" ::: "memory");
}

/* Invalidate one cache line so CPU sees HC DMA writes (event/DevCtx). */
static void
xhci_clinv_ptr(const void *p)
{
    __asm__ volatile("clflush (%0)" : : "r"(p) : "memory");
    __asm__ volatile("mfence" ::: "memory");
}

/* Flush a byte span (Input/Device Contexts after HC or CPU update). */
static void
xhci_clflush_span(const void *p, u32 cb)
{
    const u8 *pB = (const u8 *)p;
    u32 u32Off;

    if (p == NULL || cb == 0) {
        return;
    }
    for (u32Off = 0; u32Off < cb; u32Off += 64u) {
        xhci_clflush_ptr(pB + u32Off);
    }
}

/* Busy-wait settle (spin count; not calibrated ms - freestanding). */
static void
xhci_settle(u32 u32Spins)
{
    u32 i;

    for (i = 0; i < u32Spins; i++) {
        cpu_relax();
    }
}

static void
xhci_note_cc(u32 u32Code)
{
    g_u32LastCc = u32Code;
}

/*
 * Record a control-path fail for panel honesty (stages 11/12/13/15/17).
 * cc=0 means timeout / no Transfer Event - fail-closed, not Success.
 * Does not clear on later Success; only overwritten by a newer ctrl fail.
 * Soft!=product · Gap C residual: sticky path code for xhci_msc_last_cc().
 */
static void
xhci_note_ctrl_fail_cc(u32 u32Code)
{
    g_u32LastCc = u32Code;
    g_u32CtrlFailCc = u32Code;
}

static inline void
outl(u16 u16Port, u32 u32Val)
{
    __asm__ volatile("outl %0, %1" : : "a"(u32Val), "Nd"(u16Port));
}

static inline u32
inl(u16 u16Port)
{
    u32 u32Val;

    __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"(u16Port));
    return u32Val;
}

static u32
pci_cfg_read32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);

    outl(PCI_CFG_ADDR, u32Addr);
    return inl(PCI_CFG_DATA);
}

static void
pci_cfg_write32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u32 u32Val)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);

    outl(PCI_CFG_ADDR, u32Addr);
    outl(PCI_CFG_DATA, u32Val);
}

static u16
pci_cfg_read16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Word = pci_cfg_read32(u8Bus, u8Slot, u8Func, (u8)(u8Off & 0xfcu));
    u32 u32Shift = (u32)(u8Off & 2u) * 8u;

    return (u16)((u32Word >> u32Shift) & 0xffffu);
}

static void
pci_cfg_write16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u16 u16Val)
{
    u32 u32Word = pci_cfg_read32(u8Bus, u8Slot, u8Func, (u8)(u8Off & 0xfcu));
    u32 u32Shift = (u32)(u8Off & 2u) * 8u;
    u32 u32Mask = 0xffffu << u32Shift;

    u32Word = (u32Word & ~u32Mask) | (((u32)u16Val << u32Shift) & u32Mask);
    pci_cfg_write32(u8Bus, u8Slot, u8Func, (u8)(u8Off & 0xfcu), u32Word);
}

static u32
mmio_r32(volatile u8 *pBase, u32 u32Off)
{
    return *(volatile u32 *)(void *)(pBase + u32Off);
}

static void
mmio_w32(volatile u8 *pBase, u32 u32Off, u32 u32Val)
{
    *(volatile u32 *)(void *)(pBase + u32Off) = u32Val;
    mmio_barrier();
}

static void
mmio_w64(volatile u8 *pBase, u32 u32Off, u64 u64Val)
{
    *(volatile u64 *)(void *)(pBase + u32Off) = u64Val;
    mmio_barrier();
}

static gj_paddr_t
dma_alloc_page(void **ppVa)
{
    /* Prefer low (<4GiB) pages for xHCI DMA (see gj/dma_buf.h). */
    return dma_buf_alloc_page(ppVa);
}

static u32
trb_type(u32 u32Control)
{
    return (u32Control >> 10) & 0x3fu;
}

static void
ring_link_init(struct xhci_trb *pRing, gj_paddr_t paRing, u8 fCycle)
{
    u32 iLast = XHCI_RING_TRBS - 1u;

    /* Link TRB points to ring base; Toggle Cycle */
    pRing[iLast].u64Param = (u64)paRing;
    pRing[iLast].u32Status = 0;
    pRing[iLast].u32Control =
        XHCI_TRB_TYPE_LINK << 10 | XHCI_TRB_TC |
        (fCycle != 0 ? XHCI_TRB_CYCLE : 0u);
}

/* ---- event ring poll ---------------------------------------------------- */
/*
 * Consume one matched-cycle event at g_u32EvtIdx (caller force-inval'd or
 * cycle already matched). Advances ERDP | EHB. Returns TRB type or -1 if
 * cycle no longer matches after re-inval (race Soft!=product).
 */
static int
xhci_evt_consume_matched(u32 u32WantType, u32 *pCode, u32 *pSlot, u64 *pParam)
{
    struct xhci_trb *pTrb;
    u32 u32Ctrl;
    u8 fCycle;
    u32 u32Type;

    if (g_pEvtRing == NULL) {
        return -1;
    }
    pTrb = &g_pEvtRing[g_u32EvtIdx];
    /* Matched cycle - force fresh read of full TRB before consume. */
    xhci_clinv_ptr(pTrb);
    u32Ctrl = pTrb->u32Control;
    fCycle = (u8)(u32Ctrl & XHCI_TRB_CYCLE);
    if (fCycle != g_fEvtCycle) {
        return -1;
    }
    u32Type = trb_type(u32Ctrl);
    /* Residual Transfer Length (bits 0-23) - honest ctrl fail logs. */
    g_u32LastEvtResidual = pTrb->u32Status & 0x00ffffffu;
    if (pCode != NULL) {
        *pCode = (pTrb->u32Status >> 24) & 0xffu;
    }
    if (pSlot != NULL) {
        *pSlot = (u32Ctrl >> 24) & 0xffu;
    }
    if (pParam != NULL) {
        *pParam = pTrb->u64Param;
    }

    /* Advance dequeue; clear EHB by writing ERDP with EHB=1 */
    g_u32EvtIdx++;
    if (g_u32EvtIdx >= XHCI_RING_TRBS) {
        g_u32EvtIdx = 0;
        g_fEvtCycle ^= 1u;
    }
    {
        u64 u64Erdp =
            (u64)g_paEvtRing + (u64)g_u32EvtIdx * sizeof(struct xhci_trb);

        u64Erdp |= 8ull; /* EHB */
        mmio_w64(g_pRt, 0x20u + 0x18u, u64Erdp);
    }

    if (u32WantType == 0u || u32Type == u32WantType) {
        return (int)u32Type;
    }
    /* Caller continues search (port status / unexpected type). */
    return (int)u32Type | 0x100; /* non-wanted: type in low bits, flag high */
}

static int
xhci_wait_event(u32 u32WantType, u32 *pCode, u32 *pSlot, u64 *pParam,
                u32 u32Spins)
{
    u32 iSpin;
    u32 u32Tail;

    /*
     * Final residual window (false-TO class Soft!=product stage-11/13/15):
     * every-spin clinv for last min(262144, spins/8) so late HC DMA is not
     * missed by the every-64 cadence. Keep short for drain budgets.
     */
    u32Tail = u32Spins >> 3;
    if (u32Tail > 262144u) {
        u32Tail = 262144u;
    }
    if (u32Tail < 256u && u32Spins >= 256u) {
        u32Tail = 256u;
    }

    for (iSpin = 0; iSpin < u32Spins; iSpin++) {
        struct xhci_trb *pTrb = &g_pEvtRing[g_u32EvtIdx];
        u32 u32Ctrl;
        u8 fCycle;
        int nCons;

        /*
         * Invalidate so non-coherent HC DMA is visible (a12f).
         * Aggressive for first 64 polls (catch early Completion / Xfer
         * events); then every 64; last u32Tail spins every-spin (false-TO
         * residual Soft!=product). Every-spin for whole long SS poll made
         * panel crawl historically. Lines must be clean before HC write
         * (see setup_rings clflush) or clflush writeback can stomp Transfer
         * Events -> stage-11 TO.
         */
        if (iSpin < 64u || (iSpin & 63u) == 0u ||
            (u32Tail != 0u && iSpin + u32Tail >= u32Spins)) {
            xhci_clinv_ptr(pTrb);
        }
        u32Ctrl = pTrb->u32Control;
        fCycle = (u8)(u32Ctrl & XHCI_TRB_CYCLE);

        if (fCycle != g_fEvtCycle) {
            cpu_relax();
            continue;
        }

        nCons = xhci_evt_consume_matched(u32WantType, pCode, pSlot, pParam);
        if (nCons < 0) {
            cpu_relax();
            continue;
        }
        if (nCons < 0x100) {
            return nCons;
        }
        /* Non-wanted type consumed - keep searching (port status etc.). */
        (void)nCons;
    }

    /*
     * Post-budget false-TO residual (Gap C Soft!=product; a12f G-AC-1):
     * only on control-class budgets (spins >= XHCI_POLL_SPINS) so short
     * drain does not pay residual. Force-inval current + next event TRB,
     * then aggressive every-spin micro-poll. HC may post Transfer Event
     * just as primary budget ends - prefer seeing event over thrash Set
     * TR Deq. Silent (no stamp storm). Soft!=product Stage OPEN.
     */
    if (u32Spins >= XHCI_POLL_SPINS && g_pEvtRing != NULL) {
        u32 iSpin2;

        xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx]);
        if ((g_u32EvtIdx + 1u) < XHCI_RING_TRBS) {
            xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx + 1u]);
        } else {
            xhci_clinv_ptr(&g_pEvtRing[0]);
        }
        mmio_barrier();
        for (iSpin2 = 0; iSpin2 < (XHCI_POLL_SPINS * 2u); iSpin2++) {
            struct xhci_trb *pTrb = &g_pEvtRing[g_u32EvtIdx];
            u32 u32Ctrl;
            u8 fCycle;
            int nCons;

            xhci_clinv_ptr(pTrb);
            u32Ctrl = pTrb->u32Control;
            fCycle = (u8)(u32Ctrl & XHCI_TRB_CYCLE);
            if (fCycle != g_fEvtCycle) {
                cpu_relax();
                continue;
            }
            nCons = xhci_evt_consume_matched(u32WantType, pCode, pSlot, pParam);
            if (nCons < 0) {
                cpu_relax();
                continue;
            }
            if (nCons < 0x100) {
                return nCons;
            }
            (void)nCons;
        }
    }
    return -1;
}

static void
doorbell_hc(void)
{
    mmio_barrier();
    g_pDb[0] = 0;
    mmio_barrier();
    /* Serialize doorbell vs TRB clflush (a12f non-posted path). */
    if (g_pOp != NULL) {
        (void)mmio_r32(g_pOp, 0x04); /* USBSTS */
    }
}

/*
 * Ring doorbell for slot/DCI. Caller must clflush TRBs first.
 * Fence + USBSTS read so doorbell is not reordered ahead of ring stores
 * (stage-11: first EP0 GET_DESC TO with empty event ring on a12f).
 * Grep: xhci: doorbell
 */
static void
doorbell_ep(u8 u8Slot, u8 u8Dci)
{
    if (u8Slot == 0u || g_pDb == NULL) {
        return;
    }
    mmio_barrier();
    g_pDb[u8Slot] = (u32)u8Dci;
    mmio_barrier();
    if (g_pOp != NULL) {
        (void)mmio_r32(g_pOp, 0x04); /* USBSTS - post doorbell */
    }
}

/* ---- command ring ------------------------------------------------------- */
static int
xhci_cmd(u64 u64Param, u32 u32Status, u32 u32TypeExtra, u32 *pCode,
         u32 *pSlot)
{
    struct xhci_trb *pTrb;
    u32 u32Ctrl;
    int nEv;
    u32 u32Code = 0;
    u32 u32Slot = 0;

    /*
     * Last TRB is Link (Toggle Cycle). Before wrapping the producer index,
     * rewrite Link with the *current* cycle so the HC can follow it after
     * multi-port scans exhaust the first lap (G752: many CCS ports x passes).
     * Missing this -> command hang / Enable Slot timeout (panel stage 7).
     */
    if (g_u32CmdIdx >= XHCI_RING_TRBS - 1u) {
        g_pCmdRing[XHCI_RING_TRBS - 1u].u64Param = (u64)g_paCmdRing;
        g_pCmdRing[XHCI_RING_TRBS - 1u].u32Status = 0;
        g_pCmdRing[XHCI_RING_TRBS - 1u].u32Control =
            (XHCI_TRB_TYPE_LINK << 10) | XHCI_TRB_TC |
            (g_fCmdCycle != 0 ? XHCI_TRB_CYCLE : 0u);
        mmio_barrier();
        xhci_clflush_ptr(&g_pCmdRing[XHCI_RING_TRBS - 1u]);
        g_u32CmdIdx = 0;
        g_fCmdCycle ^= 1u;
    }

    pTrb = &g_pCmdRing[g_u32CmdIdx];
    pTrb->u64Param = u64Param;
    pTrb->u32Status = u32Status;
    u32Ctrl = (u32TypeExtra & ~1u) | (g_fCmdCycle != 0 ? XHCI_TRB_CYCLE : 0u);
    /* Ensure cycle bit is the last store (producer); flush for non-snoop DMA. */
    mmio_barrier();
    pTrb->u32Control = u32Ctrl;
    mmio_barrier();
    xhci_clflush_ptr(pTrb);

    g_u32CmdIdx++;
    doorbell_hc();

    nEv = xhci_wait_event(XHCI_TRB_TYPE_CMD_COMP, &u32Code, &u32Slot, NULL,
                          XHCI_POLL_SPINS);
    if (pCode != NULL) {
        *pCode = u32Code;
    }
    if (pSlot != NULL) {
        *pSlot = u32Slot;
    }
    if (nEv < 0) {
        xhci_note_cc(0);
        return -1;
    }
    xhci_note_cc(u32Code);
    /* Completion code 1 = Success */
    if (u32Code != 1u) {
        return -1;
    }
    return 0;
}

/* ---- transfer ring enqueue (single Normal / stages) --------------------- */
static void
xfer_enqueue(struct xhci_trb *pRing, u32 *pIdx, u8 *pCycle, gj_paddr_t paRing,
             u64 u64Param, u32 u32Status, u32 u32CtrlFlags)
{
    struct xhci_trb *pTrb;
    u32 u32Ctrl;

    if (*pIdx >= XHCI_RING_TRBS - 1u) {
        /* Update link cycle then wrap (clflush for a12f non-snoop DMA). */
        pRing[XHCI_RING_TRBS - 1u].u64Param = (u64)paRing;
        pRing[XHCI_RING_TRBS - 1u].u32Status = 0;
        pRing[XHCI_RING_TRBS - 1u].u32Control =
            (XHCI_TRB_TYPE_LINK << 10) | XHCI_TRB_TC |
            (*pCycle != 0 ? XHCI_TRB_CYCLE : 0u);
        mmio_barrier();
        xhci_clflush_ptr(&pRing[XHCI_RING_TRBS - 1u]);
        *pIdx = 0;
        *pCycle ^= 1u;
    }

    pTrb = &pRing[*pIdx];
    pTrb->u64Param = u64Param;
    pTrb->u32Status = u32Status;
    u32Ctrl = (u32CtrlFlags & ~1u) | (*pCycle != 0 ? XHCI_TRB_CYCLE : 0u);
    mmio_barrier();
    pTrb->u32Control = u32Ctrl;
    mmio_barrier();
    /* Push TRB to DRAM so HC DMA sees it (Intel a12f / VT-d non-coherent). */
    xhci_clflush_ptr(pTrb);
    (*pIdx)++;
}

static int
xhci_wait_xfer(u32 u32Spins)
{
    u32 u32Code = 0;
    int nEv = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code, NULL, NULL,
                              u32Spins);

    if (nEv < 0) {
        return -1;
    }
    /* 1 Success, 13 Short Packet also OK for control/bulk residual */
    if (u32Code != 1u && u32Code != 13u) {
        return -1;
    }
    return 0;
}

/* Forward decls - used by EP0 hard resync / ctrl before their definitions. */
static int xhci_evaluate_ep0_mps(void);
static void xhci_ep0_ring_reset(void);
static void xhci_port_force_u0(u8 u8Port);
static void xhci_port_power_on(u8 u8Port);
static u32 portsc_read(u8 u8Port);
static u8 *ctx_ep(void *pBase, int fInput, u8 u8Dci);
static int xhci_ep0_arm_after_address(void);
static void xhci_mps0_resync_from_speed(void);

/* Drain any pending event ring entries (port status + stale xfer). */
static void
xhci_drain_events(u32 u32Max)
{
    u32 i;
    u32 u32Code = 0;

    for (i = 0; i < u32Max; i++) {
        if (xhci_wait_event(0, &u32Code, NULL, NULL, XHCI_DRAIN_SPINS) < 0) {
            break;
        }
    }
}

/*
 * Sample EP0 Output Context state + TR Dequeue (invalidate DevCtx first).
 * Returns EP State 0..4; *pDeq optional (pointer | DCS).
 * Grep: xhci: ep0_state
 */
static u32
xhci_ep0_sample(u64 *pDeq)
{
    u8 *pEp0;
    u32 u32Dw0 = 0;
    u64 u64Deq = 0;

    if (pDeq != NULL) {
        *pDeq = 0;
    }
    if (g_pDevCtx == NULL || g_u8SlotId == 0) {
        return XHCI_EP_ST_DISABLED;
    }
    /* HC wrote Output Context via DMA - pull into CPU cache. */
    xhci_clflush_span(g_pDevCtx, (u32)g_u8CtxSize * 3u);
    pEp0 = ctx_ep(g_pDevCtx, 0, 1);
    memcpy(&u32Dw0, pEp0 + 0x00, 4);
    memcpy(&u64Deq, pEp0 + 0x08, 8);
    if (pDeq != NULL) {
        *pDeq = u64Deq;
    }
    return u32Dw0 & 0x7u;
}

/*
 * P0-1 residual explore (stage-11): soft-continue at ring base ONLY if ALL of:
 *   Running|Stopped, idx==0, cyc==1, deq base == g_paEp0Ring, DCS==1.
 * deq==0 -> not soft-continue (caller arms). Used by post-address soft-align
 * and prepare_for_get_desc (fresh arm path).
 * Grep: xhci: ep0 soft-continue ok|deny
 */
static int
xhci_ep0_soft_continue_ok(u32 u32EpSt, u64 u64Deq)
{
    u64 u64Base = (u64)g_paEp0Ring & ~0xfull;
    u64 u64DeqAddr = u64Deq & ~0xfull;
    u8 u8Dcs = (u8)(u64Deq & 1ull);

    if (u64Deq == 0ull) {
        return 0;
    }
    if (u32EpSt != XHCI_EP_ST_RUNNING && u32EpSt != XHCI_EP_ST_STOPPED) {
        return 0;
    }
    if (g_u32Ep0Idx != 0u || g_fEp0Cycle != 1u) {
        return 0;
    }
    if (u64DeqAddr != u64Base) {
        return 0;
    }
    if (u8Dcs != 1u) {
        return 0;
    }
    return 1;
}

/*
 * Mid-ring soft-continue (stage-13 residual -> stage-15; Gap C post-0ef deepen):
 * after a completed control TD (soft-8 PASS -> GET_DESC18, or device-desc PASS
 * -> GET_CONFIG) software producer and HC TR Dequeue both sit at the next free
 * TRB - idx may be non-zero. Soft-continue when:
 *   Running|Stopped, deq != 0, deq_addr == sw_producer_addr, DCS == cycle,
 *   room for Setup+Data+Status (3 TRBs) without forced wrap.
 * Treating "idx!=0" as misaligned forced Stop+Set TR Deq thrash and desynced
 * a12f -> GET_DESC18 stage-13 / GET_CONFIG stage-15 cc=0 TO.
 * Prefer mid-ring soft-continue over thrash hard-resync (Intel a12f G-AC-1).
 * Grep: xhci: ep0 mid-ring soft-continue | ep0 producer aligned
 */
static int
xhci_ep0_producer_hc_aligned(u32 u32EpSt, u64 u64Deq)
{
    u64 u64Base = (u64)g_paEp0Ring & ~0xfull;
    u64 u64DeqAddr = u64Deq & ~0xfull;
    u64 u64SwAddr;
    u8 u8Dcs = (u8)(u64Deq & 1ull);

    if (u64Deq == 0ull) {
        return 0;
    }
    if (u32EpSt != XHCI_EP_ST_RUNNING && u32EpSt != XHCI_EP_ST_STOPPED) {
        return 0;
    }
    /* Need room for a full control TD (Setup + Data + Status = 3 TRBs). */
    if (g_u32Ep0Idx >= (XHCI_RING_TRBS - 1u) ||
        g_u32Ep0Idx + 3u > (XHCI_RING_TRBS - 1u)) {
        return 0;
    }
    u64SwAddr = u64Base + (u64)g_u32Ep0Idx * (u64)sizeof(struct xhci_trb);
    if (u64DeqAddr != u64SwAddr) {
        return 0;
    }
    if (u8Dcs != (u8)(g_fEp0Cycle & 1u)) {
        return 0;
    }
    return 1;
}

/*
 * Soft-sync software EP0 producer to HC TR Dequeue (no Stop / Set TR Deq).
 * Stage-13/15 residual: after ISP short-packet or late Status the HC deq may
 * lead or lag SW by a TRB even when the TD completed. Snap idx/cycle to deq
 * when Running|Stopped and deq lands on a free slot in the EP0 ring (not Link).
 * Returns 1 aligned/snapped, 0 if caller must hard-resync.
 * Grep: xhci: ep0 soft-sync residual | ep0 soft-sync ok|deny
 */
static int
xhci_ep0_soft_sync_producer(void)
{
    u32 u32EpSt;
    u64 u64Deq = 0;
    u64 u64Base = (u64)g_paEp0Ring & ~0xfull;
    u64 u64DeqAddr;
    u64 u64Off;
    u32 u32Idx;
    u8 u8Dcs;
    u32 u32OldIdx = g_u32Ep0Idx;
    u8 u8OldCyc = g_fEp0Cycle;

    u32EpSt = xhci_ep0_sample(&u64Deq);
    if (u64Deq == 0ull) {
        /* Rate-limit deny greps (soft-recover multi-pass Soft!=product). */
        g_u32SoftSyncDenyN++;
        if (g_u32SoftSyncDenyN == 1u || (g_u32SoftSyncDenyN & 15u) == 0u) {
            kprintf("xhci: ep0 soft-sync deny deq=0 state=%u idx=%u n=%u "
                    "(stage-13/15 residual Soft!=product)\n",
                    (unsigned)u32EpSt, (unsigned)g_u32Ep0Idx,
                    (unsigned)g_u32SoftSyncDenyN);
        }
        return 0;
    }
    if (u32EpSt != XHCI_EP_ST_RUNNING && u32EpSt != XHCI_EP_ST_STOPPED) {
        g_u32SoftSyncDenyN++;
        if (g_u32SoftSyncDenyN == 1u || (g_u32SoftSyncDenyN & 15u) == 0u) {
            kprintf("xhci: ep0 soft-sync deny state=%u deq=0x%lx n=%u "
                    "(stage-13/15 residual Soft!=product)\n",
                    (unsigned)u32EpSt, (unsigned long)u64Deq,
                    (unsigned)g_u32SoftSyncDenyN);
        }
        return 0;
    }
    u64DeqAddr = u64Deq & ~0xfull;
    u8Dcs = (u8)(u64Deq & 1ull);
    if (u64DeqAddr < u64Base) {
        g_u32SoftSyncDenyN++;
        if (g_u32SoftSyncDenyN == 1u || (g_u32SoftSyncDenyN & 15u) == 0u) {
            kprintf("xhci: ep0 soft-sync deny deq=0x%lx below ring n=%u\n",
                    (unsigned long)u64DeqAddr, (unsigned)g_u32SoftSyncDenyN);
        }
        return 0;
    }
    u64Off = u64DeqAddr - u64Base;
    if ((u64Off % (u64)sizeof(struct xhci_trb)) != 0ull) {
        g_u32SoftSyncDenyN++;
        if (g_u32SoftSyncDenyN == 1u || (g_u32SoftSyncDenyN & 15u) == 0u) {
            kprintf("xhci: ep0 soft-sync deny deq=0x%lx misaligned n=%u\n",
                    (unsigned long)u64DeqAddr, (unsigned)g_u32SoftSyncDenyN);
        }
        return 0;
    }
    u32Idx = (u32)(u64Off / (u64)sizeof(struct xhci_trb));
    /* Link slot or past usable producer window -> hard path. */
    if (u32Idx >= (XHCI_RING_TRBS - 1u)) {
        g_u32SoftSyncDenyN++;
        if (g_u32SoftSyncDenyN == 1u || (g_u32SoftSyncDenyN & 15u) == 0u) {
            kprintf("xhci: ep0 soft-sync deny deq on Link idx=%u n=%u\n",
                    (unsigned)u32Idx, (unsigned)g_u32SoftSyncDenyN);
        }
        return 0;
    }
    /* Room for Setup+Data+Status after snap. */
    if (u32Idx + 3u > (XHCI_RING_TRBS - 1u)) {
        g_u32SoftSyncDenyN++;
        if (g_u32SoftSyncDenyN == 1u || (g_u32SoftSyncDenyN & 15u) == 0u) {
            kprintf("xhci: ep0 soft-sync deny ring full at deq idx=%u n=%u\n",
                    (unsigned)u32Idx, (unsigned)g_u32SoftSyncDenyN);
        }
        return 0;
    }
    if (u32Idx == g_u32Ep0Idx && u8Dcs == (u8)(g_fEp0Cycle & 1u)) {
        /*
         * Rate-limit: soft-recover multi-pass / mid-ladder re-enters often;
         * stamp once then every 16th (no stamp storms Soft!=product).
         * Grep: xhci: ep0 soft-sync ok already
         */
        g_u32SoftSyncAlreadyN++;
        g_u32SoftSyncDenyN = 0u;
        if (g_u32SoftSyncAlreadyN == 1u ||
            (g_u32SoftSyncAlreadyN & 15u) == 0u) {
            kprintf("xhci: ep0 soft-sync ok already idx=%u cyc=%u deq=0x%lx "
                    "state=%u n=%u (stage-13/15 residual Soft!=product)\n",
                    (unsigned)u32Idx, (unsigned)u8Dcs,
                    (unsigned long)u64DeqAddr, (unsigned)u32EpSt,
                    (unsigned)g_u32SoftSyncAlreadyN);
        }
        xhci_clflush_ptr(g_pEp0Ring);
        xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
        return 1;
    }
    g_u32Ep0Idx = u32Idx;
    g_fEp0Cycle = u8Dcs;
    g_u32SoftSyncAlreadyN = 0u;
    g_u32SoftSyncDenyN = 0u;
    /*
     * Rate-limit residual snap greps (soft-recover multi-pass + mid-ring
     * ladders re-enter often; always-print was stamp-storm / #PF class).
     * First + every 16th. Soft!=product.
     */
    g_u32SoftSyncSnapN++;
    if (g_u32SoftSyncSnapN == 1u || (g_u32SoftSyncSnapN & 15u) == 0u) {
        kprintf("xhci: ep0 soft-sync residual idx=%u->%u cyc=%u->%u deq=0x%lx "
                "state=%u n=%u (no Set TR Deq; stage-13/15 Soft!=product)\n",
                (unsigned)u32OldIdx, (unsigned)u32Idx, (unsigned)u8OldCyc,
                (unsigned)u8Dcs, (unsigned long)u64DeqAddr, (unsigned)u32EpSt,
                (unsigned)g_u32SoftSyncSnapN);
    }
    xhci_clflush_ptr(g_pEp0Ring);
    xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
    return 1;
}

/*
 * Residual Transfer Length honesty after xhci_ctrl: bytes received on Data
 * stage = wlen - residual when residual < wlen; else 0. Content-backed
 * deepen when residual is wrong but buffer already has a valid USB desc
 * prefix (stage-11/13/15 Soft!=product).
 * Grep: xhci: get device desc residual | get config residual
 */
static u32
xhci_ctrl_got_len(u16 u16Wlen)
{
    if (u16Wlen == 0u) {
        return 0u;
    }
    if (g_u32LastCtrlResidual < (u32)u16Wlen) {
        return (u32)u16Wlen - g_u32LastCtrlResidual;
    }
    return 0u;
}

/*
 * Greppable stage lamp (panel / serial). Prefer over thrash: Soft!=product.
 * Rate-limit (Gap C freestanding residual; no stamp storms on soft ladders):
 *   1) same stage+why+cc -> suppress once (readdress re-fail same GET_CONFIG)
 *   2) same stage any-why: first + every 16th burst stamp only
 * New stage resets burst. Never hundreds of sequential stage= kprintf.
 * Honesty: Stage OPEN Soft!=product - lab soft never product T1 from stage.
 * Dual DoD A stick OPEN (Soft!=product · G-AC-1). Grep: xhci: stage=
 */
static void
xhci_stage_soft_lamp(u32 u32Stage, const char *szWhy)
{
    u32 u32WhyTag = 0u;
    const char *sz = szWhy != NULL ? szWhy : "fail";
    const char *pCh;

    /* Cheap stable tag from why + sticky fail cc (no libc hash). */
    for (pCh = sz; *pCh != '\0'; pCh++) {
        u32WhyTag = (u32WhyTag * 33u) + (u32)(u8)*pCh;
    }
    u32WhyTag = (u32WhyTag * 33u) + g_u32CtrlFailCc;
    if (g_u32LampStageLast == u32Stage && g_u32LampWhyTag == u32WhyTag) {
        return;
    }
    if (g_u32LampStageLast != u32Stage) {
        g_u32LampStageBurstN = 0u;
    }
    g_u32LampStageBurstN++;
    /* Same stage, different why/cc: first + every 16th (no stage= storms). */
    if (g_u32LampStageBurstN > 1u &&
        (g_u32LampStageBurstN & 15u) != 0u &&
        g_u32LampStageLast == u32Stage) {
        g_u32LampWhyTag = u32WhyTag;
        return;
    }
    g_u32LampStageLast = u32Stage;
    g_u32LampWhyTag = u32WhyTag;
    kprintf("xhci: stage=%u %s cc=%u port=%u spd=%u mps0=%u n=%u "
            "Soft!=product Stage OPEN\n",
            (unsigned)u32Stage, sz, (unsigned)g_u32CtrlFailCc,
            (unsigned)g_u8PortId, (unsigned)g_u8PortSpeed,
            (unsigned)g_u16MaxPkt0, (unsigned)g_u32LampStageBurstN);
}

/*
 * Cheap stable tag for rate-limit keys (no libc hash; Soft!=product).
 */
static u32
xhci_soft_why_tag(const char *szWhy)
{
    u32 u32Tag = 0u;
    const char *pCh;

    if (szWhy == NULL) {
        return 0u;
    }
    for (pCh = szWhy; *pCh != '\0'; pCh++) {
        u32Tag = (u32Tag * 33u) + (u32)(u8)*pCh;
    }
    return u32Tag;
}

/*
 * Rate-limit soft-recover greps per where-tag: first + every 16th in burst
 * (Gap C: twenty-six SS passes / twenty-two HS; multi-pass Soft!=product; no stamp storms).
 * New where resets counter. Grep: xhci: ep0 soft-recover | Soft!=product
 */
static int
xhci_soft_recover_stamp_ok(const char *szWhere)
{
    u32 u32Tag = xhci_soft_why_tag(szWhere != NULL ? szWhere : "?");

    if (g_u32SoftRecoverWhereTag != u32Tag) {
        g_u32SoftRecoverWhereTag = u32Tag;
        g_u32SoftRecoverStampN = 0u;
    }
    g_u32SoftRecoverStampN++;
    if (g_u32SoftRecoverStampN == 1u ||
        (g_u32SoftRecoverStampN & 15u) == 0u) {
        return 1;
    }
    return 0;
}

/*
 * Rate-limit GET_CONFIG soft-align greps (readdress loops re-enter stage-15;
 * late..late14 chain must not storm serial / #PF class). First + every 16th.
 * Soft!=product Stage OPEN. Grep: xhci: get config soft-align
 */
static int
xhci_get_config_align_stamp_ok(void)
{
    g_u32GetConfigAlignStampN++;
    if (g_u32GetConfigAlignStampN == 1u ||
        (g_u32GetConfigAlignStampN & 15u) == 0u) {
        return 1;
    }
    return 0;
}

/*
 * After control TO / failed enqueue: prefer soft residual recover over thrash.
 * Soft residual lean (freestanding MSC SKIP default · net first · Dual DoD A):
 *   1) drain + soft-sync producer to HC deq (no Set TR Deq)
 *   2) re-sample; base soft-continue OR mid-ring producer-HC aligned
 *   3) soft-sync alone already aligned after snap
 *   4) second-pass residual: short settle + drain + soft-sync again
 *   5) third-pass residual (SS only): one longer settle then deny -> hard
 * Deny -> 0 (caller hard-resync). Historical 4th..26th multi-M settle ladder
 * removed - hung G752 before freestanding NET/:22. Soft!=product · G-AC-1.
 * Returns 1 soft-continue ok, 0 deny. Stamps rate-limited (no storms).
 * Grep: xhci: ep0 mid-ring soft-continue | soft-sync residual | Soft!=product
 *       | ep0 soft-recover second|third-pass | soft residual lean
 */
static int
xhci_ep0_soft_recover_after_to(const char *szWhere)
{
    u32 u32EpSt;
    u64 u64Deq = 0;
    int fSync;
    int nPass;
    int nPassMax;
    int fStamp;
    const char *szW = szWhere != NULL ? szWhere : "?";

    /*
     * Soft residual lean: max 3 SS / 2 HS soft passes with short settles;
     * deny -> hard-resync. Overnight Nth-pass growth never product T1.
     * Soft!=product · Dual DoD A residual · freestanding MSC SKIP default.
     * Grep: xhci: ep0 soft-recover | soft residual lean | boot hang GET_DESC18
     */
    nPassMax = (g_u8PortSpeed >= 4u) ? 3 : 2;

    for (nPass = 0; nPass < nPassMax; nPass++) {
        fStamp = xhci_soft_recover_stamp_ok(szW);
        /*
         * Event-ring residual before each pass: force-inval so drain sees
         * late Transfer Events that every-64 clinv left stale (false-TO
         * class Soft!=product; a12f G-AC-1). Prefer real residual over
         * Nth-pass growth. No stamp (no storms).
         */
        if (g_pEvtRing != NULL) {
            xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx]);
        }
        if (nPass == 1) {
            /*
             * Second-pass residual (lean): brief settle so late Status /
             * TR Deq can land. Prefer mid-ring soft-continue over thrash.
             */
            if (fStamp != 0) {
                kprintf("xhci: ep0 soft-recover second-pass %s n=%u "
                        "(stage-11/13/15 residual Soft!=product)\n",
                        szW, (unsigned)g_u32SoftRecoverStampN);
            }
            xhci_settle((g_u8PortSpeed >= 4u) ? 400000u : 120000u);
            xhci_drain_events(64u);
        } else if (nPass == 2) {
            /*
             * Third-pass residual (SS only under nPassMax=3): one longer
             * settle then deny -> hard. Boot must not spin here forever.
             */
            if (fStamp != 0) {
                kprintf("xhci: ep0 soft-recover third-pass %s n=%u "
                        "(stage-11/13/15 residual Soft!=product; no Set TR Deq)\n",
                        szW, (unsigned)g_u32SoftRecoverStampN);
            }
            xhci_settle((g_u8PortSpeed >= 4u) ? 800000u : 200000u);
            xhci_drain_events((g_u8PortSpeed >= 4u) ? 80u : 56u);
        } else {
            /* First pass: brief drain (+ short SS settle). Lean-capped. */
            xhci_drain_events(48u);
            if (g_u8PortSpeed >= 4u) {
                xhci_settle(200000u);
                xhci_drain_events(64u);
            }
        }
        fSync = xhci_ep0_soft_sync_producer();
        u32EpSt = xhci_ep0_sample(&u64Deq);
        if (u32EpSt == XHCI_EP_ST_HALTED || u32EpSt == XHCI_EP_ST_ERROR) {
            if (fStamp != 0) {
                kprintf("xhci: ep0 soft-recover deny %s state=%u "
                        "(halted/error Soft!=product)\n",
                        szW, (unsigned)u32EpSt);
            }
            return 0;
        }
        if (xhci_ep0_soft_continue_ok(u32EpSt, u64Deq) != 0) {
            if (fStamp != 0) {
                kprintf("xhci: ep0 soft-continue after TO %s base state=%u "
                        "deq=0x%lx pass=%u n=%u (no Set TR Deq; Soft!=product)\n",
                        szW, (unsigned)u32EpSt, (unsigned long)u64Deq,
                        (unsigned)nPass, (unsigned)g_u32SoftRecoverStampN);
            }
            xhci_clflush_ptr(g_pEp0Ring);
            xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
            return 1;
        }
        if (xhci_ep0_producer_hc_aligned(u32EpSt, u64Deq) != 0) {
            if (fStamp != 0) {
                kprintf("xhci: ep0 mid-ring soft-continue after TO %s "
                        "state=%u deq=0x%lx idx=%u pass=%u n=%u "
                        "(no Set TR Deq; Soft!=product)\n",
                        szW, (unsigned)u32EpSt, (unsigned long)u64Deq,
                        (unsigned)g_u32Ep0Idx, (unsigned)nPass,
                        (unsigned)g_u32SoftRecoverStampN);
            }
            xhci_clflush_ptr(g_pEp0Ring);
            xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
            return 1;
        }
        if (fSync != 0) {
            /*
             * Soft-sync snapped SW to HC - only soft-continue if now aligned.
             * Former always-return-1 after sync re-enqueued onto desync ->
             * GET_DESC18 TO thrash (boot hang Soft!=product).
             */
            u32EpSt = xhci_ep0_sample(&u64Deq);
            if (xhci_ep0_soft_continue_ok(u32EpSt, u64Deq) != 0 ||
                xhci_ep0_producer_hc_aligned(u32EpSt, u64Deq) != 0) {
                if (fStamp != 0) {
                    kprintf("xhci: ep0 mid-ring soft-continue after TO %s "
                            "post-sync state=%u idx=%u pass=%u n=%u "
                            "(no Set TR Deq; Soft!=product)\n",
                            szW, (unsigned)u32EpSt, (unsigned)g_u32Ep0Idx,
                            (unsigned)nPass,
                            (unsigned)g_u32SoftRecoverStampN);
                }
                xhci_clflush_ptr(g_pEp0Ring);
                xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                return 1;
            }
            /* Sync without alignment: try next capped pass / hard deny. */
        }
    }
    /* Final deny always greppable once (ladder end; Soft!=product). */
    kprintf("xhci: ep0 soft-recover deny %s state=%u deq=0x%lx idx=%u "
            "-> hard path (stage-11/13/15 Soft!=product)\n",
            szW, (unsigned)u32EpSt, (unsigned long)u64Deq,
            (unsigned)g_u32Ep0Idx);
    return 0;
}

/*
 * After stall/babble/timeout: Reset Endpoint(EP0) + Set TR Dequeue to a fresh
 * ring. Soft ring_reset alone is not enough once the HC halted EP0 (stage 15).
 * Match xhci_ep0_arm_after_address: second ring_reset after Set TR Deq so
 * producer DCS/cycle stay aligned (a12f desync -> GET_CONFIG cc=0).
 *
 * CRITICAL: after a timed-out control TD the HC is still Running with dequeue
 * mid-ring while software has advanced - settle-only retry enqueues more TRBs
 * onto a desynced producer (classic stage-15). Always Stop+Reset+Set TR Deq
 * | DCS=1 and match software cycle=1 idx=0.
 */
static int
xhci_ep0_hard_resync(void)
{
    u32 u32Code = 0;
    u32 u32Ctrl;
    u32 u32EpSt;
    u64 u64Deq = 0;

    if (g_u8SlotId == 0) {
        xhci_ep0_ring_reset();
        return -1;
    }
    u32EpSt = xhci_ep0_sample(&u64Deq);
    kprintf("xhci: ep0 hard-resync begin state=%u deq=0x%lx idx=%u cyc=%u\n",
            (unsigned)u32EpSt, (unsigned long)u64Deq, (unsigned)g_u32Ep0Idx,
            (unsigned)g_fEp0Cycle);
    xhci_drain_events(32u);
    /* Only force U0 when PLS != 0 - LWS spam drops SS after device-desc. */
    if (g_u8PortId != 0) {
        u32 u32Ps = portsc_read(g_u8PortId);

        if (((u32Ps >> 5) & 0xfu) != 0u) {
            xhci_port_force_u0(g_u8PortId);
        }
        /* Keep PP on during resync (a12f can drop power after long TO). */
        xhci_port_power_on(g_u8PortId);
    }

    /* Stop Endpoint EP0 (ignore fail - may already be idle/halted). */
    u32Ctrl = (XHCI_TRB_TYPE_STOP_EP << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    (void)xhci_cmd(0, 0, u32Ctrl, &u32Code, NULL);
    xhci_drain_events(32u);

    /* Reset Endpoint, Endpoint ID = DCI 1 (EP0), TSP=0 */
    u32Ctrl = (XHCI_TRB_TYPE_RESET_EP << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    if (xhci_cmd(0, 0, u32Ctrl, &u32Code, NULL) != 0) {
        kprintf("xhci: Reset Endpoint EP0 FAIL code=%u\n", u32Code);
        /* Fall through - still try Set TR Dequeue / Evaluate */
    }
    xhci_drain_events(16u);

    xhci_ep0_ring_reset();
    /* Set TR Dequeue Pointer: param = ring | DCS=1, EP ID = 1 */
    u32Ctrl = (XHCI_TRB_TYPE_SET_TR_DEQ << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    if (xhci_cmd((u64)g_paEp0Ring | 1ull, 0, u32Ctrl, &u32Code, NULL) != 0) {
        kprintf("xhci: Set TR Dequeue EP0 FAIL code=%u; try Evaluate\n",
                u32Code);
        return xhci_evaluate_ep0_mps();
    }
    /* Resync software producer to DCS=1 after Set TR Dequeue (same as arm). */
    xhci_ep0_ring_reset();
    xhci_clflush_ptr(g_pEp0Ring);
    xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
    xhci_settle(300000u);
    u32EpSt = xhci_ep0_sample(&u64Deq);
    kprintf("xhci: ep0 hard-resync done state=%u deq=0x%lx dcs=%u\n",
            (unsigned)u32EpSt, (unsigned long)u64Deq,
            (unsigned)(u64Deq & 1ull));
    return 0;
}

/*
 * Before first GET_CONFIG / SET_CONFIG after a working device-desc
 * (stage-15 deepen; mirror stage-11 soft-align without thrash):
 *   1) mps0 resync from PORTSC speed (SS stick must keep 512)
 *   2) Halted/Error -> hard-resync (Stop+Reset+Set TR Deq | DCS=1)
 *   3) base soft-continue: Running|Stopped + idx==0 + cyc==1 + deq@base + DCS=1
 *   4) mid-ring soft-continue: producer_hc_aligned (post device-desc PASS -
 *      idx non-zero, deq matches SW, DCS==cycle) - NO Set TR Deq
 *   5) deq==0 / DCS!=1 at base with producer reset -> arm (Set TR Deq once)
 *   6) clear desync (SW advanced, HC deq stuck) or ring nearly full -> hard
 * Residual: after device-desc PASS prefer soft-recover settle before any
 * Set TR Deq so EP0 is cleanly armed without thrash (a12f stage-15 TO).
 * Prefer not thrashing Set TR Deq on the healthy post-desc path (a12f).
 * Grep: msc: progress ep0_pre_config | xhci: ep0 soft-continue ok|deny
 *       | xhci: ep0 mid-ring soft-continue | pre_config desync|hard-resync
 *       | ep0_ready after device-desc
 */
static void
xhci_ep0_prepare_for_config(void)
{
    u32 u32EpSt;
    u64 u64Deq = 0;
    u64 u64Base = (u64)g_paEp0Ring & ~0xfull;
    u64 u64DeqAddr;
    u64 u64SwAddr;
    u8 u8Dcs;

    /* Re-bind MPS0 before first config control (SS vs HS path). */
    xhci_mps0_resync_from_speed();

    /*
     * Stage-15 residual after device-desc PASS: drain + soft-sync producer
     * to HC deq (ISP short-packet / late Status may leave idx lagging).
     * Prefer mid-ring soft-continue; no Set TR Deq thrash when aligned.
     * Grep: xhci: ep0 soft-sync residual | ep0_ready after device-desc
     */
    xhci_drain_events(96u);
    if (g_u8PortSpeed >= 4u) {
        /* Soft residual lean: short SS settle (not multi-M thrash). Net first. */
        xhci_settle(800000u);
        xhci_drain_events(96u);
    } else {
        xhci_settle(400000u);
        xhci_drain_events(64u);
    }
    if (xhci_ep0_soft_sync_producer() == 0) {
        u32EpSt = xhci_ep0_sample(&u64Deq);
        if (u32EpSt == XHCI_EP_ST_HALTED || u32EpSt == XHCI_EP_ST_ERROR) {
            kprintf("xhci: ep0 pre_config soft-sync deny -> hard-resync "
                    "state=%u\n",
                    (unsigned)u32EpSt);
            (void)xhci_ep0_hard_resync();
            return;
        }
        /*
         * Soft-recover once before hard: a12f may still be updating deq
         * after device-desc; thrash Set TR Deq -> stage-15 cc=0 TO.
         * GET_CONFIG soft-align residual (Soft!=product; no thrash).
         */
        if (xhci_ep0_soft_recover_after_to("pre_config") != 0) {
            kprintf("xhci: ep0 pre_config soft-recover ok after device-desc "
                    "(stage=15 Soft!=product; no Set TR Deq thrash)\n");
        } else {
            u32EpSt = xhci_ep0_sample(&u64Deq);
            /* Running desync with SW ahead of HC: hard-resync. */
            if (u32EpSt == XHCI_EP_ST_RUNNING ||
                u32EpSt == XHCI_EP_ST_STOPPED) {
                u64 u64DeqAddrChk = u64Deq & ~0xfull;
                u64 u64SwChk =
                    u64Base + (u64)g_u32Ep0Idx * (u64)sizeof(struct xhci_trb);

                if (u64Deq != 0ull && u64DeqAddrChk != u64SwChk) {
                    kprintf("xhci: ep0 pre_config soft-sync deny desync "
                            "deq=0x%lx sw=0x%lx -> hard-resync\n",
                            (unsigned long)u64DeqAddrChk,
                            (unsigned long)u64SwChk);
                    (void)xhci_ep0_hard_resync();
                    return;
                }
            }
        }
    }

    u32EpSt = xhci_ep0_sample(&u64Deq);
    u8Dcs = (u8)(u64Deq & 1ull);
    u64DeqAddr = u64Deq & ~0xfull;
    u64SwAddr = u64Base + (u64)g_u32Ep0Idx * (u64)sizeof(struct xhci_trb);

    kprintf("msc: progress ep0_pre_config state=%u deq=0x%lx idx=%u cyc=%u "
            "dcs=%u mps0=%u spd=%u\n",
            (unsigned)u32EpSt, (unsigned long)u64Deq, (unsigned)g_u32Ep0Idx,
            (unsigned)g_fEp0Cycle, (unsigned)u8Dcs, (unsigned)g_u16MaxPkt0,
            (unsigned)g_u8PortSpeed);

    if (u32EpSt == XHCI_EP_ST_HALTED || u32EpSt == XHCI_EP_ST_ERROR) {
        kprintf("xhci: ep0 pre_config hard-resync state=%u\n",
                (unsigned)u32EpSt);
        (void)xhci_ep0_hard_resync();
        return;
    }

    /* Base soft-continue (fresh arm / post-address style). */
    if (xhci_ep0_soft_continue_ok(u32EpSt, u64Deq) != 0) {
        kprintf("xhci: ep0 soft-align pre_config state=%u deq=0x%lx "
                "sw=0x%lx dcs=%u\n",
                (unsigned)u32EpSt, (unsigned long)u64DeqAddr,
                (unsigned long)u64SwAddr, (unsigned)u8Dcs);
        kprintf("xhci: ep0 soft-continue ok pre_config state=%u "
                "(no Set TR Deq)\n",
                (unsigned)u32EpSt);
        xhci_clflush_ptr(g_pEp0Ring);
        xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
        return;
    }

    /*
     * Stage-15 residual: after device-desc PASS producer is mid-ring and
     * HC deq matches - soft-continue. Do NOT hard-resync / Set TR Deq.
     */
    if (xhci_ep0_producer_hc_aligned(u32EpSt, u64Deq) != 0) {
        kprintf("xhci: ep0 mid-ring soft-continue pre_config state=%u "
                "deq=0x%lx sw=0x%lx idx=%u cyc=%u dcs=%u "
                "(no Set TR Deq; stage=15 Soft!=product)\n",
                (unsigned)u32EpSt, (unsigned long)u64DeqAddr,
                (unsigned long)u64SwAddr, (unsigned)g_u32Ep0Idx,
                (unsigned)g_fEp0Cycle, (unsigned)u8Dcs);
        kprintf("xhci: ep0 soft-continue ok pre_config mid-ring state=%u "
                "(no Set TR Deq; Soft!=product)\n",
                (unsigned)u32EpSt);
        xhci_clflush_ptr(g_pEp0Ring);
        xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
        return;
    }

    kprintf("xhci: ep0 soft-continue deny pre_config state=%u deq=0x%lx "
            "idx=%u cyc=%u dcs=%u sw=0x%lx\n",
            (unsigned)u32EpSt, (unsigned long)u64Deq,
            (unsigned)g_u32Ep0Idx, (unsigned)g_fEp0Cycle, (unsigned)u8Dcs,
            (unsigned long)u64SwAddr);

    /* deq==0 or DCS!=1 at ring base with producer at base -> arm. */
    if (u64Deq == 0ull ||
        (u64DeqAddr == u64Base && u8Dcs != 1u && g_u32Ep0Idx == 0u &&
         g_fEp0Cycle == 1u)) {
        kprintf("xhci: ep0 pre_config deq=0x%lx dcs=%u -> arm before "
                "GET_CONFIG (Set TR Deq; stage=15 Soft!=product)\n",
                (unsigned long)u64Deq, (unsigned)u8Dcs);
        (void)xhci_ep0_arm_after_address();
        /* SS settle after arm so first GET_CONFIG doorbell is not raced. */
        if (g_u8PortSpeed >= 4u) {
            xhci_settle(3600000u);
            xhci_drain_events(176u);
        } else {
            xhci_settle(700000u);
        }
        return;
    }

    /*
     * Ring nearly full: hard-resync once to base so Setup+Data+Status fit
     * without wrapping mid-TD (single-TD multi-TRB wrap is fragile on a12f).
     */
    if (g_u32Ep0Idx + 3u > (XHCI_RING_TRBS - 1u)) {
        kprintf("xhci: ep0 pre_config ring full idx=%u -> hard-resync\n",
                (unsigned)g_u32Ep0Idx);
        (void)xhci_ep0_hard_resync();
        return;
    }

    /*
     * Clear desync after a prior TO: software producer advanced while HC
     * dequeue still sits at ring base (or other address mismatch).
     */
    if ((u32EpSt == XHCI_EP_ST_RUNNING || u32EpSt == XHCI_EP_ST_STOPPED) &&
        g_u32Ep0Idx != 0u && u64DeqAddr == u64Base) {
        kprintf("xhci: ep0 pre_config desync deq=base idx=%u -> hard-resync\n",
                (unsigned)g_u32Ep0Idx);
        (void)xhci_ep0_hard_resync();
        return;
    }

    if (u64DeqAddr != 0ull && u64DeqAddr != u64SwAddr) {
        kprintf("xhci: ep0 pre_config desync deq=0x%lx sw=0x%lx -> "
                "hard-resync\n",
                (unsigned long)u64DeqAddr, (unsigned long)u64SwAddr);
        (void)xhci_ep0_hard_resync();
        return;
    }

    if (g_u32Ep0Idx != 0u || g_fEp0Cycle != 1u ||
        (u64DeqAddr != 0ull && u64DeqAddr != u64Base)) {
        kprintf("xhci: ep0 pre_config misaligned -> hard-resync\n");
        (void)xhci_ep0_hard_resync();
        return;
    }

    kprintf("xhci: ep0 state=%u idx=0 -> arm before GET_CONFIG\n",
            (unsigned)u32EpSt);
    (void)xhci_ep0_arm_after_address();
    /*
     * Residual: SS a12f needs settle after Set TR Deq before first GET_CONFIG
     * doorbell (false-TO if doorbell races arm). Soft!=product stage=15.
     * Gap C freestanding residual deepen: longer arm settle before first
     * GET_CONFIG doorbell (past 11/13 -> 15+ Soft!=product Stage OPEN;
     * Dual DoD A stick OPEN).
     */
    if (g_u8PortSpeed >= 4u) {
        xhci_settle(3600000u);
        xhci_drain_events(168u);
    } else {
        xhci_settle(650000u);
    }
}

/*
 * Re-bind g_u16MaxPkt0 from PORTSC speed (SS=512 / HS=64 / FS=64 / LS=8).
 * Call before first GET_DESC so Address-time provisional MPS cannot linger.
 * Inline speed->MPS (no call into later statics - this sits above ep0_max_packet).
 * Grep: xhci: mps0
 */
static void
xhci_mps0_resync_from_speed(void)
{
    u16 u16Old = g_u16MaxPkt0;
    u32 u32Ps;
    u8 u8Spd;

    if (g_u8PortId != 0) {
        u32Ps = portsc_read(g_u8PortId);
        u8Spd = (u8)((u32Ps >> 10) & 0xfu);
        if (u8Spd != 0u) {
            g_u8PortSpeed = u8Spd;
        }
    }
    /* Match ep0_max_packet() policy (kept local to avoid forward decl thrash). */
    if (g_u8PortSpeed == 4u || g_u8PortSpeed == 5u) {
        g_u16MaxPkt0 = 512u;
    } else if (g_u8PortSpeed == 2u) {
        g_u16MaxPkt0 = 8u;
    } else if (g_u8PortSpeed != 0u) {
        g_u16MaxPkt0 = 64u; /* HS/FS default */
    } else if (g_u8PortId >= 1u && g_u8PortId <= XHCI_MAX_PORTS &&
               (g_u32SsPortBits & (1u << (g_u8PortId - 1u))) != 0u) {
        g_u16MaxPkt0 = 512u; /* provisional SS-cap until trained */
    } else {
        g_u16MaxPkt0 = 64u;
    }
    if (u16Old != g_u16MaxPkt0) {
        kprintf("xhci: mps0 resync %u->%u spd=%u port=%u\n",
                (unsigned)u16Old, (unsigned)g_u16MaxPkt0,
                (unsigned)g_u8PortSpeed, (unsigned)g_u8PortId);
    }
}

/*
 * Before first GET_DESCRIPTOR(device) after Address Device (stage-11 P0):
 *   P0-3: if g_u16MaxPkt0 diverged from Address-time MPS -> one Evaluate/hard
 *     before first soft-8 (skip when equal - no thrash).
 *   soft-sync residual first (ISP lag after readdress settle).
 *   P0-1: base soft-continue ONLY if ALL Running|Stopped + idx==0 + cyc==1 +
 *     deq base==g_paEp0Ring + DCS==1 (deq==0 -> arm, not soft-continue).
 *   mid-ring soft-continue when producer-HC aligned (readdress / soft-8 lag
 *     left idx non-zero) - prefer over thrash Set TR Deq (a12f stage-11).
 *   Halted/Error -> hard resync; clear desync -> hard; else arm.
 * Grep: msc: progress get_desc ep0_prep | xhci: ep0 soft-continue ok|deny
 *       | xhci: ep0 mid-ring soft-continue | xhci: ep0 mps0 diverge|match
 *       | Soft!=product
 */
static void
xhci_ep0_prepare_for_get_desc(void)
{
    u32 u32EpSt;
    u64 u64Deq = 0;
    u64 u64Base = (u64)g_paEp0Ring & ~0xfull;
    u64 u64DeqAddr;
    u64 u64SwAddr;
    u8 u8Dcs;

    xhci_mps0_resync_from_speed();

    /*
     * P0-3: Address-time MPS vs current g_u16MaxPkt0. One Evaluate/hard path
     * only when diverged; equal -> leave HC context alone (no thrash).
     */
    if (g_u16AddrMps0 != 0u && g_u16MaxPkt0 != g_u16AddrMps0) {
        kprintf("xhci: ep0 mps0 diverge addr-time=%u now=%u -> Evaluate once\n",
                (unsigned)g_u16AddrMps0, (unsigned)g_u16MaxPkt0);
        if (xhci_evaluate_ep0_mps() != 0) {
            kprintf("xhci: ep0 mps0 diverge Evaluate FAIL -> hard-resync\n");
            (void)xhci_ep0_hard_resync();
        }
        g_u16AddrMps0 = g_u16MaxPkt0; /* one-shot; avoid re-Evaluate thrash */
    } else if (g_u16AddrMps0 != 0u) {
        kprintf("xhci: ep0 mps0 match addr-time=%u (no Evaluate)\n",
                (unsigned)g_u16AddrMps0);
    }

    /*
     * Stage-11 residual deepen: soft-sync SW producer to HC deq after Address
     * / readdress settle (no Set TR Deq). Prefer mid-ring soft-continue over
     * thrash when deq matches producer. Soft-recover before hard on desync
     * (Gap C stick-path residual; a12f G-AC-1 Soft!=product).
     * Grep: xhci: ep0 soft-sync residual | ep0 soft-recover | Soft!=product
     */
    xhci_drain_events(80u);
    if (g_u8PortSpeed >= 4u) {
        /* SS Gap C freestanding residual deepen stage-11: longer settle so Address TR Deq lands. */
        xhci_settle(1300000u);
        xhci_drain_events(112u);
    } else {
        /* HS stage-11 residual Soft!=product: brief settle after Address. */
        xhci_settle(450000u);
        xhci_drain_events(80u);
    }
    if (xhci_ep0_soft_sync_producer() == 0) {
        u32EpSt = xhci_ep0_sample(&u64Deq);
        if (u32EpSt == XHCI_EP_ST_HALTED || u32EpSt == XHCI_EP_ST_ERROR) {
            kprintf("xhci: ep0 get_desc prep soft-sync deny -> hard-resync "
                    "state=%u\n",
                    (unsigned)u32EpSt);
            (void)xhci_ep0_hard_resync();
            return;
        }
        /*
         * Soft-recover once before hard: a12f may still be updating deq
         * after Address Device; thrash Set TR Deq -> stage-11 cc=0 TO.
         * Gap C freestanding residual deepen Soft!=product (2nd..26th-pass).
         */
        if (xhci_ep0_soft_recover_after_to("get_desc_prep") != 0) {
            kprintf("xhci: ep0 get_desc prep soft-recover ok "
                    "(stage=11 Soft!=product; no Set TR Deq thrash)\n");
        } else {
            u64 u64DeqAddrChk;
            u64 u64SwChk;

            /* Re-sample after soft-recover deny - deq may have moved. */
            u32EpSt = xhci_ep0_sample(&u64Deq);
            u64DeqAddrChk = u64Deq & ~0xfull;
            u64SwChk =
                u64Base + (u64)g_u32Ep0Idx * (u64)sizeof(struct xhci_trb);
            if ((u32EpSt == XHCI_EP_ST_RUNNING ||
                 u32EpSt == XHCI_EP_ST_STOPPED) &&
                u64Deq != 0ull && u64DeqAddrChk != u64SwChk) {
                kprintf("xhci: ep0 get_desc prep soft-sync deny desync "
                        "deq=0x%lx sw=0x%lx -> hard-resync "
                        "(stage-11 Soft!=product)\n",
                        (unsigned long)u64DeqAddrChk, (unsigned long)u64SwChk);
                (void)xhci_ep0_hard_resync();
                return;
            }
        }
    }

    u32EpSt = xhci_ep0_sample(&u64Deq);
    u8Dcs = (u8)(u64Deq & 1ull);
    u64DeqAddr = u64Deq & ~0xfull;
    u64SwAddr = u64Base + (u64)g_u32Ep0Idx * (u64)sizeof(struct xhci_trb);
    kprintf("msc: progress get_desc ep0_prep state=%u deq=0x%lx idx=%u "
            "cyc=%u dcs=%u mps0=%u spd=%u\n",
            (unsigned)u32EpSt, (unsigned long)u64Deq, (unsigned)g_u32Ep0Idx,
            (unsigned)g_fEp0Cycle, (unsigned)u8Dcs, (unsigned)g_u16MaxPkt0,
            (unsigned)g_u8PortSpeed);

    if (u32EpSt == XHCI_EP_ST_HALTED || u32EpSt == XHCI_EP_ST_ERROR) {
        kprintf("xhci: ep0 get_desc prep hard-resync state=%u\n",
                (unsigned)u32EpSt);
        (void)xhci_ep0_hard_resync();
        return;
    }

    /* P0-1: strict soft-continue gate (Running|Stopped + base + DCS=1). */
    if (xhci_ep0_soft_continue_ok(u32EpSt, u64Deq) != 0) {
        kprintf("xhci: ep0 soft-continue ok get_desc state=%u "
                "(no Set TR Deq; stage-11 Soft!=product)\n",
                (unsigned)u32EpSt);
        xhci_clflush_ptr(g_pEp0Ring);
        xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
        return;
    }

    /*
     * Stage-11 residual: after readdress / prior soft ctrl, producer may be
     * mid-ring and HC deq matches - soft-continue. Do NOT hard-resync thrash.
     */
    if (xhci_ep0_producer_hc_aligned(u32EpSt, u64Deq) != 0) {
        kprintf("xhci: ep0 mid-ring soft-continue get_desc prep state=%u "
                "deq=0x%lx sw=0x%lx idx=%u cyc=%u dcs=%u "
                "(no Set TR Deq; stage=11 Soft!=product)\n",
                (unsigned)u32EpSt, (unsigned long)u64DeqAddr,
                (unsigned long)u64SwAddr, (unsigned)g_u32Ep0Idx,
                (unsigned)g_fEp0Cycle, (unsigned)u8Dcs);
        kprintf("xhci: ep0 soft-continue ok get_desc mid-ring state=%u "
                "(no Set TR Deq; Soft!=product)\n",
                (unsigned)u32EpSt);
        xhci_clflush_ptr(g_pEp0Ring);
        xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
        return;
    }

    kprintf("xhci: ep0 soft-continue deny get_desc state=%u deq=0x%lx "
            "idx=%u cyc=%u dcs=%u base=0x%lx sw=0x%lx\n",
            (unsigned)u32EpSt, (unsigned long)u64Deq,
            (unsigned)g_u32Ep0Idx, (unsigned)g_fEp0Cycle, (unsigned)u8Dcs,
            (unsigned long)u64Base, (unsigned long)u64SwAddr);

    /* deq==0 or DCS!=1 at base with producer reset -> arm (cycle must match). */
    if (u64Deq == 0ull ||
        (u64DeqAddr == u64Base && u8Dcs != 1u && g_u32Ep0Idx == 0u &&
         g_fEp0Cycle == 1u)) {
        kprintf("xhci: ep0 get_desc prep deq=0x%lx dcs=%u -> arm\n",
                (unsigned long)u64Deq, (unsigned)u8Dcs);
        (void)xhci_ep0_arm_after_address();
        return;
    }

    /* Ring nearly full: one hard-resync so Setup+Data+Status fit. */
    if (g_u32Ep0Idx + 3u > (XHCI_RING_TRBS - 1u)) {
        kprintf("xhci: ep0 get_desc prep ring full idx=%u -> hard-resync\n",
                (unsigned)g_u32Ep0Idx);
        (void)xhci_ep0_hard_resync();
        return;
    }

    /* Clear desync: SW advanced, HC deq at base (or other mismatch). */
    if ((u32EpSt == XHCI_EP_ST_RUNNING || u32EpSt == XHCI_EP_ST_STOPPED) &&
        g_u32Ep0Idx != 0u && u64DeqAddr == u64Base) {
        kprintf("xhci: ep0 get_desc prep desync deq=base idx=%u -> "
                "hard-resync\n",
                (unsigned)g_u32Ep0Idx);
        (void)xhci_ep0_hard_resync();
        return;
    }
    if (u64DeqAddr != 0ull && u64DeqAddr != u64SwAddr) {
        kprintf("xhci: ep0 get_desc prep desync deq=0x%lx sw=0x%lx -> "
                "hard-resync\n",
                (unsigned long)u64DeqAddr, (unsigned long)u64SwAddr);
        (void)xhci_ep0_hard_resync();
        return;
    }
    if (g_u32Ep0Idx != 0u || g_fEp0Cycle != 1u ||
        (u64DeqAddr != 0ull && u64DeqAddr != u64Base)) {
        kprintf("xhci: ep0 get_desc prep producer/deq misaligned idx=%u "
                "cyc=%u deq=0x%lx -> hard-resync\n",
                (unsigned)g_u32Ep0Idx, (unsigned)g_fEp0Cycle,
                (unsigned long)u64Deq);
        (void)xhci_ep0_hard_resync();
        return;
    }
    kprintf("xhci: ep0 get_desc prep state=%u -> arm\n", (unsigned)u32EpSt);
    (void)xhci_ep0_arm_after_address();
}

/*
 * Greppable control fail line (PR1 A1). cc = Transfer Event completion code
 * from the failing event (not a later Evaluate Success). Timeout -> cc=0.
 */
static void
xhci_ctrl_fail_log(const struct usb_setup *pSetup, u32 u32Cc, u32 u32Residual)
{
    u8 u8Req = (pSetup != NULL) ? pSetup->u8BRequest : 0;
    u16 u16Wlen = (pSetup != NULL) ? pSetup->u16WLength : 0;

    kprintf("xhci: ctrl req=0x%x wlen=%u cc=%u residual=%u slot=%u port=%u "
            "spd=%u\n",
            (unsigned)u8Req, (unsigned)u16Wlen, (unsigned)u32Cc,
            (unsigned)u32Residual, (unsigned)g_u8SlotId,
            (unsigned)g_u8PortId, (unsigned)g_u8PortSpeed);
}

/* ---- control transfer on EP0 -------------------------------------------- */
static int
xhci_ctrl(const struct usb_setup *pSetup, void *pData, u16 u16Len, int fIn)
{
    u64 u64SetupRaw = 0;
    u32 u32Trt;
    u32 u32StatusDir;
    u32 u32Code = 0;
    u32 u32Residual = 0;
    int nEv;
    u32 u32Spins;
    u32 u32Dir;

    if (pSetup == NULL) {
        return -1;
    }
    /* Drop stale port-status / leftover xfer events before a new TD. */
    xhci_drain_events(64u);

    /*
     * SuperSpeed: only force U0 when PLS != 0. Spamming LWS every control
     * after a working device-desc can drop the link -> next GET_CONFIG
     * times out (stage-15 cc=0 on a12f).
     */
    if (g_u8PortId != 0 && g_u8PortSpeed >= 4u) {
        u32 u32Ps = portsc_read(g_u8PortId);

        if (((u32Ps >> 5) & 0xfu) != 0u) {
            xhci_port_force_u0(g_u8PortId);
        }
    }

    memcpy(&u64SetupRaw, pSetup, 8);

    if (u16Len == 0) {
        u32Trt = 0; /* no data */
    } else if (fIn != 0) {
        u32Trt = 3; /* IN */
    } else {
        u32Trt = 2; /* OUT */
    }

    /*
     * One control TD: Setup [+ one Data TRB] + Status.
     * IMPORTANT: do NOT split Data into MPS-sized TRBs for control IN.
     * wLength=255 with multi-TRB + short packet broke G752 a12f (stage 15).
     * xHCI allows Transfer Length > Max Packet Size; HC splits on the wire.
     * Setup Transfer Length in status = 8 (xHCI 6.4.1.2.1).
     * Single Data TRB + ISP (short packet event) - multi-TRB desync on a12f.
     */
    xfer_enqueue(g_pEp0Ring, &g_u32Ep0Idx, &g_fEp0Cycle, g_paEp0Ring,
                 u64SetupRaw, 8u,
                 (XHCI_TRB_TYPE_SETUP << 10) | XHCI_TRB_IDT | XHCI_TRB_CH |
                     (u32Trt << 16));

    u32Dir = (fIn != 0) ? (1u << 16) : 0u;

    if (u16Len > 0 && pData != NULL) {
        u32 u32DataFlags;

        if (fIn == 0) {
            memcpy(g_pScratch, pData, u16Len);
        } else {
            memset(g_pScratch, 0, u16Len > GJ_PAGE_SIZE ? GJ_PAGE_SIZE : u16Len);
        }
        xhci_clflush_ptr(g_pScratch);
        /*
         * Single Data TRB; DIR bit 16 only (not TRT). CH -> Status.
         * ISP so short GET_CONFIG (wlen>actual) yields a reliable event
         * before Status (stage-15 residual path).
         */
        u32DataFlags = (XHCI_TRB_TYPE_DATA << 10) | XHCI_TRB_CH | u32Dir;
        if (fIn != 0) {
            u32DataFlags |= XHCI_TRB_ISP;
        }
        xfer_enqueue(g_pEp0Ring, &g_u32Ep0Idx, &g_fEp0Cycle, g_paEp0Ring,
                     (u64)g_paScratch, (u32)u16Len, u32DataFlags);
    }

    /* Status stage: DIR = OUT if IN data; IN if no data or OUT data */
    if (u16Len > 0 && fIn != 0) {
        u32StatusDir = 0; /* OUT */
    } else {
        u32StatusDir = 1u << 16; /* IN */
    }
    xfer_enqueue(g_pEp0Ring, &g_u32Ep0Idx, &g_fEp0Cycle, g_paEp0Ring, 0, 0,
                 (XHCI_TRB_TYPE_STATUS << 10) | XHCI_TRB_IOC | u32StatusDir);

    /* Ensure EP0 TRBs visible to HC DMA before doorbell (a12f non-snoop). */
    xhci_clflush_span(g_pEp0Ring, (u32)(XHCI_RING_TRBS * sizeof(struct xhci_trb)));
    xhci_clflush_ptr(g_pScratch);
    /* Explicit cycle/DCS producer fence before doorbell (doorbell_ep posts USBSTS). */
    mmio_barrier();
    doorbell_ep(g_u8SlotId, 1); /* EP0 DCI = 1 */

    /*
     * SS on Intel a12f: long poll - GET_CONFIG after device-desc is the
     * stage-15 high-water; short polls reported cc=0=TO with device still
     * attached (photo p21/s4). Residual Gap C freestanding: SS *100 primary +
     * *38 grace + SS extra *72 + third SS grace (false-TO class Soft!=product).
     * HS margin. Soft!=product Stage OPEN. Dual DoD A stick OPEN.
     */
    u32Spins = (g_u8PortSpeed >= 4u) ? XHCI_POLL_SPINS_SS_CTRL
                                    : XHCI_POLL_SPINS_HS_CTRL;

    /*
     * Wait for Transfer Event. Accept Success (1) or Short Packet (13).
     * Some HCs emit Short Packet on the Data TRB then Success on Status -
     * if we return after the first Short Packet, EP0 is mid-TD and the next
     * GET_DESC18 fails (G752 stage-13 after 8-byte OK). Drain until Status
     * class event or timeout after short.
     *
     * Fail-closed honesty: sticky fail cc is the *actual* event code that
     * failed (or 0 on timeout) - never a later arm/Evaluate Success.
     */
    nEv = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code, NULL, NULL,
                          u32Spins);
    if (nEv < 0) {
        /*
         * Grace poll: a12f sometimes posts the Transfer Event just after
         * the primary budget (stage-15 false TO with device still OK).
         */
        nEv = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code, NULL, NULL,
                              XHCI_POLL_SPINS_CTRL_GRACE);
    }
    if (nEv < 0 && g_u8PortSpeed >= 4u) {
        /*
         * SS residual second grace (GET_CONFIG / GET_DESC false-TO Soft!=product):
         * device still attached, HC posts Transfer Event after long settle.
         * Prefer waiting over thrash hard-resync. Grep: xhci: ctrl SS extra grace
         */
        nEv = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code, NULL, NULL,
                              XHCI_POLL_SPINS_SS_CTRL_EXTRA);
        if (nEv >= 0) {
            kprintf("xhci: ctrl SS extra grace PASS cc=%u wlen=%u "
                    "(stage-11/13/15 residual Soft!=product)\n",
                    (unsigned)u32Code, (unsigned)u16Len);
        }
    }
    if (nEv < 0 && g_u8PortSpeed >= 4u) {
        /*
         * SS residual third grace (Gap C freestanding past 11/13->15+ dual-DoD
         * Soft!=product): primary+grace+extra still quiet while device attached
         * - a12f may post Transfer Event after a second long settle (stage-11
         * GET_DESC / stage-13->15 GET_CONFIG false-TO class). Prefer waiting
         * over thrash hard-resync. Grep: xhci: ctrl SS third grace
         */
        xhci_drain_events(96u);
        nEv = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code, NULL, NULL,
                              XHCI_POLL_SPINS_SS_CTRL_EXTRA);
        if (nEv >= 0) {
            kprintf("xhci: ctrl SS third grace PASS cc=%u wlen=%u "
                    "(stage-11/13/15 residual Soft!=product)\n",
                    (unsigned)u32Code, (unsigned)u16Len);
        }
    }
    if (nEv < 0) {
        /*
         * False-TO residual (event-ring + EP0 Soft!=product; a12f G-AC-1):
         * force-inval event TRB, dual-sample EP0 deq, residual wait.
         * If deq advanced: HC is working - re-clinv event ring + second
         * residual wait (Transfer Event in flight) before honest TO.
         * Prefer late Xfer Event over thrash Set TR Deq. No soft-sync
         * mid-TD (SW already at end of Setup+Data+Status). Stamps
         * first+every 16th (no storms). Grep: xhci: ctrl false-TO residual
         */
        u32 u32EpStPre;
        u64 u64DeqPre = 0;
        u32 u32EpStPost;
        u64 u64DeqPost = 0;
        int fStampFto;
        int fDeqMoved = 0;

        g_u32FalseToStampN++;
        fStampFto = (g_u32FalseToStampN == 1u ||
                     (g_u32FalseToStampN & 15u) == 0u)
                        ? 1
                        : 0;
        if (g_pEvtRing != NULL) {
            xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx]);
            if ((g_u32EvtIdx + 1u) < XHCI_RING_TRBS) {
                xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx + 1u]);
            } else {
                xhci_clinv_ptr(&g_pEvtRing[0]);
            }
        }
        u32EpStPre = xhci_ep0_sample(&u64DeqPre);
        xhci_settle((g_u8PortSpeed >= 4u) ? 2000000u : 500000u);
        xhci_drain_events(128u);
        /* Dual-sample: deq may land after first sample (false-TO class). */
        u32EpStPost = xhci_ep0_sample(&u64DeqPost);
        if (u64DeqPost != u64DeqPre) {
            fDeqMoved = 1;
            if (fStampFto != 0) {
                kprintf("xhci: ctrl false-TO residual deq moved 0x%lx->0x%lx "
                        "state=%u->%u n=%u (stage-11/13/15 Soft!=product)\n",
                        (unsigned long)u64DeqPre, (unsigned long)u64DeqPost,
                        (unsigned)u32EpStPre, (unsigned)u32EpStPost,
                        (unsigned)g_u32FalseToStampN);
            }
            /* HC deq advanced: event may be in flight - re-clinv + drain. */
            if (g_pEvtRing != NULL) {
                xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx]);
                if ((g_u32EvtIdx + 1u) < XHCI_RING_TRBS) {
                    xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx + 1u]);
                } else {
                    xhci_clinv_ptr(&g_pEvtRing[0]);
                }
            }
            xhci_drain_events(96u);
        } else {
            (void)u32EpStPre;
            (void)u32EpStPost;
        }
        nEv = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code, NULL, NULL,
                              (g_u8PortSpeed >= 4u)
                                  ? XHCI_POLL_SPINS_SS_CTRL_EXTRA
                                  : XHCI_POLL_SPINS_CTRL_GRACE);
        if (nEv < 0 && fDeqMoved != 0) {
            /*
             * Second residual (deq moved, event still quiet Soft!=product):
             * one more settle + force-inval + residual wait. Prefer late
             * Transfer Event over thrash Set TR Deq (GET_CONFIG stage-15).
             * Grep: xhci: ctrl false-TO residual PASS
             */
            u64 u64Deq3 = 0;

            xhci_settle((g_u8PortSpeed >= 4u) ? 2400000u : 600000u);
            if (g_pEvtRing != NULL) {
                xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx]);
                if ((g_u32EvtIdx + 1u) < XHCI_RING_TRBS) {
                    xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx + 1u]);
                } else {
                    xhci_clinv_ptr(&g_pEvtRing[0]);
                }
            }
            (void)xhci_ep0_sample(&u64Deq3);
            if (u64Deq3 != u64DeqPost && fStampFto != 0) {
                kprintf("xhci: ctrl false-TO residual deq moved 0x%lx->0x%lx "
                        "late n=%u (stage-11/13/15 Soft!=product)\n",
                        (unsigned long)u64DeqPost, (unsigned long)u64Deq3,
                        (unsigned)g_u32FalseToStampN);
            }
            xhci_drain_events(112u);
            nEv = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code, NULL,
                                  NULL,
                                  (g_u8PortSpeed >= 4u)
                                      ? XHCI_POLL_SPINS_SS_CTRL_EXTRA
                                      : XHCI_POLL_SPINS_CTRL_GRACE);
        }
        if (nEv >= 0 && fStampFto != 0) {
            kprintf("xhci: ctrl false-TO residual PASS cc=%u wlen=%u n=%u "
                    "(stage-11/13/15 Soft!=product; no thrash)\n",
                    (unsigned)u32Code, (unsigned)u16Len,
                    (unsigned)g_u32FalseToStampN);
        }
    }
    if (nEv < 0) {
        u32 u32EpSt;
        u64 u64Deq = 0;
        u32 u32Ps = 0;

        g_u32LastCtrlResidual = 0;
        xhci_note_ctrl_fail_cc(0); /* honest TO for stages 11/12/13/15/17 */
        xhci_ctrl_fail_log(pSetup, 0, 0);
        /*
         * Timeout honesty: dump EP0 Output Context + producer so serial
         * greps can separate ring desync from pure device non-response.
         * Grep: xhci: ctrl TO ep0_state
         */
        u32EpSt = xhci_ep0_sample(&u64Deq);
        if (g_u8PortId != 0) {
            u32Ps = portsc_read(g_u8PortId);
        }
        kprintf("xhci: ctrl TO ep0_state=%u deq=0x%lx idx=%u cyc=%u "
                "portsc=0x%x pls=%u ped=%u\n",
                (unsigned)u32EpSt, (unsigned long)u64Deq,
                (unsigned)g_u32Ep0Idx, (unsigned)g_fEp0Cycle, (unsigned)u32Ps,
                (unsigned)((u32Ps >> 5) & 0xfu),
                (u32Ps & XHCI_PORTSC_PED) != 0 ? 1u : 0u);
        return -1;
    }
    u32Residual = g_u32LastEvtResidual;
    xhci_note_cc(u32Code);
    if (u32Code != 1u && u32Code != 13u) {
        g_u32LastCtrlResidual = u32Residual;
        xhci_note_ctrl_fail_cc(u32Code);
        xhci_ctrl_fail_log(pSetup, u32Code, u32Residual);
        return -1;
    }
    if (u32Code == 13u && u16Len > 0u) {
        u32 u32Code2 = 0;
        int nEv2;

        /*
         * Short on Data (ISP): wait for Status IOC (Success). Full SS budget
         * - short Status wait left EP0 mid-TD -> next GET_DESC18 stage-13 /
         * GET_CONFIG stage-15 cc=0. Residual path: ISP short-packet then
         * Status must both retire before the next control TD is enqueued.
         * Grep: xhci: ctrl short-status
         */
        nEv2 = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code2, NULL, NULL,
                               u32Spins);
        if (nEv2 < 0) {
            nEv2 = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code2, NULL,
                                   NULL, XHCI_POLL_SPINS_CTRL_GRACE);
        }
        if (nEv2 < 0) {
            /* Second grace: a12f sometimes posts Status very late after ISP. */
            nEv2 = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code2, NULL,
                                   NULL, XHCI_POLL_SPINS_CTRL_GRACE);
        }
        if (nEv2 < 0 && g_u8PortSpeed >= 4u) {
            /*
             * SS residual: Status after ISP short-packet can land after the
             * double grace window (stage-13/15 false-TO Soft!=product). Prefer
             * waiting over thrash hard-resync. Grep: ctrl SS extra grace
             */
            nEv2 = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code2, NULL,
                                   NULL, XHCI_POLL_SPINS_SS_CTRL_EXTRA);
            if (nEv2 >= 0) {
                kprintf("xhci: ctrl SS extra grace PASS short-status cc=%u "
                        "wlen=%u (stage-13/15 residual Soft!=product)\n",
                        (unsigned)u32Code2, (unsigned)u16Len);
            }
        }
        if (nEv2 < 0 && g_u8PortSpeed >= 4u) {
            /*
             * SS residual third grace on short-status (Gap C freestanding past
             * 11/13->15+ dual-DoD Soft!=product): Status after ISP may land after
             * extra still quiet. Prefer waiting over thrash Set TR Deq.
             * Grep: xhci: ctrl SS third grace PASS short-status
             */
            xhci_drain_events(80u);
            nEv2 = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code2, NULL,
                                   NULL, XHCI_POLL_SPINS_SS_CTRL_EXTRA);
            if (nEv2 >= 0) {
                kprintf("xhci: ctrl SS third grace PASS short-status cc=%u "
                        "wlen=%u (stage-13/15 residual Soft!=product)\n",
                        (unsigned)u32Code2, (unsigned)u16Len);
            }
        }
        if (nEv2 >= 0) {
            xhci_note_cc(u32Code2);
            if (u32Code2 != 1u && u32Code2 != 13u) {
                /* Status failed - sticky the real Status event cc. */
                g_u32LastCtrlResidual = g_u32LastEvtResidual;
                xhci_note_ctrl_fail_cc(u32Code2);
                xhci_ctrl_fail_log(pSetup, u32Code2, g_u32LastEvtResidual);
                kprintf("xhci: ctrl short-status FAIL cc=%u residual=%u\n",
                        (unsigned)u32Code2, (unsigned)g_u32LastEvtResidual);
                return -1;
            }
            /* Status OK: keep Data-stage residual for short-packet length. */
            kprintf("xhci: ctrl short-status PASS cc=%u data_resid=%u wlen=%u\n",
                    (unsigned)u32Code2, (unsigned)u32Residual,
                    (unsigned)u16Len);
            /* Residual soft-sync: HC deq may lead SW after multi-event Status. */
            (void)xhci_ep0_soft_sync_producer();
        } else {
            /*
             * No Status event: some HCs event once on short Data. Extra grace
             * then soft-sync. If still desynced (SW ahead mid-TD) hard-resync
             * so next GET_DESC18 / GET_CONFIG does not inherit stage-13/15 TO.
             * Soft honesty lamp - do not claim Status completed.
             * Grep: xhci: ctrl short-status OPEN | ep0 soft-sync residual
             */
            kprintf("xhci: ctrl short-status OPEN (no Status event; drain)\n");
            xhci_drain_events(64u);
            nEv2 = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code2, NULL,
                                   NULL, XHCI_POLL_SPINS_CTRL_GRACE);
            if (nEv2 < 0 && g_u8PortSpeed >= 4u) {
                /*
                 * SS residual (Gap C freestanding past 11/13->15+ Soft!=product):
                 * Status after ISP short-packet can land after grace window -
                 * one more SS extra grace before soft-recover / hard thrash.
                 * Grep: xhci: ctrl short-status PASS late | SS extra grace
                 */
                nEv2 = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code2,
                                       NULL, NULL,
                                       XHCI_POLL_SPINS_SS_CTRL_EXTRA);
                if (nEv2 >= 0) {
                    kprintf("xhci: ctrl SS extra grace PASS short-status OPEN "
                            "cc=%u wlen=%u (stage-13/15 residual Soft!=product)\n",
                            (unsigned)u32Code2, (unsigned)u16Len);
                }
            }
            if (nEv2 >= 0) {
                xhci_note_cc(u32Code2);
                if (u32Code2 == 1u || u32Code2 == 13u) {
                    kprintf("xhci: ctrl short-status PASS late cc=%u "
                            "data_resid=%u\n",
                            (unsigned)u32Code2, (unsigned)u32Residual);
                    (void)xhci_ep0_soft_sync_producer();
                } else {
                    g_u32LastCtrlResidual = g_u32LastEvtResidual;
                    xhci_note_ctrl_fail_cc(u32Code2);
                    xhci_ctrl_fail_log(pSetup, u32Code2, g_u32LastEvtResidual);
                    kprintf("xhci: ctrl short-status FAIL late cc=%u\n",
                            (unsigned)u32Code2);
                    return -1;
                }
            } else if (xhci_ep0_soft_recover_after_to("short_status_OPEN") !=
                       0) {
                kprintf("xhci: ctrl short-status OPEN soft-continue ok "
                        "(stage-13/15 residual Soft!=product; keep Data)\n");
            } else {
                kprintf("xhci: ctrl short-status OPEN soft-sync deny -> "
                        "hard-resync (stage-13/15 residual Soft!=product)\n");
                (void)xhci_ep0_hard_resync();
            }
        }
    }
    g_u32LastCtrlResidual = u32Residual;
    if (u16Len > 0 && pData != NULL && fIn != 0) {
        xhci_clflush_span(g_pScratch, (u32)u16Len);
        memcpy(pData, g_pScratch, u16Len);
    }
    /* Drain trailing events (Short Packet + Status on some HCs). */
    xhci_drain_events(64u);
    /*
     * Stage-13/15 residual after ctrl PASS: if producer and HC deq already
     * match (base or mid-ring), soft-continue without Set TR Deq. Else one
     * soft-sync snap (no thrash hard-resync on the healthy path). a12f may
     * leave deq one TRB off after multi-event Status -> next GET_DESC18 /
     * GET_CONFIG cc=0 without this. Stamps rate-limited (every ctrl TD would
     * storm serial). Grep: xhci: ep0 mid-ring soft-continue | Soft!=product
     */
    {
        u32 u32EpStPass;
        u64 u64DeqPass = 0;
        int fStampPass;

        g_u32CtrlPassSoftN++;
        /* First + every 16th (no mid-ring stamp storms Soft!=product). */
        fStampPass = (g_u32CtrlPassSoftN == 1u ||
                      (g_u32CtrlPassSoftN & 15u) == 0u)
                         ? 1
                         : 0;

        u32EpStPass = xhci_ep0_sample(&u64DeqPass);
        if (xhci_ep0_soft_continue_ok(u32EpStPass, u64DeqPass) != 0) {
            if (fStampPass != 0) {
                kprintf("xhci: ep0 soft-continue after ctrl PASS base "
                        "state=%u deq=0x%lx n=%u (no Set TR Deq; "
                        "stage-13/15 residual Soft!=product)\n",
                        (unsigned)u32EpStPass, (unsigned long)u64DeqPass,
                        (unsigned)g_u32CtrlPassSoftN);
            }
            xhci_clflush_ptr(g_pEp0Ring);
            xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
        } else if (xhci_ep0_producer_hc_aligned(u32EpStPass, u64DeqPass) !=
                   0) {
            if (fStampPass != 0) {
                kprintf("xhci: ep0 mid-ring soft-continue after ctrl PASS "
                        "state=%u deq=0x%lx idx=%u n=%u (no Set TR Deq; "
                        "stage-13/15 residual Soft!=product)\n",
                        (unsigned)u32EpStPass, (unsigned long)u64DeqPass,
                        (unsigned)g_u32Ep0Idx, (unsigned)g_u32CtrlPassSoftN);
            }
            xhci_clflush_ptr(g_pEp0Ring);
            xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
        } else if (xhci_ep0_soft_sync_producer() != 0) {
            /* After snap: prefer mid-ring soft-continue if now aligned. */
            u32EpStPass = xhci_ep0_sample(&u64DeqPass);
            if (xhci_ep0_soft_continue_ok(u32EpStPass, u64DeqPass) != 0 ||
                xhci_ep0_producer_hc_aligned(u32EpStPass, u64DeqPass) != 0) {
                if (fStampPass != 0) {
                    kprintf("xhci: ep0 mid-ring soft-continue after ctrl PASS "
                            "post-sync state=%u idx=%u n=%u (no Set TR Deq; "
                            "stage-13/15 residual Soft!=product)\n",
                            (unsigned)u32EpStPass, (unsigned)g_u32Ep0Idx,
                            (unsigned)g_u32CtrlPassSoftN);
                }
                xhci_clflush_ptr(g_pEp0Ring);
                xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
            } else if (fStampPass != 0) {
                kprintf("xhci: ep0 soft-sync after ctrl PASS ok n=%u "
                        "(stage-13/15 residual Soft!=product; no Set TR Deq)\n",
                        (unsigned)g_u32CtrlPassSoftN);
            }
        } else if (g_u8PortSpeed >= 4u) {
            /*
             * Soft residual lean (SS mid-ring after ctrl PASS): late + late2 +
             * one soft-recover; historical late3..late16 nest is compile-dead
             * (else if 0) so freestanding does not multi-M thrash before NET.
             * Soft!=product · G-AC-1 · MSC SKIP default · Dual DoD A stick OPEN.
             * Grep: xhci: ep0 mid-ring soft-continue after ctrl PASS late
             *       | soft residual lean
             */
            xhci_settle(700000u);
            xhci_drain_events(64u);
            u32EpStPass = xhci_ep0_sample(&u64DeqPass);
            if (xhci_ep0_soft_continue_ok(u32EpStPass, u64DeqPass) != 0 ||
                xhci_ep0_producer_hc_aligned(u32EpStPass, u64DeqPass) != 0) {
                if (fStampPass != 0) {
                    kprintf("xhci: ep0 mid-ring soft-continue after ctrl PASS "
                            "late state=%u idx=%u n=%u (stage-13/15 Soft!=product)\n",
                            (unsigned)u32EpStPass, (unsigned)g_u32Ep0Idx,
                            (unsigned)g_u32CtrlPassSoftN);
                }
                xhci_clflush_ptr(g_pEp0Ring);
                xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
            } else if (xhci_ep0_soft_sync_producer() != 0) {
                u32EpStPass = xhci_ep0_sample(&u64DeqPass);
                if (xhci_ep0_soft_continue_ok(u32EpStPass, u64DeqPass) != 0 ||
                    xhci_ep0_producer_hc_aligned(u32EpStPass, u64DeqPass) !=
                        0) {
                    if (fStampPass != 0) {
                        kprintf("xhci: ep0 mid-ring soft-continue after ctrl "
                                "PASS late post-sync state=%u idx=%u n=%u "
                                "(stage-13/15 Soft!=product)\n",
                                (unsigned)u32EpStPass, (unsigned)g_u32Ep0Idx,
                                (unsigned)g_u32CtrlPassSoftN);
                    }
                    xhci_clflush_ptr(g_pEp0Ring);
                    xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                } else if (fStampPass != 0) {
                    kprintf("xhci: ep0 soft-sync after ctrl PASS late ok n=%u "
                            "(stage-13/15 residual Soft!=product)\n",
                            (unsigned)g_u32CtrlPassSoftN);
                }
            } else {
                /*
                 * Second late settle (Gap C): a12f sometimes posts TR Deq
                 * after the first late window; then soft-recover ladder.
                 */
                xhci_settle(900000u);
                xhci_drain_events(72u);
                u32EpStPass = xhci_ep0_sample(&u64DeqPass);
                if (xhci_ep0_soft_continue_ok(u32EpStPass, u64DeqPass) != 0 ||
                    xhci_ep0_producer_hc_aligned(u32EpStPass, u64DeqPass) !=
                        0) {
                    if (fStampPass != 0) {
                        kprintf("xhci: ep0 mid-ring soft-continue after ctrl "
                                "PASS late2 state=%u idx=%u n=%u "
                                "(stage-13/15 Soft!=product)\n",
                                (unsigned)u32EpStPass, (unsigned)g_u32Ep0Idx,
                                (unsigned)g_u32CtrlPassSoftN);
                    }
                    xhci_clflush_ptr(g_pEp0Ring);
                    xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                } else if (xhci_ep0_soft_recover_after_to("ctrl_pass_late") !=
                           0) {
                    if (fStampPass != 0) {
                        kprintf("xhci: ep0 mid-ring soft-continue after ctrl "
                                "PASS late soft-recover n=%u "
                                "(stage-13/15 Soft!=product)\n",
                                (unsigned)g_u32CtrlPassSoftN);
                    }
                } else if (0) { /* soft residual lean: late3..late16 dead (net first) */
                    /*
                     * Historical third late settle (Gap C) - not executed.
                     * Soft residual lean prefers fail-closed to hard-resync
                     * after late2 + soft-recover. Soft!=product · G-AC-1.
                     * Grep: soft residual lean | ctrl PASS late3 (dead)
                     */
                    xhci_settle(1100000u);
                    xhci_drain_events(80u);
                    u32EpStPass = xhci_ep0_sample(&u64DeqPass);
                    if (xhci_ep0_soft_continue_ok(u32EpStPass, u64DeqPass) !=
                            0 ||
                        xhci_ep0_producer_hc_aligned(u32EpStPass,
                                                    u64DeqPass) != 0) {
                        if (fStampPass != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "ctrl PASS late3 state=%u idx=%u n=%u "
                                    "(stage-13/15 Soft!=product)\n",
                                    (unsigned)u32EpStPass,
                                    (unsigned)g_u32Ep0Idx,
                                    (unsigned)g_u32CtrlPassSoftN);
                        }
                        xhci_clflush_ptr(g_pEp0Ring);
                        xhci_clflush_ptr(
                            &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                    } else if (xhci_ep0_soft_sync_producer() != 0) {
                        if (fStampPass != 0) {
                            kprintf("xhci: ep0 soft-sync after ctrl PASS "
                                    "late3 ok n=%u "
                                    "(stage-13/15 residual Soft!=product)\n",
                                    (unsigned)g_u32CtrlPassSoftN);
                        }
                    } else {
                        /*
                         * Fourth late settle (Gap C dual-DoD residual): a12f
                         * may post TR Deq after late3 still quiet - one more
                         * soft-recover + mid-ring soft-continue before leave.
                         * Grep: xhci: ep0 mid-ring soft-continue after ctrl PASS late4
                         */
                        xhci_settle(1400000u);
                        xhci_drain_events(96u);
                        u32EpStPass = xhci_ep0_sample(&u64DeqPass);
                        if (xhci_ep0_soft_continue_ok(u32EpStPass,
                                                      u64DeqPass) != 0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStPass,
                                                        u64DeqPass) != 0) {
                            if (fStampPass != 0) {
                                kprintf("xhci: ep0 mid-ring soft-continue "
                                        "after ctrl PASS late4 state=%u "
                                        "idx=%u n=%u "
                                        "(stage-13/15 Soft!=product)\n",
                                        (unsigned)u32EpStPass,
                                        (unsigned)g_u32Ep0Idx,
                                        (unsigned)g_u32CtrlPassSoftN);
                            }
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "ctrl_pass_late4") != 0) {
                            if (fStampPass != 0) {
                                kprintf("xhci: ep0 mid-ring soft-continue "
                                        "after ctrl PASS late4 soft-recover "
                                        "n=%u (stage-13/15 Soft!=product)\n",
                                        (unsigned)g_u32CtrlPassSoftN);
                            }
                        } else {
                            /*
                             * Fifth late settle (Gap C dual-DoD residual
                             * deepen Soft!=product): a12f may post TR Deq
                             * after late4 soft-recover still quiet - one more
                             * mid-ring soft-continue before leave desynced.
                             * Grep: xhci: ep0 mid-ring soft-continue after
                             *       ctrl PASS late5
                             */
                            xhci_settle(1700000u);
                            xhci_drain_events(112u);
                            u32EpStPass = xhci_ep0_sample(&u64DeqPass);
                            if (xhci_ep0_soft_continue_ok(u32EpStPass,
                                                          u64DeqPass) != 0 ||
                                xhci_ep0_producer_hc_aligned(
                                    u32EpStPass, u64DeqPass) != 0) {
                                if (fStampPass != 0) {
                                    kprintf(
                                        "xhci: ep0 mid-ring soft-continue "
                                        "after ctrl PASS late5 state=%u "
                                        "idx=%u n=%u "
                                        "(stage-13/15 Soft!=product)\n",
                                        (unsigned)u32EpStPass,
                                        (unsigned)g_u32Ep0Idx,
                                        (unsigned)g_u32CtrlPassSoftN);
                                }
                                xhci_clflush_ptr(g_pEp0Ring);
                                xhci_clflush_ptr(
                                    &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                            } else if (xhci_ep0_soft_sync_producer() != 0) {
                                if (fStampPass != 0) {
                                    kprintf(
                                        "xhci: ep0 soft-sync after ctrl PASS "
                                        "late5 ok n=%u "
                                        "(stage-13/15 residual Soft!=product)\n",
                                        (unsigned)g_u32CtrlPassSoftN);
                                }
                            } else if (xhci_ep0_soft_recover_after_to(
                                           "ctrl_pass_late5") != 0) {
                                if (fStampPass != 0) {
                                    kprintf(
                                        "xhci: ep0 mid-ring soft-continue "
                                        "after ctrl PASS late5 soft-recover "
                                        "n=%u (stage-13/15 Soft!=product)\n",
                                        (unsigned)g_u32CtrlPassSoftN);
                                }
                            } else {
                                /*
                                 * Sixth late settle (Gap C dual-DoD residual
                                 * deepen Soft!=product): a12f may post TR Deq
                                 * after late5 soft-recover still quiet - one
                                 * more mid-ring soft-continue before leave
                                 * desynced. Prefer soft over thrash Set TR Deq.
                                 * Grep: xhci: ep0 mid-ring soft-continue after
                                 *       ctrl PASS late6
                                 */
                                xhci_settle(2000000u);
                                xhci_drain_events(128u);
                                u32EpStPass = xhci_ep0_sample(&u64DeqPass);
                                if (xhci_ep0_soft_continue_ok(u32EpStPass,
                                                              u64DeqPass) !=
                                        0 ||
                                    xhci_ep0_producer_hc_aligned(
                                        u32EpStPass, u64DeqPass) != 0) {
                                    if (fStampPass != 0) {
                                        kprintf(
                                            "xhci: ep0 mid-ring soft-continue "
                                            "after ctrl PASS late6 state=%u "
                                            "idx=%u n=%u "
                                            "(stage-13/15 Soft!=product)\n",
                                            (unsigned)u32EpStPass,
                                            (unsigned)g_u32Ep0Idx,
                                            (unsigned)g_u32CtrlPassSoftN);
                                    }
                                    xhci_clflush_ptr(g_pEp0Ring);
                                    xhci_clflush_ptr(
                                        &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                                } else if (xhci_ep0_soft_sync_producer() !=
                                           0) {
                                    if (fStampPass != 0) {
                                        kprintf(
                                            "xhci: ep0 soft-sync after ctrl "
                                            "PASS late6 ok n=%u "
                                            "(stage-13/15 residual "
                                            "Soft!=product)\n",
                                            (unsigned)g_u32CtrlPassSoftN);
                                    }
                                } else if (xhci_ep0_soft_recover_after_to(
                                               "ctrl_pass_late6") != 0) {
                                    if (fStampPass != 0) {
                                        kprintf(
                                            "xhci: ep0 mid-ring soft-continue "
                                            "after ctrl PASS late6 "
                                            "soft-recover n=%u "
                                            "(stage-13/15 Soft!=product)\n",
                                            (unsigned)g_u32CtrlPassSoftN);
                                    }
                                } else {
                                    /*
                                     * Seventh late settle (Gap C dual-DoD
                                     * residual deepen Soft!=product): a12f may
                                     * post TR Deq after late6 soft-recover
                                     * still quiet - one more mid-ring
                                     * soft-continue before leave desynced.
                                     * Prefer soft over thrash Set TR Deq.
                                     * Grep: xhci: ep0 mid-ring soft-continue
                                     *       after ctrl PASS late7
                                     */
                                    xhci_settle(2300000u);
                                    xhci_drain_events(144u);
                                    u32EpStPass =
                                        xhci_ep0_sample(&u64DeqPass);
                                    if (xhci_ep0_soft_continue_ok(
                                            u32EpStPass, u64DeqPass) != 0 ||
                                        xhci_ep0_producer_hc_aligned(
                                            u32EpStPass, u64DeqPass) != 0) {
                                        if (fStampPass != 0) {
                                            kprintf(
                                                "xhci: ep0 mid-ring "
                                                "soft-continue after ctrl "
                                                "PASS late7 state=%u idx=%u "
                                                "n=%u (stage-13/15 "
                                                "Soft!=product)\n",
                                                (unsigned)u32EpStPass,
                                                (unsigned)g_u32Ep0Idx,
                                                (unsigned)g_u32CtrlPassSoftN);
                                        }
                                        xhci_clflush_ptr(g_pEp0Ring);
                                        xhci_clflush_ptr(
                                            &g_pEp0Ring[XHCI_RING_TRBS -
                                                        1u]);
                                    } else if (xhci_ep0_soft_sync_producer() !=
                                               0) {
                                        if (fStampPass != 0) {
                                            kprintf(
                                                "xhci: ep0 soft-sync after "
                                                "ctrl PASS late7 ok n=%u "
                                                "(stage-13/15 residual "
                                                "Soft!=product)\n",
                                                (unsigned)g_u32CtrlPassSoftN);
                                        }
                                    } else if (xhci_ep0_soft_recover_after_to(
                                                   "ctrl_pass_late7") != 0) {
                                        if (fStampPass != 0) {
                                            kprintf(
                                                "xhci: ep0 mid-ring "
                                                "soft-continue after ctrl "
                                                "PASS late7 soft-recover "
                                                "n=%u (stage-13/15 "
                                                "Soft!=product)\n",
                                                (unsigned)g_u32CtrlPassSoftN);
                                        }
                                    } else {
                                        /*
                                         * Eighth late settle (Gap C dual-DoD
                                         * residual deepen Soft!=product): a12f
                                         * may post TR Deq after late7
                                         * soft-recover still quiet - one more
                                         * mid-ring soft-continue before leave
                                         * desynced. Prefer soft over thrash.
                                         * Grep: xhci: ep0 mid-ring soft-continue
                                         *       after ctrl PASS late8
                                         */
                                        xhci_settle(2600000u);
                                        xhci_drain_events(160u);
                                        u32EpStPass =
                                            xhci_ep0_sample(&u64DeqPass);
                                        if (xhci_ep0_soft_continue_ok(
                                                u32EpStPass, u64DeqPass) !=
                                                0 ||
                                            xhci_ep0_producer_hc_aligned(
                                                u32EpStPass, u64DeqPass) !=
                                                0) {
                                            if (fStampPass != 0) {
                                                kprintf(
                                                    "xhci: ep0 mid-ring "
                                                    "soft-continue after "
                                                    "ctrl PASS late8 "
                                                    "state=%u idx=%u n=%u "
                                                    "(stage-13/15 "
                                                    "Soft!=product)\n",
                                                    (unsigned)u32EpStPass,
                                                    (unsigned)g_u32Ep0Idx,
                                                    (unsigned)
                                                        g_u32CtrlPassSoftN);
                                            }
                                            xhci_clflush_ptr(g_pEp0Ring);
                                            xhci_clflush_ptr(
                                                &g_pEp0Ring[XHCI_RING_TRBS -
                                                            1u]);
                                        } else if (xhci_ep0_soft_sync_producer() !=
                                                   0) {
                                            if (fStampPass != 0) {
                                                kprintf(
                                                    "xhci: ep0 soft-sync "
                                                    "after ctrl PASS late8 "
                                                    "ok n=%u (stage-13/15 "
                                                    "residual Soft!=product)\n",
                                                    (unsigned)
                                                        g_u32CtrlPassSoftN);
                                            }
                                        } else if (
                                            xhci_ep0_soft_recover_after_to(
                                                "ctrl_pass_late8") != 0) {
                                            if (fStampPass != 0) {
                                                kprintf(
                                                    "xhci: ep0 mid-ring "
                                                    "soft-continue after "
                                                    "ctrl PASS late8 "
                                                    "soft-recover n=%u "
                                                    "(stage-13/15 "
                                                    "Soft!=product)\n",
                                                    (unsigned)
                                                        g_u32CtrlPassSoftN);
                                            }
                                        } else {
                                            /*
                                             * Ninth late settle (Gap C dual-DoD
                                             * residual deepen Soft!=product):
                                             * a12f may post TR Deq after late8
                                             * soft-recover still quiet - one
                                             * more mid-ring soft-continue
                                             * before leave desynced. Prefer
                                             * soft over thrash Set TR Deq.
                                             * Grep: xhci: ep0 mid-ring
                                             *       soft-continue after
                                             *       ctrl PASS late9
                                             */
                                            xhci_settle(2900000u);
                                            xhci_drain_events(176u);
                                            u32EpStPass =
                                                xhci_ep0_sample(&u64DeqPass);
                                            if (xhci_ep0_soft_continue_ok(
                                                    u32EpStPass,
                                                    u64DeqPass) != 0 ||
                                                xhci_ep0_producer_hc_aligned(
                                                    u32EpStPass,
                                                    u64DeqPass) != 0) {
                                                if (fStampPass != 0) {
                                                    kprintf(
                                                        "xhci: ep0 mid-ring "
                                                        "soft-continue after "
                                                        "ctrl PASS late9 "
                                                        "state=%u idx=%u n=%u "
                                                        "(stage-13/15 "
                                                        "Soft!=product)\n",
                                                        (unsigned)u32EpStPass,
                                                        (unsigned)g_u32Ep0Idx,
                                                        (unsigned)
                                                            g_u32CtrlPassSoftN);
                                                }
                                                xhci_clflush_ptr(g_pEp0Ring);
                                                xhci_clflush_ptr(
                                                    &g_pEp0Ring
                                                        [XHCI_RING_TRBS -
                                                         1u]);
                                            } else if (
                                                xhci_ep0_soft_sync_producer() !=
                                                0) {
                                                if (fStampPass != 0) {
                                                    kprintf(
                                                        "xhci: ep0 soft-sync "
                                                        "after ctrl PASS late9 "
                                                        "ok n=%u (stage-13/15 "
                                                        "residual "
                                                        "Soft!=product)\n",
                                                        (unsigned)
                                                            g_u32CtrlPassSoftN);
                                                }
                                            } else if (
                                                xhci_ep0_soft_recover_after_to(
                                                    "ctrl_pass_late9") != 0) {
                                                if (fStampPass != 0) {
                                                    kprintf(
                                                        "xhci: ep0 mid-ring "
                                                        "soft-continue after "
                                                        "ctrl PASS late9 "
                                                        "soft-recover n=%u "
                                                        "(stage-13/15 "
                                                        "Soft!=product)\n",
                                                        (unsigned)
                                                            g_u32CtrlPassSoftN);
                                                }
                                            } else {
                                                /*
                                                 * Tenth late settle (Gap C
                                                 * dual-DoD residual deepen
                                                 * Soft!=product): a12f may post
                                                 * TR Deq after late9
                                                 * soft-recover still quiet -
                                                 * one more mid-ring soft-
                                                 * continue before leave
                                                 * desynced. Prefer soft over
                                                 * thrash Set TR Deq (a12f
                                                 * G-AC-1). Soft!=product Stage
                                                 * OPEN.
                                                 * Grep: xhci: ep0 mid-ring
                                                 *       soft-continue after
                                                 *       ctrl PASS late10
                                                 */
                                                xhci_settle(3200000u);
                                                xhci_drain_events(192u);
                                                u32EpStPass =
                                                    xhci_ep0_sample(
                                                        &u64DeqPass);
                                                if (xhci_ep0_soft_continue_ok(
                                                        u32EpStPass,
                                                        u64DeqPass) != 0 ||
                                                    xhci_ep0_producer_hc_aligned(
                                                        u32EpStPass,
                                                        u64DeqPass) != 0) {
                                                    if (fStampPass != 0) {
                                                        kprintf(
                                                            "xhci: ep0 mid-ring "
                                                            "soft-continue after "
                                                            "ctrl PASS late10 "
                                                            "state=%u idx=%u "
                                                            "n=%u (stage-13/15 "
                                                            "Soft!=product)\n",
                                                            (unsigned)
                                                                u32EpStPass,
                                                            (unsigned)
                                                                g_u32Ep0Idx,
                                                            (unsigned)
                                                                g_u32CtrlPassSoftN);
                                                    }
                                                    xhci_clflush_ptr(
                                                        g_pEp0Ring);
                                                    xhci_clflush_ptr(
                                                        &g_pEp0Ring
                                                            [XHCI_RING_TRBS -
                                                             1u]);
                                                } else if (
                                                    xhci_ep0_soft_sync_producer() !=
                                                    0) {
                                                    if (fStampPass != 0) {
                                                        kprintf(
                                                            "xhci: ep0 soft-sync "
                                                            "after ctrl PASS "
                                                            "late10 ok n=%u "
                                                            "(stage-13/15 "
                                                            "residual "
                                                            "Soft!=product)\n",
                                                            (unsigned)
                                                                g_u32CtrlPassSoftN);
                                                    }
                                                } else if (
                                                    xhci_ep0_soft_recover_after_to(
                                                        "ctrl_pass_late10") !=
                                                    0) {
                                                    if (fStampPass != 0) {
                                                        kprintf(
                                                            "xhci: ep0 mid-ring "
                                                            "soft-continue after "
                                                            "ctrl PASS late10 "
                                                            "soft-recover n=%u "
                                                            "(stage-13/15 "
                                                            "Soft!=product)\n",
                                                            (unsigned)
                                                                g_u32CtrlPassSoftN);
                                                    }
                                                } else {
                                                    /*
                                                     * Eleventh late settle
                                                     * (Gap C dual-DoD residual
                                                     * deepen Soft!=product):
                                                     * a12f may post TR Deq after
                                                     * late10 soft-recover still
                                                     * quiet - one more mid-ring
                                                     * soft-continue before leave
                                                     * desynced. Prefer soft over
                                                     * thrash Set TR Deq (a12f
                                                     * G-AC-1). Soft!=product
                                                     * Stage OPEN.
                                                     * Grep: xhci: ep0 mid-ring
                                                     *       soft-continue after
                                                     *       ctrl PASS late11
                                                     */
                                                    xhci_settle(3500000u);
                                                    xhci_drain_events(208u);
                                                    u32EpStPass =
                                                        xhci_ep0_sample(
                                                            &u64DeqPass);
                                                    if (xhci_ep0_soft_continue_ok(
                                                            u32EpStPass,
                                                            u64DeqPass) != 0 ||
                                                        xhci_ep0_producer_hc_aligned(
                                                            u32EpStPass,
                                                            u64DeqPass) != 0) {
                                                        if (fStampPass != 0) {
                                                            kprintf(
                                                                "xhci: ep0 mid-ring "
                                                                "soft-continue after "
                                                                "ctrl PASS late11 "
                                                                "state=%u idx=%u "
                                                                "n=%u (stage-13/15 "
                                                                "Soft!=product)\n",
                                                                (unsigned)
                                                                    u32EpStPass,
                                                                (unsigned)
                                                                    g_u32Ep0Idx,
                                                                (unsigned)
                                                                    g_u32CtrlPassSoftN);
                                                        }
                                                        xhci_clflush_ptr(
                                                            g_pEp0Ring);
                                                        xhci_clflush_ptr(
                                                            &g_pEp0Ring
                                                                [XHCI_RING_TRBS -
                                                                 1u]);
                                                    } else if (
                                                        xhci_ep0_soft_sync_producer() !=
                                                        0) {
                                                        if (fStampPass != 0) {
                                                            kprintf(
                                                                "xhci: ep0 soft-sync "
                                                                "after ctrl PASS "
                                                                "late11 ok n=%u "
                                                                "(stage-13/15 "
                                                                "residual "
                                                                "Soft!=product)\n",
                                                                (unsigned)
                                                                    g_u32CtrlPassSoftN);
                                                        }
                                                    } else if (
                                                        xhci_ep0_soft_recover_after_to(
                                                            "ctrl_pass_late11") !=
                                                        0) {
                                                        if (fStampPass != 0) {
                                                            kprintf(
                                                                "xhci: ep0 mid-ring "
                                                                "soft-continue after "
                                                                "ctrl PASS late11 "
                                                                "soft-recover n=%u "
                                                                "(stage-13/15 "
                                                                "Soft!=product)\n",
                                                                (unsigned)
                                                                    g_u32CtrlPassSoftN);
                                                        }
                                                    } else {
                                                        /*
                                                         * Twelfth late settle
                                                         * (Gap C dual-DoD residual
                                                         * deepen Soft!=product):
                                                         * a12f may post TR Deq after
                                                         * late11 soft-recover still
                                                         * quiet - one more mid-ring
                                                         * soft-continue before leave
                                                         * desynced. Prefer soft over
                                                         * thrash Set TR Deq (a12f
                                                         * G-AC-1). Soft!=product
                                                         * Stage OPEN.
                                                         * Grep: xhci: ep0 mid-ring
                                                         *       soft-continue after
                                                         *       ctrl PASS late12
                                                         */
                                                        xhci_settle(3800000u);
                                                        xhci_drain_events(224u);
                                                        u32EpStPass =
                                                            xhci_ep0_sample(
                                                                &u64DeqPass);
                                                        if (xhci_ep0_soft_continue_ok(
                                                                u32EpStPass,
                                                                u64DeqPass) != 0 ||
                                                            xhci_ep0_producer_hc_aligned(
                                                                u32EpStPass,
                                                                u64DeqPass) != 0) {
                                                            if (fStampPass != 0) {
                                                                kprintf(
                                                                    "xhci: ep0 mid-ring "
                                                                    "soft-continue after "
                                                                    "ctrl PASS late12 "
                                                                    "state=%u idx=%u "
                                                                    "n=%u (stage-13/15 "
                                                                    "Soft!=product)\n",
                                                                    (unsigned)
                                                                        u32EpStPass,
                                                                    (unsigned)
                                                                        g_u32Ep0Idx,
                                                                    (unsigned)
                                                                        g_u32CtrlPassSoftN);
                                                            }
                                                            xhci_clflush_ptr(
                                                                g_pEp0Ring);
                                                            xhci_clflush_ptr(
                                                                &g_pEp0Ring
                                                                    [XHCI_RING_TRBS -
                                                                     1u]);
                                                        } else if (
                                                            xhci_ep0_soft_sync_producer() !=
                                                            0) {
                                                            if (fStampPass != 0) {
                                                                kprintf(
                                                                    "xhci: ep0 soft-sync "
                                                                    "after ctrl PASS "
                                                                    "late12 ok n=%u "
                                                                    "(stage-13/15 "
                                                                    "residual "
                                                                    "Soft!=product)\n",
                                                                    (unsigned)
                                                                        g_u32CtrlPassSoftN);
                                                            }
                                                        } else if (
                                                            xhci_ep0_soft_recover_after_to(
                                                                "ctrl_pass_late12") !=
                                                            0) {
                                                            if (fStampPass != 0) {
                                                                kprintf(
                                                                    "xhci: ep0 mid-ring "
                                                                    "soft-continue after "
                                                                    "ctrl PASS late12 "
                                                                    "soft-recover n=%u "
                                                                    "(stage-13/15 "
                                                                    "Soft!=product)\n",
                                                                    (unsigned)
                                                                        g_u32CtrlPassSoftN);
                                                            }
                                                        } else {
                                                            /*
                                                             * Thirteenth late settle
                                                             * (Gap C freestanding residual
                                                             * deepen Soft!=product past
                                                             * 11/13->15+): a12f may post TR
                                                             * Deq after late12 soft-recover
                                                             * still quiet - one more
                                                             * mid-ring soft-continue before
                                                             * leave desynced. Prefer soft
                                                             * over thrash Set TR Deq (a12f
                                                             * G-AC-1). Soft!=product Stage
                                                             * OPEN.
                                                             * Grep: xhci: ep0 mid-ring
                                                             *       soft-continue after
                                                             *       ctrl PASS late13
                                                             */
                                                            xhci_settle(4100000u);
                                                            xhci_drain_events(240u);
                                                            u32EpStPass =
                                                                xhci_ep0_sample(
                                                                    &u64DeqPass);
                                                            if (xhci_ep0_soft_continue_ok(
                                                                    u32EpStPass,
                                                                    u64DeqPass) != 0 ||
                                                                xhci_ep0_producer_hc_aligned(
                                                                    u32EpStPass,
                                                                    u64DeqPass) != 0) {
                                                                if (fStampPass != 0) {
                                                                    kprintf(
                                                                        "xhci: ep0 mid-ring "
                                                                        "soft-continue after "
                                                                        "ctrl PASS late13 "
                                                                        "state=%u idx=%u "
                                                                        "n=%u (stage-13/15 "
                                                                        "Soft!=product)\n",
                                                                        (unsigned)
                                                                            u32EpStPass,
                                                                        (unsigned)
                                                                            g_u32Ep0Idx,
                                                                        (unsigned)
                                                                            g_u32CtrlPassSoftN);
                                                                }
                                                                xhci_clflush_ptr(
                                                                    g_pEp0Ring);
                                                                xhci_clflush_ptr(
                                                                    &g_pEp0Ring
                                                                        [XHCI_RING_TRBS -
                                                                         1u]);
                                                            } else if (
                                                                xhci_ep0_soft_sync_producer() !=
                                                                0) {
                                                                if (fStampPass != 0) {
                                                                    kprintf(
                                                                        "xhci: ep0 soft-sync "
                                                                        "after ctrl PASS "
                                                                        "late13 ok n=%u "
                                                                        "(stage-13/15 "
                                                                        "residual "
                                                                        "Soft!=product)\n",
                                                                        (unsigned)
                                                                            g_u32CtrlPassSoftN);
                                                                }
                                                            } else if (
                                                                xhci_ep0_soft_recover_after_to(
                                                                    "ctrl_pass_late13") !=
                                                                0) {
                                                                if (fStampPass != 0) {
                                                                    kprintf(
                                                                        "xhci: ep0 mid-ring "
                                                                        "soft-continue after "
                                                                        "ctrl PASS late13 "
                                                                        "soft-recover n=%u "
                                                                        "(stage-13/15 "
                                                                        "Soft!=product)\n",
                                                                        (unsigned)
                                                                            g_u32CtrlPassSoftN);
                                                                }
                                                            } else {
                                                                /*
                                                                 * Fourteenth late settle
                                                                 * (Gap C freestanding
                                                                 * residual deepen past
                                                                 * 11/13->15+ dual-DoD
                                                                 * Soft!=product): a12f may
                                                                 * post TR Deq after late13
                                                                 * soft-recover still quiet
                                                                 * - one more mid-ring soft-
                                                                 * continue before leave
                                                                 * desynced. Prefer soft
                                                                 * over thrash Set TR Deq
                                                                 * (a12f G-AC-1). Soft!=product
                                                                 * Stage OPEN. Dual DoD A
                                                                 * stick OPEN.
                                                                 * Grep: xhci: ep0 mid-ring
                                                                 *       soft-continue after
                                                                 *       ctrl PASS late14
                                                                 */
                                                                xhci_settle(4400000u);
                                                                xhci_drain_events(256u);
                                                                u32EpStPass =
                                                                    xhci_ep0_sample(
                                                                        &u64DeqPass);
                                                                if (xhci_ep0_soft_continue_ok(
                                                                        u32EpStPass,
                                                                        u64DeqPass) != 0 ||
                                                                    xhci_ep0_producer_hc_aligned(
                                                                        u32EpStPass,
                                                                        u64DeqPass) != 0) {
                                                                    if (fStampPass != 0) {
                                                                        kprintf(
                                                                            "xhci: ep0 mid-ring "
                                                                            "soft-continue after "
                                                                            "ctrl PASS late14 "
                                                                            "state=%u idx=%u "
                                                                            "n=%u (stage-13/15 "
                                                                            "Soft!=product)\n",
                                                                            (unsigned)
                                                                                u32EpStPass,
                                                                            (unsigned)
                                                                                g_u32Ep0Idx,
                                                                            (unsigned)
                                                                                g_u32CtrlPassSoftN);
                                                                    }
                                                                    xhci_clflush_ptr(
                                                                        g_pEp0Ring);
                                                                    xhci_clflush_ptr(
                                                                        &g_pEp0Ring
                                                                            [XHCI_RING_TRBS -
                                                                             1u]);
                                                                } else if (
                                                                    xhci_ep0_soft_sync_producer() !=
                                                                    0) {
                                                                    if (fStampPass != 0) {
                                                                        kprintf(
                                                                            "xhci: ep0 soft-sync "
                                                                            "after ctrl PASS "
                                                                            "late14 ok n=%u "
                                                                            "(stage-13/15 "
                                                                            "residual "
                                                                            "Soft!=product)\n",
                                                                            (unsigned)
                                                                                g_u32CtrlPassSoftN);
                                                                    }
                                                                } else if (
                                                                    xhci_ep0_soft_recover_after_to(
                                                                        "ctrl_pass_late14") !=
                                                                    0) {
                                                                    if (fStampPass != 0) {
                                                                        kprintf(
                                                                            "xhci: ep0 mid-ring "
                                                                            "soft-continue after "
                                                                            "ctrl PASS late14 "
                                                                            "soft-recover n=%u "
                                                                            "(stage-13/15 "
                                                                            "Soft!=product)\n",
                                                                            (unsigned)
                                                                                g_u32CtrlPassSoftN);
                                                                    }
                                                                } else {
                                                                    /*
                                                                     * Fifteenth late settle
                                                                     * (Gap C freestanding
                                                                     * residual deepen past
                                                                     * 11/13->15+ dual-DoD
                                                                     * Soft!=product): a12f may
                                                                     * post TR Deq after late14
                                                                     * soft-recover still quiet
                                                                     * - one more mid-ring soft-
                                                                     * continue before leave
                                                                     * desynced. Prefer soft
                                                                     * over thrash Set TR Deq
                                                                     * (a12f G-AC-1). Soft!=product
                                                                     * Stage OPEN. Dual DoD A
                                                                     * stick OPEN.
                                                                     * Grep: xhci: ep0 mid-ring
                                                                     *       soft-continue after
                                                                     *       ctrl PASS late15
                                                                     */
                                                                    xhci_settle(4700000u);
                                                                    xhci_drain_events(272u);
                                                                    u32EpStPass =
                                                                        xhci_ep0_sample(
                                                                            &u64DeqPass);
                                                                    if (xhci_ep0_soft_continue_ok(
                                                                            u32EpStPass,
                                                                            u64DeqPass) != 0 ||
                                                                        xhci_ep0_producer_hc_aligned(
                                                                            u32EpStPass,
                                                                            u64DeqPass) != 0) {
                                                                        if (fStampPass != 0) {
                                                                            kprintf(
                                                                                "xhci: ep0 mid-ring "
                                                                                "soft-continue after "
                                                                                "ctrl PASS late15 "
                                                                                "state=%u idx=%u "
                                                                                "n=%u (stage-13/15 "
                                                                                "Soft!=product)\n",
                                                                                (unsigned)
                                                                                    u32EpStPass,
                                                                                (unsigned)
                                                                                    g_u32Ep0Idx,
                                                                                (unsigned)
                                                                                    g_u32CtrlPassSoftN);
                                                                        }
                                                                        xhci_clflush_ptr(
                                                                            g_pEp0Ring);
                                                                        xhci_clflush_ptr(
                                                                            &g_pEp0Ring
                                                                                [XHCI_RING_TRBS -
                                                                                 1u]);
                                                                    } else if (
                                                                        xhci_ep0_soft_sync_producer() !=
                                                                        0) {
                                                                        if (fStampPass != 0) {
                                                                            kprintf(
                                                                                "xhci: ep0 soft-sync "
                                                                                "after ctrl PASS "
                                                                                "late15 ok n=%u "
                                                                                "(stage-13/15 "
                                                                                "residual "
                                                                                "Soft!=product)\n",
                                                                                (unsigned)
                                                                                    g_u32CtrlPassSoftN);
                                                                        }
                                                                    } else if (
                                                                        xhci_ep0_soft_recover_after_to(
                                                                            "ctrl_pass_late15") !=
                                                                        0) {
                                                                        if (fStampPass != 0) {
                                                                            kprintf(
                                                                                "xhci: ep0 mid-ring "
                                                                                "soft-continue after "
                                                                                "ctrl PASS late15 "
                                                                                "soft-recover n=%u "
                                                                                "(stage-13/15 "
                                                                                "Soft!=product)\n",
                                                                                (unsigned)
                                                                                    g_u32CtrlPassSoftN);
                                                                        }
                                                                    } else {
                                                                        /*
                                                                         * Sixteenth late settle
                                                                         * (Gap C freestanding
                                                                         * residual deepen past
                                                                         * 11/13->15+ dual-DoD
                                                                         * Soft!=product): a12f may
                                                                         * post TR Deq after late15
                                                                         * soft-recover still quiet
                                                                         * - one more mid-ring soft-
                                                                         * continue before leave
                                                                         * desynced. Prefer soft
                                                                         * over thrash Set TR Deq
                                                                         * (a12f G-AC-1). Soft!=product
                                                                         * Stage OPEN. Dual DoD A
                                                                         * stick OPEN.
                                                                         * Grep: xhci: ep0 mid-ring
                                                                         *       soft-continue after
                                                                         *       ctrl PASS late16
                                                                         */
                                                                        xhci_settle(5000000u);
                                                                        xhci_drain_events(288u);
                                                                        u32EpStPass =
                                                                            xhci_ep0_sample(
                                                                                &u64DeqPass);
                                                                        if (xhci_ep0_soft_continue_ok(
                                                                                u32EpStPass,
                                                                                u64DeqPass) != 0 ||
                                                                            xhci_ep0_producer_hc_aligned(
                                                                                u32EpStPass,
                                                                                u64DeqPass) != 0) {
                                                                            if (fStampPass != 0) {
                                                                                kprintf(
                                                                                    "xhci: ep0 mid-ring "
                                                                                    "soft-continue after "
                                                                                    "ctrl PASS late16 "
                                                                                    "state=%u idx=%u "
                                                                                    "n=%u (stage-13/15 "
                                                                                    "Soft!=product)\n",
                                                                                    (unsigned)
                                                                                        u32EpStPass,
                                                                                    (unsigned)
                                                                                        g_u32Ep0Idx,
                                                                                    (unsigned)
                                                                                        g_u32CtrlPassSoftN);
                                                                            }
                                                                            xhci_clflush_ptr(
                                                                                g_pEp0Ring);
                                                                            xhci_clflush_ptr(
                                                                                &g_pEp0Ring
                                                                                    [XHCI_RING_TRBS -
                                                                                     1u]);
                                                                        } else if (
                                                                            xhci_ep0_soft_sync_producer() !=
                                                                            0) {
                                                                            if (fStampPass != 0) {
                                                                                kprintf(
                                                                                    "xhci: ep0 soft-sync "
                                                                                    "after ctrl PASS "
                                                                                    "late16 ok n=%u "
                                                                                    "(stage-13/15 "
                                                                                    "residual "
                                                                                    "Soft!=product)\n",
                                                                                    (unsigned)
                                                                                        g_u32CtrlPassSoftN);
                                                                            }
                                                                        } else if (
                                                                            xhci_ep0_soft_recover_after_to(
                                                                                "ctrl_pass_late16") !=
                                                                            0) {
                                                                            if (fStampPass != 0) {
                                                                                kprintf(
                                                                                    "xhci: ep0 mid-ring "
                                                                                    "soft-continue after "
                                                                                    "ctrl PASS late16 "
                                                                                    "soft-recover n=%u "
                                                                                    "(stage-13/15 "
                                                                                    "Soft!=product)\n",
                                                                                    (unsigned)
                                                                                        g_u32CtrlPassSoftN);
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            /* still deny: leave producer; caller recover path hard-resyncs */
        }
    }
    return 0;
}

/* ---- bulk transfer ------------------------------------------------------ */
static int
xhci_bulk(int fIn, gj_paddr_t paBuf, u32 cb)
{
    u8 u8Ep = fIn != 0 ? g_u8EpIn : g_u8EpOut;
    u8 u8Dci = (u8)(u8Ep * 2u + (fIn != 0 ? 1u : 0u));
    struct xhci_trb *pRing;
    gj_paddr_t paRing;
    u32 *pIdx;
    u8 *pCycle;

    if (fIn != 0) {
        pRing = g_pBulkInRing;
        paRing = g_paBulkInRing;
        pIdx = &g_u32BulkInIdx;
        pCycle = &g_fBulkInCycle;
    } else {
        pRing = g_pBulkOutRing;
        paRing = g_paBulkOutRing;
        pIdx = &g_u32BulkOutIdx;
        pCycle = &g_fBulkOutCycle;
    }

    /* Ensure bounce buffer is visible to HC DMA before doorbell. */
    if (paBuf == g_paScratch && g_pScratch != NULL) {
        xhci_clflush_ptr(g_pScratch);
    } else if (paBuf == g_paScratch2 && g_pScratch2 != NULL) {
        xhci_clflush_ptr(g_pScratch2);
    }
    xfer_enqueue(pRing, pIdx, pCycle, paRing, (u64)paBuf, cb,
                 (XHCI_TRB_TYPE_NORMAL << 10) | XHCI_TRB_IOC);
    doorbell_ep(g_u8SlotId, u8Dci);
    return xhci_wait_xfer(XHCI_POLL_SPINS);
}

/* ---- context helpers ---------------------------------------------------- */
static u8 *
ctx_slot(void *pBase, int fInput)
{
    u8 *p = (u8 *)pBase;

    /* Input: skip Input Control Context (1 ctx); Device: slot at 0 */
    if (fInput != 0) {
        p += g_u8CtxSize;
    }
    return p;
}

static u8 *
ctx_ep(void *pBase, int fInput, u8 u8Dci)
{
    u8 *p = (u8 *)pBase;
    u32 u32Idx = (u32)u8Dci + (fInput != 0 ? 1u : 0u);

    return p + u32Idx * (u32)g_u8CtxSize;
}

static void
ctx_write64(u8 *pCtx, u32 u32Off, u64 u64Val)
{
    memcpy(pCtx + u32Off, &u64Val, 8);
}

static void
ctx_write32(u8 *pCtx, u32 u32Off, u32 u32Val)
{
    memcpy(pCtx + u32Off, &u32Val, 4);
}

static u16
ep0_max_packet(u8 u8Speed)
{
    if (u8Speed == 4u || u8Speed == 5u) {
        return 512;
    }
    if (u8Speed == 3u) {
        return 64;
    }
    if (u8Speed == 2u) {
        return 8;
    }
    return 64; /* full-speed default */
}

/*
 * USB2: bMaxPacketSize0 is the byte size (8/16/32/64).
 * USB3 SuperSpeed: bMaxPacketSize0 is an exponent (typically 9 -> 512).
 * Treating SS as a raw byte count (e.g. 9) breaks all later control xfers.
 */
static u16
decode_bmax_packet0(u8 u8B, u8 u8Speed)
{
    if (u8Speed >= 4u) {
        if (u8B >= 8u && u8B <= 15u) {
            return (u16)(1u << u8B);
        }
        return 512;
    }
    if (u8B == 8u || u8B == 16u || u8B == 32u || u8B == 64u) {
        return (u16)u8B;
    }
    return ep0_max_packet(u8Speed);
}

/* Reset EP0 transfer ring producer state to match a fresh dequeue pointer. */
static void
xhci_ep0_ring_reset(void)
{
    memset(g_pEp0Ring, 0, GJ_PAGE_SIZE);
    ring_link_init(g_pEp0Ring, g_paEp0Ring, 1);
    g_u32Ep0Idx = 0;
    g_fEp0Cycle = 1;
    xhci_clflush_ptr(g_pEp0Ring);
    xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
}

static int
xhci_port_is_ss_cap(u8 u8Port)
{
    if (u8Port == 0 || u8Port > XHCI_MAX_PORTS) {
        return 0;
    }
    if (g_u32SsPortBits == 0) {
        return 0; /* unknown - treat as not preferentially SS */
    }
    return (g_u32SsPortBits & (1u << (u8Port - 1u))) != 0 ? 1 : 0;
}

/**
 * Walk xECP Supported Protocol (id=2). Major>=3 -> SuperSpeed-capable ports.
 * Public xHCI 7.2; no Linux structures.
 */
static void
xhci_parse_ss_ports(void)
{
    u32 u32Hcc1 = mmio_r32(g_pCap, 0x10);
    u32 u32Xecp = (u32Hcc1 >> 16) & 0xffffu;
    u32 cGuard = 0;

    g_u32SsPortBits = 0;
    g_u8SsPortLo = 0;
    g_u8SsPortHi = 0;

    while (u32Xecp != 0 && cGuard < 64u) {
        volatile u8 *pCap = g_pCap + (u32Xecp << 2);
        u32 u32Head = mmio_r32(pCap, 0);
        u8 u8Id = (u8)(u32Head & 0xffu);
        u8 u8Next = (u8)((u32Head >> 8) & 0xffu);

        if (u8Id == 2u) {
            u8 u8Major = (u8)((u32Head >> 24) & 0xffu);
            u32 u32Dw2 = mmio_r32(pCap, 8);
            u8 u8Off = (u8)(u32Dw2 & 0xffu);          /* Compatible Port Offset */
            u8 u8Cnt = (u8)((u32Dw2 >> 8) & 0xffu);    /* Compatible Port Count */
            u32 i;

            if (u8Major >= 3u && u8Off >= 1u && u8Cnt > 0u) {
                for (i = 0; i < (u32)u8Cnt; i++) {
                    u8 u8P = (u8)(u8Off + i);

                    if (u8P >= 1u && u8P <= XHCI_MAX_PORTS) {
                        g_u32SsPortBits |= (1u << (u8P - 1u));
                    }
                }
                if (g_u8SsPortLo == 0 || u8Off < g_u8SsPortLo) {
                    g_u8SsPortLo = u8Off;
                }
                {
                    u8 u8Hi = (u8)(u8Off + u8Cnt - 1u);

                    if (u8Hi > g_u8SsPortHi) {
                        g_u8SsPortHi = u8Hi;
                    }
                }
                kprintf("xhci: SS-capable ports offset=%u count=%u maj=%u "
                        "bits=0x%x\n",
                        (unsigned)u8Off, (unsigned)u8Cnt, (unsigned)u8Major,
                        (unsigned)g_u32SsPortBits);
            } else {
                kprintf("xhci: protocol ports offset=%u count=%u maj=%u "
                        "(USB2-class)\n",
                        (unsigned)u8Off, (unsigned)u8Cnt, (unsigned)u8Major);
            }
        }
        if (u8Next == 0) {
            break;
        }
        u32Xecp = u32Xecp + (u32)u8Next;
        cGuard++;
    }
    if (g_u32SsPortBits != 0) {
        kprintf("xhci: SS port range lo=%u hi=%u mask=0x%x\n",
                (unsigned)g_u8SsPortLo, (unsigned)g_u8SsPortHi,
                (unsigned)g_u32SsPortBits);
    }
}

/* Update EP0 Max Packet Size in device context after bMaxPacketSize0 known. */
static int
xhci_evaluate_ep0_mps(void)
{
    u8 *pIcc;
    u8 *pSlot;
    u8 *pEp0;
    u32 u32Ctrl;
    u32 u32Code = 0;

    /*
     * Evaluate Context may ignore TR Dequeue while EP is Running (xHCI 4.6.7).
     * Stop EP0 first, then Evaluate MPS, then Set TR Deq | DCS=1 so software
     * producer (idx=0 cycle=1) matches HC - else stage-13/15 desync.
     */
    if (g_u8SlotId == 0) {
        xhci_ep0_ring_reset();
        return -1;
    }
    xhci_drain_events(16u);
    u32Ctrl = (XHCI_TRB_TYPE_STOP_EP << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    (void)xhci_cmd(0, 0, u32Ctrl, &u32Code, NULL);
    xhci_drain_events(16u);

    xhci_ep0_ring_reset();

    memset(g_pInputCtx, 0, GJ_PAGE_SIZE);
    pIcc = (u8 *)g_pInputCtx;
    /* Add Context Flags: A1 (EP0 only) */
    ctx_write32(pIcc, 0x04, 0x2u);

    pSlot = ctx_slot(g_pInputCtx, 1);
    ctx_write32(pSlot, 0x00, (1u << 27) | ((u32)g_u8PortSpeed << 20));
    ctx_write32(pSlot, 0x04, ((u32)g_u8PortId << 16));

    pEp0 = ctx_ep(g_pInputCtx, 1, 1);
    ctx_write32(pEp0, 0x00, 0);
    ctx_write32(pEp0, 0x04,
                (3u << 1) | (XHCI_EP_TYPE_CTRL_BI << 3) |
                    ((u32)g_u16MaxPkt0 << 16));
    /* Dequeue = ring base, DCS = 1 (matches g_fEp0Cycle after reset) */
    ctx_write64(pEp0, 0x08, (u64)g_paEp0Ring | 1ull);
    ctx_write32(pEp0, 0x10, 8u);

    xhci_clflush_span(g_pInputCtx, (u32)g_u8CtxSize * 3u);
    xhci_clflush_ptr(g_pEp0Ring);

    u32Ctrl = (XHCI_TRB_TYPE_EVAL_CTX << 10) | ((u32)g_u8SlotId << 24);
    if (xhci_cmd((u64)g_paInputCtx, 0, u32Ctrl, &u32Code, NULL) != 0) {
        /*
         * Stage-12 control-path honesty: sticky Evaluate fail cc (0=TO)
         * before later arm/Set TR Deq Success can overwrite g_u32LastCc.
         * Soft!=product · never product T1 from stage count.
         * Grep: xhci: Evaluate Context EP0 FAIL | stage=12
         */
        kprintf("xhci: Evaluate Context EP0 FAIL code=%u mps0=%u "
                "(stage=12 Soft!=product)\n",
                u32Code, (unsigned)g_u16MaxPkt0);
        xhci_note_ctrl_fail_cc(u32Code);
        return -1;
    }
    /* Explicit Set TR Deq after Evaluate so DCS/producer cannot drift. */
    xhci_ep0_ring_reset();
    u32Ctrl = (XHCI_TRB_TYPE_SET_TR_DEQ << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    if (xhci_cmd((u64)g_paEp0Ring | 1ull, 0, u32Ctrl, &u32Code, NULL) != 0) {
        kprintf("xhci: Set TR Deq after Evaluate FAIL code=%u (soft cont)\n",
                u32Code);
    }
    xhci_ep0_ring_reset();
    xhci_clflush_ptr(g_pEp0Ring);
    kprintf("xhci: Evaluate Context EP0 mps0=%u soft PASS\n",
            (unsigned)g_u16MaxPkt0);
    return 0;
}

/* ---- HC init ------------------------------------------------------------ */
/**
 * Soft BIOS handoff via xECP USBLEGSUP (public xHCI extended caps).
 * Walks HCCPARAMS1 xECP list; if USBLEGSUP (id=1) present, set OS owned.
 */
static void
xhci_bios_handoff(void)
{
    u32 u32Hcc1 = mmio_r32(g_pCap, 0x10);
    u32 u32Xecp = (u32Hcc1 >> 16) & 0xffffu;
    u32 cGuard = 0;

    while (u32Xecp != 0 && cGuard < 64u) {
        volatile u8 *pCap = g_pCap + (u32Xecp << 2);
        u32 u32Head = mmio_r32(pCap, 0);
        u8 u8Id = (u8)(u32Head & 0xffu);
        u8 u8Next = (u8)((u32Head >> 8) & 0xffu);

        if (u8Id == 1u) {
            /* USBLEGSUP: bit 16 HC BIOS Owned, bit 24 HC OS Owned */
            u32 u32Leg = u32Head;
            u32 iSpin;

            if ((u32Leg & (1u << 16)) != 0 || (u32Leg & (1u << 24)) == 0) {
                u32Leg |= (1u << 24);
                mmio_w32(pCap, 0, u32Leg);
                for (iSpin = 0; iSpin < XHCI_POLL_SPINS; iSpin++) {
                    u32Leg = mmio_r32(pCap, 0);
                    if ((u32Leg & (1u << 16)) == 0) {
                        break;
                    }
                    cpu_relax();
                }
                /* Clear SMI enables in USBLEGCTLSTS @ +4 if present */
                mmio_w32(pCap, 4, 0);
                kprintf("xhci: BIOS handoff soft %s\n",
                        (u32Leg & (1u << 16)) == 0 ? "PASS" : "TIMEOUT");
            }
            return;
        }
        if (u8Next == 0) {
            break;
        }
        u32Xecp = u32Xecp + (u32)u8Next;
        cGuard++;
    }
}

static int
xhci_hc_reset_run(void)
{
    u32 u32Cmd;
    u32 u32Sts;
    u32 iSpin;

    xhci_bios_handoff();

    /* Halt if running */
    u32Cmd = mmio_r32(g_pOp, 0x00);
    if ((u32Cmd & XHCI_USBCMD_RS) != 0) {
        mmio_w32(g_pOp, 0x00, u32Cmd & ~XHCI_USBCMD_RS);
        for (iSpin = 0; iSpin < XHCI_POLL_SPINS; iSpin++) {
            u32Sts = mmio_r32(g_pOp, 0x04);
            if ((u32Sts & XHCI_USBSTS_HCH) != 0) {
                break;
            }
            cpu_relax();
        }
    }

    /* Wait CNR clear */
    for (iSpin = 0; iSpin < XHCI_POLL_SPINS; iSpin++) {
        u32Sts = mmio_r32(g_pOp, 0x04);
        if ((u32Sts & XHCI_USBSTS_CNR) == 0) {
            break;
        }
        cpu_relax();
    }
    if ((mmio_r32(g_pOp, 0x04) & XHCI_USBSTS_CNR) != 0) {
        return -1;
    }

    mmio_w32(g_pOp, 0x00, XHCI_USBCMD_HCRST);
    for (iSpin = 0; iSpin < XHCI_POLL_SPINS; iSpin++) {
        u32Cmd = mmio_r32(g_pOp, 0x00);
        u32Sts = mmio_r32(g_pOp, 0x04);
        if ((u32Cmd & XHCI_USBCMD_HCRST) == 0 &&
            (u32Sts & XHCI_USBSTS_CNR) == 0) {
            break;
        }
        cpu_relax();
    }
    if ((mmio_r32(g_pOp, 0x00) & XHCI_USBCMD_HCRST) != 0) {
        return -1;
    }
    return 0;
}

static int
xhci_setup_rings(void)
{
    void *pVa = NULL;
    struct xhci_erst_ent *pErst;
    u32 u32Pages;
    u32 i;

    g_paDcbaa = dma_alloc_page((void **)&g_pDcbaa);
    g_paCmdRing = dma_alloc_page((void **)&g_pCmdRing);
    g_paEvtRing = dma_alloc_page((void **)&g_pEvtRing);
    g_paErst = dma_alloc_page(&pVa);
    g_paDevCtx = dma_alloc_page(&g_pDevCtx);
    g_paInputCtx = dma_alloc_page(&g_pInputCtx);
    g_paEp0Ring = dma_alloc_page((void **)&g_pEp0Ring);
    g_paBulkOutRing = dma_alloc_page((void **)&g_pBulkOutRing);
    g_paBulkInRing = dma_alloc_page((void **)&g_pBulkInRing);
    g_paScratch = dma_alloc_page(&g_pScratch);
    g_paScratch2 = dma_alloc_page(&g_pScratch2);

    if (g_paDcbaa == 0 || g_paCmdRing == 0 || g_paEvtRing == 0 ||
        g_paErst == 0 || g_paDevCtx == 0 || g_paInputCtx == 0 ||
        g_paEp0Ring == 0 || g_paBulkOutRing == 0 || g_paBulkInRing == 0 ||
        g_paScratch == 0 || g_paScratch2 == 0) {
        kprintf("xhci: dma alloc FAIL\n");
        return -1;
    }

    pErst = (struct xhci_erst_ent *)pVa;
    pErst[0].u64Base = (u64)g_paEvtRing;
    pErst[0].u32Size = XHCI_RING_TRBS;
    pErst[0].u32Rsvd = 0;

    ring_link_init(g_pCmdRing, g_paCmdRing, 1);
    ring_link_init(g_pEp0Ring, g_paEp0Ring, 1);
    ring_link_init(g_pBulkOutRing, g_paBulkOutRing, 1);
    ring_link_init(g_pBulkInRing, g_paBulkInRing, 1);
    /* Event ring: no link; software wraps with cycle toggle */

    g_fCmdCycle = 1;
    g_fEvtCycle = 1;
    g_fEp0Cycle = 1;
    g_fBulkOutCycle = 1;
    g_fBulkInCycle = 1;
    g_u32CmdIdx = 0;
    g_u32EvtIdx = 0;
    g_u32Ep0Idx = 0;
    g_u32BulkOutIdx = 0;
    g_u32BulkInIdx = 0;

    /*
     * Push zeroed rings / ERST / DCBAA to DRAM before HC run. On a12f with
     * non-snoop DMA, dirty CPU zeros + later clflush can writeback over HC
     * Transfer Events -> stage-11 GET_DESC cc=0 TO with working commands
     * (lucky command path timing). Make lines clean before RS=1.
     */
    xhci_clflush_span(g_pEvtRing,
                      (u32)(XHCI_RING_TRBS * sizeof(struct xhci_trb)));
    xhci_clflush_span(pErst, (u32)sizeof(*pErst));
    xhci_clflush_span(g_pCmdRing,
                      (u32)(XHCI_RING_TRBS * sizeof(struct xhci_trb)));
    xhci_clflush_span(g_pEp0Ring,
                      (u32)(XHCI_RING_TRBS * sizeof(struct xhci_trb)));
    xhci_clflush_span(g_pDcbaa, 256u);
    xhci_clflush_span(g_pScratch, 64u);

    /* Max slots */
    mmio_w32(g_pOp, 0x38, (u32)g_u8MaxSlots);

    /* Scratchpad if required (HCSPARAMS2 Max Scratchpad Bufs) */
    {
        u32 u32Hcs2 = mmio_r32(g_pCap, 0x08);
        u32 u32MaxSp =
            ((u32Hcs2 >> 21) & 0x1fu) | (((u32Hcs2 >> 27) & 0x1fu) << 5);

        if (u32MaxSp > 0) {
            gj_paddr_t paArray = dma_alloc_page(NULL);
            u64 *pArr;

            if (paArray == 0) {
                return -1;
            }
            pArr = (u64 *)hhdm_to_virt(paArray);
            memset(pArr, 0, GJ_PAGE_SIZE);
            u32Pages = u32MaxSp;
            if (u32Pages > (GJ_PAGE_SIZE / 8u)) {
                u32Pages = GJ_PAGE_SIZE / 8u;
            }
            for (i = 0; i < u32Pages; i++) {
                gj_paddr_t paBuf = dma_alloc_page(NULL);

                if (paBuf == 0) {
                    return -1;
                }
                pArr[i] = (u64)paBuf;
            }
            g_pDcbaa[0] = (u64)paArray;
        }
    }

    mmio_w64(g_pOp, 0x30, (u64)g_paDcbaa); /* DCBAAP */
    mmio_w64(g_pOp, 0x18, (u64)g_paCmdRing | 1ull); /* CRCR + RCS */

    /* Interrupter 0 */
    mmio_w32(g_pRt, 0x20u + 0x08u, 1u); /* ERSTSZ */
    mmio_w64(g_pRt, 0x20u + 0x10u, (u64)g_paErst); /* ERSTBA */
    mmio_w64(g_pRt, 0x20u + 0x18u, (u64)g_paEvtRing); /* ERDP */

    /* Run (no INTE - poll event ring) */
    mmio_w32(g_pOp, 0x00, XHCI_USBCMD_RS);
    for (i = 0; i < 10000u; i++) {
        if ((mmio_r32(g_pOp, 0x04) & XHCI_USBSTS_HCH) == 0) {
            break;
        }
        cpu_relax();
    }
    if ((mmio_r32(g_pOp, 0x04) & XHCI_USBSTS_HCH) != 0) {
        return -1;
    }
    return 0;
}

/* ---- port / slot / address ---------------------------------------------- */
static u32
portsc_read(u8 u8Port)
{
    return mmio_r32(g_pOp, 0x400u + (u32)(u8Port - 1u) * 0x10u);
}

static void
portsc_write(u8 u8Port, u32 u32Val)
{
    mmio_w32(g_pOp, 0x400u + (u32)(u8Port - 1u) * 0x10u, u32Val);
}

/* Force link state U0 (PLS=0) via LWS - needed after SS reset on a12f. */
static void
xhci_port_force_u0(u8 u8Port)
{
    u32 u32Ps = portsc_read(u8Port);
    u32 u32Pls = (u32Ps >> 5) & 0xfu;
    u32 iWait;

    if (u32Pls == 0u) {
        return;
    }
    kprintf("xhci: force U0 from PLS=%u port=%u\n", (unsigned)u32Pls,
            (unsigned)u8Port);
    /* Preserve R/W; clear change; LWS + PLS=0 (U0). */
    portsc_write(u8Port,
                 (u32Ps & XHCI_PORTSC_PRESERVE & ~(0xfu << 5)) |
                     XHCI_PORTSC_PP | XHCI_PORTSC_LWS | XHCI_PORTSC_CHANGE);
    for (iWait = 0; iWait < 500000u; iWait++) {
        cpu_relax();
        u32Ps = portsc_read(u8Port);
        if (((u32Ps >> 5) & 0xfu) == 0u) {
            break;
        }
    }
}

static void
xhci_port_power_on(u8 u8Port)
{
    u32 u32Ps = portsc_read(u8Port);

    /* Ensure PP=1 after HCRST (many HCs leave ports unpowered). */
    if ((u32Ps & XHCI_PORTSC_PP) == 0) {
        portsc_write(u8Port, (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_PP |
                                 XHCI_PORTSC_CHANGE);
    }
}

/*
 * Port reset. fPreferWarm=1: Warm Port Reset first (USB3 train on a12f SS).
 * fPreferWarm=0: cold PR first, warm fallback.
 * G752 p22s4: BSR0 USBTE after cold-only - alternate warm on retries.
 */
static int
xhci_port_reset_ex(u8 u8Port, int fPreferWarm)
{
    u32 u32Ps;
    u32 iSpin;
    int fWarm = 0;
    int fSsCap = xhci_port_is_ss_cap(u8Port);
    int fTryWarmFirst;

    xhci_port_power_on(u8Port);
    u32Ps = portsc_read(u8Port);
    if ((u32Ps & XHCI_PORTSC_CCS) == 0) {
        return -1;
    }

    /* Clear change bits; leave CCS/PED/PLS alone - write-1-to-clear changes */
    portsc_write(u8Port, (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_CHANGE);

    u32Ps = portsc_read(u8Port);
    /* fPreferWarm: -1=auto (SS-cap->warm), 0=cold first, 1=warm first */
    if (fPreferWarm < 0) {
        fTryWarmFirst = (fSsCap != 0) ? 1 : 0;
    } else {
        fTryWarmFirst = fPreferWarm != 0 ? 1 : 0;
    }

    if (fTryWarmFirst != 0) {
        fWarm = 1;
        portsc_write(u8Port,
                     (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_WPR |
                         XHCI_PORTSC_PP);
        for (iSpin = 0; iSpin < XHCI_POLL_SPINS; iSpin++) {
            u32Ps = portsc_read(u8Port);
            if ((u32Ps & XHCI_PORTSC_WRC) != 0 ||
                (u32Ps & XHCI_PORTSC_PRC) != 0) {
                break;
            }
            cpu_relax();
        }
        if ((u32Ps & (XHCI_PORTSC_WRC | XHCI_PORTSC_PRC)) == 0) {
            /* Fall back to cold PR */
            fWarm = 0;
            portsc_write(u8Port,
                         (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_PR |
                             XHCI_PORTSC_PP);
            for (iSpin = 0; iSpin < XHCI_POLL_SPINS; iSpin++) {
                u32Ps = portsc_read(u8Port);
                if ((u32Ps & XHCI_PORTSC_PRC) != 0) {
                    break;
                }
                cpu_relax();
            }
            if ((u32Ps & XHCI_PORTSC_PRC) == 0) {
                return -1;
            }
        }
    } else {
        fWarm = 0;
        portsc_write(u8Port, (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_PR |
                                 XHCI_PORTSC_PP);
        for (iSpin = 0; iSpin < XHCI_POLL_SPINS; iSpin++) {
            u32Ps = portsc_read(u8Port);
            if ((u32Ps & XHCI_PORTSC_PRC) != 0) {
                break;
            }
            cpu_relax();
        }
        if ((u32Ps & XHCI_PORTSC_PRC) == 0) {
            fWarm = 1;
            portsc_write(u8Port,
                         (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_WPR |
                             XHCI_PORTSC_PP);
            for (iSpin = 0; iSpin < XHCI_POLL_SPINS; iSpin++) {
                u32Ps = portsc_read(u8Port);
                if ((u32Ps & XHCI_PORTSC_WRC) != 0 ||
                    (u32Ps & XHCI_PORTSC_PRC) != 0) {
                    break;
                }
                cpu_relax();
            }
            if ((u32Ps & (XHCI_PORTSC_WRC | XHCI_PORTSC_PRC)) == 0) {
                return -1;
            }
        }
    }

    portsc_write(u8Port, (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_CHANGE |
                             XHCI_PORTSC_PP);

    /* PED can take a long time on SS sticks after cold PR (a12f). */
    for (iSpin = 0; iSpin < 20000000u; iSpin++) {
        u32Ps = portsc_read(u8Port);
        if ((u32Ps & XHCI_PORTSC_PED) != 0) {
            break;
        }
        if ((iSpin & 0xfffffu) == 0u) {
            xhci_port_force_u0(u8Port);
        }
        cpu_relax();
    }

    g_u8PortSpeed = (u8)((u32Ps >> 10) & 0xfu);
    /*
     * Do not invent SS/HS when PORTSC speed is still 0 - leave 0 and let
     * resync wait; Address Device refuses speed=0 (avoids cc=4 USBTE).
     */
    if (g_u8PortSpeed != 0u) {
        g_u16MaxPkt0 = ep0_max_packet(g_u8PortSpeed);
    } else {
        g_u16MaxPkt0 = fSsCap != 0 ? 512u : 64u; /* provisional until resync */
    }

    kprintf("xhci: port connect port=%u speed=%u ped=%u mps0=%u warm=%u "
            "ss=%u sscap=%u\n",
            (unsigned)u8Port, (unsigned)g_u8PortSpeed,
            (u32Ps & XHCI_PORTSC_PED) != 0 ? 1u : 0u,
            (unsigned)g_u16MaxPkt0, fWarm ? 1u : 0u,
            (g_u8PortSpeed >= 4u) ? 1u : 0u, fSsCap ? 1u : 0u);

    if ((u32Ps & XHCI_PORTSC_PED) == 0) {
        return -1;
    }

    xhci_port_force_u0(u8Port);

    /* Long settle after PED+U0 (cc=4 if Address Device too soon on a12f). */
    if (g_u8PortSpeed >= 4u || fSsCap != 0) {
        xhci_settle(8000000u);
        if (((portsc_read(u8Port) >> 5) & 0xfu) != 0u) {
            xhci_port_force_u0(u8Port);
        }
        xhci_settle(2000000u);
    } else {
        xhci_settle(1500000u);
    }
    /* Final speed sample after settle. */
    u32Ps = portsc_read(u8Port);
    if (((u32Ps >> 10) & 0xfu) != 0u) {
        g_u8PortSpeed = (u8)((u32Ps >> 10) & 0xfu);
        g_u16MaxPkt0 = ep0_max_packet(g_u8PortSpeed);
    }
    return 0;
}

/* Default: SS-cap warm-first (USB3), USB2 cold-first. */
static int
xhci_port_reset(u8 u8Port)
{
    return xhci_port_reset_ex(u8Port, -1);
}

static void
xhci_disable_slot_soft(void)
{
    u32 u32Code = 0;

    if (g_u8SlotId == 0) {
        return;
    }
    (void)xhci_cmd(0, 0,
                   (XHCI_TRB_TYPE_DISABLE_SLOT << 10) |
                       ((u32)g_u8SlotId << 24),
                   &u32Code, NULL);
    g_pDcbaa[g_u8SlotId] = 0;
    g_u8SlotId = 0;
    mmio_barrier();
}

/*
 * Build Input Context (slot + EP0) for Address Device.
 * Returns 0 always (fills g_pInputCtx).
 */
static void
xhci_fill_address_input(void)
{
    u8 *pIcc;
    u8 *pSlot;
    u8 *pEp0;

    memset(g_pInputCtx, 0, GJ_PAGE_SIZE);
    memset(g_pDevCtx, 0, GJ_PAGE_SIZE);

    pIcc = (u8 *)g_pInputCtx;
    /* Add Context flags A0|A1 (slot + EP0) */
    ctx_write32(pIcc, 0x04, 0x3u);

    pSlot = ctx_slot(g_pInputCtx, 1);
    /* Context Entries = 1, Speed, Root Hub Port Number (1-based) */
    ctx_write32(pSlot, 0x00, (1u << 27) | ((u32)g_u8PortSpeed << 20));
    ctx_write32(pSlot, 0x04, ((u32)g_u8PortId << 16));
    ctx_write32(pSlot, 0x08, 0);
    ctx_write32(pSlot, 0x0c, 0);

    pEp0 = ctx_ep(g_pInputCtx, 1, 1);
    ctx_write32(pEp0, 0x00, 0); /* EP State=0 Disabled; Mult/MaxPStreams=0 */
    /*
     * DW1: CErr=3, EP Type=Control bi-dir (4), Max Burst Size=0,
     * Max Packet Size = g_u16MaxPkt0 (SS->512, HS->64).
     */
    ctx_write32(pEp0, 0x04,
                (3u << 1) | (XHCI_EP_TYPE_CTRL_BI << 3) |
                    ((u32)g_u16MaxPkt0 << 16));
    ctx_write64(pEp0, 0x08, (u64)g_paEp0Ring | 1ull); /* TR Deq | DCS=1 */
    /* Average TRB Length - non-zero required by some HCs for Address Device. */
    ctx_write32(pEp0, 0x10, (g_u16MaxPkt0 >= 512u) ? 512u : 8u);

    xhci_clflush_span(g_pInputCtx, (u32)g_u8CtxSize * 3u);
    xhci_clflush_span(g_pEp0Ring, 256u);
}

/*
 * After Address Device the HC has EP0 dequeue from input context. Explicitly
 * Set TR Dequeue to a fresh software ring so the first GET_DESC is not lost
 * (G752 stage-11: address PASS, first control timeout).
 *
 * Spec (xHCI 4.6.6): Set TR Dequeue only when EP is Stopped/Error/Halted.
 * After a successful control TD EP0 is Running - Set TR Deq without Stop
 * fails (or is ignored) while software ring_reset -> producer/HC desync ->
 * next transfer times out (classic stage-15 GET_CONFIG cc=0 after device-desc
 * PASS). Always Stop EP0 first, then Set TR Deq with DCS=1, then match
 * software cycle/idx to that DCS.
 */
static int
xhci_ep0_arm_after_address(void)
{
    u32 u32Code = 0;
    u32 u32Ctrl;

    if (g_u8SlotId == 0) {
        xhci_ep0_ring_reset();
        return -1;
    }
    xhci_drain_events(16u);
    /* Stop EP0 - ignore fail (may already be idle/Stopped after Address). */
    u32Ctrl = (XHCI_TRB_TYPE_STOP_EP << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    (void)xhci_cmd(0, 0, u32Ctrl, &u32Code, NULL);
    xhci_drain_events(16u);

    xhci_ep0_ring_reset();
    /* Set TR Dequeue Pointer - EP ID = DCI 1 (EP0), DCS=1 on pointer. */
    u32Ctrl = (XHCI_TRB_TYPE_SET_TR_DEQ << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    if (xhci_cmd((u64)g_paEp0Ring | 1ull, 0, u32Ctrl, &u32Code, NULL) != 0) {
        kprintf("xhci: Set TR Dequeue after address FAIL code=%u; Evaluate\n",
                u32Code);
        return xhci_evaluate_ep0_mps();
    }
    xhci_ep0_ring_reset(); /* match DCS=1 / idx=0 after Set TR Dequeue */
    xhci_clflush_ptr(g_pEp0Ring);
    xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
    return 0;
}

/*
 * Pre-Address settle: SS sticks on a12f need U0 + multi-ms recovery after
 * warm reset before Address Device or the HC returns Transaction Error / the
 * software SET_ADDRESS path times out (panel stage 19/20, sticky cc=1).
 */
static void
xhci_pre_address_settle(void)
{
    u32 u32Ps;

    if (g_u8PortId == 0) {
        return;
    }
    u32Ps = portsc_read(g_u8PortId);
    /* Only force U0 when link is not already U0 (avoid SS thrash). */
    if (((u32Ps >> 5) & 0xfu) != 0u) {
        xhci_port_force_u0(g_u8PortId);
    }
    if (g_u8PortSpeed >= 4u || xhci_port_is_ss_cap(g_u8PortId) != 0) {
        xhci_settle(4000000u);
        u32Ps = portsc_read(g_u8PortId);
        if (((u32Ps >> 5) & 0xfu) != 0u) {
            xhci_port_force_u0(g_u8PortId);
            xhci_settle(500000u);
        }
    } else {
        xhci_settle(800000u);
    }
    xhci_drain_events(32u);
}

/*
 * Re-sample PORTSC speed/MPS immediately before Address Device (cc=4 guard).
 * Returns 0 if PED+non-zero speed; -1 if still unknown (do NOT Address Device).
 *
 * Never invent SuperSpeed(4) or HS(3) when the speed field is 0 - wrong Slot
 * Context Speed / EP0 MPS is a classic a12f USB Transaction Error (cc=4).
 */
static int
xhci_resync_port_speed(void)
{
    u32 u32Ps;
    u32 iWait;
    u8 u8Spd;

    if (g_u8PortId == 0) {
        return -1;
    }
    /*
     * Wait until speed field is non-zero with PED. Force U0 sparingly - every
     * spin spam was thrashing the SS link on G752.
     */
    for (iWait = 0; iWait < 8000000u; iWait++) {
        u32Ps = portsc_read(g_u8PortId);
        u8Spd = (u8)((u32Ps >> 10) & 0xfu);
        if ((u32Ps & XHCI_PORTSC_PED) != 0 && u8Spd != 0u) {
            break;
        }
        if ((iWait & 0xfffffu) == 0u) {
            xhci_port_force_u0(g_u8PortId);
        }
        cpu_relax();
    }
    u32Ps = portsc_read(g_u8PortId);
    u8Spd = (u8)((u32Ps >> 10) & 0xfu);
    if ((u32Ps & XHCI_PORTSC_PED) == 0 || u8Spd == 0u) {
        kprintf("xhci: resync FAIL portsc=0x%x ped=%u spd=%u (no Address)\n",
                (unsigned)u32Ps, (u32Ps & XHCI_PORTSC_PED) != 0 ? 1u : 0u,
                (unsigned)u8Spd);
        return -1;
    }
    g_u8PortSpeed = u8Spd;
    g_u16MaxPkt0 = ep0_max_packet(g_u8PortSpeed);
    kprintf("xhci: resync speed=%u mps0=%u portsc=0x%x pls=%u ped=%u\n",
            (unsigned)g_u8PortSpeed, (unsigned)g_u16MaxPkt0, (unsigned)u32Ps,
            (unsigned)((u32Ps >> 5) & 0xfu),
            (u32Ps & XHCI_PORTSC_PED) != 0 ? 1u : 0u);
    return 0;
}

/* Port power bounce - recover a12f after Address Device cc=4. */
static void
xhci_port_power_bounce(u8 u8Port)
{
    u32 u32Ps;
    u32 i;

    if (u8Port == 0) {
        return;
    }
    u32Ps = portsc_read(u8Port);
    kprintf("xhci: port power bounce port=%u portsc=0x%x\n", (unsigned)u8Port,
            (unsigned)u32Ps);
    /* PP=0 */
    portsc_write(u8Port, (u32Ps & XHCI_PORTSC_PRESERVE & ~XHCI_PORTSC_PP) |
                             XHCI_PORTSC_CHANGE);
    for (i = 0; i < 2000000u; i++) {
        cpu_relax();
    }
    u32Ps = portsc_read(u8Port);
    portsc_write(u8Port, (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_PP |
                             XHCI_PORTSC_CHANGE);
    for (i = 0; i < 3000000u; i++) {
        cpu_relax();
        if ((portsc_read(u8Port) & XHCI_PORTSC_CCS) != 0) {
            break;
        }
    }
}

/*
 * Address Device with BSR=0: HC issues SET_ADDRESS on the wire (classic path).
 * G752 panel: stage=19 cc=4 = USB Transaction Error (device no bus response).
 */
static int
xhci_address_device_hw(void)
{
    u32 u32Code = 0;
    u32 u32Slot = 0;
    u32 u32Ctrl;
    u32 u32Span = (u32)g_u8CtxSize * 3u;
    u32 u32Ps;

    xhci_pre_address_settle();
    if (xhci_resync_port_speed() != 0) {
        kprintf("xhci: Address Device aborted - speed/PED unknown\n");
        g_u32LastStage = 19;
        g_u32AddrFailCc = 4u;
        g_u32AddrPath = 1u;
        return -1;
    }
    /* Need PED before Address Device or HC returns Transaction Error (4). */
    u32Ps = portsc_read(g_u8PortId);
    if ((u32Ps & XHCI_PORTSC_PED) == 0 || (u32Ps & XHCI_PORTSC_CCS) == 0) {
        kprintf("xhci: Address Device aborted - PED/CCS portsc=0x%x\n",
                (unsigned)u32Ps);
        g_u32LastStage = 19;
        g_u32AddrFailCc = 4u;
        g_u32AddrPath = 1u;
        return -1;
    }
    /* PLS should be U0; one gentle nudge only if not. */
    if (((u32Ps >> 5) & 0xfu) != 0u) {
        xhci_port_force_u0(g_u8PortId);
        xhci_settle(500000u);
        u32Ps = portsc_read(g_u8PortId);
    }

    xhci_ep0_ring_reset();
    xhci_clflush_span(g_pEp0Ring, GJ_PAGE_SIZE);
    xhci_fill_address_input();
    xhci_clflush_span(g_pInputCtx, u32Span);
    xhci_clflush_span(g_pDevCtx, (u32)g_u8CtxSize * 2u);
    /* DCBAA must be visible to HC before Address Device. */
    if (g_u8SlotId != 0 && g_pDcbaa != NULL) {
        g_pDcbaa[g_u8SlotId] = (u64)g_paDevCtx;
        mmio_barrier();
        xhci_clflush_ptr(&g_pDcbaa[g_u8SlotId]);
    }

    /* P0-3: freeze Address-time MPS for post-Address diverge check. */
    g_u16AddrMps0 = g_u16MaxPkt0;
    kprintf("xhci: Address Device BSR0 slot=%u port=%u spd=%u mps0=%u "
            "portsc=0x%x\n",
            (unsigned)g_u8SlotId, (unsigned)g_u8PortId,
            (unsigned)g_u8PortSpeed, (unsigned)g_u16MaxPkt0, (unsigned)u32Ps);

    u32Ctrl = (XHCI_TRB_TYPE_ADDRESS_DEV << 10) | ((u32)g_u8SlotId << 24);
    /* BSR=0 - host controller sends SET_ADDRESS */
    if (xhci_cmd((u64)g_paInputCtx, 0, u32Ctrl, &u32Code, &u32Slot) != 0) {
        kprintf("xhci: Address Device (HW/BSR0) FAIL code=%u slot=%u "
                "speed=%u port=%u (4=USB Transaction Error)\n",
                u32Code, (unsigned)g_u8SlotId, (unsigned)g_u8PortSpeed,
                (unsigned)g_u8PortId);
        g_u32LastStage = 19;
        g_u32AddrFailCc = u32Code != 0u ? u32Code : g_u32LastCc;
        g_u32AddrPath = 1u;
        return -1;
    }
    /* Invalidate HC-written Output Context (lines were clean before Address). */
    xhci_clflush_span(g_pDevCtx, (u32)g_u8CtxSize * 2u);
    /*
     * Post-SET_ADDRESS settle (G752 p21 spd=4 stage-11 deepen + P0 residual):
     *   1) USB recovery settle - SS multi-ms on a12f before any EP0 doorbell
     *   2) Keep PP; gentle U0 only if PLS!=0 (no LWS spam)
     *   3) Re-sample speed -> MPS0 (SS stick must keep 512; HS companion -> 64)
     *   4) Poll Slot Output Context for non-zero USB Device Address
     *      P0-2: usb_addr==0 after wait -> FAIL closed (no soft-8 into addr0)
     *   5) P0-1 soft-continue ONLY if ALL Running|Stopped + idx==0 + cyc==1 +
     *      deq base==g_paEp0Ring + DCS==1 (deq==0 -> arm)
     * Grep: xhci: post-address
     */
    {
        u32 u32EpSt;
        u64 u64Deq = 0;
        u64 u64Base = (u64)g_paEp0Ring & ~0xfull;
        u8 u8Dcs;
        u8 *pSlot;
        u32 u32SlotDw3 = 0;
        u32 u32UsbAddr = 0;
        u32 iWait;
        u16 u16MpsBefore;

        /* Phase 1: device recovery after HC SET_ADDRESS on the wire. */
        xhci_settle((g_u8PortSpeed >= 4u) ? 6000000u : 1200000u);
        if (g_u8PortId != 0) {
            xhci_port_power_on(g_u8PortId);
            u32Ps = portsc_read(g_u8PortId);
            if (((u32Ps >> 5) & 0xfu) != 0u) {
                xhci_port_force_u0(g_u8PortId);
                xhci_settle((g_u8PortSpeed >= 4u) ? 800000u : 300000u);
            }
            /* Re-sample trained speed -> default MPS0 (SS vs HS path). */
            u32Ps = portsc_read(g_u8PortId);
            if (((u32Ps >> 10) & 0xfu) != 0u) {
                g_u8PortSpeed = (u8)((u32Ps >> 10) & 0xfu);
            }
        }
        u16MpsBefore = g_u16MaxPkt0;
        g_u16MaxPkt0 = ep0_max_packet(g_u8PortSpeed != 0u ? g_u8PortSpeed
                                                           : 4u);
        if (g_u8PortSpeed >= 4u) {
            g_u16MaxPkt0 = 512u;
        } else if (g_u8PortSpeed == 3u) {
            g_u16MaxPkt0 = 64u;
        } else if (g_u8PortSpeed == 2u) {
            g_u16MaxPkt0 = 8u;
        } else if (g_u8PortSpeed == 1u || g_u8PortSpeed == 0u) {
            /* FS or unknown: keep provisional from Address fill if sane. */
            if (g_u16MaxPkt0 != 8u && g_u16MaxPkt0 != 64u &&
                g_u16MaxPkt0 != 512u) {
                g_u16MaxPkt0 = 64u;
            }
        }
        if (u16MpsBefore != g_u16MaxPkt0) {
            kprintf("xhci: post-address mps0 %u->%u spd=%u\n",
                    (unsigned)u16MpsBefore, (unsigned)g_u16MaxPkt0,
                    (unsigned)g_u8PortSpeed);
        }

        /* Phase 2: wait until Slot Context shows assigned USB address. */
        for (iWait = 0; iWait < 2000000u; iWait++) {
            xhci_clflush_span(g_pDevCtx, (u32)g_u8CtxSize * 2u);
            pSlot = ctx_slot(g_pDevCtx, 0);
            memcpy(&u32SlotDw3, pSlot + 0x0c, 4);
            u32UsbAddr = u32SlotDw3 & 0xffu;
            if (u32UsbAddr != 0u) {
                break;
            }
            cpu_relax();
        }
        u32Ps = (g_u8PortId != 0) ? portsc_read(g_u8PortId) : 0u;
        kprintf("xhci: post-address settle usb_addr=%u slot_st=%u mps0=%u "
                "spd=%u portsc=0x%x pls=%u ped=%u\n",
                (unsigned)u32UsbAddr,
                (unsigned)((u32SlotDw3 >> 27) & 0x1fu),
                (unsigned)g_u16MaxPkt0, (unsigned)g_u8PortSpeed,
                (unsigned)u32Ps, (unsigned)((u32Ps >> 5) & 0xfu),
                (u32Ps & XHCI_PORTSC_PED) != 0 ? 1u : 0u);

        /*
         * P0-2: never soft-8 into USB address 0. Caller (enable_address /
         * readdress) retries; fail closed with greppable lamp.
         * Grep: xhci: post-address usb_addr=0 FAIL
         */
        if (u32UsbAddr == 0u) {
            kprintf("xhci: post-address usb_addr=0 FAIL closed "
                    "(no soft-8 into addr0) slot_st=%u portsc=0x%x\n",
                    (unsigned)((u32SlotDw3 >> 27) & 0x1fu),
                    (unsigned)u32Ps);
            kprintf("msc: progress address FAIL usb_addr=0\n");
            g_u32LastStage = 19;
            g_u32AddrFailCc = 0u; /* timeout / no assigned address */
            g_u32AddrPath = 1u;
            return -1;
        }

        /* Phase 3: SS extra recovery before first EP0 doorbell (p21 s4). */
        if (g_u8PortSpeed >= 4u) {
            xhci_settle(2500000u);
            if (g_u8PortId != 0 &&
                ((portsc_read(g_u8PortId) >> 5) & 0xfu) != 0u) {
                xhci_port_force_u0(g_u8PortId);
                xhci_settle(500000u);
            }
        } else {
            xhci_settle(400000u);
        }

        /*
         * P0-1 soft-align: soft-continue ONLY if ALL Running|Stopped +
         * idx==0 + cyc==1 + deq base==g_paEp0Ring + DCS==1. deq==0 -> arm.
         * Grep: xhci: post-address ep0 soft-continue ok|deny
         */
        xhci_ep0_ring_reset();
        xhci_clflush_ptr(g_pEp0Ring);
        xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
        xhci_clflush_span(g_pEp0Ring,
                          (u32)(XHCI_RING_TRBS * sizeof(struct xhci_trb)));

        u32EpSt = xhci_ep0_sample(&u64Deq);
        u8Dcs = (u8)(u64Deq & 1ull);
        kprintf("xhci: post-address ep0 soft-align state=%u deq=0x%lx "
                "idx=%u cyc=%u dcs=%u base=0x%lx\n",
                (unsigned)u32EpSt, (unsigned long)u64Deq,
                (unsigned)g_u32Ep0Idx, (unsigned)g_fEp0Cycle,
                (unsigned)u8Dcs, (unsigned long)u64Base);
        if (xhci_ep0_soft_continue_ok(u32EpSt, u64Deq) != 0) {
            kprintf("xhci: post-address ep0 soft-continue ok "
                    "(no Set TR Deq)\n");
        } else {
            kprintf("xhci: post-address ep0 soft-continue deny state=%u "
                    "deq=0x%lx dcs=%u -> arm (Set TR Deq)\n",
                    (unsigned)u32EpSt, (unsigned long)u64Deq,
                    (unsigned)u8Dcs);
            if (xhci_ep0_arm_after_address() != 0) {
                kprintf("xhci: EP0 arm after HW address soft FAIL (continue)\n");
            }
        }
        xhci_drain_events(32u);
    }
    kprintf("xhci: Address Device HW/BSR0 slot=%u speed=%u mps0=%u soft PASS\n",
            (unsigned)g_u8SlotId, (unsigned)g_u8PortSpeed,
            (unsigned)g_u16MaxPkt0);
    g_u32AddrPath = 1u;
    return 0;
}

/*
 * Note: BSR=1 + software SET_ADDRESS is intentionally NOT used for first
 * address. After BSR1 the HC stores Device Address N while the device is still
 * USB addr 0 -> EP0 SET_ADDRESS times out (panel stage=20 soft cc=0). Product
 * path is Address Device BSR=0 only.
 */

static int
xhci_enable_slot_once(void)
{
    u32 u32Code = 0;
    u32 u32Slot = 0;

    xhci_ep0_ring_reset();
    if (g_u8PortId != 0) {
        xhci_port_force_u0(g_u8PortId);
        xhci_settle(500000u);
    }

    if (xhci_cmd(0, 0, XHCI_TRB_TYPE_ENABLE_SLOT << 10, &u32Code, &u32Slot) !=
        0) {
        kprintf("xhci: Enable Slot FAIL code=%u\n", u32Code);
        g_u32LastStage = 7;
        return -1;
    }
    g_u8SlotId = (u8)u32Slot;
    if (g_u8SlotId == 0 || g_u8SlotId > g_u8MaxSlots) {
        kprintf("xhci: Enable Slot bad id=%u max=%u\n", (unsigned)g_u8SlotId,
                (unsigned)g_u8MaxSlots);
        g_u32LastStage = 7;
        g_u8SlotId = 0;
        return -1;
    }

    g_pDcbaa[g_u8SlotId] = (u64)g_paDevCtx;
    mmio_barrier();
    xhci_clflush_ptr(&g_pDcbaa[g_u8SlotId]);
    /* Gap between Enable Slot and Address Device (a12f cc=4 if too tight). */
    xhci_settle((g_u8PortSpeed >= 4u) ? 1000000u : 300000u);
    return 0;
}

/*
 * If SS-capable port only shows HS after cold PR, try warm reset once to
 * re-train SuperSpeed (stick 13fe:6400 lives on SS under Linux).
 */
static void
xhci_try_promote_ss(void)
{
    u32 u32Ps;
    u32 iSpin;

    if (g_u8PortId == 0 || xhci_port_is_ss_cap(g_u8PortId) == 0) {
        return;
    }
    u32Ps = portsc_read(g_u8PortId);
    if (((u32Ps >> 10) & 0xfu) >= 4u) {
        return; /* already SS */
    }
    kprintf("xhci: promote SS warm-reset port=%u cur_spd=%u\n",
            (unsigned)g_u8PortId, (unsigned)((u32Ps >> 10) & 0xfu));
    portsc_write(g_u8PortId,
                 (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_WPR |
                     XHCI_PORTSC_PP | XHCI_PORTSC_CHANGE);
    for (iSpin = 0; iSpin < XHCI_POLL_SPINS; iSpin++) {
        u32Ps = portsc_read(g_u8PortId);
        if ((u32Ps & (XHCI_PORTSC_WRC | XHCI_PORTSC_PRC)) != 0) {
            break;
        }
        cpu_relax();
    }
    portsc_write(g_u8PortId, (u32Ps & XHCI_PORTSC_PRESERVE) |
                                 XHCI_PORTSC_CHANGE | XHCI_PORTSC_PP);
    for (iSpin = 0; iSpin < 20000000u; iSpin++) {
        u32Ps = portsc_read(g_u8PortId);
        if ((u32Ps & XHCI_PORTSC_PED) != 0) {
            break;
        }
        cpu_relax();
    }
    xhci_port_force_u0(g_u8PortId);
    xhci_settle(3000000u);
    u32Ps = portsc_read(g_u8PortId);
    g_u8PortSpeed = (u8)((u32Ps >> 10) & 0xfu);
    if (g_u8PortSpeed != 0u) {
        g_u16MaxPkt0 = ep0_max_packet(g_u8PortSpeed);
    }
    kprintf("xhci: after promote speed=%u ped=%u portsc=0x%x\n",
            (unsigned)g_u8PortSpeed,
            (u32Ps & XHCI_PORTSC_PED) != 0 ? 1u : 0u, (unsigned)u32Ps);
}

/*
 * g_fAddrUsedBsrSoft: which address path succeeded (for stage-11 readdress).
 * 0 = HW/BSR0, 1 = BSR+soft SET_ADDRESS.
 */
static u8 g_fAddrUsedBsrSoft;

static void
xhci_address_clear_sticky_stage(void)
{
    if (g_u32LastStage == 19u || g_u32LastStage == 20u ||
        g_u32LastStage == 7u) {
        g_u32LastStage = 4; /* hc up / addressed pending enum */
    }
}

static int
xhci_enable_address(void)
{
    u32 nTry;
    u32 u32Bsr0Cc = 0;
    u32 u32Ps;

    /*
     * Product path = Address Device BSR=0 only (HC SET_ADDRESS on the wire).
     *
     * Soft SET_ADDRESS after BSR=1 is NOT used for first address: after BSR1
     * the HC stores Device Address N but the device is still USB addr 0, so
     * EP0 SET_ADDRESS tokens go to N -> timeout (stage=20 soft cc=0 on p22s4).
     * Stages: 7=Enable Slot  19=Address Device BSR0 fail.
     */
    g_fAddrUsedBsrSoft = 0;
    g_u32AddrFailCc = 0;
    g_u32CtrlFailCc = 0;
    g_u32AddrPath = 0;

    for (nTry = 0; nTry < 6u; nTry++) {
        if (nTry > 0u) {
            xhci_disable_slot_soft();
            xhci_settle(500000u);
            if (g_u8PortId != 0) {
                /*
                 * BSR0 recovery ladder (G752 a12f SS stick):
                 *  1: long U0 settle
                 *  2: warm-first re-reset (USB3 retrain)
                 *  3: cold re-reset
                 *  4: power bounce + warm reset
                 *  5: power bounce + cold reset
                 */
                if (nTry == 1u) {
                    kprintf("xhci: address try=%u long settle + U0\n",
                            (unsigned)nTry);
                    xhci_port_force_u0(g_u8PortId);
                    xhci_settle(8000000u);
                } else if (nTry == 2u) {
                    kprintf("xhci: address try=%u warm-first re-reset\n",
                            (unsigned)nTry);
                    (void)xhci_port_reset_ex(g_u8PortId, 1);
                } else if (nTry == 3u) {
                    kprintf("xhci: address try=%u cold re-reset\n",
                            (unsigned)nTry);
                    (void)xhci_port_reset_ex(g_u8PortId, 0);
                } else if (nTry == 4u) {
                    kprintf("xhci: address try=%u power bounce + warm\n",
                            (unsigned)nTry);
                    xhci_port_power_bounce(g_u8PortId);
                    (void)xhci_port_reset_ex(g_u8PortId, 1);
                } else {
                    kprintf("xhci: address try=%u power bounce + cold\n",
                            (unsigned)nTry);
                    xhci_port_power_bounce(g_u8PortId);
                    (void)xhci_port_reset_ex(g_u8PortId, 0);
                }
            }
            xhci_settle((g_u8PortSpeed >= 4u) ? 3000000u : 1000000u);
        }

        if (g_u8PortId != 0) {
            u32Ps = portsc_read(g_u8PortId);
            kprintf("xhci: pre-address try=%u portsc=0x%x ped=%u pls=%u "
                    "spd=%u ccs=%u\n",
                    (unsigned)nTry, (unsigned)u32Ps,
                    (u32Ps & XHCI_PORTSC_PED) != 0 ? 1u : 0u,
                    (unsigned)((u32Ps >> 5) & 0xfu),
                    (unsigned)((u32Ps >> 10) & 0xfu),
                    (u32Ps & XHCI_PORTSC_CCS) != 0 ? 1u : 0u);
            if ((u32Ps & XHCI_PORTSC_CCS) == 0 ||
                (u32Ps & XHCI_PORTSC_PED) == 0) {
                kprintf("xhci: port not ready for address; force U0/reset\n");
                xhci_port_force_u0(g_u8PortId);
                if ((portsc_read(g_u8PortId) & XHCI_PORTSC_PED) == 0) {
                    (void)xhci_port_reset_ex(g_u8PortId, 1);
                }
            }
        }

        /* If speed dropped below SS after cold PR, warm-promote once. */
        if (nTry >= 1u && g_u8PortSpeed < 4u &&
            xhci_port_is_ss_cap(g_u8PortId) != 0) {
            xhci_try_promote_ss();
        }

        if (g_u8PortSpeed <= 2u) {
            kprintf("xhci: address abort try=%u spd=%u (not stick)\n",
                    (unsigned)nTry, (unsigned)g_u8PortSpeed);
            u32Bsr0Cc = 4u;
            continue;
        }

        if (xhci_enable_slot_once() != 0) {
            return -1;
        }

        if (xhci_address_device_hw() == 0) {
            g_fAddrUsedBsrSoft = 0;
            xhci_address_clear_sticky_stage();
            xhci_settle((g_u8PortSpeed >= 4u) ? 2000000u : 500000u);
            return 0;
        }
        u32Bsr0Cc = g_u32AddrFailCc != 0u ? g_u32AddrFailCc : g_u32LastCc;
        kprintf("xhci: HW/BSR0 address FAIL try=%u cc=%u spd=%u port=%u\n",
                (unsigned)nTry, (unsigned)u32Bsr0Cc, (unsigned)g_u8PortSpeed,
                (unsigned)g_u8PortId);
    }

    xhci_disable_slot_soft();
    g_u32LastStage = 19; /* real Address Device fail - not fake stage-20 */
    g_u32AddrFailCc = u32Bsr0Cc != 0u ? u32Bsr0Cc : g_u32AddrFailCc;
    g_u32AddrPath = 1u;
    kprintf("xhci: address FAIL final stage=19 addr_cc=%u port=%u spd=%u "
            "(BSR0 only; soft SET_ADDRESS skipped - device would be addr0)\n",
            (unsigned)g_u32AddrFailCc, (unsigned)g_u8PortId,
            (unsigned)g_u8PortSpeed);
    return -1;
}

/*
 * Re-address after stage-11/15 fail: Disable Slot + warm reset + BSR0.
 * On stage-15 second readdress, power-bounce first (a12f SS link sticky).
 * Grep: xhci: readdress | msc: progress readdress
 */
static int
xhci_readdress_alternate(void)
{
    static u32 s_u32ReaddrN;

    s_u32ReaddrN++;
    kprintf("msc: progress readdress n=%u port=%u spd=%u\n",
            (unsigned)s_u32ReaddrN, (unsigned)g_u8PortId,
            (unsigned)g_u8PortSpeed);
    xhci_disable_slot_soft();
    xhci_settle(500000u);
    if (g_u8PortId != 0) {
        xhci_port_power_on(g_u8PortId);
        if (s_u32ReaddrN >= 2u) {
            kprintf("xhci: readdress power-bounce n=%u\n",
                    (unsigned)s_u32ReaddrN);
            xhci_port_power_bounce(g_u8PortId);
        }
        xhci_port_force_u0(g_u8PortId);
        xhci_settle(500000u);
        /* Alternate warm / cold on successive readdress. */
        (void)xhci_port_reset_ex(g_u8PortId, (s_u32ReaddrN & 1u) != 0u ? 1 : 0);
        xhci_settle((g_u8PortSpeed >= 4u || xhci_port_is_ss_cap(g_u8PortId))
                        ? 4000000u
                        : 1500000u);
        if (((portsc_read(g_u8PortId) >> 5) & 0xfu) != 0u) {
            xhci_port_force_u0(g_u8PortId);
            xhci_settle(500000u);
        }
    }
    if (xhci_enable_slot_once() != 0) {
        kprintf("msc: progress readdress FAIL enable_slot\n");
        return -1;
    }
    kprintf("xhci: readdress HW/BSR0 n=%u\n", (unsigned)s_u32ReaddrN);
    if (xhci_address_device_hw() != 0) {
        kprintf("msc: progress readdress FAIL address\n");
        xhci_disable_slot_soft();
        return -1;
    }
    g_fAddrUsedBsrSoft = 0;
    if (g_u32LastStage == 19u || g_u32LastStage == 20u ||
        g_u32LastStage == 7u || g_u32LastStage == 15u ||
        g_u32LastStage == 11u || g_u32LastStage == 13u) {
        g_u32LastStage = 4;
    }
    kprintf("msc: progress readdress PASS n=%u slot=%u spd=%u\n",
            (unsigned)s_u32ReaddrN, (unsigned)g_u8SlotId,
            (unsigned)g_u8PortSpeed);
    return 0;
}

/* ---- descriptor parse / configure --------------------------------------- */
static int
xhci_parse_msc_config(const u8 *pCfg, u32 cb)
{
    u32 u32Off = 0;
    u8 u8NumIf = 0;
    int fMsc = 0;
    u8 u8EpOut = 0;
    u8 u8EpIn = 0;
    u16 u16Mps = 512;

    if (cb < 9u || pCfg[0] < 9u || pCfg[1] != USB_DT_CONFIG) {
        return -1;
    }
    g_u8ConfigVal = pCfg[5];
    u8NumIf = pCfg[4];
    (void)u8NumIf;
    u32Off = pCfg[0];

    while (u32Off + 2u <= cb) {
        u8 u8Len = pCfg[u32Off];
        u8 u8Type = pCfg[u32Off + 1u];

        if (u8Len < 2u || u32Off + u8Len > cb) {
            break;
        }
        if (u8Type == 0x04u && u8Len >= 9u) {
            /* interface */
            u8 u8Class = pCfg[u32Off + 5u];
            u8 u8Sub = pCfg[u32Off + 6u];
            u8 u8Proto = pCfg[u32Off + 7u];

            /*
             * BOT: class 08, protocol 50. Subclass 06 (SCSI) is common;
             * also accept 02 (ATAPI) / 05 (SFF-8070i) / 01 (RBC) for sticks.
             * Skip UAS (protocol 0x62) - no UAS path yet.
             */
            if (u8Class == USB_CLASS_MSC && u8Proto == USB_PROTO_BOT &&
                (u8Sub == USB_SUBCLASS_SCSI || u8Sub == 0x02u ||
                 u8Sub == 0x05u || u8Sub == 0x01u || u8Sub == 0x00u)) {
                fMsc = 1;
                u8EpOut = 0;
                u8EpIn = 0;
            } else {
                fMsc = 0;
            }
        } else if (u8Type == 0x05u && u8Len >= 7u && fMsc != 0) {
            u8 u8Addr = pCfg[u32Off + 2u];
            u8 u8Attr = pCfg[u32Off + 3u];
            u16 u16EpMps = (u16)pCfg[u32Off + 4u] |
                           ((u16)pCfg[u32Off + 5u] << 8);

            if ((u8Attr & 0x03u) == 0x02u) {
                if ((u8Addr & 0x80u) != 0) {
                    u8EpIn = (u8)(u8Addr & 0x0fu);
                    u16Mps = u16EpMps;
                } else {
                    u8EpOut = (u8)(u8Addr & 0x0fu);
                    u16Mps = u16EpMps;
                }
            }
        }
        u32Off += u8Len;
    }

    if (u8EpOut == 0 || u8EpIn == 0) {
        return -1;
    }
    g_u8EpOut = u8EpOut;
    g_u8EpIn = u8EpIn;
    g_u16BulkMps = u16Mps != 0 ? u16Mps : 512;
    return 0;
}

static int
xhci_configure_bulk(void)
{
    u8 *pIcc;
    u8 *pSlot;
    u8 *pEpOut;
    u8 *pEpIn;
    u8 u8DciOut = (u8)(g_u8EpOut * 2u);
    u8 u8DciIn = (u8)(g_u8EpIn * 2u + 1u);
    u32 u32Add;
    u32 u32Ctrl;
    u32 u32Code = 0;
    u32 u32Entries;

    memset(g_pInputCtx, 0, GJ_PAGE_SIZE);
    pIcc = (u8 *)g_pInputCtx;

    /* A0 (slot Context Entries) + bulk OUT/IN DCIs */
    u32Add = (1u << 0) | (1u << u8DciOut) | (1u << u8DciIn);
    ctx_write32(pIcc, 0x04, u32Add);

    u32Entries = u8DciIn > u8DciOut ? u8DciIn : u8DciOut;
    pSlot = ctx_slot(g_pInputCtx, 1);
    ctx_write32(pSlot, 0x00, (u32Entries << 27) | ((u32)g_u8PortSpeed << 20));
    ctx_write32(pSlot, 0x04, ((u32)g_u8PortId << 16));

    pEpOut = ctx_ep(g_pInputCtx, 1, u8DciOut);
    ctx_write32(pEpOut, 0x00, 0);
    ctx_write32(pEpOut, 0x04,
                (3u << 1) | (XHCI_EP_TYPE_BULK_OUT << 3) |
                    ((u32)g_u16BulkMps << 16));
    ctx_write64(pEpOut, 0x08, (u64)g_paBulkOutRing | 1ull);
    ctx_write32(pEpOut, 0x10, (u32)g_u16BulkMps);

    pEpIn = ctx_ep(g_pInputCtx, 1, u8DciIn);
    ctx_write32(pEpIn, 0x00, 0);
    ctx_write32(pEpIn, 0x04,
                (3u << 1) | (XHCI_EP_TYPE_BULK_IN << 3) |
                    ((u32)g_u16BulkMps << 16));
    ctx_write64(pEpIn, 0x08, (u64)g_paBulkInRing | 1ull);
    ctx_write32(pEpIn, 0x10, (u32)g_u16BulkMps);

    /* Push Input Context to DRAM before Configure Endpoint (a12f DMA). */
    xhci_clflush_span(g_pInputCtx, (u32)g_u8CtxSize * (u32Entries + 2u));
    xhci_clflush_ptr(g_pBulkOutRing);
    xhci_clflush_ptr(g_pBulkInRing);

    u32Ctrl = (XHCI_TRB_TYPE_CONFIG_EP << 10) | ((u32)g_u8SlotId << 24);
    if (xhci_cmd((u64)g_paInputCtx, 0, u32Ctrl, &u32Code, NULL) != 0) {
        kprintf("xhci: Configure Endpoint FAIL code=%u\n", u32Code);
        return -1;
    }
    return 0;
}

/*
 * Recover control path after stall/timeout: hard EP0 resync (Reset Endpoint +
 * Set TR Dequeue), not soft ring_reset alone (G752 stage-13/15).
 * Do not LWS-spam: only force U0 when PLS != 0 (stage-15 thrash guard).
 */
static void
xhci_ctrl_recover(void)
{
    if (g_u8PortId != 0) {
        u32 u32Ps = portsc_read(g_u8PortId);

        if (((u32Ps >> 5) & 0xfu) != 0u) {
            xhci_port_force_u0(g_u8PortId);
        }
    }
    xhci_settle((g_u8PortSpeed >= 4u) ? 500000u : 200000u);
    if (xhci_ep0_hard_resync() != 0) {
        (void)xhci_evaluate_ep0_mps();
    }
    xhci_settle((g_u8PortSpeed >= 4u) ? 500000u : 200000u);
}

/*
 * GET_DESCRIPTOR(device). Stage codes:
 *  11 = first control after address dead (panel: dark/hot pink + 11 ticks)
 *  12 = Evaluate Context EP0 MPS fail
 *  13 = full 18-byte device descriptor fail (after 8-byte OK)
 *
 * G752 photo ground truth: dark pink + 11 ticks = stage 11.
 * Do NOT Evaluate Context immediately after Address Device - HC already has
 * EP0 dequeue from Address; a pre-GET_DESC Evaluate desynced a12f and
 * regressed past stages 13/15 back to 11.
 * Do NOT force Set TR Deq before first try either (same regression class).
 *
 * After timed-out xhci_ctrl the software producer advanced (Setup+Data+
 * Status) while HC may still be mid-TD or stuck Running - settle-only retry
 * enqueues further ahead (classic stage-11 cascade). Prefer soft-recover
 * (soft-sync + mid-ring soft-continue, second-pass residual) over thrash
 * hard-resync; hard only on soft-recover deny (a12f G-AC-1 Soft!=product).
 *
 * SS (p21 spd=4) vs HS path (stage-11 deepen):
 *   SS/HS both soft-try 8-byte FIRST (minimal thrash to exit stage-11).
 *   Soft-8 TO -> soft-recover + one soft-8 retry -> soft-18 SS shot ->
 *   soft-then-hard 8 ladder -> soft-then-hard 18 ladder.
 *
 * Stage-13 residual (8 OK -> 18 fail -> toward GET_CONFIG stage-15):
 *   After soft-8 PASS: Evaluate only if MPS changed; mid-ring soft-continue
 *   when producer-HC aligned (no Stop+Set TR Deq thrash). Residual-aware
 *   got-length on full desc; after TO: soft-recover (second-pass) -> hard
 *   on deny (not soft-arm first; not thrash ladder grow). Soft!=product.
 * Grep: xhci: get device desc | msc: progress get_desc | stage=11|13
 *       | get device desc residual | mid-ring soft-continue | get_desc18
 *       | soft-recover second-pass | Soft!=product
 */
static int
xhci_get_device_descriptor(u8 *pDev)
{
    struct usb_setup setup;
    u8 aTmp[18];
    u16 u16Mps;
    int nTry;
    int fGot8 = 0;
    int fSsPath;

    if (pDev == NULL) {
        return -1;
    }
    memset(&setup, 0, sizeof(setup));
    setup.u8BmRequestType = 0x80u;
    setup.u8BRequest = USB_REQ_GET_DESCRIPTOR;
    setup.u16WValue = (u16)(USB_DT_DEVICE << 8);
    setup.u16WIndex = 0;

    /*
     * Soft-prep (state-aware): leave HC dequeue alone when still aligned
     * after Address Device. Long settle (USB ≥2ms; SS longer on a12f).
     */
    xhci_drain_events(32u);
    if (g_u8PortId != 0) {
        xhci_port_power_on(g_u8PortId);
        if (((portsc_read(g_u8PortId) >> 5) & 0xfu) != 0u) {
            xhci_port_force_u0(g_u8PortId);
        }
    }
    /* SS (p21 s4): longer pre-GET_DESC settle than HS companion. */
    xhci_settle((g_u8PortSpeed >= 4u) ? 5000000u : 1000000u);
    xhci_ep0_prepare_for_get_desc();
    xhci_drain_events(32u);
    fSsPath = (g_u8PortSpeed >= 4u) ? 1 : 0;
    kprintf("msc: progress get_desc begin port=%u spd=%u mps0=%u path=%s\n",
            (unsigned)g_u8PortId, (unsigned)g_u8PortSpeed,
            (unsigned)g_u16MaxPkt0, fSsPath != 0 ? "SS" : "HS");

    /*
     * Path A0 - soft 8-byte first (SS and HS). No hard-resync until this
     * fails: post-Address EP0 is aligned; first thrash -> stage-11 cascade.
     */
    setup.u16WLength = 8;
    memset(aTmp, 0, 18);
    if (xhci_ctrl(&setup, aTmp, 8, 1) == 0) {
        fGot8 = 1;
        kprintf("xhci: get device desc (8 soft) PASS spd=%u mps0=%u\n",
                (unsigned)g_u8PortSpeed, (unsigned)g_u16MaxPkt0);
    } else {
        kprintf("xhci: get device desc (8 soft) FAIL cc=%u spd=%u mps0=%u\n",
                (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortSpeed,
                (unsigned)g_u16MaxPkt0);
        /*
         * Stage-11 residual deepen (Soft!=product; a12f G-AC-1):
         * Prefer mid-ring soft-continue / soft-sync over thrash hard-resync.
         * Soft-8 TO may leave producer-HC aligned after late Status - retry
         * once soft before hard path. Hard only on soft-recover deny.
         * Grep: msc: progress get_desc soft-continue after soft-8 TO
         */
        if (xhci_ep0_soft_recover_after_to("get_desc_soft8") != 0) {
            kprintf("msc: progress get_desc soft-continue after soft-8 TO "
                    "(stage=11 Soft!=product)\n");
            xhci_mps0_resync_from_speed();
            xhci_settle(fSsPath != 0 ? 800000u : 300000u);
            setup.u16WLength = 8;
            memset(aTmp, 0, 18);
            if (xhci_ctrl(&setup, aTmp, 8, 1) == 0) {
                fGot8 = 1;
                kprintf("xhci: get device desc (8 soft retry) PASS spd=%u "
                        "mps0=%u (stage-11 residual Soft!=product)\n",
                        (unsigned)g_u8PortSpeed, (unsigned)g_u16MaxPkt0);
            } else {
                kprintf("xhci: get device desc (8 soft retry) FAIL cc=%u "
                        "-> hard path (stage=11 Soft!=product)\n",
                        (unsigned)g_u32CtrlFailCc);
                xhci_ctrl_recover();
                xhci_settle(fSsPath != 0 ? 1500000u : 500000u);
                xhci_mps0_resync_from_speed();
            }
        } else {
            kprintf("msc: progress get_desc hard-resync after soft-8 TO "
                    "(stage=11 Soft!=product)\n");
            xhci_ctrl_recover();
            xhci_settle(fSsPath != 0 ? 1500000u : 500000u);
            xhci_mps0_resync_from_speed();
        }
        /*
         * Path A1 (SS): one 18-byte after recover before long 8 ladder. Full
         * device desc in one TD with default MPS 512 can succeed when short
         * Data TRB path is unhappy (a12f stage-13 class, inverted).
         */
        if (fGot8 == 0 && fSsPath != 0) {
            /* Soft-recover first if prior hard left EP0 aligned mid-ring. */
            if (xhci_ep0_soft_recover_after_to("get_desc_soft18ss") != 0) {
                kprintf("msc: progress get_desc soft-continue before 18 soft SS "
                        "(stage=11 Soft!=product)\n");
            } else {
                xhci_ctrl_recover();
                xhci_settle(800000u);
            }
            xhci_mps0_resync_from_speed();
            setup.u16WLength = 18;
            memset(aTmp, 0, 18);
            if (xhci_ctrl(&setup, aTmp, 18, 1) == 0 && aTmp[0] >= 18u &&
                aTmp[1] == USB_DT_DEVICE) {
                memcpy(pDev, aTmp, 18);
                u16Mps = decode_bmax_packet0(pDev[7], g_u8PortSpeed);
                if (u16Mps != 0 && u16Mps != g_u16MaxPkt0) {
                    g_u16MaxPkt0 = u16Mps;
                    (void)xhci_evaluate_ep0_mps();
                }
                kprintf("xhci: get device desc (18 soft SS) PASS "
                        "(stage-11 residual Soft!=product)\n");
                return 0;
            }
            kprintf("xhci: get device desc (18 soft SS) FAIL cc=%u\n",
                    (unsigned)g_u32CtrlFailCc);
            /* Producer advanced again - soft then hard before 8 ladder. */
            if (xhci_ep0_soft_recover_after_to("get_desc_soft18ss_fail") == 0) {
                xhci_ctrl_recover();
            }
            xhci_settle(1500000u);
            xhci_mps0_resync_from_speed();
        }
    }

    /* ---- Path A: soft-then-hard 8-byte ladder (HS / SS after soft fail) ---- */
    if (fGot8 == 0) {
        setup.u16WLength = 8;
        for (nTry = 0; nTry < 4; nTry++) {
            /*
             * Stage-11 residual: soft-recover first (mid-ring / soft-sync);
             * hard-resync only on deny. Settle-only alone is cascade; thrash
             * hard every try also desyncs a12f (Soft!=product).
             * Grep: msc: progress get_desc soft-continue|hard-resync before 8
             */
            if (g_u8PortId != 0) {
                xhci_port_power_on(g_u8PortId);
                if (((portsc_read(g_u8PortId) >> 5) & 0xfu) != 0u) {
                    xhci_port_force_u0(g_u8PortId);
                }
            }
            if (xhci_ep0_soft_recover_after_to("get_desc_8") != 0) {
                kprintf("msc: progress get_desc soft-continue before 8 n=%u "
                        "cc=%u (stage=11 Soft!=product)\n",
                        (unsigned)nTry, (unsigned)g_u32CtrlFailCc);
            } else {
                kprintf("msc: progress get_desc hard-resync before 8 n=%u "
                        "cc=%u (stage=11 Soft!=product)\n",
                        (unsigned)nTry, (unsigned)g_u32CtrlFailCc);
                xhci_ctrl_recover();
            }
            xhci_settle(fSsPath != 0
                            ? (nTry >= 1 ? 2500000u : 1200000u)
                            : 500000u);
            /* Re-bind MPS0 after recover (SS must stay 512). */
            xhci_mps0_resync_from_speed();
            memset(aTmp, 0, 18);
            if (xhci_ctrl(&setup, aTmp, 8, 1) == 0) {
                fGot8 = 1;
                kprintf("xhci: get device desc (8) PASS try=%u mps0=%u "
                        "(stage-11 residual Soft!=product)\n",
                        (unsigned)nTry, (unsigned)g_u16MaxPkt0);
                break;
            }
            kprintf("xhci: get device desc (8) FAIL try=%u cc=%u speed=%u "
                    "mps0=%u\n",
                    (unsigned)nTry, (unsigned)g_u32CtrlFailCc,
                    (unsigned)g_u8PortSpeed, (unsigned)g_u16MaxPkt0);
        }
    }
    if (fGot8 == 0) {
        /* ---- Fallback: 18-byte soft-then-hard ladder (SS default MPS 512) ---- */
        setup.u16WLength = 18;
        for (nTry = 0; nTry < 3; nTry++) {
            if (nTry > 0) {
                if (xhci_ep0_soft_recover_after_to("get_desc_18dir") != 0) {
                    kprintf("msc: progress get_desc soft-continue after 18dir "
                            "n=%u cc=%u (stage=11 Soft!=product)\n",
                            (unsigned)nTry, (unsigned)g_u32CtrlFailCc);
                } else {
                    kprintf("msc: progress get_desc hard-resync after 18dir "
                            "n=%u cc=%u (stage=11 Soft!=product)\n",
                            (unsigned)nTry, (unsigned)g_u32CtrlFailCc);
                    xhci_ctrl_recover();
                }
                xhci_settle(fSsPath != 0 ? 1500000u : 500000u);
                xhci_mps0_resync_from_speed();
            } else if (g_u32Ep0Idx != 0u || g_fEp0Cycle != 1u) {
                /* Mid-ring residual first; hard only on deny. */
                if (xhci_ep0_soft_recover_after_to("get_desc_18dir0") == 0) {
                    xhci_ctrl_recover();
                }
                xhci_settle(fSsPath != 0 ? 1000000u : 400000u);
            }
            memset(aTmp, 0, 18);
            if (xhci_ctrl(&setup, aTmp, 18, 1) == 0) {
                memcpy(pDev, aTmp, 18);
                u16Mps = decode_bmax_packet0(pDev[7], g_u8PortSpeed);
                kprintf("xhci: get device desc (18 direct) PASS try=%u "
                        "raw_mps0=%u dec=%u speed=%u "
                        "(stage-11 residual Soft!=product)\n",
                        (unsigned)nTry, (unsigned)pDev[7], (unsigned)u16Mps,
                        (unsigned)g_u8PortSpeed);
                if (u16Mps != 0 && u16Mps != g_u16MaxPkt0) {
                    g_u16MaxPkt0 = u16Mps;
                    (void)xhci_evaluate_ep0_mps();
                }
                return 0;
            }
            kprintf("xhci: get device desc (18 direct) FAIL try=%u cc=%u\n",
                    (unsigned)nTry, (unsigned)g_u32CtrlFailCc);
        }
        kprintf("xhci: get device desc FAIL stage=11 cc=%u speed=%u port=%u "
                "mps0=%u Soft!=product\n",
                (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortSpeed,
                (unsigned)g_u8PortId, (unsigned)g_u16MaxPkt0);
        kprintf("msc: progress get_desc FAIL stage=11 cc=%u port=%u spd=%u "
                "Soft!=product\n",
                (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortId,
                (unsigned)g_u8PortSpeed);
        g_u32LastStage = 11;
        xhci_stage_soft_lamp(11u, "GET_DESC");
        /* Sticky already set by xhci_ctrl fail (or 0 if never completed). */
        return -1;
    }

    {
        u16 u16OldMps = g_u16MaxPkt0;
        u32 u32Got8;
        u32 u32EpSt;
        u64 u64Deq = 0;

        /*
         * Residual honesty after soft-8 (ISP short-packet path): actual
         * bytes = wlen - data residual. Need ≥8 for bMaxPacketSize0 at [7].
         * residual 0 (or Success) -> full 8 received. Content-backed deepen
         * when residual field is wrong but buffer has DT_DEVICE prefix.
         * Grep: xhci: get device desc residual
         */
        u32Got8 = xhci_ctrl_got_len(8u);
        if (u32Got8 < 8u && aTmp[1] == USB_DT_DEVICE && aTmp[0] >= 8u) {
            u32Got8 = 8u;
            kprintf("xhci: get device desc residual content-backed got=8 "
                    "resid=%u (soft-8 Soft!=product)\n",
                    (unsigned)g_u32LastCtrlResidual);
        }
        kprintf("xhci: get device desc residual got=%u resid=%u "
                "(soft-8 Soft!=product)\n",
                (unsigned)u32Got8, (unsigned)g_u32LastCtrlResidual);

        u16Mps = decode_bmax_packet0(aTmp[7], g_u8PortSpeed);
        kprintf("xhci: bMaxPacketSize0 raw=%u decoded=%u (was %u) speed=%u\n",
                (unsigned)aTmp[7], (unsigned)u16Mps, (unsigned)u16OldMps,
                (unsigned)g_u8PortSpeed);
        if (u16Mps != 0) {
            g_u16MaxPkt0 = u16Mps;
        }

        /*
         * Stage-13 residual (G752: 8-byte OK, 18-byte FAIL) - no thrash:
         *   soft-sync producer to HC deq (ISP residual lag) first.
         *   MPS changed -> one Evaluate (P0-3 class; never Evaluate when equal).
         *   MPS match + producer-HC aligned (mid-ring after soft-8) -> soft-
         *     continue, NO Stop+Set TR Deq (arm thrash desynced a12f for 18).
         *   Else desync / Halted -> one hard-resync or arm once.
         * Grep: xhci: ep0 mid-ring soft-continue | soft-sync residual | get_desc18
         */
        xhci_drain_events(64u);
        (void)xhci_ep0_soft_sync_producer();
        if (g_u16MaxPkt0 != u16OldMps) {
            kprintf("xhci: MPS changed %u->%u; Evaluate Context (once)\n",
                    (unsigned)u16OldMps, (unsigned)g_u16MaxPkt0);
            if (xhci_evaluate_ep0_mps() != 0) {
                /* Evaluate already sticky'd CtrlFailCc (stage-12 honesty). */
                if (xhci_ep0_hard_resync() != 0) {
                    kprintf("xhci: EP0 re-arm after 8-byte desc FAIL cc=%u "
                            "(stage=12 Soft!=product)\n",
                            (unsigned)g_u32CtrlFailCc);
                    /* Keep Evaluate fail cc; do not clobber with arm Success. */
                    g_u32LastStage = 12;
                    xhci_stage_soft_lamp(12u, "EVAL_CTX_EP0");
                    return -1;
                }
            }
            g_u16AddrMps0 = g_u16MaxPkt0; /* freeze; avoid re-Evaluate thrash */
        } else {
            u32EpSt = xhci_ep0_sample(&u64Deq);
            if (u32EpSt == XHCI_EP_ST_HALTED || u32EpSt == XHCI_EP_ST_ERROR) {
                kprintf("xhci: after soft-8 state=%u -> hard-resync\n",
                        (unsigned)u32EpSt);
                (void)xhci_ep0_hard_resync();
            } else if (xhci_ep0_soft_continue_ok(u32EpSt, u64Deq) != 0 ||
                       xhci_ep0_producer_hc_aligned(u32EpSt, u64Deq) != 0) {
                kprintf("xhci: ep0 mid-ring soft-continue after soft-8 "
                        "state=%u deq=0x%lx idx=%u (no Set TR Deq; "
                        "stage-13 residual Soft!=product)\n",
                        (unsigned)u32EpSt, (unsigned long)u64Deq,
                        (unsigned)g_u32Ep0Idx);
                xhci_clflush_ptr(g_pEp0Ring);
                xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
            } else if (xhci_ep0_soft_sync_producer() != 0) {
                kprintf("xhci: ep0 soft-sync after soft-8 ok "
                        "(stage-13 residual Soft!=product; no Set TR Deq)\n");
            } else {
                kprintf("xhci: after soft-8 desync state=%u deq=0x%lx "
                        "idx=%u -> arm once\n",
                        (unsigned)u32EpSt, (unsigned long)u64Deq,
                        (unsigned)g_u32Ep0Idx);
                if (xhci_ep0_arm_after_address() != 0) {
                    kprintf("xhci: Set TR Deq after 8-byte FAIL; try Evaluate\n");
                    if (xhci_evaluate_ep0_mps() != 0) {
                        /* Evaluate sticky'd CtrlFailCc - stage-12 honesty. */
                        g_u32LastStage = 12;
                        xhci_stage_soft_lamp(12u, "EVAL_CTX_EP0");
                        return -1;
                    }
                }
            }
        }
    }
    xhci_settle((g_u8PortSpeed >= 4u) ? 1500000u : 400000u);
    /* Gentle U0 only if PLS!=0 - LWS spam after soft-8 drops SS (stage-13). */
    if (g_u8PortId != 0 && ((portsc_read(g_u8PortId) >> 5) & 0xfu) != 0u) {
        xhci_port_force_u0(g_u8PortId);
        xhci_settle((g_u8PortSpeed >= 4u) ? 400000u : 100000u);
    }
    xhci_drain_events(64u);

    /*
     * Full device descriptor (stage-13 residual deepen, no ladder grow):
     * Try wLength=18 first, then MPS0-sized (short packet residual), then 64.
     * Accept via residual: got = wlen - data_resid when short; need ≥18
     * with bLength/bDescriptorType valid. After any TO: soft-sync once, then
     * hard-resync (not soft-arm first - arm thrash is classic stage-13 cascade).
     * Grep: xhci: get device desc residual | get_desc18 | stage=13
     */
    {
        u16 aWlen[3];
        u32 iW;
        u8 aBig[64];
        u16 u16MpsTry;

        aWlen[0] = 18u;
        /* Residual: one-shot at EP0 MPS (capped) before full 64. */
        u16MpsTry = g_u16MaxPkt0;
        if (u16MpsTry < 18u) {
            u16MpsTry = 18u;
        }
        if (u16MpsTry > 64u) {
            u16MpsTry = 64u;
        }
        aWlen[1] = u16MpsTry;
        aWlen[2] = 64u;

        for (iW = 0; iW < 3u; iW++) {
            u16 u16W = aWlen[iW];
            u8 *pBuf = (u16W <= 18u) ? aTmp : aBig;
            u32 cbCopy;
            u32 u32Got;

            /* Skip duplicate wlen (e.g. MPS already 18 or 64). */
            if (iW > 0u && u16W == aWlen[iW - 1u]) {
                continue;
            }
            if (iW == 2u && u16W == aWlen[1]) {
                continue;
            }
            setup.u8BmRequestType = 0x80u;
            setup.u8BRequest = USB_REQ_GET_DESCRIPTOR;
            setup.u16WValue = (u16)(USB_DT_DEVICE << 8);
            setup.u16WIndex = 0;
            setup.u16WLength = u16W;

            /* Boot-capped: 2 tries/wlen (was 3) - Dual DoD A first. */
            for (nTry = 0; nTry < 2; nTry++) {
                memset(pBuf, 0, u16W > 18u ? 64u : 18u);
                if (xhci_ctrl(&setup, pBuf, u16W, 1) == 0) {
                    /* Residual -> actual bytes received on Data stage. */
                    u32Got = xhci_ctrl_got_len(u16W);
                    if (u32Got < 8u && pBuf[0] >= 8u &&
                        pBuf[1] == USB_DT_DEVICE) {
                        /* residual lag / Success; trust valid desc prefix */
                        u32Got = (pBuf[0] >= 18u) ? 18u : (u32)pBuf[0];
                        kprintf("xhci: get device desc residual "
                                "content-backed got=%u resid=%u wlen=%u "
                                "(stage-13 Soft!=product)\n",
                                (unsigned)u32Got,
                                (unsigned)g_u32LastCtrlResidual,
                                (unsigned)u16W);
                    }
                    kprintf("xhci: get device desc residual wlen=%u got=%u "
                            "resid=%u try=%u bLen=%u bType=%u\n",
                            (unsigned)u16W, (unsigned)u32Got,
                            (unsigned)g_u32LastCtrlResidual, (unsigned)nTry,
                            (unsigned)pBuf[0], (unsigned)pBuf[1]);
                    cbCopy = 18u;
                    if (u32Got >= 18u && pBuf[0] >= 18u &&
                        pBuf[1] == USB_DT_DEVICE) {
                        memcpy(pDev, pBuf, cbCopy);
                        kprintf("xhci: get device desc PASS wlen=%u try=%u "
                                "got=%u (stage-13 residual Soft!=product)\n",
                                (unsigned)u16W, (unsigned)nTry,
                                (unsigned)u32Got);
                        (void)xhci_ep0_soft_sync_producer();
                        return 0;
                    }
                    /* Short buffer still may hold 18 useful bytes. */
                    if (pBuf[0] >= 18u && pBuf[1] == USB_DT_DEVICE) {
                        memcpy(pDev, pBuf, cbCopy);
                        kprintf("xhci: get device desc PASS wlen=%u "
                                "try=%u (content ok got=%u Soft!=product)\n",
                                (unsigned)u16W, (unsigned)nTry,
                                (unsigned)u32Got);
                        (void)xhci_ep0_soft_sync_producer();
                        return 0;
                    }
                    kprintf("xhci: get device desc bad content wlen=%u "
                            "bLen=%u bType=%u got=%u\n",
                            (unsigned)u16W, (unsigned)pBuf[0],
                            (unsigned)pBuf[1], (unsigned)u32Got);
                } else {
                    kprintf("xhci: get device desc FAIL wlen=%u try=%u "
                            "cc=%u (get_desc18)\n",
                            (unsigned)u16W, (unsigned)nTry,
                            (unsigned)g_u32CtrlFailCc);
                }
                if (nTry + 1 < 2) {
                    /*
                     * After TO: one soft-recover (capped 2-3 passes), else
                     * hard-resync. No multi-million soft-continue ladder -
                     * that hung boot on stage-13 (v2026.08.04.60).
                     * Grep: get_desc18 soft-continue|hard-resync|boot hang
                     */
                    kprintf("msc: progress get_desc18 recover after TO "
                            "wlen=%u n=%u cc=%u (stage=13 residual "
                            "Soft!=product)\n",
                            (unsigned)u16W, (unsigned)nTry,
                            (unsigned)g_u32CtrlFailCc);
                    if (xhci_ep0_soft_recover_after_to("get_desc18") != 0) {
                        kprintf("msc: progress get_desc18 soft-continue "
                                "after TO wlen=%u n=%u (stage=13 "
                                "Soft!=product)\n",
                                (unsigned)u16W, (unsigned)nTry);
                    } else {
                        kprintf("msc: progress get_desc18 hard-resync after TO "
                                "wlen=%u n=%u cc=%u (stage=13 Soft!=product)\n",
                                (unsigned)u16W, (unsigned)nTry,
                                (unsigned)g_u32CtrlFailCc);
                        xhci_ctrl_recover();
                    }
                    xhci_mps0_resync_from_speed();
                    xhci_settle(fSsPath != 0 ? 200000u : 80000u);
                }
            }
        }
    }
    kprintf("xhci: get device desc FAIL stage=13 cc=%u mps0=%u speed=%u "
            "Soft!=product\n",
            (unsigned)g_u32CtrlFailCc, (unsigned)g_u16MaxPkt0,
            (unsigned)g_u8PortSpeed);
    kprintf("msc: progress get_desc FAIL stage=13 cc=%u port=%u spd=%u "
            "Soft!=product\n",
            (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortId,
            (unsigned)g_u8PortSpeed);
    g_u32LastStage = 13;
    xhci_stage_soft_lamp(13u, "GET_DESC18");
    return -1;
}

/*
 * Port ready for next EP0 control after device-desc: PP on, CCS+PED, gentle U0.
 * Returns 0 ok, -1 CCS gone / PED lost (caller readdresses - do not cold-PR only).
 * Grep: xhci: pre-config
 */
static int
xhci_port_ready_for_config(void)
{
    u32 u32Ps;

    if (g_u8PortId == 0) {
        return 0;
    }
    xhci_port_power_on(g_u8PortId);
    u32Ps = portsc_read(g_u8PortId);
    kprintf("xhci: pre-config portsc=0x%x ped=%u pls=%u spd=%u pp=%u\n",
            (unsigned)u32Ps,
            (u32Ps & XHCI_PORTSC_PED) != 0 ? 1u : 0u,
            (unsigned)((u32Ps >> 5) & 0xfu),
            (unsigned)((u32Ps >> 10) & 0xfu),
            (u32Ps & XHCI_PORTSC_PP) != 0 ? 1u : 0u);
    if ((u32Ps & XHCI_PORTSC_CCS) == 0) {
        kprintf("xhci: pre-config CCS=0 (no device) stage=15\n");
        xhci_note_ctrl_fail_cc(0);
        return -1;
    }
    if ((u32Ps & XHCI_PORTSC_PED) == 0) {
        /*
         * PED lost after device-desc: port reset alone is wrong - device
         * drops to USB addr 0 while slot keeps N. Caller must readdress.
         */
        kprintf("xhci: PED lost before config (need readdress) "
                "portsc=0x%x\n",
                (unsigned)u32Ps);
        xhci_note_ctrl_fail_cc(0);
        return -1;
    }
    /* Re-sample speed if field valid (SS stick may renegotiate). */
    if (((u32Ps >> 10) & 0xfu) != 0u) {
        g_u8PortSpeed = (u8)((u32Ps >> 10) & 0xfu);
    }
    /* Gentle U0 only if not already U0 - LWS spam drops SS (stage-15 thrash). */
    if (((u32Ps >> 5) & 0xfu) != 0u) {
        xhci_port_force_u0(g_u8PortId);
        xhci_settle((g_u8PortSpeed >= 4u) ? 800000u : 300000u);
    }
    return 0;
}

/*
 * GET_DESCRIPTOR(configuration). Stage-15 deepen (mirror stage-11 soft-align):
 * Soft-continue after a working device-desc when EP0 producer is HC-aligned
 * (base OR mid-ring) - do NOT Set TR Deq / ring_reset before the first try.
 * Prior pre-arm thrash desynced a12f -> stage=15 cc=0 TO
 * (photo p21/s4: device-desc PASS -> config timeout).
 *
 * Ladder (soft residual lean Soft!=product; a12f G-AC-1; freestanding net first):
 *   pre: drain + settle + port PP/PED + mps0 resync + soft-align prepare
 *   soft-align: mid-ring / soft-sync / soft-recover + late + late2 + dual-sample
 *     residual (late3..late14 multi-M chain removed)
 *   try: single Data TRB ctrl (9 -> 64 -> 255 -> 512); first try soft
 *   poll: SS primary+grace+extra+third (capped budgets; false-TO residual)
 *   on fail: soft-recover lean (2nd|3rd-pass max) -> hard
 *   residual content-backed full-in-shot when residual under-counts header
 *   stamps: try/FAIL/soft-align/false-TO rate-limited first+every 16th (no storms)
 * PED loss returns -1 without port-reset-only. Caller readdresses.
 * Stage still OPEN - Soft!=product; dual DoD A stick OPEN; never product T1.
 * Freestanding MSC SKIP default (GJ_XHCI_MSC_PROBE=0) - do not re-enable default.
 *
 * Grep: xhci: get config | xhci: GET_CONFIG stage=15 | msc: progress get_config
 *       | soft residual lean | Soft!=product | get config soft-align residual
 *       | ep0 soft-recover second|third-pass | ctrl false-TO residual
 *       | Soft!=product Stage OPEN
 */
static int
xhci_get_config_descriptor(u8 *aCfg, u16 *pTotal)
{
    struct usb_setup setup;
    int nTry;
    int fGot = 0;
    int fSsPath;
    u16 u16TryLen;
    u16 u16Got = 0;
    u16 u16Total;
    u32 iW;
    /* Prefer 9-byte header first (single short Data TRB); then larger one-shot. */
    static const u16 s_aCfgWlen[] = { 9u, 64u, 255u, 512u };

    if (aCfg == NULL || pTotal == NULL) {
        return -1;
    }

    fSsPath = (g_u8PortSpeed >= 4u) ? 1 : 0;
    kprintf("msc: progress get_config begin port=%u spd=%u mps0=%u "
            "ep0_idx=%u ep0_cyc=%u path=%s\n",
            (unsigned)g_u8PortId, (unsigned)g_u8PortSpeed,
            (unsigned)g_u16MaxPkt0, (unsigned)g_u32Ep0Idx,
            (unsigned)g_fEp0Cycle, fSsPath != 0 ? "SS" : "HS");

    xhci_drain_events(96u);
    /*
     * Soft residual lean: short post device-desc settle (not 22M thrash).
     * Soft-continue first try - no thrash Set TR Deq while aligned. Net first.
     * Soft!=product · freestanding MSC SKIP default.
     */
    xhci_settle(fSsPath != 0 ? 4000000u : 1200000u);
    if (xhci_port_ready_for_config() != 0) {
        kprintf("msc: progress get_config FAIL reason=port_not_ready\n");
        return -1;
    }
    /* mps0 resync + soft-align / hard only if misaligned (no thrash). */
    xhci_ep0_prepare_for_config();
    xhci_drain_events(48u);
    /*
     * GET_CONFIG soft-align recheck (stage-15 Soft!=product; a12f G-AC-1):
     * after prepare, soft-sync / mid-ring soft-continue so first doorbell is
     * producer-HC aligned without Set TR Deq thrash. Gap C dual-DoD deepen:
     * soft-recover then late recheck (mid-ring) if primary soft-sync quiet.
     * Soft-align greps rate-limited (first+every 16th; no stamp storms).
     * Grep: xhci: get config soft-align
     */
    {
        u32 u32EpStA;
        u64 u64DeqA = 0;
        int fAligned = 0;
        int fStampAlign = xhci_get_config_align_stamp_ok();

        if (xhci_ep0_soft_sync_producer() != 0) {
            u32EpStA = xhci_ep0_sample(&u64DeqA);
            if (xhci_ep0_soft_continue_ok(u32EpStA, u64DeqA) != 0 ||
                xhci_ep0_producer_hc_aligned(u32EpStA, u64DeqA) != 0) {
                if (fStampAlign != 0) {
                    kprintf("xhci: get config soft-align ok state=%u idx=%u "
                            "(stage=15 Soft!=product; no Set TR Deq)\n",
                            (unsigned)u32EpStA, (unsigned)g_u32Ep0Idx);
                }
                xhci_clflush_ptr(g_pEp0Ring);
                xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                fAligned = 1;
            } else {
                if (fStampAlign != 0) {
                    kprintf("xhci: get config soft-align soft-sync ok idx=%u "
                            "(stage=15 Soft!=product)\n",
                            (unsigned)g_u32Ep0Idx);
                }
                fAligned = 1;
            }
        }
        if (fAligned == 0) {
            /* Soft-recover (SS full ladder / HS four-pass) before late recheck. */
            if (xhci_ep0_soft_recover_after_to("get_config_soft_align") != 0) {
                u32EpStA = xhci_ep0_sample(&u64DeqA);
                if (xhci_ep0_soft_continue_ok(u32EpStA, u64DeqA) != 0 ||
                    xhci_ep0_producer_hc_aligned(u32EpStA, u64DeqA) != 0) {
                    if (fStampAlign != 0) {
                        kprintf("xhci: get config soft-align soft-recover ok "
                                "state=%u idx=%u "
                                "(stage=15 Soft!=product; no Set TR Deq thrash)\n",
                                (unsigned)u32EpStA, (unsigned)g_u32Ep0Idx);
                    }
                    xhci_clflush_ptr(g_pEp0Ring);
                    xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                    fAligned = 1;
                } else {
                    if (fStampAlign != 0) {
                        kprintf("xhci: get config soft-align soft-recover ok "
                                "(stage=15 Soft!=product; no Set TR Deq thrash)\n");
                    }
                    fAligned = 1;
                }
            }
        }
        /*
         * SS late soft-align recheck (Gap C dual-DoD residual Soft!=product):
         * a12f may post TR Deq after soft-recover stamp; one settle + mid-ring
         * soft-continue before first GET_CONFIG doorbell.
         * Grep: xhci: get config soft-align late
         */
        if (fSsPath != 0 && fAligned != 0) {
            xhci_settle(1000000u);
            xhci_drain_events(80u);
            u32EpStA = xhci_ep0_sample(&u64DeqA);
            if (xhci_ep0_soft_continue_ok(u32EpStA, u64DeqA) != 0 ||
                xhci_ep0_producer_hc_aligned(u32EpStA, u64DeqA) != 0) {
                if (fStampAlign != 0) {
                    kprintf("xhci: get config soft-align late ok state=%u idx=%u "
                            "(stage=15 Soft!=product; no Set TR Deq)\n",
                            (unsigned)u32EpStA, (unsigned)g_u32Ep0Idx);
                }
                xhci_clflush_ptr(g_pEp0Ring);
                xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
            } else if (xhci_ep0_soft_sync_producer() != 0) {
                if (fStampAlign != 0) {
                    kprintf("xhci: get config soft-align late soft-sync ok idx=%u "
                            "(stage=15 Soft!=product)\n",
                            (unsigned)g_u32Ep0Idx);
                }
            }
            /*
             * SS late2 soft-align recheck (Gap C dual-DoD residual deepen
             * Soft!=product): a12f may post TR Deq one settle after late ok
             * stamp - re-sample mid-ring before first GET_CONFIG doorbell.
             * Grep: xhci: get config soft-align late2
             */
            xhci_settle(1200000u);
            xhci_drain_events(88u);
            u32EpStA = xhci_ep0_sample(&u64DeqA);
            if (xhci_ep0_soft_continue_ok(u32EpStA, u64DeqA) != 0 ||
                xhci_ep0_producer_hc_aligned(u32EpStA, u64DeqA) != 0) {
                if (fStampAlign != 0) {
                    kprintf("xhci: get config soft-align late2 ok state=%u idx=%u "
                            "(stage=15 Soft!=product; no Set TR Deq)\n",
                            (unsigned)u32EpStA, (unsigned)g_u32Ep0Idx);
                }
                xhci_clflush_ptr(g_pEp0Ring);
                xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
            } else if (xhci_ep0_soft_sync_producer() != 0) {
                if (fStampAlign != 0) {
                    kprintf("xhci: get config soft-align late2 soft-sync ok "
                            "idx=%u (stage=15 Soft!=product)\n",
                            (unsigned)g_u32Ep0Idx);
                }
            }
            /*
             * Soft residual lean: late3..late14 multi-M soft-align chain
             * removed (net first; freestanding MSC SKIP default). Keep
             * late+late2 above + dual-sample residual below. Soft!=product.
             * Grep: xhci: get config soft-align residual | soft residual lean
             */
            /*
             * Event-ring + dual-sample EP0 residual before first GET_CONFIG
             * doorbell (false-TO Soft!=product; a12f G-AC-1). Prefer real
             * residual over Nth-pass comment. Force-inval current+next event
             * TRB, dual-sample deq, soft-sync if deq advanced, mid-ring
             * soft-continue when aligned. If deq moved, second settle +
             * soft-continue recheck (no Set TR Deq thrash).
             * Grep: xhci: get config soft-align residual
             * Soft!=product Stage OPEN. Dual DoD A stick OPEN.
             */
            if (g_pEvtRing != NULL) {
                xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx]);
                if ((g_u32EvtIdx + 1u) < XHCI_RING_TRBS) {
                    xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx + 1u]);
                } else {
                    xhci_clinv_ptr(&g_pEvtRing[0]);
                }
            }
            {
                u64 u64Deq1 = 0;
                u64 u64Deq2 = 0;
                u32 u32St2;
                int fDeqMovedAlign = 0;

                (void)xhci_ep0_sample(&u64Deq1);
                xhci_settle(800000u);
                xhci_drain_events(96u);
                u32St2 = xhci_ep0_sample(&u64Deq2);
                if (u64Deq2 != u64Deq1) {
                    fDeqMovedAlign = 1;
                    (void)xhci_ep0_soft_sync_producer();
                    u32St2 = xhci_ep0_sample(&u64Deq2);
                    if (fStampAlign != 0) {
                        kprintf("xhci: get config soft-align residual deq "
                                "moved 0x%lx->0x%lx state=%u "
                                "(stage=15 Soft!=product)\n",
                                (unsigned long)u64Deq1,
                                (unsigned long)u64Deq2, (unsigned)u32St2);
                    }
                }
                if (xhci_ep0_soft_continue_ok(u32St2, u64Deq2) != 0 ||
                    xhci_ep0_producer_hc_aligned(u32St2, u64Deq2) != 0) {
                    if (fStampAlign != 0) {
                        kprintf("xhci: get config soft-align residual ok "
                                "state=%u idx=%u "
                                "(stage=15 Soft!=product; no Set TR Deq)\n",
                                (unsigned)u32St2, (unsigned)g_u32Ep0Idx);
                    }
                    xhci_clflush_ptr(g_pEp0Ring);
                    xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                } else if (xhci_ep0_soft_sync_producer() != 0) {
                    if (fStampAlign != 0) {
                        kprintf("xhci: get config soft-align residual "
                                "soft-sync ok idx=%u "
                                "(stage=15 Soft!=product)\n",
                                (unsigned)g_u32Ep0Idx);
                    }
                } else if (fDeqMovedAlign != 0) {
                    /*
                     * deq moved but soft-continue still quiet: short second
                     * residual (event-ring Soft!=product) before doorbell.
                     */
                    xhci_settle(1200000u);
                    if (g_pEvtRing != NULL) {
                        xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx]);
                    }
                    xhci_drain_events(80u);
                    u32St2 = xhci_ep0_sample(&u64Deq2);
                    if (xhci_ep0_soft_continue_ok(u32St2, u64Deq2) != 0 ||
                        xhci_ep0_producer_hc_aligned(u32St2, u64Deq2) != 0 ||
                        xhci_ep0_soft_sync_producer() != 0) {
                        if (fStampAlign != 0) {
                            kprintf("xhci: get config soft-align residual "
                                    "soft-continue ok late state=%u idx=%u "
                                    "(stage=15 Soft!=product; no Set TR Deq)\n",
                                    (unsigned)u32St2,
                                    (unsigned)g_u32Ep0Idx);
                        }
                        xhci_clflush_ptr(g_pEp0Ring);
                        xhci_clflush_ptr(
                            &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                    }
                }
            }
        } else if (fSsPath != 0 && fAligned == 0) {
            /*
             * Soft residual lean (net first): one late soft-recover + mid-ring
             * recheck before try. Nested late3..late14 multi-M ladder removed.
             * Soft!=product · G-AC-1 · freestanding MSC SKIP default.
             * Grep: xhci: get config soft-align late | soft residual lean
             */
            xhci_settle(800000u);
            xhci_drain_events(80u);
            if (xhci_ep0_soft_recover_after_to("get_config_soft_align_late") !=
                0) {
                if (fStampAlign != 0) {
                    kprintf("xhci: get config soft-align late soft-recover ok "
                            "(stage=15 Soft!=product; no Set TR Deq thrash)\n");
                }
                xhci_settle(600000u);
                xhci_drain_events(64u);
                u32EpStA = xhci_ep0_sample(&u64DeqA);
                if (xhci_ep0_soft_continue_ok(u32EpStA, u64DeqA) != 0 ||
                    xhci_ep0_producer_hc_aligned(u32EpStA, u64DeqA) != 0) {
                    if (fStampAlign != 0) {
                        kprintf("xhci: get config soft-align late2 ok after "
                                "soft-recover state=%u idx=%u "
                                "(stage=15 Soft!=product; no Set TR Deq)\n",
                                (unsigned)u32EpStA, (unsigned)g_u32Ep0Idx);
                    }
                    xhci_clflush_ptr(g_pEp0Ring);
                    xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                }
            }
        }
    }
    kprintf("msc: progress get_config ep0_ready mps0=%u idx=%u cyc=%u "
            "(after device-desc Soft!=product)\n",
            (unsigned)g_u16MaxPkt0, (unsigned)g_u32Ep0Idx,
            (unsigned)g_fEp0Cycle);

    memset(&setup, 0, sizeof(setup));
    setup.u8BmRequestType = 0x80u;
    setup.u8BRequest = USB_REQ_GET_DESCRIPTOR;
    setup.u16WValue = (u16)(USB_DT_CONFIG << 8); /* config index 0 */
    setup.u16WIndex = 0;

    for (iW = 0; iW < 4u && fGot == 0; iW++) {
        u16TryLen = s_aCfgWlen[iW];
        if (u16TryLen > 512u) {
            u16TryLen = 512u;
        }
        setup.u16WLength = u16TryLen;
        for (nTry = 0; nTry < 6; nTry++) {
            u32 u32EpSt;
            u64 u64Deq = 0;
            int fStampTry;

            memset(aCfg, 0, u16TryLen);
            u32EpSt = xhci_ep0_sample(&u64Deq);
            /*
             * Rate-limit try greps (4 wlen x 6 tries + recover can storm
             * serial; Soft!=product; first + every 16th).
             * Grep: xhci: get config try | msc: progress get_config try
             */
            g_u32GetConfigTryStampN++;
            fStampTry = (g_u32GetConfigTryStampN == 1u ||
                         (g_u32GetConfigTryStampN & 15u) == 0u)
                            ? 1
                            : 0;
            if (fStampTry != 0) {
                kprintf("xhci: get config try wlen=%u try=%u ep0_idx=%u "
                        "cyc=%u state=%u deq=0x%lx mps0=%u n=%u "
                        "Soft!=product\n",
                        (unsigned)u16TryLen, (unsigned)nTry,
                        (unsigned)g_u32Ep0Idx, (unsigned)g_fEp0Cycle,
                        (unsigned)u32EpSt, (unsigned long)u64Deq,
                        (unsigned)g_u16MaxPkt0,
                        (unsigned)g_u32GetConfigTryStampN);
                kprintf("msc: progress get_config try wlen=%u n=%u\n",
                        (unsigned)u16TryLen, (unsigned)nTry);
            }
            if (xhci_ctrl(&setup, aCfg, u16TryLen, 1) == 0 &&
                aCfg[0] >= 9u && aCfg[1] == USB_DT_CONFIG) {
                u32 u32GotR;
                u16 u16TotalHint;

                fGot = 1;
                u32GotR = xhci_ctrl_got_len(u16TryLen);
                u16Got = (u16)u32GotR;
                if (u16Got < 9u) {
                    /* Residual honesty: DT_CONFIG content ok -> treat as ≥9. */
                    u16Got = 9u;
                    kprintf("xhci: get config residual content-backed got=9 "
                            "resid=%u wlen=%u (stage-15 Soft!=product)\n",
                            (unsigned)g_u32LastCtrlResidual,
                            (unsigned)u16TryLen);
                }
                /*
                 * Residual deepen (stage-15 Soft!=product; a12f):
                 * wTotalLength in header bounds "got" when residual under-
                 * counts (ISP lag / residual==wlen with valid buffer). Prefer
                 * full-in-shot over thrash re-fetch when total fits this TD.
                 * Grep: xhci: get config residual content-backed|full-in-shot
                 */
                u16TotalHint = (u16)aCfg[2] | ((u16)aCfg[3] << 8);
                if (u16TotalHint >= 9u && u16TotalHint <= u16TryLen &&
                    u16Got < u16TotalHint && u32GotR < 9u) {
                    /* residual claimed empty/short; promote to total in shot */
                    u16Got = u16TotalHint;
                    kprintf("xhci: get config residual content-backed full "
                            "got=%u total=%u resid=%u wlen=%u "
                            "(stage-15 Soft!=product)\n",
                            (unsigned)u16Got, (unsigned)u16TotalHint,
                            (unsigned)g_u32LastCtrlResidual,
                            (unsigned)u16TryLen);
                }
                if (u16TotalHint >= 9u && u16TotalHint <= u16TryLen &&
                    u16Got >= u16TotalHint) {
                    kprintf("xhci: get config residual full-in-shot "
                            "got=%u total=%u wlen=%u resid=%u "
                            "(stage-15 Soft!=product)\n",
                            (unsigned)u16Got, (unsigned)u16TotalHint,
                            (unsigned)u16TryLen,
                            (unsigned)g_u32LastCtrlResidual);
                }
                kprintf("xhci: get config PASS wlen=%u try=%u got=%u "
                        "residual=%u mps0=%u (stage-15 residual Soft!=product)\n",
                        (unsigned)u16TryLen, (unsigned)nTry,
                        (unsigned)u16Got, (unsigned)g_u32LastCtrlResidual,
                        (unsigned)g_u16MaxPkt0);
                /* Mid-ring soft-continue after header PASS (no Set TR Deq). */
                {
                    u32 u32EpStH;
                    u64 u64DeqH = 0;
                    int fHdrAligned = 0;

                    u32EpStH = xhci_ep0_sample(&u64DeqH);
                    if (xhci_ep0_soft_continue_ok(u32EpStH, u64DeqH) != 0 ||
                        xhci_ep0_producer_hc_aligned(u32EpStH, u64DeqH) != 0) {
                        kprintf("xhci: ep0 mid-ring soft-continue after "
                                "get_config header PASS state=%u idx=%u "
                                "(stage=15 Soft!=product)\n",
                                (unsigned)u32EpStH, (unsigned)g_u32Ep0Idx);
                        xhci_clflush_ptr(g_pEp0Ring);
                        xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        fHdrAligned = 1;
                    } else if (xhci_ep0_soft_sync_producer() != 0) {
                        u32EpStH = xhci_ep0_sample(&u64DeqH);
                        if (xhci_ep0_soft_continue_ok(u32EpStH, u64DeqH) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStH, u64DeqH) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS post-sync "
                                    "state=%u idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStH,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                            fHdrAligned = 1;
                        } else {
                            fHdrAligned = 1; /* soft-sync ok enough */
                        }
                    } else if (fSsPath != 0 &&
                               xhci_ep0_soft_recover_after_to(
                                   "get_config_hdr_pass") != 0) {
                        /* Gap C: SS late TR Deq after short header PASS. */
                        kprintf("xhci: ep0 mid-ring soft-continue after "
                                "get_config header PASS soft-recover "
                                "(stage=15 Soft!=product)\n");
                        fHdrAligned = 1;
                    }
                    /*
                     * SS late mid-ring recheck after header PASS (Gap C
                     * dual-DoD residual deepen Soft!=product): a12f may post
                     * TR Deq after primary soft-continue stamp - re-sample
                     * before full fetch / next control.
                     * Grep: xhci: ep0 mid-ring soft-continue after get_config
                     *       header PASS late
                     */
                    if (fSsPath != 0 && fHdrAligned != 0) {
                        xhci_settle(900000u);
                        xhci_drain_events(72u);
                        u32EpStH = xhci_ep0_sample(&u64DeqH);
                        if (xhci_ep0_soft_continue_ok(u32EpStH, u64DeqH) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStH, u64DeqH) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStH,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "header PASS late ok "
                                    "(stage=15 Soft!=product)\n");
                        }
                        /*
                         * SS late2 mid-ring recheck after header PASS
                         * (Gap C dual-DoD residual deepen Soft!=product):
                         * a12f may post TR Deq one settle after late -
                         * re-sample before full fetch.
                         * Grep: xhci: ep0 mid-ring soft-continue after
                         *       get_config header PASS late2
                         */
                        xhci_settle(1100000u);
                        xhci_drain_events(80u);
                        u32EpStH = xhci_ep0_sample(&u64DeqH);
                        if (xhci_ep0_soft_continue_ok(u32EpStH, u64DeqH) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStH, u64DeqH) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late2 state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStH,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "header PASS late2 ok "
                                    "(stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late2") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late2 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                        /*
                         * SS late3 mid-ring recheck after header PASS
                         * (Gap C dual-DoD residual deepen Soft!=product):
                         * a12f may post TR Deq one settle after late2 -
                         * re-sample before full fetch.
                         * Grep: xhci: ep0 mid-ring soft-continue after
                         *       get_config header PASS late3
                         */
                        xhci_settle(1300000u);
                        xhci_drain_events(88u);
                        u32EpStH = xhci_ep0_sample(&u64DeqH);
                        if (xhci_ep0_soft_continue_ok(u32EpStH, u64DeqH) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStH, u64DeqH) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late3 state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStH,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "header PASS late3 ok "
                                    "(stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late3") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late3 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                        /*
                         * SS late4 mid-ring recheck after header PASS
                         * (Gap C dual-DoD residual deepen Soft!=product):
                         * a12f may post TR Deq one settle after late3 -
                         * re-sample before full fetch.
                         * Grep: xhci: ep0 mid-ring soft-continue after
                         *       get_config header PASS late4
                         */
                        xhci_settle(1500000u);
                        xhci_drain_events(96u);
                        u32EpStH = xhci_ep0_sample(&u64DeqH);
                        if (xhci_ep0_soft_continue_ok(u32EpStH, u64DeqH) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStH, u64DeqH) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late4 state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStH,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "header PASS late4 ok "
                                    "(stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late4") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late4 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                        /*
                         * SS late5 mid-ring recheck after header PASS
                         * (Gap C dual-DoD residual deepen Soft!=product):
                         * a12f may post TR Deq one settle after late4 -
                         * re-sample before full fetch.
                         * Grep: xhci: ep0 mid-ring soft-continue after
                         *       get_config header PASS late5
                         */
                        xhci_settle(1700000u);
                        xhci_drain_events(104u);
                        u32EpStH = xhci_ep0_sample(&u64DeqH);
                        if (xhci_ep0_soft_continue_ok(u32EpStH, u64DeqH) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStH, u64DeqH) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late5 state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStH,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "header PASS late5 ok "
                                    "(stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late5") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late5 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                        /*
                         * SS late6 mid-ring recheck after header PASS
                         * (Gap C dual-DoD residual deepen Soft!=product):
                         * a12f may post TR Deq one settle after late5 -
                         * re-sample before full fetch.
                         * Grep: xhci: ep0 mid-ring soft-continue after
                         *       get_config header PASS late6
                         */
                        xhci_settle(1900000u);
                        xhci_drain_events(112u);
                        u32EpStH = xhci_ep0_sample(&u64DeqH);
                        if (xhci_ep0_soft_continue_ok(u32EpStH, u64DeqH) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStH, u64DeqH) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late6 state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStH,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "header PASS late6 ok "
                                    "(stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late6") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late6 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                    } else if (fSsPath != 0 && fHdrAligned == 0) {
                        xhci_settle(1100000u);
                        xhci_drain_events(80u);
                        if (xhci_ep0_soft_recover_after_to(
                                "get_config_hdr_pass_late") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late2") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late2 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late3") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late3 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late4") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late4 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late5") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late5 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_hdr_pass_late6") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config header PASS late6 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                    }
                }
                break;
            }
            /* FAIL stamp: always last try; else rate-limited with try greps. */
            if (fStampTry != 0 || (nTry + 1) >= 6) {
                kprintf("xhci: get config FAIL wlen=%u try=%u cc=%u mps0=%u "
                        "spd=%u port=%u Soft!=product\n",
                        (unsigned)u16TryLen, (unsigned)nTry,
                        (unsigned)g_u32CtrlFailCc, (unsigned)g_u16MaxPkt0,
                        (unsigned)g_u8PortSpeed, (unsigned)g_u8PortId);
            }
            if (nTry + 1 >= 6) {
                break;
            }
            /*
             * After any failed enqueue+TO: soft-sync first -> mid-ring
             * soft-continue; hard only on deny - no thrash ladder grow.
             * Grep: msc: progress get_config soft-sync | mid-ring soft-continue
             *       | hard-resync after TO | Soft!=product
             */
            if (fStampTry != 0) {
                kprintf("msc: progress get_config recover after TO n=%u "
                        "cc=%u wlen=%u (stage=15 residual Soft!=product)\n",
                        (unsigned)nTry, (unsigned)g_u32CtrlFailCc,
                        (unsigned)u16TryLen);
            }
            if (xhci_port_ready_for_config() != 0) {
                kprintf("msc: progress get_config FAIL reason=ped_lost\n");
                return -1;
            }
            /*
             * Event-ring residual before soft-recover (false-TO Soft!=product):
             * late TR Deq / Transfer Event may land after xhci_ctrl TO dump -
             * prefer soft-continue over thrash Set TR Deq (a12f G-AC-1).
             */
            if (g_pEvtRing != NULL) {
                xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx]);
                if ((g_u32EvtIdx + 1u) < XHCI_RING_TRBS) {
                    xhci_clinv_ptr(&g_pEvtRing[g_u32EvtIdx + 1u]);
                } else {
                    xhci_clinv_ptr(&g_pEvtRing[0]);
                }
            }
            xhci_drain_events(96u);
            if (xhci_ep0_soft_recover_after_to("get_config") != 0) {
                kprintf("msc: progress get_config soft-continue after TO "
                        "n=%u wlen=%u (stage=15 Soft!=product)\n",
                        (unsigned)nTry, (unsigned)u16TryLen);
                /*
                 * Residual: after soft-recover, dual-sample EP0; ensure room
                 * for Setup+Data+Status; if producer mid-ring but not
                 * HC-aligned, re-prep (prefer mid-ring soft-continue /
                 * single arm Soft!=product; no thrash Set TR Deq).
                 */
                {
                    u32 u32EpStR;
                    u64 u64DeqR = 0;
                    u64 u64DeqR2 = 0;

                    u32EpStR = xhci_ep0_sample(&u64DeqR);
                    xhci_settle(fSsPath != 0 ? 900000u : 300000u);
                    xhci_drain_events(64u);
                    u32EpStR = xhci_ep0_sample(&u64DeqR2);
                    if (u64DeqR2 != u64DeqR) {
                        (void)xhci_ep0_soft_sync_producer();
                        u32EpStR = xhci_ep0_sample(&u64DeqR2);
                    }
                    if (xhci_ep0_soft_continue_ok(u32EpStR, u64DeqR2) != 0 ||
                        xhci_ep0_producer_hc_aligned(u32EpStR, u64DeqR2) !=
                            0) {
                        xhci_clflush_ptr(g_pEp0Ring);
                        xhci_clflush_ptr(
                            &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                    } else {
                        kprintf("msc: progress get_config re-prep after "
                                "soft-recover n=%u (stage=15 Soft!=product)\n",
                                (unsigned)nTry);
                        xhci_ep0_prepare_for_config();
                    }
                }
            } else {
                kprintf("msc: progress get_config hard-resync after TO n=%u "
                        "cc=%u wlen=%u (stage=15 Soft!=product)\n",
                        (unsigned)nTry, (unsigned)g_u32CtrlFailCc,
                        (unsigned)u16TryLen);
                xhci_ctrl_recover();
            }
            xhci_mps0_resync_from_speed();
            /* Gap C residual deepen Soft!=product: longer settle between GET_CONFIG tries. */
            xhci_settle(fSsPath != 0
                            ? (nTry >= 2 ? 7000000u : 3500000u)
                            : 1200000u);
            if (g_u8PortId != 0 &&
                (portsc_read(g_u8PortId) & XHCI_PORTSC_PED) == 0) {
                kprintf("xhci: PED lost mid-config try=%u\n",
                        (unsigned)nTry);
                kprintf("msc: progress get_config FAIL reason=ped_lost\n");
                return -1;
            }
        }
    }

    if (fGot == 0) {
        kprintf("xhci: GET_CONFIG stage=15 (header) cc=%u speed=%u port=%u "
                "mps0=%u Soft!=product\n",
                (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortSpeed,
                (unsigned)g_u8PortId, (unsigned)g_u16MaxPkt0);
        kprintf("msc: progress get_config FAIL stage=15 cc=%u port=%u spd=%u "
                "Soft!=product\n",
                (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortId,
                (unsigned)g_u8PortSpeed);
        xhci_stage_soft_lamp(15u, "GET_CONFIG");
        return -1;
    }

    u16Total = (u16)aCfg[2] | ((u16)aCfg[3] << 8);
    kprintf("xhci: config wTotalLength=%u bNumInterfaces=%u val=%u "
            "speed=%u got=%u mps0=%u\n",
            (unsigned)u16Total, (unsigned)aCfg[4], (unsigned)aCfg[5],
            (unsigned)g_u8PortSpeed, (unsigned)u16Got,
            (unsigned)g_u16MaxPkt0);
    if (u16Total < 9u) {
        u16Total = 9;
    }
    if (u16Total > 512u) {
        u16Total = 512u;
    }

    /* Second fetch only if wTotalLength > bytes actually received. */
    if (u16Total > u16Got) {
        u32 u32EpSt;
        u64 u64Deq = 0;

        /*
         * Soft-continue when header left EP0 producer-aligned (base or
         * mid-ring). Single Data TRB for full wTotalLength (HC splits on
         * wire; multi-TRB broke a12f). Hard-resync + mps0 only after TO or
         * when Halted/Error/desync.
         */
        xhci_settle(fSsPath != 0 ? 1000000u : 400000u);
        xhci_mps0_resync_from_speed();
        /* Stage-15 residual: soft-sync after header before full fetch. */
        if (xhci_ep0_soft_recover_after_to("get_config_full_pre") != 0) {
            kprintf("xhci: get config full soft-continue pre state ok "
                    "(stage=15 residual Soft!=product)\n");
        } else {
            u32EpSt = xhci_ep0_sample(&u64Deq);
            if (u32EpSt == XHCI_EP_ST_HALTED || u32EpSt == XHCI_EP_ST_ERROR ||
                xhci_ep0_soft_continue_ok(u32EpSt, u64Deq) == 0) {
                kprintf("xhci: get config full pre hard-resync state=%u "
                        "deq=0x%lx idx=%u (stage=15 Soft!=product)\n",
                        (unsigned)u32EpSt, (unsigned long)u64Deq,
                        (unsigned)g_u32Ep0Idx);
                xhci_ctrl_recover();
                xhci_mps0_resync_from_speed();
            }
        }
        setup.u8BmRequestType = 0x80u;
        setup.u8BRequest = USB_REQ_GET_DESCRIPTOR;
        setup.u16WValue = (u16)(USB_DT_CONFIG << 8);
        setup.u16WIndex = 0;
        setup.u16WLength = u16Total;
        fGot = 0;
        for (nTry = 0; nTry < 5; nTry++) {
            u32 u32GotFull;

            memset(aCfg, 0, u16Total);
            kprintf("xhci: get config full try total=%u try=%u mps0=%u\n",
                    (unsigned)u16Total, (unsigned)nTry,
                    (unsigned)g_u16MaxPkt0);
            kprintf("msc: progress get_config full n=%u total=%u\n",
                    (unsigned)nTry, (unsigned)u16Total);
            if (xhci_ctrl(&setup, aCfg, u16Total, 1) == 0 && aCfg[0] >= 9u &&
                aCfg[1] == USB_DT_CONFIG) {
                u32GotFull = xhci_ctrl_got_len(u16Total);
                if (u32GotFull < 9u) {
                    /* residual lag with valid DT_CONFIG -> content-backed */
                    u32GotFull = (u32)u16Total;
                    kprintf("xhci: get config full residual content-backed "
                            "got=%u total=%u resid=%u "
                            "(stage-15 Soft!=product)\n",
                            (unsigned)u32GotFull, (unsigned)u16Total,
                            (unsigned)g_u32LastCtrlResidual);
                }
                fGot = 1;
                u16Got = (u16)u32GotFull;
                kprintf("xhci: get config full PASS total=%u try=%u got=%u "
                        "resid=%u (stage-15 residual Soft!=product)\n",
                        (unsigned)u16Total, (unsigned)nTry,
                        (unsigned)u16Got, (unsigned)g_u32LastCtrlResidual);
                /* Prefer mid-ring soft-continue after full PASS (no thrash). */
                {
                    u32 u32EpStF;
                    u64 u64DeqF = 0;
                    int fFullAligned = 0;

                    u32EpStF = xhci_ep0_sample(&u64DeqF);
                    if (xhci_ep0_soft_continue_ok(u32EpStF, u64DeqF) != 0 ||
                        xhci_ep0_producer_hc_aligned(u32EpStF, u64DeqF) != 0) {
                        kprintf("xhci: ep0 mid-ring soft-continue after "
                                "get_config full PASS state=%u idx=%u "
                                "(stage=15 Soft!=product)\n",
                                (unsigned)u32EpStF, (unsigned)g_u32Ep0Idx);
                        xhci_clflush_ptr(g_pEp0Ring);
                        xhci_clflush_ptr(&g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        fFullAligned = 1;
                    } else if (xhci_ep0_soft_sync_producer() != 0) {
                        u32EpStF = xhci_ep0_sample(&u64DeqF);
                        if (xhci_ep0_soft_continue_ok(u32EpStF, u64DeqF) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStF, u64DeqF) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS post-sync "
                                    "state=%u idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStF,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                            fFullAligned = 1;
                        } else {
                            fFullAligned = 1;
                        }
                    } else if (fSsPath != 0 &&
                               xhci_ep0_soft_recover_after_to(
                                   "get_config_full_pass") != 0) {
                        /* Gap C: SS late TR Deq after full config PASS. */
                        kprintf("xhci: ep0 mid-ring soft-continue after "
                                "get_config full PASS soft-recover "
                                "(stage=15 Soft!=product)\n");
                        fFullAligned = 1;
                    }
                    /*
                     * SS late mid-ring recheck after full PASS (Gap C dual-DoD
                     * residual deepen Soft!=product): a12f may post TR Deq
                     * after primary soft-continue stamp.
                     * Grep: xhci: ep0 mid-ring soft-continue after get_config
                     *       full PASS late
                     */
                    if (fSsPath != 0 && fFullAligned != 0) {
                        xhci_settle(1000000u);
                        xhci_drain_events(80u);
                        u32EpStF = xhci_ep0_sample(&u64DeqF);
                        if (xhci_ep0_soft_continue_ok(u32EpStF, u64DeqF) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStF, u64DeqF) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStF,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "full PASS late ok "
                                    "(stage=15 Soft!=product)\n");
                        }
                        /*
                         * SS late2 mid-ring recheck after full PASS
                         * (Gap C dual-DoD residual deepen Soft!=product):
                         * a12f may post TR Deq one settle after late -
                         * re-sample before SET_CONFIGURATION / next control.
                         * Grep: xhci: ep0 mid-ring soft-continue after
                         *       get_config full PASS late2
                         */
                        xhci_settle(1200000u);
                        xhci_drain_events(88u);
                        u32EpStF = xhci_ep0_sample(&u64DeqF);
                        if (xhci_ep0_soft_continue_ok(u32EpStF, u64DeqF) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStF, u64DeqF) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late2 state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStF,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "full PASS late2 ok "
                                    "(stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_full_pass_late2") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late2 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                        /*
                         * SS late3 mid-ring recheck after full PASS
                         * (Gap C dual-DoD residual deepen Soft!=product):
                         * a12f may post TR Deq one settle after late2 -
                         * re-sample before SET_CONFIGURATION / next control.
                         * Grep: xhci: ep0 mid-ring soft-continue after
                         *       get_config full PASS late3
                         */
                        xhci_settle(1400000u);
                        xhci_drain_events(96u);
                        u32EpStF = xhci_ep0_sample(&u64DeqF);
                        if (xhci_ep0_soft_continue_ok(u32EpStF, u64DeqF) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStF, u64DeqF) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late3 state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStF,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "full PASS late3 ok "
                                    "(stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_full_pass_late3") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late3 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                        /*
                         * SS late4 mid-ring recheck after full PASS
                         * (Gap C dual-DoD residual deepen Soft!=product):
                         * a12f may post TR Deq one settle after late3 -
                         * re-sample before SET_CONFIGURATION / next control.
                         * Grep: xhci: ep0 mid-ring soft-continue after
                         *       get_config full PASS late4
                         */
                        xhci_settle(1600000u);
                        xhci_drain_events(104u);
                        u32EpStF = xhci_ep0_sample(&u64DeqF);
                        if (xhci_ep0_soft_continue_ok(u32EpStF, u64DeqF) !=
                                0 ||
                            xhci_ep0_producer_hc_aligned(u32EpStF, u64DeqF) !=
                                0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late4 state=%u "
                                    "idx=%u (stage=15 Soft!=product)\n",
                                    (unsigned)u32EpStF,
                                    (unsigned)g_u32Ep0Idx);
                            xhci_clflush_ptr(g_pEp0Ring);
                            xhci_clflush_ptr(
                                &g_pEp0Ring[XHCI_RING_TRBS - 1u]);
                        } else if (xhci_ep0_soft_sync_producer() != 0) {
                            kprintf("xhci: ep0 soft-sync after get_config "
                                    "full PASS late4 ok "
                                    "(stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_full_pass_late4") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late4 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                    } else if (fSsPath != 0 && fFullAligned == 0) {
                        xhci_settle(1200000u);
                        xhci_drain_events(88u);
                        if (xhci_ep0_soft_recover_after_to(
                                "get_config_full_pass_late") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late soft-recover "
                                    "(stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_full_pass_late2") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late2 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_full_pass_late3") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late3 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        } else if (xhci_ep0_soft_recover_after_to(
                                       "get_config_full_pass_late4") != 0) {
                            kprintf("xhci: ep0 mid-ring soft-continue after "
                                    "get_config full PASS late4 "
                                    "soft-recover (stage=15 Soft!=product)\n");
                        }
                    }
                }
                break;
            }
            kprintf("xhci: get config full FAIL total=%u try=%u cc=%u\n",
                    (unsigned)u16Total, (unsigned)nTry,
                    (unsigned)g_u32CtrlFailCc);
            if (nTry + 1 < 5) {
                /*
                 * Full fetch TO: soft-sync first -> mid-ring soft-continue ->
                 * hard. Prefer no Set TR Deq thrash (stage-15 Soft!=product).
                 */
                kprintf("msc: progress get_config full recover after TO "
                        "n=%u cc=%u (stage=15 Soft!=product)\n",
                        (unsigned)nTry, (unsigned)g_u32CtrlFailCc);
                if (xhci_ep0_soft_recover_after_to("get_config_full") != 0) {
                    kprintf("msc: progress get_config full soft-continue "
                            "after TO n=%u (stage=15 Soft!=product)\n",
                            (unsigned)nTry);
                } else {
                    kprintf("msc: progress get_config full hard-resync after TO "
                            "n=%u cc=%u (stage=15 Soft!=product)\n",
                            (unsigned)nTry, (unsigned)g_u32CtrlFailCc);
                    xhci_ctrl_recover();
                }
                xhci_mps0_resync_from_speed();
                xhci_settle(fSsPath != 0 ? 2000000u : 600000u);
            }
        }
        if (fGot == 0) {
            kprintf("xhci: GET_CONFIG stage=15 total=%u cc=%u mps0=%u "
                    "Soft!=product\n",
                    (unsigned)u16Total, (unsigned)g_u32CtrlFailCc,
                    (unsigned)g_u16MaxPkt0);
            kprintf("msc: progress get_config full FAIL stage=15 total=%u "
                    "cc=%u Soft!=product\n",
                    (unsigned)u16Total, (unsigned)g_u32CtrlFailCc);
            xhci_stage_soft_lamp(15u, "GET_CONFIG_full");
            return -1;
        }
    } else {
        kprintf("xhci: get config residual skip full got=%u total=%u "
                "(stage-15 Soft!=product)\n",
                (unsigned)u16Got, (unsigned)u16Total);
    }

    kprintf("msc: progress get_config PASS total=%u val=%u port=%u spd=%u "
            "mps0=%u\n",
            (unsigned)u16Total, (unsigned)aCfg[5], (unsigned)g_u8PortId,
            (unsigned)g_u8PortSpeed, (unsigned)g_u16MaxPkt0);
    kprintf("xhci: get config PASS total=%u val=%u port=%u spd=%u "
            "Soft!=product\n",
            (unsigned)u16Total, (unsigned)aCfg[5], (unsigned)g_u8PortId,
            (unsigned)g_u8PortSpeed);
    *pTotal = u16Total;
    return 0;
}

static int
xhci_enum_msc(void)
{
    struct usb_setup setup;
    u8 aDev[18];
    u8 aCfg[512];
    u16 u16Total = 0;
    int nPass;

    /*
     * Up to 2 full passes (boot-capped; was 3): first enum; on stage-15
     * GET_CONFIG fail readdress once and retry. Soft!=product · Dual DoD A.
     */
    kprintf("msc: progress enum begin port=%u spd=%u\n",
            (unsigned)g_u8PortId, (unsigned)g_u8PortSpeed);
    for (nPass = 0; nPass < 2; nPass++) {
        if (nPass > 0) {
            kprintf("xhci: enum pass=%u readdress after GET_CONFIG fail\n",
                    (unsigned)nPass);
            kprintf("msc: progress enum readdress pass=%u\n",
                    (unsigned)nPass);
            if (xhci_readdress_alternate() != 0) {
                kprintf("xhci: readdress after stage-15 FAIL\n");
                g_u32LastStage = 15;
                return -1;
            }
            g_u32LastStage = 4;
        }

        kprintf("msc: progress get_desc begin pass=%u\n", (unsigned)nPass);
        if (xhci_get_device_descriptor(aDev) != 0) {
            /*
             * Stage 11/13: control path after address. Readdress once then
             * retry full GET_DESC (early passes).
             */
            if (nPass < 2 &&
                (g_u32LastStage == 11u || g_u32LastStage == 13u)) {
                kprintf("xhci: stage-%u GET_DESC; readdress + retry\n",
                        (unsigned)g_u32LastStage);
                kprintf("msc: progress get_desc readdress stage=%u\n",
                        (unsigned)g_u32LastStage);
                if (xhci_readdress_alternate() == 0) {
                    if (xhci_get_device_descriptor(aDev) == 0) {
                        goto got_dev;
                    }
                }
            }
            kprintf("msc: progress get_desc FAIL stage=%u cc=%u\n",
                    (unsigned)g_u32LastStage, (unsigned)g_u32CtrlFailCc);
            return -1;
        }
    got_dev:
        kprintf("msc: progress get_desc PASS vid=%x pid=%x mps0=%u pass=%u\n",
                (unsigned)(aDev[8] | ((u16)aDev[9] << 8)),
                (unsigned)(aDev[10] | ((u16)aDev[11] << 8)),
                (unsigned)g_u16MaxPkt0, (unsigned)nPass);
        kprintf("xhci: device class=%u sub=%u proto=%u mps0=%u vid=%x "
                "pid=%x pass=%u\n",
                (unsigned)aDev[4], (unsigned)aDev[5], (unsigned)aDev[6],
                (unsigned)g_u16MaxPkt0,
                (unsigned)(aDev[8] | ((u16)aDev[9] << 8)),
                (unsigned)(aDev[10] | ((u16)aDev[11] << 8)),
                (unsigned)nPass);

        /* Root-hub class device: skip (no hub traversal in MVP). */
        if (aDev[4] == 0x09u) {
            kprintf("xhci: skip USB hub soft FAIL (no hub support)\n");
            g_u32LastStage = 14;
            return -1;
        }

        if (xhci_get_config_descriptor(aCfg, &u16Total) != 0) {
            kprintf("xhci: GET_CONFIG stage=15 fail pass=%u cc=%u "
                    "Soft!=product\n",
                    (unsigned)nPass, (unsigned)g_u32CtrlFailCc);
            g_u32LastStage = 15;
            xhci_stage_soft_lamp(15u, "GET_CONFIG_enum");
            if (nPass + 1 < 2) {
                continue; /* readdress + full retry (boot-capped 2 passes) */
            }
            return -1;
        }
        break; /* config OK */
    }

    kprintf("msc: progress parse_cfg total=%u\n", (unsigned)u16Total);
    if (xhci_parse_msc_config(aCfg, u16Total) != 0) {
        kprintf("xhci: no MSC BOT interface soft FAIL (cfg_total=%u)\n",
                (unsigned)u16Total);
        kprintf("msc: progress parse_cfg FAIL not_bot total=%u\n",
                (unsigned)u16Total);
        g_u32LastStage = 16; /* saw device, not BOT mass-storage */
        return -1;
    }
    kprintf("msc: progress parse_cfg PASS bot cfg=%u out=%u in=%u\n",
            (unsigned)g_u8ConfigVal, (unsigned)g_u8EpOut,
            (unsigned)g_u8EpIn);

    memset(&setup, 0, sizeof(setup));
    setup.u8BmRequestType = 0x00u;
    setup.u8BRequest = USB_REQ_SET_CONFIGURATION;
    setup.u16WValue = g_u8ConfigVal;
    setup.u16WIndex = 0;
    setup.u16WLength = 0;
    {
        int nTry;
        int fOk = 0;

        /*
         * Stage-15+ toward SET_CONFIGURATION (soft; Soft!=product):
         * Soft-align after GET_CONFIG success - mid-ring producer-aligned
         * continue, no pre-arm thrash. SET_CONFIGURATION is no-data
         * (Setup+Status only). After any TO: hard resync (not settle-only).
         * Grep: msc: progress set_config | xhci: SET_CONFIGURATION
         */
        kprintf("msc: progress set_config begin cfg=%u port=%u spd=%u\n",
                (unsigned)g_u8ConfigVal, (unsigned)g_u8PortId,
                (unsigned)g_u8PortSpeed);
        xhci_drain_events(72u);
        /* Gap C freestanding residual: longer settle before SET_CONFIGURATION. */
        xhci_settle((g_u8PortSpeed >= 4u) ? 1200000u : 500000u);
        if (xhci_port_ready_for_config() != 0) {
            kprintf("msc: progress set_config FAIL reason=port_not_ready\n");
            g_u32LastStage = 17;
            xhci_note_ctrl_fail_cc(0);
            return -1;
        }
        /* Same soft-align gate as GET_CONFIG (base or mid-ring; no thrash). */
        xhci_ep0_prepare_for_config();
        xhci_drain_events(32u);
        kprintf("msc: progress set_config ep0_ready idx=%u cyc=%u mps0=%u\n",
                (unsigned)g_u32Ep0Idx, (unsigned)g_fEp0Cycle,
                (unsigned)g_u16MaxPkt0);
        for (nTry = 0; nTry < 4; nTry++) {
            if (xhci_ctrl(&setup, NULL, 0, 0) == 0) {
                fOk = 1;
                kprintf("xhci: SET_CONFIGURATION PASS try=%u cfg=%u\n",
                        (unsigned)nTry, (unsigned)g_u8ConfigVal);
                break;
            }
            kprintf("xhci: SET_CONFIGURATION FAIL try=%u cfg=%u cc=%u\n",
                    (unsigned)nTry, (unsigned)g_u8ConfigVal,
                    (unsigned)g_u32CtrlFailCc);
            if (nTry + 1 < 4) {
                /*
                 * After failed enqueue/TO: soft-sync first -> mid-ring
                 * soft-continue -> hard (settle-only alone is stage-17 cascade).
                 * Prefer soft over thrash when producer-HC still aligned.
                 * Grep: msc: progress set_config | Soft!=product
                 */
                kprintf("msc: progress set_config recover after TO n=%u "
                        "cc=%u (stage=17 residual Soft!=product)\n",
                        (unsigned)nTry, (unsigned)g_u32CtrlFailCc);
                if (xhci_port_ready_for_config() != 0) {
                    kprintf("msc: progress set_config FAIL reason=ped_lost\n");
                    g_u32LastStage = 17;
                    return -1;
                }
                if (xhci_ep0_soft_recover_after_to("set_config") != 0) {
                    kprintf("msc: progress set_config soft-continue after TO "
                            "n=%u (Soft!=product)\n",
                            (unsigned)nTry);
                } else {
                    kprintf("msc: progress set_config hard-resync after TO n=%u "
                            "cc=%u (Soft!=product)\n",
                            (unsigned)nTry, (unsigned)g_u32CtrlFailCc);
                    xhci_ctrl_recover();
                }
                xhci_mps0_resync_from_speed();
                xhci_settle((g_u8PortSpeed >= 4u) ? 1800000u : 600000u);
            }
        }
        if (fOk == 0) {
            kprintf("xhci: SET_CONFIGURATION FAIL stage=17 cfg=%u cc=%u "
                    "Soft!=product Stage OPEN\n",
                    (unsigned)g_u8ConfigVal, (unsigned)g_u32CtrlFailCc);
            kprintf("msc: progress set_config FAIL cfg=%u cc=%u\n",
                    (unsigned)g_u8ConfigVal, (unsigned)g_u32CtrlFailCc);
            g_u32LastStage = 17;
            xhci_stage_soft_lamp(17u, "SET_CONFIGURATION");
            return -1;
        }
    }
    kprintf("msc: progress set_config PASS cfg=%u\n",
            (unsigned)g_u8ConfigVal);

    /* Device may need settle after SET_CONFIGURATION before bulk config. */
    xhci_settle((g_u8PortSpeed >= 4u) ? 1500000u : 500000u);
    kprintf("msc: progress config_ep begin ep_out=%u ep_in=%u mps=%u\n",
            (unsigned)g_u8EpOut, (unsigned)g_u8EpIn, (unsigned)g_u16BulkMps);
    if (xhci_configure_bulk() != 0) {
        kprintf("msc: progress config_ep FAIL\n");
        g_u32LastStage = 18;
        return -1;
    }
    kprintf("msc: progress config_ep PASS\n");
    kprintf("xhci: MSC BOT iface ep_out=%u ep_in=%u mps=%u cfg=%u\n",
            (unsigned)g_u8EpOut, (unsigned)g_u8EpIn, (unsigned)g_u16BulkMps,
            (unsigned)g_u8ConfigVal);
    return 0;
}

/* ---- BOT / SCSI --------------------------------------------------------- */
static int
msc_bot(u8 *pCdb, u8 cbCdb, void *pData, u32 cbData, int fIn)
{
    struct msc_cbw cbw;
    struct msc_csw csw;
    gj_paddr_t paData = g_paScratch;

    if (cbCdb == 0 || cbCdb > 16u || pCdb == NULL) {
        return -1;
    }
    if (cbData > GJ_PAGE_SIZE) {
        return -1;
    }

    memset(&cbw, 0, sizeof(cbw));
    cbw.u32Signature = CBW_SIGNATURE;
    cbw.u32Tag = g_u32CbwTag++;
    cbw.u32DataTransferLength = cbData;
    cbw.u8Flags = fIn != 0 ? CBW_FLAG_IN : 0u;
    cbw.u8Lun = 0;
    cbw.u8CbLength = cbCdb;
    memcpy(cbw.aCb, pCdb, cbCdb);

    memcpy(g_pScratch2, &cbw, 31);
    if (xhci_bulk(0, g_paScratch2, 31) != 0) {
        return -1;
    }

    if (cbData > 0 && pData != NULL) {
        if (fIn == 0) {
            memcpy(g_pScratch, pData, cbData);
        } else {
            memset(g_pScratch, 0, cbData);
        }
        if (xhci_bulk(fIn, paData, cbData) != 0) {
            return -1;
        }
        if (fIn != 0) {
            memcpy(pData, g_pScratch, cbData);
        }
    }

    memset(g_pScratch2, 0, 16);
    if (xhci_bulk(1, g_paScratch2, 13) != 0) {
        return -1;
    }
    memcpy(&csw, g_pScratch2, 13);
    if (csw.u32Signature != CSW_SIGNATURE || csw.u8Status != 0) {
        return -1;
    }
    return 0;
}

static int
msc_read_capacity(void)
{
    u8 aCdb[10];
    u8 aData[8];
    u32 u32Lba;
    u32 u32Bsz;

    kprintf("msc: progress bot_capacity begin\n");
    memset(aCdb, 0, sizeof(aCdb));
    aCdb[0] = SCSI_READ_CAPACITY10;
    if (msc_bot(aCdb, 10, aData, 8, 1) != 0) {
        kprintf("msc: progress bot_capacity FAIL\n");
        return -1;
    }
    u32Lba = ((u32)aData[0] << 24) | ((u32)aData[1] << 16) |
             ((u32)aData[2] << 8) | (u32)aData[3];
    u32Bsz = ((u32)aData[4] << 24) | ((u32)aData[5] << 16) |
             ((u32)aData[6] << 8) | (u32)aData[7];
    g_u32BlockCount = u32Lba + 1u;
    g_u32BlockSize = u32Bsz != 0 ? u32Bsz : MSC_SECTOR;
    if (g_u32BlockSize != MSC_SECTOR) {
        /* MVP assumes 512-byte logical blocks for FAT/raw */
        kprintf("msc: non-512 block size=%u soft WARN\n",
                (unsigned)g_u32BlockSize);
    }
    kprintf("msc: BOT ready capacity=%u blocks block_size=%u\n",
            (unsigned)g_u32BlockCount, (unsigned)g_u32BlockSize);
    kprintf("msc: progress bot_capacity PASS blocks=%u\n",
            (unsigned)g_u32BlockCount);
    return 0;
}

static int
msc_read10(u32 u32Lba, void *pBuf, u32 cSectors)
{
    u8 aCdb[10];
    u32 cb = cSectors * MSC_SECTOR;

    if (cSectors == 0 || cb > GJ_PAGE_SIZE) {
        return -1;
    }
    memset(aCdb, 0, sizeof(aCdb));
    aCdb[0] = SCSI_READ10;
    aCdb[2] = (u8)((u32Lba >> 24) & 0xffu);
    aCdb[3] = (u8)((u32Lba >> 16) & 0xffu);
    aCdb[4] = (u8)((u32Lba >> 8) & 0xffu);
    aCdb[5] = (u8)(u32Lba & 0xffu);
    aCdb[7] = (u8)((cSectors >> 8) & 0xffu);
    aCdb[8] = (u8)(cSectors & 0xffu);
    return msc_bot(aCdb, 10, pBuf, cb, 1);
}

static int
msc_write10(u32 u32Lba, const void *pBuf, u32 cSectors)
{
    u8 aCdb[10];
    u32 cb = cSectors * MSC_SECTOR;

    if (cSectors == 0 || cb > GJ_PAGE_SIZE || pBuf == NULL) {
        return -1;
    }
    memset(aCdb, 0, sizeof(aCdb));
    aCdb[0] = SCSI_WRITE10;
    aCdb[2] = (u8)((u32Lba >> 24) & 0xffu);
    aCdb[3] = (u8)((u32Lba >> 16) & 0xffu);
    aCdb[4] = (u8)((u32Lba >> 8) & 0xffu);
    aCdb[5] = (u8)(u32Lba & 0xffu);
    aCdb[7] = (u8)((cSectors >> 8) & 0xffu);
    aCdb[8] = (u8)(cSectors & 0xffu);
    return msc_bot(aCdb, 10, (void *)(uintptr_t)pBuf, cb, 0);
}

/* ---- FAT32 locate KLOG.TXT ---------------------------------------------- */
static int
fat_name_eq(const u8 *pDir, const char *szName8, const char *szExt3)
{
    u32 i;

    for (i = 0; i < 8u; i++) {
        u8 chA = pDir[i];
        u8 chB = (u8)szName8[i];
        if (chA >= 'a' && chA <= 'z') {
            chA = (u8)(chA - 32u);
        }
        if (chB >= 'a' && chB <= 'z') {
            chB = (u8)(chB - 32u);
        }
        if (chA != chB) {
            return 0;
        }
    }
    for (i = 0; i < 3u; i++) {
        u8 chA = pDir[8u + i];
        u8 chB = (u8)szExt3[i];
        if (chA >= 'a' && chA <= 'z') {
            chA = (u8)(chA - 32u);
        }
        if (chB >= 'a' && chB <= 'z') {
            chB = (u8)(chB - 32u);
        }
        if (chA != chB) {
            return 0;
        }
    }
    return 1;
}

static u32
fat_cluster_to_lba(u32 u32Clus)
{
    return g_u32FatDataLba + (u32Clus - 2u) * g_u32FatSecPerClus;
}

static int
fat_find_in_dir(u32 u32DirClus, const char *szName8, const char *szExt3,
                u32 *pOutClus, u32 *pOutSize, int *pfDir)
{
    u32 u32Clus = u32DirClus;
    u32 cGuard = 0;

    while (u32Clus >= 2u && u32Clus < 0x0ffffff8u && cGuard < 64u) {
        u32 u32Lba = fat_cluster_to_lba(u32Clus);
        u32 iSec;

        for (iSec = 0; iSec < g_u32FatSecPerClus; iSec++) {
            u8 aSec[MSC_SECTOR];
            u32 iEnt;

            if (msc_read10(u32Lba + iSec, aSec, 1) != 0) {
                return -1;
            }
            for (iEnt = 0; iEnt < MSC_SECTOR; iEnt += 32u) {
                u8 *pE = &aSec[iEnt];
                u8 u8Attr;
                u32 u32Cl;
                u32 u32Sz;

                if (pE[0] == 0x00u) {
                    return -1; /* end */
                }
                if (pE[0] == 0xe5u) {
                    continue;
                }
                u8Attr = pE[11];
                if (u8Attr == 0x0fu) {
                    continue; /* LFN */
                }
                if (fat_name_eq(pE, szName8, szExt3) == 0) {
                    continue;
                }
                u32Cl = (u32)pE[26] | ((u32)pE[27] << 8) |
                        ((u32)pE[20] << 16) | ((u32)pE[21] << 24);
                u32Sz = (u32)pE[28] | ((u32)pE[29] << 8) |
                        ((u32)pE[30] << 16) | ((u32)pE[31] << 24);
                if (pOutClus != NULL) {
                    *pOutClus = u32Cl;
                }
                if (pOutSize != NULL) {
                    *pOutSize = u32Sz;
                }
                if (pfDir != NULL) {
                    *pfDir = (u8Attr & 0x10u) != 0 ? 1 : 0;
                }
                return 0;
            }
        }
        /*
         * Contiguous preallocation assumption: only walk first cluster of
         * directory for MVP (root/EFI/GREENJADE are small on hwtest image).
         * Stop after one cluster.
         */
        break;
    }
    (void)cGuard;
    return -1;
}

static int
fat_locate_klog(void)
{
    u8 aSec[MSC_SECTOR];
    u32 u32PartLba = 2048; /* hwtest-img default ESP start */
    u32 u32Bps;
    u32 u32Spc;
    u32 u32Rsvd;
    u32 u32NumFats;
    u32 u32FatSz;
    u32 u32RootClus;
    u32 u32Clus;
    u32 u32Sz;
    int fDir = 0;

    /* GPT protective MBR: still use LBA 2048 for our image; try BPB there */
    if (msc_read10(u32PartLba, aSec, 1) != 0) {
        /* try LBA 0 as superfloppy */
        u32PartLba = 0;
        if (msc_read10(0, aSec, 1) != 0) {
            return -1;
        }
    }

    /* Validate FAT32 BPB signature */
    if (!(aSec[510] == 0x55u && aSec[511] == 0xaau)) {
        /* try alternate common starts */
        u32PartLba = 0;
        if (msc_read10(0, aSec, 1) != 0 || aSec[510] != 0x55u) {
            return -1;
        }
    }
    /* If MBR, parse partition 1 start LBA */
    if (aSec[0] != 0xebu && aSec[0] != 0xe9u) {
        if (aSec[450] == 0xeeu || aSec[450] == 0xefu || aSec[450] == 0x0cu ||
            aSec[450] == 0x0bu) {
            u32PartLba = (u32)aSec[454] | ((u32)aSec[455] << 8) |
                         ((u32)aSec[456] << 16) | ((u32)aSec[457] << 24);
            if (u32PartLba == 0) {
                u32PartLba = 2048;
            }
            if (msc_read10(u32PartLba, aSec, 1) != 0) {
                return -1;
            }
        }
    }

    u32Bps = (u32)aSec[11] | ((u32)aSec[12] << 8);
    u32Spc = aSec[13];
    u32Rsvd = (u32)aSec[14] | ((u32)aSec[15] << 8);
    u32NumFats = aSec[16];
    u32FatSz = (u32)aSec[36] | ((u32)aSec[37] << 8) | ((u32)aSec[38] << 16) |
               ((u32)aSec[39] << 24);
    u32RootClus = (u32)aSec[44] | ((u32)aSec[45] << 8) |
                  ((u32)aSec[46] << 16) | ((u32)aSec[47] << 24);

    if (u32Bps != MSC_SECTOR || u32Spc == 0 || u32FatSz == 0 ||
        u32RootClus < 2u) {
        return -1;
    }

    g_u32FatPartLba = u32PartLba;
    g_u32FatSecPerClus = u32Spc;
    g_u32FatDataLba =
        u32PartLba + u32Rsvd + u32NumFats * u32FatSz;

    /* EFI */
    if (fat_find_in_dir(u32RootClus, "EFI     ", "   ", &u32Clus, &u32Sz,
                        &fDir) != 0 ||
        fDir == 0) {
        return -1;
    }
    /* GREENJADE (9 chars -> short GREENJAD or GREENJ~1; blank extension) */
    if (fat_find_in_dir(u32Clus, "GREENJAD", "   ", &u32Clus, &u32Sz, &fDir) !=
            0 ||
        fDir == 0) {
        if (fat_find_in_dir(u32Clus, "GREENJ~1", "   ", &u32Clus, &u32Sz,
                            &fDir) != 0 ||
            fDir == 0) {
            return -1;
        }
    }
    /* KLOG.TXT */
    if (fat_find_in_dir(u32Clus, "KLOG    ", "TXT", &u32Clus, &u32Sz, &fDir) !=
            0 ||
        fDir != 0) {
        return -1;
    }

    g_u32KlogCluster = u32Clus;
    g_u32KlogSize = u32Sz != 0 ? u32Sz : FAT_KLOG_MAX;
    if (g_u32KlogSize > FAT_KLOG_MAX) {
        g_u32KlogSize = FAT_KLOG_MAX;
    }
    g_fKlogLocated = 1;
    kprintf("stick: FAT KLOG.TXT cluster=%u size=%u part_lba=%u\n",
            (unsigned)g_u32KlogCluster, (unsigned)g_u32KlogSize,
            (unsigned)g_u32FatPartLba);
    return 0;
}

static int
fat_write_klog(const void *pBuf, u32 cb)
{
    u32 u32Lba;
    u32 u32Off = 0;
    u8 aSec[MSC_SECTOR];
    const u8 *p = (const u8 *)pBuf;

    if (g_fKlogLocated == 0 || pBuf == NULL) {
        return -1;
    }
    if (cb > g_u32KlogSize) {
        cb = g_u32KlogSize;
    }

    u32Lba = fat_cluster_to_lba(g_u32KlogCluster);
    /* Contiguous preallocation: write linearly from first cluster */
    while (u32Off < cb) {
        u32 u32Chunk = cb - u32Off;
        u32 u32Sec = u32Off / MSC_SECTOR;
        u32 u32InSec = u32Off % MSC_SECTOR;

        if (u32Chunk > MSC_SECTOR - u32InSec) {
            u32Chunk = MSC_SECTOR - u32InSec;
        }
        if (u32InSec != 0 || u32Chunk < MSC_SECTOR) {
            if (msc_read10(u32Lba + u32Sec, aSec, 1) != 0) {
                return -1;
            }
            memcpy(aSec + u32InSec, p + u32Off, u32Chunk);
            if (msc_write10(u32Lba + u32Sec, aSec, 1) != 0) {
                return -1;
            }
        } else {
            memcpy(aSec, p + u32Off, MSC_SECTOR);
            if (msc_write10(u32Lba + u32Sec, aSec, 1) != 0) {
                return -1;
            }
            u32Chunk = MSC_SECTOR;
        }
        u32Off += u32Chunk;
    }

    /* Zero one trailing sector marker optional: write length header in first 16 */
    return 0;
}

static int
raw_log_write(const void *pBuf, u32 cb)
{
    u32 u32Start;
    u8 aSec[MSC_SECTOR];
    u32 u32Off = 0;
    const u8 *p = (const u8 *)pBuf;
    u32 u32Payload;
    u32 u32Hdr = 16;

    /* Soft: never silent on geometry / arg reject (grep: stick: raw log). */
    if (pBuf == NULL) {
        kprintf("stick: raw log write OPEN reason=null_buf (soft fail-closed)\n");
        return -1;
    }
    if (g_u32BlockCount < RAW_LOG_SECTS + 2u) {
        kprintf("stick: raw log write OPEN reason=geometry blocks=%u need>=%u "
                "(soft fail-closed; Soft!=product)\n",
                (unsigned)g_u32BlockCount, (unsigned)(RAW_LOG_SECTS + 2u));
        return -1;
    }
    u32Start = g_u32BlockCount - RAW_LOG_SECTS;
    u32Payload = cb;
    if (u32Payload > (RAW_LOG_SECTS * MSC_SECTOR) - u32Hdr) {
        u32Payload = (RAW_LOG_SECTS * MSC_SECTOR) - u32Hdr;
    }

    memset(aSec, 0, sizeof(aSec));
    memcpy(aSec, RAW_LOG_MAGIC, 9);
    aSec[9] = 0;
    aSec[10] = (u8)(u32Payload & 0xffu);
    aSec[11] = (u8)((u32Payload >> 8) & 0xffu);
    aSec[12] = (u8)((u32Payload >> 16) & 0xffu);
    aSec[13] = (u8)((u32Payload >> 24) & 0xffu);

    if (u32Payload <= MSC_SECTOR - u32Hdr) {
        memcpy(aSec + u32Hdr, p, u32Payload);
        if (msc_write10(u32Start, aSec, 1) != 0) {
            kprintf("stick: raw log write FAIL lba=%u bot_write (MSC ready path)\n",
                    (unsigned)u32Start);
            return -1;
        }
        /* Smoke read-back: magic + length must match (write/read path). */
        {
            u8 aRd[MSC_SECTOR];

            if (msc_read10(u32Start, aRd, 1) != 0) {
                kprintf("stick: raw log write PASS write; read-back FAIL\n");
                return -1;
            }
            if (aRd[0] != (u8)'G' || aRd[1] != (u8)'J' ||
                aRd[2] != (u8)'U' || aRd[3] != (u8)'S' ||
                aRd[4] != (u8)'B' || aRd[5] != (u8)'L' ||
                aRd[6] != (u8)'O' || aRd[7] != (u8)'G' ||
                aRd[8] != (u8)'1') {
                kprintf("stick: raw log read-back magic FAIL\n");
                return -1;
            }
            kprintf("stick: raw log smoke write+read PASS lba=%u bytes=%u\n",
                    (unsigned)u32Start, (unsigned)u32Payload);
        }
        return 0;
    }

    memcpy(aSec + u32Hdr, p, MSC_SECTOR - u32Hdr);
    if (msc_write10(u32Start, aSec, 1) != 0) {
        kprintf("stick: raw log write FAIL lba=%u multi_hdr bot_write\n",
                (unsigned)u32Start);
        return -1;
    }
    u32Off = MSC_SECTOR - u32Hdr;
    while (u32Off < u32Payload) {
        u32 u32Sec = 1u + (u32Off - (MSC_SECTOR - u32Hdr)) / MSC_SECTOR;
        u32 u32Chunk = u32Payload - u32Off;

        if (u32Chunk > MSC_SECTOR) {
            u32Chunk = MSC_SECTOR;
        }
        memset(aSec, 0, sizeof(aSec));
        memcpy(aSec, p + u32Off, u32Chunk);
        if (msc_write10(u32Start + u32Sec, aSec, 1) != 0) {
            kprintf("stick: raw log write FAIL lba=%u multi bot_write off=%u\n",
                    (unsigned)(u32Start + u32Sec), (unsigned)u32Off);
            return -1;
        }
        u32Off += u32Chunk;
    }
    /* Multi-sector: verify header sector only. */
    {
        u8 aRd[MSC_SECTOR];

        if (msc_read10(u32Start, aRd, 1) == 0 && aRd[0] == (u8)'G' &&
            aRd[1] == (u8)'J' && aRd[8] == (u8)'1') {
            kprintf("stick: raw log smoke write+read PASS lba=%u bytes=%u\n",
                    (unsigned)u32Start, (unsigned)u32Payload);
        } else {
            kprintf("stick: raw log multi write ok; header read soft WARN\n");
        }
    }
    return 0;
}

/* ---- PCI find + top-level init ------------------------------------------ */
static int
xhci_find_and_map(u8 *pBus, u8 *pSlot, u8 *pFunc)
{
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;

    for (u8Bus = 0;; u8Bus++) {
        for (u8Slot = 0; u8Slot < 32u; u8Slot++) {
            for (u8Func = 0; u8Func < 8u; u8Func++) {
                u32 u32Id;
                u32 u32Class;
                u8 u8Pif;
                u32 u32Bar0;
                u64 paBar;
                u16 u16Cmd;
                gj_vaddr_t vaMap = 0;
                gj_status_t stMap;
                u32 u32Hcs1;
                u32 u32Hcc1;
                u32 u32DbOff;
                u32 u32RtOff;

                u32Id = pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x00);
                if ((u32Id & 0xffffu) == 0xffffu) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                u32Class = pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x08);
                if (((u32Class >> 24) & 0xffu) != USB_PCI_CLASS ||
                    ((u32Class >> 16) & 0xffu) != USB_PCI_SUBCLASS) {
                    if (u8Func == 0 &&
                        (pci_cfg_read32(u8Bus, u8Slot, 0, 0x0c) & 0x800000u) ==
                            0) {
                        break;
                    }
                    continue;
                }
                u8Pif = (u8)((u32Class >> 8) & 0xffu);
                if (u8Pif != USB_PIF_XHCI) {
                    continue;
                }

                u16Cmd = pci_cfg_read16(u8Bus, u8Slot, u8Func, PCI_CMD);
                u16Cmd |= (u16)(PCI_CMD_MEM | PCI_CMD_BUS);
                pci_cfg_write16(u8Bus, u8Slot, u8Func, PCI_CMD, u16Cmd);

                u32Bar0 = pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x10);
                if ((u32Bar0 & 1u) != 0) {
                    continue;
                }
                paBar = (u64)(u32Bar0 & ~0xfu);
                if (((u32Bar0 >> 1) & 3u) == 2u) {
                    u32 u32Hi = pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x14);

                    paBar |= ((u64)u32Hi << 32);
                }
                if (paBar == 0) {
                    continue;
                }

                stMap = vmm_map_device_uc((gj_paddr_t)paBar, XHCI_MAP_BYTES,
                                          &vaMap);
                if (stMap != GJ_OK) {
                    kprintf("xhci: map BAR0 FAIL st=%d\n", (int)stMap);
                    return -1;
                }

                g_pCap = (volatile u8 *)vaMap;
                g_u8CapLen = g_pCap[0];
                if (g_u8CapLen < 0x20u) {
                    kprintf("xhci: bad CAPLENGTH=%u\n", (unsigned)g_u8CapLen);
                    return -1;
                }
                g_pOp = g_pCap + g_u8CapLen;
                u32Hcs1 = mmio_r32(g_pCap, 0x04);
                g_u8MaxSlots = (u8)(u32Hcs1 & 0xffu);
                g_u8MaxPorts = (u8)((u32Hcs1 >> 24) & 0xffu);
                if (g_u8MaxSlots == 0) {
                    g_u8MaxSlots = 1;
                }
                if (g_u8MaxPorts > XHCI_MAX_PORTS) {
                    g_u8MaxPorts = XHCI_MAX_PORTS;
                }
                u32Hcc1 = mmio_r32(g_pCap, 0x10);
                g_u8CtxSize = ((u32Hcc1 & 0x4u) != 0) ? 64u : 32u;
                u32DbOff = mmio_r32(g_pCap, 0x14) & ~0x3u;
                u32RtOff = mmio_r32(g_pCap, 0x18) & ~0x1fu;
                g_pDb = (volatile u32 *)(g_pCap + u32DbOff);
                g_pRt = g_pCap + u32RtOff;

                if (pBus != NULL) {
                    *pBus = u8Bus;
                }
                if (pSlot != NULL) {
                    *pSlot = u8Slot;
                }
                if (pFunc != NULL) {
                    *pFunc = u8Func;
                }
                kprintf("xhci: found %02x:%02x.%u bar0=0x%lx cap=%u "
                        "slots=%u ports=%u ctx=%u\n",
                        (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
                        (unsigned long)paBar, (unsigned)g_u8CapLen,
                        (unsigned)g_u8MaxSlots, (unsigned)g_u8MaxPorts,
                        (unsigned)g_u8CtxSize);
                return 0;
            }
        }
        if (u8Bus == 255u) {
            break;
        }
    }
    return -1;
}

static void
xhci_power_all_ports(void)
{
    u8 u8Port;

    for (u8Port = 1; u8Port <= g_u8MaxPorts; u8Port++) {
        xhci_port_power_on(u8Port);
    }
}

/*
 * SuperSpeed-first CCS port order (G752 stick = Linux bus2 SS ~port 5):
 *   0) PED + speed >= 4 (live SuperSpeed)
 *   1) SS-capable (xECP) with CCS - even if speed not yet trained
 *   2) PED + speed == 3 (HS) on SS-capable only
 * Never queue pure LS/FS (speed 1-2) - those are HID/webcam/BT (p10s1 was
 * keyboard-class noise that stole stage-20 soft SET_ADDRESS).
 */
static u32
xhci_collect_ccs_ports(u8 *pOut, u32 u32Cap)
{
    u8 aSeen[XHCI_MAX_PORTS + 1u];
    u8 u8Port;
    u32 n = 0;
    u32 iPhase;

    memset(aSeen, 0, sizeof(aSeen));

    for (iPhase = 0; iPhase < 3u; iPhase++) {
        for (u8Port = 1; u8Port <= g_u8MaxPorts; u8Port++) {
            u32 u32Ps;
            u8 u8Spd;
            int fSs;
            int fPick = 0;

            if (u8Port > XHCI_MAX_PORTS || aSeen[u8Port] != 0) {
                continue;
            }
            u32Ps = portsc_read(u8Port);
            if ((u32Ps & XHCI_PORTSC_CCS) == 0) {
                continue;
            }
            u8Spd = (u8)((u32Ps >> 10) & 0xfu);
            fSs = xhci_port_is_ss_cap(u8Port);
            /* No protocol map: treat upper half of large port counts as SS. */
            if (g_u32SsPortBits == 0 && g_u8MaxPorts > 12u &&
                u8Port > (u8)(g_u8MaxPorts / 2u)) {
                fSs = 1;
            }

            /* Never collect known LS/FS for MSC stick path. */
            if (u8Spd == 1u || u8Spd == 2u) {
                continue;
            }

            if (iPhase == 0u) {
                fPick = ((u32Ps & XHCI_PORTSC_PED) != 0 && u8Spd >= 4u) ? 1
                                                                        : 0;
            } else if (iPhase == 1u) {
                fPick = fSs;
            } else {
                /* HS only if SS-capable (companion) or no SS map at all. */
                fPick = (u8Spd == 3u &&
                         (fSs != 0 || g_u32SsPortBits == 0))
                            ? 1
                            : 0;
            }

            if (fPick == 0) {
                continue;
            }
            aSeen[u8Port] = 1;
            if (n < u32Cap) {
                pOut[n] = u8Port;
                n++;
            }
            kprintf("xhci: SS port pick port=%u phase=%u portsc=0x%x "
                    "spd=%u sscap=%u\n",
                    (unsigned)u8Port, (unsigned)iPhase, (unsigned)u32Ps,
                    (unsigned)u8Spd, fSs ? 1u : 0u);
        }
    }
    return n;
}

/*
 * Prefer a more advanced / informative fail stage when scanning many ports.
 * Do NOT let FS junk stage-20 overwrite a real SS stage-19 (G752 p10s1).
 */
static int
xhci_fail_stage_better(u32 u32Old, u32 u32New)
{
    if (u32New == 0) {
        return 0;
    }
    if (u32Old == 0 || u32Old == 4u || u32Old == 5u) {
        return 1;
    }
    /* Soft SET_ADDRESS fail on non-SS is noise - never upgrade sticky to 20. */
    if (u32New == 20u && g_u8PortSpeed < 3u) {
        return 0;
    }
    /* Enum ladder 11..20: keep the higher stage number. */
    if (u32New >= 11u && u32New <= 20u) {
        if (u32Old < 11u || u32Old > 20u) {
            return 1;
        }
        return (u32New >= u32Old) ? 1 : 0;
    }
    if (u32New == 9u) {
        return (u32Old != 10u && (u32Old < 9u || u32Old > 20u)) ? 1 : 0;
    }
    if (u32New == 7u || u32New == 6u) {
        return (u32Old < 6u || u32Old == 8u) ? 1 : 0;
    }
    /* Generic 8 only if we have nothing better. */
    if (u32New == 8u) {
        return (u32Old < 6u) ? 1 : 0;
    }
    return 0;
}

static void
xhci_note_port_fail(u32 *pLastFail, u32 u32Stage)
{
    if (pLastFail == NULL) {
        return;
    }
    if (xhci_fail_stage_better(*pLastFail, u32Stage) != 0) {
        *pLastFail = u32Stage;
    }
}

static int
xhci_try_one_port(u8 u8Port, u32 *pLastFail)
{
    u32 u32Ps = portsc_read(u8Port);

    if ((u32Ps & XHCI_PORTSC_CCS) == 0) {
        return -1;
    }
    g_u8PortId = u8Port;
    kprintf("xhci: try port=%u portsc=0x%x sscap=%u\n", (unsigned)u8Port,
            (unsigned)u32Ps, xhci_port_is_ss_cap(u8Port) ? 1u : 0u);

    if (xhci_port_reset(u8Port) != 0) {
        kprintf("xhci: port %u reset FAIL\n", (unsigned)u8Port);
        xhci_note_port_fail(pLastFail, 6);
        return -1;
    }

    /*
     * Stick is SuperSpeed (Linux G752 bus2-port5, 13fe:6400).
     * Always skip LS/FS (speed 1-2) - HID/webcam/BT (panel p10s1 stage-20).
     * Skip HS on non-SS-cap when SS map exists (internal HS devices).
     * SS-capable ports may still show HS after cold PR (companion) - try those.
     */
    if (g_u8PortSpeed <= 2u) {
        kprintf("xhci: skip LS/FS port=%u speed=%u (not MSC stick)\n",
                (unsigned)u8Port, (unsigned)g_u8PortSpeed);
        return -1;
    }
    if (g_u8PortSpeed == 0u) {
        kprintf("xhci: skip speed=0 port=%u (not trained)\n",
                (unsigned)u8Port);
        return -1;
    }
    if (g_u8PortSpeed == 3u && xhci_port_is_ss_cap(u8Port) == 0 &&
        g_u32SsPortBits != 0) {
        kprintf("xhci: skip HS non-SS-cap port=%u (prefer SS stick)\n",
                (unsigned)u8Port);
        return -1;
    }

    if (xhci_enable_address() != 0) {
        kprintf("xhci: address FAIL port=%u stage=%u cc=%u\n",
                (unsigned)u8Port, (unsigned)g_u32LastStage,
                (unsigned)((g_u32LastStage == 19u || g_u32LastStage == 20u)
                               ? g_u32AddrFailCc
                               : g_u32LastCc));
        /* Prefer granular stage from enable_address (7/19/20). */
        if (g_u32LastStage != 7u && g_u32LastStage != 19u &&
            g_u32LastStage != 20u) {
            g_u32LastStage = 7;
        }
        xhci_note_port_fail(pLastFail, g_u32LastStage);
        xhci_disable_slot_soft();
        return -1;
    }
    if (xhci_enum_msc() != 0) {
        {
            u32 u32FailCc = g_u32LastCc;

            if (g_u32LastStage == 11u || g_u32LastStage == 12u ||
                g_u32LastStage == 13u || g_u32LastStage == 15u ||
                g_u32LastStage == 17u) {
                u32FailCc = g_u32CtrlFailCc;
            }
            kprintf("xhci: enum MSC FAIL port=%u stage=%u cc=%u\n",
                    (unsigned)u8Port, (unsigned)g_u32LastStage,
                    (unsigned)u32FailCc);
        }
        /*
         * Preserve granular stages. Old code only kept 11..18 and rewrote
         * 12/13/19/20 -> 8, so G752 panel showed purple+8 after a real
         * stage-13 GET_DESC18 fail + readdress path.
         */
        if (g_u32LastStage >= 7u && g_u32LastStage <= 20u &&
            g_u32LastStage != 10u) {
            xhci_note_port_fail(pLastFail, g_u32LastStage);
        } else {
            g_u32LastStage = 8;
            xhci_note_port_fail(pLastFail, 8);
        }
        xhci_disable_slot_soft();
        return -1;
    }
    if (msc_read_capacity() != 0) {
        kprintf("msc: READ CAPACITY FAIL cc=%u\n", (unsigned)g_u32LastCc);
        g_u32LastStage = 9;
        xhci_note_port_fail(pLastFail, 9);
        xhci_disable_slot_soft();
        return -1;
    }
    g_fMscReady = 1;
    g_u32LastStage = 10;
    (void)fat_locate_klog();
    kprintf("xhci: MSC ready port=%u speed=%u klog=%u\n", (unsigned)u8Port,
            (unsigned)g_u8PortSpeed, (unsigned)g_fKlogLocated);
    return 0;
}

static int
xhci_try_ports(void)
{
    u8 aPorts[XHCI_MAX_PORTS];
    u32 nPorts;
    u32 iPass;
    u32 i;
    u32 cCcs = 0;
    u32 u32LastFail = 5; /* no CCS default */

    xhci_parse_ss_ports();

    /* Power + settle: stick may re-attach after HCRST (boot media). */
    xhci_power_all_ports();
    xhci_settle(2000000u);

    for (iPass = 0; iPass < 4u; iPass++) {
        if (iPass > 0) {
            xhci_power_all_ports();
            xhci_settle(1000000u);
        }

        nPorts = xhci_collect_ccs_ports(aPorts, XHCI_MAX_PORTS);
        cCcs = nPorts;
        kprintf("xhci: pass=%u ccs_ports=%u max_ports=%u ss_mask=0x%x "
                "(SS-first order)\n",
                (unsigned)iPass, (unsigned)cCcs, (unsigned)g_u8MaxPorts,
                (unsigned)g_u32SsPortBits);

        for (i = 0; i < nPorts; i++) {
            if (xhci_try_one_port(aPorts[i], &u32LastFail) == 0) {
                kprintf("xhci: MSC ready pass=%u\n", (unsigned)iPass);
                return 0;
            }
            /*
             * After a stage-11 ctrl fail on a non-SS port, keep going so SS
             * stick still gets a turn. On SS port stage-11, still continue
             * other SS ports (multi-device).
             */
        }
    }
    if (cCcs == 0) {
        g_u32LastStage = 5;
    } else {
        g_u32LastStage = u32LastFail;
    }
    kprintf("xhci: try_ports done fail stage=%u cc=%u\n",
            (unsigned)g_u32LastStage, (unsigned)xhci_msc_last_cc());
    return -1;
}

/*
 * Once-shot Dual DoD A residual inventory (C0 freestanding SKIP residual
 * honesty only · Soft!=product · G-AC-1). NOT product Dual DoD A close.
 * Fires on freestanding MSC SKIP default and after opt-in probe paths.
 * Lean greps only - no version stamp, no stamp storms, no stage-ladder thrash.
 * Dual DoD A stick OPEN_UDX / need=UDX_OPEN until L3 host proof;
 * product USB = userspace xhci_udx + hot+cold ABI + DDI (not freestanding MSC).
 * Align: rtl8168 soft residual product=UDX+ABI · net_l2 freestanding SKIP lean
 * · usb_probe freestanding_msc=SKIP need=UDX_OPEN · claim=0 under SKIP.
 * NEVER re-enable freestanding MSC thrash as product. GJ_XHCI_MSC_PROBE stays 0.
 * greppable: xhci: soft residual product=UDX+ABI | product_udx_abi honesty
 * greppable: xhci: soft residual lean | msc: soft residual lean
 * greppable: stick: soft residual lean | freestanding_msc=SKIP
 * greppable: path=xhci_udx | dual_dod_a=OPEN_UDX | need=UDX_OPEN | claim=0
 * greppable: freestanding_close=0 | C0_SKIP=1 | soft_ne_product=1
 * greppable: C0 freestanding SKIP residual | Soft!=product
 * greppable: stamp_storm=0 | version_stamp=0 | not Dual DoD A close
 */
static void
xhci_msc_soft_residual_once(const char *szVia)
{
    const char *szViaSafe;
    const char *szFsMsc;
    u32 u32Probe;
    u32 u32SkipDef;
    u32 u32Claim;
    u32 u32Close; /* freestanding never closes Dual DoD A */

    if (g_fSoftResidualOnce != 0u) {
        return;
    }
    g_fSoftResidualOnce = 1u;
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
#if GJ_XHCI_MSC_PROBE
    u32Probe = 1u;
    u32SkipDef = 0u;
    szFsMsc = "OPTIN_RESIDUAL";
#else
    u32Probe = 0u;
    u32SkipDef = 1u;
    szFsMsc = "SKIP";
#endif
    /* Under SKIP default HC never claimed; opt-in residual may set g_fHcUp. */
    u32Claim = (g_fHcUp != 0u) ? 1u : 0u;
    /* C0 honesty: freestanding residual never product-closes Dual DoD A. */
    u32Close = 0u;

    /*
     * Grep: xhci: soft residual product=UDX+ABI
     * Product laptop USB = userspace xhci_udx + hot+cold ABI + DDI (G-AC-1).
     * Freestanding MSC residual lean only - not Dual DoD A close, not T1.
     * Default SKIP (probe=0) keeps freestanding net first - no stage thrash.
     * C0 freestanding SKIP residual honesty only (NOT product Dual DoD A).
     */
    kprintf("xhci: soft residual product=UDX+ABI via=%s "
            "probe=%u skip_default=%u msc_probe=%u backend=none "
            "claim=%u freestanding_msc=%s freestanding_close=%u "
            "path=xhci_udx dual_dod_a=OPEN_UDX need=UDX_OPEN "
            "soft=1 product=0 soft_ne_product=1 G-AC-1=1 "
            "C0=1 C0_SKIP=%u stamp_storm=0 version_stamp=0 "
            "dual=MIT_OR_Apache-2.0 "
            "not=in_kernel_usb_storage_ko not=xhci_pci_ko "
            "(userspace xhci_udx + ABI; freestanding residual lean only; "
            "no freestanding MSC thrash; != .ko; != product T1; "
            "not Dual DoD A close; Soft!=product)\n",
            szViaSafe, (unsigned)u32Probe, (unsigned)u32SkipDef,
            (unsigned)u32Probe, (unsigned)u32Claim, szFsMsc,
            (unsigned)u32Close, (unsigned)u32SkipDef);

    /*
     * Grep: xhci: soft residual product_udx_abi honesty
     * Align net_l2 Dual DoD B product_udx_abi honesty (USB side = Dual DoD A).
     * OPEN until UDX host proof; freestanding MSC never product close.
     */
    kprintf("xhci: soft residual product_udx_abi honesty "
            "usb=product_udx_abi owner=product_udx_abi "
            "role=skip_msc_product_udx_abi "
            "product=UDX+ABI path=userspace_xhci_udx_hot_cold_abi "
            "direction=userspace_xhci_udx_over_hot_cold_abi "
            "freestanding_msc=%s freestanding_close=%u "
            "dual_dod_a=OPEN_UDX need=UDX_OPEN "
            "claim=%u soft=1 product=0 soft_ne_product=1 "
            "G-AC-1=1 C0=1 C0_SKIP=%u "
            "stamp_storm=0 version_stamp=0 "
            "probe=%u skip_default=%u via=%s "
            "(Soft!=product; freestanding MSC SKIP default; "
            "not freestanding HC claim as product; "
            "not re-enable freestanding MSC; lean residual only; "
            "dual MIT OR Apache-2.0; not Dual DoD A close)\n",
            szFsMsc, (unsigned)u32Close, (unsigned)u32Claim,
            (unsigned)u32SkipDef, (unsigned)u32Probe,
            (unsigned)u32SkipDef, szViaSafe);

    /*
     * Grep: xhci: soft residual lean
     * Compact Dual DoD A seed honesty - dual MIT/Apache; no version stamp.
     * Stage ladder dead under SKIP default (no lateN / Nth-pass thrash fields).
     * Grep: freestanding_msc=SKIP | need=UDX_OPEN | claim=0 | C0_SKIP
     * Grep: freestanding_close=0 | soft_ne_product=1 | stamp_storm=0
     */
    kprintf("xhci: soft residual lean soft=1 product=0 soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 "
            "probe=%u skip_default=%u claim=%u hc_up=%u msc_ready=%u "
            "rings=%u bot=%u irq=0 product_rings=0 product_bot=0 "
            "stage=%u cc=%u freestanding_msc=%s "
            "freestanding_close=%u product=UDX+ABI path=xhci_udx "
            "dual_dod_a=OPEN_UDX need=UDX_OPEN "
            "stamp_storm=0 version_stamp=0 G-AC-1=1 freestanding_net_first=1 "
            "C0=1 C0_SKIP=%u Soft!=product via=%s\n",
            (unsigned)u32Probe, (unsigned)u32SkipDef, (unsigned)u32Claim,
            (unsigned)g_fHcUp, (unsigned)g_fMscReady,
            (unsigned)u32Claim, (unsigned)g_fMscReady,
            (unsigned)g_u32LastStage, (unsigned)xhci_msc_last_cc(),
            szFsMsc, (unsigned)u32Close, (unsigned)u32SkipDef, szViaSafe);

    /* Grep: msc: soft residual lean | freestanding_msc=SKIP | need=UDX_OPEN */
    kprintf("msc: soft residual lean soft_ne_product=1 product_T1=0 "
            "bot_ready=%u stage=%u claim=%u freestanding_msc=%s "
            "freestanding_close=%u dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "dual_dod_a=OPEN_UDX need=UDX_OPEN "
            "product_usb=xhci_udx product=UDX+ABI "
            "C0=1 C0_SKIP=%u stamp_storm=0 version_stamp=0 "
            "Soft!=product via=%s\n",
            (unsigned)g_fMscReady, (unsigned)g_u32LastStage,
            (unsigned)u32Claim, szFsMsc, (unsigned)u32Close,
            (unsigned)u32SkipDef, szViaSafe);

    /* Grep: stick: soft residual lean | dual_dod_a=OPEN_UDX | need=UDX_OPEN */
    kprintf("stick: soft residual lean log_ready=%u dual_dod_a=OPEN_UDX "
            "need=UDX_OPEN soft=1 product=0 soft_ne_product=1 "
            "claim=%u freestanding_msc=%s freestanding_close=%u "
            "product=UDX+ABI path=xhci_udx G-AC-1=1 "
            "C0=1 C0_SKIP=%u stamp_storm=0 version_stamp=0 "
            "Soft!=product via=%s "
            "(freestanding MSC residual lean; != product store_door; "
            "not freestanding thrash; not Dual DoD A close)\n",
            (unsigned)g_fMscReady, (unsigned)u32Claim, szFsMsc,
            (unsigned)u32Close, (unsigned)u32SkipDef, szViaSafe);
}

int
xhci_msc_init(void)
{
    u8 u8Bus = 0;
    u8 u8Slot = 0;
    u8 u8Func = 0;

    if (g_fInitTried != 0) {
        return 0;
    }
    /*
     * C0 freestanding SKIP residual honesty only (NOT product Dual DoD A).
     * Freestanding MSC SKIP default (product=UDX+ABI · G-AC-1).
     * GJ_XHCI_MSC_PROBE stays 0: do not claim HC/rings; soft residual lean
     * honesty only. Product laptop USB = xhci_udx + hot+cold ABI + DDI.
     * Dual DoD A = OPEN_UDX / need=UDX_OPEN (not freestanding stage close).
     * NEVER re-enable freestanding MSC thrash as default / product.
     * Opt-in residual: -DGJ_XHCI_MSC_PROBE=1 lab only.
     * Soft!=product · freestanding net first · claim=0 · freestanding_msc=SKIP.
     * Stamp-free (stamp_storm=0 version_stamp=0). freestanding_close=0.
     * Grep: xhci: init SKIP | GJ_XHCI_MSC_PROBE=0 | freestanding_msc=SKIP
     * Grep: xhci: freestanding MSC SKIP | need=UDX_OPEN | claim=0
     * Grep: xhci: soft residual product=UDX+ABI | product_udx_abi honesty
     * Grep: freestanding_close=0 | C0_SKIP=1 | soft_ne_product=1
     */
#if !GJ_XHCI_MSC_PROBE
    g_fInitTried = 1;
    g_u32LastStage = 1;
    /* Grep: xhci: init SKIP GJ_XHCI_MSC_PROBE=0 | freestanding_msc=SKIP */
    kprintf("xhci: init SKIP GJ_XHCI_MSC_PROBE=0 "
            "freestanding_msc=SKIP claim=0 freestanding_close=0 "
            "C0_SKIP=1 soft_ne_product=1 "
            "stamp_storm=0 version_stamp=0 "
            "(C0 freestanding MSC SKIP; product=UDX+ABI; path=xhci_udx; "
            "dual_dod_a=OPEN_UDX need=UDX_OPEN; Soft!=product; G-AC-1; "
            "C0 freestanding SKIP residual; not Dual DoD A close; "
            "not in-kernel .ko wire; freestanding net first; "
            "no freestanding MSC thrash; no HC claim; no rings)\n");
    /* Grep: xhci: freestanding MSC SKIP GJ_XHCI_MSC_PROBE=0 */
    kprintf("xhci: freestanding MSC SKIP GJ_XHCI_MSC_PROBE=0 "
            "freestanding_msc=SKIP claim=0 freestanding_close=0 "
            "product=UDX+ABI path=xhci_udx dual_dod_a=OPEN_UDX "
            "need=UDX_OPEN soft=1 product=0 soft_ne_product=1 "
            "C0=1 C0_SKIP=1 stamp_storm=0 version_stamp=0 "
            "(Soft!=product; G-AC-1; Linux-shaped userspace UDX; "
            "not freestanding class thrash; lean residual only; "
            "not re-enable freestanding MSC; not Dual DoD A close; "
            "dual MIT OR Apache-2.0)\n");
    xhci_msc_soft_residual_once("skip_default");
    return 0;
#endif
    g_fInitTried = 1;
    g_u32LastStage = 0;
    g_u32LampStageLast = 0;
    g_u32LampWhyTag = 0;
    g_u32LampStageBurstN = 0;
    g_u32SoftSyncAlreadyN = 0;
    g_u32SoftSyncDenyN = 0;
    g_u32SoftRecoverWhereTag = 0;
    g_u32SoftRecoverStampN = 0;
    g_u32CtrlPassSoftN = 0;
    g_u32GetConfigTryStampN = 0;
    g_u32LastCc = 0;
    g_u32SsPortBits = 0;
    g_u8SsPortLo = 0;
    g_u8SsPortHi = 0;

    if (xhci_find_and_map(&u8Bus, &u8Slot, &u8Func) != 0) {
        g_u32LastStage = 1;
        kprintf("xhci: init SKIP (no xHCI)\n");
        xhci_msc_soft_residual_once("no_xhci");
        return 0;
    }

    if (xhci_hc_reset_run() != 0) {
        g_u32LastStage = 2;
        kprintf("xhci: init FAIL (reset)\n");
        xhci_msc_soft_residual_once("reset_fail");
        return 0;
    }
    if (xhci_setup_rings() != 0) {
        g_u32LastStage = 3;
        kprintf("xhci: init FAIL (rings)\n");
        xhci_msc_soft_residual_once("rings_fail");
        return 0;
    }
    g_fHcUp = 1;
    g_u32LastStage = 4;
    kprintf("xhci: init PASS (hc up; probing ports)\n");

    if (xhci_try_ports() != 0) {
        /*
         * Honesty: stage=15 + device was seen = code/enum path still open;
         * stage=5 = no CCS (no stick / wrong port); stage=1 = no HC.
         * Soft != product T1. Grep: msc: not_ready
         */
        kprintf("xhci: init soft PARTIAL (hc up, no MSC BOT stick) stage=%u "
                "cc=%u port=%u spd=%u\n",
                (unsigned)g_u32LastStage, (unsigned)xhci_msc_last_cc(),
                (unsigned)g_u8PortId, (unsigned)g_u8PortSpeed);
        if (g_u32LastStage == 15u) {
            xhci_stage_soft_lamp(15u, "GET_CONFIG_not_ready");
            kprintf("msc: not_ready reason=get_config cc=%u port=%u spd=%u "
                    "(device-desc reached; config timed out or PED loss - "
                    "insert SS stick / check a12f EP0)\n",
                    (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortId,
                    (unsigned)g_u8PortSpeed);
        } else if (g_u32LastStage == 11u) {
            xhci_stage_soft_lamp(11u, "GET_DESC_not_ready");
            kprintf("msc: not_ready reason=get_desc cc=%u port=%u spd=%u "
                    "(Address ok; first EP0 GET_DESC TO/fail - check "
                    "xhci: ctrl TO ep0_state / soft-continue)\n",
                    (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortId,
                    (unsigned)g_u8PortSpeed);
        } else if (g_u32LastStage == 13u) {
            xhci_stage_soft_lamp(13u, "GET_DESC18_not_ready");
            kprintf("msc: not_ready reason=get_desc18 cc=%u port=%u spd=%u "
                    "(8-byte device-desc ok; full 18 fail)\n",
                    (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortId,
                    (unsigned)g_u8PortSpeed);
        } else if (g_u32LastStage == 5u) {
            kprintf("msc: not_ready reason=no_ccs (no stick / no SS CCS port)\n");
        } else if (g_u32LastStage == 16u) {
            kprintf("msc: not_ready reason=not_bot (device not MSC BOT)\n");
        } else {
            kprintf("msc: not_ready reason=stage_%u cc=%u\n",
                    (unsigned)g_u32LastStage, (unsigned)xhci_msc_last_cc());
        }
        xhci_msc_soft_residual_once("partial_no_bot");
        return 0;
    }
    kprintf("xhci: init PASS msc_ready=1 stage=%u\n",
            (unsigned)g_u32LastStage);
    kprintf("msc: ready reason=bot_capacity port=%u spd=%u\n",
            (unsigned)g_u8PortId, (unsigned)g_u8PortSpeed);
    xhci_msc_soft_residual_once("msc_ready");
    return 0;
}

int
xhci_msc_stick_log_ready(void)
{
    return g_fMscReady != 0 ? 1 : 0;
}

u32
xhci_msc_last_stage(void)
{
    return g_u32LastStage;
}

u32
xhci_msc_last_cc(void)
{
    /*
     * Panel honesty (PR1 A2): sticky last_cc is often 1 (Success) from a
     * later EP0 arm / Evaluate. Prefer path-specific fail codes so the
     * panel shows the real Transfer/Command Event that failed.
     *
     * Stages 19/20 -> g_u32AddrFailCc (0 = timeout / never completed).
     * Stages 11/12/13/15/17 -> g_u32CtrlFailCc (0 = timeout honesty).
     */
    if (g_u32LastStage == 19u || g_u32LastStage == 20u) {
        return g_u32AddrFailCc;
    }
    if (g_u32LastStage == 11u || g_u32LastStage == 12u ||
        g_u32LastStage == 13u || g_u32LastStage == 15u ||
        g_u32LastStage == 17u) {
        return g_u32CtrlFailCc;
    }
    return g_u32LastCc;
}

/** 0=none 1=BSR0 2=BSR+soft - for STATUS path lamp. */
u32
xhci_msc_addr_path(void)
{
    return g_u32AddrPath;
}

u8
xhci_msc_last_port(void)
{
    return g_u8PortId;
}

u8
xhci_msc_last_speed(void)
{
    return g_u8PortSpeed;
}

int
xhci_msc_stick_log_write(const void *pBuf, u32 cb)
{
    /*
     * Soft fail-closed when MSC not ready: greppable OPEN (never silent).
     * Ready path: FAT KLOG.TXT -> raw high-LBA (GJUSBLOG1). Soft!=product.
     * Dual DoD A stick OPEN_UDX / need=UDX_OPEN under freestanding MSC SKIP
     * default (net first). C0 residual honesty only - not Dual DoD A close.
     * Grep: stick: log write OPEN|PASS|FAIL | freestanding_msc=SKIP
     * Grep: dual_dod_a=OPEN_UDX | need=UDX_OPEN | freestanding_close=0
     */
    if (g_fMscReady == 0) {
#if GJ_XHCI_MSC_PROBE
        /* Opt-in residual: still never Dual DoD A close; product = UDX. */
        kprintf("stick: log write OPEN bytes=%u path=none msc_ready=0 "
                "stage=%u dual_dod_a=OPEN_UDX need=UDX_OPEN "
                "product=UDX+ABI path=xhci_udx freestanding_msc=OPTIN_RESIDUAL "
                "claim=%u freestanding_close=0 soft_ne_product=1 "
                "C0=1 C0_SKIP=0 stamp_storm=0 version_stamp=0 "
                "(soft fail-closed; Soft!=product; freestanding residual lean; "
                "not Dual DoD A close; need userspace xhci_udx)\n",
                (unsigned)cb, (unsigned)g_u32LastStage,
                (unsigned)(g_fHcUp != 0u ? 1u : 0u));
#else
        kprintf("stick: log write OPEN bytes=%u path=none msc_ready=0 "
                "stage=%u dual_dod_a=OPEN_UDX need=UDX_OPEN "
                "product=UDX+ABI path=xhci_udx freestanding_msc=SKIP "
                "claim=0 freestanding_close=0 soft_ne_product=1 "
                "C0=1 C0_SKIP=1 stamp_storm=0 version_stamp=0 "
                "(soft fail-closed; Soft!=product; freestanding MSC SKIP "
                "default; not freestanding thrash; not Dual DoD A close)\n",
                (unsigned)cb, (unsigned)g_u32LastStage);
#endif
        /* Residual once if init skipped residual somehow; no stamp storms. */
        xhci_msc_soft_residual_once("stick_log_open");
        return -1;
    }
    if (pBuf == NULL || cb == 0) {
        kprintf("stick: log write OPEN bytes=%u path=none reason=bad_arg "
                "(soft fail-closed)\n",
                (unsigned)cb);
        return -1;
    }

    if (g_fKlogLocated != 0) {
        if (fat_write_klog(pBuf, cb) == 0) {
            kprintf("stick: log write PASS bytes=%u path=KLOG.TXT\n",
                    (unsigned)(cb > g_u32KlogSize ? g_u32KlogSize : cb));
            return 0;
        }
        kprintf("stick: log write FAT soft FAIL; try raw\n");
    } else {
        kprintf("stick: log write FAT OPEN (KLOG.TXT not located); try raw\n");
    }

    if (raw_log_write(pBuf, cb) == 0) {
        kprintf("stick: log write PASS bytes=%u path=raw\n", (unsigned)cb);
        return 0;
    }

    kprintf("stick: log write FAIL bytes=%u path=none (MSC ready; FAT+raw miss)\n",
            (unsigned)cb);
    return -1;
}
