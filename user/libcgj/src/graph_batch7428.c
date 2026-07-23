/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7428: access mode R_OK and W_OK both-set stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_access_rw_both_u_7428(uint32_t mode);
 *     - Return 1 if both R_OK and W_OK are set
 *       ((mode & 0x6) == 0x6), else 0.
 *   uint32_t __gj_access_rw_both_u_7428  (alias)
 *   __libcgj_batch7428_marker = "libcgj-batch7428"
 *
 * Exclusive continuum CREATE-ONLY (7421-7430: access mode R_OK W_OK stubs —
 * r_ok_u, w_ok_u, x_ok_u, rw_mask, known_modes, known_modes_ok, has_rw,
 * rw_both_u, any_u, continuum + batch_id_7430). Unique
 * gj_access_rw_both_u_7428 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7428_marker[] = "libcgj-batch7428";

/* Soft R+W both-set: R_OK | W_OK full mask. */
#define B7428_R_OK ((uint32_t)0x00000004u)
#define B7428_W_OK ((uint32_t)0x00000002u)
#define B7428_RW_BOTH (B7428_R_OK | B7428_W_OK)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7428_rw_both(uint32_t u32Mode)
{
	return ((u32Mode & B7428_RW_BOTH) == B7428_RW_BOTH) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_access_rw_both_u_7428 - 1 if both R_OK and W_OK are set.
 *
 * mode: raw access() mode bitmask
 *
 * Returns 1 when R_OK and W_OK are both present.
 * Self-contained; does not call sibling helpers. No parent wires.
 */
uint32_t
gj_access_rw_both_u_7428(uint32_t u32Mode)
{
	(void)NULL;
	return b7428_rw_both(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_access_rw_both_u_7428(uint32_t u32Mode)
    __attribute__((alias("gj_access_rw_both_u_7428")));
