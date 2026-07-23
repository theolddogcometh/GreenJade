/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10687: print-spool ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_spool_ok_u_10687(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print-spool directory/queue probe for the print soft continuum.
 *   uint32_t __gj_spool_ok_u_10687  (alias)
 *   __libcgj_batch10687_marker = "libcgj-batch10687"
 *
 * Exclusive continuum CREATE-ONLY (10681-10690: print soft id
 * stubs — cups_ok_u_10681, ipp_ok_u_10682, print_job_ok_u_10683,
 * printer_discover_ok_u_10684, ppd_ok_u_10685, filter_ok_u_10686,
 * spool_ok_u_10687, usb_print_ok_u_10688, print_soft_ready_u_10689,
 * batch_id_10690). Unique surface only; no multi-def. Distinct from
 * prior spool soft tags (10187, 10487). No parent wires. No __int128.
 * No spool implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10687_marker[] = "libcgj-batch10687";

/* Soft spool-ok lamp: always off (not a real spool probe). */
#define B10687_SPOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10687_spool_ok(void)
{
	return B10687_SPOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spool_ok_u_10687 - print-spool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect spool paths
 * or call libc. No parent wires.
 */
uint32_t
gj_spool_ok_u_10687(void)
{
	(void)NULL;
	return b10687_spool_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spool_ok_u_10687(void)
    __attribute__((alias("gj_spool_ok_u_10687")));
