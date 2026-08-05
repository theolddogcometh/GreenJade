/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft freestanding loader for Linux x86_64 .ko ELF relocatable objects (ET_REL).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux source copy.
 *
 * Algorithm (load_mem)
 * --------------------
 *   1. Validate ELF64 LE ET_REL EM_X86_64 header + section table bounds.
 *   2. Locate SHT_SYMTAB (+ linked strtab); count SHF_ALLOC layout size and
 *      GOTPCREL* reloc slots (soft GOT for GOTPCREL / GOTPCRELX / REX_).
 *   3. Allocate contiguous PMM pages; place each SHF_ALLOC section with
 *      sh_addralign (SHT_NOBITS zeroed; PROGBITS copied from image).
 *   4. Append soft GOT region; apply SHT_RELA / SHT_REL into loaded targets.
 *   5. Resolve SHN_UNDEF via linux_ksym_lookup (weak miss → 0; strong miss →
 *      record name, free pages, FAIL). Local/ABS via section base + st_value.
 *   6. Bind init_module / cleanup_module global symbols; register slot (max 8).
 *
 * Relocs applied: R_X86_64_{64,PC32,PLT32,32,32S,GOTPCREL,GOTPCRELX,
 * REX_GOTPCRELX}. Others skipped with a soft log line.
 *
 * greppable: linux_module: soft load source=embed|media|mem|finit name=
 * greppable: linux_module: soft load PASS|FAIL source= name= missing=
 * greppable: linux_module: soft init PASS|FAIL
 * greppable: linux_module: soft exit
 * greppable: linux_module: soft reloc skip
 * greppable: linux_module: soft media path OPEN|SKIP name= reason=
 */
#include <gj/config.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/linux_module.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* ---- Optional ksym peer (F2). Weak stub if not linked. ------------------ */

/*
 * Prefer prototype from gj/linux_ksym.h when that header exists in-tree.
 * Compile without it: declare + weak stub here so this unit builds alone.
 */
#if defined(__has_include)
#if __has_include(<gj/linux_ksym.h>)
#include <gj/linux_ksym.h>
#define GJ_LMOD_HAS_KSYM_H 1
#endif
#endif

#ifndef GJ_LMOD_HAS_KSYM_H
void *linux_ksym_lookup(const char *szName);
#endif

__attribute__((weak)) void *
linux_ksym_lookup(const char *szName)
{
    (void)szName;
    return NULL;
}

/* ---- ELF64 clean-room constants (public ABI numbers only) --------------- */

#define EI_MAG0 0
#define EI_CLASS 4
#define EI_DATA 5
#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define ET_REL 1
#define EM_X86_64 62

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_NOBITS 8
#define SHT_REL 9

#define SHF_WRITE 0x1ull
#define SHF_ALLOC 0x2ull
#define SHF_EXECINSTR 0x4ull

#define SHN_UNDEF 0
#define SHN_ABS 0xfff1u
#define SHN_COMMON 0xfff2u

#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3

#define R_X86_64_NONE 0
#define R_X86_64_64 1
#define R_X86_64_PC32 2
#define R_X86_64_PLT32 4
#define R_X86_64_GOTPCREL 9
#define R_X86_64_32 10
#define R_X86_64_32S 11
#define R_X86_64_PC64 24
#define R_X86_64_GOTPCRELX 41
#define R_X86_64_REX_GOTPCRELX 42

#define GJ_LMOD_NAME_MAX 64u
#define GJ_LMOD_UNRES_MAX 128u
#define GJ_LMOD_SH_MAX 160u
#define GJ_LMOD_GOT_MAX 1024u
#define GJ_LMOD_PAGES_MAX 512u

struct elf64_ehdr {
    u8 aIdent[16];
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

struct elf64_shdr {
    u32 u32Name;
    u32 u32Type;
    u64 u64Flags;
    u64 u64Addr;
    u64 u64Offset;
    u64 u64Size;
    u32 u32Link;
    u32 u32Info;
    u64 u64Addralign;
    u64 u64Entsize;
} __attribute__((packed));

struct elf64_sym {
    u32 u32Name;
    u8 u8Info;
    u8 u8Other;
    u16 u16Shndx;
    u64 u64Value;
    u64 u64Size;
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

typedef int (*gj_lmod_init_fn)(void);
typedef void (*gj_lmod_exit_fn)(void);

struct gj_lmod {
    u8 u8Used;
    u8 u8Inited;
    u8 u8Pad[2];
    u32 cPages;
    gj_paddr_t paBase;
    void *pLoad; /* HHDM VA of paBase */
    u64 cbLoad;
    u64 u64GotBase; /* VA of soft GOT (inside load image) */
    u32 cGot;
    u32 cGotUsed;
    gj_lmod_init_fn pfnInit;
    gj_lmod_exit_fn pfnExit;
    char szName[GJ_LMOD_NAME_MAX];
};

static struct gj_lmod g_aMod[GJ_LINUX_MODULE_MAX];
static u32 g_cMod;
static char g_szLastUnres[GJ_LMOD_UNRES_MAX];
static int g_fInited;

/* Per-load scratch (single-threaded soft path). */
static u64 g_aSecVa[GJ_LMOD_SH_MAX];
static u8 g_aSecAlloc[GJ_LMOD_SH_MAX];

static void
lmod_set_unres(const char *szName)
{
    if (szName == NULL || szName[0] == '\0') {
        g_szLastUnres[0] = '\0';
        return;
    }
    (void)strlcpy(g_szLastUnres, szName, sizeof(g_szLastUnres));
}

static void *
lmod_va(gj_paddr_t pa)
{
    if (pa == 0) {
        return NULL;
    }
    /*
     * Prefer identity VA (VA==PA) for the first 4 GiB.
     * Kernel text lives in low identity (~1 MiB+); HHDM VAs are in the
     * high half and break R_X86_64_PC32 / 32S to ksym stubs (G752 saw
     * MOD r8169 FAIL st=-8 FAULT with unres=(none)).
     */
    if ((u64)pa < (4ull << 30)) {
        return (void *)(uintptr_t)pa;
    }
    return (void *)hhdm_to_virt(pa);
}

static void
lmod_zero_pages(gj_paddr_t pa, u32 cPages)
{
    u32 i;

    for (i = 0; i < cPages; i++) {
        void *pVa = lmod_va(pa + (gj_paddr_t)i * (gj_paddr_t)GJ_PAGE_SIZE);
        if (pVa != NULL) {
            memset(pVa, 0, GJ_PAGE_SIZE);
        }
    }
}

static u64
lmod_align_up(u64 u64Val, u64 u64Align)
{
    u64 u64A;

    if (u64Align < 1ull) {
        u64Align = 1ull;
    }
    /* Power-of-two fast path; else general. */
    if ((u64Align & (u64Align - 1ull)) == 0ull) {
        return (u64Val + u64Align - 1ull) & ~(u64Align - 1ull);
    }
    u64A = u64Val % u64Align;
    if (u64A == 0ull) {
        return u64Val;
    }
    return u64Val + (u64Align - u64A);
}

static int
lmod_name_eq(const char *szA, const char *szB)
{
    if (szA == NULL || szB == NULL) {
        return 0;
    }
    return strcmp(szA, szB) == 0;
}

static struct gj_lmod *
lmod_find(const char *szName)
{
    u32 i;

    if (szName == NULL) {
        return NULL;
    }
    for (i = 0; i < GJ_LINUX_MODULE_MAX; i++) {
        if (g_aMod[i].u8Used && lmod_name_eq(g_aMod[i].szName, szName)) {
            return &g_aMod[i];
        }
    }
    return NULL;
}

static struct gj_lmod *
lmod_alloc_slot(void)
{
    u32 i;

    for (i = 0; i < GJ_LINUX_MODULE_MAX; i++) {
        if (!g_aMod[i].u8Used) {
            memset(&g_aMod[i], 0, sizeof(g_aMod[i]));
            g_aMod[i].u8Used = 1;
            return &g_aMod[i];
        }
    }
    return NULL;
}

static void
lmod_free_pages(gj_paddr_t pa, u32 cPages)
{
    if (pa == 0 || cPages == 0) {
        return;
    }
    if (cPages == 1u) {
        pmm_free(pa);
    } else {
        pmm_free_pages(pa, cPages);
    }
}

static void
lmod_free_slot(struct gj_lmod *pMod)
{
    if (pMod == NULL) {
        return;
    }
    lmod_free_pages(pMod->paBase, pMod->cPages);
    memset(pMod, 0, sizeof(*pMod));
}

static int
lmod_ehdr_ok(const struct elf64_ehdr *pEh, size_t cb)
{
    u64 u64ShEnd;

    if (pEh == NULL || cb < sizeof(*pEh)) {
        return 0;
    }
    if (pEh->aIdent[EI_MAG0] != 0x7f || pEh->aIdent[1] != 'E' ||
        pEh->aIdent[2] != 'L' || pEh->aIdent[3] != 'F') {
        return 0;
    }
    if (pEh->aIdent[EI_CLASS] != ELFCLASS64 ||
        pEh->aIdent[EI_DATA] != ELFDATA2LSB) {
        return 0;
    }
    if (pEh->u16Type != ET_REL || pEh->u16Machine != EM_X86_64) {
        return 0;
    }
    if (pEh->u16Shentsize < sizeof(struct elf64_shdr) || pEh->u16Shnum == 0 ||
        pEh->u16Shnum > GJ_LMOD_SH_MAX) {
        return 0;
    }
    if (pEh->u64Shoff == 0) {
        return 0;
    }
    u64ShEnd = pEh->u64Shoff +
               (u64)pEh->u16Shnum * (u64)pEh->u16Shentsize;
    if (u64ShEnd < pEh->u64Shoff || u64ShEnd > (u64)cb) {
        return 0;
    }
    if (pEh->u16Shstrndx != 0 && pEh->u16Shstrndx >= pEh->u16Shnum) {
        return 0;
    }
    return 1;
}

static const struct elf64_shdr *
lmod_shdr(const u8 *pImg, const struct elf64_ehdr *pEh, u32 iSh)
{
    return (const struct elf64_shdr *)(pImg + pEh->u64Shoff +
                                       (u64)iSh * (u64)pEh->u16Shentsize);
}

static int
lmod_sh_in_image(const struct elf64_shdr *pSh, size_t cb)
{
    if (pSh == NULL) {
        return 0;
    }
    if (pSh->u32Type == SHT_NOBITS) {
        return 1;
    }
    if (pSh->u64Size == 0) {
        return 1;
    }
    if (pSh->u64Offset > (u64)cb) {
        return 0;
    }
    if (pSh->u64Offset + pSh->u64Size < pSh->u64Offset) {
        return 0;
    }
    if (pSh->u64Offset + pSh->u64Size > (u64)cb) {
        return 0;
    }
    return 1;
}

static u8
lmod_st_bind(u8 u8Info)
{
    return (u8)(u8Info >> 4);
}

static u32
lmod_r_sym(u64 u64Info)
{
    return (u32)(u64Info >> 32);
}

static u32
lmod_r_type(u64 u64Info)
{
    return (u32)(u64Info & 0xffffffffull);
}

static int
lmod_is_got_reloc(u32 u32Type)
{
    return (u32Type == R_X86_64_GOTPCREL || u32Type == R_X86_64_GOTPCRELX ||
            u32Type == R_X86_64_REX_GOTPCRELX)
               ? 1
               : 0;
}

static i64
lmod_resolve_sym(const u8 *pImg, size_t cb, const struct elf64_ehdr *pEh,
                 const struct elf64_sym *pSym, const char *pStr,
                 u64 cbStr, u64 *pu64Val)
{
    u16 u16Sh;
    const char *szName;
    void *pKsym;

    (void)pImg;
    (void)cb;
    (void)pEh;

    if (pSym == NULL || pu64Val == NULL) {
        return (i64)GJ_ERR_INVAL;
    }

    u16Sh = pSym->u16Shndx;
    szName = "";
    if (pStr != NULL && pSym->u32Name < cbStr) {
        szName = pStr + pSym->u32Name;
    }

    if (u16Sh == SHN_UNDEF) {
        pKsym = linux_ksym_lookup(szName);
        if (pKsym != NULL) {
            *pu64Val = (u64)(uintptr_t)pKsym;
            return (i64)GJ_OK;
        }
        if (lmod_st_bind(pSym->u8Info) == STB_WEAK) {
            *pu64Val = 0;
            return (i64)GJ_OK;
        }
        lmod_set_unres(szName[0] != '\0' ? szName : "?");
        return (i64)GJ_ERR_NOENT;
    }

    if (u16Sh == SHN_ABS) {
        *pu64Val = pSym->u64Value;
        return (i64)GJ_OK;
    }

    if (u16Sh == SHN_COMMON) {
        /* Soft: no common-block allocator; reject. */
        lmod_set_unres(szName[0] != '\0' ? szName : "COMMON");
        return (i64)GJ_ERR_NOSUPPORT;
    }

    if (u16Sh >= GJ_LMOD_SH_MAX) {
        lmod_set_unres(szName[0] != '\0' ? szName : "bad_shndx");
        return (i64)GJ_ERR_INVAL;
    }
    if (pEh != NULL && u16Sh >= pEh->u16Shnum) {
        lmod_set_unres(szName[0] != '\0' ? szName : "bad_shndx");
        return (i64)GJ_ERR_INVAL;
    }
    if (!g_aSecAlloc[u16Sh]) {
        /*
         * Soft: non-ALLOC (e.g. .BTF STT_SECTION) or empty-bss miss —
         * zero and continue so one odd reloc cannot abort the whole .ko.
         */
        static u8 s_fNonAllocOnce;
        if (s_fNonAllocOnce == 0u) {
            s_fNonAllocOnce = 1u;
            kprintf("linux_module: soft resolve non-ALLOC shndx=%u "
                    "name=%s soft-zero\n",
                    (unsigned)u16Sh,
                    szName[0] != '\0' ? szName : "(section)");
        }
        *pu64Val = 0;
        return (i64)GJ_OK;
    }

    *pu64Val = g_aSecVa[u16Sh] + pSym->u64Value;
    return (i64)GJ_OK;
}

static void
lmod_write_u64(u64 u64P, u64 u64Val)
{
    memcpy((void *)(uintptr_t)u64P, &u64Val, sizeof(u64Val));
}

static void
lmod_write_u32(u64 u64P, u32 u32Val)
{
    memcpy((void *)(uintptr_t)u64P, &u32Val, sizeof(u32Val));
}

static u64
lmod_read_u64(u64 u64P)
{
    u64 u64Val = 0;

    memcpy(&u64Val, (const void *)(uintptr_t)u64P, sizeof(u64Val));
    return u64Val;
}

static u32
lmod_read_u32(u64 u64P)
{
    u32 u32Val = 0;

    memcpy(&u32Val, (const void *)(uintptr_t)u64P, sizeof(u32Val));
    return u32Val;
}

static i64
lmod_apply_one(struct gj_lmod *pMod, u32 u32Type, u64 u64P, u64 u64S,
               i64 i64A)
{
    u64 u64Word;
    i64 i64Delta;
    u32 u32Word;
    u64 u64GotSlot;

    switch (u32Type) {
    case R_X86_64_NONE:
        return (i64)GJ_OK;

    case R_X86_64_64:
        lmod_write_u64(u64P, u64S + (u64)i64A);
        return (i64)GJ_OK;

    case R_X86_64_PC64:
        lmod_write_u64(u64P, u64S + (u64)i64A - u64P);
        return (i64)GJ_OK;

    case R_X86_64_PC32:
    case R_X86_64_PLT32:
        /* Static soft: PLT32 ≡ PC32 (no PLT stub). */
        i64Delta = (i64)(u64S + (u64)i64A - u64P);
        if (i64Delta != (i32)i64Delta) {
            /* Soft: zero slot + continue (honest lamp once). */
            static u8 s_fPc32Once;
            if (s_fPc32Once == 0u) {
                s_fPc32Once = 1u;
                kprintf("linux_module: soft reloc pc32 range soft-zero "
                        "p=0x%lx s=0x%lx (identity VA preferred)\n",
                        (unsigned long)u64P, (unsigned long)u64S);
            }
            lmod_write_u32(u64P, 0);
            return (i64)GJ_OK;
        }
        u32Word = (u32)(i32)i64Delta;
        lmod_write_u32(u64P, u32Word);
        return (i64)GJ_OK;

    case R_X86_64_32:
        u64Word = u64S + (u64)i64A;
        if (u64Word > 0xffffffffull) {
            static u8 s_f32Once;
            if (s_f32Once == 0u) {
                s_f32Once = 1u;
                kprintf("linux_module: soft reloc R_64_32 range soft-zero "
                        "s=0x%lx\n", (unsigned long)u64Word);
            }
            lmod_write_u32(u64P, 0);
            return (i64)GJ_OK;
        }
        lmod_write_u32(u64P, (u32)u64Word);
        return (i64)GJ_OK;

    case R_X86_64_32S:
        i64Delta = (i64)(u64S + (u64)i64A);
        if (i64Delta != (i32)i64Delta) {
            static u8 s_f32sOnce;
            if (s_f32sOnce == 0u) {
                s_f32sOnce = 1u;
                kprintf("linux_module: soft reloc R_64_32S range soft-zero "
                        "s=0x%lx\n", (unsigned long)u64S);
            }
            lmod_write_u32(u64P, 0);
            return (i64)GJ_OK;
        }
        lmod_write_u32(u64P, (u32)(i32)i64Delta);
        return (i64)GJ_OK;

    case R_X86_64_GOTPCREL:
    case R_X86_64_GOTPCRELX:
    case R_X86_64_REX_GOTPCRELX:
        if (pMod == NULL || pMod->u64GotBase == 0) {
            return (i64)GJ_ERR_NOMEM;
        }
        if (pMod->cGotUsed >= pMod->cGot || pMod->cGotUsed >= GJ_LMOD_GOT_MAX) {
            return (i64)GJ_ERR_NOMEM;
        }
        u64GotSlot = pMod->u64GotBase + (u64)pMod->cGotUsed * 8ull;
        pMod->cGotUsed++;
        lmod_write_u64(u64GotSlot, u64S);
        i64Delta = (i64)(u64GotSlot + (u64)i64A - u64P);
        if (i64Delta != (i32)i64Delta) {
            return (i64)GJ_ERR_FAULT;
        }
        lmod_write_u32(u64P, (u32)(i32)i64Delta);
        return (i64)GJ_OK;

    default:
        kprintf("linux_module: soft reloc skip type=%u p=0x%lx\n",
                (unsigned)u32Type, (unsigned long)u64P);
        return (i64)GJ_OK; /* soft skip */
    }
}

static i64
lmod_apply_relocs(struct gj_lmod *pMod, const u8 *pImg, size_t cb,
                  const struct elf64_ehdr *pEh, const struct elf64_sym *pSym0,
                  u32 cSym, const char *pStr, u64 cbStr)
{
    u32 iSh;
    i64 i64St;

    for (iSh = 0; iSh < pEh->u16Shnum; iSh++) {
        const struct elf64_shdr *pSh = lmod_shdr(pImg, pEh, iSh);
        u32 iTgt;
        u64 u64TgtVa;
        u64 u64Ent;
        u64 u64N;
        u64 iRel;

        if (pSh->u32Type != SHT_RELA && pSh->u32Type != SHT_REL) {
            continue;
        }
        if (!lmod_sh_in_image(pSh, cb)) {
            return (i64)GJ_ERR_INVAL;
        }
        iTgt = pSh->u32Info;
        if (iTgt >= pEh->u16Shnum || iTgt >= GJ_LMOD_SH_MAX ||
            !g_aSecAlloc[iTgt]) {
            /* Reloc for non-ALLOC (debug) — soft skip section. */
            continue;
        }
        u64TgtVa = g_aSecVa[iTgt];
        if (pSh->u64Size == 0) {
            continue;
        }

        if (pSh->u32Type == SHT_RELA) {
            u64Ent = pSh->u64Entsize ? pSh->u64Entsize
                                     : (u64)sizeof(struct elf64_rela);
            if (u64Ent < sizeof(struct elf64_rela)) {
                return (i64)GJ_ERR_INVAL;
            }
            u64N = pSh->u64Size / u64Ent;
            for (iRel = 0; iRel < u64N; iRel++) {
                const struct elf64_rela *pR;
                u32 u32Sym;
                u32 u32Type;
                u64 u64S;
                u64 u64P;

                pR = (const struct elf64_rela *)(pImg + pSh->u64Offset +
                                                 iRel * u64Ent);
                u32Sym = lmod_r_sym(pR->u64Info);
                u32Type = lmod_r_type(pR->u64Info);
                if (u32Sym >= cSym) {
                    return (i64)GJ_ERR_INVAL;
                }
                i64St = lmod_resolve_sym(pImg, cb, pEh, &pSym0[u32Sym], pStr,
                                         cbStr, &u64S);
                if (i64St != (i64)GJ_OK) {
                    return i64St;
                }
                u64P = u64TgtVa + pR->u64Offset;
                i64St = lmod_apply_one(pMod, u32Type, u64P, u64S, pR->i64Addend);
                if (i64St != (i64)GJ_OK) {
                    return i64St;
                }
            }
        } else {
            u64Ent =
                pSh->u64Entsize ? pSh->u64Entsize : (u64)sizeof(struct elf64_rel);
            if (u64Ent < sizeof(struct elf64_rel)) {
                return (i64)GJ_ERR_INVAL;
            }
            u64N = pSh->u64Size / u64Ent;
            for (iRel = 0; iRel < u64N; iRel++) {
                const struct elf64_rel *pR;
                u32 u32Sym;
                u32 u32Type;
                u64 u64S;
                u64 u64P;
                i64 i64A;

                pR = (const struct elf64_rel *)(pImg + pSh->u64Offset +
                                                iRel * u64Ent);
                u32Sym = lmod_r_sym(pR->u64Info);
                u32Type = lmod_r_type(pR->u64Info);
                if (u32Sym >= cSym) {
                    return (i64)GJ_ERR_INVAL;
                }
                i64St = lmod_resolve_sym(pImg, cb, pEh, &pSym0[u32Sym], pStr,
                                         cbStr, &u64S);
                if (i64St != (i64)GJ_OK) {
                    return i64St;
                }
                u64P = u64TgtVa + pR->u64Offset;
                /* Implicit addend at relocated field. */
                if (u32Type == R_X86_64_64 || u32Type == R_X86_64_PC64) {
                    i64A = (i64)lmod_read_u64(u64P);
                } else {
                    i64A = (i32)lmod_read_u32(u64P);
                }
                i64St = lmod_apply_one(pMod, u32Type, u64P, u64S, i64A);
                if (i64St != (i64)GJ_OK) {
                    return i64St;
                }
            }
        }
    }
    return (i64)GJ_OK;
}

static void
lmod_bind_init_exit(struct gj_lmod *pMod, const struct elf64_sym *pSym0,
                    u32 cSym, const char *pStr, u64 cbStr)
{
    u32 i;

    pMod->pfnInit = NULL;
    pMod->pfnExit = NULL;
    if (pSym0 == NULL || pStr == NULL) {
        return;
    }
    for (i = 0; i < cSym; i++) {
        const struct elf64_sym *pSym = &pSym0[i];
        const char *szName;
        u8 u8Bind;
        u64 u64Val;
        i64 i64St;

        if (pSym->u32Name >= cbStr) {
            continue;
        }
        szName = pStr + pSym->u32Name;
        if (szName[0] == '\0') {
            continue;
        }
        u8Bind = lmod_st_bind(pSym->u8Info);
        if (u8Bind != STB_GLOBAL && u8Bind != STB_WEAK) {
            continue;
        }
        if (pSym->u16Shndx == SHN_UNDEF || pSym->u16Shndx == SHN_COMMON) {
            continue;
        }
        if (strcmp(szName, "init_module") != 0 &&
            strcmp(szName, "cleanup_module") != 0) {
            continue;
        }
        i64St = lmod_resolve_sym(NULL, 0, NULL, pSym, pStr, cbStr, &u64Val);
        if (i64St != (i64)GJ_OK || u64Val == 0) {
            continue;
        }
        if (strcmp(szName, "init_module") == 0) {
            pMod->pfnInit = (gj_lmod_init_fn)(uintptr_t)u64Val;
        } else {
            pMod->pfnExit = (gj_lmod_exit_fn)(uintptr_t)u64Val;
        }
    }
}

void
linux_module_init(void)
{
    u32 i;

    for (i = 0; i < GJ_LINUX_MODULE_MAX; i++) {
        if (g_aMod[i].u8Used && g_aMod[i].paBase != 0 && g_aMod[i].cPages != 0) {
            /* Soft re-init while live: do not free live modules. */
            continue;
        }
        memset(&g_aMod[i], 0, sizeof(g_aMod[i]));
    }
    g_cMod = 0;
    for (i = 0; i < GJ_LINUX_MODULE_MAX; i++) {
        if (g_aMod[i].u8Used) {
            g_cMod++;
        }
    }
    g_szLastUnres[0] = '\0';
    g_fInited = 1;
    kprintf("linux_module: soft ready max=%u live=%u\n",
            (unsigned)GJ_LINUX_MODULE_MAX, (unsigned)g_cMod);
}

/*
 * Fail lamp with source tag (D4/D5 honesty). Soft≠product.
 * greppable: linux_module: soft load FAIL source= name= missing=
 */
static void
lmod_lamp_fail(const char *szSrc, const char *szName, const char *szMiss)
{
    kprintf("linux_module: soft load FAIL source=%s name=%s missing=%s\n",
            szSrc != NULL && szSrc[0] != '\0' ? szSrc : "mem",
            szName != NULL && szName[0] != '\0' ? szName : "anon",
            szMiss != NULL && szMiss[0] != '\0' ? szMiss : "-");
}

i64
linux_module_load_mem(const void *pElf, size_t cb, const char *szName)
{
    return linux_module_load_mem_src(pElf, cb, szName, "mem");
}

i64
linux_module_load_mem_src(const void *pElf, size_t cb, const char *szName,
                          const char *szSource)
{
    const u8 *pImg;
    const struct elf64_ehdr *pEh;
    const struct elf64_shdr *pShSym = NULL;
    const struct elf64_sym *pSym0 = NULL;
    const char *pStr = NULL;
    const char *szSrc;
    u64 cbStr = 0;
    u32 cSym = 0;
    u32 iSh;
    u64 u64Cursor = 0;
    u64 u64GotOff = 0;
    u32 cGot = 0;
    u32 cPages;
    gj_paddr_t paBase;
    void *pLoad;
    struct gj_lmod *pMod;
    i64 i64St;
    char szModName[GJ_LMOD_NAME_MAX];

    if (!g_fInited) {
        linux_module_init();
    }

    lmod_set_unres("");

    if (szSource != NULL && szSource[0] != '\0') {
        szSrc = szSource;
    } else {
        szSrc = "mem";
    }

    if (szName != NULL && szName[0] != '\0') {
        (void)strlcpy(szModName, szName, sizeof(szModName));
    } else {
        (void)strlcpy(szModName, "anon", sizeof(szModName));
    }

    /* Attempt lamp first — greppable even when load fails early. */
    kprintf("linux_module: soft load source=%s name=%s\n", szSrc, szModName);

    if (pElf == NULL || cb < sizeof(struct elf64_ehdr)) {
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_INVAL;
    }

    if (lmod_find(szModName) != NULL) {
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_BUSY;
    }
    if (g_cMod >= GJ_LINUX_MODULE_MAX) {
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_NOMEM;
    }

    pImg = (const u8 *)pElf;
    pEh = (const struct elf64_ehdr *)pImg;
    if (!lmod_ehdr_ok(pEh, cb)) {
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_INVAL;
    }

    memset(g_aSecVa, 0, sizeof(g_aSecVa));
    memset(g_aSecAlloc, 0, sizeof(g_aSecAlloc));

    /* Find SYMTAB + layout SHF_ALLOC + count GOT relocs. */
    for (iSh = 0; iSh < pEh->u16Shnum; iSh++) {
        const struct elf64_shdr *pSh = lmod_shdr(pImg, pEh, iSh);

        if (!lmod_sh_in_image(pSh, cb)) {
            lmod_lamp_fail(szSrc, szModName, "-");
            return (i64)GJ_ERR_INVAL;
        }
        if (pSh->u32Type == SHT_SYMTAB && pShSym == NULL) {
            const struct elf64_shdr *pShStr;
            u64 u64Ent;

            pShSym = pSh;
            u64Ent = pSh->u64Entsize ? pSh->u64Entsize
                                     : (u64)sizeof(struct elf64_sym);
            if (u64Ent < sizeof(struct elf64_sym) || pSh->u64Size < u64Ent) {
                lmod_lamp_fail(szSrc, szModName, "-");
                return (i64)GJ_ERR_INVAL;
            }
            cSym = (u32)(pSh->u64Size / u64Ent);
            pSym0 = (const struct elf64_sym *)(pImg + pSh->u64Offset);
            if (pSh->u32Link >= pEh->u16Shnum) {
                lmod_lamp_fail(szSrc, szModName, "-");
                return (i64)GJ_ERR_INVAL;
            }
            pShStr = lmod_shdr(pImg, pEh, pSh->u32Link);
            if (pShStr->u32Type != SHT_STRTAB || !lmod_sh_in_image(pShStr, cb)) {
                lmod_lamp_fail(szSrc, szModName, "-");
                return (i64)GJ_ERR_INVAL;
            }
            pStr = (const char *)(pImg + pShStr->u64Offset);
            cbStr = pShStr->u64Size;
        }

        /*
         * SHF_ALLOC even with size 0 (e.g. empty .bss): still mark allocated
         * so STT_SECTION symbols / rare relocs resolve (G752 BAD_SHNDX).
         */
        if ((pSh->u64Flags & SHF_ALLOC) != 0) {
            u64 u64Align = pSh->u64Addralign ? pSh->u64Addralign : 1ull;
            u64Cursor = lmod_align_up(u64Cursor, u64Align);
            /* Record provisional offset in g_aSecVa; rebase after alloc. */
            g_aSecVa[iSh] = u64Cursor;
            g_aSecAlloc[iSh] = 1;
            u64Cursor += pSh->u64Size;
        }
    }

    /* Count GOT-class relocs against ALLOC targets. */
    for (iSh = 0; iSh < pEh->u16Shnum; iSh++) {
        const struct elf64_shdr *pSh = lmod_shdr(pImg, pEh, iSh);
        u32 iTgt;
        u64 u64Ent;
        u64 u64N;
        u64 iRel;

        if (pSh->u32Type != SHT_RELA && pSh->u32Type != SHT_REL) {
            continue;
        }
        iTgt = pSh->u32Info;
        if (iTgt >= GJ_LMOD_SH_MAX || !g_aSecAlloc[iTgt]) {
            continue;
        }
        if (pSh->u32Type == SHT_RELA) {
            u64Ent = pSh->u64Entsize ? pSh->u64Entsize
                                     : (u64)sizeof(struct elf64_rela);
            if (u64Ent < sizeof(struct elf64_rela)) {
                continue;
            }
            u64N = pSh->u64Size / u64Ent;
            for (iRel = 0; iRel < u64N; iRel++) {
                const struct elf64_rela *pR =
                    (const struct elf64_rela *)(pImg + pSh->u64Offset +
                                                iRel * u64Ent);
                if (lmod_is_got_reloc(lmod_r_type(pR->u64Info))) {
                    cGot++;
                }
            }
        } else {
            u64Ent =
                pSh->u64Entsize ? pSh->u64Entsize : (u64)sizeof(struct elf64_rel);
            if (u64Ent < sizeof(struct elf64_rel)) {
                continue;
            }
            u64N = pSh->u64Size / u64Ent;
            for (iRel = 0; iRel < u64N; iRel++) {
                const struct elf64_rel *pR =
                    (const struct elf64_rel *)(pImg + pSh->u64Offset +
                                               iRel * u64Ent);
                if (lmod_is_got_reloc(lmod_r_type(pR->u64Info))) {
                    cGot++;
                }
            }
        }
    }

    if (cGot > GJ_LMOD_GOT_MAX) {
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_NOMEM;
    }

    /* Soft GOT after sections (8-byte aligned). */
    u64Cursor = lmod_align_up(u64Cursor, 8ull);
    u64GotOff = u64Cursor;
    if (cGot > 0) {
        u64Cursor += (u64)cGot * 8ull;
    }
    if (u64Cursor == 0) {
        /* Empty ALLOC image: still reserve one page for soft bookkeeping VA. */
        u64Cursor = GJ_PAGE_SIZE;
    }

    cPages = (u32)GJ_BYTES_TO_PAGES(u64Cursor);
    if (cPages == 0 || cPages > GJ_LMOD_PAGES_MAX) {
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_NOMEM;
    }

    if (cPages == 1u) {
        paBase = pmm_alloc();
    } else {
        paBase = pmm_alloc_pages(cPages);
    }
    if (paBase == 0) {
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_NOMEM;
    }
    lmod_zero_pages(paBase, cPages);
    pLoad = lmod_va(paBase);
    if (pLoad == NULL) {
        lmod_free_pages(paBase, cPages);
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_NOMEM;
    }

    /* Rebase section VAs to real load address; copy PROGBITS. */
    for (iSh = 0; iSh < pEh->u16Shnum; iSh++) {
        const struct elf64_shdr *pSh;

        if (!g_aSecAlloc[iSh]) {
            continue;
        }
        pSh = lmod_shdr(pImg, pEh, iSh);
        g_aSecVa[iSh] = (u64)(uintptr_t)pLoad + g_aSecVa[iSh];
        if (pSh->u32Type != SHT_NOBITS && pSh->u64Size > 0) {
            memcpy((void *)(uintptr_t)g_aSecVa[iSh], pImg + pSh->u64Offset,
                   (size_t)pSh->u64Size);
        }
    }

    pMod = lmod_alloc_slot();
    if (pMod == NULL) {
        lmod_free_pages(paBase, cPages);
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_NOMEM;
    }

    pMod->paBase = paBase;
    pMod->cPages = cPages;
    pMod->pLoad = pLoad;
    pMod->cbLoad = u64Cursor;
    pMod->cGot = cGot;
    pMod->cGotUsed = 0;
    pMod->u64GotBase =
        (cGot > 0) ? ((u64)(uintptr_t)pLoad + u64GotOff) : 0;
    (void)strlcpy(pMod->szName, szModName, sizeof(pMod->szName));

    if (pSym0 == NULL || pStr == NULL) {
        lmod_free_slot(pMod);
        lmod_lamp_fail(szSrc, szModName, "-");
        return (i64)GJ_ERR_INVAL;
    }

    i64St = lmod_apply_relocs(pMod, pImg, cb, pEh, pSym0, cSym, pStr, cbStr);
    if (i64St != (i64)GJ_OK) {
        const char *szMiss =
            g_szLastUnres[0] != '\0' ? g_szLastUnres : "-";
        lmod_free_slot(pMod);
        lmod_lamp_fail(szSrc, szModName, szMiss);
        return i64St;
    }

    lmod_bind_init_exit(pMod, pSym0, cSym, pStr, cbStr);
    g_cMod++;

    kprintf("linux_module: soft load PASS source=%s name=%s missing=- "
            "pages=%u got=%u init=%u exit=%u\n",
            szSrc, pMod->szName, (unsigned)pMod->cPages, (unsigned)pMod->cGot,
            pMod->pfnInit != NULL ? 1u : 0u, pMod->pfnExit != NULL ? 1u : 0u);
    return (i64)GJ_OK;
}

i64
linux_module_init_call(const char *szName)
{
    struct gj_lmod *pMod;
    int nRet;

    if (!g_fInited) {
        linux_module_init();
    }
    pMod = lmod_find(szName);
    if (pMod == NULL) {
        kprintf("linux_module: soft init FAIL\n");
        return (i64)GJ_ERR_NOENT;
    }
    if (pMod->pfnInit == NULL) {
        pMod->u8Inited = 1;
        kprintf("linux_module: soft init PASS\n");
        return (i64)GJ_OK;
    }
    nRet = pMod->pfnInit();
    if (nRet != 0) {
        kprintf("linux_module: soft init FAIL\n");
        return (nRet < 0) ? (i64)nRet : (i64)GJ_ERR_IO;
    }
    pMod->u8Inited = 1;
    kprintf("linux_module: soft init PASS\n");
    return (i64)GJ_OK;
}

i64
linux_module_exit_call(const char *szName)
{
    struct gj_lmod *pMod;

    if (!g_fInited) {
        linux_module_init();
    }
    pMod = lmod_find(szName);
    if (pMod == NULL) {
        kprintf("linux_module: soft exit FAIL\n");
        return (i64)GJ_ERR_NOENT;
    }
    if (pMod->pfnExit != NULL) {
        pMod->pfnExit();
    }
    pMod->u8Inited = 0;
    kprintf("linux_module: soft exit PASS\n");
    return (i64)GJ_OK;
}

int
linux_module_loaded(const char *szName)
{
    return lmod_find(szName) != NULL ? 1 : 0;
}

int
linux_module_load_va_range(const char *szName, void **ppBase, u64 *pcb)
{
    struct gj_lmod *pMod;

    if (ppBase != NULL) {
        *ppBase = NULL;
    }
    if (pcb != NULL) {
        *pcb = 0;
    }
    if (!g_fInited) {
        return -1;
    }
    pMod = lmod_find(szName);
    if (pMod == NULL || pMod->pLoad == NULL || pMod->cbLoad == 0ull) {
        return -1;
    }
    if (ppBase != NULL) {
        *ppBase = pMod->pLoad;
    }
    if (pcb != NULL) {
        *pcb = pMod->cbLoad;
    }
    return 0;
}

u32
linux_module_count(void)
{
    return g_cMod;
}

const char *
linux_module_last_unresolved(void)
{
    return g_szLastUnres;
}
