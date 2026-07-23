/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8775: soft io_uring IORING_OP_POLL_ADD opcode id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_poll_add_id_8775(void);
 *     - Returns 6 (IORING_OP_POLL_ADD soft catalog value). Pure-data
 *       opcode id stub; does not submit SQEs or poll fds.
 *   uint32_t __gj_ioring_poll_add_id_8775  (alias)
 *   __libcgj_batch8775_marker = "libcgj-batch8775"
 *
 * Exclusive continuum CREATE-ONLY (8771-8780: io_uring opcode id stubs).
 * Unique gj_ioring_poll_add_id_8775 surface only; no multi-def.
 * Distinct from poll_remove (batch8776). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8775_marker[] = "libcgj-batch8775";

/* Soft IORING_OP_POLL_ADD catalog value (enum ordinal 6). */
#define B8775_OP_POLL_ADD  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8775_poll_add_id(void)
{
	return B8775_OP_POLL_ADD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_poll_add_id_8775 - soft IORING_OP_POLL_ADD opcode catalog id.
 *
 * Always returns 6. Soft continuum constant; does not poll anything.
 * No parent wires.
 */
uint32_t
gj_ioring_poll_add_id_8775(void)
{
	(void)NULL;
	return b8775_poll_add_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_poll_add_id_8775(void)
    __attribute__((alias("gj_ioring_poll_add_id_8775")));
