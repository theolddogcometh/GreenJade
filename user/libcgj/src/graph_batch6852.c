/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6852: openat2 RESOLVE_NO_MAGICLINKS resolve flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oat2_resolve_no_magiclinks_6852(void);
 *     - Returns 0x00000002 (soft RESOLVE_NO_MAGICLINKS resolve flag bit).
 *       Pure-data catalog stub; not a live openat2 syscall.
 *   uint32_t __gj_oat2_resolve_no_magiclinks_6852  (alias)
 *   __libcgj_batch6852_marker = "libcgj-batch6852"
 *
 * Exclusive continuum CREATE-ONLY (6851-6860: openat2 resolve flags
 * stubs). Unique gj_oat2_resolve_no_magiclinks_6852 surface only; no
 * multi-def. Distinct from no_xdev (batch6851) and no_symlinks
 * (batch6853). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6852_marker[] = "libcgj-batch6852";

/* Soft RESOLVE_NO_MAGICLINKS (block magic-link style path components). */
#define B6852_RESOLVE_NO_MAGICLINKS  0x00000002u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6852_no_magiclinks(void)
{
	return B6852_RESOLVE_NO_MAGICLINKS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oat2_resolve_no_magiclinks_6852 - soft RESOLVE_NO_MAGICLINKS bit.
 *
 * Always returns 0x00000002. Pure-data stub for product openat2 resolve
 * flag catalogs; does not open paths or call openat2. No parent wires.
 */
uint32_t
gj_oat2_resolve_no_magiclinks_6852(void)
{
	(void)NULL;
	return b6852_no_magiclinks();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oat2_resolve_no_magiclinks_6852(void)
    __attribute__((alias("gj_oat2_resolve_no_magiclinks_6852")));
