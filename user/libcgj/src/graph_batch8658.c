/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8658: eventfd initval soft-ok gate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_eventfd_init_ok_u_8658(uint32_t initval);
 *     - Soft eventfd create initval gate: always 1 (any unsigned
 *       initval is accepted by the kernel eventfd2 path in this stub
 *       model).
 *   uint32_t __gj_eventfd_init_ok_u_8658  (alias)
 *   __libcgj_batch8658_marker = "libcgj-batch8658"
 *
 * Exclusive continuum CREATE-ONLY (8651-8660: epoll/eventfd id stubs —
 * ctl_add_id, ctl_mod_id, ctl_del_id, in_id, out_id, err_id, hup_id,
 * eventfd_init_ok, poll_ready, batch_id_8660).
 * Unique gj_eventfd_init_ok_u_8658 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8658_marker[] = "libcgj-batch8658";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8658_init_ok(uint32_t u32Init)
{
	(void)u32Init;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_eventfd_init_ok_u_8658 - soft eventfd create initval validity.
 *
 * initval: initial counter value passed to eventfd/eventfd2
 *
 * Returns 1 for any initval (soft always-ok). Pure integer.
 * No parent wires. Does not call eventfd.
 */
uint32_t
gj_eventfd_init_ok_u_8658(uint32_t initval)
{
	(void)NULL;
	return b8658_init_ok(initval);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_eventfd_init_ok_u_8658(uint32_t initval)
    __attribute__((alias("gj_eventfd_init_ok_u_8658")));
