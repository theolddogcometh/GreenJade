/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Global gj_boot_info handoff store. Multiboot and UEFI paths fill a
 * local copy then publish here so late boot (kmain, iommu, FB) can read
 * a single stable snapshot without keeping firmware pointers live.
 *
 * Soft product surface (UEFI handoff + Multiboot2 tag inventory):
 *   boot_info_soft_memmap  — EFI MD walk; usable reclaim-type counts
 *   boot_info_soft_gop_ok  — FB geometry sanity
 *   boot_info_soft_log     — greppable serial markers (post serial_init)
 *     includes soft Multiboot2 / tag inventory (P-BOOT-2 dev path)
 *
 * Grep markers:
 *   boot: handoff soft PASS|PARTIAL|STUB …
 *   boot: memmap soft PASS|SKIP|REJECT …
 *   boot: GOP soft PASS|SKIP|REJECT …
 *   boot: soft mb2 PASS|SKIP|REJECT|PARTIAL …
 *   boot: soft mb2 tag …
 *   boot: soft mb2 mmap …
 *   boot_info: soft mb2 inventory …
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
static u32 g_cSoftMb2InvCalls;

/*
 * Multiboot2 tag types (public Multiboot2 spec; local to soft walk).
 * P-BOOT-2 dev path only — product is UEFI (P-BOOT-1).
 */
#define BI_MB2_TAG_END             0u
#define BI_MB2_TAG_CMDLINE         1u
#define BI_MB2_TAG_BOOT_LOADER     2u
#define BI_MB2_TAG_MODULE          3u
#define BI_MB2_TAG_BASIC_MEMINFO   4u
#define BI_MB2_TAG_BOOTDEV         5u
#define BI_MB2_TAG_MMAP            6u
#define BI_MB2_TAG_VBE             7u
#define BI_MB2_TAG_FRAMEBUFFER     8u
#define BI_MB2_TAG_ELF_SECTIONS    9u
#define BI_MB2_TAG_APM            10u
#define BI_MB2_TAG_EFI32          11u
#define BI_MB2_TAG_EFI64          12u
#define BI_MB2_TAG_SMBIOS         13u
#define BI_MB2_TAG_ACPI_OLD       14u
#define BI_MB2_TAG_ACPI_NEW       15u
#define BI_MB2_TAG_NETWORK        16u
#define BI_MB2_TAG_EFI_MMAP       17u
#define BI_MB2_TAG_EFI_BS         18u
#define BI_MB2_TAG_EFI32_IH       19u
#define BI_MB2_TAG_EFI64_IH       20u
#define BI_MB2_TAG_LOAD_BASE      21u

#define BI_MB2_MMAP_AVAILABLE      1u

/* Defensive ceilings for soft tag walks (firmware / QEMU can lie). */
#define BI_MB2_TOTAL_MIN          16u
#define BI_MB2_TOTAL_MAX    (4u * 1024u * 1024u) /* 4 MiB */
#define BI_MB2_TAGS_MAX          256u
#define BI_MB2_TAG_HDR             8u
#define BI_MB2_MMAP_HDR           16u /* type+size+entry_size+entry_version */
#define BI_MB2_MMAP_ENTRY_MIN     24u

struct bi_mb2_tag {
    u32 u32Type;
    u32 u32Size;
};

struct bi_mb2_tag_mmap {
    u32 u32Type;
    u32 u32Size;
    u32 u32EntrySize;
    u32 u32EntryVersion;
};

struct bi_mb2_mmap_entry {
    u64 paAddr;
    u64 cbLen;
    u32 u32Type;
    u32 u32Zero;
};

/* Soft Multiboot2 tag inventory (observability only; never panics). */
struct bi_mb2_soft_inv {
    u32 fPresent;       /* 1 if u32Mb2InfoPhys non-zero */
    u32 fOk;            /* 1 if walk completed with END tag */
    u32 fPartial;       /* 1 if walked but no END / early stop */
    u32 fReject;        /* 1 if total_size bad or pointer unusable */
    u32 u32TotalSize;   /* Multiboot2 info total_size field */
    u32 cTags;          /* tags walked (excluding END if counted separately) */
    u32 cEnd;           /* END tags seen (0 or 1 expected) */
    u32 cCmdline;
    u32 cBootLoader;
    u32 cModule;
    u32 cBasicMem;
    u32 cBootdev;
    u32 cMmap;
    u32 cVbe;
    u32 cFb;
    u32 cElf;
    u32 cApm;
    u32 cEfi32;
    u32 cEfi64;
    u32 cSmbios;
    u32 cAcpiOld;
    u32 cAcpiNew;
    u32 cNetwork;
    u32 cEfiMmap;
    u32 cEfiBs;
    u32 cEfiIh;
    u32 cLoadBase;
    u32 cOther;         /* unknown / future tag types */
    u32 cBadSize;       /* tags with size < 8 or alignment issues */
    u32 cOverrun;       /* bound checks that forced stop */
    /* First mmap tag soft classification */
    u32 cMmapEntries;
    u32 cMmapAvail;
    u64 cbMmapAvail;
    u32 u32MmapEntrySize;
    u32 u32MmapEntryVersion;
    /* First interesting payload snippets */
    u64 u64FirstFbAddr; /* framebuffer tag addr if present (type 8) */
    u32 u32FirstFbW;
    u32 u32FirstFbH;
    u32 u32FirstFbBpp;
    u32 u32BasicMemLower; /* KiB below 1 MiB if basic_meminfo present */
    u32 u32BasicMemUpper; /* KiB above 1 MiB */
    u32 u32LoadBase;      /* load_base_addr if present */
};

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

/*
 * Soft Multiboot2 mmap entry walk (first mmap tag only for summary).
 * Counts available (type=1) regions; never hard-fails.
 */
static void
boot_mb2_soft_mmap_classify(const struct bi_mb2_tag_mmap *pMmap,
                            struct bi_mb2_soft_inv *pInv)
{
    u8 *pEntry;
    u8 *pEnd;
    u32 u32EntrySize;
    u32 cEnt;

    if (pMmap == NULL || pInv == NULL) {
        return;
    }
    if (pMmap->u32Size < BI_MB2_MMAP_HDR) {
        pInv->cBadSize++;
        return;
    }
    u32EntrySize = pMmap->u32EntrySize;
    if (u32EntrySize < BI_MB2_MMAP_ENTRY_MIN) {
        pInv->cBadSize++;
        return;
    }
    pInv->u32MmapEntrySize = u32EntrySize;
    pInv->u32MmapEntryVersion = pMmap->u32EntryVersion;
    pEntry = (u8 *)pMmap + BI_MB2_MMAP_HDR;
    pEnd = (u8 *)pMmap + pMmap->u32Size;
    cEnt = 0;
    while (pEntry + BI_MB2_MMAP_ENTRY_MIN <= pEnd && cEnt < 512u) {
        const struct bi_mb2_mmap_entry *pMe =
            (const struct bi_mb2_mmap_entry *)(const void *)pEntry;

        pInv->cMmapEntries++;
        cEnt++;
        if (pMe->u32Type == BI_MB2_MMAP_AVAILABLE && pMe->cbLen != 0) {
            pInv->cMmapAvail++;
            pInv->cbMmapAvail += pMe->cbLen;
        }
        pEntry += u32EntrySize;
    }
}

/*
 * Soft Multiboot2 tag inventory from u32Mb2InfoPhys.
 * Observability only; NULL / zero / bad size → structured SKIP/REJECT.
 * greppable: boot: soft mb2 | boot_info: soft mb2 inventory
 */
static void
boot_info_soft_mb2_inventory(const struct gj_boot_info *pInfo,
                             struct bi_mb2_soft_inv *pOut)
{
    u8 *pInfoBase;
    u8 *pEnd;
    struct bi_mb2_tag *pTag;
    u32 u32Total;
    u32 cWalk;
    u32 u32Pa;

    if (pOut == NULL) {
        return;
    }
    memset(pOut, 0, sizeof(*pOut));
    g_cSoftMb2InvCalls++;

    if (pInfo == NULL) {
        return;
    }
    u32Pa = pInfo->u32Mb2InfoPhys;
    if (u32Pa == 0) {
        /* UEFI product path or MB2 filler omitted phys — soft SKIP. */
        return;
    }
    pOut->fPresent = 1;

    /*
     * Identity map / low phys: Multiboot2 info is below 4 GiB on product
     * QEMU path. Map as identity VA (same as kmain / smp_init_from_mb2).
     */
    pInfoBase = (u8 *)(gj_vaddr_t)(u64)u32Pa;
    u32Total = *(const u32 *)(const void *)pInfoBase;
    pOut->u32TotalSize = u32Total;
    if (u32Total < BI_MB2_TOTAL_MIN || u32Total > BI_MB2_TOTAL_MAX) {
        pOut->fReject = 1;
        return;
    }

    pEnd = pInfoBase + u32Total;
    pTag = (struct bi_mb2_tag *)(void *)(pInfoBase + BI_MB2_TAG_HDR);
    cWalk = 0;

    while (cWalk < BI_MB2_TAGS_MAX) {
        u32 cbTag;
        u8 *pNext;
        u32 u32Type;
        u32 u32Size;

        if ((u8 *)pTag + BI_MB2_TAG_HDR > pEnd) {
            pOut->cOverrun++;
            pOut->fPartial = 1;
            break;
        }
        u32Type = pTag->u32Type;
        u32Size = pTag->u32Size;

        if (u32Type == BI_MB2_TAG_END) {
            pOut->cEnd++;
            pOut->cTags++;
            pOut->fOk = 1;
            break;
        }

        if (u32Size < BI_MB2_TAG_HDR) {
            pOut->cBadSize++;
            pOut->fPartial = 1;
            break;
        }
        if ((u8 *)pTag + u32Size > pEnd) {
            pOut->cOverrun++;
            pOut->fPartial = 1;
            break;
        }

        pOut->cTags++;
        switch (u32Type) {
        case BI_MB2_TAG_CMDLINE:
            pOut->cCmdline++;
            break;
        case BI_MB2_TAG_BOOT_LOADER:
            pOut->cBootLoader++;
            break;
        case BI_MB2_TAG_MODULE:
            pOut->cModule++;
            break;
        case BI_MB2_TAG_BASIC_MEMINFO:
            pOut->cBasicMem++;
            /* payload: mem_lower@8, mem_upper@12 (KiB) */
            if (u32Size >= 16u && pOut->u32BasicMemLower == 0 &&
                pOut->u32BasicMemUpper == 0) {
                const u32 *pMem = (const u32 *)(const void *)((u8 *)pTag + 8);

                pOut->u32BasicMemLower = pMem[0];
                pOut->u32BasicMemUpper = pMem[1];
            }
            break;
        case BI_MB2_TAG_BOOTDEV:
            pOut->cBootdev++;
            break;
        case BI_MB2_TAG_MMAP:
            pOut->cMmap++;
            if (pOut->cMmap == 1u) {
                boot_mb2_soft_mmap_classify(
                    (const struct bi_mb2_tag_mmap *)(const void *)pTag, pOut);
            }
            break;
        case BI_MB2_TAG_VBE:
            pOut->cVbe++;
            break;
        case BI_MB2_TAG_FRAMEBUFFER:
            pOut->cFb++;
            /*
             * Multiboot2 framebuffer tag (public layout):
             *   addr@8 u64, pitch@16 u32, width@20 u32, height@24 u32,
             *   bpp@28 u8, type@29 u8, reserved@30 u8
             */
            if (u32Size >= 32u && pOut->u64FirstFbAddr == 0) {
                const u8 *pFb = (const u8 *)(const void *)pTag;

                pOut->u64FirstFbAddr =
                    *(const u64 *)(const void *)(pFb + 8);
                pOut->u32FirstFbW =
                    *(const u32 *)(const void *)(pFb + 20);
                pOut->u32FirstFbH =
                    *(const u32 *)(const void *)(pFb + 24);
                pOut->u32FirstFbBpp = (u32)pFb[28];
            }
            break;
        case BI_MB2_TAG_ELF_SECTIONS:
            pOut->cElf++;
            break;
        case BI_MB2_TAG_APM:
            pOut->cApm++;
            break;
        case BI_MB2_TAG_EFI32:
            pOut->cEfi32++;
            break;
        case BI_MB2_TAG_EFI64:
            pOut->cEfi64++;
            break;
        case BI_MB2_TAG_SMBIOS:
            pOut->cSmbios++;
            break;
        case BI_MB2_TAG_ACPI_OLD:
            pOut->cAcpiOld++;
            break;
        case BI_MB2_TAG_ACPI_NEW:
            pOut->cAcpiNew++;
            break;
        case BI_MB2_TAG_NETWORK:
            pOut->cNetwork++;
            break;
        case BI_MB2_TAG_EFI_MMAP:
            pOut->cEfiMmap++;
            break;
        case BI_MB2_TAG_EFI_BS:
            pOut->cEfiBs++;
            break;
        case BI_MB2_TAG_EFI32_IH:
        case BI_MB2_TAG_EFI64_IH:
            pOut->cEfiIh++;
            break;
        case BI_MB2_TAG_LOAD_BASE:
            pOut->cLoadBase++;
            if (u32Size >= 12u && pOut->u32LoadBase == 0) {
                pOut->u32LoadBase =
                    *(const u32 *)(const void *)((u8 *)pTag + 8);
            }
            break;
        default:
            pOut->cOther++;
            break;
        }

        cbTag = (u32Size + 7u) & ~7u;
        if (cbTag < BI_MB2_TAG_HDR) {
            pOut->cBadSize++;
            pOut->fPartial = 1;
            break;
        }
        pNext = (u8 *)pTag + cbTag;
        if (pNext <= (u8 *)pTag || pNext > pEnd) {
            pOut->cOverrun++;
            pOut->fPartial = 1;
            break;
        }
        pTag = (struct bi_mb2_tag *)(void *)pNext;
        cWalk++;
    }

    if (cWalk >= BI_MB2_TAGS_MAX && pOut->fOk == 0) {
        pOut->fPartial = 1;
    }
}

/*
 * Emit greppable Multiboot2 soft inventory lines.
 * Prefixes: "boot: soft …" and "boot_info: soft …"
 * Never hard-fails; SKIP on UEFI / absent MB2 info.
 */
static void
boot_info_soft_mb2_log(const struct gj_boot_info *pInfo)
{
    struct bi_mb2_soft_inv softInv;
    u32 u32Source;
    u32 u32Flags;
    u32 u32Pa;
    u32 cAcpi;
    const char *szVerdict;

    u32Source = (pInfo != NULL) ? pInfo->u32Source : GJ_BOOT_SRC_UNKNOWN;
    u32Flags = (pInfo != NULL) ? pInfo->u32Flags : 0;
    u32Pa = (pInfo != NULL) ? pInfo->u32Mb2InfoPhys : 0;

    boot_info_soft_mb2_inventory(pInfo, &softInv);

    if (!softInv.fPresent) {
        /* Grep: boot: soft mb2 SKIP */
        kprintf("boot: soft mb2 SKIP pa=0 source=%u flags_mb2=%d "
                "inv_calls=%u\n",
                (unsigned)u32Source,
                (u32Flags & GJ_BOOT_F_MB2_INFO) != 0,
                (unsigned)g_cSoftMb2InvCalls);
        kprintf("boot_info: soft mb2 inventory SKIP present=0\n");
        return;
    }

    if (softInv.fReject) {
        /* Grep: boot: soft mb2 REJECT */
        kprintf("boot: soft mb2 REJECT pa=0x%x total_size=%u "
                "source=%u (bad total_size floor=%u ceil=%u)\n",
                (unsigned)u32Pa, (unsigned)softInv.u32TotalSize,
                (unsigned)u32Source, (unsigned)BI_MB2_TOTAL_MIN,
                (unsigned)BI_MB2_TOTAL_MAX);
        kprintf("boot_info: soft mb2 inventory REJECT pa=0x%x total=%u\n",
                (unsigned)u32Pa, (unsigned)softInv.u32TotalSize);
        return;
    }

    cAcpi = softInv.cAcpiOld + softInv.cAcpiNew;
    /*
     * PASS    = walked to END and saw at least one mmap tag
     * PARTIAL = END without mmap, or early stop (bounds / tag cap)
     */
    if (softInv.fOk && softInv.cMmap > 0) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /* Grep: boot: soft mb2 PASS|PARTIAL */
    kprintf("boot: soft mb2 %s pa=0x%x total=%u tags=%u end=%u "
            "mmap=%u fb=%u acpi=%u/%u modules=%u cmdline=%u elf=%u "
            "other=%u bad=%u overrun=%u source=%u\n",
            szVerdict, (unsigned)u32Pa, (unsigned)softInv.u32TotalSize,
            (unsigned)softInv.cTags, (unsigned)softInv.cEnd,
            (unsigned)softInv.cMmap, (unsigned)softInv.cFb,
            (unsigned)softInv.cAcpiOld, (unsigned)softInv.cAcpiNew,
            (unsigned)softInv.cModule, (unsigned)softInv.cCmdline,
            (unsigned)softInv.cElf, (unsigned)softInv.cOther,
            (unsigned)softInv.cBadSize, (unsigned)softInv.cOverrun,
            (unsigned)u32Source);

    /* Per-class tag inventory (compact; greppable boot: soft mb2 tag). */
    kprintf("boot: soft mb2 tag counts meminfo=%u bootdev=%u vbe=%u "
            "efi32=%u efi64=%u efi_mmap=%u efi_bs=%u efi_ih=%u "
            "smbios=%u apm=%u network=%u load_base=%u boot_loader=%u\n",
            (unsigned)softInv.cBasicMem, (unsigned)softInv.cBootdev,
            (unsigned)softInv.cVbe, (unsigned)softInv.cEfi32,
            (unsigned)softInv.cEfi64, (unsigned)softInv.cEfiMmap,
            (unsigned)softInv.cEfiBs, (unsigned)softInv.cEfiIh,
            (unsigned)softInv.cSmbios, (unsigned)softInv.cApm,
            (unsigned)softInv.cNetwork, (unsigned)softInv.cLoadBase,
            (unsigned)softInv.cBootLoader);

    if (softInv.cMmap > 0) {
        /* Grep: boot: soft mb2 mmap */
        kprintf("boot: soft mb2 mmap tags=%u entries=%u avail=%u "
                "avail_mib=%lu entry_size=%u entry_ver=%u\n",
                (unsigned)softInv.cMmap, (unsigned)softInv.cMmapEntries,
                (unsigned)softInv.cMmapAvail,
                (unsigned long)(softInv.cbMmapAvail / (1024ull * 1024ull)),
                (unsigned)softInv.u32MmapEntrySize,
                (unsigned)softInv.u32MmapEntryVersion);
    } else {
        kprintf("boot: soft mb2 mmap SKIP tags=0\n");
    }

    if (softInv.cFb > 0) {
        kprintf("boot: soft mb2 fb tags=%u addr=0x%lx %ux%u bpp=%u\n",
                (unsigned)softInv.cFb,
                (unsigned long)softInv.u64FirstFbAddr,
                (unsigned)softInv.u32FirstFbW,
                (unsigned)softInv.u32FirstFbH,
                (unsigned)softInv.u32FirstFbBpp);
    } else {
        kprintf("boot: soft mb2 fb SKIP tags=0\n");
    }

    if (cAcpi > 0) {
        kprintf("boot: soft mb2 acpi old=%u new=%u (rsdp payload in tag)\n",
                (unsigned)softInv.cAcpiOld, (unsigned)softInv.cAcpiNew);
    } else {
        kprintf("boot: soft mb2 acpi SKIP tags=0\n");
    }

    if (softInv.cBasicMem > 0) {
        kprintf("boot: soft mb2 basic_mem lower_kib=%u upper_kib=%u\n",
                (unsigned)softInv.u32BasicMemLower,
                (unsigned)softInv.u32BasicMemUpper);
    }

    if (softInv.cLoadBase > 0) {
        kprintf("boot: soft mb2 load_base=0x%x\n",
                (unsigned)softInv.u32LoadBase);
    }

    /*
     * Aggregate inventory line for soft product scripts.
     * Grep: boot_info: soft mb2 inventory
     */
    kprintf("boot_info: soft mb2 inventory %s pa=0x%x total=%u tags=%u "
            "end=%u mmap=%u avail_mib=%lu fb=%u acpi=%u modules=%u "
            "flags_mb2=%d inv_calls=%u\n",
            szVerdict, (unsigned)u32Pa, (unsigned)softInv.u32TotalSize,
            (unsigned)softInv.cTags, (unsigned)softInv.cEnd,
            (unsigned)softInv.cMmap,
            (unsigned long)(softInv.cbMmapAvail / (1024ull * 1024ull)),
            (unsigned)softInv.cFb, (unsigned)cAcpi,
            (unsigned)softInv.cModule,
            (u32Flags & GJ_BOOT_F_MB2_INFO) != 0,
            (unsigned)g_cSoftMb2InvCalls);
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
        /* Grep: boot: soft mb2 SKIP (null handoff) */
        kprintf("boot: soft mb2 SKIP pa=0 reason=null_info log=%u\n",
                (unsigned)g_cSoftLogCalls);
        kprintf("boot_info: soft mb2 inventory SKIP present=0\n");
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
               (u32Flags & GJ_BOOT_F_MB2_INFO) != 0 ||
               pInfo->u64MemMap != 0 || pInfo->u32Mb2InfoPhys != 0) {
        szHandoff = "PASS";
    } else {
        szHandoff = "PARTIAL";
    }

    kprintf("boot: handoff soft %s source=%u flags=0x%x magic_ok=%d "
            "kernel=0x%lx+0x%lx rsdp=0x%lx mb2=0x%x set=%u\n",
            szHandoff, (unsigned)u32Source, (unsigned)u32Flags, fValid,
            (unsigned long)pInfo->u64KernelPhys,
            (unsigned long)pInfo->u64KernelBytes,
            (unsigned long)pInfo->u64Rsdp,
            (unsigned)pInfo->u32Mb2InfoPhys,
            (unsigned)g_cSoftSetGlobal);

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

    /* Soft Multiboot2 / tag inventory (P-BOOT-2; SKIP on UEFI product). */
    boot_info_soft_mb2_log(pInfo);
}
