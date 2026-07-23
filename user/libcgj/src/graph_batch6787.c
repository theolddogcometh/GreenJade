/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6787: inotify event-mask OR merge.
 *
 * Surface (unique symbols):
 *   uint32_t gj_inotify_mask_or_6787(uint32_t a, uint32_t b);
 *     - Return a | b (soft merge of two inotify event masks).
 *   uint32_t __gj_inotify_mask_or_6787  (alias)
 *   __libcgj_batch6787_marker = "libcgj-batch6787"
 *
 * Exclusive continuum CREATE-ONLY (6781-6790: inotify event mask stubs).
 * Unique gj_inotify_mask_or_6787 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6787_marker[] = "libcgj-batch6787";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6787_or(uint32_t u32A, uint32_t u32B)
{
	return u32A | u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inotify_mask_or_6787 - bitwise OR of two inotify event masks.
 *
 * a, b: soft inotify event bitmasks
 *
 * Returns a | b. Soft pure-data merge (e.g. want = create | delete
 * bookkeeping); no inotify_add_watch.
 */
uint32_t
gj_inotify_mask_or_6787(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b6787_or(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_inotify_mask_or_6787(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_inotify_mask_or_6787")));
