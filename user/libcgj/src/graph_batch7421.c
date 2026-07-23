/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7421: access mode R_OK bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_access_r_ok_u_7421(uint32_t mode);
 *     - Return 1 if R_OK (0x4) is set in mode, else 0.
 *   uint32_t __gj_access_r_ok_u_7421  (alias)
 *   __libcgj_batch7421_marker = "libcgj-batch7421"
 *
 * Exclusive continuum CREATE-ONLY (7421-7430: access mode R_OK W_OK stubs —
 * r_ok_u, w_ok_u, x_ok_u, rw_mask, known_modes, known_modes_ok, has_rw,
 * rw_both_u, any_u, continuum + batch_id_7430). Unique
 * gj_access_r_ok_u_7421 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7421_marker[] = "libcgj-batch7421";

/* Soft POSIX access mode R_OK: test for read permission (0x4). */
#define B7421_R_OK ((uint32_t)0x00000004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7421_r_ok(uint32_t u32Mode)
{
	return ((u32Mode & B7421_R_OK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_access_r_ok_u_7421 - 1 if R_OK is set in mode.
 *
 * mode: raw access() mode bitmask
 *
 * Returns 1 when the R_OK bit is set, else 0.
 * Pure mask test; no parent wires. Does not call access().
 */
uint32_t
gj_access_r_ok_u_7421(uint32_t u32Mode)
{
	(void)NULL;
	return b7421_r_ok(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_access_r_ok_u_7421(uint32_t u32Mode)
    __attribute__((alias("gj_access_r_ok_u_7421")));
