/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6643: signalfd mask bit position stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfd_mask_bit_pos_6643(uint32_t sig);
 *     - Returns (sig - 1) when sig is in [1, 64], else 0xffffffff.
 *       Soft pure-integer bit lane index for a 64-bit mask word.
 *   uint32_t __gj_sfd_mask_bit_pos_6643  (alias)
 *   __libcgj_batch6643_marker = "libcgj-batch6643"
 *
 * Exclusive continuum CREATE-ONLY (6641-6650: signalfd mask stubs).
 * Unique gj_sfd_mask_bit_pos_6643 surface only; no multi-def.
 * Self-contained (does not call sfd_sig_ok). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6643_marker[] = "libcgj-batch6643";

#define B6643_SIG_MIN   1u
#define B6643_SIG_MAX   64u
#define B6643_BAD_POS   0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6643_bit_pos(uint32_t uSig)
{
	if (uSig < B6643_SIG_MIN || uSig > B6643_SIG_MAX) {
		return B6643_BAD_POS;
	}
	return (uSig - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfd_mask_bit_pos_6643 - soft signalfd mask bit index for signal.
 *
 * sig: candidate signal number (1..64 valid).
 * Returns bit position 0..63, or 0xffffffff when out of range.
 * No parent wires.
 */
uint32_t
gj_sfd_mask_bit_pos_6643(uint32_t sig)
{
	(void)NULL;
	return b6643_bit_pos(sig);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfd_mask_bit_pos_6643(uint32_t sig)
    __attribute__((alias("gj_sfd_mask_bit_pos_6643")));
