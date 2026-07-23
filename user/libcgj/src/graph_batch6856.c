/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6856: openat2 RESOLVE_CACHED resolve flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oat2_resolve_cached_6856(void);
 *     - Returns 0x00000020 (soft RESOLVE_CACHED resolve flag bit).
 *       Pure-data catalog stub; not a live openat2 syscall.
 *   uint32_t __gj_oat2_resolve_cached_6856  (alias)
 *   __libcgj_batch6856_marker = "libcgj-batch6856"
 *
 * Exclusive continuum CREATE-ONLY (6851-6860: openat2 resolve flags
 * stubs). Unique gj_oat2_resolve_cached_6856 surface only; no
 * multi-def. Distinct from in_root (batch6855) and known_mask
 * (batch6857). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6856_marker[] = "libcgj-batch6856";

/* Soft RESOLVE_CACHED (only resolve if already in dcache; else fail). */
#define B6856_RESOLVE_CACHED  0x00000020u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6856_cached(void)
{
	return B6856_RESOLVE_CACHED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oat2_resolve_cached_6856 - soft RESOLVE_CACHED resolve flag bit.
 *
 * Always returns 0x00000020. Pure-data stub for product openat2 resolve
 * flag catalogs; does not open paths or call openat2. No parent wires.
 */
uint32_t
gj_oat2_resolve_cached_6856(void)
{
	(void)NULL;
	return b6856_cached();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oat2_resolve_cached_6856(void)
    __attribute__((alias("gj_oat2_resolve_cached_6856")));
