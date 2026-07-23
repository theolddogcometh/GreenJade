/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Validate / copy user pointers for Linux hot paths (Security §0.0, G-PTR-*).
 * docs/LINUX_ABI_HYBRID.md
 *
 * Soft deepen: range + present/U/(W|COW) checks, SMAP STAC/CLAC path stats.
 * Pure C freestanding; dual MIT OR Apache-2.0.
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
 */
int user_range_ok(u64 u64Va, u64 u64Cb);

/**
 * Non-zero if every page in range is present with USER (U=1).
 * Supervisor leftovers in the user band fail closed.
 * Equivalent to user_range_mapped_access(..., GJ_USER_ACCESS_READ).
 */
int user_range_mapped(u64 u64Va, u64 u64Cb);

/**
 * Present + USER for every page; if WRITE set, also require W or soft COW.
 * RO leaves without COW fail closed before STAC (avoid #PF on store).
 * u32Access 0 treated as READ.
 */
int user_range_mapped_access(u64 u64Va, u64 u64Cb, u32 u32Access);

/**
 * Copy from user to kernel buffer. Returns GJ_OK or GJ_ERR_FAULT / INVAL.
 * Uses STAC/CLAC when CR4.SMAP is enabled (page-chunk soft path).
 */
gj_status_t copy_from_user(void *pKdst, u64 u64Usrc, size_t cb);

/**
 * Copy from kernel to user. Returns GJ_OK or GJ_ERR_FAULT / INVAL.
 * Write-intent map check (W|COW) before STAC.
 */
gj_status_t copy_to_user(u64 u64Udst, const void *pKsrc, size_t cb);

/** Read one u32 from user; GJ_OK or FAULT/INVAL. */
gj_status_t user_load_u32(u64 u64Uaddr, u32 *pOut);

/** Write one u32 to user (write-intent map check). */
gj_status_t user_store_u32(u64 u64Uaddr, u32 u32Val);

/** Called when CR4.SMAP is enabled so copy paths use STAC/CLAC. */
void user_access_smap_enabled(void);

/** Non-zero if copy paths arm STAC/CLAC (soft query). */
int user_access_smap_is_enabled(void);

/** Snapshot soft counters into *pOut (zeroed if pOut NULL → no-op). */
void user_copy_stats_get(struct gj_user_copy_stats *pOut);

/** Clear soft counters (SMAP-enabled mirror preserved). */
void user_copy_stats_reset(void);
