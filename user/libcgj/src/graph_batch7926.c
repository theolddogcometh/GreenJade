/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7926: scanf conversion field-width clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scanf_width_clamp_u_7926(uint32_t width, uint32_t maxw);
 *     - Clamp field width to [0, maxw]. width 0 means unlimited → maxw
 *       when maxw != 0, else 0.
 *   uint32_t __gj_scanf_width_clamp_u_7926  (alias)
 *   __libcgj_batch7926_marker = "libcgj-batch7926"
 *
 * Exclusive continuum CREATE-ONLY (7921-7930: scanf conversion stubs —
 * digit_value, is_digit, is_space, sign_of, base_norm, width_clamp,
 * suppress_id, length_mod_id, accum_digit, batch_id_7930).
 * Unique gj_scanf_width_clamp_u_7926 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7926_marker[] = "libcgj-batch7926";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7926_width_clamp(uint32_t u32Width, uint32_t u32Maxw)
{
	if (u32Maxw == 0u) {
		return 0u;
	}
	if (u32Width == 0u) {
		/* 0 field width in catalog → unlimited → max bound. */
		return u32Maxw;
	}
	if (u32Width > u32Maxw) {
		return u32Maxw;
	}
	return u32Width;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scanf_width_clamp_u_7926 - soft scanf field-width clamp.
 *
 * width: requested conversion field width (0 = unlimited catalog)
 * maxw:  maximum allowed width bound (0 → always 0)
 *
 * Returns clamped width in [0, maxw]. Pure integer clamp; does not call
 * scanf(3). No parent wires.
 */
uint32_t
gj_scanf_width_clamp_u_7926(uint32_t u32Width, uint32_t u32Maxw)
{
	(void)NULL;
	return b7926_width_clamp(u32Width, u32Maxw);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scanf_width_clamp_u_7926(uint32_t u32Width, uint32_t u32Maxw)
    __attribute__((alias("gj_scanf_width_clamp_u_7926")));
