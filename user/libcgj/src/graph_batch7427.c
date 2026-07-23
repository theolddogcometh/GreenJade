/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7427: access mode has-R_OK-or-W_OK compound stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_access_has_rw_u_7427(uint32_t mode);
 *     - Return 1 if R_OK or W_OK is set, else 0.
 *   uint32_t __gj_access_has_rw_u_7427  (alias)
 *   __libcgj_batch7427_marker = "libcgj-batch7427"
 *
 * Exclusive continuum CREATE-ONLY (7421-7430: access mode R_OK W_OK stubs —
 * r_ok_u, w_ok_u, x_ok_u, rw_mask, known_modes, known_modes_ok, has_rw,
 * rw_both_u, any_u, continuum + batch_id_7430). Unique
 * gj_access_has_rw_u_7427 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7427_marker[] = "libcgj-batch7427";

/* Soft R/W permission bits: R_OK | W_OK. */
#define B7427_R_OK ((uint32_t)0x00000004u)
#define B7427_W_OK ((uint32_t)0x00000002u)
#define B7427_RW_MASK (B7427_R_OK | B7427_W_OK)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7427_has_rw(uint32_t u32Mode)
{
	return ((u32Mode & B7427_RW_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_access_has_rw_u_7427 - 1 if any R_OK/W_OK bit is set.
 *
 * mode: raw access() mode bitmask
 *
 * Returns 1 when R_OK and/or W_OK is present. Self-contained;
 * does not call sibling gj_access_* helpers. No parent wires.
 */
uint32_t
gj_access_has_rw_u_7427(uint32_t u32Mode)
{
	(void)NULL;
	return b7427_has_rw(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_access_has_rw_u_7427(uint32_t u32Mode)
    __attribute__((alias("gj_access_has_rw_u_7427")));
