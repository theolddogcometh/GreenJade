/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12790: cups/print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12790(void);
 *     - Returns the compile-time graph batch number for this TU (12790).
 *   uint32_t __gj_batch_id_12790  (alias)
 *   __libcgj_batch12790_marker = "libcgj-batch12790"
 *
 * Exclusive continuum CREATE-ONLY (12781-12790: cups/print soft id
 * stubs — cups_ok_u_12781, ipp_ok_u_12782, print_job_ok_u_12783,
 * printer_discover_ok_u_12784, ppd_ok_u_12785, filter_ok_u_12786,
 * spool_ok_u_12787, usb_print_ok_u_12788, print_soft_ready_u_12789,
 * batch_id_12790). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. Distinct from gj_batch_id_12590 (batch12590),
 * gj_batch_id_12390 (batch12390) and gj_batch_id_12190 (batch12190).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12790_marker[] = "libcgj-batch12790";

#define B12790_BATCH_ID  12790u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12790_id(void)
{
	return B12790_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12790 - report this TU's graph batch number.
 *
 * Always returns 12790.
 */
uint32_t
gj_batch_id_12790(void)
{
	(void)NULL;
	return b12790_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12790(void)
    __attribute__((alias("gj_batch_id_12790")));
