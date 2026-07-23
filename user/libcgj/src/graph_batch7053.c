/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7053: keyctl KEYCTL_UPDATE op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyctl_is_update_u_7053(uint32_t op);
 *     - Return 1 if op is KEYCTL_UPDATE (2), else 0.
 *   uint32_t __gj_keyctl_is_update_u_7053  (alias)
 *   __libcgj_batch7053_marker = "libcgj-batch7053"
 *
 * Exclusive continuum CREATE-ONLY (7051-7060: keyctl op stubs —
 * get_keyring_id, join_session, update, revoke, chown, setperm,
 * describe, clear, op_ok, batch_id_7060). Unique
 * gj_keyctl_is_update_u_7053 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7053_marker[] = "libcgj-batch7053";

/* KEYCTL_UPDATE: update a key (value 2). */
#define B7053_KEYCTL_UPDATE ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7053_keyctl_is_update(uint32_t u32Op)
{
	return (u32Op == B7053_KEYCTL_UPDATE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyctl_is_update_u_7053 - 1 if op is KEYCTL_UPDATE.
 *
 * op: keyctl(2) operation argument
 *
 * Returns 1 when op equals KEYCTL_UPDATE (2), else 0.
 * Pure integer compare; does not call keyctl. No parent wires.
 */
uint32_t
gj_keyctl_is_update_u_7053(uint32_t u32Op)
{
	(void)NULL;
	return b7053_keyctl_is_update(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyctl_is_update_u_7053(uint32_t u32Op)
    __attribute__((alias("gj_keyctl_is_update_u_7053")));
