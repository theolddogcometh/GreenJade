/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6756: Landlock rule type request pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ll_rule_type_pack_u_6756(uint32_t want_path, uint32_t want_net);
 *     - Pack soft path/net rule-type request flags into a 2-bit mask.
 *   uint32_t __gj_ll_rule_type_pack_u_6756  (alias)
 *   __libcgj_batch6756_marker = "libcgj-batch6756"
 *
 * Exclusive continuum CREATE-ONLY (6751-6760: landlock rule type stubs —
 * path_beneath, net_port, type_ok, path_id, net_id, pack, needs_path_attr,
 * needs_net_attr, errorish, batch_id_6760). Unique
 * gj_ll_rule_type_pack_u_6756 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6756_marker[] = "libcgj-batch6756";

/* Soft request bits: bit0 = PATH_BENEATH, bit1 = NET_PORT. */
#define B6756_WANT_PATH ((uint32_t)0x1u)
#define B6756_WANT_NET  ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6756_ll_rule_type_pack(uint32_t u32WantPath, uint32_t u32WantNet)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantPath != 0u)
		u32Pack |= B6756_WANT_PATH;
	if (u32WantNet != 0u)
		u32Pack |= B6756_WANT_NET;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ll_rule_type_pack_u_6756 - soft-pack path/net rule-type wants.
 *
 * want_path: non-zero to request PATH_BENEATH type support
 * want_net:  non-zero to request NET_PORT type support
 *
 * Returns bitmask: bit0=path, bit1=net. Pure integer pack; no parent wires.
 */
uint32_t
gj_ll_rule_type_pack_u_6756(uint32_t u32WantPath, uint32_t u32WantNet)
{
	(void)NULL;
	return b6756_ll_rule_type_pack(u32WantPath, u32WantNet);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ll_rule_type_pack_u_6756(uint32_t u32WantPath, uint32_t u32WantNet)
    __attribute__((alias("gj_ll_rule_type_pack_u_6756")));
