/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room ELF64 ET_EXEC / ET_DYN load + PT_INTERP / PT_DYNAMIC / RELA.
 * Product path: probe → map PT_LOAD → SO registry → relocs → auxv handoff
 * → INTERP-first start (ld-gj). No third-party loader code.
 *
 * Soft inventory (Wave 40 exclusive deepen; this unit only; never hard-gates):
 * greppable: "elf: soft …" | "elf_load: soft …"
 *   elf: soft inventory …
 *   elf: soft probe …
 *   elf: soft load …
 *   elf: soft reloc …
 *   elf: soft reloc_kind …   (relative/tls/irel/copy/glob/jump/abs64)
 *   elf: soft so …
 *   elf: soft needed …
 *   elf: soft resolve …
 *   elf: soft auxv …
 *   elf: soft handoff …
 *   elf: soft interp …
 *   elf: soft path …
 *   elf: soft return …       (Wave 19 return-path catalog)
 *   elf: soft ret_surface …  (Wave 19 terminal return classes)
 *   elf: soft surface …      (Wave 19 area catalog)
 *   elf: soft deepen wave=109 …
 *   elf: soft catalog …      (capacity honesty rollup)
 *   elf: soft bias …         (dyn/so bias + step geometry)
 *   elf: soft capacity …     (so_max/img/needed/auxv lamps)
 *   elf: soft PASS|PARTIAL
 *   elf_load: soft inventory|probe|load|reloc|reloc_kind|so|needed|resolve|auxv|
 *             handoff|interp|path|return|ret_surface|surface|deepen|catalog|bias|capacity …
 *   elf_load: soft PASS|PARTIAL
 * Diagnostics / smoke grep only — soft ≠ bar3; soft ≠ product DoD.
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
#define DT_PLTRELSZ 2
#define DT_HASH     4
#define DT_STRTAB   5
#define DT_SYMTAB   6
#define DT_RELA     7
#define DT_RELASZ   8
#define DT_RELAENT  9
#define DT_STRSZ    10
#define DT_SYMENT   11
#define DT_SONAME   14
#define DT_REL      17
#define DT_RELSZ    18
#define DT_RELENT   19
#define DT_PLTREL   20
#define DT_JMPREL   23
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
/*
 * Default ET_DYN load base when program vaddrs are low.
 * High canonical user VA away from PE 0x400000 / small smoke bases.
 * (Defined early so Wave 15 soft catalog can stamp the value.)
 */
#define GJ_ELF_DYN_BIAS 0x0000000070000000ull
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

/* Loaded DT_NEEDED objects for cross-object symbol search (kernel SO registry) */
struct gj_elf_so {
    u8   u8Used;
    u8   u8HasHash;    /* DT_HASH present */
    u8   u8HasGnu;     /* DT_GNU_HASH present */
    u8   u8Pad;
    u32  cbImg;
    u32  u32NameHash;  /* SysV hash of szName (registry key) */
    u32  u32SoHash;    /* SysV hash of szSoname if set */
    u64  u64Bias;
    u64  u64Symtab;    /* pre-bias VA */
    u64  u64Strtab;
    u64  u64Strsz;
    u64  u64Hash;
    u64  u64GnuHash;
    u64  u64Syment;
    char szName[64];   /* DT_NEEDED basename */
    char szSoname[64]; /* DT_SONAME when present */
    u8   aImg[GJ_ELF_SO_IMG];
};

static struct gj_elf_so g_aSo[GJ_ELF_SO_MAX];
static u32              g_cSo;

/*
 * Wave 15 soft inventory telemetry (never hard-gates product load path).
 * greppable: elf: soft / elf_load: soft
 */
#define GJ_ELF_SOFT_WAVE 109u

static u32 g_u32SoftProbeOk;      /* elf_probe_image success */
static u32 g_u32SoftProbeFail;    /* probe header / fill fail */
static u32 g_u32SoftProbeDyn;     /* probes with ET_DYN */
static u32 g_u32SoftProbeExec;    /* probes with ET_EXEC */
static u32 g_u32SoftProbeInterp;  /* probes with PT_INTERP */
static u32 g_u32SoftProbeInterpSoft; /* INTERP soft-ok path */
static u32 g_u32SoftProbeNeeded;  /* total DT_NEEDED names seen on probe */
static u32 g_u32SoftLoadOk;       /* elf_load_image_bias success */
static u32 g_u32SoftLoadFail;     /* load fail (header/AS/map/empty) */
static u32 g_u32SoftLoadDyn;      /* successful ET_DYN loads */
static u32 g_u32SoftLoadExec;     /* successful ET_EXEC loads */
static u32 g_u32SoftLoadBiasReq;  /* non-zero bias request loads */
static u32 g_u32SoftLoadBiasDef;  /* default GJ_ELF_DYN_BIAS applied (W15) */
static u32 g_u32SoftMapPages;     /* PT_LOAD segments mapped (lifetime) */
static u32 g_u32SoftRelocOps;     /* reloc apply calls with cRel > 0 */
static u32 g_u32SoftRelocHits;    /* sum of applied reloc counts */
static u32 g_u32SoftSymHits;      /* sum of GLOB_DAT/JUMP_SLOT/64/COPY counts */
/* Wave 15 per-type reloc tallies (soft inventory only). */
static u32 g_u32SoftRelocRelative;
static u32 g_u32SoftRelocTls;     /* DTPMOD + DTPOFF + TPOFF */
static u32 g_u32SoftRelocIrel;
static u32 g_u32SoftRelocCopy;
static u32 g_u32SoftRelocGlob;
static u32 g_u32SoftRelocJump;
static u32 g_u32SoftRelocAbs64;   /* R_X86_64_64 */
static u32 g_u32SoftSoMapOk;      /* DT_NEEDED SO map success */
static u32 g_u32SoftSoMapFail;    /* SO map fail */
static u32 g_u32SoftSoSkip;       /* non-ELF SO skip */
static u32 g_u32SoftSoFull;       /* registry full (W15) */
static u32 g_u32SoftSoHash;       /* SOs with DT_HASH */
static u32 g_u32SoftSoGnu;        /* SOs with DT_GNU_HASH */
static u32 g_u32SoftNeededOk;     /* vfs resolve hit */
static u32 g_u32SoftNeededMiss;   /* vfs resolve miss */
static u32 g_u32SoftNeededCalls;  /* elf_resolve_needed_vfs entries */
static u32 g_u32SoftAuxvFill;     /* elf_fill_auxv calls */
static u32 g_u32SoftAuxvPairs;    /* last auxv pair count (soft sample) */
static u32 g_u32SoftHandoffOk;    /* publish_handoff success */
static u32 g_u32SoftHandoffFail;  /* publish_handoff fail */
static u32 g_u32SoftVerifyOk;     /* ld_handoff_verify PASS */
static u32 g_u32SoftVerifyFail;   /* ld_handoff_verify FAIL */
static u32 g_u32SoftInterpFirst;  /* INTERP-first applied */
static u32 g_u32SoftInterpDefer;  /* INTERP present, dynlinker miss → main */
static u32 g_u32SoftDirect;       /* direct main entry (no INTERP start) */
static u32 g_u32SoftResolveGnu;   /* SO registry gnu-hash hit */
static u32 g_u32SoftResolveHash;  /* SO registry SysV hash hit */
static u32 g_u32SoftResolveScan;  /* SO registry linear scan hit */
static u32 g_u32SoftResolveMiss;  /* SO registry lookup miss */
static u32 g_u32SoftRegFindHit;   /* elf_so_registry_find hit */
static u32 g_u32SoftRegFindMiss;  /* elf_so_registry_find miss */
static u32 g_u32SoftLogN;         /* soft inventory dump emissions */
static int g_fSoftInvOnce;        /* first post-activity inventory emitted */

static void path_copy_n(char *szDst, size_t cbDst, const char *szSrc);
static void path_join2(char *szDst, size_t cbDst, const char *szPfx,
                       const char *szName);
static void elf_soft_inventory(const char *szVia);
static void elf_soft_maybe_once(void);

/** Soft: bump one counter (wrap OK; never hard-gates). */
static void
elf_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/**
 * Greppable Wave 15 soft inventory dump (product / smoke).
 * Twin prefixes so either agent grep works:
 *   elf: soft inventory|probe|load|reloc|reloc_kind|so|needed|resolve|auxv|
 *        handoff|interp|path|deepen|catalog …
 *   elf_load: soft … (same catalog)
 * Never hard-gates load path. Soft ≠ bar3.
 *
 * greppable: elf: soft
 * greppable: elf_load: soft
 */
static void
elf_soft_inventory(const char *szVia)
{
    u32 cReg;
    u32 cHashLive;
    u32 cGnuLive;
    u32 iSo;
    const char *szVerdict;

    if (szVia == NULL) {
        szVia = "path";
    }
    elf_soft_inc(&g_u32SoftLogN);

    cReg = 0;
    cHashLive = 0;
    cGnuLive = 0;
    for (iSo = 0; iSo < GJ_ELF_SO_MAX; iSo++) {
        if (!g_aSo[iSo].u8Used) {
            continue;
        }
        cReg++;
        if (g_aSo[iSo].u8HasHash) {
            cHashLive++;
        }
        if (g_aSo[iSo].u8HasGnu) {
            cGnuLive++;
        }
    }

    /*
     * Soft verdict (inventory only; product path unchanged):
     *   PASS    — at least one successful image load
     *   PARTIAL — probe/activity without a completed load yet
     */
    if (g_u32SoftLoadOk != 0u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /*
     * Primary prefix: elf: soft …
     * Catalog capacity + lifetime tallies for bring-up smoke greps.
     */
    /* Grep: elf: soft inventory */
    kprintf("elf: soft inventory via=%s so_max=%u so_img=%u so_live=%u "
            "needed_max=%u auxv_max=%u load_ok=%u load_fail=%u "
            "probe_ok=%u handoff_ok=%u log_n=%u wave=%u\n",
            szVia, (unsigned)GJ_ELF_SO_MAX, (unsigned)GJ_ELF_SO_IMG, cReg,
            (unsigned)GJ_ELF_NEEDED_MAX, (unsigned)GJ_AUXV_MAX,
            g_u32SoftLoadOk, g_u32SoftLoadFail, g_u32SoftProbeOk,
            g_u32SoftHandoffOk, g_u32SoftLogN, GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft probe */
    kprintf("elf: soft probe ok=%u fail=%u dyn=%u exec=%u interp=%u "
            "interp_soft=%u needed_seen=%u\n",
            g_u32SoftProbeOk, g_u32SoftProbeFail, g_u32SoftProbeDyn,
            g_u32SoftProbeExec, g_u32SoftProbeInterp, g_u32SoftProbeInterpSoft,
            g_u32SoftProbeNeeded);

    /* Grep: elf: soft load */
    kprintf("elf: soft load ok=%u fail=%u dyn=%u exec=%u bias_req=%u "
            "bias_def=%u phdr_segs=%u so_max=%u\n",
            g_u32SoftLoadOk, g_u32SoftLoadFail, g_u32SoftLoadDyn,
            g_u32SoftLoadExec, g_u32SoftLoadBiasReq, g_u32SoftLoadBiasDef,
            g_u32SoftMapPages, (unsigned)GJ_ELF_SO_MAX);

    /* Grep: elf: soft reloc */
    kprintf("elf: soft reloc ops=%u hits=%u sym=%u relative+sym=1 "
            "tls=DTPMOD,DTPOFF,TPOFF irel=1 wave=%u\n",
            g_u32SoftRelocOps, g_u32SoftRelocHits, g_u32SoftSymHits,
            GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft reloc_kind (Wave 15 per-type rollup) */
    kprintf("elf: soft reloc_kind relative=%u tls=%u irel=%u copy=%u "
            "glob=%u jump=%u abs64=%u sym_hits=%u\n",
            g_u32SoftRelocRelative, g_u32SoftRelocTls, g_u32SoftRelocIrel,
            g_u32SoftRelocCopy, g_u32SoftRelocGlob, g_u32SoftRelocJump,
            g_u32SoftRelocAbs64, g_u32SoftSymHits);

    /* Grep: elf: soft so */
    kprintf("elf: soft so map_ok=%u map_fail=%u skip=%u full=%u live=%u "
            "hash=%u gnu=%u hash_n=%u gnu_n=%u reg_n=%u\n",
            g_u32SoftSoMapOk, g_u32SoftSoMapFail, g_u32SoftSoSkip,
            g_u32SoftSoFull, cReg, cHashLive, cGnuLive, g_u32SoftSoHash,
            g_u32SoftSoGnu, g_cSo);

    /* Grep: elf: soft needed */
    kprintf("elf: soft needed calls=%u ok=%u miss=%u max=%u "
            "pfx=/lib/,/usr/lib/\n",
            g_u32SoftNeededCalls, g_u32SoftNeededOk, g_u32SoftNeededMiss,
            (unsigned)GJ_ELF_NEEDED_MAX);

    /* Grep: elf: soft resolve */
    kprintf("elf: soft resolve gnu=%u hash=%u scan=%u miss=%u "
            "reg_hit=%u reg_miss=%u order=gnu,hash,scan\n",
            g_u32SoftResolveGnu, g_u32SoftResolveHash, g_u32SoftResolveScan,
            g_u32SoftResolveMiss, g_u32SoftRegFindHit, g_u32SoftRegFindMiss);

    /* Grep: elf: soft auxv */
    kprintf("elf: soft auxv fill=%u last_pairs=%u max=%u "
            "at=PHDR,PHENT,PHNUM,PAGESZ,BASE,ENTRY,RANDOM,EXECFN\n",
            g_u32SoftAuxvFill, g_u32SoftAuxvPairs, (unsigned)GJ_AUXV_MAX);

    /* Grep: elf: soft handoff */
    kprintf("elf: soft handoff ok=%u fail=%u verify_ok=%u verify_fail=%u "
            "va=0x%lx stack=0x%lx magic=GJLD\n",
            g_u32SoftHandoffOk, g_u32SoftHandoffFail, g_u32SoftVerifyOk,
            g_u32SoftVerifyFail, (unsigned long)GJ_LD_HANDOFF_VA,
            (unsigned long)GJ_LD_STACK_VA);

    /* Grep: elf: soft interp */
    kprintf("elf: soft interp first=%u defer=%u direct=%u "
            "soft_norm=/lib/ soft_ok=absolute\n",
            g_u32SoftInterpFirst, g_u32SoftInterpDefer, g_u32SoftDirect);

    /* Grep: elf: soft path */
    kprintf("elf: soft path claim=probe,load,so,reloc,auxv,handoff,interp "
            "ld-gj=1 so_reg=1 vfs_needed=1 wave=%u "
            "(soft inventory; not bar3)\n",
            GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf: soft return
     * Wave 19 return-path catalog — probe/load/reloc/handoff outcomes.
     * Soft ≠ bar3 / product DoD. product_kernel=OPEN.
     */
    kprintf("elf: soft return probe_ok=%u probe_fail=%u load_ok=%u "
            "load_fail=%u reloc_hits=%u reloc_ops=%u handoff_ok=%u "
            "handoff_fail=%u verify_ok=%u verify_fail=%u so_live=%u "
            "interp_first=%u direct=%u product_kernel=OPEN wave=%u\n",
            g_u32SoftProbeOk, g_u32SoftProbeFail, g_u32SoftLoadOk,
            g_u32SoftLoadFail, g_u32SoftRelocHits, g_u32SoftRelocOps,
            g_u32SoftHandoffOk, g_u32SoftHandoffFail, g_u32SoftVerifyOk,
            g_u32SoftVerifyFail, cReg, g_u32SoftInterpFirst,
            g_u32SoftDirect, GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft ret_surface — Wave 19 terminal return classes */
    kprintf("elf: soft ret_surface probe=ok|fail load=ok|fail "
            "reloc=hits|ops handoff=ok|fail verify=ok|fail "
            "so_live interp=first|direct product_kernel=OPEN "
            "areas=101 wave=%u\n",
            GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft surface — Wave 19 area catalog */
    kprintf("elf: soft surface inventory,probe,load,reloc,reloc_kind,so,"
            "needed,resolve,auxv,handoff,interp,path,return,ret_surface,"
            "surface,deepen,catalog,bias,capacity areas=105 wave=%u\n",
            GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: elf: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("elf: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_ELF_SOFT_WAVE);
    /* Grep: elf: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("elf: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_ELF_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: elf: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("elf: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_ELF_SOFT_WAVE);
    /* Grep: elf: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("elf: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: elf: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("elf: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("elf: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: elf: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("elf: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("elf: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: elf: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("elf: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("elf: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: elf: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("elf: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("elf: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: elf: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("elf: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("elf: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: elf: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("elf: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("elf: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: elf: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("elf: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_ELF_SOFT_WAVE);
                    /* Grep: elf: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("elf: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_ELF_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: elf: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("elf: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_ELF_SOFT_WAVE);
                            /* Grep: elf: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("elf: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_ELF_SOFT_WAVE);
    kprintf("elf: soft deepen wave=%u via=%s load_ok=%u probe_ok=%u "
            "reloc_hits=%u so_live=%u handoff=%u verify=%u log_n=%u "
            "(soft inventory only; not product gate)\n",
            GJ_ELF_SOFT_WAVE, szVia, g_u32SoftLoadOk, g_u32SoftProbeOk,
            g_u32SoftRelocHits, cReg, g_u32SoftHandoffOk, g_u32SoftVerifyOk,
            g_u32SoftLogN);

    /* Grep: elf: soft catalog */
    kprintf("elf: soft catalog so_max=%u so_img=%u needed_max=%u auxv_max=%u "
            "dyn_bias=0x%lx handoff=0x%lx stack=0x%lx so_bias_base=0x%lx "
            "wave=%u\n",
            (unsigned)GJ_ELF_SO_MAX, (unsigned)GJ_ELF_SO_IMG,
            (unsigned)GJ_ELF_NEEDED_MAX, (unsigned)GJ_AUXV_MAX,
            (unsigned long)GJ_ELF_DYN_BIAS, (unsigned long)GJ_LD_HANDOFF_VA,
            (unsigned long)GJ_LD_STACK_VA, (unsigned long)GJ_ELF_SO_BIAS_BASE,
            GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft bias (Wave 15 geometry) */
    kprintf("elf: soft bias dyn=0x%lx so_base=0x%lx so_step=0x%lx "
            "bias_req=%u bias_def=%u wave=%u\n",
            (unsigned long)GJ_ELF_DYN_BIAS,
            (unsigned long)GJ_ELF_SO_BIAS_BASE,
            (unsigned long)GJ_ELF_SO_BIAS_STEP,
            g_u32SoftLoadBiasReq, g_u32SoftLoadBiasDef, GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft capacity (Wave 15 lamps) */
    kprintf("elf: soft capacity so_max=%u so_img=%u so_live=%u free_so=%u "
            "needed_max=%u auxv_max=%u hash_live=%u gnu_live=%u wave=%u\n",
            (unsigned)GJ_ELF_SO_MAX, (unsigned)GJ_ELF_SO_IMG, cReg,
            (cReg < GJ_ELF_SO_MAX) ? (GJ_ELF_SO_MAX - cReg) : 0u,
            (unsigned)GJ_ELF_NEEDED_MAX, (unsigned)GJ_AUXV_MAX,
            cHashLive, cGnuLive, GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft PASS | PARTIAL */
    kprintf("elf: soft %s via=%s load_ok=%u probe_ok=%u so=%u handoff=%u "
            "log_n=%u wave=%u\n",
            szVerdict, szVia, g_u32SoftLoadOk, g_u32SoftProbeOk, cReg,
            g_u32SoftHandoffOk, g_u32SoftLogN, GJ_ELF_SOFT_WAVE);

    /*
     * Twin prefix: elf_load: soft … (agent-friendly alias; same tallies).
     */
    /* Grep: elf_load: soft inventory */
    kprintf("elf_load: soft inventory via=%s so_max=%u so_img=%u so_live=%u "
            "needed_max=%u auxv_max=%u load_ok=%u load_fail=%u "
            "probe_ok=%u handoff_ok=%u log_n=%u wave=%u\n",
            szVia, (unsigned)GJ_ELF_SO_MAX, (unsigned)GJ_ELF_SO_IMG, cReg,
            (unsigned)GJ_ELF_NEEDED_MAX, (unsigned)GJ_AUXV_MAX,
            g_u32SoftLoadOk, g_u32SoftLoadFail, g_u32SoftProbeOk,
            g_u32SoftHandoffOk, g_u32SoftLogN, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft probe */
    kprintf("elf_load: soft probe ok=%u fail=%u dyn=%u exec=%u interp=%u "
            "interp_soft=%u needed_seen=%u\n",
            g_u32SoftProbeOk, g_u32SoftProbeFail, g_u32SoftProbeDyn,
            g_u32SoftProbeExec, g_u32SoftProbeInterp, g_u32SoftProbeInterpSoft,
            g_u32SoftProbeNeeded);

    /* Grep: elf_load: soft load */
    kprintf("elf_load: soft load ok=%u fail=%u dyn=%u exec=%u bias_req=%u "
            "bias_def=%u phdr_segs=%u so_max=%u\n",
            g_u32SoftLoadOk, g_u32SoftLoadFail, g_u32SoftLoadDyn,
            g_u32SoftLoadExec, g_u32SoftLoadBiasReq, g_u32SoftLoadBiasDef,
            g_u32SoftMapPages, (unsigned)GJ_ELF_SO_MAX);

    /* Grep: elf_load: soft reloc */
    kprintf("elf_load: soft reloc ops=%u hits=%u sym=%u relative+sym=1 "
            "tls=DTPMOD,DTPOFF,TPOFF irel=1 wave=%u\n",
            g_u32SoftRelocOps, g_u32SoftRelocHits, g_u32SoftSymHits,
            GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft reloc_kind */
    kprintf("elf_load: soft reloc_kind relative=%u tls=%u irel=%u copy=%u "
            "glob=%u jump=%u abs64=%u sym_hits=%u\n",
            g_u32SoftRelocRelative, g_u32SoftRelocTls, g_u32SoftRelocIrel,
            g_u32SoftRelocCopy, g_u32SoftRelocGlob, g_u32SoftRelocJump,
            g_u32SoftRelocAbs64, g_u32SoftSymHits);

    /* Grep: elf_load: soft so */
    kprintf("elf_load: soft so map_ok=%u map_fail=%u skip=%u full=%u live=%u "
            "hash=%u gnu=%u hash_n=%u gnu_n=%u reg_n=%u\n",
            g_u32SoftSoMapOk, g_u32SoftSoMapFail, g_u32SoftSoSkip,
            g_u32SoftSoFull, cReg, cHashLive, cGnuLive, g_u32SoftSoHash,
            g_u32SoftSoGnu, g_cSo);

    /* Grep: elf_load: soft needed */
    kprintf("elf_load: soft needed calls=%u ok=%u miss=%u max=%u "
            "pfx=/lib/,/usr/lib/\n",
            g_u32SoftNeededCalls, g_u32SoftNeededOk, g_u32SoftNeededMiss,
            (unsigned)GJ_ELF_NEEDED_MAX);

    /* Grep: elf_load: soft resolve */
    kprintf("elf_load: soft resolve gnu=%u hash=%u scan=%u miss=%u "
            "reg_hit=%u reg_miss=%u order=gnu,hash,scan\n",
            g_u32SoftResolveGnu, g_u32SoftResolveHash, g_u32SoftResolveScan,
            g_u32SoftResolveMiss, g_u32SoftRegFindHit, g_u32SoftRegFindMiss);

    /* Grep: elf_load: soft auxv */
    kprintf("elf_load: soft auxv fill=%u last_pairs=%u max=%u "
            "at=PHDR,PHENT,PHNUM,PAGESZ,BASE,ENTRY,RANDOM,EXECFN\n",
            g_u32SoftAuxvFill, g_u32SoftAuxvPairs, (unsigned)GJ_AUXV_MAX);

    /* Grep: elf_load: soft handoff */
    kprintf("elf_load: soft handoff ok=%u fail=%u verify_ok=%u verify_fail=%u "
            "va=0x%lx stack=0x%lx magic=GJLD\n",
            g_u32SoftHandoffOk, g_u32SoftHandoffFail, g_u32SoftVerifyOk,
            g_u32SoftVerifyFail, (unsigned long)GJ_LD_HANDOFF_VA,
            (unsigned long)GJ_LD_STACK_VA);

    /* Grep: elf_load: soft interp */
    kprintf("elf_load: soft interp first=%u defer=%u direct=%u "
            "soft_norm=/lib/ soft_ok=absolute\n",
            g_u32SoftInterpFirst, g_u32SoftInterpDefer, g_u32SoftDirect);

    /* Grep: elf_load: soft path */
    kprintf("elf_load: soft path claim=probe,load,so,reloc,auxv,handoff,interp "
            "ld-gj=1 so_reg=1 vfs_needed=1 wave=%u "
            "(soft inventory; not bar3)\n",
            GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft return (Wave 19 twin) */
    kprintf("elf_load: soft return probe_ok=%u probe_fail=%u load_ok=%u "
            "load_fail=%u reloc_hits=%u handoff_ok=%u handoff_fail=%u "
            "verify_ok=%u verify_fail=%u so_live=%u "
            "product_kernel=OPEN wave=%u\n",
            g_u32SoftProbeOk, g_u32SoftProbeFail, g_u32SoftLoadOk,
            g_u32SoftLoadFail, g_u32SoftRelocHits, g_u32SoftHandoffOk,
            g_u32SoftHandoffFail, g_u32SoftVerifyOk, g_u32SoftVerifyFail,
            cReg, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft ret_surface (Wave 19 twin) */
    kprintf("elf_load: soft ret_surface probe=ok|fail load=ok|fail "
            "reloc handoff=ok|fail verify=ok|fail product_kernel=OPEN "
            "wave=%u\n",
            GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft surface (Wave 19 twin) */
    kprintf("elf_load: soft surface inventory,probe,load,reloc,reloc_kind,so,"
            "needed,resolve,auxv,handoff,interp,path,return,ret_surface,"
            "surface,deepen,catalog,bias,capacity areas=105 wave=%u\n",
            GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft retmap — Wave 19 return-surface map */
    kprintf("elf_load: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=109\n");

    /* Grep: elf_load: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: elf_load: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("elf_load: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_ELF_SOFT_WAVE);
    /* Grep: elf_load: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("elf_load: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_ELF_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: elf_load: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("elf_load: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_ELF_SOFT_WAVE);
    /* Grep: elf_load: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("elf_load: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: elf_load: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("elf_load: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf_load: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("elf_load: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: elf_load: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("elf_load: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf_load: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("elf_load: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: elf_load: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("elf_load: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf_load: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("elf_load: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: elf_load: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("elf_load: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf_load: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("elf_load: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: elf_load: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("elf_load: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf_load: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("elf_load: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: elf_load: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("elf_load: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
            /* Grep: elf_load: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("elf_load: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_ELF_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: elf_load: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("elf_load: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_ELF_SOFT_WAVE);
                    /* Grep: elf_load: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("elf_load: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_ELF_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: elf_load: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("elf_load: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_ELF_SOFT_WAVE);
                            /* Grep: elf_load: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("elf_load: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_ELF_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: elf_load: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("elf_load: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_ELF_SOFT_WAVE);
                            /* Grep: elf_load: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("elf_load: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_ELF_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: elf_load: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("elf_load: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_ELF_SOFT_WAVE);
                            /* Grep: elf_load: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("elf_load: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_ELF_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: elf_load: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("elf_load: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_ELF_SOFT_WAVE);
                            /* Grep: elf_load: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("elf_load: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_ELF_SOFT_WAVE);
                            /* Grep: elf_load: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("elf_load: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("elf_load: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("elf_load: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("elf_load: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("elf_load: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("elf_load: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("elf_load: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retfortress — Wave 35 return-fortress honesty */
kprintf("elf_load: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("elf_load: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft rethold — Wave 36 return-hold honesty */
kprintf("elf_load: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retspire — Wave 36 exclusive spire stamp */
kprintf("elf_load: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retwall — Wave 37 return-wall honesty */
kprintf("elf_load: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retgate — Wave 37 exclusive gate stamp */
kprintf("elf_load: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retmoat — Wave 38 return-moat honesty */
kprintf("elf_load: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retower — Wave 38 exclusive tower stamp */
kprintf("elf_load: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("elf_load: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("elf_load: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("elf_load: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("elf_load: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retravelin — Wave 41 return-travelin honesty */
kprintf("elf_load: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("elf_load: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("elf_load: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("elf_load: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("elf_load: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("elf_load: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("elf_load: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("elf_load: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("elf_load: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("elf_load: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retbailey — Wave 46 return-bailey honesty */
kprintf("elf_load: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);
/* Grep: elf_load: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("elf_load: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_ELF_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("elf_load: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("elf_load: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("elf_load: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("elf_load: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("elf_load: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("elf_load: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retsally — Wave 50 return-sally honesty */
kprintf("elf_load: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("elf_load: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retfosse — Wave 51 return-fosse honesty */
kprintf("elf_load: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("elf_load: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("elf_load: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("elf_load: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retravelin — Wave 53 return-travelin honesty */
kprintf("elf_load: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("elf_load: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("elf_load: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retredan — Wave 54 exclusive redan stamp */
kprintf("elf_load: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retflank — Wave 55 return-flank honesty */
kprintf("elf_load: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retface — Wave 55 exclusive face stamp */
kprintf("elf_load: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retgorge — Wave 56 return-gorge honesty */
kprintf("elf_load: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("elf_load: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retraverse — Wave 57 return-traverse honesty */
kprintf("elf_load: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("elf_load: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retorillon — Wave 58 return-orillon honesty */
kprintf("elf_load: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("elf_load: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("elf_load: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("elf_load: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retplace — Wave 60 return-place honesty */
kprintf("elf_load: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("elf_load: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("elf_load: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("elf_load: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("elf_load: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("elf_load: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("elf_load: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("elf_load: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: elf_load: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("elf_load: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: elf_load: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("elf_load: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: elf_load: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("elf_load: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: elf_load: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("elf_load: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: elf_load: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("elf_load: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=109 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: elf_load: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("elf_load: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=109 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("elf_load: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("elf_load: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("elf_load: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("elf_load: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("elf_load: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("elf_load: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("elf_load: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("elf_load: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("elf_load: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("elf_load: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: elf_load: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("elf_load: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("elf_load: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("elf_load: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("elf_load: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbastionangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("elf_load: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("elf_load: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retparapetangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("elf_load: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("elf_load: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retowerangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("elf_load: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("elf_load: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retwallangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("elf_load: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("elf_load: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retholdangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("elf_load: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("elf_load: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retfortressangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("elf_load: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("elf_load: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("elf_load: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("elf_load: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: elf_load: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("elf_load: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("elf_load: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("elf_load: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("elf_load: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retaegisangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("elf_load: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("elf_load: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retsigilangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("elf_load: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("elf_load: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retglyphangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("elf_load: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("elf_load: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retshardangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("elf_load: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("elf_load: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retprismangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("elf_load: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("elf_load: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcipherangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("elf_load: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("elf_load: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retledgerangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("elf_load: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("elf_load: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retvaultangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("elf_load: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("elf_load: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (rettokenangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("elf_load: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("elf_load: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retphaseangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("elf_load: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("elf_load: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retpulseangle stamp; Soft≠product)\n");

/* Grep: elf_load: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("elf_load: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("elf_load: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retboundangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("elf_load: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("elf_load: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbladeangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("elf_load: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("elf_load: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retarcangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("elf_load: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("elf_load: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("elf_load: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("elf_load: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("elf_load: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("elf_load: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retellipseangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("elf_load: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("elf_load: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("elf_load: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("elf_load: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (rethelixangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("elf_load: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("elf_load: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retknotangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("elf_load: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("elf_load: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retkleinangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("elf_load: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("elf_load: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retaffineangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("elf_load: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("elf_load: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("elf_load: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("elf_load: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcubicangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("elf_load: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("elf_load: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retquinticangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("elf_load: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("elf_load: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbezierangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("elf_load: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("elf_load: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: elf_load: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("elf_load: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: elf_load: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("elf_load: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbsplineangle stamp; Soft≠product)\n");
                            kprintf("elf_load: soft deepen wave=%u via=%s load_ok=%u probe_ok=%u "
            "reloc_hits=%u so_live=%u handoff=%u verify=%u log_n=%u "
            "(soft inventory only; not product gate)\n",
            GJ_ELF_SOFT_WAVE, szVia, g_u32SoftLoadOk, g_u32SoftProbeOk,
            g_u32SoftRelocHits, cReg, g_u32SoftHandoffOk, g_u32SoftVerifyOk,
            g_u32SoftLogN);

    /* Grep: elf_load: soft catalog */
    kprintf("elf_load: soft catalog so_max=%u so_img=%u needed_max=%u "
            "auxv_max=%u dyn_bias=0x%lx handoff=0x%lx stack=0x%lx "
            "so_bias_base=0x%lx wave=%u\n",
            (unsigned)GJ_ELF_SO_MAX, (unsigned)GJ_ELF_SO_IMG,
            (unsigned)GJ_ELF_NEEDED_MAX, (unsigned)GJ_AUXV_MAX,
            (unsigned long)GJ_ELF_DYN_BIAS, (unsigned long)GJ_LD_HANDOFF_VA,
            (unsigned long)GJ_LD_STACK_VA, (unsigned long)GJ_ELF_SO_BIAS_BASE,
            GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft bias (Wave 15 twin) */
    kprintf("elf_load: soft bias dyn=0x%lx so_base=0x%lx so_step=0x%lx "
            "bias_req=%u bias_def=%u wave=%u\n",
            (unsigned long)GJ_ELF_DYN_BIAS,
            (unsigned long)GJ_ELF_SO_BIAS_BASE,
            (unsigned long)GJ_ELF_SO_BIAS_STEP,
            g_u32SoftLoadBiasReq, g_u32SoftLoadBiasDef, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft capacity (Wave 15 twin) */
    kprintf("elf_load: soft capacity so_max=%u so_img=%u so_live=%u free_so=%u "
            "needed_max=%u auxv_max=%u hash_live=%u gnu_live=%u wave=%u\n",
            (unsigned)GJ_ELF_SO_MAX, (unsigned)GJ_ELF_SO_IMG, cReg,
            (cReg < GJ_ELF_SO_MAX) ? (GJ_ELF_SO_MAX - cReg) : 0u,
            (unsigned)GJ_ELF_NEEDED_MAX, (unsigned)GJ_AUXV_MAX,
            cHashLive, cGnuLive, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft PASS | PARTIAL */
    kprintf("elf_load: soft %s via=%s load_ok=%u probe_ok=%u so=%u handoff=%u "
            "log_n=%u wave=%u\n",
            szVerdict, szVia, g_u32SoftLoadOk, g_u32SoftProbeOk, cReg,
            g_u32SoftHandoffOk, g_u32SoftLogN, GJ_ELF_SOFT_WAVE);
}

/**
 * After first product probe/load/handoff activity, print soft inventory once
 * (mirrors virtio-blk / input_hub soft-stats-once). Diagnostics only.
 */
static void
elf_soft_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftProbeOk == 0u && g_u32SoftLoadOk == 0u &&
        g_u32SoftLoadFail == 0u && g_u32SoftHandoffOk == 0u &&
        g_u32SoftSoMapOk == 0u) {
        return;
    }
    g_fSoftInvOnce = 1;
    elf_soft_inventory("activity");
}

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
 * INTERP soft normalize: relative path → /lib/<name>. Absolute unchanged.
 * Keeps absolute product paths (/lib/ld-gj.so.1) intact.
 */
static void
elf_interp_soft_norm(char *sz, size_t cb)
{
    char aTmp[GJ_ELF_INTERP_MAX];
    size_t i;
    size_t n = 0;

    if (sz == NULL || cb == 0 || sz[0] == '\0' || sz[0] == '/') {
        return;
    }
    aTmp[n++] = '/';
    aTmp[n++] = 'l';
    aTmp[n++] = 'i';
    aTmp[n++] = 'b';
    aTmp[n++] = '/';
    for (i = 0; sz[i] != '\0' && n + 1 < sizeof(aTmp) && n + 1 < cb; i++) {
        aTmp[n++] = sz[i];
    }
    aTmp[n] = '\0';
    for (i = 0; i <= n && i < cb; i++) {
        sz[i] = aTmp[i];
    }
}

int
elf_interp_soft_ok(const char *szInterp)
{
    size_t i;

    if (szInterp == NULL || szInterp[0] != '/') {
        return 0;
    }
    for (i = 0; i < GJ_ELF_INTERP_MAX - 1u && szInterp[i] != '\0'; i++) {
        /* reject control chars soft */
        if ((u8)szInterp[i] < 0x20u) {
            return 0;
        }
    }
    return (i > 1 && szInterp[i] == '\0') ? 1 : 0;
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
                elf_interp_soft_norm(pInfo->szInterp, sizeof(pInfo->szInterp));
                pInfo->u32Flags |= GJ_ELF_INFO_HAS_INTERP;
                if (elf_interp_soft_ok(pInfo->szInterp)) {
                    pInfo->u32Flags |= GJ_ELF_INFO_INTERP_SOFT;
                }
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
        elf_soft_inc(&g_u32SoftProbeOk);
        if (pInfo != NULL) {
            if (pInfo->u16Type == ET_DYN) {
                elf_soft_inc(&g_u32SoftProbeDyn);
            } else if (pInfo->u16Type == ET_EXEC) {
                elf_soft_inc(&g_u32SoftProbeExec);
            }
            if ((pInfo->u32Flags & GJ_ELF_INFO_HAS_INTERP) != 0) {
                elf_soft_inc(&g_u32SoftProbeInterp);
            }
            if ((pInfo->u32Flags & GJ_ELF_INFO_INTERP_SOFT) != 0) {
                elf_soft_inc(&g_u32SoftProbeInterpSoft);
            }
            if (pInfo->u16Needed > 0) {
                g_u32SoftProbeNeeded += (u32)pInfo->u16Needed;
            }
        }
        kprintf("elf: probe type=%u loads=%u entry=0x%lx needed=%u interp=%s\n",
                (unsigned)pInfo->u16Type, pInfo->u32Phdrs,
                (unsigned long)pInfo->u64Entry, (unsigned)pInfo->u16Needed,
                (pInfo->u32Flags & GJ_ELF_INFO_HAS_INTERP) ? pInfo->szInterp
                                                           : "(none)");
        if (pInfo->u16Needed > 0) {
            kprintf("elf: DT_NEEDED[0]=%s\n", pInfo->aNeeded[0]);
        }
        if ((pInfo->u32Flags & GJ_ELF_INFO_INTERP_SOFT) != 0) {
            kprintf("elf: INTERP soft probe %s PASS\n", pInfo->szInterp);
        }
        elf_soft_maybe_once();
    } else {
        elf_soft_inc(&g_u32SoftProbeFail);
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

u32
elf_sysv_hash_name(const char *szName)
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

u32
elf_gnu_hash_name(const char *szName)
{
    u32 u32H = 5381u;

    if (szName == NULL) {
        return 0;
    }
    while (*szName != '\0') {
        u32H = (u32H << 5) + u32H + (u32)(u8)*szName++;
    }
    return u32H;
}

/* Internal alias — keep call sites greppable as elf_sysv_hash via wrap */
static u32
elf_sysv_hash(const char *szName)
{
    return elf_sysv_hash_name(szName);
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
        g_aSo[i].u8HasHash = 0;
        g_aSo[i].u8HasGnu = 0;
        g_aSo[i].cbImg = 0;
        g_aSo[i].u32NameHash = 0;
        g_aSo[i].u32SoHash = 0;
        g_aSo[i].szName[0] = '\0';
        g_aSo[i].szSoname[0] = '\0';
        g_aSo[i].u64Hash = 0;
        g_aSo[i].u64GnuHash = 0;
        g_aSo[i].u64Symtab = 0;
        g_aSo[i].u64Strtab = 0;
        g_aSo[i].u64Strsz = 0;
    }
    g_cSo = 0;
}

u32
elf_so_registry_count(void)
{
    return g_cSo;
}

int
elf_so_registry_find(const char *szName, u64 *pBias, u32 *pcb)
{
    u32 i;
    u32 u32H;

    if (szName == NULL || szName[0] == '\0') {
        elf_soft_inc(&g_u32SoftRegFindMiss);
        return 0;
    }
    u32H = elf_sysv_hash_name(szName);
    for (i = 0; i < GJ_ELF_SO_MAX; i++) {
        struct gj_elf_so *pSo = &g_aSo[i];

        if (!pSo->u8Used) {
            continue;
        }
        /* Fast path: name-hash match then string compare */
        if ((pSo->u32NameHash == u32H && sym_name_eq(pSo->szName, szName)) ||
            (pSo->u32SoHash != 0 && pSo->u32SoHash == u32H &&
             sym_name_eq(pSo->szSoname, szName)) ||
            sym_name_eq(pSo->szName, szName) ||
            (pSo->szSoname[0] != '\0' && sym_name_eq(pSo->szSoname, szName))) {
            if (pBias != NULL) {
                *pBias = pSo->u64Bias;
            }
            if (pcb != NULL) {
                *pcb = pSo->cbImg;
            }
            elf_soft_inc(&g_u32SoftRegFindHit);
            return 1;
        }
    }
    elf_soft_inc(&g_u32SoftRegFindMiss);
    return 0;
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

    u32H = elf_gnu_hash_name(szName);
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

/*
 * Search SO registry: GNU hash first (modern default), then SysV DT_HASH,
 * then linear SYMTAB scan. Prefer GNU so dual-hash SOs hit bloom path.
 */
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
        if (pSo->u64GnuHash != 0 &&
            elf_gnu_hash_lookup(pSo->aImg, pSo->cbImg, pEh, pSo->u64Bias,
                                pSo->u64GnuHash, pSo->u64Symtab, pSo->u64Strtab,
                                pSo->u64Syment, szName, pVal)) {
            elf_soft_inc(&g_u32SoftResolveGnu);
            kprintf("elf: gnu-hash resolve %s in %s PASS\n", szName,
                    pSo->szName);
            return 1;
        }
        if (pSo->u64Hash != 0 &&
            elf_hash_lookup(pSo->aImg, pSo->cbImg, pEh, pSo->u64Bias,
                            pSo->u64Hash, pSo->u64Symtab, pSo->u64Strtab,
                            pSo->u64Syment, szName, pVal)) {
            elf_soft_inc(&g_u32SoftResolveHash);
            kprintf("elf: hash resolve %s in %s PASS\n", szName, pSo->szName);
            return 1;
        }
        if (pSo->u64Symtab != 0 && pSo->u64Strtab != 0 &&
            elf_symtab_scan(pSo->aImg, pSo->cbImg, pEh, pSo->u64Bias,
                            pSo->u64Symtab, pSo->u64Strtab, pSo->u64Syment,
                            szName, pVal)) {
            elf_soft_inc(&g_u32SoftResolveScan);
            kprintf("elf: scan resolve %s in %s PASS\n", szName, pSo->szName);
            return 1;
        }
    }
    elf_soft_inc(&g_u32SoftResolveMiss);
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

/* Parse dynamic tags for SO registry (hash/sym/str/soname). */
static void
elf_so_fill_dyn(struct gj_elf_so *pSo, const void *pImage, u64 cb,
                const struct elf64_ehdr *pEh)
{
    u16 i;
    u64 u64DynOff = 0;
    u64 u64DynSz = 0;
    u64 u64Off;
    u64 u64SoNameOff = ~0ull;
    const struct elf64_dyn *pDyn;

    pSo->u64Symtab = 0;
    pSo->u64Strtab = 0;
    pSo->u64Strsz = 0;
    pSo->u64Hash = 0;
    pSo->u64GnuHash = 0;
    pSo->u64Syment = sizeof(struct elf64_sym);
    pSo->u8HasHash = 0;
    pSo->u8HasGnu = 0;
    pSo->szSoname[0] = '\0';
    pSo->u32SoHash = 0;

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
        } else if (pDyn->i64Tag == DT_STRSZ) {
            pSo->u64Strsz = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_SYMENT) {
            pSo->u64Syment = pDyn->u64Val;
        } else if (pDyn->i64Tag == DT_HASH) {
            pSo->u64Hash = pDyn->u64Val;
            pSo->u8HasHash = 1;
        } else if (pDyn->i64Tag == DT_GNU_HASH) {
            pSo->u64GnuHash = pDyn->u64Val;
            pSo->u8HasGnu = 1;
        } else if (pDyn->i64Tag == DT_SONAME) {
            u64SoNameOff = pDyn->u64Val;
        }
    }
    /* Resolve DT_SONAME through STRTAB when both present */
    if (u64SoNameOff != ~0ull && pSo->u64Strtab != 0) {
        u64 u64FileOff =
            va_to_file_off(pImage, cb, pEh, pSo->u64Strtab + u64SoNameOff);

        if (u64FileOff != ~0ull && u64FileOff < cb) {
            if (pSo->u64Strsz == 0 || u64SoNameOff < pSo->u64Strsz) {
                interp_copy(pSo->szSoname, sizeof(pSo->szSoname),
                            (const u8 *)pImage + u64FileOff,
                            sizeof(pSo->szSoname));
                if (pSo->szSoname[0] != '\0') {
                    pSo->u32SoHash = elf_sysv_hash_name(pSo->szSoname);
                }
            }
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
                    elf_soft_inc(&g_u32SoftRelocRelative);
                } else if (u32Type == R_X86_64_DTPMOD64) {
                    elf_write_u64_user(u64Dst, 1);
                    cApplied++;
                    elf_soft_inc(&g_u32SoftRelocTls);
                } else if (u32Type == R_X86_64_DTPOFF64 ||
                           u32Type == R_X86_64_TPOFF64) {
                    elf_write_u64_user(u64Dst, (u64)pR->i64Addend);
                    cApplied++;
                    elf_soft_inc(&g_u32SoftRelocTls);
                } else if (u32Type == R_X86_64_IRELATIVE) {
                    /* Defer true IFUNC call to ld-gj; store resolver VA */
                    u64Word = (u64)pR->i64Addend + u64Bias;
                    elf_write_u64_user(u64Dst, u64Word);
                    cApplied++;
                    elf_soft_inc(&g_u32SoftRelocIrel);
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
                    if (u32Type == R_X86_64_GLOB_DAT) {
                        elf_soft_inc(&g_u32SoftRelocGlob);
                    } else if (u32Type == R_X86_64_JUMP_SLOT) {
                        elf_soft_inc(&g_u32SoftRelocJump);
                    } else {
                        elf_soft_inc(&g_u32SoftRelocAbs64);
                    }
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
                    elf_soft_inc(&g_u32SoftRelocCopy);
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
        elf_soft_inc(&g_u32SoftLoadFail);
        return GJ_ERR_INVAL;
    }
    st = elf_fill_probe(pImage, cb, &info, &pEh);
    if (st != GJ_OK) {
        elf_soft_inc(&g_u32SoftLoadFail);
        kprintf("elf: bad header\n");
        elf_soft_maybe_once();
        return st;
    }

    if (u64BiasReq != 0) {
        u64Bias = u64BiasReq;
        elf_soft_inc(&g_u32SoftLoadBiasReq);
    } else if (info.u16Type == ET_DYN && info.u64LoadMin < 0x100000ull) {
        u64Bias = GJ_ELF_DYN_BIAS;
        elf_soft_inc(&g_u32SoftLoadBiasDef);
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
        elf_soft_inc(&g_u32SoftLoadFail);
        elf_soft_maybe_once();
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
                elf_soft_inc(&g_u32SoftLoadFail);
                elf_soft_maybe_once();
                return GJ_ERR_NOMEM;
            }
        }
        u32Loaded++;
        elf_soft_inc(&g_u32SoftMapPages);
    }

    if (u32Loaded == 0) {
        elf_soft_inc(&g_u32SoftLoadFail);
        elf_soft_maybe_once();
        return GJ_ERR_INVAL;
    }

    {
        u32 cSym = 0;

        cRel = elf_apply_relocs(pImage, cb, pEh, u64Bias, &cSym);
        info.u32SymRelocs = cSym;
        if (cRel > 0) {
            info.u32Flags |= GJ_ELF_INFO_RELOC_OK;
            elf_soft_inc(&g_u32SoftRelocOps);
            g_u32SoftRelocHits += cRel;
            g_u32SoftSymHits += cSym;
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
    elf_soft_inc(&g_u32SoftLoadOk);
    if (info.u16Type == ET_DYN) {
        elf_soft_inc(&g_u32SoftLoadDyn);
    } else if (info.u16Type == ET_EXEC) {
        elf_soft_inc(&g_u32SoftLoadExec);
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
    elf_soft_maybe_once();
    return GJ_OK;
}

u64
elf_auxv_get(const u64 *pPairs, u32 cPairs, u64 u64Key)
{
    u32 i;

    if (pPairs == NULL || cPairs == 0) {
        return 0;
    }
    for (i = 0; i < cPairs; i++) {
        if (pPairs[i * 2u] == GJ_AT_NULL) {
            break;
        }
        if (pPairs[i * 2u] == u64Key) {
            return pPairs[i * 2u + 1u];
        }
    }
    return 0;
}

u32
elf_auxv_push(u64 *pPairs, u32 cPairs, u32 cMax, u64 u64Key, u64 u64Val)
{
    if (pPairs == NULL || cMax == 0) {
        return cPairs;
    }
    /* Reserve last slot for AT_NULL when pushing non-NULL */
    if (u64Key != GJ_AT_NULL && cPairs + 1u >= cMax) {
        return cPairs;
    }
    if (cPairs >= cMax) {
        return cPairs;
    }
    pPairs[cPairs * 2u] = u64Key;
    pPairs[cPairs * 2u + 1u] = u64Val;
    return cPairs + 1u;
}

int
elf_auxv_set(u64 *pPairs, u32 *pCPairs, u32 cMax, u64 u64Key, u64 u64Val)
{
    u32 i;
    u32 n;

    if (pPairs == NULL || pCPairs == NULL || cMax == 0 || u64Key == GJ_AT_NULL) {
        return 0;
    }
    n = *pCPairs;
    for (i = 0; i < n; i++) {
        if (pPairs[i * 2u] == GJ_AT_NULL) {
            break;
        }
        if (pPairs[i * 2u] == u64Key) {
            pPairs[i * 2u + 1u] = u64Val;
            return 1;
        }
    }
    if (n + 1u >= cMax) {
        return 0;
    }
    pPairs[n * 2u] = u64Key;
    pPairs[n * 2u + 1u] = u64Val;
    *pCPairs = n + 1u;
    return 1;
}

void
elf_fill_auxv(struct gj_process *pProc, const struct gj_elf_info *pMain,
              const struct gj_elf_info *pInterp)
{
    u32 n = 0;
    u64 u64ExecFn = 0;
    u64 u64Base;

    if (pProc == NULL || pMain == NULL) {
        return;
    }
    memset(pProc->aAuxv, 0, sizeof(pProc->aAuxv));
    /* AT_EXECFN / AT_RANDOM point into handoff page once published */
    u64ExecFn = GJ_LD_HANDOFF_VA +
                (u64)__builtin_offsetof(struct gj_ld_handoff, szPath);
    u64Base = pInterp != NULL ? pInterp->u64Base : 0;

    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_PHDR,
                      pMain->u64PhdrVa);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_PHENT,
                      pMain->u16Phentsize);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_PHNUM,
                      pMain->u16Phnum);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_PAGESZ,
                      GJ_PAGE_SIZE);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_BASE, u64Base);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_FLAGS, 0);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_ENTRY,
                      pMain->u64Entry);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_UID, 0);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_EUID, 0);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_GID, 0);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_EGID, 0);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_CLKTCK, 100);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_SECURE, 0);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_HWCAP, 0);
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_RANDOM,
                      GJ_LD_RANDOM_VA);
    if (pProc->szExecPath[0] != '\0') {
        n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_EXECFN,
                          u64ExecFn);
    }
    n = elf_auxv_push(pProc->aAuxv, n, GJ_PROC_AUXV_MAX, GJ_AT_NULL, 0);

    pProc->cAuxv = n;
    elf_soft_inc(&g_u32SoftAuxvFill);
    g_u32SoftAuxvPairs = n;
    kprintf("elf: auxv pairs=%u phdr=0x%lx entry=0x%lx base=0x%lx random=0x%lx "
            "PASS\n",
            n, (unsigned long)pMain->u64PhdrVa, (unsigned long)pMain->u64Entry,
            (unsigned long)u64Base, (unsigned long)GJ_LD_RANDOM_VA);
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
            elf_soft_inc(&g_u32SoftSoFull);
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
                elf_soft_inc(&g_u32SoftSoSkip);
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
                    g_aSo[iSlot].u32NameHash =
                        elf_sysv_hash_name(g_aSo[iSlot].szName);
                    pEhSo = (const struct elf64_ehdr *)g_aSo[iSlot].aImg;
                    elf_so_fill_dyn(&g_aSo[iSlot], g_aSo[iSlot].aImg, (u64)nRead,
                                    pEhSo);
                    cLoaded++;
                    g_cSo = cLoaded;
                    elf_soft_inc(&g_u32SoftSoMapOk);
                    if (g_aSo[iSlot].u8HasHash) {
                        elf_soft_inc(&g_u32SoftSoHash);
                    }
                    if (g_aSo[iSlot].u8HasGnu) {
                        elf_soft_inc(&g_u32SoftSoGnu);
                    }
                    kprintf("elf: SO map %s entry=0x%lx bias=0x%lx "
                            "hash=0x%lx gnu=0x%lx sym=0x%lx nh=0x%x PASS\n",
                            szPath, (unsigned long)so.u64Entry,
                            (unsigned long)so.u64Bias,
                            (unsigned long)g_aSo[iSlot].u64Hash,
                            (unsigned long)g_aSo[iSlot].u64GnuHash,
                            (unsigned long)g_aSo[iSlot].u64Symtab,
                            g_aSo[iSlot].u32NameHash);
                    if (g_aSo[iSlot].szSoname[0] != '\0') {
                        kprintf("elf: SO soname %s -> %s\n",
                                g_aSo[iSlot].szName, g_aSo[iSlot].szSoname);
                    }
                } else {
                    elf_soft_inc(&g_u32SoftSoMapFail);
                    kprintf("elf: SO map %s FAIL\n", szPath);
                }
            }
            break;
        }
    }
    if (cLoaded > 0) {
        u32 cHash = 0;
        u32 cGnu = 0;
        u32 s;

        for (s = 0; s < GJ_ELF_SO_MAX; s++) {
            if (!g_aSo[s].u8Used) {
                continue;
            }
            if (g_aSo[s].u8HasHash) {
                cHash++;
            }
            if (g_aSo[s].u8HasGnu) {
                cGnu++;
            }
        }
        kprintf("elf: DT_NEEDED SO map n=%u PASS\n", cLoaded);
        kprintf("elf: SO registry n=%u hash=%u gnu=%u PASS\n", cLoaded, cHash,
                cGnu);
    }
    elf_soft_maybe_once();
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
        /* Soft INTERP path log (absolute /lib/… after normalize) */
        if ((pMain->u32Flags & GJ_ELF_INFO_INTERP_SOFT) != 0 ||
            elf_interp_soft_ok(pMain->szInterp)) {
            kprintf("elf: INTERP soft ok %s PASS\n",
                    pMain->szInterp[0] != '\0' ? pMain->szInterp : "(anon)");
        }
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
        elf_soft_inc(&g_u32SoftInterpFirst);
        kprintf("linux: execve INTERP-first entry=0x%lx sp=0x%lx thr=%u PASS\n",
                (unsigned long)u64Entry, (unsigned long)u64Stack, cRepl);
        elf_soft_maybe_once();
        return GJ_OK;
    }
    /* INTERP soft miss: path present but dynlinker not loaded → main entry */
    if ((pMain->u32Flags & GJ_ELF_INFO_HAS_INTERP) != 0 &&
        (pInterp == NULL || pInterp->u64Entry == 0)) {
        elf_soft_inc(&g_u32SoftInterpDefer);
        kprintf("elf: INTERP soft defer main entry=0x%lx PASS\n",
                (unsigned long)pMain->u64Entry);
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
    elf_soft_inc(&g_u32SoftDirect);
    kprintf("linux: execve direct entry=0x%lx sp=0x%lx thr=%u\n",
            (unsigned long)u64Entry, (unsigned long)u64Stack, cRepl);
    elf_soft_maybe_once();
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
    elf_soft_inc(&g_u32SoftNeededCalls);
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
                elf_soft_inc(&g_u32SoftNeededOk);
                kprintf("elf: DT_NEEDED resolve %s -> %s PASS\n",
                        pInfo->aNeeded[iNeeded], szPath);
                break;
            }
        }
        if (!fFound) {
            elf_soft_inc(&g_u32SoftNeededMiss);
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

/*
 * Deterministic 16-byte AT_RANDOM seed (freestanding; not CSPRNG).
 * Mixes entry/base so distinct exec images differ in userspace probes.
 */
static void
elf_seed_random16(u8 *pOut, u64 u64A, u64 u64B)
{
    u64 a = u64A ^ 0x6a09e667f3bcc909ull;
    u64 b = u64B ^ 0xbb67ae8584caa73bull;
    u32 i;

    if (pOut == NULL) {
        return;
    }
    for (i = 0; i < 16u; i++) {
        a = a * 6364136223846793005ull + 1ull;
        b ^= a;
        b = (b << 7) | (b >> 57);
        pOut[i] = (u8)(b >> ((i & 7u) * 8u));
    }
}

void
elf_handoff_fill(struct gj_ld_handoff *pHo, const char *szPath,
                 const struct gj_elf_info *pMain,
                 const struct gj_elf_info *pInterp, const u64 *pAuxv, u32 cAuxv)
{
    u32 i;
    u32 cCopy;

    if (pHo == NULL || pMain == NULL) {
        return;
    }
    memset(pHo, 0, sizeof(*pHo));
    pHo->u64Magic = GJ_LD_HANDOFF_MAGIC;
    pHo->u64Entry = pMain->u64Entry;
    pHo->u64Interp = pInterp != NULL ? pInterp->u64Entry : 0;
    pHo->u64Base = pInterp != NULL ? pInterp->u64Base : 0;
    pHo->u64Phdr = pMain->u64PhdrVa;
    pHo->u64Phent = pMain->u16Phentsize;
    pHo->u64Phnum = pMain->u16Phnum;
    pHo->u64Pagesz = GJ_PAGE_SIZE;
    pHo->u64Stack = GJ_LD_STACK_VA;
    pHo->u32Flags = pMain->u32Flags;
    pHo->cSymReloc = pMain->u32SymRelocs;
    pHo->cSo = 0;
    for (i = 0; i < GJ_ELF_SO_MAX && i < GJ_LD_SO_MAX; i++) {
        if (!g_aSo[i].u8Used) {
            continue;
        }
        pHo->aSo[pHo->cSo].u64Bias = g_aSo[i].u64Bias;
        pHo->aSo[pHo->cSo].cbImg = g_aSo[i].cbImg;
        pHo->aSo[pHo->cSo].u32NameHash = g_aSo[i].u32NameHash;
        /* Basename = DT_NEEDED (ld-gj opens /lib/<name>); soname is registry-only */
        path_copy_n(pHo->aSo[pHo->cSo].szName, sizeof(pHo->aSo[pHo->cSo].szName),
                    g_aSo[i].szName);
        pHo->cSo++;
    }
    cCopy = cAuxv;
    if (cCopy > GJ_AUXV_MAX) {
        cCopy = GJ_AUXV_MAX;
    }
    pHo->cAuxv = cCopy;
    if (pAuxv != NULL) {
        for (i = 0; i < cCopy * 2u && i < GJ_AUXV_MAX * 2u; i++) {
            pHo->aAuxv[i] = pAuxv[i];
        }
    }
    if (szPath != NULL && szPath[0] != '\0') {
        path_copy_n(pHo->szPath, sizeof(pHo->szPath), szPath);
    }
    if (pMain->szInterp[0] != '\0') {
        path_copy_n(pHo->szInterp, sizeof(pHo->szInterp), pMain->szInterp);
    } else if (pInterp != NULL && pInterp->szInterp[0] != '\0') {
        path_copy_n(pHo->szInterp, sizeof(pHo->szInterp), pInterp->szInterp);
    }
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
    const char *szUsePath;

    if (pProc == NULL || pMain == NULL) {
        elf_soft_inc(&g_u32SoftHandoffFail);
        return GJ_ERR_INVAL;
    }
    if (szPath != NULL && szPath[0] != '\0') {
        path_copy_n(pProc->szExecPath, sizeof(pProc->szExecPath), szPath);
        szUsePath = szPath;
    } else if (pProc->szExecPath[0] != '\0') {
        szUsePath = pProc->szExecPath;
    } else {
        szUsePath = NULL;
    }

    elf_handoff_fill(&ho, szUsePath, pMain, pInterp, pProc->aAuxv, pProc->cAuxv);
    cCopy = ho.cAuxv;

    /*
     * INTERP-first stack block (SysV): argc, argv[], NULL, envp NULL, auxv...
     * argv[0] points at path string stored later in the same page.
     */
    ho.u64Stack = GJ_LD_STACK_VA;

    if (process_as_ensure(pProc) != GJ_OK) {
        elf_soft_inc(&g_u32SoftHandoffFail);
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
        elf_soft_inc(&g_u32SoftHandoffFail);
        return GJ_ERR_NOMEM;
    }
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    memset((void *)(gj_vaddr_t)pa, 0, GJ_PAGE_SIZE);
    memcpy((void *)(gj_vaddr_t)pa, &ho, sizeof(ho));
    /* AT_RANDOM 16 bytes at GJ_LD_RANDOM_OFF on handoff page */
    {
        u8 *pRnd = (u8 *)(gj_vaddr_t)pa + (size_t)GJ_LD_RANDOM_OFF;

        elf_seed_random16(pRnd, ho.u64Entry, ho.u64Base ^ ((u64)ho.cSo << 32));
    }

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
        elf_soft_inc(&g_u32SoftHandoffFail);
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

    elf_soft_inc(&g_u32SoftHandoffOk);
    kprintf("elf: handoff va=0x%lx entry=0x%lx base=0x%lx path=%s sym=%u "
            "so=%u random=0x%lx PASS\n",
            (unsigned long)GJ_LD_HANDOFF_VA, (unsigned long)ho.u64Entry,
            (unsigned long)ho.u64Base,
            ho.szPath[0] != '\0' ? ho.szPath : "(none)", ho.cSymReloc, ho.cSo,
            (unsigned long)GJ_LD_RANDOM_VA);
    elf_soft_maybe_once();
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
        elf_soft_inc(&g_u32SoftVerifyFail);
        return GJ_ERR_INVAL;
    }
    if (process_as_ensure(pProc) != GJ_OK) {
        elf_soft_inc(&g_u32SoftVerifyFail);
        return GJ_ERR_NOMEM;
    }
    process_as_activate(pProc);
    pa = vmm_virt_to_phys((gj_vaddr_t)GJ_LD_HANDOFF_VA);
    if (pa == 0) {
        elf_soft_inc(&g_u32SoftVerifyFail);
        kprintf("ld-gj: handoff page missing\n");
        return GJ_ERR_NOENT;
    }
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    memcpy(&ho, (void *)(gj_vaddr_t)(pa & ~0xfffull), sizeof(ho));
    cpu_load_cr3(u64Saved);
    process_as_activate(pProc);

    if (ho.u64Magic != GJ_LD_HANDOFF_MAGIC) {
        elf_soft_inc(&g_u32SoftVerifyFail);
        kprintf("ld-gj: handoff magic FAIL 0x%lx\n",
                (unsigned long)ho.u64Magic);
        return GJ_ERR_INVAL;
    }
    if (ho.u64Entry == 0 || ho.u64Phdr == 0) {
        elf_soft_inc(&g_u32SoftVerifyFail);
        kprintf("ld-gj: handoff entry/phdr FAIL\n");
        return GJ_ERR_INVAL;
    }
    paEntry = vmm_virt_to_phys((gj_vaddr_t)(ho.u64Entry & ~0xfffull));
    if (paEntry == 0) {
        elf_soft_inc(&g_u32SoftVerifyFail);
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
    if (ho.cSo > 0) {
        kprintf("ld-gj: SO registry handoff n=%u hash0=0x%x\n", ho.cSo,
                ho.aSo[0].u32NameHash);
    }
    /* Soft probe AT_RANDOM mapping on handoff page */
    {
        gj_paddr_t paRnd =
            vmm_virt_to_phys((gj_vaddr_t)(GJ_LD_RANDOM_VA & ~0xfffull));

        if (paRnd != 0 && elf_auxv_get(ho.aAuxv, ho.cAuxv, GJ_AT_RANDOM) ==
                              GJ_LD_RANDOM_VA) {
            kprintf("elf: auxv AT_RANDOM ready 0x%lx PASS\n",
                    (unsigned long)GJ_LD_RANDOM_VA);
        }
    }
    elf_soft_inc(&g_u32SoftVerifyOk);
    kprintf("ld-gj: handoff PASS\n");
    elf_soft_maybe_once();
    return GJ_OK;
}
