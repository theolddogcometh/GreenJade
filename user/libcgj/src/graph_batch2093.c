/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2093: freestanding saturating gen-count bump.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gen_count_inc_sat(uint32_t g);
 *     - Return g + 1, saturating at UINT32_MAX (no wrap). Pure integer.
 *   uint32_t __gj_gen_count_inc_sat  (alias)
 *   __libcgj_batch2093_marker = "libcgj-batch2093"
 *
 * Post-2000 id exclusive wave (2091-2100). Unique gj_gen_count_inc_sat
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2093_marker[] = "libcgj-batch2093";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2093_inc_sat(uint32_t u32G)
{
	if (u32G == 0xffffffffu) {
		return 0xffffffffu;
	}
	return u32G + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gen_count_inc_sat - bump a generation counter with saturation.
 *
 * g: current generation count (0 .. UINT32_MAX)
 *
 * Returns g + 1 when g < UINT32_MAX, else UINT32_MAX. Does not wrap.
 * Does not call libc.
 */
uint32_t
gj_gen_count_inc_sat(uint32_t u32G)
{
	(void)NULL;
	return b2093_inc_sat(u32G);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gen_count_inc_sat(uint32_t u32G)
    __attribute__((alias("gj_gen_count_inc_sat")));
