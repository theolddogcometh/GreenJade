/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13190: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13190(void);
 *     - Returns the compile-time graph batch number for this TU (13190).
 *   uint32_t __gj_batch_id_13190  (alias)
 *   __libcgj_batch13190_marker = "libcgj-batch13190"
 *
 * Exclusive continuum CREATE-ONLY (13181-13190: cups/print soft id
 * stubs — cups_ok_u_13181, ipp_ok_u_13182, print_job_ok_u_13183,
 * printer_discover_ok_u_13184, ppd_ok_u_13185, filter_ok_u_13186,
 * spool_ok_u_13187, usb_print_ok_u_13188, print_soft_ready_u_13189,
 * batch_id_13190).
 * Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_12990 (batch12990),
 * gj_batch_id_12790 (batch12790), gj_batch_id_12590 (batch12590),
 * gj_batch_id_12390 (batch12390) and gj_batch_id_12190 (batch12190).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13190_marker[] = "libcgj-batch13190";

#define B13190_BATCH_ID  13190u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13190_id(void)
{
	return B13190_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13190 - report this TU's graph batch number.
 *
 * Always returns 13190.
 */
uint32_t
gj_batch_id_13190(void)
{
	(void)NULL;
	return b13190_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13190(void)
    __attribute__((alias("gj_batch_id_13190")));
