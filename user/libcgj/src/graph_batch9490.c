/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9490: libxml2 soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9490(void);
 *     - Returns the compile-time graph batch number for this TU (9490).
 *   uint32_t __gj_batch_id_9490  (alias)
 *   __libcgj_batch9490_marker = "libcgj-batch9490"
 *
 * Exclusive continuum CREATE-ONLY (9481-9490: libxml2 soft id stubs —
 * xml2_doc_ok_u_9481, xml2_node_ok_u_9482, xml2_xpath_ok_u_9483,
 * xml2_schema_ok_u_9484, xml2_dtd_ok_u_9485, xml2_parse_ok_u_9486,
 * xml2_serialize_ok_u_9487, xml2_ns_ok_u_9488, libxml2_ready_u_9489,
 * batch_id_9490). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9490_marker[] = "libcgj-batch9490";

#define B9490_BATCH_ID  9490u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9490_id(void)
{
	return B9490_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9490 - report this TU's graph batch number.
 *
 * Always returns 9490.
 */
uint32_t
gj_batch_id_9490(void)
{
	(void)NULL;
	return b9490_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9490(void)
    __attribute__((alias("gj_batch_id_9490")));
