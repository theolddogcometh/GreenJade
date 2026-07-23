/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6642: signalfd signal number soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfd_sig_ok_6642(uint32_t sig);
 *     - Returns 1 when sig is in soft real-time/standard catalog range
 *       [1, 64], else 0. Pure-data range stub; not a live kill/signalfd.
 *   uint32_t __gj_sfd_sig_ok_6642  (alias)
 *   __libcgj_batch6642_marker = "libcgj-batch6642"
 *
 * Exclusive continuum CREATE-ONLY (6641-6650: signalfd mask stubs).
 * Unique gj_sfd_sig_ok_6642 surface only; no multi-def. Soft NSIG=64
 * catalog membership. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6642_marker[] = "libcgj-batch6642";

/* Soft signal catalog: 1..64 inclusive (standard + RT window). */
#define B6642_SIG_MIN  1u
#define B6642_SIG_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6642_sig_ok(uint32_t uSig)
{
	if (uSig < B6642_SIG_MIN || uSig > B6642_SIG_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfd_sig_ok_6642 - soft signalfd mask signal number membership.
 *
 * sig: candidate POSIX/RT signal number.
 * Returns 1 if sig is in [1, 64], else 0. Does not call libc or kernel.
 * No parent wires.
 */
uint32_t
gj_sfd_sig_ok_6642(uint32_t sig)
{
	(void)NULL;
	return b6642_sig_ok(sig);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfd_sig_ok_6642(uint32_t sig)
    __attribute__((alias("gj_sfd_sig_ok_6642")));
