/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2590: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2590(void);
 *     - Returns the compile-time graph batch number for this TU (2590).
 *   uint32_t __gj_batch_id_2590  (alias)
 *   __libcgj_batch2590_marker = "libcgj-batch2590"
 *
 * Milestone for the exclusive base-encode length / hex nibble wave
 * (batches 2581-2589: base16/32/64 encode+decode lengths, base64url
 * encode length, hex nibble from/to char). Does NOT redefine prior
 * gj_batch_id_* / gj_graph_milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2590_marker[] = "libcgj-batch2590";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2590_id(void)
{
	return 2590u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2590 — report this TU's graph batch number.
 *
 * Always returns 2590. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2590(void)
{
	(void)NULL;
	return b2590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2590(void)
    __attribute__((alias("gj_batch_id_2590")));
