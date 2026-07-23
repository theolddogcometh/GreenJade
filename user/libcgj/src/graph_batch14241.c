/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14241: polkit soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_polkit_ok_u_14241(void);
 *     - Returns 0 (polkit soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live polkit/libpolkit probe.
 *   uint32_t __gj_polkit_ok_u_14241  (alias)
 *   __libcgj_batch14241_marker = "libcgj-batch14241"
 *
 * Exclusive continuum CREATE-ONLY (14241-14250: seatd/polkit soft —
 * polkit_ok_u_14241, seatd_ok_u_14242, libseat_ok_u_14243,
 * pk_action_ok_u_14244, pk_subject_ok_u_14245, pk_details_ok_u_14246,
 * seat_session_ok_u_14247, pk_authority_ok_u_14248,
 * seatd_polkit_ready_u_14249, batch_id_14250). Unique surface only;
 * no multi-def. No parent wires. No __int128. No Polkit implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14241_marker[] = "libcgj-batch14241";

/* Soft lamp: polkit ok stub (not asserted). */
#define B14241_POLKIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14241_polkit_ok(void)
{
	return B14241_POLKIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_polkit_ok_u_14241 - polkit soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not contact polkitd,
 * open D-Bus, or probe PolicyKit authorities. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_polkit_ok_u_14241(void)
{
	(void)NULL;
	return b14241_polkit_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_polkit_ok_u_14241(void)
    __attribute__((alias("gj_polkit_ok_u_14241")));
