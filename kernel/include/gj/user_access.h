/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Validate / copy user pointers for Linux hot paths and native syscalls.
 *
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0.
 * Implementation: kernel/mm/user_copy.c (STAC/CLAC when SMAP on).
 *
 * Scope
 * -----
 * Every path that touches user memory under the *current* address space
 * must go through these helpers (G-PTR-1). Fail closed to GJ_ERR_FAULT /
 * GJ_ERR_INVAL — never panic on bad user addresses (G-PTR-3). Product
 * builds must not offer kernel-pointer bypass (G-PTR-2: debug-only).
 *
 * Design anchors
 * --------------
 *   docs/SECURITY_CORE_DESIGN.md   §0.0 / fail closed; SMAP pairing
 *   docs/DESIGN_SPEC_COMPLETE.md   G-PTR-1..3, G-MAP-2 user VA band
 *   docs/LINUX_ABI_HYBRID.md       Linux personality copy edges
 *   docs/X86_64_INTEL_PLATFORM.md  P-MEM-6 SMAP; STAC/CLAC
 *   docs/GLIBC_COMPAT.md           8 MiB user floor vs kernel embeds
 *
 * User VA window (canonical Linux personality)
 * --------------------------------------------
 *   GJ_USER_VA_BASE  0x0000_0000_0080_0000  (8 MiB)
 *   GJ_USER_VA_END   0x0000_0000_8000_0000  (2 GiB exclusive)
 *
 * Floor is 8 MiB: kernel image+BSS (embeds) grow past classic PE 0x400000.
 * Product maps (INTERP/SO/PE) use high bases; low identity stays kernel-only
 * (G-MAP-2 / harden leaves U=0 outside this band — smep.h).
 *
 * Check ladder (copy path)
 * ------------------------
 *   1. user_range_ok          — wholly inside [BASE, END); no present bits
 *   2. user_range_mapped_*    — present + U=1; WRITE ⇒ W or soft COW
 *   3. STAC (if SMAP) → chunked copy → CLAC
 *   4. RO without COW fails at step 2 before STAC (avoid #PF on store)
 *
 * Soft product surface
 * --------------------
 *   USER_RANGE_OK       — window geometry only
 *   USER_RANGE_MAPPED   — present + USER (read intent)
 *   USER_RANGE_ACCESS   — present + USER + optional W|COW
 *   USER_COPY_FROM/TO   — bulk copy with SMAP arming
 *   USER_LOAD/STORE_U32 — scalar helpers
 *   USER_COPY_STATS     — soft counters (never hard-gate boot)
 *   USER_SMAP_NOTIFY    — user_access_smap_enabled from cpu_enable_smap
 *
 * Layering
 * --------
 *   smep.h cpu_enable_smap → user_access_smap_enabled
 *   vmm_read_pte / COW bit for mapped_access WRITE check
 *   linux_hot / native syscall dispatch call copy_* under process CR3
 *
 * greppable: G-PTR USER_RANGE_OK USER_RANGE_MAPPED USER_COPY_STATS
 * greppable: GJ_USER_VA_BASE GJ_USER_VA_END GJ_USER_COPY_MAX SMAP STAC CLAC
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

/*
 * Canonical Linux personality user window.
 * Floor is 8 MiB: kernel image+BSS (embeds) grow past classic PE 0x400000.
 * Product maps (INTERP/SO/PE) use high bases; low identity stays kernel-only.
 */
#define GJ_USER_VA_BASE  0x0000000000800000ull
#define GJ_USER_VA_END   0x0000000080000000ull

/** Soft max bytes for a single copy_* (full user window span). */
#define GJ_USER_COPY_MAX ((u64)(GJ_USER_VA_END - GJ_USER_VA_BASE))

/* Access intent for user_range_mapped_access (bitmask). */
#define GJ_USER_ACCESS_READ  (1u << 0)
#define GJ_USER_ACCESS_WRITE (1u << 1)

/**
 * Soft product counters for copy/SMAP paths (wrap OK; diagnostics only).
 * Smoke / continuum may snapshot via user_copy_stats_get; never hard-gate.
 *
 * From/To/Load/Store *Ok / *Fault / *Inval classify terminal status.
 * u64Stac/u64Clac count arming only when SMAP path is live.
 * u64Chunks is page-chunk iterations under STAC/CLAC.
 */
struct gj_user_copy_stats {
    u64 u64FromOk;
    u64 u64FromFault;
    u64 u64FromInval;
    u64 u64ToOk;
    u64 u64ToFault;
    u64 u64ToInval;
    u64 u64LoadOk;
    u64 u64LoadFault;
    u64 u64LoadInval;
    u64 u64StoreOk;
    u64 u64StoreFault;
    u64 u64BytesFrom;
    u64 u64BytesTo;
    u64 u64RangeOkFail;
    u64 u64RangeMapFail;
    u64 u64Stac;       /* STAC executions (SMAP path only) */
    u64 u64Clac;       /* CLAC executions (SMAP path only) */
    u64 u64SmapOn;     /* 0/1 mirror of enabled flag (soft) */
    u64 u64Chunks;     /* page-chunk copies under STAC/CLAC */
};

/**
 * Non-zero if [va, va+cb) is entirely in the user VA window.
 * Does not check present bits. Empty cb succeeds (G-PTR soft).
 * Overflow (va+cb wrap) fails closed.
 */
int user_range_ok(u64 u64Va, u64 u64Cb);

/**
 * Non-zero if every page in range is present with USER (U=1).
 * Supervisor leftovers in the user band fail closed.
 * Equivalent to user_range_mapped_access(..., GJ_USER_ACCESS_READ).
 * Walks under *active* CR3 (caller must have process AS activated).
 */
int user_range_mapped(u64 u64Va, u64 u64Cb);

/**
 * Present + USER for every page; if WRITE set, also require W or soft COW.
 *
 * RO leaves without COW fail closed before STAC (avoid #PF on store).
 * COW-writable soft: treat software COW bit as write-intent OK; actual
 * break happens on #PF / vmm_cow_break_page, not inside copy_to_user.
 * u32Access 0 treated as READ.
 */
int user_range_mapped_access(u64 u64Va, u64 u64Cb, u32 u32Access);

/**
 * Copy from user to kernel buffer.
 *
 * Returns GJ_OK, GJ_ERR_FAULT (not mapped / bad U), or GJ_ERR_INVAL
 * (null kdst, oversize, outside window). Uses STAC/CLAC when CR4.SMAP
 * is enabled (page-chunk soft path). G-PTR-1.
 */
gj_status_t copy_from_user(void *pKdst, u64 u64Usrc, size_t cb);

/**
 * Copy from kernel to user.
 *
 * Write-intent map check (W|COW) before STAC. Same status codes as
 * copy_from_user. Does not break COW itself — writable COW leaves are
 * accepted so the store can raise #PF into the COW path if needed, or
 * product may break first; fail closed if RO without COW.
 */
gj_status_t copy_to_user(u64 u64Udst, const void *pKsrc, size_t cb);

/**
 * Read one u32 from user; GJ_OK or FAULT/INVAL.
 * Same window + present/U checks as copy_from_user of 4 bytes.
 */
gj_status_t user_load_u32(u64 u64Uaddr, u32 *pOut);

/**
 * Write one u32 to user (write-intent map check).
 * pOut-style: u32Val is the value; u64Uaddr is the user destination.
 */
gj_status_t user_store_u32(u64 u64Uaddr, u32 u32Val);

/**
 * Called when CR4.SMAP is enabled so copy paths use STAC/CLAC.
 * Invoked from cpu_enable_smap (smep.h); safe to call once.
 */
void user_access_smap_enabled(void);

/**
 * Non-zero if copy paths arm STAC/CLAC (soft query).
 * Mirrors enable notify, not a live CR4 re-read on every call.
 */
int user_access_smap_is_enabled(void);

/**
 * Snapshot soft counters into *pOut (no-op if pOut NULL).
 */
void user_copy_stats_get(struct gj_user_copy_stats *pOut);

/**
 * Clear soft counters (SMAP-enabled mirror preserved so u64SmapOn stays
 * consistent with user_access_smap_is_enabled after reset).
 */
void user_copy_stats_reset(void);
