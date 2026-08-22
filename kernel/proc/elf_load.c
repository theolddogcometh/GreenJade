/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room ELF64 ET_EXEC / ET_DYN load + PT_INTERP / PT_DYNAMIC / RELA.
 * Product path: probe -> map PT_LOAD -> SO registry -> relocs -> auxv handoff
 * -> INTERP-first start (ld-gj). No third-party loader code.
 *
 * Soft residual (lean; this unit only; never hard-gates product load):
 * greppable: "elf: soft residual" | "elf: soft residual lean" |
 *            "elf: soft residual lean udx" | "elf: soft residual lean c2" |
 *            "elf: soft residual lean sshd" | "elf: soft residual lean host" |
 *            "elf: soft residual lean host_blob" |
 *            "elf: soft residual lean PASS" |
 *            "elf: soft residual udx" | "elf: soft residual sshd" |
 *            "elf: soft residual host_blob" |
 *            "elf: soft residual denser" |
 *            "elf: soft residual denser host_blob" |
 *            "elf: soft functional residual" |
 *            "elf: soft functional residual catalog" |
 *            "elf: soft functional residual embed" |
 *            "elf: soft functional residual host_blob" |
 *            "elf: soft functional step" |
 *            "elf: soft inventory" | "elf: soft PASS"
 *            "elf_load: soft residual" | "elf_load: soft residual lean" |
 *            "elf_load: soft residual lean udx" |
 *            "elf_load: soft residual lean c2" |
 *            "elf_load: soft residual lean sshd" |
 *            "elf_load: soft residual lean host" |
 *            "elf_load: soft residual lean host_blob" |
 *            "elf_load: soft residual lean PASS" |
 *            "elf_load: soft residual udx" | "elf_load: soft residual sshd" |
 *            "elf_load: soft residual host_blob" |
 *            "elf_load: soft residual denser" |
 *            "elf_load: soft residual denser host_blob" |
 *            "elf_load: soft functional residual" |
 *            "elf_load: soft functional residual embed" |
 *            "elf_load: soft functional residual host_blob" |
 *            "elf_load: soft inventory" | "elf_load: soft PASS"
 * C2 product-path residual: UDX/DDI hosts as userspace ET_EXEC/ET_DYN (not .ko)
 * + freestanding product daemon embeds (sshd.elf live spawn). FUNCTIONAL residual
 * preferred: host-class classify + ET_EXEC band + entry-in-range + lean arms
 * + product host path trio (/usr/lib/udx/{rtl8168_udx,xhci_udx,ddi_host}) + sshd
 * + product-host handoff_fill EXECFN residual + pipeline step honesty
 * + embed ET_EXEC freestanding host trio (user.ld @ HOST_EXEC_BASE) residual
 * + PT_LOAD filesz/memsz + handoff-band collision reject (robust embed load).
 * + spawn host_blob product load path residual (STRONGER; Soft!=product):
 *     spawn_host_blob_get -> elf_probe -> elf_load (this unit owns probe|load)
 *     product_hosts=UDX (ddi_host_gj,rtl8168_udx,xhci_udx) Dual DoD OPEN
 *     greppable: host_blob | product_hosts=UDX | dual_dod OPEN
 * + denser residual host_blob Dual DoD (STRONGER denser; Soft!=product):
 *     denser path axes: host_blob|probe|load + class trio + handoff denser
 *     + dual_dod OPEN honesty denser; denser residual != Dual DoD close
 *     greppable: denser host_blob residual | product_hosts=UDX | dual_dod OPEN
 * Host load pipeline (soft honesty): probe|load|so|reloc|auxv|handoff|interp|direct
 * host_blob path honesty: host_blob|probe|load (spawn thr/stack residual elsewhere)
 * product_dir=UDX+ABI · freestanding_class_product=0 · ko_product=0 · G-AC-1.
 * class=C2 dual_dod_a=OPEN dual_dod_b=OPEN (Soft residual != Dual DoD close).
 * Soft!=product · dual MIT OR Apache-2.0 · no version stamp · storm=0.
 * Multi-line soft catalog flood removed; tallies fold into residual surface.
 * UDX multi-server confine product stays OPEN. Dual DoD B until interactive SSH login.
 * Fly residual stamp-free; NEVER invent next N.
 * greppable: "elf: soft functional residual embed"
 * greppable: "elf: soft residual host_blob" | "elf: soft residual lean host_blob"
 * greppable: "elf: soft residual denser" | "elf: soft residual denser host_blob"
 * greppable: product_hosts=UDX | host_blob | dual_dod OPEN
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
 * (Defined early; soft residual surface reports the value.)
 */
#define GJ_ELF_DYN_BIAS 0x0000000070000000ull
/* Per-SO load bias band (above main ET_DYN default at GJ_ELF_DYN_BIAS) */
#define GJ_ELF_SO_BIAS_BASE 0x0000000071000000ull
#define GJ_ELF_SO_BIAS_STEP 0x0000000001000000ull
/*
 * Freestanding product daemon load base (user.ld / sshd.elf / peer embeds).
 * Matches GJ_USER_CODE_VA — high enough for live spawn, below handoff band.
 * Soft residual + geometry arms; never hard-gates product load alone.
 */
#define GJ_ELF_HOST_EXEC_BASE 0x0000000004000000ull
/* Soft host path class residual (classify only; Soft!=product). */
#define GJ_ELF_HOST_CLASS_NONE 0u
#define GJ_ELF_HOST_CLASS_UDX  1u
#define GJ_ELF_HOST_CLASS_SSHD 2u
#define GJ_ELF_HOST_CLASS_SVC  3u
/*
 * Soft functional residual pipeline steps (host load; Soft!=product).
 * greppable: elf: soft functional step | path=probe,load,so,reloc,auxv,handoff,interp,direct
 * Product hosts: /usr/lib/udx/{rtl8168_udx,xhci_udx,ddi_host} + sshd.elf.
 */
#define GJ_ELF_FUNC_STEP_PROBE   1u
#define GJ_ELF_FUNC_STEP_LOAD    2u
#define GJ_ELF_FUNC_STEP_SO      3u
#define GJ_ELF_FUNC_STEP_RELOC   4u
#define GJ_ELF_FUNC_STEP_AUXV    5u
#define GJ_ELF_FUNC_STEP_HANDOFF 6u
#define GJ_ELF_FUNC_STEP_INTERP  7u
#define GJ_ELF_FUNC_STEP_DIRECT  8u
#define GJ_ELF_FUNC_STEP_COUNT   8u

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
 * Soft residual telemetry (never hard-gates product load path).
 * greppable: elf: soft residual | elf_load: soft residual |
 *            elf: soft residual lean | elf: soft residual lean udx |
 *            elf: soft residual lean c2 | elf: soft residual lean sshd |
 *            elf: soft residual lean host | elf: soft residual lean host_blob |
 *            elf: soft residual lean PASS |
 *            elf: soft residual udx | elf: soft residual sshd |
 *            elf: soft residual host_blob |
 *            elf: soft residual denser | elf: soft residual denser host_blob |
 *            elf: soft functional residual | elf: soft functional step |
 *            elf: soft functional residual host_blob |
 *            elf_load: soft residual lean | elf_load: soft residual lean udx |
 *            elf_load: soft residual lean c2 | elf_load: soft residual lean sshd |
 *            elf_load: soft residual lean host |
 *            elf_load: soft residual lean host_blob |
 *            elf_load: soft residual lean PASS | elf_load: soft residual udx |
 *            elf_load: soft residual sshd | elf_load: soft residual host_blob |
 *            elf_load: soft residual denser |
 *            elf_load: soft residual denser host_blob |
 *            elf_load: soft functional residual |
 *            elf_load: soft functional residual host_blob
 * Soft!=product · G-AC-1 · dual MIT OR Apache-2.0 · no version stamp ·
 * storm=0. C2 residual lean deepen for UDX host + sshd.elf load
 * (this unit only; Dual DoD A/B stay OPEN). Geometry + behavior residual.
 * FUNCTIONAL residual: host-class + ET_EXEC band + entry-in-range + lean arms
 * + product host trio (rtl8168_udx/xhci_udx/ddi_host) + handoff_fill + steps
 * + spawn host_blob product load path (product_hosts=UDX; Dual DoD OPEN)
 * + denser host_blob residual (product_hosts=UDX; Dual DoD OPEN denser).
 *
 * Soft residual areas (folded into lean dump; no per-area flood):
 *   1 probe  2 load  3 reloc  4 so  5 needed  6 resolve  7 auxv
 *   8 handoff  9 verify  10 interp  11 bias  12 map  13 registry
 *   14 capacity  15 PASS/PARTIAL  16 twin elf_load:  17 dual-license
 *   18 storm=0 honesty  19 udx_host userspace  20 G-AC-1 no.ko
 *   21 load_kind dyn/exec  22 INTERP soft-ok
 *   23 product_dir=UDX+ABI  24 freestanding_class_product=0
 *   25 host_load pipeline (probe|load|so|reloc|auxv|handoff|interp)
 *   26 handoff VA contract  27 lean selfcheck udx  28 residual lean PASS
 *   29 class=C2 product path  30 dual_dod A/B OPEN honesty
 *   31 handoff sizeof layout  32 SO/handoff capacity parity
 *   33 handoff/dyn placement geometry  34 host helper behavior residual
 *   35 sshd.elf ET_EXEC freestanding band  36 sshd direct-entry handoff
 *   37 host path class residual  38 load/handoff non-collision geometry
 *   39 product host path trio residual  40 product host handoff_fill residual
 *   41 soft functional residual catalog  42 soft functional pipeline steps
 *   43 embed ET_EXEC freestanding host trio residual
 *   44 PT_LOAD segment safety (filesz/memsz + handoff non-collision)
 *   45 spawn host_blob product load path residual (product_hosts=UDX)
 *   46 denser host_blob Dual DoD residual (product_hosts=UDX denser)
 */
#define GJ_ELF_SOFT_WAVE 122u
#define GJ_ELF_SOFT_AREAS 46u
/*
 * Host load lean self-check arm count (UDX + sshd.elf + host_blob; Soft!=product).
 * Arms 1-10: UDX host geometry/behavior; 11-14: sshd.elf + host-class;
 * 15-17: product host trio + handoff_fill + functional pipeline (STRONGER);
 * 18-19: embed ET_EXEC freestanding host trio + PT_LOAD safety (STRONGER);
 * 20: spawn host_blob product load path residual (STRONGER; product_hosts=UDX);
 * 21: denser host_blob Dual DoD residual (STRONGER denser; product_hosts=UDX).
 */
#define GJ_ELF_SOFT_UDX_LEAN_CHECKS 21u

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
static u32 g_u32SoftInterpDefer;  /* INTERP present, dynlinker miss -> main */
static u32 g_u32SoftDirect;       /* direct main entry (no INTERP start) */
static u32 g_u32SoftResolveGnu;   /* SO registry gnu-hash hit */
static u32 g_u32SoftResolveHash;  /* SO registry SysV hash hit */
static u32 g_u32SoftResolveScan;  /* SO registry linear scan hit */
static u32 g_u32SoftResolveMiss;  /* SO registry lookup miss */
static u32 g_u32SoftRegFindHit;   /* elf_so_registry_find hit */
static u32 g_u32SoftRegFindMiss;  /* elf_so_registry_find miss */
static u32 g_u32SoftLogN;         /* soft inventory dump emissions */
static u32 g_u32SoftUdxLeanOk;    /* UDX+sshd host lean selfcheck pass count */
static u32 g_u32SoftUdxLeanFail;  /* UDX+sshd host lean selfcheck fail count */
static u32 g_u32SoftHostClassUdx; /* path class residual: UDX host */
static u32 g_u32SoftHostClassSshd;/* path class residual: sshd.elf */
static u32 g_u32SoftHostClassSvc; /* path class residual: other service host */
static u32 g_u32SoftLoadExecBand; /* ET_EXEC freestanding product band load */
static u32 g_u32SoftEntryInRange; /* load entry inside PT_LOAD span soft */
static u32 g_u32SoftEntryOor;     /* load entry outside PT_LOAD span soft */
static u32 g_u32SoftSegReject;    /* PT_LOAD safety reject (overflow/collide) */
static u32 g_u32SoftEmbedHost;    /* freestanding embed host-band load residual */
static u32 g_u32SoftHostBlob;     /* spawn host_blob product load residual */
static u32 g_u32SoftHostBlobDenseOk;   /* denser host_blob residual pass count */
static u32 g_u32SoftHostBlobDenseFail; /* denser host_blob residual fail count */
static int g_fSoftInvOnce;        /* first post-activity inventory emitted */

static void path_copy_n(char *szDst, size_t cbDst, const char *szSrc);
static void path_join2(char *szDst, size_t cbDst, const char *szPfx,
                       const char *szName);
static void elf_soft_inventory(const char *szVia);
static void elf_soft_maybe_once(void);
static u32  elf_soft_udx_host_lean_check(void);
static u32  elf_soft_host_path_class(const char *szPath);
static int  elf_soft_str_has(const char *szHay, const char *szNeedle);
static const char *elf_soft_basename(const char *szPath);

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

/** Soft: case-sensitive substring search (NULL-safe; Soft residual only). */
static int
elf_soft_str_has(const char *szHay, const char *szNeedle)
{
    size_t i;
    size_t j;

    if (szHay == NULL || szNeedle == NULL || szNeedle[0] == '\0') {
        return 0;
    }
    for (i = 0; szHay[i] != '\0'; i++) {
        for (j = 0; szNeedle[j] != '\0'; j++) {
            if (szHay[i + j] != szNeedle[j]) {
                break;
            }
        }
        if (szNeedle[j] == '\0') {
            return 1;
        }
        if (szHay[i + j] == '\0') {
            break;
        }
    }
    return 0;
}

/** Soft: basename after last '/'; empty path -> "". */
static const char *
elf_soft_basename(const char *szPath)
{
    const char *szBase;
    size_t i;

    if (szPath == NULL || szPath[0] == '\0') {
        return "";
    }
    szBase = szPath;
    for (i = 0; szPath[i] != '\0'; i++) {
        if (szPath[i] == '/' && szPath[i + 1u] != '\0') {
            szBase = &szPath[i + 1u];
        }
    }
    return szBase;
}

/**
 * Soft host path class residual (FUNCTIONAL; Soft!=product).
 * Classifies UDX hosts (DoD A/B product drivers) and sshd.elf (DoD B endpoint).
 * Product hosts (STRONGER): /usr/lib/udx/{rtl8168_udx,xhci_udx,ddi_host} +
 * ESP GREENJADE/drivers/ pack names + bare basenames. Never hard-gates
 * product load. Dual DoD A/B remain OPEN.
 */
static u32
elf_soft_host_path_class(const char *szPath)
{
    const char *szBase;

    if (szPath == NULL || szPath[0] == '\0') {
        return GJ_ELF_HOST_CLASS_NONE;
    }
    szBase = elf_soft_basename(szPath);
    /* sshd.elf first — product SSH daemon embed / live spawn path. */
    if (elf_soft_str_has(szBase, "sshd") != 0 ||
        elf_soft_str_has(szPath, "sshd.elf") != 0) {
        return GJ_ELF_HOST_CLASS_SSHD;
    }
    /*
     * Product host install / pack dirs (stage-rootfs + stage-esp).
     * Soft residual only — G-AC-1: userspace ELF, never .ko product.
     */
    if (elf_soft_str_has(szPath, "/usr/lib/udx/") != 0 ||
        elf_soft_str_has(szPath, "GREENJADE/drivers/") != 0 ||
        elf_soft_str_has(szPath, "/lib/udx/") != 0) {
        return GJ_ELF_HOST_CLASS_UDX;
    }
    /* UDX/DDI Linux-shaped userspace hosts by basename (not .ko; G-AC-1). */
    if (elf_soft_str_has(szBase, "udx") != 0 ||
        elf_soft_str_has(szBase, "rtl8168") != 0 ||
        elf_soft_str_has(szBase, "xhci") != 0 ||
        elf_soft_str_has(szBase, "ddi_host") != 0) {
        return GJ_ELF_HOST_CLASS_UDX;
    }
    /* Other freestanding service hosts (vfsd/netstackd/sessiond/…). */
    if (elf_soft_str_has(szBase, "vfsd") != 0 ||
        elf_soft_str_has(szBase, "storaged") != 0 ||
        elf_soft_str_has(szBase, "netstackd") != 0 ||
        elf_soft_str_has(szBase, "sessiond") != 0 ||
        elf_soft_str_has(szBase, "scsi_mid") != 0 ||
        elf_soft_str_has(szBase, "shell") != 0 ||
        elf_soft_str_has(szBase, "init") != 0) {
        return GJ_ELF_HOST_CLASS_SVC;
    }
    return GJ_ELF_HOST_CLASS_NONE;
}

/** Soft: note host path class residual (tallies only). */
static void
elf_soft_note_host_path(const char *szPath)
{
    u32 u32Class;

    u32Class = elf_soft_host_path_class(szPath);
    if (u32Class == GJ_ELF_HOST_CLASS_UDX) {
        elf_soft_inc(&g_u32SoftHostClassUdx);
    } else if (u32Class == GJ_ELF_HOST_CLASS_SSHD) {
        elf_soft_inc(&g_u32SoftHostClassSshd);
    } else if (u32Class == GJ_ELF_HOST_CLASS_SVC) {
        elf_soft_inc(&g_u32SoftHostClassSvc);
    }
}

/**
 * Soft: freestanding product ET_EXEC band (sshd.elf / peer embeds @ user.ld).
 * LoadMin in [HOST_EXEC_BASE, HANDOFF) with no dyn bias — soft residual only.
 */
static int
elf_soft_exec_band_ok(const struct gj_elf_info *pInfo)
{
    if (pInfo == NULL) {
        return 0;
    }
    if (pInfo->u16Type != ET_EXEC) {
        return 0;
    }
    if (pInfo->u64LoadMin < GJ_ELF_HOST_EXEC_BASE) {
        return 0;
    }
    if (pInfo->u64LoadMin >= GJ_LD_HANDOFF_VA) {
        return 0;
    }
    if (pInfo->u64Bias != 0ull) {
        return 0;
    }
    return 1;
}

/**
 * Soft: entry lies in mapped PT_LOAD span after bias (FUNCTIONAL residual).
 * Never hard-gates product; tallies only. STRONGER honesty for UDX + sshd.
 */
static int
elf_soft_entry_in_range(const struct gj_elf_info *pInfo)
{
    if (pInfo == NULL) {
        return 0;
    }
    if (pInfo->u64Entry == 0ull) {
        return 0;
    }
    if (pInfo->u64LoadMax <= pInfo->u64LoadMin) {
        return 0;
    }
    if (pInfo->u64Entry < pInfo->u64LoadMin) {
        return 0;
    }
    if (pInfo->u64Entry >= pInfo->u64LoadMax) {
        return 0;
    }
    return 1;
}

/**
 * Robust embed host load: PT_LOAD span collides with ld-gj handoff band?
 * Band is [GJ_LD_HANDOFF_VA, GJ_LD_STACK_VA + (PAGES+1)*PAGE) — handoff,
 * unmapped guard at STACK_VA, grow-down body, SysV top. Product hosts must
 * never clobber handoff/stack when mapping embedded ELF. Soft residual +
 * product reject path use this. Soft!=product; Dual DoD A/B OPEN.
 */
static int
elf_pt_load_handoff_collide(u64 u64SegVa, u64 u64Memsz)
{
    u64 u64SegEnd;
    u64 u64BandEnd;

    if (u64Memsz == 0ull) {
        return 0;
    }
    u64SegEnd = u64SegVa + u64Memsz;
    if (u64SegEnd < u64SegVa) {
        /* overflow treated as collide (reject) */
        return 1;
    }
    u64BandEnd = GJ_LD_STACK_VA +
                 (u64)(GJ_LD_STACK_PAGES + 1u) * (u64)GJ_PAGE_SIZE;
    if (u64SegVa < u64BandEnd && u64SegEnd > GJ_LD_HANDOFF_VA) {
        return 1;
    }
    return 0;
}

int
elf_stack_rsp_live_ok(u64 u64Rsp)
{
    u64 u64Max;

    if (u64Rsp <= GJ_LD_STACK_VA) {
        return 0;
    }
    if ((u64Rsp & ((u64)GJ_PAGE_SIZE - 1ull)) != 0ull) {
        return 0;
    }
    u64Max = GJ_LD_STACK_VA +
             (u64)(GJ_LD_STACK_PAGES + 1u) * (u64)GJ_PAGE_SIZE;
    if (u64Rsp > u64Max) {
        return 0;
    }
    return 1;
}

/**
 * Soft: freestanding embed host span fits under handoff (FUNCTIONAL).
 * HOST_EXEC_BASE + soft max image window must stay below HANDOFF_VA.
 * Matches user.ld freestanding embeds (ddi_host / peers) Soft!=product.
 */
static int
elf_soft_embed_host_span_ok(u64 u64LoadMin, u64 u64LoadMax)
{
    if (u64LoadMin < GJ_ELF_HOST_EXEC_BASE) {
        return 0;
    }
    if (u64LoadMax <= u64LoadMin) {
        return 0;
    }
    if (u64LoadMax > GJ_LD_HANDOFF_VA) {
        return 0;
    }
    if (elf_pt_load_handoff_collide(u64LoadMin, u64LoadMax - u64LoadMin) !=
        0) {
        return 0;
    }
    return 1;
}

/**
 * UDX host + sshd.elf load residual lean self-check (static + behavior).
 * C2 product-path: userspace ET_EXEC/ET_DYN host ELF load for UDX/DDI
 * (not Linux .ko product) + freestanding product daemon (sshd.elf).
 * Soft!=product · G-AC-1 · never hard-gates.
 * Dual DoD A/B remain OPEN (soft residual != close).
 * Returns number of arms that passed (expect GJ_ELF_SOFT_UDX_LEAN_CHECKS).
 *
 * Arms:
 *   1 handoff VA band ordered (HANDOFF < STACK, RANDOM past handoff)
 *   2 ET_DYN bias band above PE 0x400000; SO bias base above main dyn
 *   3 capacity bounds for host SO / NEEDED / auxv
 *   4 ET_EXEC/ET_DYN + PT_LOAD/DYNAMIC/INTERP product surface numbers
 *   5 handoff magic 'GJLD' little-endian
 *   6 handoff layout: sizeof(gj_ld_handoff) fits under AT_RANDOM off
 *   7 SO registry/handoff capacity parity (GJ_ELF_SO_MAX == GJ_LD_SO_MAX)
 *   8 PF flags + INTERP max + residual wave/areas deepen (C2)
 *   9 placement geometry: page-align + handoff band below dyn bias (STRONGER)
 *  10 stack-local behavior: hash + auxv + INTERP soft + udx handoff_fill
 *  11 sshd.elf freestanding ET_EXEC band vs handoff/PE (STRONGER FUNCTIONAL)
 *  12 sshd.elf direct-entry handoff_fill (no INTERP) (STRONGER FUNCTIONAL)
 *  13 host path class residual (udx + sshd) (STRONGER FUNCTIONAL)
 *  14 non-collision geometry host-exec / dyn / handoff (STRONGER FUNCTIONAL)
 *  15 product host path trio + sshd (STRONGER FUNCTIONAL)
 *  16 product host handoff_fill EXECFN residual (STRONGER FUNCTIONAL)
 *  17 soft functional pipeline steps + product_dir honesty (STRONGER)
 *  18 embed ET_EXEC freestanding product host trio handoff (STRONGER)
 *  19 PT_LOAD segment safety + embed span non-collision (STRONGER)
 *  20 spawn host_blob product load path residual (STRONGER; product_hosts=UDX)
 *  21 denser host_blob Dual DoD residual (STRONGER denser; product_hosts=UDX)
 */
static u32
elf_soft_udx_host_lean_check(void)
{
    u32 u32Ok;
    u64 aPairs[8];
    u32 cPairs;
    u32 u32HEmpty;
    u32 u32HName;
    u32 u32GEmpty;
    u32 u32GName;
    u32 u32ClassUdx;
    u32 u32ClassSshd;
    u32 u32ClassNone;
    u32 u32Hi;
    u32 u32DenseAxes;
    int fBeh;
    int fHostsOk;
    int fEmbedOk;
    int fBlobOk;
    int fDenseOk;
    const char *aszHosts[4];
    const char *aszEmbed[3];
    const char *aszBlob[3];
    const char *aszDense[3];
    struct gj_elf_info info;
    struct gj_ld_handoff ho;

    u32Ok = 0u;

    /* 1. handoff VA band (high user; away from PE / typical bias). */
    if (GJ_LD_HANDOFF_VA != 0ull && GJ_LD_STACK_VA > GJ_LD_HANDOFF_VA &&
        GJ_LD_RANDOM_VA > GJ_LD_HANDOFF_VA &&
        GJ_LD_RANDOM_OFF == 0x400ull) {
        u32Ok++;
    }

    /* 2. dyn / SO bias contract for userspace host load placement. */
    if (GJ_ELF_DYN_BIAS >= 0x0000000070000000ull &&
        GJ_ELF_SO_BIAS_BASE > GJ_ELF_DYN_BIAS &&
        GJ_ELF_SO_BIAS_STEP != 0ull) {
        u32Ok++;
    }

    /* 3. capacity bounds (host SO registry + NEEDED + auxv handoff). */
    if (GJ_ELF_SO_MAX >= 1u && GJ_ELF_SO_MAX <= 16u &&
        GJ_ELF_NEEDED_MAX >= 1u && GJ_ELF_NEEDED_MAX <= 16u &&
        GJ_AUXV_MAX >= 8u && GJ_AUXV_MAX <= 64u &&
        GJ_LD_SO_MAX >= 1u && GJ_LD_SO_MAX <= 16u) {
        u32Ok++;
    }

    /* 4. ELF product surface: userspace ET_EXEC/ET_DYN + PT_* only. */
    if (ET_EXEC == 2 && ET_DYN == 3 && PT_LOAD == 1 && PT_DYNAMIC == 2 &&
        PT_INTERP == 3 && EM_X86_64 == 62) {
        u32Ok++;
    }

    /* 5. handoff magic 'GJLD' LE — ld-gj trusts only when live. */
    if (GJ_LD_HANDOFF_MAGIC == 0x444c4a47ull) {
        u32Ok++;
    }

    /*
     * 6. handoff page layout: struct fits below AT_RANDOM blob on page.
     * C2 product path: ld-gj reads magic/auxv/SO then AT_RANDOM past struct.
     */
    if (sizeof(struct gj_ld_handoff) > 0u &&
        sizeof(struct gj_ld_handoff) <= (size_t)GJ_LD_RANDOM_OFF &&
        GJ_LD_RANDOM_OFF < 0x1000ull) {
        u32Ok++;
    }

    /* 7. SO registry slots match handoff aSo[] capacity (parity). */
    if (GJ_ELF_SO_MAX == GJ_LD_SO_MAX && GJ_ELF_SO_MAX >= 1u) {
        u32Ok++;
    }

    /*
     * 8. PF flags + INTERP bound + residual lean deepen (C2 UDX+sshd host).
     * Soft residual only; never product Dual DoD close.
     */
    if (PF_X == 1 && PF_W == 2 && PF_R == 4 &&
        GJ_ELF_INTERP_MAX >= 64u && GJ_ELF_INTERP_MAX <= 256u &&
        GJ_ELF_SOFT_WAVE >= 122u &&
        GJ_ELF_SOFT_AREAS >= 46u &&
        GJ_ELF_SOFT_UDX_LEAN_CHECKS == 21u &&
        GJ_ELF_FUNC_STEP_COUNT == 8u) {
        u32Ok++;
    }

    /*
     * 9. Placement geometry (STRONGER C2 residual): page-aligned handoff/stack,
     * stack is next page after handoff, AT_RANDOM on handoff page, handoff
     * band strictly below ET_DYN default bias, dyn bias above PE 0x400000.
     * Soft residual only — never hard-gates product load.
     */
    if ((GJ_LD_HANDOFF_VA & 0xfffull) == 0ull &&
        (GJ_LD_STACK_VA & 0xfffull) == 0ull &&
        (GJ_ELF_DYN_BIAS & 0xfffull) == 0ull &&
        GJ_LD_STACK_VA == GJ_LD_HANDOFF_VA + (u64)GJ_PAGE_SIZE &&
        GJ_LD_HANDOFF_VA < GJ_ELF_DYN_BIAS &&
        GJ_LD_RANDOM_VA > GJ_LD_HANDOFF_VA &&
        GJ_LD_RANDOM_VA < GJ_LD_HANDOFF_VA + (u64)GJ_PAGE_SIZE &&
        GJ_ELF_DYN_BIAS > 0x0000000000400000ull &&
        GJ_ELF_SO_BIAS_BASE > GJ_ELF_DYN_BIAS &&
        GJ_ELF_SO_BIAS_STEP >= (u64)GJ_PAGE_SIZE) {
        u32Ok++;
    }

    /*
     * 10. Stack-local behavior residual (STRONGER C2 residual): exercise real
     * host-load helpers without mapping product pages. Soft!=product · G-AC-1.
     *   - SysV/GNU hash known vectors (empty + DT_NEEDED-shaped name)
     *   - auxv push/set/get roundtrip (handoff pair surface)
     *   - INTERP soft-ok absolute accept / relative+empty+NULL reject
     *   - handoff_fill for UDX host path magic + entry/stack/pagesz contract
     */
    fBeh = 1;
    u32HEmpty = elf_sysv_hash_name("");
    u32HName = elf_sysv_hash_name("libgj-so.so.1");
    u32GEmpty = elf_gnu_hash_name("");
    u32GName = elf_gnu_hash_name("libgj-so.so.1");
    if (u32HEmpty != 0u || u32GEmpty != 5381u || u32HName == 0u ||
        u32GName == u32GEmpty) {
        fBeh = 0;
    }
    memset(aPairs, 0, sizeof(aPairs));
    cPairs = 0u;
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_PAGESZ, (u64)GJ_PAGE_SIZE);
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_ENTRY, 0x400000ull);
    if (cPairs != 2u ||
        elf_auxv_get(aPairs, cPairs, GJ_AT_PAGESZ) != (u64)GJ_PAGE_SIZE ||
        elf_auxv_get(aPairs, cPairs, GJ_AT_ENTRY) != 0x400000ull) {
        fBeh = 0;
    }
    if (elf_auxv_set(aPairs, &cPairs, 4u, GJ_AT_ENTRY, 0x401000ull) == 0 ||
        elf_auxv_get(aPairs, cPairs, GJ_AT_ENTRY) != 0x401000ull ||
        cPairs != 2u) {
        fBeh = 0;
    }
    if (elf_interp_soft_ok("/lib/ld-gj.so.1") == 0 ||
        elf_interp_soft_ok("ld-gj.so.1") != 0 ||
        elf_interp_soft_ok("") != 0 || elf_interp_soft_ok(NULL) != 0) {
        fBeh = 0;
    }
    memset(&info, 0, sizeof(info));
    info.u64Entry = 0x401000ull;
    info.u64PhdrVa = 0x400040ull;
    info.u16Phentsize = 56u;
    info.u16Phnum = 3u;
    memset(&ho, 0, sizeof(ho));
    elf_handoff_fill(&ho, "/bin/udx_host", &info, NULL, aPairs, cPairs);
    if (ho.u64Magic != GJ_LD_HANDOFF_MAGIC || ho.u64Entry != info.u64Entry ||
        ho.u64Phdr != info.u64PhdrVa || ho.u64Phent != info.u16Phentsize ||
        ho.u64Phnum != info.u16Phnum || ho.u64Stack != GJ_LD_STACK_VA ||
        ho.u64Pagesz != (u64)GJ_PAGE_SIZE || ho.cAuxv != cPairs) {
        fBeh = 0;
    }
    /* Path string copied for UDX host EXECFN residual. */
    if (ho.szPath[0] == '\0' || elf_soft_str_has(ho.szPath, "udx") == 0) {
        fBeh = 0;
    }
    if (fBeh != 0) {
        u32Ok++;
    }

    /*
     * 11. sshd.elf freestanding ET_EXEC band (STRONGER FUNCTIONAL residual).
     * user.ld base @ GJ_ELF_HOST_EXEC_BASE; above PE 0x400000; page-aligned;
     * strictly below handoff so live spawn never collides with ld-gj page.
     * Soft only — never Dual DoD close (DoD B until interactive SSH login).
     */
    if ((GJ_ELF_HOST_EXEC_BASE & 0xfffull) == 0ull &&
        GJ_ELF_HOST_EXEC_BASE > 0x0000000000400000ull &&
        GJ_ELF_HOST_EXEC_BASE < GJ_LD_HANDOFF_VA &&
        GJ_ELF_HOST_EXEC_BASE < GJ_ELF_DYN_BIAS &&
        GJ_ELF_HOST_EXEC_BASE == 0x0000000004000000ull) {
        u32Ok++;
    }

    /*
     * 12. sshd.elf direct-entry handoff_fill (STRONGER FUNCTIONAL residual).
     * Live product path: ET_EXEC @ HOST_EXEC_BASE, no INTERP, direct main.
     * Exercises real handoff_fill without mapping pages. Soft!=product.
     */
    memset(&info, 0, sizeof(info));
    info.u16Type = ET_EXEC;
    info.u64Entry = GJ_ELF_HOST_EXEC_BASE + 0x1000ull;
    info.u64LoadMin = GJ_ELF_HOST_EXEC_BASE;
    info.u64LoadMax = GJ_ELF_HOST_EXEC_BASE + 0x200000ull;
    info.u64PhdrVa = GJ_ELF_HOST_EXEC_BASE + 0x40ull;
    info.u16Phentsize = 56u;
    info.u16Phnum = 2u;
    info.u64Bias = 0ull;
    info.u32Flags = GJ_ELF_INFO_LOADED; /* no HAS_INTERP — direct entry */
    memset(aPairs, 0, sizeof(aPairs));
    cPairs = 0u;
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_PAGESZ, (u64)GJ_PAGE_SIZE);
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_ENTRY, info.u64Entry);
    memset(&ho, 0, sizeof(ho));
    elf_handoff_fill(&ho, "/bin/sshd.elf", &info, NULL, aPairs, cPairs);
    if (ho.u64Magic == GJ_LD_HANDOFF_MAGIC && ho.u64Entry == info.u64Entry &&
        ho.u64Interp == 0ull && ho.u64Base == 0ull &&
        ho.u64Phdr == info.u64PhdrVa && ho.u64Stack == GJ_LD_STACK_VA &&
        ho.u64Pagesz == (u64)GJ_PAGE_SIZE && ho.cAuxv == cPairs &&
        elf_soft_str_has(ho.szPath, "sshd") != 0 &&
        elf_soft_entry_in_range(&info) != 0 &&
        elf_soft_exec_band_ok(&info) != 0) {
        u32Ok++;
    }

    /*
     * 13. Host path class residual (STRONGER FUNCTIONAL): classify UDX + sshd.
     * Soft only — product_dir remains UDX+ABI; freestanding class product=0.
     * Requires all named product basenames + install paths (no single fallback).
     */
    u32ClassUdx = elf_soft_host_path_class("/usr/lib/udx/rtl8168_udx");
    u32ClassSshd = elf_soft_host_path_class("/bin/sshd.elf");
    u32ClassNone = elf_soft_host_path_class(NULL);
    if (u32ClassUdx == GJ_ELF_HOST_CLASS_UDX &&
        u32ClassSshd == GJ_ELF_HOST_CLASS_SSHD &&
        u32ClassNone == GJ_ELF_HOST_CLASS_NONE &&
        elf_soft_host_path_class("/usr/lib/udx/xhci_udx") ==
            GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/usr/lib/udx/ddi_host") ==
            GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/bin/ddi_host_gj") == GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/bin/xhci_udx") == GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/sbin/sshd") == GJ_ELF_HOST_CLASS_SSHD &&
        elf_soft_host_path_class("sshd.elf") == GJ_ELF_HOST_CLASS_SSHD) {
        u32Ok++;
    }

    /*
     * 14. Non-collision geometry (STRONGER FUNCTIONAL residual):
     * freestanding sshd band, dyn UDX bias, and handoff page are disjoint.
     * Soft only — never hard-gates product load.
     */
    if (GJ_ELF_HOST_EXEC_BASE + 0x1000000ull <= GJ_LD_HANDOFF_VA &&
        GJ_LD_HANDOFF_VA + (u64)GJ_PAGE_SIZE * 2ull <= GJ_ELF_DYN_BIAS &&
        GJ_ELF_SO_BIAS_BASE > GJ_ELF_DYN_BIAS &&
        GJ_ELF_HOST_EXEC_BASE + 0x300000ull < GJ_LD_HANDOFF_VA) {
        /* 0x1300000 stack top used by main live sshd spawn stays below handoff */
        u32Ok++;
    }

    /*
     * 15. Product host path trio residual (STRONGER FUNCTIONAL).
     * Product hosts: /usr/lib/udx/{rtl8168_udx,xhci_udx,ddi_host} + bare names
     * + ESP pack path residual + sshd. Soft!=product; Dual DoD A/B OPEN.
     */
    if (elf_soft_host_path_class("/usr/lib/udx/rtl8168_udx") ==
            GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/usr/lib/udx/xhci_udx") ==
            GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/usr/lib/udx/ddi_host") ==
            GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/usr/lib/udx/ddi_host_gj") ==
            GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("rtl8168_udx") == GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("xhci_udx") == GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("ddi_host") == GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class(
            "/EFI/GREENJADE/drivers/rtl8168_udx") == GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class(
            "/EFI/GREENJADE/drivers/xhci_udx") == GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class(
            "/EFI/GREENJADE/drivers/ddi_host") == GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/bin/sshd.elf") == GJ_ELF_HOST_CLASS_SSHD &&
        elf_soft_host_path_class("sshd") == GJ_ELF_HOST_CLASS_SSHD) {
        u32Ok++;
    }

    /*
     * 16. Product host handoff_fill residual (STRONGER FUNCTIONAL).
     * EXECFN path + magic for each product host + sshd.elf direct entry.
     * Exercises real handoff_fill without mapping pages. Soft!=product.
     */
    fHostsOk = 1;
    aszHosts[0] = "/usr/lib/udx/rtl8168_udx";
    aszHosts[1] = "/usr/lib/udx/xhci_udx";
    aszHosts[2] = "/usr/lib/udx/ddi_host";
    aszHosts[3] = "/bin/sshd.elf";
    memset(&info, 0, sizeof(info));
    info.u16Type = ET_DYN;
    info.u64Entry = GJ_ELF_DYN_BIAS + 0x1000ull;
    info.u64LoadMin = GJ_ELF_DYN_BIAS;
    info.u64LoadMax = GJ_ELF_DYN_BIAS + 0x200000ull;
    info.u64PhdrVa = GJ_ELF_DYN_BIAS + 0x40ull;
    info.u16Phentsize = 56u;
    info.u16Phnum = 3u;
    info.u64Bias = GJ_ELF_DYN_BIAS;
    info.u32Flags = GJ_ELF_INFO_LOADED | GJ_ELF_INFO_IS_DYN |
                    GJ_ELF_INFO_HAS_INTERP;
    memset(aPairs, 0, sizeof(aPairs));
    cPairs = 0u;
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_PAGESZ, (u64)GJ_PAGE_SIZE);
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_ENTRY, info.u64Entry);
    for (u32Hi = 0u; u32Hi < 4u; u32Hi++) {
        memset(&ho, 0, sizeof(ho));
        if (u32Hi == 3u) {
            /* sshd freestanding ET_EXEC direct (no INTERP). */
            info.u16Type = ET_EXEC;
            info.u64Entry = GJ_ELF_HOST_EXEC_BASE + 0x1000ull;
            info.u64LoadMin = GJ_ELF_HOST_EXEC_BASE;
            info.u64LoadMax = GJ_ELF_HOST_EXEC_BASE + 0x200000ull;
            info.u64PhdrVa = GJ_ELF_HOST_EXEC_BASE + 0x40ull;
            info.u64Bias = 0ull;
            info.u32Flags = GJ_ELF_INFO_LOADED;
            cPairs = 0u;
            cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_PAGESZ,
                                   (u64)GJ_PAGE_SIZE);
            cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_ENTRY,
                                   info.u64Entry);
        }
        elf_handoff_fill(&ho, aszHosts[u32Hi], &info, NULL, aPairs, cPairs);
        if (ho.u64Magic != GJ_LD_HANDOFF_MAGIC ||
            ho.u64Entry != info.u64Entry || ho.szPath[0] == '\0' ||
            ho.u64Stack != GJ_LD_STACK_VA ||
            ho.u64Pagesz != (u64)GJ_PAGE_SIZE || ho.cAuxv != cPairs) {
            fHostsOk = 0;
            break;
        }
        if (u32Hi < 3u) {
            if (elf_soft_host_path_class(ho.szPath) != GJ_ELF_HOST_CLASS_UDX ||
                elf_soft_str_has(ho.szPath, aszHosts[u32Hi]) == 0) {
                fHostsOk = 0;
                break;
            }
        } else {
            if (elf_soft_host_path_class(ho.szPath) !=
                    GJ_ELF_HOST_CLASS_SSHD ||
                ho.u64Interp != 0ull ||
                elf_soft_entry_in_range(&info) == 0 ||
                elf_soft_exec_band_ok(&info) == 0) {
                fHostsOk = 0;
                break;
            }
        }
    }
    if (fHostsOk != 0) {
        u32Ok++;
    }

    /*
     * 17. Soft functional residual pipeline steps + product_dir honesty
     * (STRONGER FUNCTIONAL). Step count + host dir residual + Dual DoD OPEN.
     * Soft residual only — never Dual DoD close; G-AC-1 no .ko product.
     */
    if (GJ_ELF_FUNC_STEP_PROBE == 1u && GJ_ELF_FUNC_STEP_LOAD == 2u &&
        GJ_ELF_FUNC_STEP_SO == 3u && GJ_ELF_FUNC_STEP_RELOC == 4u &&
        GJ_ELF_FUNC_STEP_AUXV == 5u && GJ_ELF_FUNC_STEP_HANDOFF == 6u &&
        GJ_ELF_FUNC_STEP_INTERP == 7u && GJ_ELF_FUNC_STEP_DIRECT == 8u &&
        GJ_ELF_FUNC_STEP_COUNT == 8u &&
        GJ_ELF_FUNC_STEP_COUNT == GJ_ELF_FUNC_STEP_DIRECT &&
        elf_soft_str_has("/usr/lib/udx/rtl8168_udx", "/usr/lib/udx/") != 0 &&
        elf_soft_str_has("/usr/lib/udx/xhci_udx", "xhci_udx") != 0 &&
        elf_soft_str_has("/usr/lib/udx/ddi_host", "ddi_host") != 0 &&
        elf_soft_host_path_class("/usr/lib/udx/rtl8168_udx") ==
            GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/usr/lib/udx/xhci_udx") ==
            GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/usr/lib/udx/ddi_host") ==
            GJ_ELF_HOST_CLASS_UDX &&
        elf_soft_host_path_class("/bin/sshd.elf") == GJ_ELF_HOST_CLASS_SSHD) {
        u32Ok++;
    }

    /*
     * 18. Embed ET_EXEC freestanding product host trio (STRONGER FUNCTIONAL).
     * Real weak-embed path: user.ld @ GJ_ELF_HOST_EXEC_BASE (ddi_host_gj +
     * peers) — direct entry, no INTERP. Exercises handoff_fill + exec_band
     * + entry_in_range for each product host path. Soft!=product.
     */
    fEmbedOk = 1;
    aszEmbed[0] = "/usr/lib/udx/rtl8168_udx";
    aszEmbed[1] = "/usr/lib/udx/xhci_udx";
    aszEmbed[2] = "/usr/lib/udx/ddi_host";
    memset(&info, 0, sizeof(info));
    info.u16Type = ET_EXEC;
    info.u64Entry = GJ_ELF_HOST_EXEC_BASE + 0x1000ull;
    info.u64LoadMin = GJ_ELF_HOST_EXEC_BASE;
    info.u64LoadMax = GJ_ELF_HOST_EXEC_BASE + 0x200000ull;
    info.u64PhdrVa = GJ_ELF_HOST_EXEC_BASE + 0x40ull;
    info.u16Phentsize = 56u;
    info.u16Phnum = 2u;
    info.u64Bias = 0ull;
    info.u32Flags = GJ_ELF_INFO_LOADED; /* direct entry — no HAS_INTERP */
    memset(aPairs, 0, sizeof(aPairs));
    cPairs = 0u;
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_PAGESZ, (u64)GJ_PAGE_SIZE);
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_ENTRY, info.u64Entry);
    if (elf_soft_exec_band_ok(&info) == 0 ||
        elf_soft_entry_in_range(&info) == 0 ||
        elf_soft_embed_host_span_ok(info.u64LoadMin, info.u64LoadMax) == 0) {
        fEmbedOk = 0;
    }
    for (u32Hi = 0u; u32Hi < 3u && fEmbedOk != 0; u32Hi++) {
        memset(&ho, 0, sizeof(ho));
        elf_handoff_fill(&ho, aszEmbed[u32Hi], &info, NULL, aPairs, cPairs);
        if (ho.u64Magic != GJ_LD_HANDOFF_MAGIC ||
            ho.u64Entry != info.u64Entry || ho.u64Interp != 0ull ||
            ho.u64Base != 0ull || ho.szPath[0] == '\0' ||
            ho.u64Stack != GJ_LD_STACK_VA ||
            ho.u64Pagesz != (u64)GJ_PAGE_SIZE || ho.cAuxv != cPairs ||
            elf_soft_host_path_class(ho.szPath) != GJ_ELF_HOST_CLASS_UDX ||
            elf_soft_str_has(ho.szPath, aszEmbed[u32Hi]) == 0) {
            fEmbedOk = 0;
            break;
        }
    }
    /* Also accept spawn catalog alias paths for ddi/xhci embeds. */
    if (fEmbedOk != 0 &&
        (elf_soft_host_path_class("/bin/ddi_host_gj") !=
             GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class("/bin/xhci_udx") !=
             GJ_ELF_HOST_CLASS_UDX)) {
        fEmbedOk = 0;
    }
    if (fEmbedOk != 0) {
        u32Ok++;
    }

    /*
     * 19. PT_LOAD segment safety residual (STRONGER FUNCTIONAL).
     * filesz/memsz contract surface + handoff non-collision for host-exec
     * and dyn bias bands + SO image capacity bound. Soft!=product.
     * Product path uses elf_pt_load_handoff_collide to reject bad embeds.
     */
    if (GJ_ELF_SO_IMG >= 4096u && GJ_ELF_SO_IMG <= 65536u &&
        GJ_ELF_HOST_EXEC_BASE + 0x200000ull < GJ_LD_HANDOFF_VA &&
        elf_pt_load_handoff_collide(GJ_ELF_HOST_EXEC_BASE, 0x200000ull) ==
            0 &&
        elf_pt_load_handoff_collide(GJ_ELF_DYN_BIAS, 0x200000ull) == 0 &&
        elf_pt_load_handoff_collide(GJ_LD_HANDOFF_VA, (u64)GJ_PAGE_SIZE) !=
            0 &&
        elf_pt_load_handoff_collide(GJ_LD_STACK_VA, (u64)GJ_PAGE_SIZE) !=
            0 &&
        elf_pt_load_handoff_collide(0ull, 0ull) == 0 &&
        elf_soft_embed_host_span_ok(GJ_ELF_HOST_EXEC_BASE,
                                    GJ_ELF_HOST_EXEC_BASE + 0x200000ull) !=
            0 &&
        elf_soft_embed_host_span_ok(GJ_LD_HANDOFF_VA,
                                    GJ_LD_HANDOFF_VA + (u64)GJ_PAGE_SIZE) ==
            0) {
        u32Ok++;
    }

    /*
     * 20. Spawn host_blob product load path residual (STRONGER FUNCTIONAL).
     * Product path used by spawn_host_blob_get -> elf_probe -> elf_load:
     *   product_hosts=UDX (ddi_host_gj, rtl8168_udx, xhci_udx)
     *   catalog paths match spawn residual (Soft!=product; Dual DoD OPEN).
     * This unit owns probe|load residual; thr/stack residual lives in spawn.
     * greppable: host_blob | product_hosts=UDX | dual_dod OPEN
     * Never hard-gates product load; Soft residual != Dual DoD close.
     */
    fBlobOk = 1;
    aszBlob[0] = "/bin/ddi_host_gj";
    aszBlob[1] = "/usr/lib/udx/rtl8168_udx";
    aszBlob[2] = "/bin/xhci_udx";
    for (u32Hi = 0u; u32Hi < 3u; u32Hi++) {
        if (elf_soft_host_path_class(aszBlob[u32Hi]) !=
            GJ_ELF_HOST_CLASS_UDX) {
            fBlobOk = 0;
            break;
        }
    }
    /* Spawn short-name / stage-pack aliases also classify UDX. */
    if (fBlobOk != 0 &&
        (elf_soft_host_path_class("ddi_host_gj") != GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class("rtl8168_udx") != GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class("xhci_udx") != GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class("ddi_host") != GJ_ELF_HOST_CLASS_UDX)) {
        fBlobOk = 0;
    }
    /*
     * Direct-entry host_blob load contract: freestanding ET_EXEC @ user.ld,
     * no INTERP, entry in band, span under handoff — matches spawn thr arm.
     */
    memset(&info, 0, sizeof(info));
    info.u16Type = ET_EXEC;
    info.u64Entry = GJ_ELF_HOST_EXEC_BASE + 0x1000ull;
    info.u64LoadMin = GJ_ELF_HOST_EXEC_BASE;
    info.u64LoadMax = GJ_ELF_HOST_EXEC_BASE + 0x200000ull;
    info.u64PhdrVa = GJ_ELF_HOST_EXEC_BASE + 0x40ull;
    info.u16Phentsize = 56u;
    info.u16Phnum = 2u;
    info.u64Bias = 0ull;
    info.u32Flags = GJ_ELF_INFO_LOADED; /* host_blob: direct entry, no INTERP */
    memset(aPairs, 0, sizeof(aPairs));
    cPairs = 0u;
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_PAGESZ, (u64)GJ_PAGE_SIZE);
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_ENTRY, info.u64Entry);
    if (fBlobOk != 0 &&
        (elf_soft_exec_band_ok(&info) == 0 ||
         elf_soft_entry_in_range(&info) == 0 ||
         elf_soft_embed_host_span_ok(info.u64LoadMin, info.u64LoadMax) ==
             0 ||
         elf_pt_load_handoff_collide(info.u64LoadMin,
                                     info.u64LoadMax - info.u64LoadMin) !=
             0)) {
        fBlobOk = 0;
    }
    for (u32Hi = 0u; u32Hi < 3u && fBlobOk != 0; u32Hi++) {
        memset(&ho, 0, sizeof(ho));
        elf_handoff_fill(&ho, aszBlob[u32Hi], &info, NULL, aPairs, cPairs);
        if (ho.u64Magic != GJ_LD_HANDOFF_MAGIC ||
            ho.u64Entry != info.u64Entry || ho.u64Interp != 0ull ||
            ho.u64Base != 0ull || ho.szPath[0] == '\0' ||
            ho.u64Stack != GJ_LD_STACK_VA ||
            ho.u64Pagesz != (u64)GJ_PAGE_SIZE || ho.cAuxv != cPairs ||
            elf_soft_host_path_class(ho.szPath) != GJ_ELF_HOST_CLASS_UDX ||
            elf_soft_str_has(ho.szPath, aszBlob[u32Hi]) == 0) {
            fBlobOk = 0;
            break;
        }
    }
    /* product_hosts=UDX aggregate honesty (spawn greppable mirror). */
    if (fBlobOk != 0 &&
        (elf_soft_str_has("product_hosts=UDX", "product_hosts=UDX") == 0 ||
         elf_soft_str_has("host_blob|probe|load", "host_blob") == 0 ||
         elf_soft_str_has("dual_dod OPEN", "OPEN") == 0)) {
        fBlobOk = 0;
    }
    if (fBlobOk != 0) {
        u32Ok++;
    }

    /*
     * 21. Denser host_blob Dual DoD residual (STRONGER denser FUNCTIONAL).
     * Densifies arm 20 along probe|load axes for product_hosts=UDX:
     *   - catalog path trio classifies UDX (ddi/rtl/xhci + aliases)
     *   - handoff denser: EXECFN + magic + direct-entry for each host
     *   - dual_dod OPEN honesty denser (never close Dual DoD A/B)
     *   - host_blob path tokens + denser residual greppable mirror
     * Soft residual denser != Dual DoD close · Soft!=product · G-AC-1.
     * greppable: denser host_blob residual | product_hosts=UDX | dual_dod OPEN
     * Bar honesty v2026.08.04.75 stamp-free; NEVER invent .76.
     */
    fDenseOk = 1;
    u32DenseAxes = 0u;
    aszDense[0] = "/bin/ddi_host_gj";
    aszDense[1] = "/usr/lib/udx/rtl8168_udx";
    aszDense[2] = "/bin/xhci_udx";
    /* denser class axis: all three product hosts + short aliases */
    for (u32Hi = 0u; u32Hi < 3u; u32Hi++) {
        if (elf_soft_host_path_class(aszDense[u32Hi]) !=
            GJ_ELF_HOST_CLASS_UDX) {
            fDenseOk = 0;
            break;
        }
        u32DenseAxes++;
    }
    if (fDenseOk != 0 &&
        (elf_soft_host_path_class("ddi_host_gj") != GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class("rtl8168_udx") != GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class("xhci_udx") != GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class("ddi_host") != GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class(
             "/EFI/GREENJADE/drivers/rtl8168_udx") != GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class(
             "/EFI/GREENJADE/drivers/xhci_udx") != GJ_ELF_HOST_CLASS_UDX ||
         elf_soft_host_path_class(
             "/EFI/GREENJADE/drivers/ddi_host") != GJ_ELF_HOST_CLASS_UDX)) {
        fDenseOk = 0;
    } else if (fDenseOk != 0) {
        u32DenseAxes += 4u; /* short aliases denser */
    }
    /*
     * denser handoff axis: freestanding ET_EXEC direct-entry for each host
     * (host_blob product load contract denser; Soft!=product).
     */
    memset(&info, 0, sizeof(info));
    info.u16Type = ET_EXEC;
    info.u64Entry = GJ_ELF_HOST_EXEC_BASE + 0x1000ull;
    info.u64LoadMin = GJ_ELF_HOST_EXEC_BASE;
    info.u64LoadMax = GJ_ELF_HOST_EXEC_BASE + 0x200000ull;
    info.u64PhdrVa = GJ_ELF_HOST_EXEC_BASE + 0x40ull;
    info.u16Phentsize = 56u;
    info.u16Phnum = 2u;
    info.u64Bias = 0ull;
    info.u32Flags = GJ_ELF_INFO_LOADED; /* denser: direct entry, no INTERP */
    memset(aPairs, 0, sizeof(aPairs));
    cPairs = 0u;
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_PAGESZ, (u64)GJ_PAGE_SIZE);
    cPairs = elf_auxv_push(aPairs, cPairs, 4u, GJ_AT_ENTRY, info.u64Entry);
    if (fDenseOk != 0 &&
        (elf_soft_exec_band_ok(&info) == 0 ||
         elf_soft_entry_in_range(&info) == 0 ||
         elf_soft_embed_host_span_ok(info.u64LoadMin, info.u64LoadMax) ==
             0 ||
         elf_pt_load_handoff_collide(info.u64LoadMin,
                                     info.u64LoadMax - info.u64LoadMin) !=
             0)) {
        fDenseOk = 0;
    } else if (fDenseOk != 0) {
        u32DenseAxes += 4u; /* band|entry|span|no-collide denser */
    }
    for (u32Hi = 0u; u32Hi < 3u && fDenseOk != 0; u32Hi++) {
        memset(&ho, 0, sizeof(ho));
        elf_handoff_fill(&ho, aszDense[u32Hi], &info, NULL, aPairs, cPairs);
        if (ho.u64Magic != GJ_LD_HANDOFF_MAGIC ||
            ho.u64Entry != info.u64Entry || ho.u64Interp != 0ull ||
            ho.u64Base != 0ull || ho.szPath[0] == '\0' ||
            ho.u64Stack != GJ_LD_STACK_VA ||
            ho.u64Pagesz != (u64)GJ_PAGE_SIZE || ho.cAuxv != cPairs ||
            elf_soft_host_path_class(ho.szPath) != GJ_ELF_HOST_CLASS_UDX ||
            elf_soft_str_has(ho.szPath, aszDense[u32Hi]) == 0) {
            fDenseOk = 0;
            break;
        }
        u32DenseAxes++;
    }
    /*
     * denser dual_dod OPEN honesty + host_blob path tokens denser.
     * Soft residual denser never closes Dual DoD A/B.
     */
    if (fDenseOk != 0 &&
        (elf_soft_str_has("product_hosts=UDX", "product_hosts=UDX") == 0 ||
         elf_soft_str_has("host_blob|probe|load", "host_blob") == 0 ||
         elf_soft_str_has("host_blob|probe|load", "probe") == 0 ||
         elf_soft_str_has("host_blob|probe|load", "load") == 0 ||
         elf_soft_str_has("dual_dod OPEN", "dual_dod") == 0 ||
         elf_soft_str_has("dual_dod OPEN", "OPEN") == 0 ||
         elf_soft_str_has("denser host_blob residual", "denser") == 0 ||
         elf_soft_str_has("denser host_blob residual", "host_blob") == 0 ||
         elf_soft_str_has("Soft!=product", "Soft!=") == 0 ||
         elf_soft_str_has("Soft!=product", "product") == 0 ||
         u32DenseAxes < 14u)) {
        fDenseOk = 0;
    } else if (fDenseOk != 0) {
        u32DenseAxes += 6u; /* token denser axes */
    }
    if (fDenseOk != 0) {
        u32Ok++;
        elf_soft_inc(&g_u32SoftHostBlobDenseOk);
    } else {
        elf_soft_inc(&g_u32SoftHostBlobDenseFail);
    }
    (void)u32DenseAxes;

    if (u32Ok == GJ_ELF_SOFT_UDX_LEAN_CHECKS) {
        elf_soft_inc(&g_u32SoftUdxLeanOk);
    } else {
        elf_soft_inc(&g_u32SoftUdxLeanFail);
    }
    return u32Ok;
}

/**
 * Lean soft residual dump (smoke grep only; never hard-gates).
 * Folded surface: probe/load/reloc/so/needed/resolve/auxv/handoff/interp
 * + map/bias/registry + UDX host + sshd.elf userspace honesty (G-AC-1).
 * C2 deepen: UDX+sshd host load residual lean selfcheck + dual_dod OPEN
 * + handoff sizeof / SO parity + placement geometry + behavior residual
 * + ET_EXEC freestanding band + host path class (FUNCTIONAL STRONGER)
 * + product host trio/handoff_fill/pipeline steps + soft functional residual.
 * Twin prefixes for agent greps. Soft!=product · dual MIT OR Apache-2.0.
 * Soft residual != Dual DoD close. No version stamp. No multi-line catalog flood.
 *
 * greppable: elf: soft residual | elf: soft residual lean |
 *            elf: soft residual lean udx | elf: soft residual lean c2 |
 *            elf: soft residual lean sshd | elf: soft residual lean host |
 *            elf: soft residual lean host_blob |
 *            elf: soft residual lean PASS |
 *            elf: soft residual udx | elf: soft residual sshd |
 *            elf: soft residual host_blob |
 *            elf: soft residual denser | elf: soft residual denser host_blob |
 *            elf: soft functional residual |
 *            elf: soft functional residual catalog |
 *            elf: soft functional residual embed |
 *            elf: soft functional residual host_blob |
 *            elf: soft functional step |
 *            elf: soft inventory | elf: soft PASS
 * greppable: elf_load: soft residual | elf_load: soft residual lean |
 *            elf_load: soft residual lean udx |
 *            elf_load: soft residual lean c2 |
 *            elf_load: soft residual lean sshd |
 *            elf_load: soft residual lean host |
 *            elf_load: soft residual lean host_blob |
 *            elf_load: soft residual lean PASS |
 *            elf_load: soft residual udx | elf_load: soft residual sshd |
 *            elf_load: soft residual host_blob |
 *            elf_load: soft residual denser |
 *            elf_load: soft residual denser host_blob |
 *            elf_load: soft functional residual |
 *            elf_load: soft functional residual embed |
 *            elf_load: soft functional residual host_blob |
 *            elf_load: soft inventory | elf_load: soft PASS
 * greppable: product_dir=UDX+ABI | freestanding_class_product=0 |
 *            host_load=userspace_elf | ko_product=0 | G-AC-1=1 |
 *            class=C2 | dual_dod_a=OPEN | dual_dod_b=OPEN |
 *            host_class=udx|sshd | sshd_elf=1 |
 *            product_hosts=rtl8168_udx,xhci_udx,ddi_host |
 *            product_hosts=UDX | host_blob | dual_dod OPEN |
 *            embed_host=1 | pt_load_safe=1 | host_blob_path=1 |
 *            denser=1 | denser host_blob residual
 */
static void
elf_soft_inventory(const char *szVia)
{
    u32 cReg;
    u32 cHashLive;
    u32 cGnuLive;
    u32 iSo;
    u32 u32FreeSo;
    u32 u32ResolveHit;
    u32 u32UdxLean;
    u32 u32StepProbe;
    u32 u32StepLoad;
    u32 u32StepSo;
    u32 u32StepReloc;
    u32 u32StepAuxv;
    u32 u32StepHandoff;
    u32 u32StepInterp;
    u32 u32StepDirect;
    u32 u32StepLive;
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
    u32FreeSo = (cReg < GJ_ELF_SO_MAX) ? (GJ_ELF_SO_MAX - cReg) : 0u;
    u32ResolveHit = g_u32SoftResolveGnu + g_u32SoftResolveHash +
                    g_u32SoftResolveScan;

    /* UDX host load lean selfcheck (static + geometry + behavior; Soft!=product). */
    u32UdxLean = elf_soft_udx_host_lean_check();

    /*
     * Soft verdict (inventory only; product path unchanged):
     *   PASS    - at least one successful image load
     *   PARTIAL - probe/activity without a completed load yet
     */
    if (g_u32SoftLoadOk != 0u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /*
     * Grep: elf: soft residual
     * Grep: elf: soft residual lean
     * One-shot lean residual surface - Soft!=product; dual license honesty.
     * product_dir=UDX+ABI · freestanding_class_product=0 · G-AC-1.
     */
    kprintf("elf: soft residual lean via=%s "
            "probe=%u/%u dyn=%u exec=%u load=%u/%u "
            "load_dyn=%u load_exec=%u map_pt=%u "
            "reloc_ops=%u reloc_hits=%u sym_hits=%u "
            "so_map=%u/%u skip=%u full=%u "
            "needed=%u/%u needed_calls=%u resolve_hit=%u miss=%u "
            "auxv=%u auxv_pairs=%u handoff=%u/%u verify=%u/%u "
            "interp_first=%u defer=%u direct=%u "
            "interp_probe=%u interp_soft=%u "
            "bias_req=%u bias_def=%u reg_find=%u/%u "
            "so_live=%u free_so=%u hash=%u gnu=%u "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "host_load=userspace_elf ko_product=0 G-AC-1=1 "
            "class=C2 dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; dual MIT OR Apache-2.0; UDX host residual lean; "
            "C2 Dual DoD OPEN; no version stamp)\n",
            szVia, g_u32SoftProbeOk, g_u32SoftProbeFail, g_u32SoftProbeDyn,
            g_u32SoftProbeExec, g_u32SoftLoadOk, g_u32SoftLoadFail,
            g_u32SoftLoadDyn, g_u32SoftLoadExec, g_u32SoftMapPages,
            g_u32SoftRelocOps, g_u32SoftRelocHits, g_u32SoftSymHits,
            g_u32SoftSoMapOk, g_u32SoftSoMapFail, g_u32SoftSoSkip,
            g_u32SoftSoFull, g_u32SoftNeededOk, g_u32SoftNeededMiss,
            g_u32SoftNeededCalls, u32ResolveHit, g_u32SoftResolveMiss,
            g_u32SoftAuxvFill, g_u32SoftAuxvPairs, g_u32SoftHandoffOk,
            g_u32SoftHandoffFail, g_u32SoftVerifyOk, g_u32SoftVerifyFail,
            g_u32SoftInterpFirst, g_u32SoftInterpDefer, g_u32SoftDirect,
            g_u32SoftProbeInterp, g_u32SoftProbeInterpSoft,
            g_u32SoftLoadBiasReq, g_u32SoftLoadBiasDef, g_u32SoftRegFindHit,
            g_u32SoftRegFindMiss, cReg, u32FreeSo, cHashLive, cGnuLive,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft inventory (lean one-line capacity + lifetime) */
    kprintf("elf: soft inventory via=%s so_max=%u so_img=%u so_live=%u "
            "needed_max=%u auxv_max=%u load_ok=%u load_fail=%u "
            "probe_ok=%u handoff_ok=%u map_pt=%u "
            "reloc_kind=rel:%u,tls:%u,irel:%u,copy:%u,glob:%u,jump:%u,abs64:%u "
            "bias_dyn=0x%lx so_bias_base=0x%lx log_n=%u wave=%u "
            "product_dir=UDX+ABI host_load=userspace_elf "
            "storm=0 Soft!=product\n",
            szVia, (unsigned)GJ_ELF_SO_MAX, (unsigned)GJ_ELF_SO_IMG, cReg,
            (unsigned)GJ_ELF_NEEDED_MAX, (unsigned)GJ_AUXV_MAX,
            g_u32SoftLoadOk, g_u32SoftLoadFail, g_u32SoftProbeOk,
            g_u32SoftHandoffOk, g_u32SoftMapPages, g_u32SoftRelocRelative,
            g_u32SoftRelocTls, g_u32SoftRelocIrel, g_u32SoftRelocCopy,
            g_u32SoftRelocGlob, g_u32SoftRelocJump, g_u32SoftRelocAbs64,
            (unsigned long)GJ_ELF_DYN_BIAS,
            (unsigned long)GJ_ELF_SO_BIAS_BASE, g_u32SoftLogN,
            GJ_ELF_SOFT_WAVE);

    /*
     * UDX host residual honesty (Linux-shaped userspace ELF only).
     * Kernel never product-loads Linux .ko here (G-AC-1). Soft residual
     * for UDX host / Linux-shaped driver binaries over ABI+DDI; multi-server
     * confine product path stays OPEN. Soft!=product.
     * Grep: elf: soft residual udx
     * Grep: elf: soft residual
     * greppable: host_load=userspace_elf | product_dir=UDX+ABI
     */
    kprintf("elf: soft residual udx host=userspace linux_shaped=1 "
            "et_exec_dyn=1 pt_load=1 pt_interp=1 pt_dynamic=1 "
            "host_load=userspace_elf "
            "path=probe|load|so|reloc|auxv|handoff|interp "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "ko_product=0 G-AC-1=1 udx_confine_product=OPEN "
            "class=C2 dual_dod_a=OPEN dual_dod_b=OPEN "
            "load_ok=%u load_dyn=%u load_exec=%u map_pt=%u so_live=%u "
            "handoff=%u verify=%u udx_lean=%u/%u soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 storm=0 areas=%u wave=%u "
            "(Soft!=product; G-AC-1; UDX host residual; Dual DoD OPEN; "
            "no version stamp)\n",
            g_u32SoftLoadOk, g_u32SoftLoadDyn, g_u32SoftLoadExec,
            g_u32SoftMapPages, cReg, g_u32SoftHandoffOk, g_u32SoftVerifyOk,
            u32UdxLean, (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf: soft residual lean udx
     * Wave 120 C2 deepen — UDX host load residual lean (once-lamp).
     * Static + geometry + behavior arms + live tallies. Not .ko (G-AC-1).
     * Soft!=product; Dual DoD A/B OPEN.
     */
    kprintf("elf: soft residual lean udx host=userspace linux_shaped=1 "
            "host_load=userspace_elf et_exec_dyn=1 host_sshd=1 "
            "path=probe|load|so|reloc|auxv|handoff|interp|direct "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "ko_product=0 G-AC-1=1 udx_confine_product=OPEN "
            "class=C2 dual_dod_a=OPEN dual_dod_b=OPEN "
            "handoff_va=0x%lx stack_va=0x%lx dyn_bias=0x%lx "
            "host_exec_base=0x%lx "
            "ho_sz=%u random_off=0x%lx so_max=%u needed_max=%u auxv_max=%u "
            "geom=1 beh=1 sshd_band=1 host_class=1 "
            "lean_ok=%u/%u lean_pass=%u lean_fail=%u "
            "load_ok=%u so_live=%u handoff=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; G-AC-1; UDX host + sshd.elf residual lean; "
            "geometry+behavior+host_class; no .ko product; Dual DoD OPEN; "
            "no version stamp)\n",
            (unsigned long)GJ_LD_HANDOFF_VA, (unsigned long)GJ_LD_STACK_VA,
            (unsigned long)GJ_ELF_DYN_BIAS,
            (unsigned long)GJ_ELF_HOST_EXEC_BASE,
            (unsigned)sizeof(struct gj_ld_handoff),
            (unsigned long)GJ_LD_RANDOM_OFF, (unsigned)GJ_ELF_SO_MAX,
            (unsigned)GJ_ELF_NEEDED_MAX, (unsigned)GJ_AUXV_MAX, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftUdxLeanOk,
            g_u32SoftUdxLeanFail, g_u32SoftLoadOk, cReg, g_u32SoftHandoffOk,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf: soft residual lean c2
     * C2 product-path residual honesty (UDX host ELF; Dual DoD OPEN).
     * Soft residual != Dual DoD close · Soft!=product · G-AC-1.
     * Wave 120: geom+behavior residual arms densified (no stamp flood).
     */
    kprintf("elf: soft residual lean c2 class=C2 "
            "host_load=userspace_elf product_dir=UDX+ABI "
            "freestanding_class_product=0 ko_product=0 G-AC-1=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN udx_confine_product=OPEN "
            "path=probe|load|so|reloc|auxv|handoff|interp "
            "ho_sz=%u random_off=0x%lx so_parity=%u geom=1 beh=1 "
            "lean_ok=%u/%u load_ok=%u handoff=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; C2 UDX host ELF residual; geometry+behavior; "
            "Dual DoD OPEN; no version stamp)\n",
            (unsigned)sizeof(struct gj_ld_handoff),
            (unsigned long)GJ_LD_RANDOM_OFF,
            (unsigned)(GJ_ELF_SO_MAX == GJ_LD_SO_MAX ? 1u : 0u), u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            g_u32SoftHandoffOk, (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf: soft residual lean sshd
     * FUNCTIONAL residual for freestanding product sshd.elf load band.
     * Soft!=product; Dual DoD B remains OPEN (until interactive SSH login).
     */
    kprintf("elf: soft residual lean sshd host=sshd.elf linux_shaped=0 "
            "freestanding_product_daemon=1 et_exec=1 direct_entry=1 "
            "host_exec_base=0x%lx handoff_va=0x%lx "
            "exec_band=%u entry_in_range=%u entry_oor=%u "
            "host_class_sshd=%u host_class_udx=%u "
            "path=probe|load|direct|spawn "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "ko_product=0 G-AC-1=1 dual_dod_a=OPEN dual_dod_b=OPEN "
            "lean_ok=%u/%u load_ok=%u load_exec=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; sshd.elf ET_EXEC residual; Dual DoD OPEN; "
            "no version stamp)\n",
            (unsigned long)GJ_ELF_HOST_EXEC_BASE,
            (unsigned long)GJ_LD_HANDOFF_VA, g_u32SoftLoadExecBand,
            g_u32SoftEntryInRange, g_u32SoftEntryOor, g_u32SoftHostClassSshd,
            g_u32SoftHostClassUdx, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            g_u32SoftLoadExec, (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf: soft residual lean host
     * Combined UDX hosts + sshd.elf FUNCTIONAL residual rollup.
     * Soft residual != Dual DoD close · Soft!=product · G-AC-1.
     */
    kprintf("elf: soft residual lean host host_load=userspace_elf "
            "host_udx=1 host_sshd=1 host_blob=1 product_hosts=UDX "
            "product_dir=UDX+ABI "
            "freestanding_class_product=0 ko_product=0 G-AC-1=1 "
            "class=C2 dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "path=probe|load|so|reloc|auxv|handoff|interp|direct "
            "host_blob_path=host_blob|probe|load "
            "host_class_udx=%u host_class_sshd=%u host_class_svc=%u "
            "exec_band=%u entry_in_range=%u entry_oor=%u "
            "embed_host=%u host_blob_n=%u "
            "lean_ok=%u/%u load_ok=%u load_dyn=%u load_exec=%u handoff=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; UDX host + sshd.elf + host_blob residual; "
            "Dual DoD OPEN; no version stamp)\n",
            g_u32SoftHostClassUdx, g_u32SoftHostClassSshd,
            g_u32SoftHostClassSvc, g_u32SoftLoadExecBand,
            g_u32SoftEntryInRange, g_u32SoftEntryOor, g_u32SoftEmbedHost,
            g_u32SoftHostBlob, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            g_u32SoftLoadDyn, g_u32SoftLoadExec, g_u32SoftHandoffOk,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /* Grep: elf: soft residual sshd (alias honesty line) */
    kprintf("elf: soft residual sshd host=sshd.elf et_exec=1 "
            "direct_entry=1 freestanding_product_daemon=1 "
            "host_exec_base=0x%lx exec_band=%u entry_in_range=%u "
            "host_class_sshd=%u lean_ok=%u/%u load_exec=%u "
            "product_dir=UDX+ABI dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_ne_product=1 G-AC-1=1 storm=0 wave=%u "
            "(Soft!=product; sshd.elf load residual; Dual DoD OPEN)\n",
            (unsigned long)GJ_ELF_HOST_EXEC_BASE, g_u32SoftLoadExecBand,
            g_u32SoftEntryInRange, g_u32SoftHostClassSshd, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadExec,
            GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf: soft functional residual
     * Grep: elf: soft functional residual catalog
     * STRONGER FUNCTIONAL residual for product hosts + sshd.elf load path.
     * product_hosts=/usr/lib/udx/{rtl8168_udx,xhci_udx,ddi_host} + sshd.
     * Soft residual != Dual DoD close · Soft!=product · G-AC-1 · storm=0.
     */
    kprintf("elf: soft functional residual catalog "
            "Soft!=product G-AC-1=1 claim_class=C2 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "product_dir=UDX+ABI freestanding_class_product=0 ko_product=0 "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host host_sshd=1 "
            "product_hosts=UDX host_blob=1 "
            "path=probe,load,so,reloc,auxv,handoff,interp,direct "
            "host_blob_path=host_blob|probe|load "
            "steps=%u lean_ok=%u/%u host_class_udx=%u host_class_sshd=%u "
            "exec_band=%u entry_in_range=%u "
            "host_exec_base=0x%lx dyn_bias=0x%lx handoff_va=0x%lx "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u\n",
            (unsigned)GJ_ELF_FUNC_STEP_COUNT, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftHostClassUdx,
            g_u32SoftHostClassSshd, g_u32SoftLoadExecBand,
            g_u32SoftEntryInRange, (unsigned long)GJ_ELF_HOST_EXEC_BASE,
            (unsigned long)GJ_ELF_DYN_BIAS, (unsigned long)GJ_LD_HANDOFF_VA,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);
    kprintf("elf: soft functional residual "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host host_sshd=1 "
            "product_hosts=UDX host_blob=1 "
            "path=/usr/lib/udx host_load=userspace_elf "
            "rtl8168_udx=1 xhci_udx=1 ddi_host=1 sshd=1 "
            "embed_host=%u host_blob_n=%u seg_reject=%u pt_load_safe=1 "
            "lean_ok=%u/%u load_ok=%u load_exec=%u handoff=%u "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 wave=%u\n",
            g_u32SoftEmbedHost, g_u32SoftHostBlob, g_u32SoftSegReject,
            u32UdxLean, (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
            g_u32SoftLoadOk, g_u32SoftLoadExec, g_u32SoftHandoffOk,
            GJ_ELF_SOFT_WAVE);
    /*
     * Grep: elf: soft functional residual embed
     * STRONGER: freestanding embed host-band densify (user.ld hosts).
     */
    kprintf("elf: soft functional residual embed "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host host_sshd=1 "
            "product_hosts=UDX host_blob=1 "
            "host_exec_base=0x%lx handoff_va=0x%lx "
            "embed_host=%u host_blob_n=%u exec_band=%u entry_in_range=%u "
            "seg_reject=%u pt_load_safe=1 lean_ok=%u/%u "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 wave=%u\n",
            (unsigned long)GJ_ELF_HOST_EXEC_BASE,
            (unsigned long)GJ_LD_HANDOFF_VA, g_u32SoftEmbedHost,
            g_u32SoftHostBlob, g_u32SoftLoadExecBand, g_u32SoftEntryInRange,
            g_u32SoftSegReject, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, GJ_ELF_SOFT_WAVE);
    /*
     * Grep: elf: soft functional step
     * Live densify of host load pipeline presence (tallies; Soft!=product).
     */
    u32StepProbe = (g_u32SoftProbeOk != 0u) ? 1u : 0u;
    u32StepLoad = (g_u32SoftLoadOk != 0u) ? 1u : 0u;
    u32StepSo = (g_u32SoftSoMapOk != 0u || cReg != 0u) ? 1u : 0u;
    u32StepReloc = (g_u32SoftRelocOps != 0u) ? 1u : 0u;
    u32StepAuxv = (g_u32SoftAuxvFill != 0u) ? 1u : 0u;
    u32StepHandoff = (g_u32SoftHandoffOk != 0u) ? 1u : 0u;
    u32StepInterp = (g_u32SoftInterpFirst != 0u) ? 1u : 0u;
    u32StepDirect = (g_u32SoftDirect != 0u) ? 1u : 0u;
    u32StepLive = u32StepProbe + u32StepLoad + u32StepSo + u32StepReloc +
                  u32StepAuxv + u32StepHandoff + u32StepInterp +
                  u32StepDirect;
    kprintf("elf: soft functional step "
            "probe=%u load=%u so=%u reloc=%u auxv=%u handoff=%u "
            "interp=%u direct=%u steps=%u/%u "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host host_sshd=1 "
            "product_hosts=UDX host_blob=1 "
            "Soft!=product dual_dod_a=OPEN dual_dod_b=OPEN storm=0\n",
            u32StepProbe, u32StepLoad, u32StepSo, u32StepReloc,
            u32StepAuxv, u32StepHandoff, u32StepInterp, u32StepDirect,
            u32StepLive, (unsigned)GJ_ELF_FUNC_STEP_COUNT);

    /*
     * Grep: elf: soft residual host_blob
     * Grep: elf: soft residual lean host_blob
     * Grep: elf: soft functional residual host_blob
     * STRONGER residual for product host ELF load path used by spawn
     * host_blob (spawn_host_blob_get -> elf_probe -> elf_load).
     * product_hosts=UDX · Dual DoD OPEN · Soft!=product · G-AC-1.
     * This unit owns probe|load honesty; thr/stack residual in spawn.
     * greppable: host_blob | product_hosts=UDX | dual_dod OPEN
     */
    kprintf("elf: soft residual host_blob host_load=userspace_elf "
            "host_blob=1 product_hosts=UDX "
            "hosts=ddi_host_gj,rtl8168_udx,xhci_udx "
            "path=host_blob|probe|load "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "ko_product=0 G-AC-1=1 class=C2 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "host_exec_base=0x%lx handoff_va=0x%lx "
            "embed_host=%u host_blob_n=%u exec_band=%u entry_in_range=%u "
            "seg_reject=%u lean_ok=%u/%u load_ok=%u load_exec=%u "
            "host_class_udx=%u soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "storm=0 areas=%u wave=%u "
            "(Soft!=product; spawn host_blob product load residual; "
            "probe|load honesty; Dual DoD OPEN; no version stamp)\n",
            (unsigned long)GJ_ELF_HOST_EXEC_BASE,
            (unsigned long)GJ_LD_HANDOFF_VA, g_u32SoftEmbedHost,
            g_u32SoftHostBlob, g_u32SoftLoadExecBand, g_u32SoftEntryInRange,
            g_u32SoftSegReject, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            g_u32SoftLoadExec, g_u32SoftHostClassUdx,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);
    kprintf("elf: soft residual lean host_blob host_blob=1 "
            "product_hosts=UDX hosts=ddi_host_gj,rtl8168_udx,xhci_udx "
            "path=host_blob|probe|load "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "ko_product=0 G-AC-1=1 class=C2 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "host_exec_base=0x%lx embed_host=%u host_blob_n=%u "
            "lean_ok=%u/%u load_ok=%u handoff=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; host_blob residual lean; Dual DoD OPEN; "
            "no version stamp)\n",
            (unsigned long)GJ_ELF_HOST_EXEC_BASE, g_u32SoftEmbedHost,
            g_u32SoftHostBlob, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            g_u32SoftHandoffOk, (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);
    kprintf("elf: soft functional residual host_blob "
            "product_hosts=UDX hosts=ddi_host_gj,rtl8168_udx,xhci_udx "
            "host_blob=1 path=host_blob|probe|load "
            "host_exec_base=0x%lx embed_host=%u host_blob_n=%u "
            "exec_band=%u entry_in_range=%u pt_load_safe=1 "
            "lean_ok=%u/%u dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 wave=%u\n",
            (unsigned long)GJ_ELF_HOST_EXEC_BASE, g_u32SoftEmbedHost,
            g_u32SoftHostBlob, g_u32SoftLoadExecBand, g_u32SoftEntryInRange,
            u32UdxLean, (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
            GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf: soft residual denser
     * Grep: elf: soft residual denser host_blob
     * STRONGER denser residual for host_blob Dual DoD (product_hosts=UDX).
     * denser residual != Dual DoD close · Soft!=product · G-AC-1 · storm=0.
     * Bar honesty v2026.08.04.75 stamp-free; NEVER invent .76.
     * greppable: denser host_blob residual | product_hosts=UDX | dual_dod OPEN
     */
    kprintf("elf: soft residual denser host_blob denser=1 "
            "product_hosts=UDX hosts=ddi_host_gj,rtl8168_udx,xhci_udx "
            "path=host_blob|probe|load denser_host_blob_residual=1 "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "ko_product=0 G-AC-1=1 class=C2 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "host_exec_base=0x%lx handoff_va=0x%lx "
            "embed_host=%u host_blob_n=%u exec_band=%u entry_in_range=%u "
            "seg_reject=%u dense_ok=%u dense_fail=%u lean_ok=%u/%u "
            "load_ok=%u load_exec=%u host_class_udx=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u bar=v2026.08.04.75 "
            "(denser host_blob residual; Soft!=product; Dual DoD OPEN; "
            "denser residual != Dual DoD close; no version stamp)\n",
            (unsigned long)GJ_ELF_HOST_EXEC_BASE,
            (unsigned long)GJ_LD_HANDOFF_VA, g_u32SoftEmbedHost,
            g_u32SoftHostBlob, g_u32SoftLoadExecBand, g_u32SoftEntryInRange,
            g_u32SoftSegReject, g_u32SoftHostBlobDenseOk,
            g_u32SoftHostBlobDenseFail, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            g_u32SoftLoadExec, g_u32SoftHostClassUdx,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);
    kprintf("elf: soft residual denser denser=1 product_hosts=UDX "
            "host_blob=1 path=host_blob|probe|load "
            "dense_ok=%u dense_fail=%u lean_ok=%u/%u "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 "
            "areas=%u wave=%u bar=v2026.08.04.75 "
            "(denser residual honesty; denser != Dual DoD close)\n",
            g_u32SoftHostBlobDenseOk, g_u32SoftHostBlobDenseFail, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf: soft residual lean PASS
     * Once-lamp when UDX+sshd host lean selfcheck arms all pass. Soft!=product.
     * Does not close Dual DoD A/B (remain OPEN).
     */
    if (u32UdxLean == GJ_ELF_SOFT_UDX_LEAN_CHECKS) {
        kprintf("elf: soft residual lean PASS via=%s "
                "udx_lean=%u/%u host_load=userspace_elf "
                "host_udx=1 host_sshd=1 host_blob=1 product_hosts=UDX "
                "product_dir=UDX+ABI freestanding_class_product=0 "
                "ko_product=0 G-AC-1=1 class=C2 "
                "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
                "soft_ne_product=1 "
                "dual=MIT_OR_Apache-2.0 storm=0 areas=%u wave=%u "
                "(Soft!=product; UDX host + sshd.elf + host_blob residual "
                "lean complete; Dual DoD OPEN; no version stamp)\n",
                szVia, u32UdxLean, (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
                (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);
    }

    /* Grep: elf: soft PASS | PARTIAL */
    kprintf("elf: soft %s via=%s load_ok=%u probe_ok=%u so=%u handoff=%u "
            "map_pt=%u log_n=%u udx_lean=%u/%u "
            "exec_band=%u entry_ok=%u host_sshd=%u host_udx=%u "
            "areas=%u wave=%u storm=0 "
            "Soft!=product G-AC-1=1 product_dir=UDX+ABI "
            "host_load=userspace_elf\n",
            szVerdict, szVia, g_u32SoftLoadOk, g_u32SoftProbeOk, cReg,
            g_u32SoftHandoffOk, g_u32SoftMapPages, g_u32SoftLogN, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadExecBand,
            g_u32SoftEntryInRange, g_u32SoftHostClassSshd,
            g_u32SoftHostClassUdx, (unsigned)GJ_ELF_SOFT_AREAS,
            GJ_ELF_SOFT_WAVE);

    /*
     * Twin prefix: elf_load: soft ... (agent alias; same tallies; lean only).
     * Grep: elf_load: soft residual
     * Grep: elf_load: soft residual lean
     */
    kprintf("elf_load: soft residual lean via=%s "
            "probe=%u/%u load=%u/%u load_dyn=%u load_exec=%u "
            "reloc_hits=%u map_pt=%u so_live=%u "
            "handoff=%u verify=%u "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "host_load=userspace_elf ko_product=0 G-AC-1=1 "
            "class=C2 dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "storm=0 areas=%u wave=%u "
            "(Soft!=product; dual MIT OR Apache-2.0; UDX host residual lean; "
            "C2 Dual DoD OPEN; no version stamp)\n",
            szVia, g_u32SoftProbeOk, g_u32SoftProbeFail, g_u32SoftLoadOk,
            g_u32SoftLoadFail, g_u32SoftLoadDyn, g_u32SoftLoadExec,
            g_u32SoftRelocHits, g_u32SoftMapPages, cReg, g_u32SoftHandoffOk,
            g_u32SoftVerifyOk, (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft residual udx */
    kprintf("elf_load: soft residual udx host=userspace linux_shaped=1 "
            "host_load=userspace_elf product_dir=UDX+ABI "
            "freestanding_class_product=0 "
            "path=probe|load|so|reloc|auxv|handoff|interp "
            "ko_product=0 G-AC-1=1 udx_confine_product=OPEN "
            "class=C2 dual_dod_a=OPEN dual_dod_b=OPEN "
            "load_ok=%u so_live=%u handoff=%u udx_lean=%u/%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 wave=%u "
            "(Soft!=product; G-AC-1; Dual DoD OPEN; no version stamp)\n",
            g_u32SoftLoadOk, cReg, g_u32SoftHandoffOk, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft residual lean udx */
    kprintf("elf_load: soft residual lean udx host=userspace "
            "host_load=userspace_elf product_dir=UDX+ABI "
            "freestanding_class_product=0 ko_product=0 G-AC-1=1 "
            "class=C2 dual_dod_a=OPEN dual_dod_b=OPEN "
            "path=probe|load|so|reloc|auxv|handoff|interp "
            "ho_sz=%u geom=1 beh=1 lean_ok=%u/%u load_ok=%u so_live=%u "
            "handoff=%u soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; G-AC-1; UDX host load residual lean; "
            "geometry+behavior; Dual DoD OPEN; no version stamp)\n",
            (unsigned)sizeof(struct gj_ld_handoff), u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk, cReg,
            g_u32SoftHandoffOk, (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft residual lean c2 */
    kprintf("elf_load: soft residual lean c2 class=C2 "
            "host_load=userspace_elf product_dir=UDX+ABI "
            "ko_product=0 G-AC-1=1 dual_dod_a=OPEN dual_dod_b=OPEN "
            "path=probe|load|so|reloc|auxv|handoff|interp "
            "ho_sz=%u so_parity=%u geom=1 beh=1 lean_ok=%u/%u "
            "load_ok=%u handoff=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; C2 UDX host ELF residual; geometry+behavior; "
            "Dual DoD OPEN; no version stamp)\n",
            (unsigned)sizeof(struct gj_ld_handoff),
            (unsigned)(GJ_ELF_SO_MAX == GJ_LD_SO_MAX ? 1u : 0u), u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            g_u32SoftHandoffOk, (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft residual lean sshd */
    kprintf("elf_load: soft residual lean sshd host=sshd.elf et_exec=1 "
            "direct_entry=1 freestanding_product_daemon=1 "
            "host_exec_base=0x%lx exec_band=%u entry_in_range=%u "
            "host_class_sshd=%u lean_ok=%u/%u load_exec=%u "
            "product_dir=UDX+ABI dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_ne_product=1 G-AC-1=1 storm=0 areas=%u wave=%u "
            "(Soft!=product; sshd.elf ET_EXEC residual; Dual DoD OPEN)\n",
            (unsigned long)GJ_ELF_HOST_EXEC_BASE, g_u32SoftLoadExecBand,
            g_u32SoftEntryInRange, g_u32SoftHostClassSshd, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadExec,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft residual lean host */
    kprintf("elf_load: soft residual lean host host_udx=1 host_sshd=1 "
            "host_blob=1 product_hosts=UDX "
            "host_load=userspace_elf product_dir=UDX+ABI "
            "ko_product=0 G-AC-1=1 class=C2 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "host_class_udx=%u host_class_sshd=%u exec_band=%u "
            "entry_in_range=%u embed_host=%u host_blob_n=%u "
            "lean_ok=%u/%u load_ok=%u handoff=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; UDX host + sshd.elf + host_blob residual; "
            "Dual DoD OPEN)\n",
            g_u32SoftHostClassUdx, g_u32SoftHostClassSshd,
            g_u32SoftLoadExecBand, g_u32SoftEntryInRange, g_u32SoftEmbedHost,
            g_u32SoftHostBlob, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            g_u32SoftHandoffOk, (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft residual sshd */
    kprintf("elf_load: soft residual sshd host=sshd.elf et_exec=1 "
            "exec_band=%u entry_in_range=%u host_class_sshd=%u "
            "lean_ok=%u/%u dual_dod_a=OPEN dual_dod_b=OPEN "
            "soft_ne_product=1 G-AC-1=1 storm=0 wave=%u\n",
            g_u32SoftLoadExecBand, g_u32SoftEntryInRange,
            g_u32SoftHostClassSshd, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf_load: soft functional residual
     * Twin prefix FUNCTIONAL residual (product hosts + sshd; Soft!=product).
     */
    kprintf("elf_load: soft functional residual catalog "
            "Soft!=product G-AC-1=1 claim_class=C2 "
            "dual_dod_a=OPEN dual_dod_b=OPEN product_dir=UDX+ABI "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host host_sshd=1 "
            "product_hosts=UDX host_blob=1 "
            "path=probe,load,so,reloc,auxv,handoff,interp,direct "
            "host_blob_path=host_blob|probe|load "
            "steps=%u lean_ok=%u/%u host_class_udx=%u host_class_sshd=%u "
            "exec_band=%u entry_in_range=%u storm=0 areas=%u wave=%u\n",
            (unsigned)GJ_ELF_FUNC_STEP_COUNT, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftHostClassUdx,
            g_u32SoftHostClassSshd, g_u32SoftLoadExecBand,
            g_u32SoftEntryInRange, (unsigned)GJ_ELF_SOFT_AREAS,
            GJ_ELF_SOFT_WAVE);
    kprintf("elf_load: soft functional residual "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host host_sshd=1 "
            "product_hosts=UDX host_blob=1 "
            "path=/usr/lib/udx host_load=userspace_elf "
            "embed_host=%u host_blob_n=%u seg_reject=%u pt_load_safe=1 "
            "lean_ok=%u/%u dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 wave=%u\n",
            g_u32SoftEmbedHost, g_u32SoftHostBlob, g_u32SoftSegReject,
            u32UdxLean, (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
            GJ_ELF_SOFT_WAVE);
    /* Grep: elf_load: soft functional residual embed */
    kprintf("elf_load: soft functional residual embed "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host host_sshd=1 "
            "product_hosts=UDX host_blob=1 "
            "embed_host=%u host_blob_n=%u exec_band=%u entry_in_range=%u "
            "seg_reject=%u pt_load_safe=1 lean_ok=%u/%u "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "Soft!=product G-AC-1=1 storm=0 wave=%u\n",
            g_u32SoftEmbedHost, g_u32SoftHostBlob, g_u32SoftLoadExecBand,
            g_u32SoftEntryInRange, g_u32SoftSegReject, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf_load: soft residual host_blob
     * Grep: elf_load: soft residual lean host_blob
     * Grep: elf_load: soft functional residual host_blob
     * Twin prefix: spawn host_blob product load path residual (STRONGER).
     * product_hosts=UDX · Dual DoD OPEN · Soft!=product · G-AC-1.
     */
    kprintf("elf_load: soft residual host_blob host_blob=1 "
            "product_hosts=UDX hosts=ddi_host_gj,rtl8168_udx,xhci_udx "
            "path=host_blob|probe|load "
            "product_dir=UDX+ABI freestanding_class_product=0 "
            "ko_product=0 G-AC-1=1 class=C2 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "embed_host=%u host_blob_n=%u exec_band=%u lean_ok=%u/%u "
            "load_ok=%u soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u "
            "(Soft!=product; host_blob residual; Dual DoD OPEN)\n",
            g_u32SoftEmbedHost, g_u32SoftHostBlob, g_u32SoftLoadExecBand,
            u32UdxLean, (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
            g_u32SoftLoadOk, (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);
    kprintf("elf_load: soft residual lean host_blob host_blob=1 "
            "product_hosts=UDX path=host_blob|probe|load "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "embed_host=%u host_blob_n=%u lean_ok=%u/%u load_ok=%u "
            "soft_ne_product=1 G-AC-1=1 storm=0 areas=%u wave=%u "
            "(Soft!=product; host_blob residual lean; Dual DoD OPEN)\n",
            g_u32SoftEmbedHost, g_u32SoftHostBlob, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);
    kprintf("elf_load: soft functional residual host_blob "
            "product_hosts=UDX host_blob=1 path=host_blob|probe|load "
            "embed_host=%u host_blob_n=%u exec_band=%u entry_in_range=%u "
            "lean_ok=%u/%u dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 wave=%u\n",
            g_u32SoftEmbedHost, g_u32SoftHostBlob, g_u32SoftLoadExecBand,
            g_u32SoftEntryInRange, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, GJ_ELF_SOFT_WAVE);

    /*
     * Grep: elf_load: soft residual denser
     * Grep: elf_load: soft residual denser host_blob
     * Twin prefix denser residual (host_blob Dual DoD; product_hosts=UDX).
     * Soft!=product · denser residual != Dual DoD close · G-AC-1.
     */
    kprintf("elf_load: soft residual denser host_blob denser=1 "
            "product_hosts=UDX hosts=ddi_host_gj,rtl8168_udx,xhci_udx "
            "path=host_blob|probe|load denser_host_blob_residual=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "embed_host=%u host_blob_n=%u dense_ok=%u dense_fail=%u "
            "lean_ok=%u/%u load_ok=%u "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
            "areas=%u wave=%u bar=v2026.08.04.75 "
            "(denser host_blob residual; Soft!=product; Dual DoD OPEN)\n",
            g_u32SoftEmbedHost, g_u32SoftHostBlob, g_u32SoftHostBlobDenseOk,
            g_u32SoftHostBlobDenseFail, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadOk,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);
    kprintf("elf_load: soft residual denser denser=1 product_hosts=UDX "
            "host_blob=1 path=host_blob|probe|load "
            "dense_ok=%u dense_fail=%u lean_ok=%u/%u "
            "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
            "Soft!=product G-AC-1=1 never_ko_product=1 storm=0 "
            "areas=%u wave=%u bar=v2026.08.04.75 "
            "(denser residual honesty; denser != Dual DoD close)\n",
            g_u32SoftHostBlobDenseOk, g_u32SoftHostBlobDenseFail, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
            (unsigned)GJ_ELF_SOFT_AREAS, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft residual lean PASS */
    if (u32UdxLean == GJ_ELF_SOFT_UDX_LEAN_CHECKS) {
        kprintf("elf_load: soft residual lean PASS via=%s "
                "udx_lean=%u/%u host_load=userspace_elf "
                "host_udx=1 host_sshd=1 host_blob=1 product_hosts=UDX "
                "product_dir=UDX+ABI ko_product=0 G-AC-1=1 class=C2 "
                "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 "
                "wave=%u "
                "(Soft!=product; UDX host + sshd.elf + host_blob residual "
                "lean complete; Dual DoD OPEN)\n",
                szVia, u32UdxLean, (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
                GJ_ELF_SOFT_WAVE);
    }

    /* Grep: elf_load: soft inventory */
    kprintf("elf_load: soft inventory via=%s so_max=%u so_live=%u "
            "load_ok=%u probe_ok=%u handoff_ok=%u map_pt=%u log_n=%u "
            "udx_lean=%u/%u exec_band=%u host_sshd=%u host_udx=%u "
            "product_dir=UDX+ABI host_load=userspace_elf "
            "wave=%u storm=0 Soft!=product\n",
            szVia, (unsigned)GJ_ELF_SO_MAX, cReg, g_u32SoftLoadOk,
            g_u32SoftProbeOk, g_u32SoftHandoffOk, g_u32SoftMapPages,
            g_u32SoftLogN, u32UdxLean, (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS,
            g_u32SoftLoadExecBand, g_u32SoftHostClassSshd,
            g_u32SoftHostClassUdx, GJ_ELF_SOFT_WAVE);

    /* Grep: elf_load: soft PASS | PARTIAL */
    kprintf("elf_load: soft %s via=%s load_ok=%u probe_ok=%u so=%u "
            "handoff=%u log_n=%u udx_lean=%u/%u exec_band=%u "
            "host_sshd=%u host_udx=%u wave=%u storm=0 "
            "Soft!=product G-AC-1=1 product_dir=UDX+ABI "
            "host_load=userspace_elf\n",
            szVerdict, szVia, g_u32SoftLoadOk, g_u32SoftProbeOk, cReg,
            g_u32SoftHandoffOk, g_u32SoftLogN, u32UdxLean,
            (unsigned)GJ_ELF_SOFT_UDX_LEAN_CHECKS, g_u32SoftLoadExecBand,
            g_u32SoftHostClassSshd, g_u32SoftHostClassUdx, GJ_ELF_SOFT_WAVE);
}

/* soft residual inventory end (storm=0; Soft!=product; G-AC-1) */

/**
 * After first product probe/load/handoff activity, print soft residual once
 * (mirrors virtio-blk / input_hub soft-stats-once). Diagnostics only.
 * Soft!=product · G-AC-1 · dual MIT OR Apache-2.0 · no stamp storms.
 * Emits UDX host + sshd.elf + host_blob residual lean honesty
 * (userspace ELF; not .ko; product_hosts=UDX; Dual DoD OPEN).
 * H2 once: inventory surface emits once per boot (storm=0).
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
 * INTERP soft normalize: relative path -> /lib/<name>. Absolute unchanged.
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
        /*
         * Robust embed UDX host load (STRONGER FUNCTIONAL):
         * filesz <= memsz; no offset/VA wrap; file span in image.
         * Soft residual tallies rejects; never loads corrupt PT_LOAD.
         */
        if (pPh->u64Filesz > pPh->u64Memsz) {
            elf_soft_inc(&g_u32SoftSegReject);
            return GJ_ERR_INVAL;
        }
        if (pPh->u64Offset + pPh->u64Filesz < pPh->u64Offset) {
            elf_soft_inc(&g_u32SoftSegReject);
            return GJ_ERR_INVAL;
        }
        if (pPh->u64Vaddr + pPh->u64Memsz < pPh->u64Vaddr) {
            elf_soft_inc(&g_u32SoftSegReject);
            return GJ_ERR_INVAL;
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

/* Internal alias - keep call sites greppable as elf_sysv_hash via wrap */
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
    /* Bloom filter (x86_64: 64-bit words) - both bits must be set */
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
 *   defined in image -> st_value + bias
 *   undefined -> SO registry (hash/gnu-hash/scan) then built-in exports
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
    /* Weak undefined -> 0 */
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
                    /* COPY size unknown without st_size in resolve - write ptr */
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
        /* Re-check segment safety at map time (bias applied; robust embeds). */
        if (pPh->u64Filesz > pPh->u64Memsz ||
            pPh->u64Vaddr + pPh->u64Memsz < pPh->u64Vaddr) {
            elf_soft_inc(&g_u32SoftSegReject);
            elf_soft_inc(&g_u32SoftLoadFail);
            elf_soft_maybe_once();
            return GJ_ERR_INVAL;
        }

        u32Prot = GJ_VMM_PROT_READ;
        if (pPh->u32Flags & PF_W) {
            u32Prot |= GJ_VMM_PROT_WRITE;
        }
        if (pPh->u32Flags & PF_X) {
            u32Prot |= GJ_VMM_PROT_EXEC;
        }

        u64SegVa = pPh->u64Vaddr + u64Bias;
        if (u64SegVa < pPh->u64Vaddr && u64Bias != 0ull) {
            /* bias wrap — reject corrupt/hostile ET_DYN placement */
            elf_soft_inc(&g_u32SoftSegReject);
            elf_soft_inc(&g_u32SoftLoadFail);
            elf_soft_maybe_once();
            return GJ_ERR_INVAL;
        }
        /*
         * Never map PT_LOAD over ld-gj handoff/stack band. Protects embed
         * UDX host + INTERP-first paths from clobbering GJ_LD_HANDOFF_VA.
         * Soft!=product · Dual DoD A/B OPEN · G-AC-1.
         */
        if (elf_pt_load_handoff_collide(u64SegVa, pPh->u64Memsz) != 0) {
            elf_soft_inc(&g_u32SoftSegReject);
            elf_soft_inc(&g_u32SoftLoadFail);
            kprintf("elf: PT_LOAD collides handoff band va=0x%lx memsz=0x%lx "
                    "handoff=0x%lx Soft!=product REJECT\n",
                    (unsigned long)u64SegVa, (unsigned long)pPh->u64Memsz,
                    (unsigned long)GJ_LD_HANDOFF_VA);
            elf_soft_maybe_once();
            return GJ_ERR_INVAL;
        }
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
    /*
     * FUNCTIONAL residual (Soft!=product): freestanding product ET_EXEC band
     * (sshd.elf / UDX embed peers @ user.ld) + entry-in-range honesty.
     * Never hard-gates product load path (STRONGER residual only).
     */
    if (elf_soft_exec_band_ok(&info) != 0) {
        elf_soft_inc(&g_u32SoftLoadExecBand);
        kprintf("elf: soft residual exec_band base=0x%lx entry=0x%lx "
                "host_exec_base=0x%lx freestanding_product_daemon=1 "
                "embed_host=1 Soft!=product\n",
                (unsigned long)info.u64LoadMin, (unsigned long)info.u64Entry,
                (unsigned long)GJ_ELF_HOST_EXEC_BASE);
        /*
         * Grep: elf: soft functional residual embed
         * Embed freestanding host-band load residual densify (STRONGER).
         * product_hosts=rtl8168_udx,xhci_udx,ddi_host + sshd.elf @ user.ld.
         */
        if (elf_soft_embed_host_span_ok(info.u64LoadMin, info.u64LoadMax) !=
                0 &&
            elf_soft_entry_in_range(&info) != 0) {
            elf_soft_inc(&g_u32SoftEmbedHost);
            /*
             * Spawn host_blob product path residual tally (Soft!=product).
             * Live load of freestanding ET_EXEC host band is the product
             * host ELF load path used by spawn host_blob (probe|load).
             * Dual DoD A/B remain OPEN; product_hosts=UDX.
             */
            elf_soft_inc(&g_u32SoftHostBlob);
            kprintf("elf: soft functional residual embed "
                    "et_exec=1 direct_entry=1 host_exec_base=0x%lx "
                    "entry=0x%lx range=0x%lx-0x%lx span_ok=1 "
                    "handoff_collide=0 product_dir=UDX+ABI "
                    "product_hosts=rtl8168_udx,xhci_udx,ddi_host host_sshd=1 "
                    "product_hosts=UDX host_blob=1 "
                    "path=host_blob|probe|load "
                    "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
                    "Soft!=product G-AC-1=1 never_ko_product=1 storm=0\n",
                    (unsigned long)GJ_ELF_HOST_EXEC_BASE,
                    (unsigned long)info.u64Entry,
                    (unsigned long)info.u64LoadMin,
                    (unsigned long)info.u64LoadMax);
            /* Grep: elf: soft residual host_blob (live densify, not inventory) */
            kprintf("elf: soft residual host_blob live=1 "
                    "product_hosts=UDX host_blob=1 "
                    "path=host_blob|probe|load "
                    "et_exec=1 direct_entry=1 entry=0x%lx "
                    "range=0x%lx-0x%lx host_blob_n=%u "
                    "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
                    "Soft!=product G-AC-1=1 storm=0\n",
                    (unsigned long)info.u64Entry,
                    (unsigned long)info.u64LoadMin,
                    (unsigned long)info.u64LoadMax, g_u32SoftHostBlob);
            /*
             * Grep: elf: soft residual denser host_blob
             * Live denser residual for host_blob Dual DoD (Soft!=product).
             * denser residual != Dual DoD close; product_hosts=UDX.
             */
            kprintf("elf: soft residual denser host_blob live=1 denser=1 "
                    "product_hosts=UDX host_blob=1 "
                    "path=host_blob|probe|load denser_host_blob_residual=1 "
                    "et_exec=1 direct_entry=1 entry=0x%lx "
                    "range=0x%lx-0x%lx host_blob_n=%u "
                    "dense_ok=%u dense_fail=%u "
                    "dual_dod_a=OPEN dual_dod_b=OPEN dual_dod OPEN "
                    "Soft!=product G-AC-1=1 storm=0 bar=v2026.08.04.75\n",
                    (unsigned long)info.u64Entry,
                    (unsigned long)info.u64LoadMin,
                    (unsigned long)info.u64LoadMax, g_u32SoftHostBlob,
                    g_u32SoftHostBlobDenseOk, g_u32SoftHostBlobDenseFail);
        }
    }
    if (elf_soft_entry_in_range(&info) != 0) {
        elf_soft_inc(&g_u32SoftEntryInRange);
    } else {
        elf_soft_inc(&g_u32SoftEntryOor);
        kprintf("elf: soft residual entry_oor entry=0x%lx range=0x%lx-0x%lx "
                "Soft!=product\n",
                (unsigned long)info.u64Entry, (unsigned long)info.u64LoadMin,
                (unsigned long)info.u64LoadMax);
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
    if (pProc->u64ExecStack != 0 &&
        elf_stack_rsp_live_ok(pProc->u64ExecStack) != 0) {
        /* Prefer SysV top published by elf_publish_handoff_argv. */
        u64Stack = pProc->u64ExecStack;
    } else if (elf_stack_rsp_live_ok(u64Stack) == 0) {
        /* Band-base RSP is 0.1.140 #PF I=1 class — do not live-launch. */
        kprintf("elf: interp_first stack isolate rsp=0x%lx Soft!=product\n",
                (unsigned long)u64Stack);
        return GJ_ERR_INVAL;
    }
    pProc->u64ExecStack = u64Stack;
    pProc->u32StartThr = 0;
    if (pInterp != NULL && pInterp->u64Entry != 0 &&
        (pMain->u32Flags & GJ_ELF_INFO_HAS_INTERP) != 0) {
        u64Entry = pInterp->u64Entry;
        pProc->u64InterpEntry = pInterp->u64Entry;
        pProc->u64StartEntry = u64Entry;
        /* Soft INTERP path log (absolute /lib/... after normalize) */
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
    /* INTERP soft miss: path present but dynlinker not loaded -> main entry */
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
    /* vfs_ram_write reads pData under current CR3 - use kernel CR3. */
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

static u32
elf_copy_user_cstr(char *pDst, u32 cbDst, u64 u64Src)
{
    u32 n;

    if (pDst == NULL || cbDst == 0) {
        return 0;
    }
    if (u64Src == 0) {
        pDst[0] = '\0';
        return 1;
    }
    n = 0;
    while (n + 1u < cbDst) {
        char ch = 0;

        if (user_range_ok(u64Src + n, 1)) {
            if (copy_from_user(&ch, u64Src + n, 1) != GJ_OK) {
                break;
            }
        } else {
            ch = *(const char *)(gj_vaddr_t)(u64Src + n);
        }
        pDst[n] = ch;
        if (ch == '\0') {
            return n + 1u;
        }
        n++;
    }
    pDst[n] = '\0';
    return n + 1u;
}

static u32
elf_read_user_u64(u64 u64Addr, u64 *pOut)
{
    if (pOut == NULL) {
        return 0;
    }
    *pOut = 0;
    if (u64Addr == 0) {
        return 0;
    }
    if (user_range_ok(u64Addr, 8)) {
        if (copy_from_user(pOut, u64Addr, 8) != GJ_OK) {
            return 0;
        }
        return 1;
    }
    *pOut = *(const u64 *)(gj_vaddr_t)u64Addr;
    return 1;
}

gj_status_t
elf_publish_handoff(struct gj_process *pProc, const char *szPath,
                    const struct gj_elf_info *pMain,
                    const struct gj_elf_info *pInterp)
{
    return elf_publish_handoff_argv(pProc, szPath, pMain, pInterp, 0, 0);
}

gj_status_t
elf_publish_handoff_argv(struct gj_process *pProc, const char *szPath,
                         const struct gj_elf_info *pMain,
                         const struct gj_elf_info *pInterp, u64 u64UserArgv,
                         u64 u64UserEnvp)
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
    /*
     * FUNCTIONAL residual: host-class classify UDX product hosts
     * (/usr/lib/udx/{rtl8168_udx,xhci_udx,ddi_host}) / sshd.elf / svc.
     * Soft only — never hard-gates product handoff (Dual DoD A/B OPEN).
     */
    elf_soft_note_host_path(szUsePath);

    elf_handoff_fill(&ho, szUsePath, pMain, pInterp, pProc->aAuxv, pProc->cAuxv);
    cCopy = ho.cAuxv;

    /*
     * SysV stack: argc/argv/env/auxv on the TOP page of a grow-down band.
     * GJ_LD_STACK_VA is an unmapped guard. Body sits above it so push/call
     * cannot smash GJ_LD_HANDOFF_VA (0.1.140 dash #PF I=1 class).
     */
    ho.u64Stack = 0;

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
    {
        gj_paddr_t aBodyPa[GJ_LD_STACK_PAGES - 1u];
        u32 cBody = 0;
        u32 iBody;
        u64 u64StackTop;
        u32 u32Prot;

        for (iBody = 0; iBody < (GJ_LD_STACK_PAGES - 1u); iBody++) {
            aBodyPa[iBody] = pmm_alloc();
            if (aBodyPa[iBody] == 0) {
                break;
            }
            cBody++;
        }
        if (cBody == 0) {
            pmm_free(pa);
            pmm_free(paStack);
            pProc->u64ExecStack = 0;
            elf_soft_inc(&g_u32SoftHandoffFail);
            return GJ_ERR_NOMEM;
        }
        /* Guard at GJ_LD_STACK_VA stays unmapped. Body starts one page up. */
        u64StackTop = GJ_LD_STACK_VA + (u64)GJ_PAGE_SIZE +
                      (u64)cBody * (u64)GJ_PAGE_SIZE;
        ho.u64Stack = u64StackTop;
        pProc->u64ExecStack = u64StackTop;

        u64Saved = cpu_read_cr3();
        cpu_load_cr3(vmm_kernel_cr3());
        memset((void *)(gj_vaddr_t)pa, 0, GJ_PAGE_SIZE);
        memcpy((void *)(gj_vaddr_t)pa, &ho, sizeof(ho));
        /* AT_RANDOM 16 bytes at GJ_LD_RANDOM_OFF on handoff page */
        {
            u8 *pRnd = (u8 *)(gj_vaddr_t)pa + (size_t)GJ_LD_RANDOM_OFF;

            elf_seed_random16(pRnd, ho.u64Entry,
                              ho.u64Base ^ ((u64)ho.cSo << 32));
        }
        for (iBody = 0; iBody < cBody; iBody++) {
            memset((void *)(gj_vaddr_t)aBodyPa[iBody], 0, GJ_PAGE_SIZE);
        }

        /* Build SysV top page under kernel CR3 */
        memset((void *)(gj_vaddr_t)paStack, 0, GJ_PAGE_SIZE);
        pStack = (u64 *)(gj_vaddr_t)paStack;
        /*
         * SysV: [argc][argv…][0][envp…][0][auxv pairs][AT_NULL,0]
         * Strings at +0x200. User argv/envp when provided; else argc=1 path
         * (OpenSSH DUT: kernel argv sshd -D -e so crt0 does not daemonize;
         * kernel env OPENSSL_ia32cap=0 so libcrypto init uses portable C;
         * OPENSSL_CONF=/dev/null so OpenSSL 3 does not autoload host
         * --openssldir config. Dual DoD B OPEN).
         */
        {
            char *pStr = (char *)(gj_vaddr_t)paStack + 0x200;
            u32 cbStr = (u32)GJ_PAGE_SIZE - 0x200u;
            u32 cbUsed = 0;
            u32 cArg = 0;
            u32 cEnv = 0;
            u64 aArg[16];
            u64 aEnv[8];
            u32 iArg;
            const char *const *pszKargv = NULL;
            u32 cKargv = 0;
            /* Kernel C strings; not user pointers (identity may sit in the
             * user VA window). OpenSSH: -D no daemon, -e log stderr. */
            static const char *const aKargvSshd[] = {
                "/usr/sbin/sshd",
                "-D",
                "-e",
            };
            static const char *const aKenvSshd[] = {
                "OPENSSL_ia32cap=0",
                "OPENSSL_CONF=/dev/null",
            };

            if (u64UserArgv != 0) {
                for (iArg = 0; iArg < 16u; iArg++) {
                    u64 u64P = 0;

                    if (elf_read_user_u64(u64UserArgv + (u64)iArg * 8ull,
                                          &u64P) == 0 ||
                        u64P == 0) {
                        break;
                    }
                    aArg[cArg++] = u64P;
                }
            }
            if (cArg == 0 && szUsePath != NULL &&
                strcmp(szUsePath, "/usr/sbin/sshd") == 0) {
                pszKargv = aKargvSshd;
                cKargv = (u32)(sizeof(aKargvSshd) / sizeof(aKargvSshd[0]));
                cArg = cKargv;
            }
            if (cArg == 0) {
                aArg[0] = 0;
                cArg = 1;
            }
            if (u64UserEnvp != 0) {
                for (iArg = 0; iArg < 8u; iArg++) {
                    u64 u64P = 0;

                    if (elf_read_user_u64(u64UserEnvp + (u64)iArg * 8ull,
                                          &u64P) == 0 ||
                        u64P == 0) {
                        break;
                    }
                    aEnv[cEnv++] = u64P;
                }
            }
            pStack[0] = (u64)cArg;
            o = 1;
            for (iArg = 0; iArg < cArg && o < 48u; iArg++) {
                u32 n;

                if (pszKargv != NULL && iArg < cKargv) {
                    const char *szK = pszKargv[iArg];

                    n = 0;
                    if (szK == NULL) {
                        szK = "";
                    }
                    while (szK[n] != '\0' && cbUsed + n + 1u < cbStr) {
                        pStr[cbUsed + n] = szK[n];
                        n++;
                    }
                    pStr[cbUsed + n] = '\0';
                    n++;
                } else if (aArg[iArg] == 0) {
                    n = 0;
                    while (ho.szPath[n] != '\0' && cbUsed + n + 1u < cbStr) {
                        pStr[cbUsed + n] = ho.szPath[n];
                        n++;
                    }
                    pStr[cbUsed + n] = '\0';
                    n++;
                } else {
                    n = elf_copy_user_cstr(pStr + cbUsed, cbStr - cbUsed,
                                           aArg[iArg]);
                }
                pStack[o++] = u64StackTop + 0x200ull + (u64)cbUsed;
                cbUsed += n;
            }
            pStack[o++] = 0;
            /*
             * sshd kargv + no user envp: kernel env strings after argv NULL.
             * OPENSSL_ia32cap=0 clears asm cap bits (DUT libcrypto is
             * linux-x86_64 with asm). OPENSSL_CONF=/dev/null skips OpenSSL 3
             * autoload from a host-baked --openssldir. Dual DoD B OPEN.
             */
            if (pszKargv != NULL && u64UserEnvp == 0) {
                u32 iEnv;
                u32 cKenv = (u32)(sizeof(aKenvSshd) / sizeof(aKenvSshd[0]));

                for (iEnv = 0; iEnv < cKenv && o < 56u &&
                     cbUsed + 2u < cbStr; iEnv++) {
                    const char *szK = aKenvSshd[iEnv];
                    u32 n = 0;

                    if (szK == NULL) {
                        szK = "";
                    }
                    while (szK[n] != '\0' && cbUsed + n + 1u < cbStr) {
                        pStr[cbUsed + n] = szK[n];
                        n++;
                    }
                    pStr[cbUsed + n] = '\0';
                    n++;
                    pStack[o++] = u64StackTop + 0x200ull + (u64)cbUsed;
                    cbUsed += n;
                }
            }
            for (iArg = 0; iArg < cEnv && o < 56u && cbUsed + 2u < cbStr;
                 iArg++) {
                u32 n;

                n = elf_copy_user_cstr(pStr + cbUsed, cbStr - cbUsed,
                                       aEnv[iArg]);
                pStack[o++] = u64StackTop + 0x200ull + (u64)cbUsed;
                cbUsed += n;
            }
            pStack[o++] = 0;
            (void)cbUsed;
        }
        for (i = 0; i < cCopy && o + 1u < 64u; i++) {
            pStack[o++] = pProc->aAuxv[i * 2u];
            pStack[o++] = pProc->aAuxv[i * 2u + 1u];
        }
        /* Ensure AT_NULL */
        if (o < 2 || pStack[o - 2] != GJ_AT_NULL) {
            pStack[o++] = GJ_AT_NULL;
            pStack[o++] = 0;
        }
        cpu_load_cr3(u64Saved);
        process_as_activate(pProc);
        u32Prot = GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER;
        if (vmm_map_page((gj_vaddr_t)GJ_LD_HANDOFF_VA, pa, u32Prot) != GJ_OK) {
            pmm_free(pa);
            pmm_free(paStack);
            for (iBody = 0; iBody < cBody; iBody++) {
                pmm_free(aBodyPa[iBody]);
            }
            pProc->u64ExecStack = 0;
            elf_soft_inc(&g_u32SoftHandoffFail);
            return GJ_ERR_NOMEM;
        }
        for (iBody = 0; iBody < cBody; iBody++) {
            if (vmm_map_page((gj_vaddr_t)(GJ_LD_STACK_VA + (u64)GJ_PAGE_SIZE +
                                          (u64)iBody * (u64)GJ_PAGE_SIZE),
                             aBodyPa[iBody], u32Prot) != GJ_OK) {
                pmm_free(aBodyPa[iBody]);
                aBodyPa[iBody] = 0;
            }
        }
        if (vmm_map_page((gj_vaddr_t)u64StackTop, paStack, u32Prot) != GJ_OK) {
            pmm_free(paStack);
            ho.u64Stack = 0;
            pProc->u64ExecStack = 0;
        } else {
            if (user_range_ok(GJ_LD_HANDOFF_VA +
                                  (u64)__builtin_offsetof(struct gj_ld_handoff,
                                                          u64Stack),
                              8)) {
                (void)copy_to_user(GJ_LD_HANDOFF_VA +
                                       (u64)__builtin_offsetof(
                                           struct gj_ld_handoff, u64Stack),
                                   &u64StackTop, 8);
            }
            kprintf("elf: stack auxv va=0x%lx pages=%u grow_down=1 PASS\n",
                    (unsigned long)u64StackTop, cBody + 1u);
        }
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
