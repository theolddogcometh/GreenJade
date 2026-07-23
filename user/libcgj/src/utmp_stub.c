/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * utmp / utmpx — freestanding in-memory session table fill-in.
 * Soft deepen: timestamps on put/logwtmp; getutmp/getutmpx convert;
 * DEAD_PROCESS clear; BOOT_TIME seed.
 */
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <utmp.h>
#include <utmpx.h>

#define UTMP_MAX 32

static char g_szUtmpPath[256] = "/var/run/utmp";
static char g_szUtmpxPath[256] = "/var/run/utmp";
static struct utmp g_aTab[UTMP_MAX];
static int g_cUsed;
static int g_iCursor;
static struct utmp g_stLast;
static int g_fBoot;

static void
ut_stamp(struct utmp *pUt)
{
	time_t t;

	if (pUt == NULL) {
		return;
	}
	t = time(NULL);
	if (t == (time_t)-1) {
		t = 0;
	}
	pUt->ut_tv.tv_sec = (int)t;
	pUt->ut_tv.tv_usec = 0;
}

static void
ut_ensure_boot(void)
{
	struct utmp u;

	if (g_fBoot) {
		return;
	}
	g_fBoot = 1;
	memset(&u, 0, sizeof(u));
	u.ut_type = BOOT_TIME;
	ut_stamp(&u);
	if (g_cUsed < UTMP_MAX) {
		g_aTab[g_cUsed++] = u;
	}
}

static void
copy_field(char *szDst, size_t cbDst, const char *szSrc)
{
	size_t n;

	if (szDst == NULL || cbDst == 0) {
		return;
	}
	memset(szDst, 0, cbDst);
	if (szSrc == NULL) {
		return;
	}
	n = strlen(szSrc);
	if (n >= cbDst) {
		n = cbDst - 1;
	}
	memcpy(szDst, szSrc, n);
}

void
setutent(void)
{
	ut_ensure_boot();
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
	ut_ensure_boot();
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

	ut_ensure_boot();
	if (pId == NULL) {
		return NULL;
	}
	for (i = 0; i < g_cUsed; i++) {
		if (g_aTab[i].ut_type == pId->ut_type) {
			if (pId->ut_type == RUN_LVL ||
			    pId->ut_type == BOOT_TIME ||
			    pId->ut_type == NEW_TIME ||
			    pId->ut_type == OLD_TIME) {
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

	ut_ensure_boot();
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
	struct utmp st;

	ut_ensure_boot();
	if (pUt == NULL) {
		errno = EINVAL;
		return NULL;
	}
	st = *pUt;
	/* Soft: stamp if caller left time zero. */
	if (st.ut_tv.tv_sec == 0) {
		ut_stamp(&st);
	}
	/* Replace matching id/type or append. */
	for (i = 0; i < g_cUsed; i++) {
		if (g_aTab[i].ut_type == st.ut_type &&
		    strncmp(g_aTab[i].ut_id, st.ut_id,
			    sizeof(g_aTab[i].ut_id)) == 0) {
			g_aTab[i] = st;
			g_stLast = st;
			return &g_aTab[i];
		}
		/* Also replace same line for USER/LOGIN */
		if ((st.ut_type == USER_PROCESS ||
		     st.ut_type == LOGIN_PROCESS) &&
		    (g_aTab[i].ut_type == USER_PROCESS ||
		     g_aTab[i].ut_type == LOGIN_PROCESS) &&
		    strncmp(g_aTab[i].ut_line, st.ut_line,
			    sizeof(g_aTab[i].ut_line)) == 0) {
			g_aTab[i] = st;
			g_stLast = st;
			return &g_aTab[i];
		}
	}
	if (g_cUsed >= UTMP_MAX) {
		errno = ENOMEM;
		return NULL;
	}
	g_aTab[g_cUsed] = st;
	g_stLast = st;
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
	/* Soft: path recorded; table stays in-memory. */
	g_iCursor = 0;
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
	struct utmp st;

	if (pUt == NULL) {
		return;
	}
	st = *pUt;
	if (st.ut_type == EMPTY) {
		st.ut_type = USER_PROCESS;
	}
	if (st.ut_tv.tv_sec == 0) {
		ut_stamp(&st);
	}
	(void)pututline(&st);
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
			ut_stamp(&g_aTab[i]);
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
	copy_field(u.ut_line, sizeof(u.ut_line), szLine);
	copy_field(u.ut_user, sizeof(u.ut_user), szName);
	copy_field(u.ut_host, sizeof(u.ut_host), szHost);
	/* Soft id from line tail */
	if (szLine != NULL) {
		size_t n = strlen(szLine);
		size_t k = (n > 4) ? n - 4 : 0;
		size_t j = 0;

		while (szLine[k] != '\0' && j < sizeof(u.ut_id)) {
			u.ut_id[j++] = szLine[k++];
		}
	}
	ut_stamp(&u);
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

void
getutmp(const struct utmpx *pUtx, struct utmp *pUt)
{
	if (pUtx == NULL || pUt == NULL) {
		return;
	}
	memset(pUt, 0, sizeof(*pUt));
	pUt->ut_type = pUtx->ut_type;
	pUt->ut_pid = pUtx->ut_pid;
	memcpy(pUt->ut_line, pUtx->ut_line, sizeof(pUt->ut_line));
	memcpy(pUt->ut_id, pUtx->ut_id, sizeof(pUt->ut_id));
	memcpy(pUt->ut_user, pUtx->ut_user, sizeof(pUt->ut_user));
	memcpy(pUt->ut_host, pUtx->ut_host, sizeof(pUt->ut_host));
	pUt->ut_exit.e_termination = pUtx->ut_exit.e_termination;
	pUt->ut_exit.e_exit = pUtx->ut_exit.e_exit;
	pUt->ut_session = pUtx->ut_session;
	pUt->ut_tv.tv_sec = (int)pUtx->ut_tv.tv_sec;
	pUt->ut_tv.tv_usec = (int)pUtx->ut_tv.tv_usec;
	memcpy(pUt->ut_addr_v6, pUtx->ut_addr_v6, sizeof(pUt->ut_addr_v6));
}

void
getutmpx(const struct utmp *pUt, struct utmpx *pUtx)
{
	if (pUt == NULL || pUtx == NULL) {
		return;
	}
	memset(pUtx, 0, sizeof(*pUtx));
	pUtx->ut_type = pUt->ut_type;
	pUtx->ut_pid = pUt->ut_pid;
	memcpy(pUtx->ut_line, pUt->ut_line, sizeof(pUtx->ut_line));
	memcpy(pUtx->ut_id, pUt->ut_id, sizeof(pUtx->ut_id));
	memcpy(pUtx->ut_user, pUt->ut_user, sizeof(pUtx->ut_user));
	memcpy(pUtx->ut_host, pUt->ut_host, sizeof(pUtx->ut_host));
	pUtx->ut_exit.e_termination = pUt->ut_exit.e_termination;
	pUtx->ut_exit.e_exit = pUt->ut_exit.e_exit;
	pUtx->ut_session = pUt->ut_session;
	pUtx->ut_tv.tv_sec = pUt->ut_tv.tv_sec;
	pUtx->ut_tv.tv_usec = pUt->ut_tv.tv_usec;
	memcpy(pUtx->ut_addr_v6, pUt->ut_addr_v6, sizeof(pUtx->ut_addr_v6));
}
