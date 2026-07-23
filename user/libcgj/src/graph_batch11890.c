/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11890: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11890(void);
 *     - Returns the compile-time graph batch number for this TU (11890).
 *   uint32_t __gj_batch_id_11890  (alias)
 *   __libcgj_batch11890_marker = "libcgj-batch11890"
 *
 * Exclusive continuum CREATE-ONLY (11881-11890: cups/print soft id
 * stubs — cups_ok_u_11881, ipp_ok_u_11882, print_job_ok_u_11883,
 * printer_discover_ok_u_11884, ppd_ok_u_11885, filter_ok_u_11886,
 * spool_ok_u_11887, usb_print_ok_u_11888, print_soft_ready_u_11889,
 * batch_id_11890). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11890_marker[] = "libcgj-batch11890";

#define B11890_BATCH_ID  11890u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11890_id(void)
{
	return B11890_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11890 - report this TU's graph batch number.
 *
 * Always returns 11890.
 */
uint32_t
gj_batch_id_11890(void)
{
	(void)NULL;
	return b11890_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11890(void)
    __attribute__((alias("gj_batch_id_11890")));
