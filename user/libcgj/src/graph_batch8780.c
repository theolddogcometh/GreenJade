/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8780: io_uring opcode id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8780(void);
 *     - Returns the compile-time graph batch number for this TU (8780).
 *   uint32_t __gj_batch_id_8780  (alias)
 *   __libcgj_batch8780_marker = "libcgj-batch8780"
 *
 * Exclusive continuum CREATE-ONLY (8771-8780: io_uring opcode id
 * stubs —
 * ioring_nop_id_8771, ioring_read_id_8772, ioring_write_id_8773,
 * ioring_fsync_id_8774, ioring_poll_add_id_8775, ioring_poll_remove_id_8776,
 * ioring_accept_id_8777, ioring_connect_id_8778, ioring_ready_u_8779,
 * batch_id_8780).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8780_marker[] = "libcgj-batch8780";

#define B8780_BATCH_ID  8780u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8780_id(void)
{
	return B8780_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8780 - report this TU's graph batch number.
 *
 * Always returns 8780.
 */
uint32_t
gj_batch_id_8780(void)
{
	(void)NULL;
	return b8780_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8780(void)
    __attribute__((alias("gj_batch_id_8780")));
