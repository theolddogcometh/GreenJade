/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch80: minimal freestanding POSIX BRE/ERE subset.
 * Clean-room pure C — integer/pointer only (no SSE, no libc string.h).
 *
 * Existing surface NOT redefined (avoid multi-def with regex_stub.c):
 *   regcomp / regexec / regerror / regfree → regex_stub.c
 *     (regcomp always returns REG_BADPAT; engine not implemented there)
 *
 * This TU adds only unique gj_re_* symbols:
 *   gj_re_match(const char *pat, const char *str)
 *     Unanchored search: 0 if pattern matches somewhere in str, -1 otherwise.
 *     '^' anchors to start of str; '$' anchors to end.
 *   gj_re_fullmatch(const char *pat, const char *str)
 *     Whole-string match (as if ^pat$ without requiring the carets).
 *   gj_re_search(const char *pat, const char *str, size_t *pSo, size_t *pEo)
 *     Like match, but reports byte span [so, eo) on success.
 *   gj_re_match_flags — BRE/ERE/ICASE/NEWLINE bitmask soft deepen
 *   __gj_re_match / __gj_re_fullmatch / __gj_re_search / __gj_re_match_flags
 *   __libcgj_batch80_marker = "libcgj-batch80"
 *
 * Soft deepen: ICASE / NEWLINE / BRE flag paths, shorthands \\d\\s\\w,
 * null-arg reject in search entry.
 *
 * Supported ERE subset:
 *   . * + ? | ( ) [ ] [^ ] a-z ranges  ^ $  \-escapes for meta
 *   \\d \\D \\s \\S \\w \\W  (GNU-ish shorthands, freestanding)
 * Unsupported: {m,n}, backrefs, collating elements, POSIX [[:class:]].
 * BRE: treat most metas as literal unless escaped (see GJ_RE_BRE flag).
 *
 * No third-party regex source was copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch80_marker[] = "libcgj-batch80";

/* Optional flags for gj_re_*_flags helpers (bitmask). */
#define GJ_RE_EXTENDED 1 /* ERE (default for gj_re_match) */
#define GJ_RE_BRE      2 /* basic RE: ( ) { } | * require \ */
#define GJ_RE_ICASE    4 /* case-insensitive */
#define GJ_RE_NEWLINE  8 /* . does not match \n; ^/$ also at \n edges */

/* --------------------------------------------------------------------------
 * Freestanding helpers
 * -------------------------------------------------------------------------- */

static size_t
b80_strlen(const char *s)
{
	size_t n = 0;

	if (s == NULL) {
		return 0;
	}
	while (s[n] != '\0') {
		n++;
	}
	return n;
}

static int
b80_tolower(int c)
{
	if (c >= 'A' && c <= 'Z') {
		return c - 'A' + 'a';
	}
	return c;
}

static int
b80_eqc(int a, int b, int fIcase)
{
	if (fIcase) {
		return b80_tolower(a) == b80_tolower(b);
	}
	return a == b;
}

static int
b80_isdigit(int c)
{
	return c >= '0' && c <= '9';
}

static int
b80_isspace(int c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ||
	       c == '\v';
}

static int
b80_isword(int c)
{
	return b80_isdigit(c) || (c >= 'A' && c <= 'Z') ||
	       (c >= 'a' && c <= 'z') || c == '_';
}

/* --------------------------------------------------------------------------
 * Pattern helpers — classify ERE vs BRE meta
 * -------------------------------------------------------------------------- */

typedef struct {
	const char *pat;
	const char *pend; /* exclusive end of pattern window, or NULL = NUL */
	const char *base; /* original string start (for ^ / so) */
	const char *str;  /* current string position */
	const char *send; /* exclusive end of string, or NULL = scan to NUL */
	int         flags;
} b80_ctx;

static int b80_match_alt(b80_ctx *cx, const char *p, const char *s);

static int
b80_at_end_pat(const b80_ctx *cx, const char *p)
{
	if (cx->pend != NULL) {
		return p >= cx->pend;
	}
	return p == NULL || *p == '\0';
}

static int
b80_at_end_str(const b80_ctx *cx, const char *s)
{
	if (cx->send != NULL) {
		return s >= cx->send;
	}
	return s == NULL || *s == '\0';
}

static int
b80_is_ere(const b80_ctx *cx)
{
	return (cx->flags & GJ_RE_BRE) == 0;
}

/*
 * Find matching ')' for '(' at p-1 (p points just after open).
 * Respects nesting and character classes. Returns pointer to ')' or NULL.
 */
static const char *
b80_find_rparen(const b80_ctx *cx, const char *p)
{
	int depth = 1;
	int in_class = 0;

	while (!b80_at_end_pat(cx, p)) {
		char c = *p;

		if (c == '\\') {
			p++;
			if (b80_at_end_pat(cx, p)) {
				break;
			}
			p++;
			continue;
		}
		if (in_class) {
			if (c == ']') {
				in_class = 0;
			}
			p++;
			continue;
		}
		if (c == '[') {
			in_class = 1;
			p++;
			/* allow leading ^ and ] */
			if (!b80_at_end_pat(cx, p) && *p == '^') {
				p++;
			}
			if (!b80_at_end_pat(cx, p) && *p == ']') {
				p++;
			}
			continue;
		}
		if (b80_is_ere(cx)) {
			if (c == '(') {
				depth++;
			} else if (c == ')') {
				depth--;
				if (depth == 0) {
					return p;
				}
			}
		} else {
			/* BRE: \( \) */
			if (c == '\\' && !b80_at_end_pat(cx, p + 1)) {
				/* already handled above */
			}
		}
		p++;
	}
	return NULL;
}

/* BRE-aware: find matching \) after \( */
static const char *
b80_find_rparen_bre(const b80_ctx *cx, const char *p)
{
	int depth = 1;

	while (!b80_at_end_pat(cx, p)) {
		if (*p == '\\' && !b80_at_end_pat(cx, p + 1)) {
			char n = p[1];

			if (n == '(') {
				depth++;
				p += 2;
				continue;
			}
			if (n == ')') {
				depth--;
				if (depth == 0) {
					return p; /* points at '\' of \) */
				}
				p += 2;
				continue;
			}
			p += 2;
			continue;
		}
		if (*p == '[') {
			p++;
			if (!b80_at_end_pat(cx, p) && *p == '^') {
				p++;
			}
			if (!b80_at_end_pat(cx, p) && *p == ']') {
				p++;
			}
			while (!b80_at_end_pat(cx, p) && *p != ']') {
				if (*p == '\\' && !b80_at_end_pat(cx, p + 1)) {
					p += 2;
				} else {
					p++;
				}
			}
			if (!b80_at_end_pat(cx, p) && *p == ']') {
				p++;
			}
			continue;
		}
		p++;
	}
	return NULL;
}

/*
 * Split top-level alternation: return pointer to '|' or NULL if none.
 * p..end is the branch window (end exclusive, or NUL-terminated).
 */
static const char *
b80_find_alt(const b80_ctx *cx, const char *p, const char *end)
{
	int depth = 0;
	int in_class = 0;
	const char *lim = end;

	while (!b80_at_end_pat(cx, p) && (lim == NULL || p < lim)) {
		char c = *p;

		if (c == '\\') {
			if (b80_is_ere(cx)) {
				p++;
				if (b80_at_end_pat(cx, p) ||
				    (lim != NULL && p >= lim)) {
					break;
				}
				p++;
				continue;
			}
			/* BRE: \| is alternation at depth 0 */
			if (!b80_at_end_pat(cx, p + 1) &&
			    (lim == NULL || p + 1 < lim)) {
				char n = p[1];

				if (n == '|' && depth == 0 && !in_class) {
					return p;
				}
				if (n == '(') {
					depth++;
					p += 2;
					continue;
				}
				if (n == ')') {
					if (depth > 0) {
						depth--;
					}
					p += 2;
					continue;
				}
				p += 2;
				continue;
			}
			p++;
			continue;
		}
		if (in_class) {
			if (c == ']') {
				in_class = 0;
			}
			p++;
			continue;
		}
		if (c == '[') {
			in_class = 1;
			p++;
			if ((lim == NULL || p < lim) && !b80_at_end_pat(cx, p) &&
			    *p == '^') {
				p++;
			}
			if ((lim == NULL || p < lim) && !b80_at_end_pat(cx, p) &&
			    *p == ']') {
				p++;
			}
			continue;
		}
		if (b80_is_ere(cx)) {
			if (c == '(') {
				depth++;
			} else if (c == ')') {
				if (depth > 0) {
					depth--;
				}
			} else if (c == '|' && depth == 0) {
				return p;
			}
		}
		p++;
	}
	return NULL;
}

/* One character class match; *pp advanced past ']'. Returns 1/0 match. */
static int
b80_class_match(b80_ctx *cx, const char **pp, int ch)
{
	const char *p = *pp;
	int         neg = 0;
	int         matched = 0;
	int         first = 1;

	if (b80_at_end_pat(cx, p) || *p != '[') {
		return 0;
	}
	p++;
	if (!b80_at_end_pat(cx, p) && *p == '^') {
		neg = 1;
		p++;
	}
	/* empty class is invalid — treat as no match */
	if (b80_at_end_pat(cx, p) || *p == ']') {
		/* allow ] as first data char */
		if (!b80_at_end_pat(cx, p) && *p == ']') {
			matched = (ch == ']');
			p++;
			first = 0;
		}
	}
	while (!b80_at_end_pat(cx, p) && *p != ']') {
		int a;
		int b;

		if (*p == '\\' && !b80_at_end_pat(cx, p + 1)) {
			p++;
			a = (unsigned char)*p;
			p++;
		} else {
			a = (unsigned char)*p;
			p++;
		}
		if (!b80_at_end_pat(cx, p) && *p == '-' &&
		    !b80_at_end_pat(cx, p + 1) && p[1] != ']') {
			p++; /* skip '-' */
			if (*p == '\\' && !b80_at_end_pat(cx, p + 1)) {
				p++;
				b = (unsigned char)*p;
				p++;
			} else {
				b = (unsigned char)*p;
				p++;
			}
			if (cx->flags & GJ_RE_ICASE) {
				int lo = b80_tolower(ch);
				int la = b80_tolower(a);
				int lb = b80_tolower(b);

				if (la <= lo && lo <= lb) {
					matched = 1;
				}
			} else if (a <= ch && ch <= b) {
				matched = 1;
			}
		} else {
			if (b80_eqc(a, ch, cx->flags & GJ_RE_ICASE)) {
				matched = 1;
			}
		}
		(void)first;
		first = 0;
	}
	if (!b80_at_end_pat(cx, p) && *p == ']') {
		p++;
	}
	*pp = p;
	if (neg) {
		return !matched;
	}
	return matched;
}

static int
b80_is_quant(const b80_ctx *cx, const char *p)
{
	if (b80_at_end_pat(cx, p)) {
		return 0;
	}
	if (b80_is_ere(cx)) {
		return *p == '*' || *p == '+' || *p == '?';
	}
	/* BRE: * is special unescaped; \+ \? are special when escaped */
	if (*p == '*') {
		return 1;
	}
	if (*p == '\\' && !b80_at_end_pat(cx, p + 1)) {
		return p[1] == '+' || p[1] == '?';
	}
	return 0;
}

static char
b80_quant_kind(const b80_ctx *cx, const char *p, const char **pAfter)
{
	if (b80_is_ere(cx)) {
		char k = *p;

		*pAfter = p + 1;
		return k;
	}
	if (*p == '*') {
		*pAfter = p + 1;
		return '*';
	}
	if (*p == '\\' && !b80_at_end_pat(cx, p + 1)) {
		*pAfter = p + 2;
		return p[1];
	}
	*pAfter = p;
	return 0;
}

/* Match one atom at s; returns advanced s or NULL on failure.
 * *pp advanced past atom (no quantifier). */
static const char *
b80_match_atom(b80_ctx *cx, const char **pp, const char *s)
{
	const char *p = *pp;
	int         ch;

	if (b80_at_end_pat(cx, p)) {
		return s;
	}

	/* group */
	if (b80_is_ere(cx) && *p == '(') {
		const char *rp = b80_find_rparen(cx, p + 1);
		const char *inner_end;
		const char *save_pend;
		const char *ns;
		int         ok;

		if (rp == NULL) {
			return NULL;
		}
		inner_end = rp;
		save_pend = cx->pend;
		cx->pend = inner_end;
		ok = b80_match_alt(cx, p + 1, s);
		ns = cx->str;
		cx->pend = save_pend;
		if (!ok) {
			return NULL;
		}
		*pp = rp + 1;
		return ns;
	}
	if (!b80_is_ere(cx) && *p == '\\' && !b80_at_end_pat(cx, p + 1) &&
	    p[1] == '(') {
		const char *rp = b80_find_rparen_bre(cx, p + 2);
		const char *save_pend;
		const char *ns;
		int         ok;

		if (rp == NULL) {
			return NULL;
		}
		save_pend = cx->pend;
		cx->pend = rp;
		ok = b80_match_alt(cx, p + 2, s);
		ns = cx->str;
		cx->pend = save_pend;
		if (!ok) {
			return NULL;
		}
		*pp = rp + 2;
		return ns;
	}

	/* end of string required for atom consume */
	if (b80_at_end_str(cx, s)) {
		/* only empty group would work — already handled */
		return NULL;
	}
	ch = (unsigned char)*s;

	if (*p == '\\') {
		p++;
		if (b80_at_end_pat(cx, p)) {
			/* trailing backslash: match literal \ */
			if (ch != '\\') {
				return NULL;
			}
			*pp = p;
			return s + 1;
		}
		switch (*p) {
		case 'd':
			if (!b80_isdigit(ch)) {
				return NULL;
			}
			break;
		case 'D':
			if (b80_isdigit(ch)) {
				return NULL;
			}
			break;
		case 's':
			if (!b80_isspace(ch)) {
				return NULL;
			}
			break;
		case 'S':
			if (b80_isspace(ch)) {
				return NULL;
			}
			break;
		case 'w':
			if (!b80_isword(ch)) {
				return NULL;
			}
			break;
		case 'W':
			if (b80_isword(ch)) {
				return NULL;
			}
			break;
		case 'n':
			if (ch != '\n') {
				return NULL;
			}
			break;
		case 't':
			if (ch != '\t') {
				return NULL;
			}
			break;
		default:
			/* BRE specials handled above for \( ; else literal */
			if (!b80_eqc((unsigned char)*p, ch,
			             cx->flags & GJ_RE_ICASE)) {
				return NULL;
			}
			break;
		}
		*pp = p + 1;
		return s + 1;
	}

	if (*p == '.') {
		if ((cx->flags & GJ_RE_NEWLINE) && ch == '\n') {
			return NULL;
		}
		*pp = p + 1;
		return s + 1;
	}

	if (*p == '[') {
		const char *q = p;

		if (!b80_class_match(cx, &q, ch)) {
			return NULL;
		}
		*pp = q;
		return s + 1;
	}

	/* plain literal; in ERE, meta without atom context shouldn't appear */
	if (b80_eqc((unsigned char)*p, ch, cx->flags & GJ_RE_ICASE)) {
		*pp = p + 1;
		return s + 1;
	}
	return NULL;
}

/*
 * Greedy quantifier: match atom as many times as possible then backtrack.
 * kind: '*' | '+' | '?'
 */
static int
b80_match_quant(b80_ctx *cx, const char *atom, const char *after_atom,
                char kind, const char *rest, const char *s)
{
	const char *stack[128];
	int         n = 0;
	const char *cur = s;
	int         minc = 0;
	int         maxc = 128;
	int         i;

	if (kind == '+') {
		minc = 1;
	} else if (kind == '?') {
		maxc = 1;
	}

	/* collect greedy matches of atom */
	stack[n++] = cur;
	while (n <= maxc) {
		const char *p = atom;
		const char *ns;
		const char *save_pend = cx->pend;

		/* atom window is atom..after_atom */
		cx->pend = after_atom;
		ns = b80_match_atom(cx, &p, cur);
		cx->pend = save_pend;
		if (ns == NULL || ns == cur) {
			/* fail or empty — stop to avoid infinite loop */
			break;
		}
		/* ensure atom consumed fully */
		if (p != after_atom) {
			/* try match_alt style for multi-char atom */
			const char *save = cx->str;
			int         ok;

			cx->pend = after_atom;
			cx->str = cur;
			ok = b80_match_alt(cx, atom, cur);
			ns = cx->str;
			cx->pend = save_pend;
			if (!ok || ns == cur) {
				cx->str = save;
				break;
			}
			cx->str = save;
		}
		cur = ns;
		if (n < 128) {
			stack[n++] = cur;
		} else {
			break;
		}
		if (kind == '?') {
			break;
		}
	}

	/* backtrack from most greedy */
	for (i = n - 1; i >= 0; i--) {
		if (i < minc) {
			break;
		}
		if (b80_match_alt(cx, rest, stack[i])) {
			return 1;
		}
	}
	return 0;
}

/* Match a single branch (no top-level |); advances cx->str on success. */
static int
b80_match_branch(b80_ctx *cx, const char *p, const char *s)
{
	while (!b80_at_end_pat(cx, p)) {
		const char *atom_start;
		const char *after_atom;
		const char *after_quant;
		char        kind;
		const char *ns;
		const char *p2;

		/* $ anchor */
		if (*p == '$' && b80_is_ere(cx)) {
			/* only special at end of branch / before | */
			const char *q = p + 1;

			if (b80_at_end_pat(cx, q) || *q == '|') {
				if (b80_at_end_str(cx, s)) {
					cx->str = s;
					return 1;
				}
				if ((cx->flags & GJ_RE_NEWLINE) && s != NULL &&
				    *s == '\n') {
					cx->str = s;
					return 1;
				}
				return 0;
			}
			/* literal $ mid-pattern */
		}
		if (!b80_is_ere(cx) && *p == '\\' &&
		    !b80_at_end_pat(cx, p + 1) && p[1] == '$') {
			const char *q = p + 2;

			if (b80_at_end_pat(cx, q) ||
			    (*q == '\\' && !b80_at_end_pat(cx, q + 1) &&
			     q[1] == '|')) {
				if (b80_at_end_str(cx, s)) {
					cx->str = s;
					return 1;
				}
				return 0;
			}
		}

		/* ^ only meaningful at start of branch — mid ^ is literal in ERE */
		if (*p == '^' && b80_is_ere(cx) && s == cx->base) {
			p++;
			continue;
		}

		atom_start = p;
		/* skip atom for quant detection */
		if (b80_is_ere(cx) && *p == '(') {
			const char *rp = b80_find_rparen(cx, p + 1);

			after_atom = (rp != NULL) ? rp + 1 : p + 1;
		} else if (!b80_is_ere(cx) && *p == '\\' &&
		           !b80_at_end_pat(cx, p + 1) && p[1] == '(') {
			const char *rp = b80_find_rparen_bre(cx, p + 2);

			after_atom = (rp != NULL) ? rp + 2 : p + 2;
		} else if (*p == '[') {
			const char *q = p;
			int         ch = b80_at_end_str(cx, s) ? 0
			                                       : (unsigned char)*s;

			(void)b80_class_match(cx, &q, ch);
			after_atom = q;
		} else if (*p == '\\') {
			after_atom = p + 1;
			if (!b80_at_end_pat(cx, after_atom)) {
				after_atom++;
			}
		} else {
			after_atom = p + 1;
		}

		if (b80_is_quant(cx, after_atom)) {
			kind = b80_quant_kind(cx, after_atom, &after_quant);
			if (b80_match_quant(cx, atom_start, after_atom, kind,
			                    after_quant, s)) {
				return 1;
			}
			return 0;
		}

		/* no quantifier — match one atom */
		p2 = p;
		ns = b80_match_atom(cx, &p2, s);
		if (ns == NULL) {
			return 0;
		}
		p = p2;
		s = ns;
	}
	cx->str = s;
	return 1;
}

/* Match alternation of branches. */
static int
b80_match_alt(b80_ctx *cx, const char *p, const char *s)
{
	const char *end = cx->pend;
	const char *bar;

	for (;;) {
		bar = b80_find_alt(cx, p, end);
		if (bar == NULL) {
			/* last branch: p..end */
			return b80_match_branch(cx, p, s);
		}
		{
			const char *save_pend = cx->pend;
			int         ok;

			cx->pend = bar;
			ok = b80_match_branch(cx, p, s);
			cx->pend = save_pend;
			if (ok) {
				return 1;
			}
		}
		/* next branch after | */
		if (b80_is_ere(cx)) {
			p = bar + 1;
		} else {
			/* bar points at '\' of \| */
			p = bar + 2;
		}
	}
}

/* Does pattern match at position s (anchored here)? */
static int
b80_match_here(b80_ctx *cx, const char *pat, const char *s)
{
	const char *p = pat;

	/* strip leading ^ */
	if (!b80_at_end_pat(cx, p)) {
		if (b80_is_ere(cx) && *p == '^') {
			if (s != cx->base) {
				if (!((cx->flags & GJ_RE_NEWLINE) && s > cx->base &&
				      s[-1] == '\n')) {
					return 0;
				}
			}
			p++;
		} else if (!b80_is_ere(cx) && *p == '\\' &&
		           !b80_at_end_pat(cx, p + 1) && p[1] == '^') {
			if (s != cx->base) {
				return 0;
			}
			p += 2;
		}
	}
	return b80_match_alt(cx, p, s);
}

/* Core search: find first match; set *pSo *pEo if non-NULL. */
static int
b80_search(const char *pat, const char *str, size_t *pSo, size_t *pEo,
           int flags, int fFull)
{
	const char *s;
	size_t      slen;
	b80_ctx     cx;
	int         anchored = 0;

	if (pat == NULL || str == NULL) {
		return -1;
	}

	slen = b80_strlen(str);
	cx.pat = pat;
	cx.pend = NULL;
	cx.base = str;
	cx.str = str;
	cx.send = NULL;
	cx.flags = flags;

	if (fFull) {
		if (!b80_match_here(&cx, pat, str)) {
			return -1;
		}
		if (!b80_at_end_str(&cx, cx.str)) {
			return -1;
		}
		if (pSo != NULL) {
			*pSo = 0;
		}
		if (pEo != NULL) {
			*pEo = (size_t)(cx.str - str);
		}
		return 0;
	}

	/* detect leading ^ for early exit */
	if ((flags & GJ_RE_BRE) == 0 && pat[0] == '^') {
		anchored = 1;
	}
	if ((flags & GJ_RE_BRE) != 0 && pat[0] == '\\' && pat[1] == '^') {
		anchored = 1;
	}

	s = str;
	do {
		cx.str = s;
		cx.pend = NULL;
		if (b80_match_here(&cx, pat, s)) {
			if (pSo != NULL) {
				*pSo = (size_t)(s - str);
			}
			if (pEo != NULL) {
				*pEo = (size_t)(cx.str - str);
			}
			return 0;
		}
		if (anchored) {
			break;
		}
		if (*s == '\0') {
			break;
		}
		s++;
	} while (s <= str + slen);

	(void)slen;
	return -1;
}

/* --------------------------------------------------------------------------
 * Public surface
 * -------------------------------------------------------------------------- */

/*
 * gj_re_match — unanchored ERE search.
 * Returns 0 on match, -1 on no match or NULL args.
 */
int
gj_re_match(const char *pat, const char *str)
{
	return b80_search(pat, str, NULL, NULL, GJ_RE_EXTENDED, 0);
}

/*
 * gj_re_fullmatch — entire string must match pattern (ERE).
 */
int
gj_re_fullmatch(const char *pat, const char *str)
{
	return b80_search(pat, str, NULL, NULL, GJ_RE_EXTENDED, 1);
}

/*
 * gj_re_search — unanchored ERE; optional [so, eo) byte span.
 */
int
gj_re_search(const char *pat, const char *str, size_t *pSo, size_t *pEo)
{
	return b80_search(pat, str, pSo, pEo, GJ_RE_EXTENDED, 0);
}

/*
 * gj_re_match_flags — same as search/full with flags bitmask.
 * fFull != 0 → fullmatch semantics.
 */
int
gj_re_match_flags(const char *pat, const char *str, int flags, int fFull,
                  size_t *pSo, size_t *pEo)
{
	int f = flags;

	if ((f & (GJ_RE_EXTENDED | GJ_RE_BRE)) == 0) {
		f |= GJ_RE_EXTENDED;
	}
	return b80_search(pat, str, pSo, pEo, f, fFull);
}

int __gj_re_match(const char *pat, const char *str)
    __attribute__((alias("gj_re_match")));
int __gj_re_fullmatch(const char *pat, const char *str)
    __attribute__((alias("gj_re_fullmatch")));
int __gj_re_search(const char *pat, const char *str, size_t *pSo, size_t *pEo)
    __attribute__((alias("gj_re_search")));
int __gj_re_match_flags(const char *pat, const char *str, int flags, int fFull,
                        size_t *pSo, size_t *pEo)
    __attribute__((alias("gj_re_match_flags")));
