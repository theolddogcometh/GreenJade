/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7626: setrlimit resource code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setrlimit_res_code_7626(void);
 *     - Returns the soft setrlimit-resource code tag for this continuum
 *       (always 0x53523736, fourCC "SR76"). Not a live setrlimit call.
 *   uint32_t __gj_setrlimit_res_code_7626  (alias)
 *   __libcgj_batch7626_marker = "libcgj-batch7626"
 *
 * Exclusive continuum CREATE-ONLY (7621-7630: setrlimit resource stubs).
 * Unique gj_setrlimit_res_code_7626 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7626_marker[] = "libcgj-batch7626";

/* Setrlimit resource code fourCC "SR76". */
#define B7626_SRL_CODE  0x53523736u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7626_code(void)
{
	return B7626_SRL_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setrlimit_res_code_7626 - setrlimit resource code tag.
 *
 * Always returns 0x53523736 ("SR76"). Soft pure-data product tag.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_setrlimit_res_code_7626(void)
{
	(void)NULL;
	return b7626_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_setrlimit_res_code_7626(void)
    __attribute__((alias("gj_setrlimit_res_code_7626")));
