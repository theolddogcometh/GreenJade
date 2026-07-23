/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room ELF64 ET_EXEC / ET_DYN loader + PT_INTERP / PT_DYNAMIC / RELA.
 * Product: probe → load → SO map → relocs → auxv handoff → INTERP-first (ld-gj).
 */
#pragma once

#include <gj/process.h>
#include <gj/types.h>

#define GJ_ELF_INTERP_MAX 128u
#define GJ_ELF_NEEDED_MAX 4u
#define GJ_ELF_NEEDED_LEN 64u
#define GJ_AUXV_MAX       24u

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

/* Linux AT_* (clean-room numbers for auxv handoff) */
#define GJ_AT_NULL    0u
#define GJ_AT_PHDR    3u
#define GJ_AT_PHENT   4u
#define GJ_AT_PHNUM   5u
#define GJ_AT_PAGESZ  6u
#define GJ_AT_BASE    7u
#define GJ_AT_FLAGS   8u
#define GJ_AT_ENTRY   9u
#define GJ_AT_UID     11u
#define GJ_AT_EUID    12u
#define GJ_AT_GID     13u
#define GJ_AT_EGID    14u
#define GJ_AT_SECURE  23u
#define GJ_AT_RANDOM  25u
#define GJ_AT_EXECFN  31u

/*
 * Fixed user VA for dynlinker handoff page (ld-gj reads this after INTERP load).
 * Placed in high user band, away from PE 0x400000 and typical load biases.
 */
#define GJ_LD_HANDOFF_VA    0x000000006ff00000ull
#define GJ_LD_STACK_VA      0x000000006ff01000ull /* argc/argv/env/auxv block */
#define GJ_LD_HANDOFF_MAGIC 0x444c4a47ull /* 'GJLD' little-endian */

#define GJ_LD_SO_MAX 4u

/* One mapped DT_NEEDED object for userspace multi-SO resolve */
struct gj_ld_so_ent {
    u64  u64Bias;       /* load bias (base for ET_DYN) */
    u32  cbImg;         /* file image size (if staged) */
    u32  u32Pad;
    char szName[56];    /* soname e.g. libgj-so.so.1 */
};

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
 */
gj_status_t elf_load_image(struct gj_process *pProc, const void *pImage, u64 cb,
                           struct gj_elf_info *pInfo);

/**
 * Load with explicit bias (0 = auto).
 */
gj_status_t elf_load_image_bias(struct gj_process *pProc, const void *pImage,
                                u64 cb, u64 u64BiasReq,
                                struct gj_elf_info *pInfo);

/**
 * Build auxv-shaped key/value pairs for dynlinker handoff into pProc.
 * cPairs written to pProc->cAuxv; pairs in pProc->aAuxv.
 */
void elf_fill_auxv(struct gj_process *pProc, const struct gj_elf_info *pMain,
                   const struct gj_elf_info *pInterp);

/**
 * Resolve DT_NEEDED names against vfs paths (/lib, /usr/lib).
 * Returns count openable; logs each hit.
 */
u32 elf_resolve_needed_vfs(const struct gj_elf_info *pInfo);

/**
 * Map DT_NEEDED shared objects that are ELF64 into pProc (ET_DYN bias).
 * Returns count successfully loaded. Non-ELF stubs (placeholders) are skipped.
 */
u32 elf_load_needed_sos(struct gj_process *pProc, const struct gj_elf_info *pInfo);

/**
 * INTERP-first: set pProc start entry/stack; rewrite user threads of pProc.
 * u64Stack 0 → GJ_LD_STACK_VA. Returns 0 on success.
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
