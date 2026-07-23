/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5279: pure-value USB EP0 max packet size (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_usb_ep0_max_u(void);
 *     - Return the full-speed / default control endpoint 0 max packet
 *       size in bytes (64).
 *   uint32_t __gj_usb_ep0_max_u  (alias)
 *   __libcgj_batch5279_marker = "libcgj-batch5279"
 *
 * Exclusive continuum CREATE-ONLY (5271-5280: USB unique —
 * usb_pid_setup_u, usb_pid_in_u, usb_pid_out_u, usb_pid_data0_u,
 * usb_pid_data1_u, usb_pid_ack_u, usb_pid_nak_u, usb_pid_stall_u,
 * usb_ep0_max_u, batch_id_5280). Unique gj_usb_ep0_max_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5279_marker[] = "libcgj-batch5279";

/* Default control EP0 max packet size (bytes). */
#define B5279_EP0_MAX  ((uint32_t)64u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5279_ep0_max(void)
{
	return B5279_EP0_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_usb_ep0_max_u - USB endpoint 0 max packet size constant (64).
 *
 * Always returns 64. Self-contained; no parent wires.
 */
uint32_t
gj_usb_ep0_max_u(void)
{
	(void)NULL;
	return b5279_ep0_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_usb_ep0_max_u(void)
    __attribute__((alias("gj_usb_ep0_max_u")));
