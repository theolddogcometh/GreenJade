/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7435: chmod known access-mode soft baseline.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chmod_known_mode_7435(void);
 *     - Returns soft ACCESSPERMS mask (0777 / 0x1FF):
 *       S_IRWXU | S_IRWXG | S_IRWXO.
 *   uint32_t __gj_chmod_known_mode_7435  (alias)
 *   __libcgj_batch7435_marker = "libcgj-batch7435"
 *
 * Exclusive continuum CREATE-ONLY (7431-7440: chmod mode bit stubs —
 * irusr, iwusr, ixusr, irwxu_mask, known_mode, known_mode_ok,
 * has_x, irwxu_u, any_u, continuum + batch_id_7440). Unique
 * gj_chmod_known_mode_7435 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7435_marker[] = "libcgj-batch7435";

/*
 * Soft known chmod access bits baseline (compile-time tags only):
 *   S_IRWXU 0700 | S_IRWXG 0070 | S_IRWXO 0007 = ACCESSPERMS 0777
 * Soft value 0x1FF. Not a kernel probe.
 */
#define B7435_ACCESSPERMS ((uint32_t)0x000001FFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7435_known_mode(void)
{
	return B7435_ACCESSPERMS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chmod_known_mode_7435 - soft known chmod access-mode mask.
 *
 * Always returns 0x1FF. Soft pure-data product tag; does not call
 * chmod. No parent wires.
 */
uint32_t
gj_chmod_known_mode_7435(void)
{
	(void)NULL;
	return b7435_known_mode();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_chmod_known_mode_7435(void)
    __attribute__((alias("gj_chmod_known_mode_7435")));
