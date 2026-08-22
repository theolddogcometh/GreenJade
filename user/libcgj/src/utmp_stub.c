/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * utmp / utmpx — in-memory table plus file-backed wtmp/utmp.
 * OpenSSH USE_LOGIN (HAVE_LOGIN) records via login/logout/logwtmp.
 * Dual DoD B OPEN.
 */
#include <errno.h>
#include <fcntl.h>
#include <lastlog.h>
#include <paths.h>
#include <pwd.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
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

static void
ut_write_all(int nFd, const void *pBuf, size_t cb)
{
	const unsigned char *p = (const unsigned char *)pBuf;
	size_t cbLeft = cb;

	while (cbLeft > 0) {
		ssize_t nW;

		nW = write(nFd, p, cbLeft);
		if (nW <= 0) {
			return;
		}
		p += (size_t)nW;
		cbLeft -= (size_t)nW;
	}
}

/* Rewrite utmp image (slot table). Best-effort; missing dir is OK. */
static void
ut_flush_utmp(void)
{
	int nFd;
	int nFlags;

	if (g_szUtmpPath[0] == '\0') {
		return;
	}
	nFlags = O_WRONLY | O_CREAT | O_TRUNC;
#ifdef O_CLOEXEC
	nFlags |= O_CLOEXEC;
#endif
	nFd = open(g_szUtmpPath, nFlags, 0644);
	if (nFd < 0) {
		return;
	}
	if (g_cUsed > 0) {
		ut_write_all(nFd, g_aTab, (size_t)g_cUsed * sizeof(g_aTab[0]));
	}
	(void)close(nFd);
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
			ut_flush_utmp();
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
			ut_flush_utmp();
			return &g_aTab[i];
		}
	}
	if (g_cUsed >= UTMP_MAX) {
		errno = ENOMEM;
		return NULL;
	}
	g_aTab[g_cUsed] = st;
	g_stLast = st;
	g_cUsed++;
	ut_flush_utmp();
	return &g_aTab[g_cUsed - 1];
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

/* Lastlog(5) slot file: uid * sizeof(struct lastlog). Soft create. */
static void
ut_lastlog_put(const struct utmp *pUt)
{
	struct lastlog ll;
	struct passwd *pPw;
	int nFd;
	int nFlags;
	off_t off;
	const char *szPath;

	if (pUt == NULL || pUt->ut_user[0] == '\0') {
		return;
	}
	pPw = getpwnam(pUt->ut_user);
	if (pPw == NULL) {
		return;
	}
#ifdef _PATH_LASTLOG
	szPath = _PATH_LASTLOG;
#else
	szPath = "/var/log/lastlog";
#endif
	memset(&ll, 0, sizeof(ll));
	ll.ll_time = pUt->ut_tv.tv_sec;
	copy_field(ll.ll_line, sizeof(ll.ll_line), pUt->ut_line);
	copy_field(ll.ll_host, sizeof(ll.ll_host), pUt->ut_host);
	nFlags = O_WRONLY | O_CREAT;
#ifdef O_CLOEXEC
	nFlags |= O_CLOEXEC;
#endif
	nFd = open(szPath, nFlags, 0644);
	if (nFd < 0) {
		return;
	}
	off = (off_t)pPw->pw_uid * (off_t)sizeof(ll);
	if (lseek(nFd, off, SEEK_SET) != off) {
		(void)close(nFd);
		return;
	}
	ut_write_all(nFd, &ll, sizeof(ll));
	(void)close(nFd);
}

void
updwtmp(const char *szFile, const struct utmp *pUt)
{
	int nFd;
	int nFlags;
	const char *szPath;

	if (pUt == NULL) {
		return;
	}
	szPath = szFile;
	if (szPath == NULL || szPath[0] == '\0') {
#ifdef _PATH_WTMP
		szPath = _PATH_WTMP;
#else
		szPath = "/var/log/wtmp";
#endif
	}
	nFlags = O_WRONLY | O_CREAT | O_APPEND;
#ifdef O_CLOEXEC
	nFlags |= O_CLOEXEC;
#endif
	nFd = open(szPath, nFlags, 0644);
	if (nFd < 0) {
		return;
	}
	ut_write_all(nFd, pUt, sizeof(*pUt));
	(void)close(nFd);
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
#ifdef _PATH_WTMP
	updwtmp(_PATH_WTMP, &st);
#else
	updwtmp("/var/log/wtmp", &st);
#endif
	ut_lastlog_put(&st);
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
			ut_flush_utmp();
#ifdef _PATH_WTMP
			updwtmp(_PATH_WTMP, &g_aTab[i]);
#else
			updwtmp("/var/log/wtmp", &g_aTab[i]);
#endif
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
	if (szName != NULL && szName[0] != '\0') {
		u.ut_type = USER_PROCESS;
	} else {
		u.ut_type = DEAD_PROCESS;
	}
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
#ifdef _PATH_WTMP
	updwtmp(_PATH_WTMP, &u);
#else
	updwtmp("/var/log/wtmp", &u);
#endif
}

/* ---- utmpx: convert; layouts differ (timeval vs two ints) -------------- */

static struct utmpx g_stUtx;
static struct lastlogx g_stLastx;

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
	struct utmp *pUt;

	pUt = getutent();
	if (pUt == NULL) {
		return NULL;
	}
	getutmpx(pUt, &g_stUtx);
	return &g_stUtx;
}

struct utmpx *
getutxid(const struct utmpx *pId)
{
	struct utmp stId;
	struct utmp *pUt;

	if (pId == NULL) {
		return NULL;
	}
	getutmp(pId, &stId);
	pUt = getutid(&stId);
	if (pUt == NULL) {
		return NULL;
	}
	getutmpx(pUt, &g_stUtx);
	return &g_stUtx;
}

struct utmpx *
getutxline(const struct utmpx *pLine)
{
	struct utmp stLine;
	struct utmp *pUt;

	if (pLine == NULL) {
		return NULL;
	}
	getutmp(pLine, &stLine);
	pUt = getutline(&stLine);
	if (pUt == NULL) {
		return NULL;
	}
	getutmpx(pUt, &g_stUtx);
	return &g_stUtx;
}

struct utmpx *
pututxline(const struct utmpx *pUtx)
{
	struct utmp st;
	struct utmp *pUt;

	if (pUtx == NULL) {
		errno = EINVAL;
		return NULL;
	}
	getutmp(pUtx, &st);
	pUt = pututline(&st);
	if (pUt == NULL) {
		return NULL;
	}
	getutmpx(pUt, &g_stUtx);
	return &g_stUtx;
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

void
logwtmpx(const char *szLine, const char *szName, const char *szHost)
{
	struct utmpx ux;

	memset(&ux, 0, sizeof(ux));
	if (szName != NULL && szName[0] != '\0') {
		ux.ut_type = USER_PROCESS;
	} else {
		ux.ut_type = DEAD_PROCESS;
	}
	copy_field(ux.ut_line, sizeof(ux.ut_line), szLine);
	copy_field(ux.ut_user, sizeof(ux.ut_user), szName);
	copy_field(ux.ut_host, sizeof(ux.ut_host), szHost);
	if (szLine != NULL) {
		size_t n = strlen(szLine);
		size_t k = (n > 4) ? n - 4 : 0;
		size_t j = 0;

		while (szLine[k] != '\0' && j < sizeof(ux.ut_id)) {
			ux.ut_id[j++] = szLine[k++];
		}
	}
	{
		time_t t = time(NULL);

		if (t == (time_t)-1) {
			t = 0;
		}
		ux.ut_tv.tv_sec = t;
		ux.ut_tv.tv_usec = 0;
	}
#ifdef _PATH_WTMP
	updwtmpx(_PATH_WTMP, &ux);
#else
	updwtmpx("/var/log/wtmp", &ux);
#endif
}

static int
ll_read_slot(uid_t uid, struct lastlog *pLl)
{
	int nFd;
	int nFlags;
	off_t off;
	const char *szPath;
	unsigned char *p;
	size_t cbLeft;

#ifdef _PATH_LASTLOG
	szPath = _PATH_LASTLOG;
#else
	szPath = "/var/log/lastlog";
#endif
	nFlags = O_RDONLY;
#ifdef O_CLOEXEC
	nFlags |= O_CLOEXEC;
#endif
	nFd = open(szPath, nFlags);
	if (nFd < 0) {
		return -1;
	}
	off = (off_t)uid * (off_t)sizeof(*pLl);
	if (lseek(nFd, off, SEEK_SET) != off) {
		(void)close(nFd);
		return -1;
	}
	memset(pLl, 0, sizeof(*pLl));
	p = (unsigned char *)pLl;
	cbLeft = sizeof(*pLl);
	while (cbLeft > 0) {
		ssize_t nR = read(nFd, p, cbLeft);

		if (nR < 0) {
			(void)close(nFd);
			return -1;
		}
		if (nR == 0) {
			break;
		}
		p += (size_t)nR;
		cbLeft -= (size_t)nR;
	}
	(void)close(nFd);
	if (cbLeft == sizeof(*pLl)) {
		return -1;
	}
	return 0;
}

struct lastlogx *
getlastlogx(uid_t uid, struct lastlogx *pLl)
{
	struct lastlog ll;
	struct lastlogx *pOut;

	pOut = (pLl != NULL) ? pLl : &g_stLastx;
	if (ll_read_slot(uid, &ll) != 0) {
		return NULL;
	}
	if (ll.ll_time == 0 && ll.ll_line[0] == '\0') {
		return NULL;
	}
	memset(pOut, 0, sizeof(*pOut));
	pOut->ll_tv.tv_sec = ll.ll_time;
	pOut->ll_tv.tv_usec = 0;
	memcpy(pOut->ll_line, ll.ll_line, sizeof(pOut->ll_line));
	memcpy(pOut->ll_host, ll.ll_host, sizeof(pOut->ll_host));
	return pOut;
}

struct lastlogx *
getlastlogxbyname(const char *szName, struct lastlogx *pLl)
{
	struct passwd *pPw;

	if (szName == NULL || szName[0] == '\0') {
		errno = EINVAL;
		return NULL;
	}
	pPw = getpwnam(szName);
	if (pPw == NULL) {
		return NULL;
	}
	return getlastlogx(pPw->pw_uid, pLl);
}
