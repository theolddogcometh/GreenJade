/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8138: strcat unbounded-kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcat_kind_unbounded_8138(void);
 *     - Return soft strcat kind id (1 = unbounded full-src append).
 *   uint32_t __gj_strcat_kind_unbounded_8138  (alias)
 *   __libcgj_batch8138_marker = "libcgj-batch8138"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Unique gj_strcat_kind_unbounded_8138 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8138_marker[] = "libcgj-batch8138";

/* Soft strcat kind: 1 = unbounded full-source append (classic strcat). */
#define B8138_KIND_UNBOUNDED  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8138_kind_unbounded(void)
{
	return B8138_KIND_UNBOUNDED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcat_kind_unbounded_8138 - soft strcat unbounded kind id.
 *
 * Always returns 1 (unbounded full-src append catalog id). Does not
 * call strcat(3). No parent wires.
 */
uint32_t
gj_strcat_kind_unbounded_8138(void)
{
	(void)NULL;
	return b8138_kind_unbounded();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcat_kind_unbounded_8138(void)
    __attribute__((alias("gj_strcat_kind_unbounded_8138")));
