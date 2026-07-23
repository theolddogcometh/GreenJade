/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7923: scanf conversion whitespace-class stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scanf_is_space_u_7923(uint32_t ch);
 *     - Return 1 if ch is scanf skip-whitespace (sp/tab/lf/cr/ff/vt),
 *       else 0.
 *   uint32_t __gj_scanf_is_space_u_7923  (alias)
 *   __libcgj_batch7923_marker = "libcgj-batch7923"
 *
 * Exclusive continuum CREATE-ONLY (7921-7930: scanf conversion stubs —
 * digit_value, is_digit, is_space, sign_of, base_norm, width_clamp,
 * suppress_id, length_mod_id, accum_digit, batch_id_7930).
 * Unique gj_scanf_is_space_u_7923 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7923_marker[] = "libcgj-batch7923";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7923_is_space(uint32_t u32Ch)
{
	/* C locale isspace class used by scanf directive whitespace skip. */
	if (u32Ch == (uint32_t)' ' ||
	    u32Ch == (uint32_t)'\t' ||
	    u32Ch == (uint32_t)'\n' ||
	    u32Ch == (uint32_t)'\r' ||
	    u32Ch == (uint32_t)'\f' ||
	    u32Ch == (uint32_t)'\v') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scanf_is_space_u_7923 - soft scanf whitespace-class predicate.
 *
 * ch: code unit
 *
 * Returns 1 for space/tab/lf/cr/ff/vt, else 0. Catalog class only; does
 * not call isspace(3) or scanf(3). No parent wires.
 */
uint32_t
gj_scanf_is_space_u_7923(uint32_t u32Ch)
{
	(void)NULL;
	return b7923_is_space(u32Ch);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scanf_is_space_u_7923(uint32_t u32Ch)
    __attribute__((alias("gj_scanf_is_space_u_7923")));
