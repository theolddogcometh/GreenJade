/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7328: fcntl is-F_GETFD cmd predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fcntl_is_getfd_u_7328(uint32_t cmd);
 *     - Return 1 if cmd is F_GETFD (1), else 0.
 *   uint32_t __gj_fcntl_is_getfd_u_7328  (alias)
 *   __libcgj_batch7328_marker = "libcgj-batch7328"
 *
 * Exclusive continuum CREATE-ONLY (7321-7330: fcntl cmd stubs —
 * dupfd_id, getfd_id, setfd_id, getfl_id, setfl_id, getlk_id,
 * setlk_id, is_getfd, cmd_ok, batch_id_7330).
 * Unique gj_fcntl_is_getfd_u_7328 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7328_marker[] = "libcgj-batch7328";

/* F_GETFD: get file descriptor flags (value 1 on Linux). */
#define B7328_F_GETFD ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7328_fcntl_is_getfd(uint32_t u32Cmd)
{
	return (u32Cmd == B7328_F_GETFD) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fcntl_is_getfd_u_7328 - 1 if cmd is F_GETFD.
 *
 * cmd: fcntl(2) command argument
 *
 * Returns 1 when cmd equals F_GETFD (1), else 0.
 * Pure integer compare; does not call fcntl. No parent wires.
 */
uint32_t
gj_fcntl_is_getfd_u_7328(uint32_t u32Cmd)
{
	(void)NULL;
	return b7328_fcntl_is_getfd(u32Cmd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fcntl_is_getfd_u_7328(uint32_t u32Cmd)
    __attribute__((alias("gj_fcntl_is_getfd_u_7328")));
