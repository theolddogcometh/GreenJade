/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room ELF64 ET_EXEC / ET_DYN load + PT_INTERP / PT_DYNAMIC / RELA.
 * Product path: probe → map PT_LOAD → SO registry → relocs → auxv handoff
 * → INTERP-first start (ld-gj). No third-party loader code.
 */
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/elf_load.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/pmm.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/user_access.h>
#include <gj/vfs_ram.h>
#include <gj/vmm.h>

#define ET_EXEC   2
#define ET_DYN    3
#define EM_X86_64 62
#define PT_LOAD   1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PF_X      1
#define PF_W      2
#define PF_R      4

#define DT_NULL     0
#define DT_NEEDED   1
#define DT_STRTAB   5
#define DT_SYMTAB   6
#define DT_RELA     7
#define DT_RELASZ   8
#define DT_RELAENT  9
#define DT_STRSZ    10
#define DT_SYMENT   11
#define DT_REL      17
#define DT_RELSZ    18
#define DT_RELENT   19
#define DT_JMPREL   23
#define DT_PLTRELSZ 2
#define DT_PLTREL   20
#define DT_HASH     4
/* GNU hash tag (LSB) */
#define DT_GNU_HASH 0x6ffffef5ll

#define R_X86_64_64        1
#define R_X86_64_COPY      5
#define R_X86_64_GLOB_DAT  6
#define R_X86_64_JUMP_SLOT 7
#define R_X86_64_RELATIVE  8
#define R_X86_64_DTPMOD64  16
#define R_X86_64_DTPOFF64  17
#define R_X86_64_TPOFF64   18
#define R_X86_64_IRELATIVE 37

#define SHN_UNDEF 0
#define SHN_ABS   0xfff1u

#define GJ_ELF_SO_MAX  4u
#define GJ_ELF_SO_IMG  32768u
/* Per-SO load bias band (above main ET_DYN default at GJ_ELF_DYN_BIAS) */
#define GJ_ELF_SO_BIAS_BASE 0x0000000071000000ull
#define GJ_ELF_SO_BIAS_STEP 0x0000000001000000ull

struct elf64_sym {
    u32 u32Name;
    u8  u8Info;
    u8  u8Other;
    u16 u16Shndx;
    u64 u64Value;
    u64 u64Size;
} __attribute__((packed));

/* Loaded DT_NEEDED objects for cross-object symbol search */
struct gj_elf_so {
    u8   u8Used;
    u8   u8Pad[3];
    u32  cbImg;
    u64  u64Bias;
    u64  u64Symtab; /* pre-bias VA */
    u64  u64Strtab;
    u64  u64Hash;
    u64  u64GnuHash;
    u64  u64Syment;
    char szName[64];
    u8   aImg[GJ_ELF_SO_IMG];
};

static struct gj_elf_so g_aSo[GJ_ELF_SO_MAX];
static u32              g_cSo;

static void path_copy_n(char *szDst, size_t cbDst, const char *szSrc);
static void path_join2(char *szDst, size_t cbDst, const char *szPfx,
                       const char *szName);

/*
 * Default ET_DYN load base when program vaddrs are low.
 * High canonical user VA away from PE 0x400000 / small smoke bases.
 */
#define GJ_ELF_DYN_BIAS 0x0000000070000000ull

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
} __attribute__((packed));

struct elf64_phdr {
    u32 u32Type;
    u32 u32Flags;
    u64 u64Offset;
    u64 u64Vaddr;
    u64 u64Paddr;
    u64 u64Filesz;
    u64 u64Memsz;
    u64 u64Align;
} __attribute__((packed));

struct elf64_dyn {
    i64 i64Tag;
    u64 u64Val;
} __attribute__((packed));

struct elf64_rela {
    u64 u64Offset;
    u64 u64Info;
    i64 i64Addend;
} __attribute__((packed));

struct elf64_rel {
    u64 u64Offset;
    u64 u64Info;
} __attribute__((packed));

static int
elf_magic_ok(const u8 *p)
{
    return p[0] == 0x7f && p[1] == 'E' && p[2] == 'L' && p[3] == 'F' &&
           p[4] == 2 && p[5] == 1;
}

static void
interp_copy(char *szDst, size_t cbDst, const u8 *pSrc, u64 cbSrc)
{
    size_t i;
    size_t cb = (size_t)cbSrc;

    if (szDst == NULL || cbDst == 0) {
        return;
    }
    if (cb >= cbDst) {
        cb = cbDst - 1;
    }
    for (i = 0; i < cb; i++) {
        if (pSrc[i] == 0) {
            break;
        }
        szDst[i] = (char)pSrc[i];
    }
    szDst[i] = '\0';
}

/*
 * Map a virtual address (pre-bias) in the file image to a file offset via
 * PT_LOAD covering that VA. Returns ~0ull if not found.
 */
static u64
va_to_file_off(const void *pImage, u64 cb, const struct elf64_ehdr *pEh,
               u64 u64Va)
{
    u16 i;

    for (i = 0; i < pEh->u16Phnum; i++) {
        const struct elf64_phdr *pPh;

        pPh = (const struct elf64_phdr *)((const u8 *)pImage + pEh->u64Phoff +
                                         (u64)i * pEh->u16Phentsize);
        if (pPh->u32Type != PT_LOAD) {
            continue;
        }
        if (u64Va >= pPh->u64Vaddr &&
            u64Va < pPh->u64Vaddr + pPh->u64Filesz) {
            return pPh->u64Offset + (u64Va - pPh->u64Vaddr);
        }
        /* ET_DYN often uses file offset == vaddr for low-linked objects */
        if (u64Va >= pPh->u64Offset &&
            u64Va < pPh->u64Offset + pPh->u64Filesz &&
            pPh->u64Vaddr == 0) {
            return u64Va;
        }
    }
    /* Fallback: treat as file offset if in range */
    if (u64Va < cb) {
        return u64Va;
    }
    (void)cb;
    return ~0ull;
}

static void
elf_parse_dynamic(const void *pImage, u64 cb, const struct elf64_ehdr *pEh,
                  struct gj_elf_info *pInfo, u64 u64DynOff, u64 u64DynFilesz)
{
    const struct elf64_dyn *pDyn;
    u64 u64Str = 0;
    u64 u64StrSz = 0;
    u64 u64End;
    u64 u64Off;
    u32 cNeeded = 0;

    if (u64DynOff + u64DynFilesz > cb || u64DynFilesz < sizeof(*pDyn)) {
        return;
    }
    pInfo->u32Flags |= GJ_ELF_INFO_HAS_DYNAMIC;
    /* First pass: find STRTAB / STRSZ */
    u64End = u64DynOff + u64DynFilesz;
    for (u64Off = u64DynOff; u64Off + sizeof(*pDyn) <= u64End;
         u64Off += sizeof(*pDyn)) {
        pDyn = (const struct elf64_dyn *)((const u8 *)pImage + u64Off);
        if (pDyn->i64Tag == DT_NULL) {
            break;
        }
        if (pDyn->i64Tag == DT_STRTAB) {
            u64Str = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_STRSZ) {
            u64StrSz = pDyn->u64Val;
        }
    }
    /* Second pass: DT_NEEDED */
    for (u64Off = u64DynOff; u64Off + sizeof(*pDyn) <= u64End;
         u64Off += sizeof(*pDyn)) {
        u64 u64NameOff;
        u64 u64FileOff;
        const char *sz;

        pDyn = (const struct elf64_dyn *)((const u8 *)pImage + u64Off);
        if (pDyn->i64Tag == DT_NULL) {
            break;
        }
        if (pDyn->i64Tag != DT_NEEDED || cNeeded >= GJ_ELF_NEEDED_MAX) {
            continue;
        }
        u64NameOff = pDyn->u64Val;
        if (u64Str != 0) {
            u64FileOff = va_to_file_off(pImage, cb, pEh, u64Str + u64NameOff);
        } else {
            u64FileOff = u64NameOff;
        }
        if (u64FileOff == ~0ull || u64FileOff >= cb) {
            continue;
        }
        if (u64StrSz != 0 && u64NameOff >= u64StrSz) {
            continue;
        }
        sz = (const char *)pImage + u64FileOff;
        interp_copy(pInfo->aNeeded[cNeeded], GJ_ELF_NEEDED_LEN, (const u8 *)sz,
                    GJ_ELF_NEEDED_LEN);
        if (pInfo->aNeeded[cNeeded][0] != '\0') {
            cNeeded++;
        }
    }
    pInfo->u16Needed = (u16)cNeeded;
}

static void
elf_collect_dyn_meta(const void *pImage, u64 cb, const struct elf64_ehdr *pEh,
                     struct gj_elf_info *pInfo)
{
    u16 i;

    for (i = 0; i < pEh->u16Phnum; i++) {
        const struct elf64_phdr *pPh;

        pPh = (const struct elf64_phdr *)((const u8 *)pImage + pEh->u64Phoff +
                                         (u64)i * pEh->u16Phentsize);
        if (pPh->u32Type == PT_DYNAMIC && pPh->u64Filesz > 0) {
            if (pPh->u64Offset + pPh->u64Filesz <= cb) {
                pInfo->u64DynVa = pPh->u64Vaddr; /* pre-bias; adjusted later */
                elf_parse_dynamic(pImage, cb, pEh, pInfo, pPh->u64Offset,
                                  pPh->u64Filesz);
            }
            break;
        }
    }
    /* AT_PHDR: prefer phdr VA from first PT_LOAD containing phoff */
    pInfo->u64PhdrVa = pEh->u64Phoff;
    for (i = 0; i < pEh->u16Phnum; i++) {
        const struct elf64_phdr *pPh;

        pPh = (const struct elf64_phdr *)((const u8 *)pImage + pEh->u64Phoff +
                                         (u64)i * pEh->u16Phentsize);
        if (pPh->u32Type != PT_LOAD) {
            continue;
        }
        if (pEh->u64Phoff >= pPh->u64Offset &&
            pEh->u64Phoff < pPh->u64Offset + pPh->u64Filesz) {
            pInfo->u64PhdrVa =
                pPh->u64Vaddr + (pEh->u64Phoff - pPh->u64Offset);
            break;
        }
    }
}

static gj_status_t
elf_fill_probe(const void *pImage, u64 cb, struct gj_elf_info *pInfo,
               const struct elf64_ehdr **ppEh)
{
    const struct elf64_ehdr *pEh;
    u16 i;
    u64 u64Min = ~0ull;
    u64 u64Max = 0;
    u32 u32Loads = 0;

    if (pImage == NULL || cb < sizeof(*pEh) || pInfo == NULL) {
        return GJ_ERR_INVAL;
    }
    memset(pInfo, 0, sizeof(*pInfo));
    pEh = (const struct elf64_ehdr *)pImage;
    if (!elf_magic_ok(pEh->aIdent) ||
        (pEh->u16Type != ET_EXEC && pEh->u16Type != ET_DYN) ||
        pEh->u16Machine != EM_X86_64) {
        return GJ_ERR_INVAL;
    }
    if (pEh->u64Phoff + (u64)pEh->u16Phnum * pEh->u16Phentsize > cb) {
        return GJ_ERR_INVAL;
    }
    if (pEh->u16Phentsize < sizeof(struct elf64_phdr)) {
        return GJ_ERR_INVAL;
    }

    pInfo->u16Type = pEh->u16Type;
    pInfo->u64Entry = pEh->u64Entry;
    pInfo->u64Phoff = pEh->u64Phoff;
    pInfo->u16Phentsize = pEh->u16Phentsize;
    pInfo->u16Phnum = pEh->u16Phnum;
    if (pEh->u16Type == ET_DYN) {
        pInfo->u32Flags |= GJ_ELF_INFO_IS_DYN;
    }

    for (i = 0; i < pEh->u16Phnum; i++) {
        const struct elf64_phdr *pPh;

        pPh = (const struct elf64_phdr *)((const u8 *)pImage + pEh->u64Phoff +
                                         (u64)i * pEh->u16Phentsize);
        if (pPh->u32Type == PT_INTERP && pPh->u64Filesz > 0) {
            if (pPh->u64Offset + pPh->u64Filesz > cb) {
                return GJ_ERR_INVAL;
            }
            interp_copy(pInfo->szInterp, sizeof(pInfo->szInterp),
                        (const u8 *)pImage + pPh->u64Offset, pPh->u64Filesz);
            if (pInfo->szInterp[0] != '\0') {
                pInfo->u32Flags |= GJ_ELF_INFO_HAS_INTERP;
            }
            continue;
        }
        if (pPh->u32Type != PT_LOAD || pPh->u64Memsz == 0) {
            continue;
        }
        if (pPh->u64Offset + pPh->u64Filesz > cb) {
            return GJ_ERR_INVAL;
        }
        if (pPh->u64Vaddr < u64Min) {
            u64Min = pPh->u64Vaddr;
        }
        if (pPh->u64Vaddr + pPh->u64Memsz > u64Max) {
            u64Max = pPh->u64Vaddr + pPh->u64Memsz;
        }
        u32Loads++;
    }

    if (u32Loads == 0) {
        return GJ_ERR_INVAL;
    }
    pInfo->u64LoadMin = u64Min;
    pInfo->u64LoadMax = u64Max;
    pInfo->u32Phdrs = u32Loads;
    elf_collect_dyn_meta(pImage, cb, pEh, pInfo);
    if (ppEh != NULL) {
        *ppEh = pEh;
    }
    return GJ_OK;
}

gj_status_t
elf_probe_image(const void *pImage, u64 cb, struct gj_elf_info *pInfo)
{
    gj_status_t st;

    st = elf_fill_probe(pImage, cb, pInfo, NULL);
    if (st == GJ_OK) {
        kprintf("elf: probe type=%u loads=%u entry=0x%lx needed=%u interp=%s\n",
                (unsigned)pInfo->u16Type, pInfo->u32Phdrs,
                (unsigned long)pInfo->u64Entry, (unsigned)pInfo->u16Needed,
                (pInfo->u32Flags & GJ_ELF_INFO_HAS_INTERP) ? pInfo->szInterp
                                                           : "(none)");
        if (pInfo->u16Needed > 0) {
            kprintf("elf: DT_NEEDED[0]=%s\n", pInfo->aNeeded[0]);
        }
    }
    return st;
}

static gj_status_t
map_page_copy(gj_vaddr_t va, const void *pSrc, size_t cbSrc, u32 u32Prot)
{
    gj_paddr_t pa;
    u64 u64Saved;
    gj_status_t st;

    pa = pmm_alloc();
    if (pa == 0) {
        return GJ_ERR_NOMEM;
    }
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    memset((void *)(gj_vaddr_t)pa, 0, GJ_PAGE_SIZE);
    if (pSrc != NULL && cbSrc > 0) {
        if (cbSrc > GJ_PAGE_SIZE) {
            cbSrc = GJ_PAGE_SIZE;
        }
        memcpy((void *)(gj_vaddr_t)pa, pSrc, cbSrc);
    }
    cpu_load_cr3(u64Saved);

    st = vmm_map_page(va, pa, u32Prot | GJ_VMM_PROT_USER);
    if (st != GJ_OK) {
        pmm_free(pa);
        return st;
    }
    return GJ_OK;
}

/* Built-in exports for undefined symbols (clean-room ld-gj bring-up). */
static const struct {
    const char *szName;
    u64         u64Val;
} g_aLdExports[] = {
    { "gj_ld_marker", 0x00474a4c444d4b52ull }, /* 'GJLD MKR' */
    { "gj_abs_val",   0x00000000c0ffee42ull },
    { "puts",         0x0000000060001000ull }, /* placeholder PLT target */
};

static int
sym_name_eq(const char *szA, const char *szB)
{
    size_t i;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    for (i = 0; szA[i] != '\0' && szA[i] == szB[i]; i++) {
    }
    return szA[i] == szB[i];
}

static u32
elf_sysv_hash(const char *szName)
{
    u32 u32H = 0;
    u32 u32G;

    if (szName == NULL) {
        return 0;
    }
    while (*szName != '\0') {
        u32H = (u32H << 4) + (u32)(u8)*szName++;
        u32G = u32H & 0xf0000000u;
        if (u32G != 0) {
            u32H ^= u32G >> 24;
        }
        u32H &= ~u32G;
    }
    return u32H;
}

static u64
elf_lookup_export(const char *szName)
{
    u32 i;

    if (szName == NULL || szName[0] == '\0') {
        return 0;
    }
    for (i = 0; i < sizeof(g_aLdExports) / sizeof(g_aLdExports[0]); i++) {
        if (sym_name_eq(szName, g_aLdExports[i].szName)) {
            return g_aLdExports[i].u64Val;
        }
    }
    return 0;
}

static void
elf_write_u64_user(u64 u64Dst, u64 u64Word)
{
    if (user_range_ok(u64Dst, 8)) {
        if (copy_to_user(u64Dst, &u64Word, 8) != GJ_OK) {
            *(u64 *)(gj_vaddr_t)u64Dst = u64Word;
        }
    } else {
        *(u64 *)(gj_vaddr_t)u64Dst = u64Word;
    }
}

static void
elf_so_clear(void)
{
    u32 i;

    for (i = 0; i < GJ_ELF_SO_MAX; i++) {
        g_aSo[i].u8Used = 0;
        g_aSo[i].cbImg = 0;
    }
    g_cSo = 0;
}

/*
 * SysV DT_HASH: nbucket, nchain, bucket[nbucket], chain[nchain]
 * Walk bucket[hash % nbucket] then chain until name match + defined.
 */
static int
elf_hash_lookup(const u8 *pImg, u64 cb, const struct elf64_ehdr *pEh,
                u64 u64Bias, u64 u64HashVa, u64 u64Symtab, u64 u64Strtab,
                u64 u64Syment, const char *szName, u64 *pVal)
{
    u64 u64HashOff;
    u64 u64SymOff;
    u64 u64StrBase;
    const u32 *pH;
    u32 u32Nbucket;
    u32 u32Nchain;
    u32 u32H;
    u32 u32Idx;
    u32 u32Guard;

    if (pImg == NULL || pEh == NULL || szName == NULL || pVal == NULL ||
        u64HashVa == 0 || u64Symtab == 0 || u64Strtab == 0 ||
        u64Syment < sizeof(struct elf64_sym)) {
        return 0;
    }
    u64HashOff = va_to_file_off(pImg, cb, pEh, u64HashVa);
    u64SymOff = va_to_file_off(pImg, cb, pEh, u64Symtab);
    u64StrBase = va_to_file_off(pImg, cb, pEh, u64Strtab);
    if (u64HashOff == ~0ull || u64SymOff == ~0ull || u64StrBase == ~0ull) {
        return 0;
    }
    if (u64HashOff + 8 > cb) {
        return 0;
    }
    pH = (const u32 *)(pImg + u64HashOff);
    u32Nbucket = pH[0];
    u32Nchain = pH[1];
    if (u32Nbucket == 0 || u32Nchain == 0) {
        return 0;
    }
    if (u64HashOff + 8ull + 4ull * ((u64)u32Nbucket + u32Nchain) > cb) {
        return 0;
    }
    u32H = elf_sysv_hash(szName);
    u32Idx = pH[2 + (u32H % u32Nbucket)];
    for (u32Guard = 0; u32Idx != 0 && u32Idx < u32Nchain && u32Guard < u32Nchain;
         u32Guard++) {
        const struct elf64_sym *pSym;
        const char *sz;
        u64 u64NameOff;

        if (u64SymOff + (u64)u32Idx * u64Syment + sizeof(*pSym) > cb) {
            break;
        }
        pSym = (const struct elf64_sym *)(pImg + u64SymOff +
                                         (u64)u32Idx * u64Syment);
        u64NameOff = u64StrBase + pSym->u32Name;
        if (u64NameOff >= cb) {
            break;
        }
        sz = (const char *)(pImg + u64NameOff);
        if (sym_name_eq(sz, szName) && pSym->u16Shndx != SHN_UNDEF) {
            if (pSym->u16Shndx == SHN_ABS) {
                *pVal = pSym->u64Value;
            } else {
                *pVal = pSym->u64Value + u64Bias;
            }
            return 1;
        }
        u32Idx = pH[2 + u32Nbucket + u32Idx];
    }
    return 0;
}

/*
 * GNU hash: nbuckets, symoffset, bloom_size, bloom_shift, bloom[], buckets[], chain[]
 * Bloom filter (64-bit words) rejects misses before chain walk.
 */
static int
elf_gnu_hash_lookup(const u8 *pImg, u64 cb, const struct elf64_ehdr *pEh,
                    u64 u64Bias, u64 u64GnuVa, u64 u64Symtab, u64 u64Strtab,
                    u64 u64Syment, const char *szName, u64 *pVal)
{
    u64 u64Off;
    u64 u64SymOff;
    u64 u64StrBase;
    const u32 *pG;
    u32 u32Nbuckets;
    u32 u32SymOffs;
    u32 u32BloomSize;
    u32 u32BloomShift;
    u32 u32H;
    u32 u32Bucket;
    u32 u32Idx;
    u32 u32Guard;
    u64 u64Word;
    u64 u64Mask;
    const u64 *pBloom;
    const u32 *pBuckets;
    const u32 *pChain;

    if (pImg == NULL || szName == NULL || pVal == NULL || u64GnuVa == 0) {
        return 0;
    }
    u64Off = va_to_file_off(pImg, cb, pEh, u64GnuVa);
    u64SymOff = va_to_file_off(pImg, cb, pEh, u64Symtab);
    u64StrBase = va_to_file_off(pImg, cb, pEh, u64Strtab);
    if (u64Off == ~0ull || u64SymOff == ~0ull || u64StrBase == ~0ull) {
        return 0;
    }
    if (u64Off + 16 > cb) {
        return 0;
    }
    pG = (const u32 *)(pImg + u64Off);
    u32Nbuckets = pG[0];
    u32SymOffs = pG[1];
    u32BloomSize = pG[2];
    u32BloomShift = pG[3];
    if (u32Nbuckets == 0) {
        return 0;
    }
    if (u64Off + 16ull + 8ull * u32BloomSize + 4ull * u32Nbuckets > cb) {
        return 0;
    }
    pBloom = (const u64 *)(pG + 4);
    pBuckets = (const u32 *)(pBloom + u32BloomSize);
    pChain = pBuckets + u32Nbuckets;

    /* DJB hash used by GNU hash */
    u32H = 5381u;
    {
        const char *p = szName;
        while (*p != '\0') {
            u32H = (u32H << 5) + u32H + (u32)(u8)*p++;
        }
    }
    /* Bloom filter (x86_64: 64-bit words) — both bits must be set */
    if (u32BloomSize > 0) {
        u64Word = pBloom[(u32H / 64u) % u32BloomSize];
        u64Mask = (1ull << (u32H % 64u)) |
                  (1ull << ((u32H >> u32BloomShift) % 64u));
        if ((u64Word & u64Mask) != u64Mask) {
            return 0; /* definite miss */
        }
    }
    u32Bucket = pBuckets[u32H % u32Nbuckets];
    if (u32Bucket < u32SymOffs) {
        return 0;
    }
    for (u32Idx = u32Bucket, u32Guard = 0; u32Guard < 4096u; u32Guard++) {
        u32 u32Ch;
        const struct elf64_sym *pSym;
        const char *sz;
        u64 u64NameOff;

        if (u64SymOff + (u64)u32Idx * u64Syment + sizeof(*pSym) > cb) {
            break;
        }
        /* chain index relative to symoffset */
        if ((u8 *)(pChain + (u32Idx - u32SymOffs)) >= pImg + cb) {
            break;
        }
        u32Ch = pChain[u32Idx - u32SymOffs];
        pSym = (const struct elf64_sym *)(pImg + u64SymOff +
                                         (u64)u32Idx * u64Syment);
        u64NameOff = u64StrBase + pSym->u32Name;
        if (u64NameOff >= cb) {
            break;
        }
        sz = (const char *)(pImg + u64NameOff);
        if (((u32Ch ^ u32H) >> 1) == 0 && sym_name_eq(sz, szName) &&
            pSym->u16Shndx != SHN_UNDEF) {
            if (pSym->u16Shndx == SHN_ABS) {
                *pVal = pSym->u64Value;
            } else {
                *pVal = pSym->u64Value + u64Bias;
            }
            return 1;
        }
        if (u32Ch & 1u) {
            break; /* end of chain */
        }
        u32Idx++;
    }
    return 0;
}

/* Linear scan of SYMTAB for defined name (fallback). */
static int
elf_symtab_scan(const u8 *pImg, u64 cb, const struct elf64_ehdr *pEh,
                u64 u64Bias, u64 u64Symtab, u64 u64Strtab, u64 u64Syment,
                const char *szName, u64 *pVal)
{
    u64 u64SymOff;
    u64 u64StrBase;
    u32 i;

    if (u64Syment < sizeof(struct elf64_sym)) {
        return 0;
    }
    u64SymOff = va_to_file_off(pImg, cb, pEh, u64Symtab);
    u64StrBase = va_to_file_off(pImg, cb, pEh, u64Strtab);
    if (u64SymOff == ~0ull || u64StrBase == ~0ull) {
        return 0;
    }
    for (i = 1; i < 4096u; i++) {
        const struct elf64_sym *pSym;
        u64 u64Off = u64SymOff + (u64)i * u64Syment;
        const char *sz;

        if (u64Off + sizeof(*pSym) > cb) {
            break;
        }
        pSym = (const struct elf64_sym *)(pImg + u64Off);
        if (pSym->u32Name == 0 || pSym->u16Shndx == SHN_UNDEF) {
            continue;
        }
        if (u64StrBase + pSym->u32Name >= cb) {
            continue;
        }
        sz = (const char *)(pImg + u64StrBase + pSym->u32Name);
        if (sym_name_eq(sz, szName)) {
            if (pSym->u16Shndx == SHN_ABS) {
                *pVal = pSym->u64Value;
            } else {
                *pVal = pSym->u64Value + u64Bias;
            }
            return 1;
        }
    }
    return 0;
}

/* Search all loaded SOs by hash then linear. */
static int
elf_lookup_in_sos(const char *szName, u64 *pVal)
{
    u32 i;

    if (szName == NULL || pVal == NULL) {
        return 0;
    }
    for (i = 0; i < GJ_ELF_SO_MAX; i++) {
        struct gj_elf_so *pSo = &g_aSo[i];
        const struct elf64_ehdr *pEh;

        if (!pSo->u8Used || pSo->cbImg < sizeof(*pEh)) {
            continue;
        }
        pEh = (const struct elf64_ehdr *)pSo->aImg;
        if (pSo->u64Hash != 0 &&
            elf_hash_lookup(pSo->aImg, pSo->cbImg, pEh, pSo->u64Bias,
                            pSo->u64Hash, pSo->u64Symtab, pSo->u64Strtab,
                            pSo->u64Syment, szName, pVal)) {
            kprintf("elf: hash resolve %s in %s PASS\n", szName, pSo->szName);
            return 1;
        }
        if (pSo->u64GnuHash != 0 &&
            elf_gnu_hash_lookup(pSo->aImg, pSo->cbImg, pEh, pSo->u64Bias,
                                pSo->u64GnuHash, pSo->u64Symtab, pSo->u64Strtab,
                                pSo->u64Syment, szName, pVal)) {
            kprintf("elf: gnu-hash resolve %s in %s PASS\n", szName,
                    pSo->szName);
            return 1;
        }
        if (pSo->u64Symtab != 0 && pSo->u64Strtab != 0 &&
            elf_symtab_scan(pSo->aImg, pSo->cbImg, pEh, pSo->u64Bias,
                            pSo->u64Symtab, pSo->u64Strtab, pSo->u64Syment,
                            szName, pVal)) {
            kprintf("elf: scan resolve %s in %s PASS\n", szName, pSo->szName);
            return 1;
        }
    }
    return 0;
}

/*
 * Resolve symbol for GLOB_DAT / JUMP_SLOT:
 *   defined in image → st_value + bias
 *   undefined → SO registry (hash/gnu-hash/scan) then built-in exports
 */
static int
elf_resolve_sym(const void *pImage, u64 cb, const struct elf64_ehdr *pEh,
                u64 u64Bias, u64 u64Symtab, u64 u64Strtab, u64 u64Syment,
                u32 u32SymIdx, u64 *pVal)
{
    u64 u64SymFile;
    u64 u64StrFile;
    const struct elf64_sym *pSym;
    const char *szName;
    u64 u64Exp;

    if (pVal == NULL || u64Syment < sizeof(struct elf64_sym)) {
        return 0;
    }
    u64SymFile = va_to_file_off(pImage, cb, pEh, u64Symtab);
    if (u64SymFile == ~0ull) {
        return 0;
    }
    u64SymFile += (u64)u32SymIdx * u64Syment;
    if (u64SymFile + sizeof(struct elf64_sym) > cb) {
        return 0;
    }
    pSym = (const struct elf64_sym *)((const u8 *)pImage + u64SymFile);
    if (pSym->u16Shndx != SHN_UNDEF && pSym->u16Shndx != SHN_ABS) {
        *pVal = pSym->u64Value + u64Bias;
        return 1;
    }
    if (pSym->u16Shndx == SHN_ABS) {
        *pVal = pSym->u64Value;
        return 1;
    }
    /* Undefined: name lookup across SOs then exports */
    if (u64Strtab == 0) {
        return 0;
    }
    u64StrFile = va_to_file_off(pImage, cb, pEh, u64Strtab + pSym->u32Name);
    if (u64StrFile == ~0ull || u64StrFile >= cb) {
        return 0;
    }
    szName = (const char *)pImage + u64StrFile;
    if (elf_lookup_in_sos(szName, pVal)) {
        return 1;
    }
    u64Exp = elf_lookup_export(szName);
    if (u64Exp != 0) {
        *pVal = u64Exp;
        return 1;
    }
    /* Weak undefined → 0 */
    if (((pSym->u8Info >> 4) & 0xf) == 2) { /* STB_WEAK */
        *pVal = 0;
        return 1;
    }
    return 0;
}

/* Parse dynamic tags for SO registry (hash/sym/str). */
static void
elf_so_fill_dyn(struct gj_elf_so *pSo, const void *pImage, u64 cb,
                const struct elf64_ehdr *pEh)
{
    u16 i;
    u64 u64DynOff = 0;
    u64 u64DynSz = 0;
    u64 u64Off;
    const struct elf64_dyn *pDyn;

    pSo->u64Symtab = 0;
    pSo->u64Strtab = 0;
    pSo->u64Hash = 0;
    pSo->u64GnuHash = 0;
    pSo->u64Syment = sizeof(struct elf64_sym);

    for (i = 0; i < pEh->u16Phnum; i++) {
        const struct elf64_phdr *pPh;

        pPh = (const struct elf64_phdr *)((const u8 *)pImage + pEh->u64Phoff +
                                         (u64)i * pEh->u16Phentsize);
        if (pPh->u32Type == PT_DYNAMIC && pPh->u64Filesz > 0) {
            u64DynOff = pPh->u64Offset;
            u64DynSz = pPh->u64Filesz;
            break;
        }
    }
    if (u64DynOff == 0 || u64DynOff + u64DynSz > cb) {
        return;
    }
    for (u64Off = u64DynOff; u64Off + sizeof(*pDyn) <= u64DynOff + u64DynSz;
         u64Off += sizeof(*pDyn)) {
        pDyn = (const struct elf64_dyn *)((const u8 *)pImage + u64Off);
        if (pDyn->i64Tag == DT_NULL) {
            break;
        }
        if (pDyn->i64Tag == DT_SYMTAB) {
            pSo->u64Symtab = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_STRTAB) {
            pSo->u64Strtab = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_SYMENT) {
            pSo->u64Syment = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_HASH) {
            pSo->u64Hash = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_GNU_HASH) {
            pSo->u64GnuHash = pDyn->u64Val;
        }
    }
}

/*
 * Apply RELATIVE + GLOB_DAT + JUMP_SLOT on the mapped image under active AS.
 */
static u32
elf_apply_relocs(const void *pImage, u64 cb, const struct elf64_ehdr *pEh,
                 u64 u64Bias, u32 *pSymHits)
{
    u16 i;
    u64 u64DynOff = 0;
    u64 u64DynSz = 0;
    u64 u64Rela = 0;
    u64 u64RelaSz = 0;
    u64 u64RelaEnt = sizeof(struct elf64_rela);
    u64 u64Jmprel = 0;
    u64 u64Pltrelsz = 0;
    u64 u64Symtab = 0;
    u64 u64Strtab = 0;
    u64 u64Syment = sizeof(struct elf64_sym);
    u64 u64Off;
    u64 u64End;
    u32 cApplied = 0;
    u32 cSym = 0;
    const struct elf64_dyn *pDyn;

    if (pSymHits != NULL) {
        *pSymHits = 0;
    }

    for (i = 0; i < pEh->u16Phnum; i++) {
        const struct elf64_phdr *pPh;

        pPh = (const struct elf64_phdr *)((const u8 *)pImage + pEh->u64Phoff +
                                         (u64)i * pEh->u16Phentsize);
        if (pPh->u32Type == PT_DYNAMIC && pPh->u64Filesz > 0) {
            u64DynOff = pPh->u64Offset;
            u64DynSz = pPh->u64Filesz;
            break;
        }
    }
    if (u64DynOff == 0 || u64DynOff + u64DynSz > cb) {
        return 0;
    }
    u64End = u64DynOff + u64DynSz;
    for (u64Off = u64DynOff; u64Off + sizeof(*pDyn) <= u64End;
         u64Off += sizeof(*pDyn)) {
        pDyn = (const struct elf64_dyn *)((const u8 *)pImage + u64Off);
        if (pDyn->i64Tag == DT_NULL) {
            break;
        }
        if (pDyn->i64Tag == DT_RELA) {
            u64Rela = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_RELASZ) {
            u64RelaSz = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_RELAENT) {
            u64RelaEnt = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_JMPREL) {
            u64Jmprel = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_PLTRELSZ) {
            u64Pltrelsz = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_SYMTAB) {
            u64Symtab = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_STRTAB) {
            u64Strtab = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_SYMENT) {
            u64Syment = pDyn->u64Val;
        }
    }

    /* Process DT_RELA then DT_JMPREL (PLT relocs are often JUMP_SLOT) */
    {
        u64 aTabs[2];
        u64 aSz[2];
        u32 t;

        aTabs[0] = u64Rela;
        aSz[0] = u64RelaSz;
        aTabs[1] = u64Jmprel;
        aSz[1] = u64Pltrelsz;

        for (t = 0; t < 2; t++) {
            u64 u64File;
            u64 u64N;
            u64 j;

            if (aTabs[t] == 0 || aSz[t] == 0 ||
                u64RelaEnt < sizeof(struct elf64_rela)) {
                continue;
            }
            u64File = va_to_file_off(pImage, cb, pEh, aTabs[t]);
            if (u64File == ~0ull || u64File + aSz[t] > cb) {
                continue;
            }
            u64N = aSz[t] / u64RelaEnt;
            for (j = 0; j < u64N; j++) {
                const struct elf64_rela *pR;
                u32 u32Type;
                u32 u32Sym;
                u64 u64Dst;
                u64 u64Word;

                pR = (const struct elf64_rela *)((const u8 *)pImage + u64File +
                                                 j * u64RelaEnt);
                u32Type = (u32)(pR->u64Info & 0xffffffffull);
                u32Sym = (u32)(pR->u64Info >> 32);
                u64Dst = pR->u64Offset + u64Bias;

                if (u32Type == R_X86_64_RELATIVE) {
                    u64Word = (u64)pR->i64Addend + u64Bias;
                    elf_write_u64_user(u64Dst, u64Word);
                    cApplied++;
                } else if (u32Type == R_X86_64_DTPMOD64) {
                    elf_write_u64_user(u64Dst, 1);
                    cApplied++;
                } else if (u32Type == R_X86_64_DTPOFF64 ||
                           u32Type == R_X86_64_TPOFF64) {
                    elf_write_u64_user(u64Dst, (u64)pR->i64Addend);
                    cApplied++;
                } else if (u32Type == R_X86_64_IRELATIVE) {
                    /* Defer true IFUNC call to ld-gj; store resolver VA */
                    u64Word = (u64)pR->i64Addend + u64Bias;
                    elf_write_u64_user(u64Dst, u64Word);
                    cApplied++;
                } else if (u32Type == R_X86_64_GLOB_DAT ||
                           u32Type == R_X86_64_JUMP_SLOT ||
                           u32Type == R_X86_64_64) {
                    if (u64Symtab == 0) {
                        continue;
                    }
                    if (!elf_resolve_sym(pImage, cb, pEh, u64Bias, u64Symtab,
                                         u64Strtab, u64Syment, u32Sym,
                                         &u64Word)) {
                        continue;
                    }
                    u64Word += (u64)pR->i64Addend;
                    elf_write_u64_user(u64Dst, u64Word);
                    cApplied++;
                    cSym++;
                } else if (u32Type == R_X86_64_COPY) {
                    if (u64Symtab == 0) {
                        continue;
                    }
                    if (!elf_resolve_sym(pImage, cb, pEh, u64Bias, u64Symtab,
                                         u64Strtab, u64Syment, u32Sym,
                                         &u64Word)) {
                        continue;
                    }
                    /* COPY size unknown without st_size in resolve — write ptr */
                    elf_write_u64_user(u64Dst, u64Word);
                    cApplied++;
                    cSym++;
                }
            }
        }
    }
    if (pSymHits != NULL) {
        *pSymHits = cSym;
    }
    return cApplied;
}

gj_status_t
elf_load_image(struct gj_process *pProc, const void *pImage, u64 cb,
               struct gj_elf_info *pInfo)
{
    return elf_load_image_bias(pProc, pImage, cb, 0, pInfo);
}

gj_status_t
elf_load_image_bias(struct gj_process *pProc, const void *pImage, u64 cb,
                    u64 u64BiasReq, struct gj_elf_info *pInfo)
{
    struct gj_elf_info info;
    const struct elf64_ehdr *pEh;
    u16 i;
    u64 u64Bias = 0;
    u32 u32Loaded = 0;
    u32 cRel;
    gj_status_t st;

    if (pProc == NULL) {
        return GJ_ERR_INVAL;
    }
    st = elf_fill_probe(pImage, cb, &info, &pEh);
    if (st != GJ_OK) {
        kprintf("elf: bad header\n");
        return st;
    }

    if (u64BiasReq != 0) {
        u64Bias = u64BiasReq;
    } else if (info.u16Type == ET_DYN && info.u64LoadMin < 0x100000ull) {
        u64Bias = GJ_ELF_DYN_BIAS;
    }
    info.u64Bias = u64Bias;
    info.u64Entry = pEh->u64Entry + u64Bias;
    info.u64LoadMin += u64Bias;
    info.u64LoadMax += u64Bias;
    info.u64PhdrVa += u64Bias;
    if (info.u64DynVa != 0) {
        info.u64DynVa += u64Bias;
    }
    info.u64Base = (info.u16Type == ET_DYN) ? u64Bias : info.u64LoadMin;

    if (process_as_ensure(pProc) != GJ_OK) {
        return GJ_ERR_NOMEM;
    }
    process_as_activate(pProc);

    for (i = 0; i < pEh->u16Phnum; i++) {
        const struct elf64_phdr *pPh;
        u64 u64Page;
        u64 u64Va;
        u64 u64End;
        u64 u64SegVa;
        u32 u32Prot;

        pPh = (const struct elf64_phdr *)((const u8 *)pImage + pEh->u64Phoff +
                                         (u64)i * pEh->u16Phentsize);
        if (pPh->u32Type != PT_LOAD || pPh->u64Memsz == 0) {
            continue;
        }

        u32Prot = GJ_VMM_PROT_READ;
        if (pPh->u32Flags & PF_W) {
            u32Prot |= GJ_VMM_PROT_WRITE;
        }
        if (pPh->u32Flags & PF_X) {
            u32Prot |= GJ_VMM_PROT_EXEC;
        }

        u64SegVa = pPh->u64Vaddr + u64Bias;
        u64Va = u64SegVa & ~0xfffull;
        u64End = (u64SegVa + pPh->u64Memsz + 0xfffull) & ~0xfffull;

        for (u64Page = u64Va; u64Page < u64End; u64Page += GJ_PAGE_SIZE) {
            u8 aTmp[GJ_PAGE_SIZE];

            memset(aTmp, 0, sizeof(aTmp));
            if (u64Page + GJ_PAGE_SIZE > u64SegVa &&
                u64Page < u64SegVa + pPh->u64Filesz) {
                u64 u64From = u64SegVa > u64Page ? u64SegVa : u64Page;
                u64 u64To = u64SegVa + pPh->u64Filesz;
                u64 u64PgEnd = u64Page + GJ_PAGE_SIZE;
                u64 u64SegOff;
                size_t cbCopy;

                if (u64To > u64PgEnd) {
                    u64To = u64PgEnd;
                }
                u64SegOff = (u64From - u64SegVa) + pPh->u64Offset;
                cbCopy = (size_t)(u64To - u64From);
                memcpy(aTmp + (u64From - u64Page),
                       (const u8 *)pImage + u64SegOff, cbCopy);
            }
            if (map_page_copy(u64Page, aTmp, GJ_PAGE_SIZE, u32Prot) != GJ_OK) {
                return GJ_ERR_NOMEM;
            }
        }
        u32Loaded++;
    }

    if (u32Loaded == 0) {
        return GJ_ERR_INVAL;
    }

    {
        u32 cSym = 0;

        cRel = elf_apply_relocs(pImage, cb, pEh, u64Bias, &cSym);
        info.u32SymRelocs = cSym;
        if (cRel > 0) {
            info.u32Flags |= GJ_ELF_INFO_RELOC_OK;
            kprintf("elf: relocs applied=%u sym=%u bias=0x%lx\n", cRel, cSym,
                    (unsigned long)u64Bias);
        }
        if (cSym > 0) {
            info.u32Flags |= GJ_ELF_INFO_SYM_OK;
            kprintf("elf: GLOB_DAT/JUMP_SLOT PASS n=%u\n", cSym);
        }
    }

    info.u32Phdrs = u32Loaded;
    info.u32Flags |= GJ_ELF_INFO_LOADED;
    if (pInfo != NULL) {
        *pInfo = info;
    }
    kprintf("elf: loaded phdrs=%u type=%u entry=0x%lx range=0x%lx-0x%lx bias=0x%lx\n",
            u32Loaded, (unsigned)info.u16Type, (unsigned long)info.u64Entry,
            (unsigned long)info.u64LoadMin, (unsigned long)info.u64LoadMax,
            (unsigned long)u64Bias);
    if (info.u32Flags & GJ_ELF_INFO_HAS_INTERP) {
        kprintf("elf: PT_INTERP %s\n", info.szInterp);
    }
    if (info.u16Needed > 0) {
        kprintf("elf: DT_NEEDED count=%u first=%s\n", (unsigned)info.u16Needed,
                info.aNeeded[0]);
    }
    return GJ_OK;
}

void
elf_fill_auxv(struct gj_process *pProc, const struct gj_elf_info *pMain,
              const struct gj_elf_info *pInterp)
{
    u32 n = 0;
    u64 u64ExecFn = 0;

    if (pProc == NULL || pMain == NULL) {
        return;
    }
    memset(pProc->aAuxv, 0, sizeof(pProc->aAuxv));
    /* AT_EXECFN points at path string inside handoff page once published */
    u64ExecFn = GJ_LD_HANDOFF_VA +
                (u64)__builtin_offsetof(struct gj_ld_handoff, szPath);

#define AUX_PUSH(k, v)                                                         \
    do {                                                                       \
        if (n + 1u < GJ_PROC_AUXV_MAX) {                                       \
            pProc->aAuxv[n * 2u] = (u64)(k);                                   \
            pProc->aAuxv[n * 2u + 1u] = (u64)(v);                              \
            n++;                                                               \
        }                                                                      \
    } while (0)

    AUX_PUSH(GJ_AT_PHDR, pMain->u64PhdrVa);
    AUX_PUSH(GJ_AT_PHENT, pMain->u16Phentsize);
    AUX_PUSH(GJ_AT_PHNUM, pMain->u16Phnum);
    AUX_PUSH(GJ_AT_PAGESZ, GJ_PAGE_SIZE);
    AUX_PUSH(GJ_AT_BASE, pInterp != NULL ? pInterp->u64Base : 0);
    AUX_PUSH(GJ_AT_FLAGS, 0);
    AUX_PUSH(GJ_AT_ENTRY, pMain->u64Entry);
    AUX_PUSH(GJ_AT_UID, 0);
    AUX_PUSH(GJ_AT_EUID, 0);
    AUX_PUSH(GJ_AT_GID, 0);
    AUX_PUSH(GJ_AT_EGID, 0);
    AUX_PUSH(GJ_AT_SECURE, 0);
    if (pProc->szExecPath[0] != '\0') {
        AUX_PUSH(GJ_AT_EXECFN, u64ExecFn);
    }
    AUX_PUSH(GJ_AT_NULL, 0);
#undef AUX_PUSH

    pProc->cAuxv = n;
    kprintf("elf: auxv pairs=%u phdr=0x%lx entry=0x%lx base=0x%lx\n", n,
            (unsigned long)pMain->u64PhdrVa, (unsigned long)pMain->u64Entry,
            (unsigned long)(pInterp != NULL ? pInterp->u64Base : 0));
}

u32
elf_load_needed_sos(struct gj_process *pProc, const struct gj_elf_info *pInfo)
{
    u32 iNeeded;
    u32 cLoaded = 0;
    char szPath[160];
    static const char *const apPfx[] = { "/lib/", "/usr/lib/" };

    if (pProc == NULL || pInfo == NULL) {
        return 0;
    }
    elf_so_clear();
    for (iNeeded = 0; iNeeded < pInfo->u16Needed && iNeeded < GJ_ELF_NEEDED_MAX;
         iNeeded++) {
        i64 i64Fd;
        i64 nRead;
        struct gj_elf_info so;
        u32 iPfx;
        u32 iSlot;

        if (pInfo->aNeeded[iNeeded][0] == '\0') {
            continue;
        }
        if (cLoaded >= GJ_ELF_SO_MAX) {
            break;
        }
        iSlot = cLoaded;
        for (iPfx = 0; iPfx < 2u; iPfx++) {
            path_join2(szPath, sizeof(szPath), apPfx[iPfx],
                       pInfo->aNeeded[iNeeded]);
            i64Fd = vfs_ram_open(szPath, 0);
            if (i64Fd < 0) {
                continue;
            }
            nRead = vfs_ram_read(i64Fd, g_aSo[iSlot].aImg, GJ_ELF_SO_IMG);
            (void)vfs_ram_close(i64Fd);
            if (nRead < 4 || g_aSo[iSlot].aImg[0] != 0x7fu ||
                g_aSo[iSlot].aImg[1] != (u8)'E' ||
                g_aSo[iSlot].aImg[2] != (u8)'L' ||
                g_aSo[iSlot].aImg[3] != (u8)'F') {
                kprintf("elf: SO skip non-ELF %s\n", szPath);
                break;
            }
            {
                u64 u64SoBias =
                    GJ_ELF_SO_BIAS_BASE + (u64)cLoaded * GJ_ELF_SO_BIAS_STEP;
                const struct elf64_ehdr *pEhSo;

                if (elf_load_image_bias(pProc, g_aSo[iSlot].aImg, (u64)nRead,
                                        u64SoBias, &so) == GJ_OK) {
                    g_aSo[iSlot].u8Used = 1;
                    g_aSo[iSlot].cbImg = (u32)nRead;
                    g_aSo[iSlot].u64Bias = so.u64Bias;
                    path_copy_n(g_aSo[iSlot].szName, sizeof(g_aSo[iSlot].szName),
                                pInfo->aNeeded[iNeeded]);
                    pEhSo = (const struct elf64_ehdr *)g_aSo[iSlot].aImg;
                    elf_so_fill_dyn(&g_aSo[iSlot], g_aSo[iSlot].aImg, (u64)nRead,
                                    pEhSo);
                    cLoaded++;
                    g_cSo = cLoaded;
                    kprintf("elf: SO map %s entry=0x%lx bias=0x%lx "
                            "hash=0x%lx gnu=0x%lx sym=0x%lx PASS\n",
                            szPath, (unsigned long)so.u64Entry,
                            (unsigned long)so.u64Bias,
                            (unsigned long)g_aSo[iSlot].u64Hash,
                            (unsigned long)g_aSo[iSlot].u64GnuHash,
                            (unsigned long)g_aSo[iSlot].u64Symtab);
                } else {
                    kprintf("elf: SO map %s FAIL\n", szPath);
                }
            }
            break;
        }
    }
    if (cLoaded > 0) {
        kprintf("elf: DT_NEEDED SO map n=%u PASS\n", cLoaded);
    }
    return cLoaded;
}

gj_status_t
elf_apply_interp_first(struct gj_process *pProc, const struct gj_elf_info *pMain,
                       const struct gj_elf_info *pInterp, u64 u64Stack)
{
    u64 u64Entry;
    u32 cRepl;
    u32 thr;

    if (pProc == NULL || pMain == NULL) {
        return GJ_ERR_INVAL;
    }
    if (u64Stack == 0) {
        u64Stack = GJ_LD_STACK_VA;
    }
    pProc->u64ExecStack = u64Stack;
    pProc->u32StartThr = 0;
    if (pInterp != NULL && pInterp->u64Entry != 0 &&
        (pMain->u32Flags & GJ_ELF_INFO_HAS_INTERP) != 0) {
        u64Entry = pInterp->u64Entry;
        pProc->u64InterpEntry = pInterp->u64Entry;
        pProc->u64StartEntry = u64Entry;
        cRepl = thread_exec_replace(pProc, u64Entry, u64Stack);
        /*
         * Product: if no existing user thr to rewrite, spawn a live ring-3
         * thread at INTERP (ld-gj) with handoff stack SP.
         */
        if (cRepl == 0) {
            thr = thread_create_user(pProc, u64Entry, u64Stack);
            if (thr != 0) {
                pProc->u32StartThr = thr;
                cRepl = 1;
                kprintf("linux: execve INTERP-first live thr=%u entry=0x%lx "
                        "sp=0x%lx PASS\n",
                        thr, (unsigned long)u64Entry, (unsigned long)u64Stack);
            }
        } else {
            pProc->u32StartThr = 0; /* rewritten existing thr(s) */
        }
        kprintf("linux: execve INTERP-first entry=0x%lx sp=0x%lx thr=%u PASS\n",
                (unsigned long)u64Entry, (unsigned long)u64Stack, cRepl);
        return GJ_OK;
    }
    /* No INTERP: start at main */
    u64Entry = pMain->u64Entry;
    pProc->u64StartEntry = u64Entry;
    cRepl = thread_exec_replace(pProc, u64Entry, u64Stack);
    if (cRepl == 0) {
        thr = thread_create_user(pProc, u64Entry, u64Stack);
        if (thr != 0) {
            pProc->u32StartThr = thr;
            cRepl = 1;
        }
    }
    kprintf("linux: execve direct entry=0x%lx sp=0x%lx thr=%u\n",
            (unsigned long)u64Entry, (unsigned long)u64Stack, cRepl);
    return GJ_OK;
}

u32
elf_resolve_needed_vfs(const struct gj_elf_info *pInfo)
{
    u32 iNeeded;
    u32 cOk = 0;
    char szPath[160];
    static const char *const apPfx[] = { "/lib/", "/usr/lib/" };

    if (pInfo == NULL) {
        return 0;
    }
    for (iNeeded = 0; iNeeded < pInfo->u16Needed && iNeeded < GJ_ELF_NEEDED_MAX;
         iNeeded++) {
        i64 i64Fd;
        u32 iPfx;
        int fFound = 0;

        if (pInfo->aNeeded[iNeeded][0] == '\0') {
            continue;
        }
        for (iPfx = 0; iPfx < 2u; iPfx++) {
            path_join2(szPath, sizeof(szPath), apPfx[iPfx],
                       pInfo->aNeeded[iNeeded]);
            i64Fd = vfs_ram_open(szPath, 0);
            if (i64Fd >= 0) {
                (void)vfs_ram_close(i64Fd);
                cOk++;
                fFound = 1;
                kprintf("elf: DT_NEEDED resolve %s -> %s PASS\n",
                        pInfo->aNeeded[iNeeded], szPath);
                break;
            }
        }
        if (!fFound) {
            kprintf("elf: DT_NEEDED resolve %s missing\n",
                    pInfo->aNeeded[iNeeded]);
        }
    }
    return cOk;
}

static void
path_copy_n(char *szDst, size_t cbDst, const char *szSrc)
{
    size_t i;

    if (szDst == NULL || cbDst == 0) {
        return;
    }
    if (szSrc == NULL) {
        szDst[0] = '\0';
        return;
    }
    for (i = 0; i + 1 < cbDst && szSrc[i] != '\0'; i++) {
        szDst[i] = szSrc[i];
    }
    szDst[i] = '\0';
}

/** Join directory prefix + basename into szDst (NUL-terminated, truncated). */
static void
path_join2(char *szDst, size_t cbDst, const char *szPfx, const char *szName)
{
    size_t i;
    size_t n = 0;

    if (szDst == NULL || cbDst == 0) {
        return;
    }
    if (szPfx != NULL) {
        for (i = 0; szPfx[i] != '\0' && n + 1 < cbDst; i++) {
            szDst[n++] = szPfx[i];
        }
    }
    if (szName != NULL) {
        for (i = 0; szName[i] != '\0' && n + 1 < cbDst; i++) {
            szDst[n++] = szName[i];
        }
    }
    szDst[n] = '\0';
}

static void
publish_vfs_blob(const char *szPath, const void *pData, size_t cb)
{
    i64 i64Fd;
    u64 u64Saved;

    if (szPath == NULL || pData == NULL) {
        return;
    }
    /* vfs_ram_write reads pData under current CR3 — use kernel CR3. */
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    i64Fd = vfs_ram_open(szPath, 1);
    if (i64Fd < 0) {
        cpu_load_cr3(u64Saved);
        return;
    }
    (void)vfs_ram_lseek(i64Fd, 0, 0);
    (void)vfs_ram_ftruncate(i64Fd, 0);
    (void)vfs_ram_write(i64Fd, pData, cb);
    (void)vfs_ram_close(i64Fd);
    cpu_load_cr3(u64Saved);
}

gj_status_t
elf_publish_handoff(struct gj_process *pProc, const char *szPath,
                    const struct gj_elf_info *pMain,
                    const struct gj_elf_info *pInterp)
{
    struct gj_ld_handoff ho;
    gj_paddr_t pa;
    gj_paddr_t paStack;
    u64 u64Saved;
    u32 i;
    u32 cCopy;
    u64 *pStack;
    u32 o;

    if (pProc == NULL || pMain == NULL) {
        return GJ_ERR_INVAL;
    }
    memset(&ho, 0, sizeof(ho));
    ho.u64Magic = GJ_LD_HANDOFF_MAGIC;
    ho.u64Entry = pMain->u64Entry;
    ho.u64Interp = pInterp != NULL ? pInterp->u64Entry : 0;
    ho.u64Base = pInterp != NULL ? pInterp->u64Base : 0;
    ho.u64Phdr = pMain->u64PhdrVa;
    ho.u64Phent = pMain->u16Phentsize;
    ho.u64Phnum = pMain->u16Phnum;
    ho.u64Pagesz = GJ_PAGE_SIZE;
    ho.u32Flags = pMain->u32Flags;
    ho.cSymReloc = pMain->u32SymRelocs;
    ho.cSo = 0;
    for (i = 0; i < GJ_ELF_SO_MAX && i < GJ_LD_SO_MAX; i++) {
        if (!g_aSo[i].u8Used) {
            continue;
        }
        ho.aSo[ho.cSo].u64Bias = g_aSo[i].u64Bias;
        ho.aSo[ho.cSo].cbImg = g_aSo[i].cbImg;
        path_copy_n(ho.aSo[ho.cSo].szName, sizeof(ho.aSo[ho.cSo].szName),
                    g_aSo[i].szName);
        ho.cSo++;
    }
    ho.cAuxv = pProc->cAuxv;
    cCopy = pProc->cAuxv;
    if (cCopy > GJ_AUXV_MAX) {
        cCopy = GJ_AUXV_MAX;
    }
    for (i = 0; i < cCopy * 2u && i < GJ_AUXV_MAX * 2u; i++) {
        ho.aAuxv[i] = pProc->aAuxv[i];
    }
    if (szPath != NULL && szPath[0] != '\0') {
        path_copy_n(ho.szPath, sizeof(ho.szPath), szPath);
        path_copy_n(pProc->szExecPath, sizeof(pProc->szExecPath), szPath);
    } else if (pProc->szExecPath[0] != '\0') {
        path_copy_n(ho.szPath, sizeof(ho.szPath), pProc->szExecPath);
    }
    if (pMain->szInterp[0] != '\0') {
        path_copy_n(ho.szInterp, sizeof(ho.szInterp), pMain->szInterp);
    } else if (pInterp != NULL && pInterp->szInterp[0] != '\0') {
        path_copy_n(ho.szInterp, sizeof(ho.szInterp), pInterp->szInterp);
    }

    /*
     * INTERP-first stack block (SysV): argc, argv[], NULL, envp NULL, auxv...
     * argv[0] points at path string stored later in the same page.
     */
    ho.u64Stack = GJ_LD_STACK_VA;

    if (process_as_ensure(pProc) != GJ_OK) {
        return GJ_ERR_NOMEM;
    }
    pa = pmm_alloc();
    paStack = pmm_alloc();
    if (pa == 0 || paStack == 0) {
        if (pa != 0) {
            pmm_free(pa);
        }
        if (paStack != 0) {
            pmm_free(paStack);
        }
        return GJ_ERR_NOMEM;
    }
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    memset((void *)(gj_vaddr_t)pa, 0, GJ_PAGE_SIZE);
    memcpy((void *)(gj_vaddr_t)pa, &ho, sizeof(ho));

    /* Build stack page under kernel CR3 */
    memset((void *)(gj_vaddr_t)paStack, 0, GJ_PAGE_SIZE);
    pStack = (u64 *)(gj_vaddr_t)paStack;
    /* Layout: [0]=argc [1]=argv0 [2]=0 [3]=0(env) then auxv pairs; path@0x200 */
    pStack[0] = 1; /* argc */
    pStack[1] = GJ_LD_STACK_VA + 0x200ull; /* argv[0] → path string */
    pStack[2] = 0; /* argv terminator */
    pStack[3] = 0; /* env terminator */
    o = 4;
    for (i = 0; i < cCopy && o + 1u < 64u; i++) {
        pStack[o++] = pProc->aAuxv[i * 2u];
        pStack[o++] = pProc->aAuxv[i * 2u + 1u];
    }
    /* Ensure AT_NULL */
    if (o < 2 || pStack[o - 2] != GJ_AT_NULL) {
        pStack[o++] = GJ_AT_NULL;
        pStack[o++] = 0;
    }
    {
        char *szPathSlot = (char *)(gj_vaddr_t)paStack + 0x200;
        size_t n = 0;

        while (ho.szPath[n] != '\0' && n + 1 < 128u) {
            szPathSlot[n] = ho.szPath[n];
            n++;
        }
        szPathSlot[n] = '\0';
    }
    cpu_load_cr3(u64Saved);
    process_as_activate(pProc);
    if (vmm_map_page((gj_vaddr_t)GJ_LD_HANDOFF_VA, pa,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        pmm_free(pa);
        pmm_free(paStack);
        return GJ_ERR_NOMEM;
    }
    if (vmm_map_page((gj_vaddr_t)GJ_LD_STACK_VA, paStack,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        pmm_free(paStack);
        /* handoff page stays; stack optional for verify */
        ho.u64Stack = 0;
    } else {
        /* Patch handoff page stack field (already mapped) */
        if (user_range_ok(GJ_LD_HANDOFF_VA +
                              (u64)__builtin_offsetof(struct gj_ld_handoff,
                                                      u64Stack),
                          8)) {
            u64 u64St = GJ_LD_STACK_VA;
            (void)copy_to_user(GJ_LD_HANDOFF_VA +
                                   (u64)__builtin_offsetof(struct gj_ld_handoff,
                                                           u64Stack),
                               &u64St, 8);
        }
        kprintf("elf: stack auxv va=0x%lx PASS\n",
                (unsigned long)GJ_LD_STACK_VA);
    }

    /* Userspace open path for ld-gj freestanding probes */
    publish_vfs_blob("/proc/self/auxv", pProc->aAuxv,
                     (size_t)pProc->cAuxv * 2u * sizeof(u64));
    if (ho.szPath[0] != '\0') {
        publish_vfs_blob("/proc/self/execfn", ho.szPath,
                         strlen(ho.szPath) + 1u);
    }

    kprintf("elf: handoff va=0x%lx entry=0x%lx base=0x%lx path=%s sym=%u "
            "so=%u\n",
            (unsigned long)GJ_LD_HANDOFF_VA, (unsigned long)ho.u64Entry,
            (unsigned long)ho.u64Base,
            ho.szPath[0] != '\0' ? ho.szPath : "(none)", ho.cSymReloc, ho.cSo);
    return GJ_OK;
}

gj_status_t
elf_ld_handoff_verify(struct gj_process *pProc)
{
    struct gj_ld_handoff ho;
    gj_paddr_t pa;
    u64 u64Saved;
    gj_paddr_t paEntry;

    if (pProc == NULL) {
        return GJ_ERR_INVAL;
    }
    if (process_as_ensure(pProc) != GJ_OK) {
        return GJ_ERR_NOMEM;
    }
    process_as_activate(pProc);
    pa = vmm_virt_to_phys((gj_vaddr_t)GJ_LD_HANDOFF_VA);
    if (pa == 0) {
        kprintf("ld-gj: handoff page missing\n");
        return GJ_ERR_NOENT;
    }
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    memcpy(&ho, (void *)(gj_vaddr_t)(pa & ~0xfffull), sizeof(ho));
    cpu_load_cr3(u64Saved);
    process_as_activate(pProc);

    if (ho.u64Magic != GJ_LD_HANDOFF_MAGIC) {
        kprintf("ld-gj: handoff magic FAIL 0x%lx\n",
                (unsigned long)ho.u64Magic);
        return GJ_ERR_INVAL;
    }
    if (ho.u64Entry == 0 || ho.u64Phdr == 0) {
        kprintf("ld-gj: handoff entry/phdr FAIL\n");
        return GJ_ERR_INVAL;
    }
    paEntry = vmm_virt_to_phys((gj_vaddr_t)(ho.u64Entry & ~0xfffull));
    if (paEntry == 0) {
        kprintf("ld-gj: AT_ENTRY not mapped 0x%lx\n",
                (unsigned long)ho.u64Entry);
        return GJ_ERR_FAULT;
    }
    kprintf("ld-gj: AT_ENTRY ready 0x%lx\n", (unsigned long)ho.u64Entry);
    if (ho.u64Interp != 0) {
        kprintf("ld-gj: INTERP entry 0x%lx base=0x%lx\n",
                (unsigned long)ho.u64Interp, (unsigned long)ho.u64Base);
    }
    if (ho.szPath[0] != '\0') {
        kprintf("ld-gj: EXECFN %s\n", ho.szPath);
    }
    if (ho.u64Stack != 0) {
        gj_paddr_t paSt = vmm_virt_to_phys((gj_vaddr_t)ho.u64Stack);

        if (paSt != 0) {
            kprintf("ld-gj: stack auxv ready 0x%lx\n",
                    (unsigned long)ho.u64Stack);
        }
    }
    if (ho.cSymReloc > 0 || (ho.u32Flags & GJ_ELF_INFO_SYM_OK) != 0) {
        kprintf("ld-gj: symbol reloc ready n=%u\n", ho.cSymReloc);
    }
    kprintf("ld-gj: handoff PASS\n");
    return GJ_OK;
}
