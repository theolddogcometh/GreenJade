/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8655: epoll EPOLLOUT event bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_out_id_8655(void);
 *     - Return soft epoll event bit id for EPOLLOUT (0x004).
 *   uint32_t __gj_epoll_out_id_8655  (alias)
 *   __libcgj_batch8655_marker = "libcgj-batch8655"
 *
 * Exclusive continuum CREATE-ONLY (8651-8660: epoll/eventfd id stubs —
 * ctl_add_id, ctl_mod_id, ctl_del_id, in_id, out_id, err_id, hup_id,
 * eventfd_init_ok, poll_ready, batch_id_8660).
 * Unique gj_epoll_out_id_8655 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8655_marker[] = "libcgj-batch8655";

/* EPOLLOUT: ready for write (bit 2). */
#define B8655_EPOLLOUT  ((uint32_t)0x004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8655_out_id(void)
{
	return B8655_EPOLLOUT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_out_id_8655 - soft EPOLLOUT event bit id.
 *
 * Always returns 0x004. Pure constant tag; no parent wires. Does not
 * inspect live epoll_event masks.
 */
uint32_t
gj_epoll_out_id_8655(void)
{
	(void)NULL;
	return b8655_out_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_out_id_8655(void)
    __attribute__((alias("gj_epoll_out_id_8655")));
