/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3640: exclusive string-need wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3640(void);
 *     - Returns the compile-time graph batch number for this TU (3640).
 *   uint32_t __gj_batch_id_3640  (alias)
 *   __libcgj_batch3640_marker = "libcgj-batch3640"
 *
 * Milestone for the exclusive continuum CREATE-ONLY wave (batches
 * 3631-3639: str_replace_n_u, str_split_first_u, str_split_last_u,
 * str_join_need_u, str_repeat_need_u, str_pad_left_need_u,
 * str_pad_right_need_u, str_center_need_u, str_strip_quotes_u). Does
 * NOT redefine gj_batch_id / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3640_marker[] = "libcgj-batch3640";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3640_id(void)
{
	return 3640u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3640 - report this TU's graph batch number.
 *
 * Always returns 3640.
 */
uint32_t
gj_batch_id_3640(void)
{
	(void)NULL;
	return b3640_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3640(void)
    __attribute__((alias("gj_batch_id_3640")));
