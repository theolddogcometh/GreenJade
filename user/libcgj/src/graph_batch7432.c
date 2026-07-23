/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7432: chmod mode S_IWUSR bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chmod_iwusr_u_7432(uint32_t mode);
 *     - Return 1 if S_IWUSR (0200 / 0x80) is set, else 0.
 *   uint32_t __gj_chmod_iwusr_u_7432  (alias)
 *   __libcgj_batch7432_marker = "libcgj-batch7432"
 *
 * Exclusive continuum CREATE-ONLY (7431-7440: chmod mode bit stubs —
 * irusr, iwusr, ixusr, irwxu_mask, known_mode, known_mode_ok,
 * has_x, irwxu_u, any_u, continuum + batch_id_7440). Unique
 * gj_chmod_iwusr_u_7432 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7432_marker[] = "libcgj-batch7432";

/* Soft S_IWUSR: owner-write permission (0200). */
#define B7432_S_IWUSR ((uint32_t)0x00000080u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7432_chmod_iwusr(uint32_t u32Mode)
{
	return ((u32Mode & B7432_S_IWUSR) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chmod_iwusr_u_7432 - 1 if S_IWUSR is set in mode.
 *
 * mode: raw chmod/stat mode bitmask
 *
 * Returns 1 when the owner-write bit is set, else 0.
 * Pure mask test; no parent wires. Does not call chmod.
 */
uint32_t
gj_chmod_iwusr_u_7432(uint32_t u32Mode)
{
	(void)NULL;
	return b7432_chmod_iwusr(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chmod_iwusr_u_7432(uint32_t u32Mode)
    __attribute__((alias("gj_chmod_iwusr_u_7432")));
