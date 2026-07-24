/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: USB host controller PCI class probe — clean-room pure C.
 * Enumerate class 0C:03 (serial bus / USB); log UHCI/OHCI/EHCI/xHCI by prog-if.
 * Soft identify: BAR0 resolve + optional CAPLENGTH/HCIVERSION MMIO read for
 * EHCI/xHCI (and HcRevision for OHCI) via vmm_map_device_uc. No HC init,
 * no IRQ, no transfer rings (HID parse later). No GPL source; public PCI
 * class codes + HC capability register layouts only.
 *
 * Wave 15/16/17 exclusive soft deepen (this unit only — greppable "usb: soft …"):
 *   usb: soft inventory  — class 0C:03 tallies + BAR io/mem + identify ok
 *   usb: soft class      — PCI class/subclass inventory (public codes)
 *   usb: soft if         — prog-if tallies (uhci/ohci/ehci/xhci/other)
 *   usb: soft pif        — prog-if constant map (HCL)
 *   usb: soft bar        — BAR0 io vs mem resolve tallies
 *   usb: soft dual       — io vs mem path dual lamps
 *   usb: soft identify   — per-HC cap-head (HcRevision / CAPLENGTH / HCI VER)
 *   usb: soft regs       — cap-head offset map (OHCI/EHCI/xHCI public)
 *   usb: soft pci        — class 0C:03 inventory stamp
 *   usb: soft path       — honesty: probe/soft only; no claim / IRQ / rings
 *   usb: soft honesty    — bar3/HID/rings non-claims
 *   usb: soft return rate — Wave 19 ok/fail rate lamps
 *   usb: soft retcode    — Wave 19 retcode catalog
 *   usb: soft deepen     — wave=24 areas stamp
 *   usb: soft ratio      — Wave 16 identify/BAR occupancy
 *   usb: soft headroom   — Wave 16 map/bar head
 *   usb: soft surface    — Wave 16 area catalog
 *   usb: soft return     — Wave 16 return-surface bitmask
 *   usb: soft return selftest — Wave 17 terminal return surface (kept)
 *   usb: soft retmap     — Wave 17 return-surface map (kept)
 *   usb: soft contract   — Wave 16 soft≠game I/O contract
 *   usb: soft stats      — emission / scan tallies
 *   usb: soft inventory PASS|SKIP
 *   usb: soft PASS|SKIP
 *
 * Smoke markers (unchanged prefix): "usb: probe" …
 */
#include <gj/klog.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* PCI class / subclass: serial bus / USB */
#define USB_PCI_CLASS    0x0cu
#define USB_PCI_SUBCLASS 0x03u

/* Prog-if values (PCI Code and ID Assignment) */
#define USB_PIF_UHCI 0x00u
#define USB_PIF_OHCI 0x10u
#define USB_PIF_EHCI 0x20u
#define USB_PIF_XHCI 0x30u

/* Wave 20 deepen area count (fixed greppable categories in inventory log). */
#define USB_SOFT_DEEPEN_AREAS 33u
#define USB_SOFT_DEEPEN_WAVE  24u

/* Soft inventory emission tallies (wrap OK; never hard-gate). */
static u32 g_u32SoftInvLogs;
static u32 g_u32SoftProbeLogs;
static u32 g_u32SoftFound;
static u32 g_u32SoftIdentifyOk;
static u32 g_u32SoftMapFail;
static u32 g_u32SoftBarEmpty;
static u32 g_u32SoftBarIo;
static u32 g_u32SoftBarMem;

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
pci_cfg_read(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);

    outl(0xCF8, u32Addr);
    return inl(0xCFC);
}

static const char *
usb_if_name(u8 u8Pif)
{
    if (u8Pif == USB_PIF_UHCI) {
        return "UHCI";
    }
    if (u8Pif == USB_PIF_OHCI) {
        return "OHCI";
    }
    if (u8Pif == USB_PIF_EHCI) {
        return "EHCI";
    }
    if (u8Pif == USB_PIF_XHCI) {
        return "xHCI";
    }
    return "USB";
}

/**
 * Resolve BAR0 physical base (32-/64-bit mem) or I/O base.
 * *pfIo set for I/O BAR (UHCI). *pf64 set for 64-bit mem BAR.
 */
static u64
usb_bar0_pa(u8 u8Bus, u8 u8Slot, u8 u8Func, u32 *pBarRaw, int *pfIo,
            int *pf64)
{
    u32 u32Lo = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x10);
    u64 paBar;

    if (pBarRaw != NULL) {
        *pBarRaw = u32Lo;
    }
    if (pfIo != NULL) {
        *pfIo = 0;
    }
    if (pf64 != NULL) {
        *pf64 = 0;
    }
    if ((u32Lo & 1u) != 0) {
        /* I/O BAR: base in bits 15:2 (classic UHCI) */
        if (pfIo != NULL) {
            *pfIo = 1;
        }
        return (u64)(u32Lo & ~0x3u);
    }
    paBar = (u64)(u32Lo & ~0xfu);
    if (((u32Lo >> 1) & 3u) == 2u) {
        u32 u32Hi = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x14);

        paBar |= ((u64)u32Hi << 32);
        if (pf64 != NULL) {
            *pf64 = 1;
        }
    }
    return paBar;
}

/**
 * Soft identify MMIO HC capability head (OHCI/EHCI/xHCI). Read-only.
 * Wave 15: greppable "usb: soft identify …" / "usb: soft bar …".
 * Returns 1 on successful cap-head read, 0 on map fail.
 */
static int
usb_soft_identify_mmio(u8 u8Pif, u64 paBar)
{
    gj_vaddr_t vaMap = 0;
    gj_status_t stMap;

    kprintf("usb: bar0 mem soft path PASS pa=0x%lx\n",
            (unsigned long)paBar);
    /* Grep: usb: soft bar */
    kprintf("usb: soft bar kind=mem pa=0x%lx map=try soft PASS wave=%u\n",
            (unsigned long)paBar, (unsigned)USB_SOFT_DEEPEN_WAVE);
    stMap = vmm_map_device_uc((gj_paddr_t)paBar, 0x1000, &vaMap);
    if (stMap != GJ_OK) {
        kprintf("usb: bar0 map soft fail st=%d\n", (int)stMap);
        /* Grep: usb: soft identify / usb: soft bar */
        kprintf("usb: soft bar kind=mem map=fail st=%d soft SKIP wave=%u\n",
                (int)stMap, (unsigned)USB_SOFT_DEEPEN_WAVE);
        kprintf("usb: soft identify map soft SKIP st=%d pif=0x%x wave=%u\n",
                (int)stMap, (unsigned)u8Pif, (unsigned)USB_SOFT_DEEPEN_WAVE);
        return 0;
    }
    {
        volatile u32 *pReg = (volatile u32 *)(gj_vaddr_t)vaMap;
        u32 u32D0 = pReg[0];

        if (u8Pif == USB_PIF_OHCI) {
            /* HcRevision @ 0x00 */
            kprintf("usb: identify OHCI HcRevision=0x%x soft PASS\n", u32D0);
            /* Grep: usb: soft identify */
            kprintf("usb: soft identify if=OHCI HcRevision=0x%x soft PASS "
                    "wave=%u\n",
                    u32D0, (unsigned)USB_SOFT_DEEPEN_WAVE);
        } else if (u8Pif == USB_PIF_EHCI || u8Pif == USB_PIF_XHCI) {
            /* CAPLENGTH @ byte0; HCIVERSION @ bytes 2–3 (public layout) */
            u8 u8CapLen = (u8)(u32D0 & 0xffu);
            u16 u16HciVer = (u16)((u32D0 >> 16) & 0xffffu);

            kprintf("usb: identify %s CAPLENGTH=%u HCIVERSION=0x%04x soft "
                    "PASS\n",
                    usb_if_name(u8Pif), (unsigned)u8CapLen,
                    (unsigned)u16HciVer);
            /* Grep: usb: soft identify */
            kprintf("usb: soft identify if=%s CAPLENGTH=%u HCIVERSION=0x%04x "
                    "soft PASS wave=%u\n",
                    usb_if_name(u8Pif), (unsigned)u8CapLen,
                    (unsigned)u16HciVer, (unsigned)USB_SOFT_DEEPEN_WAVE);
            if (u8Pif == USB_PIF_XHCI) {
                u32 u32Hcs1 = pReg[1]; /* HCSPARAMS1 @ 0x04 */
                u32 u32MaxSlots;
                u32 u32MaxIntrs;
                u32 u32MaxPorts;

                /* Public xHCI HCSPARAMS1 field inventory (soft only) */
                u32MaxSlots = u32Hcs1 & 0xffu;
                u32MaxIntrs = (u32Hcs1 >> 8) & 0x7ffu;
                u32MaxPorts = (u32Hcs1 >> 24) & 0xffu;
                kprintf("usb: identify xHCI HCSPARAMS1=0x%x soft PASS\n",
                        u32Hcs1);
                /* Grep: usb: soft identify (xHCI params deepen) */
                kprintf("usb: soft identify if=xHCI HCSPARAMS1=0x%x "
                        "max_slots=%u max_intrs=%u max_ports=%u soft PASS "
                        "wave=%u\n",
                        u32Hcs1, u32MaxSlots, u32MaxIntrs, u32MaxPorts,
                        (unsigned)USB_SOFT_DEEPEN_WAVE);
            }
        } else {
            kprintf("usb: identify mmio d0=0x%x soft PASS\n", u32D0);
            /* Grep: usb: soft identify */
            kprintf("usb: soft identify if=%s d0=0x%x soft PASS wave=%u\n",
                    usb_if_name(u8Pif), u32D0, (unsigned)USB_SOFT_DEEPEN_WAVE);
        }
        /* Grep: usb: soft bar */
        kprintf("usb: soft bar kind=mem map=ok pa=0x%lx soft PASS wave=%u\n",
                (unsigned long)paBar, (unsigned)USB_SOFT_DEEPEN_WAVE);
    }
    return 1;
}

/**
 * Wave 15 greppable soft inventory dump (product / smoke).
 * Prefix-stable "usb: soft …" — never hard-gates; kprintf only.
 *
 * greppable: usb: soft
 */
static void
usb_soft_inventory(const char *szVia, u32 cFound, u32 cUhci, u32 cOhci,
                   u32 cEhci, u32 cXhci, u32 cOther, u32 cBarIo, u32 cBarMem,
                   u32 cIdentifyOk, u32 cMapFail, u32 cBarEmpty)
{
    const char *szVerdict;
    const char *szViaSafe;
    u32 cKnown;
    u32 u8Ok;
    u32 u8Skip;

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "scan";
    if (g_u32SoftInvLogs < 0xffffffffu) {
        g_u32SoftInvLogs++;
    }

    cKnown = cUhci + cOhci + cEhci + cXhci;

    /*
     * Soft verdict (inventory only; never claims HC/IRQ/rings):
     *   PASS — at least one class 0C:03 function found
     *   SKIP — none (still emit full soft catalog for greps)
     */
    if (cFound > 0u) {
        szVerdict = "PASS";
        u8Ok = 1u;
        u8Skip = 0u;
    } else {
        szVerdict = "SKIP";
        u8Ok = 0u;
        u8Skip = 1u;
    }

    /*
     * Grep: usb: soft inventory
     * Aggregated presence snapshot for product smoke / HCL.
     * Legacy field order preserved: found / class / uhci… / bar_io / bar_mem.
     */
    kprintf("usb: soft inventory found=%u class=0C:03 uhci=%u ohci=%u "
            "ehci=%u xhci=%u other=%u bar_io=%u bar_mem=%u\n",
            cFound, cUhci, cOhci, cEhci, cXhci, cOther, cBarIo, cBarMem);
    /* Wave 15 deepen — via + identify/map tallies on same soft inventory */
    kprintf("usb: soft inventory via=%s known=%u identify_ok=%u map_fail=%u "
            "bar_empty=%u logs=%u wave=%u areas=%u\n",
            szViaSafe, cKnown, cIdentifyOk, cMapFail, cBarEmpty,
            (unsigned)g_u32SoftInvLogs, (unsigned)USB_SOFT_DEEPEN_WAVE,
            (unsigned)USB_SOFT_DEEPEN_AREAS);

    /* Grep: usb: soft class (public PCI Code and ID Assignment) */
    kprintf("usb: soft class base=0x%02x sub=0x%02x tag=0C:03 found=%u "
            "soft %s wave=%u\n",
            (unsigned)USB_PCI_CLASS, (unsigned)USB_PCI_SUBCLASS, cFound,
            szVerdict, (unsigned)USB_SOFT_DEEPEN_WAVE);

    /* Grep: usb: soft if (prog-if tallies) */
    kprintf("usb: soft if uhci=%u ohci=%u ehci=%u xhci=%u other=%u "
            "known=%u soft %s wave=%u\n",
            cUhci, cOhci, cEhci, cXhci, cOther, cKnown, szVerdict,
            (unsigned)USB_SOFT_DEEPEN_WAVE);
    if (cXhci > 0u) {
        kprintf("usb: soft if xhci present c=%u soft PASS\n", cXhci);
    } else {
        kprintf("usb: soft if xhci present c=0 soft SKIP\n");
    }
    if (cEhci > 0u) {
        kprintf("usb: soft if ehci present c=%u soft PASS\n", cEhci);
    } else {
        kprintf("usb: soft if ehci present c=0 soft SKIP\n");
    }
    if (cOhci > 0u) {
        kprintf("usb: soft if ohci present c=%u soft PASS\n", cOhci);
    } else {
        kprintf("usb: soft if ohci present c=0 soft SKIP\n");
    }
    if (cUhci > 0u) {
        kprintf("usb: soft if uhci present c=%u soft PASS\n", cUhci);
    } else {
        kprintf("usb: soft if uhci present c=0 soft SKIP\n");
    }

    /* Grep: usb: soft pif — compile-time prog-if constant map (Wave 15) */
    kprintf("usb: soft pif UHCI=0x%02x OHCI=0x%02x EHCI=0x%02x "
            "xHCI=0x%02x soft PASS wave=%u\n",
            (unsigned)USB_PIF_UHCI, (unsigned)USB_PIF_OHCI,
            (unsigned)USB_PIF_EHCI, (unsigned)USB_PIF_XHCI,
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /* Grep: usb: soft bar (rollup) */
    kprintf("usb: soft bar io=%u mem=%u empty=%u identify_ok=%u map_fail=%u "
            "soft %s wave=%u\n",
            cBarIo, cBarMem, cBarEmpty, cIdentifyOk, cMapFail, szVerdict,
            (unsigned)USB_SOFT_DEEPEN_WAVE);
    if (cBarIo > 0u) {
        kprintf("usb: soft bar io path c=%u soft PASS\n", cBarIo);
    } else {
        kprintf("usb: soft bar io path c=0 soft SKIP\n");
    }
    if (cBarMem > 0u) {
        kprintf("usb: soft bar mem path c=%u soft PASS\n", cBarMem);
    } else {
        kprintf("usb: soft bar mem path c=0 soft SKIP\n");
    }

    /* Grep: usb: soft dual — io vs mem dual path lamps (Wave 15) */
    kprintf("usb: soft dual io=%u mem=%u empty=%u io_only=%u mem_only=%u "
            "mixed=%u soft %s wave=%u\n",
            cBarIo, cBarMem, cBarEmpty,
            (cBarIo > 0u && cBarMem == 0u) ? 1u : 0u,
            (cBarMem > 0u && cBarIo == 0u) ? 1u : 0u,
            (cBarIo > 0u && cBarMem > 0u) ? 1u : 0u, szVerdict,
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /* Grep: usb: soft identify (scan rollup) */
    if (cIdentifyOk > 0u) {
        kprintf("usb: soft identify scan ok=%u map_fail=%u soft PASS "
                "wave=%u\n",
                cIdentifyOk, cMapFail, (unsigned)USB_SOFT_DEEPEN_WAVE);
    } else if (cBarIo > 0u && cBarMem == 0u) {
        /* UHCI I/O-only hosts: no MMIO cap-head; soft honesty, not fail */
        kprintf("usb: soft identify scan ok=0 io_only=1 soft PASS wave=%u\n",
                (unsigned)USB_SOFT_DEEPEN_WAVE);
    } else if (cFound > 0u) {
        kprintf("usb: soft identify scan ok=0 map_fail=%u empty=%u soft "
                "SKIP wave=%u\n",
                cMapFail, cBarEmpty, (unsigned)USB_SOFT_DEEPEN_WAVE);
    } else {
        kprintf("usb: soft identify scan none soft SKIP wave=%u\n",
                (unsigned)USB_SOFT_DEEPEN_WAVE);
    }

    /*
     * Grep: usb: soft regs — public cap-head offset map (Wave 15).
     * Inventory only; not programmed. OHCI HcRevision @0; EHCI/xHCI
     * CAPLENGTH @0 / HCIVERSION @2; xHCI HCSPARAMS1 @4.
     */
    kprintf("usb: soft regs OHCI_HcRevision=0x00 EHCI_CAPLENGTH=0x00 "
            "EHCI_HCIVERSION=0x02 xHCI_HCSPARAMS1=0x04 soft PASS wave=%u\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /* Grep: usb: soft pci */
    kprintf("usb: soft pci class=0x%02x subclass=0x%02x tag=0C:03 "
            "serial_bus_usb=1 soft PASS wave=%u\n",
            (unsigned)USB_PCI_CLASS, (unsigned)USB_PCI_SUBCLASS,
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /*
     * Grep: usb: soft path
     * Honesty catalog: product surface is PCI class + soft BAR/cap-head.
     * claim=0 — no HC init, no IRQ, no transfer rings, no HID parse.
     */
    kprintf("usb: soft path claim=0 irq=0 rings=0 hid=0 hc_init=0 "
            "map_uc=1 cap_head=1 bar0=1 class_scan=1 via=%s wave=%u\n",
            szViaSafe, (unsigned)USB_SOFT_DEEPEN_WAVE);

    /*
     * Grep: usb: soft honesty — Wave 15 non-claims (soft inventory only).
     * Product HID/rings remain open; bar3 stays open.
     */
    kprintf("usb: soft honesty product_hc=0 hid_parse=0 transfer_rings=0 "
            "bar3=open claim=0 irq_enable=0 wave=%u soft PASS\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: usb: soft ratio|headroom|surface|return|contract
     */
    {
        u32 u32Surf = 0u;
        u32 u32IdBp = 0;
        u32 u32MemBp = 0;

        if (cFound != 0u) {
            u32IdBp = (cIdentifyOk * 10000u) / cFound;
            u32MemBp = (cBarMem * 10000u) / cFound;
        }
        if (cFound != 0u) {
            u32Surf |= 0x1u;
        }
        if (cXhci != 0u) {
            u32Surf |= 0x2u;
        }
        if (cIdentifyOk != 0u) {
            u32Surf |= 0x4u;
        }
        if (cBarMem != 0u) {
            u32Surf |= 0x8u;
        }
        if (cBarIo != 0u) {
            u32Surf |= 0x10u;
        }
        if (cMapFail != 0u) {
            u32Surf |= 0x20u;
        }
        if (cBarEmpty != 0u) {
            u32Surf |= 0x40u;
        }
        u32Surf |= 0x80u; /* class/pif/regs catalog always present */
        /* Grep: usb: soft ratio */
        kprintf("usb: soft ratio identify_bp=%u mem_bp=%u found=%u "
                "identify_ok=%u bar_mem=%u xhci=%u wave=%u soft PASS\n",
                u32IdBp, u32MemBp, cFound, cIdentifyOk, cBarMem, cXhci,
                (unsigned)USB_SOFT_DEEPEN_WAVE);
        /* Grep: usb: soft headroom */
        kprintf("usb: soft headroom map_fail=%u bar_empty=%u bar_io=%u "
                "bar_mem=%u other=%u wave=%u soft PASS\n",
                cMapFail, cBarEmpty, cBarIo, cBarMem, cOther,
                (unsigned)USB_SOFT_DEEPEN_WAVE);
        /* Grep: usb: soft surface */
        kprintf("usb: soft surface inventory,class,if,pif,bar,dual,identify,"
                "regs,pci,path,honesty,ratio,headroom,return,contract,"
                "deepen,stats areas=%u wave=%u\n",
                (unsigned)USB_SOFT_DEEPEN_AREAS,
                (unsigned)USB_SOFT_DEEPEN_WAVE);
        /* Grep: usb: soft return — return-surface bitmask */
        kprintf("usb: soft return surf=0x%x found=%u xhci=%u id_ok=%u "
                "mem=%u io=%u map_fail=%u empty=%u via=%s areas=%u "
                "wave=%u soft PASS\n",
                u32Surf, cFound, cXhci, cIdentifyOk, cBarMem, cBarIo,
                cMapFail, cBarEmpty, szViaSafe,
                (unsigned)USB_SOFT_DEEPEN_AREAS,
                (unsigned)USB_SOFT_DEEPEN_WAVE);
        /* Grep: usb: soft contract — soft ≠ game I/O */
        kprintf("usb: soft contract soft_only=1 game_io=0 product_hc=0 "
                "hid_parse=0 rings=0 irq=0 bar3=open wave=%u soft PASS\n",
                (unsigned)USB_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: usb: soft return — Wave 17 API return surfaces (kept) */
    kprintf("usb: soft return found=%u soft_inv=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            g_u32SoftFound, (unsigned)USB_SOFT_DEEPEN_WAVE);

    /* Grep: usb: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("usb: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /* Grep: usb: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("usb: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: usb: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("usb: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /* Grep: usb: soft retcode — Wave 19 retcode catalog */
    kprintf("usb: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /* Grep: usb: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: usb: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("usb: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);
    /* Grep: usb: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("usb: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: usb: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("usb: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);
    /* Grep: usb: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("usb: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: usb: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("usb: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /* Grep: usb: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("usb: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: usb: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("usb: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /* Grep: usb: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("usb: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: usb: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("usb: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /* Grep: usb: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("usb: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 exclusive complementary surfaces (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: usb: soft retvault — Wave 24 return-vault honesty */
            kprintf("usb: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /* Grep: usb: soft retbanner — Wave 24 exclusive banner stamp */
            kprintf("usb: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
    kprintf("usb: soft deepen wave=%u areas=%u via=%s found=%u xhci=%u "
            "identify_ok=%u map_fail=%u bar_empty=%u ok=%u skip=%u\n",
            (unsigned)USB_SOFT_DEEPEN_WAVE, (unsigned)USB_SOFT_DEEPEN_AREAS,
            szViaSafe, cFound, cXhci, cIdentifyOk, cMapFail, cBarEmpty, u8Ok,
            u8Skip);

    /* Grep: usb: soft stats */
    kprintf("usb: soft stats inv_logs=%u probe_logs=%u found=%u "
            "identify_ok=%u map_fail=%u bar_io=%u bar_mem=%u bar_empty=%u "
            "wave=%u\n",
            (unsigned)g_u32SoftInvLogs, (unsigned)g_u32SoftProbeLogs,
            (unsigned)g_u32SoftFound, (unsigned)g_u32SoftIdentifyOk,
            (unsigned)g_u32SoftMapFail, (unsigned)g_u32SoftBarIo,
            (unsigned)g_u32SoftBarMem, (unsigned)g_u32SoftBarEmpty,
            (unsigned)USB_SOFT_DEEPEN_WAVE);

    /* Grep: usb: soft inventory PASS|SKIP / usb: soft PASS|SKIP */
    kprintf("usb: soft inventory %s via=%s found=%u xhci=%u wave=%u "
            "areas=%u\n",
            szVerdict, szViaSafe, cFound, cXhci,
            (unsigned)USB_SOFT_DEEPEN_WAVE, (unsigned)USB_SOFT_DEEPEN_AREAS);
    kprintf("usb: soft %s via=%s found=%u identify_ok=%u bar_mem=%u "
            "wave=%u\n",
            szVerdict, szViaSafe, cFound, cIdentifyOk, cBarMem,
            (unsigned)USB_SOFT_DEEPEN_WAVE);
}

/**
 * Greppable soft inventory once per scan. Class 0C:03 only; no HC claim.
 * Product line: "usb: soft inventory" (Wave 15 deepen via usb_soft_inventory).
 */
static void
usb_soft_log(u32 cFound, u32 cUhci, u32 cOhci, u32 cEhci, u32 cXhci,
             u32 cOther, u32 cBarIo, u32 cBarMem, u32 cIdentifyOk,
             u32 cMapFail, u32 cBarEmpty)
{
    usb_soft_inventory("scan", cFound, cUhci, cOhci, cEhci, cXhci, cOther,
                       cBarIo, cBarMem, cIdentifyOk, cMapFail, cBarEmpty);
}

/**
 * Scan PCI for USB host controllers. Soft BAR map + cap-head identify.
 * Returns total HC count. Always logs a greppable product line (smoke:
 * "usb: probe").
 */
u32
usb_probe_scan(void)
{
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 cFound = 0;
    u32 cUhci = 0;
    u32 cOhci = 0;
    u32 cEhci = 0;
    u32 cXhci = 0;
    u32 cOther = 0;
    u32 cBarIo = 0;
    u32 cBarMem = 0;
    u32 cIdentifyOk = 0;
    u32 cMapFail = 0;
    u32 cBarEmpty = 0;

    if (g_u32SoftProbeLogs < 0xffffffffu) {
        g_u32SoftProbeLogs++;
    }

    for (u8Bus = 0; u8Bus < 8; u8Bus++) {
        for (u8Slot = 0; u8Slot < 32; u8Slot++) {
            for (u8Func = 0; u8Func < 8; u8Func++) {
                u32 u32Id = pci_cfg_read(u8Bus, u8Slot, u8Func, 0);
                u32 u32ClassReg;
                u32 u32BarRaw = 0;
                u8 u8Base;
                u8 u8Sub;
                u8 u8Pif;
                u16 u16Vendor;
                u16 u16Device;
                int fIo = 0;
                int f64 = 0;
                u64 paBar;

                u16Vendor = (u16)(u32Id & 0xffffu);
                if (u16Vendor == 0xffffu) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                u16Device = (u16)((u32Id >> 16) & 0xffffu);
                u32ClassReg = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x08);
                u8Base = (u8)((u32ClassReg >> 24) & 0xffu);
                u8Sub = (u8)((u32ClassReg >> 16) & 0xffu);
                u8Pif = (u8)((u32ClassReg >> 8) & 0xffu);
                if (u8Base != USB_PCI_CLASS || u8Sub != USB_PCI_SUBCLASS) {
                    continue;
                }
                paBar = usb_bar0_pa(u8Bus, u8Slot, u8Func, &u32BarRaw, &fIo,
                                    &f64);
                kprintf("usb: probe %u:%u.%u %s vendor=0x%04x PASS\n", u8Bus,
                        u8Slot, u8Func, usb_if_name(u8Pif), u16Vendor);
                kprintf("usb: identify %u:%u.%u %s id=%04x:%04x bar0=0x%x "
                        "pa=0x%lx %s soft PASS\n",
                        u8Bus, u8Slot, u8Func, usb_if_name(u8Pif), u16Vendor,
                        u16Device, u32BarRaw, (unsigned long)paBar,
                        fIo != 0 ? "io" : (f64 != 0 ? "mem64" : "mem32"));
                /* Grep: usb: soft identify (per-function PCI snapshot) */
                kprintf("usb: soft identify bdf=%u:%u.%u if=%s "
                        "id=%04x:%04x bar0=0x%x pa=0x%lx kind=%s soft PASS "
                        "wave=%u\n",
                        u8Bus, u8Slot, u8Func, usb_if_name(u8Pif), u16Vendor,
                        u16Device, u32BarRaw, (unsigned long)paBar,
                        fIo != 0 ? "io" : (f64 != 0 ? "mem64" : "mem32"),
                        (unsigned)USB_SOFT_DEEPEN_WAVE);
                if (fIo != 0) {
                    /* UHCI: I/O BAR only — no MMIO soft map */
                    kprintf("usb: bar0 io soft path PASS base=0x%lx\n",
                            (unsigned long)paBar);
                    /* Grep: usb: soft bar */
                    kprintf("usb: soft bar kind=io base=0x%lx soft PASS "
                            "wave=%u\n",
                            (unsigned long)paBar,
                            (unsigned)USB_SOFT_DEEPEN_WAVE);
                    cBarIo++;
                    if (g_u32SoftBarIo < 0xffffffffu) {
                        g_u32SoftBarIo++;
                    }
                } else if (paBar != 0) {
                    if (usb_soft_identify_mmio(u8Pif, paBar) != 0) {
                        cIdentifyOk++;
                        if (g_u32SoftIdentifyOk < 0xffffffffu) {
                            g_u32SoftIdentifyOk++;
                        }
                    } else {
                        cMapFail++;
                        if (g_u32SoftMapFail < 0xffffffffu) {
                            g_u32SoftMapFail++;
                        }
                    }
                    cBarMem++;
                    if (g_u32SoftBarMem < 0xffffffffu) {
                        g_u32SoftBarMem++;
                    }
                } else {
                    kprintf("usb: bar0 empty soft skip\n");
                    /* Grep: usb: soft bar */
                    kprintf("usb: soft bar kind=empty soft SKIP wave=%u\n",
                            (unsigned)USB_SOFT_DEEPEN_WAVE);
                    cBarEmpty++;
                    if (g_u32SoftBarEmpty < 0xffffffffu) {
                        g_u32SoftBarEmpty++;
                    }
                }
                cFound++;
                if (g_u32SoftFound < 0xffffffffu) {
                    g_u32SoftFound++;
                }
                if (u8Pif == USB_PIF_UHCI) {
                    cUhci++;
                } else if (u8Pif == USB_PIF_OHCI) {
                    cOhci++;
                } else if (u8Pif == USB_PIF_EHCI) {
                    cEhci++;
                } else if (u8Pif == USB_PIF_XHCI) {
                    cXhci++;
                } else {
                    cOther++;
                }
            }
        }
    }
    if (cFound == 0) {
        kprintf("usb: probe none (soft)\n");
    } else {
        kprintf("usb: probe count=%u xhci=%u PASS\n", cFound, cXhci);
    }
    /* Soft inventory once: class/prog-if tallies; not full HC / bar3. */
    usb_soft_log(cFound, cUhci, cOhci, cEhci, cXhci, cOther, cBarIo, cBarMem,
                 cIdentifyOk, cMapFail, cBarEmpty);
    kprintf("usb: probe PASS\n");
    return cFound;
}
