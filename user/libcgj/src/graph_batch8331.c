/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8331: env name form probe stub (_u).
 *
 * Surface (unique symbols):
 *   int gj_env_name_ok_u_8331(const char *name);
 *     - Return 1 if name is a non-empty POSIX-ish environment variable
 *       name: first char [A-Za-z_], subsequent [A-Za-z0-9_], else 0.
 *       NULL / empty → 0. Cap scan at 4096 bytes. Probe only — does
 *       not call getenv or touch the process environment.
 *   int __gj_env_name_ok_u_8331  (alias)
 *   __libcgj_batch8331_marker = "libcgj-batch8331"
 *
 * Env/path probe stubs wave (8331-8340). Distinct from gj_env_key_ok_u
 * (batch3046), gj_env_is_name_char / _u, and gj_env_parse_line —
 * unique _u_8331 surface; no multi-def. No parent wires. No real
 * getenv.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8331_marker[] = "libcgj-batch8331";

/* Inclusive scan budget for env name octets. */
#define B8331_SCAN_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b8331_is_start(unsigned char u8C)
{
	if ((u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ||
	    (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ||
	    u8C == (unsigned char)'_') {
		return 1;
	}
	return 0;
}

static int
b8331_is_body(unsigned char u8C)
{
	if (b8331_is_start(u8C)) {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	return 0;
}

/* 1 if szName is a non-empty POSIX-ish env name within scan budget. */
static int
b8331_name_ok(const char *szName)
{
	size_t i;

	if (szName == NULL || szName[0] == '\0') {
		return 0;
	}
	if (!b8331_is_start((unsigned char)szName[0])) {
		return 0;
	}
	i = 1u;
	while (i < B8331_SCAN_MAX && szName[i] != '\0') {
		if (!b8331_is_body((unsigned char)szName[i])) {
			return 0;
		}
		i++;
	}
	/* Over-budget without NUL: reject (unbounded / truncated). */
	if (i >= B8331_SCAN_MAX && szName[i] != '\0') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_env_name_ok_u_8331 - 1 if name is a valid env variable name form.
 *
 * name: NUL-terminated candidate name (may be NULL)
 *
 * Probe stub only — no getenv. No parent wires.
 */
int
gj_env_name_ok_u_8331(const char *szName)
{
	(void)NULL;
	return b8331_name_ok(szName);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_env_name_ok_u_8331(const char *szName)
    __attribute__((alias("gj_env_name_ok_u_8331")));
