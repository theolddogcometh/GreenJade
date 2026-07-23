/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12190: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12190(void);
 *     - Returns the compile-time graph batch number for this TU (12190).
 *   uint32_t __gj_batch_id_12190  (alias)
 *   __libcgj_batch12190_marker = "libcgj-batch12190"
 *
 * Exclusive continuum CREATE-ONLY (12181-12190: cups/print soft id
 * stubs — cups_ok_u_12181, ipp_ok_u_12182, print_job_ok_u_12183,
 * printer_discover_ok_u_12184, ppd_ok_u_12185, filter_ok_u_12186,
 * spool_ok_u_12187, usb_print_ok_u_12188, print_soft_ready_u_12189,
 * batch_id_12190). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12190_marker[] = "libcgj-batch12190";

#define B12190_BATCH_ID  12190u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12190_id(void)
{
	return B12190_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12190 - report this TU's graph batch number.
 *
 * Always returns 12190.
 */
uint32_t
gj_batch_id_12190(void)
{
	(void)NULL;
	return b12190_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12190(void)
    __attribute__((alias("gj_batch_id_12190")));
