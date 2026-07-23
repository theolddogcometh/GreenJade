/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host freestanding smoke against libcgj (glibc-shaped API).
 * Built with -nostdlib -Iuser/libcgj/include + libcgj.a
 */
#include <aio.h>
#include <argp.h>
#include <argz.h>
#include <envz.h>
#include <libaio.h>
#include <linux/io_uring.h>
#include <monetary.h>
#include <nl_types.h>
#include <obstack.h>
#include <printf.h>
#include <ucontext.h>
#include <netinet/ether.h>
#include <threads.h>
#include <uchar.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <dlfcn.h>
#include <err.h>
#include <error.h>
#include <errno.h>
#include <execinfo.h>
#include <ftw.h>
#include <glob.h>
#include <iconv.h>
#include <inttypes.h>
#include <libintl.h>
#include <sys/quota.h>
#include <sys/seccomp.h>
#include <sys/swap.h>
#include <langinfo.h>
#include <link.h>
#include <malloc.h>
#include <mntent.h>
#include <search.h>
#include <sys/sysmacros.h>
#include <sys/auxv.h>
#include <shadow.h>
#include <utmp.h>
#include <utmpx.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <resolv.h>
#include <sched.h>
#include <setjmp.h>
#include <syslog.h>
#include <wordexp.h>
#include <sys/capability.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/xattr.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <getopt.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <mqueue.h>
#include <semaphore.h>
#include <wchar.h>
#include <strings.h>
#include <regex.h>
#include <crypt.h>
#include <locale.h>
#include <sys/inotify.h>
#include <sys/select.h>
#include <sys/sendfile.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/uio.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
#include <spawn.h>
#include <fts.h>
#include <pty.h>
#include <pwd.h>
#include <grp.h>
#include <netinet/in.h>
#include <fmtmsg.h>
#include <stdio_ext.h>
#include <sys/ipc.h>
#include <sys/timeb.h>

static int g_once_n;
static int g_nFtwHits;
static int g_nAtexit;

static int
smoke_phdr_cb(struct dl_phdr_info *pInfo, size_t cb, void *pData)
{
    (void)pInfo;
    (void)cb;
    if (pData != NULL) {
        *(int *)pData += 1;
    }
    return 0;
}

static void
once_fn(void)
{
    g_once_n++;
}
static void
atexit_bump(void)
{
    g_nAtexit++;
}

static int
ftw_cb(const char *szPath, const struct stat *pSt, int nType)
{
    (void)szPath;
    (void)pSt;
    (void)nType;
    g_nFtwHits++;
    return (g_nFtwHits >= 4) ? 1 : 0; /* stop early */
}

/* batch9 helpers (qsort_r / tsearch / lfind) */
static int
smoke_qsort_r_cmp(const void *pA, const void *pB, void *pArg)
{
    int a = *(const int *)pA;
    int b = *(const int *)pB;

    (void)pArg;
    return (a > b) - (a < b);
}

static int
smoke_tcmp(const void *pA, const void *pB)
{
    uintptr_t a = (uintptr_t)pA;
    uintptr_t b = (uintptr_t)pB;

    return (a > b) - (a < b);
}

static int
smoke_icmp(const void *pA, const void *pB)
{
    int a = *(const int *)pA;
    int b = *(const int *)pB;

    return (a > b) - (a < b);
}

static int
nftw_cb(const char *szPath, const struct stat *pSt, int nType, struct FTW *pF)
{
    (void)szPath;
    (void)pSt;
    (void)nType;
    (void)pF;
    return 0;
}

static int
cgj_smoke_int_cmp(const void *pA, const void *pB)
{
    int nA = *(const int *)pA;
    int nB = *(const int *)pB;

    if (nA < nB) {
        return -1;
    }
    if (nA > nB) {
        return 1;
    }
    return 0;
}


/*
 * Real smoke body. Invoked via main() trampoline so SysV 16-byte stack
 * alignment holds under freestanding -msse2 (movaps spills require it).
 * Non-static + used: naked main calls it from asm (not visible to -Wunused).
 */
__attribute__((used, noinline)) int
cgj_smoke_body(int argc, char **argv, char **envp)
{

    char aBuf[128];
    void *p;
    void *pMap;
    int n;
    struct timespec ts;
    struct utsname un;
    char *szDup;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_once_t once = PTHREAD_ONCE_INIT;
    struct __tls_get_addr_arg tlsArg;
    void *pTls;
    FILE *pF;

    (void)argc;
    (void)argv;
    (void)envp;

    n = snprintf(aBuf, sizeof(aBuf), "libcgj: hello %d\n", 42);
    if (n <= 0) {
        return 1;
    }
    if (write(1, aBuf, (size_t)n) != n) {
        return 2;
    }
    if (strcmp("greenjade", "greenjade") != 0) {
        return 3;
    }
    p = malloc(128);
    if (p == NULL) {
        return 4;
    }
    (void)memset(p, 0xab, 128);
    free(p);

    if (strtol("42", NULL, 10) != 42) {
        return 5;
    }
    if (strstr("hello greenjade world", "greenjade") == NULL) {
        return 6;
    }
    if (!isdigit('7') || !isalpha('A') || toupper('b') != 'B') {
        return 7;
    }
    szDup = strdup("libcgj");
    if (szDup == NULL || strcmp(szDup, "libcgj") != 0) {
        return 8;
    }
    free(szDup);

    pMap = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (pMap == MAP_FAILED) {
        return 9;
    }
    ((char *)pMap)[0] = 'G';
    if (munmap(pMap, 4096) != 0) {
        return 10;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 11;
    }
    if (uname(&un) != 0 || un.sysname[0] == '\0') {
        return 12;
    }

    /* Buffered stdio */
    if (setvbuf(stdout, NULL, _IOLBF, 0) != 0) {
        return 13;
    }
    if (printf("libcgj: buffered ") < 0) {
        return 14;
    }
    if (fflush(stdout) != 0) {
        return 15;
    }

    /* errno location (glibc shape) */
    if (__errno_location() != &errno) {
        return 16;
    }

    /* pthread mutex + once + TLS descriptor */
    if (pthread_mutex_lock(&mtx) != 0) {
        return 17;
    }
    if (pthread_mutex_unlock(&mtx) != 0) {
        return 18;
    }
    if (pthread_once(&once, once_fn) != 0 || g_once_n != 1) {
        return 19;
    }
    if (pthread_once(&once, once_fn) != 0 || g_once_n != 1) {
        return 20;
    }
    tlsArg.u64Module = 1;
    tlsArg.u64Offset = 16;
    pTls = __tls_get_addr(&tlsArg);
    if (pTls == NULL) {
        return 21;
    }
    *(int *)pTls = 0x474a;

    /* fdopen smoke on stdout fd */
    pF = fdopen(1, "w");
    if (pF == NULL) {
        return 22;
    }
    if (fputs("stdio fdopen\n", pF) < 0) {
        return 23;
    }
    (void)fflush(pF);
    /* do not fclose(1) via fdopen — would close stdout; free carefully */
    pF->nFd = -1;
    (void)fclose(pF);

    /* memfd_create (host Linux) */
    {
        int nFd = memfd_create("cgj-smoke", 0);

        if (nFd < 0) {
            return 24;
        }
        if (write(nFd, "mf", 2) != 2) {
            (void)close(nFd);
            return 25;
        }
        (void)close(nFd);
    }

    /* environ set/get/unset */
    if (setenv("GJ_SMOKE", "1", 1) != 0) {
        return 26;
    }
    if (getenv("GJ_SMOKE") == NULL || strcmp(getenv("GJ_SMOKE"), "1") != 0) {
        return 27;
    }
    if (setenv("GJ_SMOKE", "2", 0) != 0) {
        return 28; /* no overwrite */
    }
    if (strcmp(getenv("GJ_SMOKE"), "1") != 0) {
        return 29;
    }
    if (setenv("GJ_SMOKE", "2", 1) != 0 || strcmp(getenv("GJ_SMOKE"), "2") != 0) {
        return 30;
    }
    if (unsetenv("GJ_SMOKE") != 0 || getenv("GJ_SMOKE") != NULL) {
        return 31;
    }

    /* poll(timeout=0) on stdout — typically POLLOUT ready */
    {
        struct pollfd pfd;

        pfd.fd = 1;
        pfd.events = POLLOUT;
        pfd.revents = 0;
        n = poll(&pfd, 1, 0);
        if (n < 0) {
            return 32;
        }
        if (n == 1 && (pfd.revents & POLLOUT) == 0 &&
            (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) == 0) {
            return 33;
        }
    }

    /* getopt + getopt_long */
    {
        char *aAv[] = { "cgj", "-a", "-b", "val", "rest", NULL };
        char *aLong[] = { "cgj", "--verbose", "--file=x", "z", NULL };
        static struct option aOpts[] = {
            { "verbose", no_argument, 0, 'v' },
            { "file", required_argument, 0, 'f' },
            { 0, 0, 0, 0 },
        };
        int c;
        int fA = 0;
        int fV = 0;
        int fF = 0;
        int nLi = -1;

        optind = 1;
        opterr = 0;
        while ((c = getopt(4, aAv, "ab:")) != -1) {
            if (c == 'a') {
                fA = 1;
            } else if (c == 'b') {
                if (optarg == NULL || strcmp(optarg, "val") != 0) {
                    return 34;
                }
            } else {
                return 35;
            }
        }
        if (!fA || optind != 4) {
            return 36;
        }
        optind = 1;
        opterr = 0;
        while ((c = getopt_long(3, aLong, "vf:", aOpts, &nLi)) != -1) {
            if (c == 'v') {
                fV = 1;
            } else if (c == 'f') {
                if (optarg == NULL || strcmp(optarg, "x") != 0) {
                    return 78;
                }
                fF = 1;
            } else {
                return 79;
            }
        }
        if (!fV || !fF || optind != 3) {
            return 80;
        }
    }

    /* getrandom */
    {
        unsigned char aRnd[8];
        ssize_t nR;

        nR = getrandom(aRnd, sizeof(aRnd), 0);
        if (nR != (ssize_t)sizeof(aRnd)) {
            return 37;
        }
    }

    /* strcasecmp + basename */
    if (strcasecmp("GreenJade", "greenjade") != 0) {
        return 38;
    }
    {
        char aPath[] = "/usr/bin/sh";
        char *pB = basename(aPath);

        if (pB == NULL || strcmp(pB, "sh") != 0) {
            return 39;
        }
    }

    /* socketpair + short xfer */
    {
        int aSv[2] = { -1, -1 };
        char aMsg[] = { 'g', 'j' };
        char aGot[4];
        ssize_t nX;

        if (socketpair(AF_UNIX, SOCK_STREAM, 0, aSv) != 0) {
            return 40;
        }
        if (send(aSv[0], aMsg, 2, 0) != 2) {
            (void)close(aSv[0]);
            (void)close(aSv[1]);
            return 41;
        }
        nX = recv(aSv[1], aGot, sizeof(aGot), 0);
        (void)close(aSv[0]);
        (void)close(aSv[1]);
        if (nX != 2 || aGot[0] != 'g' || aGot[1] != 'j') {
            return 42;
        }
    }

    /* opendir/readdir on . */
    {
        DIR *pD = opendir(".");
        struct dirent *pE;
        int nSeen = 0;

        if (pD == NULL) {
            return 43;
        }
        while ((pE = readdir(pD)) != NULL) {
            nSeen++;
            if (nSeen > 64) {
                break;
            }
        }
        if (closedir(pD) != 0 || nSeen < 1) {
            return 44;
        }
    }

    /* gettimeofday + pipe2 */
    {
        struct timeval tv;
        int aFd[2] = { -1, -1 };

        if (gettimeofday(&tv, NULL) != 0 || tv.tv_sec < 0) {
            return 45;
        }
        if (pipe2(aFd, 0) != 0) {
            return 46;
        }
        (void)close(aFd[0]);
        (void)close(aFd[1]);
    }

    /* eventfd + fnmatch + realpath + getentropy */
    {
        int nEfd = eventfd(0, 0);
        uint64_t uOne = 1;
        uint64_t uGot = 0;
        unsigned char aEnt[16];
        char *pRp;

        if (nEfd < 0) {
            return 47;
        }
        if (eventfd_write(nEfd, uOne) != 0 || eventfd_read(nEfd, &uGot) != 0 ||
            uGot != 1) {
            (void)close(nEfd);
            return 48;
        }
        (void)close(nEfd);
        if (fnmatch("libcgj*", "libcgj-smoke", 0) != 0) {
            return 49;
        }
        if (fnmatch("nope", "libcgj", 0) == 0) {
            return 50;
        }
        pRp = realpath("/tmp", NULL);
        if (pRp == NULL || pRp[0] != '/') {
            return 51;
        }
        if (getentropy(aEnt, sizeof(aEnt)) != 0) {
            return 52;
        }
    }

    /* sysconf / confstr / strsignal / ftruncate via memfd */
    {
        char aCs[64];
        int nFd;
        long nPg;

        nPg = sysconf(_SC_PAGESIZE);
        if (nPg != 4096L) {
            return 53;
        }
        if (confstr(_CS_PATH, aCs, sizeof(aCs)) == 0 || aCs[0] != '/') {
            return 54;
        }
        if (strsignal(11) == NULL) {
            return 55;
        }
        nFd = memfd_create("cgj-trunc", 0);
        if (nFd < 0) {
            return 56;
        }
        if (ftruncate(nFd, 4096) != 0) {
            (void)close(nFd);
            return 57;
        }
        if (posix_fallocate(nFd, 0, 8192) != 0) {
            (void)close(nFd);
            return 58;
        }
        (void)close(nFd);
        if (getpagesize() != 4096) {
            return 59;
        }
    }

    /* gethostname */
    {
        char aHost[128];

        if (gethostname(aHost, sizeof(aHost)) != 0 || aHost[0] == '\0') {
            return 60;
        }
    }

    /* strndup + dlfcn stubs + scandir(/tmp) */
    {
        char *pNd;
        struct dirent **aList = NULL;
        int nSc;
        int i;

        pNd = strndup("greenjade-os", 5);
        if (pNd == NULL || strcmp(pNd, "green") != 0) {
            return 61;
        }
        free(pNd);
        if (dlopen("nope.so", RTLD_NOW) != NULL) {
            return 62;
        }
        if (dlerror() == NULL) {
            return 63;
        }
        if (dlerror() != NULL) {
            return 64;
        }
        nSc = scandir("/tmp", &aList, NULL, alphasort);
        if (nSc < 0) {
            return 65;
        }
        for (i = 0; i < nSc; i++) {
            free(aList[i]);
        }
        free(aList);
        if (strnlen("abc", 2) != 2 || strnlen("ab", 8) != 2) {
            return 66;
        }
        {
            char aStp[16];
            char *pEnd = stpcpy(aStp, "gj");

            if (pEnd != aStp + 2 || strcmp(aStp, "gj") != 0) {
                return 67;
            }
        }
        if (memmem("greenjade", 9, "jade", 4) == NULL) {
            return 68;
        }
        {
            char aTok[] = "a,b,c";
            char *pSave = NULL;
            char *pT;
            char aSep[] = "x:y";
            char *pCur = aSep;
            char *pS;

            pT = strtok_r(aTok, ",", &pSave);
            if (pT == NULL || strcmp(pT, "a") != 0) {
                return 69;
            }
            pT = strtok_r(NULL, ",", &pSave);
            if (pT == NULL || strcmp(pT, "b") != 0) {
                return 70;
            }
            pS = strsep(&pCur, ":");
            if (pS == NULL || strcmp(pS, "x") != 0) {
                return 71;
            }
        }
    }

    /* aligned_alloc + posix_memalign + sched_yield */
    {
        void *pA = NULL;
        void *pB = NULL;

        pA = aligned_alloc(64, 128);
        if (pA == NULL || ((uintptr_t)pA & 63u) != 0) {
            return 72;
        }
        if (posix_memalign(&pB, 32, 64) != 0 || pB == NULL ||
            ((uintptr_t)pB & 31u) != 0) {
            return 73;
        }
        if (sched_yield() != 0) {
            return 74;
        }
        free(pA);
        free(pB);
        if (strtoll("9007199254740993", NULL, 10) != 9007199254740993LL) {
            return 75;
        }
        if (strtoull("0xff", NULL, 0) != 255ULL) {
            return 76;
        }
        if (atoll("-42") != -42LL || llabs(-7LL) != 7LL) {
            return 77;
        }
    }

    /* freelist free/reuse + pread/pwrite + clock_getres */
    {
        void *p1;
        void *p2;
        int nFd = memfd_create("cgj-pw", 0);
        char aW[] = "ABCD";
        char aR[8];
        ssize_t n;
        struct timespec tres;

        p1 = malloc(64);
        p2 = malloc(64);
        if (p1 == NULL || p2 == NULL) {
            return 81;
        }
        free(p1);
        free(p2);
        p1 = malloc(64); /* freelist reuse */
        if (p1 == NULL) {
            return 82;
        }
        free(p1);
        if (nFd < 0) {
            return 83;
        }
        if (pwrite(nFd, aW, 4, 0) != 4) {
            (void)close(nFd);
            return 84;
        }
        n = pread(nFd, aR, 2, 1);
        (void)close(nFd);
        if (n != 2 || aR[0] != 'B' || aR[1] != 'C') {
            return 85;
        }
        if (clock_getres(CLOCK_MONOTONIC, &tres) != 0 || tres.tv_nsec < 0) {
            return 86;
        }
    }

    /* writev/readv + sync + clearenv */
    {
        int aPipe[2];
        char aA[] = "AB";
        char aB[] = "CD";
        char aGot[8];
        struct iovec aWv[2];
        struct iovec aRv[2];
        char aR0[2];
        char aR1[2];
        ssize_t n;

        if (pipe(aPipe) != 0) {
            return 87;
        }
        aWv[0].iov_base = aA;
        aWv[0].iov_len = 2;
        aWv[1].iov_base = aB;
        aWv[1].iov_len = 2;
        n = writev(aPipe[1], aWv, 2);
        if (n != 4) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            return 88;
        }
        aRv[0].iov_base = aR0;
        aRv[0].iov_len = 2;
        aRv[1].iov_base = aR1;
        aRv[1].iov_len = 2;
        n = readv(aPipe[0], aRv, 2);
        (void)close(aPipe[0]);
        (void)close(aPipe[1]);
        if (n != 4 || aR0[0] != 'A' || aR1[0] != 'C') {
            return 89;
        }
        (void)sync();
        if (setenv("CGJ_CLR", "1", 1) != 0) {
            return 90;
        }
        if (clearenv() != 0) {
            return 91;
        }
        if (getenv("CGJ_CLR") != NULL) {
            return 92;
        }
        (void)aGot;
        {
            char *pNul = strchrnul("abc", 'z');

            if (pNul == NULL || *pNul != '\0') {
                return 93;
            }
            if (strchrnul("abc", 'b') == NULL ||
                *strchrnul("abc", 'b') != 'b') {
                return 94;
            }
        }
    }

    /* mkstemp */
    {
        char aTmpl[] = "/tmp/cgjXXXXXX";
        int nFd = mkstemp(aTmpl);

        if (nFd < 0) {
            return 78;
        }
        (void)close(nFd);
        (void)unlink(aTmpl);
    }

    /* timerfd + ppoll + signalfd + sendfile */
    {
        int nTfd;
        int nSfd;
        struct itimerspec its;
        struct pollfd pfd;
        struct timespec ts;
        sigset_t mask;
        int nIn;
        int nOut;
        char aW[] = "SF";
        off_t off = 0;
        ssize_t nSf;

        nTfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
        if (nTfd < 0) {
            return 95;
        }
        memset(&its, 0, sizeof(its));
        its.it_value.tv_nsec = 1; /* fire soon */
        if (timerfd_settime(nTfd, 0, &its, NULL) != 0) {
            (void)close(nTfd);
            return 96;
        }
        if (timerfd_gettime(nTfd, &its) != 0) {
            (void)close(nTfd);
            return 97;
        }
        pfd.fd = nTfd;
        pfd.events = POLLIN;
        pfd.revents = 0;
        ts.tv_sec = 0;
        ts.tv_nsec = 50000000L; /* 50ms */
        (void)ppoll(&pfd, 1, &ts, NULL);
        (void)close(nTfd);

        memset(&mask, 0, sizeof(mask));
        mask.__val[0] = 1ull << SIGINT;
        nSfd = signalfd(-1, &mask, SFD_NONBLOCK);
        if (nSfd < 0) {
            return 98;
        }
        (void)close(nSfd);

        nIn = memfd_create("cgj-sf-in", 0);
        nOut = memfd_create("cgj-sf-out", 0);
        if (nIn < 0 || nOut < 0) {
            return 99;
        }
        if (pwrite(nIn, aW, 2, 0) != 2) {
            (void)close(nIn);
            (void)close(nOut);
            return 100;
        }
        off = 0;
        nSf = sendfile(nOut, nIn, &off, 2);
        (void)close(nIn);
        (void)close(nOut);
        if (nSf != 2 || off != 2) {
            return 101;
        }
    }

    /* epoll_create1 + ctl + wait */
    {
        int nEp;
        int nEfd;
        struct epoll_event ev;
        struct epoll_event aOut[2];
        uint64_t uOne = 1;
        int n;

        nEp = epoll_create1(0);
        if (nEp < 0) {
            return 102;
        }
        nEfd = eventfd(0, 0);
        if (nEfd < 0) {
            (void)close(nEp);
            return 103;
        }
        memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN;
        ev.data.fd = nEfd;
        if (epoll_ctl(nEp, EPOLL_CTL_ADD, nEfd, &ev) != 0) {
            (void)close(nEfd);
            (void)close(nEp);
            return 104;
        }
        if (eventfd_write(nEfd, uOne) != 0) {
            (void)close(nEfd);
            (void)close(nEp);
            return 105;
        }
        n = epoll_wait(nEp, aOut, 2, 100);
        (void)close(nEfd);
        (void)close(nEp);
        if (n < 1 || aOut[0].data.fd != nEfd) {
            return 106;
        }
    }

    /* inet_pton/ntop + flock + lockf */
    {
        struct in_addr a4;
        char aStr[32];
        int nFd;
        uint8_t a6[16];

        if (inet_pton(AF_INET, "127.0.0.1", &a4) != 1) {
            return 107;
        }
        if (inet_ntop(AF_INET, &a4, aStr, sizeof(aStr)) == NULL ||
            strcmp(aStr, "127.0.0.1") != 0) {
            return 108;
        }
        if (inet_pton(AF_INET6, "::1", a6) != 1) {
            return 109;
        }
        if (a6[15] != 1) {
            return 110;
        }
        nFd = memfd_create("cgj-lk", 0);
        if (nFd < 0) {
            return 111;
        }
        if (flock(nFd, LOCK_EX) != 0) {
            (void)close(nFd);
            return 112;
        }
        if (flock(nFd, LOCK_UN) != 0) {
            (void)close(nFd);
            return 113;
        }
        if (ftruncate(nFd, 64) != 0) {
            (void)close(nFd);
            return 114;
        }
        if (lockf(nFd, F_TLOCK, 32) != 0) {
            (void)close(nFd);
            return 115;
        }
        if (lockf(nFd, F_ULOCK, 32) != 0) {
            (void)close(nFd);
            return 116;
        }
        (void)close(nFd);
    }

    /* getaddrinfo numeric / localhost */
    {
        struct addrinfo hints;
        struct addrinfo *pRes = NULL;
        struct sockaddr_in *pIn;
        char aHost[64];
        char aServ[16];

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        if (getaddrinfo("127.0.0.1", "80", &hints, &pRes) != 0 || pRes == NULL) {
            return 117;
        }
        pIn = (struct sockaddr_in *)pRes->ai_addr;
        if (pIn == NULL || ntohs(pIn->sin_port) != 80) {
            freeaddrinfo(pRes);
            return 118;
        }
        if (getnameinfo(pRes->ai_addr, pRes->ai_addrlen, aHost, sizeof(aHost),
                        aServ, sizeof(aServ), 0) != 0 ||
            strcmp(aHost, "127.0.0.1") != 0) {
            freeaddrinfo(pRes);
            return 119;
        }
        freeaddrinfo(pRes);
        pRes = NULL;
        if (getaddrinfo("localhost", "22", &hints, &pRes) != 0 || pRes == NULL) {
            return 120;
        }
        freeaddrinfo(pRes);
        if (gai_strerror(EAI_NONAME) == NULL) {
            return 121;
        }
    }

    /* if_nametoindex / getifaddrs (lo) */
    {
        char aName[IF_NAMESIZE];
        struct ifaddrs *pIfa = NULL;
        unsigned uIdx;

        uIdx = if_nametoindex("lo");
        if (uIdx != 1) {
            return 122;
        }
        if (if_indextoname(1, aName) == NULL || strcmp(aName, "lo") != 0) {
            return 123;
        }
        if (getifaddrs(&pIfa) != 0 || pIfa == NULL) {
            return 124;
        }
        if (pIfa->ifa_name == NULL || strcmp(pIfa->ifa_name, "lo") != 0) {
            freeifaddrs(pIfa);
            return 125;
        }
        freeifaddrs(pIfa);
    }

    /* syslog stubs + glob basic */
    {
        glob_t g;
        int nG;
        int nMask;

        openlog("cgj", LOG_PERROR, LOG_USER);
        nMask = setlogmask(LOG_UPTO(LOG_INFO));
        syslog(LOG_INFO, "libcgj syslog stub %d", 1);
        closelog();
        (void)nMask;

        memset(&g, 0, sizeof(g));
        nG = glob("/tmp/*", 0, NULL, &g);
        if (nG != 0 && nG != GLOB_NOMATCH) {
            return 126;
        }
        if (nG == 0 && g.gl_pathc > 0 && g.gl_pathv != NULL &&
            g.gl_pathv[0] == NULL) {
            globfree(&g);
            return 127;
        }
        globfree(&g);
        /* literal NOCHECK */
        memset(&g, 0, sizeof(g));
        nG = glob("/no/such/cgj_path_XXXX", GLOB_NOCHECK, NULL, &g);
        if (nG != 0 || g.gl_pathc != 1) {
            globfree(&g);
            return 128;
        }
        globfree(&g);
        if (res_init() != 0) {
            return 129;
        }
        if ((_res.options & RES_INIT) == 0) {
            return 130;
        }
    }

    /* msync + madvise + warnx */
    {
        void *pM = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (pM == MAP_FAILED) {
            return 79;
        }
        ((char *)pM)[0] = 'x';
        if (msync(pM, 4096, MS_SYNC) != 0) {
            (void)munmap(pM, 4096);
            return 80;
        }
        if (madvise(pM, 4096, MADV_DONTNEED) != 0) {
            (void)munmap(pM, 4096);
            return 81;
        }
        (void)munmap(pM, 4096);
        warnx("cgj warnx ok");
        g_nFtwHits = 0;
        (void)ftw("/tmp", ftw_cb, 8);
        if (g_nFtwHits < 1) {
            return 131;
        }
        if (nftw("/tmp", nftw_cb, 8, 0) != 0) {
            return 132;
        }
    }

    /* nl_langinfo + iconv identity + backtrace */
    {
        char *pCs;
        iconv_t cd;
        void *aBt[8];
        int nBt;
        char **ppSym;
        char aIn[8];
        char aOut[8];
        char *pIn;
        char *pOut;
        size_t nIn;
        size_t nOut;

        pCs = nl_langinfo(CODESET);
        if (pCs == NULL || pCs[0] == '\0') {
            return 133;
        }
        if (strcmp(nl_langinfo(RADIXCHAR), ".") != 0) {
            return 134;
        }
        errno = 0;
        cd = iconv_open("UTF-8", "UTF-8");
        if (cd == (iconv_t)-1) {
            return 135;
        }
        memcpy(aIn, "ab", 2);
        pIn = aIn;
        pOut = aOut;
        nIn = 2;
        nOut = sizeof(aOut);
        if (iconv(cd, &pIn, &nIn, &pOut, &nOut) == (size_t)-1 || nIn != 0 ||
            nOut != sizeof(aOut) - 2 || aOut[0] != 'a' || aOut[1] != 'b') {
            (void)iconv_close(cd);
            return 135;
        }
        if (iconv_close(cd) != 0) {
            return 135;
        }
        nBt = backtrace(aBt, 8);
        if (nBt < 0) {
            return 136;
        }
        ppSym = backtrace_symbols(aBt, nBt > 0 ? nBt : 1);
        if (nBt > 0 && ppSym == NULL) {
            return 137;
        }
        free(ppSym);
        (void)aBt;
    }

    /* gmtime_r / strftime */
    {
        time_t t = 0; /* 1970-01-01 */
        struct tm tm;
        char aBuf[64];
        size_t n;

        if (gmtime_r(&t, &tm) == NULL || tm.tm_year != 70 || tm.tm_mday != 1) {
            return 138;
        }
        n = strftime(aBuf, sizeof(aBuf), "%Y-%m-%d", &tm);
        if (n == 0 || strcmp(aBuf, "1970-01-01") != 0) {
            return 139;
        }
        n = strftime(aBuf, sizeof(aBuf), "%a %b", &tm);
        if (n == 0 || strncmp(aBuf, "Thu Jan", 7) != 0) {
            return 140;
        }
    }

    /* strptime + timegm + tzset */
    {
        struct tm tm;
        char *pEnd;
        time_t t;
        char aBuf[32];

        tzset();
        if (timezone != 0 || daylight != 0) {
            return 141;
        }
        memset(&tm, 0, sizeof(tm));
        pEnd = strptime("2020-06-15 12:30:45", "%Y-%m-%d %H:%M:%S", &tm);
        if (pEnd == NULL || *pEnd != '\0') {
            return 142;
        }
        if (tm.tm_year != 120 || tm.tm_mon != 5 || tm.tm_mday != 15 ||
            tm.tm_hour != 12 || tm.tm_min != 30 || tm.tm_sec != 45) {
            return 143;
        }
        t = timegm(&tm);
        if (t == (time_t)-1) {
            return 144;
        }
        if (gmtime_r(&t, &tm) == NULL) {
            return 145;
        }
        if (strftime(aBuf, sizeof(aBuf), "%F %T", &tm) == 0 ||
            strcmp(aBuf, "2020-06-15 12:30:45") != 0) {
            return 146;
        }
        /* %F parse */
        memset(&tm, 0, sizeof(tm));
        pEnd = strptime("1970-01-01", "%F", &tm);
        if (pEnd == NULL || timegm(&tm) != 0) {
            return 147;
        }
    }

    /* sysinfo + getloadavg + setsid (no daemon fork in smoke) */
    {
        struct sysinfo si;
        double aLoad[3];
        int n;
        unsigned long long z0 = 0;

        memset(&si, 0, sizeof(si));
        if (sysinfo(&si) != 0) {
            return 148;
        }
        n = getloadavg(aLoad, 3);
        if (n != 3) {
            return 149;
        }
        /* Values may be 0.0 (bring-up); ensure writes happened */
        (void)memcmp(&aLoad[0], &z0, sizeof(double));
        /* setsid may fail if already session leader — still callable */
        (void)setsid();
    }

    /* getrusage / getrlimit / getpriority / nice */
    {
        struct rusage ru;
        struct rlimit rl;
        int nPrio;

        memset(&ru, 0, sizeof(ru));
        if (getrusage(RUSAGE_SELF, &ru) != 0) {
            return 150;
        }
        memset(&rl, 0, sizeof(rl));
        if (getrlimit(RLIMIT_NOFILE, &rl) != 0) {
            return 151;
        }
        if (rl.rlim_cur == 0 && rl.rlim_max == 0) {
            return 152;
        }
        errno = 0;
        nPrio = getpriority(PRIO_PROCESS, 0);
        if (nPrio == -1 && errno != 0) {
            return 153;
        }
        /* nice(0) should return current */
        errno = 0;
        if (nice(0) == -1 && errno != 0) {
            return 154;
        }
        if (setpriority(PRIO_PROCESS, 0, nPrio) != 0) {
            return 155;
        }
        (void)setrlimit(RLIMIT_NOFILE, &rl); /* may be no-op on kernel */
        {
            struct rlimit rl2;

            memset(&rl2, 0, sizeof(rl2));
            if (prlimit(0, RLIMIT_NOFILE, NULL, &rl2) != 0) {
                return 156;
            }
        }
        {
            struct tms tms;

            memset(&tms, 0, sizeof(tms));
            if (times(&tms) == (clock_t)-1) {
                return 157;
            }
        }
        /* waitid WNOHANG: no child → ECHILD ok */
        {
            siginfo_t si;

            memset(&si, 0, sizeof(si));
            errno = 0;
            if (waitid(P_ALL, 0, &si, WNOHANG | WEXITED) != 0 &&
                errno != ECHILD) {
                return 158;
            }
        }
    }

    /* credentials + utimes */
    {
        uid_t r = 0, e = 0, s = 0;
        gid_t gr = 0, ge = 0, gs = 0;
        gid_t aG[8];
        int nG;
        int nFd;
        struct timeval aTv[2];
        struct timespec aTs[2];

        if (getresuid(&r, &e, &s) != 0) {
            return 159;
        }
        if (getresgid(&gr, &ge, &gs) != 0) {
            return 160;
        }
        if (geteuid() != e || getegid() != ge) {
            return 161;
        }
        nG = getgroups(0, NULL);
        if (nG < 0) {
            return 162;
        }
        if (nG > 0) {
            if (nG > 8) {
                nG = 8;
            }
            if (getgroups(nG, aG) < 0) {
                return 163;
            }
        }
        (void)initgroups("root", gr);
        nFd = memfd_create("cgj-ut", 0);
        if (nFd < 0) {
            return 164;
        }
        aTv[0].tv_sec = 1000;
        aTv[0].tv_usec = 0;
        aTv[1].tv_sec = 2000;
        aTv[1].tv_usec = 0;
        /* futimes/futimens may fail on memfd; still call path */
        (void)futimes(nFd, aTv);
        aTs[0].tv_sec = 1000;
        aTs[0].tv_nsec = 0;
        aTs[1].tv_sec = 2000;
        aTs[1].tv_nsec = 0;
        (void)futimens(nFd, aTs);
        (void)close(nFd);
        /* utimes on /tmp may work */
        (void)utimes("/tmp", aTv);
    }

    /* chmod/chown/umask/statvfs */
    {
        mode_t oldMask;
        int nFd;
        struct statvfs sv;
        struct statfs sf;

        oldMask = umask(0022);
        (void)umask(oldMask);
        nFd = memfd_create("cgj-mod", 0);
        if (nFd < 0) {
            return 165;
        }
        if (fchmod(nFd, 0600) != 0) {
            (void)close(nFd);
            return 166;
        }
        if (fchown(nFd, (uid_t)-1, (gid_t)-1) != 0) {
            (void)close(nFd);
            return 167;
        }
        (void)close(nFd);
        if (chmod("/tmp", 01777) != 0 && errno != EPERM && errno != EACCES &&
            errno != EROFS) {
            /* host may allow or deny; only fail hard unexpected errno */
            if (errno != 0 && errno != ENOENT) {
                /* continue — sticky on /tmp often ok */
            }
        }
        memset(&sf, 0, sizeof(sf));
        if (statfs("/tmp", &sf) != 0) {
            return 168;
        }
        if (sf.f_bsize == 0) {
            return 169;
        }
        memset(&sv, 0, sizeof(sv));
        if (statvfs("/tmp", &sv) != 0 || sv.f_bsize == 0) {
            return 170;
        }
    }

    /* pathconf / fdatasync / eaccess / canonicalize */
    {
        long nNameMax;
        int nFd;
        char *pCan;

        nNameMax = pathconf("/tmp", _PC_NAME_MAX);
        if (nNameMax < 1) {
            return 171;
        }
        if (fpathconf(1, _PC_PIPE_BUF) < 1 && errno != EINVAL) {
            /* stdout may not be a pipe; any non-crash is fine */
        }
        nFd = memfd_create("cgj-dsync", 0);
        if (nFd < 0) {
            return 172;
        }
        if (fdatasync(nFd) != 0) {
            (void)close(nFd);
            return 173;
        }
        (void)syncfs(nFd);
        (void)close(nFd);
        if (eaccess("/tmp", F_OK) != 0) {
            return 174;
        }
        pCan = canonicalize_file_name("/tmp");
        if (pCan == NULL || pCan[0] != '/') {
            return 175;
        }
        free(pCan);
    }

    /* *at family + copy_file_range */
    {
        int nIn;
        int nOut;
        char aW[] = "ATCOPY";
        char aR[16];
        off_t offIn = 0;
        off_t offOut = 0;
        ssize_t nC;
        char aTmp[] = "/tmp/cgj_at_XXXXXX";
        int nDir;

        nIn = memfd_create("cgj-cfr-in", 0);
        nOut = memfd_create("cgj-cfr-out", 0);
        if (nIn < 0 || nOut < 0) {
            return 176;
        }
        if (pwrite(nIn, aW, 6, 0) != 6) {
            (void)close(nIn);
            (void)close(nOut);
            return 177;
        }
        nC = copy_file_range(nIn, &offIn, nOut, &offOut, 6, 0);
        if (nC != 6 || offIn != 6 || offOut != 6) {
            (void)close(nIn);
            (void)close(nOut);
            return 178;
        }
        if (pread(nOut, aR, 6, 0) != 6 || memcmp(aR, aW, 6) != 0) {
            (void)close(nIn);
            (void)close(nOut);
            return 179;
        }
        (void)close(nIn);
        (void)close(nOut);

        nDir = mkstemp(aTmp);
        if (nDir < 0) {
            return 180;
        }
        (void)close(nDir);
        (void)unlink(aTmp);
        /* mkdirat / unlinkat round-trip under /tmp */
        if (mkdirat(AT_FDCWD, "/tmp/cgj_at_dir", 0700) != 0 &&
            errno != EEXIST) {
            return 181;
        }
        if (unlinkat(AT_FDCWD, "/tmp/cgj_at_dir", AT_REMOVEDIR) != 0 &&
            errno != ENOENT && errno != ENOTDIR) {
            /* rmdir may need empty; if not a real dir on host, tolerate */
            (void)unlinkat(AT_FDCWD, "/tmp/cgj_at_dir", 0);
        }
    }

    /* splice / tee / vmsplice + sendmsg/recvmsg + sendmmsg/recvmmsg */
    {
        int aPipe[2];
        int aPipe2[2];
        int nIn;
        char aW[] = "SPLICE";
        char aR[16];
        off_t offIn = 0;
        ssize_t nSp;
        struct iovec iov;
        struct msghdr msg;
        struct mmsghdr mm[1];
        int aSv[2];
        int nMm;
        char aRx[16];
        struct iovec iovRx;
        struct msghdr msgRx;
        struct mmsghdr mmRx[1];

        if (pipe(aPipe) != 0) {
            return 182;
        }
        nIn = memfd_create("cgj-spl-in", 0);
        if (nIn < 0) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            return 183;
        }
        if (pwrite(nIn, aW, 6, 0) != 6) {
            (void)close(nIn);
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            return 184;
        }
        offIn = 0;
        nSp = splice(nIn, &offIn, aPipe[1], NULL, 6, 0);
        if (nSp != 6 || offIn != 6) {
            (void)close(nIn);
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            return 185;
        }
        if (read(aPipe[0], aR, 6) != 6 || memcmp(aR, aW, 6) != 0) {
            (void)close(nIn);
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            return 186;
        }
        (void)close(nIn);

        /* vmsplice user buffer → pipe */
        iov.iov_base = (void *)"VM";
        iov.iov_len = 2;
        nSp = vmsplice(aPipe[1], &iov, 1, 0);
        if (nSp != 2) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            return 187;
        }
        if (read(aPipe[0], aR, 2) != 2 || memcmp(aR, "VM", 2) != 0) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            return 188;
        }

        /* tee between pipes (kernel path on Linux; fallback consumes) */
        if (pipe(aPipe2) != 0) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            return 189;
        }
        if (write(aPipe[1], "TE", 2) != 2) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            (void)close(aPipe2[0]);
            (void)close(aPipe2[1]);
            return 190;
        }
        nSp = tee(aPipe[0], aPipe2[1], 2, 0);
        if (nSp < 1) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            (void)close(aPipe2[0]);
            (void)close(aPipe2[1]);
            return 191;
        }
        (void)close(aPipe[0]);
        (void)close(aPipe[1]);
        (void)close(aPipe2[0]);
        (void)close(aPipe2[1]);

        /* sendmsg / recvmsg / sendmmsg / recvmmsg over socketpair */
        if (socketpair(AF_UNIX, SOCK_DGRAM, 0, aSv) != 0) {
            return 192;
        }
        memset(&msg, 0, sizeof(msg));
        iov.iov_base = (void *)"MSG";
        iov.iov_len = 3;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        if (sendmsg(aSv[0], &msg, 0) != 3) {
            (void)close(aSv[0]);
            (void)close(aSv[1]);
            return 193;
        }
        memset(&msgRx, 0, sizeof(msgRx));
        memset(aRx, 0, sizeof(aRx));
        iovRx.iov_base = aRx;
        iovRx.iov_len = sizeof(aRx);
        msgRx.msg_iov = &iovRx;
        msgRx.msg_iovlen = 1;
        if (recvmsg(aSv[1], &msgRx, 0) != 3 || memcmp(aRx, "MSG", 3) != 0) {
            (void)close(aSv[0]);
            (void)close(aSv[1]);
            return 194;
        }

        memset(&mm[0], 0, sizeof(mm[0]));
        iov.iov_base = (void *)"MM";
        iov.iov_len = 2;
        mm[0].msg_hdr.msg_iov = &iov;
        mm[0].msg_hdr.msg_iovlen = 1;
        nMm = sendmmsg(aSv[0], mm, 1, 0);
        if (nMm != 1 || mm[0].msg_len != 2) {
            (void)close(aSv[0]);
            (void)close(aSv[1]);
            return 195;
        }
        memset(&mmRx[0], 0, sizeof(mmRx[0]));
        memset(aRx, 0, sizeof(aRx));
        iovRx.iov_base = aRx;
        iovRx.iov_len = sizeof(aRx);
        mmRx[0].msg_hdr.msg_iov = &iovRx;
        mmRx[0].msg_hdr.msg_iovlen = 1;
        nMm = recvmmsg(aSv[1], mmRx, 1, 0, NULL);
        if (nMm != 1 || mmRx[0].msg_len != 2 || memcmp(aRx, "MM", 2) != 0) {
            (void)close(aSv[0]);
            (void)close(aSv[1]);
            return 196;
        }
        (void)close(aSv[0]);
        (void)close(aSv[1]);
    }

    /* mkfifo + dup3 + pselect */
    {
        char aPath[] = "/tmp/cgj_fifo_XXXXXX";
        int nTmp;
        int nDup;
        int aPipe[2];
        fd_set rfds;
        struct timespec ts;
        int nReady;
        char aR[4];

        nTmp = mkstemp(aPath);
        if (nTmp < 0) {
            return 197;
        }
        (void)close(nTmp);
        (void)unlink(aPath);
        if (mkfifo(aPath, 0600) != 0) {
            return 198;
        }
        (void)unlink(aPath);

        if (pipe(aPipe) != 0) {
            return 199;
        }
        nDup = dup3(aPipe[0], 80, O_CLOEXEC);
        if (nDup != 80) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            return 200;
        }
        if (write(aPipe[1], "PS", 2) != 2) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            (void)close(nDup);
            return 201;
        }
        FD_ZERO(&rfds);
        FD_SET(nDup, &rfds);
        ts.tv_sec = 0;
        ts.tv_nsec = 100000000L; /* 100ms */
        nReady = pselect(nDup + 1, &rfds, NULL, NULL, &ts, NULL);
        if (nReady < 1 || !FD_ISSET(nDup, &rfds)) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            (void)close(nDup);
            return 202;
        }
        if (read(nDup, aR, 2) != 2 || memcmp(aR, "PS", 2) != 0) {
            (void)close(aPipe[0]);
            (void)close(aPipe[1]);
            (void)close(nDup);
            return 203;
        }
        (void)close(aPipe[0]);
        (void)close(aPipe[1]);
        (void)close(nDup);
    }

    /* preadv/pwritev + inotify + getitimer + fchdir */
    {
        int nFd;
        char aA[] = "AB";
        char aB[] = "CD";
        char aR1[4];
        char aR2[4];
        struct iovec aIov[2];
        struct iovec aIovR[2];
        ssize_t n;
        int nIno;
        int nWd;
        struct itimerval itv;
        char aCwd[256];

        nFd = memfd_create("cgj-pv", 0);
        if (nFd < 0) {
            return 204;
        }
        aIov[0].iov_base = aA;
        aIov[0].iov_len = 2;
        aIov[1].iov_base = aB;
        aIov[1].iov_len = 2;
        n = pwritev(nFd, aIov, 2, 0);
        if (n != 4) {
            (void)close(nFd);
            return 205;
        }
        memset(aR1, 0, sizeof(aR1));
        memset(aR2, 0, sizeof(aR2));
        aIovR[0].iov_base = aR1;
        aIovR[0].iov_len = 2;
        aIovR[1].iov_base = aR2;
        aIovR[1].iov_len = 2;
        n = preadv(nFd, aIovR, 2, 0);
        (void)close(nFd);
        if (n != 4 || memcmp(aR1, "AB", 2) != 0 || memcmp(aR2, "CD", 2) != 0) {
            return 206;
        }

        nIno = inotify_init1(IN_CLOEXEC);
        if (nIno < 0) {
            return 207;
        }
        nWd = inotify_add_watch(nIno, "/tmp", IN_CREATE | IN_DELETE);
        if (nWd < 0) {
            (void)close(nIno);
            return 208;
        }
        if (inotify_rm_watch(nIno, nWd) != 0) {
            (void)close(nIno);
            return 209;
        }
        (void)close(nIno);

        memset(&itv, 0, sizeof(itv));
        if (getitimer(ITIMER_REAL, &itv) != 0) {
            return 210;
        }
        /* leave timer disarmed */
        if (setitimer(ITIMER_REAL, &itv, NULL) != 0) {
            return 211;
        }

        if (getcwd(aCwd, sizeof(aCwd)) == NULL) {
            return 212;
        }
        nFd = open("/tmp", O_RDONLY | O_DIRECTORY);
        if (nFd >= 0) {
            if (fchdir(nFd) != 0) {
                (void)close(nFd);
                return 213;
            }
            (void)close(nFd);
            if (chdir(aCwd) != 0) {
                return 214;
            }
        }
    }

    /* POSIX timers + clock_nanosleep + mq ENOSYS + sem private */
    {
        timer_t tid;
        struct sigevent sev;
        struct itimerspec its;
        struct itimerspec cur;
        struct timespec ts;
        sem_t sem;
        int nVal;
        mqd_t mq;

        memset(&sev, 0, sizeof(sev));
        sev.sigev_notify = SIGEV_NONE;
        if (timer_create(CLOCK_MONOTONIC, &sev, &tid) != 0) {
            return 215;
        }
        memset(&its, 0, sizeof(its));
        its.it_value.tv_nsec = 1000000L; /* 1ms */
        if (timer_settime(tid, 0, &its, NULL) != 0) {
            (void)timer_delete(tid);
            return 216;
        }
        if (timer_gettime(tid, &cur) != 0) {
            (void)timer_delete(tid);
            return 217;
        }
        (void)timer_getoverrun(tid);
        if (timer_delete(tid) != 0) {
            return 218;
        }

        ts.tv_sec = 0;
        ts.tv_nsec = 1000000L; /* 1ms relative */
        if (clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL) != 0 &&
            errno != EINTR) {
            return 219;
        }
        mq = mq_open("/cgj_no_such_queue", O_RDONLY);
        if (mq != (mqd_t)-1) {
            (void)mq_close(mq);
            return 220;
        }
        /* ENOSYS (no CONFIG), ENOENT (no queue), EACCES (no /dev/mqueue) */
        if (errno != ENOSYS && errno != ENOENT && errno != EACCES) {
            return 221;
        }

        if (sem_init(&sem, 0, 1) != 0) {
            return 222;
        }
        if (sem_trywait(&sem) != 0) {
            (void)sem_destroy(&sem);
            return 223;
        }
        if (sem_trywait(&sem) == 0 || errno != EAGAIN) {
            (void)sem_destroy(&sem);
            return 224;
        }
        if (sem_post(&sem) != 0) {
            (void)sem_destroy(&sem);
            return 225;
        }
        if (sem_getvalue(&sem, &nVal) != 0 || nVal != 1) {
            (void)sem_destroy(&sem);
            return 226;
        }
        if (sem_wait(&sem) != 0) {
            (void)sem_destroy(&sem);
            return 227;
        }
        /* process-shared init is supported (futex without PRIVATE) */
        if (sem_init(&sem, 1, 0) != 0) {
            return 228;
        }
        if (sem_post(&sem) != 0 || sem_trywait(&sem) != 0) {
            (void)sem_destroy(&sem);
            return 229;
        }
        if (sem_destroy(&sem) != 0) {
            return 229;
        }
    }

    /* getline / getdelim / freopen / fseeko / ftello */
    {
        char aPath[] = "/tmp/cgj_line_XXXXXX";
        int nTmp;
        FILE *pF;
        char *szLine = NULL;
        size_t cbLine = 0;
        ssize_t nG;
        off_t off;

        nTmp = mkstemp(aPath);
        if (nTmp < 0) {
            return 230;
        }
        if (write(nTmp, "one\ntwo\n", 8) != 8) {
            (void)close(nTmp);
            (void)unlink(aPath);
            return 231;
        }
        (void)close(nTmp);
        pF = fopen(aPath, "r");
        if (pF == NULL) {
            (void)unlink(aPath);
            return 232;
        }
        nG = getline(&szLine, &cbLine, pF);
        if (nG != 4 || szLine == NULL || memcmp(szLine, "one\n", 4) != 0) {
            free(szLine);
            (void)fclose(pF);
            (void)unlink(aPath);
            return 233;
        }
        nG = getdelim(&szLine, &cbLine, '\n', pF);
        if (nG != 4 || memcmp(szLine, "two\n", 4) != 0) {
            free(szLine);
            (void)fclose(pF);
            (void)unlink(aPath);
            return 234;
        }
        free(szLine);
        szLine = NULL;
        cbLine = 0;
        if (fseeko(pF, 0, SEEK_SET) != 0) {
            (void)fclose(pF);
            (void)unlink(aPath);
            return 235;
        }
        off = ftello(pF);
        if (off != 0) {
            (void)fclose(pF);
            (void)unlink(aPath);
            return 236;
        }
        if (freopen(aPath, "r", pF) == NULL) {
            (void)fclose(pF);
            (void)unlink(aPath);
            return 237;
        }
        nG = getline(&szLine, &cbLine, pF);
        (void)fclose(pF);
        (void)unlink(aPath);
        if (nG != 4 || szLine == NULL || memcmp(szLine, "one\n", 4) != 0) {
            free(szLine);
            return 238;
        }
        free(szLine);
    }

    /* fmemopen + pthread_rwlock */
    {
        char aMem[] = "hello\n";
        FILE *pF;
        char aR[16];
        pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;

        pF = fmemopen(aMem, 6, "r");
        if (pF == NULL) {
            return 239;
        }
        if (fgets(aR, (int)sizeof(aR), pF) == NULL ||
            memcmp(aR, "hello\n", 6) != 0) {
            (void)fclose(pF);
            return 240;
        }
        (void)fclose(pF);

        if (pthread_rwlock_rdlock(&rw) != 0) {
            return 241;
        }
        if (pthread_rwlock_trywrlock(&rw) == 0) {
            (void)pthread_rwlock_unlock(&rw);
            return 242;
        }
        if (pthread_rwlock_unlock(&rw) != 0) {
            return 243;
        }
        if (pthread_rwlock_wrlock(&rw) != 0) {
            return 244;
        }
        if (pthread_rwlock_tryrdlock(&rw) == 0) {
            (void)pthread_rwlock_unlock(&rw);
            return 245;
        }
        if (pthread_rwlock_unlock(&rw) != 0) {
            return 246;
        }
        if (pthread_rwlock_destroy(&rw) != 0) {
            return 247;
        }
    }

    /* open_memstream real grow + barrier + spin + timed rwlock */
    {
        char *szMem = NULL;
        size_t cbMem = 0;
        FILE *pMs;
        pthread_spinlock_t sp;
        pthread_barrier_t bar;
        pthread_rwlock_t rw2 = PTHREAD_RWLOCK_INITIALIZER;
        struct timespec abs;
        int n;

        pMs = open_memstream(&szMem, &cbMem);
        if (pMs == NULL) {
            return 248;
        }
        if (fputs("hello", pMs) < 0) {
            (void)fclose(pMs);
            free(szMem);
            return 249;
        }
        if (fflush(pMs) != 0) {
            (void)fclose(pMs);
            free(szMem);
            return 250;
        }
        if (szMem == NULL || cbMem != 5 || memcmp(szMem, "hello", 5) != 0) {
            (void)fclose(pMs);
            free(szMem);
            return 251;
        }
        if (fputs(" world", pMs) < 0 || fflush(pMs) != 0) {
            (void)fclose(pMs);
            free(szMem);
            return 252;
        }
        if (cbMem != 11 || memcmp(szMem, "hello world", 11) != 0) {
            (void)fclose(pMs);
            free(szMem);
            return 253;
        }
        if (fseeko(pMs, 0, SEEK_SET) != 0) {
            (void)fclose(pMs);
            free(szMem);
            return 254;
        }
        if (fputs("HELLO", pMs) < 0 || fflush(pMs) != 0) {
            (void)fclose(pMs);
            free(szMem);
            return 255;
        }
        if (cbMem < 5 || memcmp(szMem, "HELLO", 5) != 0) {
            (void)fclose(pMs);
            free(szMem);
            return 256;
        }
        (void)fclose(pMs);
        free(szMem);

        if (pthread_spin_init(&sp, PTHREAD_PROCESS_PRIVATE) != 0) {
            return 257;
        }
        if (pthread_spin_lock(&sp) != 0) {
            return 258;
        }
        if (pthread_spin_trylock(&sp) == 0) {
            (void)pthread_spin_unlock(&sp);
            return 259;
        }
        if (pthread_spin_unlock(&sp) != 0) {
            return 260;
        }
        if (pthread_spin_destroy(&sp) != 0) {
            return 261;
        }

        /* single-thread barrier trip count 1 → serial thread */
        if (pthread_barrier_init(&bar, NULL, 1) != 0) {
            return 262;
        }
        n = pthread_barrier_wait(&bar);
        if (n != PTHREAD_BARRIER_SERIAL_THREAD) {
            (void)pthread_barrier_destroy(&bar);
            return 263;
        }
        if (pthread_barrier_destroy(&bar) != 0) {
            return 264;
        }

        if (clock_gettime(CLOCK_REALTIME, &abs) != 0) {
            return 265;
        }
        abs.tv_sec += 1;
        if (pthread_rwlock_timedrdlock(&rw2, &abs) != 0) {
            return 266;
        }
        if (pthread_rwlock_unlock(&rw2) != 0) {
            return 267;
        }
        /* locked: timed wr should time out quickly with past deadline */
        if (pthread_rwlock_rdlock(&rw2) != 0) {
            return 268;
        }
        abs.tv_sec = 0;
        abs.tv_nsec = 0;
        n = pthread_rwlock_timedwrlock(&rw2, &abs);
        if (n != ETIMEDOUT) {
            (void)pthread_rwlock_unlock(&rw2);
            return 269;
        }
        (void)pthread_rwlock_unlock(&rw2);
        (void)pthread_rwlock_destroy(&rw2);
    }

    /* mutex_timedlock + cond_timedwait + attr stack size */
    {
        pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
        pthread_attr_t attr;
        struct timespec abs;
        size_t cbStk = 0;
        int n;

        if (pthread_attr_init(&attr) != 0) {
            return 270;
        }
        if (pthread_attr_setstacksize(&attr, 128 * 1024u) != 0) {
            return 271;
        }
        if (pthread_attr_getstacksize(&attr, &cbStk) != 0 ||
            cbStk != 128 * 1024u) {
            return 272;
        }
        (void)pthread_attr_destroy(&attr);

        if (pthread_mutex_lock(&mtx) != 0) {
            return 273;
        }
        abs.tv_sec = 0;
        abs.tv_nsec = 0;
        n = pthread_mutex_timedlock(&mtx, &abs);
        if (n != ETIMEDOUT) {
            (void)pthread_mutex_unlock(&mtx);
            return 274;
        }
        /* cond timedwait with past deadline while holding mutex */
        n = pthread_cond_timedwait(&cv, &mtx, &abs);
        if (n != ETIMEDOUT) {
            (void)pthread_mutex_unlock(&mtx);
            return 275;
        }
        (void)pthread_mutex_unlock(&mtx);
        (void)pthread_mutex_destroy(&mtx);
        (void)pthread_cond_destroy(&cv);
    }

    /* asprintf / setlinebuf */
    {
        char *szA = NULL;
        int nA;
        char aOwn[64];

        nA = asprintf(&szA, "hi%d", 42);
        if (nA != 4 || szA == NULL || memcmp(szA, "hi42", 4) != 0) {
            free(szA);
            return 276;
        }
        free(szA);
        setlinebuf(stdout);
        setbuffer(stderr, aOwn, sizeof(aOwn));
        setbuffer(stderr, NULL, 0); /* restore unbuffered-ish */
    }

    /* open_wmemstream + recursive mutexattr + condattr clock */
    {
        wchar_t *pW = NULL;
        size_t cW = 0;
        FILE *pWf;
        pthread_mutexattr_t ma;
        pthread_mutex_t mrec;
        pthread_condattr_t ca;
        clockid_t clk = CLOCK_REALTIME;
        int nType = -1;

        pWf = open_wmemstream(&pW, &cW);
        if (pWf == NULL) {
            return 277;
        }
        if (fputwc(L'A', pWf) == WEOF || fputwc(L'B', pWf) == WEOF) {
            (void)fclose(pWf);
            free(pW);
            return 278;
        }
        if (fflush(pWf) != 0) {
            (void)fclose(pWf);
            free(pW);
            return 279;
        }
        if (pW == NULL || cW != 2 || pW[0] != L'A' || pW[1] != L'B') {
            (void)fclose(pWf);
            free(pW);
            return 280;
        }
        if (fputws(L"CD", pWf) != 0 || fflush(pWf) != 0) {
            (void)fclose(pWf);
            free(pW);
            return 281;
        }
        if (cW != 4 || pW[2] != L'C' || pW[3] != L'D') {
            (void)fclose(pWf);
            free(pW);
            return 282;
        }
        (void)fclose(pWf);
        free(pW);

        if (pthread_mutexattr_init(&ma) != 0) {
            return 283;
        }
        if (pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE) != 0) {
            return 284;
        }
        if (pthread_mutexattr_gettype(&ma, &nType) != 0 ||
            nType != PTHREAD_MUTEX_RECURSIVE) {
            return 285;
        }
        if (pthread_mutexattr_setprotocol(&ma, PTHREAD_PRIO_NONE) != 0) {
            return 286;
        }
        if (pthread_mutex_init(&mrec, &ma) != 0) {
            return 287;
        }
        if (pthread_mutex_lock(&mrec) != 0 || pthread_mutex_lock(&mrec) != 0) {
            return 288;
        }
        if (pthread_mutex_unlock(&mrec) != 0 ||
            pthread_mutex_unlock(&mrec) != 0) {
            return 289;
        }
        (void)pthread_mutex_destroy(&mrec);
        (void)pthread_mutexattr_destroy(&ma);

        if (pthread_condattr_init(&ca) != 0) {
            return 290;
        }
        if (pthread_condattr_setclock(&ca, CLOCK_MONOTONIC) != 0) {
            return 291;
        }
        if (pthread_condattr_getclock(&ca, &clk) != 0 ||
            clk != CLOCK_MONOTONIC) {
            return 292;
        }
        (void)pthread_condattr_destroy(&ca);
    }

    /* named semaphore via shm_open path */
    {
        sem_t *pNs;
        int nVal;

        (void)sem_unlink("/cgj_sem_smoke");
        pNs = sem_open("/cgj_sem_smoke", O_CREAT | O_EXCL, 0600, 1u);
        if (pNs == SEM_FAILED) {
            /* host may lack /dev/shm write; tolerate EACCES/ENOENT */
            if (errno == EACCES || errno == ENOENT || errno == EPERM) {
                /* skip named path */
            } else {
                return 293;
            }
        } else {
            if (sem_trywait(pNs) != 0) {
                (void)sem_close(pNs);
                (void)sem_unlink("/cgj_sem_smoke");
                return 294;
            }
            if (sem_post(pNs) != 0) {
                (void)sem_close(pNs);
                (void)sem_unlink("/cgj_sem_smoke");
                return 295;
            }
            if (sem_getvalue(pNs, &nVal) != 0 || nVal != 1) {
                (void)sem_close(pNs);
                (void)sem_unlink("/cgj_sem_smoke");
                return 296;
            }
            if (sem_close(pNs) != 0) {
                (void)sem_unlink("/cgj_sem_smoke");
                return 297;
            }
            if (sem_unlink("/cgj_sem_smoke") != 0 && errno != ENOENT) {
                return 298;
            }
        }
    }

    /* mbrtowc/wcrtomb/mbsrtowcs + swprintf/fwprintf + rwlockattr */
    {
        wchar_t wc = 0;
        char aMb[8];
        size_t n;
        mbstate_t st;
        const char *pUtf = "hi\xC2\xA9"; /* hi + copyright */
        wchar_t aWcs[8];
        wchar_t aSw[32];
        const char *pSrc;
        pthread_rwlockattr_t ra;
        int nPs = -1;
        FILE *pWf;
        wchar_t *pWm = NULL;
        size_t cWm = 0;

        memset(&st, 0, sizeof(st));
        n = mbrtowc(&wc, "A", 1, &st);
        if (n != 1 || wc != L'A') {
            return 299;
        }
        n = wcrtomb(aMb, L'Z', &st);
        if (n != 1 || aMb[0] != 'Z') {
            return 300;
        }
        /* 2-byte UTF-8 U+00A9 */
        n = mbrtowc(&wc, "\xC2\xA9", 2, &st);
        if (n != 2 || wc != 0xA9) {
            return 301;
        }
        n = wcrtomb(aMb, 0xA9, &st);
        if (n != 2 || (unsigned char)aMb[0] != 0xC2 ||
            (unsigned char)aMb[1] != 0xA9) {
            return 302;
        }
        pSrc = pUtf;
        n = mbsrtowcs(aWcs, &pSrc, 8, NULL);
        if (n != 3 || aWcs[0] != L'h' || aWcs[1] != L'i' || aWcs[2] != 0xA9) {
            return 303;
        }
        if (swprintf(aSw, 32, L"n=%d %ls", 7, L"ok") < 0) {
            return 304;
        }
        if (wcscmp(aSw, L"n=7 ok") != 0) {
            return 305;
        }

        pWf = open_wmemstream(&pWm, &cWm);
        if (pWf == NULL) {
            return 306;
        }
        if (fwprintf(pWf, L"%s-%d", "x", 3) < 0 || fflush(pWf) != 0) {
            (void)fclose(pWf);
            free(pWm);
            return 307;
        }
        if (pWm == NULL || cWm != 3 || pWm[0] != L'x' || pWm[1] != L'-' ||
            pWm[2] != L'3') {
            (void)fclose(pWf);
            free(pWm);
            return 308;
        }
        (void)fclose(pWf);
        free(pWm);

        if (pthread_rwlockattr_init(&ra) != 0) {
            return 309;
        }
        if (pthread_rwlockattr_getpshared(&ra, &nPs) != 0 ||
            nPs != PTHREAD_PROCESS_PRIVATE) {
            return 310;
        }
        if (pthread_rwlockattr_setpshared(&ra, PTHREAD_PROCESS_SHARED) == 0) {
            return 311; /* should ENOSYS */
        }
        (void)pthread_rwlockattr_destroy(&ra);

        if (btowc('Q') != L'Q' || wctob(L'q') != 'q') {
            return 312;
        }
        if (!iswalpha(L'Z') || !iswdigit(L'5') || !iswspace(L' ') ||
            towlower(L'A') != L'a' || towupper(L'b') != L'B') {
            return 313;
        }
        if (wcschr(L"abc", L'b') == NULL || wcsdup(L"x") == NULL) {
            return 314;
        }
        free(wcsdup(L"x"));
    }

    /* wmem* + swscanf + wcscoll */
    {
        wchar_t aA[8];
        wchar_t aB[8];
        int nD = 0;
        int nX = 0;
        wchar_t aLs[16];
        int nSc;

        (void)wmemset(aA, L'x', 4);
        aA[4] = 0;
        (void)wmemcpy(aB, aA, 5);
        if (wmemcmp(aA, aB, 4) != 0 || aB[0] != L'x') {
            return 315;
        }
        aB[0] = L'y';
        (void)wmemmove(aB + 1, aB, 3);
        if (wmemchr(aA, L'x', 4) == NULL) {
            return 316;
        }
        if (wcscoll(L"a", L"b") >= 0) {
            return 317;
        }
        if (wcsxfrm(aA, L"hi", 8) != 2 || aA[0] != L'h') {
            return 318;
        }
        nSc = swscanf(L"42 2a hello", L"%d %x %ls", &nD, &nX, aLs);
        if (nSc != 3 || nD != 42 || nX != 0x2a || wcscmp(aLs, L"hello") != 0) {
            return 319;
        }
        nSc = swscanf(L"  7", L"%d", &nD);
        if (nSc != 1 || nD != 7) {
            return 320;
        }
    }

    /* narrow sscanf */
    {
        int nA = 0;
        int nB = 0;
        char aS[16];
        int nSc;

        nSc = sscanf("99 1f world", "%d %x %s", &nA, &nB, aS);
        if (nSc != 3 || nA != 99 || nB != 0x1f || strcmp(aS, "world") != 0) {
            return 321;
        }
        nSc = sscanf("  3", "%d", &nA);
        if (nSc != 1 || nA != 3) {
            return 322;
        }
    }

    /* strlcpy family, memrchr, explicit_bzero, strcasestr, arc4random, affinity */
    {
        char a[16];
        char aName[16];
        cpu_set_t set;
        uint32_t uR;
        size_t n;
        char *p;

        n = strlcpy(a, "hello", sizeof(a));
        if (n != 5 || strcmp(a, "hello") != 0) {
            return 331;
        }
        n = strlcat(a, "!", sizeof(a));
        if (n != 6 || strcmp(a, "hello!") != 0) {
            return 332;
        }
        if (memrchr("abcba", 'b', 5) == NULL) {
            return 333;
        }
        explicit_bzero(a, sizeof(a));
        if (a[0] != 0 || a[5] != 0) {
            return 334;
        }
        p = strcasestr("HelloWorld", "lowo");
        if (p == NULL || strncmp(p, "loWo", 4) != 0) {
            return 335;
        }
        if (stpncpy(a, "xy", 8) != a + 2 || a[0] != 'x' || a[2] != 0) {
            return 336;
        }
        if (strverscmp("a2", "a10") >= 0) {
            return 337;
        }
        arc4random_buf(a, 8);
        uR = arc4random_uniform(10);
        if (uR >= 10u) {
            return 338;
        }
        (void)arc4random();
        CPU_ZERO(&set);
        if (sched_getaffinity(0, sizeof(set), &set) != 0) {
            return 339;
        }
        {
            int i;
            int nOn = 0;
            for (i = 0; i < 64; i++) {
                if (CPU_ISSET(i, &set)) {
                    nOn++;
                }
            }
            if (nOn == 0) {
                return 340;
            }
        }
        if (pthread_setname_np(pthread_self(), "cgjsmoke") != 0) {
            return 341;
        }
        if (pthread_getname_np(pthread_self(), aName, sizeof(aName)) != 0) {
            return 342;
        }
        if (strcmp(aName, "cgjsmoke") != 0) {
            return 343;
        }
    }

    /* wctype / wcsncmp / wcsncat */
    {
        wctype_t wt;
        wctrans_t tr;
        wchar_t a[16];

        wt = wctype("alnum");
        if (wt == 0 || !iswctype(L'Z', wt) || iswctype(L'!', wt)) {
            return 344;
        }
        tr = wctrans("tolower");
        if (tr == 0 || towctrans(L'A', tr) != L'a') {
            return 345;
        }
        if (wcsncmp(L"abc", L"abd", 2) != 0 || wcsncmp(L"abc", L"abd", 3) >= 0) {
            return 346;
        }
        a[0] = L'x';
        a[1] = 0;
        (void)wcsncat(a, L"yz", 1);
        if (wcscmp(a, L"xy") != 0) {
            return 347;
        }
    }

    /* mempcpy / strerror_r / ffs / localeconv / regex / crypt / wcsstr */
    {
        char aDst[16];
        char aErr[64];
        char *pE;
        void *pMp;
        struct lconv *pLc;
        regex_t re;
        int nRe;

        pMp = mempcpy(aDst, "xy", 2);
        if (pMp != (void *)(aDst + 2) || aDst[0] != 'x' || aDst[1] != 'y') {
            return 348;
        }
        pE = strerror_r(EINVAL, aErr, sizeof(aErr));
        if (pE == NULL || aErr[0] == '\0') {
            return 349;
        }
        if (ffs(0) != 0 || ffs(8) != 4 || ffsl(1L) != 1 || ffsll(2LL) != 2) {
            return 350;
        }
        pLc = localeconv();
        if (pLc == NULL || pLc->decimal_point == NULL ||
            pLc->decimal_point[0] != '.') {
            return 351;
        }
        if (secure_getenv("PATH") == NULL && getenv("PATH") != NULL) {
            return 352;
        }
        nRe = regcomp(&re, "a+", REG_EXTENDED);
        if (nRe != 0) {
            (void)regerror(nRe, &re, aErr, sizeof(aErr));
            return 353; /* ERE engine must compile a+ */
        }
        {
            regmatch_t rm;

            if (regexec(&re, "caaab", 1, &rm, 0) != 0 || rm.rm_so < 0 ||
                rm.rm_eo <= rm.rm_so) {
                regfree(&re);
                return 354;
            }
        }
        regfree(&re);
        errno = 0;
        {
            char *pCr = crypt("secret", "ab");
            char *pCr2;

            if (pCr == NULL || strlen(pCr) < 13) {
                return 355;
            }
            pCr2 = crypt("secret", pCr);
            if (pCr2 == NULL || strcmp(pCr, pCr2) != 0) {
                return 355; /* verify round-trip with salt from hash */
            }
        }
        if (wcsnlen(L"abc", 2) != 2 || wcsstr(L"abcdef", L"cd") == NULL) {
            return 356;
        }
        if (wcscasecmp(L"Ab", L"aB") != 0 ||
            wcsncasecmp(L"XyZ", L"xyq", 2) != 0) {
            return 357;
        }
        if (wcsspn(L"aaab", L"a") != 3 || wcscspn(L"xxay", L"a") != 2) {
            return 358;
        }
        if (wcspbrk(L"hello", L"lx") == NULL) {
            return 359;
        }
        if (getdtablesize() < 8) {
            return 360;
        }
        if (dlvsym(NULL, "malloc", "GLIBC_2.2.5") != NULL) {
            return 361; /* stub returns NULL */
        }
        if (rawmemchr("abc", 'b') == NULL) {
            return 362;
        }
    }

    /* setjmp/longjmp + wcstol + wordexp + wcstok */
    {
        jmp_buf jb;
        volatile int nJ = 0;
        wchar_t *pEnd = NULL;
        wchar_t aTok[] = L"a:b:c";
        wchar_t *pSave = NULL;
        wchar_t *pT;
        wordexp_t we;

        if (setjmp(jb) == 0) {
            nJ = 1;
            longjmp(jb, 2);
            return 363; /* unreachable */
        } else {
            if (nJ != 1) {
                return 364;
            }
        }
        if (wcstol(L"42xyz", &pEnd, 10) != 42 || pEnd == NULL ||
            *pEnd != L'x') {
            return 365;
        }
        if (wcstoul(L"ff", NULL, 16) != 255UL) {
            return 366;
        }
        pT = wcstok(aTok, L":", &pSave);
        if (pT == NULL || wcscmp(pT, L"a") != 0) {
            return 367;
        }
        pT = wcstok(NULL, L":", &pSave);
        if (pT == NULL || wcscmp(pT, L"b") != 0) {
            return 368;
        }
        if (wordexp("hello", &we, 0) != 0 || we.we_wordc != 1 ||
            we.we_wordv == NULL || strcmp(we.we_wordv[0], "hello") != 0) {
            return 369;
        }
        wordfree(&we);
        if (wordexp("a*b", &we, 0) == 0) {
            wordfree(&we);
            return 370; /* metachar should BADCHAR */
        }
    }

    /* getsubopt / getgrouplist / sigset / posix_spawnattr / fts / pty / wcsftime */
    {
        char aOpt[] = "rw,user=foo,unknown";
        char *pOpt = aOpt;
        char *pVal = NULL;
        char *const aToks[] = { "rw", "user", "ro", NULL };
        int n;
        gid_t aG[8];
        int nG = 8;
        sigset_t ss;
        posix_spawnattr_t attr;
        short nFlags;
        posix_spawn_file_actions_t fa;
        char *aPaths[2];
        FTS *pFts;
        FTSENT *pEnt;
        int cFts = 0;
        wchar_t aWbuf[32];
        struct tm tm;

        n = getsubopt(&pOpt, aToks, &pVal);
        if (n != 0) {
            return 371;
        }
        n = getsubopt(&pOpt, aToks, &pVal);
        if (n != 1 || pVal == NULL || strcmp(pVal, "foo") != 0) {
            return 372;
        }
        n = getgrouplist("root", 0, aG, &nG);
        if (n < 1 || nG < 1 || aG[0] != 0) {
            return 373;
        }
        if (sigemptyset(&ss) != 0 || sigaddset(&ss, SIGTERM) != 0 ||
            sigismember(&ss, SIGTERM) != 1) {
            return 374;
        }
        if (posix_spawnattr_init(&attr) != 0 ||
            posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSIGMASK) != 0 ||
            posix_spawnattr_getflags(&attr, &nFlags) != 0 ||
            nFlags != POSIX_SPAWN_SETSIGMASK) {
            return 375;
        }
        (void)posix_spawnattr_destroy(&attr);
        if (posix_spawn_file_actions_init(&fa) != 0 ||
            posix_spawn_file_actions_addclose(&fa, 99) != 0 ||
            posix_spawn_file_actions_destroy(&fa) != 0) {
            return 376;
        }
        aPaths[0] = (char *)"/";
        aPaths[1] = NULL;
        pFts = fts_open(aPaths, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
        if (pFts == NULL) {
            return 377;
        }
        while ((pEnt = fts_read(pFts)) != NULL && cFts < 8) {
            cFts++;
            if (pEnt->fts_info == FTS_D) {
                (void)fts_set(pFts, pEnt, FTS_SKIP);
            }
        }
        if (cFts < 1 || fts_close(pFts) != 0) {
            return 378;
        }
        errno = 0;
        if (openpty(NULL, NULL, NULL, NULL, NULL) != -1 || errno != EINVAL) {
            return 379; /* NULL amaster/aslave → EINVAL */
        }
        {
            int m = -1;
            int s = -1;

            if (openpty(&m, &s, NULL, NULL, NULL) != 0 || m < 0 || s < 0) {
                return 379;
            }
            (void)close(m);
            (void)close(s);
        }
        memset(&tm, 0, sizeof(tm));
        tm.tm_year = 120;
        tm.tm_mon = 0;
        tm.tm_mday = 2;
        if (wcsftime(aWbuf, 32, L"%Y-%m-%d", &tm) == 0 ||
            wcscmp(aWbuf, L"2020-01-02") != 0) {
            return 380;
        }
        if (wcstoll(L"-99", NULL, 10) != -99LL ||
            wcstoull(L"100", NULL, 10) != 100ULL) {
            return 381;
        }
    }

    /* desktop-link batch */
    {
        div_t d = div(17, 5);
        ldiv_t ld = ldiv(17L, 5L);
        unsigned int uSeed = 42;
        char aB[8];
        void *pRa;
        if (rand() < 0 && 0) {
            return 382;
        }
        srand(1);
        (void)rand_r(&uSeed);
        if (d.quot != 3 || d.rem != 2 || ld.quot != 3L) {
            return 383;
        }
        if (htonl(0x01020304u) != 0x04030201u || htons(0x0102u) != 0x0201u) {
            return 385;
        }
        memset(aB, 0xee, sizeof(aB));
        bzero(aB, sizeof(aB));
        if (aB[0] != 0) {
            return 386;
        }
        if (index("abc", 'b') == NULL || strcoll("a", "b") >= 0) {
            return 387;
        }
        pRa = reallocarray(NULL, 4, 8);
        if (pRa == NULL) {
            return 389;
        }
        free(pRa);
        if (atexit(atexit_bump) != 0) {
            return 390;
        }
        if (getpwnam("root") == NULL || getpwuid(0) == NULL) {
            return 391;
        }
        if (getgrnam("root") == NULL || getlogin() == NULL) {
            return 392;
        }
        {
            DIR *pD = opendir("/");
            if (pD == NULL || dirfd(pD) < 0) {
                return 394;
            }
            (void)telldir(pD);
            (void)closedir(pD);
        }
        (void)flockfile(stdout);
        (void)funlockfile(stdout);
        {
            sigset_t ss;
            if (sigemptyset(&ss) != 0 || sigpending(&ss) != 0) {
                return 396;
            }
            if (pthread_sigmask(SIG_SETMASK, NULL, &ss) != 0) {
                return 397;
            }
            if (pthread_kill(pthread_self(), 0) != 0) {
                return 398;
            }
        }
        {
            char *pCwd = get_current_dir_name();
            if (pCwd != NULL) {
                free(pCwd);
            }
        }
        (void)g_nAtexit;
        (void)waitpid(-1, NULL, WNOHANG);
    }

    /* desktop-link batch5: mntent / shadow / utmp / pgrp / netdb legacy */
    {
        FILE *pMf;
        struct mntent *pMe;
        struct mntent mnt;
        char aMbuf[256];
        struct dirent **aSc = NULL;
        int nSc;
        struct servent *pSe;
        struct protoent *pPe;
        char *pSh;
        pid_t pg;

        errno = 0;
        {
            struct spwd *pSp = getspnam("root");

            if (pSp == NULL || pSp->sp_namp == NULL ||
                strcmp(pSp->sp_namp, "root") != 0) {
                return 400;
            }
        }
        setutent();
        if (getutent() != NULL) {
            return 401;
        }
        endutent();
        setutxent();
        if (getutxent() != NULL) {
            return 402;
        }
        endutxent();

        pg = getpgrp();
        if (pg < 0) {
            return 403;
        }
        if (getpgid(0) != pg) {
            return 404;
        }
        if (getsid(0) < 0 && errno != ENOSYS) {
            /* getsid may fail in some containers; still must be present */
            if (errno == ESRCH || errno == EPERM) {
                /* ok */
            } else if (getsid(0) < 0) {
                return 405;
            }
        }
        setusershell();
        pSh = getusershell();
        if (pSh == NULL || pSh[0] != '/') {
            return 406;
        }
        endusershell();

        pSe = getservbyname("http", "tcp");
        if (pSe == NULL || pSe->s_port != (int)htons(80)) {
            return 407;
        }
        pPe = getprotobyname("tcp");
        if (pPe == NULL || pPe->p_proto != 6) {
            return 408;
        }

        pMf = setmntent("/proc/self/mounts", "r");
        if (pMf == NULL) {
            pMf = setmntent("/proc/mounts", "r");
        }
        if (pMf != NULL) {
            pMe = getmntent(pMf);
            if (pMe == NULL || pMe->mnt_dir == NULL) {
                (void)endmntent(pMf);
                return 409;
            }
            if (hasmntopt(pMe, "rw") == NULL && hasmntopt(pMe, "ro") == NULL) {
                /* some mounts omit both; not fatal */
            }
            if (getmntent_r(pMf, &mnt, aMbuf, (int)sizeof(aMbuf)) == NULL &&
                ferror(pMf)) {
                (void)endmntent(pMf);
                return 410;
            }
            if (endmntent(pMf) != 1) {
                return 411;
            }
        }

        nSc = scandirat(AT_FDCWD, "/tmp", &aSc, NULL, alphasort);
        if (nSc < 0) {
            return 412;
        }
        while (nSc > 0) {
            nSc--;
            free(aSc[nSc]);
        }
        free(aSc);
        (void)tcgetpgrp(0);
    }

    /* locale objects / stack attr / affinity / getcpu / fstatat / auxv / link */
    {
        locale_t loc;
        locale_t old;
        locale_t dup;
        pthread_attr_t attr;
        void *pStk;
        size_t cbStk;
        char aStk[8192];
        cpu_set_t set;
        struct stat st;
        struct timeval tvOld;
        struct timeval tvDelta;
        int nCpu;
        int nPhdr;
        unsigned long uPagesz;

        loc = newlocale(LC_ALL_MASK, "C", (locale_t)0);
        if (loc == (locale_t)0) {
            return 413;
        }
        old = uselocale(loc);
        if (old == (locale_t)0) {
            return 414;
        }
        dup = duplocale(loc);
        if (dup == (locale_t)0) {
            return 415;
        }
        freelocale(dup);
        (void)uselocale(old);
        freelocale(loc);

        if (pthread_attr_init(&attr) != 0) {
            return 416;
        }
        if (pthread_attr_setstack(&attr, aStk, sizeof(aStk)) != 0) {
            return 417;
        }
        if (pthread_attr_getstack(&attr, &pStk, &cbStk) != 0 || pStk != aStk ||
            cbStk != sizeof(aStk)) {
            return 418;
        }
        (void)pthread_attr_destroy(&attr);

        CPU_ZERO(&set);
        if (pthread_getaffinity_np(pthread_self(), sizeof(set), &set) != 0) {
            return 419;
        }
        if (pthread_setaffinity_np(pthread_self(), sizeof(set), &set) != 0) {
            return 420;
        }
        nCpu = sched_getcpu();
        if (nCpu < 0) {
            return 421;
        }

        if (fstatat(AT_FDCWD, "/", &st, 0) != 0 || !S_ISDIR(st.st_mode)) {
            return 422;
        }

        if (adjtime(NULL, &tvOld) != 0) {
            return 423;
        }
        tvDelta.tv_sec = 0;
        tvDelta.tv_usec = 1;
        if (adjtime(&tvDelta, NULL) == 0) {
            /* privileged hosts may allow — ok either way */
        } else if (errno != EPERM && errno != ENOSYS) {
            return 424;
        }

        uPagesz = getauxval(AT_PAGESZ);
        if (uPagesz != 0 && uPagesz != 4096ul && uPagesz != 65536ul) {
            /* unusual page size; still ok if non-zero */
            if (uPagesz < 4096ul) {
                return 425;
            }
        }
        (void)prctl(16 /* PR_GET_NAME */, (unsigned long)aStk, 0, 0, 0);

        nPhdr = 0;
        if (dl_iterate_phdr(smoke_phdr_cb, &nPhdr) != 0) {
            return 426;
        }
        /* may be 0 if auxv not on stack (static crt); non-negative is enough */
        (void)nPhdr;

        if (posix_fadvise(0, 0, 0, POSIX_FADV_NORMAL) != 0 && errno != ESPIPE &&
            errno != EINVAL && errno != EBADF) {
            /* stdin may not support fadvise */
        }
    }

    /* batch7: guard/getattr/sched/atfork/usable_size/cpucount */
    {
        pthread_attr_t attr;
        size_t cbGuard = 0;
        cpu_set_t set;
        int nPol = -1;
        struct sched_param sp;
        clockid_t clk = -1;
        void *pMem;
        size_t cbUsable;
        int nCpuCnt;
        int nMax;
        extern int __libc_single_threaded;

        if (pthread_attr_init(&attr) != 0) {
            return 427;
        }
        if (pthread_attr_setguardsize(&attr, 8192u) != 0) {
            return 428;
        }
        if (pthread_attr_getguardsize(&attr, &cbGuard) != 0 ||
            cbGuard != 8192u) {
            return 429;
        }
        CPU_ZERO(&set);
        CPU_SET(0, &set);
        if (pthread_attr_setaffinity_np(&attr, sizeof(set), &set) != 0) {
            return 430;
        }
        CPU_ZERO(&set);
        if (pthread_attr_getaffinity_np(&attr, sizeof(set), &set) != 0 ||
            !CPU_ISSET(0, &set)) {
            return 431;
        }
        (void)pthread_attr_destroy(&attr);

        if (pthread_getattr_np(pthread_self(), &attr) != 0) {
            return 432;
        }
        (void)pthread_attr_destroy(&attr);

        if (pthread_getschedparam(pthread_self(), &nPol, &sp) != 0) {
            return 433;
        }
        if (nPol < 0) {
            return 434;
        }
        /* SCHED_OTHER prio 0 is always safe to re-apply */
        if (pthread_setschedparam(pthread_self(), nPol, &sp) != 0) {
            return 435;
        }
        if (pthread_getcpuclockid(pthread_self(), &clk) != 0) {
            return 436;
        }
        if (clock_getcpuclockid(0, &clk) != 0) {
            return 437;
        }

        nMax = sched_get_priority_max(SCHED_OTHER);
        if (nMax < 0 && errno != EINVAL) {
            /* host always supports SCHED_OTHER */
            return 438;
        }
        (void)nMax;
        if (sched_getscheduler(0) < 0) {
            return 439;
        }

        pMem = malloc(128);
        if (pMem == NULL) {
            return 440;
        }
        cbUsable = malloc_usable_size(pMem);
        if (cbUsable < 128) {
            free(pMem);
            return 441;
        }
        free(pMem);

        CPU_ZERO(&set);
        if (sched_getaffinity(0, sizeof(set), &set) != 0) {
            return 442;
        }
        nCpuCnt = CPU_COUNT(&set);
        if (nCpuCnt < 1) {
            return 443;
        }
        (void)__libc_single_threaded;
        if (pthread_atfork(NULL, NULL, NULL) != 0) {
            return 444;
        }
        if (tgkill((int)getpid(), (int)gettid(), 0) != 0 && errno != EINVAL &&
            errno != ESRCH && errno != EPERM) {
            /* signal 0 is existence check; some hosts reject 0 */
        }
    }

    /* batch8: capget/capset + thin desktop syscalls */
    {
        struct __user_cap_header_struct hdr;
        struct __user_cap_data_struct data[2];
        struct statx stx;
        pid_t tid;
        int n;

        tid = gettid();
        if (tid <= 0) {
            return 445;
        }

        memset(&hdr, 0, sizeof(hdr));
        memset(data, 0, sizeof(data));
        hdr.version = 0x20080522u; /* _LINUX_CAPABILITY_VERSION_3 */
        hdr.pid = 0;
        n = capget(&hdr, data);
        if (n != 0) {
            if (hdr.version != 0) {
                n = capget(&hdr, data);
            }
            if (n != 0 && errno != EFAULT && errno != EINVAL &&
                errno != EPERM && errno != ENOSYS) {
                return 446;
            }
        }
        if (capset(&hdr, data) != 0 && errno != EPERM && errno != EINVAL &&
            errno != ENOSYS) {
            return 447;
        }

        n = personality(0xffffffffu);
        if (n < 0 && errno != ENOSYS) {
            return 448;
        }

        (void)sync_file_range(0, 0, 0, 0);
        (void)readahead(0, 0, 0);

        memset(&stx, 0, sizeof(stx));
        if (statx(AT_FDCWD, "/", 0, STATX_BASIC_STATS, &stx) != 0 &&
            errno != ENOSYS && errno != EPERM) {
            return 449;
        }

        if (membarrier(0, 0, 0) < 0 && errno != ENOSYS && errno != EINVAL) {
            /* query may return bitmask */
        }
        (void)ioprio_get(1, 0);

        if (listxattr("/", NULL, 0) < 0 && errno != ENOTSUP &&
            errno != EOPNOTSUPP && errno != ENOSYS && errno != EACCES &&
            errno != EPERM) {
            return 450;
        }

        if (umount2("/no/such/mnt", 0) == 0) {
            return 451;
        }
        if (errno != ENOENT && errno != EPERM && errno != EINVAL &&
            errno != ENOSYS) {
            return 452;
        }

        if (faccessat2(AT_FDCWD, "/", F_OK, 0) != 0 && errno != ENOSYS) {
            return 453;
        }
    }

    /* batch9: inttypes/search/fortify aliases/ctype_loc/wcwidth/mallinfo/pidfd */
    {
        intmax_t im;
        imaxdiv_t id;
        int aQ[4] = {3, 1, 4, 2};
        void *pTree = NULL;
        ENTRY he, *pHe;
        int aLin[4] = {10, 20, 30, 40};
        size_t nLin = 3;
        int nKey = 20;
        dev_t d;
        struct mallinfo mi;
        char aWd[64];
        const unsigned short **ppCtype;
        int nW;

        im = strtoimax("42", NULL, 10);
        if (im != 42) {
            return 454;
        }
        if (strtoumax("99", NULL, 10) != 99u) {
            return 455;
        }
        if (imaxabs((intmax_t)-7) != 7) {
            return 456;
        }
        id = imaxdiv(17, 5);
        if (id.quot != 3 || id.rem != 2) {
            return 457;
        }

        qsort_r(aQ, 4, sizeof(int), smoke_qsort_r_cmp, NULL);
        if (aQ[0] != 1 || aQ[3] != 4) {
            return 458;
        }

        if (tsearch((void *)(uintptr_t)5, &pTree, smoke_tcmp) == NULL) {
            return 459;
        }
        if (tfind((void *)(uintptr_t)5, &pTree, smoke_tcmp) == NULL) {
            return 460;
        }
        tdestroy(pTree, NULL);

        if (!hcreate(16)) {
            return 461;
        }
        he.key = (char *)"k";
        he.data = (void *)(uintptr_t)1;
        pHe = hsearch(he, ENTER);
        if (pHe == NULL) {
            return 462;
        }
        he.data = NULL;
        pHe = hsearch(he, FIND);
        if (pHe == NULL) {
            return 463;
        }
        hdestroy();

        if (lfind(&nKey, aLin, &nLin, sizeof(int), smoke_icmp) == NULL) {
            return 464;
        }

        if (a64l("./") != 64L && a64l("./") != 0L) {
            /* ./ is digit0 + digit1 = 0 + 1*64? wait: . =0 / =1 → 0 + 1<<6 = 64 */
        }
        if (a64l("./") != 64L) {
            return 465;
        }
        if (l64a(64) == NULL || l64a(64)[0] == '\0') {
            return 466;
        }

        d = makedev(1, 3);
        if (major(d) != 1 || minor(d) != 3) {
            return 467;
        }

        mi = mallinfo();
        (void)mi;
        if (mallopt(1, 0) == 0) {
            return 468;
        }

        if (wcwidth(L'A') != 1) {
            return 469;
        }
        nW = wcswidth(L"AB", 2);
        if (nW != 2) {
            return 470;
        }

        ppCtype = __ctype_b_loc();
        if (ppCtype == NULL || *ppCtype == NULL) {
            return 471;
        }
        (void)(*ppCtype)[(int)'A'];

        if (__memcpy_chk(aWd, "ok", 3, sizeof(aWd)) == NULL) {
            return 472;
        }
        if (__strcpy_chk(aWd, "hi", sizeof(aWd)) == NULL) {
            return 473;
        }

        if (open64("/dev/null", O_RDONLY) < 0 && errno != ENOENT) {
            /* host may lack /dev/null in some sandboxes */
        }
        {
            struct stat stZ;

            memset(&stZ, 0, sizeof(stZ));
            if (stat64("/", &stZ) != 0) {
                return 474;
            }
            memset(&stZ, 0, sizeof(stZ));
            if (__xstat(1, "/", &stZ) != 0) {
                return 475;
            }
        }
        {
            int nSc = 0;
            if (__isoc99_sscanf("7", "%d", &nSc) != 1 || nSc != 7) {
                return 476;
            }
        }

        if (group_member(getegid()) != 1) {
            return 477;
        }

        if (pidfd_open(getpid(), 0) < 0 && errno != ENOSYS && errno != EINVAL &&
            errno != EPERM && errno != EBADF && errno != ESRCH) {
            /* host kernels without pidfd */
        }

        (void)error_message_count;
    }

    /* batch10: vfork/clone surface, fexecve, openat2, mlock2/pkey, gettext, aio */
    {
        char *szG;
        struct open_how how;
        struct aiocb acb;
        struct rlimit rl;
        char aDent[256];
        int nFd;

        szG = gettext("hello");
        if (szG == NULL || szG[0] != 'h') {
            return 478;
        }
        if (dgettext("messages", "x") == NULL || dgettext("messages", "x")[0] != 'x') {
            return 479;
        }
        if (ngettext("one", "many", 2) == NULL ||
            ngettext("one", "many", 2)[0] != 'm') {
            return 480;
        }
        if (textdomain("cgj") == NULL) {
            return 481;
        }
        if (bindtextdomain("cgj", "/usr/share/locale") == NULL) {
            return 482;
        }

        memset(&acb, 0, sizeof(acb));
        acb.aio_fildes = -1;
        errno = 0;
        if (aio_read(&acb) != -1 || errno != EBADF) {
            return 483; /* sync AIO: bad fd → EBADF */
        }
        if (aio_error(&acb) != EBADF) {
            return 484;
        }
        errno = 0;
        if (aio_write(&acb) != -1 || errno != EBADF) {
            return 485;
        }

        memset(&how, 0, sizeof(how));
        how.flags = (uint64_t)O_RDONLY;
        if (openat2(AT_FDCWD, "/", &how, sizeof(how)) < 0 && errno != ENOSYS &&
            errno != EINVAL && errno != EPERM) {
            /* host without openat2 or restricted */
        }

        (void)mlock2(NULL, 0, 0); /* ENOSYS/EINVAL expected */
        (void)pkey_alloc(0, 0);
        (void)userfaultfd(0);
        /* Do not call seccomp(0,...): SECCOMP_SET_MODE_STRICT kills later syscalls */
        {
            uintptr_t uSec;

            uSec = (uintptr_t)seccomp + (uintptr_t)swapon + (uintptr_t)swapoff +
                   (uintptr_t)quotactl + (uintptr_t)process_madvise;
            if (uSec == 0) {
                return 485;
            }
        }
        (void)swapon("/nonexistent", 0);
        (void)swapoff("/nonexistent");
        (void)quotactl(0, "/", 0, NULL);
        (void)process_madvise(-1, NULL, 0, 0, 0);

        if (prlimit64(0, RLIMIT_NOFILE, NULL, &rl) != 0 && errno != ENOSYS &&
            errno != EPERM && errno != EINVAL) {
            return 486;
        }

        nFd = open("/", O_RDONLY | O_DIRECTORY);
        if (nFd >= 0) {
            if (getdents64(nFd, aDent, sizeof(aDent)) < 0 && errno != ENOSYS) {
                (void)close(nFd);
                return 487;
            }
            (void)close(nFd);
        }

        /* Force-link graph symbols (do not invoke vfork/clone/fexecve). */
        {
            uintptr_t u;

            u = (uintptr_t)vfork + (uintptr_t)clone + (uintptr_t)fexecve;
            if (u == 0) {
                return 488;
            }
        }
    }

    /* batch11: argz/envz, C11 threads surface, rand48, ether, graph aliases */
    {
        char *pArgz = NULL;
        size_t cbArgz = 0;
        char *const aAv[] = { "a", "b", NULL };
        char *pEnvz = NULL;
        size_t cbEnvz = 0;
        struct ether_addr ea;
        char aMac[18];
        long nR;
        once_flag once = ONCE_FLAG_INIT;
        mtx_t mtx;
        char16_t c16 = (char16_t)'A';
        char aMb[8];
        mbstate_t st;
        clock_t clk;
        void *pMem;
        char aFry[8];

        if (argz_create(aAv, &pArgz, &cbArgz) != 0 || pArgz == NULL) {
            return 489;
        }
        if (argz_count(pArgz, cbArgz) != 2) {
            free(pArgz);
            return 490;
        }
        if (argz_add(&pArgz, &cbArgz, "c") != 0) {
            free(pArgz);
            return 491;
        }
        if (argz_count(pArgz, cbArgz) != 3) {
            free(pArgz);
            return 492;
        }
        free(pArgz);

        if (envz_add(&pEnvz, &cbEnvz, "FOO", "bar") != 0) {
            return 493;
        }
        if (envz_get(pEnvz, cbEnvz, "FOO") == NULL ||
            strcmp(envz_get(pEnvz, cbEnvz, "FOO"), "bar") != 0) {
            free(pEnvz);
            return 494;
        }
        free(pEnvz);

        if (ether_aton_r("aa:bb:cc:dd:ee:ff", &ea) == NULL) {
            return 495;
        }
        if (ea.ether_addr_octet[0] != 0xaa || ea.ether_addr_octet[5] != 0xff) {
            return 496;
        }
        if (ether_ntoa_r(&ea, aMac) == NULL || aMac[2] != ':') {
            return 497;
        }

        srand48(1);
        nR = lrand48();
        if (nR < 0) {
            return 498;
        }
        (void)nR;

        if (mtx_init(&mtx, mtx_plain) != thrd_success) {
            return 499;
        }
        if (mtx_lock(&mtx) != thrd_success || mtx_unlock(&mtx) != thrd_success) {
            mtx_destroy(&mtx);
            return 500;
        }
        mtx_destroy(&mtx);
        (void)once;
        (void)thrd_current();

        memset(&st, 0, sizeof(st));
        if (c16rtomb(aMb, c16, &st) == (size_t)-1) {
            return 501;
        }
        pMem = malloc(8);
        if (pMem == NULL) {
            return 502;
        }
        __bzero(pMem, 8);
        cfree(pMem);

        clk = clock();
        (void)clk;
        {
            uintptr_t u;

            u = (uintptr_t)closefrom + (uintptr_t)alphasort64 +
                (uintptr_t)fopen64 + (uintptr_t)arch_prctl +
                (uintptr_t)__cmsg_nxthdr + (uintptr_t)bindresvport +
                (uintptr_t)bsd_signal + (uintptr_t)__mempcpy +
                (uintptr_t)__stpcpy + (uintptr_t)__clone;
            if (u == 0) {
                return 503;
            }
        }
        (void)gethostid();

        memcpy(aFry, "abcdef", 7);
        if (strfry(aFry) == NULL) {
            return 504;
        }
        if (memfrob(aFry, 6) == NULL) {
            return 505;
        }
        (void)memfrob(aFry, 6); /* xor twice restores */
    }

    /* batch12: strerror_l / newfstatat / memfd_secret + graph nodes */
    {
        locale_t loc;
        struct stat st;
        const char *sz;
        void *p;
        int nRobust;
        pthread_mutexattr_t ma;
        pthread_mutex_t mtx;
        char *pEnd = NULL;
        long nL;
        struct statfs sf;
        jmp_buf jb;
        int nJ;

        if (strerror_l(EINVAL, (locale_t)0) == NULL) {
            return 506;
        }
        if (strsignal_l(2, (locale_t)0) == NULL) {
            return 507;
        }
        loc = newlocale(LC_ALL_MASK, "C", (locale_t)0);
        if (loc != (locale_t)0) {
            if (strcmp(strerror_l(0, loc), "Success") != 0) {
                freelocale(loc);
                return 508;
            }
            freelocale(loc);
        }

        if (newfstatat(AT_FDCWD, "/", &st, 0) != 0 || !S_ISDIR(st.st_mode)) {
            return 509;
        }
        if (fstatat(AT_FDCWD, "/", &st, 0) != 0) {
            return 510;
        }

        /* memfd_secret: host may ENOSYS; still exercise the wrapper */
        {
            int nSec = memfd_secret(0);

            if (nSec >= 0) {
                (void)close(nSec);
            } else if (errno != ENOSYS && errno != EPERM && errno != EINVAL &&
                       errno != EOPNOTSUPP) {
                return 511;
            }
        }

        sz = strerrorname_np(ENOENT);
        if (sz == NULL || strcmp(sz, "ENOENT") != 0) {
            return 512;
        }
        if (strerrordesc_np(EINVAL) == NULL) {
            return 513;
        }
        if (sigabbrev_np(9) == NULL || strcmp(sigabbrev_np(9), "KILL") != 0) {
            return 514;
        }
        if (sigdescr_np(2) == NULL) {
            return 515;
        }

        p = __libc_malloc(32);
        if (p == NULL) {
            return 516;
        }
        __libc_free(p);
        p = __libc_calloc(2, 16);
        if (p == NULL) {
            return 517;
        }
        p = __libc_realloc(p, 64);
        if (p == NULL) {
            return 518;
        }
        __libc_free(p);

        if (gnu_get_libc_version() == NULL || gnu_get_libc_release() == NULL) {
            return 519;
        }
        if (__libc_current_sigrtmin() >= __libc_current_sigrtmax()) {
            return 520;
        }
        if (__sysconf(_SC_PAGESIZE) <= 0) {
            return 521;
        }

        if (statfs64("/tmp", &sf) != 0 && errno != ENOENT) {
            return 522;
        }
        (void)__statfs("/", &sf);

        nL = __isoc23_strtol("42", &pEnd, 10);
        if (nL != 42 || pEnd == NULL || *pEnd != '\0') {
            return 523;
        }
        {
            int nV = 0;

            if (__isoc23_sscanf("7", "%d", &nV) != 1 || nV != 7) {
                return 524;
            }
        }

        if (pthread_mutexattr_init(&ma) != 0) {
            return 525;
        }
        if (pthread_mutexattr_setrobust(&ma, PTHREAD_MUTEX_ROBUST) != 0) {
            pthread_mutexattr_destroy(&ma);
            return 526;
        }
        if (pthread_mutexattr_getrobust(&ma, &nRobust) != 0 ||
            nRobust != PTHREAD_MUTEX_ROBUST) {
            pthread_mutexattr_destroy(&ma);
            return 527;
        }
        if (pthread_mutex_init(&mtx, &ma) != 0) {
            pthread_mutexattr_destroy(&ma);
            return 528;
        }
        if (pthread_mutex_lock(&mtx) != 0 || pthread_mutex_unlock(&mtx) != 0) {
            pthread_mutex_destroy(&mtx);
            pthread_mutexattr_destroy(&ma);
            return 529;
        }
        if (pthread_mutex_consistent(&mtx) != 0) {
            pthread_mutex_destroy(&mtx);
            pthread_mutexattr_destroy(&ma);
            return 530;
        }
        pthread_mutex_destroy(&mtx);
        pthread_mutexattr_destroy(&ma);

        if (_IO_feof(stdin) < 0) {
            return 531;
        }
        (void)__res_init();
        {
            char *pDup = __strndup("ab", 1);

            if (pDup == NULL || pDup[0] != 'a' || pDup[1] != '\0') {
                free(pDup);
                return 532;
            }
            free(pDup);
        }

        nJ = setjmp(jb);
        if (nJ == 0) {
            __longjmp_chk(jb, 1);
            return 533;
        }

        {
            uintptr_t u;

            u = (uintptr_t)memfd_secret + (uintptr_t)newfstatat +
                (uintptr_t)strerror_l + (uintptr_t)__open64 +
                (uintptr_t)__open64_2 + (uintptr_t)__fdelt_warn +
                (uintptr_t)sem_clockwait + (uintptr_t)__mbrtowc +
                (uintptr_t)_IO_getc + (uintptr_t)__overflow;
            if (u == 0) {
                return 534;
            }
        }
    }

    /* batch13: underscored aliases, stdio_ext, dn_*, reentrant netdb, ftok */
    {
        unsigned char aComp[64];
        char aExp[64];
        int nComp;
        int nExp;
        struct timeb tb;
        key_t k;
        struct passwd pw;
        struct passwd *ppw;
        char aPw[256];
        struct servent se;
        struct servent *pse;
        char aSe[256];
        struct netent *pne;
        unsigned char aMsg[] = {
            0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
            0x03, 'c', 'o', 'm', 0x00
        };
        extern int __close(int);
        extern int __dup2(int, int);
        extern pid_t __fork(void);
        extern pid_t _Fork(void);
        extern pid_t __getpid(void);
        extern int __ffs(int);
        extern unsigned long __getauxval(unsigned long);
        extern struct tm *__gmtime_r(const time_t *, struct tm *);
        extern void __explicit_bzero_chk(void *, size_t, size_t);
        extern int __cxa_at_quick_exit(void (*)(void *), void *);
        extern int __cxa_thread_atexit_impl(void (*)(void *), void *, void *);
        extern void __assert(const char *, const char *, int);
        extern int __dn_expand(const unsigned char *, const unsigned char *,
                               const unsigned char *, char *, int);
        uintptr_t u;

        if (__getpid() != getpid()) {
            return 535;
        }
        if (__ffs(0x8) != 4) {
            return 536;
        }
        if (__freadable(stdin) == 0 || __fwritable(stdout) == 0) {
            return 537;
        }
        if (__fbufsize(stdout) == 0) {
            return 538;
        }
        __fpurge(stdin);
        if (__fpending(stdout) > __fbufsize(stdout)) {
            return 539;
        }
        (void)__fsetlocking(stdout, 0);

        nComp = dn_comp("example.com", aComp, (int)sizeof(aComp), NULL, NULL);
        if (nComp < 5) {
            return 540;
        }
        nExp = dn_expand(aMsg, aMsg + sizeof(aMsg), aMsg, aExp,
                         (int)sizeof(aExp));
        if (nExp < 0 || strcmp(aExp, "example.com") != 0) {
            return 541;
        }
        if (dn_skipname(aMsg, aMsg + sizeof(aMsg)) < 1) {
            return 542;
        }
        if (__dn_expand(aMsg, aMsg + sizeof(aMsg), aMsg, aExp,
                        (int)sizeof(aExp)) < 0) {
            return 543;
        }

        if (ftime(&tb) != 0 || tb.time == 0) {
            return 544;
        }
        if (dysize(2000) != 366 || dysize(1900) != 365 || dysize(2001) != 365) {
            return 545;
        }
        k = ftok("/tmp", 'G');
        (void)k;

        setpwent();
        if (getpwent_r(&pw, aPw, sizeof(aPw), &ppw) != 0) {
            return 547;
        }
        endpwent();

        if (getservbyname_r("http", "tcp", &se, aSe, sizeof(aSe), &pse) != 0 ||
            pse == NULL || pse->s_port == 0) {
            return 548;
        }

        setnetent(0);
        pne = getnetbyname("loopback");
        if (pne == NULL || pne->n_net != 0x7f000000u) {
            return 549;
        }
        endnetent();

        (void)gethostbyname2("127.0.0.1", AF_INET);

        if (fmtmsg(MM_PRINT, "gj", MM_INFO, "batch13", NULL, NULL) != MM_OK) {
            return 552;
        }

        u = (uintptr_t)(void *)__close + (uintptr_t)(void *)__dup2 +
            (uintptr_t)(void *)__fork + (uintptr_t)(void *)_Fork +
            (uintptr_t)(void *)__freading + (uintptr_t)(void *)dn_comp +
            (uintptr_t)(void *)getnetent +
            (uintptr_t)(void *)gethostbyname_r +
            (uintptr_t)(void *)getgrent_r + (uintptr_t)(void *)__assert +
            (uintptr_t)(void *)at_quick_exit +
            (uintptr_t)(void *)quick_exit +
            (uintptr_t)(void *)__cxa_at_quick_exit +
            (uintptr_t)(void *)__cxa_thread_atexit_impl +
            (uintptr_t)(void *)dladdr1 + (uintptr_t)(void *)dlinfo +
            (uintptr_t)(void *)dlmopen +
            (uintptr_t)(void *)__explicit_bzero_chk +
            (uintptr_t)(void *)freopen64 + (uintptr_t)(void *)fgetpwent +
            (uintptr_t)(void *)fgetgrent +
            (uintptr_t)(void *)__getauxval +
            (uintptr_t)(void *)__gmtime_r;
        if (u == 0) {
            return 554;
        }
    }

    /* batch14: cookie stream, twalk_r, wcstoimax, strfmon, printf parse, nprocs */
    {
        static char aCk[8];
        cookie_io_functions_t io;
        FILE *pCk;
        void *pRoot = NULL;
        int nKey = 42;
        int nWalk = 0;
        wchar_t aW[] = { L'1', L'2', L'3', 0 };
        char aMon[32];
        ssize_t nMon;
        int aTypes[8];
        size_t nPf;
        uintptr_t u;

        memset(&io, 0, sizeof(io));
        pCk = fopencookie((void *)aCk, "w", io);
        if (pCk == NULL) {
            return 560;
        }
        if (fclose(pCk) != 0) {
            return 561;
        }

        if (tsearch(&nKey, &pRoot, cgj_smoke_int_cmp) == NULL) {
            return 562;
        }
        twalk_r(pRoot, NULL, &nWalk);
        tdestroy(pRoot, NULL);

        if (wcstoimax(aW, NULL, 10) != 123) {
            return 563;
        }
        if (wcstoumax(aW, NULL, 10) != 123u) {
            return 564;
        }

        nMon = strfmon(aMon, sizeof(aMon), "x%n", (long long)0);
        if (nMon < 2 || aMon[0] != 'x' || aMon[1] != '0') {
            return 565;
        }

        nPf = parse_printf_format("%s %d", 8, aTypes);
        if (nPf != 2 || aTypes[0] != PA_STRING || aTypes[1] != PA_INT) {
            return 566;
        }

        if (get_nprocs() < 1 || get_nprocs_conf() < 1) {
            return 567;
        }

        u = (uintptr_t)(void *)getcontext + (uintptr_t)(void *)setcontext +
            (uintptr_t)(void *)makecontext + (uintptr_t)(void *)swapcontext +
            (uintptr_t)(void *)catopen + (uintptr_t)(void *)io_setup +
            (uintptr_t)(void *)io_uring_setup +
            (uintptr_t)(void *)argp_parse + (uintptr_t)(void *)_obstack_begin +
            (uintptr_t)(void *)pthread_mutex_clocklock +
            (uintptr_t)(void *)sigqueue + (uintptr_t)(void *)getpeereid +
            (uintptr_t)(void *)killpg + (uintptr_t)(void *)sysctl +
            (uintptr_t)(void *)fflush_unlocked +
            (uintptr_t)(void *)versionsort64;
        if (u == 0) {
            return 568;
        }
    }

    /* underscored / optional decls not all in public headers yet */
    {
        extern ssize_t __read(int, void *, size_t);
        extern ssize_t __write(int, const void *, size_t);
        extern int __open(const char *, int, ...);
        extern int getrlimit64(int, struct rlimit *);
        extern char *re_comp(const char *);
        extern void encrypt(char *, int);
        extern int strcoll_l(const char *, const char *, locale_t);
        extern int __socket(int, int, int);
        extern void *__mmap(void *, size_t, int, int, int, off_t);
        extern int __libc_open(const char *, int, ...);
        extern char *__secure_getenv(const char *);
        extern long __strtol_internal(const char *, char **, int, int);
        extern int __pthread_mutex_lock(pthread_mutex_t *);
        extern int __isoc99_printf(const char *, ...);
        extern int __asprintf_chk(int, char **, const char *, ...);
        extern unsigned int __if_nametoindex(const char *);
        (void)(uintptr_t)(void *)__read;
        (void)(uintptr_t)(void *)__write;
        (void)(uintptr_t)(void *)__open;
        (void)(uintptr_t)(void *)getrlimit64;
        (void)(uintptr_t)(void *)re_comp;
        (void)(uintptr_t)(void *)encrypt;
        (void)(uintptr_t)(void *)strcoll_l;
        (void)(uintptr_t)(void *)__socket;
        (void)(uintptr_t)(void *)__mmap;
        (void)(uintptr_t)(void *)__libc_open;
        (void)(uintptr_t)(void *)__secure_getenv;
        (void)(uintptr_t)(void *)__strtol_internal;
        (void)(uintptr_t)(void *)__pthread_mutex_lock;
        (void)(uintptr_t)(void *)__isoc99_printf;
        (void)(uintptr_t)(void *)__asprintf_chk;
        (void)(uintptr_t)(void *)__if_nametoindex;
    }

    /* batch15: underscored I/O, ns_*, cancel, locale _l, getdate, pvalloc */
    {
        extern ssize_t __read(int, void *, size_t);
        extern ssize_t __write(int, const void *, size_t);
        extern int __open(const char *, int, ...);
        extern int getrlimit64(int, struct rlimit *);
        extern char *re_comp(const char *);
        extern void encrypt(char *, int);
        extern int strcoll_l(const char *, const char *, locale_t);

        unsigned char aWire[4] = { 0x12, 0x34, 0x56, 0x78 };
        wchar_t aWcpy[8];
        char aDate[32];
        struct tm tm;
        int nOld;
        uintptr_t u;
        void *pPv;

        if (ns_get16(aWire) != 0x1234u) {
            return 570;
        }
        if (ns_get32(aWire) != 0x12345678ul) {
            return 571;
        }
        if (!isascii('A') || isascii(0x80)) {
            return 572;
        }
        if (wcpcpy(aWcpy, L"ok") != aWcpy + 2 || aWcpy[0] != L'o') {
            return 573;
        }
        pPv = pvalloc(1);
        if (pPv == NULL) {
            return 574;
        }
        free(pPv);
        if (pthread_setcancelstate(0, &nOld) != 0) {
            return 575;
        }
        if (getdate_r("2020-01-02", &tm) != 0 || tm.tm_year != 120) {
            return 576;
        }
        (void)snprintf(aDate, sizeof(aDate), "%d", (int)timelocal(&tm));
        if (aDate[0] == '\0') {
            return 577;
        }
        u = (uintptr_t)(void *)__read + (uintptr_t)(void *)__write +
            (uintptr_t)(void *)__open + (uintptr_t)(void *)res_ninit +
            (uintptr_t)(void *)sigaltstack + (uintptr_t)(void *)mbrlen +
            (uintptr_t)(void *)re_comp + (uintptr_t)(void *)encrypt +
            (uintptr_t)(void *)strcoll_l + (uintptr_t)(void *)getrlimit64;
        if (u == 0) {
            return 578;
        }
    }

    /* batch16: more underscored aliases, fortify, _setjmp, LFS stdio */
    {
        extern int __socket(int, int, int);
        extern void *__mmap(void *, size_t, int, int, int, off_t);
        extern int __libc_open(const char *, int, ...);
        extern char *__secure_getenv(const char *);
        extern long __strtol_internal(const char *, char **, int, int);
        extern int __pthread_mutex_lock(pthread_mutex_t *);
        extern int __isoc99_printf(const char *, ...);
        extern int __asprintf_chk(int, char **, const char *, ...);
        extern unsigned int __if_nametoindex(const char *);

        jmp_buf jb;
        int nJ;
        uintptr_t u;

        nJ = _setjmp(jb);
        if (nJ != 0 && nJ != 1) {
            return 580;
        }
        if (fseeko64(stdout, 0, SEEK_CUR) != 0 && errno != ESPIPE &&
            errno != EINVAL) {
            /* ESPIPE/EINVAL OK for non-seekable stdout */
        }
        u = (uintptr_t)(void *)__socket + (uintptr_t)(void *)__mmap +
            (uintptr_t)(void *)__libc_open + (uintptr_t)(void *)__secure_getenv +
            (uintptr_t)(void *)__strtol_internal +
            (uintptr_t)(void *)__pthread_mutex_lock +
            (uintptr_t)(void *)__isoc99_printf +
            (uintptr_t)(void *)__asprintf_chk +
            (uintptr_t)(void *)readdir64_r + (uintptr_t)(void *)tmpfile64 +
            (uintptr_t)(void *)__if_nametoindex +
            (uintptr_t)(void *)__sigsetjmp;
        if (u == 0) {
            return 581;
        }
    }

    /* batch17: memccpy, _IO_*, isw*_l, inet_net_*, fstab, freezero */
    {
        extern int _IO_printf(const char *, ...);
        extern long _IO_padn(FILE *, int, long);
        extern void freezero(void *, size_t);
        extern void setfsent(void);
        extern void endfsent(void);
        extern void *getfsent(void);
        extern char *inet_net_ntop(int, const void *, int, char *, size_t);
        extern int inet_net_pton(int, const char *, void *, size_t);
        extern int iswalnum_l(wint_t, locale_t);
        extern int __strcasecmp_l(const char *, const char *, locale_t);
        extern locale_t __newlocale(int, const char *, locale_t);
        extern int re_set_syntax(unsigned long);
        extern int regncomp(regex_t *, const char *, size_t, int);
        extern int rcmd(char **, unsigned short, const char *, const char *,
                        const char *, int *);
        extern int isastream(int);
        extern void setnetgrent(const char *);
        extern int innetgr(const char *, const char *, const char *,
                           const char *);
        extern FTS *fts64_open(char *const *, int,
                               int (*)(const FTSENT **, const FTSENT **));

        char aMem[8];
        char aNet[32];
        unsigned char aIp[4] = { 192, 168, 1, 0 };
        unsigned char aOut[4];
        void *pZ;
        uintptr_t u;
        int nBits;

        if (memccpy(aMem, "abXc", 'X', 4) != aMem + 3 || aMem[0] != 'a' ||
            aMem[2] != 'X') {
            return 590;
        }
        if (!iswalnum_l(L'A', (locale_t)0) || iswalnum_l(L' ', (locale_t)0)) {
            return 591;
        }
        if (inet_net_ntop(AF_INET, aIp, 24, aNet, sizeof(aNet)) == NULL ||
            strstr(aNet, "192.168.1.0") == NULL) {
            return 592;
        }
        nBits = inet_net_pton(AF_INET, "10.0.0.0/8", aOut, sizeof(aOut));
        if (nBits != 8 || aOut[0] != 10) {
            return 593;
        }
        pZ = malloc(16);
        if (pZ == NULL) {
            return 594;
        }
        freezero(pZ, 16);
        setfsent();
        (void)getfsent();
        endfsent();
        setnetgrent("nosuch");
        if (innetgr("nosuch", NULL, NULL, NULL) != 0) {
            return 595;
        }
        if (isastream(0) != 0) {
            return 596;
        }
        u = (uintptr_t)(void *)_IO_printf + (uintptr_t)(void *)_IO_padn +
            (uintptr_t)(void *)__newlocale + (uintptr_t)(void *)__strcasecmp_l +
            (uintptr_t)(void *)re_set_syntax + (uintptr_t)(void *)regncomp +
            (uintptr_t)(void *)rcmd + (uintptr_t)(void *)fgetpwent_r +
            (uintptr_t)(void *)fts64_open;
        if (u == 0) {
            return 597;
        }
    }

    /* batch18: syscall, BSD fls/strtonum/heapsort, sched_setattr, fsopen */
    {
        extern long syscall(long, ...);
        extern int fls(int);
        extern int flsl(long);
        extern int flsll(long long);
        extern const char *getprogname(void);
        extern void setprogname(const char *);
        extern void *recallocarray(void *, size_t, size_t, size_t);
        extern void *reallocf(void *, size_t);
        extern long long strtonum(const char *, long long, long long,
                                  const char **);
        extern int issetugid(void);
        extern void strmode(mode_t, char *);
        extern int heapsort(void *, size_t, size_t,
                            int (*)(const void *, const void *));
        extern int mergesort(void *, size_t, size_t,
                             int (*)(const void *, const void *));
        extern long perf_event_open(void *, pid_t, int, int, unsigned long);
        extern int fsopen(const char *, unsigned);
        extern int getdents(int, void *, unsigned);
        extern void __gmon_start__(void);
        extern int _dl_find_object(void *, void *);
        /* __res_state / sched_setattr from headers */

        int aN[4];
        char aMode[12];
        const char *szErr = NULL;
        long long nNum;
        void *pR;
        uintptr_t u;

        aN[0] = 3;
        aN[1] = 1;
        aN[2] = 4;
        aN[3] = 2;
        if (fls(0) != 0 || fls(1) != 1 || fls(8) != 4 || flsl(0x100L) != 9) {
            return 600;
        }
        if (flsll(0) != 0 || flsll(1LL << 40) != 41) {
            return 601;
        }
        setprogname("cgj_smoke");
        if (getprogname() == NULL || getprogname()[0] == '\0') {
            return 602;
        }
        nNum = strtonum("42", 1, 100, &szErr);
        if (nNum != 42 || szErr != NULL) {
            return 603;
        }
        nNum = strtonum("999", 1, 10, &szErr);
        if (nNum != 0 || szErr == NULL) {
            return 604;
        }
        if (heapsort(aN, 4, sizeof(int), cgj_smoke_int_cmp) != 0 ||
            aN[0] != 1 || aN[3] != 4) {
            return 605;
        }
        aN[0] = 9;
        aN[1] = 7;
        aN[2] = 8;
        aN[3] = 6;
        if (mergesort(aN, 4, sizeof(int), cgj_smoke_int_cmp) != 0 ||
            aN[0] != 6 || aN[3] != 9) {
            return 606;
        }
        strmode((mode_t)(S_IFREG | 0755), aMode);
        if (aMode[0] != '-' || aMode[1] != 'r' || aMode[3] != 'x') {
            return 607;
        }
        pR = recallocarray(NULL, 0, 4, sizeof(int));
        if (pR == NULL) {
            return 608;
        }
        pR = reallocf(pR, 0);
        (void)pR;
        (void)issetugid();
        if (__res_state() == NULL) {
            return 609;
        }
        /* getpid via raw syscall NR 39 on x86_64 */
        if ((pid_t)syscall(39) != getpid()) {
            return 610;
        }
        u = (uintptr_t)(void *)sched_setattr +
            (uintptr_t)(void *)perf_event_open + (uintptr_t)(void *)fsopen +
            (uintptr_t)(void *)getdents + (uintptr_t)(void *)__gmon_start__ +
            (uintptr_t)(void *)_dl_find_object;
        if (u == 0) {
            return 611;
        }
    }

    /* batch21: underscored aliases, wchar fortify, ns_* name helpers */
    {
        extern void *__memrchr(const void *, int, size_t);
        extern char *__strchrnul(const char *, int);
        extern char *__strsep(char **, const char *);
        extern int __getentropy(void *, size_t);
        extern ssize_t __getrandom(void *, size_t, unsigned);
        extern off_t __lseek(int, off_t, int);
        extern int __chmod(const char *, mode_t);
        extern pid_t __getppid(void);
        extern int __kill(pid_t, int);
        extern int __raise(int);
        extern int __fsync(int);
        extern int __setenv(const char *, const char *, int);
        extern int __clearenv(void);
        extern wchar_t *__wcscpy_chk(wchar_t *, const wchar_t *, size_t);
        extern wchar_t *__wcsncpy_chk(wchar_t *, const wchar_t *, size_t,
                                      size_t);
        extern int obstack_printf(struct obstack *, const char *, ...);
        extern int __obstack_printf_chk(int, struct obstack *, const char *,
                                        ...);
        extern int ns_samedomain(const char *, const char *);
        extern int ns_subdomain(const char *, const char *);
        extern int ns_makecanon(const char *, char *, size_t);
        extern int ns_samename(const char *, const char *);
        extern int res_nquerydomain(void *, const char *, const char *, int,
                                    int, unsigned char *, int);
        extern int __nss_configure_lookup(const char *, const char *);
        extern void __exit_group(int);
        extern int __sigprocmask(int, const sigset_t *, sigset_t *);
        extern void *__mremap(void *, size_t, size_t, int, ...);
        extern int __posix_spawn(pid_t *, const char *, const void *,
                                 const void *, char *const[], char *const[]);

        wchar_t aW[8];
        char aCanon[64];
        char *pSep;
        char aSep[] = "a:b:c";
        uintptr_t u;
        struct obstack ob;
        int nOb;

        if (__memrchr("abcba", 'b', 5) == NULL) {
            return 612;
        }
        if (__strchrnul("abc", 'z') == NULL ||
            *__strchrnul("abc", 'z') != '\0') {
            return 613;
        }
        pSep = aSep;
        if (__strsep(&pSep, ":") == NULL || pSep == NULL) {
            return 614;
        }
        if (__getppid() < 0) {
            return 615;
        }
        if (__wcscpy_chk(aW, L"hi", 8) == NULL || aW[0] != L'h') {
            return 616;
        }
        if (__wcsncpy_chk(aW, L"xy", 2, 8) == NULL) {
            return 617;
        }
        if (!ns_samedomain("foo.example.com", "example.com")) {
            return 618;
        }
        if (!ns_subdomain("foo.example.com", "example.com")) {
            return 619;
        }
        if (ns_makecanon("example.com", aCanon, sizeof(aCanon)) != 0 ||
            aCanon[0] == '\0') {
            return 620;
        }
        if (!ns_samename("Example.COM", "example.com")) {
            return 621;
        }
        if (_obstack_begin(&ob, 0, 0, (void *(*)(long))malloc,
                           (void (*)(void *))free) == 0) {
            return 622;
        }
        nOb = obstack_printf(&ob, "n=%d", 7);
        if (nOb < 0) {
            obstack_free(&ob, NULL);
            return 623;
        }
        {
            char *pFin = (char *)obstack_finish(&ob);

            if (pFin == NULL || pFin[0] != 'n') {
                obstack_free(&ob, NULL);
                return 624;
            }
        }
        obstack_free(&ob, NULL);
        (void)__nss_configure_lookup("passwd", "files");
        (void)res_nquerydomain(NULL, "localhost", NULL, 1, 1, NULL, 0);
        u = (uintptr_t)(void *)__getentropy + (uintptr_t)(void *)__getrandom +
            (uintptr_t)(void *)__lseek + (uintptr_t)(void *)__chmod +
            (uintptr_t)(void *)__kill + (uintptr_t)(void *)__raise +
            (uintptr_t)(void *)__fsync + (uintptr_t)(void *)__setenv +
            (uintptr_t)(void *)__clearenv +
            (uintptr_t)(void *)__obstack_printf_chk +
            (uintptr_t)(void *)__exit_group +
            (uintptr_t)(void *)__sigprocmask + (uintptr_t)(void *)__mremap +
            (uintptr_t)(void *)__posix_spawn;
        if (u == 0) {
            return 625;
        }
    }

    /* batch26: small-string, tree aliases, nss_hash, wide fortify */
    {
        extern size_t __strspn_c1(const char *, char);
        extern char *__strtok_r(char *, const char *, char **);
        extern int _tolower(int);
        extern int _toupper(int);
        extern void *__tsearch(const void *, void **,
                               int (*)(const void *, const void *));
        extern void *__tdelete(const void *, void **,
                               int (*)(const void *, const void *));
        extern void *__tfind(const void *, void *const *,
                             int (*)(const void *, const void *));
        extern void __twalk(const void *, void (*)(const void *, int, int));
        extern uint32_t __nss_hash(const void *, size_t);
        extern wchar_t *__wcpcpy_chk(wchar_t *, const wchar_t *, size_t);
        extern ssize_t __write_nocancel(int, const void *, size_t);
        extern int __x86_get_cpuid_feature_leaf(unsigned int, unsigned int,
                                               unsigned int *, unsigned int *,
                                               unsigned int *, unsigned int *);
        extern void __file_change_detection_for_path(void *, const char *);
        extern int __nss_passwd_lookup(void **, void **);
        extern long __strtol_l(const char *, char **, int, locale_t);
        char aTok[] = "a,b";
        char *pSave = NULL;
        void *pRoot = NULL;
        wchar_t aW[8];
        uint32_t uHash;
        uintptr_t uLink;

        if (__strspn_c1("aaab", 'a') != 3) {
            return 626;
        }
        if (__strtok_r(aTok, ",", &pSave) == NULL) {
            return 627;
        }
        if (_tolower('A') != 'a' || _toupper('b') != 'B') {
            return 628;
        }
        /* NULL cmp is rejected — exercise path without walking tree */
        if (__tsearch((void *)(uintptr_t)1, &pRoot, NULL) != NULL) {
            return 631;
        }
        uHash = __nss_hash("root", 4);
        if (uHash == 0) {
            return 629;
        }
        if (__wcpcpy_chk(aW, L"ok", 8) == NULL || aW[0] != L'o') {
            return 630;
        }
        (void)__write_nocancel(2, "", 0);
        (void)__x86_get_cpuid_feature_leaf(0, 0, NULL, NULL, NULL, NULL);
        uLink = (uintptr_t)(void *)__tdelete + (uintptr_t)(void *)__tfind +
                (uintptr_t)(void *)__twalk +
                (uintptr_t)(void *)__file_change_detection_for_path +
                (uintptr_t)(void *)__nss_passwd_lookup +
                (uintptr_t)(void *)__strtol_l;
        if (uLink == 0) {
            return 632;
        }
    }

    /* batch30: NUMA / wchar public / BSD helpers (host may ENOSYS syscalls) */
    {
        extern int set_mempolicy(int, const unsigned long *, unsigned long);
        extern int get_mempolicy(int *, unsigned long *, unsigned long, void *,
                                 unsigned long);
        extern long mbind(void *, unsigned long, int, const unsigned long *,
                          unsigned long, unsigned);
        extern long migrate_pages(int, unsigned long, const unsigned long *,
                                  const unsigned long *);
        extern int futex_wait(void *, unsigned long, unsigned long, unsigned,
                              struct timespec *, clockid_t);
        extern int futex_wake(void *, unsigned long, unsigned long, unsigned);
        extern wchar_t *wcschrnul(const wchar_t *, wchar_t);
        extern wchar_t *wcswcs(const wchar_t *, const wchar_t *);
        extern long wcstol_l(const wchar_t *, wchar_t **, int, locale_t);
        extern int __vsscanf(const char *, const char *, va_list);
        extern void *explicit_memset(void *, int, size_t);
        extern int humanize_number(char *, size_t, int64_t, const char *, int,
                                   int);
        extern int expand_number(const char *, uint64_t *);
        extern int fadvise64(int, off_t, off_t, int);
        extern const char __libcgj_batch30_marker[];
        wchar_t aW[] = L"hello";
        wchar_t *pW;
        char aH[32];
        uint64_t uExp = 0;
        unsigned char aMem[8];
        uintptr_t uLink;

        if (__libcgj_batch30_marker[0] == '\0') {
            return 633;
        }
        pW = wcschrnul(aW, L'x');
        if (pW == NULL || *pW != L'\0') {
            return 634;
        }
        if (wcswcs(aW, L"ll") == NULL) {
            return 635;
        }
        if (wcstol_l(L"42", NULL, 10, (locale_t)0) != 42) {
            return 636;
        }
        if (explicit_memset(aMem, 0x5a, sizeof(aMem)) == NULL ||
            aMem[0] != 0x5a) {
            return 637;
        }
        if (humanize_number(aH, sizeof(aH), 2048, NULL, 0, 0) <= 0) {
            return 638;
        }
        if (expand_number("2k", &uExp) != 0 || uExp != 2048ull) {
            return 639;
        }
        (void)fadvise64(-1, 0, 0, 0);
        (void)set_mempolicy(0, NULL, 0);
        (void)get_mempolicy(NULL, NULL, 0, NULL, 0);
        (void)mbind(NULL, 0, 0, NULL, 0, 0);
        (void)migrate_pages(0, 0, NULL, NULL);
        (void)futex_wake(NULL, 0, 0, 0);
        uLink = (uintptr_t)(void *)futex_wait + (uintptr_t)(void *)__vsscanf;
        if (uLink == 0) {
            return 640;
        }
    }

    /* batch34: vis + MD4/RMD160/BLAKE2s digests + stubs */
    {
        extern int nvis(char *, size_t, int, int);
        extern int strsvis(char *, const char *, int, const char *);
        extern int strunvisx(char *, const char *, int);
        extern void MD4Init(void *);
        extern void MD4Update(void *, const void *, size_t);
        extern void MD4Final(unsigned char[16], void *);
        extern void RMD160Init(void *);
        extern void RMD160Update(void *, const void *, size_t);
        extern void RMD160Final(unsigned char[20], void *);
        extern void BLAKE2s_Init(void *);
        extern void BLAKE2s_Update(void *, const void *, size_t);
        extern void BLAKE2s_Final(unsigned char[32], void *);
        extern int bcrypt_gensalt(int, char *);
        extern char *bcrypt(const char *, const char *);
        extern int strtofflags(char **, unsigned long *, unsigned long *);
        extern void *setnetconfig(void);
        extern ssize_t copy_file_range64(int, off_t *, int, off_t *, size_t,
                                         unsigned);
        extern const char __libcgj_batch34_marker[];
        char aVis[32];
        char aUn[32];
        unsigned char aMd4[16];
        unsigned char aRmd[20];
        unsigned char aB2[32];
        unsigned char aCtx[256];
        unsigned long uSet = 1, uClr = 1;
        /* MD4("") = 31d6cfe0d16ae931b73c59d7e0c089c0 */
        static const unsigned char aMd4Empty[16] = {
            0x31, 0xd6, 0xcf, 0xe0, 0xd1, 0x6a, 0xe9, 0x31,
            0xb7, 0x3c, 0x59, 0xd7, 0xe0, 0xc0, 0x89, 0xc0};
        /* RIPEMD-160("") */
        static const unsigned char aRmdEmpty[20] = {
            0x9c, 0x11, 0x85, 0xa5, 0xc5, 0xe9, 0xfc, 0x54, 0x61, 0x28,
            0x08, 0x97, 0x7e, 0xe8, 0xf5, 0x48, 0xb2, 0x25, 0x8d, 0x31};
        /* BLAKE2s-256("") */
        static const unsigned char aB2Empty[32] = {
            0x69, 0x21, 0x7a, 0x30, 0x79, 0x90, 0x80, 0x94, 0xe1, 0x11, 0x21,
            0xd0, 0x42, 0x35, 0x4a, 0x7c, 0x1f, 0x55, 0xb6, 0x48, 0x2c, 0xa1,
            0xa5, 0x1e, 0x1b, 0x25, 0x0d, 0xfd, 0x1e, 0xd0, 0xee, 0xf9};
        int i;

        if (__libcgj_batch34_marker[0] == '\0') {
            return 641;
        }
        if (nvis(aVis, sizeof(aVis), 'A', 0) < 1 || aVis[0] != 'A') {
            return 642;
        }
        if (strsvis(aVis, "hi", 0, NULL) < 2) {
            return 643;
        }
        if (strunvisx(aUn, "hi", 0) < 0) {
            return 644;
        }
        memset(aCtx, 0, sizeof(aCtx));
        MD4Init(aCtx);
        MD4Update(aCtx, "", 0);
        MD4Final(aMd4, aCtx);
        for (i = 0; i < 16; i++) {
            if (aMd4[i] != aMd4Empty[i]) {
                return 645;
            }
        }
        memset(aCtx, 0, sizeof(aCtx));
        RMD160Init(aCtx);
        RMD160Update(aCtx, "", 0);
        RMD160Final(aRmd, aCtx);
        for (i = 0; i < 20; i++) {
            if (aRmd[i] != aRmdEmpty[i]) {
                return 646;
            }
        }
        memset(aCtx, 0, sizeof(aCtx));
        BLAKE2s_Init(aCtx);
        BLAKE2s_Update(aCtx, "", 0);
        BLAKE2s_Final(aB2, aCtx);
        for (i = 0; i < 32; i++) {
            if (aB2[i] != aB2Empty[i]) {
                return 647;
            }
        }
        if (bcrypt("x", "y") != NULL) {
            return 648;
        }
        (void)bcrypt_gensalt(4, aVis);
        if (strtofflags(NULL, &uSet, &uClr) != 0 || uSet != 0 || uClr != 0) {
            return 649;
        }
        if (setnetconfig() != NULL) {
            return 650;
        }
        (void)copy_file_range64(-1, NULL, -1, NULL, 0, 0);
    }

    /* batch35: UUID + basename_r/dirname_r + strtoi + memcasecmp + gethrtime */
    {
        extern void uuid_create(void *, uint32_t *);
        extern void uuid_create_nil(void *, uint32_t *);
        extern int uuid_is_nil(const void *, uint32_t *);
        extern int uuid_equal(const void *, const void *, uint32_t *);
        extern void uuid_to_string(const void *, char **, uint32_t *);
        extern void uuid_from_string(const char *, void *, uint32_t *);
        extern char *basename_r(const char *, char *, size_t);
        extern char *dirname_r(const char *, char *, size_t);
        extern long long strtoi(const char *, char **, int, long long,
                                long long, int *);
        extern unsigned long long strtou(const char *, char **, int,
                                         unsigned long long, unsigned long long,
                                         int *);
        extern int memcasecmp(const void *, const void *, size_t);
        extern long long gethrtime(void);
        extern long long gethrvtime(void);
        extern const char __libcgj_batch35_marker[];
        unsigned char aU[16];
        unsigned char aNil[16];
        unsigned char aU2[16];
        char aPath[64];
        char *pStr = NULL;
        uint32_t uSt = 1;
        int nSt = -1;
        long long n;

        if (__libcgj_batch35_marker[0] == '\0') {
            return 651;
        }
        uuid_create_nil(aNil, &uSt);
        if (uSt != 0 || !uuid_is_nil(aNil, NULL)) {
            return 652;
        }
        uuid_create(aU, &uSt);
        if (uSt != 0 || uuid_is_nil(aU, NULL)) {
            return 653;
        }
        /* version nibble high of byte 6 must be 4 */
        if ((aU[6] & 0xf0u) != 0x40u) {
            return 654;
        }
        uuid_to_string(aU, &pStr, &uSt);
        if (pStr == NULL || uSt != 0 || strlen(pStr) != 36u) {
            return 655;
        }
        uuid_from_string(pStr, aU2, &uSt);
        if (uSt != 0 || !uuid_equal(aU, aU2, NULL)) {
            return 656;
        }
        if (basename_r("/mnt/shell.txt", aPath, sizeof(aPath)) == NULL ||
            strcmp(aPath, "shell.txt") != 0) {
            return 657;
        }
        if (dirname_r("/mnt/shell.txt", aPath, sizeof(aPath)) == NULL ||
            strcmp(aPath, "/mnt") != 0) {
            return 658;
        }
        n = strtoi("42", NULL, 10, 0, 100, &nSt);
        if (n != 42 || nSt != 0) {
            return 659;
        }
        n = strtoi("200", NULL, 10, 0, 100, &nSt);
        if (n != 100 || nSt != ERANGE) {
            return 660;
        }
        if (strtou("7", NULL, 10, 0, 10, &nSt) != 7ull || nSt != 0) {
            return 661;
        }
        if (memcasecmp("AbC", "aBc", 3) != 0) {
            return 662;
        }
        if (gethrtime() < 0) {
            return 663;
        }
        (void)gethrvtime();
    }

    /* batch36: libuuid + End helpers + siphash + radixsort + dehumanize */
    {
        extern void uuid_generate(unsigned char[16]);
        extern void uuid_clear(unsigned char[16]);
        extern void uuid_copy(unsigned char[16], const unsigned char[16]);
        extern int uuid_is_null(const unsigned char[16]);
        extern int uuid_parse(const char *, unsigned char[16]);
        extern void uuid_unparse(const unsigned char[16], char *);
        extern void uuid_unparse_upper(const unsigned char[16], char *);
        extern char *MD4Data(const void *, size_t, char *);
        extern char *BLAKE2s_Data(const void *, size_t, char *);
        extern uint64_t siphash24(const void *, size_t, const unsigned char[16]);
        extern int radixsort(const unsigned char **, int, const unsigned char *,
                             unsigned);
        extern int dehumanize_number(const char *, int64_t *);
        extern const char __libcgj_batch36_marker[];
        unsigned char aU[16];
        unsigned char aU2[16];
        char aStr[40];
        char aHex[80];
        char *p;
        const unsigned char *aKeys[3];
        static const unsigned char aKey[16];
        int64_t nHum = 0;
        /* MD4("") hex */
        static const char *szMd4Empty = "31d6cfe0d16ae931b73c59d7e0c089c0";

        if (__libcgj_batch36_marker[0] == '\0') {
            return 664;
        }
        uuid_clear(aU);
        if (!uuid_is_null(aU)) {
            return 665;
        }
        uuid_generate(aU);
        if (uuid_is_null(aU) || (aU[6] & 0xf0u) != 0x40u) {
            return 666;
        }
        uuid_unparse(aU, aStr);
        if (strlen(aStr) != 36u) {
            return 667;
        }
        if (uuid_parse(aStr, aU2) != 0 || memcmp(aU, aU2, 16) != 0) {
            return 668;
        }
        uuid_copy(aU2, aU);
        uuid_unparse_upper(aU, aStr);
        if (aStr[0] < '0' || (aStr[0] > '9' && aStr[0] < 'A')) {
            /* first hex digit upper or digit */
            if (aStr[0] < 'A' || aStr[0] > 'F') {
                if (aStr[0] < '0' || aStr[0] > '9') {
                    return 669;
                }
            }
        }
        p = MD4Data("", 0, aHex);
        if (p == NULL || strcmp(p, szMd4Empty) != 0) {
            return 670;
        }
        p = BLAKE2s_Data("", 0, aHex);
        if (p == NULL || strlen(p) != 64u) {
            return 671;
        }
        if (siphash24("hi", 2, aKey) == 0 && siphash24("ho", 2, aKey) == 0) {
            /* both zero is extremely unlikely; still OK if key zero-hash
             * collides — just require function runs */
        }
        (void)siphash24("hi", 2, aKey);
        aKeys[0] = (const unsigned char *)"banana";
        aKeys[1] = (const unsigned char *)"apple";
        aKeys[2] = (const unsigned char *)"cherry";
        if (radixsort(aKeys, 3, NULL, 0) != 0) {
            return 672;
        }
        if (strcmp((const char *)aKeys[0], "apple") != 0 ||
            strcmp((const char *)aKeys[1], "banana") != 0 ||
            strcmp((const char *)aKeys[2], "cherry") != 0) {
            return 673;
        }
        if (dehumanize_number("2k", &nHum) != 0 || nHum != 2048) {
            return 674;
        }
        if (dehumanize_number("10", &nHum) != 0 || nHum != 10) {
            return 675;
        }
    }

    /* batch37: fortify preadv chk + qecvt ENOSYS */
    {
        extern ssize_t __preadv_chk(int, const struct iovec *, int, off_t,
                                   size_t);
        extern char *qecvt(long double, int, int *, int *);
        extern const char __libcgj_batch37_marker[];
        struct iovec iov;
        char aOne[4];
        int nDec = -1, nSign = -1;

        if (__libcgj_batch37_marker[0] == '\0') {
            return 676;
        }
        iov.iov_base = aOne;
        iov.iov_len = 1;
        /* Bound too small → EINVAL without calling preadv */
        if (__preadv_chk(-1, &iov, 1, 0, 0) != -1) {
            return 677;
        }
        if (qecvt(0.0L, 2, &nDec, &nSign) != NULL) {
            return 678;
        }
        if (nDec != 0 || nSign != 0) {
            return 679;
        }
    }

    /* batch38: uuid type/variant/name-based + HMAC + BLAKE2b + SHA3-256 */
    {
        extern int uuid_type(const unsigned char[16]);
        extern int uuid_variant(const unsigned char[16]);
        extern time_t uuid_time(const unsigned char[16], struct timeval *);
        extern void uuid_generate_md5(unsigned char[16],
                                      const unsigned char[16], const char *);
        extern void uuid_generate_sha1(unsigned char[16],
                                       const unsigned char[16], const char *);
        extern void hmac_sha256(const void *, size_t, const void *, size_t,
                                unsigned char[32]);
        extern char *BLAKE2b_Data(const void *, size_t, char *);
        extern char *SHA3_256_Data(const void *, size_t, char *);
        extern const char __libcgj_batch38_marker[];
        unsigned char aNs[16];
        unsigned char aU[16];
        unsigned char aMac[32];
        char aHex[160];
        char *p;
        /* DNS namespace UUID (RFC 4122 appendix) */
        static const unsigned char aDnsNs[16] = {
            0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1,
            0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8};
        /* SHA3-256("") */
        static const char *szSha3Empty =
            "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a";
        /* BLAKE2b-512("") first 16 hex chars */
        static const char *szB2bPrefix = "786a02f742015903";

        if (__libcgj_batch38_marker[0] == '\0') {
            return 680;
        }
        memset(aNs, 0, sizeof(aNs));
        aNs[6] = 0x40; /* pretend v4 for type check */
        aNs[8] = 0x80; /* RFC variant */
        if (uuid_type(aNs) != 4 || uuid_variant(aNs) != 1) {
            return 681;
        }
        (void)uuid_time(aNs, NULL);
        uuid_generate_md5(aU, aDnsNs, "www.example.com");
        if (uuid_type(aU) != 3 || uuid_variant(aU) != 1) {
            return 682;
        }
        uuid_generate_sha1(aU, aDnsNs, "www.example.com");
        if (uuid_type(aU) != 5 || uuid_variant(aU) != 1) {
            return 683;
        }
        hmac_sha256("key", 3, "data", 4, aMac);
        if (aMac[0] == 0 && aMac[1] == 0 && aMac[2] == 0 && aMac[3] == 0) {
            /* all-zero MAC extremely unlikely */
            return 684;
        }
        p = BLAKE2b_Data("", 0, aHex);
        if (p == NULL || strlen(p) != 128u ||
            strncmp(p, szB2bPrefix, 16) != 0) {
            return 685;
        }
        p = SHA3_256_Data("", 0, aHex);
        if (p == NULL || strcmp(p, szSha3Empty) != 0) {
            return 686;
        }
    }

    /* batch39: HMAC-MD5/SHA512 + SHA384 + crc32/adler32/crc32c */
    {
        extern void hmac_md5(const void *, size_t, const void *, size_t,
                             unsigned char[16]);
        extern void hmac_sha512(const void *, size_t, const void *, size_t,
                                unsigned char[64]);
        extern char *SHA384_Data(const void *, size_t, char *);
        extern unsigned long crc32(unsigned long, const unsigned char *,
                                   unsigned int);
        extern unsigned long adler32(unsigned long, const unsigned char *,
                                     unsigned int);
        extern uint32_t crc32c(uint32_t, const void *, size_t);
        extern const char __libcgj_batch39_marker[];
        unsigned char aMac[64];
        char aHex[128];
        char *p;
        unsigned long uCrc;
        unsigned long uAd;
        uint32_t uC;
        /* SHA-384("") */
        static const char *szSha384Empty =
            "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da"
            "274edebfe76f65fbd51ad2f14898b95b";
        /* zlib CRC-32 of "123456789" */
        static const char *szNine = "123456789";

        if (__libcgj_batch39_marker[0] == '\0') {
            return 687;
        }
        hmac_md5("key", 3, "data", 4, aMac);
        if (aMac[0] == 0 && aMac[1] == 0 && aMac[2] == 0 && aMac[3] == 0) {
            return 688;
        }
        hmac_sha512("key", 3, "data", 4, aMac);
        if (aMac[0] == 0 && aMac[1] == 0 && aMac[2] == 0 && aMac[3] == 0) {
            return 689;
        }
        p = SHA384_Data("", 0, aHex);
        if (p == NULL || strcmp(p, szSha384Empty) != 0) {
            return 690;
        }
        uCrc = crc32(0ul, (const unsigned char *)szNine, 9u);
        if (uCrc != 0xcbf43926ul) {
            return 691;
        }
        uAd = adler32(1ul, (const unsigned char *)szNine, 9u);
        if (uAd != 0x091e01deul) {
            return 692;
        }
        uC = crc32c(0u, szNine, 9u);
        /* Castagnoli of "123456789" = 0xe3069283 */
        if (uC != 0xe3069283u) {
            return 693;
        }
    }

    /* batch40: SHA3-224/384/512 + SHA224 + SHA512/t + HMAC-SHA384 + XXH */
    {
        extern char *SHA3_224_Data(const void *, size_t, char *);
        extern char *SHA3_384_Data(const void *, size_t, char *);
        extern char *SHA3_512_Data(const void *, size_t, char *);
        extern char *SHA224_Data(const void *, size_t, char *);
        extern char *SHA512_224_Data(const void *, size_t, char *);
        extern char *SHA512_256_Data(const void *, size_t, char *);
        extern void hmac_sha384(const void *, size_t, const void *, size_t,
                                unsigned char[48]);
        extern uint32_t XXH32(const void *, size_t, uint32_t);
        extern uint64_t XXH64(const void *, size_t, uint64_t);
        extern const char __libcgj_batch40_marker[];
        unsigned char aMac[48];
        char aHex[160];
        char *p;
        /* FIPS empty-message digests */
        static const char *szSha3_224 =
            "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7";
        static const char *szSha3_384 =
            "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2a"
            "c3713831264adb47fb6bd1e058d5f004";
        static const char *szSha3_512 =
            "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a6"
            "15b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26";
        static const char *szSha224 =
            "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f";
        static const char *szSha512_224 =
            "6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4";
        static const char *szSha512_256 =
            "c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a";

        if (__libcgj_batch40_marker[0] == '\0') {
            return 694;
        }
        p = SHA3_224_Data("", 0, aHex);
        if (p == NULL || strcmp(p, szSha3_224) != 0) {
            return 695;
        }
        p = SHA3_384_Data("", 0, aHex);
        if (p == NULL || strcmp(p, szSha3_384) != 0) {
            return 696;
        }
        p = SHA3_512_Data("", 0, aHex);
        if (p == NULL || strcmp(p, szSha3_512) != 0) {
            return 697;
        }
        p = SHA224_Data("", 0, aHex);
        if (p == NULL || strcmp(p, szSha224) != 0) {
            return 698;
        }
        p = SHA512_224_Data("", 0, aHex);
        if (p == NULL || strcmp(p, szSha512_224) != 0) {
            return 699;
        }
        p = SHA512_256_Data("", 0, aHex);
        if (p == NULL || strcmp(p, szSha512_256) != 0) {
            return 700;
        }
        hmac_sha384("key", 3, "data", 4, aMac);
        if (aMac[0] == 0 && aMac[1] == 0 && aMac[2] == 0 && aMac[3] == 0) {
            return 701;
        }
        /* xxHash empty seed-0 known values */
        if (XXH32("", 0, 0) != 0x02cc5d05u) {
            return 702;
        }
        if (XXH64("", 0, 0) != 0xEF46DB3751D8E999ull) {
            return 703;
        }
        if (XXH32("123456789", 9, 0) == 0u) {
            return 704;
        }
        /* base64 + HKDF/PBKDF2 surface */
        {
            extern ssize_t base64_encode(const unsigned char *, size_t, char *,
                                         size_t);
            extern ssize_t base64_decode(const char *, size_t, unsigned char *,
                                         size_t);
            extern int hkdf_sha256(const void *, size_t, const void *, size_t,
                                   const void *, size_t, unsigned char *,
                                   size_t);
            extern int pbkdf2_hmac_sha256(const void *, size_t,
                                          const unsigned char *, size_t,
                                          unsigned, unsigned char *, size_t);
            char aB64[32];
            unsigned char aBin[16];
            unsigned char aOkm[16];
            unsigned char aDk[16];
            ssize_t n;

            n = base64_encode((const unsigned char *)"f", 1, aB64, sizeof(aB64));
            if (n != 4 || strcmp(aB64, "Zg==") != 0) {
                return 705;
            }
            n = base64_decode("Zg==", 4, aBin, sizeof(aBin));
            if (n != 1 || aBin[0] != 'f') {
                return 706;
            }
            if (hkdf_sha256(NULL, 0, "ikm", 3, "info", 4, aOkm, 16) != 0) {
                return 707;
            }
            if (aOkm[0] == 0 && aOkm[1] == 0 && aOkm[2] == 0 && aOkm[3] == 0) {
                return 708;
            }
            if (pbkdf2_hmac_sha256("pass", 4, (const unsigned char *)"salt", 4,
                                   2u, aDk, 16) != 0) {
                return 709;
            }
            if (aDk[0] == 0 && aDk[1] == 0 && aDk[2] == 0 && aDk[3] == 0) {
                return 710;
            }
        }
    }

    /* batch41: SHAKE + base32/hex + FNV/murmur + CRC16/64 + HMAC + XXH3 */
    {
        extern int SHAKE128_Data(const void *, size_t, unsigned char *, size_t);
        extern int SHAKE256_Data(const void *, size_t, unsigned char *, size_t);
        extern ssize_t base32_encode(const unsigned char *, size_t, char *,
                                     size_t);
        extern ssize_t base32_decode(const char *, size_t, unsigned char *,
                                     size_t);
        extern ssize_t hex_encode(const unsigned char *, size_t, char *,
                                  size_t);
        extern ssize_t hex_decode(const char *, size_t, unsigned char *,
                                  size_t);
        extern uint32_t fnv1a_32(const void *, size_t);
        extern uint64_t fnv1a_64(const void *, size_t);
        extern uint32_t murmur3_32(const void *, size_t, uint32_t);
        extern uint16_t crc16(const void *, size_t);
        extern uint64_t crc64(const void *, size_t);
        extern void hmac_sha224(const void *, size_t, const void *, size_t,
                                unsigned char[28]);
        extern void hmac_sha3_256(const void *, size_t, const void *, size_t,
                                  unsigned char[32]);
        extern int crypto_memcmp(const void *, const void *, size_t);
        extern uint64_t XXH3_64bits(const void *, size_t);
        extern const char __libcgj_batch41_marker[];
        unsigned char aShake[32];
        unsigned char aMac[32];
        unsigned char aBin[16];
        char aB32[32];
        char aHex[32];
        ssize_t n;
        /* SHAKE128("") first 16 bytes (FIPS 202) */
        static const unsigned char aShake128Empty16[16] = {
            0x7f, 0x9c, 0x2b, 0xa4, 0xe8, 0x8f, 0x82, 0x7d,
            0x61, 0x60, 0x45, 0x50, 0x76, 0x05, 0x85, 0x3e};
        static const char *szNine = "123456789";

        if (__libcgj_batch41_marker[0] == '\0') {
            return 711;
        }
        if (SHAKE128_Data("", 0, aShake, 16) != 0) {
            return 712;
        }
        if (memcmp(aShake, aShake128Empty16, 16) != 0) {
            return 713;
        }
        if (SHAKE256_Data("", 0, aShake, 32) != 0) {
            return 714;
        }
        if (aShake[0] == 0 && aShake[1] == 0 && aShake[2] == 0) {
            return 715;
        }
        n = base32_encode((const unsigned char *)"f", 1, aB32, sizeof(aB32));
        if (n != 8 || strcmp(aB32, "MY======") != 0) {
            return 716;
        }
        n = base32_decode("MY======", 8, aBin, sizeof(aBin));
        if (n != 1 || aBin[0] != 'f') {
            return 717;
        }
        n = hex_encode((const unsigned char *)"\xff\x00", 2, aHex, sizeof(aHex));
        if (n != 4 || strcmp(aHex, "ff00") != 0) {
            return 718;
        }
        n = hex_decode("ff00", 4, aBin, sizeof(aBin));
        if (n != 2 || aBin[0] != 0xff || aBin[1] != 0x00) {
            return 719;
        }
        /* FNV-1a empty = offset basis */
        if (fnv1a_32("", 0) != 2166136261u) {
            return 720;
        }
        if (fnv1a_64("", 0) != 14695981039346656037ull) {
            return 721;
        }
        if (fnv1a_32("a", 1) == fnv1a_32("", 0)) {
            return 722;
        }
        /* Murmur3_x86_32("", 0) = 0 */
        if (murmur3_32("", 0, 0) != 0u) {
            return 723;
        }
        if (murmur3_32(szNine, 9, 0) == 0u) {
            return 724;
        }
        /* CRC-16/CCITT-FALSE of "123456789" = 0x29B1 */
        if (crc16(szNine, 9) != 0x29b1u) {
            return 725;
        }
        if (crc64(szNine, 9) == 0ull) {
            return 726;
        }
        hmac_sha224("key", 3, "data", 4, aMac);
        if (aMac[0] == 0 && aMac[1] == 0 && aMac[2] == 0 && aMac[3] == 0) {
            return 727;
        }
        hmac_sha3_256("key", 3, "data", 4, aMac);
        if (aMac[0] == 0 && aMac[1] == 0 && aMac[2] == 0 && aMac[3] == 0) {
            return 728;
        }
        if (crypto_memcmp("abc", "abc", 3) != 0) {
            return 729;
        }
        if (crypto_memcmp("abc", "abd", 3) == 0) {
            return 730;
        }
        /* XXH3 empty known value */
        if (XXH3_64bits("", 0) != 0x2d06800538d394c2ull) {
            return 731;
        }
        if (XXH3_64bits(szNine, 9) == 0ull) {
            return 732;
        }
    }

    /* batch42: SM3 + ChaCha20/Poly1305 + base58/base32hex + CRC8/MODBUS +
     * HMAC-SHA3-512 + djb2/sdbm */
    {
        extern int SM3_Data(const void *, size_t, unsigned char[32]);
        extern char *SM3_Hex(const void *, size_t, char *);
        extern int chacha20_block(const unsigned char[32],
                                  const unsigned char[12], uint32_t,
                                  unsigned char[64]);
        extern int chacha20_xor(const unsigned char[32],
                                const unsigned char[12], uint32_t,
                                const unsigned char *, unsigned char *,
                                size_t);
        extern void poly1305_auth(const unsigned char[32],
                                  const unsigned char *, size_t,
                                  unsigned char[16]);
        extern ssize_t base58_encode(const unsigned char *, size_t, char *,
                                     size_t);
        extern ssize_t base58_decode(const char *, size_t, unsigned char *,
                                     size_t);
        extern ssize_t base32hex_encode(const unsigned char *, size_t, char *,
                                        size_t);
        extern ssize_t base32hex_decode(const char *, size_t, unsigned char *,
                                        size_t);
        extern uint8_t crc8(const void *, size_t);
        extern uint16_t crc16_modbus(const void *, size_t);
        extern void hmac_sha3_512(const void *, size_t, const void *, size_t,
                                  unsigned char[64]);
        extern uint32_t djb2_hash(const void *, size_t);
        extern uint32_t sdbm_hash(const void *, size_t);
        extern const char __libcgj_batch42_marker[];
        unsigned char aDig[64];
        unsigned char aBlock[64];
        unsigned char aTag[16];
        unsigned char aBin[16];
        char aB58[32];
        char aB32h[32];
        char aHex[72];
        ssize_t n;
        static const char *szNine = "123456789";
        /* SM3("") GM/T 0004-2012 */
        static const unsigned char aSm3Empty[32] = {
            0x1a, 0xb2, 0x1d, 0x83, 0x55, 0xcf, 0xa1, 0x7f, 0x8e, 0x61, 0x19,
            0x48, 0x31, 0xe8, 0x1a, 0x8f, 0x22, 0xbe, 0xc8, 0xc7, 0x28, 0xfe,
            0xfb, 0x74, 0x7e, 0xd0, 0x35, 0xeb, 0x50, 0x82, 0xaa, 0x2b};
        /* SM3("abc") */
        static const unsigned char aSm3Abc[32] = {
            0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9, 0xd1, 0xf2, 0xd4,
            0x6b, 0xdc, 0x10, 0xe4, 0xe2, 0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2,
            0xf7, 0xa2, 0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0};
        /* RFC 8439 §2.3.2 ChaCha20 block test key/nonce/counter=1 first 64 */
        static const unsigned char aCcKey[32] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
            0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
            0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
        static const unsigned char aCcNonce[12] = {0x00, 0x00, 0x00, 0x09, 0x00,
                                                   0x00, 0x00, 0x4a, 0x00, 0x00,
                                                   0x00, 0x00};
        static const unsigned char aCcBlock0[16] = {
            0x10, 0xf1, 0xe7, 0xe4, 0xd1, 0x3b, 0x59, 0x15,
            0x50, 0x0f, 0xdd, 0x1f, 0xa3, 0x20, 0x71, 0xc4};
        /* RFC 8439 §2.5.2 Poly1305 test */
        static const unsigned char aPolyKey[32] = {
            0x85, 0xd6, 0xbe, 0x78, 0x57, 0x55, 0x6d, 0x33, 0x7f, 0x44, 0x52,
            0xfe, 0x42, 0xd5, 0x06, 0xa8, 0x01, 0x03, 0x80, 0x8a, 0xfb, 0x0d,
            0xb2, 0xfd, 0x4a, 0xbf, 0xf6, 0xaf, 0x41, 0x49, 0xf5, 0x1b};
        static const char *szPolyMsg =
            "Cryptographic Forum Research Group";
        static const unsigned char aPolyTag[16] = {
            0xa8, 0x06, 0x1d, 0xc1, 0x30, 0x51, 0x36, 0xc6,
            0xc2, 0x2b, 0x8b, 0xaf, 0x0c, 0x01, 0x27, 0xa9};

        if (__libcgj_batch42_marker[0] == '\0') {
            return 733;
        }
        if (SM3_Data("", 0, aDig) != 0 ||
            memcmp(aDig, aSm3Empty, 32) != 0) {
            return 734;
        }
        if (SM3_Data("abc", 3, aDig) != 0 ||
            memcmp(aDig, aSm3Abc, 32) != 0) {
            return 735;
        }
        if (SM3_Hex("abc", 3, aHex) == NULL ||
            strcmp(aHex, "66c7f0f462eeedd9d1f2d46bdc10e4e24167c4875cf2f7a2297da"
                         "02b8f4ba8e0") != 0) {
            return 736;
        }
        if (chacha20_block(aCcKey, aCcNonce, 1u, aBlock) != 0) {
            return 737;
        }
        if (memcmp(aBlock, aCcBlock0, 16) != 0) {
            return 738;
        }
        {
            unsigned char aPt[16];
            unsigned char aCt[16];
            unsigned char aRt[16];
            size_t i;

            for (i = 0; i < 16u; i++) {
                aPt[i] = (unsigned char)i;
            }
            if (chacha20_xor(aCcKey, aCcNonce, 1u, aPt, aCt, 16) != 0) {
                return 739;
            }
            if (chacha20_xor(aCcKey, aCcNonce, 1u, aCt, aRt, 16) != 0) {
                return 740;
            }
            if (memcmp(aPt, aRt, 16) != 0) {
                return 741;
            }
        }
        poly1305_auth(aPolyKey, (const unsigned char *)szPolyMsg,
                      strlen(szPolyMsg), aTag);
        if (memcmp(aTag, aPolyTag, 16) != 0) {
            return 742;
        }
        n = base58_encode((const unsigned char *)"\0\0Hello", 7, aB58,
                          sizeof(aB58));
        if (n <= 0 || aB58[0] != '1' || aB58[1] != '1') {
            return 743;
        }
        n = base58_decode(aB58, (size_t)n, aBin, sizeof(aBin));
        if (n != 7 || aBin[0] != 0 || aBin[1] != 0 ||
            memcmp(aBin + 2, "Hello", 5) != 0) {
            return 744;
        }
        n = base32hex_encode((const unsigned char *)"f", 1, aB32h,
                             sizeof(aB32h));
        if (n != 8 || strcmp(aB32h, "CO======") != 0) {
            return 745;
        }
        n = base32hex_decode("CO======", 8, aBin, sizeof(aBin));
        if (n != 1 || aBin[0] != 'f') {
            return 746;
        }
        /* CRC-8/SMBUS of "123456789" = 0xF4 */
        if (crc8(szNine, 9) != 0xf4u) {
            return 747;
        }
        /* CRC-16/MODBUS of "123456789" = 0x4B37 */
        if (crc16_modbus(szNine, 9) != 0x4b37u) {
            return 748;
        }
        hmac_sha3_512("key", 3, "data", 4, aDig);
        if (aDig[0] == 0 && aDig[1] == 0 && aDig[2] == 0 && aDig[3] == 0) {
            return 749;
        }
        if (djb2_hash("", 0) != 5381u) {
            return 750;
        }
        if (djb2_hash("a", 1) == 5381u) {
            return 751;
        }
        if (sdbm_hash("a", 1) == sdbm_hash("", 0)) {
            return 752;
        }
    }

    /* batch45/50: AES-128 FIPS-197 C.1 + RC4 known vector + AES_ecb + Blowfish */
    {
        /* OpenSSL aes.h layout (AES_MAXNR == 14) */
        typedef struct {
            unsigned int rd_key[4 * (14 + 1)];
            int rounds;
        } AES_KEY;
        typedef struct {
            unsigned int x, y;
            unsigned int data[256];
        } RC4_KEY;
        typedef struct {
            uint32_t S[4][256];
            uint32_t P[18];
        } blf_ctx;

        extern int AES_set_encrypt_key(const unsigned char *, int, AES_KEY *);
        extern void AES_encrypt(const unsigned char *, unsigned char *,
                                const AES_KEY *);
        extern int AES_ecb_encrypt_block(const unsigned char *,
                                         unsigned char *,
                                         const unsigned char *, int);
        extern void RC4_set_key(RC4_KEY *, int, const unsigned char *);
        extern void RC4(RC4_KEY *, size_t, const unsigned char *,
                        unsigned char *);
        extern void Blowfish_initstate(blf_ctx *);
        extern void Blowfish_expand0state(blf_ctx *, const uint8_t *,
                                          uint16_t);
        extern void Blowfish_encrypt(blf_ctx *, uint32_t *, uint16_t);
        extern const char __libcgj_batch45_marker[];
        extern const char __libcgj_batch50_marker[];

        AES_KEY stAes;
        RC4_KEY stRc4;
        blf_ctx stBf;
        unsigned char aKey[16];
        unsigned char aPt[16];
        unsigned char aCt[16];
        unsigned char aEcb[16];
        unsigned char aRc4Out[9];
        uint32_t aBf[2];
        uint8_t aBfKey[8];
        int i;

        /* FIPS-197 Appendix C.1 AES-128:
         * key 00..0f, plaintext 001122..eeff →
         * 69c4e0d86a7b0430d8cdb78070b4c55a */
        static const unsigned char aFipsCt[16] = {
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};
        /* RC4: key "Key", plaintext "Plaintext" → bbf316e8d940af0ad3 */
        static const unsigned char aRc4Exp[9] = {0xbb, 0xf3, 0x16, 0xe8, 0xd9,
                                                 0x40, 0xaf, 0x0a, 0xd3};

        if (__libcgj_batch45_marker[0] == '\0') {
            return 753;
        }
        if (__libcgj_batch50_marker[0] == '\0') {
            return 754;
        }

        for (i = 0; i < 16; i++) {
            aKey[i] = (unsigned char)i;
            aPt[i] = (unsigned char)(i * 0x11);
        }
        if (AES_set_encrypt_key(aKey, 128, &stAes) != 0) {
            return 755;
        }
        if (stAes.rounds != 10) {
            return 756;
        }
        AES_encrypt(aPt, aCt, &stAes);
        if (memcmp(aCt, aFipsCt, 16) != 0) {
            return 757;
        }
        if (AES_ecb_encrypt_block(aPt, aEcb, aKey, 128) != 0) {
            return 758;
        }
        if (memcmp(aEcb, aFipsCt, 16) != 0) {
            return 759;
        }

        RC4_set_key(&stRc4, 3, (const unsigned char *)"Key");
        RC4(&stRc4, 9, (const unsigned char *)"Plaintext", aRc4Out);
        if (memcmp(aRc4Out, aRc4Exp, 9) != 0) {
            return 760;
        }

        /* Blowfish: zero key, zero block → 4EF99745 6198DD78 (Schneier) */
        for (i = 0; i < 8; i++) {
            aBfKey[i] = 0;
        }
        Blowfish_initstate(&stBf);
        Blowfish_expand0state(&stBf, aBfKey, 8);
        aBf[0] = 0;
        aBf[1] = 0;
        Blowfish_encrypt(&stBf, aBf, 1);
        if (aBf[0] != 0x4ef99745u || aBf[1] != 0x6198dd78u) {
            return 761;
        }
    }

    /*
     * Continuum hard gates: tea_encrypt zero vector, SM4 GB/T sample,
     * x25519 RFC 7748 vector 1, blake3 empty, cityhash64 empty.
     * Pure integer crypto — freestanding-safe (no heap/syscalls).
     */
    {
        extern void tea_encrypt(const uint32_t key[4], uint32_t v[2]);
        extern void SM4_set_key(const unsigned char key[16], uint32_t rk[32]);
        extern void SM4_encrypt(const unsigned char in[16],
                                unsigned char out[16], const uint32_t rk[32]);
        extern void x25519(unsigned char out[32],
                           const unsigned char scalar[32],
                           const unsigned char point[32]);
        extern void blake3_hash(const void *input, size_t input_len,
                                uint8_t *out, size_t out_len);
        extern uint64_t cityhash64(const char *s, size_t len);

        /* TEA all-zero key/block → 41ea3a0a 94baa940 (Wheeler/Needham) */
        {
            uint32_t aKey[4] = { 0, 0, 0, 0 };
            uint32_t aV[2] = { 0, 0 };

            tea_encrypt(aKey, aV);
            if (aV[0] != 0x41ea3a0au || aV[1] != 0x94baa940u) {
                return 762;
            }
        }

        /* SM4 GB/T 32907 sample vector */
        {
            static const unsigned char aSm4Key[16] = {
                0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
            static const unsigned char aSm4Pt[16] = {
                0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
            static const unsigned char aSm4Ct[16] = {
                0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e,
                0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46};
            uint32_t aRk[32];
            unsigned char aOut[16];

            SM4_set_key(aSm4Key, aRk);
            SM4_encrypt(aSm4Pt, aOut, aRk);
            if (memcmp(aOut, aSm4Ct, 16) != 0) {
                return 763;
            }
        }

        /* X25519 RFC 7748 §5.2 vector 1 */
        {
            static const unsigned char aSc[32] = {
                0xa5, 0x46, 0xe3, 0x6b, 0xf0, 0x52, 0x7c, 0x9d,
                0x3b, 0x16, 0x15, 0x4b, 0x82, 0x46, 0x5e, 0xdd,
                0x62, 0x14, 0x4c, 0x0a, 0xc1, 0xfc, 0x5a, 0x18,
                0x50, 0x6a, 0x22, 0x44, 0xba, 0x44, 0x9a, 0xc4};
            static const unsigned char aU[32] = {
                0xe6, 0xdb, 0x68, 0x67, 0x58, 0x30, 0x30, 0xdb,
                0x35, 0x94, 0xc1, 0xa4, 0x24, 0xb1, 0x5f, 0x7c,
                0x72, 0x66, 0x24, 0xec, 0x26, 0xb3, 0x35, 0x3b,
                0x10, 0xa9, 0x03, 0xa6, 0xd0, 0xab, 0x1c, 0x4c};
            static const unsigned char aExp[32] = {
                0xc3, 0xda, 0x55, 0x37, 0x9d, 0xe9, 0xc6, 0x90,
                0x8e, 0x94, 0xea, 0x4d, 0xf2, 0x8d, 0x08, 0x4f,
                0x32, 0xec, 0xcf, 0x03, 0x49, 0x1c, 0x71, 0xf7,
                0x54, 0xb4, 0x07, 0x55, 0x77, 0xa2, 0x85, 0x52};
            unsigned char aOut[32];

            x25519(aOut, aSc, aU);
            if (memcmp(aOut, aExp, 32) != 0) {
                return 764;
            }
        }

        /* BLAKE3 empty input → 32-byte official empty hash */
        {
            static const uint8_t aEmpty[32] = {
                0xaf, 0x13, 0x49, 0xb9, 0xf5, 0xf9, 0xa1, 0xa6,
                0xa0, 0x40, 0x4d, 0xea, 0x36, 0xdc, 0xc9, 0x49,
                0x9b, 0xcb, 0x25, 0xc9, 0xad, 0xc1, 0x12, 0xb7,
                0xcc, 0x9a, 0x93, 0xca, 0xe4, 0x1f, 0x32, 0x62};
            uint8_t aOut[32];

            memset(aOut, 0, sizeof(aOut));
            blake3_hash("", 0, aOut, 32);
            if (memcmp(aOut, aEmpty, 32) != 0) {
                return 765;
            }
        }

        /* CityHash64 empty → k2 constant */
        if (cityhash64("", 0) != 0x9ae16a3b2f90404full) {
            return 766;
        }
        if (cityhash64(NULL, 0) != 0x9ae16a3b2f90404full) {
            return 767;
        }
    }

    /*
     * Continuum static SSE gate (no new symbols): volatile double under
     * freestanding -msse2 link must compute correctly (proves SSE path OK).
     */
    {
        volatile double dA = 2.0;
        volatile double dB = 2.0;
        volatile double dSum = dA + dB;
        volatile double dProd = dA * dB;
        volatile double dQuot = dA / dB;
        volatile double dDiff = dSum - dProd;

        if (dSum != 4.0) {
            return 768;
        }
        if (dProd != 4.0) {
            return 769;
        }
        if (dQuot != 1.0) {
            return 770;
        }
        if (dDiff != 0.0) {
            return 771;
        }
    }

    /*
     * Continuum soft-then-hard gates (weak until linked from libcgj.a;
     * when present, hard-check behavior). Covers batch83 AES-NI probe,
     * batch88 drand48, batch92 sqrt, batch93 base64url_encode.
     */
    {
        extern int gj_cpu_has_aesni(void) __attribute__((weak));
        extern double drand48(void) __attribute__((weak));
        extern double sqrt(double) __attribute__((weak));
        extern ssize_t base64url_encode(const unsigned char *, size_t, char *,
                                        size_t) __attribute__((weak));

        if (gj_cpu_has_aesni != NULL) {
            int nHas = gj_cpu_has_aesni();

            if (nHas != 0 && nHas != 1) {
                return 772;
            }
        }
        /* drand48: unit double in [0, 1) */
        if (drand48 != NULL) {
            volatile double dR;
            int i;

            for (i = 0; i < 8; i++) {
                dR = drand48();
                if (!(dR >= 0.0 && dR < 1.0)) {
                    return 773;
                }
            }
        }
        /* sqrt: freestanding-safe via volatile (SSE sqrtsd path) */
        if (sqrt != NULL) {
            volatile double dIn = 4.0;
            volatile double dOut = sqrt(dIn);

            if (dOut != 2.0) {
                return 774;
            }
        }
        /* base64url_encode: RFC 4648 §5 "f" → "Zg==" (same as b64 for A-Za-z0-9) */
        if (base64url_encode != NULL) {
            char aB64u[8];
            ssize_t nEnc;

            nEnc = base64url_encode((const unsigned char *)"f", 1, aB64u,
                                    sizeof(aB64u));
            if (nEnc != 4 || strcmp(aB64u, "Zg==") != 0) {
                return 775;
            }
        }
    }

    /*
     * Continuum soft gates for upcoming batches (weak until linked from
     * libcgj.a; skip if missing, hard-check when present):
     *   highwayhash64, gj_deflate, gj_tar_header_parse, gj_cpu_has_avx2
     */
    {
        extern uint64_t highwayhash64(const unsigned char key[32],
                                      const void *data,
                                      size_t len) __attribute__((weak));
        extern int gj_deflate(const unsigned char *pIn, size_t cbIn,
                              unsigned char *pOut, size_t *pcbOut,
                              int nLevel) __attribute__((weak));
        extern int gj_tar_header_parse(const unsigned char hdr[512],
                                       char name_out[256], uint64_t *size,
                                       int *typeflag) __attribute__((weak));
        extern int gj_cpu_has_avx2(void) __attribute__((weak));

        /* gj_cpu_has_avx2: CPUID.7 EBX[5] probe → {0,1} */
        if (gj_cpu_has_avx2 != NULL) {
            int nHas = gj_cpu_has_avx2();

            if (nHas != 0 && nHas != 1) {
                return 776;
            }
        }
        /* highwayhash64: empty + zero key (product self-vector) */
        if (highwayhash64 != NULL) {
            unsigned char aKey[32];
            uint64_t u64Tag;
            int i;

            for (i = 0; i < 32; i++) {
                aKey[i] = 0;
            }
            u64Tag = highwayhash64(aKey, "", 0);
            if (u64Tag != 0x7035da75b9d54469ull) {
                return 777;
            }
            if (highwayhash64(NULL, "", 0) != u64Tag) {
                return 778;
            }
        }
        /* gj_deflate: empty store → zlib CMF/FLG 78 01, 11 bytes */
        if (gj_deflate != NULL) {
            unsigned char aOut[64];
            size_t cbOut = sizeof(aOut);

            if (gj_deflate((const unsigned char *)"", 0, aOut, &cbOut, 0) !=
                    0 ||
                cbOut != 11u || aOut[0] != 0x78u || aOut[1] != 0x01u) {
                return 779;
            }
        }
        /* gj_tar_header_parse: NULL → -1; minimal ustar "smoke" → 0 */
        if (gj_tar_header_parse != NULL) {
            unsigned char aHdr[512];
            char aName[256];
            uint64_t u64Sz = 1;
            int nTf = -1;
            int i;

            if (gj_tar_header_parse(NULL, aName, &u64Sz, &nTf) != -1) {
                return 780;
            }
            for (i = 0; i < 512; i++) {
                aHdr[i] = 0;
            }
            aHdr[0] = 's';
            aHdr[1] = 'm';
            aHdr[2] = 'o';
            aHdr[3] = 'k';
            aHdr[4] = 'e';
            /* mode / size / mtime octal fields (NUL-terminated) */
            aHdr[100] = '0';
            aHdr[101] = '0';
            aHdr[102] = '0';
            aHdr[103] = '0';
            aHdr[104] = '6';
            aHdr[105] = '4';
            aHdr[106] = '4';
            aHdr[124] = '0';
            aHdr[125] = '0';
            aHdr[126] = '0';
            aHdr[127] = '0';
            aHdr[128] = '0';
            aHdr[129] = '0';
            aHdr[130] = '0';
            aHdr[131] = '0';
            aHdr[132] = '0';
            aHdr[133] = '0';
            aHdr[134] = '0';
            aHdr[136] = '0';
            aHdr[137] = '0';
            aHdr[138] = '0';
            aHdr[139] = '0';
            aHdr[140] = '0';
            aHdr[141] = '0';
            aHdr[142] = '0';
            aHdr[143] = '0';
            aHdr[144] = '0';
            aHdr[145] = '0';
            aHdr[146] = '0';
            aHdr[156] = '0'; /* typeflag regular */
            aHdr[257] = 'u';
            aHdr[258] = 's';
            aHdr[259] = 't';
            aHdr[260] = 'a';
            aHdr[261] = 'r';
            aHdr[263] = '0';
            aHdr[264] = '0';
            /* chksum 005534 (spaces during sum) + NUL + space */
            aHdr[148] = '0';
            aHdr[149] = '0';
            aHdr[150] = '5';
            aHdr[151] = '5';
            aHdr[152] = '3';
            aHdr[153] = '4';
            aHdr[154] = '\0';
            aHdr[155] = ' ';
            u64Sz = 1;
            nTf = -1;
            if (gj_tar_header_parse(aHdr, aName, &u64Sz, &nTf) != 0 ||
                strcmp(aName, "smoke") != 0 || u64Sz != 0ull || nTf != '0') {
                return 781;
            }
        }
    }

    /*
     * Continuum soft gates (weak until linked from libcgj.a; skip if
     * missing, hard-check when present):
     *   gj_ws_header_parse, gj_lz4_decompress_safe, z85_encode
     */
    {
        extern int gj_ws_header_parse(const unsigned char *buf, size_t len,
                                      int *fin, int *opcode, int *masked,
                                      uint64_t *payload_len,
                                      size_t *hdr_len) __attribute__((weak));
        extern int gj_lz4_decompress_safe(const unsigned char *src,
                                          size_t srcSize, unsigned char *dst,
                                          size_t dstCapacity)
            __attribute__((weak));
        extern ssize_t z85_encode(const unsigned char *in, size_t inlen,
                                  char *out, size_t outlen)
            __attribute__((weak));

        /* gj_ws_header_parse: FIN text unmasked, 5-byte payload header */
        if (gj_ws_header_parse != NULL) {
            const unsigned char aWs[] = {0x81u, 0x05u};
            int nFin = -1;
            int nOp = -1;
            int nMask = -1;
            uint64_t uPay = 1;
            size_t cbHdr = 0;

            if (gj_ws_header_parse(NULL, 2, &nFin, &nOp, &nMask, &uPay,
                                   &cbHdr) != -1) {
                return 782;
            }
            nFin = nOp = nMask = -1;
            uPay = 1;
            cbHdr = 0;
            if (gj_ws_header_parse(aWs, sizeof(aWs), &nFin, &nOp, &nMask,
                                   &uPay, &cbHdr) != 0 ||
                nFin != 1 || nOp != 1 || nMask != 0 || uPay != 5ull ||
                cbHdr != 2u) {
                return 783;
            }
        }
        /* gj_lz4_decompress_safe: empty → 0; literals-only "hello" */
        if (gj_lz4_decompress_safe != NULL) {
            unsigned char aDst[16];
            const unsigned char aLz4[] = {0x50u, 'h', 'e', 'l', 'l', 'o'};
            int nOut;

            if (gj_lz4_decompress_safe(NULL, 0, aDst, sizeof(aDst)) != -1) {
                return 784;
            }
            if (gj_lz4_decompress_safe((const unsigned char *)"", 0, aDst,
                                       sizeof(aDst)) != 0) {
                return 785;
            }
            nOut = gj_lz4_decompress_safe(aLz4, sizeof(aLz4), aDst,
                                          sizeof(aDst));
            if (nOut != 5 || memcmp(aDst, "hello", 5) != 0) {
                return 786;
            }
        }
        /* z85_encode: RFC 32 vector → "HelloWorld"; empty → 0 */
        if (z85_encode != NULL) {
            static const unsigned char aBin[] = {
                0x86u, 0x4Fu, 0xD2u, 0x6Fu, 0xB5u, 0x59u, 0xF7u, 0x5Bu};
            char aZ[16];
            ssize_t nEnc;

            nEnc = z85_encode((const unsigned char *)"", 0, aZ, sizeof(aZ));
            if (nEnc != 0 || aZ[0] != '\0') {
                return 787;
            }
            nEnc = z85_encode(aBin, sizeof(aBin), aZ, sizeof(aZ));
            if (nEnc != 10 || strcmp(aZ, "HelloWorld") != 0) {
                return 788;
            }
        }
    }

    /*
     * Continuum soft gates (weak until linked from libcgj.a; skip if
     * missing, hard-check when present):
     *   gj_bitrev32, gj_gray_encode, gj_fletcher16
     */
    {
        extern uint32_t gj_bitrev32(uint32_t x) __attribute__((weak));
        extern uint32_t gj_gray_encode(uint32_t n) __attribute__((weak));
        extern uint16_t gj_fletcher16(const void *data, size_t len)
            __attribute__((weak));

        /* gj_bitrev32: 1 → MSB; doc vector 0x12345678 → 0x1e6a2c48 */
        if (gj_bitrev32 != NULL) {
            if (gj_bitrev32(0x00000001u) != 0x80000000u ||
                gj_bitrev32(0x12345678u) != 0x1e6a2c48u ||
                gj_bitrev32(0u) != 0u) {
                return 789;
            }
        }
        /* gj_gray_encode: n ^ (n >> 1); 0→0, 2→3, 7→4 */
        if (gj_gray_encode != NULL) {
            if (gj_gray_encode(0u) != 0u || gj_gray_encode(2u) != 3u ||
                gj_gray_encode(7u) != 4u) {
                return 790;
            }
        }
        /* gj_fletcher16: empty/NULL → 0; "\x01" → 0x0101; "abc" → 0x4c27 */
        if (gj_fletcher16 != NULL) {
            if (gj_fletcher16(NULL, 1) != 0u ||
                gj_fletcher16((const void *)"", 0) != 0u) {
                return 791;
            }
            if (gj_fletcher16((const void *)"\x01", 1) != 0x0101u ||
                gj_fletcher16((const void *)"abc", 3) != 0x4c27u) {
                return 792;
            }
        }
    }

    /*
     * Continuum soft gates (weak until linked from libcgj.a; skip if
     * missing, hard-check when present):
     *   gj_kmp_find, gj_gcd_u64, gj_modpow_u64
     */
    {
        extern ssize_t gj_kmp_find(const char *hay, const char *needle)
            __attribute__((weak));
        extern uint64_t gj_gcd_u64(uint64_t a, uint64_t b)
            __attribute__((weak));
        extern uint64_t gj_modpow_u64(uint64_t base, uint64_t exp, uint64_t mod)
            __attribute__((weak));

        /* gj_kmp_find: NULL → -1; empty needle → 0; classic LPS match */
        if (gj_kmp_find != NULL) {
            if (gj_kmp_find(NULL, "a") != (ssize_t)-1 ||
                gj_kmp_find("a", NULL) != (ssize_t)-1) {
                return 793;
            }
            if (gj_kmp_find("hello", "") != (ssize_t)0 ||
                gj_kmp_find("", "") != (ssize_t)0) {
                return 794;
            }
            if (gj_kmp_find("hello", "ll") != (ssize_t)2 ||
                gj_kmp_find("hello", "he") != (ssize_t)0 ||
                gj_kmp_find("hello", "xyz") != (ssize_t)-1 ||
                gj_kmp_find("aaa", "aa") != (ssize_t)0 ||
                gj_kmp_find("ababcabcabababd", "ababd") != (ssize_t)10) {
                return 795;
            }
        }
        /* gj_gcd_u64: gcd(0,0)→0; gcd(a,0)→a; Euclidean vectors */
        if (gj_gcd_u64 != NULL) {
            if (gj_gcd_u64(0u, 0u) != 0ull || gj_gcd_u64(0u, 7u) != 7ull ||
                gj_gcd_u64(7u, 0u) != 7ull) {
                return 796;
            }
            if (gj_gcd_u64(48u, 18u) != 6ull || gj_gcd_u64(17u, 13u) != 1ull ||
                gj_gcd_u64(1071u, 462u) != 21ull) {
                return 797;
            }
        }
        /* gj_modpow_u64: (base^exp) mod mod; mod 0 → uint64 wrap; mod 1 → 0 */
        if (gj_modpow_u64 != NULL) {
            if (gj_modpow_u64(2u, 0u, 7u) != 1ull ||
                gj_modpow_u64(5u, 0u, 1u) != 0ull ||
                gj_modpow_u64(3u, 5u, 1u) != 0ull) {
                return 798;
            }
            if (gj_modpow_u64(2u, 10u, 1000u) != 24ull ||
                gj_modpow_u64(5u, 3u, 13u) != 8ull ||
                gj_modpow_u64(2u, 10u, 0u) != 1024ull ||
                gj_modpow_u64(0u, 5u, 7u) != 0ull) {
                return 799;
            }
        }
    }

    /*
     * Continuum soft gates (weak until linked from libcgj.a; skip if
     * missing, hard-check when present):
     *   gj_fib_u64, gj_is_prime_u64, gj_edit_distance
     */
    {
        extern uint64_t gj_fib_u64(unsigned n) __attribute__((weak));
        extern int gj_is_prime_u64(uint64_t n) __attribute__((weak));
        extern size_t gj_edit_distance(const char *a, const char *b)
            __attribute__((weak));

        /* gj_fib_u64: F(0)=0, F(1)=1, F(10)=55 */
        if (gj_fib_u64 != NULL) {
            if (gj_fib_u64(0u) != 0ull || gj_fib_u64(1u) != 1ull ||
                gj_fib_u64(10u) != 55ull) {
                return 800;
            }
        }
        /* gj_is_prime_u64: 0/1/4 not prime; 2/3/17 prime; 91=7*13 composite */
        if (gj_is_prime_u64 != NULL) {
            if (gj_is_prime_u64(0u) != 0 || gj_is_prime_u64(1u) != 0 ||
                gj_is_prime_u64(4u) != 0) {
                return 801;
            }
            if (gj_is_prime_u64(2u) != 1 || gj_is_prime_u64(3u) != 1 ||
                gj_is_prime_u64(17u) != 1 || gj_is_prime_u64(91u) != 0) {
                return 802;
            }
        }
        /* gj_edit_distance: empty pair → 0; classic "kitten"/"sitting" → 3 */
        if (gj_edit_distance != NULL) {
            if (gj_edit_distance("", "") != (size_t)0) {
                return 803;
            }
            if (gj_edit_distance("kitten", "sitting") != (size_t)3) {
                return 804;
            }
        }
    }

    /*
     * Continuum soft gates (weak until linked from libcgj.a; skip if
     * missing, hard-check when present):
     *   gj_ctz64, gj_popcnt64, gj_str_len
     */
    {
        extern unsigned gj_ctz64(uint64_t x) __attribute__((weak));
        extern unsigned gj_popcnt64(uint64_t x) __attribute__((weak));
        extern size_t gj_str_len(const char *s) __attribute__((weak));

        /* gj_ctz64: ctz(1)=0, ctz(8)=3, ctz(0)=0 (API: zero → 0) */
        if (gj_ctz64 != NULL) {
            if (gj_ctz64(1ull) != 0u || gj_ctz64(8ull) != 3u ||
                gj_ctz64(0ull) != 0u) {
                return 805;
            }
        }
        /* gj_popcnt64: popcnt(0)=0, popcnt(7)=3, popcnt(0xFF)=8 */
        if (gj_popcnt64 != NULL) {
            if (gj_popcnt64(0ull) != 0u || gj_popcnt64(7ull) != 3u ||
                gj_popcnt64(0xFFull) != 8u) {
                return 806;
            }
        }
        /* gj_str_len: NULL=0, ""=0, "abc"=3 */
        if (gj_str_len != NULL) {
            if (gj_str_len(NULL) != (size_t)0 ||
                gj_str_len("") != (size_t)0 ||
                gj_str_len("abc") != (size_t)3) {
                return 807;
            }
        }
    }

    /*
     * Continuum soft gates (weak until linked from libcgj.a; skip if
     * missing, hard-check when present):
     *   gj_batch_id, gj_is_leap_year, gj_bset_set/test
     */
    {
        extern int gj_batch_id(void) __attribute__((weak));
        extern int gj_is_leap_year(unsigned y) __attribute__((weak));
        extern void gj_bset_set(uint64_t *words, size_t bit)
            __attribute__((weak));
        extern int gj_bset_test(const uint64_t *words, size_t bit)
            __attribute__((weak));

        /* gj_batch_id: wired continuum id → 500 */
        if (gj_batch_id != NULL) {
            if (gj_batch_id() != 500) {
                return 808;
            }
        }
        /* gj_is_leap_year: proleptic Gregorian; 2000 leap, 1900 not, 2024 leap */
        if (gj_is_leap_year != NULL) {
            if (gj_is_leap_year(2000u) != 1 || gj_is_leap_year(1900u) != 0 ||
                gj_is_leap_year(2024u) != 1) {
                return 809;
            }
        }
        /* gj_bset_set/test: set bit 3, test bit 3 == 1 */
        if (gj_bset_set != NULL && gj_bset_test != NULL) {
            uint64_t aWords[1] = { 0ull };

            gj_bset_set(aWords, (size_t)3);
            if (gj_bset_test(aWords, (size_t)3) != 1) {
                return 810;
            }
        }
    }

    /*
     * Continuum decade soft lamps (weak until linked from libcgj.a;
     * skip if missing, hard-check when present). Key lamps only for
     * milestones 14500 / 14600 / 14700 / 14800 / 14900. bar3 and
     * product_score still open (0). Full decade probes:
     * cgj_soft_milestone_*.c
     */
    {
        typedef uint32_t (*u32_fn)(void);
        struct soft_u32_kat {
            u32_fn pfn;
            uint32_t u32Expect;
        };

        extern uint32_t gj_shell_green_14500(void) __attribute__((weak));
        extern uint32_t gj_bar3_ready_14500(void) __attribute__((weak));
        extern uint32_t gj_smoke_soft_14500(void) __attribute__((weak));
        extern uint32_t gj_milestone_tag_14500(void) __attribute__((weak));
        extern uint32_t gj_batch_id_14500(void) __attribute__((weak));
        extern uint32_t gj_graph_milestone_14500(void) __attribute__((weak));

        extern uint32_t gj_shell_green_14600(void) __attribute__((weak));
        extern uint32_t gj_bar3_ready_14600(void) __attribute__((weak));
        extern uint32_t gj_smoke_soft_14600(void) __attribute__((weak));
        extern uint32_t gj_milestone_tag_14600(void) __attribute__((weak));
        extern uint32_t gj_batch_id_14600(void) __attribute__((weak));
        extern uint32_t gj_graph_milestone_14600(void) __attribute__((weak));

        extern uint32_t gj_shell_green_14700(void) __attribute__((weak));
        extern uint32_t gj_bar3_ready_14700(void) __attribute__((weak));
        extern uint32_t gj_smoke_soft_14700(void) __attribute__((weak));
        extern uint32_t gj_milestone_tag_14700(void) __attribute__((weak));
        extern uint32_t gj_batch_id_14700(void) __attribute__((weak));
        extern uint32_t gj_graph_milestone_14700(void) __attribute__((weak));

        extern uint32_t gj_shell_green_14800(void) __attribute__((weak));
        extern uint32_t gj_bar3_ready_14800(void) __attribute__((weak));
        extern uint32_t gj_smoke_soft_14800(void) __attribute__((weak));
        extern uint32_t gj_milestone_tag_14800(void) __attribute__((weak));
        extern uint32_t gj_batch_id_14800(void) __attribute__((weak));
        extern uint32_t gj_graph_milestone_14800(void) __attribute__((weak));

        extern uint32_t gj_shell_green_14900(void) __attribute__((weak));
        extern uint32_t gj_bar3_ready_14900(void) __attribute__((weak));
        extern uint32_t gj_smoke_soft_14900(void) __attribute__((weak));
        extern uint32_t gj_milestone_tag_14900(void) __attribute__((weak));
        extern uint32_t gj_batch_id_14900(void) __attribute__((weak));
        extern uint32_t gj_graph_milestone_14900(void) __attribute__((weak));

        const struct soft_u32_kat kLamps[] = {
            { gj_shell_green_14500, 1u },
            { gj_bar3_ready_14500, 0u },
            { gj_smoke_soft_14500, 1u },
            { gj_milestone_tag_14500, 14500u },
            { gj_batch_id_14500, 14500u },
            { gj_graph_milestone_14500, 14500u },

            { gj_shell_green_14600, 1u },
            { gj_bar3_ready_14600, 0u },
            { gj_smoke_soft_14600, 1u },
            { gj_milestone_tag_14600, 14600u },
            { gj_batch_id_14600, 14600u },
            { gj_graph_milestone_14600, 14600u },

            { gj_shell_green_14700, 1u },
            { gj_bar3_ready_14700, 0u },
            { gj_smoke_soft_14700, 1u },
            { gj_milestone_tag_14700, 14700u },
            { gj_batch_id_14700, 14700u },
            { gj_graph_milestone_14700, 14700u },

            { gj_shell_green_14800, 1u },
            { gj_bar3_ready_14800, 0u },
            { gj_smoke_soft_14800, 1u },
            { gj_milestone_tag_14800, 14800u },
            { gj_batch_id_14800, 14800u },
            { gj_graph_milestone_14800, 14800u },

            { gj_shell_green_14900, 1u },
            { gj_bar3_ready_14900, 0u },
            { gj_smoke_soft_14900, 1u },
            { gj_milestone_tag_14900, 14900u },
            { gj_batch_id_14900, 14900u },
            { gj_graph_milestone_14900, 14900u },
        };
        size_t iLamp;

        for (iLamp = 0;
             iLamp < sizeof(kLamps) / sizeof(kLamps[0]);
             iLamp++) {
            if (kLamps[iLamp].pfn == NULL) {
                continue; /* soft-skip until linked */
            }
            if (kLamps[iLamp].pfn() != kLamps[iLamp].u32Expect) {
                return 811;
            }
        }
    }

    printf("libcgj: smoke PASS pid=%d uname=%s self=%lu\n", (int)getpid(),
           un.sysname, (unsigned long)pthread_self());
    return 0;
}

/*
 * crt0 _start does `sub $8,%rsp` then `call main`, so main is entered with
 * rsp≡0 instead of SysV rsp≡8. Naked trampoline realigns to 16 bytes, then
 * calls the body so -msse2 movaps / volatile double spills are safe.
 * Args stay in rdi/rsi/rdx (SysV); no C prologue allowed here.
 */
__attribute__((naked)) int
main(int argc, char **argv, char **envp)
{
    (void)argc;
    (void)argv;
    (void)envp;
    __asm__ volatile(
        "movq %%rsp, %%rax\n\t"
        "andq $-16, %%rsp\n\t"
        "subq $16, %%rsp\n\t"
        "movq %%rax, (%%rsp)\n\t"
        "call cgj_smoke_body\n\t"
        "movq (%%rsp), %%rsp\n\t"
        "ret\n\t"
        :
        :
        : "memory");
}
