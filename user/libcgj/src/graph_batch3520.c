/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3520: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3520(void);
 *     - Returns the compile-time graph batch number for this TU (3520).
 *   uint32_t __gj_batch_id_3520  (alias)
 *   __libcgj_batch3520_marker = "libcgj-batch3520"
 *
 * Exclusive continuum CREATE-ONLY wave (3511-3520: u64_mod_mul_u,
 * u32_mod_inv_u, u32_mod_div_u, u32_fact_mod_u, u32_binom_mod_u,
 * u32_catalan_u, u32_fib_mod_u, u64_fib_mod_u, u32_lucas_u,
 * batch_id_3520). Distinct from gj_batch_id / prior batch_id_* —
 * unique surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3520_marker[] = "libcgj-batch3520";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3520_id(void)
{
	return 3520u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3520 - report this TU's graph batch number.
 *
 * Always returns 3520. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3520(void)
{
	(void)NULL;
	return b3520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3520(void)
    __attribute__((alias("gj_batch_id_3520")));
