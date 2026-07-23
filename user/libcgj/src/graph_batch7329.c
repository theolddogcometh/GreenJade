/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7329: fcntl cmd soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fcntl_cmd_ok_u_7329(uint32_t cmd);
 *     - Return 1 if cmd is a known soft F_* command in
 *       {F_DUPFD..F_SETLK}, else 0.
 *   uint32_t __gj_fcntl_cmd_ok_u_7329  (alias)
 *   __libcgj_batch7329_marker = "libcgj-batch7329"
 *
 * Exclusive continuum CREATE-ONLY (7321-7330: fcntl cmd stubs —
 * dupfd_id, getfd_id, setfd_id, getfl_id, setfl_id, getlk_id,
 * setlk_id, is_getfd, cmd_ok, batch_id_7330).
 * Unique gj_fcntl_cmd_ok_u_7329 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7329_marker[] = "libcgj-batch7329";

/*
 * Soft catalog fcntl cmds (Linux base):
 *   F_DUPFD=0, F_GETFD=1, F_SETFD=2, F_GETFL=3,
 *   F_SETFL=4, F_GETLK=5, F_SETLK=6
 */
#define B7329_F_DUPFD ((uint32_t)0u)
#define B7329_F_GETFD ((uint32_t)1u)
#define B7329_F_SETFD ((uint32_t)2u)
#define B7329_F_GETFL ((uint32_t)3u)
#define B7329_F_SETFL ((uint32_t)4u)
#define B7329_F_GETLK ((uint32_t)5u)
#define B7329_F_SETLK ((uint32_t)6u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7329_fcntl_cmd_ok(uint32_t u32Cmd)
{
	return (u32Cmd == B7329_F_DUPFD ||
	        u32Cmd == B7329_F_GETFD ||
	        u32Cmd == B7329_F_SETFD ||
	        u32Cmd == B7329_F_GETFL ||
	        u32Cmd == B7329_F_SETFL ||
	        u32Cmd == B7329_F_GETLK ||
	        u32Cmd == B7329_F_SETLK) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fcntl_cmd_ok_u_7329 - 1 if cmd is a soft-known F_* command.
 *
 * cmd: fcntl(2) command argument
 *
 * Returns 1 when cmd is in {F_DUPFD, F_GETFD, F_SETFD, F_GETFL,
 * F_SETFL, F_GETLK, F_SETLK}, else 0. Soft catalog check; does not
 * call fcntl. No parent wires.
 */
uint32_t
gj_fcntl_cmd_ok_u_7329(uint32_t u32Cmd)
{
	(void)NULL;
	return b7329_fcntl_cmd_ok(u32Cmd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fcntl_cmd_ok_u_7329(uint32_t u32Cmd)
    __attribute__((alias("gj_fcntl_cmd_ok_u_7329")));
