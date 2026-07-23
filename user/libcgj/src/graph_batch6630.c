/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6630: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6630(void);
 *     - Returns the compile-time graph batch number for this TU (6630).
 *   uint32_t __gj_batch_id_6630  (alias)
 *   __libcgj_batch6630_marker = "libcgj-batch6630"
 *
 * Exclusive continuum CREATE-ONLY (6621-6630: epoll event bit stubs —
 * in, out, err, hup, rdhup, et, oneshot, pri, errorish, batch_id_6630).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6630_marker[] = "libcgj-batch6630";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6630_id(void)
{
	return 6630u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6630 - report this TU's graph batch number.
 *
 * Always returns 6630. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6630(void)
{
	(void)NULL;
	return b6630_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6630(void)
    __attribute__((alias("gj_batch_id_6630")));
