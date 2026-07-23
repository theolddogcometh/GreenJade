/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7538: cooperative terminate signal-number test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ksig_is_coop_term_u_7538(uint32_t signo);
 *     - Return 1 if signo is SIGHUP(1)/SIGINT(2)/SIGTERM(15), else 0.
 *   uint32_t __gj_ksig_is_coop_term_u_7538  (alias)
 *   __libcgj_batch7538_marker = "libcgj-batch7538"
 *
 * Exclusive continuum CREATE-ONLY (7531-7540: kill signal number stubs —
 * kill_num, term_num, int_num, hup_num, stop_num, is_kill, is_term,
 * is_coop_term, is_fatal, batch_id_7540). Unique
 * gj_ksig_is_coop_term_u_7538 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7538_marker[] = "libcgj-batch7538";

/* Linux x86_64 cooperative terminate/request signals. */
#define B7538_SIGHUP   ((uint32_t)1u)
#define B7538_SIGINT   ((uint32_t)2u)
#define B7538_SIGTERM  ((uint32_t)15u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7538_is_coop_term(uint32_t u32Signo)
{
	if (u32Signo == B7538_SIGHUP || u32Signo == B7538_SIGINT ||
	    u32Signo == B7538_SIGTERM) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ksig_is_coop_term_u_7538 - 1 if signo is HUP/INT/TERM.
 *
 * signo: raw Linux-style signal number (no signal.h / no kill syscall)
 *
 * Returns 1 for SIGHUP(1)/SIGINT(2)/SIGTERM(15), else 0.
 * Pure number membership stub. No parent wires.
 */
uint32_t
gj_ksig_is_coop_term_u_7538(uint32_t u32Signo)
{
	(void)NULL;
	return b7538_is_coop_term(u32Signo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ksig_is_coop_term_u_7538(uint32_t u32Signo)
    __attribute__((alias("gj_ksig_is_coop_term_u_7538")));
