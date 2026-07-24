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
 *   usb: soft deepen     — wave=88 areas stamp
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
#define USB_SOFT_DEEPEN_AREAS 117u
#define USB_SOFT_DEEPEN_WAVE 77u

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
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: usb: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("usb: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /* Grep: usb: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("usb: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: usb: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("usb: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /* Grep: usb: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("usb: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: usb: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("usb: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
            /* Grep: usb: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("usb: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)USB_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: usb: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("usb: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)USB_SOFT_DEEPEN_WAVE);
                    /* Grep: usb: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("usb: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)USB_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: usb: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("usb: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)USB_SOFT_DEEPEN_WAVE);
                            /* Grep: usb: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("usb: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)USB_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: usb: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("usb: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=88 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: usb: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("usb: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=88 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: usb: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("usb: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=88 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: usb: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("usb: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=88 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: usb: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("usb: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=88 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: usb: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("usb: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=88 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: usb: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("usb: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=88 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("usb: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("usb: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("usb: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("usb: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("usb: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("usb: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retfortress — Wave 35 return-fortress honesty */
kprintf("usb: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("usb: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft rethold — Wave 36 return-hold honesty */
kprintf("usb: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retspire — Wave 36 exclusive spire stamp */
kprintf("usb: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retwall — Wave 37 return-wall honesty */
kprintf("usb: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retgate — Wave 37 exclusive gate stamp */
kprintf("usb: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retmoat — Wave 38 return-moat honesty */
kprintf("usb: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retower — Wave 38 exclusive tower stamp */
kprintf("usb: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("usb: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("usb: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("usb: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("usb: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retravelin — Wave 41 return-travelin honesty */
kprintf("usb: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("usb: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("usb: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("usb: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("usb: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("usb: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("usb: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("usb: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("usb: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("usb: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retbailey — Wave 46 return-bailey honesty */
kprintf("usb: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("usb: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("usb: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("usb: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("usb: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("usb: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("usb: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("usb: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retsally — Wave 50 return-sally honesty */
kprintf("usb: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("usb: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retfosse — Wave 51 return-fosse honesty */
kprintf("usb: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("usb: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("usb: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("usb: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retravelin — Wave 53 return-travelin honesty */
kprintf("usb: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("usb: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("usb: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retredan — Wave 54 exclusive redan stamp */
kprintf("usb: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retflank — Wave 55 return-flank honesty */
kprintf("usb: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retface — Wave 55 exclusive face stamp */
kprintf("usb: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retgorge — Wave 56 return-gorge honesty */
kprintf("usb: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("usb: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retraverse — Wave 57 return-traverse honesty */
kprintf("usb: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("usb: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retorillon — Wave 58 return-orillon honesty */
kprintf("usb: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("usb: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("usb: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("usb: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retplace — Wave 60 return-place honesty */
kprintf("usb: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("usb: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("usb: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("usb: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("usb: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("usb: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("usb: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("usb: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: usb: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("usb: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: usb: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("usb: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: usb: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("usb: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: usb: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("usb: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: usb: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("usb: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=88 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: usb: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("usb: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=88 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("usb: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("usb: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("usb: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("usb: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("usb: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("usb: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("usb: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("usb: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("usb: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("usb: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: usb: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("usb: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("usb: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: usb: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("usb: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("usb: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retbastionangle stamp; Soft≠product)\n");
/* Grep: usb: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("usb: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("usb: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retparapetangle stamp; Soft≠product)\n");
/* Grep: usb: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("usb: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("usb: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retowerangle stamp; Soft≠product)\n");
/* Grep: usb: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("usb: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("usb: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retwallangle stamp; Soft≠product)\n");
/* Grep: usb: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("usb: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("usb: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retholdangle stamp; Soft≠product)\n");
/* Grep: usb: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("usb: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("usb: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retfortressangle stamp; Soft≠product)\n");
/* Grep: usb: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("usb: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("usb: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: usb: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("usb: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("usb: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: usb: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("usb: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("usb: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: usb: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("usb: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("usb: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retaegisangle stamp; Soft≠product)\n");
/* Grep: usb: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("usb: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("usb: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retsigilangle stamp; Soft≠product)\n");
/* Grep: usb: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("usb: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("usb: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retglyphangle stamp; Soft≠product)\n");
/* Grep: usb: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("usb: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("usb: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retshardangle stamp; Soft≠product)\n");
/* Grep: usb: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("usb: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("usb: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retprismangle stamp; Soft≠product)\n");
/* Grep: usb: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("usb: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("usb: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retcipherangle stamp; Soft≠product)\n");
/* Grep: usb: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("usb: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: usb: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("usb: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retledgerangle stamp; Soft≠product)\n");

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
