/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7431: chmod mode S_IRUSR bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chmod_irusr_u_7431(uint32_t mode);
 *     - Return 1 if S_IRUSR (0400 / 0x100) is set, else 0.
 *   uint32_t __gj_chmod_irusr_u_7431  (alias)
 *   __libcgj_batch7431_marker = "libcgj-batch7431"
 *
 * Exclusive continuum CREATE-ONLY (7431-7440: chmod mode bit stubs —
 * irusr, iwusr, ixusr, irwxu_mask, known_mode, known_mode_ok,
 * has_x, irwxu_u, any_u, continuum + batch_id_7440). Unique
 * gj_chmod_irusr_u_7431 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7431_marker[] = "libcgj-batch7431";

/* Soft S_IRUSR: owner-read permission (0400). */
#define B7431_S_IRUSR ((uint32_t)0x00000100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7431_chmod_irusr(uint32_t u32Mode)
{
	return ((u32Mode & B7431_S_IRUSR) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chmod_irusr_u_7431 - 1 if S_IRUSR is set in mode.
 *
 * mode: raw chmod/stat mode bitmask
 *
 * Returns 1 when the owner-read bit is set, else 0.
 * Pure mask test; no parent wires. Does not call chmod.
 */
uint32_t
gj_chmod_irusr_u_7431(uint32_t u32Mode)
{
	(void)NULL;
	return b7431_chmod_irusr(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chmod_irusr_u_7431(uint32_t u32Mode)
    __attribute__((alias("gj_chmod_irusr_u_7431")));
