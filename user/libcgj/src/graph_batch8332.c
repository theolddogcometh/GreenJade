/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8332: env value empty probe stub (_u).
 *
 * Surface (unique symbols):
 *   int gj_env_empty_u_8332(const char *val);
 *     - Return 1 if val is NULL or the empty string ""; else 0.
 *       Probe of a candidate env value buffer only — does not call
 *       getenv or touch the process environment.
 *   int __gj_env_empty_u_8332  (alias)
 *   __libcgj_batch8332_marker = "libcgj-batch8332"
 *
 * Env/path probe stubs wave (8331-8340). Distinct from gj_str_is_empty /
 * _u / empty_p_u and gj_env_val_ok_u — unique _u_8332 surface; no
 * multi-def. No parent wires. No real getenv.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8332_marker[] = "libcgj-batch8332";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff szVal is NULL or "". */
static int
b8332_empty(const char *szVal)
{
	if (szVal == NULL) {
		return 1;
	}
	if (szVal[0] == '\0') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_env_empty_u_8332 - 1 if val is NULL or empty.
 *
 * val: candidate env value string, or NULL
 *
 * Probe stub only — no getenv. No parent wires.
 */
int
gj_env_empty_u_8332(const char *szVal)
{
	(void)NULL;
	return b8332_empty(szVal);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_env_empty_u_8332(const char *szVal)
    __attribute__((alias("gj_env_empty_u_8332")));
