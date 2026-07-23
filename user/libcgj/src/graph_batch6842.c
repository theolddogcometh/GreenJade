/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6842: close_range CLOSE_RANGE_CLOEXEC flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cr_cloexec_6842(void);
 *     - Returns 0x4 (soft CLOSE_RANGE_CLOEXEC: set FD_CLOEXEC instead
 *       of closing). Pure-data catalog stub; not a live close_range.
 *   uint32_t __gj_cr_cloexec_6842  (alias)
 *   __libcgj_batch6842_marker = "libcgj-batch6842"
 *
 * Exclusive continuum CREATE-ONLY (6841-6850: close_range flags stubs —
 * unshare, cloexec, has_unshare, has_cloexec, known_mask, has_any,
 * flags_ok, plain, both, batch_id_6850). Unique gj_cr_cloexec_6842
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6842_marker[] = "libcgj-batch6842";

/* CLOSE_RANGE_CLOEXEC: set FD_CLOEXEC rather than close (1U << 2). */
#define B6842_CLOSE_RANGE_CLOEXEC ((uint32_t)0x00000004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6842_cloexec(void)
{
	return B6842_CLOSE_RANGE_CLOEXEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cr_cloexec_6842 - soft CLOSE_RANGE_CLOEXEC flag value.
 *
 * Always returns 4. Catalog stub for close_range(2) flag bit 2.
 * No parent wires.
 */
uint32_t
gj_cr_cloexec_6842(void)
{
	(void)NULL;
	return b6842_cloexec();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cr_cloexec_6842(void)
    __attribute__((alias("gj_cr_cloexec_6842")));
