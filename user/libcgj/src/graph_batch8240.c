/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8240: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8240(void);
 *     - Returns the compile-time graph batch number for this TU (8240).
 *   uint32_t __gj_batch_id_8240  (alias)
 *   __libcgj_batch8240_marker = "libcgj-batch8240"
 *
 * Exclusive continuum CREATE-ONLY (8231-8240: memcmp/memchr probe
 * stubs — memcmp_eq, memcmp_lt, memcmp_gt, memchr_found, memchr_miss,
 * memrchr_found, mem_zero_len, mem_overlap, mem_align_ok,
 * batch_id_8240). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8240_marker[] = "libcgj-batch8240";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8240_id(void)
{
	return 8240u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8240 - report this TU's graph batch number.
 *
 * Always returns 8240. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8240(void)
{
	(void)NULL;
	return b8240_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8240(void)
    __attribute__((alias("gj_batch_id_8240")));
