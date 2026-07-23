/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7438: chmod full S_IRWXU mask-set stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chmod_irwxu_u_7438(uint32_t mode);
 *     - Return 1 if all S_IRWXU bits are set
 *       (S_IRUSR|S_IWUSR|S_IXUSR == 0x1C0 subset of mode), else 0.
 *   uint32_t __gj_chmod_irwxu_u_7438  (alias)
 *   __libcgj_batch7438_marker = "libcgj-batch7438"
 *
 * Exclusive continuum CREATE-ONLY (7431-7440: chmod mode bit stubs —
 * irusr, iwusr, ixusr, irwxu_mask, known_mode, known_mode_ok,
 * has_x, irwxu_u, any_u, continuum + batch_id_7440). Unique
 * gj_chmod_irwxu_u_7438 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7438_marker[] = "libcgj-batch7438";

/* Soft S_IRWXU: full owner rwx permission trio. */
#define B7438_S_IRUSR ((uint32_t)0x00000100u)
#define B7438_S_IWUSR ((uint32_t)0x00000080u)
#define B7438_S_IXUSR ((uint32_t)0x00000040u)
#define B7438_S_IRWXU \
	(B7438_S_IRUSR | B7438_S_IWUSR | B7438_S_IXUSR)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7438_chmod_irwxu(uint32_t u32Mode)
{
	return ((u32Mode & B7438_S_IRWXU) == B7438_S_IRWXU) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chmod_irwxu_u_7438 - 1 if full S_IRWXU mask is set.
 *
 * mode: raw chmod/stat mode bitmask
 *
 * Returns 1 when S_IRUSR, S_IWUSR, and S_IXUSR are all present.
 * Self-contained; does not call sibling helpers. No parent wires.
 */
uint32_t
gj_chmod_irwxu_u_7438(uint32_t u32Mode)
{
	(void)NULL;
	return b7438_chmod_irwxu(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chmod_irwxu_u_7438(uint32_t u32Mode)
    __attribute__((alias("gj_chmod_irwxu_u_7438")));
