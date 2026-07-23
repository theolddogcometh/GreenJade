/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * utmp / utmpx — freestanding in-memory session table fill-in.
 */
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <utmp.h>
#include <utmpx.h>

#define UTMP_MAX 32

static char g_szUtmpPath[256] = "/var/run/utmp";
static char g_szUtmpxPath[256] = "/var/run/utmp";
static struct utmp g_aTab[UTMP_MAX];
static int g_cUsed;
static int g_iCursor;
static struct utmp g_stLast;

void
setutent(void)
{
	g_iCursor = 0;
}

void
endutent(void)
{
	g_iCursor = 0;
}

struct utmp *
getutent(void)
{
	errno = 0;
	while (g_iCursor < g_cUsed) {
		return &g_aTab[g_iCursor++];
	}
	return NULL;
}

struct utmp *
getutid(const struct utmp *pId)
{
	int i;

	if (pId == NULL) {
		return NULL;
	}
	for (i = 0; i < g_cUsed; i++) {
		if (g_aTab[i].ut_type == pId->ut_type) {
			if (pId->ut_type == RUN_LVL ||
			    pId->ut_type == BOOT_TIME ||
			    pId->ut_type == NEW_TIME) {
				return &g_aTab[i];
			}
			if (strncmp(g_aTab[i].ut_id, pId->ut_id,
				    sizeof(g_aTab[i].ut_id)) == 0) {
				return &g_aTab[i];
			}
		}
	}
	return NULL;
}

struct utmp *
getutline(const struct utmp *pLine)
{
	int i;

	if (pLine == NULL) {
		return NULL;
	}
	for (i = 0; i < g_cUsed; i++) {
		if ((g_aTab[i].ut_type == LOGIN_PROCESS ||
		     g_aTab[i].ut_type == USER_PROCESS) &&
		    strncmp(g_aTab[i].ut_line, pLine->ut_line,
			    sizeof(g_aTab[i].ut_line)) == 0) {
			return &g_aTab[i];
		}
	}
	return NULL;
}

struct utmp *
pututline(const struct utmp *pUt)
{
	int i;

	if (pUt == NULL) {
		errno = EINVAL;
		return NULL;
	}
	/* Replace matching id/line or append. */
	for (i = 0; i < g_cUsed; i++) {
		if (g_aTab[i].ut_type == pUt->ut_type &&
		    strncmp(g_aTab[i].ut_id, pUt->ut_id,
			    sizeof(g_aTab[i].ut_id)) == 0) {
			g_aTab[i] = *pUt;
			g_stLast = *pUt;
			return &g_aTab[i];
		}
	}
	if (g_cUsed >= UTMP_MAX) {
		errno = ENOMEM;
		return NULL;
	}
	g_aTab[g_cUsed] = *pUt;
	g_stLast = *pUt;
	return &g_aTab[g_cUsed++];
}

int
utmpname(const char *szFile)
{
	size_t n;

	if (szFile == NULL) {
		errno = EINVAL;
		return -1;
	}
	n = strlen(szFile);
	if (n + 1 > sizeof(g_szUtmpPath)) {
		errno = ENAMETOOLONG;
		return -1;
	}
	memcpy(g_szUtmpPath, szFile, n + 1);
	return 0;
}

void
updwtmp(const char *szFile, const struct utmp *pUt)
{
	(void)szFile;
	if (pUt != NULL) {
		(void)pututline(pUt);
	}
}

void
login(const struct utmp *pUt)
{
	if (pUt != NULL) {
		(void)pututline(pUt);
	}
}

int
logout(const char *szLine)
{
	int i;

	if (szLine == NULL) {
		return 0;
	}
	for (i = 0; i < g_cUsed; i++) {
		if (strncmp(g_aTab[i].ut_line, szLine,
			    sizeof(g_aTab[i].ut_line)) == 0) {
			g_aTab[i].ut_type = DEAD_PROCESS;
			g_aTab[i].ut_user[0] = '\0';
			return 1;
		}
	}
	return 0;
}

void
logwtmp(const char *szLine, const char *szName, const char *szHost)
{
	struct utmp u;

	memset(&u, 0, sizeof(u));
	u.ut_type = USER_PROCESS;
	if (szLine != NULL) {
		size_t n = strlen(szLine);

		if (n >= sizeof(u.ut_line)) {
			n = sizeof(u.ut_line) - 1;
		}
		memcpy(u.ut_line, szLine, n);
	}
	if (szName != NULL) {
		size_t n = strlen(szName);

		if (n >= sizeof(u.ut_user)) {
			n = sizeof(u.ut_user) - 1;
		}
		memcpy(u.ut_user, szName, n);
	}
	if (szHost != NULL) {
		size_t n = strlen(szHost);

		if (n >= sizeof(u.ut_host)) {
			n = sizeof(u.ut_host) - 1;
		}
		memcpy(u.ut_host, szHost, n);
	}
	(void)pututline(&u);
}

/* ---- utmpx thin wrappers over utmp table -------------------------------- */

void
setutxent(void)
{
	setutent();
}

void
endutxent(void)
{
	endutent();
}

struct utmpx *
getutxent(void)
{
	return (struct utmpx *)getutent();
}

struct utmpx *
getutxid(const struct utmpx *pId)
{
	return (struct utmpx *)getutid((const struct utmp *)pId);
}

struct utmpx *
getutxline(const struct utmpx *pLine)
{
	return (struct utmpx *)getutline((const struct utmp *)pLine);
}

struct utmpx *
pututxline(const struct utmpx *pUtx)
{
	return (struct utmpx *)pututline((const struct utmp *)pUtx);
}

int
utmpxname(const char *szFile)
{
	size_t n;

	if (szFile == NULL) {
		errno = EINVAL;
		return -1;
	}
	n = strlen(szFile);
	if (n + 1 > sizeof(g_szUtmpxPath)) {
		errno = ENAMETOOLONG;
		return -1;
	}
	memcpy(g_szUtmpxPath, szFile, n + 1);
	return utmpname(szFile);
}

void
updwtmpx(const char *szFile, const struct utmpx *pUtx)
{
	updwtmp(szFile, (const struct utmp *)pUtx);
}
