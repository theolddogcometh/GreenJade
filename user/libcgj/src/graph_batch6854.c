/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6854: openat2 RESOLVE_BENEATH resolve flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oat2_resolve_beneath_6854(void);
 *     - Returns 0x00000008 (soft RESOLVE_BENEATH resolve flag bit).
 *       Pure-data catalog stub; not a live openat2 syscall.
 *   uint32_t __gj_oat2_resolve_beneath_6854  (alias)
 *   __libcgj_batch6854_marker = "libcgj-batch6854"
 *
 * Exclusive continuum CREATE-ONLY (6851-6860: openat2 resolve flags
 * stubs). Unique gj_oat2_resolve_beneath_6854 surface only; no
 * multi-def. Distinct from no_symlinks (batch6853) and in_root
 * (batch6855). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6854_marker[] = "libcgj-batch6854";

/* Soft RESOLVE_BENEATH (block ".." escape from dirfd during resolve). */
#define B6854_RESOLVE_BENEATH  0x00000008u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6854_beneath(void)
{
	return B6854_RESOLVE_BENEATH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oat2_resolve_beneath_6854 - soft RESOLVE_BENEATH resolve flag bit.
 *
 * Always returns 0x00000008. Pure-data stub for product openat2 resolve
 * flag catalogs; does not open paths or call openat2. No parent wires.
 */
uint32_t
gj_oat2_resolve_beneath_6854(void)
{
	(void)NULL;
	return b6854_beneath();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oat2_resolve_beneath_6854(void)
    __attribute__((alias("gj_oat2_resolve_beneath_6854")));
