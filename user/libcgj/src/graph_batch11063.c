/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11063: input soft continuum stub.
 * Exclusive continuum CREATE-ONLY. No parent wires. No __int128.
 * Clean-room freestanding pure C (integer only).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11063_marker[] = "libcgj-batch11063";

static uint32_t
b11063_val(void)
{
	return 0u;
}

uint32_t
gj_evdev_ok_u_11063(void)
{
	(void)NULL;
	return b11063_val();
}

uint32_t __gj_evdev_ok_u_11063(void)
    __attribute__((alias("gj_evdev_ok_u_11063")));
