/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9130: xml soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9130(void);
 *     - Returns the compile-time graph batch number for this TU (9130).
 *   uint32_t __gj_batch_id_9130  (alias)
 *   __libcgj_batch9130_marker = "libcgj-batch9130"
 *
 * Exclusive continuum CREATE-ONLY (9121-9130: xml soft id stubs —
 * xml_element_ok_u_9121, xml_attr_ok_u_9122, xml_text_ok_u_9123,
 * xml_comment_ok_u_9124, xml_cdata_ok_u_9125, xml_pi_ok_u_9126,
 * xml_ns_ok_u_9127, xml_depth_ok_u_9128, xml_ready_u_9129,
 * batch_id_9130). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128. No XML parser implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9130_marker[] = "libcgj-batch9130";

#define B9130_BATCH_ID  9130u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9130_id(void)
{
	return B9130_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9130 - report this TU's graph batch number.
 *
 * Always returns 9130.
 */
uint32_t
gj_batch_id_9130(void)
{
	(void)NULL;
	return b9130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9130(void)
    __attribute__((alias("gj_batch_id_9130")));
