/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1944: info-class log level predicate.
 *
 * Surface (unique symbols):
 *   int gj_log_level_is_info(int level);
 *     — Return 1 if level equals syslog INFO (6), else 0.
 *       Soft classification only; does not map NOTICE.
 *   int __gj_log_level_is_info  (alias)
 *   __libcgj_batch1944_marker = "libcgj-batch1944"
 *
 * Logging/trace exclusive pure-data wave (1941–1950). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1944_marker[] = "libcgj-batch1944";

/* Syslog INFO severity. */
#define B1944_LOG_INFO  6

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if nLvl is INFO. */
static int
b1944_is_info(int nLvl)
{
	return (nLvl == B1944_LOG_INFO) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_log_level_is_info — 1 if level is INFO (6).
 *
 * level: syslog-style severity code
 * Returns 1 when level == 6, else 0.
 */
int
gj_log_level_is_info(int nLvl)
{
	(void)NULL;
	return b1944_is_info(nLvl);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_log_level_is_info(int nLvl)
    __attribute__((alias("gj_log_level_is_info")));
