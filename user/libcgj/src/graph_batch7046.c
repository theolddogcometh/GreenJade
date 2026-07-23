/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7046: io_pgetevents timeout optional stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_io_pgetevents_timeout_opt_u_7046(uint32_t timeout_present);
 *     - Return 1 always (timeout pointer is optional / may be NULL).
 *   uint32_t __gj_io_pgetevents_timeout_opt_u_7046  (alias)
 *   __libcgj_batch7046_marker = "libcgj-batch7046"
 *
 * Exclusive continuum CREATE-ONLY (7041-7050: io_pgetevents stubs —
 * nr, min_ok, max_ok, range_ok, events_req, timeout_opt, sigmask_opt,
 * pack, args_ok, batch_id_7050). Unique
 * gj_io_pgetevents_timeout_opt_u_7046 surface only; no multi-def. Does
 * NOT redefine io_pgetevents. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7046_marker[] = "libcgj-batch7046";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7046_io_pgetevents_timeout_opt(uint32_t u32TimeoutPresent)
{
	/* timeout may be NULL (infinite wait) or non-NULL; always soft-ok. */
	(void)u32TimeoutPresent;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_io_pgetevents_timeout_opt_u_7046 - soft timeout optional check.
 *
 * timeout_present: non-zero if timeout pointer is non-NULL (soft flag)
 *
 * Always returns 1. Documents that timeout is optional for
 * io_pgetevents; does not call the syscall. No parent wires.
 */
uint32_t
gj_io_pgetevents_timeout_opt_u_7046(uint32_t u32TimeoutPresent)
{
	(void)NULL;
	return b7046_io_pgetevents_timeout_opt(u32TimeoutPresent);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_io_pgetevents_timeout_opt_u_7046(uint32_t u32TimeoutPresent)
    __attribute__((alias("gj_io_pgetevents_timeout_opt_u_7046")));
