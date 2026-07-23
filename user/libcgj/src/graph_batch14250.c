/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14250: seatd/polkit soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14250(void);
 *     - Returns the compile-time graph batch number for this TU (14250).
 *   uint32_t __gj_batch_id_14250  (alias)
 *   __libcgj_batch14250_marker = "libcgj-batch14250"
 *
 * Exclusive continuum CREATE-ONLY (14241-14250: seatd/polkit soft —
 * polkit_ok_u_14241, seatd_ok_u_14242, libseat_ok_u_14243,
 * pk_action_ok_u_14244, pk_subject_ok_u_14245, pk_details_ok_u_14246,
 * seat_session_ok_u_14247, pk_authority_ok_u_14248,
 * seatd_polkit_ready_u_14249, batch_id_14250). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Closes the 14241-14250
 * continuum. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14250_marker[] = "libcgj-batch14250";

#define B14250_BATCH_ID  14250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14250_id(void)
{
	return B14250_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14250 - report this TU's graph batch number.
 *
 * Always returns 14250. Soft pure-data identity for the seatd/polkit
 * soft continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_14250(void)
{
	(void)NULL;
	return b14250_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14250(void)
    __attribute__((alias("gj_batch_id_14250")));
