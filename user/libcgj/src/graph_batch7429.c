/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7429: access mode any-known-permission stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_access_any_u_7429(uint32_t mode);
 *     - Return 1 if any known access permission bit is set
 *       ((mode & 0x7) != 0), else 0.
 *   uint32_t __gj_access_any_u_7429  (alias)
 *   __libcgj_batch7429_marker = "libcgj-batch7429"
 *
 * Exclusive continuum CREATE-ONLY (7421-7430: access mode R_OK W_OK stubs —
 * r_ok_u, w_ok_u, x_ok_u, rw_mask, known_modes, known_modes_ok, has_rw,
 * rw_both_u, any_u, continuum + batch_id_7430). Unique
 * gj_access_any_u_7429 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7429_marker[] = "libcgj-batch7429";

/* Soft known access modes: X_OK|W_OK|R_OK. */
#define B7429_KNOWN_MODES ((uint32_t)0x00000007u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7429_any(uint32_t u32Mode)
{
	return ((u32Mode & B7429_KNOWN_MODES) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_access_any_u_7429 - 1 if any known access permission bit is set.
 *
 * mode: raw access() mode bitmask
 *
 * Returns 1 when at least one of X_OK, W_OK, R_OK is set.
 * Pure mask test; no parent wires. Does not call access().
 */
uint32_t
gj_access_any_u_7429(uint32_t u32Mode)
{
	(void)NULL;
	return b7429_any(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_access_any_u_7429(uint32_t u32Mode)
    __attribute__((alias("gj_access_any_u_7429")));
