/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14247: seat-session soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seat_session_ok_u_14247(void);
 *     - Returns 0 (seat-session soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live seatd/logind session probe.
 *   uint32_t __gj_seat_session_ok_u_14247  (alias)
 *   __libcgj_batch14247_marker = "libcgj-batch14247"
 *
 * Exclusive continuum CREATE-ONLY (14241-14250: seatd/polkit soft —
 * polkit_ok_u_14241, seatd_ok_u_14242, libseat_ok_u_14243,
 * pk_action_ok_u_14244, pk_subject_ok_u_14245, pk_details_ok_u_14246,
 * seat_session_ok_u_14247, pk_authority_ok_u_14248,
 * seatd_polkit_ready_u_14249, batch_id_14250). Unique surface only;
 * no multi-def. No parent wires. No __int128. No seatd implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14247_marker[] = "libcgj-batch14247";

/* Soft lamp: seat-session ok stub (not asserted). */
#define B14247_SEAT_SESSION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14247_seat_session_ok(void)
{
	return B14247_SEAT_SESSION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seat_session_ok_u_14247 - seat-session soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not enumerate seat
 * sessions, claim VTs, or probe seatd/logind session objects. Does
 * not call libc. No parent wires.
 */
uint32_t
gj_seat_session_ok_u_14247(void)
{
	(void)NULL;
	return b14247_seat_session_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seat_session_ok_u_14247(void)
    __attribute__((alias("gj_seat_session_ok_u_14247")));
