/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7434: chmod mode S_IRWXU mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chmod_irwxu_mask_7434(void);
 *     - Returns soft S_IRWXU mask (0700 / 0x1C0):
 *       S_IRUSR | S_IWUSR | S_IXUSR.
 *   uint32_t __gj_chmod_irwxu_mask_7434  (alias)
 *   __libcgj_batch7434_marker = "libcgj-batch7434"
 *
 * Exclusive continuum CREATE-ONLY (7431-7440: chmod mode bit stubs —
 * irusr, iwusr, ixusr, irwxu_mask, known_mode, known_mode_ok,
 * has_x, irwxu_u, any_u, continuum + batch_id_7440). Unique
 * gj_chmod_irwxu_mask_7434 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7434_marker[] = "libcgj-batch7434";

/*
 * Soft S_IRWXU composite (compile-time tags only):
 *   S_IRUSR 0400 (0x100)
 *   S_IWUSR 0200 (0x080)
 *   S_IXUSR 0100 (0x040)
 * Soft value 0x1C0. Not a kernel probe.
 */
#define B7434_S_IRUSR ((uint32_t)0x00000100u)
#define B7434_S_IWUSR ((uint32_t)0x00000080u)
#define B7434_S_IXUSR ((uint32_t)0x00000040u)
#define B7434_S_IRWXU \
	(B7434_S_IRUSR | B7434_S_IWUSR | B7434_S_IXUSR)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7434_irwxu_mask(void)
{
	return B7434_S_IRWXU;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chmod_irwxu_mask_7434 - soft S_IRWXU composite mask.
 *
 * Always returns 0x1C0. Soft pure-data product tag; does not call
 * chmod. No parent wires.
 */
uint32_t
gj_chmod_irwxu_mask_7434(void)
{
	(void)NULL;
	return b7434_irwxu_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chmod_irwxu_mask_7434(void)
    __attribute__((alias("gj_chmod_irwxu_mask_7434")));
