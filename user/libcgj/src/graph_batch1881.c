/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1881: fatal/unblockable signal-number hint.
 *
 * Surface (unique symbols):
 *   int gj_signo_is_fatal_hint(int signo);
 *     — Return 1 if signo is a classic Linux x86_64 unblockable signal
 *       (cannot be caught, blocked, or ignored), else 0.
 *   int __gj_signo_is_fatal_hint  (alias)
 *   __libcgj_batch1881_marker = "libcgj-batch1881"
 *
 * Linux x86_64 numeric values used (local constants only; no signal.h):
 *   SIGKILL = 9   — force-terminate; cannot be handled
 *   SIGSTOP = 19  — stop process; cannot be handled
 * "Etc." is limited to this unblockable pair on Linux.
 *
 * Signal/errno exclusive pure-data wave (1881–1889). Distinct names —
 * avoid multi-def with prior batches.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1881_marker[] = "libcgj-batch1881";

/* Linux x86_64: unblockable signals (local; no signal.h). */
#define B1881_SIGKILL  9
#define B1881_SIGSTOP  19

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if signo is SIGKILL or SIGSTOP, else 0. */
static int
b1881_is_fatal(int nSigno)
{
	if (nSigno == B1881_SIGKILL || nSigno == B1881_SIGSTOP) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_signo_is_fatal_hint — 1 if signo is unblockable (KILL/STOP).
 *
 * signo: Linux-style signal number (raw int; no libc signal API)
 * Returns 1 for SIGKILL(9)/SIGSTOP(19), else 0.
 * (void)NULL keeps stddef NULL in the freestanding surface.
 */
int
gj_signo_is_fatal_hint(int nSigno)
{
	(void)NULL;
	return b1881_is_fatal(nSigno);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_signo_is_fatal_hint(int nSigno)
    __attribute__((alias("gj_signo_is_fatal_hint")));
