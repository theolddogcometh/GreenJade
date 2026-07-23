/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7539: fatal/unblockable signal-number test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ksig_is_fatal_u_7539(uint32_t signo);
 *     - Return 1 if signo is SIGKILL(9) or SIGSTOP(19), else 0.
 *   uint32_t __gj_ksig_is_fatal_u_7539  (alias)
 *   __libcgj_batch7539_marker = "libcgj-batch7539"
 *
 * Exclusive continuum CREATE-ONLY (7531-7540: kill signal number stubs —
 * kill_num, term_num, int_num, hup_num, stop_num, is_kill, is_term,
 * is_coop_term, is_fatal, batch_id_7540). Unique gj_ksig_is_fatal_u_7539
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7539_marker[] = "libcgj-batch7539";

/* Linux x86_64 unblockable signals. */
#define B7539_SIGKILL  ((uint32_t)9u)
#define B7539_SIGSTOP  ((uint32_t)19u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7539_is_fatal(uint32_t u32Signo)
{
	if (u32Signo == B7539_SIGKILL || u32Signo == B7539_SIGSTOP) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ksig_is_fatal_u_7539 - 1 if signo is unblockable (KILL/STOP).
 *
 * signo: raw Linux-style signal number (no signal.h / no kill syscall)
 *
 * Returns 1 for SIGKILL(9)/SIGSTOP(19), else 0.
 * Pure number membership stub. No parent wires.
 */
uint32_t
gj_ksig_is_fatal_u_7539(uint32_t u32Signo)
{
	(void)NULL;
	return b7539_is_fatal(u32Signo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ksig_is_fatal_u_7539(uint32_t u32Signo)
    __attribute__((alias("gj_ksig_is_fatal_u_7539")));
