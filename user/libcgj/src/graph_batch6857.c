/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6857: openat2 resolve known-bits mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oat2_resolve_known_mask_6857(void);
 *     - Returns 0x0000003f (soft OR of catalog RESOLVE_* bits:
 *       NO_XDEV|NO_MAGICLINKS|NO_SYMLINKS|BENEATH|IN_ROOT|CACHED).
 *       Pure-data catalog stub; not a live openat2 syscall.
 *   uint32_t __gj_oat2_resolve_known_mask_6857  (alias)
 *   __libcgj_batch6857_marker = "libcgj-batch6857"
 *
 * Exclusive continuum CREATE-ONLY (6851-6860: openat2 resolve flags
 * stubs). Unique gj_oat2_resolve_known_mask_6857 surface only; no
 * multi-def. Distinct from cached (batch6856) and bit_count
 * (batch6858). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6857_marker[] = "libcgj-batch6857";

/*
 * Soft known RESOLVE_* bits from this exclusive wave catalog:
 * NO_XDEV|NO_MAGICLINKS|NO_SYMLINKS|BENEATH|IN_ROOT|CACHED == 0x3f.
 */
#define B6857_RESOLVE_KNOWN_MASK  0x0000003fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6857_known_mask(void)
{
	return B6857_RESOLVE_KNOWN_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oat2_resolve_known_mask_6857 - soft openat2 resolve known-bits mask.
 *
 * Always returns 0x0000003f. Pure-data stub for product openat2 resolve
 * flag catalogs; does not open paths or call openat2. No parent wires.
 */
uint32_t
gj_oat2_resolve_known_mask_6857(void)
{
	(void)NULL;
	return b6857_known_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oat2_resolve_known_mask_6857(void)
    __attribute__((alias("gj_oat2_resolve_known_mask_6857")));
