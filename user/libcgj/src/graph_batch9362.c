/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9362: soft Polkit auth-no result id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pk_auth_no_id_9362(void);
 *     - Returns 0 (not-authorized soft catalog value). Pure-data result
 *       id stub; not a real PolkitAuthorizationResult probe.
 *   uint32_t __gj_pk_auth_no_id_9362  (alias)
 *   __libcgj_batch9362_marker = "libcgj-batch9362"
 *
 * Exclusive continuum CREATE-ONLY (9361-9370: polkit soft id stubs —
 * pk_auth_yes_id_9361, pk_auth_no_id_9362, pk_auth_challenge_id_9363,
 * pk_action_ok_u_9364, pk_subject_ok_u_9365, pk_details_ok_u_9366,
 * pk_session_ok_u_9367, pk_authority_ok_u_9368, polkit_ready_u_9369,
 * batch_id_9370). Unique surface only; no multi-def. No parent wires.
 * No __int128. No Polkit implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9362_marker[] = "libcgj-batch9362";

/* Soft Polkit not-authorized result catalog id (no). */
#define B9362_PK_AUTH_NO_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9362_auth_no_id(void)
{
	return B9362_PK_AUTH_NO_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pk_auth_no_id_9362 - soft Polkit not-authorized (no) result id.
 *
 * Always returns 0. Soft continuum constant; does not talk to polkitd
 * or call libc. No parent wires.
 */
uint32_t
gj_pk_auth_no_id_9362(void)
{
	(void)NULL;
	return b9362_auth_no_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pk_auth_no_id_9362(void)
    __attribute__((alias("gj_pk_auth_no_id_9362")));
