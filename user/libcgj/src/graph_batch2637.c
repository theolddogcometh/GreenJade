/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2637: product launch environment soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_launch_env_ok(const char *env);
 *     - Return 1 if env is non-NULL and its byte length is in
 *       0..4096 (empty string allowed; NULL rejected). Soft structural
 *       bound for product launch environment blobs (NAME=value lines
 *       or similar); does not parse KEY=VAL or call putenv.
 *   int __gj_launch_env_ok  (alias)
 *   __libcgj_batch2637_marker = "libcgj-batch2637"
 *
 * Product path exclusive wave (2631-2640). Distinct from
 * gj_launch_args_ok (batch2369 argv blob bound) — unique
 * gj_launch_env_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2637_marker[] = "libcgj-batch2637";

/* Inclusive max launch-env blob length in bytes. */
#define B2637_ENV_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

/* Soft bound check for a launch-env C string. */
static int
b2637_env_ok(const char *szEnv)
{
	size_t nLen;

	if (szEnv == NULL) {
		return 0;
	}
	nLen = 0u;
	while (szEnv[nLen] != '\0') {
		if (nLen >= (size_t)B2637_ENV_MAX) {
			return 0;
		}
		nLen++;
	}
	/* Empty string (nLen == 0) is ok. */
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_launch_env_ok - soft check for a product launch-env blob.
 *
 * env: NUL-terminated environment string (may be empty)
 *
 * Returns 1 when env is non-NULL and length <= 4096, else 0.
 * Does not parse KEY=VAL lines or call libc.
 */
int
gj_launch_env_ok(const char *szEnv)
{
	(void)NULL;
	return b2637_env_ok(szEnv);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_launch_env_ok(const char *szEnv)
    __attribute__((alias("gj_launch_env_ok")));
