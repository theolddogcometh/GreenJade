/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * shadow password — freestanding in-memory table + line parse.
 * Soft deepen: seed root+nobody; optional /etc/shadow load; putspent
 * upserts table; sgetspent field validation.
 */
#include <errno.h>
#include <shadow.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SP_MAX  32
#define SP_LINE 256

struct sp_ent {
	int used;
	struct spwd sp;
	char namp[64];
	char pwdp[128];
};

static struct sp_ent g_aTab[SP_MAX];
static int g_fInit;
static int g_iCursor;
static int g_fLocked;

static void
sp_seed(const char *szName, const char *szPw)
{
	int i;

	for (i = 0; i < SP_MAX; i++) {
		if (!g_aTab[i].used) {
			size_t n0;
			size_t n1;

			memset(&g_aTab[i], 0, sizeof(g_aTab[i]));
			g_aTab[i].used = 1;
			n0 = strlen(szName);
			n1 = strlen(szPw);
			if (n0 >= sizeof(g_aTab[i].namp)) {
				n0 = sizeof(g_aTab[i].namp) - 1;
			}
			if (n1 >= sizeof(g_aTab[i].pwdp)) {
				n1 = sizeof(g_aTab[i].pwdp) - 1;
			}
			memcpy(g_aTab[i].namp, szName, n0);
			g_aTab[i].namp[n0] = '\0';
			memcpy(g_aTab[i].pwdp, szPw, n1);
			g_aTab[i].pwdp[n1] = '\0';
			g_aTab[i].sp.sp_namp = g_aTab[i].namp;
			g_aTab[i].sp.sp_pwdp = g_aTab[i].pwdp;
			g_aTab[i].sp.sp_lstchg = -1;
			g_aTab[i].sp.sp_min = -1;
			g_aTab[i].sp.sp_max = -1;
			g_aTab[i].sp.sp_warn = -1;
			g_aTab[i].sp.sp_inact = -1;
			g_aTab[i].sp.sp_expire = -1;
			g_aTab[i].sp.sp_flag = 0;
			return;
		}
	}
}

static int
sp_upsert(const struct spwd *pSp)
{
	int i;
	int nFree = -1;
	size_t n0;
	size_t n1;

	if (pSp == NULL || pSp->sp_namp == NULL || pSp->sp_pwdp == NULL) {
		return -1;
	}
	for (i = 0; i < SP_MAX; i++) {
		if (g_aTab[i].used &&
		    strcmp(g_aTab[i].namp, pSp->sp_namp) == 0) {
			nFree = i;
			break;
		}
		if (!g_aTab[i].used && nFree < 0) {
			nFree = i;
		}
	}
	if (nFree < 0) {
		return -1;
	}
	n0 = strlen(pSp->sp_namp);
	n1 = strlen(pSp->sp_pwdp);
	if (n0 >= sizeof(g_aTab[nFree].namp)) {
		n0 = sizeof(g_aTab[nFree].namp) - 1;
	}
	if (n1 >= sizeof(g_aTab[nFree].pwdp)) {
		n1 = sizeof(g_aTab[nFree].pwdp) - 1;
	}
	memset(&g_aTab[nFree], 0, sizeof(g_aTab[nFree]));
	g_aTab[nFree].used = 1;
	memcpy(g_aTab[nFree].namp, pSp->sp_namp, n0);
	g_aTab[nFree].namp[n0] = '\0';
	memcpy(g_aTab[nFree].pwdp, pSp->sp_pwdp, n1);
	g_aTab[nFree].pwdp[n1] = '\0';
	g_aTab[nFree].sp = *pSp;
	g_aTab[nFree].sp.sp_namp = g_aTab[nFree].namp;
	g_aTab[nFree].sp.sp_pwdp = g_aTab[nFree].pwdp;
	return 0;
}

static void
sp_load_file(const char *szPath)
{
	FILE *pF;
	char aLine[SP_LINE];

	pF = fopen(szPath, "r");
	if (pF == NULL) {
		return;
	}
	while (fgets(aLine, (int)sizeof(aLine), pF) != NULL) {
		struct spwd *p;

		if (aLine[0] == '#' || aLine[0] == '\n' || aLine[0] == '\0') {
			continue;
		}
		p = sgetspent(aLine);
		if (p != NULL) {
			(void)sp_upsert(p);
		}
	}
	(void)fclose(pF);
}

static void
sp_init(void)
{
	if (g_fInit) {
		return;
	}
	g_fInit = 1;
	memset(g_aTab, 0, sizeof(g_aTab));
	/* Synthetic root — locked password field. */
	sp_seed("root", "*");
	/* Soft nobody for desktop multi-user soft path. */
	sp_seed("nobody", "!");
	/* Optional host /etc/shadow overlay (best-effort). */
	sp_load_file("/etc/shadow");
	g_iCursor = 0;
}

static struct spwd *
sp_find(const char *szName)
{
	int i;

	sp_init();
	if (szName == NULL) {
		return NULL;
	}
	for (i = 0; i < SP_MAX; i++) {
		if (g_aTab[i].used &&
		    strcmp(g_aTab[i].namp, szName) == 0) {
			return &g_aTab[i].sp;
		}
	}
	return NULL;
}

struct spwd *
getspnam(const char *szName)
{
	struct spwd *p;

	errno = 0;
	p = sp_find(szName);
	if (p == NULL) {
		errno = 0; /* not found, not an error on many systems */
	}
	return p;
}

struct spwd *
getspent(void)
{
	sp_init();
	while (g_iCursor < SP_MAX) {
		if (g_aTab[g_iCursor].used) {
			return &g_aTab[g_iCursor++].sp;
		}
		g_iCursor++;
	}
	return NULL;
}

void
setspent(void)
{
	sp_init();
	g_iCursor = 0;
}

void
endspent(void)
{
	g_iCursor = 0;
}

int
putspent(const struct spwd *pSp, FILE *pF)
{
	if (pSp == NULL || pF == NULL || pSp->sp_namp == NULL ||
	    pSp->sp_pwdp == NULL) {
		errno = EINVAL;
		return -1;
	}
	/* Soft: keep in-memory table in sync with written line. */
	(void)sp_upsert(pSp);
	if (fprintf(pF, "%s:%s:%ld:%ld:%ld:%ld:%ld:%ld:%lu\n", pSp->sp_namp,
		    pSp->sp_pwdp, pSp->sp_lstchg, pSp->sp_min, pSp->sp_max,
		    pSp->sp_warn, pSp->sp_inact, pSp->sp_expire,
		    pSp->sp_flag) < 0) {
		return -1;
	}
	return 0;
}

int
lckpwdf(void)
{
	if (g_fLocked) {
		errno = EBUSY;
		return -1;
	}
	g_fLocked = 1;
	return 0;
}

int
ulckpwdf(void)
{
	g_fLocked = 0;
	return 0;
}

/* Parse "name:passwd:lstchg:min:max:warn:inact:expire:flag" */
struct spwd *
sgetspent(const char *szLine)
{
	static struct sp_ent ent;
	char aLine[SP_LINE];
	char *p;
	char *fields[9];
	int n = 0;
	size_t i;

	if (szLine == NULL) {
		errno = EINVAL;
		return NULL;
	}
	for (i = 0; i + 1 < sizeof(aLine) && szLine[i] != '\0' &&
		     szLine[i] != '\n';
	     i++) {
		aLine[i] = szLine[i];
	}
	aLine[i] = '\0';
	/* Soft: skip comments / empty */
	if (aLine[0] == '\0' || aLine[0] == '#') {
		errno = EINVAL;
		return NULL;
	}
	p = aLine;
	fields[n++] = p;
	while (*p != '\0' && n < 9) {
		if (*p == ':') {
			*p = '\0';
			fields[n++] = p + 1;
		}
		p++;
	}
	if (n < 2 || fields[0][0] == '\0') {
		errno = EINVAL;
		return NULL;
	}
	memset(&ent, 0, sizeof(ent));
	ent.used = 1;
	{
		size_t n0 = strlen(fields[0]);
		size_t n1 = strlen(fields[1]);

		if (n0 >= sizeof(ent.namp)) {
			n0 = sizeof(ent.namp) - 1;
		}
		if (n1 >= sizeof(ent.pwdp)) {
			n1 = sizeof(ent.pwdp) - 1;
		}
		memcpy(ent.namp, fields[0], n0);
		ent.namp[n0] = '\0';
		memcpy(ent.pwdp, fields[1], n1);
		ent.pwdp[n1] = '\0';
	}
	ent.sp.sp_namp = ent.namp;
	ent.sp.sp_pwdp = ent.pwdp;
	ent.sp.sp_lstchg = (n > 2 && fields[2][0]) ? strtol(fields[2], NULL, 10)
						   : -1;
	ent.sp.sp_min = (n > 3 && fields[3][0]) ? strtol(fields[3], NULL, 10)
						: -1;
	ent.sp.sp_max = (n > 4 && fields[4][0]) ? strtol(fields[4], NULL, 10)
						: -1;
	ent.sp.sp_warn = (n > 5 && fields[5][0]) ? strtol(fields[5], NULL, 10)
						 : -1;
	ent.sp.sp_inact = (n > 6 && fields[6][0]) ? strtol(fields[6], NULL, 10)
						  : -1;
	ent.sp.sp_expire = (n > 7 && fields[7][0]) ? strtol(fields[7], NULL, 10)
						   : -1;
	ent.sp.sp_flag = (n > 8 && fields[8][0])
			     ? (unsigned long)strtoul(fields[8], NULL, 10)
			     : 0;
	return &ent.sp;
}

struct spwd *
fgetspent(FILE *pF)
{
	char aLine[SP_LINE];

	if (pF == NULL) {
		errno = EINVAL;
		return NULL;
	}
	for (;;) {
		if (fgets(aLine, (int)sizeof(aLine), pF) == NULL) {
			return NULL;
		}
		if (aLine[0] == '#' || aLine[0] == '\n' || aLine[0] == '\0') {
			continue;
		}
		return sgetspent(aLine);
	}
}

int
getspnam_r(const char *szName, struct spwd *pSp, char *szBuf, size_t cb,
	   struct spwd **ppResult)
{
	struct spwd *p;
	size_t n1;
	size_t n2;

	if (ppResult != NULL) {
		*ppResult = NULL;
	}
	if (pSp == NULL || szBuf == NULL || cb < 2) {
		return ERANGE;
	}
	p = sp_find(szName);
	if (p == NULL) {
		return 0;
	}
	n1 = strlen(p->sp_namp) + 1;
	n2 = strlen(p->sp_pwdp) + 1;
	if (n1 + n2 > cb) {
		return ERANGE;
	}
	memcpy(szBuf, p->sp_namp, n1);
	memcpy(szBuf + n1, p->sp_pwdp, n2);
	*pSp = *p;
	pSp->sp_namp = szBuf;
	pSp->sp_pwdp = szBuf + n1;
	if (ppResult != NULL) {
		*ppResult = pSp;
	}
	return 0;
}
