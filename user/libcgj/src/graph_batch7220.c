/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7220: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7220(void);
 *     - Returns the compile-time graph batch number for this TU (7220).
 *   uint32_t __gj_batch_id_7220  (alias)
 *   __libcgj_batch7220_marker = "libcgj-batch7220"
 *
 * Exclusive continuum CREATE-ONLY (7211-7220: poll event bit stubs —
 * in_id, out_id, err_id, has_in, has_out, has_err,
 * events_ok, events_pack, events_errorish, batch_id_7220).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7220_marker[] = "libcgj-batch7220";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7220_id(void)
{
	return 7220u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7220 - report this TU's graph batch number.
 *
 * Always returns 7220.
 */
uint32_t
gj_batch_id_7220(void)
{
	(void)NULL;
	return b7220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7220(void)
    __attribute__((alias("gj_batch_id_7220")));
