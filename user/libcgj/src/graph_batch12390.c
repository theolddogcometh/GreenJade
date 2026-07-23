/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12390: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12390(void);
 *     - Returns the compile-time graph batch number for this TU (12390).
 *   uint32_t __gj_batch_id_12390  (alias)
 *   __libcgj_batch12390_marker = "libcgj-batch12390"
 *
 * Exclusive continuum CREATE-ONLY (12381-12390: cups/print soft id
 * stubs — cups_ok_u_12381, ipp_ok_u_12382, print_job_ok_u_12383,
 * printer_discover_ok_u_12384, ppd_ok_u_12385, filter_ok_u_12386,
 * spool_ok_u_12387, usb_print_ok_u_12388, print_soft_ready_u_12389,
 * batch_id_12390). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12390_marker[] = "libcgj-batch12390";

#define B12390_BATCH_ID  12390u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12390_id(void)
{
	return B12390_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12390 - report this TU's graph batch number.
 *
 * Always returns 12390.
 */
uint32_t
gj_batch_id_12390(void)
{
	(void)NULL;
	return b12390_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12390(void)
    __attribute__((alias("gj_batch_id_12390")));
