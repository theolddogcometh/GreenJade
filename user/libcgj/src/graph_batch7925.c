/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7925: scanf conversion base-normalize stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scanf_base_norm_u_7925(uint32_t base);
 *     - Normalize conversion base: 0 stays 0 (auto); 2..36 pass;
 *       otherwise return 0 (invalid → auto/fail catalog).
 *   uint32_t __gj_scanf_base_norm_u_7925  (alias)
 *   __libcgj_batch7925_marker = "libcgj-batch7925"
 *
 * Exclusive continuum CREATE-ONLY (7921-7930: scanf conversion stubs —
 * digit_value, is_digit, is_space, sign_of, base_norm, width_clamp,
 * suppress_id, length_mod_id, accum_digit, batch_id_7930).
 * Unique gj_scanf_base_norm_u_7925 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7925_marker[] = "libcgj-batch7925";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7925_base_norm(uint32_t u32Base)
{
	/* 0 = auto (0x/0 prefix rules); 2..36 = fixed radix. */
	if (u32Base == 0u) {
		return 0u;
	}
	if (u32Base >= 2u && u32Base <= 36u) {
		return u32Base;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scanf_base_norm_u_7925 - soft scanf conversion base normalize.
 *
 * base: requested radix (0 auto, or 2..36)
 *
 * Returns base when in {0} U [2,36], else 0. Catalog only; does not
 * call strtol/scanf. No parent wires.
 */
uint32_t
gj_scanf_base_norm_u_7925(uint32_t u32Base)
{
	(void)NULL;
	return b7925_base_norm(u32Base);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scanf_base_norm_u_7925(uint32_t u32Base)
    __attribute__((alias("gj_scanf_base_norm_u_7925")));
