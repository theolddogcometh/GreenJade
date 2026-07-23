/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8355: signal-number valid-range predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sig_valid_u_8355(uint32_t signo);
 *     - Return 1 if signo is in the soft Linux range 1..64, else 0.
 *   uint32_t __gj_sig_valid_u_8355  (alias)
 *   __libcgj_batch8355_marker = "libcgj-batch8355"
 *
 * Exclusive continuum CREATE-ONLY (8351-8360: signal/errno id stubs —
 * errno_ok_u, errno_inval_id, errno_nomem_id, errno_again_id,
 * sig_valid_u, sig_term_id, sig_int_id, sig_mask_empty_u,
 * sig_mask_full_u, batch_id_8360). Unique gj_sig_valid_u_8355 surface
 * only; no multi-def. No real signal API (no raise/kill/sigaction).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8355_marker[] = "libcgj-batch8355";

/*
 * Soft Linux x86_64 signal range (local; no signal.h).
 * 0 is not a valid signal; _NSIG is often 65 so valid numbers are 1..64.
 */
#define B8355_SIG_MIN  1u
#define B8355_SIG_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8355_sig_valid(uint32_t u32Signo)
{
	if (u32Signo >= B8355_SIG_MIN && u32Signo <= B8355_SIG_MAX) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sig_valid_u_8355 - 1 if soft signal number is in 1..64.
 *
 * signo: candidate signal number (unsigned view)
 *
 * Returns 1 for 1..64 inclusive, else 0. Catalog range check only;
 * does not deliver or mask signals. No parent wires.
 */
uint32_t
gj_sig_valid_u_8355(uint32_t u32Signo)
{
	(void)NULL;
	return b8355_sig_valid(u32Signo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sig_valid_u_8355(uint32_t u32Signo)
    __attribute__((alias("gj_sig_valid_u_8355")));
