/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1933: battery full-status hint.
 *
 * Surface (unique symbols):
 *   int gj_battery_is_full_hint(int status);
 *     — Return 1 if status equals the soft full code 4
 *       (power_supply-style STATUS_FULL), else 0. Pure-data hint only.
 *   int __gj_battery_is_full_hint  (alias)
 *   __libcgj_batch1933_marker = "libcgj-batch1933"
 *
 * Power/thermal exclusive pure-data wave (1931–1940). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1933_marker[] = "libcgj-batch1933";

/* Soft power_supply status: FULL. */
#define B1933_STATUS_FULL  4

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1933_is_full(int nStatus)
{
	return (nStatus == B1933_STATUS_FULL) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_is_full_hint — 1 if status is FULL (4).
 *
 * status: power_supply-style battery status code
 * Returns 1 when status == 4, else 0.
 */
int
gj_battery_is_full_hint(int nStatus)
{
	(void)NULL;
	return b1933_is_full(nStatus);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_battery_is_full_hint(int nStatus)
    __attribute__((alias("gj_battery_is_full_hint")));
