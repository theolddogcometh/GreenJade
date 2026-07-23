/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8362: STDIN_FILENO constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fd_stdin_id_8362(void);
 *     - Return soft STDIN_FILENO (0).
 *   uint32_t __gj_fd_stdin_id_8362  (alias)
 *   __libcgj_batch8362_marker = "libcgj-batch8362"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Unique gj_fd_stdin_id_8362 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8362_marker[] = "libcgj-batch8362";

/* Soft STDIN_FILENO: standard input descriptor. */
#define B8362_STDIN_FILENO ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8362_fd_stdin_id(void)
{
	return B8362_STDIN_FILENO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_stdin_id_8362 - soft STDIN_FILENO constant.
 *
 * Always returns 0. Catalog constant only; does not touch streams.
 * No parent wires.
 */
uint32_t
gj_fd_stdin_id_8362(void)
{
	(void)NULL;
	return b8362_fd_stdin_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fd_stdin_id_8362(void)
    __attribute__((alias("gj_fd_stdin_id_8362")));
