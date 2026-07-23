/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8365: open O_RDONLY oflag constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oflag_rdonly_u_8365(void);
 *     - Return soft O_RDONLY constant (0).
 *   uint32_t __gj_oflag_rdonly_u_8365  (alias)
 *   __libcgj_batch8365_marker = "libcgj-batch8365"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Unique gj_oflag_rdonly_u_8365 surface only; no multi-def.
 * Distinct from gj_open_rdonly_u_7411 (predicate). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8365_marker[] = "libcgj-batch8365";

/* Soft O_RDONLY: open for reading only (access mode 0). */
#define B8365_O_RDONLY ((uint32_t)0x0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8365_oflag_rdonly(void)
{
	return B8365_O_RDONLY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oflag_rdonly_u_8365 - soft O_RDONLY oflag constant.
 *
 * Always returns 0. Catalog bit value only; does not call open.
 * No parent wires.
 */
uint32_t
gj_oflag_rdonly_u_8365(void)
{
	(void)NULL;
	return b8365_oflag_rdonly();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oflag_rdonly_u_8365(void)
    __attribute__((alias("gj_oflag_rdonly_u_8365")));
