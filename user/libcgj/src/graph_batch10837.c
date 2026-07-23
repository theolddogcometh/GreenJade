/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10837: IM soft input-method ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_method_ok_u_10837(void);
 *     - Returns 0 (soft stub: generic input-method framework slot is
 *       soft/unprobed; not a runtime IM framework probe).
 *   uint32_t __gj_input_method_ok_u_10837  (alias)
 *   __libcgj_batch10837_marker = "libcgj-batch10837"
 *
 * Exclusive continuum CREATE-ONLY (10831-10840: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→10840). Unique
 * gj_input_method_ok_u_10837 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs in this wave. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10837_marker[] = "libcgj-batch10837";

/* Soft-stub input-method ok value (unprobed). */
#define B10837_INPUT_METHOD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10837_input_method_ok(void)
{
	return B10837_INPUT_METHOD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_method_ok_u_10837 - soft input-method ok stub.
 *
 * Always returns 0 (soft/unprobed input-method slot). Does not call
 * libc or probe any IM framework. No parent wires.
 */
uint32_t
gj_input_method_ok_u_10837(void)
{
	(void)NULL;
	return b10837_input_method_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_method_ok_u_10837(void)
    __attribute__((alias("gj_input_method_ok_u_10837")));
