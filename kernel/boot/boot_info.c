/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Global gj_boot_info handoff store. Multiboot and UEFI paths fill a
 * local copy then publish here so late boot (kmain, iommu, FB) can read
 * a single stable snapshot without keeping firmware pointers live.
 *
 * Soft product surface (UEFI handoff deepen):
 *   boot_info_soft_memmap  — EFI MD walk; usable reclaim-type counts
 *   boot_info_soft_gop_ok  — FB geometry sanity
 *   boot_info_soft_log     — greppable serial markers (post serial_init)
 *
 * Grep markers:
 *   boot: handoff soft PASS|PARTIAL|STUB …
 *   boot: memmap soft PASS|SKIP|REJECT …
 *   boot: GOP soft PASS|SKIP|REJECT …
 *
 * Pure C11 freestanding; dual MIT OR Apache-2.0.
 */
#include <gj/boot_info.h>
#include <gj/klog.h>
#include <gj/string.h>

static struct gj_boot_info g_BootInfo;

/* Soft publish counters (observability; never gate product). */
static u32 g_cSoftSetGlobal;
static u32 g_cSoftUefiPublish;
static u32 g_cSoftLogCalls;

/* OR capability bits from populated fields (idempotent with caller flags). */
static u32
boot_info_derive_flags(const struct gj_boot_info *pInfo)
{
    u32 u32Flags = 0;

    if (pInfo == NULL) {
        return 0;
    }
    if (pInfo->u64MemMap != 0 && pInfo->u64MemMapBytes != 0) {
        u32Flags |= GJ_BOOT_F_MEMMAP;
    }
    if (pInfo->u64Rsdp != 0) {
        u32Flags |= GJ_BOOT_F_RSDP;
    }
    if (pInfo->u64FbBase != 0) {
        u32Flags |= GJ_BOOT_F_FB;
    }
    if (pInfo->u64KernelPhys != 0 && pInfo->u64KernelBytes != 0) {
        u32Flags |= GJ_BOOT_F_KERNEL_IMG;
    }
    if (pInfo->u32Mb2InfoPhys != 0) {
        u32Flags |= GJ_BOOT_F_MB2_INFO;
    }
    return u32Flags;
}

/* Soft: reclaimable after ExitBootServices (match kmain_uefi). */
static int
boot_efi_type_usable(u32 u32Type)
{
    if (u32Type == GJ_BOOT_EFI_MT_CONVENTIONAL ||
        u32Type == GJ_BOOT_EFI_MT_LOADER_CODE ||
        u32Type == GJ_BOOT_EFI_MT_LOADER_DATA ||
        u32Type == GJ_BOOT_EFI_MT_BS_CODE ||
        u32Type == GJ_BOOT_EFI_MT_BS_DATA ||
        u32Type == GJ_BOOT_EFI_MT_ACPI_RECLAIM) {
        return 1;
    }
    return 0;
}

/* Zero and stamp magic/version. No-op if pInfo is NULL (caller may probe). */
void
boot_info_clear(struct gj_boot_info *pInfo)
{
    if (pInfo == NULL) {
        return;
    }
    memset(pInfo, 0, sizeof(*pInfo));
    pInfo->u32Magic = GJ_BOOT_INFO_MAGIC;
    pInfo->u32Version = GJ_BOOT_INFO_VERSION;
}

/* Copy caller buffer into the process-global handoff. Ignores NULL. */
void
boot_info_set_global(const struct gj_boot_info *pInfo)
{
    if (pInfo == NULL) {
        return;
    }
    g_BootInfo = *pInfo;
    /* Ensure magic/version even if filler only set payload fields. */
    if (g_BootInfo.u32Magic == 0) {
        g_BootInfo.u32Magic = GJ_BOOT_INFO_MAGIC;
    }
    if (g_BootInfo.u32Version == 0) {
        g_BootInfo.u32Version = GJ_BOOT_INFO_VERSION;
    }
    g_BootInfo.u32Flags |= boot_info_derive_flags(&g_BootInfo);
    g_cSoftSetGlobal++;
    if (g_BootInfo.u32Source == GJ_BOOT_SRC_UEFI) {
        g_cSoftUefiPublish++;
    }
}

/* Always returns the static global (never NULL). */
const struct gj_boot_info *
boot_info_get(void)
{
    return &g_BootInfo;
}

int
boot_info_valid(const struct gj_boot_info *pInfo)
{
    if (pInfo == NULL) {
        return 0;
    }
    if (pInfo->u32Magic != GJ_BOOT_INFO_MAGIC) {
        return 0;
    }
    if (pInfo->u32Version != GJ_BOOT_INFO_VERSION) {
        return 0;
    }
    return 1;
}

void
boot_info_soft_memmap(const struct gj_boot_info *pInfo,
                      struct gj_boot_soft_memmap *pOut)
{
    u64 u64DescSize;
    u64 u64Off;

    if (pOut == NULL) {
        return;
    }
    memset(pOut, 0, sizeof(*pOut));
    if (pInfo == NULL) {
        return;
    }
    /* Soft memmap walk is EFI-shaped (desc stride). Multiboot → present=0. */
    if (pInfo->u64MemMap == 0 || pInfo->u64MemMapBytes == 0) {
        return;
    }
    if (pInfo->u32Source == GJ_BOOT_SRC_MULTIBOOT2 &&
        pInfo->u64MemDescSize == 0) {
        /* MB2 mmap uses tag-local entry size; soft EFI walker does not apply. */
        pOut->fPresent = 0;
        return;
    }
    pOut->fPresent = 1;
    u64DescSize = pInfo->u64MemDescSize;
    if (u64DescSize < sizeof(struct gj_boot_efi_md)) {
        u64DescSize = sizeof(struct gj_boot_efi_md);
    }
    pOut->u64DescSize = u64DescSize;

    for (u64Off = 0;
         u64Off + u64DescSize <= pInfo->u64MemMapBytes;
         u64Off += u64DescSize) {
        const struct gj_boot_efi_md *pMd =
            (const struct gj_boot_efi_md *)(gj_vaddr_t)(pInfo->u64MemMap +
                                                        u64Off);
        u32 u32T;
        u64 u64Pages;

        pOut->cDescs++;
        u32T = pMd->u32Type;
        u64Pages = pMd->u64Pages;
        if (u64Pages == 0) {
            continue;
        }
        if (!boot_efi_type_usable(u32T)) {
            continue;
        }
        pOut->cUsableDescs++;
        pOut->cUsablePages += u64Pages;
        if (u32T == GJ_BOOT_EFI_MT_CONVENTIONAL) {
            pOut->cConvPages += u64Pages;
        }
    }
    pOut->cbUsable = pOut->cUsablePages * 4096ull;
    if (pOut->cUsablePages > 0) {
        pOut->fOk = 1;
    }
}

int
boot_info_soft_gop_ok(const struct gj_boot_info *pInfo)
{
    u32 u32BytesPerPx;
    u64 u64MinPitch;

    if (pInfo == NULL) {
        return 0;
    }
    if (pInfo->u64FbBase == 0) {
        return 0;
    }
    if (pInfo->u32FbWidth == 0 || pInfo->u32FbHeight == 0) {
        return 0;
    }
    if (pInfo->u32FbBpp != 15u && pInfo->u32FbBpp != 16u &&
        pInfo->u32FbBpp != 24u && pInfo->u32FbBpp != 32u) {
        return 0;
    }
    /* Soft bpp → bytes/pixel (15 treated as 2). */
    if (pInfo->u32FbBpp <= 16u) {
        u32BytesPerPx = 2u;
    } else if (pInfo->u32FbBpp <= 24u) {
        u32BytesPerPx = 3u;
    } else {
        u32BytesPerPx = 4u;
    }
    u64MinPitch = (u64)pInfo->u32FbWidth * (u64)u32BytesPerPx;
    if ((u64)pInfo->u32FbPitch < u64MinPitch) {
        return 0;
    }
    return 1;
}

void
boot_info_soft_log(const struct gj_boot_info *pInfo)
{
    struct gj_boot_soft_memmap softMap;
    int fValid;
    int fGop;
    u32 u32Flags;
    u32 u32Source;
    const char *szHandoff;

    g_cSoftLogCalls++;
    if (pInfo == NULL) {
        kprintf("boot: handoff soft STUB pInfo=NULL set=%u uefi=%u log=%u\n",
                (unsigned)g_cSoftSetGlobal, (unsigned)g_cSoftUefiPublish,
                (unsigned)g_cSoftLogCalls);
        kprintf("boot: memmap soft SKIP present=0\n");
        kprintf("boot: GOP soft SKIP base=0\n");
        return;
    }

    fValid = boot_info_valid(pInfo);
    u32Flags = pInfo->u32Flags;
    u32Source = pInfo->u32Source;

    /*
     * PASS  = valid magic/version and (memmap soft-ok or at least one cap bit)
     * PARTIAL = valid but empty map / no caps
     * STUB  = magic/version mismatch (still publishable for partial halt)
     */
    if (!fValid) {
        szHandoff = "STUB";
    } else if ((u32Flags & GJ_BOOT_F_MEMMAP) != 0 ||
               (u32Flags & GJ_BOOT_F_KERNEL_IMG) != 0 ||
               pInfo->u64MemMap != 0) {
        szHandoff = "PASS";
    } else {
        szHandoff = "PARTIAL";
    }

    kprintf("boot: handoff soft %s source=%u flags=0x%x magic_ok=%d "
            "kernel=0x%lx+0x%lx rsdp=0x%lx set=%u\n",
            szHandoff, (unsigned)u32Source, (unsigned)u32Flags, fValid,
            (unsigned long)pInfo->u64KernelPhys,
            (unsigned long)pInfo->u64KernelBytes,
            (unsigned long)pInfo->u64Rsdp, (unsigned)g_cSoftSetGlobal);

    boot_info_soft_memmap(pInfo, &softMap);
    if (!softMap.fPresent) {
        kprintf("boot: memmap soft SKIP present=0 bytes=%lu desc_size=%lu "
                "flags_memmap=%d\n",
                (unsigned long)pInfo->u64MemMapBytes,
                (unsigned long)pInfo->u64MemDescSize,
                (u32Flags & GJ_BOOT_F_MEMMAP) != 0);
    } else if (!softMap.fOk) {
        kprintf("boot: memmap soft REJECT present=1 descs=%lu usable=0 "
                "desc_size=%lu map=0x%lx bytes=%lu\n",
                (unsigned long)softMap.cDescs,
                (unsigned long)softMap.u64DescSize,
                (unsigned long)pInfo->u64MemMap,
                (unsigned long)pInfo->u64MemMapBytes);
    } else {
        kprintf("boot: memmap soft PASS descs=%lu usable_descs=%lu "
                "usable_pages=%lu usable_mib=%lu conv_pages=%lu "
                "desc_size=%lu map=0x%lx\n",
                (unsigned long)softMap.cDescs,
                (unsigned long)softMap.cUsableDescs,
                (unsigned long)softMap.cUsablePages,
                (unsigned long)(softMap.cbUsable / (1024ull * 1024ull)),
                (unsigned long)softMap.cConvPages,
                (unsigned long)softMap.u64DescSize,
                (unsigned long)pInfo->u64MemMap);
    }

    fGop = boot_info_soft_gop_ok(pInfo);
    if (pInfo->u64FbBase == 0) {
        kprintf("boot: GOP soft SKIP base=0 flags_fb=%d\n",
                (u32Flags & GJ_BOOT_F_FB) != 0);
    } else if (!fGop) {
        kprintf("boot: GOP soft REJECT base=0x%lx %ux%u pitch=%u bpp=%u\n",
                (unsigned long)pInfo->u64FbBase, pInfo->u32FbWidth,
                pInfo->u32FbHeight, pInfo->u32FbPitch, pInfo->u32FbBpp);
    } else {
        kprintf("boot: GOP soft PASS base=0x%lx %ux%u pitch=%u bpp=%u\n",
                (unsigned long)pInfo->u64FbBase, pInfo->u32FbWidth,
                pInfo->u32FbHeight, pInfo->u32FbPitch, pInfo->u32FbBpp);
    }
}
