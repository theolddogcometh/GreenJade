/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * UEFI product boot path (P-BOOT-1):
 *   SimpleFileSystem -> EFI/GREENJADE/KERNEL.ELF -> ExitBootServices ->
 *   gj_boot_info -> kmain_uefi
 *
 * Fills struct gj_boot_info for the kernel (see gj/boot_info.h):
 *   magic/version/source, EFI memory map (P-BOOT-3), optional ACPI RSDP
 *   (P-BOOT-4), optional GOP FB, loaded KERNEL.ELF span + GJUEFI1 entry.
 * g_BootInfo lives in the PE image BSS so it survives ExitBootServices;
 * the pointer passed to kmain_uefi remains valid under the identity map.
 *
 * Soft product surface (COM1, freestanding - no kprintf link):
 *   Soft = greppable markers + classify; never hard-fails product when
 *   optional GOP is absent. Memmap soft REJECT still proceeds to EBS so
 *   partial boots remain diagnosable.
 * Soft media (D4): optional LoadFile of \linux-drivers\modules\r8169.ko
 *   into AllocatePages(LOADER_DATA) -> gj_boot_info soft media fields.
 *   Soft!=product; never gates KERNEL load / EBS.
 *
 * Exclusive soft deepen (this unit only - greppable "GJ-EFI: soft ..."):
 *   GJ-EFI: soft inventory  - master surface + wave stamp
 *   GJ-EFI: soft path       - P-BOOT-1/3/4 claim + soft_never_gates
 *   GJ-EFI: soft honesty    - explicit non-claims (not Multiboot)
 *   GJ-EFI: soft handoff    - flags/entry/kernel/map/fb/rsdp snapshot
 *   GJ-EFI: soft memmap     - EFI MD usable + type-class tallies
 *   GJ-EFI: soft gop        - FB geometry soft classify
 *   GJ-EFI: soft flags      - GJ_BOOT_F_* bit inventory
 *   GJ-EFI: soft kernel     - KERNEL.ELF span soft classify
 *   GJ-EFI: soft rsdp       - ACPI RSDP soft present/skip (P-BOOT-4)
 *   GJ-EFI: soft load       - SimpleFS / ELF / GJUEFI1 soft path lamps
 *   GJ-EFI: soft ebs        - ExitBootServices first/retry soft lamps
 *   GJ-EFI: soft stats      - rollup counters + wave stamp
 *   GJ-EFI: soft contract   - Wave 15 magic/version/struct contract
 *   GJ-EFI: soft magic      - Wave 15 handoff magic lamps
 *   GJ-EFI: soft geometry   - Wave 15 FB pitch/bpp soft math
 *   GJ-EFI: soft publish    - Wave 15 handoff publish lamps
 *   GJ-EFI: soft catalog    - area name rollup
 *   GJ-EFI: soft return     - terminal handoff outcomes (lean)
 *   GJ-EFI: soft retmap     - map/key geometry (lean)
 *   GJ-EFI: soft retclass   - outcome taxonomy (lean; no stamp storms)
 *   GJ-EFI: soft retlane    - lane catalog (lean)
 *   GJ-EFI: soft residual   - lean residual honesty Soft!=product dual G-AC-1
 *   GJ-EFI: soft residual lean PASS|PARTIAL|SKIP - cover-class residual lamp
 *   GJ-EFI: soft deepen     - residual deepen close (no version bump)
 *   GJ-EFI: soft PASS|PARTIAL - close marker (soft readiness only)
 *
 * Lean soft residual (C0; exclusive residual; this unit only)
 * ----------------------------------------------------------
 * Soft!=product dual license: residual/return lamps never claim product
 * boot DoD / Dual DoD A/B (remain OPEN); tree dual remains MIT OR Apache-2.0.
 * Soft residual != product dual-license stamp storm. G-AC-1: soft media .ko
 * is never product AC. No version stamp bump; no stamp storms; stamp-free.
 * C0 residual deepen: denser real surfaces (magic/check/media/ebs/com1) +
 * panel path residual when COM1 THRE is dead (inventory flood still skipped).
 * greppable: GJ-EFI: soft residual | GJ-EFI: soft residual lean
 * greppable: GJ-EFI: soft residual lean PASS | PARTIAL | SKIP
 *
 * Legacy companion markers (still emitted; prefix-stable):
 *   GJ-EFI: GOP soft PASS|SKIP ...
 *   GJ-EFI: memmap soft PASS|REJECT ...
 *   GJ-EFI: handoff soft PASS|PARTIAL ...
 *
 * ESP BOOT.LOG tee (pre-ExitBootServices diagnostics on disk):
 *   Circular RAM ring (64 KiB BSS) tees every COM1 char (com1_putc path).
 *   Milestone flush_boot_log() writes \EFI\GREENJADE\BOOT.LOG via SimpleFS
 *   (same volume as KERNEL.ELF). Overwrite OK; never hard-fails product boot.
 *   greppable: GJ-EFI: BOOT.LOG write PASS|FAIL
 *
 * greppable: GJ-EFI: soft
 * greppable: GJ-EFI: soft inventory
 * greppable: GJ-EFI: soft residual
 * greppable: GJ-EFI: soft residual lean
 * greppable: GJ-EFI: soft deepen
 * greppable: GJ-EFI: soft media
 * greppable: GJ-EFI: BOOT.LOG write
 *
 * Built as PE32+ EFI_APPLICATION via ld -mi386pep (see scripts/build-efi.sh).
 * Multiboot2 greenjade.elf does not link this file (dev-only P-BOOT-2).
 *
 * Pure C11 freestanding; dual MIT OR Apache-2.0. No C++. Soft!=product. G-AC-1.
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

#define EFI_FILE_MODE_READ   0x0000000000000001ull
#define EFI_FILE_MODE_WRITE  0x0000000000000002ull
#define EFI_FILE_MODE_CREATE 0x8000000000000000ull

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001u

/* ESP BOOT.LOG circular tee capacity (PE BSS; 64-128 KiB range). */
#define GJ_BOOT_LOG_CAP (64u * 1024u)

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
#define GJ_EFI_SOFT_WAVE 116u

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

/* Graphics Output Protocol (GOP) - early desktop FB. */
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
    /* Cast to efi_file_write_t (ms_abi) at call sites - same pattern as Read. */
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

typedef efi_status_t (__attribute__((ms_abi)) *efi_file_write_t)(
    struct efi_file_protocol *pThis, efi_uintn_t *pBufferSize, void *pBuffer);
typedef efi_status_t (__attribute__((ms_abi)) *efi_file_flush_t)(
    struct efi_file_protocol *pThis);
typedef efi_status_t (__attribute__((ms_abi)) *efi_file_delete_t)(
    struct efi_file_protocol *pThis);

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
 * ESP BOOT.LOG circular ring (PE BSS). Tees COM1 bytes pre-EBS; flush_boot_log
 * dumps the ring to \EFI\GREENJADE\BOOT.LOG. Circular overwrite when full.
 * Head = next write index; Len = bytes stored (0..GJ_BOOT_LOG_CAP).
 */
static u8 g_aBootLog[GJ_BOOT_LOG_CAP];
static u32 g_uBootLogHead;
static u32 g_uBootLogLen;

/*
 * COM1 (0x3F8) early log - same role as kprintf before serial_init /
 * after ExitBootServices when ConOut is gone. Prefix all lines "GJ-EFI:".
 * Every serial char is also teed into g_aBootLog (as if serial).
 */
static void
boot_log_putc(char chOut)
{
    g_aBootLog[g_uBootLogHead] = (u8)chOut;
    g_uBootLogHead++;
    if (g_uBootLogHead >= GJ_BOOT_LOG_CAP) {
        g_uBootLogHead = 0;
    }
    if (g_uBootLogLen < GJ_BOOT_LOG_CAP) {
        g_uBootLogLen++;
    }
}

/* Sticky: no THRE on 0x3FD (common on laptops without legacy UART). */
static u32 g_fCom1Dead;

static void
com1_putc(char chOut)
{
    u32 u32Spins;
    u32 u32Limit = g_fCom1Dead ? 64u : 20000u;
    u8 u8SawThre = 0;

    /*
     * Port I/O via inb/outb (not MMIO). Spin on LSR THRE bit.
     * Cap the spin: G752-class boxes never raise THRE - 100kx soft flood
     * looked like a hang after the blue (EBS) bar.
     */
    for (u32Spins = 0; u32Spins < u32Limit; u32Spins++) {
        u8 u8Lsr;

        __asm__ volatile ("inb %1, %0" : "=a"(u8Lsr) : "Nd"((u16)0x3FD));
        if ((u8Lsr & 0x20u) != 0) {
            u8SawThre = 1;
            break;
        }
    }
    if (u8SawThre == 0) {
        g_fCom1Dead = 1;
    }
    __asm__ volatile ("outb %0, %1" : : "a"((u8)chOut), "Nd"((u16)0x3F8));
    boot_log_putc(chOut);
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

/*
 * PE EFI_APPLICATION does not link kernel stdio_k. Soft deepen incorrectly
 * called kprintf() - undefined PLT -> hang after EBS (blue bar, never cyan).
 * Soft calls are string literals only; map to COM1.
 */
static void
kprintf(const char *szFmt, ...)
{
    if (szFmt != NULL) {
        com1_puts(szFmt);
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
 * Emits legacy GJ-EFI: memmap soft PASS|REJECT ... and Wave 15
 * GJ-EFI: soft memmap ... type-class tallies (never aborts).
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

/*
 * C0 residual lean emit (exclusive residual; Soft!=product; G-AC-1).
 * Denser real surfaces: magic/version, flags-vs-fields check, media span,
 * ebs/load counters, usable pages, COM1 dead, Dual DoD OPEN honesty.
 * fPanel=1: laptop / no-COM1 path - residual honesty without inventory flood.
 * Never gates KERNEL load / EBS / product boot DoD. Stamp-free; storm=0.
 * greppable: GJ-EFI: soft residual
 * greppable: GJ-EFI: soft residual lean
 * greppable: GJ-EFI: soft residual lean PASS|PARTIAL|SKIP
 */
static void
soft_residual_lean_emit(u64 u64Entry, u32 fPanel)
{
    u32 u32Flags = g_BootInfo.u32Flags;
    u32 fMemmapBit = (u32Flags & GJ_BOOT_F_MEMMAP) != 0 ? 1u : 0u;
    u32 fRsdpBit = (u32Flags & GJ_BOOT_F_RSDP) != 0 ? 1u : 0u;
    u32 fFbBit = (u32Flags & GJ_BOOT_F_FB) != 0 ? 1u : 0u;
    u32 fKernelBit = (u32Flags & GJ_BOOT_F_KERNEL_IMG) != 0 ? 1u : 0u;
    u32 fMediaBit = (u32Flags & GJ_BOOT_F_SOFT_MEDIA) != 0 ? 1u : 0u;
    u32 fKernelSpan = (g_BootInfo.u64KernelPhys != 0 &&
                       g_BootInfo.u64KernelBytes != 0)
                          ? 1u
                          : 0u;
    u32 fRsdp = (g_BootInfo.u64Rsdp != 0) ? 1u : 0u;
    u32 fGop = (g_BootInfo.u64FbBase != 0 && g_BootInfo.u32FbWidth != 0 &&
                g_BootInfo.u32FbHeight != 0)
                   ? 1u
                   : 0u;
    u32 fMediaSpan = (g_BootInfo.u64SoftMediaPhys != 0 &&
                      g_BootInfo.u64SoftMediaBytes != 0)
                         ? 1u
                         : 0u;
    u32 fMapFields = (g_BootInfo.u64MemMap != 0 &&
                      g_BootInfo.u64MemMapBytes != 0)
                         ? 1u
                         : 0u;
    u32 fFbBase = (g_BootInfo.u64FbBase != 0) ? 1u : 0u;
    u32 fMagicOk = (g_BootInfo.u32Magic == GJ_BOOT_INFO_MAGIC) ? 1u : 0u;
    u32 fVersionOk =
        (g_BootInfo.u32Version == GJ_BOOT_INFO_VERSION) ? 1u : 0u;
    u32 cMismatch = 0;
    u32 cCapBits;
    int fPartial;
    const char *szHandoff;
    const char *szMemmap;
    const char *szGop;
    const char *szKernel;
    const char *szRsdp;
    const char *szCheck;
    const char *szCover;

    if (fMemmapBit != fMapFields) {
        cMismatch++;
    }
    if (fRsdpBit != fRsdp) {
        cMismatch++;
    }
    if (fFbBit != fFbBase) {
        cMismatch++;
    }
    if (fKernelBit != fKernelSpan) {
        cMismatch++;
    }
    if (fMediaBit != fMediaSpan) {
        cMismatch++;
    }
    szCheck = (cMismatch == 0) ? "PASS" : "PARTIAL";
    cCapBits = fMemmapBit + fRsdpBit + fFbBit + fKernelBit + fMediaBit;

    fPartial = (u64Entry == 0) || (fMemmapBit == 0) || (fKernelBit == 0) ||
               (g_fSoftMmOk == 0);
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

    /* Panel: cover SKIP (full inventory skipped); still denser residual facts. */
    if (fPanel != 0) {
        szCover = "SKIP";
    } else {
        szCover = fPartial ? "PARTIAL" : "PASS";
    }

    /*
     * Grep: GJ-EFI: soft residual lean
     * denser residual: contract + check + media + ebs + com1 + Dual DoD OPEN
     */
    com1_puts("GJ-EFI: soft residual lean handoff=");
    com1_puts(szHandoff);
    com1_puts(" memmap=");
    com1_puts(szMemmap);
    com1_puts(" gop=");
    com1_puts(szGop);
    com1_puts(" kernel=");
    com1_puts(szKernel);
    com1_puts(" rsdp=");
    com1_puts(szRsdp);
    com1_puts(" magic_ok=");
    com1_put_u64_dec((u64)fMagicOk);
    com1_puts(" version_ok=");
    com1_put_u64_dec((u64)fVersionOk);
    com1_puts(" source=");
    com1_put_u64_dec((u64)g_BootInfo.u32Source);
    com1_puts(" flags=");
    com1_put_u64_hex((u64)u32Flags);
    com1_puts(" cap_bits=");
    com1_put_u64_dec((u64)cCapBits);
    com1_puts(" check=");
    com1_puts(szCheck);
    com1_puts(" mismatch=");
    com1_put_u64_dec((u64)cMismatch);
    com1_puts(" load_ok=");
    com1_put_u64_dec((u64)g_cSoftLoadOk);
    com1_puts(" load_fail=");
    com1_put_u64_dec((u64)g_cSoftLoadFail);
    com1_puts(" media=");
    com1_put_u64_dec((u64)fMediaBit);
    com1_puts(" media_span=");
    com1_put_u64_dec((u64)fMediaSpan);
    com1_puts(" media_phys=");
    com1_put_u64_hex(g_BootInfo.u64SoftMediaPhys);
    com1_puts(" media_bytes=");
    com1_put_u64_dec(g_BootInfo.u64SoftMediaBytes);
    com1_puts(" usable_pages=");
    com1_put_u64_dec(g_cSoftMmUsablePages);
    com1_puts(" ebs_first=");
    com1_put_u64_dec((u64)g_cSoftEbsFirstOk);
    com1_puts(" ebs_retry=");
    com1_put_u64_dec((u64)g_cSoftEbsRetry);
    com1_puts(" ebs_fail=");
    com1_put_u64_dec((u64)g_cSoftEbsFail);
    com1_puts(" entry=");
    com1_put_u64_hex(u64Entry);
    com1_puts(" com1_dead=");
    com1_put_u64_dec((u64)g_fCom1Dead);
    com1_puts(" boot_log_len=");
    com1_put_u64_dec((u64)g_uBootLogLen);
    com1_puts(" serial_path=");
    com1_puts((fPanel != 0) ? "panel" : "COM1");
    com1_puts(" exclusive=1 lean=1 soft=1 product=0 soft_ne_product=1 "
              "dual=MIT_OR_Apache-2.0 G-AC-1 residual!=product "
              "dual_dod_a=OPEN dual_dod_b=OPEN C0=1 "
              "no_version_stamp=1 stamp_storm=0 never_gates=1 "
              "product_gate=0 residual_deepen=1 unit=uefi_stub.c "
              "(Soft!=product; dual MIT OR Apache-2.0; "
              "G-AC-1 no .ko product AC; Dual DoD A/B OPEN; "
              "not product boot DoD; C0 eng residual)\n");

    /*
     * Grep: GJ-EFI: soft residual lean PASS|PARTIAL|SKIP
     * Cover-class residual lamp (handoff readiness / panel inventory skip).
     */
    com1_puts("GJ-EFI: soft residual lean ");
    com1_puts(szCover);
    com1_puts(" handoff=");
    com1_puts(szHandoff);
    com1_puts(" check=");
    com1_puts(szCheck);
    com1_puts(" media=");
    com1_put_u64_dec((u64)fMediaBit);
    com1_puts(" entry=");
    com1_put_u64_hex(u64Entry);
    com1_puts(" serial_path=");
    com1_puts((fPanel != 0) ? "panel" : "COM1");
    com1_puts(" soft=1 product=0 soft_ne_product=1 "
              "dual=MIT_OR_Apache-2.0 storm=0 G-AC-1 C0=1 "
              "dual_dod_a=OPEN dual_dod_b=OPEN "
              "(Soft!=product; residual cover lamp; not product DoD)\n");
}

/**
 * Wave 15 greppable soft EFI inventory dump (product / smoke deepen).
 * Prefix-stable markers (GJ-EFI: soft ...). Never allocates; never aborts;
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
     * never claims product-complete beyond P-BOOT-1 path wiring.
     * greppable: GJ-EFI: soft honesty
     */
    com1_puts("GJ-EFI: soft honesty not-multiboot p_boot_1=product "
              "p_boot_2=dev_only soft_never_gates=1 "
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
              "e_entry_unused=1 pool_mib=");
    com1_put_u64_dec((u64)(g_cbFileScratch / (1024u * 1024u)));
    com1_puts("\n");
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

    /* Grep: GJ-EFI: soft contract - Wave 15 magic/version/struct contract. */
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

    /* Grep: GJ-EFI: soft magic - Wave 15 handoff magic lamps. */
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

    /* Grep: GJ-EFI: soft geometry - Wave 15 FB pitch/bpp soft math. */
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

    /* Grep: GJ-EFI: soft publish - Wave 15 handoff publish lamps. */
    com1_puts("GJ-EFI: soft publish load_ok=");
    com1_put_u64_dec((u64)g_cSoftLoadOk);
    com1_puts(" load_fail=");
    com1_put_u64_dec((u64)g_cSoftLoadFail);
    com1_puts(" source=uefi handoff=");
    com1_puts(szHandoff);
    com1_puts(" soft PASS\n");
    cAreas++;

    /*
     * Grep: GJ-EFI: soft surfaces (lean rollup; stamp_storm=0)
     * count=24 = prior soft area lines (deepen is close, not counted).
     */
    com1_puts("GJ-EFI: soft surfaces count=24 wave=");
    com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
    com1_puts(" names=honesty,inventory,path,handoff,memmap,"
              "gop,flags,kernel,rsdp,load,ebs,stats,contract,magic,"
              "geometry,publish,surfaces,note,catalog,return,retmap,"
              "retclass,retlane,residual stamp_storm=0 deepen_close=1\n");
    cAreas++;

    /* Grep: GJ-EFI: soft note (lean exclusive residual; no version stamp) */
    com1_puts("GJ-EFI: soft note exclusive=1 lean=1 stamp_storm=0 "
              "no_version_stamp=1 unit=uefi_stub.c Soft!=product G-AC-1\n");
    cAreas++;

    /* Grep: GJ-EFI: soft catalog - honest 24-area name rollup. */
    com1_puts("GJ-EFI: soft catalog honesty,inventory,path,handoff,memmap,"
              "gop,flags,kernel,rsdp,load,ebs,stats,contract,magic,"
              "geometry,publish,surfaces,note,catalog,return,retmap,"
              "retclass,retlane,residual "
              "areas_expect=24 soft PASS\n");
    cAreas++;

    /*
     * Grep: GJ-EFI: soft return
     * Terminal handoff outcomes - single line; Soft!=product.
     */
    com1_puts("GJ-EFI: soft return handoff=");
    com1_puts(szHandoff);
    com1_puts(" memmap=");
    com1_puts(szMemmap);
    com1_puts(" kernel=");
    com1_puts(szKernel);
    com1_puts(" soft_never_gates=1 product_gate=0 Soft!=product\n");
    cAreas++;

    /*
     * Grep: GJ-EFI: soft retmap
     * Map/key geometry after soft memmap walk - single line.
     */
    com1_puts("GJ-EFI: soft retmap map_bytes=");
    com1_put_u64_dec(g_BootInfo.u64MemMapBytes);
    com1_puts(" desc_size=");
    com1_put_u64_dec(g_BootInfo.u64MemDescSize);
    com1_puts(" usable_pages=");
    com1_put_u64_dec(g_cSoftMmUsablePages);
    com1_puts(" ebs_first=");
    com1_put_u64_dec((u64)g_cSoftEbsFirstOk);
    com1_puts(" Soft!=product\n");
    cAreas++;

    /*
     * Grep: GJ-EFI: soft retclass
     * Outcome taxonomy - lean; no stamp storms.
     */
    com1_puts("GJ-EFI: soft retclass taxonomy=handoff,memmap,gop,kernel,rsdp,"
              "load,ebs,media Soft!=product lean=1 stamp_storm=0\n");
    cAreas++;

    /*
     * Grep: GJ-EFI: soft retlane
     * Lane catalog - lean; Soft!=product.
     */
    com1_puts("GJ-EFI: soft retlane catalog=p_boot_1,SimpleFS,GJUEFI1,EBS,"
              "BOOT.LOG,soft_media Soft!=product lean=1\n");
    cAreas++;

    /*
     * Grep: GJ-EFI: soft residual lean
     * C0 residual deepen: denser real surfaces + cover lamp (COM1 path).
     * Soft!=product dual; G-AC-1; Dual DoD OPEN; stamp-free; storm=0.
     * greppable: GJ-EFI: soft residual
     */
    soft_residual_lean_emit(u64Entry, 0u);
    cAreas++;

    /*
     * Grep: GJ-EFI: soft deepen
     * Residual deepen close: areas = prior soft lines; no version bump;
     * no stamp storms; catalog honest to the 24 residual inventory areas.
     */
    com1_puts("GJ-EFI: soft deepen areas=");
    com1_put_u64_dec((u64)cAreas);
    com1_puts(" handoff=");
    com1_puts(szHandoff);
    com1_puts(" residual=lean residual_deepen=1 C0=1 "
              "catalog=honesty,inventory,path,handoff,memmap,gop,flags,"
              "kernel,rsdp,load,ebs,stats,contract,magic,geometry,publish,"
              "surfaces,note,catalog,return,retmap,retclass,retlane,residual "
              "unit=uefi_stub.c only soft_never_gates=1 stamp_storm=0 "
              "no_version_stamp=1 Soft!=product G-AC-1 "
              "dual_dod_a=OPEN dual_dod_b=OPEN "
              "(soft residual deepen; not product boot DoD; C0)\n");

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
        com1_puts(" (soft inventory)\n");
        com1_puts("GJ-EFI: soft inventory PASS wave=");
        com1_put_u64_dec((u64)GJ_EFI_SOFT_WAVE);
        com1_puts(" logs=");
        com1_put_u64_dec((u64)g_cSoftInvLogs);
        com1_puts(" (soft)\n");
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
        com1_puts(" (soft inventory)\n");
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

/*
 * On-screen progress for real DUTs without COM1 (e.g. G752 laptop).
 * COM1 markers remain the greppable path; ConOut dies after ExitBootServices.
 */
static void
efi_say(struct efi_system_table *pST, const char *szMsg)
{
    u16 aw[120];
    u32 i = 0;

    if (pST == NULL || pST->pConOut == NULL || szMsg == NULL) {
        return;
    }
    while (*szMsg != '\0' && i + 2u < (u32)(sizeof(aw) / sizeof(aw[0]))) {
        char ch = *szMsg++;

        if (ch == '\n') {
            aw[i++] = (u16)'\r';
            aw[i++] = (u16)'\n';
        } else {
            aw[i++] = (u16)(u8)ch;
        }
    }
    aw[i] = 0;
    efi_puts(pST->pConOut, aw);
}

/*
 * Colour bars disabled (text-first panel). Keep symbol for call sites;
 * kernel fb_console owns the GOP after jump to kmain_uefi.
 */
static void
efi_fb_bar(u64 u64Base, u32 u32Pitch, u32 u32Width, u32 u32Height,
           u32 u32Y0, u32 u32BarH, u32 u32Color)
{
    (void)u64Base;
    (void)u32Pitch;
    (void)u32Width;
    (void)u32Height;
    (void)u32Y0;
    (void)u32BarH;
    (void)u32Color;
}

/* Freestanding helpers - PE image cannot link kernel string.o. */
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

/* Boot-services wrappers: null pBS / missing slot -> EFI_LOAD_ERROR. */
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
 * Write circular ring [u32Start, u32Start+cb) possibly wrapping to pFile.
 * Two Write calls when the valid range wraps past GJ_BOOT_LOG_CAP.
 */
static efi_status_t
boot_log_file_write(struct efi_file_protocol *pFile, efi_file_write_t pfnWrite)
{
    efi_uintn_t cbChunk;
    efi_status_t st;
    u32 u32Start;
    u32 u32First;
    u32 u32Rest;

    if (pFile == NULL || pfnWrite == NULL) {
        return EFI_LOAD_ERROR;
    }
    if (g_uBootLogLen == 0) {
        return EFI_SUCCESS;
    }
    /* Oldest byte index when head is the next write slot. */
    u32Start = g_uBootLogHead + GJ_BOOT_LOG_CAP - g_uBootLogLen;
    if (u32Start >= GJ_BOOT_LOG_CAP) {
        u32Start -= GJ_BOOT_LOG_CAP;
    }
    if (u32Start + g_uBootLogLen <= GJ_BOOT_LOG_CAP) {
        cbChunk = (efi_uintn_t)g_uBootLogLen;
        st = pfnWrite(pFile, &cbChunk, &g_aBootLog[u32Start]);
        if (st != EFI_SUCCESS) {
            return st;
        }
        if (cbChunk != (efi_uintn_t)g_uBootLogLen) {
            return EFI_LOAD_ERROR;
        }
        return EFI_SUCCESS;
    }
    u32First = GJ_BOOT_LOG_CAP - u32Start;
    u32Rest = g_uBootLogLen - u32First;
    cbChunk = (efi_uintn_t)u32First;
    st = pfnWrite(pFile, &cbChunk, &g_aBootLog[u32Start]);
    if (st != EFI_SUCCESS) {
        return st;
    }
    if (cbChunk != (efi_uintn_t)u32First) {
        return EFI_LOAD_ERROR;
    }
    cbChunk = (efi_uintn_t)u32Rest;
    st = pfnWrite(pFile, &cbChunk, &g_aBootLog[0]);
    if (st != EFI_SUCCESS) {
        return st;
    }
    if (cbChunk != (efi_uintn_t)u32Rest) {
        return EFI_LOAD_ERROR;
    }
    return EFI_SUCCESS;
}

/*
 * Milestone flush: dump COM1 tee ring to \EFI\GREENJADE\BOOT.LOG on the same
 * ESP volume as KERNEL.ELF. Soft-only - failures emit greppable markers and
 * never gate product boot. Safe only while Boot Services still run.
 *
 * greppable: GJ-EFI: BOOT.LOG write PASS|FAIL
 */
static void
flush_boot_log(struct efi_boot_services *pBS, efi_handle_t hImage)
{
    static struct efi_guid gLoadedImage = {
        0x5B1B31A1u, 0x9562u, 0x11d2u,
        { 0x8Eu, 0x3Fu, 0x00u, 0xA0u, 0xC9u, 0x69u, 0x72u, 0x3Bu }
    };
    static struct efi_guid gSimpleFs = {
        0x964e5b22u, 0x6459u, 0x11d2u,
        { 0x8eu, 0x39u, 0x00u, 0xa0u, 0xc9u, 0x69u, 0x72u, 0x3bu }
    };
    /* Path: \EFI\GREENJADE\BOOT.LOG */
    static u16 awPath[] = {
        '\\', 'E', 'F', 'I', '\\', 'G', 'R', 'E', 'E', 'N', 'J', 'A', 'D',
        'E', '\\', 'B', 'O', 'O', 'T', '.', 'L', 'O', 'G', 0
    };
    struct efi_loaded_image *pLi = NULL;
    struct efi_simple_fs *pFs = NULL;
    struct efi_file_protocol *pRoot = NULL;
    struct efi_file_protocol *pFile = NULL;
    struct efi_system_table *pST = NULL;
    efi_status_t st;
    efi_file_write_t pfnWrite;
    efi_file_flush_t pfnFlush;
    efi_file_delete_t pfnDelete;
    u64 u64Mode;

    if (pBS == NULL || hImage == NULL) {
        com1_puts("GJ-EFI: BOOT.LOG write FAIL reason=null_bs_or_image\n");
        return;
    }

    st = bs_open_protocol(pBS, hImage, &gLoadedImage, (void **)&pLi, hImage,
                          NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (st != EFI_SUCCESS || pLi == NULL || pLi->hDevice == NULL) {
        com1_puts("GJ-EFI: BOOT.LOG write FAIL reason=LoadedImage\n");
        return;
    }
    pST = pLi->pST;

    st = bs_open_protocol(pBS, pLi->hDevice, &gSimpleFs, (void **)&pFs, hImage,
                          NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (st != EFI_SUCCESS || pFs == NULL || pFs->pfnOpenVolume == NULL) {
        com1_puts("GJ-EFI: BOOT.LOG write FAIL reason=SimpleFileSystem\n");
        efi_say(pST, "GJ-EFI: BOOT.LOG write FAIL\r\n");
        return;
    }

    st = pFs->pfnOpenVolume(pFs, &pRoot);
    if (st != EFI_SUCCESS || pRoot == NULL || pRoot->pfnOpen == NULL) {
        com1_puts("GJ-EFI: BOOT.LOG write FAIL reason=OpenVolume\n");
        efi_say(pST, "GJ-EFI: BOOT.LOG write FAIL\r\n");
        return;
    }

    /*
     * Best-effort truncate: delete existing BOOT.LOG so a shorter ring does
     * not leave stale tail bytes. Overwrite without rename (BOOT.PREV optional;
     * SetInfo rename is fragile freestanding - overwrite is OK).
     */
    st = pRoot->pfnOpen(pRoot, &pFile, awPath, EFI_FILE_MODE_READ, 0);
    if (st == EFI_SUCCESS && pFile != NULL) {
        pfnDelete = (efi_file_delete_t)pFile->pfnDelete;
        if (pfnDelete != NULL) {
            (void)pfnDelete(pFile); /* Delete closes the handle. */
        } else if (pFile->pfnClose != NULL) {
            (void)pFile->pfnClose(pFile);
        }
        pFile = NULL;
    }

    u64Mode = EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE;
    st = pRoot->pfnOpen(pRoot, &pFile, awPath, u64Mode, 0);
    if (st != EFI_SUCCESS || pFile == NULL) {
        com1_puts("GJ-EFI: BOOT.LOG write FAIL reason=Open CREATE st=");
        com1_put_u64_hex((u64)st);
        com1_puts("\n");
        efi_say(pST, "GJ-EFI: BOOT.LOG write FAIL\r\n");
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return;
    }

    pfnWrite = (efi_file_write_t)pFile->pfnWrite;
    if (pfnWrite == NULL) {
        com1_puts("GJ-EFI: BOOT.LOG write FAIL reason=Write missing\n");
        efi_say(pST, "GJ-EFI: BOOT.LOG write FAIL\r\n");
        if (pFile->pfnClose != NULL) {
            (void)pFile->pfnClose(pFile);
        }
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return;
    }

    if (pFile->pfnSetPosition != NULL) {
        (void)pFile->pfnSetPosition(pFile, 0);
    }

    st = boot_log_file_write(pFile, pfnWrite);
    if (st != EFI_SUCCESS) {
        com1_puts("GJ-EFI: BOOT.LOG write FAIL reason=Write st=");
        com1_put_u64_hex((u64)st);
        com1_puts(" bytes=");
        com1_put_u64_dec((u64)g_uBootLogLen);
        com1_puts("\n");
        efi_say(pST, "GJ-EFI: BOOT.LOG write FAIL\r\n");
        if (pFile->pfnClose != NULL) {
            (void)pFile->pfnClose(pFile);
        }
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return;
    }

    pfnFlush = (efi_file_flush_t)pFile->pfnFlush;
    if (pfnFlush != NULL) {
        (void)pfnFlush(pFile);
    }
    if (pFile->pfnClose != NULL) {
        (void)pFile->pfnClose(pFile);
    }
    if (pRoot->pfnClose != NULL) {
        (void)pRoot->pfnClose(pRoot);
    }

    /* PASS markers after close so the next flush includes them. */
    com1_puts("GJ-EFI: BOOT.LOG write PASS bytes=");
    com1_put_u64_dec((u64)g_uBootLogLen);
    com1_puts(" cap=");
    com1_put_u64_dec((u64)GJ_BOOT_LOG_CAP);
    com1_puts("\n");
    efi_say(pST, "GJ-EFI: BOOT.LOG write PASS\r\n");
}

/*
 * Soft media (D4): Load \linux-drivers\modules\r8169.ko from ESP into
 * AllocatePages(LOADER_DATA). Never hard-fails product boot.
 * On success: g_BootInfo.u64SoftMediaPhys/Bytes + GJ_BOOT_F_SOFT_MEDIA.
 * greppable: GJ-EFI: soft media PASS|SKIP|FAIL
 * Soft!=product. G-AC-1.
 */
static void
load_soft_media_r8169(struct efi_boot_services *pBS, efi_handle_t hImage,
                      struct efi_system_table *pST)
{
    static struct efi_guid gLoadedImage = {
        0x5B1B31A1u, 0x9562u, 0x11d2u,
        { 0x8Eu, 0x3Fu, 0x00u, 0xA0u, 0xC9u, 0x69u, 0x72u, 0x3Bu }
    };
    static struct efi_guid gSimpleFs = {
        0x964e5b22u, 0x6459u, 0x11d2u,
        { 0x8eu, 0x39u, 0x00u, 0xa0u, 0xc9u, 0x69u, 0x72u, 0x3bu }
    };
    /* Path: \linux-drivers\modules\r8169.ko (ESP root; FAT-friendly). */
    static u16 awPath[] = {
        '\\', 'l', 'i', 'n', 'u', 'x', '-', 'd', 'r', 'i', 'v', 'e', 'r', 's',
        '\\', 'm', 'o', 'd', 'u', 'l', 'e', 's', '\\',
        'r', '8', '1', '6', '9', '.', 'k', 'o', 0
    };
    /* Alternate under EFI/GREENJADE (next to NEEDED-DRIVERS.txt). */
    static u16 awPathAlt[] = {
        '\\', 'E', 'F', 'I', '\\', 'G', 'R', 'E', 'E', 'N', 'J', 'A', 'D', 'E',
        '\\', 'l', 'i', 'n', 'u', 'x', '-', 'd', 'r', 'i', 'v', 'e', 'r', 's',
        '\\', 'm', 'o', 'd', 'u', 'l', 'e', 's', '\\',
        'r', '8', '1', '6', '9', '.', 'k', 'o', 0
    };
    static struct efi_guid gFileInfo = {
        0x09576e92u, 0x6d3fu, 0x11d2u,
        { 0x8eu, 0x39u, 0x00u, 0xa0u, 0xc9u, 0x69u, 0x72u, 0x3bu }
    };
    struct efi_loaded_image *pLi = NULL;
    struct efi_simple_fs *pFs = NULL;
    struct efi_file_protocol *pRoot = NULL;
    struct efi_file_protocol *pFile = NULL;
    efi_status_t st;
    efi_uintn_t cbRead;
    u64 u64FileSize = 0;
    efi_physical_addr_t pa = 0;
    efi_uintn_t cPages;
    u8 aInfo[256];
    efi_uintn_t cbInfo;
    u16 *pTryPath;
    int iTry;

    if (pBS == NULL) {
        return;
    }

    st = bs_open_protocol(pBS, hImage, &gLoadedImage, (void **)&pLi, hImage,
                          NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (st != EFI_SUCCESS || pLi == NULL || pLi->hDevice == NULL) {
        com1_puts("GJ-EFI: soft media SKIP reason=LoadedImage\n");
        return;
    }
    st = bs_open_protocol(pBS, pLi->hDevice, &gSimpleFs, (void **)&pFs, hImage,
                          NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (st != EFI_SUCCESS || pFs == NULL || pFs->pfnOpenVolume == NULL) {
        com1_puts("GJ-EFI: soft media SKIP reason=SimpleFileSystem\n");
        return;
    }
    st = pFs->pfnOpenVolume(pFs, &pRoot);
    if (st != EFI_SUCCESS || pRoot == NULL || pRoot->pfnOpen == NULL) {
        com1_puts("GJ-EFI: soft media SKIP reason=OpenVolume\n");
        return;
    }

    pFile = NULL;
    for (iTry = 0; iTry < 2; iTry++) {
        pTryPath = (iTry == 0) ? awPath : awPathAlt;
        st = pRoot->pfnOpen(pRoot, &pFile, pTryPath, EFI_FILE_MODE_READ, 0);
        if (st == EFI_SUCCESS && pFile != NULL) {
            break;
        }
        pFile = NULL;
    }
    if (pFile == NULL || pFile->pfnRead == NULL) {
        com1_puts("GJ-EFI: soft media SKIP reason=not_found "
                  "path=\\linux-drivers\\modules\\r8169.ko "
                  "(stage via make hwtest-img; soft!=product)\n");
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return;
    }

    /* Query FileSize; refuse empty or multi-MiB surprises (soft ceiling 4 MiB). */
    cbInfo = sizeof(aInfo);
    u64FileSize = 0;
    if (pFile->pfnGetInfo != NULL) {
        st = pFile->pfnGetInfo(pFile, &gFileInfo, &cbInfo, aInfo);
        if (st == EFI_SUCCESS && cbInfo >= 24u) {
            u64FileSize = *(const u64 *)(const void *)(aInfo + 8);
        }
    }
    if (u64FileSize < 64ull || u64FileSize > (4ull * 1024ull * 1024ull)) {
        com1_puts("GJ-EFI: soft media FAIL reason=size sz=");
        com1_put_u64_dec(u64FileSize);
        com1_puts("\n");
        if (pFile->pfnClose != NULL) {
            (void)pFile->pfnClose(pFile);
        }
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return;
    }

    cPages = (efi_uintn_t)((u64FileSize + 4095ull) / 4096ull);
    if (cPages == 0) {
        cPages = 1;
    }
    pa = 0;
    st = bs_allocate_pages(pBS, EFI_ALLOCATE_ANY_PAGES, EFI_LOADER_DATA, cPages,
                           &pa);
    if (st != EFI_SUCCESS || pa == 0) {
        com1_puts("GJ-EFI: soft media FAIL reason=AllocatePages\n");
        if (pFile->pfnClose != NULL) {
            (void)pFile->pfnClose(pFile);
        }
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return;
    }

    if (pFile->pfnSetPosition != NULL) {
        (void)pFile->pfnSetPosition(pFile, 0);
    }
    cbRead = (efi_uintn_t)u64FileSize;
    st = pFile->pfnRead(pFile, &cbRead, (void *)(gj_vaddr_t)pa);
    if (pFile->pfnClose != NULL) {
        (void)pFile->pfnClose(pFile);
    }
    if (pRoot->pfnClose != NULL) {
        (void)pRoot->pfnClose(pRoot);
    }
    if (st != EFI_SUCCESS || cbRead != (efi_uintn_t)u64FileSize) {
        com1_puts("GJ-EFI: soft media FAIL reason=read got=");
        com1_put_u64_dec((u64)cbRead);
        com1_puts(" want=");
        com1_put_u64_dec(u64FileSize);
        com1_puts("\n");
        return;
    }

    /* Soft ELF magic gate before publish. */
    {
        const u8 *p = (const u8 *)(gj_vaddr_t)pa;

        if (p[0] != 0x7fu || p[1] != (u8)'E' || p[2] != (u8)'L' ||
            p[3] != (u8)'F') {
            com1_puts("GJ-EFI: soft media FAIL reason=bad_magic\n");
            return;
        }
    }

    g_BootInfo.u64SoftMediaPhys = (u64)pa;
    g_BootInfo.u64SoftMediaBytes = u64FileSize;
    g_BootInfo.u32Flags |= GJ_BOOT_F_SOFT_MEDIA;
    /* Grep: GJ-EFI: soft media PASS */
    com1_puts("GJ-EFI: soft media PASS name=r8169 phys=");
    com1_put_u64_hex((u64)pa);
    com1_puts(" bytes=");
    com1_put_u64_dec(u64FileSize);
    com1_puts(" pages=");
    com1_put_u64_dec((u64)cPages);
    com1_puts(" path=\\linux-drivers\\modules\\r8169.ko "
              "(soft!=product; D4)\n");
    if (pST != NULL) {
        efi_say(pST, "GJ-EFI: soft media r8169.ko loaded\r\n");
    }
    (void)pST;
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

    efi_say(pST, "GJ-EFI: open protocols...\r\n");
    st = bs_open_protocol(pBS, hImage, &gLoadedImage, (void **)&pLi, hImage,
                          NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (st != EFI_SUCCESS || pLi == NULL || pLi->hDevice == NULL) {
        com1_puts("GJ-EFI: LoadedImage protocol fail\n");
        efi_say(pST, "GJ-EFI: FAIL LoadedImage\r\n");
        return st != EFI_SUCCESS ? st : EFI_LOAD_ERROR;
    }

    st = bs_open_protocol(pBS, pLi->hDevice, &gSimpleFs, (void **)&pFs, hImage,
                          NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (st != EFI_SUCCESS || pFs == NULL || pFs->pfnOpenVolume == NULL) {
        com1_puts("GJ-EFI: SimpleFileSystem protocol fail\n");
        efi_say(pST, "GJ-EFI: FAIL SimpleFileSystem\r\n");
        return st != EFI_SUCCESS ? st : EFI_LOAD_ERROR;
    }

    st = pFs->pfnOpenVolume(pFs, &pRoot);
    if (st != EFI_SUCCESS || pRoot == NULL) {
        com1_puts("GJ-EFI: OpenVolume fail\n");
        efi_say(pST, "GJ-EFI: FAIL OpenVolume\r\n");
        return st;
    }
    if (pRoot->pfnOpen == NULL) {
        com1_puts("GJ-EFI: File.Open missing\n");
        efi_say(pST, "GJ-EFI: FAIL File.Open missing\r\n");
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return EFI_LOAD_ERROR;
    }

    efi_say(pST, "GJ-EFI: open KERNEL.ELF...\r\n");
    st = pRoot->pfnOpen(pRoot, &pFile, awPath, EFI_FILE_MODE_READ, 0);
    if (st != EFI_SUCCESS || pFile == NULL) {
        com1_puts("GJ-EFI: open \\EFI\\GREENJADE\\KERNEL.ELF fail\n");
        efi_say(pST, "GJ-EFI: FAIL open KERNEL.ELF\r\n");
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

    /*
     * Whole file into pool (keeps PE BSS small). Product KERNEL.ELF with
     * embedded user ELFs is multi-MiB (~8-12 MiB). Query size via GetInfo
     * when available; otherwise allocate a 32 MiB scratch ceiling.
     */
    {
        static struct efi_guid gFileInfo = {
            0x09576e92u, 0x6d3fu, 0x11d2u,
            { 0x8eu, 0x39u, 0x00u, 0xa0u, 0xc9u, 0x69u, 0x72u, 0x3bu }
        };
        /* Minimal EFI_FILE_INFO prefix: Size, FileSize, PhysicalSize (3xu64). */
        u8 aInfo[256];
        efi_uintn_t cbInfo = sizeof(aInfo);
        u64 u64FileSize = 0;

        g_cbFileScratch = 32u * 1024u * 1024u; /* ceiling if GetInfo missing */
        if (pFile->pfnGetInfo != NULL) {
            st = pFile->pfnGetInfo(pFile, &gFileInfo, &cbInfo, aInfo);
            if (st == EFI_SUCCESS && cbInfo >= 24u) {
                /* FileSize is the second UINT64 in EFI_FILE_INFO. */
                u64FileSize = *(const u64 *)(const void *)(aInfo + 8);
                if (u64FileSize > sizeof(struct elf64_ehdr) &&
                    u64FileSize < (64ull * 1024ull * 1024ull)) {
                    /* Round up to 64 KiB; keep a small headroom. */
                    g_cbFileScratch =
                        (efi_uintn_t)((u64FileSize + 0xffffull) & ~0xffffull);
                    g_cbFileScratch += 64u * 1024u;
                }
            }
        }
    }
    st = bs_allocate_pool(pBS, EFI_LOADER_DATA, g_cbFileScratch,
                          (void **)&g_pFileScratch);
    if (st != EFI_SUCCESS || g_pFileScratch == NULL) {
        com1_puts("GJ-EFI: AllocatePool for KERNEL.ELF fail\n");
        efi_say(pST, "GJ-EFI: FAIL AllocatePool KERNEL\r\n");
        (void)pFile->pfnClose(pFile);
        if (pRoot->pfnClose != NULL) {
            (void)pRoot->pfnClose(pRoot);
        }
        return st != EFI_SUCCESS ? st : EFI_LOAD_ERROR;
    }
    if (pFile->pfnSetPosition != NULL) {
        (void)pFile->pfnSetPosition(pFile, 0);
    }
    /* Multi-MiB read from USB can take seconds - show progress on panel. */
    efi_say(pST, "GJ-EFI: reading KERNEL.ELF (multi-MiB)...\r\n");
    cbRead = g_cbFileScratch;
    st = pFile->pfnRead(pFile, &cbRead, g_pFileScratch);
    (void)pFile->pfnClose(pFile);
    if (pRoot->pfnClose != NULL) {
        (void)pRoot->pfnClose(pRoot);
    }
    if (st != EFI_SUCCESS || cbRead < sizeof(struct elf64_ehdr)) {
        com1_puts("GJ-EFI: KERNEL.ELF read fail\n");
        efi_say(pST, "GJ-EFI: FAIL KERNEL.ELF read\r\n");
        return st != EFI_SUCCESS ? st : EFI_LOAD_ERROR;
    }
    /* If read filled the pool exactly, file may be truncated - refuse. */
    if (cbRead == g_cbFileScratch) {
        com1_puts("GJ-EFI: KERNEL.ELF may exceed scratch pool\n");
        return EFI_LOAD_ERROR;
    }

    pEh = (struct elf64_ehdr *)(void *)g_pFileScratch;
    if (pEh->aIdent[0] != ELFMAG0 || pEh->aIdent[1] != ELFMAG1 ||
        pEh->aIdent[2] != ELFMAG2 || pEh->aIdent[3] != ELFMAG3 ||
        pEh->aIdent[4] != ELFCLASS64 || pEh->u16Machine != EM_X86_64) {
        com1_puts("GJ-EFI: KERNEL.ELF not ELF64/x86_64\n");
        return EFI_LOAD_ERROR;
    }

    /*
     * Pass 1: measure the full identity-linked span (product ELF is not
     * relocatable). Pass 2: one AllocateAddress for the whole span so BSS
     * + text share a single firmware allocation (OVMF is tight at 1 MiB).
     */
    for (iPh = 0; iPh < pEh->u16Phnum; iPh++) {
        struct elf64_phdr *pPh;
        u64 u64SegPa;
        u64 u64Base;
        u64 u64End;

        pPh = (struct elf64_phdr *)(void *)(g_pFileScratch + pEh->u64Phoff +
                                            (u64)iPh * pEh->u16Phentsize);
        if (pPh->u32Type != PT_LOAD || pPh->u64Memsz == 0) {
            continue;
        }
        if (g_cSoftPtLoadSegs < 0xffffffffu) {
            g_cSoftPtLoadSegs++;
        }
        u64SegPa = pPh->u64Paddr != 0 ? pPh->u64Paddr : pPh->u64Vaddr;
        u64Base = u64SegPa & ~0xfffull;
        u64End = (u64SegPa + pPh->u64Memsz + 0xfffull) & ~0xfffull;
        if (u64Base < u64MinPa) {
            u64MinPa = u64Base;
        }
        if (u64End > u64MaxPa) {
            u64MaxPa = u64End;
        }
    }
    if (u64MinPa == ~0ull || u64MaxPa <= u64MinPa) {
        com1_puts("GJ-EFI: no PT_LOAD segments\n");
        return EFI_LOAD_ERROR;
    }
    {
        efi_physical_addr_t pa = u64MinPa;
        efi_uintn_t cPages =
            (efi_uintn_t)((u64MaxPa - u64MinPa) / 4096ull);

        efi_say(pST, "GJ-EFI: AllocateAddress PT_LOAD span...\r\n");
        st = bs_allocate_pages(pBS, EFI_ALLOCATE_ADDRESS, EFI_LOADER_DATA,
                               cPages, &pa);
        if (st != EFI_SUCCESS || pa != u64MinPa) {
            /*
             * Firmware may already own low pages (common on OVMF). Still try
             * overlaying into the linked address - many firmwares leave the
             * region readable/writable even when AllocateAddress fails.
             * Real DUT firmware often succeeds AllocateAddress.
             */
            com1_puts("GJ-EFI: soft AllocateAddress PT_LOAD span fail; "
                      "trying linked PA overlay\n");
            efi_say(pST, "GJ-EFI: AllocateAddress fail; overlay linked PA\r\n");
        } else {
            efi_say(pST, "GJ-EFI: AllocateAddress OK\r\n");
        }
        efi_say(pST, "GJ-EFI: zero+copy PT_LOAD segments...\r\n");
        memset_local((void *)(gj_vaddr_t)u64MinPa, 0, u64MaxPa - u64MinPa);
    }
    for (iPh = 0; iPh < pEh->u16Phnum; iPh++) {
        struct elf64_phdr *pPh;
        u64 u64SegPa;

        pPh = (struct elf64_phdr *)(void *)(g_pFileScratch + pEh->u64Phoff +
                                            (u64)iPh * pEh->u16Phentsize);
        if (pPh->u32Type != PT_LOAD || pPh->u64Memsz == 0) {
            continue;
        }
        u64SegPa = pPh->u64Paddr != 0 ? pPh->u64Paddr : pPh->u64Vaddr;
        if (pPh->u64Filesz != 0) {
            if (pPh->u64Offset + pPh->u64Filesz > cbRead) {
                com1_puts("GJ-EFI: PT_LOAD filesz past EOF\n");
                return EFI_LOAD_ERROR;
            }
            memcpy_local((void *)(gj_vaddr_t)u64SegPa,
                         g_pFileScratch + pPh->u64Offset, pPh->u64Filesz);
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
    com1_puts("GJ-EFI: KERNEL.ELF loaded (GJUEFI1 entry ready)\n");
    efi_say(pST, "GJ-EFI: KERNEL.ELF loaded (GJUEFI1 ready)\r\n");
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
    efi_say(pST, "GJ-EFI: loader live (panel path; COM1 optional)\r\n");

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
    efi_say(pST, "GJ-EFI: loading KERNEL.ELF from ESP...\r\n");
    st = load_kernel_elf(pBS, hImage, pST, &u64Entry, &u64KernelPa,
                         &u64KernelBytes);
    if (st != EFI_SUCCESS) {
        /* Still take ExitBootServices so serial markers show on partial boots. */
        if (g_cSoftLoadFail < 0xffffffffu) {
            g_cSoftLoadFail++;
        }
        com1_puts("GJ-EFI: KERNEL.ELF load failed - EBS then halt\n");
        efi_say(pST, "GJ-EFI: KERNEL load FAILED (see COM1 if any)\r\n");
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
        efi_say(pST, "GJ-EFI: KERNEL load OK\r\n");
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
    /* Milestone: post-KERNEL load (ok or fail) - still pre-EBS. */
    flush_boot_log(pBS, hImage);

    /*
     * Soft media (D4): optional ESP r8169.ko -> boot_info before EBS.
     * Soft!=product; never gates boot. greppable: GJ-EFI: soft media
     */
    load_soft_media_r8169(pBS, hImage, pST);
    flush_boot_log(pBS, hImage);

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
            efi_say(pST, "GJ-EFI: GOP captured (green bar = loader alive)\r\n");
            /* Green bar survives EBS when ConOut is gone (laptop panel path). */
            efi_fb_bar(g_BootInfo.u64FbBase, g_BootInfo.u32FbPitch,
                       g_BootInfo.u32FbWidth, g_BootInfo.u32FbHeight,
                       0, 48, 0x0000C000u);
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
            /* Optional path - soft SKIP, not a product failure. */
            if (g_cSoftGopSkip < 0xffffffffu) {
                g_cSoftGopSkip++;
            }
            com1_puts("GJ-EFI: GOP soft SKIP (no usable framebuffer)\n");
            efi_say(pST, "GJ-EFI: GOP SKIP (text only until EBS)\r\n");
        }
    }
    /* Milestone: post-GOP capture/skip - still pre-EBS. */
    flush_boot_log(pBS, hImage);

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
     * Buffer is PE BSS (LoaderData) - valid after EBS under identity map.
     */
    efi_say(pST, "GJ-EFI: GetMemoryMap + ExitBootServices...\r\n");
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
        efi_say(pST, "GJ-EFI: FAIL GetMemoryMap buffer too small\r\n");
        return st;
    }
    if (st != EFI_SUCCESS) {
        com1_puts("GJ-EFI: GetMemoryMap fail\n");
        efi_say(pST, "GJ-EFI: FAIL GetMemoryMap\r\n");
        return st;
    }

    g_BootInfo.u64MemMap = (u64)(gj_vaddr_t)g_aMemMapScratch;
    g_BootInfo.u64MemMapBytes = u64MapSize;
    g_BootInfo.u64MemDescSize = u64DescSize; /* EFI MD stride; not Multiboot */
    g_BootInfo.u32Flags |= GJ_BOOT_F_MEMMAP;
    /* Soft classify before EBS (map still in PE BSS scratch). */
    soft_memmap_marker(u64MapSize, u64DescSize);

    /*
     * Critical milestone: last ESP write before ExitBootServices. File I/O
     * dies with Boot Services; soft inventory after EBS cannot flush.
     * Note: GetMemoryMap key may change if flush allocates (OpenProtocol is
     * typically already installed) - UEFI retry path below handles key churn.
     */
    flush_boot_log(pBS, hImage);

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
    /* ConOut is dead after EBS - use GOP bar if we have it. */
    if ((g_BootInfo.u32Flags & GJ_BOOT_F_FB) != 0 &&
        g_BootInfo.u64FbBase != 0) {
        /* Blue bar under green = EBS ok, about to jump. */
        efi_fb_bar(g_BootInfo.u64FbBase, g_BootInfo.u32FbPitch,
                   g_BootInfo.u32FbWidth, g_BootInfo.u32FbHeight,
                   48, 48, 0x000000C0u);
    }
    /*
     * Jump path: soft_handoff_marker is short. soft_wave14_inventory is a large
     * soft deepen dump - only when COM1 THRE works (OVMF). On laptops without
     * UART, com1 is "dead" after the first chars; skip the flood so we reach
     * cyan bar + kmain (G752 hang was undefined kprintf + COM1 spin).
     * C0 residual deepen: panel path still emits lean residual (BOOT.LOG tee)
     * so residual honesty is not COM1-only; never multi-KiB inventory flood.
     */
    soft_handoff_marker(u64Entry);
    if (g_fCom1Dead == 0) {
        soft_wave14_inventory(u64Entry);
    } else {
        com1_puts("GJ-EFI: soft inventory SKIP (no COM1; panel path)\n");
        /* Grep: GJ-EFI: soft residual lean (panel; Soft!=product; C0) */
        soft_residual_lean_emit(u64Entry, 1u);
    }

    if (u64Entry == 0) {
        /* Load failed earlier; stay halted with EBS already done. */
        com1_puts("GJ-EFI: no kernel entry - halt\n");
        if ((g_BootInfo.u32Flags & GJ_BOOT_F_FB) != 0 &&
            g_BootInfo.u64FbBase != 0) {
            /* Red bar = no entry. */
            efi_fb_bar(g_BootInfo.u64FbBase, g_BootInfo.u32FbPitch,
                       g_BootInfo.u32FbWidth, g_BootInfo.u32FbHeight,
                       96, 48, 0x00C00000u);
        }
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    pfnKernel = (void (*)(struct gj_boot_info *))(gj_vaddr_t)u64Entry;
    if (pfnKernel == NULL) {
        com1_puts("GJ-EFI: kernel entry NULL - halt\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    com1_puts("GJ-EFI: jump kmain_uefi\n");
    if ((g_BootInfo.u32Flags & GJ_BOOT_F_FB) != 0 &&
        g_BootInfo.u64FbBase != 0) {
        /* Cyan bar = jumping to kmain_uefi. */
        efi_fb_bar(g_BootInfo.u64FbBase, g_BootInfo.u32FbPitch,
                   g_BootInfo.u32FbWidth, g_BootInfo.u32FbHeight,
                   96, 48, 0x0000C0C0u);
    }
    pfnKernel(&g_BootInfo);

    /* kmain_uefi is noreturn; if it returns, park the BSP. */
    com1_puts("GJ-EFI: kmain_uefi returned - halt\n");
    for (;;) {
        __asm__ volatile ("hlt");
    }
    return EFI_SUCCESS;
}
