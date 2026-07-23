/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8139: strncat bounded-kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncat_kind_bounded_8139(void);
 *     - Return soft strncat kind id (2 = n-bounded payload append).
 *   uint32_t __gj_strncat_kind_bounded_8139  (alias)
 *   __libcgj_batch8139_marker = "libcgj-batch8139"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Unique gj_strncat_kind_bounded_8139 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8139_marker[] = "libcgj-batch8139";

/* Soft strncat kind: 2 = n-bounded payload append (classic strncat). */
#define B8139_KIND_BOUNDED  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8139_kind_bounded(void)
{
	return B8139_KIND_BOUNDED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncat_kind_bounded_8139 - soft strncat bounded kind id.
 *
 * Always returns 2 (n-bounded payload append catalog id). Does not
 * call strncat(3). No parent wires.
 */
uint32_t
gj_strncat_kind_bounded_8139(void)
{
	(void)NULL;
	return b8139_kind_bounded();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncat_kind_bounded_8139(void)
    __attribute__((alias("gj_strncat_kind_bounded_8139")));
