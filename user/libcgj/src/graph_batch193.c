/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch193: TLS alert body parse.
 * Clean-room freestanding pure C from the public TLS Alert layout
 * (RFC 5246 §7.2 / RFC 8446 §6) — 2-byte fixed body:
 *   AlertLevel level;             // 1 octet (warning=1, fatal=2)
 *   AlertDescription description; // 1 octet
 * Builds under userspace -msse2. No third-party TLS source was copied.
 *
 * Surface (unique symbols):
 *   int gj_tls_alert_parse(const unsigned char *in, size_t len,
 *                          unsigned *level, unsigned *desc);
 *     Parse a 2-byte TLS alert body. level = AlertLevel,
 *     desc = AlertDescription. Returns 0 on success, -1 on short/NULL
 *     input. Does not validate enum ranges — callers apply policy.
 *   __gj_tls_alert_parse  (alias)
 *   __libcgj_batch193_marker = "libcgj-batch193"
 *
 * Related (not redefined here):
 *   gj_tls_record_header_parse / build  → graph_batch136.c
 *
 * Note: this helper only decodes the alert wire fields. It does not
 * enforce ContentType == alert (21), record-layer framing, or known
 * description codes — callers apply policy.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch193_marker[] = "libcgj-batch193";

/* Fixed TLS alert body size (level + description). */
#define B193_ALERT_LEN 2u

/* ---- gj_tls_alert_parse ------------------------------------------------ */

/*
 * Decode a TLS Alert structure from in[0..len). Both out pointers are
 * required. Wire layout is two octets: level then description.
 */
int
gj_tls_alert_parse(const unsigned char *pIn, size_t cbLen,
                   unsigned *puLevel, unsigned *puDesc)
{
	unsigned uLevel;
	unsigned uDesc;

	if (pIn == NULL || puLevel == NULL || puDesc == NULL) {
		return -1;
	}
	if (cbLen < B193_ALERT_LEN) {
		return -1;
	}

	uLevel = (unsigned)pIn[0];
	uDesc = (unsigned)pIn[1];

	*puLevel = uLevel;
	*puDesc = uDesc;
	return 0;
}

int __gj_tls_alert_parse(const unsigned char *in, size_t len,
                         unsigned *level, unsigned *desc)
    __attribute__((alias("gj_tls_alert_parse")));
