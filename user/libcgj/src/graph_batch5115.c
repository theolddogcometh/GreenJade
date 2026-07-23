/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5115: Linux x86_64 SYS_exit number (60).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sys_nr_exit_u(void);
 *     - Returns the well-known x86_64 syscall number for exit (60).
 *   uint32_t __gj_sys_nr_exit_u  (alias)
 *   __libcgj_batch5115_marker = "libcgj-batch5115"
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

const char __libcgj_batch5115_marker[] = "libcgj-batch5115";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5115_nr_exit(void)
{
	return 60u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sys_nr_exit_u - Linux x86_64 SYS_exit number.
 *
 * Always returns 60 (SYS_exit on x86_64). Integer table constant only;
 * does not invoke a syscall. No parent wires.
 */
uint32_t
gj_sys_nr_exit_u(void)
{
	(void)NULL;
	return b5115_nr_exit();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sys_nr_exit_u(void)
    __attribute__((alias("gj_sys_nr_exit_u")));
