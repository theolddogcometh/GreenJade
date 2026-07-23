/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5657: network setup flags pack.
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_setup_pack_5657(uint32_t mode, uint32_t flags,
 *                                   uint32_t iface);
 *     - Pack first-boot network page state:
 *         bit0..3   = mode (0 none, 1 dhcp, 2 static, 3 wifi, ..15)
 *         bit4..19  = flags mask (ipv6, proxy, metered, ...)
 *         bit20..27 = iface index (0..255)
 *         bit28..31 = mix nibble
 *   uint32_t gj_net_setup_mode_5657(uint32_t packed);
 *   uint32_t gj_net_setup_flags_5657(uint32_t packed);
 *   uint32_t gj_net_setup_iface_5657(uint32_t packed);
 *   uint32_t gj_batch_id_5657(void);
 *   __ aliases
 *   __libcgj_batch5657_marker = "libcgj-batch5657"
 *
 * Exclusive continuum CREATE-ONLY (5651-5660: post-install first-boot
 * wizard stubs, locale, keyboard, network setup flags). Unique *_5657
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5657_marker[] = "libcgj-batch5657";

#define B5657_BATCH_ID 5657u

/* Soft flag bits in the 16-bit flags field (documentation):
 * 0 ipv6_enable, 1 use_proxy, 2 metered, 3 skip_network,
 * 4 wifi_hidden, 5 dhcpv6, 6 mdns, 7 ntp_auto
 */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5657_mix4(uint32_t u32X)
{
	u32X ^= u32X >> 16;
	u32X *= 0x165667b1u;
	u32X ^= u32X >> 15;
	return u32X & 0xfu;
}

static uint32_t
b5657_pack(uint32_t u32Mode, uint32_t u32Flags, uint32_t u32Iface)
{
	uint32_t u32P;

	u32Mode = u32Mode & 0xfu;
	u32Flags = u32Flags & 0xffffu;
	u32Iface = u32Iface & 0xffu;

	u32P = u32Mode | (u32Flags << 4) | (u32Iface << 20);
	return u32P | (b5657_mix4(u32P ^ B5657_BATCH_ID) << 28);
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_net_setup_pack_5657(uint32_t mode, uint32_t flags, uint32_t iface)
{
	(void)NULL;
	return b5657_pack(mode, flags, iface);
}

uint32_t
gj_net_setup_mode_5657(uint32_t packed)
{
	return packed & 0xfu;
}

uint32_t
gj_net_setup_flags_5657(uint32_t packed)
{
	return (packed >> 4) & 0xffffu;
}

uint32_t
gj_net_setup_iface_5657(uint32_t packed)
{
	return (packed >> 20) & 0xffu;
}

uint32_t
gj_batch_id_5657(void)
{
	return B5657_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_net_setup_pack_5657(uint32_t mode, uint32_t flags,
    uint32_t iface)
    __attribute__((alias("gj_net_setup_pack_5657")));

uint32_t __gj_net_setup_mode_5657(uint32_t packed)
    __attribute__((alias("gj_net_setup_mode_5657")));

uint32_t __gj_net_setup_flags_5657(uint32_t packed)
    __attribute__((alias("gj_net_setup_flags_5657")));

uint32_t __gj_net_setup_iface_5657(uint32_t packed)
    __attribute__((alias("gj_net_setup_iface_5657")));

uint32_t __gj_batch_id_5657(void)
    __attribute__((alias("gj_batch_id_5657")));
