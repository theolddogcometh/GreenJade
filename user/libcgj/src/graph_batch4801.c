/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4801: event-loop fd validity predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_fd_valid_u(int fd);
 *     - Return 1 when fd >= 0 (valid descriptor domain), else 0.
 *       Pure predicate; does not touch the OS or errno.
 *   uint32_t __gj_ev_fd_valid_u  (alias)
 *   __libcgj_batch4801_marker = "libcgj-batch4801"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810: ev_fd_valid_u,
 * ev_timeout_ms_u, ev_interest_read_u, ev_interest_write_u,
 * ev_interest_err_u, ev_mask_has_u, ev_mask_or_u, ev_ready_count_u,
 * ev_id_next_u, batch_id_4810). Unique gj_ev_fd_valid_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4801_marker[] = "libcgj-batch4801";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4801_fd_valid(int nFd)
{
	if (nFd >= 0) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_fd_valid_u - report whether an event-loop fd is in the valid domain.
 *
 * fd: candidate file descriptor as a signed int
 *
 * Returns 1 when fd >= 0, otherwise 0. Self-contained; no parent wires.
 */
uint32_t
gj_ev_fd_valid_u(int nFd)
{
	(void)NULL;
	return b4801_fd_valid(nFd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_fd_valid_u(int nFd)
    __attribute__((alias("gj_ev_fd_valid_u")));
