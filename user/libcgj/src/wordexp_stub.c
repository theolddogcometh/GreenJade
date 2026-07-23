/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * wordexp/wordfree — freestanding fill-in (soft deepen):
 *   quote-aware field split, ~ → $HOME, $VAR / ${VAR},
 *   WRDE_APPEND / WRDE_REUSE / WRDE_UNDEF,
 *   single/double quotes. Shell metachar glob/pipe/redir still WRDE_BADCHAR.
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>

static int
is_shell_meta(char ch)
{
	return ch == '*' || ch == '?' || ch == '[' || ch == '{' || ch == '`' ||
	       ch == '|' || ch == '&' || ch == ';' || ch == '<' || ch == '>' ||
	       ch == '(' || ch == ')';
}

static int
is_ifs(char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n';
}

static int
expand_token(const char *szIn, char **ppOut, int nFlags)
{
	char aBuf[1024];
	size_t o = 0;
	size_t i = 0;
	int quote = 0; /* 1=single 2=double */

	if (szIn == NULL) {
		*ppOut = NULL;
		return WRDE_NOSPACE;
	}
	/* leading ~ */
	if (szIn[0] == '~' && (szIn[1] == '\0' || szIn[1] == '/')) {
		const char *home = getenv("HOME");

		if (home == NULL) {
			home = "/";
		}
		while (*home != '\0' && o + 1 < sizeof(aBuf)) {
			aBuf[o++] = *home++;
		}
		i = 1;
	}
	while (szIn[i] != '\0' && o + 1 < sizeof(aBuf)) {
		char ch = szIn[i];

		if (quote == 1) {
			if (ch == '\'') {
				quote = 0;
				i++;
				continue;
			}
			aBuf[o++] = ch;
			i++;
			continue;
		}
		if (quote == 2) {
			if (ch == '"') {
				quote = 0;
				i++;
				continue;
			}
			if (ch == '\\' && szIn[i + 1] != '\0') {
				aBuf[o++] = szIn[i + 1];
				i += 2;
				continue;
			}
			if (ch == '$') {
				/* fall through to $ expand */
			} else {
				aBuf[o++] = ch;
				i++;
				continue;
			}
		}
		if (ch == '\'') {
			quote = 1;
			i++;
			continue;
		}
		if (ch == '"') {
			quote = 2;
			i++;
			continue;
		}
		if (ch == '\\' && szIn[i + 1] != '\0') {
			aBuf[o++] = szIn[i + 1];
			i += 2;
			continue;
		}
		if (ch == '$') {
			const char *name;
			size_t nl = 0;
			char aName[128];
			const char *val;

			i++;
			if (szIn[i] == '{') {
				i++;
				while (szIn[i] != '\0' && szIn[i] != '}' &&
				       nl + 1 < sizeof(aName)) {
					aName[nl++] = szIn[i++];
				}
				if (szIn[i] == '}') {
					i++;
				}
			} else {
				while ((szIn[i] >= 'A' && szIn[i] <= 'Z') ||
				       (szIn[i] >= 'a' && szIn[i] <= 'z') ||
				       (szIn[i] >= '0' && szIn[i] <= '9') ||
				       szIn[i] == '_') {
					if (nl + 1 < sizeof(aName)) {
						aName[nl++] = szIn[i];
					}
					i++;
				}
			}
			aName[nl] = '\0';
			name = aName;
			if (nl == 0) {
				/* lone $ */
				if (o + 1 < sizeof(aBuf)) {
					aBuf[o++] = '$';
				}
				continue;
			}
			val = getenv(name);
			if (val == NULL) {
				if ((nFlags & WRDE_UNDEF) != 0) {
					return WRDE_BADVAL;
				}
			} else {
				while (*val != '\0' && o + 1 < sizeof(aBuf)) {
					aBuf[o++] = *val++;
				}
			}
			continue;
		}
		if (is_shell_meta(ch)) {
			return WRDE_BADCHAR;
		}
		aBuf[o++] = ch;
		i++;
	}
	if (quote != 0) {
		return WRDE_SYNTAX;
	}
	aBuf[o] = '\0';
	*ppOut = strdup(aBuf);
	if (*ppOut == NULL) {
		return WRDE_NOSPACE;
	}
	return 0;
}

/*
 * Quote-aware field split: extract next token starting at *pIdx into aTok.
 * Returns 0 on token, 1 on end, <0 on syntax error (WRDE_*).
 */
static int
next_field(const char *szWords, size_t *pIdx, char *aTok, size_t cbTok)
{
	size_t i = *pIdx;
	size_t o = 0;
	int quote = 0;

	while (szWords[i] != '\0' && is_ifs(szWords[i])) {
		i++;
	}
	if (szWords[i] == '\0') {
		*pIdx = i;
		return 1;
	}
	while (szWords[i] != '\0') {
		char ch = szWords[i];

		if (quote == 1) {
			if (o + 1 >= cbTok) {
				return WRDE_NOSPACE;
			}
			aTok[o++] = ch;
			if (ch == '\'') {
				quote = 0;
			}
			i++;
			continue;
		}
		if (quote == 2) {
			if (o + 1 >= cbTok) {
				return WRDE_NOSPACE;
			}
			aTok[o++] = ch;
			if (ch == '\\' && szWords[i + 1] != '\0') {
				if (o + 1 >= cbTok) {
					return WRDE_NOSPACE;
				}
				aTok[o++] = szWords[i + 1];
				i += 2;
				continue;
			}
			if (ch == '"') {
				quote = 0;
			}
			i++;
			continue;
		}
		if (is_ifs(ch)) {
			break;
		}
		if (ch == '\'') {
			quote = 1;
			if (o + 1 >= cbTok) {
				return WRDE_NOSPACE;
			}
			aTok[o++] = ch;
			i++;
			continue;
		}
		if (ch == '"') {
			quote = 2;
			if (o + 1 >= cbTok) {
				return WRDE_NOSPACE;
			}
			aTok[o++] = ch;
			i++;
			continue;
		}
		if (ch == '\\' && szWords[i + 1] != '\0') {
			if (o + 2 >= cbTok) {
				return WRDE_NOSPACE;
			}
			aTok[o++] = ch;
			aTok[o++] = szWords[i + 1];
			i += 2;
			continue;
		}
		if (o + 1 >= cbTok) {
			return WRDE_NOSPACE;
		}
		aTok[o++] = ch;
		i++;
	}
	if (quote != 0) {
		return WRDE_SYNTAX;
	}
	aTok[o] = '\0';
	*pIdx = i;
	return 0;
}

static void
we_free_words(wordexp_t *pWe)
{
	size_t i;
	size_t cTotal;

	if (pWe == NULL || pWe->we_wordv == NULL) {
		return;
	}
	cTotal = pWe->we_wordc + pWe->we_offs;
	for (i = pWe->we_offs; i < cTotal; i++) {
		free(pWe->we_wordv[i]);
		pWe->we_wordv[i] = NULL;
	}
	free(pWe->we_wordv);
	pWe->we_wordv = NULL;
	pWe->we_wordc = 0;
}

int
wordexp(const char *szWords, wordexp_t *pWe, int nFlags)
{
	size_t cWords = 0;
	size_t cCap;
	size_t iOff;
	size_t i;
	size_t idx = 0;
	char **ppV;
	char *szExp;
	char aTok[1024];
	int nErr;
	int nField;

	if (pWe == NULL) {
		return WRDE_NOSPACE;
	}
	if (szWords == NULL) {
		if ((nFlags & WRDE_APPEND) == 0) {
			pWe->we_wordc = 0;
			pWe->we_wordv = NULL;
		}
		return 0;
	}
	if ((nFlags & WRDE_DOOFFS) == 0 && (nFlags & WRDE_APPEND) == 0) {
		pWe->we_offs = 0;
	}
	/* REUSE: free previous words unless APPEND (APPEND keeps them). */
	if ((nFlags & WRDE_REUSE) != 0 && (nFlags & WRDE_APPEND) == 0) {
		we_free_words(pWe);
	}
	/* Refuse command substitution when NOCMD */
	if ((nFlags & WRDE_NOCMD) != 0) {
		const char *p;
		int quote = 0;

		for (p = szWords; *p != '\0'; p++) {
			if (quote == 1) {
				if (*p == '\'') {
					quote = 0;
				}
				continue;
			}
			if (quote == 2) {
				if (*p == '"') {
					quote = 0;
				} else if (*p == '\\' && p[1] != '\0') {
					p++;
				}
				continue;
			}
			if (*p == '\'') {
				quote = 1;
				continue;
			}
			if (*p == '"') {
				quote = 2;
				continue;
			}
			if (*p == '`' || (*p == '$' && p[1] == '(')) {
				return WRDE_CMDSUB;
			}
		}
	}

	if ((nFlags & WRDE_APPEND) != 0 && pWe->we_wordv != NULL) {
		cWords = pWe->we_wordc + pWe->we_offs;
		cCap = cWords + 8;
		ppV = pWe->we_wordv;
		/* grow if needed below */
	} else {
		cCap = pWe->we_offs + 8;
		ppV = (char **)calloc(cCap + 1, sizeof(char *));
		if (ppV == NULL) {
			return WRDE_NOSPACE;
		}
		for (iOff = 0; iOff < pWe->we_offs; iOff++) {
			ppV[iOff] = NULL;
		}
		cWords = pWe->we_offs;
	}

	for (;;) {
		nField = next_field(szWords, &idx, aTok, sizeof(aTok));
		if (nField == 1) {
			break;
		}
		if (nField < 0) {
			if ((nFlags & WRDE_APPEND) == 0) {
				for (i = pWe->we_offs; i < cWords; i++) {
					free(ppV[i]);
				}
				free(ppV);
			}
			return nField;
		}
		if (cWords + 1 >= cCap) {
			size_t cNew = cCap * 2 + 8;
			char **ppN =
			    (char **)realloc(ppV, (cNew + 1) * sizeof(char *));

			if (ppN == NULL) {
				if ((nFlags & WRDE_APPEND) == 0) {
					for (i = pWe->we_offs; i < cWords;
					     i++) {
						free(ppV[i]);
					}
					free(ppV);
				}
				return WRDE_NOSPACE;
			}
			ppV = ppN;
			cCap = cNew;
		}
		szExp = NULL;
		nErr = expand_token(aTok, &szExp, nFlags);
		if (nErr != 0) {
			if ((nFlags & WRDE_APPEND) == 0) {
				for (i = pWe->we_offs; i < cWords; i++) {
					free(ppV[i]);
				}
				free(ppV);
			}
			return nErr;
		}
		ppV[cWords++] = szExp;
	}
	ppV[cWords] = NULL;
	pWe->we_wordc = cWords - pWe->we_offs;
	pWe->we_wordv = ppV;
	return 0;
}

void
wordfree(wordexp_t *pWe)
{
	we_free_words(pWe);
}
