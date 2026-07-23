/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8071: strlen SIZE_MAX id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strlen_size_max_id_8071(void);
 *     - Return soft SIZE_MAX catalog constant (0xffffffff).
 *   uint32_t __gj_strlen_size_max_id_8071  (alias)
 *   __libcgj_batch8071_marker = "libcgj-batch8071"
 *
 * Exclusive continuum CREATE-ONLY (8071-8080: strlen max stubs —
 * size_max_id, ssize_max_id, empty_len_id, len_is_zero, len_is_max,
 * len_below_max, len_ok, len_clamp, len_errorish, batch_id_8080).
 * Unique gj_strlen_size_max_id_8071 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8071_marker[] = "libcgj-batch8071";

/* Soft 32-bit SIZE_MAX catalog value for strlen bound checks. */
#define B8071_SIZE_MAX ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8071_strlen_size_max_id(void)
{
	return B8071_SIZE_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strlen_size_max_id_8071 - soft SIZE_MAX constant.
 *
 * Always returns 0xffffffff. Catalog id only; does not call strlen(3).
 * No parent wires.
 */
uint32_t
gj_strlen_size_max_id_8071(void)
{
	(void)NULL;
	return b8071_strlen_size_max_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strlen_size_max_id_8071(void)
    __attribute__((alias("gj_strlen_size_max_id_8071")));
