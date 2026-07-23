/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5160: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5160(void);
 *     - Returns the compile-time graph batch number for this TU (5160).
 *   uint32_t __gj_batch_id_5160  (alias)
 *   __libcgj_batch5160_marker = "libcgj-batch5160"
 *
 * Exclusive continuum CREATE-ONLY (5151-5160: tty_cols_default_u,
 * tty_rows_default_u, tty_is_ctrl_u, tty_is_print_u, tty_to_ctrl_u,
 * tty_bell_u, tty_tab_u, tty_lf_u, tty_cr_u, batch_id_5160).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5160_marker[] = "libcgj-batch5160";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5160_id(void)
{
	return 5160u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5160 - report this TU's graph batch number.
 *
 * Always returns 5160. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5160(void)
{
	(void)NULL;
	return b5160_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5160(void)
    __attribute__((alias("gj_batch_id_5160")));
