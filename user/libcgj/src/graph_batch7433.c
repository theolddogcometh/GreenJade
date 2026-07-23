/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7433: chmod mode S_IXUSR bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chmod_ixusr_u_7433(uint32_t mode);
 *     - Return 1 if S_IXUSR (0100 / 0x40) is set, else 0.
 *   uint32_t __gj_chmod_ixusr_u_7433  (alias)
 *   __libcgj_batch7433_marker = "libcgj-batch7433"
 *
 * Exclusive continuum CREATE-ONLY (7431-7440: chmod mode bit stubs —
 * irusr, iwusr, ixusr, irwxu_mask, known_mode, known_mode_ok,
 * has_x, irwxu_u, any_u, continuum + batch_id_7440). Unique
 * gj_chmod_ixusr_u_7433 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7433_marker[] = "libcgj-batch7433";

/* Soft S_IXUSR: owner-execute permission (0100). */
#define B7433_S_IXUSR ((uint32_t)0x00000040u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7433_chmod_ixusr(uint32_t u32Mode)
{
	return ((u32Mode & B7433_S_IXUSR) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chmod_ixusr_u_7433 - 1 if S_IXUSR is set in mode.
 *
 * mode: raw chmod/stat mode bitmask
 *
 * Returns 1 when the owner-execute bit is set, else 0.
 * Pure mask test; no parent wires. Does not call chmod.
 */
uint32_t
gj_chmod_ixusr_u_7433(uint32_t u32Mode)
{
	(void)NULL;
	return b7433_chmod_ixusr(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chmod_ixusr_u_7433(uint32_t u32Mode)
    __attribute__((alias("gj_chmod_ixusr_u_7433")));
