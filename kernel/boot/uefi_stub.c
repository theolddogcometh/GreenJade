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
#define EfiConventionalMemory  7u
#define EfiACPIReclaimMemory   9u

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
        com1_puts("GJ-EFI: KERNEL.ELF load failed — EBS then halt\n");
    } else {
        g_BootInfo.u64KernelPhys = u64KernelPa;
        g_BootInfo.u64KernelBytes = u64KernelBytes;
        g_BootInfo.u32Flags |= GJ_BOOT_F_KERNEL_IMG;
    }

    /* Optional GOP fill for early desktop FB (kernel may ignore). */
    {
        static struct efi_guid gGop = {
            0x9042a9deu, 0x23dcu, 0x4a38u,
            { 0x96u, 0xfbu, 0x7au, 0xdeu, 0xd0u, 0x80u, 0x51u, 0x6au }
        };
        typedef efi_status_t (__attribute__((ms_abi)) *loc_t)(
            struct efi_guid *, void *, void **);
        loc_t pfnLoc = (loc_t)pBS->aFn[BS_LocateProtocol];
        struct efi_gop *pGop = NULL;

        if (pfnLoc != NULL &&
            pfnLoc(&gGop, NULL, (void **)&pGop) == EFI_SUCCESS && pGop != NULL &&
            pGop->pMode != NULL && pGop->pMode->pInfo != NULL &&
            pGop->pMode->paFrameBuffer != 0) {
            g_BootInfo.u64FbBase = (u64)pGop->pMode->paFrameBuffer;
            g_BootInfo.u32FbWidth = pGop->pMode->pInfo->u32HRes;
            g_BootInfo.u32FbHeight = pGop->pMode->pInfo->u32VRes;
            g_BootInfo.u32FbPitch =
                pGop->pMode->pInfo->u32PixelsPerScanLine * 4u;
            g_BootInfo.u32FbBpp = 32;
            g_BootInfo.u32Flags |= GJ_BOOT_F_FB;
            com1_puts("GJ-EFI: GOP framebuffer captured\n");
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
                if (guid_eq(&pST->pConfigurationTable[iTable].guid, &gAcpi20)) {
                    break;
                }
            }
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

    st = bs_exit_boot_services(pBS, hImage, u64MapKey);
    if (st != EFI_SUCCESS) {
        u64MapSize = sizeof(g_aMemMapScratch);
        st = bs_call_get_memory_map(pBS, &u64MapSize,
                                    (struct efi_memory_descriptor *)(void *)
                                        g_aMemMapScratch,
                                    &u64MapKey, &u64DescSize, &u32DescVer);
        if (st != EFI_SUCCESS) {
            com1_puts("GJ-EFI: GetMemoryMap retry fail\n");
            return st;
        }
        g_BootInfo.u64MemMapBytes = u64MapSize;
        g_BootInfo.u64MemDescSize = u64DescSize;
        g_BootInfo.u32Flags |= GJ_BOOT_F_MEMMAP;
        st = bs_exit_boot_services(pBS, hImage, u64MapKey);
        if (st != EFI_SUCCESS) {
            com1_puts("GJ-EFI: ExitBootServices fail (after retry)\n");
            return st;
        }
    }

    com1_puts("GJ-EFI: ExitBootServices ok\n");

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
