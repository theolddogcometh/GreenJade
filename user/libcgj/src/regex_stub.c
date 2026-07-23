/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX regex — freestanding ERE subset (literals, ., [], ^$, |, (), *, +, ?).
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

/* Match from s[si] against pat[pi]…; return end index in s or -1. */
static int
match_here(const char *pat, int pi, const char *s, int si, int cflags);

static int
eqc(int cflags, unsigned char a, unsigned char b)
{
	if ((cflags & REG_ICASE) != 0) {
		return tolower((int)a) == tolower((int)b);
	}
	return a == b;
}

/* Advance pi past one atom; write match of s[si] into *pAdv (chars of s). */
static int
one_atom(const char *pat, int *pPi, const char *s, int si, int cflags,
	 int *pAdv)
{
	int pi = *pPi;
	unsigned char ch;
	int has = (s[si] != '\0');

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
			char *sub;
			size_t len = (size_t)(j - start);

			sub = (char *)malloc(len + 1);
			if (sub == NULL) {
				return -1;
			}
			memcpy(sub, pat + start, len);
			sub[len] = '\0';
			end = match_here(sub, 0, s, si, cflags);
			free(sub);
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
		return si == 0 ? 1 : 0;
	}
	if (pat[pi] == '$') {
		*pPi = pi + 1;
		return s[si] == '\0' ? 1 : 0;
	}
	if (!has && pat[pi] != '$') {
		return 0;
	}
	ch = (unsigned char)s[si];
	if (pat[pi] == '.') {
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
	    pat[pi] == '?' || pat[pi] == ')') {
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

/* Top-level alternation: try each branch. */
static int
match_here(const char *pat, int pi, const char *s, int si, int cflags)
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
				char *br;
				size_t len = (size_t)(i - start);
				int pos = si;
				int p = 0;
				int r, adv;
				char q;

				br = (char *)malloc(len + 1);
				if (br == NULL) {
					return -1;
				}
				memcpy(br, pat + start, len);
				br[len] = '\0';
				while (br[p] != '\0') {
					int p0 = p;

					r = one_atom(br, &p, s, pos, cflags,
						     &adv);
					if (r < 0) {
						free(br);
						return -1;
					}
					q = br[p];
					if (q == '*' || q == '+' || q == '?') {
						p++;
						if (q == '?') {
							if (r) {
								int t =
								    match_here(
									br, p, s,
									pos +
									    adv,
									cflags);
								if (t >= 0) {
									free(br);
									return t;
								}
							}
							/* skip */
							continue;
						}
						if (q == '*') {
							int best = match_here(
							    br, p, s, pos,
							    cflags);
							int cpos = pos;
							int n;

							for (n = 0; n < 256;
							     n++) {
								int pp = p0;
								int a = 0;
								int rr =
								    one_atom(
									br, &pp,
									s, cpos,
									cflags,
									&a);
								if (rr <= 0 ||
								    a == 0) {
									break;
								}
								cpos += a;
								{
									int t =
									    match_here(
										br,
										p,
										s,
										cpos,
										cflags);
									if (t >=
									    0) {
										best =
										    t;
									}
								}
							}
							free(br);
							return best;
						}
						/* + */
						{
							int best = -1;
							int cpos = pos;
							int n;
							int any = 0;

							for (n = 0; n < 256;
							     n++) {
								int pp = p0;
								int a = 0;
								int rr =
								    one_atom(
									br, &pp,
									s, cpos,
									cflags,
									&a);
								if (rr <= 0 ||
								    a == 0) {
									break;
								}
								any = 1;
								cpos += a;
								{
									int t =
									    match_here(
										br,
										p,
										s,
										cpos,
										cflags);
									if (t >=
									    0) {
										best =
										    t;
									}
								}
							}
							free(br);
							return (any &&
								best >= 0)
								   ? best
								   : -1;
						}
					}
					if (!r) {
						free(br);
						goto next_branch;
					}
					pos += adv;
				}
				free(br);
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
	n = strlen(szPattern);
	if (n > 0 && szPattern[n - 1] == '\\') {
		return REG_EESCAPE;
	}
	pCopy = (char *)malloc(n + 1);
	if (pCopy == NULL) {
		return REG_ESPACE;
	}
	memcpy(pCopy, szPattern, n + 1);
	pPreg->re_pat = pCopy;
	pPreg->re_cflags = nCflags;
	pPreg->re_nsub = count_subs(szPattern);
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

	(void)nEflags;
	if (pPreg == NULL || pPreg->re_pat == NULL || szString == NULL) {
		return REG_BADPAT;
	}
	len = (int)strlen(szString);
	if (pPreg->re_pat[0] == '^') {
		end = match_here(pPreg->re_pat, 0, szString, 0,
				 pPreg->re_cflags);
		if (end < 0) {
			return REG_NOMATCH;
		}
		if (nMatch > 0 && pMatch != NULL) {
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
		end = match_here(pPreg->re_pat, 0, szString, i,
				 pPreg->re_cflags);
		if (end >= i) {
			if (nMatch > 0 && pMatch != NULL) {
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
