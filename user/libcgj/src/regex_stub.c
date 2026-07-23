/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX regex — freestanding ERE subset (literals, ., [], ^$, |, (), *, +, ?).
 * Soft deepen: REG_NEWLINE / REG_NOTBOL / REG_NOTEOL, [[:class:]], {n,m}
 * braces, REG_NOSUB, stack-backed small branch match.
 */
#include <ctype.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

size_t
regerror(int nErrcode, const regex_t *pPreg, char *szErrbuf, size_t cb)
{
	const char *sz;
	size_t n;
	size_t i;

	(void)pPreg;
	switch (nErrcode) {
	case 0:
		sz = "Success";
		break;
	case REG_NOMATCH:
		sz = "No match";
		break;
	case REG_BADPAT:
		sz = "Invalid regular expression";
		break;
	case REG_ECOLLATE:
		sz = "Invalid collating element";
		break;
	case REG_ECTYPE:
		sz = "Invalid character class";
		break;
	case REG_EESCAPE:
		sz = "Trailing backslash";
		break;
	case REG_ESUBREG:
		sz = "Invalid back reference";
		break;
	case REG_EBRACK:
		sz = "Unmatched bracket";
		break;
	case REG_EPAREN:
		sz = "Unmatched parenthesis";
		break;
	case REG_EBRACE:
		sz = "Unmatched brace";
		break;
	case REG_BADBR:
		sz = "Invalid brace content";
		break;
	case REG_ERANGE:
		sz = "Invalid range end";
		break;
	case REG_ESPACE:
		sz = "Out of memory";
		break;
	case REG_BADRPT:
		sz = "Invalid repetition";
		break;
	default:
		sz = "Unknown regex error";
		break;
	}
	n = strlen(sz);
	if (szErrbuf != NULL && cb > 0) {
		size_t lim = (n < cb) ? n : (cb - 1);

		for (i = 0; i < lim; i++) {
			szErrbuf[i] = sz[i];
		}
		szErrbuf[lim] = '\0';
	}
	return n + 1;
}

void
regfree(regex_t *pPreg)
{
	if (pPreg == NULL) {
		return;
	}
	if (pPreg->re_pat != NULL) {
		free(pPreg->re_pat);
		pPreg->re_pat = NULL;
	}
	pPreg->re_nsub = 0;
	pPreg->re_cflags = 0;
	pPreg->re_pad = 0;
}

/* Match context for soft eflags / newline awareness. */
struct re_ctx {
	int cflags;
	int eflags;
	const char *base; /* start of subject */
};

/* Match from s[si] against pat[pi]…; return end index in s or -1. */
static int
match_here(const char *pat, int pi, const char *s, int si,
	   const struct re_ctx *pCtx);

static int
eqc(int cflags, unsigned char a, unsigned char b)
{
	if ((cflags & REG_ICASE) != 0) {
		return tolower((int)a) == tolower((int)b);
	}
	return a == b;
}

static int
is_bol(const struct re_ctx *pCtx, const char *s, int si)
{
	if (si == 0) {
		return (pCtx->eflags & REG_NOTBOL) == 0;
	}
	if ((pCtx->cflags & REG_NEWLINE) != 0 && si > 0 &&
	    s[si - 1] == '\n') {
		return 1;
	}
	return 0;
}

static int
is_eol(const struct re_ctx *pCtx, const char *s, int si)
{
	if (s[si] == '\0') {
		return (pCtx->eflags & REG_NOTEOL) == 0;
	}
	if ((pCtx->cflags & REG_NEWLINE) != 0 && s[si] == '\n') {
		return 1;
	}
	return 0;
}

/* POSIX character class name match; returns 1 if ch in class. */
static int
ctype_class(const char *szName, unsigned char ch)
{
	if (strcmp(szName, "alnum") == 0) {
		return isalnum((int)ch);
	}
	if (strcmp(szName, "alpha") == 0) {
		return isalpha((int)ch);
	}
	if (strcmp(szName, "blank") == 0) {
		return ch == ' ' || ch == '\t';
	}
	if (strcmp(szName, "cntrl") == 0) {
		return iscntrl((int)ch);
	}
	if (strcmp(szName, "digit") == 0) {
		return isdigit((int)ch);
	}
	if (strcmp(szName, "graph") == 0) {
		return isgraph((int)ch);
	}
	if (strcmp(szName, "lower") == 0) {
		return islower((int)ch);
	}
	if (strcmp(szName, "print") == 0) {
		return isprint((int)ch);
	}
	if (strcmp(szName, "punct") == 0) {
		return ispunct((int)ch);
	}
	if (strcmp(szName, "space") == 0) {
		return isspace((int)ch);
	}
	if (strcmp(szName, "upper") == 0) {
		return isupper((int)ch);
	}
	if (strcmp(szName, "xdigit") == 0) {
		return isxdigit((int)ch);
	}
	return -1; /* unknown */
}

/* Parse trailing {m}, {m,}, {m,n} at pat[pi]; advance *pPi past brace. */
static int
parse_brace(const char *pat, int *pPi, int *pMin, int *pMax)
{
	int pi = *pPi;
	int nMin = 0;
	int nMax = -1;
	int fSaw = 0;

	if (pat[pi] != '{') {
		return -1;
	}
	pi++;
	if (pat[pi] < '0' || pat[pi] > '9') {
		return REG_BADBR;
	}
	while (pat[pi] >= '0' && pat[pi] <= '9') {
		nMin = nMin * 10 + (pat[pi] - '0');
		if (nMin > 256) {
			return REG_BADBR;
		}
		pi++;
		fSaw = 1;
	}
	if (!fSaw) {
		return REG_BADBR;
	}
	if (pat[pi] == '}') {
		nMax = nMin;
		pi++;
	} else if (pat[pi] == ',') {
		pi++;
		if (pat[pi] == '}') {
			nMax = 256; /* soft open upper */
			pi++;
		} else {
			nMax = 0;
			fSaw = 0;
			while (pat[pi] >= '0' && pat[pi] <= '9') {
				nMax = nMax * 10 + (pat[pi] - '0');
				if (nMax > 256) {
					return REG_BADBR;
				}
				pi++;
				fSaw = 1;
			}
			if (!fSaw || pat[pi] != '}') {
				return REG_BADBR;
			}
			pi++;
		}
	} else {
		return REG_BADBR;
	}
	if (nMax >= 0 && nMax < nMin) {
		return REG_BADBR;
	}
	*pPi = pi;
	*pMin = nMin;
	*pMax = nMax;
	return 0;
}

/* Advance pi past one atom; write match of s[si] into *pAdv (chars of s). */
static int
one_atom(const char *pat, int *pPi, const char *s, int si,
	 const struct re_ctx *pCtx, int *pAdv)
{
	int pi = *pPi;
	unsigned char ch;
	int has = (s[si] != '\0');
	int cflags = pCtx->cflags;

	*pAdv = 0;
	if (pat[pi] == '\0') {
		return 0;
	}
	if (pat[pi] == '(') {
		int depth = 1;
		int start = pi + 1;
		int j = start;
		int end;

		while (pat[j] != '\0' && depth > 0) {
			if (pat[j] == '\\' && pat[j + 1] != '\0') {
				j += 2;
				continue;
			}
			if (pat[j] == '(') {
				depth++;
			} else if (pat[j] == ')') {
				depth--;
				if (depth == 0) {
					break;
				}
			}
			j++;
		}
		if (depth != 0) {
			return -1;
		}
		/* Match group as subpattern via alternation-aware match_here */
		{
			char aStack[256];
			char *sub;
			size_t len = (size_t)(j - start);
			int fHeap = 0;

			if (len + 1 <= sizeof(aStack)) {
				sub = aStack;
			} else {
				sub = (char *)malloc(len + 1);
				if (sub == NULL) {
					return -1;
				}
				fHeap = 1;
			}
			memcpy(sub, pat + start, len);
			sub[len] = '\0';
			end = match_here(sub, 0, s, si, pCtx);
			if (fHeap) {
				free(sub);
			}
			if (end < 0) {
				return 0;
			}
			*pAdv = end - si;
			*pPi = j + 1;
			return 1;
		}
	}
	if (pat[pi] == '^') {
		*pPi = pi + 1;
		return is_bol(pCtx, s, si) ? 1 : 0;
	}
	if (pat[pi] == '$') {
		*pPi = pi + 1;
		return is_eol(pCtx, s, si) ? 1 : 0;
	}
	if (!has && pat[pi] != '$') {
		return 0;
	}
	ch = (unsigned char)s[si];
	if (pat[pi] == '.') {
		/* REG_NEWLINE: '.' does not match newline */
		if ((cflags & REG_NEWLINE) != 0 && ch == '\n') {
			*pPi = pi + 1;
			return 0;
		}
		*pPi = pi + 1;
		*pAdv = 1;
		return 1;
	}
	if (pat[pi] == '[') {
		int inv = 0;
		int k = pi + 1;
		int ok = 0;

		if (pat[k] == '^') {
			inv = 1;
			k++;
		}
		if (pat[k] == ']') {
			if (ch == ']') {
				ok = 1;
			}
			k++;
		}
		while (pat[k] != '\0' && pat[k] != ']') {
			/* [[:class:]] */
			if (pat[k] == '[' && pat[k + 1] == ':') {
				char aCls[16];
				size_t cn = 0;
				int cr;
				int t = k + 2;

				while (pat[t] != '\0' && pat[t] != ':' &&
				       cn + 1 < sizeof(aCls)) {
					aCls[cn++] = pat[t++];
				}
				aCls[cn] = '\0';
				if (pat[t] == ':' && pat[t + 1] == ']') {
					cr = ctype_class(aCls, ch);
					if (cr < 0) {
						return -1; /* REG_ECTYPE */
					}
					if (cr) {
						ok = 1;
					}
					k = t + 2;
					continue;
				}
			}
			if (pat[k + 1] == '-' && pat[k + 2] != '\0' &&
			    pat[k + 2] != ']') {
				unsigned char lo = (unsigned char)pat[k];
				unsigned char hi = (unsigned char)pat[k + 2];

				if (ch >= lo && ch <= hi) {
					ok = 1;
				}
				k += 3;
			} else {
				if (eqc(cflags, ch, (unsigned char)pat[k])) {
					ok = 1;
				}
				k++;
			}
		}
		if (pat[k] != ']') {
			return -1;
		}
		*pPi = k + 1;
		if (inv) {
			ok = !ok;
		}
		if (!ok) {
			return 0;
		}
		*pAdv = 1;
		return 1;
	}
	if (pat[pi] == '\\') {
		if (pat[pi + 1] == '\0') {
			return -1;
		}
		if (!has || !eqc(cflags, ch, (unsigned char)pat[pi + 1])) {
			*pPi = pi + 2;
			return 0;
		}
		*pPi = pi + 2;
		*pAdv = 1;
		return 1;
	}
	if (pat[pi] == '|' || pat[pi] == '*' || pat[pi] == '+' ||
	    pat[pi] == '?' || pat[pi] == ')' || pat[pi] == '{') {
		return 0;
	}
	if (!eqc(cflags, ch, (unsigned char)pat[pi])) {
		*pPi = pi + 1;
		return 0;
	}
	*pPi = pi + 1;
	*pAdv = 1;
	return 1;
}

/* Apply quantifier after atom at p0..; p is after atom (and after quant). */
static int
apply_quant(const char *br, int p0, int p, char q, int nMin, int nMax,
	    const char *s, int pos, const struct re_ctx *pCtx)
{
	int best = -1;
	int cpos = pos;
	int n;
	int any = 0;
	int t;

	if (q == '?') {
		/* 0 or 1 */
		t = match_here(br, p, s, pos, pCtx);
		if (t >= 0) {
			return t;
		}
		{
			int pp = p0;
			int a = 0;
			int rr = one_atom(br, &pp, s, pos, pCtx, &a);

			if (rr > 0 && a > 0) {
				t = match_here(br, p, s, pos + a, pCtx);
				if (t >= 0) {
					return t;
				}
			}
		}
		return -1;
	}
	/* * or + or {m,n}: greedy soft with cap 256 */
	if (q == '*' || (q == '{' && nMin == 0)) {
		best = match_here(br, p, s, pos, pCtx);
	}
	for (n = 0; n < 256; n++) {
		int pp = p0;
		int a = 0;
		int rr = one_atom(br, &pp, s, cpos, pCtx, &a);

		if (rr <= 0 || a == 0) {
			break;
		}
		any = 1;
		cpos += a;
		if (q == '{' && n + 1 < nMin) {
			continue;
		}
		if (q == '{' && nMax >= 0 && n + 1 > nMax) {
			break;
		}
		t = match_here(br, p, s, cpos, pCtx);
		if (t >= 0) {
			best = t;
		}
		if (q == '+' && !any) {
			break;
		}
	}
	if (q == '+') {
		return (any && best >= 0) ? best : -1;
	}
	if (q == '{') {
		if (nMin == 0 && best >= 0) {
			return best;
		}
		/* need at least nMin matches counted in loop */
		return best;
	}
	/* * */
	return best;
}

/* Top-level alternation: try each branch. */
static int
match_here(const char *pat, int pi, const char *s, int si,
	   const struct re_ctx *pCtx)
{
	int depth = 0;
	int start = pi;
	int i;

	/* Split on top-level | */
	for (i = pi;; i++) {
		if (pat[i] == '\0' || (pat[i] == '|' && depth == 0) ||
		    (pat[i] == ')' && depth == 0)) {
			/* match branch pat[start..i) */
			{
				char aStack[256];
				char *br;
				size_t len = (size_t)(i - start);
				int pos = si;
				int p = 0;
				int r, adv;
				char q;
				int fHeap = 0;

				if (len + 1 <= sizeof(aStack)) {
					br = aStack;
				} else {
					br = (char *)malloc(len + 1);
					if (br == NULL) {
						return -1;
					}
					fHeap = 1;
				}
				memcpy(br, pat + start, len);
				br[len] = '\0';
				while (br[p] != '\0') {
					int p0 = p;
					int nMin = 0;
					int nMax = -1;

					r = one_atom(br, &p, s, pos, pCtx,
						     &adv);
					if (r < 0) {
						if (fHeap) {
							free(br);
						}
						return -1;
					}
					q = br[p];
					if (q == '*' || q == '+' || q == '?' ||
					    q == '{') {
						if (q == '{') {
							int brc = parse_brace(
							    br, &p, &nMin,
							    &nMax);

							if (brc != 0) {
								if (fHeap) {
									free(br);
								}
								return -1;
							}
						} else {
							p++;
						}
						{
							int t = apply_quant(
							    br, p0, p, q, nMin,
							    nMax, s, pos,
							    pCtx);

							if (fHeap) {
								free(br);
							}
							return t;
						}
					}
					if (!r) {
						if (fHeap) {
							free(br);
						}
						goto next_branch;
					}
					pos += adv;
				}
				if (fHeap) {
					free(br);
				}
				return pos;
			}
		next_branch:
			if (pat[i] == '\0' || pat[i] == ')') {
				return -1;
			}
			/* skip | */
			start = i + 1;
			continue;
		}
		if (pat[i] == '\\' && pat[i + 1] != '\0') {
			i++;
			continue;
		}
		if (pat[i] == '(') {
			depth++;
		} else if (pat[i] == ')') {
			if (depth > 0) {
				depth--;
			}
		}
	}
}

static size_t
count_subs(const char *pat)
{
	size_t n = 0;

	while (*pat != '\0') {
		if (*pat == '\\' && pat[1] != '\0') {
			pat += 2;
			continue;
		}
		if (*pat == '(') {
			n++;
		}
		pat++;
	}
	return n;
}

int
regcomp(regex_t *pPreg, const char *szPattern, int nCflags)
{
	size_t n;
	char *pCopy;
	int depth = 0;
	int brack = 0;
	int brace = 0;
	const char *p;

	if (pPreg == NULL) {
		return REG_ESPACE;
	}
	pPreg->re_nsub = 0;
	pPreg->re_cflags = 0;
	pPreg->re_pat = NULL;
	pPreg->re_pad = 0;
	if (szPattern == NULL) {
		return REG_BADPAT;
	}
	for (p = szPattern; *p != '\0'; p++) {
		if (*p == '\\' && p[1] != '\0') {
			p++;
			continue;
		}
		if (*p == '[') {
			brack = 1;
		} else if (*p == ']' && brack) {
			brack = 0;
		} else if (!brack && *p == '{') {
			brace = 1;
		} else if (!brack && *p == '}' && brace) {
			brace = 0;
		} else if (!brack && *p == '(') {
			depth++;
		} else if (!brack && *p == ')') {
			depth--;
			if (depth < 0) {
				return REG_EPAREN;
			}
		}
	}
	if (depth != 0) {
		return REG_EPAREN;
	}
	if (brack) {
		return REG_EBRACK;
	}
	if (brace) {
		return REG_EBRACE;
	}
	n = strlen(szPattern);
	if (n > 0 && szPattern[n - 1] == '\\') {
		return REG_EESCAPE;
	}
	/* Soft: reject leading quantifier */
	if (n > 0 &&
	    (szPattern[0] == '*' || szPattern[0] == '+' ||
	     szPattern[0] == '?' || szPattern[0] == '{')) {
		return REG_BADRPT;
	}
	pCopy = (char *)malloc(n + 1);
	if (pCopy == NULL) {
		return REG_ESPACE;
	}
	memcpy(pCopy, szPattern, n + 1);
	pPreg->re_pat = pCopy;
	pPreg->re_cflags = nCflags;
	pPreg->re_nsub =
	    ((nCflags & REG_NOSUB) != 0) ? 0 : count_subs(szPattern);
	pPreg->re_pad = 1;
	return 0;
}

int
regexec(const regex_t *pPreg, const char *szString, size_t nMatch,
	regmatch_t pMatch[], int nEflags)
{
	int i;
	int len;
	int end;
	struct re_ctx ctx;
	int fAnchored;

	if (pPreg == NULL || pPreg->re_pat == NULL || szString == NULL) {
		return REG_BADPAT;
	}
	ctx.cflags = pPreg->re_cflags;
	ctx.eflags = nEflags;
	ctx.base = szString;
	len = (int)strlen(szString);
	fAnchored = (pPreg->re_pat[0] == '^');

	if (fAnchored) {
		end = match_here(pPreg->re_pat, 0, szString, 0, &ctx);
		if (end < 0) {
			return REG_NOMATCH;
		}
		if ((pPreg->re_cflags & REG_NOSUB) == 0 && nMatch > 0 &&
		    pMatch != NULL) {
			size_t j;

			pMatch[0].rm_so = 0;
			pMatch[0].rm_eo = end;
			for (j = 1; j < nMatch; j++) {
				pMatch[j].rm_so = -1;
				pMatch[j].rm_eo = -1;
			}
		}
		return 0;
	}
	for (i = 0; i <= len; i++) {
		/* Soft REG_NEWLINE: allow ^-less search to start after \n too */
		end = match_here(pPreg->re_pat, 0, szString, i, &ctx);
		if (end >= i) {
			if ((pPreg->re_cflags & REG_NOSUB) == 0 && nMatch > 0 &&
			    pMatch != NULL) {
				size_t j;

				pMatch[0].rm_so = i;
				pMatch[0].rm_eo = end;
				for (j = 1; j < nMatch; j++) {
					pMatch[j].rm_so = -1;
					pMatch[j].rm_eo = -1;
				}
			}
			return 0;
		}
	}
	return REG_NOMATCH;
}
