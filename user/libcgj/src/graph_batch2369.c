/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2369: launch-args string soft-ok predicate
 * (post-2360 product path exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_launch_args_ok(const char *args);
 *     - Return 1 if args is non-NULL and its byte length is in
 *       0..4096 (empty string allowed; NULL rejected). Soft structural
 *       bound for product launch argument blobs; does not parse argv.
 *   int __gj_launch_args_ok  (alias)
 *   __libcgj_batch2369_marker = "libcgj-batch2369"
 *
 * Post-2360 product path exclusive wave (2361-2370). Unique
 * launch_args_ok surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2369_marker[] = "libcgj-batch2369";

/* Inclusive max launch-args blob length in bytes. */
#define B2369_ARGS_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

/* Soft bound check for a launch-args C string. */
static int
b2369_args_ok(const char *szArgs)
{
	size_t nLen;

	if (szArgs == NULL) {
		return 0;
	}
	nLen = 0u;
	while (szArgs[nLen] != '\0') {
		if (nLen >= (size_t)B2369_ARGS_MAX) {
			return 0;
		}
		nLen++;
	}
	/* Empty string (nLen == 0) is ok. */
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_launch_args_ok - soft check for a product launch-args blob.
 *
 * args: NUL-terminated launch argument string (may be empty)
 *
 * Returns 1 when args is non-NULL and length <= 4096, else 0.
 * Does not parse shell quoting or call libc.
 */
int
gj_launch_args_ok(const char *szArgs)
{
	(void)NULL;
	return b2369_args_ok(szArgs);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_launch_args_ok(const char *szArgs)
    __attribute__((alias("gj_launch_args_ok")));
