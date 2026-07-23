/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1943: warn-class log level predicate.
 *
 * Surface (unique symbols):
 *   int gj_log_level_is_warn(int level);
 *     — Return 1 if level equals syslog WARNING (4), else 0.
 *       Soft classification only; does not map NOTICE.
 *   int __gj_log_level_is_warn  (alias)
 *   __libcgj_batch1943_marker = "libcgj-batch1943"
 *
 * Logging/trace exclusive pure-data wave (1941–1950). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1943_marker[] = "libcgj-batch1943";

/* Syslog WARNING severity. */
#define B1943_LOG_WARNING  4

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if nLvl is WARNING. */
static int
b1943_is_warn(int nLvl)
{
	return (nLvl == B1943_LOG_WARNING) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_log_level_is_warn — 1 if level is WARNING (4).
 *
 * level: syslog-style severity code
 * Returns 1 when level == 4, else 0.
 */
int
gj_log_level_is_warn(int nLvl)
{
	(void)NULL;
	return b1943_is_warn(nLvl);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_log_level_is_warn(int nLvl)
    __attribute__((alias("gj_log_level_is_warn")));
