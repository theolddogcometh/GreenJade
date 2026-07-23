/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6749: seccomp SECCOMP_RET_ACTION_FULL extract stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seccomp_action_full_u_6749(uint32_t ret);
 *     - Return the action field of a seccomp filter return value
 *       (ret & SECCOMP_RET_ACTION_FULL / 0xffff0000).
 *   uint32_t __gj_seccomp_action_full_u_6749  (alias)
 *   __libcgj_batch6749_marker = "libcgj-batch6749"
 *
 * Exclusive continuum CREATE-ONLY (6741-6750: seccomp filter action stubs —
 * allow, errno, trap, kill_thread, kill_process, trace, log, user_notif,
 * action_full, batch_id_6750). Unique gj_seccomp_action_full_u_6749 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6749_marker[] = "libcgj-batch6749";

/* SECCOMP_RET_ACTION_FULL: mask isolating the action class bits. */
#define B6749_RET_ACTION_FULL ((uint32_t)0xffff0000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6749_action_full(uint32_t u32Ret)
{
	return u32Ret & B6749_RET_ACTION_FULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seccomp_action_full_u_6749 - extract seccomp action field from ret.
 *
 * ret: raw seccomp filter return value (action | data)
 *
 * Returns ret & SECCOMP_RET_ACTION_FULL (upper 16 bits). Pure mask
 * extract; no parent wires.
 */
uint32_t
gj_seccomp_action_full_u_6749(uint32_t u32Ret)
{
	(void)NULL;
	return b6749_action_full(u32Ret);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seccomp_action_full_u_6749(uint32_t u32Ret)
    __attribute__((alias("gj_seccomp_action_full_u_6749")));
