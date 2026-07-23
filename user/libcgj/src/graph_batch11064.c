/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11064: input soft continuum stub.
 * Exclusive continuum CREATE-ONLY. No parent wires. No __int128.
 * Clean-room freestanding pure C (integer only).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11064_marker[] = "libcgj-batch11064";

static uint32_t
b11064_val(void)
{
	return 0u;
}

uint32_t
gj_hidraw_ok_u_11064(void)
{
	(void)NULL;
	return b11064_val();
}

uint32_t __gj_hidraw_ok_u_11064(void)
    __attribute__((alias("gj_hidraw_ok_u_11064")));
