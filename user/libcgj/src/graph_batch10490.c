/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10490: print soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10490(void);
 *     - Returns the compile-time graph batch number for this TU (10490).
 *   uint32_t __gj_batch_id_10490  (alias)
 *   __libcgj_batch10490_marker = "libcgj-batch10490"
 *
 * Exclusive continuum CREATE-ONLY (10481-10490: print soft id
 * stubs — cups_ok_u_10481, ipp_ok_u_10482, print_job_ok_u_10483,
 * printer_discover_ok_u_10484, ppd_ok_u_10485, filter_ok_u_10486,
 * spool_ok_u_10487, usb_print_ok_u_10488, print_soft_ready_u_10489,
 * batch_id_10490). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. Closes the 10481-10490 print soft wave.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10490_marker[] = "libcgj-batch10490";

#define B10490_BATCH_ID  10490u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10490_id(void)
{
	return B10490_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10490 - report this TU's graph batch number.
 *
 * Always returns 10490. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_10490(void)
{
	(void)NULL;
	return b10490_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10490(void)
    __attribute__((alias("gj_batch_id_10490")));
