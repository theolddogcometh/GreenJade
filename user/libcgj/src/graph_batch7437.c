/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7437: chmod has-execute compound stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chmod_has_x_u_7437(uint32_t mode);
 *     - Return 1 if any execute bit is set
 *       (S_IXUSR | S_IXGRP | S_IXOTH), else 0.
 *   uint32_t __gj_chmod_has_x_u_7437  (alias)
 *   __libcgj_batch7437_marker = "libcgj-batch7437"
 *
 * Exclusive continuum CREATE-ONLY (7431-7440: chmod mode bit stubs —
 * irusr, iwusr, ixusr, irwxu_mask, known_mode, known_mode_ok,
 * has_x, irwxu_u, any_u, continuum + batch_id_7440). Unique
 * gj_chmod_has_x_u_7437 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7437_marker[] = "libcgj-batch7437";

/* Soft execute bits: S_IXUSR | S_IXGRP | S_IXOTH. */
#define B7437_S_IXUSR ((uint32_t)0x00000040u)
#define B7437_S_IXGRP ((uint32_t)0x00000008u)
#define B7437_S_IXOTH ((uint32_t)0x00000001u)
#define B7437_X_MASK \
	(B7437_S_IXUSR | B7437_S_IXGRP | B7437_S_IXOTH)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7437_chmod_has_x(uint32_t u32Mode)
{
	return ((u32Mode & B7437_X_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chmod_has_x_u_7437 - 1 if any execute permission bit is set.
 *
 * mode: raw chmod/stat mode bitmask
 *
 * Returns 1 when S_IXUSR and/or S_IXGRP and/or S_IXOTH is present.
 * Self-contained; does not call sibling gj_chmod_* helpers. No parent wires.
 */
uint32_t
gj_chmod_has_x_u_7437(uint32_t u32Mode)
{
	(void)NULL;
	return b7437_chmod_has_x(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chmod_has_x_u_7437(uint32_t u32Mode)
    __attribute__((alias("gj_chmod_has_x_u_7437")));
