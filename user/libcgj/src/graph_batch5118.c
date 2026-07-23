/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5118: simple syscall argument-count heuristic.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_arg_count_u(uint32_t nr);
 *     - Coarse arg arity for well-known x86_64 numbers: read/write/open
 *       -> 3, close/exit -> 1, else 0. Table heuristic only.
 *   uint32_t __gj_sys_arg_count_u  (alias)
 *   __libcgj_batch5118_marker = "libcgj-batch5118"
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

const char __libcgj_batch5118_marker[] = "libcgj-batch5118";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Minimal known-table heuristic covering the 5111-5115 surface numbers:
 *   0 read  -> 3 (fd, buf, count)
 *   1 write -> 3 (fd, buf, count)
 *   2 open  -> 3 (path, flags, mode)
 *   3 close -> 1 (fd)
 *  60 exit  -> 1 (status)
 *  else     -> 0 (unknown / not in table)
 */
static uint32_t
b5118_arg_count(uint32_t u32Nr)
{
	if (u32Nr <= 2u) {
		return 3u;
	}
	if (u32Nr == 3u) {
		return 1u;
	}
	if (u32Nr == 60u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_arg_count_u - simple x86_64 syscall arg-count heuristic.
 *
 * nr: candidate syscall number
 *
 * Returns a coarse arity for the known set {0,1,2,3,60}; 0 for others.
 * Not a complete kernel ABI table. No parent wires.
 */
uint32_t
gj_sys_arg_count_u(uint32_t u32Nr)
{
	(void)NULL;
	return b5118_arg_count(u32Nr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_arg_count_u(uint32_t u32Nr)
    __attribute__((alias("gj_sys_arg_count_u")));
