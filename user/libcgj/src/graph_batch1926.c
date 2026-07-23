/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1926: SSH well-known port hint.
 *
 * Surface (unique symbols):
 *   int gj_port_is_ssh_hint(unsigned port);
 *     — Return 1 if port is the conventional SSH port 22, else 0.
 *       Pure local equality data only.
 *   int __gj_port_is_ssh_hint  (alias)
 *   __libcgj_batch1926_marker = "libcgj-batch1926"
 *
 * Network port/service exclusive pure-data wave (1921–1930). Distinct
 * names — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1926_marker[] = "libcgj-batch1926";

/* Conventional SSH port. */
#define B1926_SSH  22u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port equals 22. */
static int
b1926_is_ssh(unsigned uPort)
{
	return (uPort == B1926_SSH) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_is_ssh_hint — 1 if port is 22.
 *
 * port: TCP/UDP port number
 * Returns 1 for SSH-conventional port, else 0.
 */
int
gj_port_is_ssh_hint(unsigned uPort)
{
	(void)NULL; /* stddef NULL available in freestanding TU */
	return b1926_is_ssh(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_is_ssh_hint(unsigned uPort)
    __attribute__((alias("gj_port_is_ssh_hint")));
