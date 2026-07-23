/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9940: xen soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9940(void);
 *     - Returns the compile-time graph batch number for this TU (9940).
 *   uint32_t __gj_batch_id_9940  (alias)
 *   __libcgj_batch9940_marker = "libcgj-batch9940"
 *
 * Exclusive continuum CREATE-ONLY (9931-9940: xen soft id stubs —
 * xen_domain_ok_u_9931, xen_grant_ok_u_9932, xen_evtchn_ok_u_9933,
 * xen_xenbus_ok_u_9934, xen_netfront_ok_u_9935, xen_blkfront_ok_u_9936,
 * xen_console_ok_u_9937, xen_balloon_ok_u_9938, xen_ready_u_9939,
 * batch_id_9940). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128. No Xen implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9940_marker[] = "libcgj-batch9940";

#define B9940_BATCH_ID  9940u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9940_id(void)
{
	return B9940_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9940 - report this TU's graph batch number.
 *
 * Always returns 9940. Link-time presence tags the exclusive xen soft
 * id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9940(void)
{
	(void)NULL;
	return b9940_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9940(void)
    __attribute__((alias("gj_batch_id_9940")));
