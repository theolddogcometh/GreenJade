/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5085: soft assert condition predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbg_assert_ok_u(uint32_t cond);
 *     - Return 1 if cond is nonzero, else 0. Soft assert-ok probe:
 *       does not abort; only reports whether the condition holds.
 *   uint32_t __gj_dbg_assert_ok_u  (alias)
 *   __libcgj_batch5085_marker = "libcgj-batch5085"
 *
 * Exclusive continuum CREATE-ONLY (5081-5090: debug/assert unique).
 * Unique gj_dbg_assert_ok_u surface only; no multi-def. Distinct from
 * gj_dbg_likely_u (batch5083). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5085_marker[] = "libcgj-batch5085";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5085_assert_ok(uint32_t u32Cond)
{
	if (u32Cond != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbg_assert_ok_u - 1 if cond is true (nonzero), else 0.
 *
 * cond: assertion condition under test
 *
 * Soft check only; never aborts or sets errno. Equivalent to
 * (cond ? 1 : 0). No parent wires.
 */
uint32_t
gj_dbg_assert_ok_u(uint32_t u32Cond)
{
	(void)NULL;
	return b5085_assert_ok(u32Cond);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbg_assert_ok_u(uint32_t u32Cond)
    __attribute__((alias("gj_dbg_assert_ok_u")));
