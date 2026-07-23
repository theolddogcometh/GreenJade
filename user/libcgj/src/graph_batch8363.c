/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8363: STDOUT_FILENO constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fd_stdout_id_8363(void);
 *     - Return soft STDOUT_FILENO (1).
 *   uint32_t __gj_fd_stdout_id_8363  (alias)
 *   __libcgj_batch8363_marker = "libcgj-batch8363"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Unique gj_fd_stdout_id_8363 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8363_marker[] = "libcgj-batch8363";

/* Soft STDOUT_FILENO: standard output descriptor. */
#define B8363_STDOUT_FILENO ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8363_fd_stdout_id(void)
{
	return B8363_STDOUT_FILENO;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fd_stdout_id_8363 - soft STDOUT_FILENO constant.
 *
 * Always returns 1. Catalog constant only; does not touch streams.
 * No parent wires.
 */
uint32_t
gj_fd_stdout_id_8363(void)
{
	(void)NULL;
	return b8363_fd_stdout_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fd_stdout_id_8363(void)
    __attribute__((alias("gj_fd_stdout_id_8363")));
