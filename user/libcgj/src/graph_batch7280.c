/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7280: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7280(void);
 *     - Returns the compile-time graph batch number for this TU (7280).
 *   uint32_t __gj_batch_id_7280  (alias)
 *   __libcgj_batch7280_marker = "libcgj-batch7280"
 *
 * Exclusive continuum CREATE-ONLY (7271-7280: accept4 flag stubs —
 * cloexec_id, nonblock_id, known_mask_id, has_cloexec, has_nonblock,
 * is_zero, flags_ok, flags_pack, flags_errorish, batch_id_7280).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7280_marker[] = "libcgj-batch7280";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7280_id(void)
{
	return 7280u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7280 - report this TU's graph batch number.
 *
 * Always returns 7280. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7280(void)
{
	(void)NULL;
	return b7280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7280(void)
    __attribute__((alias("gj_batch_id_7280")));
