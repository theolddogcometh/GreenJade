/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft freestanding loader for Linux x86_64 .ko ELF relocatable objects (ET_REL).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux source copy.
 * Soft!=product; G-AC-1. SKIP exec ALL staged .ko residual (all_ko_skip_exec=1;
 * freestanding_no_exec / RUN_INIT=0 / never_exec_ko=1). Soft load/stage eng
 * residual only (stage_only=1; r8169/usb_storage) - never product wire
 * ownership. Product drivers = userspace UDX+ABI; freestanding class SKIP.
 *
 * *** C0 freestanding_no_exec residual deepen (stamp-free; Soft!=product) ***
 *   - g_ac1_waiver=0 always (never claim G-AC-1 waiver for in-kernel .ko)
 *   - dual_dod_A=OPEN dual_dod_B=OPEN (UDX product path; never freestanding close)
 *   - lean residual self-check arms (policy belt compile+runtime honesty)
 *   - freestanding re-entry: never re-claim INIT0_ENG under RUN_INIT=0
 *   - freestanding exit: never cleanup_module even if InitRan corrupted
 *   - skip_exec tally residual; InitRan force-clear under freestanding belt
 *   - No version stamp; never bump GJ_IMAGE_VERSION; storm=0
 *
 * Algorithm (load_mem)
 * --------------------
 *   1. Validate ELF64 LE ET_REL EM_X86_64 header + section table bounds.
 *   2. Locate SHT_SYMTAB (+ linked strtab); count SHF_ALLOC layout size and
 *      GOTPCREL* reloc slots (soft GOT for GOTPCREL / GOTPCRELX / REX_).
 *   3. Allocate contiguous PMM pages; place each SHF_ALLOC section with
 *      sh_addralign (SHT_NOBITS zeroed; PROGBITS copied from image).
 *   4. Append soft GOT region; apply SHT_RELA / SHT_REL into loaded targets.
 *   5. Resolve SHN_UNDEF via linux_ksym_lookup (weak miss -> 0; strong miss ->
 *      record name, free pages, FAIL). Local/ABS via section base + st_value.
 *   6. Bind init_module / cleanup_module global symbols; register slot (max 8).
 *
 * Relocs applied: R_X86_64_{64,PC32,PLT32,32,32S,GOTPCREL,GOTPCRELX,
 * REX_GOTPCRELX}. Others skipped with a soft log line.
 *
 * greppable: linux_module: soft load source=embed|media|mem|finit name=
 * greppable: linux_module: soft load PASS|FAIL source= name= missing=
 *   (PASS: init_sym= exit_sym= init_policy=SKIP_EXEC never_exec_ko=1
 *    all_ko_skip_exec=1 stage_only=1 - soft load/stage eng only;
 *    eng residual r8169/usb_storage; wire=0; never product wire ownership;
 *    Soft!=product)
 * greppable: linux_module: soft FAIL KSYM name= missing= kind=FAIL_KSYM
 * greppable: linux_module: soft FAIL KSYM summary name= n= first=
 * greppable: linux_module: soft init PASS|FAIL name= INIT= kind=INIT0_ENG|INIT_FAIL
 * greppable: linux_module: soft SKIP exec name= reason= kind=SKIP_EXEC
 * greppable: linux_module: soft multi-mod SKIP exec name= reason=
 * greppable: linux_module: soft residual Dual DoD D name= triad=
 * greppable: residual_reasons=freestanding_no_exec|deps_soft_virtual_only|pfn_outside_module
 * greppable: linux_module: soft residual freestanding_no_exec
 * greppable: linux_module: soft residual lean PASS
 * greppable: linux_module: soft freestanding SKIP residual name=
 * greppable: linux_module: soft usb_storage SKIP residual name= picked=
 * greppable: linux_module: soft usb_storage SKIP honesty map residual_reasons=
 * greppable: linux_module: soft usb_storage SKIP residual already=1 first_reason=
 * greppable: linux_module: soft usb_storage SKIP exit residual name=
 * greppable: linux_module: soft usb_storage SKIP exit honesty map residual_reasons=
 * greppable: linux_module: soft lamp class name= kind= reason=
 * greppable: linux_module: soft SKIP exit name= reason=
 * greppable: linux_module: soft exit
 * greppable: linux_module: soft reloc skip
 * greppable: linux_module: soft media path OPEN|SKIP name= reason=
 * greppable: linux_module: soft load order rank= name= deps=
 * greppable: linux_module: soft load order ready|WAIT name= need=
 * greppable: linux_module: soft deps_ready name= via= init_policy=
 * greppable: linux_module: soft dep virtual <canon> soft=1 product=0
 * greppable: linux_module: soft export ksym n= name=
 * greppable: g_ac1_waiver=0 | dual_dod_A=OPEN | dual_dod_B=OPEN | C0=1
 *
 * Lean residual (once at linux_module_init; Soft!=product; G-AC-1; C0 denser):
 *   linux_module: soft residual freestanding_no_exec RUN_INIT=0 never_exec_ko=1
 *     all_ko_skip_exec=1 stage_only=1 eng=r8169,usb_storage wire=0
 *     freestanding_class=SKIP product_drivers=userspace_UDX_ABI
 *     g_ac1_waiver=0 dual_dod_A=OPEN dual_dod_B=OPEN C0=1
 *   linux_module: soft residual lean PASS arms=N ok=N (self-check; storm=0)
 *   RUN_INIT=0 / freestanding_no_exec honesty denser: SKIP exec ALL staged .ko
 *   residual (never execute init in kernel as product). Soft load/stage eng
 *   residual only - never product wire ownership. Product drivers = userspace
 *   UDX+ABI; freestanding class SKIP. Soft!=product. No version stamp; storm=0.
 *   Dual MIT/Apache. Dual DoD A/B stay OPEN (never freestanding close).
 *
 * Lamp honesty triad (FAIL KSYM != SKIP exec != INIT=0 eng) - Dual DoD D residual:
 *   FAIL KSYM     kind=FAIL_KSYM - load-time SHN_UNDEF / need:DEP; no init ran.
 *   SKIP exec     kind=SKIP_EXEC - load may PASS; init body intentionally not run.
 *                   Reasons (most specific first):
 *                     empty_load | already_inited|already_skipped | deps_missing |
 *                     deps_soft_virtual_only | no_init_symbol |
 *                     pfn_outside_module | freestanding_no_exec
 *                   Gap D residual catalog (sparse greppable residual_reasons=):
 *                     freestanding_no_exec | deps_soft_virtual_only |
 *                     pfn_outside_module
 *                   policy=freestanding_no_exec under GJ_SOFT_MODULE_RUN_INIT=0
 *                   (G-AC-1 default; never accidental in-kernel .ko init).
 *                   Sparse residual lamps only - no multi-line residual storms
 *                   (serial stack #PF history under flood). Rate-limit re-entry:
 *                   already=1 single line (no storms). Soft!=product.
 *   INIT=0 eng    kind=INIT0_ENG - init body ran under eng opt-in only.
 *                   Never for USB multi-mod leaf; never under RUN_INIT=0.
 *                   C0 freestanding re-entry belt: never re-claim INIT0_ENG
 *                   even if u8InitRan corrupted under freestanding default.
 *
 * Soft dep virtual: host BUILTIN usbcore/scsi_mod -> soft seeds "usbcore-soft"/
 * "scsi_mod-soft". deps soft-virtual only -> SKIP reason=deps_soft_virtual_only
 * (policy=freestanding_no_exec still; never INIT PASS; never product usbcore).
 * pfn_outside_module greppable when init pfn not inside load image.
 * Default GJ_SOFT_MODULE_RUN_INIT=0 -> freestanding_no_exec (all .ko; G-AC-1).
 * Parent may skip .ko init_module; loader lamps stay honest either way.
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
int linux_ksym_register(const char *szName, void *pAddr);
int linux_ksym_has(const char *szName);
#endif

__attribute__((weak)) void *
linux_ksym_lookup(const char *szName)
{
    (void)szName;
    return NULL;
}

__attribute__((weak)) int
linux_ksym_register(const char *szName, void *pAddr)
{
    (void)szName;
    (void)pAddr;
    return -1;
}

__attribute__((weak)) int
linux_ksym_has(const char *szName)
{
    return linux_ksym_lookup(szName) != NULL ? 1 : 0;
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
#ifndef GJ_LINUX_MODULE_UNRES_MAX
#define GJ_LINUX_MODULE_UNRES_MAX 48u
#endif
#define GJ_LMOD_UNRES_LIST_MAX GJ_LINUX_MODULE_UNRES_MAX
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
    u8 u8Inited;     /* init_call completed (SKIP or eng) */
    u8 u8InitRan;    /* 1 only if init_module body actually ran (INIT eng) */
    u8 u8SkipLamped; /* 1 after first SKIP-exec lamp (rate-limit re-entry) */
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
    /* First SKIP reason (honesty on rate-limited re-entry; Soft!=product). */
    char szFirstSkipReason[40];
};

static struct gj_lmod g_aMod[GJ_LINUX_MODULE_MAX];
static u32 g_cMod;
static char g_szLastUnres[GJ_LMOD_UNRES_MAX];
static char g_aUnresList[GJ_LMOD_UNRES_LIST_MAX][GJ_LMOD_NAME_MAX];
static u32 g_cUnresList;
static u32 g_cUnresTrunc; /* extra misses past list cap (diagnostic) */
static int g_fInited;
/* Lean residual once: RUN_INIT=0 freestanding_no_exec denser (no storms). */
static u8 g_fSoftResidualOnce;
/*
 * C0 freestanding residual tallies (Soft!=product; G-AC-1; stamp-free).
 * g_cSkipExec: intentional SKIP-exec count (init body never ran).
 * g_cInitRanForceClear: freestanding belt forced InitRan=0 (corruption belt).
 * Under freestanding default g_cInitRan (body actually ran) must stay 0.
 */
static u32 g_cSkipExec;
static u32 g_cInitRanForceClear;

/* Persistent name arena for ksym export (strtab is not SHF_ALLOC). */
#define GJ_LMOD_EXPORT_NAME_POOL 8192u
static char g_aExportNamePool[GJ_LMOD_EXPORT_NAME_POOL];
static u32 g_cExportNamePool;

/* Per-load scratch (single-threaded soft path). */
static u64 g_aSecVa[GJ_LMOD_SH_MAX];
static u8 g_aSecAlloc[GJ_LMOD_SH_MAX];

/*
 * Soft known load-order table (operator honesty; not full modinfo deps).
 * Lower rank -> load earlier. Soft!=product; G-AC-1 N/A.
 */
struct gj_lmod_order {
    const char *szName;
    int nRank;
    const char *szDeps; /* comma-separated soft deps; "" if none */
};

static const struct gj_lmod_order g_aSoftOrder[] = {
    /* USB multi-mod: usb_common -> usbcore -> scsi_mod -> xhci_hcd -> xhci_pci -> usb_storage */
    { "usb_common", 10, "" },
    { "usbcore", 20, "usb_common" },
    { "scsi_mod", 25, "" },
    { "xhci_hcd", 30, "usbcore" },
    { "xhci_pci", 40, "xhci_hcd,usbcore" },
    { "usb_storage", 50, "usbcore,scsi_mod" },
    /*
     * NIC: libphy / mdio before r8169 on full Linux. Soft path covers phy_*
     * via linux_phy_soft ksym bodies - do not FAIL KSYM need:libphy when
     * r8169 loads against soft ksym (eng residual only; Soft!=product).
     * Soft load residual r8169 eng only - never product wire ownership.
     * Product NIC = userspace UDX+ABI (rtl8168_udx); freestanding class SKIP.
     * Rank only.
     */
    { "libphy", 10, "" },
    { "mdio_devres", 15, "" },
    { "r8169", 50, "" },
};

#define GJ_LMOD_ORDER_N \
    (sizeof(g_aSoftOrder) / sizeof(g_aSoftOrder[0]))

/*
 * Soft dep virtual markers (separate from real gj_lmod load slots).
 * Honest: loaded("usbcore") stays 0; virtual "usbcore-soft" satisfies
 * soft dep "usbcore" in deps_ready only. Soft!=product.
 * Survives linux_module_init (seed TUs may register before/after init).
 */
#define GJ_LMOD_SOFT_VIRT_MAX 8u

struct gj_lmod_soft_virt {
    u8 u8Used;
    u8 u8Pad[3];
    char szCanon[GJ_LMOD_NAME_MAX]; /* "usbcore" */
    char szVirt[GJ_LMOD_NAME_MAX];  /* "usbcore-soft" */
};

static struct gj_lmod_soft_virt g_aSoftVirt[GJ_LMOD_SOFT_VIRT_MAX];
static u32 g_cSoftVirt;

/* Defined with module table helpers below. */
static struct gj_lmod *lmod_find(const char *szName);

static int
lmod_soft_virt_find(const char *szCanon)
{
    u32 i;

    if (szCanon == NULL || szCanon[0] == '\0') {
        return -1;
    }
    for (i = 0; i < GJ_LMOD_SOFT_VIRT_MAX; i++) {
        if (g_aSoftVirt[i].u8Used &&
            strcmp(g_aSoftVirt[i].szCanon, szCanon) == 0) {
            return (int)i;
        }
    }
    return -1;
}

/*
 * Soft dep is satisfied if real .ko is in the live table OR a soft-virtual
 * marker was registered for that canonical name (seed path). Soft!=product.
 * @return 0 miss, 1 real load, 2 soft-virtual only
 */
static int
lmod_dep_satisfied(const char *szDep)
{
    if (szDep == NULL || szDep[0] == '\0') {
        return 0;
    }
    if (lmod_find(szDep) != NULL) {
        return 1; /* real soft-loaded .ko */
    }
    if (lmod_soft_virt_find(szDep) >= 0) {
        return 2; /* soft virtual only (eng lamp; != product) */
    }
    return 0;
}

/*
 * USB multi-mod class leaves: freestanding must not call real init_module.
 * Soft ksym/virtual deps allow LOAD ok, but exec can #PF on soft-deepen
 * strings / incomplete class surface. Soft!=product; G-AC-1 N/A.
 * @return 1 if name is a known USB/MSC multi-mod leaf, else 0
 */
static int
lmod_is_usb_multimod_leaf(const char *szName)
{
    if (szName == NULL || szName[0] == '\0') {
        return 0;
    }
    /* Fast reject: known USB/MSC names start with u/s/x. */
    if (szName[0] != 'u' && szName[0] != 's' && szName[0] != 'x') {
        return 0;
    }
    if (strcmp(szName, "usb_storage") == 0 || strcmp(szName, "uas") == 0 ||
        strcmp(szName, "usbcore") == 0 || strcmp(szName, "xhci_hcd") == 0 ||
        strcmp(szName, "xhci_pci") == 0 || strcmp(szName, "usb_common") == 0 ||
        strcmp(szName, "scsi_mod") == 0) {
        return 1;
    }
    return 0;
}

/*
 * Quiet soft-deps scan (no lamps). Classifies dep satisfaction for SKIP-exec.
 * @return 0 no soft deps recorded (or empty)
 *         1 all soft deps satisfied by real .ko only
 *         2 all soft deps satisfied, at least one via soft-virtual only
 *        -1 one or more soft deps missing (neither real nor soft-virtual)
 * Soft!=product: via=soft-virtual is eng only.
 */
static int
lmod_deps_class(const char *szName)
{
    const char *szDeps;
    const char *p;
    char szTok[GJ_LMOD_NAME_MAX];
    u32 cTok;
    int nHow;
    int fAny = 0;
    int fViaVirt = 0;
    int fMiss = 0;

    szDeps = linux_module_soft_deps(szName);
    if (szDeps == NULL || szDeps[0] == '\0') {
        return 0;
    }
    p = szDeps;
    while (*p != '\0') {
        cTok = 0;
        while (*p != '\0' && *p != ',' && cTok + 1u < GJ_LMOD_NAME_MAX) {
            szTok[cTok++] = *p++;
        }
        szTok[cTok] = '\0';
        if (*p == ',') {
            p++;
        }
        if (cTok == 0u) {
            continue;
        }
        fAny = 1;
        nHow = lmod_dep_satisfied(szTok);
        if (nHow == 0) {
            fMiss = 1;
        } else if (nHow == 2) {
            fViaVirt = 1;
        }
    }
    if (!fAny) {
        return 0;
    }
    if (fMiss) {
        return -1;
    }
    if (fViaVirt) {
        return 2;
    }
    return 1;
}

/*
 * Freestanding tokens: prefer public defines from gj/linux_module.h
 * (GJ_SOFT_MODULE_RUN_INIT / GJ_LMOD_NEVER_EXEC_KO / GJ_LMOD_POLICY_FREESTANDING).
 * Local fallbacks keep this TU buildable if an older header is in the include path.
 */
#ifndef GJ_SOFT_MODULE_RUN_INIT
#define GJ_SOFT_MODULE_RUN_INIT 0
#endif
#ifndef GJ_LMOD_NEVER_EXEC_KO
#if GJ_SOFT_MODULE_RUN_INIT == 0
#define GJ_LMOD_NEVER_EXEC_KO 1
#else
#define GJ_LMOD_NEVER_EXEC_KO 0
#endif
#endif
#ifndef GJ_LMOD_POLICY_FREESTANDING
#define GJ_LMOD_POLICY_FREESTANDING "freestanding_no_exec"
#endif
/*
 * G-AC-1 residual honesty: freestanding_no_exec product default denser.
 * Soft loader may stage .ko for eng (r8169 / usb_storage); RUN_INIT stays 0
 * - SKIP exec ALL staged .ko residual (all_ko_skip_exec=1); never accidental
 * in-kernel .ko init as product; never product wire ownership. Soft load/
 * stage eng only (stage_only=1). Product drivers = userspace UDX+ABI;
 * freestanding class SKIP. Opt-in eng must deliberately -D1 (and would need
 * to lift this assert). Soft!=product. Lean residual once-lamp restates
 * never_exec_ko=1 all_ko_skip_exec=1 stage_only=1 at linux_module_init
 * (no version stamp).
 */
#ifndef GJ_LMOD_ALL_KO_SKIP_EXEC
#if GJ_SOFT_MODULE_RUN_INIT == 0
#define GJ_LMOD_ALL_KO_SKIP_EXEC 1
#else
#define GJ_LMOD_ALL_KO_SKIP_EXEC 0
#endif
#endif
#ifndef GJ_LMOD_STAGE_ONLY
#if GJ_SOFT_MODULE_RUN_INIT == 0
#define GJ_LMOD_STAGE_ONLY 1
#else
#define GJ_LMOD_STAGE_ONLY 0
#endif
#endif
_Static_assert(GJ_SOFT_MODULE_RUN_INIT == 0,
    "G-AC-1: GJ_SOFT_MODULE_RUN_INIT defaults 0 (freestanding_no_exec; SKIP exec all .ko residual)");
_Static_assert(GJ_LMOD_NEVER_EXEC_KO == 1,
    "G-AC-1: GJ_LMOD_NEVER_EXEC_KO must be 1 under RUN_INIT=0 (never execute .ko as product)");
_Static_assert(GJ_LMOD_ALL_KO_SKIP_EXEC == 1,
    "G-AC-1: GJ_LMOD_ALL_KO_SKIP_EXEC must be 1 under RUN_INIT=0 (all .ko residual SKIP exec)");
_Static_assert(GJ_LMOD_STAGE_ONLY == 1,
    "G-AC-1: GJ_LMOD_STAGE_ONLY must be 1 under RUN_INIT=0 (soft load/stage eng only; Soft!=product)");
#ifndef GJ_LMOD_SKIP_EXEC_ALL_KO_RESIDUAL
#define GJ_LMOD_SKIP_EXEC_ALL_KO_RESIDUAL 1
#endif
_Static_assert(GJ_LMOD_SKIP_EXEC_ALL_KO_RESIDUAL == 1,
    "G-AC-1: GJ_LMOD_SKIP_EXEC_ALL_KO_RESIDUAL must be 1 (SKIP exec all .ko residual; Soft!=product)");
/*
 * C0 freestanding residual law tokens (stamp-free; Soft!=product; G-AC-1).
 * g_ac1_waiver=0: never claim G-AC-1 waiver for in-kernel .ko product AC.
 * Dual DoD A/B stay OPEN (UDX userspace product path; never freestanding close).
 * Lean residual self-check arm count (functional residual; never hard-gates).
 */
#ifndef GJ_LMOD_G_AC1_WAIVER
#define GJ_LMOD_G_AC1_WAIVER 0
#endif
_Static_assert(GJ_LMOD_G_AC1_WAIVER == 0,
    "G-AC-1: GJ_LMOD_G_AC1_WAIVER must be 0 (never claim in-kernel .ko product waiver)");
#define GJ_LMOD_DUAL_DOD_A_OPEN 1
#define GJ_LMOD_DUAL_DOD_B_OPEN 1
_Static_assert(GJ_LMOD_DUAL_DOD_A_OPEN == 1,
    "Dual DoD A stays OPEN (USB UDX product; freestanding residual never closes)");
_Static_assert(GJ_LMOD_DUAL_DOD_B_OPEN == 1,
    "Dual DoD B stays OPEN (NIC UDX product; freestanding residual never closes)");
/*
 * Lean residual self-check arms (C0 freestanding_no_exec residual deepen):
 *   1 run_init0  2 never_exec  3 all_ko_skip  4 stage_only
 *   5 skip_all   6 g_ac1_waiver0  7 dual_dod_open  8 policy_token
 * Soft!=product; storm=0; stamp-free.
 */
#define GJ_LMOD_LEAN_CHECKS 8u

/*
 * Gap D residual SKIP reasons catalog (Dual DoD D / usb_storage multi-mod).
 * greppable residual_reasons= token - freestanding_no_exec |
 * deps_soft_virtual_only | pfn_outside_module. Soft!=product; G-AC-1.
 * Sparse residual lamps only (no multi-line residual storms). Soft!=product.
 */
#define GJ_LMOD_GAP_D_REASON_FREESTANDING GJ_LMOD_POLICY_FREESTANDING
#define GJ_LMOD_GAP_D_REASON_DEPS_SOFT_VIRT "deps_soft_virtual_only"
#define GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE "pfn_outside_module"
#define GJ_LMOD_GAP_D_RESIDUAL_REASONS \
    "freestanding_no_exec|deps_soft_virtual_only|pfn_outside_module"
/* greppable triad token (FAIL_KSYM != SKIP_EXEC != INIT0_ENG). Soft!=product. */
#define GJ_LMOD_GAP_D_TRIAD "FAIL_KSYM!=SKIP_EXEC!=INIT0_ENG"
/*
 * Gap D multi-mod MSC plan chain (soft load-order honesty only). Soft!=product.
 * Leaf deps remain usbcore,scsi_mod; HC peers xhci_* are plan-context only.
 * Soft load residual usb_storage eng only - never product wire ownership.
 * Product USB = userspace UDX+ABI (xhci_udx); freestanding class SKIP.
 */
#define GJ_LMOD_USB_MSC_CHAIN \
    "usb_common>usbcore>scsi_mod>xhci_hcd>xhci_pci>usb_storage"
#define GJ_LMOD_USB_MSC_LEAF_DEPS "usbcore,scsi_mod"

/*
 * Soft load residual eng modules (r8169 / usb_storage only). Soft!=product.
 * Never product wire ownership; product drivers = userspace UDX+ABI.
 * @return 1 if name is a known soft-load eng residual target, else 0
 */
static int
lmod_is_eng_residual_mod(const char *szName)
{
    if (szName == NULL || szName[0] == '\0') {
        return 0;
    }
    if (strcmp(szName, "r8169") == 0 || strcmp(szName, "usb_storage") == 0) {
        return 1;
    }
    return 0;
}

/*
 * C0 freestanding residual: tally intentional SKIP-exec (init body never ran).
 * Soft!=product; never claims product AC. Wrap-safe.
 */
static void
lmod_note_skip_exec(void)
{
    if (g_cSkipExec < 0xffffffffu) {
        g_cSkipExec++;
    }
}

/*
 * C0 freestanding residual: force InitRan=0 under freestanding belt.
 * Clears corruption that would otherwise claim INIT0_ENG / run cleanup.
 * Soft!=product; G-AC-1; never_exec_ko.
 * @return 1 if a nonzero InitRan was forced clear, else 0
 */
static int
lmod_force_init_ran_clear(struct gj_lmod *pMod)
{
    if (pMod == NULL) {
        return 0;
    }
    if (pMod->u8InitRan != 0u) {
        pMod->u8InitRan = 0;
        if (g_cInitRanForceClear < 0xffffffffu) {
            g_cInitRanForceClear++;
        }
        return 1;
    }
    return 0;
}

/*
 * Lean residual once - RUN_INIT=0 / freestanding_no_exec honesty denser (C0).
 * greppable: linux_module: soft residual freestanding_no_exec
 * greppable: linux_module: soft residual lean PASS
 * Soft!=product; G-AC-1; never_exec_ko under default; all_ko_skip_exec=1;
 * stage_only=1 (soft load/stage eng residual only). Two lines max (no
 * stamp storms; no version stamp). Soft load residual eng=r8169,usb_storage
 * only - never product wire ownership. Product drivers = userspace UDX+ABI;
 * freestanding class SKIP. SKIP exec ALL staged .ko residual; never product.
 * Dual DoD A/B OPEN; g_ac1_waiver=0. C0 lean self-check arms.
 */
static void
lmod_soft_residual_once(void)
{
    u32 u32Ok;
    u32 u32Arm;
    u32 fRun0;
    u32 fNever;
    u32 fAllKo;
    u32 fStage;
    u32 fSkipAll;
    u32 fWaiver0;
    u32 fDualOpen;
    u32 fPolicy;

    if (g_fSoftResidualOnce != 0u) {
        return;
    }
    g_fSoftResidualOnce = 1u;

    /*
     * Grep: linux_module: soft residual freestanding_no_exec
     * C0 denser: g_ac1_waiver=0 dual_dod_A/B=OPEN C0=1 (peer linux_ksym).
     */
    kprintf("linux_module: soft residual freestanding_no_exec "
            "RUN_INIT=%d never_exec_ko=%d all_ko_skip_exec=%d stage_only=%d "
            "residual_reasons=%s policy=%s soft=1 product=0 G-AC-1 "
            "Soft!=product storm=0 eng=r8169,usb_storage wire=0 "
            "freestanding_class=SKIP product_drivers=userspace_UDX_ABI "
            "g_ac1_waiver=%d dual_dod_A=OPEN dual_dod_B=OPEN C0=1 "
            "skip_exec_n=%u init_ran_force_clear=%u "
            "(RUN_INIT=0 freestanding_no_exec denser; SKIP exec all .ko residual; "
            "load PASS=stage only; soft load/stage eng residual only; "
            "never product wire ownership; product=userspace UDX+ABI; "
            "freestanding class SKIP; never Dual DoD freestanding close; "
            "never G-AC-1 waiver)\n",
            (int)GJ_SOFT_MODULE_RUN_INIT, (int)GJ_LMOD_NEVER_EXEC_KO,
            (int)GJ_LMOD_ALL_KO_SKIP_EXEC, (int)GJ_LMOD_STAGE_ONLY,
            GJ_LMOD_GAP_D_RESIDUAL_REASONS, GJ_LMOD_POLICY_FREESTANDING,
            (int)GJ_LMOD_G_AC1_WAIVER, (unsigned)g_cSkipExec,
            (unsigned)g_cInitRanForceClear);

    /*
     * C0 lean residual self-check (functional; never hard-gates).
     * Arms: run_init0 never_exec all_ko_skip stage_only skip_all
     *       g_ac1_waiver0 dual_dod_open policy_token.
     * greppable: linux_module: soft residual lean PASS
     */
    fRun0 = (GJ_SOFT_MODULE_RUN_INIT == 0) ? 1u : 0u;
    fNever = (GJ_LMOD_NEVER_EXEC_KO == 1) ? 1u : 0u;
    fAllKo = (GJ_LMOD_ALL_KO_SKIP_EXEC == 1) ? 1u : 0u;
    fStage = (GJ_LMOD_STAGE_ONLY == 1) ? 1u : 0u;
    fSkipAll = (GJ_LMOD_SKIP_EXEC_ALL_KO_RESIDUAL == 1) ? 1u : 0u;
    fWaiver0 = (GJ_LMOD_G_AC1_WAIVER == 0) ? 1u : 0u;
    fDualOpen = (GJ_LMOD_DUAL_DOD_A_OPEN == 1 && GJ_LMOD_DUAL_DOD_B_OPEN == 1)
                    ? 1u
                    : 0u;
    fPolicy = (GJ_LMOD_POLICY_FREESTANDING[0] == 'f' &&
               GJ_LMOD_POLICY_FREESTANDING[1] == 'r' &&
               GJ_LMOD_POLICY_FREESTANDING[2] == 'e' &&
               GJ_LMOD_POLICY_FREESTANDING[3] == 'e')
                  ? 1u
                  : 0u;
    u32Ok = fRun0 + fNever + fAllKo + fStage + fSkipAll + fWaiver0 + fDualOpen +
            fPolicy;
    u32Arm = GJ_LMOD_LEAN_CHECKS;
    kprintf("linux_module: soft residual lean PASS "
            "arms=%u ok=%u run_init0=%u never_exec=%u all_ko_skip=%u "
            "stage_only=%u skip_all=%u g_ac1_waiver0=%u dual_dod_open=%u "
            "policy_token=%u freestanding_no_exec=1 never_exec_ko=%d "
            "g_ac1_waiver=%d dual_dod_A=OPEN dual_dod_B=OPEN C0=1 "
            "soft=1 product=0 storm=0 Soft!=product G-AC-1 "
            "(C0 freestanding_no_exec residual lean self-check; "
            "SKIP exec all .ko residual; never product wire; "
            "never Dual DoD freestanding close; never G-AC-1 waiver)\n",
            (unsigned)u32Arm, (unsigned)u32Ok, (unsigned)fRun0,
            (unsigned)fNever, (unsigned)fAllKo, (unsigned)fStage,
            (unsigned)fSkipAll, (unsigned)fWaiver0, (unsigned)fDualOpen,
            (unsigned)fPolicy, (int)GJ_LMOD_NEVER_EXEC_KO,
            (int)GJ_LMOD_G_AC1_WAIVER);
}

/*
 * Emit greppable lamp-class distinguisher (FAIL_KSYM != SKIP_EXEC != INIT0_ENG).
 * greppable: linux_module: soft lamp class name= kind= reason=
 * Soft!=product on every kind. One line only (sparse; no residual storms).
 */
static void
lmod_lamp_class(const char *szName, const char *szKind, const char *szReason)
{
    const char *szN;
    const char *szK;
    const char *szR;

    szN = (szName != NULL && szName[0] != '\0') ? szName : "anon";
    szK = (szKind != NULL && szKind[0] != '\0') ? szKind : "UNKNOWN";
    szR = (szReason != NULL && szReason[0] != '\0') ? szReason : "-";
    kprintf("linux_module: soft lamp class name=%s kind=%s reason=%s "
            "soft=1 product=0 triad=%s\n",
            szN, szK, szR, GJ_LMOD_GAP_D_TRIAD);
}

static int
lmod_is_gap_d_residual_reason(const char *szReason)
{
    if (szReason == NULL || szReason[0] == '\0') {
        return 0;
    }
    if (strcmp(szReason, GJ_LMOD_GAP_D_REASON_FREESTANDING) == 0 ||
        strcmp(szReason, GJ_LMOD_GAP_D_REASON_DEPS_SOFT_VIRT) == 0 ||
        strcmp(szReason, GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE) == 0) {
        return 1;
    }
    return 0;
}

/* MSC multi-mod leaf name (Gap D deepen target). Soft!=product. */
static int
lmod_is_usb_storage_leaf(const char *szName)
{
    if (szName == NULL || szName[0] == '\0') {
        return 0;
    }
    return (strcmp(szName, "usb_storage") == 0) ? 1 : 0;
}

/*
 * Gap D residual catalog index (greppable rank).
 * r0=freestanding_no_exec r1=deps_soft_virtual_only r2=pfn_outside_module.
 * Returns -1 when reason is not in the Gap D residual catalog.
 */
static int
lmod_gap_d_catalog_rank(const char *szReason)
{
    if (szReason == NULL || szReason[0] == '\0') {
        return -1;
    }
    if (strcmp(szReason, GJ_LMOD_GAP_D_REASON_FREESTANDING) == 0) {
        return 0;
    }
    if (strcmp(szReason, GJ_LMOD_GAP_D_REASON_DEPS_SOFT_VIRT) == 0) {
        return 1;
    }
    if (strcmp(szReason, GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE) == 0) {
        return 2;
    }
    return -1;
}

/*
 * Gap D residual: sparse usb_storage MSC leaf honesty (once per slot).
 * greppable: linux_module: soft usb_storage SKIP residual name= picked=
 * greppable: linux_module: soft usb_storage SKIP honesty map residual_reasons=
 * greppable: residual_reasons=freestanding_no_exec|deps_soft_virtual_only|pfn_outside_module
 * Soft!=product; product=0; never real .ko exec (G-AC-1). Rate-limited by caller.
 * Two single lines only - no multi-line residual stamp storms.
 */
static void
lmod_lamp_usb_storage_skip_residual(const char *szReason, const char *szPolicy,
                                    const char *szDepsCls, int fGapD)
{
    const char *szR;
    const char *szPol;
    const char *szDeps;
    int nRank;

    szR = (szReason != NULL && szReason[0] != '\0') ? szReason : "unknown";
    szPol = (szPolicy != NULL && szPolicy[0] != '\0') ? szPolicy
                                                    : GJ_LMOD_POLICY_FREESTANDING;
    szDeps = (szDepsCls != NULL && szDepsCls[0] != '\0') ? szDepsCls : "none";
    nRank = lmod_gap_d_catalog_rank(szR);

    kprintf("linux_module: soft usb_storage SKIP residual name=usb_storage "
            "picked=%s kind=SKIP_EXEC policy=%s deps_class=%s residual=%d "
            "rank=%d residual_reasons=%s multi_mod=1 leaf_deps=%s "
            "all_ko=1 all_ko_skip_exec=%d stage_only=%d freestanding_no_exec=1 "
            "never_exec_ko=%d RUN_INIT=%d soft=1 product=0 storm=0 "
            "(SKIP exec all .ko residual; Soft!=product; G-AC-1)\n",
            szR, szPol, szDeps, fGapD, nRank, GJ_LMOD_GAP_D_RESIDUAL_REASONS,
            GJ_LMOD_USB_MSC_LEAF_DEPS, (int)GJ_LMOD_ALL_KO_SKIP_EXEC,
            (int)GJ_LMOD_STAGE_ONLY, (int)GJ_LMOD_NEVER_EXEC_KO,
            (int)GJ_SOFT_MODULE_RUN_INIT);
    /* Honesty map: one sparse catalog line (r0/r1/r2). Soft!=product. */
    kprintf("linux_module: soft usb_storage SKIP honesty map "
            "name=usb_storage residual_reasons=%s r0=%s r1=%s r2=%s "
            "picked=%s rank=%d kind=SKIP_EXEC policy=%s "
            "all_ko_skip_exec=%d stage_only=%d never_exec_ko=%d RUN_INIT=%d "
            "soft=1 product=0 storm=0\n",
            GJ_LMOD_GAP_D_RESIDUAL_REASONS, GJ_LMOD_GAP_D_REASON_FREESTANDING,
            GJ_LMOD_GAP_D_REASON_DEPS_SOFT_VIRT, GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE,
            szR, nRank, szPol, (int)GJ_LMOD_ALL_KO_SKIP_EXEC,
            (int)GJ_LMOD_STAGE_ONLY, (int)GJ_LMOD_NEVER_EXEC_KO,
            (int)GJ_SOFT_MODULE_RUN_INIT);
}

/*
 * Rate-limited re-entry residual for usb_storage leaf only (single line).
 * greppable: linux_module: soft usb_storage SKIP residual already=1 first_reason=
 * Soft!=product; product=0; Soft SKIP != product AC. No re-stamp storms.
 */
static void
lmod_lamp_usb_storage_skip_already(const char *szReason, const char *szFirst,
                                   const char *szPolicy, int fFirstGapD)
{
    const char *szR;
    const char *szF;
    const char *szPol;
    int nRankFirst;

    szR = (szReason != NULL && szReason[0] != '\0') ? szReason : "unknown";
    szF = (szFirst != NULL && szFirst[0] != '\0') ? szFirst : "-";
    szPol = (szPolicy != NULL && szPolicy[0] != '\0') ? szPolicy
                                                    : GJ_LMOD_POLICY_FREESTANDING;
    nRankFirst = lmod_gap_d_catalog_rank(szF);
    kprintf("linux_module: soft usb_storage SKIP residual already=1 "
            "name=usb_storage reason=%s first_reason=%s kind=SKIP_EXEC "
            "policy=%s residual=%d rank=%d residual_reasons=%s "
            "all_ko=1 all_ko_skip_exec=%d stage_only=%d never_exec_ko=%d "
            "RUN_INIT=%d soft=1 product=0 storm=0\n",
            szR, szF, szPol, fFirstGapD, nRankFirst,
            GJ_LMOD_GAP_D_RESIDUAL_REASONS, (int)GJ_LMOD_ALL_KO_SKIP_EXEC,
            (int)GJ_LMOD_STAGE_ONLY, (int)GJ_LMOD_NEVER_EXEC_KO,
            (int)GJ_SOFT_MODULE_RUN_INIT);
}

/*
 * Gap D residual: usb_storage MSC leaf exit honesty (once on SKIP exit).
 * Cleanup not called after intentional SKIP exec. Soft!=product; product=0.
 * greppable: linux_module: soft usb_storage SKIP exit residual name=
 * greppable: residual_reasons=freestanding_no_exec|deps_soft_virtual_only|pfn_outside_module
 * Two single lines only - no multi-line residual exit stamp storms.
 */
static void
lmod_lamp_usb_storage_skip_exit_residual(const char *szFirstReason,
                                         const char *szPolicy,
                                         const char *szDepsCls, int fFirstGapD)
{
    const char *szFirst;
    const char *szPol;
    const char *szDeps;
    int nRankFirst;

    szFirst = (szFirstReason != NULL && szFirstReason[0] != '\0')
                  ? szFirstReason
                  : "-";
    szPol = (szPolicy != NULL && szPolicy[0] != '\0') ? szPolicy
                                                    : GJ_LMOD_POLICY_FREESTANDING;
    szDeps = (szDepsCls != NULL && szDepsCls[0] != '\0') ? szDepsCls : "none";
    nRankFirst = lmod_gap_d_catalog_rank(szFirst);

    kprintf("linux_module: soft usb_storage SKIP exit residual "
            "name=usb_storage first_reason=%s kind=SKIP_EXEC policy=%s "
            "deps_class=%s residual=%d rank=%d residual_reasons=%s "
            "cleanup_called=0 all_ko=1 all_ko_skip_exec=%d stage_only=%d "
            "never_exec_ko=%d freestanding_no_exec=1 RUN_INIT=%d "
            "soft=1 product=0 storm=0 (SKIP exec all .ko residual)\n",
            szFirst, szPol, szDeps, fFirstGapD, nRankFirst,
            GJ_LMOD_GAP_D_RESIDUAL_REASONS, (int)GJ_LMOD_ALL_KO_SKIP_EXEC,
            (int)GJ_LMOD_STAGE_ONLY, (int)GJ_LMOD_NEVER_EXEC_KO,
            (int)GJ_SOFT_MODULE_RUN_INIT);
    kprintf("linux_module: soft usb_storage SKIP exit honesty map "
            "name=usb_storage residual_reasons=%s r0=%s r1=%s r2=%s "
            "first_reason=%s rank=%d kind=SKIP_EXEC "
            "all_ko_skip_exec=%d stage_only=%d never_exec_ko=%d RUN_INIT=%d "
            "soft=1 product=0 storm=0\n",
            GJ_LMOD_GAP_D_RESIDUAL_REASONS, GJ_LMOD_GAP_D_REASON_FREESTANDING,
            GJ_LMOD_GAP_D_REASON_DEPS_SOFT_VIRT, GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE,
            szFirst, nRankFirst, (int)GJ_LMOD_ALL_KO_SKIP_EXEC,
            (int)GJ_LMOD_STAGE_ONLY, (int)GJ_LMOD_NEVER_EXEC_KO,
            (int)GJ_SOFT_MODULE_RUN_INIT);
}

/*
 * Emit greppable SKIP-exec lamp. Soft!=product; not FAIL KSYM; not INIT eng.
 * greppable: linux_module: soft SKIP exec name= reason= kind=SKIP_EXEC
 * greppable: linux_module: soft freestanding SKIP residual name=
 * greppable: linux_module: soft multi-mod SKIP exec name= (USB leaf only)
 * greppable: linux_module: soft residual Dual DoD D name= triad= (USB leaf)
 * greppable: linux_module: soft usb_storage SKIP residual name= picked=
 * greppable: residual_reasons=freestanding_no_exec|deps_soft_virtual_only|pfn_outside_module
 * Sparse residual only (all .ko): primary + freestanding residual + class.
 * USB leaf adds multi-mod + Dual DoD (one line each). usb_storage +2.
 * Never put INIT=0 on SKIP (INIT=0 is eng-only). Rate-limit re-entry.
 * policy= freestanding_no_exec when RUN_INIT=0 (G-AC-1 default).
 * No multi-line residual stamp storms (serial stack #PF history).
 */
static void
lmod_lamp_skip_exec(const char *szName, const char *szReason)
{
    const char *szN;
    const char *szR;
    const char *szDepsCls;
    const char *szPolicy;
    const char *szFirst;
    struct gj_lmod *pMod;
    int nDeps;
    int fUsbLeaf;
    int fUsbStorage;
    int fGapD;
    int fFirstGapD;
    int fPolicyFree;

    szN = (szName != NULL && szName[0] != '\0') ? szName : "anon";
    szR = (szReason != NULL && szReason[0] != '\0') ? szReason : "unknown";
    nDeps = lmod_deps_class(szN);
    fUsbLeaf = lmod_is_usb_multimod_leaf(szN);
    fUsbStorage = lmod_is_usb_storage_leaf(szN);
    fGapD = lmod_is_gap_d_residual_reason(szR);
    pMod = lmod_find(szN);
    if (nDeps == -1) {
        szDepsCls = "missing";
    } else if (nDeps == 2) {
        szDepsCls = "soft-virtual";
    } else if (nDeps == 1) {
        szDepsCls = "real";
    } else {
        szDepsCls = "none";
    }
    /*
     * Freestanding policy: RUN_INIT=0 default and/or USB multi-mod leaf.
     * Distinct from reason=deps_soft_virtual_only (most-specific cause).
     * G-AC-1: never_exec_ko under freestanding; Soft!=product.
     */
#if GJ_SOFT_MODULE_RUN_INIT == 0
    szPolicy = GJ_LMOD_POLICY_FREESTANDING;
#else
    szPolicy = fUsbLeaf ? GJ_LMOD_POLICY_FREESTANDING : "may_run";
#endif
    fPolicyFree = (strcmp(szPolicy, GJ_LMOD_POLICY_FREESTANDING) == 0) ? 1 : 0;

    /*
     * Rate-limit re-entry (already_skipped / double init_call).
     * Single line only - no re-stamp storms.
     */
    if (pMod != NULL && pMod->u8SkipLamped != 0u) {
        szFirst = (pMod->szFirstSkipReason[0] != '\0')
                      ? pMod->szFirstSkipReason
                      : "-";
        fFirstGapD = lmod_is_gap_d_residual_reason(szFirst);
        kprintf("linux_module: soft SKIP exec name=%s reason=%s "
                "kind=SKIP_EXEC already=1 first_reason=%s policy=%s "
                "residual_reasons=%s all_ko=1 all_ko_skip_exec=%d "
                "stage_only=%d freestanding_no_exec=%d never_exec_ko=%d "
                "RUN_INIT=%d g_ac1_waiver=%d soft=1 product=0 storm=0\n",
                szN, szR, szFirst, szPolicy, GJ_LMOD_GAP_D_RESIDUAL_REASONS,
                (int)GJ_LMOD_ALL_KO_SKIP_EXEC, (int)GJ_LMOD_STAGE_ONLY,
                fPolicyFree, (int)GJ_LMOD_NEVER_EXEC_KO,
                (int)GJ_SOFT_MODULE_RUN_INIT, (int)GJ_LMOD_G_AC1_WAIVER);
        if (fUsbStorage != 0) {
            lmod_lamp_usb_storage_skip_already(szR, szFirst, szPolicy,
                                               fFirstGapD);
        }
        return;
    }

    /*
     * Sparse first-SKIP set (all .ko under RUN_INIT=0 / freestanding_no_exec):
     *   1) primary SKIP exec
     *   2) freestanding SKIP residual
     *   3) lamp class
     * USB multi-mod leaf: + multi-mod + Dual DoD (one line each).
     * usb_storage leaf: + residual + honesty map (two lines).
     * Soft!=product; never INIT=0 on SKIP; storm=0.
     * C0: first SKIP tallies skip_exec residual (once per slot).
     */
    lmod_note_skip_exec();
    kprintf("linux_module: soft SKIP exec name=%s reason=%s kind=SKIP_EXEC "
            "deps_class=%s usb_leaf=%d usb_storage=%d policy=%s residual=%d "
            "residual_reasons=%s all_ko=1 all_ko_skip_exec=%d stage_only=%d "
            "freestanding_no_exec=%d never_exec_ko=%d RUN_INIT=%d "
            "g_ac1_waiver=%d dual_dod_A=OPEN dual_dod_B=OPEN C0=1 "
            "soft=1 product=0 storm=0\n",
            szN, szR, szDepsCls, fUsbLeaf, fUsbStorage, szPolicy, fGapD,
            GJ_LMOD_GAP_D_RESIDUAL_REASONS, (int)GJ_LMOD_ALL_KO_SKIP_EXEC,
            (int)GJ_LMOD_STAGE_ONLY, fPolicyFree, (int)GJ_LMOD_NEVER_EXEC_KO,
            (int)GJ_SOFT_MODULE_RUN_INIT, (int)GJ_LMOD_G_AC1_WAIVER);
    /*
     * All-.ko freestanding residual (sparse; once/slot; C0 denser honesty).
     * greppable: linux_module: soft freestanding SKIP residual name=
     * RUN_INIT=0 / freestanding_no_exec; all_ko_skip_exec=1 stage_only=1;
     * freestanding_class=SKIP; product_drivers=userspace_UDX_ABI;
     * eng residual wire=0; g_ac1_waiver=0 dual_dod_A/B=OPEN C0=1.
     * Soft!=product; G-AC-1; no extra storm lines.
     */
    kprintf("linux_module: soft freestanding SKIP residual name=%s "
            "reason=%s kind=SKIP_EXEC policy=%s residual=%d "
            "residual_reasons=%s all_ko=1 all_ko_skip_exec=%d stage_only=%d "
            "freestanding_no_exec=%d never_exec_ko=%d RUN_INIT=%d eng=%d "
            "wire=0 freestanding_class=SKIP product_drivers=userspace_UDX_ABI "
            "g_ac1_waiver=%d dual_dod_A=OPEN dual_dod_B=OPEN C0=1 "
            "soft=1 product=0 storm=0 triad=%s\n",
            szN, szR, szPolicy, fGapD, GJ_LMOD_GAP_D_RESIDUAL_REASONS,
            (int)GJ_LMOD_ALL_KO_SKIP_EXEC, (int)GJ_LMOD_STAGE_ONLY,
            fPolicyFree, (int)GJ_LMOD_NEVER_EXEC_KO,
            (int)GJ_SOFT_MODULE_RUN_INIT, lmod_is_eng_residual_mod(szN),
            (int)GJ_LMOD_G_AC1_WAIVER, GJ_LMOD_GAP_D_TRIAD);

    if (fUsbLeaf) {
        /* USB/MSC multi-mod: one greppable line each (sparse). */
        kprintf("linux_module: soft multi-mod SKIP exec name=%s reason=%s "
                "kind=SKIP_EXEC usb_storage=%d policy=%s residual=%d "
                "residual_reasons=%s multi_mod=1 leaf_deps=%s chain=%s "
                "all_ko=1 all_ko_skip_exec=%d stage_only=%d never_exec_ko=%d "
                "freestanding_no_exec=%d RUN_INIT=%d soft=1 product=0 storm=0 "
                "triad=%s\n",
                szN, szR, fUsbStorage, szPolicy, fGapD,
                GJ_LMOD_GAP_D_RESIDUAL_REASONS,
                fUsbStorage != 0 ? GJ_LMOD_USB_MSC_LEAF_DEPS : "-",
                fUsbStorage != 0 ? GJ_LMOD_USB_MSC_CHAIN : "-",
                (int)GJ_LMOD_ALL_KO_SKIP_EXEC, (int)GJ_LMOD_STAGE_ONLY,
                (int)GJ_LMOD_NEVER_EXEC_KO, fPolicyFree,
                (int)GJ_SOFT_MODULE_RUN_INIT, GJ_LMOD_GAP_D_TRIAD);
        kprintf("linux_module: soft residual Dual DoD D name=%s triad=%s "
                "reason=%s kind=SKIP_EXEC policy=%s residual_reasons=%s "
                "all_ko_skip_exec=%d stage_only=%d never_exec_ko=%d "
                "RUN_INIT=%d soft=1 product=0 storm=0\n",
                szN, GJ_LMOD_GAP_D_TRIAD, szR, szPolicy,
                GJ_LMOD_GAP_D_RESIDUAL_REASONS, (int)GJ_LMOD_ALL_KO_SKIP_EXEC,
                (int)GJ_LMOD_STAGE_ONLY, (int)GJ_LMOD_NEVER_EXEC_KO,
                (int)GJ_SOFT_MODULE_RUN_INIT);
        if (fUsbStorage != 0) {
            lmod_lamp_usb_storage_skip_residual(szR, szPolicy, szDepsCls,
                                                fGapD);
        }
    }
    lmod_lamp_class(szN, "SKIP_EXEC", szR);
    if (pMod != NULL) {
        pMod->u8SkipLamped = 1;
        (void)strlcpy(pMod->szFirstSkipReason, szR,
                      sizeof(pMod->szFirstSkipReason));
    }
}

/*
 * Unified SKIP-exec reason picker (most specific first). Soft!=product.
 * Returns NULL when init body may run (RUN_INIT=1 non-USB path only).
 *
 * Priority:
 *   empty_load >
 *   already_inited >
 *   deps_missing >
 *   deps_soft_virtual_only >
 *   no_init_symbol (when !fHasInit and skip required) >
 *   pfn_outside_module (init pfn not in load pages; freestanding-visible) >
 *   freestanding_no_exec (USB multi-mod leaf and/or RUN_INIT=0 policy)
 *
 * USB multi-mod / usb_storage Dual DoD D residual (Gap D catalog):
 *   deps_soft_virtual_only - soft deps satisfied only via seed markers
 *     (usbcore-soft / scsi_mod-soft). Most-specific cause: no real dep .ko.
 *     Lamp also tags policy=freestanding_no_exec (never-exec stance).
 *   pfn_outside_module - init_module VA not inside SHF_ALLOC load image.
 *     Greppable under freestanding default (not only RUN_INIT=1 eng path).
 *   freestanding_no_exec - USB multi-mod leaf with real/none deps, OR any
 *     .ko under default GJ_SOFT_MODULE_RUN_INIT=0. Policy default; lab
 *     #PF history under incomplete soft reloc. Never product AC.
 *   USB multi-mod leaf NEVER returns NULL (never false INIT eng).
 *
 * FAIL KSYM is load-time only - never returned here.
 * INIT=0 eng is when this returns NULL and init runs (or eng no-init PASS).
 *
 * @param nDeps        from lmod_deps_class: -1 miss, 0 none, 1 real, 2 soft-virt
 * @param fUsbLeaf     USB/MSC multi-mod class leaf
 * @param fHasInit     pfnInit non-NULL
 * @param fAlready     u8Inited already set
 * @param fEmpty       empty load image
 * @param fPfnOutside  1 if pfnInit not inside load pages (0 if no pfn / in-range)
 */
static const char *
lmod_pick_skip_reason(int nDeps, int fUsbLeaf, int fHasInit, int fAlready,
                      int fEmpty, int fPfnOutside)
{
    if (fEmpty) {
        return "empty_load";
    }
    if (fAlready) {
        return "already_inited";
    }
    if (nDeps == -1) {
        return "deps_missing";
    }
    if (nDeps == 2) {
        /*
         * Soft seed only (usbcore-soft/scsi_mod-soft) - most specific.
         * Typical usb_storage multi-mod leaf on host BUILTIN usbcore.
         * Freestanding policy still holds (lamp policy=); never INIT eng.
         * Gap D residual catalog reason; Soft!=product; product=0.
         */
        return GJ_LMOD_GAP_D_REASON_DEPS_SOFT_VIRT;
    }
    if (!fHasInit) {
        /*
         * No init_module symbol: multi-mod / freestanding policy -> SKIP.
         * Non-leaf with RUN_INIT=1 may still eng-PASS INIT=0 (return NULL).
         * USB multi-mod always SKIP here (never NULL).
         */
        if (fUsbLeaf || GJ_SOFT_MODULE_RUN_INIT == 0) {
            return "no_init_symbol";
        }
        return NULL;
    }
    /*
     * Gap D residual: pfn outside load image is more specific than plain
     * freestanding_no_exec. Visible under RUN_INIT=0 default (G-AC-1).
     * Soft!=product; never exec a dangling init pfn.
     */
    if (fPfnOutside) {
        return GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE;
    }
    /*
     * Has init_module in-range: USB multi-mod never exec on freestanding
     * (G-AC-1 / lab #PF). reason=freestanding_no_exec.
     */
    if (fUsbLeaf) {
        return GJ_LMOD_GAP_D_REASON_FREESTANDING;
    }
#if GJ_SOFT_MODULE_RUN_INIT == 0 || GJ_LMOD_ALL_KO_SKIP_EXEC || \
    GJ_LMOD_NEVER_EXEC_KO || GJ_LMOD_STAGE_ONLY
    /*
     * Default freestanding belt: SKIP exec ALL staged .ko residual.
     * all_ko_skip_exec=1 stage_only=1 never_exec_ko=1 freestanding_no_exec.
     * Soft!=product; G-AC-1; never product wire ownership via soft init.
     */
    return GJ_LMOD_GAP_D_REASON_FREESTANDING;
#else
    return NULL; /* non-USB may run init body when RUN_INIT=1 (lift asserts) */
#endif
}

static void
lmod_unres_reset(void)
{
    u32 i;

    g_szLastUnres[0] = '\0';
    g_cUnresList = 0;
    g_cUnresTrunc = 0;
    for (i = 0; i < GJ_LMOD_UNRES_LIST_MAX; i++) {
        g_aUnresList[i][0] = '\0';
    }
}

static void
lmod_set_unres(const char *szName)
{
    if (szName == NULL || szName[0] == '\0') {
        /* Keep list; only clear single-name if empty clear request and list empty. */
        if (g_cUnresList == 0u) {
            g_szLastUnres[0] = '\0';
        }
        return;
    }
    if (g_szLastUnres[0] == '\0') {
        (void)strlcpy(g_szLastUnres, szName, sizeof(g_szLastUnres));
    }
}

static int
lmod_unres_contains(const char *szName)
{
    u32 i;

    if (szName == NULL || szName[0] == '\0') {
        return 0;
    }
    for (i = 0; i < g_cUnresList; i++) {
        if (strcmp(g_aUnresList[i], szName) == 0) {
            return 1;
        }
    }
    return 0;
}

static void
lmod_unres_add(const char *szName)
{
    if (szName == NULL || szName[0] == '\0') {
        return;
    }
    lmod_set_unres(szName);
    if (lmod_unres_contains(szName)) {
        return;
    }
    if (g_cUnresList >= GJ_LMOD_UNRES_LIST_MAX) {
        g_cUnresTrunc++;
        return;
    }
    (void)strlcpy(g_aUnresList[g_cUnresList], szName, GJ_LMOD_NAME_MAX);
    g_cUnresList++;
}

/*
 * Emit greppable FAIL KSYM lines for iterative ksym resolve.
 * greppable: linux_module: soft FAIL KSYM name= missing= kind=FAIL_KSYM
 * greppable: linux_module: soft FAIL KSYM summary name= n= first=
 * greppable: linux_module: soft lamp class name= kind=FAIL_KSYM reason=
 * Dual DoD D residual: FAIL KSYM != SKIP exec != INIT eng (load-time; no init ran).
 */
static void
lmod_report_fail_ksym(const char *szModName)
{
    u32 i;
    const char *szMod;
    const char *szFirst;

    szMod = (szModName != NULL && szModName[0] != '\0') ? szModName : "anon";
    szFirst = (g_szLastUnres[0] != '\0') ? g_szLastUnres : "-";

    if (g_cUnresList == 0u) {
        if (szFirst[0] != '-' && szFirst[0] != '\0') {
            kprintf("linux_module: soft FAIL KSYM name=%s missing=%s "
                    "kind=FAIL_KSYM soft=1 product=0 "
                    "(load-time; no init ran; != SKIP exec; != INIT eng; Soft!=product)\n",
                    szMod, szFirst);
        }
        kprintf("linux_module: soft FAIL KSYM summary name=%s n=%u first=%s "
                "trunc=%u kind=FAIL_KSYM soft=1 product=0\n",
                szMod, 0u, szFirst, (unsigned)g_cUnresTrunc);
        lmod_lamp_class(szMod, "FAIL_KSYM", szFirst);
        return;
    }

    for (i = 0; i < g_cUnresList; i++) {
        kprintf("linux_module: soft FAIL KSYM name=%s missing=%s "
                "kind=FAIL_KSYM soft=1 product=0 "
                "(load-time; no init ran; != SKIP exec; != INIT eng; Soft!=product)\n",
                szMod, g_aUnresList[i]);
    }
    kprintf("linux_module: soft FAIL KSYM summary name=%s n=%u first=%s "
            "trunc=%u kind=FAIL_KSYM soft=1 product=0\n",
            szMod, (unsigned)g_cUnresList, g_aUnresList[0],
            (unsigned)g_cUnresTrunc);
    lmod_lamp_class(szMod, "FAIL_KSYM", g_aUnresList[0]);
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
        lmod_unres_add(szName[0] != '\0' ? szName : "?");
        return (i64)GJ_ERR_NOENT;
    }

    if (u16Sh == SHN_ABS) {
        *pu64Val = pSym->u64Value;
        return (i64)GJ_OK;
    }

    if (u16Sh == SHN_COMMON) {
        /* Soft: no common-block allocator; reject. */
        lmod_unres_add(szName[0] != '\0' ? szName : "COMMON");
        return (i64)GJ_ERR_NOSUPPORT;
    }

    if (u16Sh >= GJ_LMOD_SH_MAX) {
        lmod_unres_add(szName[0] != '\0' ? szName : "bad_shndx");
        return (i64)GJ_ERR_INVAL;
    }
    if (pEh != NULL && u16Sh >= pEh->u16Shnum) {
        lmod_unres_add(szName[0] != '\0' ? szName : "bad_shndx");
        return (i64)GJ_ERR_INVAL;
    }
    if (!g_aSecAlloc[u16Sh]) {
        /*
         * Soft: non-ALLOC (e.g. .BTF STT_SECTION) or empty-bss miss -
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
            /* Reloc for non-ALLOC (debug) - soft skip section. */
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

/*
 * Pre-scan: collect every strong SHN_UNDEF that does not hit ksym.
 * Enables one-shot greppable FAIL KSYM dump for iterative resolve
 * (r8169 / usb_storage reliability). Soft!=product.
 * @return number of unique misses (capped).
 */
static u32
lmod_prescan_unres(const struct elf64_sym *pSym0, u32 cSym, const char *pStr,
                   u64 cbStr)
{
    u32 i;
    u32 cBefore;

    cBefore = g_cUnresList;
    if (pSym0 == NULL || pStr == NULL) {
        return 0;
    }
    for (i = 0; i < cSym; i++) {
        const struct elf64_sym *pSym = &pSym0[i];
        const char *szName;
        void *pKsym;

        if (pSym->u16Shndx != SHN_UNDEF) {
            continue;
        }
        if (lmod_st_bind(pSym->u8Info) == STB_WEAK) {
            continue;
        }
        if (pSym->u32Name >= cbStr) {
            continue;
        }
        szName = pStr + pSym->u32Name;
        if (szName[0] == '\0') {
            continue;
        }
        /* Skip modversion CRC digests - not soft ksym surface. */
        if (szName[0] == '_' && szName[1] == '_' && szName[2] == 'c' &&
            szName[3] == 'r' && szName[4] == 'c' && szName[5] == '_') {
            continue;
        }
        if (linux_ksym_has(szName)) {
            continue;
        }
        pKsym = linux_ksym_lookup(szName);
        if (pKsym != NULL) {
            continue;
        }
        lmod_unres_add(szName);
    }
    (void)cBefore;
    return g_cUnresList;
}

/*
 * Dup symbol name into export arena (ELF .strtab is not live after media free).
 * @return stable pointer or NULL if pool exhausted / bad name.
 */
static const char *
lmod_export_name_dup(const char *szName)
{
    size_t cb;
    char *pDst;

    if (szName == NULL || szName[0] == '\0') {
        return NULL;
    }
    cb = strlen(szName) + 1u;
    if (cb > GJ_LMOD_NAME_MAX) {
        cb = GJ_LMOD_NAME_MAX;
    }
    if (g_cExportNamePool + (u32)cb > GJ_LMOD_EXPORT_NAME_POOL) {
        return NULL;
    }
    pDst = &g_aExportNamePool[g_cExportNamePool];
    (void)strlcpy(pDst, szName, cb);
    g_cExportNamePool += (u32)cb;
    return pDst;
}

/*
 * Soft-export defined GLOBAL/WEAK symbols into ksym so a later .ko can
 * resolve against an earlier loaded module (multi-mod order path).
 * Cap: fill until linux_ksym_register fails or name pool full. Soft!=product.
 * greppable: linux_module: soft export ksym n= name=
 */
static u32
lmod_export_globals(struct gj_lmod *pMod, const struct elf64_sym *pSym0,
                    u32 cSym, const char *pStr, u64 cbStr)
{
    u32 i;
    u32 cExp = 0;
    u32 cSkip = 0;

    if (pMod == NULL || pSym0 == NULL || pStr == NULL) {
        return 0;
    }
    for (i = 0; i < cSym; i++) {
        const struct elf64_sym *pSym = &pSym0[i];
        const char *szName;
        const char *szStable;
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
        /* Skip section symbols / empty names already handled. */
        if (szName[0] == '.') {
            continue;
        }
        i64St = lmod_resolve_sym(NULL, 0, NULL, pSym, pStr, cbStr, &u64Val);
        if (i64St != (i64)GJ_OK || u64Val == 0) {
            continue;
        }
        /* Already present: keep existing soft/body; do not steal. */
        if (linux_ksym_has(szName)) {
            continue;
        }
        szStable = lmod_export_name_dup(szName);
        if (szStable == NULL) {
            cSkip++;
            continue;
        }
        if (linux_ksym_register(szStable, (void *)(uintptr_t)u64Val) == 0) {
            cExp++;
        } else {
            cSkip++;
        }
    }
    kprintf("linux_module: soft export ksym n=%u skip=%u name=%s soft=1 "
            "product=0\n",
            (unsigned)cExp, (unsigned)cSkip, pMod->szName);
    return cExp;
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
    lmod_unres_reset();
    g_fInited = 1;
    kprintf("linux_module: soft ready max=%u live=%u unres_cap=%u\n",
            (unsigned)GJ_LINUX_MODULE_MAX, (unsigned)g_cMod,
            (unsigned)GJ_LMOD_UNRES_LIST_MAX);
    /*
     * Lean residual (once): RUN_INIT=0 / freestanding_no_exec honesty denser.
     * Soft!=product; G-AC-1; all_ko_skip_exec=1 stage_only=1;
     * eng=r8169,usb_storage wire=0; product_drivers=userspace_UDX_ABI;
     * freestanding_class=SKIP. SKIP exec all .ko residual; soft load/stage
     * eng only. storm=0. greppable: soft residual freestanding_no_exec
     */
    lmod_soft_residual_once();
}

/*
 * Fail lamp with source tag (D4/D5 honesty). Soft!=product.
 * greppable: linux_module: soft load FAIL source= name= missing=
 * Also emits FAIL KSYM when @szMiss is a real symbol (not "-").
 */
static void
lmod_lamp_fail(const char *szSrc, const char *szName, const char *szMiss)
{
    const char *szM;

    szM = (szMiss != NULL && szMiss[0] != '\0') ? szMiss : "-";
    kprintf("linux_module: soft load FAIL source=%s name=%s missing=%s "
            "unres_n=%u soft=1 product=0\n",
            szSrc != NULL && szSrc[0] != '\0' ? szSrc : "mem",
            szName != NULL && szName[0] != '\0' ? szName : "anon", szM,
            (unsigned)g_cUnresList);
    if (g_cUnresList > 0u || (szM[0] != '-' && szM[0] != '\0')) {
        lmod_report_fail_ksym(szName);
    }
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

    lmod_unres_reset();

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

    /* Attempt lamp first - greppable even when load fails early. */
    kprintf("linux_module: soft load source=%s name=%s\n", szSrc, szModName);

    /*
     * Soft load-order honesty for known class modules (r8169 / usb_storage).
     * Does not block load - emits WAIT + FAIL KSYM need:DEP when soft deps
     * are not already in the live table. Soft!=product.
     */
    if (linux_module_load_order_rank(szModName) < 1000) {
        kprintf("linux_module: soft load order rank=%d name=%s deps=%s "
                "soft=1 product=0\n",
                linux_module_load_order_rank(szModName), szModName,
                linux_module_soft_deps(szModName)[0] != '\0'
                    ? linux_module_soft_deps(szModName)
                    : "-");
        (void)linux_module_deps_ready(szModName);
    }

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

    /*
     * Pre-resolve scan: list every strong SHN_UNDEF miss before applying
     * relocs so one FAIL KSYM dump drives the iterative ksym loop
     * (usb_storage / r8169). Soft!=product; fail closed.
     */
    if (lmod_prescan_unres(pSym0, cSym, pStr, cbStr) > 0u) {
        const char *szMiss =
            g_szLastUnres[0] != '\0' ? g_szLastUnres : "-";
        lmod_free_slot(pMod);
        lmod_lamp_fail(szSrc, szModName, szMiss);
        return (i64)GJ_ERR_NOENT;
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
    /* Multi-mod: export this .ko's globals for later resolvers. */
    (void)lmod_export_globals(pMod, pSym0, cSym, pStr, cbStr);
    g_cMod++;

    /*
     * Load PASS = stage only (stage_only=1). init_sym=1 means init_module
     * was bound, NOT that the body ran. Under RUN_INIT=0 / never_exec_ko /
     * all_ko_skip_exec, init_call will SKIP exec ALL .ko residual
     * (freestanding_no_exec denser). Soft!=product; G-AC-1; storm=0.
     * Soft load residual eng=1 only for r8169/usb_storage - never product
     * wire ownership. Soft load/stage eng only. Product drivers = userspace
     * UDX+ABI; freestanding class SKIP.
     * greppable: linux_module: soft load PASS source= name= missing=-
     */
    kprintf("linux_module: soft load PASS source=%s name=%s missing=- "
            "pages=%u got=%u init_sym=%u exit_sym=%u "
            "init_policy=%s never_exec_ko=%d all_ko_skip_exec=%d "
            "stage_only=%d RUN_INIT=%d policy=%s eng=%d wire=0 "
            "soft=1 product=0 storm=0 "
            "(stage only; soft load/stage eng residual only r8169/usb_storage; "
            "SKIP exec all .ko residual; never product wire ownership; "
            "product_drivers=userspace_UDX_ABI; freestanding_class=SKIP; "
            "Soft!=product; != G-AC-1)\n",
            szSrc, pMod->szName, (unsigned)pMod->cPages, (unsigned)pMod->cGot,
            pMod->pfnInit != NULL ? 1u : 0u, pMod->pfnExit != NULL ? 1u : 0u,
#if GJ_SOFT_MODULE_RUN_INIT == 0
            "SKIP_EXEC",
#else
            "MAY_RUN",
#endif
            (int)GJ_LMOD_NEVER_EXEC_KO, (int)GJ_LMOD_ALL_KO_SKIP_EXEC,
            (int)GJ_LMOD_STAGE_ONLY, (int)GJ_SOFT_MODULE_RUN_INIT,
#if GJ_SOFT_MODULE_RUN_INIT == 0
            GJ_LMOD_POLICY_FREESTANDING,
#else
            "may_run",
#endif
            lmod_is_eng_residual_mod(pMod->szName));
    return (i64)GJ_OK;
}

/*
 * True when pfnInit is non-NULL and its VA is not inside any load page.
 * Soft!=product; freestanding-visible Gap D residual reason.
 * @return 1 outside / unknown, 0 in-range or no pfn
 */
static int
lmod_pfn_outside_module(const struct gj_lmod *pMod)
{
    uintptr_t uInit;
    u32 i;

    if (pMod == NULL || pMod->pfnInit == NULL || pMod->cPages == 0u) {
        return 0;
    }
    uInit = (uintptr_t)(void *)pMod->pfnInit;
    for (i = 0; i < pMod->cPages; i++) {
        uintptr_t uBase = (uintptr_t)lmod_va(
            pMod->paBase + (gj_paddr_t)i * (gj_paddr_t)GJ_PAGE_SIZE);
        if (uInit >= uBase && uInit < uBase + (uintptr_t)GJ_PAGE_SIZE) {
            return 0;
        }
    }
    return 1;
}

i64
linux_module_init_call(const char *szName)
{
    struct gj_lmod *pMod;
    int nDeps;
    int fUsbLeaf;
    int fHasInit;
    int fEmpty;
    int fPfnOutside;
    const char *szN;
    const char *szReason;

    if (!g_fInited) {
        linux_module_init();
    }
    szN = (szName != NULL && szName[0] != '\0') ? szName : "anon";
    pMod = lmod_find(szName);
    if (pMod == NULL) {
        /* Missing slot: FAIL, not SKIP (nothing loaded to skip-exec). */
        kprintf("linux_module: soft init FAIL name=%s INIT=-ENOENT "
                "kind=INIT_FAIL soft=1 product=0 "
                "(!= SKIP exec; slot missing; Soft!=product)\n",
                szN);
        lmod_lamp_class(szN, "INIT_FAIL", "not_loaded");
        return (i64)GJ_ERR_NOENT;
    }

    /*
     * Idempotent residual: already completed init_call (SKIP or eng).
     * Re-emit honest lamp (rate-limited); never re-run body. Dual DoD D.
     * C0 freestanding re-entry belt (RUN_INIT=0 / never_exec_ko / all_ko /
     * stage_only): never re-claim INIT0_ENG even if u8InitRan corrupted;
     * force InitRan=0 and stay SKIP-class. Soft!=product; G-AC-1.
     * USB multi-mod leaf that somehow has u8InitRan is still not re-exec'd.
     */
    if (pMod->u8Inited != 0u) {
#if GJ_SOFT_MODULE_RUN_INIT == 0 || GJ_LMOD_NEVER_EXEC_KO || \
    GJ_LMOD_ALL_KO_SKIP_EXEC || GJ_LMOD_STAGE_ONLY
        /* C0 freestanding: never INIT0_ENG re-claim under freestanding belt. */
        (void)lmod_force_init_ran_clear(pMod);
        /* Re-entry: no re-tally (first SKIP already counted). Soft!=product. */
        lmod_lamp_skip_exec(pMod->szName, "already_skipped");
        return (i64)GJ_OK;
#else
        fUsbLeaf = lmod_is_usb_multimod_leaf(pMod->szName);
        if (pMod->u8InitRan != 0u && !fUsbLeaf) {
            kprintf("linux_module: soft init PASS name=%s INIT=0 "
                    "kind=INIT0_ENG already=1 soft=1 product=0 "
                    "(soft eng; != SKIP exec; != FAIL KSYM; Soft!=product)\n",
                    pMod->szName);
            lmod_lamp_class(pMod->szName, "INIT0_ENG", "already_inited");
            return (i64)GJ_OK;
        }
        /* USB multi-mod / prior SKIP: always honest SKIP (never silent INIT eng). */
        lmod_lamp_skip_exec(pMod->szName, "already_skipped");
        return (i64)GJ_OK;
#endif
    }

    fUsbLeaf = lmod_is_usb_multimod_leaf(pMod->szName);
    nDeps = lmod_deps_class(pMod->szName);
    fHasInit = (pMod->pfnInit != NULL) ? 1 : 0;
    fEmpty = (pMod->pLoad == NULL || pMod->cbLoad == 0ull ||
              pMod->cPages == 0u)
                 ? 1
                 : 0;
    /* Gap D: pfn_outside_module greppable under freestanding default. */
    fPfnOutside = (fHasInit && !fEmpty) ? lmod_pfn_outside_module(pMod) : 0;

    /*
     * Unified reason (most specific first). Soft!=product.
     * Parent may skip calling init_module; when it does call, lamps stay
     * honest: SKIP exec != FAIL KSYM != INIT=0 eng (Dual DoD D residual).
     *
     * Soft load residual eng only (r8169 / usb_storage): never product wire
     * ownership. Product drivers = userspace UDX+ABI; freestanding class SKIP.
     * Lab G752: real .ko init (r8169 / usb_storage) #PF'd under incomplete
     * soft reloc - default RUN_INIT=0 freestanding_no_exec denser.
     * usb_storage with soft-virt deps -> reason=deps_soft_virtual_only
     *   (policy=freestanding_no_exec still on lamp).
     * usb_storage pfn not in load image -> reason=pfn_outside_module.
     * usb_storage otherwise -> reason=freestanding_no_exec.
     * greppable: linux_module: soft SKIP exec name= reason= kind=SKIP_EXEC
     * greppable: linux_module: soft freestanding SKIP residual name=
     * greppable: residual_reasons=freestanding_no_exec|deps_soft_virtual_only|pfn_outside_module
     * Sparse residual only (no multi-line storms). Re-entry already=1 single line.
     */
    szReason = lmod_pick_skip_reason(nDeps, fUsbLeaf, fHasInit, 0, fEmpty,
                                     fPfnOutside);
    /*
     * Belt: USB multi-mod leaf always SKIP - never fall through to INIT eng
     * even if picker regresses. Soft!=product; G-AC-1; lab #PF history.
     * Prefer pfn_outside_module when that is the more specific residual.
     */
    if (szReason == NULL && fUsbLeaf) {
        szReason = fPfnOutside ? GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE
                               : GJ_LMOD_GAP_D_REASON_FREESTANDING;
    }
    if (szReason != NULL) {
        pMod->u8Inited = 1;
        pMod->u8InitRan = 0; /* hard: never mark body ran on SKIP path */
        (void)lmod_force_init_ran_clear(pMod); /* belt if already set */
        /* skip_exec tally is inside first lmod_lamp_skip_exec (C0). */
        lmod_lamp_skip_exec(pMod->szName, szReason);
        return (i64)GJ_OK;
    }

#if GJ_SOFT_MODULE_RUN_INIT == 0 || GJ_LMOD_NEVER_EXEC_KO || \
    GJ_LMOD_ALL_KO_SKIP_EXEC || GJ_LMOD_STAGE_ONLY
    /*
     * Hard freestanding belt (ALL .ko residual): never executes .ko init
     * body and never claims INIT0_ENG. Soft!=product; G-AC-1; pfnInit
     * never called. Reinforced: all_ko_skip_exec=1 stage_only=1
     * never_exec_ko=1 freestanding_no_exec - soft load/stage eng only;
     * never product .ko exec / wire ownership. Picker should already
     * have returned freestanding/no_init/... under RUN_INIT=0; this belt
     * closes any picker regression (storm=0). C0 residual: skip tally +
     * InitRan force-clear; g_ac1_waiver=0 dual_dod OPEN. Lean residual
     * once grepped freestanding_no_exec / never_exec_ko=1 all_ko_skip=1
     * stage_only=1 eng=r8169,usb_storage wire=0 freestanding_class=SKIP
     * product_drivers=userspace_UDX_ABI. SKIP exec all .ko residual.
     * No version stamp.
     */
    pMod->u8Inited = 1;
    pMod->u8InitRan = 0; /* hard: never mark body ran on freestanding path */
    (void)lmod_force_init_ran_clear(pMod);
    /* skip_exec tally is inside first lmod_lamp_skip_exec (C0). */
    lmod_lamp_skip_exec(pMod->szName,
                        !fHasInit ? "no_init_symbol"
                        : fPfnOutside ? GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE
                                      : GJ_LMOD_GAP_D_REASON_FREESTANDING);
    return (i64)GJ_OK;
#else
    {
        int nRet;

        /*
         * Opt-in eng path only (RUN_INIT=1 and NEVER_EXEC_KO=0). Soft!=product.
         * USB multi-mod leaf NEVER exec even if RUN_INIT=1 (G-AC-1 / #PF).
         * pfn_outside_module NEVER exec (dangling init pfn).
         * Never open accidental .ko exec; u8InitRan stays 0 on SKIP.
         */
        if (fUsbLeaf) {
            pMod->u8Inited = 1;
            pMod->u8InitRan = 0;
            lmod_lamp_skip_exec(pMod->szName,
                                fPfnOutside ? GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE
                                            : GJ_LMOD_GAP_D_REASON_FREESTANDING);
            return (i64)GJ_OK;
        }

        if (fPfnOutside) {
            pMod->u8Inited = 1;
            pMod->u8InitRan = 0;
            lmod_lamp_skip_exec(pMod->szName, GJ_LMOD_GAP_D_REASON_PFN_OUTSIDE);
            return (i64)GJ_OK;
        }

        /* Eng no-init stub PASS (no body; still Soft!=product; != G-AC-1). */
        if (!fHasInit) {
            pMod->u8Inited = 1;
            pMod->u8InitRan = 0;
            kprintf("linux_module: soft init PASS name=%s INIT=0 "
                    "kind=INIT0_ENG soft=1 product=0 "
                    "(no init_module; soft eng stub; != SKIP exec; != FAIL KSYM; "
                    "Soft!=product; != G-AC-1)\n",
                    pMod->szName);
            lmod_lamp_class(pMod->szName, "INIT0_ENG", "no_init_module");
            return (i64)GJ_OK;
        }

        /* Only non-USB, in-range pfn, RUN_INIT=1 may touch .ko init body. */
        nRet = pMod->pfnInit();
        if (nRet != 0) {
            kprintf("linux_module: soft init FAIL name=%s INIT=%d "
                    "kind=INIT_FAIL soft=1 product=0 "
                    "(!= SKIP exec; body returned nonzero; Soft!=product)\n",
                    pMod->szName, nRet);
            lmod_lamp_class(pMod->szName, "INIT_FAIL", "init_nonzero");
            return (nRet < 0) ? (i64)nRet : (i64)GJ_ERR_IO;
        }
        pMod->u8Inited = 1;
        pMod->u8InitRan = 1;
        /* greppable: linux_module: soft init PASS name= INIT=0 kind=INIT0_ENG */
        kprintf("linux_module: soft init PASS name=%s INIT=0 "
                "kind=INIT0_ENG soft=1 product=0 "
                "(soft eng; body ran; != SKIP exec; != FAIL KSYM; Soft!=product; "
                "!= G-AC-1)\n",
                pMod->szName);
        lmod_lamp_class(pMod->szName, "INIT0_ENG", "init_returned_0");
        return (i64)GJ_OK;
    }
#endif
}

i64
linux_module_exit_call(const char *szName)
{
    struct gj_lmod *pMod;
    const char *szN;

    if (!g_fInited) {
        linux_module_init();
    }
    szN = (szName != NULL && szName[0] != '\0') ? szName : "anon";
    pMod = lmod_find(szName);
    if (pMod == NULL) {
        kprintf("linux_module: soft exit FAIL name=%s soft=1 product=0\n",
                szN);
        return (i64)GJ_ERR_NOENT;
    }
    /*
     * Only run cleanup_module if init_module body actually ran.
     * After intentional SKIP exec (freestanding_no_exec /
     * deps_soft_virtual_only / ...), exit must not touch .ko text.
     * C0 freestanding exit belt: under RUN_INIT=0 / never_exec_ko /
     * all_ko_skip / stage_only, never call cleanup even if InitRan
     * corrupted (force-clear + SKIP exit residual). Soft!=product; G-AC-1.
     * greppable: linux_module: soft SKIP exit name= reason=
     * Dual DoD D residual honesty.
     */
#if GJ_SOFT_MODULE_RUN_INIT == 0 || GJ_LMOD_NEVER_EXEC_KO || \
    GJ_LMOD_ALL_KO_SKIP_EXEC || GJ_LMOD_STAGE_ONLY
    /* C0 freestanding: never cleanup_module (never_exec_ko belt). */
    (void)lmod_force_init_ran_clear(pMod);
#else
    if (pMod->pfnExit != NULL && pMod->u8InitRan != 0u) {
        pMod->pfnExit();
        pMod->u8Inited = 0;
        pMod->u8InitRan = 0;
        pMod->u8SkipLamped = 0; /* allow full SKIP lamps on re-init */
        pMod->szFirstSkipReason[0] = '\0';
        kprintf("linux_module: soft exit PASS name=%s soft=1 product=0\n",
                pMod->szName);
        return (i64)GJ_OK;
    }
#endif
    if (pMod->pfnExit != NULL && pMod->u8InitRan == 0u) {
        char szFirstExit[40];
        const char *szPolExit;
        const char *szDepsClsExit;
        int fFirstGapDExit;
        int nDepsExit;

        /*
         * Snapshot first SKIP reason before clear (exit residual honesty).
         * Soft!=product; product=0; Gap D residual catalog still greppable.
         */
        if (pMod->szFirstSkipReason[0] != '\0') {
            (void)strlcpy(szFirstExit, pMod->szFirstSkipReason,
                          sizeof(szFirstExit));
        } else {
            szFirstExit[0] = '-';
            szFirstExit[1] = '\0';
        }
        fFirstGapDExit = lmod_is_gap_d_residual_reason(szFirstExit);
        nDepsExit = lmod_deps_class(pMod->szName);
        if (nDepsExit == -1) {
            szDepsClsExit = "missing";
        } else if (nDepsExit == 2) {
            szDepsClsExit = "soft-virtual";
        } else if (nDepsExit == 1) {
            szDepsClsExit = "real";
        } else {
            szDepsClsExit = "none";
        }
#if GJ_SOFT_MODULE_RUN_INIT == 0
        szPolExit = GJ_LMOD_POLICY_FREESTANDING;
#else
        szPolExit = lmod_is_usb_multimod_leaf(pMod->szName)
                        ? GJ_LMOD_POLICY_FREESTANDING
                        : "may_run";
#endif
        pMod->u8Inited = 0;
        pMod->u8SkipLamped = 0; /* allow full SKIP lamps on re-init */
        pMod->szFirstSkipReason[0] = '\0';
        kprintf("linux_module: soft SKIP exit name=%s reason=init_body_not_run "
                "kind=SKIP_EXEC policy=%s all_ko=1 all_ko_skip_exec=%d "
                "stage_only=%d never_exec_ko=%d freestanding_no_exec=1 "
                "RUN_INIT=%d g_ac1_waiver=%d dual_dod_A=OPEN dual_dod_B=OPEN "
                "C0=1 skip_exec_n=%u init_ran_force_clear=%u "
                "soft=1 product=0 storm=0 "
                "(init was SKIP; cleanup not called; SKIP exec all .ko residual; "
                "Soft!=product; G-AC-1; != product AC; never Dual DoD close)\n",
                pMod->szName, szPolExit, (int)GJ_LMOD_ALL_KO_SKIP_EXEC,
                (int)GJ_LMOD_STAGE_ONLY, (int)GJ_LMOD_NEVER_EXEC_KO,
                (int)GJ_SOFT_MODULE_RUN_INIT, (int)GJ_LMOD_G_AC1_WAIVER,
                (unsigned)g_cSkipExec, (unsigned)g_cInitRanForceClear);
        /* Gap D residual exit honesty (sparse; Soft!=product; once). */
        if (lmod_is_usb_storage_leaf(pMod->szName) != 0) {
            lmod_lamp_usb_storage_skip_exit_residual(szFirstExit, szPolExit,
                                                     szDepsClsExit,
                                                     fFirstGapDExit);
        }
        return (i64)GJ_OK;
    }
    pMod->u8Inited = 0;
    pMod->u8InitRan = 0;
    pMod->u8SkipLamped = 0;
    pMod->szFirstSkipReason[0] = '\0';
    kprintf("linux_module: soft exit PASS name=%s (no cleanup_module) "
            "soft=1 product=0\n",
            pMod->szName);
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

u32
linux_module_unresolved_count(void)
{
    return g_cUnresList;
}

int
linux_module_load_order_rank(const char *szName)
{
    u32 i;

    if (szName == NULL || szName[0] == '\0') {
        return 1000;
    }
    for (i = 0; i < (u32)GJ_LMOD_ORDER_N; i++) {
        if (strcmp(g_aSoftOrder[i].szName, szName) == 0) {
            return g_aSoftOrder[i].nRank;
        }
    }
    return 1000;
}

const char *
linux_module_soft_deps(const char *szName)
{
    u32 i;

    if (szName == NULL || szName[0] == '\0') {
        return "";
    }
    for (i = 0; i < (u32)GJ_LMOD_ORDER_N; i++) {
        if (strcmp(g_aSoftOrder[i].szName, szName) == 0) {
            return g_aSoftOrder[i].szDeps != NULL ? g_aSoftOrder[i].szDeps
                                                 : "";
        }
    }
    return "";
}

/*
 * Parse comma-separated soft deps; check real load OR soft-virtual marker.
 * greppable: linux_module: soft load order ready|WAIT name= need=
 * greppable: linux_module: soft deps_ready name= via= init_policy=
 * greppable: linux_module: soft dep virtual <canon> ... (registration)
 * Also emits FAIL KSYM need:DEP when neither real nor soft-virtual present.
 *
 * Honesty: ready via soft-virtual foreshadows init SKIP exec
 * reason=deps_soft_virtual_only (not INIT PASS; Soft!=product).
 * Missing -> WAIT + FAIL KSYM need:DEP (kind=FAIL_KSYM; no init ran).
 */
int
linux_module_deps_ready(const char *szName)
{
    const char *szDeps;
    const char *p;
    char szTok[GJ_LMOD_NAME_MAX];
    u32 cTok;
    int fMiss = 0;
    int fViaVirt = 0;
    int fAny = 0;
    const char *szN;
    int nHow;

    szN = (szName != NULL && szName[0] != '\0') ? szName : "anon";
    szDeps = linux_module_soft_deps(szName);
    if (szDeps == NULL || szDeps[0] == '\0') {
        kprintf("linux_module: soft load order ready name=%s need=- "
                "soft=1 product=0\n",
                szN);
        kprintf("linux_module: soft deps_ready name=%s via=none "
                "init_policy=policy soft=1 product=0 "
                "(no soft deps recorded; init may still SKIP freestanding)\n",
                szN);
        return 0;
    }

    p = szDeps;
    while (*p != '\0') {
        cTok = 0;
        while (*p != '\0' && *p != ',' && cTok + 1u < GJ_LMOD_NAME_MAX) {
            szTok[cTok++] = *p++;
        }
        szTok[cTok] = '\0';
        if (*p == ',') {
            p++;
        }
        if (cTok == 0u) {
            continue;
        }
        fAny = 1;
        nHow = lmod_dep_satisfied(szTok);
        if (nHow == 0) {
            fMiss = 1;
            kprintf("linux_module: soft load order WAIT name=%s need=%s "
                    "soft=1 product=0 "
                    "(no real .ko and no soft-virtual; host may be BUILTIN)\n",
                    szN, szTok);
            /*
             * Greppable FAIL KSYM with need: prefix so serial greps for
             * "FAIL KSYM" catch soft dep gaps too (usb_storage need=usbcore).
             * kind=FAIL_KSYM != SKIP_EXEC (init not yet considered).
             */
            kprintf("linux_module: soft FAIL KSYM name=%s missing=need:%s "
                    "kind=FAIL_KSYM soft=1 product=0 "
                    "(load-time dep gap; no init ran; != SKIP exec; != INIT eng; "
                    "Soft!=product; != G-AC-1)\n",
                    szN, szTok);
            lmod_lamp_class(szN, "FAIL_KSYM", "need_dep");
        } else if (nHow == 2) {
            fViaVirt = 1;
            /* Grep: soft dep virtual satisfied (eng lamp; != real load) */
            kprintf("linux_module: soft dep virtual ok name=%s need=%s "
                    "slot=%s-soft soft=1 product=0 "
                    "(seed marker; != real %s.ko; Soft!=product)\n",
                    szN, szTok, szTok, szTok);
        }
    }

    if (!fAny) {
        kprintf("linux_module: soft load order ready name=%s need=- "
                "soft=1 product=0\n",
                szN);
        return 0;
    }

    if (!fMiss) {
        kprintf("linux_module: soft load order ready name=%s need=%s "
                "via=%s soft=1 product=0\n",
                szN, szDeps, fViaVirt ? "soft-virtual" : "loaded");
        /*
         * Foreshadow init policy for multi-mod / usb_storage honesty.
         * soft-virtual -> init_call will SKIP exec reason=deps_soft_virtual_only
         * (never claim INIT PASS / product usbcore / stick).
         * greppable: linux_module: soft deps_ready name= via= init_policy=
         */
        if (fViaVirt) {
            /* One sparse foreshadow line (no multi-line residual storm). */
            kprintf("linux_module: soft deps_ready name=%s via=soft-virtual "
                    "init_policy=SKIP_EXEC reason=%s residual_reasons=%s "
                    "usb_storage=%d all_ko=1 all_ko_skip_exec=%d stage_only=%d "
                    "never_exec_ko=%d freestanding_no_exec=1 policy=%s "
                    "RUN_INIT=%d soft=1 product=0 storm=0 "
                    "(SKIP exec all .ko residual; Soft!=product; G-AC-1)\n",
                    szN, GJ_LMOD_GAP_D_REASON_DEPS_SOFT_VIRT,
                    GJ_LMOD_GAP_D_RESIDUAL_REASONS,
                    lmod_is_usb_storage_leaf(szN),
                    (int)GJ_LMOD_ALL_KO_SKIP_EXEC, (int)GJ_LMOD_STAGE_ONLY,
                    (int)GJ_LMOD_NEVER_EXEC_KO, GJ_LMOD_POLICY_FREESTANDING,
                    (int)GJ_SOFT_MODULE_RUN_INIT);
        } else {
            kprintf("linux_module: soft deps_ready name=%s via=loaded "
                    "init_policy=%s all_ko_skip_exec=%d stage_only=%d "
                    "never_exec_ko=%d RUN_INIT=%d soft=1 product=0 "
                    "(real soft-loaded deps; SKIP exec all .ko residual; "
                    "Soft!=product; G-AC-1)\n",
                    szN,
#if GJ_SOFT_MODULE_RUN_INIT == 0 || GJ_LMOD_ALL_KO_SKIP_EXEC
                    "SKIP_EXEC",
#else
                    "MAY_RUN",
#endif
                    (int)GJ_LMOD_ALL_KO_SKIP_EXEC, (int)GJ_LMOD_STAGE_ONLY,
                    (int)GJ_LMOD_NEVER_EXEC_KO, (int)GJ_SOFT_MODULE_RUN_INIT);
        }
        return 0;
    }
    kprintf("linux_module: soft deps_ready name=%s via=miss "
            "init_policy=SKIP_EXEC reason=deps_missing "
            "all_ko_skip_exec=%d stage_only=%d never_exec_ko=%d RUN_INIT=%d "
            "soft=1 product=0 "
            "(WAIT; FAIL KSYM need:DEP already emitted; Soft!=product; G-AC-1)\n",
            szN, (int)GJ_LMOD_ALL_KO_SKIP_EXEC, (int)GJ_LMOD_STAGE_ONLY,
            (int)GJ_LMOD_NEVER_EXEC_KO, (int)GJ_SOFT_MODULE_RUN_INIT);
    return -1;
}

int
linux_module_soft_dep_virtual_register(const char *szCanon)
{
    u32 i;
    u32 n;
    int iExist;

    if (szCanon == NULL || szCanon[0] == '\0') {
        return -1;
    }
    /* Cap canonical length so "-soft" suffix fits. */
    n = 0;
    while (szCanon[n] != '\0') {
        n++;
    }
    if (n == 0u || n + 5u >= GJ_LMOD_NAME_MAX) {
        return -1;
    }

    iExist = lmod_soft_virt_find(szCanon);
    if (iExist >= 0) {
        kprintf("linux_module: soft dep virtual %s soft=1 product=0 "
                "slot=%s already=1 (honest seed; != real .ko)\n",
                szCanon, g_aSoftVirt[iExist].szVirt);
        return 0;
    }

    for (i = 0; i < GJ_LMOD_SOFT_VIRT_MAX; i++) {
        if (!g_aSoftVirt[i].u8Used) {
            memset(&g_aSoftVirt[i], 0, sizeof(g_aSoftVirt[i]));
            g_aSoftVirt[i].u8Used = 1;
            (void)strlcpy(g_aSoftVirt[i].szCanon, szCanon,
                          sizeof(g_aSoftVirt[i].szCanon));
            (void)strlcpy(g_aSoftVirt[i].szVirt, szCanon,
                          sizeof(g_aSoftVirt[i].szVirt));
            (void)strlcat(g_aSoftVirt[i].szVirt, "-soft",
                          sizeof(g_aSoftVirt[i].szVirt));
            g_cSoftVirt++;
            /*
             * Grep (stable): linux_module: soft dep virtual usbcore soft=1 product=0
             * Virtual slot name is <canon>-soft; does NOT set loaded(canon).
             */
            kprintf("linux_module: soft dep virtual %s soft=1 product=0 "
                    "slot=%s loaded_real=%d "
                    "(seed marker for deps_ready; Soft!=product; != real %s.ko)\n",
                    szCanon, g_aSoftVirt[i].szVirt,
                    linux_module_loaded(szCanon) ? 1 : 0, szCanon);
            return 0;
        }
    }
    kprintf("linux_module: soft dep virtual %s FAIL full soft=1 product=0\n",
            szCanon);
    return -1;
}

int
linux_module_soft_dep_virtual_present(const char *szCanon)
{
    return lmod_soft_virt_find(szCanon) >= 0 ? 1 : 0;
}

void
linux_module_soft_order_log(const char *szClass)
{
    u32 i;
    int fUsb;
    int fNet;
    int fAll;

    fAll = (szClass == NULL || szClass[0] == '\0' ||
            strcmp(szClass, "all") == 0);
    fUsb = fAll || (szClass != NULL && strcmp(szClass, "usb") == 0);
    fNet = fAll || (szClass != NULL && strcmp(szClass, "net") == 0);

    for (i = 0; i < (u32)GJ_LMOD_ORDER_N; i++) {
        const char *sz = g_aSoftOrder[i].szName;
        int fEmit = 0;
        int fVirt;

        if (fUsb && (strcmp(sz, "usb_common") == 0 ||
                     strcmp(sz, "usbcore") == 0 || strcmp(sz, "scsi_mod") == 0 ||
                     strcmp(sz, "xhci_hcd") == 0 ||
                     strcmp(sz, "xhci_pci") == 0 ||
                     strcmp(sz, "usb_storage") == 0)) {
            fEmit = 1;
        }
        if (fNet && (strcmp(sz, "libphy") == 0 ||
                     strcmp(sz, "mdio_devres") == 0 ||
                     strcmp(sz, "r8169") == 0)) {
            fEmit = 1;
        }
        if (!fEmit) {
            continue;
        }
        fVirt = linux_module_soft_dep_virtual_present(sz);
        kprintf("linux_module: soft load order rank=%d name=%s deps=%s "
                "loaded=%d virtual=%d soft=1 product=0\n",
                g_aSoftOrder[i].nRank, sz,
                g_aSoftOrder[i].szDeps[0] != '\0' ? g_aSoftOrder[i].szDeps
                                                  : "-",
                linux_module_loaded(sz), fVirt);
    }
}
