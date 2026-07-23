/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1941: syslog-style log level soft-ok range.
 *
 * Surface (unique symbols):
 *   int gj_log_level_ok(int level);
 *     — Return 1 if level is a valid syslog-style severity in 0..7
 *       (EMERG..DEBUG), else 0. Pure range check; does not consult
 *       a logger instance or locale.
 *   int __gj_log_level_ok  (alias)
 *   __libcgj_batch1941_marker = "libcgj-batch1941"
 *
 * Logging/trace exclusive pure-data wave (1941–1950). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1941_marker[] = "libcgj-batch1941";

/* Syslog severity inclusive bounds (EMERG=0 .. DEBUG=7). */
#define B1941_LVL_MIN  0
#define B1941_LVL_MAX  7

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if nLvl is in [B1941_LVL_MIN, B1941_LVL_MAX]. */
static int
b1941_level_ok(int nLvl)
{
	if (nLvl < B1941_LVL_MIN) {
		return 0;
	}
	if (nLvl > B1941_LVL_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_log_level_ok — 1 if level is a syslog severity 0..7.
 *
 * level: candidate severity (signed so negatives reject cleanly)
 * Returns 1 when in range, else 0.
 */
int
gj_log_level_ok(int nLvl)
{
	(void)NULL;
	return b1941_level_ok(nLvl);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_log_level_ok(int nLvl)
    __attribute__((alias("gj_log_level_ok")));
