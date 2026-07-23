/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7051: keyctl KEYCTL_GET_KEYRING_ID op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyctl_is_get_keyring_id_u_7051(uint32_t op);
 *     - Return 1 if op is KEYCTL_GET_KEYRING_ID (0), else 0.
 *   uint32_t __gj_keyctl_is_get_keyring_id_u_7051  (alias)
 *   __libcgj_batch7051_marker = "libcgj-batch7051"
 *
 * Exclusive continuum CREATE-ONLY (7051-7060: keyctl op stubs —
 * get_keyring_id, join_session, update, revoke, chown, setperm,
 * describe, clear, op_ok, batch_id_7060). Unique
 * gj_keyctl_is_get_keyring_id_u_7051 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7051_marker[] = "libcgj-batch7051";

/* KEYCTL_GET_KEYRING_ID: ask for a keyring's ID (value 0). */
#define B7051_KEYCTL_GET_KEYRING_ID ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7051_keyctl_is_get_keyring_id(uint32_t u32Op)
{
	return (u32Op == B7051_KEYCTL_GET_KEYRING_ID) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyctl_is_get_keyring_id_u_7051 - 1 if op is KEYCTL_GET_KEYRING_ID.
 *
 * op: keyctl(2) operation argument
 *
 * Returns 1 when op equals KEYCTL_GET_KEYRING_ID (0), else 0.
 * Pure integer compare; does not call keyctl. No parent wires.
 */
uint32_t
gj_keyctl_is_get_keyring_id_u_7051(uint32_t u32Op)
{
	(void)NULL;
	return b7051_keyctl_is_get_keyring_id(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyctl_is_get_keyring_id_u_7051(uint32_t u32Op)
    __attribute__((alias("gj_keyctl_is_get_keyring_id_u_7051")));
