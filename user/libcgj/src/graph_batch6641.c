/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6641: signalfd kernel mask byte size stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sfd_mask_bytes_6641(void);
 *     - Returns 8 (soft kernel signalfd4 sizemask / sigset byte length
 *       on the product catalog). Pure-data stub; not a live syscall.
 *   uint32_t __gj_sfd_mask_bytes_6641  (alias)
 *   __libcgj_batch6641_marker = "libcgj-batch6641"
 *
 * Exclusive continuum CREATE-ONLY (6641-6650: signalfd mask stubs —
 * sfd_mask_bytes_6641, sfd_sig_ok_6642, sfd_mask_bit_pos_6643,
 * sfd_mask_set_bit_6644, sfd_mask_clr_bit_6645, sfd_mask_has_bit_6646,
 * sfd_mask_empty_p_6647, sfd_flags_ok_6648, sfd_mask_stub_ready_6649,
 * continuum + batch_id_6650). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6641_marker[] = "libcgj-batch6641";

/* Soft kernel signalfd4 mask size (bytes). */
#define B6641_MASK_BYTES  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6641_mask_bytes(void)
{
	return B6641_MASK_BYTES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfd_mask_bytes_6641 - soft signalfd kernel mask byte length.
 *
 * Always returns 8. Soft compile-time catalog size for signalfd4
 * sizemask; does not issue a syscall. No parent wires.
 */
uint32_t
gj_sfd_mask_bytes_6641(void)
{
	(void)NULL;
	return b6641_mask_bytes();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sfd_mask_bytes_6641(void)
    __attribute__((alias("gj_sfd_mask_bytes_6641")));
