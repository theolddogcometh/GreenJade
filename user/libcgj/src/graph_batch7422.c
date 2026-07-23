/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7422: access mode W_OK bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_access_w_ok_u_7422(uint32_t mode);
 *     - Return 1 if W_OK (0x2) is set in mode, else 0.
 *   uint32_t __gj_access_w_ok_u_7422  (alias)
 *   __libcgj_batch7422_marker = "libcgj-batch7422"
 *
 * Exclusive continuum CREATE-ONLY (7421-7430: access mode R_OK W_OK stubs —
 * r_ok_u, w_ok_u, x_ok_u, rw_mask, known_modes, known_modes_ok, has_rw,
 * rw_both_u, any_u, continuum + batch_id_7430). Unique
 * gj_access_w_ok_u_7422 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7422_marker[] = "libcgj-batch7422";

/* Soft POSIX access mode W_OK: test for write permission (0x2). */
#define B7422_W_OK ((uint32_t)0x00000002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7422_w_ok(uint32_t u32Mode)
{
	return ((u32Mode & B7422_W_OK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_access_w_ok_u_7422 - 1 if W_OK is set in mode.
 *
 * mode: raw access() mode bitmask
 *
 * Returns 1 when the W_OK bit is set, else 0.
 * Pure mask test; no parent wires. Does not call access().
 */
uint32_t
gj_access_w_ok_u_7422(uint32_t u32Mode)
{
	(void)NULL;
	return b7422_w_ok(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_access_w_ok_u_7422(uint32_t u32Mode)
    __attribute__((alias("gj_access_w_ok_u_7422")));
