/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7537: kill signal is-SIGTERM number test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ksig_is_term_u_7537(uint32_t signo);
 *     - Return 1 if signo is SIGTERM (15), else 0.
 *   uint32_t __gj_ksig_is_term_u_7537  (alias)
 *   __libcgj_batch7537_marker = "libcgj-batch7537"
 *
 * Exclusive continuum CREATE-ONLY (7531-7540: kill signal number stubs —
 * kill_num, term_num, int_num, hup_num, stop_num, is_kill, is_term,
 * is_coop_term, is_fatal, batch_id_7540). Unique gj_ksig_is_term_u_7537
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7537_marker[] = "libcgj-batch7537";

/* Linux x86_64: SIGTERM = 15. */
#define B7537_SIGTERM  ((uint32_t)15u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7537_is_term(uint32_t u32Signo)
{
	return (u32Signo == B7537_SIGTERM) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ksig_is_term_u_7537 - 1 if signo is SIGTERM (15).
 *
 * signo: raw Linux-style signal number (no signal.h / no kill syscall)
 *
 * Returns 1 when signo equals 15, else 0. Pure equality stub.
 * No parent wires.
 */
uint32_t
gj_ksig_is_term_u_7537(uint32_t u32Signo)
{
	(void)NULL;
	return b7537_is_term(u32Signo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ksig_is_term_u_7537(uint32_t u32Signo)
    __attribute__((alias("gj_ksig_is_term_u_7537")));
