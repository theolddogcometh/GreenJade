/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4290: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4290(void);
 *     - Returns the compile-time graph batch number for this TU (4290).
 *   uint32_t __gj_batch_id_4290  (alias)
 *   __libcgj_batch4290_marker = "libcgj-batch4290"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ascii_isdigit_u,
 * ascii_isxdigit_u, ascii_isalpha_u, ascii_isalnum_u, ascii_isspace_u,
 * ascii_toupper_u, ascii_tolower_u, ascii_isprint_u, hex_nibble_val_u,
 * batch_id_4290.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4290_marker[] = "libcgj-batch4290";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4290_id(void)
{
	return 4290u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4290 - report this TU's graph batch number.
 *
 * Always returns 4290.
 */
uint32_t
gj_batch_id_4290(void)
{
	(void)NULL;
	return b4290_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4290(void)
    __attribute__((alias("gj_batch_id_4290")));
