/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8367: open O_RDWR oflag constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oflag_rdwr_u_8367(void);
 *     - Return soft O_RDWR constant (2).
 *   uint32_t __gj_oflag_rdwr_u_8367  (alias)
 *   __libcgj_batch8367_marker = "libcgj-batch8367"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Unique gj_oflag_rdwr_u_8367 surface only; no multi-def.
 * Distinct from gj_open_rdwr_u_7413 (predicate). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8367_marker[] = "libcgj-batch8367";

/* Soft O_RDWR: open for reading and writing (access mode 2). */
#define B8367_O_RDWR ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8367_oflag_rdwr(void)
{
	return B8367_O_RDWR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oflag_rdwr_u_8367 - soft O_RDWR oflag constant.
 *
 * Always returns 2. Catalog bit value only; does not call open.
 * No parent wires.
 */
uint32_t
gj_oflag_rdwr_u_8367(void)
{
	(void)NULL;
	return b8367_oflag_rdwr();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oflag_rdwr_u_8367(void)
    __attribute__((alias("gj_oflag_rdwr_u_8367")));
