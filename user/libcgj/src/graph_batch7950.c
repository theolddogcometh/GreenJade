/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7950: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7950(void);
 *     - Returns the compile-time graph batch number for this TU (7950).
 *   uint32_t __gj_batch_id_7950  (alias)
 *   __libcgj_batch7950_marker = "libcgj-batch7950"
 *
 * Exclusive continuum CREATE-ONLY (7941-7950: strtod parse stubs —
 * is_ws, sign_code, digit_val, hexdigit_val, is_x_marker, is_exp,
 * is_binexp, is_dp, special_inf_id, batch_id_7950).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7950_marker[] = "libcgj-batch7950";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7950_id(void)
{
	return 7950u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7950 - report this TU's graph batch number.
 *
 * Always returns 7950. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7950(void)
{
	(void)NULL;
	return b7950_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7950(void)
    __attribute__((alias("gj_batch_id_7950")));
