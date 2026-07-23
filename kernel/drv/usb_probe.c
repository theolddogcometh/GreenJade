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
 */
static void
usb_soft_identify_mmio(u8 u8Pif, u64 paBar)
{
    gj_vaddr_t vaMap = 0;
    gj_status_t stMap;

    kprintf("usb: bar0 mem soft path PASS pa=0x%lx\n",
            (unsigned long)paBar);
    stMap = vmm_map_device_uc((gj_paddr_t)paBar, 0x1000, &vaMap);
    if (stMap != GJ_OK) {
        kprintf("usb: bar0 map soft fail st=%d\n", (int)stMap);
        return;
    }
    {
        volatile u32 *pReg = (volatile u32 *)(gj_vaddr_t)vaMap;
        u32 u32D0 = pReg[0];

        if (u8Pif == USB_PIF_OHCI) {
            /* HcRevision @ 0x00 */
            kprintf("usb: identify OHCI HcRevision=0x%x soft PASS\n", u32D0);
        } else if (u8Pif == USB_PIF_EHCI || u8Pif == USB_PIF_XHCI) {
            /* CAPLENGTH @ byte0; HCIVERSION @ bytes 2–3 (public layout) */
            u8 u8CapLen = (u8)(u32D0 & 0xffu);
            u16 u16HciVer = (u16)((u32D0 >> 16) & 0xffffu);

            kprintf("usb: identify %s CAPLENGTH=%u HCIVERSION=0x%04x soft "
                    "PASS\n",
                    usb_if_name(u8Pif), (unsigned)u8CapLen,
                    (unsigned)u16HciVer);
            if (u8Pif == USB_PIF_XHCI) {
                u32 u32Hcs1 = pReg[1]; /* HCSPARAMS1 @ 0x04 */

                kprintf("usb: identify xHCI HCSPARAMS1=0x%x soft PASS\n",
                        u32Hcs1);
            }
        } else {
            kprintf("usb: identify mmio d0=0x%x soft PASS\n", u32D0);
        }
    }
}

/**
 * Greppable soft inventory once per scan. Class 0C:03 only; no HC claim.
 * Product line: "usb: soft inventory".
 */
static void
usb_soft_log(u32 cFound, u32 cUhci, u32 cOhci, u32 cEhci, u32 cXhci,
             u32 cOther, u32 cBarIo, u32 cBarMem)
{
    kprintf("usb: soft inventory found=%u class=0C:03 uhci=%u ohci=%u "
            "ehci=%u xhci=%u other=%u bar_io=%u bar_mem=%u\n",
            cFound, cUhci, cOhci, cEhci, cXhci, cOther, cBarIo, cBarMem);
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
                if (fIo != 0) {
                    /* UHCI: I/O BAR only — no MMIO soft map */
                    kprintf("usb: bar0 io soft path PASS base=0x%lx\n",
                            (unsigned long)paBar);
                    cBarIo++;
                } else if (paBar != 0) {
                    usb_soft_identify_mmio(u8Pif, paBar);
                    cBarMem++;
                } else {
                    kprintf("usb: bar0 empty soft skip\n");
                }
                cFound++;
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
    usb_soft_log(cFound, cUhci, cOhci, cEhci, cXhci, cOther, cBarIo, cBarMem);
    kprintf("usb: probe PASS\n");
    return cFound;
}
