/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1942: error-class log level predicate.
 *
 * Surface (unique symbols):
 *   int gj_log_level_is_error(int level);
 *     — Return 1 if level is an error-class syslog severity
 *       (EMERG..ERR = 0..3), else 0. Soft classification only.
 *   int __gj_log_level_is_error  (alias)
 *   __libcgj_batch1942_marker = "libcgj-batch1942"
 *
 * Logging/trace exclusive pure-data wave (1941–1950). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1942_marker[] = "libcgj-batch1942";

/* Error class: EMERG=0, ALERT=1, CRIT=2, ERR=3. */
#define B1942_ERR_MIN  0
#define B1942_ERR_MAX  3

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if nLvl is in the error severity band. */
static int
b1942_is_error(int nLvl)
{
	if (nLvl < B1942_ERR_MIN) {
		return 0;
	}
	if (nLvl > B1942_ERR_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_log_level_is_error — 1 if level is EMERG..ERR (0..3).
 *
 * level: syslog-style severity code
 * Returns 1 for error-class severities, else 0.
 */
int
gj_log_level_is_error(int nLvl)
{
	(void)NULL;
	return b1942_is_error(nLvl);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_log_level_is_error(int nLvl)
    __attribute__((alias("gj_log_level_is_error")));
