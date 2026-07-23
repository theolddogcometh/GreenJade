/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8368: open O_CREAT oflag constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oflag_creat_u_8368(void);
 *     - Return soft O_CREAT constant (0x40 / octal 0100).
 *   uint32_t __gj_oflag_creat_u_8368  (alias)
 *   __libcgj_batch8368_marker = "libcgj-batch8368"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Unique gj_oflag_creat_u_8368 surface only; no multi-def.
 * Distinct from gj_open_creat_u_7414 / gj_shm_o_creat_id_7142.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8368_marker[] = "libcgj-batch8368";

/* Soft O_CREAT: create if does not exist (0x40 / octal 0100). */
#define B8368_O_CREAT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8368_oflag_creat(void)
{
	return B8368_O_CREAT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oflag_creat_u_8368 - soft O_CREAT oflag constant.
 *
 * Always returns 0x40 (Linux/POSIX O_CREAT). Catalog bit value only;
 * does not call open. No parent wires.
 */
uint32_t
gj_oflag_creat_u_8368(void)
{
	(void)NULL;
	return b8368_oflag_creat();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oflag_creat_u_8368(void)
    __attribute__((alias("gj_oflag_creat_u_8368")));
