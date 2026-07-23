/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6746: seccomp SECCOMP_RET_TRACE action stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seccomp_is_trace_u_6746(uint32_t ret);
 *     - Return 1 if the seccomp filter return value action is
 *       SECCOMP_RET_TRACE (0x7ff00000), else 0.
 *   uint32_t __gj_seccomp_is_trace_u_6746  (alias)
 *   __libcgj_batch6746_marker = "libcgj-batch6746"
 *
 * Exclusive continuum CREATE-ONLY (6741-6750: seccomp filter action stubs —
 * allow, errno, trap, kill_thread, kill_process, trace, log, user_notif,
 * action_full, batch_id_6750). Unique gj_seccomp_is_trace_u_6746 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6746_marker[] = "libcgj-batch6746";

/* SECCOMP_RET_ACTION_FULL: upper 16 bits hold the action class. */
#define B6746_RET_ACTION_FULL ((uint32_t)0xffff0000u)
/* SECCOMP_RET_TRACE: pass to a tracer or disallow. */
#define B6746_RET_TRACE       ((uint32_t)0x7ff00000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6746_is_trace(uint32_t u32Ret)
{
	return ((u32Ret & B6746_RET_ACTION_FULL) == B6746_RET_TRACE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seccomp_is_trace_u_6746 - 1 if seccomp ret action is TRACE.
 *
 * ret: raw seccomp filter return value (action | data)
 *
 * Returns 1 when the action field equals SECCOMP_RET_TRACE, else 0.
 * Pure mask compare; no parent wires.
 */
uint32_t
gj_seccomp_is_trace_u_6746(uint32_t u32Ret)
{
	(void)NULL;
	return b6746_is_trace(u32Ret);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seccomp_is_trace_u_6746(uint32_t u32Ret)
    __attribute__((alias("gj_seccomp_is_trace_u_6746")));
