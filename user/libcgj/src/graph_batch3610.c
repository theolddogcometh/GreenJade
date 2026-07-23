/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3610: batch identity for exclusive wave
 * 3601-3610.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3610(void);
 *     - Returns the compile-time graph batch number for this TU (3610).
 *   uint32_t __gj_batch_id_3610  (alias)
 *   __libcgj_batch3610_marker = "libcgj-batch3610"
 *
 * CREATE-ONLY exclusive continuum wave (3601-3610): u32_add_ovf_p_u,
 * u64_add_ovf_p_u, u32_sub_ovf_p_u, u64_sub_ovf_p_u, u32_mul_ovf_p_u,
 * u64_mul_ovf_p_u, u32_add_with_carry_u, u64_add_with_carry_u,
 * u32_sub_with_borrow_u, batch_id_3610. Unique gj_batch_id_3610
 * surface only; no multi-def. Does NOT redefine gj_batch_id / prior
 * batch_id_* symbols. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3610_marker[] = "libcgj-batch3610";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3610_id(void)
{
	return 3610u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3610 - report this TU's graph batch number.
 *
 * Always returns 3610. Link-time presence tags the 3601-3610 wave.
 * No parent wires.
 */
uint32_t
gj_batch_id_3610(void)
{
	(void)NULL;
	return b3610_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3610(void)
    __attribute__((alias("gj_batch_id_3610")));
