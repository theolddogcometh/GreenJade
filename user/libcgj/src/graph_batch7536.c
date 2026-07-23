/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7536: kill signal is-SIGKILL number test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ksig_is_kill_u_7536(uint32_t signo);
 *     - Return 1 if signo is SIGKILL (9), else 0.
 *   uint32_t __gj_ksig_is_kill_u_7536  (alias)
 *   __libcgj_batch7536_marker = "libcgj-batch7536"
 *
 * Exclusive continuum CREATE-ONLY (7531-7540: kill signal number stubs —
 * kill_num, term_num, int_num, hup_num, stop_num, is_kill, is_term,
 * is_coop_term, is_fatal, batch_id_7540). Unique gj_ksig_is_kill_u_7536
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7536_marker[] = "libcgj-batch7536";

/* Linux x86_64: SIGKILL = 9. */
#define B7536_SIGKILL  ((uint32_t)9u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7536_is_kill(uint32_t u32Signo)
{
	return (u32Signo == B7536_SIGKILL) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ksig_is_kill_u_7536 - 1 if signo is SIGKILL (9).
 *
 * signo: raw Linux-style signal number (no signal.h / no kill syscall)
 *
 * Returns 1 when signo equals 9, else 0. Pure equality stub.
 * No parent wires.
 */
uint32_t
gj_ksig_is_kill_u_7536(uint32_t u32Signo)
{
	(void)NULL;
	return b7536_is_kill(u32Signo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ksig_is_kill_u_7536(uint32_t u32Signo)
    __attribute__((alias("gj_ksig_is_kill_u_7536")));
