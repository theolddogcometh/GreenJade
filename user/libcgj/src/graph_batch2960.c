/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2960: wave identity for exclusive string helpers.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2960(void);
 *     - Returns the compile-time graph batch number for this TU (2960).
 *   uint32_t __gj_batch_id_2960  (alias)
 *   __libcgj_batch2960_marker = "libcgj-batch2960"
 *
 * Milestone for the exclusive string helpers wave (batches 2951-2959:
 * str_tolower_n_u, str_toupper_n_u, str_eq_ci_u, str_starts_ci_u,
 * str_ends_ci_u, str_trim_left_u, str_trim_right_u, str_count_char_u,
 * str_replace_char_u). Does NOT redefine gj_batch_id / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2960_marker[] = "libcgj-batch2960";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2960_id(void)
{
	return 2960u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2960 - report this TU's graph batch number.
 *
 * Always returns 2960.
 */
uint32_t
gj_batch_id_2960(void)
{
	(void)NULL;
	return b2960_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2960(void)
    __attribute__((alias("gj_batch_id_2960")));
