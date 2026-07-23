/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6841: close_range CLOSE_RANGE_UNSHARE flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cr_unshare_6841(void);
 *     - Returns 0x2 (soft CLOSE_RANGE_UNSHARE: unshare fd table first).
 *       Pure-data catalog stub; not a live close_range syscall.
 *   uint32_t __gj_cr_unshare_6841  (alias)
 *   __libcgj_batch6841_marker = "libcgj-batch6841"
 *
 * Exclusive continuum CREATE-ONLY (6841-6850: close_range flags stubs —
 * unshare, cloexec, has_unshare, has_cloexec, known_mask, has_any,
 * flags_ok, plain, both, batch_id_6850). Unique gj_cr_unshare_6841
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6841_marker[] = "libcgj-batch6841";

/* CLOSE_RANGE_UNSHARE: unshare file descriptor table first (1U << 1). */
#define B6841_CLOSE_RANGE_UNSHARE ((uint32_t)0x00000002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6841_unshare(void)
{
	return B6841_CLOSE_RANGE_UNSHARE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cr_unshare_6841 - soft CLOSE_RANGE_UNSHARE flag value.
 *
 * Always returns 2. Catalog stub for close_range(2) flag bit 1.
 * No parent wires.
 */
uint32_t
gj_cr_unshare_6841(void)
{
	(void)NULL;
	return b6841_unshare();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cr_unshare_6841(void)
    __attribute__((alias("gj_cr_unshare_6841")));
