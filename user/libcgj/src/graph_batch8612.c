/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8612: stack canary ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_stack_canary_ok_u_8612(uint32_t canary);
 *     - Return 1 if canary is non-zero (soft SSP canary ok).
 *   uint32_t __gj_stack_canary_ok_u_8612  (alias)
 *   __libcgj_batch8612_marker = "libcgj-batch8612"
 *
 * Exclusive continuum CREATE-ONLY (8611-8620: stack/canary id stubs —
 * stack_guard_id, stack_canary_ok_u, stack_align_u, stack_grow_down_id,
 * stack_page_id, ssp_ready_u, ssp_fail_id, frame_ok_u, ra_align_u,
 * batch_id_8620). Unique gj_stack_canary_ok_u_8612 surface only; no
 * multi-def. Soft catalog check only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8612_marker[] = "libcgj-batch8612";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8612_stack_canary_ok(uint32_t u32Canary)
{
	/* Soft catalog: any non-zero canary is treated as ok. */
	return (u32Canary != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_canary_ok_u_8612 - 1 if soft stack canary looks valid.
 *
 * canary: candidate stack-protector canary word
 *
 * Returns 1 when canary is non-zero, else 0. Does not call
 * __stack_chk_fail. No parent wires.
 */
uint32_t
gj_stack_canary_ok_u_8612(uint32_t u32Canary)
{
	(void)NULL;
	return b8612_stack_canary_ok(u32Canary);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_stack_canary_ok_u_8612(uint32_t u32Canary)
    __attribute__((alias("gj_stack_canary_ok_u_8612")));
