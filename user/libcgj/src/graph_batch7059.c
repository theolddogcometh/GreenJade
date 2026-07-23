/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7059: keyctl op validity soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyctl_op_ok_u_7059(uint32_t op);
 *     - Return 1 if op is a known keyctl op in [0, 7]
 *       (GET_KEYRING_ID..CLEAR), else 0.
 *   uint32_t __gj_keyctl_op_ok_u_7059  (alias)
 *   __libcgj_batch7059_marker = "libcgj-batch7059"
 *
 * Exclusive continuum CREATE-ONLY (7051-7060: keyctl op stubs —
 * get_keyring_id, join_session, update, revoke, chown, setperm,
 * describe, clear, op_ok, batch_id_7060). Unique
 * gj_keyctl_op_ok_u_7059 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7059_marker[] = "libcgj-batch7059";

/* Soft catalog: KEYCTL_GET_KEYRING_ID=0 .. KEYCTL_CLEAR=7. */
#define B7059_KEYCTL_OP_MAX ((uint32_t)7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7059_keyctl_op_ok(uint32_t u32Op)
{
	return (u32Op <= B7059_KEYCTL_OP_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyctl_op_ok_u_7059 - 1 if op is a known soft keyctl op.
 *
 * op: keyctl(2) operation argument
 *
 * Returns 1 for ops in [0, 7] (GET_KEYRING_ID..CLEAR), else 0.
 * Soft range check; does not call keyctl. No parent wires.
 */
uint32_t
gj_keyctl_op_ok_u_7059(uint32_t u32Op)
{
	(void)NULL;
	return b7059_keyctl_op_ok(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyctl_op_ok_u_7059(uint32_t u32Op)
    __attribute__((alias("gj_keyctl_op_ok_u_7059")));
