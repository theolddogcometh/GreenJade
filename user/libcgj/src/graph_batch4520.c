/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4520: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4520(void);
 *     - Returns the compile-time graph batch number for this TU (4520).
 *   uint32_t __gj_batch_id_4520  (alias)
 *   __libcgj_batch4520_marker = "libcgj-batch4520"
 *
 * Exclusive continuum CREATE-ONLY (4511-4520: atomic-less counters
 * unique — counter_inc_u2, counter_dec_u2, counter_add_u2,
 * counter_sub_u, counter_reset_u, counter_get_u, counter_max_u,
 * counter_min_u, counter_cas_u2, batch_id_4520). Does NOT redefine
 * gj_batch_id / prior batch_id_* — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4520_marker[] = "libcgj-batch4520";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4520_id(void)
{
	return 4520u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4520 - report this TU's graph batch number.
 *
 * Always returns 4520. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * Self-contained; no parent wires.
 */
uint32_t
gj_batch_id_4520(void)
{
	(void)NULL;
	return b4520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4520(void)
    __attribute__((alias("gj_batch_id_4520")));
