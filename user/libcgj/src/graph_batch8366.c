/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8366: open O_WRONLY oflag constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oflag_wronly_u_8366(void);
 *     - Return soft O_WRONLY constant (1).
 *   uint32_t __gj_oflag_wronly_u_8366  (alias)
 *   __libcgj_batch8366_marker = "libcgj-batch8366"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Unique gj_oflag_wronly_u_8366 surface only; no multi-def.
 * Distinct from gj_open_wronly_u_7412 (predicate). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8366_marker[] = "libcgj-batch8366";

/* Soft O_WRONLY: open for writing only (access mode 1). */
#define B8366_O_WRONLY ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8366_oflag_wronly(void)
{
	return B8366_O_WRONLY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oflag_wronly_u_8366 - soft O_WRONLY oflag constant.
 *
 * Always returns 1. Catalog bit value only; does not call open.
 * No parent wires.
 */
uint32_t
gj_oflag_wronly_u_8366(void)
{
	(void)NULL;
	return b8366_oflag_wronly();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oflag_wronly_u_8366(void)
    __attribute__((alias("gj_oflag_wronly_u_8366")));
