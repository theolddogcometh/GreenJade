/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8369: open O_TRUNC oflag constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oflag_trunc_u_8369(void);
 *     - Return soft O_TRUNC constant (0x200 / octal 01000).
 *   uint32_t __gj_oflag_trunc_u_8369  (alias)
 *   __libcgj_batch8369_marker = "libcgj-batch8369"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Unique gj_oflag_trunc_u_8369 surface only; no multi-def.
 * Distinct from gj_open_trunc_u_7416 (predicate). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8369_marker[] = "libcgj-batch8369";

/* Soft O_TRUNC: truncate to zero length on open (0x200 / octal 01000). */
#define B8369_O_TRUNC ((uint32_t)0x200u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8369_oflag_trunc(void)
{
	return B8369_O_TRUNC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oflag_trunc_u_8369 - soft O_TRUNC oflag constant.
 *
 * Always returns 0x200 (Linux/POSIX O_TRUNC). Catalog bit value only;
 * does not call open. No parent wires.
 */
uint32_t
gj_oflag_trunc_u_8369(void)
{
	(void)NULL;
	return b8369_oflag_trunc();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oflag_trunc_u_8369(void)
    __attribute__((alias("gj_oflag_trunc_u_8369")));
