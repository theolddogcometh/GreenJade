/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8364: STDERR_FILENO constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fd_stderr_id_8364(void);
 *     - Return soft STDERR_FILENO (2).
 *   uint32_t __gj_fd_stderr_id_8364  (alias)
 *   __libcgj_batch8364_marker = "libcgj-batch8364"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Unique gj_fd_stderr_id_8364 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8364_marker[] = "libcgj-batch8364";

/* Soft STDERR_FILENO: standard error descriptor. */
#define B8364_STDERR_FILENO ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8364_fd_stderr_id(void)
{
	return B8364_STDERR_FILENO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_stderr_id_8364 - soft STDERR_FILENO constant.
 *
 * Always returns 2. Catalog constant only; does not touch streams.
 * No parent wires.
 */
uint32_t
gj_fd_stderr_id_8364(void)
{
	(void)NULL;
	return b8364_fd_stderr_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fd_stderr_id_8364(void)
    __attribute__((alias("gj_fd_stderr_id_8364")));
