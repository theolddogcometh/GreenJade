/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4908: 2x2 identity off-diagonal element (zero).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mat2_identity_b_u(void);
 *     - Returns the off-diagonal entry of I2 (always 0).
 *   uint32_t __gj_mat2_identity_b_u  (alias)
 *   __libcgj_batch4908_marker = "libcgj-batch4908"
 *
 * Exclusive continuum CREATE-ONLY (4901-4910). Distinct from
 * gj_mat2_identity_a_u (batch4907) — unique gj_mat2_identity_b_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4908_marker[] = "libcgj-batch4908";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4908_identity_b(void)
{
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mat2_identity_b_u - off-diagonal entry of the 2x2 identity.
 *
 * Always returns 0. Soft compile-time constant surface for I2[b].
 * Self-contained; no parent wires.
 */
uint32_t
gj_mat2_identity_b_u(void)
{
	(void)NULL;
	return b4908_identity_b();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mat2_identity_b_u(void)
    __attribute__((alias("gj_mat2_identity_b_u")));
