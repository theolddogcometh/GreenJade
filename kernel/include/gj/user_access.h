/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Validate / copy user pointers for Linux hot paths (Security §0.0, G-PTR-*).
 * docs/LINUX_ABI_HYBRID.md
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

/**
 * Non-zero if [va, va+cb) is entirely in the user VA window.
 * Does not check present bits.
 */
int user_range_ok(u64 u64Va, u64 u64Cb);

/**
 * Non-zero if every page in range is present with USER (U=1).
 * Supervisor leftovers in the user band fail closed.
 */
int user_range_mapped(u64 u64Va, u64 u64Cb);

/**
 * Copy from user to kernel buffer. Returns GJ_OK or GJ_ERR_FAULT.
 * Uses STAC/CLAC when CR4.SMAP is enabled.
 */
gj_status_t copy_from_user(void *pKdst, u64 u64Usrc, size_t cb);

/**
 * Copy from kernel to user. Returns GJ_OK or GJ_ERR_FAULT.
 */
gj_status_t copy_to_user(u64 u64Udst, const void *pKsrc, size_t cb);

/** Read one u32 from user; 0 ok. */
gj_status_t user_load_u32(u64 u64Uaddr, u32 *pOut);

/** Write one u32 to user. */
gj_status_t user_store_u32(u64 u64Uaddr, u32 u32Val);

/** Called when CR4.SMAP is enabled so copy paths use STAC/CLAC. */
void user_access_smap_enabled(void);
