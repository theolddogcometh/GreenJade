/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7128: timer_create SIGEV_NONE notify stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tc_sigev_none_u_7128(uint32_t sigev_notify);
 *     - Return 1 if sigev_notify is SIGEV_NONE (1), else 0.
 *   uint32_t __gj_tc_sigev_none_u_7128  (alias)
 *   __libcgj_batch7128_marker = "libcgj-batch7128"
 *
 * Exclusive continuum CREATE-ONLY (7121-7130: timer_create clock stubs —
 * realtime, monotonic, process_cputime, thread_cputime, monotonic_raw,
 * boottime, timer_abstime, sigev_none, clk_ok, batch_id_7130). Unique
 * gj_tc_sigev_none_u_7128 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7128_marker[] = "libcgj-batch7128";

/* Soft SIGEV_NONE for sigevent.sigev_notify (value 1). */
#define B7128_SIGEV_NONE ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7128_tc_sigev_none(uint32_t u32Notify)
{
	return (u32Notify == B7128_SIGEV_NONE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tc_sigev_none_u_7128 - 1 if sigev_notify is SIGEV_NONE.
 *
 * sigev_notify: sigevent.sigev_notify for timer_create(2)
 *
 * Returns 1 when sigev_notify equals SIGEV_NONE (1), else 0.
 * Pure integer compare; no parent wires.
 */
uint32_t
gj_tc_sigev_none_u_7128(uint32_t u32SigevNotify)
{
	(void)NULL;
	return b7128_tc_sigev_none(u32SigevNotify);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tc_sigev_none_u_7128(uint32_t u32SigevNotify)
    __attribute__((alias("gj_tc_sigev_none_u_7128")));
