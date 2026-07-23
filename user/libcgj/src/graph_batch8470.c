/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8470: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8470(void);
 *     - Returns the compile-time graph batch number for this TU (8470).
 *   uint32_t __gj_batch_id_8470  (alias)
 *   __libcgj_batch8470_marker = "libcgj-batch8470"
 *
 * Exclusive continuum CREATE-ONLY (8461-8470: uuid/layout tag stubs —
 * ver4_id, var_rfc_id, nil_ok, str_len_id, bin_len_id, dash_pos_ok,
 * hex_ok, parse_ready, fmt_ready, batch_id_8470).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8470_marker[] = "libcgj-batch8470";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8470_id(void)
{
	return 8470u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8470 - report this TU's graph batch number.
 *
 * Always returns 8470.
 */
uint32_t
gj_batch_id_8470(void)
{
	(void)NULL;
	return b8470_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8470(void)
    __attribute__((alias("gj_batch_id_8470")));
