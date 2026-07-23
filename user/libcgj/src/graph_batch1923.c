/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1923: user (registered) port-range hint.
 *
 * Surface (unique symbols):
 *   int gj_port_is_user_hint(unsigned port);
 *     — Return 1 if port is in the IANA registered/user range
 *       1024..49151 inclusive, else 0. Pure local range data only.
 *   int __gj_port_is_user_hint  (alias)
 *   __libcgj_batch1923_marker = "libcgj-batch1923"
 *
 * Network port/service exclusive pure-data wave (1921–1930). Distinct
 * from gj_cs_is_user_hint / gj_ss_is_user_hint (CPU segment RPL) —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1923_marker[] = "libcgj-batch1923";

/* IANA registered / user port range. */
#define B1923_USER_LO  1024u
#define B1923_USER_HI  49151u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port is in [B1923_USER_LO, B1923_USER_HI]. */
static int
b1923_is_user(unsigned uPort)
{
	return (uPort >= B1923_USER_LO && uPort <= B1923_USER_HI) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_is_user_hint — 1 if port is 1024..49151.
 *
 * port: TCP/UDP port number
 * Returns 1 in user/registered range, else 0.
 */
int
gj_port_is_user_hint(unsigned uPort)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1923_is_user(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_is_user_hint(unsigned uPort)
    __attribute__((alias("gj_port_is_user_hint")));
