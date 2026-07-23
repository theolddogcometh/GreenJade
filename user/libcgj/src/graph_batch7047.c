/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7047: io_pgetevents sigmask optional stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_io_pgetevents_sigmask_opt_u_7047(uint32_t sigmask_present);
 *     - Return 1 always (sigmask pointer is optional / may be NULL).
 *   uint32_t __gj_io_pgetevents_sigmask_opt_u_7047  (alias)
 *   __libcgj_batch7047_marker = "libcgj-batch7047"
 *
 * Exclusive continuum CREATE-ONLY (7041-7050: io_pgetevents stubs —
 * nr, min_ok, max_ok, range_ok, events_req, timeout_opt, sigmask_opt,
 * pack, args_ok, batch_id_7050). Unique
 * gj_io_pgetevents_sigmask_opt_u_7047 surface only; no multi-def. Does
 * NOT redefine io_pgetevents. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7047_marker[] = "libcgj-batch7047";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7047_io_pgetevents_sigmask_opt(uint32_t u32SigmaskPresent)
{
	/* sigmask may be NULL (no temporary mask change); always soft-ok. */
	(void)u32SigmaskPresent;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_io_pgetevents_sigmask_opt_u_7047 - soft sigmask optional check.
 *
 * sigmask_present: non-zero if sigmask pointer is non-NULL (soft flag)
 *
 * Always returns 1. Documents that sigmask is the distinguishing
 * optional arg of io_pgetevents vs io_getevents; does not call the
 * syscall. No parent wires.
 */
uint32_t
gj_io_pgetevents_sigmask_opt_u_7047(uint32_t u32SigmaskPresent)
{
	(void)NULL;
	return b7047_io_pgetevents_sigmask_opt(u32SigmaskPresent);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_io_pgetevents_sigmask_opt_u_7047(uint32_t u32SigmaskPresent)
    __attribute__((alias("gj_io_pgetevents_sigmask_opt_u_7047")));
