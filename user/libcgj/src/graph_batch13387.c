/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13387: spool ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_spool_ok_u_13387(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       print spool / CUPS queue probe for the cups/print continuum.
 *   uint32_t __gj_spool_ok_u_13387  (alias)
 *   __libcgj_batch13387_marker = "libcgj-batch13387"
 *
 * Exclusive continuum CREATE-ONLY (13381-13390: cups/print soft id
 * stubs — cups_ok_u_13381, ipp_ok_u_13382, print_job_ok_u_13383,
 * printer_discover_ok_u_13384, ppd_ok_u_13385, filter_ok_u_13386,
 * spool_ok_u_13387, usb_print_ok_u_13388, print_soft_ready_u_13389,
 * batch_id_13390).
 * Unique surface only; no multi-def. Distinct from
 * gj_spool_ok_u_13187 (batch13187), gj_spool_ok_u_12987 (batch12987),
 * gj_spool_ok_u_12787 (batch12787), gj_spool_ok_u_12587 (batch12587),
 * gj_spool_ok_u_12387 (batch12387), gj_spool_ok_u_10187 (batch10187)
 * and gj_cups_queue_ok_u_9446 (batch9446). No parent wires. No
 * __int128. No CUPS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13387_marker[] = "libcgj-batch13387";

/* Soft spool-ok lamp: always off (not a real spool/queue probe). */
#define B13387_SPOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13387_spool_ok(void)
{
	return B13387_SPOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spool_ok_u_13387 - spool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect print queues
 * or call libc. No parent wires.
 */
uint32_t
gj_spool_ok_u_13387(void)
{
	(void)NULL;
	return b13387_spool_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spool_ok_u_13387(void)
    __attribute__((alias("gj_spool_ok_u_13387")));
