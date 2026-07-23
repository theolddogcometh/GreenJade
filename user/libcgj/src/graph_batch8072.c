/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8072: strlen SSIZE_MAX id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strlen_ssize_max_id_8072(void);
 *     - Return soft SSIZE_MAX catalog constant (0x7fffffff).
 *   uint32_t __gj_strlen_ssize_max_id_8072  (alias)
 *   __libcgj_batch8072_marker = "libcgj-batch8072"
 *
 * Exclusive continuum CREATE-ONLY (8071-8080: strlen max stubs —
 * size_max_id, ssize_max_id, empty_len_id, len_is_zero, len_is_max,
 * len_below_max, len_ok, len_clamp, len_errorish, batch_id_8080).
 * Unique gj_strlen_ssize_max_id_8072 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8072_marker[] = "libcgj-batch8072";

/* Soft 31-bit SSIZE_MAX catalog value (common glibc ssize_t max). */
#define B8072_SSIZE_MAX ((uint32_t)0x7fffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8072_strlen_ssize_max_id(void)
{
	return B8072_SSIZE_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strlen_ssize_max_id_8072 - soft SSIZE_MAX constant.
 *
 * Always returns 0x7fffffff. Catalog id only; does not call strlen(3).
 * No parent wires.
 */
uint32_t
gj_strlen_ssize_max_id_8072(void)
{
	(void)NULL;
	return b8072_strlen_ssize_max_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strlen_ssize_max_id_8072(void)
    __attribute__((alias("gj_strlen_ssize_max_id_8072")));
