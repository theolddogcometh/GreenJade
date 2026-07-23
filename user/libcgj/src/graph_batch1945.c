/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1945: debug-class log level predicate.
 *
 * Surface (unique symbols):
 *   int gj_log_level_is_debug(int level);
 *     — Return 1 if level equals syslog DEBUG (7), else 0.
 *       Soft classification only.
 *   int __gj_log_level_is_debug  (alias)
 *   __libcgj_batch1945_marker = "libcgj-batch1945"
 *
 * Logging/trace exclusive pure-data wave (1941–1950). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1945_marker[] = "libcgj-batch1945";

/* Syslog DEBUG severity. */
#define B1945_LOG_DEBUG  7

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if nLvl is DEBUG. */
static int
b1945_is_debug(int nLvl)
{
	return (nLvl == B1945_LOG_DEBUG) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_log_level_is_debug — 1 if level is DEBUG (7).
 *
 * level: syslog-style severity code
 * Returns 1 when level == 7, else 0.
 */
int
gj_log_level_is_debug(int nLvl)
{
	(void)NULL;
	return b1945_is_debug(nLvl);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_log_level_is_debug(int nLvl)
    __attribute__((alias("gj_log_level_is_debug")));
