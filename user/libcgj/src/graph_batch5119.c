/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5119: errno success predicate (e == 0).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_errno_ok_u(uint32_t e);
 *     - Returns 1 when e == 0 (no error), else 0.
 *   uint32_t __gj_sys_errno_ok_u  (alias)
 *   __libcgj_batch5119_marker = "libcgj-batch5119"
 *
 * Exclusive continuum CREATE-ONLY (5111-5120: sys_nr_read_u, sys_nr_write_u,
 * sys_nr_open_u, sys_nr_close_u, sys_nr_exit_u, sys_is_io_u, sys_is_exit_u,
 * sys_arg_count_u, sys_errno_ok_u, batch_id_5120). Distinct syscall unique
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5119_marker[] = "libcgj-batch5119";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5119_errno_ok(uint32_t u32E)
{
	if (u32E == 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_errno_ok_u - true when the errno-like code is success (0).
 *
 * e: candidate errno / status code (unsigned view)
 *
 * Returns 1 if e == 0, else 0. Does not touch thread-local errno.
 * Pure integer predicate; no parent wires.
 */
uint32_t
gj_sys_errno_ok_u(uint32_t u32E)
{
	(void)NULL;
	return b5119_errno_ok(u32E);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_errno_ok_u(uint32_t u32E)
    __attribute__((alias("gj_sys_errno_ok_u")));
