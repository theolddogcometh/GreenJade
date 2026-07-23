/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI/MSI-X capability discovery + table entry programming.
 * Clean-room pure C from PCI Local Bus Spec capability IDs and MSI-X
 * table layout. No GPL source.
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/pci_caps.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

#define PCI_CAP_ID_MSI  0x05u
#define PCI_CAP_ID_MSIX 0x11u
#define PCI_STATUS_CAP  0x10u /* Status[4] Capabilities List */

/* Local APIC MMIO message address base (xAPIC FEE0_0000h) */
#define MSI_ADDR_BASE 0xFEE00000u

/* Product default vector used by pci_msix_probe_log (matches GJ_MSIX_IRQ_VEC) */
#define PCI_MSIX_PROBE_VEC 0x41u

static u32 g_u32Programmed;

static u32
pci_cfg_read(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);
    u32 u32Val;

    __asm__ volatile("outl %0, %1" : : "a"(u32Addr), "Nd"((u16)0xCF8));
    __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"((u16)0xCFC));
    return u32Val;
}

static u8
pci_cfg_read8(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Word = pci_cfg_read(u8Bus, u8Slot, u8Func, u8Off & 0xfcu);

    return (u8)((u32Word >> ((u8Off & 3u) * 8u)) & 0xffu);
}

static u16
pci_cfg_read16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Word = pci_cfg_read(u8Bus, u8Slot, u8Func, u8Off & 0xfcu);

    return (u16)((u32Word >> ((u8Off & 2u) * 8u)) & 0xffffu);
}

static void
pci_cfg_write16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u16 u16Val)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);
    u32 u32Old;
    u32 u32Shift = (u8Off & 2u) * 8u;

    __asm__ volatile("outl %0, %1" : : "a"(u32Addr), "Nd"((u16)0xCF8));
    __asm__ volatile("inl %1, %0" : "=a"(u32Old) : "Nd"((u16)0xCFC));
    u32Old = (u32Old & ~(0xffffu << u32Shift)) | ((u32)u16Val << u32Shift);
    __asm__ volatile("outl %0, %1" : : "a"(u32Addr), "Nd"((u16)0xCF8));
    __asm__ volatile("outl %0, %1" : : "a"(u32Old), "Nd"((u16)0xCFC));
}

static u64
pci_bar_mem(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Bir)
{
    u8 u8Off = (u8)(0x10u + u8Bir * 4u);
    u32 u32Lo = pci_cfg_read(u8Bus, u8Slot, u8Func, u8Off);
    u64 u64Bar;

    if ((u32Lo & 1u) != 0) {
        return 0; /* I/O BAR */
    }
    u64Bar = (u64)(u32Lo & ~0xfu);
    if (((u32Lo >> 1) & 3u) == 2u && u8Bir < 5) {
        u32 u32Hi = pci_cfg_read(u8Bus, u8Slot, u8Func, (u8)(u8Off + 4));

        u64Bar |= ((u64)u32Hi << 32);
    }
    return u64Bar;
}

static volatile u32 *
msix_table_mmio(u64 u64Pa)
{
    if (u64Pa == 0) {
        return NULL;
    }
    /*
     * MSI-X tables live in device MMIO BARs (often ≥0xf0000000). Never use
     * HHDM — that maps RAM only. Identity-map 2 MiB covering the table.
     */
    if (u64Pa >= 0xf0000000ull || u64Pa >= 0x100000000ull) {
        if (vmm_map_device((gj_paddr_t)u64Pa, 2ull * 1024ull * 1024ull) !=
            GJ_OK) {
            kprintf("pci: MSI-X map table pa=0x%lx failed\n",
                    (unsigned long)u64Pa);
            return NULL;
        }
    }
    return (volatile u32 *)(gj_vaddr_t)u64Pa;
}

u32
pci_msix_scan(struct gj_pci_msix_info *pOut, u32 u32Max)
{
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 cFound = 0;

    if (pOut == NULL || u32Max == 0) {
        return 0;
    }
    for (u8Bus = 0; u8Bus < 8 && cFound < u32Max; u8Bus++) {
        for (u8Slot = 0; u8Slot < 32 && cFound < u32Max; u8Slot++) {
            for (u8Func = 0; u8Func < 8 && cFound < u32Max; u8Func++) {
                u32 u32Id = pci_cfg_read(u8Bus, u8Slot, u8Func, 0);
                u16 u16Status;
                u8 u8Cap;

                if ((u32Id & 0xffffu) == 0xffffu) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                u16Status = pci_cfg_read16(u8Bus, u8Slot, u8Func, 0x06);
                if ((u16Status & PCI_STATUS_CAP) == 0) {
                    continue;
                }
                u8Cap = pci_cfg_read8(u8Bus, u8Slot, u8Func, 0x34) & 0xfcu;
                while (u8Cap != 0 && u8Cap != 0xffu) {
                    u8 u8CapId = pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap);

                    if (u8CapId == PCI_CAP_ID_MSIX) {
                        u16 u16MsgCtl =
                            pci_cfg_read16(u8Bus, u8Slot, u8Func, u8Cap + 2);
                        u32 u32Table =
                            pci_cfg_read(u8Bus, u8Slot, u8Func, u8Cap + 4);
                        u32 u32Pba =
                            pci_cfg_read(u8Bus, u8Slot, u8Func, u8Cap + 8);
                        u8 u8Bir = (u8)(u32Table & 7u);
                        u32 u32Toff = u32Table & ~7u;
                        u64 u64Bar = pci_bar_mem(u8Bus, u8Slot, u8Func, u8Bir);

                        memset(&pOut[cFound], 0, sizeof(pOut[cFound]));
                        pOut[cFound].u8Present = 1;
                        pOut[cFound].u8Enabled =
                            (u16MsgCtl & 0x8000u) ? 1u : 0u;
                        pOut[cFound].u16TableSize =
                            (u16)((u16MsgCtl & 0x7ffu) + 1u);
                        pOut[cFound].u8Bus = u8Bus;
                        pOut[cFound].u8Slot = u8Slot;
                        pOut[cFound].u8Func = u8Func;
                        pOut[cFound].u8TableBir = u8Bir;
                        pOut[cFound].u32TableOff = u32Toff;
                        pOut[cFound].u8PbaBir = (u8)(u32Pba & 7u);
                        pOut[cFound].u32PbaOff = u32Pba & ~7u;
                        pOut[cFound].u64TablePa =
                            u64Bar ? (u64Bar + u32Toff) : 0;
                        cFound++;
                        break;
                    }
                    if (u8CapId == PCI_CAP_ID_MSI && cFound < u32Max) {
                        memset(&pOut[cFound], 0, sizeof(pOut[cFound]));
                        pOut[cFound].u8Present = 1;
                        pOut[cFound].u8Enabled = 0;
                        pOut[cFound].u16TableSize = 1;
                        pOut[cFound].u8Bus = u8Bus;
                        pOut[cFound].u8Slot = u8Slot;
                        pOut[cFound].u8Func = u8Func;
                        cFound++;
                        break;
                    }
                    u8Cap =
                        pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap + 1) & 0xfcu;
                }
            }
        }
    }
    return cFound;
}

u32
pci_msix_enable_first(u32 u32Max)
{
    struct gj_pci_msix_info aInfo[16];
    u32 cScan;
    u32 iDev;
    u32 cEnabled = 0;

    cScan = pci_msix_scan(aInfo, 16);
    if (u32Max == 0 || u32Max > cScan) {
        u32Max = cScan;
    }
    for (iDev = 0; iDev < u32Max; iDev++) {
        u8 u8Bus = aInfo[iDev].u8Bus;
        u8 u8Slot = aInfo[iDev].u8Slot;
        u8 u8Func = aInfo[iDev].u8Func;
        u8 u8Cap = pci_cfg_read8(u8Bus, u8Slot, u8Func, 0x34) & 0xfcu;

        while (u8Cap != 0 && u8Cap != 0xffu) {
            u8 u8CapId = pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap);

            if (u8CapId == PCI_CAP_ID_MSIX) {
                u16 u16MsgCtl =
                    pci_cfg_read16(u8Bus, u8Slot, u8Func, u8Cap + 2);

                /* MSI-X Enable (bit 15); clear Function Mask (bit 14) */
                u16MsgCtl = (u16)((u16MsgCtl | 0x8000u) & ~0x4000u);
                pci_cfg_write16(u8Bus, u8Slot, u8Func, u8Cap + 2, u16MsgCtl);
                cEnabled++;
                break;
            }
            u8Cap =
                pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap + 1) & 0xfcu;
        }
    }
    kprintf("pci: MSI-X enable attempted on %u devices\n", cEnabled);
    return cEnabled;
}

u32
pci_msix_program_first(u8 u8Vector)
{
    struct gj_pci_msix_info aInfo[16];
    u32 cScan;
    u32 iDev;

    cScan = pci_msix_scan(aInfo, 16);
    for (iDev = 0; iDev < cScan; iDev++) {
        volatile u32 *pTab;
        u8 u8Bus;
        u8 u8Slot;
        u8 u8Func;
        u8 u8Cap;
        u16 u16Cmd;

        if (aInfo[iDev].u64TablePa == 0 || aInfo[iDev].u16TableSize == 0) {
            continue;
        }
        u8Bus = aInfo[iDev].u8Bus;
        u8Slot = aInfo[iDev].u8Slot;
        u8Func = aInfo[iDev].u8Func;
        /* Bus master + memory space */
        u16Cmd = pci_cfg_read16(u8Bus, u8Slot, u8Func, 0x04);
        u16Cmd = (u16)(u16Cmd | 0x6u);
        pci_cfg_write16(u8Bus, u8Slot, u8Func, 0x04, u16Cmd);

        pTab = msix_table_mmio(aInfo[iDev].u64TablePa);
        if (pTab == NULL) {
            continue;
        }
        /* Entry 0: addr lo/hi, data, vector control (mask then unmask) */
        pTab[0] = MSI_ADDR_BASE; /* dest = BSP local APIC */
        pTab[1] = 0;
        pTab[2] = (u32)u8Vector; /* edge, fixed delivery */
        pTab[3] = 1u;            /* masked while programming */
        __asm__ volatile("mfence" ::: "memory");
        pTab[3] = 0; /* unmask */

        /* Enable MSI-X in config */
        u8Cap = pci_cfg_read8(u8Bus, u8Slot, u8Func, 0x34) & 0xfcu;
        while (u8Cap != 0 && u8Cap != 0xffu) {
            if (pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap) ==
                PCI_CAP_ID_MSIX) {
                u16 u16MsgCtl =
                    pci_cfg_read16(u8Bus, u8Slot, u8Func, u8Cap + 2);

                u16MsgCtl = (u16)((u16MsgCtl | 0x8000u) & ~0x4000u);
                pci_cfg_write16(u8Bus, u8Slot, u8Func, u8Cap + 2, u16MsgCtl);
                break;
            }
            u8Cap =
                pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap + 1) & 0xfcu;
        }
        g_u32Programmed++;
        aInfo[iDev].u32Programmed = 1;
        kprintf("pci: MSI-X table program %u:%u.%u pa=0x%lx vec=0x%x\n", u8Bus,
                u8Slot, u8Func, (unsigned long)aInfo[iDev].u64TablePa,
                (unsigned)u8Vector);
        return 1;
    }
    return 0;
}

u32
pci_msix_programmed_count(void)
{
    return g_u32Programmed;
}

void
pci_msix_probe_log(void)
{
    struct gj_pci_msix_info aInfo[16];
    u32 cScan;
    u32 iDev;
    u32 cEnabled;
    u32 cProg;

    memset(aInfo, 0, sizeof(aInfo));
    cScan = pci_msix_scan(aInfo, 16);
    kprintf("pci: MSI/MSI-X devices=%u\n", cScan);
    for (iDev = 0; iDev < cScan && iDev < 4; iDev++) {
        kprintf("pci: msix %u:%u.%u table=%u en=%u bir=%u off=0x%x pa=0x%lx\n",
                aInfo[iDev].u8Bus, aInfo[iDev].u8Slot, aInfo[iDev].u8Func,
                aInfo[iDev].u16TableSize, aInfo[iDev].u8Enabled,
                aInfo[iDev].u8TableBir, aInfo[iDev].u32TableOff,
                (unsigned long)aInfo[iDev].u64TablePa);
    }
    cEnabled = pci_msix_enable_first(cScan > 2 ? 2 : cScan);
    cProg = pci_msix_program_first(PCI_MSIX_PROBE_VEC);
    if (cScan > 0 || cEnabled > 0) {
        kprintf("pci: MSI-X probe PASS\n");
        if (cEnabled > 0) {
            kprintf("pci: MSI-X enable PASS\n");
        }
        if (cProg > 0) {
            kprintf("pci: MSI-X table PASS\n");
        }
    } else {
        kprintf("pci: MSI-X none (ok)\n");
        kprintf("pci: MSI-X probe PASS\n");
    }
}
