/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4907: 2x2 identity diagonal element (one).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mat2_identity_a_u(void);
 *     - Returns the main-diagonal entry of I2 (always 1).
 *   uint32_t __gj_mat2_identity_a_u  (alias)
 *   __libcgj_batch4907_marker = "libcgj-batch4907"
 *
 * Exclusive continuum CREATE-ONLY (4901-4910). Unique
 * gj_mat2_identity_a_u surface only; no multi-def. Pair with
 * gj_mat2_identity_b_u (batch4908) for the off-diagonal zero.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4907_marker[] = "libcgj-batch4907";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4907_identity_a(void)
{
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat2_identity_a_u - main-diagonal entry of the 2x2 identity.
 *
 * Always returns 1. Soft compile-time constant surface for I2[a].
 * Self-contained; no parent wires.
 */
uint32_t
gj_mat2_identity_a_u(void)
{
	(void)NULL;
	return b4907_identity_a();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mat2_identity_a_u(void)
    __attribute__((alias("gj_mat2_identity_a_u")));
