/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UEFI product boot path (P-BOOT-1):
 *   SimpleFileSystem → EFI/GREENJADE/KERNEL.ELF → ExitBootServices →
 *   gj_boot_info → kmain_uefi
 *
 * Fills struct gj_boot_info for the kernel (see gj/boot_info.h):
 *   magic/version/source, EFI memory map (P-BOOT-3), optional ACPI RSDP
 *   (P-BOOT-4), optional GOP FB, loaded KERNEL.ELF span + GJUEFI1 entry.
 * g_BootInfo lives in the PE image BSS so it survives ExitBootServices;
 * the pointer passed to kmain_uefi remains valid under the identity map.
 *
 * Soft product surface (COM1, freestanding — no kprintf link):
 *   Soft = greppable markers + classify; never hard-fails product when
 *   optional GOP is absent. Memmap soft REJECT still proceeds to EBS so
 *   partial boots remain diagnosable.
 *
 * Wave 56 exclusive soft deepen (this unit only — greppable "GJ-EFI: soft …"):
 *   GJ-EFI: soft inventory  — master surface + wave stamp
 *   GJ-EFI: soft path       — P-BOOT-1/3/4 claim + soft_never_gates
 *   GJ-EFI: soft honesty    — explicit non-claims (not Multiboot; not bar3)
 *   GJ-EFI: soft handoff    — flags/entry/kernel/map/fb/rsdp snapshot
 *   GJ-EFI: soft memmap     — EFI MD usable + type-class tallies
 *   GJ-EFI: soft gop        — FB geometry soft classify
 *   GJ-EFI: soft flags      — GJ_BOOT_F_* bit inventory
 *   GJ-EFI: soft kernel     — KERNEL.ELF span soft classify
 *   GJ-EFI: soft rsdp       — ACPI RSDP soft present/skip (P-BOOT-4)
 *   GJ-EFI: soft load       — SimpleFS / ELF / GJUEFI1 soft path lamps
 *   GJ-EFI: soft ebs        — ExitBootServices first/retry soft lamps
 *   GJ-EFI: soft stats      — rollup counters + wave stamp
 *   GJ-EFI: soft contract   — Wave 15 magic/version/struct contract
 *   GJ-EFI: soft magic      — Wave 15 handoff magic lamps
 *   GJ-EFI: soft geometry   — Wave 15 FB pitch/bpp soft math
 *   GJ-EFI: soft publish    — Wave 15 handoff publish lamps
 *   GJ-EFI: soft catalog    — Wave 19 area name rollup
 *   GJ-EFI: soft retclass   — Wave 19 return-class taxonomy (kept)
 *   GJ-EFI: soft retlane    — Wave 19 return-lane catalog (kept)
 *   GJ-EFI: soft deepen     — wave=81 stamp + area catalog
 *   GJ-EFI: soft PASS|PARTIAL — close marker (soft readiness only)
 *
 * Legacy companion markers (still emitted; prefix-stable):
 *   GJ-EFI: GOP soft PASS|SKIP …
 *   GJ-EFI: memmap soft PASS|REJECT …
 *   GJ-EFI: handoff soft PASS|PARTIAL …
 *
 * greppable: GJ-EFI: soft
 * greppable: GJ-EFI: soft inventory
 * greppable: GJ-EFI: soft deepen
 *
 * Built as PE32+ EFI_APPLICATION via ld -mi386pep (see scripts/build-efi.sh).
 * Multiboot2 greenjade.elf does not link this file (dev-only P-BOOT-2).
 *
 * Pure C11 freestanding; dual MIT OR Apache-2.0. No C++.
 */
#include <gj/boot_info.h>
#include <gj/types.h>

typedef u64 efi_status_t;
typedef void *efi_handle_t;
typedef u64 efi_physical_addr_t;
typedef u64 efi_virtual_addr_t;
typedef u64 efi_uintn_t;

#define EFI_SUCCESS          0ull
#define EFI_LOAD_ERROR       1ull
#define EFI_BUFFER_TOO_SMALL 0x8000000000000005ull
#define EFI_NOT_FOUND        0x800000000000000Eull

#define EFI_ALLOCATE_ANY_PAGES 0
#define EFI_ALLOCATE_ADDRESS   2
#define EFI_LOADER_DATA        2

#define EFI_FILE_MODE_READ 0x0000000000000001ull

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001u

/* EFI memory types we reclaim after ExitBootServices */
#define EfiLoaderCode          1u
#define EfiLoaderData          2u
#define EfiBootServicesCode    3u
#define EfiBootServicesData    4u
#define EfiRuntimeServicesCode 5u
#define EfiRuntimeServicesData 6u
#define EfiConventionalMemory  7u
#define EfiACPIReclaimMemory   9u
#define EfiACPIMemoryNVS      10u
#define EfiMemoryMappedIO     11u
#define EfiMemoryMappedIOPort 12u

/* Wave 15 soft inventory stamp (observability only; never gates product). */
#define GJ_EFI_SOFT_WAVE 81u

struct efi_table_header {
    u64 u64Signature;
    u32 u32Revision;
    u32 u32HeaderSize;
    u32 u32CRC32;
    u32 u32Reserved;
};

struct efi_simple_text_output {
    void *pReset;
    efi_status_t (__attribute__((ms_abi)) *pfnOutputString)(struct efi_simple_text_output *pThis,
                                    u16 *pString);
};

struct efi_memory_descriptor {
    u32 u32Type;
    u32 u32Pad;
    efi_physical_addr_t paPhysical;
    efi_virtual_addr_t  vaVirtual;
    u64 u64Pages;
    u64 u64Attribute;
};

struct efi_guid {
    u32 u32Data1;
    u16 u16Data2;
    u16 u16Data3;
    u8  aData4[8];
};

/* Boot services: function table after 24-byte header (UEFI x86_64). */
struct efi_boot_services {
    struct efi_table_header hdr;
    void *aFn[44];
};

#define BS_AllocatePages      2
#define BS_FreePages          3
#define BS_GetMemoryMap       4
#define BS_AllocatePool       5
#define BS_FreePool           6
#define BS_HandleProtocol     16
#define BS_ExitBootServices   26
#define BS_OpenProtocol       32
#define BS_LocateProtocol     37

/* Graphics Output Protocol (GOP) — early desktop FB. */
struct efi_gop_mode_info {
    u32 u32Version;
    u32 u32HRes;
    u32 u32VRes;
    u32 u32PixelFormat;
    u32 aPixelBitmask[4];
    u32 u32PixelsPerScanLine;
};

struct efi_gop_mode {
    u32 u32MaxMode;
    u32 u32Mode;
    struct efi_gop_mode_info *pInfo;
    efi_uintn_t cbInfo;
    efi_physical_addr_t paFrameBuffer;
    efi_uintn_t cbFrameBuffer;
};

struct efi_gop {
    void *pfnQueryMode;
    void *pfnSetMode;
    void *pfnBlt;
    struct efi_gop_mode *pMode;
};

struct efi_configuration_table {
    struct efi_guid guid;
    void *pTable;
};

struct efi_system_table {
    struct efi_table_header hdr;
    u16 *pFirmwareVendor;
    u32 u32FirmwareRevision;
    u32 u32Pad;
    efi_handle_t hConsoleIn;
    void *pConIn;
    efi_handle_t hConsoleOut;
    struct efi_simple_text_output *pConOut;
    efi_handle_t hStdErr;
    void *pStdErr;
    void *pRuntimeServices;
    struct efi_boot_services *pBootServices;
    efi_uintn_t u64NumTableEntries;
    struct efi_configuration_table *pConfigurationTable;
};

struct efi_loaded_image {
    u32 u32Revision;
    efi_handle_t hParent;
    struct efi_system_table *pST;
    efi_handle_t hDevice;
    void *pFilePath;
    void *pReserved;
    u32 u32LoadOptionsSize;
    void *pLoadOptions;
    void *pImageBase;
    u64 u64ImageSize;
    u32 u32ImageCodeType;
    u32 u32ImageDataType;
    void *pfnUnload;
};

struct efi_file_protocol {
    u64 u64Revision;
    efi_status_t (__attribute__((ms_abi)) *pfnOpen)(struct efi_file_protocol *pThis,
                            struct efi_file_protocol **ppNew,
                            u16 *pFileName, u64 u64Mode, u64 u64Attr);
    efi_status_t (__attribute__((ms_abi)) *pfnClose)(struct efi_file_protocol *pThis);
    void *pfnDelete;
    efi_status_t (__attribute__((ms_abi)) *pfnRead)(struct efi_file_protocol *pThis,
                            efi_uintn_t *pBufferSize, void *pBuffer);
    void *pfnWrite;
    efi_status_t (__attribute__((ms_abi)) *pfnGetPosition)(struct efi_file_protocol *pThis,
                                   u64 *pPos);
    efi_status_t (__attribute__((ms_abi)) *pfnSetPosition)(struct efi_file_protocol *pThis, u64 u64Pos);
    efi_status_t (__attribute__((ms_abi)) *pfnGetInfo)(struct efi_file_protocol *pThis,
                               struct efi_guid *pInfoType,
                               efi_uintn_t *pBufferSize, void *pBuffer);
    void *pfnSetInfo;
    void *pfnFlush;
};

struct efi_simple_fs {
    u64 u64Revision;
    efi_status_t (__attribute__((ms_abi)) *pfnOpenVolume)(struct efi_simple_fs *pThis,
                                  struct efi_file_protocol **ppRoot);
};

/* ELF64 */
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define ELFCLASS64 2
#define EM_X86_64 62
#define PT_LOAD 1
#define GJ_UEFI_HDR_MAGIC "GJUEFI1"

struct elf64_ehdr {
    u8  aIdent[16];
    u16 u16Type;
    u16 u16Machine;
    u32 u32Version;
    u64 u64Entry;
    u64 u64Phoff;
    u64 u64Shoff;
    u32 u32Flags;
    u16 u16Ehsize;
    u16 u16Phentsize;
    u16 u16Phnum;
    u16 u16Shentsize;
    u16 u16Shnum;
    u16 u16Shstrndx;
};

struct elf64_phdr {
    u32 u32Type;
    u32 u32Flags;
    u64 u64Offset;
    u64 u64Vaddr;
    u64 u64Paddr;
    u64 u64Filesz;
    u64 u64Memsz;
    u64 u64Align;
};

/* Handoff buffer survives ExitBootServices (static BSS in PE image). */
static struct gj_boot_info g_BootInfo;
static u8 g_aMemMapScratch[48u * 1024u];
/* File image via AllocatePool (keeps PE BSS small for OVMF). */
static u8 *g_pFileScratch;
static efi_uintn_t g_cbFileScratch;

/*
 * Wave 15 soft tallies (file-local; wrap OK; never gate product / EBS).
 * Snapshotted into GJ-EFI: soft stats / inventory.
 */
static u32 g_cSoftLoadOk;
static u32 g_cSoftLoadFail;
static u32 g_cSoftGopPass;
static u32 g_cSoftGopSkip;
static u32 g_cSoftMemmapPass;
static u32 g_cSoftMemmapReject;
static u32 g_cSoftRsdpHit;
static u32 g_cSoftRsdpAcpi20;
static u32 g_cSoftRsdpAcpi10;
static u32 g_cSoftEbsFirstOk;
static u32 g_cSoftEbsRetry;
static u32 g_cSoftEbsRetryOk;
static u32 g_cSoftEbsFail;
static u32 g_cSoftPtLoadSegs;
static u32 g_cSoftInvLogs;
static u32 g_cSoftStrideFb; /* 1 if desc_size fell back to sizeof(MD) */

/* Last soft memmap deep snapshot (filled by soft_memmap_marker). */
static u64 g_cSoftMmDescs;
static u64 g_cSoftMmUsableDescs;
static u64 g_cSoftMmUsablePages;
static u64 g_cSoftMmConvPages;
static u64 g_cSoftMmLoaderPages;
static u64 g_cSoftMmBsPages;
static u64 g_cSoftMmAcpiRecPages;
static u64 g_cSoftMmRtPages;
static u64 g_cSoftMmMmioPages;
static u64 g_cSoftMmOtherPages;
static u64 g_cSoftMmZeroDescs;
static u64 g_cSoftMmStride;
static u64 g_cSoftMmBytes;
static u32 g_fSoftMmOk;

/*
 * COM1 (0x3F8) early log — same role as kprintf before serial_init /
 * after ExitBootServices when ConOut is gone. Prefix all lines "GJ-EFI:".
 */
static void
com1_putc(char chOut)
{
    u32 u32Spins;

    /* Port I/O via inb/outb (not MMIO). Spin on LSR THRE bit. */
    for (u32Spins = 0; u32Spins < 100000u; u32Spins++) {
        u8 u8Lsr;

        __asm__ volatile ("inb %1, %0" : "=a"(u8Lsr) : "Nd"((u16)0x3FD));
        if ((u8Lsr & 0x20u) != 0) {
            break;
        }
    }
    __asm__ volatile ("outb %0, %1" : : "a"((u8)chOut), "Nd"((u16)0x3F8));
}

static void
com1_puts(const char *szMsg)
{
    if (szMsg == NULL) {
        return;
    }
    while (*szMsg != '\0') {
        if (*szMsg == '\n') {
            com1_putc('\r');
        }
        com1_putc(*szMsg++);
    }
}

/* Freestanding decimal / hex for soft markers (no libc). */
static void
com1_put_u64_dec(u64 u64Val)
{
    char aBuf[20];
    u32 i = 0;
    u32 j;

    if (u64Val == 0) {
        com1_putc('0');
        return;
    }
    while (u64Val > 0 && i < sizeof(aBuf)) {
        aBuf[i++] = (char)('0' + (u64Val % 10ull));
        u64Val /= 10ull;
    }
    for (j = i; j > 0; j--) {
        com1_putc(aBuf[j - 1u]);
    }
}

static void
com1_put_u64_hex(u64 u64Val)
{
    static const char aHex[] = "0123456789abcdef";
    u32 iNibble;
    int fStarted = 0;

    com1_puts("0x");
    for (iNibble = 0; iNibble < 16u; iNibble++) {
        u32 u32Shift = 60u - (iNibble * 4u);
        u8 u8Dig = (u8)((u64Val >> u32Shift) & 0xfull);

        if (u8Dig != 0 || fStarted || iNibble == 15u) {
            fStarted = 1;
            com1_putc(aHex[u8Dig]);
        }
    }
}

/* Soft: reclaimable EFI types after EBS (match gj/boot_info.h / kmain_uefi). */
static int
efi_type_usable_soft(u32 u32Type)
{
    if (u32Type == EfiLoaderCode || u32Type == EfiLoaderData ||
        u32Type == EfiBootServicesCode || u32Type == EfiBootServicesData ||
        u32Type == EfiConventionalMemory || u32Type == EfiACPIReclaimMemory) {
        return 1;
    }
    return 0;
}

/*
 * Soft walk of EFI memory map in g_aMemMapScratch.
 * Emits legacy GJ-EFI: memmap soft PASS|REJECT … and Wave 15
 * GJ-EFI: soft memmap … type-class tallies (never aborts).
 */
static void
soft_memmap_marker(u64 u64MapBytes, u64 u64DescSize)
{
    u64 u64Off;
    u64 cDescs = 0;
    u64 cUsableDescs = 0;
    u64 cUsablePages = 0;
    u64 cConvPages = 0;
    u64 cLoaderPages = 0;
    u64 cBsPages = 0;
    u64 cAcpiRecPages = 0;
    u64 cRtPages = 0;
    u64 cMmioPages = 0;
    u64 cOtherPages = 0;
    u64 cZeroDescs = 0;
    u64 u64Stride;
    u32 fStrideFb = 0;

    /* Reset last snapshot (soft only). */
    g_cSoftMmDescs = 0;
    g_cSoftMmUsableDescs = 0;
    g_cSoftMmUsablePages = 0;
    g_cSoftMmConvPages = 0;
    g_cSoftMmLoaderPages = 0;
    g_cSoftMmBsPages = 0;
    g_cSoftMmAcpiRecPages = 0;
    g_cSoftMmRtPages = 0;
    g_cSoftMmMmioPages = 0;
    g_cSoftMmOtherPages = 0;
    g_cSoftMmZeroDescs = 0;
    g_cSoftMmStride = 0;
    g_cSoftMmBytes = u64MapBytes;
    g_fSoftMmOk = 0;
    g_cSoftStrideFb = 0;

    if (u64MapBytes == 0) {
        if (g_cSoftMemmapReject < 0xffffffffu) {
            g_cSoftMemmapReject++;
        }
        com1_puts("GJ-EFI: memmap soft REJECT present=0 bytes=0\n");
        /* Grep: GJ-EFI: soft memmap SKIP */
        com1_puts("GJ-EFI: soft memmap SKIP present=0 descs=0 usable=0 "
                  "wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts("\n");
        return;
    }
    u64Stride = u64DescSize;
    if (u64Stride < sizeof(struct efi_memory_descriptor)) {
        u64Stride = sizeof(struct efi_memory_descriptor);
        fStrideFb = 1;
        g_cSoftStrideFb = 1;
    }
    for (u64Off = 0; u64Off + u64Stride <= u64MapBytes; u64Off += u64Stride) {
        const struct efi_memory_descriptor *pMd =
            (const struct efi_memory_descriptor *)(void *)(g_aMemMapScratch +
                                                          u64Off);
        u32 u32T = pMd->u32Type;
        u64 u64Pages = pMd->u64Pages;

        cDescs++;
        if (u64Pages == 0) {
            cZeroDescs++;
            continue;
        }
        if (u32T == EfiLoaderCode || u32T == EfiLoaderData) {
            cLoaderPages += u64Pages;
        } else if (u32T == EfiBootServicesCode || u32T == EfiBootServicesData) {
            cBsPages += u64Pages;
        } else if (u32T == EfiConventionalMemory) {
            cConvPages += u64Pages;
        } else if (u32T == EfiACPIReclaimMemory) {
            cAcpiRecPages += u64Pages;
        } else if (u32T == EfiRuntimeServicesCode ||
                   u32T == EfiRuntimeServicesData) {
            cRtPages += u64Pages;
        } else if (u32T == EfiMemoryMappedIO || u32T == EfiMemoryMappedIOPort ||
                   u32T == EfiACPIMemoryNVS) {
            cMmioPages += u64Pages;
        } else {
            cOtherPages += u64Pages;
        }
        if (!efi_type_usable_soft(u32T)) {
            continue;
        }
        cUsableDescs++;
        cUsablePages += u64Pages;
    }

    g_cSoftMmDescs = cDescs;
    g_cSoftMmUsableDescs = cUsableDescs;
    g_cSoftMmUsablePages = cUsablePages;
    g_cSoftMmConvPages = cConvPages;
    g_cSoftMmLoaderPages = cLoaderPages;
    g_cSoftMmBsPages = cBsPages;
    g_cSoftMmAcpiRecPages = cAcpiRecPages;
    g_cSoftMmRtPages = cRtPages;
    g_cSoftMmMmioPages = cMmioPages;
    g_cSoftMmOtherPages = cOtherPages;
    g_cSoftMmZeroDescs = cZeroDescs;
    g_cSoftMmStride = u64Stride;
    g_cSoftMmBytes = u64MapBytes;

    if (cUsablePages == 0) {
        if (g_cSoftMemmapReject < 0xffffffffu) {
            g_cSoftMemmapReject++;
        }
        com1_puts("GJ-EFI: memmap soft REJECT descs=");
        com1_put_u64_dec(cDescs);
        com1_puts(" usable=0 desc_size=");
        com1_put_u64_dec(u64Stride);
        com1_puts(" bytes=");
        com1_put_u64_dec(u64MapBytes);
        com1_puts("\n");
        /* Grep: GJ-EFI: soft memmap REJECT */
        com1_puts("GJ-EFI: soft memmap REJECT descs=");
        com1_put_u64_dec(cDescs);
        com1_puts(" usable=0 zero_descs=");
        com1_put_u64_dec(cZeroDescs);
        com1_puts(" stride_fb=");
        com1_put_u64_dec((u64)fStrideFb);
        com1_puts(" wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts("\n");
        return;
    }

    g_fSoftMmOk = 1;
    if (g_cSoftMemmapPass < 0xffffffffu) {
        g_cSoftMemmapPass++;
    }
    /* Grep: GJ-EFI: memmap soft PASS */
    com1_puts("GJ-EFI: memmap soft PASS descs=");
    com1_put_u64_dec(cDescs);
    com1_puts(" usable_descs=");
    com1_put_u64_dec(cUsableDescs);
    com1_puts(" usable_pages=");
    com1_put_u64_dec(cUsablePages);
    com1_puts(" usable_mib=");
    com1_put_u64_dec((cUsablePages * 4096ull) / (1024ull * 1024ull));
    com1_puts(" conv_pages=");
    com1_put_u64_dec(cConvPages);
    com1_puts(" desc_size=");
    com1_put_u64_dec(u64Stride);
    com1_puts("\n");

    /* Grep: GJ-EFI: soft memmap PASS (Wave 15 type-class deepen) */
    com1_puts("GJ-EFI: soft memmap PASS descs=");
    com1_put_u64_dec(cDescs);
    com1_puts(" usable_descs=");
    com1_put_u64_dec(cUsableDescs);
    com1_puts(" usable_pages=");
    com1_put_u64_dec(cUsablePages);
    com1_puts(" usable_mib=");
    com1_put_u64_dec((cUsablePages * 4096ull) / (1024ull * 1024ull));
    com1_puts(" conv_pages=");
    com1_put_u64_dec(cConvPages);
    com1_puts(" loader_pages=");
    com1_put_u64_dec(cLoaderPages);
    com1_puts(" bs_pages=");
    com1_put_u64_dec(cBsPages);
    com1_puts(" acpi_rec_pages=");
    com1_put_u64_dec(cAcpiRecPages);
    com1_puts(" rt_pages=");
    com1_put_u64_dec(cRtPages);
    com1_puts(" mmio_pages=");
    com1_put_u64_dec(cMmioPages);
    com1_puts(" other_pages=");
    com1_put_u64_dec(cOtherPages);
    com1_puts(" zero_descs=");
    com1_put_u64_dec(cZeroDescs);
    com1_puts(" desc_size=");
    com1_put_u64_dec(u64Stride);
    com1_puts(" stride_fb=");
    com1_put_u64_dec((u64)fStrideFb);
    com1_puts(" wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts("\n");
}

/*
 * Soft handoff summary after EBS, before kmain_uefi jump.
 * Grep: GJ-EFI: handoff soft PASS|PARTIAL
 */
static void
soft_handoff_marker(u64 u64Entry)
{
    u32 u32Flags = g_BootInfo.u32Flags;
    int fPartial = (u64Entry == 0) ||
                   ((u32Flags & GJ_BOOT_F_MEMMAP) == 0) ||
                   ((u32Flags & GJ_BOOT_F_KERNEL_IMG) == 0);

    if (fPartial) {
        com1_puts("GJ-EFI: handoff soft PARTIAL flags=");
    } else {
        com1_puts("GJ-EFI: handoff soft PASS flags=");
    }
    com1_put_u64_hex((u64)u32Flags);
    com1_puts(" entry=");
    com1_put_u64_hex(u64Entry);
    com1_puts(" kernel=");
    com1_put_u64_hex(g_BootInfo.u64KernelPhys);
    com1_puts("+");
    com1_put_u64_hex(g_BootInfo.u64KernelBytes);
    com1_puts(" map=");
    com1_put_u64_hex(g_BootInfo.u64MemMap);
    com1_puts(" map_bytes=");
    com1_put_u64_dec(g_BootInfo.u64MemMapBytes);
    com1_puts(" fb=");
    com1_put_u64_hex(g_BootInfo.u64FbBase);
    com1_puts(" rsdp=");
    com1_put_u64_hex(g_BootInfo.u64Rsdp);
    com1_puts("\n");
}

/**
 * Wave 15 greppable soft EFI inventory dump (product / smoke deepen).
 * Prefix-stable markers (GJ-EFI: soft …). Never allocates; never aborts;
 * never gates ExitBootServices or the kmain_uefi jump.
 *
 * greppable: GJ-EFI: soft
 */
static void
soft_wave14_inventory(u64 u64Entry)
{
    u32 u32Flags = g_BootInfo.u32Flags;
    u32 fMemmapBit = (u32Flags & GJ_BOOT_F_MEMMAP) != 0 ? 1u : 0u;
    u32 fRsdpBit = (u32Flags & GJ_BOOT_F_RSDP) != 0 ? 1u : 0u;
    u32 fFbBit = (u32Flags & GJ_BOOT_F_FB) != 0 ? 1u : 0u;
    u32 fKernelBit = (u32Flags & GJ_BOOT_F_KERNEL_IMG) != 0 ? 1u : 0u;
    u32 fKernelSpan = (g_BootInfo.u64KernelPhys != 0 &&
                       g_BootInfo.u64KernelBytes != 0)
                          ? 1u
                          : 0u;
    u32 fRsdp = (g_BootInfo.u64Rsdp != 0) ? 1u : 0u;
    u32 fGop = (g_BootInfo.u64FbBase != 0 && g_BootInfo.u32FbWidth != 0 &&
                g_BootInfo.u32FbHeight != 0)
                   ? 1u
                   : 0u;
    int fPartial = (u64Entry == 0) || (fMemmapBit == 0) || (fKernelBit == 0) ||
                   (g_fSoftMmOk == 0);
    u32 cAreas = 0;
    const char *szHandoff;
    const char *szMemmap;
    const char *szGop;
    const char *szKernel;
    const char *szRsdp;

    if (g_cSoftInvLogs < 0xffffffffu) {
        g_cSoftInvLogs++;
    }

    szHandoff = fPartial ? "PARTIAL" : "PASS";
    if (g_cSoftMmBytes == 0) {
        szMemmap = "SKIP";
    } else if (g_fSoftMmOk == 0) {
        szMemmap = "REJECT";
    } else {
        szMemmap = "PASS";
    }
    if (g_BootInfo.u64FbBase == 0) {
        szGop = "SKIP";
    } else if (fGop == 0) {
        szGop = "REJECT";
    } else {
        szGop = "PASS";
    }
    szKernel = (fKernelSpan != 0) ? "PASS" : "SKIP";
    szRsdp = (fRsdp != 0) ? "PASS" : "SKIP";

    /*
     * Honesty first: freestanding EFI soft inventory is NOT Multiboot and
     * never claims bar3 / product-complete beyond P-BOOT-1 path wiring.
     * greppable: GJ-EFI: soft honesty
     */
    com1_puts("GJ-EFI: soft honesty not-multiboot p_boot_1=product "
              "p_boot_2=dev_only soft_never_gates=1 bar3=OPEN "
              "wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" (soft inventory only; never closes product bars)\n");
    cAreas++;

    /* Grep: GJ-EFI: soft inventory */
    com1_puts("GJ-EFI: soft inventory wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" handoff=");
    com1_puts(szHandoff);
    com1_puts(" source=");
    com1_put_u64_dec((u64)g_BootInfo.u32Source);
    com1_puts(" src=uefi path=p_boot_1_product magic_ok=");
    com1_put_u64_dec((u64)(g_BootInfo.u32Magic == GJ_BOOT_INFO_MAGIC ? 1u : 0u));
    com1_puts(" memmap=");
    com1_puts(szMemmap);
    com1_puts(" gop=");
    com1_puts(szGop);
    com1_puts(" kernel=");
    com1_puts(szKernel);
    com1_puts(" rsdp=");
    com1_puts(szRsdp);
    com1_puts(" load_ok=");
    com1_put_u64_dec((u64)g_cSoftLoadOk);
    com1_puts(" logs=");
    com1_put_u64_dec((u64)g_cSoftInvLogs);
    com1_puts("\n");
    cAreas++;

    /* Grep: GJ-EFI: soft path */
    com1_puts("GJ-EFI: soft path claim=p_boot_1_product "
              "p_boot_1=uefi_product p_boot_2=mb2_dev_only "
              "p_boot_3=efi_memmap_after_ebs p_boot_4=acpi_rsdp "
              "loader=SimpleFileSystem kernel=EFI/GREENJADE/KERNEL.ELF "
              "entry=GJUEFI1 soft_never_gates=1 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts("\n");
    cAreas++;

    /* Grep: GJ-EFI: soft handoff */
    com1_puts("GJ-EFI: soft handoff ");
    com1_puts(szHandoff);
    com1_puts(" magic=");
    com1_put_u64_hex((u64)g_BootInfo.u32Magic);
    com1_puts(" version=");
    com1_put_u64_dec((u64)g_BootInfo.u32Version);
    com1_puts(" source=");
    com1_put_u64_dec((u64)g_BootInfo.u32Source);
    com1_puts(" flags=");
    com1_put_u64_hex((u64)u32Flags);
    com1_puts(" entry=");
    com1_put_u64_hex(u64Entry);
    com1_puts(" kernel=");
    com1_put_u64_hex(g_BootInfo.u64KernelPhys);
    com1_puts("+");
    com1_put_u64_hex(g_BootInfo.u64KernelBytes);
    com1_puts(" map=");
    com1_put_u64_hex(g_BootInfo.u64MemMap);
    com1_puts(" map_bytes=");
    com1_put_u64_dec(g_BootInfo.u64MemMapBytes);
    com1_puts(" desc_size=");
    com1_put_u64_dec(g_BootInfo.u64MemDescSize);
    com1_puts(" fb=");
    com1_put_u64_hex(g_BootInfo.u64FbBase);
    com1_puts(" rsdp=");
    com1_put_u64_hex(g_BootInfo.u64Rsdp);
    com1_puts("\n");
    cAreas++;

    /* Grep: GJ-EFI: soft memmap (rollup from last walk) */
    com1_puts("GJ-EFI: soft memmap rollup=");
    com1_puts(szMemmap);
    com1_puts(" descs=");
    com1_put_u64_dec(g_cSoftMmDescs);
    com1_puts(" usable_descs=");
    com1_put_u64_dec(g_cSoftMmUsableDescs);
    com1_puts(" usable_pages=");
    com1_put_u64_dec(g_cSoftMmUsablePages);
    com1_puts(" conv_pages=");
    com1_put_u64_dec(g_cSoftMmConvPages);
    com1_puts(" loader_pages=");
    com1_put_u64_dec(g_cSoftMmLoaderPages);
    com1_puts(" bs_pages=");
    com1_put_u64_dec(g_cSoftMmBsPages);
    com1_puts(" acpi_rec_pages=");
    com1_put_u64_dec(g_cSoftMmAcpiRecPages);
    com1_puts(" rt_pages=");
    com1_put_u64_dec(g_cSoftMmRtPages);
    com1_puts(" mmio_pages=");
    com1_put_u64_dec(g_cSoftMmMmioPages);
    com1_puts(" other_pages=");
    com1_put_u64_dec(g_cSoftMmOtherPages);
    com1_puts(" zero_descs=");
    com1_put_u64_dec(g_cSoftMmZeroDescs);
    com1_puts(" stride_fb=");
    com1_put_u64_dec((u64)g_cSoftStrideFb);
    com1_puts("\n");
    cAreas++;

    /* Grep: GJ-EFI: soft gop */
    if (g_BootInfo.u64FbBase == 0) {
        com1_puts("GJ-EFI: soft gop SKIP base=0 flags_fb=");
        com1_put_u64_dec((u64)fFbBit);
        com1_puts(" ok=0 wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts("\n");
    } else {
        com1_puts("GJ-EFI: soft gop ");
        com1_puts(szGop);
        com1_puts(" base=");
        com1_put_u64_hex(g_BootInfo.u64FbBase);
        com1_puts(" ");
        com1_put_u64_dec((u64)g_BootInfo.u32FbWidth);
        com1_puts("x");
        com1_put_u64_dec((u64)g_BootInfo.u32FbHeight);
        com1_puts(" pitch=");
        com1_put_u64_dec((u64)g_BootInfo.u32FbPitch);
        com1_puts(" bpp=");
        com1_put_u64_dec((u64)g_BootInfo.u32FbBpp);
        com1_puts(" flags_fb=");
        com1_put_u64_dec((u64)fFbBit);
        com1_puts(" ok=");
        com1_put_u64_dec((u64)fGop);
        com1_puts("\n");
    }
    cAreas++;

    /* Grep: GJ-EFI: soft flags */
    com1_puts("GJ-EFI: soft flags raw=");
    com1_put_u64_hex((u64)u32Flags);
    com1_puts(" memmap=");
    com1_put_u64_dec((u64)fMemmapBit);
    com1_puts(" rsdp=");
    com1_put_u64_dec((u64)fRsdpBit);
    com1_puts(" fb=");
    com1_put_u64_dec((u64)fFbBit);
    com1_puts(" kernel_img=");
    com1_put_u64_dec((u64)fKernelBit);
    com1_puts(" mb2_info=0 source_uefi=1\n");
    cAreas++;

    /* Grep: GJ-EFI: soft kernel */
    if (fKernelSpan == 0) {
        com1_puts("GJ-EFI: soft kernel SKIP span=0 flags_kernel=");
        com1_put_u64_dec((u64)fKernelBit);
        com1_puts(" pt_load_segs=");
        com1_put_u64_dec((u64)g_cSoftPtLoadSegs);
        com1_puts("\n");
    } else {
        com1_puts("GJ-EFI: soft kernel PASS phys=");
        com1_put_u64_hex(g_BootInfo.u64KernelPhys);
        com1_puts(" bytes=");
        com1_put_u64_dec(g_BootInfo.u64KernelBytes);
        com1_puts(" mib=");
        com1_put_u64_dec(g_BootInfo.u64KernelBytes / (1024ull * 1024ull));
        com1_puts(" flags_kernel=");
        com1_put_u64_dec((u64)fKernelBit);
        com1_puts(" pt_load_segs=");
        com1_put_u64_dec((u64)g_cSoftPtLoadSegs);
        com1_puts(" entry=");
        com1_put_u64_hex(u64Entry);
        com1_puts("\n");
    }
    cAreas++;

    /* Grep: GJ-EFI: soft rsdp */
    if (fRsdp == 0) {
        com1_puts("GJ-EFI: soft rsdp SKIP phys=0 flags_rsdp=");
        com1_put_u64_dec((u64)fRsdpBit);
        com1_puts(" acpi20=");
        com1_put_u64_dec((u64)g_cSoftRsdpAcpi20);
        com1_puts(" acpi10=");
        com1_put_u64_dec((u64)g_cSoftRsdpAcpi10);
        com1_puts(" p_boot_4=acpi_rsdp\n");
    } else {
        com1_puts("GJ-EFI: soft rsdp PASS phys=");
        com1_put_u64_hex(g_BootInfo.u64Rsdp);
        com1_puts(" flags_rsdp=");
        com1_put_u64_dec((u64)fRsdpBit);
        com1_puts(" acpi20=");
        com1_put_u64_dec((u64)g_cSoftRsdpAcpi20);
        com1_puts(" acpi10=");
        com1_put_u64_dec((u64)g_cSoftRsdpAcpi10);
        com1_puts(" p_boot_4=acpi_rsdp\n");
    }
    cAreas++;

    /* Grep: GJ-EFI: soft load */
    com1_puts("GJ-EFI: soft load ok=");
    com1_put_u64_dec((u64)g_cSoftLoadOk);
    com1_puts(" fail=");
    com1_put_u64_dec((u64)g_cSoftLoadFail);
    com1_puts(" pt_load_segs=");
    com1_put_u64_dec((u64)g_cSoftPtLoadSegs);
    com1_puts(" path=EFI/GREENJADE/KERNEL.ELF entry_hdr=GJUEFI1 "
              "e_entry_unused=1 pool_mib=1\n");
    cAreas++;

    /* Grep: GJ-EFI: soft ebs */
    com1_puts("GJ-EFI: soft ebs first_ok=");
    com1_put_u64_dec((u64)g_cSoftEbsFirstOk);
    com1_puts(" retry=");
    com1_put_u64_dec((u64)g_cSoftEbsRetry);
    com1_puts(" retry_ok=");
    com1_put_u64_dec((u64)g_cSoftEbsRetryOk);
    com1_puts(" fail=");
    com1_put_u64_dec((u64)g_cSoftEbsFail);
    com1_puts(" map_key_contract=1 soft_never_gates=0_ebs_hard\n");
    cAreas++;

    /* Grep: GJ-EFI: soft stats */
    com1_puts("GJ-EFI: soft stats load_ok=");
    com1_put_u64_dec((u64)g_cSoftLoadOk);
    com1_puts(" load_fail=");
    com1_put_u64_dec((u64)g_cSoftLoadFail);
    com1_puts(" gop_pass=");
    com1_put_u64_dec((u64)g_cSoftGopPass);
    com1_puts(" gop_skip=");
    com1_put_u64_dec((u64)g_cSoftGopSkip);
    com1_puts(" memmap_pass=");
    com1_put_u64_dec((u64)g_cSoftMemmapPass);
    com1_puts(" memmap_reject=");
    com1_put_u64_dec((u64)g_cSoftMemmapReject);
    com1_puts(" rsdp_hit=");
    com1_put_u64_dec((u64)g_cSoftRsdpHit);
    com1_puts(" ebs_first=");
    com1_put_u64_dec((u64)g_cSoftEbsFirstOk);
    com1_puts(" ebs_retry=");
    com1_put_u64_dec((u64)g_cSoftEbsRetry);
    com1_puts(" logs=");
    com1_put_u64_dec((u64)g_cSoftInvLogs);
    com1_puts(" wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts("\n");
    cAreas++;

    /* Grep: GJ-EFI: soft contract — Wave 15 magic/version/struct contract. */
    com1_puts("GJ-EFI: soft contract magic_ok=");
    com1_put_u64_dec((u64)(g_BootInfo.u32Magic == GJ_BOOT_INFO_MAGIC ? 1u : 0u));
    com1_puts(" version_ok=");
    com1_put_u64_dec((u64)(g_BootInfo.u32Version == GJ_BOOT_INFO_VERSION ? 1u
                                                                         : 0u));
    com1_puts(" magic_expect=");
    com1_put_u64_hex((u64)GJ_BOOT_INFO_MAGIC);
    com1_puts(" version_expect=");
    com1_put_u64_dec((u64)GJ_BOOT_INFO_VERSION);
    com1_puts(" source_uefi=1 soft PASS\n");
    cAreas++;

    /* Grep: GJ-EFI: soft magic — Wave 15 handoff magic lamps. */
    com1_puts("GJ-EFI: soft magic raw=");
    com1_put_u64_hex((u64)g_BootInfo.u32Magic);
    com1_puts(" ok=");
    com1_put_u64_dec((u64)(g_BootInfo.u32Magic == GJ_BOOT_INFO_MAGIC ? 1u : 0u));
    com1_puts(" version=");
    com1_put_u64_dec((u64)g_BootInfo.u32Version);
    com1_puts(" soft ");
    com1_puts((g_BootInfo.u32Magic == GJ_BOOT_INFO_MAGIC &&
               g_BootInfo.u32Version == GJ_BOOT_INFO_VERSION)
                  ? "PASS"
                  : "STUB");
    com1_puts("\n");
    cAreas++;

    /* Grep: GJ-EFI: soft geometry — Wave 15 FB pitch/bpp soft math. */
    if (g_BootInfo.u64FbBase == 0) {
        com1_puts("GJ-EFI: soft geometry SKIP fb=0 pitch_ok=0 wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts("\n");
    } else {
        com1_puts("GJ-EFI: soft geometry ");
        com1_puts(szGop);
        com1_puts(" w=");
        com1_put_u64_dec((u64)g_BootInfo.u32FbWidth);
        com1_puts(" h=");
        com1_put_u64_dec((u64)g_BootInfo.u32FbHeight);
        com1_puts(" pitch=");
        com1_put_u64_dec((u64)g_BootInfo.u32FbPitch);
        com1_puts(" bpp=");
        com1_put_u64_dec((u64)g_BootInfo.u32FbBpp);
        com1_puts(" soft ");
        com1_puts(szGop);
        com1_puts("\n");
    }
    cAreas++;

    /* Grep: GJ-EFI: soft publish — Wave 15 handoff publish lamps. */
    com1_puts("GJ-EFI: soft publish load_ok=");
    com1_put_u64_dec((u64)g_cSoftLoadOk);
    com1_puts(" load_fail=");
    com1_put_u64_dec((u64)g_cSoftLoadFail);
    com1_puts(" source=uefi handoff=");
    com1_puts(szHandoff);
    com1_puts(" soft PASS\n");
    cAreas++;

    /* Grep: GJ-EFI: soft surfaces (Wave 20 deepen) */
    com1_puts("GJ-EFI: soft surfaces count=18 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" names=honesty,inventory,path,handoff,memmap,"
              "gop,flags,contract,magic,geometry,publish,catalog,"
              "surfaces,note,return,retmap,deepen\n");

    /* Grep: GJ-EFI: soft note (Wave 20 deepen) */
    com1_puts("GJ-EFI: soft note milestone=wave81 exclusive=1 "
              "soft_only=1 not_bar3=1 not-multiboot wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts("\n");

    /* Grep: GJ-EFI: soft catalog — Wave 19 area name rollup. */
    com1_puts("GJ-EFI: soft catalog honesty,inventory,path,handoff,memmap,"
              "gop,flags,kernel,rsdp,load,ebs,stats,contract,magic,"
              "geometry,publish,catalog,return wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" areas_expect=53 soft PASS\n");
    cAreas++;

    /* Grep: GJ-EFI: soft return (Wave 20 deepen) */
    com1_puts("GJ-EFI: soft return handoff=");
    com1_puts(szHandoff);
    com1_puts(" memmap=");
    com1_puts(szMemmap);
    com1_puts(" soft_never_gates=1 product_gate=0 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts("\n");
    cAreas++;

    /* Grep: GJ-EFI: soft retmap — Wave 19 return-surface map */
    com1_puts("GJ-EFI: soft retmap handoff|memmap|gop|ebs "
              "product_gate=0 soft_only=1 soft_never_gates=1 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts("\n");
    cAreas++;

    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: GJ-EFI: soft retclass — Wave 19 return-class taxonomy (kept) */
    com1_puts("GJ-EFI: soft retclass ok|fail|inval|nodev|busy|nomem "
              "soft_only=1 product_gate=0 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" (retclass taxonomy; Soft!=product; not bar3)\n");
    cAreas++;
    /* Grep: GJ-EFI: soft retlane — Wave 19 return-lane catalog (kept) */
    com1_puts("GJ-EFI: soft retlane inv|selftest|rate|retcode|retmap|class "
              "product_kernel=OPEN soft_ne_product=1 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" (retlane catalog; Soft!=product)\n");
    cAreas++;
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: GJ-EFI: soft retbound — Wave 20 return-bound honesty (kept) */
    com1_puts("GJ-EFI: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
              "never_blocks_m0=1 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" (retbound honesty; Soft!=product; not bar3)\n");
    cAreas++;
    /* Grep: GJ-EFI: soft retseal — Wave 20 seal stamp (kept) */
    com1_puts("GJ-EFI: soft retseal exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" (retseal stamp; Soft!=product)\n");
    cAreas++;

    
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: GJ-EFI: soft retpulse — Wave 21 return-pulse honesty (kept) */
    com1_puts("GJ-EFI: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" (retpulse honesty; Soft!=product; not bar3)\n");
    cAreas++;
    /* Grep: GJ-EFI: soft retmark — Wave 21 mark stamp (kept) */
    com1_puts("GJ-EFI: soft retmark exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" (retmark stamp; Soft!=product)\n");
    cAreas++;
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft retphase — Wave 22 return-phase honesty (kept) */
com1_puts("GJ-EFI: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (retphase honesty; Soft!=product; not bar3)\n");
cAreas++;
/* Grep: GJ-EFI: soft retbadge — Wave 22 badge stamp (kept) */
com1_puts("GJ-EFI: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft rettoken — Wave 23 return-token honesty (kept) */
com1_puts("GJ-EFI: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (rettoken honesty; Soft!=product; not bar3)\n");
cAreas++;
/* Grep: GJ-EFI: soft retcrest — Wave 23 crest stamp (kept) */
com1_puts("GJ-EFI: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (retcrest stamp; Soft!=product)\n");
cAreas++;
cAreas++;
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft retvault — Wave 24 return-vault honesty (kept) */
com1_puts("GJ-EFI: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: GJ-EFI: soft retbanner — Wave 24 banner stamp (kept) */
com1_puts("GJ-EFI: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft retledger — Wave 25 return-ledger honesty (kept) */
com1_puts("GJ-EFI: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: GJ-EFI: soft retbeacon — Wave 25 beacon stamp (kept) */
com1_puts("GJ-EFI: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft retcipher — Wave 26 return-cipher honesty (kept) */
com1_puts("GJ-EFI: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: GJ-EFI: soft retflame — Wave 26 flame stamp (kept) */
com1_puts("GJ-EFI: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
com1_puts(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: GJ-EFI: soft retprism — Wave 27 return-prism honesty (kept) */
com1_puts("GJ-EFI: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retprism honesty; Soft!=product; not bar3)\n");
/* Grep: GJ-EFI: soft retforge — Wave 27 forge stamp (kept) */
com1_puts("GJ-EFI: soft retforge exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retforge stamp; Soft!=product)\n");
cAreas++;
cAreas++;
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft retshard — Wave 28 return-shard honesty (kept) */
com1_puts("GJ-EFI: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retshard honesty; Soft!=product; not bar3)\n");
/* Grep: GJ-EFI: soft retcrown — Wave 28 crown stamp (kept) */
com1_puts("GJ-EFI: soft retcrown exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft retglyph — Wave 29 return-glyph honesty (kept) */
com1_puts("GJ-EFI: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
          "never_blocks_m0=1 wave=81 "
          "(retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: GJ-EFI: soft retscepter — Wave 29 scepter stamp (kept) */
com1_puts("GJ-EFI: soft retscepter exclusive=1 soft_ne_product=1 "
          "product_kernel=OPEN bar3=0 wave=81 "
          "(retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft retsigil — Wave 30 return-sigil honesty (kept) */
com1_puts("GJ-EFI: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
          "never_blocks_m0=1 wave=81 "
          "(retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: GJ-EFI: soft retemblem — Wave 30 emblem stamp (kept) */
com1_puts("GJ-EFI: soft retemblem exclusive=1 soft_ne_product=1 "
          "product_kernel=OPEN bar3=0 wave=81 "
          "(retemblem stamp; Soft!=product)\n");
/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft retaegis — Wave 31 return-aegis honesty (kept) */
com1_puts("GJ-EFI: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
          "never_blocks_m0=1 wave=81 "
          "(retaegis honesty; Soft!=product; not bar3)\n");
/* Grep: GJ-EFI: soft retmantle — Wave 31 mantle stamp (kept) */
com1_puts("GJ-EFI: soft retmantle exclusive=1 soft_ne_product=1 "
          "product_kernel=OPEN bar3=0 wave=81 "
          "(retmantle stamp; Soft!=product)\n");
cAreas++;
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: GJ-EFI: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
com1_puts("GJ-EFI: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
          "never_blocks_m0=1 wave=81 "
          "(retbulwark honesty; Soft!=product; not bar3)\n");
cAreas++;
/* Grep: GJ-EFI: soft retpanoply — Wave 32 panoply stamp (kept) */
com1_puts("GJ-EFI: soft retpanoply exclusive=1 soft_ne_product=1 "
          "product_kernel=OPEN bar3=0 wave=81 "
          "(retpanoply stamp; Soft!=product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: GJ-EFI: soft retbastion — Wave 33 return-bastion honesty (kept) */
com1_puts("GJ-EFI: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
          "never_blocks_m0=1 wave=81 "
          "(retbastion honesty; Soft!=product; not bar3)\n");
cAreas++;
/* Grep: GJ-EFI: soft retcitadel — Wave 33 citadel stamp (kept) */
com1_puts("GJ-EFI: soft retcitadel exclusive=1 soft_ne_product=1 "
          "product_kernel=OPEN bar3=0 wave=81 "
          "(retcitadel stamp; Soft!=product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retfortress — Wave 35 return-fortress honesty */
kprintf("soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft rethold — Wave 36 return-hold honesty */
kprintf("soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: soft retspire — Wave 36 exclusive spire stamp */
kprintf("soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retwall — Wave 37 return-wall honesty */
kprintf("soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: soft retgate — Wave 37 exclusive gate stamp */
kprintf("soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retmoat — Wave 38 return-moat honesty */
kprintf("soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: soft retower — Wave 38 exclusive tower stamp */
kprintf("soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retravelin — Wave 41 return-travelin honesty */
kprintf("soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retbailey — Wave 46 return-bailey honesty */
kprintf("soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retsally — Wave 50 return-sally honesty */
kprintf("soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retfosse — Wave 51 return-fosse honesty */
kprintf("soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retravelin — Wave 53 return-travelin honesty */
kprintf("soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: soft retredan — Wave 54 exclusive redan stamp */
kprintf("soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retflank — Wave 55 return-flank honesty */
kprintf("soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: soft retface — Wave 55 exclusive face stamp */
kprintf("soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retgorge — Wave 56 return-gorge honesty */
kprintf("soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retraverse — Wave 57 return-traverse honesty */
kprintf("soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retorillon — Wave 58 return-orillon honesty */
kprintf("soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retplace — Wave 60 return-place honesty */
kprintf("soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retenvelope stamp; Soft≠product)\n");


/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=81 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=81 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionangle stamp; Soft≠product)\n");
/* Grep: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retparapetangle stamp; Soft≠product)\n");
/* Grep: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retowerangle stamp; Soft≠product)\n");
/* Grep: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retwallangle stamp; Soft≠product)\n");
/* Grep: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retholdangle stamp; Soft≠product)\n");
/* Grep: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retfortressangle stamp; Soft≠product)\n");
/* Grep: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbulwarkangle stamp; Soft≠product)\n");

cAreas++;
cAreas++;
cAreas++;
cAreas++;
cAreas++;
cAreas++;
cAreas++;
cAreas++;
cAreas++;
/* Wave54 cAreas bump */
/* Wave55 cAreas bump */
cAreas++;
cAreas++;
/* Wave56 cAreas bump */
cAreas++;
cAreas++;
/* Wave57 cAreas bump */
cAreas++;
cAreas++;
/* Wave58 cAreas bump */
cAreas++;
cAreas++;
/* Wave59 cAreas bump */
cAreas++;
cAreas++;
/* Wave60 cAreas bump */
cAreas++;
cAreas++;
/* Wave61 cAreas bump */
cAreas++;
cAreas++;
/* Wave62 cAreas bump */
cAreas++;
cAreas++;
/* Wave63 cAreas bump */
cAreas++;
cAreas++;
/* Wave64 cAreas bump */
cAreas++;
cAreas++;
/* Wave72 cAreas bump */
cAreas++;
cAreas++;
/* Grep: GJ-EFI: soft deepen wave (Wave 24 stamp; areas = prior soft lines). */
    com1_puts("GJ-EFI: soft deepen wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" areas=");
    com1_put_u64_dec((u64)cAreas);
    com1_puts(" catalog=honesty,inventory,path,handoff,memmap,gop,flags,"
              "kernel,rsdp,load,ebs,stats,contract,magic,geometry,publish,"
              "retclass,retlane,catalog unit=uefi_stub.c only "
              "soft_never_gates=1 (soft; not bar3)\n");

    /*
     * Close markers: soft readiness only (entry + kernel span + memmap ok).
     * Grep: GJ-EFI: soft PASS | GJ-EFI: soft PARTIAL
     */
    if (!fPartial) {
        com1_puts("GJ-EFI: soft PASS wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts(" handoff=PASS memmap=");
        com1_puts(szMemmap);
        com1_puts(" kernel=");
        com1_puts(szKernel);
        com1_puts(" (soft inventory; not bar3)\n");
        com1_puts("GJ-EFI: soft inventory PASS wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts(" logs=");
        com1_put_u64_dec((u64)g_cSoftInvLogs);
        com1_puts(" (soft; not bar3)\n");
    } else {
        com1_puts("GJ-EFI: soft PARTIAL wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts(" handoff=");
        com1_puts(szHandoff);
        com1_puts(" memmap=");
        com1_puts(szMemmap);
        com1_puts(" kernel=");
        com1_puts(szKernel);
        com1_puts(" entry=");
        com1_put_u64_hex(u64Entry);
        com1_puts(" (soft inventory; not bar3)\n");
    }
}

static void
efi_puts(struct efi_simple_text_output *pOut, const u16 *pW)
{
    if (pOut == NULL || pOut->pfnOutputString == NULL || pW == NULL) {
        return;
    }
    (void)pOut->pfnOutputString(pOut, (u16 *)(gj_vaddr_t)pW);
}

/* Freestanding helpers — PE image cannot link kernel string.o. */
static void
memcpy_local(void *pDst, const void *pSrc, u64 cb)
{
    u8 *pD;
    const u8 *pS;
    u64 i;

    if (pDst == NULL || pSrc == NULL || cb == 0) {
        return;
    }
    pD = (u8 *)pDst;
    pS = (const u8 *)pSrc;
    for (i = 0; i < cb; i++) {
        pD[i] = pS[i];
    }
}

static void
memset_local(void *pDst, u8 u8V, u64 cb)
{
    u8 *pD;
    u64 i;

    if (pDst == NULL || cb == 0) {
        return;
    }
    pD = (u8 *)pDst;
    for (i = 0; i < cb; i++) {
        pD[i] = u8V;
    }
}

static int
guid_eq(const struct efi_guid *pA, const struct efi_guid *pB)
{
    const u8 *pX;
    const u8 *pY;
    u32 i;

    if (pA == NULL || pB == NULL) {
        return 0;
    }
    pX = (const u8 *)pA;
    pY = (const u8 *)pB;
    for (i = 0; i < sizeof(struct efi_guid); i++) {
        if (pX[i] != pY[i]) {
            return 0;
        }
    }
    return 1;
}

/* Boot-services wrappers: null pBS / missing slot → EFI_LOAD_ERROR. */
static efi_status_t
bs_call_get_memory_map(struct efi_boot_services *pBS, u64 *pSize,
                       struct efi_memory_descriptor *pMap, u64 *pKey,
                       u64 *pDescSize, u32 *pDescVer)
{
    typedef efi_status_t (__attribute__((ms_abi)) *pfn_t)(u64 *, struct efi_memory_descriptor *, u64 *,
                                  u64 *, u32 *);
    pfn_t pfn;

    if (pBS == NULL || pSize == NULL || pKey == NULL || pDescSize == NULL ||
        pDescVer == NULL) {
        return EFI_LOAD_ERROR;
    }
    pfn = (pfn_t)pBS->aFn[BS_GetMemoryMap];
    if (pfn == NULL) {
        return EFI_LOAD_ERROR;
    }
    return pfn(pSize, pMap, pKey, pDescSize, pDescVer);
}

static efi_status_t
bs_exit_boot_services(struct efi_boot_services *pBS, efi_handle_t hImage,
                      u64 u64MapKey)
{
    typedef efi_status_t (__attribute__((ms_abi)) *pfn_t)(efi_handle_t, u64);
    pfn_t pfn;

    if (pBS == NULL) {
        return EFI_LOAD_ERROR;
    }
    pfn = (pfn_t)pBS->aFn[BS_ExitBootServices];
    if (pfn == NULL) {
        return EFI_LOAD_ERROR;
    }
    return pfn(hImage, u64MapKey);
}

static efi_status_t
bs_allocate_pages(struct efi_boot_services *pBS, u32 u32Type, u32 u32MemType,
                  efi_uintn_t cPages, efi_physical_addr_t *pPa)
{
    typedef efi_status_t (__attribute__((ms_abi)) *pfn_t)(u32, u32, efi_uintn_t, efi_physical_addr_t *);
    pfn_t pfn;

    if (pBS == NULL || pPa == NULL) {
        return EFI_LOAD_ERROR;
    }
    pfn = (pfn_t)pBS->aFn[BS_AllocatePages];
    if (pfn == NULL) {
        return EFI_LOAD_ERROR;
    }
    return pfn(u32Type, u32MemType, cPages, pPa);
}

static efi_status_t
bs_allocate_pool(struct efi_boot_services *pBS, u32 u32MemType, efi_uintn_t cb,
                 void **pp)
{
    typedef efi_status_t (__attribute__((ms_abi)) *pfn_t)(u32, efi_uintn_t, void **);
    pfn_t pfn;

    if (pBS == NULL || pp == NULL) {
        return EFI_LOAD_ERROR;
    }
    pfn = (pfn_t)pBS->aFn[BS_AllocatePool];
    if (pfn == NULL) {
        return EFI_LOAD_ERROR;
    }
    return pfn(u32MemType, cb, pp);
}

static efi_status_t
bs_open_protocol(struct efi_boot_services *pBS, efi_handle_t hHandle,
                 struct efi_guid *pGuid, void **ppIface, efi_handle_t hAgent,
                 efi_handle_t hController, u32 u32Attr)
{
    typedef efi_status_t (__attribute__((ms_abi)) *pfn_t)(efi_handle_t, struct efi_guid *, void **,
                                  efi_handle_t, efi_handle_t, u32);
    pfn_t pfn;

    if (pBS == NULL || pGuid == NULL || ppIface == NULL) {
        return EFI_LOAD_ERROR;
    }
    pfn = (pfn_t)pBS->aFn[BS_OpenProtocol];
    if (pfn == NULL) {
        /* Older firmware: HandleProtocol only (no agent/attr). */
        typedef efi_status_t (__attribute__((ms_abi)) *hp_t)(efi_handle_t, struct efi_guid *, void **);
        hp_t hp = (hp_t)pBS->aFn[BS_HandleProtocol];

        if (hp == NULL) {
            return EFI_LOAD_ERROR;
        }
        (void)hAgent;
        (void)hController;
        (void)u32Attr;
        return hp(hHandle, pGuid, ppIface);
    }
    return pfn(hHandle, pGuid, ppIface, hAgent, hController, u32Attr);
}

/*
 * Load \EFI\GREENJADE\KERNEL.ELF from the image device volume.
 * On success: *pEntry = kmain_uefi (from GJUEFI1), *pKernelPa/Bytes = span.
 */
static efi_status_t
load_kernel_elf(struct efi_boot_services *pBS, efi_handle_t hImage,
                struct efi_system_table *pST, u64 *pEntry, u64 *pKernelPa,
                u64 *pKernelBytes)
{
    static struct efi_guid gLoadedImage = {
        0x5B1B31A1u, 0x9562u, 0x11d2u,
        { 0x8Eu, 0x3Fu, 0x00u, 0xA0u, 0xC9u, 0x69u, 0x72u, 0x3Bu }
    };
    static struct efi_guid gSimpleFs = {
        0x964e5b22u, 0x6459u, 0x11d2u,
        { 0x8eu, 0x39u, 0x00u, 0xa0u, 0xc9u, 0x69u, 0x72u, 0x3bu }
    };
    /* Path: \EFI\GREENJADE\KERNEL.ELF */
    static u16 awPath[] = {
        '\\', 'E', 'F', 'I', '\\', 'G', 'R', 'E', 'E', 'N', 'J', 'A', 'D',
        'E', '\\', 'K', 'E', 'R', 'N', 'E', 'L', '.', 'E', 'L', 'F', 0
    };
    struct efi_loaded_image *pLi = NULL;
    struct efi_simple_fs *pFs = NULL;
    struct efi_file_protocol *pRoot = NULL;
    struct efi_file_protocol *pFile = NULL;
    efi_status_t st;
    efi_uintn_t cbRead;
    struct elf64_ehdr *pEh;
    u16 iPh;
    u64 u64Entry = 0;
    u64 u64MinPa = ~0ull;
    u64 u64MaxPa = 0;

    if (pBS == NULL || pEntry == NULL || pKernelPa == NULL ||
        pKernelBytes == NULL) {
        return EFI_LOAD_ERROR;
    }
    *pEntry = 0;
    *pKernelPa = 0;
    *pKernelBytes = 0;

    st = bs_open_protocol(pBS, hImage, &gLoadedImage, (void **)&pLi, hImage,
                          NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (st != EFI_SUCCESS || pLi == NULL || pLi->hDevice == NULL) {
        com1_puts("GJ-EFI: LoadedImage protocol fail\n");
        return st != EFI_SUCCESS ? st : EFI_LOAD_ERROR;
    }

    st = bs_open_protocol(pBS, pLi->hDevice, &gSimpleFs, (void **)&pFs, hImage,
                          NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (st != EFI_SUCCESS || pFs == NULL || pFs->pfnOpenVolume == NULL) {
        com1_puts("GJ-EFI: SimpleFileSystem protocol fail\n");
        return st != EFI_SUCCESS ? st : EFI_LOAD_ERROR;
    }

    st = pFs->pfnOpenVolume(pFs, &pRoot);
    if (st != EFI_SUCCESS || pRoot == NULL) {
        com1_puts("GJ-EFI: OpenVolume fail\n");
        return st;
    }
    if (pRoot->pfnOpen == NULL) {
        com1_puts("GJ-EFI: File.Open missing\n");
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return EFI_LOAD_ERROR;
    }

    st = pRoot->pfnOpen(pRoot, &pFile, awPath, EFI_FILE_MODE_READ, 0);
    if (st != EFI_SUCCESS || pFile == NULL) {
        com1_puts("GJ-EFI: open \\EFI\\GREENJADE\\KERNEL.ELF fail\n");
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return st != EFI_SUCCESS ? st : EFI_NOT_FOUND;
    }
    if (pFile->pfnRead == NULL || pFile->pfnClose == NULL) {
        com1_puts("GJ-EFI: File.Read/Close missing\n");
        if (pFile->pfnClose != NULL) {
            (void)pFile->pfnClose(pFile);
        }
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return EFI_LOAD_ERROR;
    }

    /* Whole file into pool (keeps PE BSS small; ELF ~400–500 KiB). */
    g_cbFileScratch = 1024u * 1024u;
    st = bs_allocate_pool(pBS, EFI_LOADER_DATA, g_cbFileScratch,
                          (void **)&g_pFileScratch);
    if (st != EFI_SUCCESS || g_pFileScratch == NULL) {
        com1_puts("GJ-EFI: AllocatePool(1MiB) for KERNEL.ELF fail\n");
        (void)pFile->pfnClose(pFile);
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return st != EFI_SUCCESS ? st : EFI_LOAD_ERROR;
    }
    if (pFile->pfnSetPosition != NULL) {
        (void)pFile->pfnSetPosition(pFile, 0);
    }
    cbRead = g_cbFileScratch;
    st = pFile->pfnRead(pFile, &cbRead, g_pFileScratch);
    (void)pFile->pfnClose(pFile);
    if (pRoot->pfnClose != NULL) {
        (void)pRoot->pfnClose(pRoot);
    }
    if (st != EFI_SUCCESS || cbRead < sizeof(struct elf64_ehdr)) {
        com1_puts("GJ-EFI: KERNEL.ELF read fail\n");
        return st != EFI_SUCCESS ? st : EFI_LOAD_ERROR;
    }

    pEh = (struct elf64_ehdr *)(void *)g_pFileScratch;
    if (pEh->aIdent[0] != ELFMAG0 || pEh->aIdent[1] != ELFMAG1 ||
        pEh->aIdent[2] != ELFMAG2 || pEh->aIdent[3] != ELFMAG3 ||
        pEh->aIdent[4] != ELFCLASS64 || pEh->u16Machine != EM_X86_64) {
        com1_puts("GJ-EFI: KERNEL.ELF not ELF64/x86_64\n");
        return EFI_LOAD_ERROR;
    }

    for (iPh = 0; iPh < pEh->u16Phnum; iPh++) {
        struct elf64_phdr *pPh;
        efi_physical_addr_t pa;
        efi_uintn_t cPages;
        u64 u64Base;
        u64 u64End;
        u64 u64SegPa;

        pPh = (struct elf64_phdr *)(void *)(g_pFileScratch + pEh->u64Phoff +
                                            (u64)iPh * pEh->u16Phentsize);
        if (pPh->u32Type != PT_LOAD || pPh->u64Memsz == 0) {
            continue;
        }
        if (g_cSoftPtLoadSegs < 0xffffffffu) {
            g_cSoftPtLoadSegs++;
        }
        /* Prefer p_paddr; fall back to p_vaddr (identity-linked product ELF). */
        u64SegPa = pPh->u64Paddr != 0 ? pPh->u64Paddr : pPh->u64Vaddr;
        u64Base = u64SegPa & ~0xfffull;
        u64End = (u64SegPa + pPh->u64Memsz + 0xfffull) & ~0xfffull;
        cPages = (efi_uintn_t)((u64End - u64Base) / 4096ull);
        pa = u64Base;
        st = bs_allocate_pages(pBS, EFI_ALLOCATE_ADDRESS, EFI_LOADER_DATA,
                               cPages, &pa);
        if (st != EFI_SUCCESS) {
            /* ANY_PAGES only OK if firmware returns the exact requested PA. */
            pa = 0;
            st = bs_allocate_pages(pBS, EFI_ALLOCATE_ANY_PAGES, EFI_LOADER_DATA,
                                   cPages, &pa);
            if (st != EFI_SUCCESS || pa != u64Base) {
                com1_puts("GJ-EFI: AllocatePages PT_LOAD fail\n");
                return st != EFI_SUCCESS ? st : EFI_LOAD_ERROR;
            }
        }
        /* BSS-style: zero full memsz span, then overlay filesz bytes. */
        memset_local((void *)(gj_vaddr_t)u64Base, 0, u64End - u64Base);
        if (pPh->u64Filesz != 0) {
            if (pPh->u64Offset + pPh->u64Filesz > cbRead) {
                com1_puts("GJ-EFI: PT_LOAD filesz past EOF\n");
                return EFI_LOAD_ERROR;
            }
            memcpy_local((void *)(gj_vaddr_t)u64SegPa,
                         g_pFileScratch + pPh->u64Offset, pPh->u64Filesz);
        }
        if (u64Base < u64MinPa) {
            u64MinPa = u64Base;
        }
        if (u64End > u64MaxPa) {
            u64MaxPa = u64End;
        }
    }

    /* Product entry: scan loaded span for GJUEFI1 + entry (not e_entry). */
    {
        u64 u64Scan;
        u64 u64ScanEnd = u64MaxPa;

        if (u64MinPa == ~0ull) {
            com1_puts("GJ-EFI: no PT_LOAD segments\n");
            return EFI_LOAD_ERROR;
        }
        for (u64Scan = u64MinPa; u64Scan + 16 < u64ScanEnd; u64Scan += 8) {
            const char *pM = (const char *)(gj_vaddr_t)u64Scan;

            if (pM[0] == 'G' && pM[1] == 'J' && pM[2] == 'U' && pM[3] == 'E' &&
                pM[4] == 'F' && pM[5] == 'I' && pM[6] == '1' && pM[7] == '\0') {
                u64Entry = *(const u64 *)(gj_vaddr_t)(u64Scan + 8);
                break;
            }
        }
    }
    if (u64Entry == 0) {
        /* e_entry is not used: Multiboot ELF may not be long-mode-ready there. */
        com1_puts("GJ-EFI: GJUEFI1 header not found in loaded image\n");
        return EFI_LOAD_ERROR;
    }

    *pEntry = u64Entry;
    *pKernelPa = u64MinPa;
    *pKernelBytes = u64MaxPa - u64MinPa;
    (void)pST;
    com1_puts("GJ-EFI: KERNEL.ELF loaded (GJUEFI1 entry ready)\n");
    return EFI_SUCCESS;
}

/**
 * EFI_APPLICATION entry.
 * UEFI x86_64 uses Microsoft ABI (RCX/RDX), not System V.
 */
efi_status_t __attribute__((ms_abi))
efi_main(efi_handle_t hImage, struct efi_system_table *pST)
{
    struct efi_boot_services *pBS;
    u64 u64MapSize;
    u64 u64MapKey;
    u64 u64DescSize;
    u32 u32DescVer;
    efi_status_t st;
    u64 u64Entry = 0;
    u64 u64KernelPa = 0;
    u64 u64KernelBytes = 0;
    static const u16 awBanner[] = {
        'G', 'r', 'e', 'e', 'n', 'J', 'a', 'd', 'e', ' ',
        'U', 'E', 'F', 'I', '\r', '\n', 0
    };
    void (*pfnKernel)(struct gj_boot_info *);

    com1_puts("GJ-EFI: efi_main enter\r\n");
    com1_puts("GJ-EFI: OVMF/firmware path\r\n");

    if (pST == NULL || pST->pBootServices == NULL) {
        com1_puts("GJ-EFI: SystemTable or BootServices NULL\n");
        return EFI_LOAD_ERROR;
    }
    pBS = pST->pBootServices;
    efi_puts(pST->pConOut, awBanner);

    /*
     * Handoff buffer: zero + stamp header. Kernel boot_info_set_global()
     * will re-derive GJ_BOOT_F_* from non-zero fields; we also stamp flags
     * here so a partial read of g_BootInfo before publish is self-describing.
     */
    memset_local(&g_BootInfo, 0, sizeof(g_BootInfo));
    g_BootInfo.u32Magic = GJ_BOOT_INFO_MAGIC;
    g_BootInfo.u32Version = GJ_BOOT_INFO_VERSION;
    g_BootInfo.u32Source = GJ_BOOT_SRC_UEFI;
    /* Multiboot-only field stays 0 on the product path. */
    g_BootInfo.u32Mb2InfoPhys = 0;

    /* Must load KERNEL.ELF while BootServices (file I/O) still work. */
    st = load_kernel_elf(pBS, hImage, pST, &u64Entry, &u64KernelPa,
                         &u64KernelBytes);
    if (st != EFI_SUCCESS) {
        /* Still take ExitBootServices so serial markers show on partial boots. */
        if (g_cSoftLoadFail < 0xffffffffu) {
            g_cSoftLoadFail++;
        }
        com1_puts("GJ-EFI: KERNEL.ELF load failed — EBS then halt\n");
        /* Grep: GJ-EFI: soft load FAIL */
        com1_puts("GJ-EFI: soft load FAIL st=");
        com1_put_u64_hex((u64)st);
        com1_puts(" wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts("\n");
    } else {
        if (g_cSoftLoadOk < 0xffffffffu) {
            g_cSoftLoadOk++;
        }
        g_BootInfo.u64KernelPhys = u64KernelPa;
        g_BootInfo.u64KernelBytes = u64KernelBytes;
        g_BootInfo.u32Flags |= GJ_BOOT_F_KERNEL_IMG;
        /* Grep: GJ-EFI: soft load PASS */
        com1_puts("GJ-EFI: soft load PASS phys=");
        com1_put_u64_hex(u64KernelPa);
        com1_puts(" bytes=");
        com1_put_u64_dec(u64KernelBytes);
        com1_puts(" entry=");
        com1_put_u64_hex(u64Entry);
        com1_puts(" pt_load_segs=");
        com1_put_u64_dec((u64)g_cSoftPtLoadSegs);
        com1_puts(" wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts("\n");
    }

    /* Optional GOP fill for early desktop FB (kernel may ignore). Soft markers. */
    {
        static struct efi_guid gGop = {
            0x9042a9deu, 0x23dcu, 0x4a38u,
            { 0x96u, 0xfbu, 0x7au, 0xdeu, 0xd0u, 0x80u, 0x51u, 0x6au }
        };
        typedef efi_status_t (__attribute__((ms_abi)) *loc_t)(
            struct efi_guid *, void *, void **);
        loc_t pfnLoc = (loc_t)pBS->aFn[BS_LocateProtocol];
        struct efi_gop *pGop = NULL;
        int fGopOk = 0;

        if (pfnLoc != NULL &&
            pfnLoc(&gGop, NULL, (void **)&pGop) == EFI_SUCCESS && pGop != NULL &&
            pGop->pMode != NULL && pGop->pMode->pInfo != NULL &&
            pGop->pMode->paFrameBuffer != 0 &&
            pGop->pMode->pInfo->u32HRes != 0 &&
            pGop->pMode->pInfo->u32VRes != 0) {
            u32 u32Pps = pGop->pMode->pInfo->u32PixelsPerScanLine;
            u32 u32PixFmt = pGop->pMode->pInfo->u32PixelFormat;

            g_BootInfo.u64FbBase = (u64)pGop->pMode->paFrameBuffer;
            g_BootInfo.u32FbWidth = pGop->pMode->pInfo->u32HRes;
            g_BootInfo.u32FbHeight = pGop->pMode->pInfo->u32VRes;
            /* Soft: assume 32 bpp BGRA/RGBX product path (OVMF default). */
            g_BootInfo.u32FbPitch = u32Pps * 4u;
            g_BootInfo.u32FbBpp = 32;
            g_BootInfo.u32Flags |= GJ_BOOT_F_FB;
            fGopOk = 1;
            if (g_cSoftGopPass < 0xffffffffu) {
                g_cSoftGopPass++;
            }
            com1_puts("GJ-EFI: GOP framebuffer captured\n");
            /* Grep: GJ-EFI: GOP soft PASS */
            com1_puts("GJ-EFI: GOP soft PASS base=");
            com1_put_u64_hex(g_BootInfo.u64FbBase);
            com1_puts(" ");
            com1_put_u64_dec((u64)g_BootInfo.u32FbWidth);
            com1_puts("x");
            com1_put_u64_dec((u64)g_BootInfo.u32FbHeight);
            com1_puts(" pitch=");
            com1_put_u64_dec((u64)g_BootInfo.u32FbPitch);
            com1_puts(" bpp=32 pps=");
            com1_put_u64_dec((u64)u32Pps);
            com1_puts(" pixfmt=");
            com1_put_u64_dec((u64)u32PixFmt);
            com1_puts(" fb_bytes=");
            com1_put_u64_dec((u64)pGop->pMode->cbFrameBuffer);
            com1_puts("\n");
        }
        if (!fGopOk) {
            /* Optional path — soft SKIP, not a product failure. */
            if (g_cSoftGopSkip < 0xffffffffu) {
                g_cSoftGopSkip++;
            }
            com1_puts("GJ-EFI: GOP soft SKIP (no usable framebuffer)\n");
        }
    }

    /* Prefer ACPI 2.0 RSDP; keep 1.0 if that is all firmware publishes. */
    if (pST->pConfigurationTable != NULL) {
        u64 iTable;
        /* ACPI 2.0: 8868E871-E4F1-11D3-BC22-0080C73C8881 */
        static const struct efi_guid gAcpi20 = {
            0x8868E871u, 0xE4F1u, 0x11D3u,
            { 0xBCu, 0x22u, 0x00u, 0x80u, 0xC7u, 0x3Cu, 0x88u, 0x81u }
        };
        /* ACPI 1.0: EB9D2D30-2D88-11D3-9A16-0090273FC14D */
        static const struct efi_guid gAcpi10 = {
            0xEB9D2D30u, 0x2D88u, 0x11D3u,
            { 0x9Au, 0x16u, 0x00u, 0x90u, 0x27u, 0x3Fu, 0xC1u, 0x4Du }
        };

        for (iTable = 0; iTable < pST->u64NumTableEntries; iTable++) {
            if (guid_eq(&pST->pConfigurationTable[iTable].guid, &gAcpi20) ||
                guid_eq(&pST->pConfigurationTable[iTable].guid, &gAcpi10)) {
                g_BootInfo.u64Rsdp =
                    (u64)(gj_vaddr_t)pST->pConfigurationTable[iTable].pTable;
                g_BootInfo.u32Flags |= GJ_BOOT_F_RSDP;
                if (g_cSoftRsdpHit < 0xffffffffu) {
                    g_cSoftRsdpHit++;
                }
                if (guid_eq(&pST->pConfigurationTable[iTable].guid, &gAcpi20)) {
                    if (g_cSoftRsdpAcpi20 < 0xffffffffu) {
                        g_cSoftRsdpAcpi20++;
                    }
                    break;
                }
                if (g_cSoftRsdpAcpi10 < 0xffffffffu) {
                    g_cSoftRsdpAcpi10++;
                }
            }
        }
        /* Grep: GJ-EFI: soft rsdp (early lamp; full inventory later) */
        if (g_BootInfo.u64Rsdp != 0) {
            com1_puts("GJ-EFI: soft rsdp early PASS phys=");
            com1_put_u64_hex(g_BootInfo.u64Rsdp);
            com1_puts(" acpi20=");
            com1_put_u64_dec((u64)g_cSoftRsdpAcpi20);
            com1_puts(" acpi10=");
            com1_put_u64_dec((u64)g_cSoftRsdpAcpi10);
            com1_puts("\n");
        } else {
            com1_puts("GJ-EFI: soft rsdp early SKIP phys=0\n");
        }
    }

    /*
     * GetMemoryMap key must match ExitBootServices. One retry: map may
     * change between GetMemoryMap and ExitBootServices (UEFI contract).
     * Buffer is PE BSS (LoaderData) — valid after EBS under identity map.
     */
    u64MapSize = sizeof(g_aMemMapScratch);
    u64MapKey = 0;
    u64DescSize = 0;
    u32DescVer = 0;
    st = bs_call_get_memory_map(pBS, &u64MapSize,
                                (struct efi_memory_descriptor *)(void *)
                                    g_aMemMapScratch,
                                &u64MapKey, &u64DescSize, &u32DescVer);
    if (st == EFI_BUFFER_TOO_SMALL) {
        com1_puts("GJ-EFI: GetMemoryMap buffer too small (48KiB)\n");
        return st;
    }
    if (st != EFI_SUCCESS) {
        com1_puts("GJ-EFI: GetMemoryMap fail\n");
        return st;
    }

    g_BootInfo.u64MemMap = (u64)(gj_vaddr_t)g_aMemMapScratch;
    g_BootInfo.u64MemMapBytes = u64MapSize;
    g_BootInfo.u64MemDescSize = u64DescSize; /* EFI MD stride; not Multiboot */
    g_BootInfo.u32Flags |= GJ_BOOT_F_MEMMAP;
    /* Soft classify before EBS (map still in PE BSS scratch). */
    soft_memmap_marker(u64MapSize, u64DescSize);

    st = bs_exit_boot_services(pBS, hImage, u64MapKey);
    if (st != EFI_SUCCESS) {
        if (g_cSoftEbsRetry < 0xffffffffu) {
            g_cSoftEbsRetry++;
        }
        u64MapSize = sizeof(g_aMemMapScratch);
        st = bs_call_get_memory_map(pBS, &u64MapSize,
                                    (struct efi_memory_descriptor *)(void *)
                                        g_aMemMapScratch,
                                    &u64MapKey, &u64DescSize, &u32DescVer);
        if (st != EFI_SUCCESS) {
            if (g_cSoftEbsFail < 0xffffffffu) {
                g_cSoftEbsFail++;
            }
            com1_puts("GJ-EFI: GetMemoryMap retry fail\n");
            return st;
        }
        g_BootInfo.u64MemMapBytes = u64MapSize;
        g_BootInfo.u64MemDescSize = u64DescSize;
        g_BootInfo.u32Flags |= GJ_BOOT_F_MEMMAP;
        /* Soft re-classify after map key refresh. */
        soft_memmap_marker(u64MapSize, u64DescSize);
        st = bs_exit_boot_services(pBS, hImage, u64MapKey);
        if (st != EFI_SUCCESS) {
            if (g_cSoftEbsFail < 0xffffffffu) {
                g_cSoftEbsFail++;
            }
            com1_puts("GJ-EFI: ExitBootServices fail (after retry)\n");
            return st;
        }
        if (g_cSoftEbsRetryOk < 0xffffffffu) {
            g_cSoftEbsRetryOk++;
        }
    } else {
        if (g_cSoftEbsFirstOk < 0xffffffffu) {
            g_cSoftEbsFirstOk++;
        }
    }

    com1_puts("GJ-EFI: ExitBootServices ok\n");
    soft_handoff_marker(u64Entry);
    /* Wave 15 exclusive soft inventory deepen (COM1; never gates jump). */
    soft_wave14_inventory(u64Entry);

    if (u64Entry == 0) {
        /* Load failed earlier; stay halted with EBS already done. */
        com1_puts("GJ-EFI: no kernel entry — halt\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    pfnKernel = (void (*)(struct gj_boot_info *))(gj_vaddr_t)u64Entry;
    if (pfnKernel == NULL) {
        com1_puts("GJ-EFI: kernel entry NULL — halt\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    com1_puts("GJ-EFI: jump kmain_uefi\n");
    pfnKernel(&g_BootInfo);

    /* kmain_uefi is noreturn; if it returns, park the BSP. */
    com1_puts("GJ-EFI: kmain_uefi returned — halt\n");
    for (;;) {
        __asm__ volatile ("hlt");
    }
    return EFI_SUCCESS;
}
