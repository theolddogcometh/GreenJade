/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14244: Polkit action ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pk_action_ok_u_14244(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       PolkitAction / action-id validity probe.
 *   uint32_t __gj_pk_action_ok_u_14244  (alias)
 *   __libcgj_batch14244_marker = "libcgj-batch14244"
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

const char __libcgj_batch14244_marker[] = "libcgj-batch14244";

/* Soft Polkit action-ok lamp: always off (not a real action probe). */
#define B14244_PK_ACTION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14244_pk_action_ok(void)
{
	return B14244_PK_ACTION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pk_action_ok_u_14244 - Polkit action ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not look up action ids
 * or call polkitd/libc. No parent wires.
 */
uint32_t
gj_pk_action_ok_u_14244(void)
{
	(void)NULL;
	return b14244_pk_action_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pk_action_ok_u_14244(void)
    __attribute__((alias("gj_pk_action_ok_u_14244")));
