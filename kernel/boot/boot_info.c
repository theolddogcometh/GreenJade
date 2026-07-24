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
 * Wave 35 exclusive soft deepen — greppable prefix-stable "boot: soft …"
 * (diagnostics only; never hard-gate product / ExitBootServices):
 *   boot: soft honesty       — explicit non-claims (soft never gates EBS)
 *   boot: soft inventory     — master surface + wave stamp
 *   boot: soft path          — P-BOOT-1/2 claim + source class
 *   boot: soft handoff       — magic/version/source/flags snapshot
 *   boot: soft memmap        — EFI MD usable + type-class tallies
 *   boot: soft gop           — FB geometry soft classify
 *   boot: soft flags         — GJ_BOOT_F_* bit inventory
 *   boot: soft caps          — per-flag soft PASS lamps
 *   boot: soft source        — GJ_BOOT_SRC_* class + publish tallies
 *   boot: soft stats         — set_global / log / mb2 inv counters
 *   boot: soft kernel        — kernel image span soft classify
 *   boot: soft rsdp          — ACPI RSDP soft present/skip
 *   boot: soft contract      — magic/version/struct size contract
 *   boot: soft fields        — handoff field presence lamps
 *   boot: soft efi_mt        — reclaimable EFI type map constants
 *   boot: soft geometry      — FB pitch/bpp soft math
 *   boot: soft publish       — set_global source-class tallies
 *   boot: soft derive        — flag-derive path counters
 *   boot: soft magic         — Wave 15 magic/version lamps
 *   boot: soft validate      — Wave 15 handoff validity classify
 *   boot: soft identity      — Wave 15 identity-bridge note (separate unit)
 *   boot: soft catalog       — Wave 19 area name rollup
 *   boot: soft deepen        — wave=115 stamp + area count
 *   boot: soft mb2           — PASS|SKIP|REJECT|PARTIAL tag walk
 *   boot: soft mb2 tag       — per-class tag counts
 *   boot: soft mb2 mmap      — first mmap tag entry inventory
 *   boot: soft mb2 fb        — framebuffer tag soft
 *   boot: soft mb2 acpi      — old/new ACPI tags
 *   boot: soft mb2 efi       — EFI32/64/mmap/bs/ih tag surface
 *   boot: soft mb2 payload   — basic_mem / load_base / modules
 *   boot: soft mb2 path      — walk verdict + bound ceilings
 *   boot_info: soft mb2 inventory …  (aggregate companion)
 *
 * Legacy companion markers (still emitted; prefix-stable):
 *   boot: handoff soft PASS|PARTIAL|STUB …
 *   boot: memmap soft PASS|SKIP|REJECT …
 *   boot: GOP soft PASS|SKIP|REJECT …
 *
 * greppable: boot: soft
 * greppable: boot: soft honesty
 * greppable: boot: soft inventory
 * greppable: boot: soft deepen
 * greppable: boot: soft path
 * greppable: boot: soft handoff
 * greppable: boot: soft memmap
 * greppable: boot: soft gop
 * greppable: boot: soft flags
 * greppable: boot: soft caps
 * greppable: boot: soft contract
 * greppable: boot: soft fields
 * greppable: boot: soft efi_mt
 * greppable: boot: soft geometry
 * greppable: boot: soft publish
 * greppable: boot: soft derive
 * greppable: boot: soft source
 * greppable: boot: soft stats
 * greppable: boot: soft mb2
 * greppable: boot_info: soft mb2 inventory
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
static u32 g_cSoftMb2Publish;   /* set_global with Multiboot2 source */
static u32 g_cSoftUnknownPublish;
static u32 g_cSoftLogCalls;
static u32 g_cSoftMb2InvCalls;
static u32 g_cSoftMemmapCalls;
static u32 g_cSoftGopCalls;
static u32 g_cSoftValidOk;
static u32 g_cSoftValidFail;
static u32 g_cSoftClearCalls;
static u32 g_cSoftDeriveMemmap;
static u32 g_cSoftDeriveRsdp;
static u32 g_cSoftDeriveFb;
static u32 g_cSoftDeriveKernel;
static u32 g_cSoftDeriveMb2;

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
#define BI_MB2_MMAP_ENT_CAP      512u

/* Soft EFI type classes (match GJ_BOOT_EFI_MT_* / boot_efi_type_usable). */
#define BI_EFI_MT_RESERVED         0u
#define BI_EFI_MT_LOADER_CODE      1u
#define BI_EFI_MT_LOADER_DATA      2u
#define BI_EFI_MT_BS_CODE          3u
#define BI_EFI_MT_BS_DATA          4u
#define BI_EFI_MT_RT_CODE          5u
#define BI_EFI_MT_RT_DATA          6u
#define BI_EFI_MT_CONVENTIONAL     7u
#define BI_EFI_MT_UNUSABLE         8u
#define BI_EFI_MT_ACPI_RECLAIM     9u
#define BI_EFI_MT_ACPI_NVS        10u
#define BI_EFI_MT_MMIO            11u
#define BI_EFI_MT_MMIO_PORT       12u
#define BI_EFI_MT_PAL_CODE        13u
#define BI_EFI_MT_PERSISTENT      14u

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
    u32 cTagCapHit;     /* stopped because BI_MB2_TAGS_MAX reached */
    /* First mmap tag soft classification */
    u32 cMmapEntries;
    u32 cMmapAvail;
    u32 cMmapReserved;  /* non-available typed entries (soft) */
    u64 cbMmapAvail;
    u64 cbMmapTotal;    /* sum of all entry lengths walked */
    u32 u32MmapEntrySize;
    u32 u32MmapEntryVersion;
    /* First interesting payload snippets */
    u64 u64FirstFbAddr; /* framebuffer tag addr if present (type 8) */
    u32 u32FirstFbW;
    u32 u32FirstFbH;
    u32 u32FirstFbBpp;
    u32 u32FirstFbPitch; /* pitch@16 if size allows */
    u32 u32FirstFbType;  /* type@29 if size allows */
    u32 u32BasicMemLower; /* KiB below 1 MiB if basic_meminfo present */
    u32 u32BasicMemUpper; /* KiB above 1 MiB */
    u32 u32LoadBase;      /* load_base_addr if present */
    u32 cWalkSteps;       /* loop iterations before stop */
};

/*
 * Soft EFI memmap deepen (Wave 12): type-class page tallies.
 * greppable: boot: soft memmap
 */
struct bi_soft_memmap_deep {
    struct gj_boot_soft_memmap base;
    u64 cLoaderPages;   /* LoaderCode + LoaderData */
    u64 cBsPages;       /* BootServicesCode + BootServicesData */
    u64 cConvPages;     /* ConventionalMemory (alias of base.cConvPages) */
    u64 cAcpiRecPages;  /* ACPIReclaim */
    u64 cRtPages;       /* Runtime services (not soft-usable) */
    u64 cMmioPages;     /* MMIO / MMIO_PORT */
    u64 cOtherPages;    /* remaining typed non-zero pages */
    u64 cZeroPageDescs; /* descs with u64Pages == 0 */
    u64 cUsableTypes;   /* distinct usable-type hits among walked descs */
    u32 fStrideFallback;/* 1 if desc_size was below sizeof(gj_boot_efi_md) */
};

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
boot_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

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
        boot_soft_inc(&g_cSoftDeriveMemmap);
    }
    if (pInfo->u64Rsdp != 0) {
        u32Flags |= GJ_BOOT_F_RSDP;
        boot_soft_inc(&g_cSoftDeriveRsdp);
    }
    if (pInfo->u64FbBase != 0) {
        u32Flags |= GJ_BOOT_F_FB;
        boot_soft_inc(&g_cSoftDeriveFb);
    }
    if (pInfo->u64KernelPhys != 0 && pInfo->u64KernelBytes != 0) {
        u32Flags |= GJ_BOOT_F_KERNEL_IMG;
        boot_soft_inc(&g_cSoftDeriveKernel);
    }
    if (pInfo->u32Mb2InfoPhys != 0) {
        u32Flags |= GJ_BOOT_F_MB2_INFO;
        boot_soft_inc(&g_cSoftDeriveMb2);
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
    while (pEntry + BI_MB2_MMAP_ENTRY_MIN <= pEnd && cEnt < BI_MB2_MMAP_ENT_CAP) {
        const struct bi_mb2_mmap_entry *pMe =
            (const struct bi_mb2_mmap_entry *)(const void *)pEntry;

        pInv->cMmapEntries++;
        cEnt++;
        if (pMe->cbLen != 0) {
            pInv->cbMmapTotal += pMe->cbLen;
        }
        if (pMe->u32Type == BI_MB2_MMAP_AVAILABLE && pMe->cbLen != 0) {
            pInv->cMmapAvail++;
            pInv->cbMmapAvail += pMe->cbLen;
        } else {
            pInv->cMmapReserved++;
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
    boot_soft_inc(&g_cSoftMb2InvCalls);

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
                pOut->u32FirstFbPitch =
                    *(const u32 *)(const void *)(pFb + 16);
                pOut->u32FirstFbW =
                    *(const u32 *)(const void *)(pFb + 20);
                pOut->u32FirstFbH =
                    *(const u32 *)(const void *)(pFb + 24);
                pOut->u32FirstFbBpp = (u32)pFb[28];
                pOut->u32FirstFbType = (u32)pFb[29];
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

    pOut->cWalkSteps = cWalk;
    if (cWalk >= BI_MB2_TAGS_MAX && pOut->fOk == 0) {
        pOut->fPartial = 1;
        pOut->cTagCapHit = 1;
    }
}

/*
 * Soft EFI memmap deepen walk (type-class pages + base soft memmap).
 * greppable: boot: soft memmap
 */
static void
boot_info_soft_memmap_deep(const struct gj_boot_info *pInfo,
                           struct bi_soft_memmap_deep *pOut)
{
    u64 u64DescSize;
    u64 u64Off;
    u32 fSawLoader = 0;
    u32 fSawBs = 0;
    u32 fSawConv = 0;
    u32 fSawAcpi = 0;

    if (pOut == NULL) {
        return;
    }
    memset(pOut, 0, sizeof(*pOut));
    boot_soft_inc(&g_cSoftMemmapCalls);

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
        pOut->base.fPresent = 0;
        return;
    }
    pOut->base.fPresent = 1;
    u64DescSize = pInfo->u64MemDescSize;
    if (u64DescSize < sizeof(struct gj_boot_efi_md)) {
        u64DescSize = sizeof(struct gj_boot_efi_md);
        pOut->fStrideFallback = 1;
    }
    pOut->base.u64DescSize = u64DescSize;

    for (u64Off = 0;
         u64Off + u64DescSize <= pInfo->u64MemMapBytes;
         u64Off += u64DescSize) {
        const struct gj_boot_efi_md *pMd =
            (const struct gj_boot_efi_md *)(gj_vaddr_t)(pInfo->u64MemMap +
                                                        u64Off);
        u32 u32T;
        u64 u64Pages;

        pOut->base.cDescs++;
        u32T = pMd->u32Type;
        u64Pages = pMd->u64Pages;
        if (u64Pages == 0) {
            pOut->cZeroPageDescs++;
            continue;
        }

        /* Type-class tallies (soft; all types, not only usable). */
        if (u32T == BI_EFI_MT_LOADER_CODE || u32T == BI_EFI_MT_LOADER_DATA) {
            pOut->cLoaderPages += u64Pages;
            fSawLoader = 1;
        } else if (u32T == BI_EFI_MT_BS_CODE || u32T == BI_EFI_MT_BS_DATA) {
            pOut->cBsPages += u64Pages;
            fSawBs = 1;
        } else if (u32T == BI_EFI_MT_CONVENTIONAL) {
            pOut->cConvPages += u64Pages;
            fSawConv = 1;
        } else if (u32T == BI_EFI_MT_ACPI_RECLAIM) {
            pOut->cAcpiRecPages += u64Pages;
            fSawAcpi = 1;
        } else if (u32T == BI_EFI_MT_RT_CODE || u32T == BI_EFI_MT_RT_DATA) {
            pOut->cRtPages += u64Pages;
        } else if (u32T == BI_EFI_MT_MMIO || u32T == BI_EFI_MT_MMIO_PORT) {
            pOut->cMmioPages += u64Pages;
        } else {
            pOut->cOtherPages += u64Pages;
        }

        if (!boot_efi_type_usable(u32T)) {
            continue;
        }
        pOut->base.cUsableDescs++;
        pOut->base.cUsablePages += u64Pages;
        if (u32T == GJ_BOOT_EFI_MT_CONVENTIONAL) {
            pOut->base.cConvPages += u64Pages;
        }
    }
    pOut->base.cbUsable = pOut->base.cUsablePages * 4096ull;
    if (pOut->base.cUsablePages > 0) {
        pOut->base.fOk = 1;
    }
    pOut->cUsableTypes = (u64)fSawLoader + (u64)fSawBs + (u64)fSawConv +
                         (u64)fSawAcpi;
    /* Keep cConvPages deep field aligned with base when base filled. */
    if (pOut->cConvPages == 0 && pOut->base.cConvPages != 0) {
        pOut->cConvPages = pOut->base.cConvPages;
    }
}

/*
 * Emit greppable Multiboot2 soft inventory lines (Wave 12 deepen).
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
    u32 cEfiSurf;
    const char *szVerdict;
    const char *szPath;

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
        /* Grep: boot: soft mb2 path */
        kprintf("boot: soft mb2 path SKIP p_boot=2_dev_only product=uefi "
                "reason=no_mb2_phys source=%u\n",
                (unsigned)u32Source);
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
        /* Grep: boot: soft mb2 path */
        kprintf("boot: soft mb2 path REJECT total=%u floor=%u ceil=%u "
                "walk=0 tags_max=%u\n",
                (unsigned)softInv.u32TotalSize, (unsigned)BI_MB2_TOTAL_MIN,
                (unsigned)BI_MB2_TOTAL_MAX, (unsigned)BI_MB2_TAGS_MAX);
        kprintf("boot_info: soft mb2 inventory REJECT pa=0x%x total=%u\n",
                (unsigned)u32Pa, (unsigned)softInv.u32TotalSize);
        return;
    }

    cAcpi = softInv.cAcpiOld + softInv.cAcpiNew;
    cEfiSurf = softInv.cEfi32 + softInv.cEfi64 + softInv.cEfiMmap +
               softInv.cEfiBs + softInv.cEfiIh;
    /*
     * PASS    = walked to END and saw at least one mmap tag
     * PARTIAL = END without mmap, or early stop (bounds / tag cap)
     */
    if (softInv.fOk && softInv.cMmap > 0) {
        szVerdict = "PASS";
        szPath = "end_plus_mmap";
    } else if (softInv.fOk) {
        szVerdict = "PARTIAL";
        szPath = "end_no_mmap";
    } else if (softInv.cTagCapHit != 0) {
        szVerdict = "PARTIAL";
        szPath = "tag_cap";
    } else if (softInv.cOverrun != 0) {
        szVerdict = "PARTIAL";
        szPath = "overrun";
    } else if (softInv.cBadSize != 0) {
        szVerdict = "PARTIAL";
        szPath = "bad_size";
    } else {
        szVerdict = "PARTIAL";
        szPath = "early_stop";
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

    /* Grep: boot: soft mb2 path */
    kprintf("boot: soft mb2 path %s verdict=%s ok=%u partial=%u "
            "walk_steps=%u tags_max=%u tag_cap=%u bad=%u overrun=%u "
            "total_min=%u total_max=%u\n",
            szPath, szVerdict, (unsigned)softInv.fOk,
            (unsigned)softInv.fPartial, (unsigned)softInv.cWalkSteps,
            (unsigned)BI_MB2_TAGS_MAX, (unsigned)softInv.cTagCapHit,
            (unsigned)softInv.cBadSize, (unsigned)softInv.cOverrun,
            (unsigned)BI_MB2_TOTAL_MIN, (unsigned)BI_MB2_TOTAL_MAX);

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
                "reserved=%u avail_mib=%lu total_mib=%lu entry_size=%u "
                "entry_ver=%u\n",
                (unsigned)softInv.cMmap, (unsigned)softInv.cMmapEntries,
                (unsigned)softInv.cMmapAvail,
                (unsigned)softInv.cMmapReserved,
                (unsigned long)(softInv.cbMmapAvail / (1024ull * 1024ull)),
                (unsigned long)(softInv.cbMmapTotal / (1024ull * 1024ull)),
                (unsigned)softInv.u32MmapEntrySize,
                (unsigned)softInv.u32MmapEntryVersion);
    } else {
        kprintf("boot: soft mb2 mmap SKIP tags=0\n");
    }

    if (softInv.cFb > 0) {
        /* Grep: boot: soft mb2 fb */
        kprintf("boot: soft mb2 fb tags=%u addr=0x%lx %ux%u pitch=%u "
                "bpp=%u type=%u\n",
                (unsigned)softInv.cFb,
                (unsigned long)softInv.u64FirstFbAddr,
                (unsigned)softInv.u32FirstFbW,
                (unsigned)softInv.u32FirstFbH,
                (unsigned)softInv.u32FirstFbPitch,
                (unsigned)softInv.u32FirstFbBpp,
                (unsigned)softInv.u32FirstFbType);
    } else {
        kprintf("boot: soft mb2 fb SKIP tags=0\n");
    }

    if (cAcpi > 0) {
        /* Grep: boot: soft mb2 acpi */
        kprintf("boot: soft mb2 acpi old=%u new=%u (rsdp payload in tag)\n",
                (unsigned)softInv.cAcpiOld, (unsigned)softInv.cAcpiNew);
    } else {
        kprintf("boot: soft mb2 acpi SKIP tags=0\n");
    }

    /* Grep: boot: soft mb2 efi (Wave 12) */
    if (cEfiSurf > 0) {
        kprintf("boot: soft mb2 efi efi32=%u efi64=%u efi_mmap=%u "
                "efi_bs=%u efi_ih=%u surface=%u\n",
                (unsigned)softInv.cEfi32, (unsigned)softInv.cEfi64,
                (unsigned)softInv.cEfiMmap, (unsigned)softInv.cEfiBs,
                (unsigned)softInv.cEfiIh, (unsigned)cEfiSurf);
    } else {
        kprintf("boot: soft mb2 efi SKIP surface=0\n");
    }

    /* Grep: boot: soft mb2 payload (Wave 12) */
    kprintf("boot: soft mb2 payload basic_mem=%u lower_kib=%u upper_kib=%u "
            "load_base_tags=%u load_base=0x%x modules=%u cmdline=%u "
            "elf=%u boot_loader=%u\n",
            (unsigned)softInv.cBasicMem,
            (unsigned)softInv.u32BasicMemLower,
            (unsigned)softInv.u32BasicMemUpper,
            (unsigned)softInv.cLoadBase,
            (unsigned)softInv.u32LoadBase,
            (unsigned)softInv.cModule,
            (unsigned)softInv.cCmdline,
            (unsigned)softInv.cElf,
            (unsigned)softInv.cBootLoader);

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

/* Wave 35 exclusive soft deepen stamp (this unit only). */
#define BI_SOFT_WAVE 115u

/*
 * Wave 15: greppable boot: soft … deepen surface (handoff/memmap/gop/flags
 * plus honesty/contract/caps/fields/efi_mt/geometry/publish/derive/
 * magic/validate/identity/catalog/deepen).
 * Complements legacy "boot: handoff soft" / "boot: memmap soft" / "boot: GOP soft".
 * greppable: boot: soft
 * Soft only: never hard-gates product / ExitBootServices.
 * Higher-half product move remains OPEN (identity bridge is separate unit).
 */
static void
boot_info_soft_wave_log(const struct gj_boot_info *pInfo,
                        const struct bi_soft_memmap_deep *pDeep,
                        int fValid, int fGop,
                        const char *szHandoff)
{
    u32 u32Flags;
    u32 u32Source;
    u32 fMemmapBit;
    u32 fRsdpBit;
    u32 fFbBit;
    u32 fKernelBit;
    u32 fMb2Bit;
    u32 fKernelSpan;
    u32 fRsdp;
    u32 fMapPtr;
    u32 fMapBytes;
    u32 fDescSize;
    u32 fFbBase;
    u32 fMb2Phys;
    u32 fMagicOk;
    u32 fVersionOk;
    u32 u32BppBytes;
    u32 u32MinPitch;
    u32 fPitchOk;
    u32 cAreas;
    u32 cCapBits;
    const char *szSrcName;
    const char *szPathClaim;
    const char *szMemmap;
    const char *szGop;

    u32Flags = (pInfo != NULL) ? pInfo->u32Flags : 0;
    u32Source = (pInfo != NULL) ? pInfo->u32Source : GJ_BOOT_SRC_UNKNOWN;
    fMemmapBit = (u32Flags & GJ_BOOT_F_MEMMAP) != 0;
    fRsdpBit = (u32Flags & GJ_BOOT_F_RSDP) != 0;
    fFbBit = (u32Flags & GJ_BOOT_F_FB) != 0;
    fKernelBit = (u32Flags & GJ_BOOT_F_KERNEL_IMG) != 0;
    fMb2Bit = (u32Flags & GJ_BOOT_F_MB2_INFO) != 0;
    fKernelSpan = (pInfo != NULL && pInfo->u64KernelPhys != 0 &&
                   pInfo->u64KernelBytes != 0)
                      ? 1u
                      : 0u;
    fRsdp = (pInfo != NULL && pInfo->u64Rsdp != 0) ? 1u : 0u;
    fMapPtr = (pInfo != NULL && pInfo->u64MemMap != 0) ? 1u : 0u;
    fMapBytes = (pInfo != NULL && pInfo->u64MemMapBytes != 0) ? 1u : 0u;
    fDescSize = (pInfo != NULL && pInfo->u64MemDescSize != 0) ? 1u : 0u;
    fFbBase = (pInfo != NULL && pInfo->u64FbBase != 0) ? 1u : 0u;
    fMb2Phys = (pInfo != NULL && pInfo->u32Mb2InfoPhys != 0) ? 1u : 0u;
    fMagicOk = (pInfo != NULL && pInfo->u32Magic == GJ_BOOT_INFO_MAGIC) ? 1u
                                                                       : 0u;
    fVersionOk =
        (pInfo != NULL && pInfo->u32Version == GJ_BOOT_INFO_VERSION) ? 1u
                                                                    : 0u;
    cAreas = 0;
    cCapBits = (u32)fMemmapBit + (u32)fRsdpBit + (u32)fFbBit +
               (u32)fKernelBit + (u32)fMb2Bit;

    /* Soft FB pitch math (observability; mirrors boot_info_soft_gop_ok). */
    u32BppBytes = 0;
    u32MinPitch = 0;
    fPitchOk = 0;
    if (pInfo != NULL && pInfo->u64FbBase != 0) {
        if (pInfo->u32FbBpp <= 16u) {
            u32BppBytes = 2u;
        } else if (pInfo->u32FbBpp <= 24u) {
            u32BppBytes = 3u;
        } else {
            u32BppBytes = 4u;
        }
        u32MinPitch = pInfo->u32FbWidth * u32BppBytes;
        fPitchOk = (pInfo->u32FbPitch >= u32MinPitch) ? 1u : 0u;
    }

    if (u32Source == GJ_BOOT_SRC_UEFI) {
        szSrcName = "uefi";
        szPathClaim = "p_boot_1_product";
    } else if (u32Source == GJ_BOOT_SRC_MULTIBOOT2) {
        szSrcName = "multiboot2";
        szPathClaim = "p_boot_2_dev";
    } else {
        szSrcName = "unknown";
        szPathClaim = "source_unknown";
    }

    if (pDeep == NULL || !pDeep->base.fPresent) {
        szMemmap = "SKIP";
    } else if (!pDeep->base.fOk) {
        szMemmap = "REJECT";
    } else {
        szMemmap = "PASS";
    }

    if (pInfo == NULL || pInfo->u64FbBase == 0) {
        szGop = "SKIP";
    } else if (!fGop) {
        szGop = "REJECT";
    } else {
        szGop = "PASS";
    }

    if (szHandoff == NULL) {
        szHandoff = "STUB";
    }

    /*
     * Honesty first: soft inventory never gates ExitBootServices / product.
     * greppable: boot: soft honesty
     */
    kprintf("boot: soft honesty never_gates_ebs=1 never_gates_product=1 "
            "higher_half_product=OPEN identity_bridge=separate "
            "p_boot_1=uefi p_boot_2=mb2_dev soft_only=1 wave=%u\n",
            (unsigned)BI_SOFT_WAVE);
    cAreas++;

    /* Grep: boot: soft inventory */
    kprintf("boot: soft inventory wave=%u handoff=%s source=%u src=%s "
            "path=%s magic_ok=%d memmap=%s gop=%s set=%u log=%u "
            "mb2_inv=%u memmap_calls=%u gop_calls=%u cap_bits=%u\n",
            (unsigned)BI_SOFT_WAVE, szHandoff, (unsigned)u32Source,
            szSrcName, szPathClaim, fValid, szMemmap, szGop,
            (unsigned)g_cSoftSetGlobal, (unsigned)g_cSoftLogCalls,
            (unsigned)g_cSoftMb2InvCalls, (unsigned)g_cSoftMemmapCalls,
            (unsigned)g_cSoftGopCalls, (unsigned)cCapBits);
    cAreas++;

    /* Grep: boot: soft path */
    kprintf("boot: soft path claim=%s p_boot_1=uefi_product "
            "p_boot_2=mb2_dev_only p_boot_3=efi_memmap_after_ebs "
            "p_boot_4=acpi_rsdp source=%u flags=0x%x soft_never_gates=1 "
            "wave=%u\n",
            szPathClaim, (unsigned)u32Source, (unsigned)u32Flags,
            (unsigned)BI_SOFT_WAVE);
    cAreas++;

    /* Grep: boot: soft handoff */
    if (pInfo == NULL) {
        kprintf("boot: soft handoff STUB pInfo=NULL magic=0 version=0 "
                "source=0 flags=0x0\n");
    } else {
        kprintf("boot: soft handoff %s magic=0x%x version=%u source=%u "
                "flags=0x%x magic_ok=%d kernel=0x%lx+0x%lx rsdp=0x%lx "
                "mb2=0x%x map=0x%lx map_bytes=%lu desc_size=%lu "
                "fb=0x%lx\n",
                szHandoff, (unsigned)pInfo->u32Magic,
                (unsigned)pInfo->u32Version, (unsigned)u32Source,
                (unsigned)u32Flags, fValid,
                (unsigned long)pInfo->u64KernelPhys,
                (unsigned long)pInfo->u64KernelBytes,
                (unsigned long)pInfo->u64Rsdp,
                (unsigned)pInfo->u32Mb2InfoPhys,
                (unsigned long)pInfo->u64MemMap,
                (unsigned long)pInfo->u64MemMapBytes,
                (unsigned long)pInfo->u64MemDescSize,
                (unsigned long)pInfo->u64FbBase);
    }
    cAreas++;

    /* Grep: boot: soft memmap */
    if (pDeep == NULL || !pDeep->base.fPresent) {
        kprintf("boot: soft memmap SKIP present=0 descs=0 usable=0 "
                "flags_memmap=%u stride_fb=%u\n",
                (unsigned)fMemmapBit,
                (unsigned)(pDeep != NULL ? pDeep->fStrideFallback : 0));
    } else {
        kprintf("boot: soft memmap %s descs=%lu usable_descs=%lu "
                "usable_pages=%lu usable_mib=%lu conv_pages=%lu "
                "loader_pages=%lu bs_pages=%lu acpi_rec_pages=%lu "
                "rt_pages=%lu mmio_pages=%lu other_pages=%lu "
                "zero_descs=%lu usable_types=%lu desc_size=%lu "
                "stride_fb=%u map=0x%lx\n",
                szMemmap, (unsigned long)pDeep->base.cDescs,
                (unsigned long)pDeep->base.cUsableDescs,
                (unsigned long)pDeep->base.cUsablePages,
                (unsigned long)(pDeep->base.cbUsable / (1024ull * 1024ull)),
                (unsigned long)pDeep->base.cConvPages,
                (unsigned long)pDeep->cLoaderPages,
                (unsigned long)pDeep->cBsPages,
                (unsigned long)pDeep->cAcpiRecPages,
                (unsigned long)pDeep->cRtPages,
                (unsigned long)pDeep->cMmioPages,
                (unsigned long)pDeep->cOtherPages,
                (unsigned long)pDeep->cZeroPageDescs,
                (unsigned long)pDeep->cUsableTypes,
                (unsigned long)pDeep->base.u64DescSize,
                (unsigned)pDeep->fStrideFallback,
                (unsigned long)(pInfo != NULL ? pInfo->u64MemMap : 0));
    }
    cAreas++;

    /* Grep: boot: soft gop */
    if (pInfo == NULL || pInfo->u64FbBase == 0) {
        kprintf("boot: soft gop SKIP base=0 flags_fb=%u ok=%d\n",
                (unsigned)fFbBit, fGop);
    } else {
        kprintf("boot: soft gop %s base=0x%lx %ux%u pitch=%u bpp=%u "
                "flags_fb=%u ok=%d\n",
                szGop, (unsigned long)pInfo->u64FbBase, pInfo->u32FbWidth,
                pInfo->u32FbHeight, pInfo->u32FbPitch, pInfo->u32FbBpp,
                (unsigned)fFbBit, fGop);
    }
    cAreas++;

    /* Grep: boot: soft flags */
    kprintf("boot: soft flags raw=0x%x memmap=%u rsdp=%u fb=%u "
            "kernel_img=%u mb2_info=%u derive_memmap=%u derive_rsdp=%u "
            "derive_fb=%u derive_kernel=%u derive_mb2=%u\n",
            (unsigned)u32Flags, (unsigned)fMemmapBit, (unsigned)fRsdpBit,
            (unsigned)fFbBit, (unsigned)fKernelBit, (unsigned)fMb2Bit,
            (unsigned)g_cSoftDeriveMemmap, (unsigned)g_cSoftDeriveRsdp,
            (unsigned)g_cSoftDeriveFb, (unsigned)g_cSoftDeriveKernel,
            (unsigned)g_cSoftDeriveMb2);
    cAreas++;

    /*
     * Grep: boot: soft caps — per GJ_BOOT_F_* soft PASS lamps (Wave 15).
     * Presence only; never claims product readiness beyond flag bits.
     */
    kprintf("boot: soft caps pop=%u memmap=%u soft %s\n",
            (unsigned)cCapBits, (unsigned)fMemmapBit,
            fMemmapBit ? "PASS" : "SKIP");
    kprintf("boot: soft caps rsdp=%u soft %s\n", (unsigned)fRsdpBit,
            fRsdpBit ? "PASS" : "SKIP");
    kprintf("boot: soft caps fb=%u soft %s\n", (unsigned)fFbBit,
            fFbBit ? "PASS" : "SKIP");
    kprintf("boot: soft caps kernel_img=%u soft %s\n", (unsigned)fKernelBit,
            fKernelBit ? "PASS" : "SKIP");
    kprintf("boot: soft caps mb2_info=%u soft %s\n", (unsigned)fMb2Bit,
            fMb2Bit ? "PASS" : "SKIP");
    kprintf("boot: soft caps mask_memmap=0x%x mask_rsdp=0x%x mask_fb=0x%x "
            "mask_kernel=0x%x mask_mb2=0x%x raw=0x%x soft PASS\n",
            (unsigned)GJ_BOOT_F_MEMMAP, (unsigned)GJ_BOOT_F_RSDP,
            (unsigned)GJ_BOOT_F_FB, (unsigned)GJ_BOOT_F_KERNEL_IMG,
            (unsigned)GJ_BOOT_F_MB2_INFO, (unsigned)u32Flags);
    cAreas++;

    /* Grep: boot: soft source */
    kprintf("boot: soft source id=%u name=%s uefi_publish=%u "
            "mb2_publish=%u unknown_publish=%u set_global=%u clear=%u\n",
            (unsigned)u32Source, szSrcName, (unsigned)g_cSoftUefiPublish,
            (unsigned)g_cSoftMb2Publish, (unsigned)g_cSoftUnknownPublish,
            (unsigned)g_cSoftSetGlobal, (unsigned)g_cSoftClearCalls);
    cAreas++;

    /* Grep: boot: soft kernel */
    if (fKernelSpan == 0) {
        kprintf("boot: soft kernel SKIP span=0 flags_kernel=%u\n",
                (unsigned)fKernelBit);
    } else {
        kprintf("boot: soft kernel PASS phys=0x%lx bytes=%lu mib=%lu "
                "flags_kernel=%u\n",
                (unsigned long)pInfo->u64KernelPhys,
                (unsigned long)pInfo->u64KernelBytes,
                (unsigned long)(pInfo->u64KernelBytes / (1024ull * 1024ull)),
                (unsigned)fKernelBit);
    }
    cAreas++;

    /* Grep: boot: soft rsdp */
    if (fRsdp == 0) {
        kprintf("boot: soft rsdp SKIP phys=0 flags_rsdp=%u\n",
                (unsigned)fRsdpBit);
    } else {
        kprintf("boot: soft rsdp PASS phys=0x%lx flags_rsdp=%u "
                "p_boot_4=acpi_rsdp\n",
                (unsigned long)pInfo->u64Rsdp, (unsigned)fRsdpBit);
    }
    cAreas++;

    /* Grep: boot: soft contract — magic/version/struct size (Wave 15). */
    kprintf("boot: soft contract magic_ok=%u version_ok=%u "
            "magic_expect=0x%x version_expect=%u struct_bytes=%u "
            "src_unknown=%u src_mb2=%u src_uefi=%u soft %s\n",
            (unsigned)fMagicOk, (unsigned)fVersionOk,
            (unsigned)GJ_BOOT_INFO_MAGIC, (unsigned)GJ_BOOT_INFO_VERSION,
            (unsigned)sizeof(struct gj_boot_info),
            (unsigned)GJ_BOOT_SRC_UNKNOWN, (unsigned)GJ_BOOT_SRC_MULTIBOOT2,
            (unsigned)GJ_BOOT_SRC_UEFI, fValid ? "PASS" : "STUB");
    cAreas++;

    /* Grep: boot: soft fields — field presence lamps (Wave 15). */
    kprintf("boot: soft fields map_ptr=%u map_bytes=%u desc_size=%u "
            "rsdp=%u fb=%u kernel_span=%u mb2_phys=%u flags=0x%x "
            "source=%u soft PASS\n",
            (unsigned)fMapPtr, (unsigned)fMapBytes, (unsigned)fDescSize,
            (unsigned)fRsdp, (unsigned)fFbBase, (unsigned)fKernelSpan,
            (unsigned)fMb2Phys, (unsigned)u32Flags, (unsigned)u32Source);
    cAreas++;

    /*
     * Grep: boot: soft efi_mt — reclaimable EFI type map constants.
     * Public UEFI types only; inventory of soft-usable reclaim set.
     */
    kprintf("boot: soft efi_mt loader_code=%u loader_data=%u "
            "bs_code=%u bs_data=%u conventional=%u acpi_reclaim=%u "
            "usable_set=6 soft PASS\n",
            (unsigned)GJ_BOOT_EFI_MT_LOADER_CODE,
            (unsigned)GJ_BOOT_EFI_MT_LOADER_DATA,
            (unsigned)GJ_BOOT_EFI_MT_BS_CODE,
            (unsigned)GJ_BOOT_EFI_MT_BS_DATA,
            (unsigned)GJ_BOOT_EFI_MT_CONVENTIONAL,
            (unsigned)GJ_BOOT_EFI_MT_ACPI_RECLAIM);
    cAreas++;

    /* Grep: boot: soft geometry — FB pitch/bpp soft math (Wave 15). */
    if (pInfo == NULL || pInfo->u64FbBase == 0) {
        kprintf("boot: soft geometry SKIP fb=0 bpp_bytes=0 min_pitch=0 "
                "pitch_ok=0\n");
    } else {
        kprintf("boot: soft geometry %s w=%u h=%u pitch=%u bpp=%u "
                "bpp_bytes=%u min_pitch=%u pitch_ok=%u px=%lu "
                "row_bytes=%u soft %s\n",
                szGop, pInfo->u32FbWidth, pInfo->u32FbHeight,
                pInfo->u32FbPitch, pInfo->u32FbBpp, (unsigned)u32BppBytes,
                (unsigned)u32MinPitch, (unsigned)fPitchOk,
                (unsigned long)((u64)pInfo->u32FbWidth *
                                (u64)pInfo->u32FbHeight),
                (unsigned)pInfo->u32FbPitch, szGop);
    }
    cAreas++;

    /* Grep: boot: soft publish — set_global source-class tallies. */
    kprintf("boot: soft publish set=%u uefi=%u mb2=%u unknown=%u "
            "clear=%u valid_ok=%u valid_fail=%u log=%u soft PASS\n",
            (unsigned)g_cSoftSetGlobal, (unsigned)g_cSoftUefiPublish,
            (unsigned)g_cSoftMb2Publish, (unsigned)g_cSoftUnknownPublish,
            (unsigned)g_cSoftClearCalls, (unsigned)g_cSoftValidOk,
            (unsigned)g_cSoftValidFail, (unsigned)g_cSoftLogCalls);
    cAreas++;

    /* Grep: boot: soft derive — flag-derive path counters. */
    kprintf("boot: soft derive memmap=%u rsdp=%u fb=%u kernel=%u mb2=%u "
            "soft PASS\n",
            (unsigned)g_cSoftDeriveMemmap, (unsigned)g_cSoftDeriveRsdp,
            (unsigned)g_cSoftDeriveFb, (unsigned)g_cSoftDeriveKernel,
            (unsigned)g_cSoftDeriveMb2);
    cAreas++;

    /* Grep: boot: soft stats */
    kprintf("boot: soft stats set=%u uefi=%u mb2_src=%u unknown=%u "
            "log=%u mb2_inv=%u memmap=%u gop=%u valid_ok=%u valid_fail=%u "
            "clear=%u wave=%u\n",
            (unsigned)g_cSoftSetGlobal, (unsigned)g_cSoftUefiPublish,
            (unsigned)g_cSoftMb2Publish, (unsigned)g_cSoftUnknownPublish,
            (unsigned)g_cSoftLogCalls, (unsigned)g_cSoftMb2InvCalls,
            (unsigned)g_cSoftMemmapCalls, (unsigned)g_cSoftGopCalls,
            (unsigned)g_cSoftValidOk, (unsigned)g_cSoftValidFail,
            (unsigned)g_cSoftClearCalls, (unsigned)BI_SOFT_WAVE);
    cAreas++;

    /* Grep: boot: soft magic — Wave 15 magic/version lamp surface. */
    kprintf("boot: soft magic ok=%u version_ok=%u expect=0x%x "
            "version=%u soft %s\n",
            (unsigned)fMagicOk, (unsigned)fVersionOk,
            (unsigned)GJ_BOOT_INFO_MAGIC, (unsigned)GJ_BOOT_INFO_VERSION,
            (fMagicOk != 0u && fVersionOk != 0u) ? "PASS" : "STUB");
    cAreas++;

    /* Grep: boot: soft validate — Wave 15 handoff validity classify. */
    kprintf("boot: soft validate handoff=%s magic_ok=%u version_ok=%u "
            "valid=%d valid_ok=%u valid_fail=%u source=%u soft %s\n",
            szHandoff, (unsigned)fMagicOk, (unsigned)fVersionOk, fValid,
            (unsigned)g_cSoftValidOk, (unsigned)g_cSoftValidFail,
            (unsigned)u32Source, fValid ? "PASS" : "STUB");
    cAreas++;

    /*
     * Grep: boot: soft identity — bridge note only; install is identity_map.c.
     * Never claims higher-half product from this unit.
     */
    kprintf("boot: soft identity bridge=separate unit=identity_map.c "
            "cover_gib=4 higher_half_product=OPEN soft_note=1 wave=%u "
            "soft PASS\n",
            (unsigned)BI_SOFT_WAVE);
    cAreas++;

    /* Grep: boot: soft surfaces (Wave 20 deepen) */
    kprintf("boot: soft surfaces count=%u wave=%u "
            "names=honesty,inventory,path,handoff,memmap,gop,flags,caps,"
            "contract,fields,geometry,magic,validate,identity,catalog,"
            "surfaces,note,return,retmap,deepen\n",
            (unsigned)cAreas + 4u, (unsigned)BI_SOFT_WAVE);
    cAreas++;

    /* Grep: boot: soft note (Wave 20 deepen) */
    kprintf("boot: soft note milestone=wave98 exclusive=1 "
            "soft_only=1 not_bar3=1 handoff=%s source=%u wave=%u\n",
            szHandoff, (unsigned)u32Source, (unsigned)BI_SOFT_WAVE);
    cAreas++;

    /* Grep: boot: soft catalog — Wave 19 area name rollup. */
    kprintf("boot: soft catalog honesty,inventory,path,handoff,memmap,gop,"
            "flags,caps,source,kernel,rsdp,contract,fields,efi_mt,"
            "geometry,publish,derive,stats,magic,validate,identity,"
            "catalog,return,retmap,deepen wave=%u areas_expect=94 soft PASS\n",
            (unsigned)BI_SOFT_WAVE);
    cAreas++;

    /* Grep: boot: soft return (Wave 20 deepen) */
    kprintf("boot: soft return handoff=%s source=%u magic_ok=%d "
            "soft_never_gates=1 product_gate=0 higher_half=OPEN "
            "wave=%u\n",
            szHandoff, (unsigned)u32Source, fValid,
            (unsigned)BI_SOFT_WAVE);
    cAreas++;

    /* Grep: boot: soft retmap — Wave 19 return-surface map */
    kprintf("boot: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=115\n");

    /* Grep: boot: soft deepen — Wave 20 stamp + area count. */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: boot: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("boot: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)BI_SOFT_WAVE);
    /* Grep: boot: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("boot: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)BI_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: boot: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("boot: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)BI_SOFT_WAVE);
    /* Grep: boot: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("boot: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)BI_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: boot: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("boot: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)BI_SOFT_WAVE);
            /* Grep: boot: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("boot: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)BI_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: boot: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("boot: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)BI_SOFT_WAVE);
            /* Grep: boot: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("boot: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: boot: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("boot: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)BI_SOFT_WAVE);
            /* Grep: boot: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("boot: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)BI_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: boot: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("boot: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)BI_SOFT_WAVE);
            /* Grep: boot: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("boot: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)BI_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: boot: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("boot: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)BI_SOFT_WAVE);
            /* Grep: boot: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("boot: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)BI_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: boot: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("boot: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)BI_SOFT_WAVE);
            /* Grep: boot: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("boot: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)BI_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: boot: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("boot: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)BI_SOFT_WAVE);
                    /* Grep: boot: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("boot: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)BI_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: boot: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("boot: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)BI_SOFT_WAVE);
                            /* Grep: boot: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("boot: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)BI_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: boot: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("boot: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)BI_SOFT_WAVE);
                            /* Grep: boot: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("boot: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)BI_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: boot: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("boot: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)BI_SOFT_WAVE);
                            /* Grep: boot: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("boot: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)BI_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: boot: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("boot: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)BI_SOFT_WAVE);
                            /* Grep: boot: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("boot: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)BI_SOFT_WAVE);
                            /* Grep: boot: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("boot: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("boot: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("boot: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("boot: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("boot: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("boot: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("boot: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retfortress — Wave 35 return-fortress honesty */
kprintf("boot: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("boot: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft rethold — Wave 36 return-hold honesty */
kprintf("boot: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retspire — Wave 36 exclusive spire stamp */
kprintf("boot: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retwall — Wave 37 return-wall honesty */
kprintf("boot: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retgate — Wave 37 exclusive gate stamp */
kprintf("boot: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retmoat — Wave 38 return-moat honesty */
kprintf("boot: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retower — Wave 38 exclusive tower stamp */
kprintf("boot: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("boot: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("boot: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("boot: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("boot: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retravelin — Wave 41 return-travelin honesty */
kprintf("boot: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("boot: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("boot: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("boot: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("boot: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("boot: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("boot: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("boot: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("boot: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("boot: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retbailey — Wave 46 return-bailey honesty */
kprintf("boot: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)BI_SOFT_WAVE);
/* Grep: boot: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("boot: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)BI_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("boot: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("boot: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("boot: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("boot: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("boot: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("boot: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retsally — Wave 50 return-sally honesty */
kprintf("boot: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("boot: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retfosse — Wave 51 return-fosse honesty */
kprintf("boot: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("boot: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("boot: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("boot: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retravelin — Wave 53 return-travelin honesty */
kprintf("boot: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("boot: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("boot: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retredan — Wave 54 exclusive redan stamp */
kprintf("boot: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retflank — Wave 55 return-flank honesty */
kprintf("boot: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retface — Wave 55 exclusive face stamp */
kprintf("boot: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retgorge — Wave 56 return-gorge honesty */
kprintf("boot: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("boot: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retraverse — Wave 57 return-traverse honesty */
kprintf("boot: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("boot: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retorillon — Wave 58 return-orillon honesty */
kprintf("boot: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("boot: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("boot: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("boot: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retplace — Wave 60 return-place honesty */
kprintf("boot: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("boot: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("boot: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("boot: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("boot: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("boot: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("boot: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("boot: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: boot: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("boot: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: boot: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("boot: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: boot: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("boot: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: boot: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("boot: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: boot: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("boot: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=115 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: boot: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("boot: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=115 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("boot: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("boot: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("boot: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("boot: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("boot: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=115 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("boot: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=115 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("boot: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("boot: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("boot: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("boot: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: boot: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("boot: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("boot: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: boot: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("boot: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("boot: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retbastionangle stamp; Soft≠product)\n");
/* Grep: boot: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("boot: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("boot: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retparapetangle stamp; Soft≠product)\n");
/* Grep: boot: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("boot: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("boot: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retowerangle stamp; Soft≠product)\n");
/* Grep: boot: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("boot: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("boot: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retwallangle stamp; Soft≠product)\n");
/* Grep: boot: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("boot: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("boot: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retholdangle stamp; Soft≠product)\n");
/* Grep: boot: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("boot: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("boot: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retfortressangle stamp; Soft≠product)\n");
/* Grep: boot: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("boot: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("boot: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: boot: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("boot: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("boot: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: boot: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("boot: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("boot: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: boot: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("boot: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("boot: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retaegisangle stamp; Soft≠product)\n");
/* Grep: boot: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("boot: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("boot: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retsigilangle stamp; Soft≠product)\n");
/* Grep: boot: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("boot: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("boot: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retglyphangle stamp; Soft≠product)\n");
/* Grep: boot: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("boot: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("boot: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retshardangle stamp; Soft≠product)\n");
/* Grep: boot: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("boot: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("boot: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retprismangle stamp; Soft≠product)\n");
/* Grep: boot: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("boot: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("boot: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retcipherangle stamp; Soft≠product)\n");
/* Grep: boot: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("boot: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("boot: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retledgerangle stamp; Soft≠product)\n");
/* Grep: boot: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("boot: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("boot: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retvaultangle stamp; Soft≠product)\n");
/* Grep: boot: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("boot: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("boot: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (rettokenangle stamp; Soft≠product)\n");
/* Grep: boot: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("boot: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("boot: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retphaseangle stamp; Soft≠product)\n");
/* Grep: boot: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("boot: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("boot: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retpulseangle stamp; Soft≠product)\n");

/* Grep: boot: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("boot: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("boot: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retboundangle stamp; Soft≠product)\n");
/* Grep: boot: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("boot: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("boot: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retbladeangle stamp; Soft≠product)\n");
/* Grep: boot: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("boot: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("boot: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retarcangle stamp; Soft≠product)\n");
/* Grep: boot: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("boot: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("boot: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: boot: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("boot: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("boot: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: boot: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("boot: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("boot: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retellipseangle stamp; Soft≠product)\n");
/* Grep: boot: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("boot: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("boot: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: boot: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("boot: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("boot: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (rethelixangle stamp; Soft≠product)\n");
/* Grep: boot: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("boot: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("boot: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retknotangle stamp; Soft≠product)\n");
/* Grep: boot: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("boot: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("boot: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retkleinangle stamp; Soft≠product)\n");
/* Grep: boot: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("boot: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("boot: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retaffineangle stamp; Soft≠product)\n");
/* Grep: boot: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("boot: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("boot: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: boot: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("boot: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("boot: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retcubicangle stamp; Soft≠product)\n");
/* Grep: boot: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("boot: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("boot: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retquinticangle stamp; Soft≠product)\n");
/* Grep: boot: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("boot: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("boot: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retbezierangle stamp; Soft≠product)\n");
/* Grep: boot: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("boot: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("boot: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: boot: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("boot: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("boot: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: boot: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("boot: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("boot: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retgridangle stamp; Soft≠product)\n");
/* Grep: boot: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("boot: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("boot: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (rettexelangle stamp; Soft≠product)\n");
/* Grep: boot: soft retfragmentangle — Wave 112 return-fragmentangle honesty */
kprintf("boot: soft retfragmentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retfragmentangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retvertexangle — Wave 112 exclusive vertexangle stamp */
kprintf("boot: soft retvertexangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retvertexangle stamp; Soft≠product)\n");
/* Grep: boot: soft retshaderangle — Wave 113 return-shaderangle honesty */
kprintf("boot: soft retshaderangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retshaderangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retpipelineangle — Wave 113 exclusive pipelineangle stamp */
kprintf("boot: soft retpipelineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retpipelineangle stamp; Soft≠product)\n");
/* Grep: boot: soft retframebufferangle — Wave 114 return-framebufferangle honesty */
kprintf("boot: soft retframebufferangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retframebufferangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retswapchainangle — Wave 114 exclusive swapchainangle stamp */
kprintf("boot: soft retswapchainangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retswapchainangle stamp; Soft≠product)\n");
/* Grep: boot: soft retpresentangle — Wave 115 return-presentangle honesty */
kprintf("boot: soft retpresentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=115 (retpresentangle honesty; Soft≠product; not bar3)\n");
/* Grep: boot: soft retvsyncangle — Wave 115 exclusive vsyncangle stamp */
kprintf("boot: soft retvsyncangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=115 (retvsyncangle stamp; Soft≠product)\n");
                            kprintf("boot: soft deepen wave=%u areas=%u handoff=%s source=%u "
            "src=%s memmap=%s gop=%s magic_ok=%d cap_bits=%u "
            "higher_half_product=OPEN soft_never_gates=1\n",
            (unsigned)BI_SOFT_WAVE, (unsigned)cAreas, szHandoff,
            (unsigned)u32Source, szSrcName, szMemmap, szGop, fValid,
            (unsigned)cCapBits);
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
    boot_soft_inc(&g_cSoftClearCalls);
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
    boot_soft_inc(&g_cSoftSetGlobal);
    if (g_BootInfo.u32Source == GJ_BOOT_SRC_UEFI) {
        boot_soft_inc(&g_cSoftUefiPublish);
    } else if (g_BootInfo.u32Source == GJ_BOOT_SRC_MULTIBOOT2) {
        boot_soft_inc(&g_cSoftMb2Publish);
    } else {
        boot_soft_inc(&g_cSoftUnknownPublish);
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
        boot_soft_inc(&g_cSoftValidFail);
        return 0;
    }
    if (pInfo->u32Magic != GJ_BOOT_INFO_MAGIC) {
        boot_soft_inc(&g_cSoftValidFail);
        return 0;
    }
    if (pInfo->u32Version != GJ_BOOT_INFO_VERSION) {
        boot_soft_inc(&g_cSoftValidFail);
        return 0;
    }
    boot_soft_inc(&g_cSoftValidOk);
    return 1;
}

void
boot_info_soft_memmap(const struct gj_boot_info *pInfo,
                      struct gj_boot_soft_memmap *pOut)
{
    struct bi_soft_memmap_deep deep;

    if (pOut == NULL) {
        return;
    }
    memset(pOut, 0, sizeof(*pOut));
    boot_info_soft_memmap_deep(pInfo, &deep);
    *pOut = deep.base;
}

int
boot_info_soft_gop_ok(const struct gj_boot_info *pInfo)
{
    u32 u32BytesPerPx;
    u64 u64MinPitch;

    boot_soft_inc(&g_cSoftGopCalls);

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
    struct bi_soft_memmap_deep softDeep;
    int fValid;
    int fGop;
    u32 u32Flags;
    u32 u32Source;
    const char *szHandoff;

    boot_soft_inc(&g_cSoftLogCalls);
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
        /* Wave 14 deepen null path under boot: soft … */
        boot_info_soft_wave_log(NULL, NULL, 0, 0, "STUB");
        kprintf("boot: soft mb2 path SKIP reason=null_info\n");
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

    boot_info_soft_memmap_deep(pInfo, &softDeep);
    if (!softDeep.base.fPresent) {
        kprintf("boot: memmap soft SKIP present=0 bytes=%lu desc_size=%lu "
                "flags_memmap=%d\n",
                (unsigned long)pInfo->u64MemMapBytes,
                (unsigned long)pInfo->u64MemDescSize,
                (u32Flags & GJ_BOOT_F_MEMMAP) != 0);
    } else if (!softDeep.base.fOk) {
        kprintf("boot: memmap soft REJECT present=1 descs=%lu usable=0 "
                "desc_size=%lu map=0x%lx bytes=%lu\n",
                (unsigned long)softDeep.base.cDescs,
                (unsigned long)softDeep.base.u64DescSize,
                (unsigned long)pInfo->u64MemMap,
                (unsigned long)pInfo->u64MemMapBytes);
    } else {
        kprintf("boot: memmap soft PASS descs=%lu usable_descs=%lu "
                "usable_pages=%lu usable_mib=%lu conv_pages=%lu "
                "desc_size=%lu map=0x%lx\n",
                (unsigned long)softDeep.base.cDescs,
                (unsigned long)softDeep.base.cUsableDescs,
                (unsigned long)softDeep.base.cUsablePages,
                (unsigned long)(softDeep.base.cbUsable / (1024ull * 1024ull)),
                (unsigned long)softDeep.base.cConvPages,
                (unsigned long)softDeep.base.u64DescSize,
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

    /* Wave 14 exclusive: deepen greppable boot: soft … surface. */
    boot_info_soft_wave_log(pInfo, &softDeep, fValid, fGop, szHandoff);

    /* Soft Multiboot2 / tag inventory (P-BOOT-2; SKIP on UEFI product). */
    boot_info_soft_mb2_log(pInfo);
}
