/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8657: epoll EPOLLHUP event bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_hup_id_8657(void);
 *     - Return soft epoll event bit id for EPOLLHUP (0x010).
 *   uint32_t __gj_epoll_hup_id_8657  (alias)
 *   __libcgj_batch8657_marker = "libcgj-batch8657"
 *
 * Exclusive continuum CREATE-ONLY (8651-8660: epoll/eventfd id stubs —
 * ctl_add_id, ctl_mod_id, ctl_del_id, in_id, out_id, err_id, hup_id,
 * eventfd_init_ok, poll_ready, batch_id_8660).
 * Unique gj_epoll_hup_id_8657 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8657_marker[] = "libcgj-batch8657";

/* EPOLLHUP: hang up (bit 4). */
#define B8657_EPOLLHUP  ((uint32_t)0x010u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8657_hup_id(void)
{
	return B8657_EPOLLHUP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_hup_id_8657 - soft EPOLLHUP event bit id.
 *
 * Always returns 0x010. Pure constant tag; no parent wires. Does not
 * inspect live epoll_event masks.
 */
uint32_t
gj_epoll_hup_id_8657(void)
{
	(void)NULL;
	return b8657_hup_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_hup_id_8657(void)
    __attribute__((alias("gj_epoll_hup_id_8657")));
