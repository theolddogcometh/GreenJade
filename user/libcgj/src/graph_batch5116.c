/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5116: classify I/O-family syscall numbers.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_is_io_u(uint32_t nr);
 *     - Returns 1 when nr is in {0, 1, 2, 3} (read/write/open/close on
 *       x86_64), else 0.
 *   uint32_t __gj_sys_is_io_u  (alias)
 *   __libcgj_batch5116_marker = "libcgj-batch5116"
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

const char __libcgj_batch5116_marker[] = "libcgj-batch5116";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5116_is_io(uint32_t u32Nr)
{
	if (u32Nr <= 3u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_is_io_u - true when nr is a basic I/O syscall number.
 *
 * nr: candidate x86_64 syscall number
 *
 * Returns 1 if nr is 0 (read), 1 (write), 2 (open), or 3 (close);
 * otherwise 0. Pure integer predicate; no parent wires.
 */
uint32_t
gj_sys_is_io_u(uint32_t u32Nr)
{
	(void)NULL;
	return b5116_is_io(u32Nr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_is_io_u(uint32_t u32Nr)
    __attribute__((alias("gj_sys_is_io_u")));
