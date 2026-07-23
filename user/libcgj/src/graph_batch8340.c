/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8340: env/path probe wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8340(void);
 *     - Returns the compile-time graph batch number for this TU (8340).
 *   uint32_t __gj_batch_id_8340  (alias)
 *   __libcgj_batch8340_marker = "libcgj-batch8340"
 *
 * Env/path probe stubs wave closer (8331-8340: env_name_ok, env_empty,
 * path_abs, path_rel, path_sep, path_dot, path_dotdot, path_len_ok,
 * path_nul). Distinct from gj_batch_id / gj_batch_id_N of other TUs —
 * unique _8340 surface; no multi-def. No parent wires. No real getenv.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8340_marker[] = "libcgj-batch8340";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8340_id(void)
{
	return 8340u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8340 - report this TU's graph batch number.
 *
 * Always returns 8340.
 */
uint32_t
gj_batch_id_8340(void)
{
	(void)NULL;
	return b8340_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8340(void)
    __attribute__((alias("gj_batch_id_8340")));
