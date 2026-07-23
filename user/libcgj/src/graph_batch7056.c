/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7056: keyctl KEYCTL_SETPERM op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyctl_is_setperm_u_7056(uint32_t op);
 *     - Return 1 if op is KEYCTL_SETPERM (5), else 0.
 *   uint32_t __gj_keyctl_is_setperm_u_7056  (alias)
 *   __libcgj_batch7056_marker = "libcgj-batch7056"
 *
 * Exclusive continuum CREATE-ONLY (7051-7060: keyctl op stubs —
 * get_keyring_id, join_session, update, revoke, chown, setperm,
 * describe, clear, op_ok, batch_id_7060). Unique
 * gj_keyctl_is_setperm_u_7056 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7056_marker[] = "libcgj-batch7056";

/* KEYCTL_SETPERM: set perms on a key (value 5). */
#define B7056_KEYCTL_SETPERM ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7056_keyctl_is_setperm(uint32_t u32Op)
{
	return (u32Op == B7056_KEYCTL_SETPERM) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyctl_is_setperm_u_7056 - 1 if op is KEYCTL_SETPERM.
 *
 * op: keyctl(2) operation argument
 *
 * Returns 1 when op equals KEYCTL_SETPERM (5), else 0.
 * Pure integer compare; does not call keyctl. No parent wires.
 */
uint32_t
gj_keyctl_is_setperm_u_7056(uint32_t u32Op)
{
	(void)NULL;
	return b7056_keyctl_is_setperm(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyctl_is_setperm_u_7056(uint32_t u32Op)
    __attribute__((alias("gj_keyctl_is_setperm_u_7056")));
