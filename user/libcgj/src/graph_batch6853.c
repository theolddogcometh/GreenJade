/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6853: openat2 RESOLVE_NO_SYMLINKS resolve flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oat2_resolve_no_symlinks_6853(void);
 *     - Returns 0x00000004 (soft RESOLVE_NO_SYMLINKS resolve flag bit).
 *       Pure-data catalog stub; not a live openat2 syscall.
 *   uint32_t __gj_oat2_resolve_no_symlinks_6853  (alias)
 *   __libcgj_batch6853_marker = "libcgj-batch6853"
 *
 * Exclusive continuum CREATE-ONLY (6851-6860: openat2 resolve flags
 * stubs). Unique gj_oat2_resolve_no_symlinks_6853 surface only; no
 * multi-def. Distinct from no_magiclinks (batch6852) and beneath
 * (batch6854). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6853_marker[] = "libcgj-batch6853";

/* Soft RESOLVE_NO_SYMLINKS (block all symlink traversals during resolve). */
#define B6853_RESOLVE_NO_SYMLINKS  0x00000004u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6853_no_symlinks(void)
{
	return B6853_RESOLVE_NO_SYMLINKS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oat2_resolve_no_symlinks_6853 - soft RESOLVE_NO_SYMLINKS bit.
 *
 * Always returns 0x00000004. Pure-data stub for product openat2 resolve
 * flag catalogs; does not open paths or call openat2. No parent wires.
 */
uint32_t
gj_oat2_resolve_no_symlinks_6853(void)
{
	(void)NULL;
	return b6853_no_symlinks();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oat2_resolve_no_symlinks_6853(void)
    __attribute__((alias("gj_oat2_resolve_no_symlinks_6853")));
