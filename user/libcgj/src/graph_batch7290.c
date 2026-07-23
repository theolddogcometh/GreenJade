/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7290: pipe2 flags ready soft gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pipe2_flags_ready_7290(uint32_t mask_ok, uint32_t any_or_zero);
 *     - Soft ready gate: both mask_ok and any_or_zero non-zero → 1; else 0.
 *   uint32_t __gj_pipe2_flags_ready_7290  (alias)
 *   uint32_t gj_batch_id_7290(void);
 *   uint32_t __gj_batch_id_7290  (alias)
 *   __libcgj_batch7290_marker = "libcgj-batch7290"
 *
 * Exclusive continuum CREATE-ONLY (7281-7290: pipe2 flag stubs —
 * cloexec, nonblock, direct, zero_ok, pack, mask_ok, combo, strip,
 * any_known, flags_ready). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7290_marker[] = "libcgj-batch7290";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7290_flags_ready(uint32_t u32MaskOk, uint32_t u32AnyOrZero)
{
	if (u32MaskOk == 0u || u32AnyOrZero == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pipe2_flags_ready_7290 - soft pipe2 flags ready gate.
 *
 * mask_ok:     non-zero when flags mask soft-ok
 * any_or_zero: non-zero when zero_ok or any_known soft-ok (caller OR)
 *
 * Returns 1 when both gates pass, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_pipe2_flags_ready_7290(uint32_t mask_ok, uint32_t any_or_zero)
{
	(void)NULL;
	return b7290_flags_ready(mask_ok, any_or_zero);
}

/*
 * gj_batch_id_7290 - report this TU's graph batch number.
 *
 * Always returns 7290.
 */
uint32_t
gj_batch_id_7290(void)
{
	return 7290u;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_pipe2_flags_ready_7290(uint32_t mask_ok, uint32_t any_or_zero)
    __attribute__((alias("gj_pipe2_flags_ready_7290")));

uint32_t __gj_batch_id_7290(void)
    __attribute__((alias("gj_batch_id_7290")));
