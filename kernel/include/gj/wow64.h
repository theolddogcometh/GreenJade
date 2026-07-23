/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * WoW64 / 32-bit Linux personality soft-thunk (kernel long mode only).
 * Pure C; dual MIT OR Apache-2.0 — clean-room NR map, no Wine/GPL paste.
 *
 * -------------------------------------------------------------------------
 * Role
 * -------------------------------------------------------------------------
 * Kernel stays in long mode. 32-bit Linux-shaped syscalls (int 0x80 CS32 or
 * hybrid soft smoke) are remapped to x86_64 Linux NRs + arg fixups so the
 * existing Option C hybrid path can handle them. Full PE32 loader and true
 * far-return enter are separate (gj/pe32.h); this module is NR/arg thunk only.
 *
 * Entry surfaces
 * --------------
 *   - Hybrid SYSCALL with wow64_enabled() process mode
 *   - trap_dispatch CS32 int 0x80 (uses translate + adjust)
 *   - Pure-C soft smokes (wow64_thunk_soft) without hardware compat CS
 *
 * Soft thunk steps (wow64_thunk_soft)
 * ----------------------------------
 *   1) Map i386 NR → x86_64 NR (identity if unmapped; cold ENOSYS later)
 *   2) Zero-extend six arg slots to low 32 bits
 *   3) Special fixups: mmap2 pgoff pages→bytes; socketcall demux
 *
 * greppable: GJ_WOW64_THUNK_ wow64_thunk_soft wow64_translate_nr
 * Related: gj/pe32.h, gj/linux_abi.h, gj/linux_dispatch.h,
 *          docs/PROTON_PERSONALITY.md · docs/LINUX_ABI_HYBRID.md
 */
#pragma once

#include <gj/types.h>

/* i386 mmap2 page size for pgoff (pages → bytes). Matches GJ_PAGE_SIZE. */
#define GJ_WOW64_PAGE_SHIFT  12u
#define GJ_WOW64_PAGE_SIZE   (1u << GJ_WOW64_PAGE_SHIFT)

/*
 * Soft-thunk flag bits (wow64_thunk_soft / wow64_adjust_args).
 * Combined mask describes what the soft path did for diagnostics / smoke.
 */
#define GJ_WOW64_THUNK_NR_MAPPED     (1u << 0) /* NR remapped (not identity) */
#define GJ_WOW64_THUNK_ARGS_ZX       (1u << 1) /* args zero-extended to 32 */
#define GJ_WOW64_THUNK_MMAP2_PGOFF   (1u << 2) /* mmap2 pgoff pages→bytes */
#define GJ_WOW64_THUNK_SOCKETCALL    (1u << 3) /* socketcall demux applied */
#define GJ_WOW64_THUNK_SOCKET_NEED_PULL (1u << 4) /* args live in user ptr */
#define GJ_WOW64_THUNK_IDENTITY      (1u << 5) /* NR identity pass-through */

/*
 * i386 socketcall subcall ids (public man socketcall; clean-room).
 * Used by wow64_socketcall_nr / adjust_args demux of NR 102.
 */
#define GJ_WOW64_SYS_SOCKET      1u
#define GJ_WOW64_SYS_BIND        2u
#define GJ_WOW64_SYS_CONNECT     3u
#define GJ_WOW64_SYS_LISTEN      4u
#define GJ_WOW64_SYS_ACCEPT      5u
#define GJ_WOW64_SYS_GETSOCKNAME 6u
#define GJ_WOW64_SYS_GETPEERNAME 7u
#define GJ_WOW64_SYS_SOCKETPAIR  8u
#define GJ_WOW64_SYS_SEND        9u
#define GJ_WOW64_SYS_RECV        10u
#define GJ_WOW64_SYS_SENDTO      11u
#define GJ_WOW64_SYS_RECVFROM    12u
#define GJ_WOW64_SYS_SHUTDOWN    13u
#define GJ_WOW64_SYS_SETSOCKOPT  14u
#define GJ_WOW64_SYS_GETSOCKOPT  15u
#define GJ_WOW64_SYS_SENDMSG     16u
#define GJ_WOW64_SYS_RECVMSG     17u
#define GJ_WOW64_SYS_ACCEPT4     18u
#define GJ_WOW64_SYS_RECVMMSG    19u
#define GJ_WOW64_SYS_SENDMMSG    20u

/**
 * Soft-thunk result: remapped NR + up to 6 zero-extended / adjusted args.
 * aArgs must be pre-loaded with raw 32-bit-in-64 entry values before
 * wow64_thunk_soft (or left zero for NR-only probes).
 */
struct gj_wow64_thunk {
    u32 u32Nr32;       /* original i386 NR */
    u32 u32Nr64;       /* mapped x86_64 NR */
    u32 u32Flags;      /* GJ_WOW64_THUNK_* */
    u32 u32SocketCall; /* socketcall subcall if demuxed; else 0 */
    u64 aArgs[6];      /* hybrid arg slots (rdi..r9 shaped) */
};

/**
 * Non-zero when process is in 32-bit Linux personality mode.
 * Soft process-global for bring-up; product may later bind to PCB flags.
 */
int  wow64_enabled(void);
void wow64_set(int fOn);

/**
 * Map i386 Linux syscall NR → x86_64 NR for hybrid dispatch.
 * Returns 0 and writes *pOutNr; -1 if pOutNr is NULL (defensive).
 * Unmapped NRs pass through (identity) so cold path can ENOSYS cleanly.
 * Notable: i386 mmap2 (192) → mmap (9); path NRs (open/openat/stat family);
 * socketcall (102) → socket (41) coarse — prefer wow64_thunk_soft demux.
 * mmap2 page-offset conversion is done by wow64_adjust_args / thunk_soft.
 */
int  wow64_translate_nr(u32 u32Nr32, u32 *pOutNr);

/** 1 if u32Nr32 has an explicit map (not identity); 0 if pass-through. */
int  wow64_nr_is_mapped(u32 u32Nr32);

/**
 * Zero-extend six hybrid arg slots to low 32 bits (i386 register width).
 * No-op if pArgs is NULL. Returns number of slots touched (0 or 6).
 * Does not sign-extend; high garbage from long mode save is cleared.
 */
u32  wow64_args_zero_extend(u64 *pArgs, u32 cArgs);

/**
 * Soft arg fixups after NR map, given original i386 NR.
 * - mmap2 (192): aArgs[5] pgoff pages → byte offset (<< 12)
 * - socketcall (102): demux aArgs[0] → *pOutNr64 + SOCKETCALL flags
 *   (user arg block pull is dispatcher / trap responsibility)
 * pOutNr64 may be NULL if caller already holds mapped NR.
 * Returns GJ_WOW64_THUNK_* flag mask applied (0 if nothing / NULL args).
 */
u32  wow64_adjust_args(u32 u32Nr32, u32 *pOutNr64, u64 *pArgs, u32 cArgs);

/**
 * Full soft thunk: translate NR, zero-extend args, apply mmap2/socketcall.
 * pThunk->aArgs must be pre-loaded with raw 32-bit-in-64 entry values (or 0).
 * On success returns 0; -1 if pThunk is NULL.
 * Soft counters: wow64_thunk_hits / mmap2_hits / socketcall_hits.
 */
int  wow64_thunk_soft(struct gj_wow64_thunk *pThunk);

/**
 * Map i386 socketcall subcall → x86_64 NR.
 * Returns 0 and writes *pOutNr; -1 if unknown call or pOutNr NULL.
 */
int  wow64_socketcall_nr(u32 u32Call, u32 *pOutNr);

/**
 * Convert mmap2 page-offset (pages) to mmap byte offset.
 * u32PgoffPages << GJ_WOW64_PAGE_SHIFT; no overflow check (soft).
 */
u64  wow64_mmap2_pgoff_to_bytes(u32 u32PgoffPages);

/**
 * Truncate a value to a 32-bit user pointer (low 4 GiB).
 * Soft canonicalization for CS32 pointer-shaped args in long mode slots.
 */
u64  wow64_ptr32(u64 u64Val);

/**
 * Counters for smoke / GJ_SYS_PLATFORM_INFO visibility.
 * Wrap OK; diagnostics only.
 */
u32  wow64_calls(void);           /* total translate / thunk entries */
u32  wow64_map_hits(void);        /* explicit NR map (not identity) */
u32  wow64_identity_hits(void);   /* pass-through NR */
u32  wow64_thunk_hits(void);      /* wow64_thunk_soft success */
u32  wow64_mmap2_hits(void);      /* mmap2 pgoff fixups */
u32  wow64_socketcall_hits(void); /* socketcall demux */
