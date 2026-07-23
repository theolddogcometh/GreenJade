/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1932: battery charging-status hint.
 *
 * Surface (unique symbols):
 *   int gj_battery_is_charging_hint(int status);
 *     — Return 1 if status equals the soft charging code 1
 *       (power_supply-style STATUS_CHARGING), else 0. Pure-data hint;
 *       not a live ACPI or sysfs read.
 *   int __gj_battery_is_charging_hint  (alias)
 *   __libcgj_batch1932_marker = "libcgj-batch1932"
 *
 * Power/thermal exclusive pure-data wave (1931–1940). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1932_marker[] = "libcgj-batch1932";

/* Soft power_supply status: CHARGING. */
#define B1932_STATUS_CHARGING  1

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1932_is_charging(int nStatus)
{
	return (nStatus == B1932_STATUS_CHARGING) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_is_charging_hint — 1 if status is CHARGING (1).
 *
 * status: power_supply-style battery status code
 * Returns 1 when status == 1, else 0.
 */
int
gj_battery_is_charging_hint(int nStatus)
{
	(void)NULL;
	return b1932_is_charging(nStatus);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_battery_is_charging_hint(int nStatus)
    __attribute__((alias("gj_battery_is_charging_hint")));
