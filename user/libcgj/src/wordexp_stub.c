/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * wordexp/wordfree — freestanding fill-in:
 *   field split, ~ → $HOME, $VAR / ${VAR}, single/double quotes.
 *   Shell metachar glob/pipe/redir still WRDE_BADCHAR (no full shell).
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>

static int
is_shell_meta(char c)
{
	return c == '*' || c == '?' || c == '[' || c == '{' || c == '`' ||
	       c == '|' || c == '&' || c == ';' || c == '<' || c == '>' ||
	       c == '(' || c == ')';
}

static int
expand_token(const char *szIn, char **ppOut)
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
		char c = szIn[i];

		if (quote == 1) {
			if (c == '\'') {
				quote = 0;
				i++;
				continue;
			}
			aBuf[o++] = c;
			i++;
			continue;
		}
		if (quote == 2) {
			if (c == '"') {
				quote = 0;
				i++;
				continue;
			}
			if (c == '\\' && szIn[i + 1] != '\0') {
				aBuf[o++] = szIn[i + 1];
				i += 2;
				continue;
			}
			if (c == '$') {
				/* fall through to $ expand */
			} else {
				aBuf[o++] = c;
				i++;
				continue;
			}
		}
		if (c == '\'') {
			quote = 1;
			i++;
			continue;
		}
		if (c == '"') {
			quote = 2;
			i++;
			continue;
		}
		if (c == '\\' && szIn[i + 1] != '\0') {
			aBuf[o++] = szIn[i + 1];
			i += 2;
			continue;
		}
		if (c == '$') {
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
			val = (nl > 0) ? getenv(name) : NULL;
			if (val != NULL) {
				while (*val != '\0' && o + 1 < sizeof(aBuf)) {
					aBuf[o++] = *val++;
				}
			}
			continue;
		}
		if (is_shell_meta(c)) {
			return WRDE_BADCHAR;
		}
		aBuf[o++] = c;
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

int
wordexp(const char *szWords, wordexp_t *pWe, int nFlags)
{
	char *szCopy;
	char *pTok;
	char *pSave;
	size_t cWords = 0;
	size_t cCap;
	size_t iOff;
	size_t i;
	char **ppV;
	char *szExp;
	int nErr;

	if (pWe == NULL) {
		return WRDE_NOSPACE;
	}
	if (szWords == NULL) {
		pWe->we_wordc = 0;
		pWe->we_wordv = NULL;
		return 0;
	}
	if ((nFlags & WRDE_DOOFFS) == 0) {
		pWe->we_offs = 0;
	}
	/* Refuse command substitution when NOCMD (default expand still rejects `) */
	if ((nFlags & WRDE_NOCMD) != 0) {
		const char *p;

		for (p = szWords; *p != '\0'; p++) {
			if (*p == '`' || (*p == '$' && p[1] == '(')) {
				return WRDE_CMDSUB;
			}
		}
	}
	szCopy = strdup(szWords);
	if (szCopy == NULL) {
		return WRDE_NOSPACE;
	}
	cCap = pWe->we_offs + 8;
	ppV = (char **)calloc(cCap + 1, sizeof(char *));
	if (ppV == NULL) {
		free(szCopy);
		return WRDE_NOSPACE;
	}
	for (iOff = 0; iOff < pWe->we_offs; iOff++) {
		ppV[iOff] = NULL;
	}
	cWords = pWe->we_offs;
	pSave = NULL;
	pTok = strtok_r(szCopy, " \t\n", &pSave);
	while (pTok != NULL) {
		if (cWords + 1 >= cCap) {
			size_t cNew = cCap * 2 + 8;
			char **ppN =
			    (char **)realloc(ppV, (cNew + 1) * sizeof(char *));

			if (ppN == NULL) {
				for (i = pWe->we_offs; i < cWords; i++) {
					free(ppV[i]);
				}
				free(ppV);
				free(szCopy);
				return WRDE_NOSPACE;
			}
			ppV = ppN;
			cCap = cNew;
		}
		szExp = NULL;
		nErr = expand_token(pTok, &szExp);
		if (nErr != 0) {
			for (i = pWe->we_offs; i < cWords; i++) {
				free(ppV[i]);
			}
			free(ppV);
			free(szCopy);
			return nErr;
		}
		ppV[cWords++] = szExp;
		pTok = strtok_r(NULL, " \t\n", &pSave);
	}
	free(szCopy);
	ppV[cWords] = NULL;
	pWe->we_wordc = cWords - pWe->we_offs;
	pWe->we_wordv = ppV;
	return 0;
}

void
wordfree(wordexp_t *pWe)
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
