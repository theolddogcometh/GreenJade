/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9485: libxml2 DTD ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xml2_dtd_ok_u_9485(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       libxml2 DTD validity probe.
 *   uint32_t __gj_xml2_dtd_ok_u_9485  (alias)
 *   __libcgj_batch9485_marker = "libcgj-batch9485"
 *
 * Exclusive continuum CREATE-ONLY (9481-9490: libxml2 soft id stubs —
 * xml2_doc_ok_u_9481, xml2_node_ok_u_9482, xml2_xpath_ok_u_9483,
 * xml2_schema_ok_u_9484, xml2_dtd_ok_u_9485, xml2_parse_ok_u_9486,
 * xml2_serialize_ok_u_9487, xml2_ns_ok_u_9488, libxml2_ready_u_9489,
 * batch_id_9490). Unique surface only; no multi-def. No parent wires.
 * No __int128. No libxml2 implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9485_marker[] = "libcgj-batch9485";

/* Soft libxml2 dtd-ok lamp: always off (not a real DTD probe). */
#define B9485_XML2_DTD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9485_xml2_dtd_ok(void)
{
	return B9485_XML2_DTD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml2_dtd_ok_u_9485 - libxml2 DTD ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not validate DTDs or
 * call libc/libxml2. No parent wires.
 */
uint32_t
gj_xml2_dtd_ok_u_9485(void)
{
	(void)NULL;
	return b9485_xml2_dtd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xml2_dtd_ok_u_9485(void)
    __attribute__((alias("gj_xml2_dtd_ok_u_9485")));
