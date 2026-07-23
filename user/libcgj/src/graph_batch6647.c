/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6647: signalfd mask empty predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfd_mask_empty_p_6647(uint64_t mask);
 *     - Returns 1 when mask is zero (no signals selected), else 0.
 *       Soft pure-integer empty-set predicate for signalfd masks.
 *   uint32_t __gj_sfd_mask_empty_p_6647  (alias)
 *   __libcgj_batch6647_marker = "libcgj-batch6647"
 *
 * Exclusive continuum CREATE-ONLY (6641-6650: signalfd mask stubs).
 * Unique gj_sfd_mask_empty_p_6647 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6647_marker[] = "libcgj-batch6647";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6647_empty_p(uint64_t uMask)
{
	return (uMask == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfd_mask_empty_p_6647 - soft signalfd empty-mask predicate.
 *
 * mask: 64-bit soft mask word.
 * Returns 1 if no bits are set, else 0. No parent wires.
 */
uint32_t
gj_sfd_mask_empty_p_6647(uint64_t mask)
{
	(void)NULL;
	return b6647_empty_p(mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfd_mask_empty_p_6647(uint64_t mask)
    __attribute__((alias("gj_sfd_mask_empty_p_6647")));
