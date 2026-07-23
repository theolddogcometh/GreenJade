/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7057: keyctl KEYCTL_DESCRIBE op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyctl_is_describe_u_7057(uint32_t op);
 *     - Return 1 if op is KEYCTL_DESCRIBE (6), else 0.
 *   uint32_t __gj_keyctl_is_describe_u_7057  (alias)
 *   __libcgj_batch7057_marker = "libcgj-batch7057"
 *
 * Exclusive continuum CREATE-ONLY (7051-7060: keyctl op stubs —
 * get_keyring_id, join_session, update, revoke, chown, setperm,
 * describe, clear, op_ok, batch_id_7060). Unique
 * gj_keyctl_is_describe_u_7057 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7057_marker[] = "libcgj-batch7057";

/* KEYCTL_DESCRIBE: describe a key (value 6). */
#define B7057_KEYCTL_DESCRIBE ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7057_keyctl_is_describe(uint32_t u32Op)
{
	return (u32Op == B7057_KEYCTL_DESCRIBE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyctl_is_describe_u_7057 - 1 if op is KEYCTL_DESCRIBE.
 *
 * op: keyctl(2) operation argument
 *
 * Returns 1 when op equals KEYCTL_DESCRIBE (6), else 0.
 * Pure integer compare; does not call keyctl. No parent wires.
 */
uint32_t
gj_keyctl_is_describe_u_7057(uint32_t u32Op)
{
	(void)NULL;
	return b7057_keyctl_is_describe(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyctl_is_describe_u_7057(uint32_t u32Op)
    __attribute__((alias("gj_keyctl_is_describe_u_7057")));
