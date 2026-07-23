/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8654: epoll EPOLLIN event bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_in_id_8654(void);
 *     - Return soft epoll event bit id for EPOLLIN (0x001).
 *   uint32_t __gj_epoll_in_id_8654  (alias)
 *   __libcgj_batch8654_marker = "libcgj-batch8654"
 *
 * Exclusive continuum CREATE-ONLY (8651-8660: epoll/eventfd id stubs —
 * ctl_add_id, ctl_mod_id, ctl_del_id, in_id, out_id, err_id, hup_id,
 * eventfd_init_ok, poll_ready, batch_id_8660).
 * Unique gj_epoll_in_id_8654 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8654_marker[] = "libcgj-batch8654";

/* EPOLLIN: data available for read (bit 0). */
#define B8654_EPOLLIN  ((uint32_t)0x001u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8654_in_id(void)
{
	return B8654_EPOLLIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_in_id_8654 - soft EPOLLIN event bit id.
 *
 * Always returns 0x001. Pure constant tag; no parent wires. Does not
 * inspect live epoll_event masks.
 */
uint32_t
gj_epoll_in_id_8654(void)
{
	(void)NULL;
	return b8654_in_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_in_id_8654(void)
    __attribute__((alias("gj_epoll_in_id_8654")));
