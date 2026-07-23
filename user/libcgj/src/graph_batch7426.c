/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7426: access mode known-modes subset check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_access_known_modes_ok_7426(uint32_t mode);
 *     - Return 1 if mode is a subset of the known baseline
 *       ((mode & ~0x7) == 0).
 *   uint32_t __gj_access_known_modes_ok_7426  (alias)
 *   __libcgj_batch7426_marker = "libcgj-batch7426"
 *
 * Exclusive continuum CREATE-ONLY (7421-7430: access mode R_OK W_OK stubs —
 * r_ok_u, w_ok_u, x_ok_u, rw_mask, known_modes, known_modes_ok, has_rw,
 * rw_both_u, any_u, continuum + batch_id_7430). Unique
 * gj_access_known_modes_ok_7426 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7426_marker[] = "libcgj-batch7426";

/* Soft known access modes: X_OK|W_OK|R_OK. F_OK (0) is always a subset. */
#define B7426_KNOWN_MODES ((uint32_t)0x00000007u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7426_known_modes_ok(uint32_t u32Mode)
{
	return ((u32Mode & ~B7426_KNOWN_MODES) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_access_known_modes_ok_7426 - mode is subset of known baseline.
 *
 * mode: soft access() mode bitmask
 *
 * Returns 1 if no unknown bits present; else 0. Zero mode (F_OK) is ok.
 * Pure mask test; no parent wires. Does not call access().
 */
uint32_t
gj_access_known_modes_ok_7426(uint32_t u32Mode)
{
	(void)NULL;
	return b7426_known_modes_ok(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_access_known_modes_ok_7426(uint32_t u32Mode)
    __attribute__((alias("gj_access_known_modes_ok_7426")));
