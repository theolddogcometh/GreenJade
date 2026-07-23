/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12990: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12990(void);
 *     - Returns the compile-time graph batch number for this TU (12990).
 *   uint32_t __gj_batch_id_12990  (alias)
 *   __libcgj_batch12990_marker = "libcgj-batch12990"
 *
 * Exclusive continuum CREATE-ONLY (12981-12990: cups/print soft id
 * stubs — cups_ok_u_12981, ipp_ok_u_12982, print_job_ok_u_12983,
 * printer_discover_ok_u_12984, ppd_ok_u_12985, filter_ok_u_12986,
 * spool_ok_u_12987, usb_print_ok_u_12988, print_soft_ready_u_12989,
 * batch_id_12990). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_12790 (batch12790),
 * gj_batch_id_12590 (batch12590), gj_batch_id_12390 (batch12390) and
 * gj_batch_id_12190 (batch12190). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12990_marker[] = "libcgj-batch12990";

#define B12990_BATCH_ID  12990u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12990_id(void)
{
	return B12990_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12990 - report this TU's graph batch number.
 *
 * Always returns 12990.
 */
uint32_t
gj_batch_id_12990(void)
{
	(void)NULL;
	return b12990_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12990(void)
    __attribute__((alias("gj_batch_id_12990")));
