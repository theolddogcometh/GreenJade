/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room minimal xHCI + USB MSC BOT write path for GreenJade.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux USB paste.
 *
 * Public specs only (Intel xHCI / USB 2.0/3.x / MSC BOT / SCSI / FAT32).
 *
 * *** SOFT SCAFFOLD ONLY (ABI-first pivot) ***
 * Product direction: Linux ABI Option C + virtio T0 + userspace/DDI drivers.
 * This file is bring-up / lab stick-log experiment — not T1 product close,
 * not bar3, not a reason to thrash EP0 recovery as primary engineering.
 * Soft ≠ product. Linux inventory (xhci_hcd) remains G752 ground truth.
 *
 * Scope (MVP honest):
 *   - First PCI 0C:03 prog-if 0x30, BAR0 UC map, bus master
 *   - HC reset + run, single-segment command + event rings (poll, no IRQ)
 *   - First connected port: reset, Enable Slot, Address Device
 *   - Control: GET_DESCRIPTOR / SET_CONFIGURATION for MSC BOT (08/06/50)
 *   - Configure Endpoint for bulk IN/OUT; BOT READ CAPACITY(10) + WRITE(10)
 *   - FAT32 overwrite of pre-sized EFI/GREENJADE/KLOG.TXT, else raw LBA log
 *
 * Soft ≠ HID ≠ bar3. QEMU Multiboot without xHCI → SKIP cleanly.
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
#define XHCI_PORTSC_PP       (1u << 9)  /* Port Power — required after HCRST */
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
#define XHCI_TRB_CH     (1u << 4) /* Chain */
#define XHCI_TRB_IOC    (1u << 5)
#define XHCI_TRB_IDT    (1u << 6)
/* Address Device Command TRB: Block Set Address Request (software SET_ADDRESS) */
#define XHCI_ADDR_BSR   (1u << 9)

#define USB_REQ_SET_ADDRESS 0x05u

#define XHCI_EP_TYPE_CTRL_BI 4u
#define XHCI_EP_TYPE_BULK_OUT 2u
#define XHCI_EP_TYPE_BULK_IN  6u

#define XHCI_RING_TRBS   64u
#define XHCI_MAX_PORTS   32u
#define XHCI_POLL_SPINS  5000000u
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
/* Residual Transfer Length from last consumed event (status bits 0–23). */
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

/* Busy-wait settle (spin count; not calibrated ms — freestanding). */
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
 * cc=0 means timeout / no Transfer Event — fail-closed, not Success.
 * Does not clear on later Success; only overwritten by a newer ctrl fail.
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
static int
xhci_wait_event(u32 u32WantType, u32 *pCode, u32 *pSlot, u64 *pParam,
                u32 u32Spins)
{
    u32 iSpin;

    for (iSpin = 0; iSpin < u32Spins; iSpin++) {
        struct xhci_trb *pTrb = &g_pEvtRing[g_u32EvtIdx];
        u32 u32Ctrl;
        u8 fCycle;
        u32 u32Type;

        /*
         * Occasional line invalidate so non-coherent HC DMA is visible.
         * Every-spin clflush made panel boot crawl — sample every 256 polls.
         */
        if ((iSpin & 255u) == 0u) {
            xhci_clinv_ptr(pTrb);
        }
        u32Ctrl = pTrb->u32Control;
        fCycle = (u8)(u32Ctrl & XHCI_TRB_CYCLE);

        if (fCycle != g_fEvtCycle) {
            cpu_relax();
            continue;
        }

        /* Matched cycle — force fresh read of full TRB before consume. */
        xhci_clinv_ptr(pTrb);
        u32Ctrl = pTrb->u32Control;
        u32Type = trb_type(u32Ctrl);
        /* Residual Transfer Length (bits 0–23) — honest ctrl fail logs. */
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
                (u64)g_paEvtRing +
                (u64)g_u32EvtIdx * sizeof(struct xhci_trb);

            u64Erdp |= 8ull; /* EHB */
            mmio_w64(g_pRt, 0x20u + 0x18u, u64Erdp);
        }

        if (u32WantType == 0u || u32Type == u32WantType) {
            return (int)u32Type;
        }
        /* Port status change etc. — keep draining until wanted type */
        if (u32Type == XHCI_TRB_TYPE_PORT_STATUS) {
            continue;
        }
        if (u32WantType != 0u && u32Type != u32WantType) {
            /* unexpected but may still be useful; continue searching */
            continue;
        }
    }
    return -1;
}

static void
doorbell_hc(void)
{
    g_pDb[0] = 0;
    mmio_barrier();
}

static void
doorbell_ep(u8 u8Slot, u8 u8Dci)
{
    g_pDb[u8Slot] = (u32)u8Dci;
    mmio_barrier();
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
     * multi-port scans exhaust the first lap (G752: many CCS ports × passes).
     * Missing this → command hang / Enable Slot timeout (panel stage 7).
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
        /* Update link cycle then wrap */
        pRing[XHCI_RING_TRBS - 1u].u64Param = (u64)paRing;
        pRing[XHCI_RING_TRBS - 1u].u32Status = 0;
        pRing[XHCI_RING_TRBS - 1u].u32Control =
            (XHCI_TRB_TYPE_LINK << 10) | XHCI_TRB_TC |
            (*pCycle != 0 ? XHCI_TRB_CYCLE : 0u);
        mmio_barrier();
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

/* Forward decls — used by EP0 hard resync / ctrl before their definitions. */
static int xhci_evaluate_ep0_mps(void);
static void xhci_ep0_ring_reset(void);
static void xhci_port_force_u0(u8 u8Port);
static u32 portsc_read(u8 u8Port);

/* Drain any pending event ring entries (port status + stale xfer). */
static void
xhci_drain_events(u32 u32Max)
{
    u32 i;
    u32 u32Code = 0;

    for (i = 0; i < u32Max; i++) {
        if (xhci_wait_event(0, &u32Code, NULL, NULL, 80u) < 0) {
            break;
        }
    }
}

/*
 * After stall/babble/timeout: Reset Endpoint(EP0) + Set TR Dequeue to a fresh
 * ring. Soft ring_reset alone is not enough once the HC halted EP0 (stage 15).
 */
static int
xhci_ep0_hard_resync(void)
{
    u32 u32Code = 0;
    u32 u32Ctrl;

    if (g_u8SlotId == 0) {
        xhci_ep0_ring_reset();
        return -1;
    }
    xhci_drain_events(32u);
    if (g_u8PortId != 0) {
        xhci_port_force_u0(g_u8PortId);
    }

    /* Stop Endpoint EP0 (ignore fail — may already be idle/halted). */
    u32Ctrl = (XHCI_TRB_TYPE_STOP_EP << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    (void)xhci_cmd(0, 0, u32Ctrl, &u32Code, NULL);
    xhci_drain_events(16u);

    /* Reset Endpoint, Endpoint ID = DCI 1 (EP0), TSP=0 */
    u32Ctrl = (XHCI_TRB_TYPE_RESET_EP << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    if (xhci_cmd(0, 0, u32Ctrl, &u32Code, NULL) != 0) {
        kprintf("xhci: Reset Endpoint EP0 FAIL code=%u\n", u32Code);
        /* Fall through — still try Set TR Dequeue / Evaluate */
    }

    xhci_ep0_ring_reset();
    /* Set TR Dequeue Pointer: param = ring | DCS=1, EP ID = 1 */
    u32Ctrl = (XHCI_TRB_TYPE_SET_TR_DEQ << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    if (xhci_cmd((u64)g_paEp0Ring | 1ull, 0, u32Ctrl, &u32Code, NULL) != 0) {
        kprintf("xhci: Set TR Dequeue EP0 FAIL code=%u; try Evaluate\n",
                u32Code);
        return xhci_evaluate_ep0_mps();
    }
    xhci_settle(200000u);
    return 0;
}

/*
 * Greppable control fail line (PR1 A1). cc = Transfer Event completion code
 * from the failing event (not a later Evaluate Success). Timeout → cc=0.
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
     * after a working device-desc can drop the link → next GET_CONFIG
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
     */
    xfer_enqueue(g_pEp0Ring, &g_u32Ep0Idx, &g_fEp0Cycle, g_paEp0Ring,
                 u64SetupRaw, 8u,
                 (XHCI_TRB_TYPE_SETUP << 10) | XHCI_TRB_IDT | XHCI_TRB_CH |
                     (u32Trt << 16));

    u32Dir = (fIn != 0) ? (1u << 16) : 0u;

    if (u16Len > 0 && pData != NULL) {
        if (fIn == 0) {
            memcpy(g_pScratch, pData, u16Len);
        } else {
            memset(g_pScratch, 0, u16Len > GJ_PAGE_SIZE ? GJ_PAGE_SIZE : u16Len);
        }
        xhci_clflush_ptr(g_pScratch);
        /* Single Data TRB; DIR bit 16 only (not TRT). CH → Status. */
        xfer_enqueue(g_pEp0Ring, &g_u32Ep0Idx, &g_fEp0Cycle, g_paEp0Ring,
                     (u64)g_paScratch, (u32)u16Len,
                     (XHCI_TRB_TYPE_DATA << 10) | XHCI_TRB_CH | u32Dir);
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
    xhci_clflush_span(g_pEp0Ring, 256u);
    doorbell_ep(g_u8SlotId, 1); /* EP0 DCI = 1 */

    /* SS on Intel a12f: longer poll (config after device desc often needs it). */
    u32Spins = (g_u8PortSpeed >= 4u) ? (XHCI_POLL_SPINS * 4u) : XHCI_POLL_SPINS;

    /*
     * Wait for Transfer Event. Accept Success (1) or Short Packet (13).
     * Some HCs emit Short Packet on the Data TRB then Success on Status —
     * if we return after the first Short Packet, EP0 is mid-TD and the next
     * GET_DESC18 fails (G752 stage-13 after 8-byte OK). Drain until Status
     * class event or timeout after short.
     *
     * Fail-closed honesty: sticky fail cc is the *actual* event code that
     * failed (or 0 on timeout) — never a later arm/Evaluate Success.
     */
    nEv = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code, NULL, NULL,
                          u32Spins);
    if (nEv < 0) {
        g_u32LastCtrlResidual = 0;
        xhci_note_ctrl_fail_cc(0);
        xhci_ctrl_fail_log(pSetup, 0, 0);
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

        /* Short on Data: wait briefly for Status IOC (Success). */
        nEv2 = xhci_wait_event(XHCI_TRB_TYPE_XFER_EVENT, &u32Code2, NULL, NULL,
                               u32Spins / 4u + 100000u);
        if (nEv2 >= 0) {
            xhci_note_cc(u32Code2);
            if (u32Code2 != 1u && u32Code2 != 13u) {
                /* Status failed — sticky the real Status event cc. */
                g_u32LastCtrlResidual = g_u32LastEvtResidual;
                xhci_note_ctrl_fail_cc(u32Code2);
                xhci_ctrl_fail_log(pSetup, u32Code2, g_u32LastEvtResidual);
                return -1;
            }
            /* Status OK: keep Data-stage residual for short-packet length. */
        }
        /* Timeout on second event is soft OK — some HCs only event once. */
    }
    g_u32LastCtrlResidual = u32Residual;
    if (u16Len > 0 && pData != NULL && fIn != 0) {
        xhci_clflush_span(g_pScratch, (u32)u16Len);
        memcpy(pData, g_pScratch, u16Len);
    }
    /* Drain trailing events (Short Packet + Status on some HCs). */
    xhci_drain_events(16u);
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
 * USB3 SuperSpeed: bMaxPacketSize0 is an exponent (typically 9 → 512).
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
        return 0; /* unknown — treat as not preferentially SS */
    }
    return (g_u32SsPortBits & (1u << (u8Port - 1u))) != 0 ? 1 : 0;
}

/**
 * Walk xECP Supported Protocol (id=2). Major>=3 → SuperSpeed-capable ports.
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
     * Evaluate Context reloads EP0 TR Dequeue Pointer from input context.
     * Software producer index MUST reset to match or later control TDs desync
     * (classic stage-8 hang after first 8-byte GET_DESCRIPTOR).
     */
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

    xhci_clflush_ptr(g_pInputCtx);
    xhci_clflush_ptr((u8 *)g_pInputCtx + 64u);

    u32Ctrl = (XHCI_TRB_TYPE_EVAL_CTX << 10) | ((u32)g_u8SlotId << 24);
    if (xhci_cmd((u64)g_paInputCtx, 0, u32Ctrl, &u32Code, NULL) != 0) {
        kprintf("xhci: Evaluate Context EP0 FAIL code=%u mps0=%u\n", u32Code,
                (unsigned)g_u16MaxPkt0);
        return -1;
    }
    /* Evaluate reloads dequeue — resync software producer again. */
    xhci_ep0_ring_reset();
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

    /* Run (no INTE — poll event ring) */
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

/* Force link state U0 (PLS=0) via LWS — needed after SS reset on a12f. */
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
 * G752 p22s4: BSR0 USBTE after cold-only — alternate warm on retries.
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

    /* Clear change bits; leave CCS/PED/PLS alone — write-1-to-clear changes */
    portsc_write(u8Port, (u32Ps & XHCI_PORTSC_PRESERVE) | XHCI_PORTSC_CHANGE);

    u32Ps = portsc_read(u8Port);
    /* fPreferWarm: -1=auto (SS-cap→warm), 0=cold first, 1=warm first */
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
     * Do not invent SS/HS when PORTSC speed is still 0 — leave 0 and let
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
     * Max Packet Size = g_u16MaxPkt0 (SS→512, HS→64).
     */
    ctx_write32(pEp0, 0x04,
                (3u << 1) | (XHCI_EP_TYPE_CTRL_BI << 3) |
                    ((u32)g_u16MaxPkt0 << 16));
    ctx_write64(pEp0, 0x08, (u64)g_paEp0Ring | 1ull); /* TR Deq | DCS=1 */
    /* Average TRB Length — non-zero required by some HCs for Address Device. */
    ctx_write32(pEp0, 0x10, (g_u16MaxPkt0 >= 512u) ? 512u : 8u);

    xhci_clflush_span(g_pInputCtx, (u32)g_u8CtxSize * 3u);
    xhci_clflush_span(g_pEp0Ring, 256u);
}

/*
 * After Address Device the HC has EP0 dequeue from input context. Explicitly
 * Set TR Dequeue to a fresh software ring so the first GET_DESC is not lost
 * (G752 stage-11: address PASS, first control timeout).
 */
static int
xhci_ep0_arm_after_address(void)
{
    u32 u32Code = 0;
    u32 u32Ctrl;

    xhci_ep0_ring_reset();
    if (g_u8SlotId == 0) {
        return -1;
    }
    /* Set TR Dequeue Pointer — EP ID = DCI 1 (EP0), DCS=1 on pointer. */
    u32Ctrl = (XHCI_TRB_TYPE_SET_TR_DEQ << 10) | (1u << 16) |
              ((u32)g_u8SlotId << 24);
    if (xhci_cmd((u64)g_paEp0Ring | 1ull, 0, u32Ctrl, &u32Code, NULL) != 0) {
        kprintf("xhci: Set TR Dequeue after address FAIL code=%u; Evaluate\n",
                u32Code);
        return xhci_evaluate_ep0_mps();
    }
    xhci_ep0_ring_reset(); /* match DCS=1 / idx=0 after Set TR Dequeue */
    xhci_clflush_ptr(g_pEp0Ring);
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
 * Never invent SuperSpeed(4) or HS(3) when the speed field is 0 — wrong Slot
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
     * Wait until speed field is non-zero with PED. Force U0 sparingly — every
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

/* Port power bounce — recover a12f after Address Device cc=4. */
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
        kprintf("xhci: Address Device aborted — speed/PED unknown\n");
        g_u32LastStage = 19;
        g_u32AddrFailCc = 4u;
        g_u32AddrPath = 1u;
        return -1;
    }
    /* Need PED before Address Device or HC returns Transaction Error (4). */
    u32Ps = portsc_read(g_u8PortId);
    if ((u32Ps & XHCI_PORTSC_PED) == 0 || (u32Ps & XHCI_PORTSC_CCS) == 0) {
        kprintf("xhci: Address Device aborted — PED/CCS portsc=0x%x\n",
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

    kprintf("xhci: Address Device BSR0 slot=%u port=%u spd=%u mps0=%u "
            "portsc=0x%x\n",
            (unsigned)g_u8SlotId, (unsigned)g_u8PortId,
            (unsigned)g_u8PortSpeed, (unsigned)g_u16MaxPkt0, (unsigned)u32Ps);

    u32Ctrl = (XHCI_TRB_TYPE_ADDRESS_DEV << 10) | ((u32)g_u8SlotId << 24);
    /* BSR=0 — host controller sends SET_ADDRESS */
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
    /* Invalidate HC-written Output Context. */
    xhci_clflush_span(g_pDevCtx, (u32)g_u8CtxSize * 2u);
    /* USB ≥2ms recovery after SET_ADDRESS before next control. */
    xhci_settle((g_u8PortSpeed >= 4u) ? 3000000u : 800000u);
    if (g_u8PortId != 0) {
        u32Ps = portsc_read(g_u8PortId);
        if (((u32Ps >> 5) & 0xfu) != 0u) {
            xhci_port_force_u0(g_u8PortId);
            xhci_settle(300000u);
        }
    }
    if (xhci_ep0_arm_after_address() != 0) {
        kprintf("xhci: EP0 arm after HW address soft FAIL (continue)\n");
    }
    kprintf("xhci: Address Device HW/BSR0 slot=%u speed=%u soft PASS\n",
            (unsigned)g_u8SlotId, (unsigned)g_u8PortSpeed);
    g_u32AddrPath = 1u;
    return 0;
}

/*
 * Note: BSR=1 + software SET_ADDRESS is intentionally NOT used for first
 * address. After BSR1 the HC stores Device Address N while the device is still
 * USB addr 0 → EP0 SET_ADDRESS times out (panel stage=20 soft cc=0). Product
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
     * EP0 SET_ADDRESS tokens go to N → timeout (stage=20 soft cc=0 on p22s4).
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
    g_u32LastStage = 19; /* real Address Device fail — not fake stage-20 */
    g_u32AddrFailCc = u32Bsr0Cc != 0u ? u32Bsr0Cc : g_u32AddrFailCc;
    g_u32AddrPath = 1u;
    kprintf("xhci: address FAIL final stage=19 addr_cc=%u port=%u spd=%u "
            "(BSR0 only; soft SET_ADDRESS skipped — device would be addr0)\n",
            (unsigned)g_u32AddrFailCc, (unsigned)g_u8PortId,
            (unsigned)g_u8PortSpeed);
    return -1;
}

/* Re-address after stage-11 GET_DESC failure: always BSR0 (HC SET_ADDRESS). */
static int
xhci_readdress_alternate(void)
{
    xhci_disable_slot_soft();
    xhci_settle(500000u);
    if (g_u8PortId != 0) {
        xhci_port_force_u0(g_u8PortId);
        xhci_settle(500000u);
        (void)xhci_port_reset_ex(g_u8PortId, 1);
        xhci_settle(2000000u);
    }
    if (xhci_enable_slot_once() != 0) {
        return -1;
    }
    kprintf("xhci: readdress HW/BSR0 after stage-11\n");
    if (xhci_address_device_hw() != 0) {
        xhci_disable_slot_soft();
        return -1;
    }
    g_fAddrUsedBsrSoft = 0;
    if (g_u32LastStage == 19u || g_u32LastStage == 20u ||
        g_u32LastStage == 7u) {
        g_u32LastStage = 4;
    }
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
             * Skip UAS (protocol 0x62) — no UAS path yet.
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
 */
static void
xhci_ctrl_recover(void)
{
    if (g_u8PortId != 0) {
        xhci_port_force_u0(g_u8PortId);
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
 * Do NOT Evaluate Context immediately after Address Device — HC already has
 * EP0 dequeue from Address; a pre-GET_DESC Evaluate desynced a12f and
 * regressed past stages 13/15 back to 11.
 *
 * Order: settle → 8-byte first (safest) → Evaluate only if MPS changes or
 * after short GET_DESC before 18 → 18-byte. Optional 18-first only as fallback.
 */
static int
xhci_get_device_descriptor(u8 *pDev)
{
    struct usb_setup setup;
    u8 aTmp[18];
    u16 u16Mps;
    int nTry;
    int fGot8 = 0;

    if (pDev == NULL) {
        return -1;
    }
    memset(&setup, 0, sizeof(setup));
    setup.u8BmRequestType = 0x80u;
    setup.u8BRequest = USB_REQ_GET_DESCRIPTOR;
    setup.u16WValue = (u16)(USB_DT_DEVICE << 8);
    setup.u16WIndex = 0;

    /*
     * Address Device already programmed EP0 TR dequeue to ring base.
     * Explicit arm once, long settle (USB ≥2ms; SS longer on a12f).
     */
    (void)xhci_ep0_arm_after_address();
    if (g_u8PortId != 0 && ((portsc_read(g_u8PortId) >> 5) & 0xfu) != 0u) {
        xhci_port_force_u0(g_u8PortId);
    }
    xhci_settle((g_u8PortSpeed >= 4u) ? 4000000u : 1000000u);
    xhci_drain_events(32u);

    /*
     * Path 0 (SS first): try full 18-byte before short 8 — some a12f paths
     * desync after short GET_DESC and never recover for 18 (stage-13).
     */
    if (g_u8PortSpeed >= 4u) {
        setup.u16WLength = 18;
        for (nTry = 0; nTry < 3; nTry++) {
            memset(aTmp, 0, 18);
            if (xhci_ctrl(&setup, aTmp, 18, 1) == 0 && aTmp[0] >= 18u &&
                aTmp[1] == USB_DT_DEVICE) {
                memcpy(pDev, aTmp, 18);
                u16Mps = decode_bmax_packet0(pDev[7], g_u8PortSpeed);
                if (u16Mps != 0 && u16Mps != g_u16MaxPkt0) {
                    g_u16MaxPkt0 = u16Mps;
                    (void)xhci_evaluate_ep0_mps();
                }
                kprintf("xhci: get device desc (18 first SS) PASS try=%u\n",
                        (unsigned)nTry);
                return 0;
            }
            kprintf("xhci: get device desc (18 first) FAIL try=%u cc=%u\n",
                    (unsigned)nTry, (unsigned)g_u32LastCc);
            (void)xhci_ep0_arm_after_address();
            xhci_settle(800000u);
        }
    }

    /* ---- Path A: 8-byte first (HS / SS fallback) ---- */
    setup.u16WLength = 8;
    for (nTry = 0; nTry < 4; nTry++) {
        if (xhci_ctrl(&setup, aTmp, 8, 1) == 0) {
            fGot8 = 1;
            break;
        }
        kprintf("xhci: get device desc (8) FAIL try=%u cc=%u speed=%u mps0=%u\n",
                (unsigned)nTry, (unsigned)g_u32LastCc,
                (unsigned)g_u8PortSpeed, (unsigned)g_u16MaxPkt0);
        if (nTry + 1 < 4) {
            /* Soft recover only — avoid Stop/Reset on first post-address fails */
            if (g_u8PortId != 0) {
                xhci_port_force_u0(g_u8PortId);
            }
            xhci_settle(500000u);
            if (nTry >= 1) {
                (void)xhci_evaluate_ep0_mps();
            }
        }
    }
    if (fGot8 == 0) {
        /* ---- Fallback: 18-byte one-shot (SS with default MPS 512) ---- */
        setup.u16WLength = 18;
        for (nTry = 0; nTry < 3; nTry++) {
            if (xhci_ctrl(&setup, aTmp, 18, 1) == 0) {
                memcpy(pDev, aTmp, 18);
                u16Mps = decode_bmax_packet0(pDev[7], g_u8PortSpeed);
                kprintf("xhci: get device desc (18 direct) PASS try=%u "
                        "raw_mps0=%u dec=%u speed=%u\n",
                        (unsigned)nTry, (unsigned)pDev[7], (unsigned)u16Mps,
                        (unsigned)g_u8PortSpeed);
                if (u16Mps != 0 && u16Mps != g_u16MaxPkt0) {
                    g_u16MaxPkt0 = u16Mps;
                    (void)xhci_evaluate_ep0_mps();
                }
                return 0;
            }
            kprintf("xhci: get device desc (18 direct) FAIL try=%u cc=%u\n",
                    (unsigned)nTry, (unsigned)g_u32LastCc);
            if (nTry + 1 < 3) {
                xhci_ctrl_recover();
            }
        }
        kprintf("xhci: get device desc FAIL stage=11 cc=%u speed=%u\n",
                (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortSpeed);
        g_u32LastStage = 11;
        /* Sticky already set by xhci_ctrl fail (or 0 if never completed). */
        return -1;
    }

    {
        u16 u16OldMps = g_u16MaxPkt0;

        u16Mps = decode_bmax_packet0(aTmp[7], g_u8PortSpeed);
        kprintf("xhci: bMaxPacketSize0 raw=%u decoded=%u (was %u) speed=%u\n",
                (unsigned)aTmp[7], (unsigned)u16Mps, (unsigned)u16OldMps,
                (unsigned)g_u8PortSpeed);
        if (u16Mps != 0) {
            g_u16MaxPkt0 = u16Mps;
        }

        /*
         * Stage 13 (G752: 8-byte OK, 18-byte FAIL):
         * Prefer Set TR Dequeue re-arm always. Evaluate only if MPS changed —
         * Evaluate when MPS already correct desynced a12f for GET_DESC18.
         */
        xhci_drain_events(32u);
        if (g_u16MaxPkt0 != u16OldMps) {
            kprintf("xhci: MPS changed %u→%u; Evaluate Context\n",
                    (unsigned)u16OldMps, (unsigned)g_u16MaxPkt0);
            if (xhci_evaluate_ep0_mps() != 0) {
                if (xhci_ep0_arm_after_address() != 0) {
                    kprintf("xhci: EP0 re-arm after 8-byte desc FAIL cc=%u\n",
                            (unsigned)g_u32LastCc);
                    xhci_note_ctrl_fail_cc(g_u32LastCc);
                    g_u32LastStage = 12;
                    return -1;
                }
            }
        } else {
            if (xhci_ep0_arm_after_address() != 0) {
                kprintf("xhci: Set TR Deq after 8-byte FAIL; try Evaluate\n");
                if (xhci_evaluate_ep0_mps() != 0) {
                    xhci_note_ctrl_fail_cc(g_u32LastCc);
                    g_u32LastStage = 12;
                    return -1;
                }
            }
        }
    }
    xhci_settle((g_u8PortSpeed >= 4u) ? 1500000u : 400000u);
    if (g_u8PortId != 0) {
        xhci_port_force_u0(g_u8PortId);
    }
    xhci_drain_events(32u);

    /*
     * Full device descriptor. Try wLength=18 first, then 64 (short packet),
     * then MPS-sized. Short-packet path is more reliable on some SS sticks.
     */
    {
        static const u16 s_aWlen[] = { 18u, 64u, 512u };
        u32 iW;
        u8 aBig[64];

        for (iW = 0; iW < 3u; iW++) {
            u16 u16W = s_aWlen[iW];
            u8 *pBuf = (u16W <= 18u) ? aTmp : aBig;
            u32 cbCopy;

            if (u16W > 64u) {
                u16W = (g_u16MaxPkt0 >= 64u) ? g_u16MaxPkt0 : 64u;
                if (u16W > 64u) {
                    u16W = 64u; /* aBig budget */
                }
            }
            setup.u8BmRequestType = 0x80u;
            setup.u8BRequest = USB_REQ_GET_DESCRIPTOR;
            setup.u16WValue = (u16)(USB_DT_DEVICE << 8);
            setup.u16WIndex = 0;
            setup.u16WLength = u16W;

            for (nTry = 0; nTry < 3; nTry++) {
                memset(pBuf, 0, u16W > 18u ? 64u : 18u);
                if (xhci_ctrl(&setup, pBuf, u16W, 1) == 0) {
                    cbCopy = 18u;
                    if (pBuf[0] >= 18u && pBuf[1] == USB_DT_DEVICE) {
                        memcpy(pDev, pBuf, cbCopy);
                        kprintf("xhci: get device desc PASS wlen=%u try=%u\n",
                                (unsigned)u16W, (unsigned)nTry);
                        return 0;
                    }
                    /* Short buffer still may hold 18 useful bytes. */
                    if (pBuf[0] >= 8u && pBuf[1] == USB_DT_DEVICE) {
                        memcpy(pDev, pBuf, cbCopy);
                        if (pBuf[0] >= 18u) {
                            kprintf("xhci: get device desc PASS wlen=%u "
                                    "try=%u (short ok)\n",
                                    (unsigned)u16W, (unsigned)nTry);
                            return 0;
                        }
                    }
                    kprintf("xhci: get device desc bad content wlen=%u "
                            "bLen=%u bType=%u\n",
                            (unsigned)u16W, (unsigned)pBuf[0],
                            (unsigned)pBuf[1]);
                } else {
                    kprintf("xhci: get device desc FAIL wlen=%u try=%u cc=%u\n",
                            (unsigned)u16W, (unsigned)nTry,
                            (unsigned)g_u32LastCc);
                }
                if (nTry + 1 < 3) {
                    /* Soft arm first; hard resync only on last retry. */
                    if (nTry == 0) {
                        (void)xhci_ep0_arm_after_address();
                        xhci_settle(500000u);
                    } else {
                        xhci_ctrl_recover();
                    }
                }
            }
        }
    }
    kprintf("xhci: get device desc FAIL stage=13 cc=%u mps0=%u speed=%u\n",
            (unsigned)g_u32CtrlFailCc, (unsigned)g_u16MaxPkt0,
            (unsigned)g_u8PortSpeed);
    g_u32LastStage = 13;
    return -1;
}

static int
xhci_enum_msc(void)
{
    struct usb_setup setup;
    u8 aDev[18];
    u8 aCfg[512];
    u16 u16Total;

    if (xhci_get_device_descriptor(aDev) != 0) {
        /*
         * Stage 11/13: control path after address. Readdress opposite method
         * once (BSR0 ↔ BSR+soft) then retry full GET_DESC.
         */
        if (g_u32LastStage == 11u || g_u32LastStage == 13u) {
            kprintf("xhci: stage-%u GET_DESC; readdress alternate + retry\n",
                    (unsigned)g_u32LastStage);
            if (xhci_readdress_alternate() == 0) {
                if (xhci_get_device_descriptor(aDev) == 0) {
                    goto got_dev;
                }
            }
        }
        return -1;
    }
got_dev:
    kprintf("xhci: device class=%u sub=%u proto=%u mps0=%u vid=%x pid=%x\n",
            (unsigned)aDev[4], (unsigned)aDev[5], (unsigned)aDev[6],
            (unsigned)g_u16MaxPkt0,
            (unsigned)(aDev[8] | ((u16)aDev[9] << 8)),
            (unsigned)(aDev[10] | ((u16)aDev[11] << 8)));

    /* Root-hub class device: skip (no hub traversal in MVP). */
    if (aDev[4] == 0x09u) {
        kprintf("xhci: skip USB hub soft FAIL (no hub support)\n");
        g_u32LastStage = 14;
        return -1;
    }

    /*
     * GET_DESCRIPTOR(config). Stage 15 = fail.
     * G752: device-desc PASS then config cc=0 timeout = EP0 desync or LWS thrash.
     * Hard resync EP0 once after device-desc TDs; long settle; try wLength 9 then 64.
     * No Evaluate-first. On cc=0 always hard resync between tries.
     */
    {
        int nTry;
        int fGot = 0;
        u16 u16TryLen;
        u16 u16Got = 0;
        u32 iW;
        static const u16 s_aCfgWlen[] = { 9u, 64u, 255u };

        xhci_drain_events(64u);
        xhci_ctrl_recover(); /* Reset EP + Set TR Deq after device-desc TDs */
        xhci_settle((g_u8PortSpeed >= 4u) ? 2000000u : 500000u);
        if (g_u8PortId != 0) {
            u32 u32Ps = portsc_read(g_u8PortId);

            kprintf("xhci: pre-config portsc=0x%x ped=%u pls=%u spd=%u\n",
                    (unsigned)u32Ps,
                    (u32Ps & XHCI_PORTSC_PED) != 0 ? 1u : 0u,
                    (unsigned)((u32Ps >> 5) & 0xfu),
                    (unsigned)((u32Ps >> 10) & 0xfu));
            if ((u32Ps & XHCI_PORTSC_PED) == 0) {
                kprintf("xhci: PED lost before config; re-reset port\n");
                (void)xhci_port_reset(g_u8PortId);
                xhci_ctrl_recover();
                xhci_settle(2000000u);
            }
        }
        xhci_drain_events(32u);

        memset(&setup, 0, sizeof(setup));
        setup.u8BmRequestType = 0x80u;
        setup.u8BRequest = USB_REQ_GET_DESCRIPTOR;
        setup.u16WValue = (u16)(USB_DT_CONFIG << 8);
        setup.u16WIndex = 0;

        for (iW = 0; iW < 3u && fGot == 0; iW++) {
            u16TryLen = s_aCfgWlen[iW];
            if (u16TryLen > (u16)sizeof(aCfg)) {
                u16TryLen = (u16)sizeof(aCfg);
            }
            setup.u16WLength = u16TryLen;
            for (nTry = 0; nTry < 5; nTry++) {
                memset(aCfg, 0, u16TryLen);
                if (xhci_ctrl(&setup, aCfg, u16TryLen, 1) == 0 &&
                    aCfg[0] >= 9u && aCfg[1] == USB_DT_CONFIG) {
                    fGot = 1;
                    if (g_u32LastCtrlResidual < (u32)u16TryLen) {
                        u16Got =
                            (u16)((u32)u16TryLen - g_u32LastCtrlResidual);
                    } else {
                        u16Got = 0;
                    }
                    if (u16Got < 9u) {
                        u16Got = 9u;
                    }
                    kprintf("xhci: get config PASS wlen=%u try=%u got=%u "
                            "residual=%u\n",
                            (unsigned)u16TryLen, (unsigned)nTry,
                            (unsigned)u16Got,
                            (unsigned)g_u32LastCtrlResidual);
                    break;
                }
                kprintf("xhci: get config FAIL wlen=%u try=%u cc=%u mps0=%u\n",
                        (unsigned)u16TryLen, (unsigned)nTry,
                        (unsigned)g_u32CtrlFailCc, (unsigned)g_u16MaxPkt0);
                if (nTry + 1 < 5) {
                    if (g_u32CtrlFailCc == 0u || nTry >= 1) {
                        xhci_ctrl_recover();
                        xhci_settle(1000000u);
                    } else {
                        (void)xhci_ep0_arm_after_address();
                        xhci_settle(500000u);
                    }
                }
            }
        }

        if (fGot == 0) {
            kprintf("xhci: get config FAIL stage=15 (header) cc=%u speed=%u\n",
                    (unsigned)g_u32CtrlFailCc, (unsigned)g_u8PortSpeed);
            g_u32LastStage = 15;
            return -1;
        }

        u16Total = (u16)aCfg[2] | ((u16)aCfg[3] << 8);
        kprintf("xhci: config wTotalLength=%u bNumInterfaces=%u val=%u "
                "speed=%u got=%u\n",
                (unsigned)u16Total, (unsigned)aCfg[4], (unsigned)aCfg[5],
                (unsigned)g_u8PortSpeed, (unsigned)u16Got);
        if (u16Total < 9u) {
            u16Total = 9;
        }
        if (u16Total > sizeof(aCfg)) {
            u16Total = (u16)sizeof(aCfg);
        }

        /* Second fetch only if wTotalLength > bytes actually received. */
        if (u16Total > u16Got) {
            xhci_settle(300000u);
            (void)xhci_ep0_arm_after_address();
            setup.u8BmRequestType = 0x80u;
            setup.u8BRequest = USB_REQ_GET_DESCRIPTOR;
            setup.u16WValue = (u16)(USB_DT_CONFIG << 8);
            setup.u16WIndex = 0;
            setup.u16WLength = u16Total;
            fGot = 0;
            for (nTry = 0; nTry < 5; nTry++) {
                memset(aCfg, 0, u16Total);
                if (xhci_ctrl(&setup, aCfg, u16Total, 1) == 0 &&
                    aCfg[0] >= 9u && aCfg[1] == USB_DT_CONFIG) {
                    fGot = 1;
                    kprintf("xhci: get config full PASS total=%u try=%u\n",
                            (unsigned)u16Total, (unsigned)nTry);
                    break;
                }
                kprintf("xhci: get config full FAIL total=%u try=%u cc=%u\n",
                        (unsigned)u16Total, (unsigned)nTry,
                        (unsigned)g_u32CtrlFailCc);
                if (nTry + 1 < 5) {
                    /* Soft re-arm twice, then hard resync. */
                    if (nTry < 2) {
                        (void)xhci_ep0_arm_after_address();
                        xhci_settle(500000u);
                    } else {
                        xhci_ctrl_recover();
                    }
                }
            }
            if (fGot == 0) {
                kprintf("xhci: get config FAIL stage=15 total=%u cc=%u\n",
                        (unsigned)u16Total, (unsigned)g_u32CtrlFailCc);
                g_u32LastStage = 15;
                return -1;
            }
        }
    }

    if (xhci_parse_msc_config(aCfg, u16Total) != 0) {
        kprintf("xhci: no MSC BOT interface soft FAIL (cfg_total=%u)\n",
                (unsigned)u16Total);
        g_u32LastStage = 16; /* saw device, not BOT mass-storage */
        return -1;
    }

    memset(&setup, 0, sizeof(setup));
    setup.u8BmRequestType = 0x00u;
    setup.u8BRequest = USB_REQ_SET_CONFIGURATION;
    setup.u16WValue = g_u8ConfigVal;
    setup.u16WIndex = 0;
    setup.u16WLength = 0;
    {
        int nTry;
        int fOk = 0;

        for (nTry = 0; nTry < 3; nTry++) {
            if (xhci_ctrl(&setup, NULL, 0, 0) == 0) {
                fOk = 1;
                break;
            }
            kprintf("xhci: SET_CONFIGURATION FAIL try=%u cfg=%u cc=%u\n",
                    (unsigned)nTry, (unsigned)g_u8ConfigVal,
                    (unsigned)g_u32CtrlFailCc);
            if (nTry + 1 < 3) {
                xhci_ctrl_recover();
            }
        }
        if (fOk == 0) {
            kprintf("xhci: SET_CONFIGURATION FAIL stage=17 cfg=%u cc=%u\n",
                    (unsigned)g_u8ConfigVal, (unsigned)g_u32CtrlFailCc);
            g_u32LastStage = 17;
            return -1;
        }
    }

    if (xhci_configure_bulk() != 0) {
        g_u32LastStage = 18;
        return -1;
    }
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

    memset(aCdb, 0, sizeof(aCdb));
    aCdb[0] = SCSI_READ_CAPACITY10;
    if (msc_bot(aCdb, 10, aData, 8, 1) != 0) {
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
    /* GREENJADE (9 chars → short GREENJAD or GREENJ~1; blank extension) */
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

    if (g_u32BlockCount < RAW_LOG_SECTS + 2u || pBuf == NULL) {
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
            return -1;
        }
        return 0;
    }

    memcpy(aSec + u32Hdr, p, MSC_SECTOR - u32Hdr);
    if (msc_write10(u32Start, aSec, 1) != 0) {
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
            return -1;
        }
        u32Off += u32Chunk;
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
 *   1) SS-capable (xECP) with CCS — even if speed not yet trained
 *   2) PED + speed == 3 (HS) on SS-capable only
 * Never queue pure LS/FS (speed 1–2) — those are HID/webcam/BT (p10s1 was
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
    /* Soft SET_ADDRESS fail on non-SS is noise — never upgrade sticky to 20. */
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
     * Always skip LS/FS (speed 1–2) — HID/webcam/BT (panel p10s1 stage-20).
     * Skip HS on non-SS-cap when SS map exists (internal HS devices).
     * SS-capable ports may still show HS after cold PR (companion) — try those.
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
         * 12/13/19/20 → 8, so G752 panel showed purple+8 after a real
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

int
xhci_msc_init(void)
{
    u8 u8Bus = 0;
    u8 u8Slot = 0;
    u8 u8Func = 0;

    if (g_fInitTried != 0) {
        return 0;
    }
    g_fInitTried = 1;
    g_u32LastStage = 0;
    g_u32LastCc = 0;
    g_u32SsPortBits = 0;
    g_u8SsPortLo = 0;
    g_u8SsPortHi = 0;

    if (xhci_find_and_map(&u8Bus, &u8Slot, &u8Func) != 0) {
        g_u32LastStage = 1;
        kprintf("xhci: init SKIP (no xHCI)\n");
        return 0;
    }

    if (xhci_hc_reset_run() != 0) {
        g_u32LastStage = 2;
        kprintf("xhci: init FAIL (reset)\n");
        return 0;
    }
    if (xhci_setup_rings() != 0) {
        g_u32LastStage = 3;
        kprintf("xhci: init FAIL (rings)\n");
        return 0;
    }
    g_fHcUp = 1;
    g_u32LastStage = 4;
    kprintf("xhci: init PASS (hc up; probing ports)\n");

    if (xhci_try_ports() != 0) {
        kprintf("xhci: init soft PARTIAL (hc up, no MSC BOT stick) stage=%u\n",
                (unsigned)g_u32LastStage);
        return 0;
    }
    kprintf("xhci: init PASS msc_ready=1 stage=%u\n",
            (unsigned)g_u32LastStage);
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
     * Stages 19/20 → g_u32AddrFailCc (0 = timeout / never completed).
     * Stages 11/12/13/15/17 → g_u32CtrlFailCc (0 = timeout honesty).
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

/** 0=none 1=BSR0 2=BSR+soft — for STATUS path lamp. */
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
    if (g_fMscReady == 0 || pBuf == NULL || cb == 0) {
        kprintf("stick: log write FAIL bytes=%u path=none (not ready)\n",
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
    }

    if (raw_log_write(pBuf, cb) == 0) {
        kprintf("stick: log write PASS bytes=%u path=raw\n", (unsigned)cb);
        return 0;
    }

    kprintf("stick: log write FAIL bytes=%u path=none\n", (unsigned)cb);
    return -1;
}
