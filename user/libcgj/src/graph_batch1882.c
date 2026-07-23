/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1882: terminal/request signal-number hint.
 *
 * Surface (unique symbols):
 *   int gj_signo_is_term_hint(int signo);
 *     — Return 1 if signo is a common cooperative terminate/request
 *       signal (SIGHUP/SIGINT/SIGTERM), else 0.
 *   int __gj_signo_is_term_hint  (alias)
 *   __libcgj_batch1882_marker = "libcgj-batch1882"
 *
 * Linux x86_64 numeric values used (local constants only; no signal.h):
 *   SIGHUP  = 1   — hangup / controlling terminal closed
 *   SIGINT  = 2   — interactive interrupt (Ctrl-C)
 *   SIGTERM = 15  — polite termination request
 *
 * Signal/errno exclusive pure-data wave (1881–1889). Distinct from
 * gj_signo_is_fatal_hint (batch1881) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1882_marker[] = "libcgj-batch1882";

/* Linux x86_64: cooperative terminate signals (local; no signal.h). */
#define B1882_SIGHUP   1
#define B1882_SIGINT   2
#define B1882_SIGTERM  15

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if signo is HUP, INT, or TERM. */
static int
b1882_is_term(int nSigno)
{
	if (nSigno == B1882_SIGHUP || nSigno == B1882_SIGINT ||
	    nSigno == B1882_SIGTERM) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_signo_is_term_hint — 1 if signo is SIGHUP/SIGINT/SIGTERM.
 *
 * signo: Linux-style signal number (raw int; no libc signal API)
 * Returns 1 for HUP(1)/INT(2)/TERM(15), else 0.
 */
int
gj_signo_is_term_hint(int nSigno)
{
	(void)NULL;
	return b1882_is_term(nSigno);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_signo_is_term_hint(int nSigno)
    __attribute__((alias("gj_signo_is_term_hint")));
