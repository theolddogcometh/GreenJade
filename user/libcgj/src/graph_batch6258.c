/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6258: input event queue depth clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_queue_depth_6258(uint32_t depth);
 *     - Soft event-queue depth clamp into [1, 256]. Zero maps to 1
 *       (minimum live queue); values above 256 clamp to 256. Pure
 *       data; does not allocate queue storage.
 *   uint32_t __gj_input_queue_depth_6258  (alias)
 *   __libcgj_batch6258_marker = "libcgj-batch6258"
 *
 * Exclusive continuum CREATE-ONLY (6251-6260: input lag / poll rate
 * stubs). Unique gj_input_queue_depth_6258 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6258_marker[] = "libcgj-batch6258";

#define B6258_DEPTH_MIN  1u
#define B6258_DEPTH_MAX  256u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6258_queue_depth(uint32_t u32Depth)
{
	if (u32Depth < B6258_DEPTH_MIN)
		return B6258_DEPTH_MIN;
	if (u32Depth > B6258_DEPTH_MAX)
		return B6258_DEPTH_MAX;
	return u32Depth;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_queue_depth_6258 - input event queue depth clamp.
 *
 * depth: requested soft queue capacity (events)
 *
 * Returns clamped depth in [1, 256]. Soft pure-data clamp for the
 * 6251-6260 continuum. No parent wires.
 */
uint32_t
gj_input_queue_depth_6258(uint32_t depth)
{
	(void)NULL;
	return b6258_queue_depth(depth);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_queue_depth_6258(uint32_t depth)
    __attribute__((alias("gj_input_queue_depth_6258")));
