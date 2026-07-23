/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch138: CSV field parse (RFC 4180 lite).
 * Clean-room freestanding pure C — builds under userspace -msse2.
 * No third-party CSV library source was copied.
 *
 * Surface (0 success, -1 error):
 *   int gj_csv_next_field(const char **pp, const char *end, char *out,
 *                         size_t outcap, size_t *olen, int *is_last);
 *     Parse one field from [*pp, end). Advances *pp past the field and a
 *     trailing comma when more fields follow. Unescapes RFC 4180 doubled
 *     quotes inside quoted fields. Writes field bytes to out[0..*olen) when
 *     outcap is large enough; if outcap > *olen and out != NULL, also
 *     writes a trailing NUL. out == NULL && outcap == 0 is measure-only
 *     (sets *olen, still advances on success). *is_last is 1 when this
 *     field ends the record (no trailing comma). Returns -1 on bad args,
 *     unclosed quote, junk after a closing quote, or field longer than
 *     outcap (non-measure mode); *pp is left unchanged on error.
 *
 *   int gj_csv_count_fields(const char *line, size_t len);
 *     Count fields in line[0..len). Empty input is one empty field.
 *     Returns the count, or -1 on malformed input / bad args.
 *
 *   __gj_csv_next_field / __gj_csv_count_fields  (aliases)
 *   __libcgj_batch138_marker = "libcgj-batch138"
 *
 * Lite rules:
 *   - Comma separates fields; CR/LF end the record (not included in field).
 *   - Field may be quoted with "; "" inside quotes is a single ".
 *   - Unquoted fields run until comma, CR, LF, or end.
 *   - After a closing quote, only comma / CR / LF / end are accepted.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch138_marker[] = "libcgj-batch138";

/* ---- gj_csv_next_field ------------------------------------------------- */

int
gj_csv_next_field(const char **pp, const char *end, char *out,
    size_t outcap, size_t *olen, int *is_last)
{
	const char *p;
	const char *pStart;
	size_t n = 0u;
	int nMeasure;
	int nQuoted;
	int nOverflow = 0;

	if (pp == NULL || *pp == NULL || end == NULL || olen == NULL ||
	    is_last == NULL) {
		return -1;
	}
	if (out == NULL && outcap != 0u) {
		return -1;
	}
	if (*pp > end) {
		return -1;
	}

	nMeasure = (out == NULL && outcap == 0u) ? 1 : 0;
	pStart = *pp;
	p = pStart;
	nQuoted = 0;

	if (p < end && *p == '"') {
		/* Quoted field: opening " consumed, content until closing ". */
		nQuoted = 1;
		p++;
		for (;;) {
			if (p >= end) {
				/* Unclosed quote. */
				return -1;
			}
			if (*p == '"') {
				if ((p + 1) < end && p[1] == '"') {
					/* Escaped quote → one " in output. */
					if (!nMeasure) {
						if (n >= outcap) {
							nOverflow = 1;
						} else {
							out[n] = '"';
						}
					}
					n++;
					p += 2;
					continue;
				}
				/* Closing quote. */
				p++;
				break;
			}
			if (!nMeasure) {
				if (n >= outcap) {
					nOverflow = 1;
				} else {
					out[n] = *p;
				}
			}
			n++;
			p++;
		}

		/*
		 * After closing quote: only comma, CR, LF, or end-of-buffer.
		 * Anything else is non-conformant junk.
		 */
		if (p < end && *p != ',' && *p != '\r' && *p != '\n') {
			return -1;
		}
	} else {
		/* Unquoted field: until separator or record end. */
		while (p < end && *p != ',' && *p != '\r' && *p != '\n') {
			if (!nMeasure) {
				if (n >= outcap) {
					nOverflow = 1;
				} else {
					out[n] = *p;
				}
			}
			n++;
			p++;
		}
		(void)nQuoted;
	}

	if (!nMeasure && nOverflow) {
		/* Report needed size; leave *pp unchanged for retry. */
		*olen = n;
		return -1;
	}

	/* Terminator: comma → more fields; else record end. */
	if (p < end && *p == ',') {
		p++;
		*is_last = 0;
	} else {
		/* Optional CR/LF after the last field (still within [start,end)). */
		if (p < end && *p == '\r') {
			p++;
		}
		if (p < end && *p == '\n') {
			p++;
		}
		*is_last = 1;
	}

	*olen = n;
	if (out != NULL && outcap > n) {
		out[n] = '\0';
	}
	*pp = p;
	return 0;
}

int __gj_csv_next_field(const char **pp, const char *end, char *out,
    size_t outcap, size_t *olen, int *is_last)
    __attribute__((alias("gj_csv_next_field")));

/* ---- gj_csv_count_fields ----------------------------------------------- */

int
gj_csv_count_fields(const char *line, size_t len)
{
	const char *p;
	const char *end;
	int nFields = 0;
	int nLast = 0;
	size_t cb;

	if (line == NULL && len != 0u) {
		return -1;
	}

	/* Empty record: one empty field. */
	if (line == NULL || len == 0u) {
		return 1;
	}

	p = line;
	end = line + len;

	for (;;) {
		if (gj_csv_next_field(&p, end, NULL, 0u, &cb, &nLast) != 0) {
			return -1;
		}
		if (nFields == 0x7fffffff) {
			return -1;
		}
		nFields++;
		if (nLast) {
			break;
		}
	}

	return nFields;
}

int __gj_csv_count_fields(const char *line, size_t len)
    __attribute__((alias("gj_csv_count_fields")));
