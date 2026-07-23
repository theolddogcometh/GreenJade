/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6859: openat2 resolve flag catalog membership.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oat2_resolve_flag_ok_6859(uint32_t resolve);
 *     - Returns 1 when resolve has only known soft RESOLVE_* bits from
 *       the 6851-6856 catalog (NO_XDEV|NO_MAGICLINKS|NO_SYMLINKS|
 *       BENEATH|IN_ROOT|CACHED == 0x3f), else 0.
 *       Pure-data mask stub; not a live openat2 validation.
 *   uint32_t __gj_oat2_resolve_flag_ok_6859  (alias)
 *   __libcgj_batch6859_marker = "libcgj-batch6859"
 *
 * Exclusive continuum CREATE-ONLY (6851-6860: openat2 resolve flags
 * stubs —
 * resolve_no_xdev_6851, resolve_no_magiclinks_6852,
 * resolve_no_symlinks_6853, resolve_beneath_6854, resolve_in_root_6855,
 * resolve_cached_6856, resolve_known_mask_6857, resolve_bit_count_6858,
 * resolve_flag_ok_6859, continuum + batch_id_6860). Unique surface
 * only; no multi-def. Soft known-bits mask covers classic resolve
 * flags. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6859_marker[] = "libcgj-batch6859";

/*
 * Soft known RESOLVE_* bits from this exclusive wave catalog:
 * NO_XDEV|NO_MAGICLINKS|NO_SYMLINKS|BENEATH|IN_ROOT|CACHED == 0x3f.
 */
#define B6859_RESOLVE_KNOWN  0x0000003fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6859_flag_ok(uint32_t uResolve)
{
	return ((uResolve & ~B6859_RESOLVE_KNOWN) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oat2_resolve_flag_ok_6859 - soft openat2 RESOLVE flag membership.
 *
 * resolve: candidate open_how.resolve flags word (low 32 bits).
 * Returns 1 if every set bit is in the known soft catalog mask
 * 0x0000003f, else 0. Does not query the kernel. No parent wires.
 */
uint32_t
gj_oat2_resolve_flag_ok_6859(uint32_t resolve)
{
	(void)NULL;
	return b6859_flag_ok(resolve);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oat2_resolve_flag_ok_6859(uint32_t resolve)
    __attribute__((alias("gj_oat2_resolve_flag_ok_6859")));
