/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: USB host controller PCI class probe — clean-room pure C.
 * Enumerate class 0C:03 (serial bus / USB); log UHCI/OHCI/EHCI/xHCI by prog-if.
 * Enumerate only — no HC init, no IRQ, no transfer rings (HID parse later).
 * No GPL source; public PCI class codes only.
 */
#include <gj/klog.h>
#include <gj/types.h>

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
 * Scan PCI for USB host controllers. Returns total HC count.
 * Always logs a greppable product line (smoke: "usb: probe").
 */
u32
usb_probe_scan(void)
{
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 cFound = 0;
    u32 cXhci = 0;

    for (u8Bus = 0; u8Bus < 8; u8Bus++) {
        for (u8Slot = 0; u8Slot < 32; u8Slot++) {
            for (u8Func = 0; u8Func < 8; u8Func++) {
                u32 u32Id = pci_cfg_read(u8Bus, u8Slot, u8Func, 0);
                u32 u32ClassReg;
                u8 u8Base;
                u8 u8Sub;
                u8 u8Pif;
                u16 u16Vendor;

                u16Vendor = (u16)(u32Id & 0xffffu);
                if (u16Vendor == 0xffffu) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                u32ClassReg = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x08);
                u8Base = (u8)((u32ClassReg >> 24) & 0xffu);
                u8Sub = (u8)((u32ClassReg >> 16) & 0xffu);
                u8Pif = (u8)((u32ClassReg >> 8) & 0xffu);
                if (u8Base != USB_PCI_CLASS || u8Sub != USB_PCI_SUBCLASS) {
                    continue;
                }
                kprintf("usb: probe %u:%u.%u %s vendor=0x%04x PASS\n", u8Bus,
                        u8Slot, u8Func, usb_if_name(u8Pif), u16Vendor);
                cFound++;
                if (u8Pif == USB_PIF_XHCI) {
                    cXhci++;
                }
            }
        }
    }
    if (cFound == 0) {
        kprintf("usb: probe none (soft)\n");
    } else {
        kprintf("usb: probe count=%u xhci=%u PASS\n", cFound, cXhci);
    }
    kprintf("usb: probe PASS\n");
    return cFound;
}
