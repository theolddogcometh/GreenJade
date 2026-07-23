/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7439: chmod any-access-bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chmod_any_u_7439(uint32_t mode);
 *     - Return 1 if any ACCESSPERMS bit is set
 *       ((mode & 0x1FF) != 0), else 0.
 *   uint32_t __gj_chmod_any_u_7439  (alias)
 *   __libcgj_batch7439_marker = "libcgj-batch7439"
 *
 * Exclusive continuum CREATE-ONLY (7431-7440: chmod mode bit stubs —
 * irusr, iwusr, ixusr, irwxu_mask, known_mode, known_mode_ok,
 * has_x, irwxu_u, any_u, continuum + batch_id_7440). Unique
 * gj_chmod_any_u_7439 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7439_marker[] = "libcgj-batch7439";

/* Soft ACCESSPERMS: S_IRWXU | S_IRWXG | S_IRWXO (0777). */
#define B7439_ACCESSPERMS ((uint32_t)0x000001FFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7439_chmod_any(uint32_t u32Mode)
{
	return ((u32Mode & B7439_ACCESSPERMS) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chmod_any_u_7439 - 1 if any access-permission bit is set.
 *
 * mode: raw chmod/stat mode bitmask
 *
 * Returns 1 when at least one of the 0777 ACCESSPERMS bits is set.
 * Pure mask test; no parent wires. Does not call chmod.
 */
uint32_t
gj_chmod_any_u_7439(uint32_t u32Mode)
{
	(void)NULL;
	return b7439_chmod_any(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chmod_any_u_7439(uint32_t u32Mode)
    __attribute__((alias("gj_chmod_any_u_7439")));
