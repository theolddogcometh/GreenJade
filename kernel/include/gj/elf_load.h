/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room ELF64 ET_EXEC / ET_DYN loader + PT_INTERP / PT_DYNAMIC / RELA.
 * Pure C11 freestanding; dual MIT OR Apache-2.0. No binutils/glibc paste.
 *
 * -------------------------------------------------------------------------
 * Role
 * -------------------------------------------------------------------------
 * Kernel product path for Linux-shaped ET_EXEC/ET_DYN images:
 *   probe → load PT_LOAD → SO map (DT_NEEDED) → relocs → auxv handoff
 *   → INTERP-first (ld-gj) when PT_INTERP present.
 *
 * Product pipeline
 * ----------------
 *   elf_probe_image          — validate + collect INTERP/NEEDED (no map)
 *   elf_load_image[_bias]    — map PT_LOAD + relative relocs
 *   elf_resolve_needed_vfs   — soft path openability of DT_NEEDED
 *   elf_load_needed_sos      — map SOs into pProc; fills SO registry
 *   elf_fill_auxv            — PCB auxv pairs for dynlinker
 *   elf_publish_handoff      - map GJ_LD_HANDOFF_VA + seed /proc/self/auxv etc
 *   elf_apply_interp_first   — start entry = INTERP; rewrite user thr
 *   elf_ld_handoff_verify    — smoke: magic/entry/phdr live
 *
 * Fixed handoff VAs (high user band)
 * ----------------------------------
 *   GJ_LD_HANDOFF_VA / STACK / RANDOM — away from PE 0x400000 and typical
 *   ELF load biases so PE and ELF smokes do not collide.
 *
 * greppable: GJ_ELF_INFO_ GJ_LD_HANDOFF GJ_AT_ elf_interp_soft
 * Related: gj/process.h (PCB exec fields), gj/user_task.h, gj/spawn.h,
 *          user/ld-gj (userspace dynlinker consumer of handoff).
 * docs/GLIBC_COMPAT.md · docs/LINUX_ABI_HYBRID.md
 */
#pragma once

#include <gj/process.h>
#include <gj/types.h>

#define GJ_ELF_INTERP_MAX 128u
#define GJ_ELF_NEEDED_MAX 4u
#define GJ_ELF_NEEDED_LEN 64u
#define GJ_AUXV_MAX       24u

/**
 * Probe/load result for one ELF image (main or INTERP or SO).
 * Flags are GJ_ELF_INFO_*; string fields are soft-truncated + NUL.
 */
struct gj_elf_info {
    u64 u64Entry;
    u64 u64LoadMin;
    u64 u64LoadMax;
    u32 u32Phdrs;       /* number of PT_LOAD segments mapped */
    u32 u32Flags;       /* GJ_ELF_INFO_* */
    u64 u64Bias;        /* load bias applied (ET_DYN) */
    u64 u64Phoff;       /* file offset of program headers */
    u64 u64PhdrVa;      /* AT_PHDR: first phdr VA (post-bias) */
    u64 u64DynVa;       /* PT_DYNAMIC VA (post-bias), 0 if none */
    u64 u64Base;        /* load base (bias for DYN; load min for EXEC) */
    u16 u16Phentsize;
    u16 u16Phnum;
    u16 u16Type;        /* ET_EXEC / ET_DYN */
    u16 u16Needed;
    u32 u32SymRelocs;   /* GLOB_DAT/JUMP_SLOT applied */
    char szInterp[GJ_ELF_INTERP_MAX];
    char aNeeded[GJ_ELF_NEEDED_MAX][GJ_ELF_NEEDED_LEN];
};

#define GJ_ELF_INFO_HAS_INTERP  1u
#define GJ_ELF_INFO_IS_DYN      2u
#define GJ_ELF_INFO_LOADED      4u
#define GJ_ELF_INFO_HAS_DYNAMIC 8u
#define GJ_ELF_INFO_RELOC_OK    16u
#define GJ_ELF_INFO_SYM_OK      32u /* GLOB_DAT/JUMP_SLOT applied */
/* INTERP path present and soft-ok (absolute); dynlinker may still be embed */
#define GJ_ELF_INFO_INTERP_SOFT 64u

/* Linux AT_* (clean-room numbers for auxv handoff; public ABI docs only) */
#define GJ_AT_NULL     0u
#define GJ_AT_PHDR     3u
#define GJ_AT_PHENT    4u
#define GJ_AT_PHNUM    5u
#define GJ_AT_PAGESZ   6u
#define GJ_AT_BASE     7u
#define GJ_AT_FLAGS    8u
#define GJ_AT_ENTRY    9u
#define GJ_AT_UID      11u
#define GJ_AT_EUID     12u
#define GJ_AT_GID      13u
#define GJ_AT_EGID     14u
#define GJ_AT_PLATFORM 15u
#define GJ_AT_HWCAP    16u
#define GJ_AT_CLKTCK   17u
#define GJ_AT_SECURE   23u
#define GJ_AT_RANDOM   25u
#define GJ_AT_EXECFN   31u

/*
 * Fixed user VA for dynlinker handoff page (ld-gj reads this after INTERP load).
 * Placed in high user band, away from PE 0x400000 and typical load biases.
 */
#define GJ_LD_HANDOFF_VA    0x000000006ff00000ull
#define GJ_LD_STACK_VA      0x000000006ff01000ull /* argc/argv/env/auxv block */
#define GJ_LD_HANDOFF_MAGIC 0x444c4a47ull /* 'GJLD' little-endian */
/* 16-byte AT_RANDOM blob on handoff page (past sizeof(gj_ld_handoff) ~1016) */
#define GJ_LD_RANDOM_OFF    0x400ull
#define GJ_LD_RANDOM_VA     (GJ_LD_HANDOFF_VA + GJ_LD_RANDOM_OFF)

#define GJ_LD_SO_MAX 4u

/**
 * One mapped DT_NEEDED object for userspace multi-SO resolve.
 * Soft registry also keeps these for elf_so_registry_find.
 */
struct gj_ld_so_ent {
    u64  u64Bias;       /* load bias (base for ET_DYN) */
    u32  cbImg;         /* file image size (if staged) */
    u32  u32NameHash;   /* SysV hash of szName (was pad; ld-gj ignores) */
    char szName[56];    /* basename / soname e.g. libgj-so.so.1 */
};

/**
 * Kernel→ld-gj handoff page (mapped at GJ_LD_HANDOFF_VA).
 * Magic must be GJ_LD_HANDOFF_MAGIC before ld-gj trusts fields.
 * aAuxv is flat key/value pairs (not terminated until AT_NULL pair written).
 */
struct gj_ld_handoff {
    u64  u64Magic;
    u64  u64Entry;      /* AT_ENTRY — main binary */
    u64  u64Interp;     /* interpreter entry (ld-gj) */
    u64  u64Base;       /* AT_BASE — interpreter base */
    u64  u64Phdr;       /* AT_PHDR */
    u64  u64Phent;
    u64  u64Phnum;
    u64  u64Pagesz;
    u64  u64Stack;      /* INTERP-first: VA of argc on stack page */
    u32  cAuxv;
    u32  u32Flags;      /* GJ_ELF_INFO_* mirror from main */
    u32  cSymReloc;     /* GLOB_DAT/JUMP_SLOT applied count */
    u32  cSo;           /* number of aSo[] entries filled */
    u64  aAuxv[GJ_AUXV_MAX * 2u];
    char szPath[128];   /* AT_EXECFN path */
    char szInterp[GJ_ELF_INTERP_MAX];
    struct gj_ld_so_ent aSo[GJ_LD_SO_MAX];
};

/**
 * Probe ELF64 image: validate header, collect PT_LOAD + PT_INTERP + DT_NEEDED.
 * Does not map pages. Returns GJ_OK and fills *pInfo on success.
 */
gj_status_t elf_probe_image(const void *pImage, u64 cb, struct gj_elf_info *pInfo);

/**
 * Parse ELF64 image, map PT_LOAD into pProc AS, apply R_X86_64_RELATIVE if present.
 * Auto bias for ET_DYN. Sets GJ_ELF_INFO_LOADED / RELOC_OK soft flags.
 */
gj_status_t elf_load_image(struct gj_process *pProc, const void *pImage, u64 cb,
                           struct gj_elf_info *pInfo);

/**
 * Load with explicit bias (0 = auto).
 * Non-zero bias is page-aligned request for ET_DYN placement control.
 */
gj_status_t elf_load_image_bias(struct gj_process *pProc, const void *pImage,
                                u64 cb, u64 u64BiasReq,
                                struct gj_elf_info *pInfo);

/**
 * Build auxv-shaped key/value pairs for dynlinker handoff into pProc.
 * cPairs written to pProc->cAuxv; pairs in pProc->aAuxv.
 * pInterp may be NULL when no PT_INTERP (AT_BASE 0).
 */
void elf_fill_auxv(struct gj_process *pProc, const struct gj_elf_info *pMain,
                   const struct gj_elf_info *pInterp);

/**
 * Resolve DT_NEEDED names against vfs paths (/lib, /usr/lib).
 * Returns count openable; logs each hit. Does not map.
 */
u32 elf_resolve_needed_vfs(const struct gj_elf_info *pInfo);

/**
 * Map DT_NEEDED shared objects that are ELF64 into pProc (ET_DYN bias).
 * Returns count successfully loaded. Non-ELF stubs (placeholders) are skipped.
 * Updates soft SO registry for elf_so_registry_*.
 */
u32 elf_load_needed_sos(struct gj_process *pProc, const struct gj_elf_info *pInfo);

/**
 * INTERP-first: set pProc start entry/stack; rewrite user threads of pProc.
 * u64Stack 0 → GJ_LD_STACK_VA. Returns 0 on success.
 * After this, first user thr enters ld-gj, not main.
 */
gj_status_t elf_apply_interp_first(struct gj_process *pProc,
                                   const struct gj_elf_info *pMain,
                                   const struct gj_elf_info *pInterp,
                                   u64 u64Stack);

/**
 * Map GJ_LD_HANDOFF_VA into pProc and fill dynlinker handoff (auxv + paths).
 * Also seeds /proc/self/auxv and /proc/self/execfn on vfs for userspace open.
 */
gj_status_t elf_publish_handoff(struct gj_process *pProc, const char *szPath,
                                const struct gj_elf_info *pMain,
                                const struct gj_elf_info *pInterp);

/**
 * Verify handoff page + AT_ENTRY mapping for ld-gj product path smoke.
 * Returns GJ_OK when magic/entry/phdr look live.
 */
gj_status_t elf_ld_handoff_verify(struct gj_process *pProc);

/* ---- deepen helpers: hash / SO registry / INTERP soft / auxv ---- */

/** SysV ELF hash (DT_HASH). */
u32 elf_sysv_hash_name(const char *szName);

/** GNU hash (DJB; DT_GNU_HASH). */
u32 elf_gnu_hash_name(const char *szName);

/**
 * Look up auxv key in flat key/value pairs. Returns value or 0 if absent.
 * cPairs is number of pairs (not elements).
 */
u64 elf_auxv_get(const u64 *pPairs, u32 cPairs, u64 u64Key);

/**
 * Set or replace auxv key; appends if missing. *pCPairs updated.
 * Returns 1 on success, 0 if full / bad args. Does not leave trailing AT_NULL.
 */
int elf_auxv_set(u64 *pPairs, u32 *pCPairs, u32 cMax, u64 u64Key, u64 u64Val);

/**
 * Append one key/value pair. Returns new pair count (unchanged if full).
 */
u32 elf_auxv_push(u64 *pPairs, u32 cPairs, u32 cMax, u64 u64Key, u64 u64Val);

/**
 * Pack handoff fields (no map). Copies SO registry + auxv pairs into *pHo.
 */
void elf_handoff_fill(struct gj_ld_handoff *pHo, const char *szPath,
                      const struct gj_elf_info *pMain,
                      const struct gj_elf_info *pInterp, const u64 *pAuxv,
                      u32 cAuxv);

/** Number of live SO registry slots after elf_load_needed_sos. */
u32 elf_so_registry_count(void);

/**
 * Find mapped SO by basename or DT_SONAME. Fills *pBias / *pcb when non-NULL.
 * Returns 1 if found.
 */
int elf_so_registry_find(const char *szName, u64 *pBias, u32 *pcb);

/**
 * INTERP soft gate: non-empty absolute path, fits GJ_ELF_INTERP_MAX-1.
 * Relative paths fail (caller may soft-normalize first).
 * Sets/clears GJ_ELF_INFO_INTERP_SOFT in product path after probe.
 */
int elf_interp_soft_ok(const char *szInterp);
