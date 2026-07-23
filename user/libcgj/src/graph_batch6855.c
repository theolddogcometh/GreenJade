/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6855: openat2 RESOLVE_IN_ROOT resolve flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oat2_resolve_in_root_6855(void);
 *     - Returns 0x00000010 (soft RESOLVE_IN_ROOT resolve flag bit).
 *       Pure-data catalog stub; not a live openat2 syscall.
 *   uint32_t __gj_oat2_resolve_in_root_6855  (alias)
 *   __libcgj_batch6855_marker = "libcgj-batch6855"
 *
 * Exclusive continuum CREATE-ONLY (6851-6860: openat2 resolve flags
 * stubs). Unique gj_oat2_resolve_in_root_6855 surface only; no
 * multi-def. Distinct from beneath (batch6854) and cached (batch6856).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6855_marker[] = "libcgj-batch6855";

/* Soft RESOLVE_IN_ROOT (treat dirfd as chroot-like root during resolve). */
#define B6855_RESOLVE_IN_ROOT  0x00000010u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6855_in_root(void)
{
	return B6855_RESOLVE_IN_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oat2_resolve_in_root_6855 - soft RESOLVE_IN_ROOT resolve flag bit.
 *
 * Always returns 0x00000010. Pure-data stub for product openat2 resolve
 * flag catalogs; does not open paths or call openat2. No parent wires.
 */
uint32_t
gj_oat2_resolve_in_root_6855(void)
{
	(void)NULL;
	return b6855_in_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oat2_resolve_in_root_6855(void)
    __attribute__((alias("gj_oat2_resolve_in_root_6855")));
