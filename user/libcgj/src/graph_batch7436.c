/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7436: chmod known-mode subset check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chmod_known_mode_ok_7436(uint32_t mode);
 *     - Return 1 if mode is a subset of soft ALLPERMS
 *       ((mode & ~0xFFF) == 0) — ACCESSPERMS | S_ISUID | S_ISGID | S_ISVTX.
 *   uint32_t __gj_chmod_known_mode_ok_7436  (alias)
 *   __libcgj_batch7436_marker = "libcgj-batch7436"
 *
 * Exclusive continuum CREATE-ONLY (7431-7440: chmod mode bit stubs —
 * irusr, iwusr, ixusr, irwxu_mask, known_mode, known_mode_ok,
 * has_x, irwxu_u, any_u, continuum + batch_id_7440). Unique
 * gj_chmod_known_mode_ok_7436 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7436_marker[] = "libcgj-batch7436";

/* Soft ALLPERMS: ACCESSPERMS | S_ISUID | S_ISGID | S_ISVTX (07777). */
#define B7436_ALLPERMS ((uint32_t)0x00000FFFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7436_known_mode_ok(uint32_t u32Mode)
{
	return ((u32Mode & ~B7436_ALLPERMS) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chmod_known_mode_ok_7436 - mode is subset of soft ALLPERMS.
 *
 * mode: soft chmod mode bitmask
 *
 * Returns 1 if no bits outside 07777 present; else 0. Zero mode is ok.
 * Pure mask test; no parent wires. Does not call chmod.
 */
uint32_t
gj_chmod_known_mode_ok_7436(uint32_t u32Mode)
{
	(void)NULL;
	return b7436_known_mode_ok(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chmod_known_mode_ok_7436(uint32_t u32Mode)
    __attribute__((alias("gj_chmod_known_mode_ok_7436")));
