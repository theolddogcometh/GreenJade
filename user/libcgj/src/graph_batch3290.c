/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3290: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3290(void);
 *     - Returns the compile-time graph batch number for this TU (3290).
 *   uint32_t __gj_batch_id_3290  (alias)
 *   __libcgj_batch3290_marker = "libcgj-batch3290"
 *
 * Exclusive continuum CREATE-ONLY (3281-3290): str_empty_p_u,
 * str_nonempty_p_u, str_has_prefix_u, str_has_suffix_u,
 * str_contains_char_u, str_contains_str_u, str_index_of_u,
 * str_last_index_of_u, str_count_substr_u, batch_id_3290.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3290_marker[] = "libcgj-batch3290";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3290_id(void)
{
return 3290u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3290 - report this TU's graph batch number.
 *
 * Always returns 3290.
 */
uint32_t
gj_batch_id_3290(void)
{
(void)NULL;
return b3290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3290(void)
    __attribute__((alias("gj_batch_id_3290")));
