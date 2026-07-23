/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host smoke: dlopen product libc.so.6 (libcgj) and resolve exports.
 * Cannot DT_NEEDED soname libc.so.6 against host ld-linux (glibc expects
 * private NPTL symbols). On GreenJade, ld-gj loads this SO as the real libc.
 *
 * Built with host libc (not freestanding).
 * RTLD_DEEPBIND: product-internal calls (e.g. drand48 → seed48) must not
 * interpose onto host glibc when soft gates invoke product symbols.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef int (*printf_fn)(const char *, ...);
typedef void *(*malloc_fn)(size_t);
typedef void (*free_fn)(void *);
typedef int (*strcmp_fn)(const char *, const char *);

static const char *const kPaths[] = {
    "build/user/libc.so.6",
    "./build/user/libc.so.6",
    NULL,
};

int
main(void)
{
    void *h = NULL;
    const char *szPath = NULL;
    int i;
    printf_fn pPrintf;
    malloc_fn pMalloc;
    free_fn pFree;
    strcmp_fn pStrcmp;
    volatile unsigned long long *pMark;
    void *p;

    for (i = 0; kPaths[i] != NULL; i++) {
        h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
        if (h != NULL) {
            szPath = kPaths[i];
            break;
        }
    }
    if (h == NULL) {
        fprintf(stderr, "libcgj: dyn dlopen fail: %s\n", dlerror());
        return 1;
    }

    pMark = (volatile unsigned long long *)dlsym(h, "gj_libc_marker");
    pPrintf = (printf_fn)dlsym(h, "printf");
    pMalloc = (malloc_fn)dlsym(h, "malloc");
    pFree = (free_fn)dlsym(h, "free");
    pStrcmp = (strcmp_fn)dlsym(h, "strcmp");

    if (pMark == NULL || pPrintf == NULL || pMalloc == NULL || pFree == NULL ||
        pStrcmp == NULL) {
        fprintf(stderr, "libcgj: dyn dlsym fail: %s\n", dlerror());
        (void)dlclose(h);
        return 2;
    }
    /* glibc-shaped entry helpers must export */
    if (dlsym(h, "__errno_location") == NULL ||
        dlsym(h, "__libc_start_main") == NULL ||
        dlsym(h, "__tls_get_addr") == NULL ||
        dlsym(h, "pthread_mutex_lock") == NULL) {
        fprintf(stderr, "libcgj: dyn missing glibc helpers: %s\n", dlerror());
        (void)dlclose(h);
        return 6;
    }
    /* pipe zero-copy + mmsg surface gates */
    if (dlsym(h, "splice") == NULL || dlsym(h, "tee") == NULL ||
        dlsym(h, "vmsplice") == NULL || dlsym(h, "sendmsg") == NULL ||
        dlsym(h, "recvmsg") == NULL || dlsym(h, "sendmmsg") == NULL ||
        dlsym(h, "recvmmsg") == NULL || dlsym(h, "accept4") == NULL) {
        fprintf(stderr, "libcgj: dyn missing splice/mmsg: %s\n", dlerror());
        (void)dlclose(h);
        return 7;
    }
    if (dlsym(h, "mkfifo") == NULL || dlsym(h, "mknod") == NULL ||
        dlsym(h, "mknodat") == NULL || dlsym(h, "mkfifoat") == NULL ||
        dlsym(h, "pselect") == NULL || dlsym(h, "dup3") == NULL) {
        fprintf(stderr, "libcgj: dyn missing mkfifo/pselect: %s\n", dlerror());
        (void)dlclose(h);
        return 8;
    }
    if (dlsym(h, "inotify_init1") == NULL || dlsym(h, "preadv") == NULL ||
        dlsym(h, "pwritev") == NULL || dlsym(h, "fchdir") == NULL ||
        dlsym(h, "getitimer") == NULL || dlsym(h, "setitimer") == NULL ||
        dlsym(h, "ttyname_r") == NULL) {
        fprintf(stderr, "libcgj: dyn missing inotify/itimer: %s\n", dlerror());
        (void)dlclose(h);
        return 9;
    }
    if (dlsym(h, "timer_create") == NULL || dlsym(h, "timer_settime") == NULL ||
        dlsym(h, "timer_gettime") == NULL || dlsym(h, "timer_delete") == NULL ||
        dlsym(h, "timer_getoverrun") == NULL ||
        dlsym(h, "clock_nanosleep") == NULL || dlsym(h, "mq_open") == NULL ||
        dlsym(h, "sem_init") == NULL || dlsym(h, "sem_post") == NULL ||
        dlsym(h, "sem_wait") == NULL) {
        fprintf(stderr, "libcgj: dyn missing timer/mq/sem: %s\n", dlerror());
        (void)dlclose(h);
        return 10;
    }
    if (dlsym(h, "getline") == NULL || dlsym(h, "getdelim") == NULL ||
        dlsym(h, "freopen") == NULL || dlsym(h, "fseeko") == NULL ||
        dlsym(h, "ftello") == NULL) {
        fprintf(stderr, "libcgj: dyn missing getline/freopen: %s\n", dlerror());
        (void)dlclose(h);
        return 11;
    }
    if (dlsym(h, "fmemopen") == NULL || dlsym(h, "open_memstream") == NULL ||
        dlsym(h, "pthread_rwlock_rdlock") == NULL ||
        dlsym(h, "pthread_rwlock_wrlock") == NULL) {
        fprintf(stderr, "libcgj: dyn missing fmemopen/rwlock: %s\n", dlerror());
        (void)dlclose(h);
        return 12;
    }
    if (dlsym(h, "pthread_barrier_wait") == NULL ||
        dlsym(h, "pthread_spin_lock") == NULL ||
        dlsym(h, "pthread_rwlock_timedrdlock") == NULL ||
        dlsym(h, "pthread_rwlock_timedwrlock") == NULL) {
        fprintf(stderr, "libcgj: dyn missing barrier/spin: %s\n", dlerror());
        (void)dlclose(h);
        return 13;
    }
    if (dlsym(h, "pthread_mutex_timedlock") == NULL ||
        dlsym(h, "pthread_cond_timedwait") == NULL ||
        dlsym(h, "pthread_attr_setstacksize") == NULL ||
        dlsym(h, "pthread_attr_getstacksize") == NULL) {
        fprintf(stderr, "libcgj: dyn missing timed/attr: %s\n", dlerror());
        (void)dlclose(h);
        return 14;
    }
    if (dlsym(h, "asprintf") == NULL || dlsym(h, "vasprintf") == NULL ||
        dlsym(h, "setlinebuf") == NULL || dlsym(h, "setbuffer") == NULL) {
        fprintf(stderr, "libcgj: dyn missing asprintf: %s\n", dlerror());
        (void)dlclose(h);
        return 15;
    }
    if (dlsym(h, "open_wmemstream") == NULL || dlsym(h, "fputwc") == NULL ||
        dlsym(h, "pthread_mutexattr_settype") == NULL ||
        dlsym(h, "pthread_condattr_setclock") == NULL ||
        dlsym(h, "wcslen") == NULL) {
        fprintf(stderr, "libcgj: dyn missing wmem/attr: %s\n", dlerror());
        (void)dlclose(h);
        return 16;
    }
    if (dlsym(h, "mbrtowc") == NULL || dlsym(h, "wcrtomb") == NULL ||
        dlsym(h, "mbsrtowcs") == NULL || dlsym(h, "wcsrtombs") == NULL ||
        dlsym(h, "fwprintf") == NULL || dlsym(h, "swprintf") == NULL ||
        dlsym(h, "vfwprintf") == NULL ||
        dlsym(h, "pthread_rwlockattr_init") == NULL) {
        fprintf(stderr, "libcgj: dyn missing mbr/fwprintf: %s\n", dlerror());
        (void)dlclose(h);
        return 17;
    }
    if (dlsym(h, "btowc") == NULL || dlsym(h, "iswalpha") == NULL ||
        dlsym(h, "towlower") == NULL || dlsym(h, "wcsdup") == NULL) {
        fprintf(stderr, "libcgj: dyn missing wctype: %s\n", dlerror());
        (void)dlclose(h);
        return 18;
    }
    if (dlsym(h, "wmemcpy") == NULL || dlsym(h, "wmemcmp") == NULL ||
        dlsym(h, "swscanf") == NULL || dlsym(h, "fwscanf") == NULL ||
        dlsym(h, "wcscoll") == NULL || dlsym(h, "wmemchr") == NULL) {
        fprintf(stderr, "libcgj: dyn missing wmem/scanf: %s\n", dlerror());
        (void)dlclose(h);
        return 19;
    }
    if (dlsym(h, "strlcpy") == NULL || dlsym(h, "mempcpy") == NULL ||
        dlsym(h, "strerror_r") == NULL || dlsym(h, "ffs") == NULL ||
        dlsym(h, "localeconv") == NULL || dlsym(h, "regcomp") == NULL ||
        dlsym(h, "arc4random") == NULL || dlsym(h, "sched_getaffinity") == NULL ||
        dlsym(h, "pthread_setname_np") == NULL || dlsym(h, "wcsstr") == NULL ||
        dlsym(h, "secure_getenv") == NULL || dlsym(h, "dlvsym") == NULL ||
        dlsym(h, "crypt") == NULL || dlsym(h, "getpass") == NULL) {
        fprintf(stderr, "libcgj: dyn missing batch2: %s\n", dlerror());
        (void)dlclose(h);
        return 20;
    }
    if (dlsym(h, "strlcpy") == NULL || dlsym(h, "strlcat") == NULL ||
        dlsym(h, "memrchr") == NULL || dlsym(h, "explicit_bzero") == NULL ||
        dlsym(h, "strcasestr") == NULL || dlsym(h, "stpncpy") == NULL ||
        dlsym(h, "strverscmp") == NULL || dlsym(h, "arc4random") == NULL ||
        dlsym(h, "arc4random_buf") == NULL ||
        dlsym(h, "arc4random_uniform") == NULL ||
        dlsym(h, "sched_getaffinity") == NULL ||
        dlsym(h, "sched_setaffinity") == NULL ||
        dlsym(h, "pthread_setname_np") == NULL ||
        dlsym(h, "pthread_getname_np") == NULL) {
        fprintf(stderr, "libcgj: dyn missing str/arc4/sched/name: %s\n",
                dlerror());
        (void)dlclose(h);
        return 20;
    }
    if (dlsym(h, "sscanf") == NULL || dlsym(h, "fscanf") == NULL ||
        dlsym(h, "scanf") == NULL || dlsym(h, "vsscanf") == NULL) {
        fprintf(stderr, "libcgj: dyn missing narrow scanf: %s\n", dlerror());
        (void)dlclose(h);
        return 20;
    }
    if (dlsym(h, "wcstol") == NULL || dlsym(h, "wcstoul") == NULL ||
        dlsym(h, "wcstoll") == NULL || dlsym(h, "wcstoull") == NULL ||
        dlsym(h, "wcstok") == NULL || dlsym(h, "wcsftime") == NULL ||
        dlsym(h, "setjmp") == NULL || dlsym(h, "longjmp") == NULL ||
        dlsym(h, "sigsetjmp") == NULL || dlsym(h, "getsubopt") == NULL ||
        dlsym(h, "getgrouplist") == NULL || dlsym(h, "wordexp") == NULL ||
        dlsym(h, "wordfree") == NULL || dlsym(h, "posix_spawn") == NULL ||
        dlsym(h, "posix_spawnp") == NULL ||
        dlsym(h, "posix_spawnattr_init") == NULL ||
        dlsym(h, "posix_spawn_file_actions_init") == NULL ||
        dlsym(h, "fts_open") == NULL || dlsym(h, "fts_read") == NULL ||
        dlsym(h, "fts_close") == NULL || dlsym(h, "fts_set") == NULL ||
        dlsym(h, "openpty") == NULL || dlsym(h, "forkpty") == NULL ||
        dlsym(h, "sigemptyset") == NULL || dlsym(h, "sigprocmask") == NULL ||
        dlsym(h, "sigaction") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch3: %s\n", dlerror());
        (void)dlclose(h);
        return 21;
    }
    if (dlsym(h, "wait") == NULL || dlsym(h, "waitpid") == NULL ||
        dlsym(h, "wait4") == NULL || dlsym(h, "execvp") == NULL ||
        dlsym(h, "execl") == NULL || dlsym(h, "system") == NULL ||
        dlsym(h, "popen") == NULL || dlsym(h, "pclose") == NULL ||
        dlsym(h, "atexit") == NULL || dlsym(h, "__cxa_atexit") == NULL ||
        dlsym(h, "rand") == NULL || dlsym(h, "srand") == NULL ||
        dlsym(h, "random") == NULL || dlsym(h, "div") == NULL ||
        dlsym(h, "ldiv") == NULL || dlsym(h, "reallocarray") == NULL ||
        dlsym(h, "memalign") == NULL || dlsym(h, "htonl") == NULL ||
        dlsym(h, "htons") == NULL || dlsym(h, "ntohl") == NULL ||
        dlsym(h, "bzero") == NULL || dlsym(h, "index") == NULL ||
        dlsym(h, "strcoll") == NULL || dlsym(h, "dprintf") == NULL ||
        dlsym(h, "fgetpos") == NULL || dlsym(h, "flockfile") == NULL ||
        dlsym(h, "tmpfile") == NULL || dlsym(h, "getpwnam") == NULL ||
        dlsym(h, "getpwuid") == NULL || dlsym(h, "getgrnam") == NULL ||
        dlsym(h, "getlogin") == NULL || dlsym(h, "dirfd") == NULL ||
        dlsym(h, "readdir_r") == NULL || dlsym(h, "sigpending") == NULL ||
        dlsym(h, "sigwait") == NULL || dlsym(h, "pthread_kill") == NULL ||
        dlsym(h, "pthread_sigmask") == NULL ||
        dlsym(h, "__assert_fail") == NULL ||
        dlsym(h, "__stack_chk_fail") == NULL ||
        dlsym(h, "program_invocation_name") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch4: %s\n", dlerror());
        (void)dlclose(h);
        return 22;
    }
    if (dlsym(h, "setmntent") == NULL || dlsym(h, "getmntent") == NULL ||
        dlsym(h, "endmntent") == NULL || dlsym(h, "hasmntopt") == NULL ||
        dlsym(h, "getspnam") == NULL || dlsym(h, "getutent") == NULL ||
        dlsym(h, "setutent") == NULL || dlsym(h, "getutxent") == NULL ||
        dlsym(h, "scandirat") == NULL || dlsym(h, "getpgrp") == NULL ||
        dlsym(h, "setpgid") == NULL || dlsym(h, "getpgid") == NULL ||
        dlsym(h, "getsid") == NULL || dlsym(h, "tcgetpgrp") == NULL ||
        dlsym(h, "tcsetpgrp") == NULL || dlsym(h, "tcgetsid") == NULL ||
        dlsym(h, "getusershell") == NULL ||
        dlsym(h, "getservbyname") == NULL ||
        dlsym(h, "getprotobyname") == NULL ||
        dlsym(h, "gethostbyname") == NULL ||
        dlsym(h, "getdomainname") == NULL || dlsym(h, "chroot") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch5: %s\n", dlerror());
        (void)dlclose(h);
        return 23;
    }
    if (dlsym(h, "newlocale") == NULL || dlsym(h, "uselocale") == NULL ||
        dlsym(h, "duplocale") == NULL || dlsym(h, "freelocale") == NULL ||
        dlsym(h, "pthread_attr_setstack") == NULL ||
        dlsym(h, "pthread_attr_getstack") == NULL ||
        dlsym(h, "pthread_setaffinity_np") == NULL ||
        dlsym(h, "pthread_getaffinity_np") == NULL ||
        dlsym(h, "sched_getcpu") == NULL || dlsym(h, "adjtime") == NULL ||
        dlsym(h, "fstatat") == NULL || dlsym(h, "dl_iterate_phdr") == NULL ||
        dlsym(h, "getauxval") == NULL || dlsym(h, "prctl") == NULL ||
        dlsym(h, "clock_settime") == NULL ||
        dlsym(h, "posix_fadvise") == NULL ||
        dlsym(h, "posix_madvise") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch6: %s\n", dlerror());
        (void)dlclose(h);
        return 24;
    }
    if (dlsym(h, "pthread_attr_setguardsize") == NULL ||
        dlsym(h, "pthread_attr_getguardsize") == NULL ||
        dlsym(h, "pthread_attr_setaffinity_np") == NULL ||
        dlsym(h, "pthread_attr_getaffinity_np") == NULL ||
        dlsym(h, "pthread_getattr_np") == NULL ||
        dlsym(h, "pthread_setschedparam") == NULL ||
        dlsym(h, "pthread_getschedparam") == NULL ||
        dlsym(h, "pthread_setschedprio") == NULL ||
        dlsym(h, "pthread_getcpuclockid") == NULL ||
        dlsym(h, "pthread_atfork") == NULL ||
        dlsym(h, "__register_atfork") == NULL ||
        dlsym(h, "malloc_usable_size") == NULL ||
        dlsym(h, "__libc_single_threaded") == NULL ||
        dlsym(h, "tgkill") == NULL ||
        dlsym(h, "sched_setscheduler") == NULL ||
        dlsym(h, "sched_getscheduler") == NULL ||
        dlsym(h, "sched_setparam") == NULL ||
        dlsym(h, "sched_getparam") == NULL ||
        dlsym(h, "sched_get_priority_max") == NULL ||
        dlsym(h, "sched_get_priority_min") == NULL ||
        dlsym(h, "sched_rr_get_interval") == NULL ||
        dlsym(h, "__sched_cpucount") == NULL ||
        dlsym(h, "clock_getcpuclockid") == NULL ||
        dlsym(h, "process_vm_readv") == NULL ||
        dlsym(h, "process_vm_writev") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch7: %s\n", dlerror());
        (void)dlclose(h);
        return 25;
    }
    if (dlsym(h, "capget") == NULL || dlsym(h, "capset") == NULL ||
        dlsym(h, "sync_file_range") == NULL || dlsym(h, "readahead") == NULL ||
        dlsym(h, "personality") == NULL || dlsym(h, "reboot") == NULL ||
        dlsym(h, "klogctl") == NULL || dlsym(h, "gettid") == NULL ||
        dlsym(h, "execveat") == NULL || dlsym(h, "close_range") == NULL ||
        dlsym(h, "statx") == NULL || dlsym(h, "mount") == NULL ||
        dlsym(h, "umount2") == NULL || dlsym(h, "pivot_root") == NULL ||
        dlsym(h, "setxattr") == NULL || dlsym(h, "getxattr") == NULL ||
        dlsym(h, "listxattr") == NULL || dlsym(h, "removexattr") == NULL ||
        dlsym(h, "faccessat2") == NULL || dlsym(h, "fchmodat2") == NULL ||
        dlsym(h, "membarrier") == NULL || dlsym(h, "ioprio_get") == NULL ||
        dlsym(h, "ioprio_set") == NULL ||
        dlsym(h, "name_to_handle_at") == NULL ||
        dlsym(h, "open_by_handle_at") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch8: %s\n", dlerror());
        (void)dlclose(h);
        return 26;
    }
    if (dlsym(h, "strtoimax") == NULL || dlsym(h, "strtoumax") == NULL ||
        dlsym(h, "imaxabs") == NULL || dlsym(h, "imaxdiv") == NULL ||
        dlsym(h, "qsort_r") == NULL || dlsym(h, "_Exit") == NULL ||
        dlsym(h, "group_member") == NULL || dlsym(h, "getwd") == NULL ||
        dlsym(h, "tsearch") == NULL || dlsym(h, "tfind") == NULL ||
        dlsym(h, "hsearch") == NULL || dlsym(h, "lfind") == NULL ||
        dlsym(h, "error") == NULL || dlsym(h, "error_at_line") == NULL ||
        dlsym(h, "__memcpy_chk") == NULL || dlsym(h, "__strcpy_chk") == NULL ||
        dlsym(h, "__printf_chk") == NULL || dlsym(h, "__open_2") == NULL ||
        dlsym(h, "open64") == NULL || dlsym(h, "pread64") == NULL ||
        dlsym(h, "mmap64") == NULL || dlsym(h, "stat64") == NULL ||
        dlsym(h, "__xstat") == NULL || dlsym(h, "__isoc99_sscanf") == NULL ||
        dlsym(h, "__ctype_b_loc") == NULL || dlsym(h, "wcwidth") == NULL ||
        dlsym(h, "major") == NULL || dlsym(h, "makedev") == NULL ||
        dlsym(h, "mallinfo") == NULL || dlsym(h, "mallopt") == NULL ||
        dlsym(h, "pidfd_open") == NULL || dlsym(h, "a64l") == NULL ||
        dlsym(h, "l64a") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch9: %s\n", dlerror());
        (void)dlclose(h);
        return 27;
    }
    if (dlsym(h, "vfork") == NULL || dlsym(h, "clone") == NULL ||
        dlsym(h, "fexecve") == NULL || dlsym(h, "openat2") == NULL ||
        dlsym(h, "gettext") == NULL || dlsym(h, "dgettext") == NULL ||
        dlsym(h, "aio_read") == NULL || dlsym(h, "aio_write") == NULL ||
        dlsym(h, "aio_error") == NULL || dlsym(h, "prlimit64") == NULL ||
        dlsym(h, "getdents64") == NULL || dlsym(h, "mlock2") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch10: %s\n", dlerror());
        (void)dlclose(h);
        return 28;
    }
    if (dlsym(h, "argz_create") == NULL || dlsym(h, "argz_count") == NULL ||
        dlsym(h, "argz_add") == NULL || dlsym(h, "envz_add") == NULL ||
        dlsym(h, "envz_get") == NULL || dlsym(h, "call_once") == NULL ||
        dlsym(h, "mtx_init") == NULL || dlsym(h, "thrd_create") == NULL ||
        dlsym(h, "lrand48") == NULL || dlsym(h, "srand48") == NULL ||
        dlsym(h, "ether_aton") == NULL || dlsym(h, "ether_ntoa") == NULL ||
        dlsym(h, "closefrom") == NULL || dlsym(h, "cfree") == NULL ||
        dlsym(h, "__bzero") == NULL || dlsym(h, "alphasort64") == NULL ||
        dlsym(h, "fopen64") == NULL || dlsym(h, "arch_prctl") == NULL ||
        dlsym(h, "c16rtomb") == NULL || dlsym(h, "c32rtomb") == NULL ||
        dlsym(h, "bindresvport") == NULL || dlsym(h, "bsd_signal") == NULL ||
        dlsym(h, "__mempcpy") == NULL || dlsym(h, "clock") == NULL ||
        dlsym(h, "memfrob") == NULL || dlsym(h, "strfry") == NULL ||
        dlsym(h, "__cmsg_nxthdr") == NULL || dlsym(h, "__clone") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch11: %s\n", dlerror());
        (void)dlclose(h);
        return 29;
    }
    if (dlsym(h, "strerror_l") == NULL || dlsym(h, "strsignal_l") == NULL ||
        dlsym(h, "newfstatat") == NULL || dlsym(h, "memfd_secret") == NULL ||
        dlsym(h, "strerrorname_np") == NULL ||
        dlsym(h, "strerrordesc_np") == NULL ||
        dlsym(h, "sigabbrev_np") == NULL || dlsym(h, "sigdescr_np") == NULL ||
        dlsym(h, "__libc_malloc") == NULL || dlsym(h, "__libc_free") == NULL ||
        dlsym(h, "gnu_get_libc_version") == NULL ||
        dlsym(h, "__sysconf") == NULL || dlsym(h, "statfs64") == NULL ||
        dlsym(h, "__res_init") == NULL || dlsym(h, "__isoc23_strtol") == NULL ||
        dlsym(h, "__isoc23_sscanf") == NULL ||
        dlsym(h, "pthread_mutexattr_setrobust") == NULL ||
        dlsym(h, "pthread_mutex_consistent") == NULL ||
        dlsym(h, "sem_clockwait") == NULL || dlsym(h, "__longjmp_chk") == NULL ||
        dlsym(h, "_IO_getc") == NULL || dlsym(h, "__uflow") == NULL ||
        dlsym(h, "__open64_2") == NULL || dlsym(h, "__strndup") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch12: %s\n", dlerror());
        (void)dlclose(h);
        return 30;
    }
    if (dlsym(h, "__close") == NULL || dlsym(h, "__dup2") == NULL ||
        dlsym(h, "__fork") == NULL || dlsym(h, "_Fork") == NULL ||
        dlsym(h, "__getpid") == NULL || dlsym(h, "__gmtime_r") == NULL ||
        dlsym(h, "__ffs") == NULL || dlsym(h, "__getpagesize") == NULL ||
        dlsym(h, "__getauxval") == NULL || dlsym(h, "__fbufsize") == NULL ||
        dlsym(h, "__freadable") == NULL || dlsym(h, "__fwritable") == NULL ||
        dlsym(h, "__freading") == NULL || dlsym(h, "__fwriting") == NULL ||
        dlsym(h, "__fpending") == NULL || dlsym(h, "__fpurge") == NULL ||
        dlsym(h, "__flbf") == NULL || dlsym(h, "__fsetlocking") == NULL ||
        dlsym(h, "freopen64") == NULL || dlsym(h, "ftok") == NULL ||
        dlsym(h, "ftime") == NULL || dlsym(h, "dysize") == NULL ||
        dlsym(h, "dn_expand") == NULL || dlsym(h, "dn_comp") == NULL ||
        dlsym(h, "dn_skipname") == NULL || dlsym(h, "__dn_expand") == NULL ||
        dlsym(h, "gethostbyname_r") == NULL ||
        dlsym(h, "gethostbyname2") == NULL ||
        dlsym(h, "getservbyname_r") == NULL ||
        dlsym(h, "getpwent_r") == NULL || dlsym(h, "getgrent_r") == NULL ||
        dlsym(h, "fgetpwent") == NULL || dlsym(h, "fgetgrent") == NULL ||
        dlsym(h, "getnetent") == NULL || dlsym(h, "getnetbyname") == NULL ||
        dlsym(h, "setnetent") == NULL || dlsym(h, "endnetent") == NULL ||
        dlsym(h, "dladdr1") == NULL || dlsym(h, "dlinfo") == NULL ||
        dlsym(h, "dlmopen") == NULL || dlsym(h, "at_quick_exit") == NULL ||
        dlsym(h, "quick_exit") == NULL ||
        dlsym(h, "__cxa_at_quick_exit") == NULL ||
        dlsym(h, "__cxa_thread_atexit_impl") == NULL ||
        dlsym(h, "fmtmsg") == NULL ||
        dlsym(h, "__explicit_bzero_chk") == NULL ||
        dlsym(h, "__assert") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch13: %s\n", dlerror());
        (void)dlclose(h);
        return 31;
    }
    /* batch14: ucontext/cat/strfmon/aio/io_uring/obstack/argp/clocklocks */
    if (dlsym(h, "getcontext") == NULL || dlsym(h, "setcontext") == NULL ||
        dlsym(h, "makecontext") == NULL || dlsym(h, "swapcontext") == NULL ||
        dlsym(h, "catopen") == NULL || dlsym(h, "catgets") == NULL ||
        dlsym(h, "catclose") == NULL || dlsym(h, "strfmon") == NULL ||
        dlsym(h, "strfmon_l") == NULL || dlsym(h, "monstartup") == NULL ||
        dlsym(h, "profil") == NULL || dlsym(h, "_mcleanup") == NULL ||
        dlsym(h, "io_setup") == NULL || dlsym(h, "io_submit") == NULL ||
        dlsym(h, "io_getevents") == NULL || dlsym(h, "io_destroy") == NULL ||
        dlsym(h, "io_cancel") == NULL || dlsym(h, "io_uring_setup") == NULL ||
        dlsym(h, "io_uring_enter") == NULL ||
        dlsym(h, "io_uring_register") == NULL ||
        dlsym(h, "obstack_free") == NULL || dlsym(h, "_obstack_begin") == NULL ||
        dlsym(h, "argp_parse") == NULL || dlsym(h, "argp_help") == NULL ||
        dlsym(h, "parse_printf_format") == NULL ||
        dlsym(h, "register_printf_function") == NULL ||
        dlsym(h, "fopencookie") == NULL || dlsym(h, "twalk_r") == NULL ||
        dlsym(h, "wcstoimax") == NULL || dlsym(h, "wcstoumax") == NULL ||
        dlsym(h, "pthread_mutex_clocklock") == NULL ||
        dlsym(h, "pthread_cond_clockwait") == NULL ||
        dlsym(h, "pthread_rwlock_clockrdlock") == NULL ||
        dlsym(h, "sigqueue") == NULL || dlsym(h, "pthread_sigqueue") == NULL ||
        dlsym(h, "get_nprocs") == NULL || dlsym(h, "getpeereid") == NULL ||
        dlsym(h, "killpg") == NULL || dlsym(h, "sysctl") == NULL ||
        dlsym(h, "fflush_unlocked") == NULL ||
        dlsym(h, "versionsort64") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch14: %s\n", dlerror());
        (void)dlclose(h);
        return 32;
    }
    /* batch15: aliases, cancel, ns_*, re_*, locale _l, getdate */
    if (dlsym(h, "__open") == NULL || dlsym(h, "__read") == NULL ||
        dlsym(h, "__write") == NULL || dlsym(h, "getrlimit64") == NULL ||
        dlsym(h, "sendfile64") == NULL || dlsym(h, "sigaltstack") == NULL ||
        dlsym(h, "pthread_setcancelstate") == NULL ||
        dlsym(h, "pthread_cancel") == NULL ||
        dlsym(h, "ns_initparse") == NULL || dlsym(h, "res_ninit") == NULL ||
        dlsym(h, "sockatmark") == NULL || dlsym(h, "ptrace") == NULL ||
        dlsym(h, "re_comp") == NULL || dlsym(h, "isascii") == NULL ||
        dlsym(h, "mbrlen") == NULL || dlsym(h, "wcpcpy") == NULL ||
        dlsym(h, "pvalloc") == NULL || dlsym(h, "getdate_r") == NULL ||
        dlsym(h, "encrypt") == NULL || dlsym(h, "ntp_gettime") == NULL ||
        dlsym(h, "timelocal") == NULL || dlsym(h, "isalnum_l") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch15: %s\n", dlerror());
        (void)dlclose(h);
        return 33;
    }
    /* batch16: more underscored + fortify + LFS + pthread aliases */
    if (dlsym(h, "__socket") == NULL || dlsym(h, "__bind") == NULL ||
        dlsym(h, "__mmap") == NULL || dlsym(h, "__libc_open") == NULL ||
        dlsym(h, "__libc_read") == NULL || dlsym(h, "__secure_getenv") == NULL ||
        dlsym(h, "__strtol_internal") == NULL ||
        dlsym(h, "__pthread_mutex_lock") == NULL ||
        dlsym(h, "__pthread_create") == NULL ||
        dlsym(h, "__isoc99_printf") == NULL ||
        dlsym(h, "__asprintf_chk") == NULL ||
        dlsym(h, "__syslog_chk") == NULL || dlsym(h, "_setjmp") == NULL ||
        dlsym(h, "__sigsetjmp") == NULL || dlsym(h, "fseeko64") == NULL ||
        dlsym(h, "ftello64") == NULL || dlsym(h, "readdir64_r") == NULL ||
        dlsym(h, "__getpwnam_r") == NULL ||
        dlsym(h, "__if_nametoindex") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch16: %s\n", dlerror());
        (void)dlclose(h);
        return 34;
    }
    /* batch17: fts64, fstab, getpw/gets, fortify path, _IO_*, pthread more */
    if (dlsym(h, "fts64_open") == NULL || dlsym(h, "fts64_read") == NULL ||
        dlsym(h, "statvfs64") == NULL || dlsym(h, "fstatvfs64") == NULL ||
        dlsym(h, "__fseeko64") == NULL || dlsym(h, "__ftello64") == NULL ||
        dlsym(h, "fgetpwent_r") == NULL || dlsym(h, "fgetgrent_r") == NULL ||
        dlsym(h, "getpw") == NULL || dlsym(h, "gets") == NULL ||
        dlsym(h, "getfsent") == NULL || dlsym(h, "setfsent") == NULL ||
        dlsym(h, "_flushlbf") == NULL || dlsym(h, "_IO_fopen") == NULL ||
        dlsym(h, "_IO_fclose") == NULL || dlsym(h, "fgetwc_unlocked") == NULL ||
        dlsym(h, "__readlink_chk") == NULL ||
        dlsym(h, "__libc_memalign") == NULL ||
        dlsym(h, "__libc_secure_getenv") == NULL ||
        dlsym(h, "__sigaddset") == NULL ||
        dlsym(h, "__pthread_mutex_init") == NULL ||
        dlsym(h, "__strcasestr") == NULL || dlsym(h, "freezero") == NULL ||
        dlsym(h, "__default_morecore") == NULL ||
        dlsym(h, "__openat_2") == NULL ||
        dlsym(h, "__wmemcpy_chk") == NULL ||
        dlsym(h, "timegm_r") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch17: %s\n", dlerror());
        (void)dlclose(h);
        return 35;
    }
    /* batch18: syscall, BSD fls/strtonum/heapsort, sched_setattr, fsopen */
    if (dlsym(h, "syscall") == NULL || dlsym(h, "fls") == NULL ||
        dlsym(h, "flsl") == NULL || dlsym(h, "flsll") == NULL ||
        dlsym(h, "getprogname") == NULL || dlsym(h, "setprogname") == NULL ||
        dlsym(h, "recallocarray") == NULL || dlsym(h, "reallocf") == NULL ||
        dlsym(h, "strtonum") == NULL || dlsym(h, "issetugid") == NULL ||
        dlsym(h, "strmode") == NULL || dlsym(h, "heapsort") == NULL ||
        dlsym(h, "mergesort") == NULL || dlsym(h, "__res_state") == NULL ||
        dlsym(h, "sched_setattr") == NULL ||
        dlsym(h, "sched_getattr") == NULL ||
        dlsym(h, "perf_event_open") == NULL || dlsym(h, "fsopen") == NULL ||
        dlsym(h, "fsconfig") == NULL || dlsym(h, "getdents") == NULL ||
        dlsym(h, "__execve") == NULL || dlsym(h, "__gmon_start__") == NULL ||
        dlsym(h, "_dl_find_object") == NULL ||
        dlsym(h, "mkstemp64") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch18: %s\n", dlerror());
        (void)dlclose(h);
        return 36;
    }
    /* batch19: LFS preadv64/llseek, mktemp, pthread kind_np, DNS, gai_a */
    if (dlsym(h, "preadv64") == NULL || dlsym(h, "pwritev64") == NULL ||
        dlsym(h, "llseek") == NULL || dlsym(h, "_llseek") == NULL ||
        dlsym(h, "scandirat64") == NULL || dlsym(h, "mktemp") == NULL ||
        dlsym(h, "mkostemps64") == NULL || dlsym(h, "_longjmp") == NULL ||
        dlsym(h, "pthread_setconcurrency") == NULL ||
        dlsym(h, "pthread_getconcurrency") == NULL ||
        dlsym(h, "pthread_mutexattr_setkind_np") == NULL ||
        dlsym(h, "ns_name_pack") == NULL || dlsym(h, "ns_name_unpack") == NULL ||
        dlsym(h, "res_hnok") == NULL || dlsym(h, "res_dnok") == NULL ||
        dlsym(h, "ntp_gettimex") == NULL || dlsym(h, "lio_listio") == NULL ||
        dlsym(h, "getaddrinfo_a") == NULL || dlsym(h, "gai_error") == NULL ||
        dlsym(h, "modify_ldt") == NULL || dlsym(h, "__libc_freeres") == NULL ||
        dlsym(h, "mcount") == NULL || dlsym(h, "__fentry__") == NULL ||
        dlsym(h, "re_search_2") == NULL || dlsym(h, "inet_nsap_addr") == NULL ||
        dlsym(h, "strtoimax_l") == NULL || dlsym(h, "chflags") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch19: %s\n", dlerror());
        (void)dlclose(h);
        return 37;
    }
    /* batch20: utime/madvise64, moncontrol, getut*_r, mcheck, libc init */
    if (dlsym(h, "utime") == NULL || dlsym(h, "madvise64") == NULL ||
        dlsym(h, "lockf64") == NULL || dlsym(h, "__memmem") == NULL ||
        dlsym(h, "moncontrol") == NULL ||
        dlsym(h, "__libc_dlopen_mode") == NULL ||
        dlsym(h, "getutent_r") == NULL || dlsym(h, "getsgent_r") == NULL ||
        dlsym(h, "getaliasent_r") == NULL ||
        dlsym(h, "__wprintf_chk") == NULL || dlsym(h, "isctype") == NULL ||
        dlsym(h, "mq_getsetattr") == NULL || dlsym(h, "mcheck") == NULL ||
        dlsym(h, "mtrace") == NULL || dlsym(h, "__libc_init_first") == NULL ||
        dlsym(h, "__libc_fcntl64") == NULL ||
        dlsym(h, "__ns_name_compress") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch20: %s\n", dlerror());
        (void)dlclose(h);
        return 38;
    }
    /* batch21: underscored path/cred aliases, wchar fortify, nss/resolv */
    if (dlsym(h, "__memrchr") == NULL || dlsym(h, "__strchrnul") == NULL ||
        dlsym(h, "__strsep") == NULL || dlsym(h, "__copy_file_range") == NULL ||
        dlsym(h, "__close_range") == NULL || dlsym(h, "__getentropy") == NULL ||
        dlsym(h, "__getrandom") == NULL || dlsym(h, "__pread64") == NULL ||
        dlsym(h, "__lseek") == NULL || dlsym(h, "__chmod") == NULL ||
        dlsym(h, "__getppid") == NULL || dlsym(h, "__kill") == NULL ||
        dlsym(h, "__raise") == NULL || dlsym(h, "__fsync") == NULL ||
        dlsym(h, "__mlock") == NULL || dlsym(h, "__setenv") == NULL ||
        dlsym(h, "__posix_spawn") == NULL || dlsym(h, "__wcscpy_chk") == NULL ||
        dlsym(h, "__wcsncpy_chk") == NULL || dlsym(h, "obstack_printf") == NULL ||
        dlsym(h, "__obstack_printf_chk") == NULL ||
        dlsym(h, "__res_mkquery") == NULL ||
        dlsym(h, "res_nquerydomain") == NULL ||
        dlsym(h, "__nss_configure_lookup") == NULL ||
        dlsym(h, "ns_samedomain") == NULL || dlsym(h, "ns_makecanon") == NULL ||
        dlsym(h, "__exit_group") == NULL || dlsym(h, "__sigprocmask") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch21: %s\n", dlerror());
        (void)dlclose(h);
        return 39;
    }
    /* batch22: fpurge/sig2str/b64/nss_lookup2/libc_socket */
    if (dlsym(h, "fpurge") == NULL || dlsym(h, "sig2str") == NULL ||
        dlsym(h, "str2sig") == NULL || dlsym(h, "b64_ntop") == NULL ||
        dlsym(h, "b64_pton") == NULL || dlsym(h, "res_randomid") == NULL ||
        dlsym(h, "dn_count_labels") == NULL || dlsym(h, "p_type") == NULL ||
        dlsym(h, "ns_format_ttl") == NULL || dlsym(h, "ns_parse_ttl") == NULL ||
        dlsym(h, "res_hostalias") == NULL ||
        dlsym(h, "__nss_passwd_lookup2") == NULL ||
        dlsym(h, "__sched_setaffinity_new") == NULL ||
        dlsym(h, "__setxattr") == NULL || dlsym(h, "__libc_socket") == NULL ||
        dlsym(h, "__libc_connect") == NULL ||
        dlsym(h, "__libc_sendmsg") == NULL ||
        dlsym(h, "__libc_pause") == NULL || dlsym(h, "__libc_fsync") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch22: %s\n", dlerror());
        (void)dlclose(h);
        return 40;
    }
    /* batch23: fortify wctomb, inet6_option, RPC/XDR surface */
    if (dlsym(h, "__wctomb_chk") == NULL || dlsym(h, "__mbrtowc_chk") == NULL ||
        dlsym(h, "inet6_option_space") == NULL ||
        dlsym(h, "inet6_option_init") == NULL || dlsym(h, "gtty") == NULL ||
        dlsym(h, "stty") == NULL || dlsym(h, "revoke") == NULL ||
        dlsym(h, "__libc_dynarray_resize") == NULL ||
        dlsym(h, "__libc_scratch_buffer_grow") == NULL ||
        dlsym(h, "__nss_database_get") == NULL ||
        dlsym(h, "_nss_files_getpwnam_r") == NULL ||
        dlsym(h, "_nss_dns_gethostbyname_r") == NULL ||
        dlsym(h, "getrpcent") == NULL || dlsym(h, "xdrmem_create") == NULL ||
        dlsym(h, "xdr_int") == NULL || dlsym(h, "clnt_create") == NULL ||
        dlsym(h, "authnone_create") == NULL || dlsym(h, "svcudp_create") == NULL ||
        dlsym(h, "cbc_crypt") == NULL || dlsym(h, "ecb_crypt") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch23: %s\n", dlerror());
        (void)dlclose(h);
        return 41;
    }
    /* batch24: strtok/timespec_get/authdes/fgetln/cxa_guard */
    if (dlsym(h, "strtok") == NULL || dlsym(h, "timespec_get") == NULL ||
        dlsym(h, "timespec_getres") == NULL || dlsym(h, "setlogin") == NULL ||
        dlsym(h, "setproctitle") == NULL || dlsym(h, "siginterrupt") == NULL ||
        dlsym(h, "sigvec") == NULL || dlsym(h, "res_nmkquery") == NULL ||
        dlsym(h, "res_nsend") == NULL || dlsym(h, "authdes_create") == NULL ||
        dlsym(h, "clnt_broadcast") == NULL || dlsym(h, "xdr_quad_t") == NULL ||
        dlsym(h, "svcraw_create") == NULL || dlsym(h, "ustat") == NULL ||
        dlsym(h, "fgetln") == NULL || dlsym(h, "strnstr") == NULL ||
        dlsym(h, "timingsafe_bcmp") == NULL ||
        dlsym(h, "getipnodebyname") == NULL ||
        dlsym(h, "freehostent") == NULL ||
        dlsym(h, "__cxa_guard_acquire") == NULL ||
        dlsym(h, "__cxa_pure_virtual") == NULL ||
        dlsym(h, "__reallocarray") == NULL ||
        dlsym(h, "__wcrtomb_chk") == NULL || dlsym(h, "tmpnam64") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch24: %s\n", dlerror());
        (void)dlclose(h);
        return 42;
    }
    /* batch25: underscored snprintf/resolv/nss_files/alloc_buffer */
    if (dlsym(h, "__snprintf") == NULL || dlsym(h, "__vsnprintf") == NULL ||
        dlsym(h, "__strerror_r") == NULL || dlsym(h, "__strftime_l") == NULL ||
        dlsym(h, "__iswctype") == NULL || dlsym(h, "__posix_getopt") == NULL ||
        dlsym(h, "__res_search") == NULL || dlsym(h, "__libc_dn_expand") == NULL ||
        dlsym(h, "__strsep_g") == NULL || dlsym(h, "__strcspn_c1") == NULL ||
        dlsym(h, "_pthread_cleanup_push") == NULL ||
        dlsym(h, "__lll_lock_wait_private") == NULL ||
        dlsym(h, "__libc_alloc_buffer_allocate") == NULL ||
        dlsym(h, "printf_size") == NULL ||
        dlsym(h, "_nss_files_getpwent_r") == NULL ||
        dlsym(h, "__inet_aton_exact") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch25: %s\n", dlerror());
        (void)dlclose(h);
        return 43;
    }
    /* batch26: strspn_c, strtok_r, tree, nss_hash, file_change, fortify wide */
    if (dlsym(h, "__strspn_c1") == NULL || dlsym(h, "__strtok_r") == NULL ||
        dlsym(h, "_tolower") == NULL || dlsym(h, "_toupper") == NULL ||
        dlsym(h, "__strtol_l") == NULL || dlsym(h, "__tdelete") == NULL ||
        dlsym(h, "__tsearch") == NULL || dlsym(h, "__towctrans") == NULL ||
        dlsym(h, "__wcpcpy_chk") == NULL || dlsym(h, "__wmempcpy_chk") == NULL ||
        dlsym(h, "__write_nocancel") == NULL || dlsym(h, "__nss_hash") == NULL ||
        dlsym(h, "__nss_passwd_lookup") == NULL ||
        dlsym(h, "__nss_files_fopen") == NULL ||
        dlsym(h, "__file_change_detection_for_path") == NULL ||
        dlsym(h, "__file_is_unchanged") == NULL ||
        dlsym(h, "__x86_get_cpuid_feature_leaf") == NULL ||
        dlsym(h, "_environ") == NULL || dlsym(h, "__vfscanf") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch26: %s\n", dlerror());
        (void)dlclose(h);
        return 44;
    }
    /* batch27: memcmpeq, nocancel, gconv/iconv, _IO_* FILE surface */
    if (dlsym(h, "__memcmpeq") == NULL || dlsym(h, "__strchr_c") == NULL ||
        dlsym(h, "__wcsnlen") == NULL || dlsym(h, "__isoc23_strtol_l") == NULL ||
        dlsym(h, "__morecore") == NULL ||
        dlsym(h, "__fcntl_nocancel") == NULL ||
        dlsym(h, "__waitpid_nocancel") == NULL ||
        dlsym(h, "__openat64_nocancel") == NULL ||
        dlsym(h, "__poll_nocancel") == NULL ||
        dlsym(h, "__accept_nocancel") == NULL ||
        dlsym(h, "__gconv_open") == NULL || dlsym(h, "__iconv_open") == NULL ||
        dlsym(h, "_IO_vfscanf") == NULL || dlsym(h, "_IO_sscanf") == NULL ||
        dlsym(h, "_IO_getline") == NULL ||
        dlsym(h, "_IO_peekc_locked") == NULL ||
        dlsym(h, "_IO_file_init") == NULL ||
        dlsym(h, "_IO_file_underflow") == NULL ||
        dlsym(h, "__printf_fphex") == NULL ||
        dlsym(h, "__vfprintf_internal") == NULL ||
        dlsym(h, "__malloc_usable_size") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch27: %s\n", dlerror());
        (void)dlclose(h);
        return 45;
    }
    /* batch28: mseal / shadow stack / mount enum / LSM / consttime */
    if (dlsym(h, "mseal") == NULL || dlsym(h, "map_shadow_stack") == NULL ||
        dlsym(h, "listmount") == NULL || dlsym(h, "statmount") == NULL ||
        dlsym(h, "lsm_get_self_attr") == NULL ||
        dlsym(h, "lsm_list_modules") == NULL ||
        dlsym(h, "futex_requeue") == NULL ||
        dlsym(h, "set_robust_list") == NULL ||
        dlsym(h, "consttime_memequal") == NULL ||
        dlsym(h, "__mseal") == NULL ||
        dlsym(h, "__consttime_memequal") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch28: %s\n", dlerror());
        (void)dlclose(h);
        return 46;
    }
    /* batch29: thr_*, cap_*, clnt_*, text_domain */
    if (dlsym(h, "thr_create") == NULL || dlsym(h, "thr_self") == NULL ||
        dlsym(h, "cap_init") == NULL || dlsym(h, "cap_get_proc") == NULL ||
        dlsym(h, "clnt_call") == NULL || dlsym(h, "text_domain") == NULL ||
        dlsym(h, "cookie_read") == NULL ||
        dlsym(h, "_dl_allocate_tls") == NULL ||
        dlsym(h, "__libcgj_batch29_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch29: %s\n", dlerror());
        (void)dlclose(h);
        return 47;
    }
    /* batch30: NUMA / futex_wait / wchar public / BSD helpers */
    if (dlsym(h, "set_mempolicy") == NULL || dlsym(h, "get_mempolicy") == NULL ||
        dlsym(h, "mbind") == NULL || dlsym(h, "migrate_pages") == NULL ||
        dlsym(h, "futex_wait") == NULL || dlsym(h, "futex_wake") == NULL ||
        dlsym(h, "wcschrnul") == NULL || dlsym(h, "wcswcs") == NULL ||
        dlsym(h, "wcstol_l") == NULL || dlsym(h, "__vsscanf") == NULL ||
        dlsym(h, "explicit_memset") == NULL ||
        dlsym(h, "humanize_number") == NULL ||
        dlsym(h, "pidfile_open") == NULL || dlsym(h, "fadvise64") == NULL ||
        dlsym(h, "arc4random_stir") == NULL ||
        dlsym(h, "__vdso_clock_gettime") == NULL ||
        dlsym(h, "__libcgj_batch30_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch30: %s\n", dlerror());
        (void)dlclose(h);
        return 48;
    }
    /* batch31: libmd digest completion + unvis */
    if (dlsym(h, "MD5Update") == NULL || dlsym(h, "MD5Final") == NULL ||
        dlsym(h, "MD5End") == NULL || dlsym(h, "MD5Data") == NULL ||
        dlsym(h, "SHA1Update") == NULL || dlsym(h, "SHA1Final") == NULL ||
        dlsym(h, "SHA256_Update") == NULL || dlsym(h, "SHA256_Final") == NULL ||
        dlsym(h, "SHA512_Init") == NULL || dlsym(h, "SHA512_Update") == NULL ||
        dlsym(h, "SHA512_Final") == NULL || dlsym(h, "strunvis") == NULL ||
        dlsym(h, "unvis") == NULL ||
        dlsym(h, "__libcgj_batch31_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch31: %s\n", dlerror());
        (void)dlclose(h);
        return 49;
    }
    /* batch32: remaining GI + BSD vis + libmd */
    if (dlsym(h, "__GI_sigprocmask") == NULL || dlsym(h, "__GI_dlopen") == NULL ||
        dlsym(h, "__GI_waitpid") == NULL || dlsym(h, "__GI_poll") == NULL ||
        dlsym(h, "stravis") == NULL || dlsym(h, "strnvis") == NULL ||
        dlsym(h, "MD5Init") == NULL || dlsym(h, "getgrouplist_2") == NULL ||
        dlsym(h, "pidfd_getfd_np") == NULL ||
        dlsym(h, "__libcgj_batch32_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch32: %s\n", dlerror());
        (void)dlclose(h);
        return 50;
    }
    /* batch33: thr prio / time64 / getrandom_r */
    if (dlsym(h, "thr_setprio") == NULL || dlsym(h, "thr_getprio") == NULL ||
        dlsym(h, "thr_keycreate_once") == NULL ||
        dlsym(h, "getrandom_r") == NULL ||
        dlsym(h, "__clock_gettime64") == NULL ||
        dlsym(h, "ppoll_time64") == NULL ||
        dlsym(h, "__mmap64") == NULL ||
        dlsym(h, "__libcgj_batch33_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch33: %s\n", dlerror());
        (void)dlclose(h);
        return 51;
    }
    /* batch34: MD4/BLAKE2s + remaining BSD vis + bcrypt/netconfig */
    if (dlsym(h, "MD4Init") == NULL || dlsym(h, "MD4Update") == NULL ||
        dlsym(h, "MD4Final") == NULL || dlsym(h, "RMD160Init") == NULL ||
        dlsym(h, "BLAKE2s_Init") == NULL || dlsym(h, "BLAKE2s_Final") == NULL ||
        dlsym(h, "bcrypt") == NULL || dlsym(h, "strtofflags") == NULL ||
        dlsym(h, "setnetconfig") == NULL ||
        dlsym(h, "copy_file_range64") == NULL || dlsym(h, "nvis") == NULL ||
        dlsym(h, "svis") == NULL || dlsym(h, "strsvis") == NULL ||
        dlsym(h, "strunvisx") == NULL ||
        dlsym(h, "__libcgj_batch34_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch34: %s\n", dlerror());
        (void)dlclose(h);
        return 52;
    }
    /* batch35: BSD UUID + strtoi/strtou + gethrtime */
    if (dlsym(h, "uuid_create") == NULL || dlsym(h, "uuid_to_string") == NULL ||
        dlsym(h, "uuid_from_string") == NULL ||
        dlsym(h, "strtoi") == NULL || dlsym(h, "strtou") == NULL ||
        dlsym(h, "memcasecmp") == NULL || dlsym(h, "gethrtime") == NULL ||
        dlsym(h, "basename_r") == NULL || dlsym(h, "dirname_r") == NULL ||
        dlsym(h, "__libcgj_batch35_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch35: %s\n", dlerror());
        (void)dlclose(h);
        return 53;
    }
    /* batch36: libuuid + digest End + siphash + radixsort */
    if (dlsym(h, "uuid_generate") == NULL || dlsym(h, "uuid_unparse") == NULL ||
        dlsym(h, "uuid_clear") == NULL || dlsym(h, "uuid_copy") == NULL ||
        dlsym(h, "uuid_parse") == NULL || dlsym(h, "MD4End") == NULL ||
        dlsym(h, "MD4Data") == NULL || dlsym(h, "BLAKE2s_Data") == NULL ||
        dlsym(h, "siphash24") == NULL || dlsym(h, "radixsort") == NULL ||
        dlsym(h, "dehumanize_number") == NULL ||
        dlsym(h, "__libcgj_batch36_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch36: %s\n", dlerror());
        (void)dlclose(h);
        return 54;
    }
    /* batch37: fortify preadv + qecvt stubs */
    if (dlsym(h, "__preadv_chk") == NULL || dlsym(h, "__pwritev_chk") == NULL ||
        dlsym(h, "__creat64") == NULL || dlsym(h, "qecvt") == NULL ||
        dlsym(h, "fcvt") == NULL ||
        dlsym(h, "__libcgj_batch37_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch37: %s\n", dlerror());
        (void)dlclose(h);
        return 55;
    }
    /* batch38: libuuid remainder + HMAC + BLAKE2b + SHA3-256 */
    if (dlsym(h, "uuid_type") == NULL || dlsym(h, "uuid_variant") == NULL ||
        dlsym(h, "uuid_time") == NULL ||
        dlsym(h, "uuid_generate_md5") == NULL ||
        dlsym(h, "uuid_generate_sha1") == NULL ||
        dlsym(h, "hmac_sha1") == NULL || dlsym(h, "hmac_sha256") == NULL ||
        dlsym(h, "BLAKE2b_Init") == NULL || dlsym(h, "BLAKE2b_Final") == NULL ||
        dlsym(h, "BLAKE2b_Data") == NULL ||
        dlsym(h, "SHA3_256_Init") == NULL ||
        dlsym(h, "SHA3_256_Final") == NULL ||
        dlsym(h, "SHA3_256_Data") == NULL ||
        dlsym(h, "__libcgj_batch38_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch38: %s\n", dlerror());
        (void)dlclose(h);
        return 56;
    }
    /* batch39: HMAC-MD5/SHA512 + crc32 + SHA384 */
    if (dlsym(h, "hmac_md5") == NULL || dlsym(h, "hmac_sha512") == NULL ||
        dlsym(h, "SHA384_Init") == NULL || dlsym(h, "crc32_z") == NULL ||
        dlsym(h, "crc32c") == NULL || dlsym(h, "adler32") == NULL ||
        dlsym(h, "__libcgj_batch39_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch39: %s\n", dlerror());
        (void)dlclose(h);
        return 55;
    }
    /* batch40: SHA3 family + SHA224 + SHA512/t + XXH + HMAC-SHA384 +
     * HKDF/PBKDF2/base64 */
    if (dlsym(h, "SHA3_224_Init") == NULL || dlsym(h, "SHA3_384_Data") == NULL ||
        dlsym(h, "SHA3_512_Final") == NULL || dlsym(h, "SHA224_Data") == NULL ||
        dlsym(h, "SHA512_224_Init") == NULL ||
        dlsym(h, "SHA512_256_Data") == NULL || dlsym(h, "hmac_sha384") == NULL ||
        dlsym(h, "XXH32") == NULL || dlsym(h, "XXH64") == NULL ||
        dlsym(h, "hkdf_sha256") == NULL || dlsym(h, "pbkdf2_hmac_sha256") == NULL ||
        dlsym(h, "base64_encode") == NULL || dlsym(h, "SHA512_256_Init") == NULL ||
        dlsym(h, "__libcgj_batch40_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch40: %s\n", dlerror());
        (void)dlclose(h);
        return 57;
    }
    /* batch41: SHAKE + base32/hex/FNV/murmur/XXH3/HMAC/CRC/crypto_memcmp */
    if (dlsym(h, "base32_encode") == NULL || dlsym(h, "hex_encode") == NULL ||
        dlsym(h, "fnv1a_32") == NULL || dlsym(h, "murmur3_32") == NULL ||
        dlsym(h, "XXH3_64bits") == NULL || dlsym(h, "hmac_sha224") == NULL ||
        dlsym(h, "crypto_memcmp") == NULL ||
        dlsym(h, "SHAKE128_Data") == NULL || dlsym(h, "SHAKE256_Final") == NULL ||
        dlsym(h, "hmac_sha3_256") == NULL || dlsym(h, "crc16") == NULL ||
        dlsym(h, "crc64") == NULL || dlsym(h, "base32_decode") == NULL ||
        dlsym(h, "__libcgj_batch41_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch41: %s\n", dlerror());
        (void)dlclose(h);
        return 58;
    }
    /* batch42: SM3 + ChaCha20/Poly1305 + base58 + CRC8/MODBUS + HMAC-SHA3-512 */
    if (dlsym(h, "SM3_Data") == NULL || dlsym(h, "SM3_Init") == NULL ||
        dlsym(h, "chacha20_xor") == NULL || dlsym(h, "chacha20_block") == NULL ||
        dlsym(h, "poly1305_auth") == NULL || dlsym(h, "base58_encode") == NULL ||
        dlsym(h, "base58_decode") == NULL ||
        dlsym(h, "base32hex_encode") == NULL || dlsym(h, "crc8") == NULL ||
        dlsym(h, "crc16_modbus") == NULL || dlsym(h, "hmac_sha3_512") == NULL ||
        dlsym(h, "djb2_hash") == NULL || dlsym(h, "sdbm_hash") == NULL ||
        dlsym(h, "__libcgj_batch42_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch42: %s\n", dlerror());
        (void)dlclose(h);
        return 59;
    }

    /* batch43: ChaCha20 encrypt + HChaCha20 + XChaCha20 */
    if (dlsym(h, "chacha20_encrypt") == NULL || dlsym(h, "hchacha20") == NULL ||
        dlsym(h, "xchacha20_xor") == NULL ||
        dlsym(h, "__libcgj_batch43_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch43: %s\n", dlerror());
        (void)dlclose(h);
        return 60;
    }
    /* batch44: MD2 + sm3 */
    if (dlsym(h, "MD2Init") == NULL || dlsym(h, "MD2Update") == NULL ||
        dlsym(h, "MD2Final") == NULL || dlsym(h, "MD2Data") == NULL ||
        dlsym(h, "sm3_init") == NULL || dlsym(h, "sm3_update") == NULL ||
        dlsym(h, "sm3_final") == NULL || dlsym(h, "sm3_hash") == NULL ||
        dlsym(h, "__libcgj_batch44_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch44: %s\n", dlerror());
        (void)dlclose(h);
        return 61;
    }
    /* batch45: AES-128 + RC4 + Blowfish */
    if (dlsym(h, "AES_set_encrypt_key") == NULL ||
        dlsym(h, "AES_encrypt") == NULL || dlsym(h, "AES_decrypt") == NULL ||
        dlsym(h, "RC4_set_key") == NULL || dlsym(h, "RC4") == NULL ||
        dlsym(h, "Blowfish_initstate") == NULL ||
        dlsym(h, "Blowfish_expand0state") == NULL ||
        dlsym(h, "Blowfish_encrypt") == NULL ||
        dlsym(h, "__libcgj_batch45_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch45: %s\n", dlerror());
        (void)dlclose(h);
        return 62;
    }
    /* batch50: AES_ecb_encrypt_block convenience */
    if (dlsym(h, "AES_ecb_encrypt_block") == NULL ||
        dlsym(h, "__libcgj_batch50_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch50: %s\n", dlerror());
        (void)dlclose(h);
        return 66;
    }
    /* batch45/50 behavioral KATs (soft if missing, hard if present but wrong) */
    {
        typedef struct {
            unsigned int rd_key[4 * (14 + 1)];
            int rounds;
        } AES_KEY;
        typedef struct {
            unsigned int x, y;
            unsigned int data[256];
        } RC4_KEY;
        typedef struct {
            unsigned int S[4][256];
            unsigned int P[18];
        } blf_ctx;
        typedef int (*aes_set_fn)(const unsigned char *, int, AES_KEY *);
        typedef void (*aes_enc_fn)(const unsigned char *, unsigned char *,
                                   const AES_KEY *);
        typedef int (*aes_ecb_fn)(const unsigned char *, unsigned char *,
                                  const unsigned char *, int);
        typedef void (*rc4_set_fn)(RC4_KEY *, int, const unsigned char *);
        typedef void (*rc4_fn)(RC4_KEY *, size_t, const unsigned char *,
                               unsigned char *);
        typedef void (*bf_init_fn)(blf_ctx *);
        typedef void (*bf_exp_fn)(blf_ctx *, const unsigned char *,
                                  unsigned short);
        typedef void (*bf_enc_fn)(blf_ctx *, unsigned int *, unsigned short);

        aes_set_fn pAesSet = (aes_set_fn)dlsym(h, "AES_set_encrypt_key");
        aes_enc_fn pAesEnc = (aes_enc_fn)dlsym(h, "AES_encrypt");
        aes_ecb_fn pAesEcb = (aes_ecb_fn)dlsym(h, "AES_ecb_encrypt_block");
        rc4_set_fn pRc4Set = (rc4_set_fn)dlsym(h, "RC4_set_key");
        rc4_fn pRc4 = (rc4_fn)dlsym(h, "RC4");
        bf_init_fn pBfInit = (bf_init_fn)dlsym(h, "Blowfish_initstate");
        bf_exp_fn pBfExp = (bf_exp_fn)dlsym(h, "Blowfish_expand0state");
        bf_enc_fn pBfEnc = (bf_enc_fn)dlsym(h, "Blowfish_encrypt");

        static const unsigned char aFipsCt[16] = {
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};
        static const unsigned char aRc4Exp[9] = {0xbb, 0xf3, 0x16, 0xe8, 0xd9,
                                                 0x40, 0xaf, 0x0a, 0xd3};
        unsigned char aKey[16];
        unsigned char aPt[16];
        unsigned char aCt[16];
        int i;

        for (i = 0; i < 16; i++) {
            aKey[i] = (unsigned char)i;
            aPt[i] = (unsigned char)(i * 0x11);
        }

        /* AES_set_encrypt_key + AES_encrypt — FIPS-197 C.1 */
        if (pAesSet != NULL && pAesEnc != NULL) {
            AES_KEY stKey;

            if (pAesSet(aKey, 128, &stKey) != 0 || stKey.rounds != 10) {
                fprintf(stderr, "libcgj: dyn AES_set_encrypt_key KAT fail\n");
                (void)dlclose(h);
                return 67;
            }
            pAesEnc(aPt, aCt, &stKey);
            if (memcmp(aCt, aFipsCt, 16) != 0) {
                fprintf(stderr, "libcgj: dyn AES_encrypt FIPS-197 C.1 fail\n");
                (void)dlclose(h);
                return 68;
            }
        }

        /* AES_ecb_encrypt_block one-shot — same vector */
        if (pAesEcb != NULL) {
            if (pAesEcb(aPt, aCt, aKey, 128) != 0 ||
                memcmp(aCt, aFipsCt, 16) != 0) {
                fprintf(stderr, "libcgj: dyn AES_ecb_encrypt_block KAT fail\n");
                (void)dlclose(h);
                return 69;
            }
        }

        /* RC4 short known vector (Key / Plaintext) */
        if (pRc4Set != NULL && pRc4 != NULL) {
            RC4_KEY stRc4;
            unsigned char aOut[9];

            pRc4Set(&stRc4, 3, (const unsigned char *)"Key");
            pRc4(&stRc4, 9, (const unsigned char *)"Plaintext", aOut);
            if (memcmp(aOut, aRc4Exp, 9) != 0) {
                fprintf(stderr, "libcgj: dyn RC4 KAT fail\n");
                (void)dlclose(h);
                return 70;
            }
        }

        /* Blowfish zero-key zero-block (optional; hard if present but wrong) */
        if (pBfInit != NULL && pBfExp != NULL && pBfEnc != NULL) {
            blf_ctx stBf;
            unsigned int aBf[2];
            unsigned char aBfKey[8];

            for (i = 0; i < 8; i++) {
                aBfKey[i] = 0;
            }
            pBfInit(&stBf);
            pBfExp(&stBf, aBfKey, 8);
            aBf[0] = 0;
            aBf[1] = 0;
            pBfEnc(&stBf, aBf, 1);
            if (aBf[0] != 0x4ef99745u || aBf[1] != 0x6198dd78u) {
                fprintf(stderr, "libcgj: dyn Blowfish KAT fail\n");
                (void)dlclose(h);
                return 71;
            }
        }
    }
    /* continuum soft KATs: tea / SM4 / x25519 / blake3 / cityhash64
     * (optional: skip if missing; hard fail if present but wrong) */
    {
        typedef void (*tea_fn)(const unsigned int *, unsigned int *);
        typedef void (*sm4_set_fn)(const unsigned char *, unsigned int *);
        typedef void (*sm4_enc_fn)(const unsigned char *, unsigned char *,
                                   const unsigned int *);
        typedef void (*x25519_fn)(unsigned char *, const unsigned char *,
                                  const unsigned char *);
        typedef void (*blake3_fn)(const void *, size_t, unsigned char *,
                                  size_t);
        typedef unsigned long long (*city64_fn)(const char *, size_t);

        tea_fn pTea = (tea_fn)dlsym(h, "tea_encrypt");
        sm4_set_fn pSm4Set = (sm4_set_fn)dlsym(h, "SM4_set_key");
        sm4_enc_fn pSm4Enc = (sm4_enc_fn)dlsym(h, "SM4_encrypt");
        x25519_fn pX25519 = (x25519_fn)dlsym(h, "x25519");
        blake3_fn pBlake3 = (blake3_fn)dlsym(h, "blake3_hash");
        city64_fn pCity = (city64_fn)dlsym(h, "cityhash64");

        /* TEA zero key/block → 41ea3a0a 94baa940 */
        if (pTea != NULL) {
            unsigned int aKey[4] = { 0, 0, 0, 0 };
            unsigned int aV[2] = { 0, 0 };

            pTea(aKey, aV);
            if (aV[0] != 0x41ea3a0au || aV[1] != 0x94baa940u) {
                fprintf(stderr, "libcgj: dyn tea_encrypt zero KAT fail\n");
                (void)dlclose(h);
                return 72;
            }
        }

        /* SM4 GB/T 32907 sample */
        if (pSm4Set != NULL && pSm4Enc != NULL) {
            static const unsigned char aSm4Key[16] = {
                0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
            static const unsigned char aSm4Pt[16] = {
                0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
                0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
            static const unsigned char aSm4Ct[16] = {
                0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e,
                0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46};
            unsigned int aRk[32];
            unsigned char aOut[16];

            pSm4Set(aSm4Key, aRk);
            pSm4Enc(aSm4Pt, aOut, aRk);
            if (memcmp(aOut, aSm4Ct, 16) != 0) {
                fprintf(stderr, "libcgj: dyn SM4 GB/T sample fail\n");
                (void)dlclose(h);
                return 73;
            }
        }

        /* X25519 RFC 7748 §5.2 vector 1 */
        if (pX25519 != NULL) {
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

            pX25519(aOut, aSc, aU);
            if (memcmp(aOut, aExp, 32) != 0) {
                fprintf(stderr, "libcgj: dyn x25519 RFC7748 v1 fail\n");
                (void)dlclose(h);
                return 74;
            }
        }

        /* BLAKE3 empty hash (soft if blake3_hash absent) */
        if (pBlake3 != NULL) {
            static const unsigned char aEmpty[32] = {
                0xaf, 0x13, 0x49, 0xb9, 0xf5, 0xf9, 0xa1, 0xa6,
                0xa0, 0x40, 0x4d, 0xea, 0x36, 0xdc, 0xc9, 0x49,
                0x9b, 0xcb, 0x25, 0xc9, 0xad, 0xc1, 0x12, 0xb7,
                0xcc, 0x9a, 0x93, 0xca, 0xe4, 0x1f, 0x32, 0x62};
            unsigned char aOut[32];
            int j;

            for (j = 0; j < 32; j++) {
                aOut[j] = 0;
            }
            pBlake3("", 0, aOut, 32);
            if (memcmp(aOut, aEmpty, 32) != 0) {
                fprintf(stderr, "libcgj: dyn blake3 empty hash fail\n");
                (void)dlclose(h);
                return 75;
            }
        }

        /* CityHash64 empty → k2 */
        if (pCity != NULL) {
            if (pCity("", 0) != 0x9ae16a3b2f90404full) {
                fprintf(stderr, "libcgj: dyn cityhash64 empty fail\n");
                (void)dlclose(h);
                return 76;
            }
        }
    }
    /*
     * Continuum soft-then-hard gates (dlsym; skip if missing, hard if present):
     * - host SSE double path (static; no product symbols required)
     * - gj_cpu_has_aesni → {0,1}
     * - drand48 → [0,1)
     * - sqrt(4.0) → 2.0 (volatile)
     * - base64url_encode("f") → "Zg=="
     */
    {
        typedef int (*aesni_fn)(void);
        typedef double (*drand48_fn)(void);
        typedef double (*sqrt_fn)(double);
        typedef ssize_t (*b64url_fn)(const unsigned char *, size_t, char *,
                                     size_t);
        aesni_fn pAesni = (aesni_fn)dlsym(h, "gj_cpu_has_aesni");
        drand48_fn pDrand = (drand48_fn)dlsym(h, "drand48");
        sqrt_fn pSqrt = (sqrt_fn)dlsym(h, "sqrt");
        b64url_fn pB64u = (b64url_fn)dlsym(h, "base64url_encode");
        volatile double dA = 2.0;
        volatile double dB = 2.0;
        volatile double dSum = dA + dB;
        volatile double dProd = dA * dB;
        volatile double dQuot = dA / dB;

        if (dSum != 4.0 || dProd != 4.0 || dQuot != 1.0) {
            fprintf(stderr, "libcgj: dyn SSE double path fail\n");
            (void)dlclose(h);
            return 77;
        }
        if (pAesni != NULL) {
            int nHas = pAesni();

            if (nHas != 0 && nHas != 1) {
                fprintf(stderr, "libcgj: dyn gj_cpu_has_aesni bad %d\n", nHas);
                (void)dlclose(h);
                return 78;
            }
        }
        if (pDrand != NULL) {
            volatile double dR;
            int j;

            for (j = 0; j < 8; j++) {
                dR = pDrand();
                if (!(dR >= 0.0 && dR < 1.0)) {
                    fprintf(stderr, "libcgj: dyn drand48 range fail\n");
                    (void)dlclose(h);
                    return 79;
                }
            }
        }
        if (pSqrt != NULL) {
            volatile double dIn = 4.0;
            volatile double dOut = pSqrt(dIn);

            if (dOut != 2.0) {
                fprintf(stderr, "libcgj: dyn sqrt(4.0) fail\n");
                (void)dlclose(h);
                return 80;
            }
        }
        if (pB64u != NULL) {
            char aB64u[8];
            ssize_t nEnc;

            nEnc = pB64u((const unsigned char *)"f", 1, aB64u, sizeof(aB64u));
            if (nEnc != 4 || strcmp(aB64u, "Zg==") != 0) {
                fprintf(stderr, "libcgj: dyn base64url_encode KAT fail\n");
                (void)dlclose(h);
                return 81;
            }
        }
    }
    /*
     * Continuum soft gates for upcoming batches (dlsym; skip if missing,
     * hard fail if present but wrong):
     *   highwayhash64, gj_deflate, gj_tar_header_parse, gj_cpu_has_avx2
     */
    {
        typedef unsigned long long (*hh_fn)(const unsigned char *, const void *,
                                            size_t);
        typedef int (*deflate_fn)(const unsigned char *, size_t, unsigned char *,
                                  size_t *, int);
        typedef int (*tar_fn)(const unsigned char *, char *,
                              unsigned long long *, int *);
        typedef int (*avx2_fn)(void);

        hh_fn pHh = (hh_fn)dlsym(h, "highwayhash64");
        deflate_fn pDef = (deflate_fn)dlsym(h, "gj_deflate");
        tar_fn pTar = (tar_fn)dlsym(h, "gj_tar_header_parse");
        avx2_fn pAvx2 = (avx2_fn)dlsym(h, "gj_cpu_has_avx2");

        if (pAvx2 != NULL) {
            int nHas = pAvx2();

            if (nHas != 0 && nHas != 1) {
                fprintf(stderr, "libcgj: dyn gj_cpu_has_avx2 bad %d\n", nHas);
                (void)dlclose(h);
                return 82;
            }
        }
        if (pHh != NULL) {
            unsigned char aKey[32];
            unsigned long long u64Tag;
            int j;

            for (j = 0; j < 32; j++) {
                aKey[j] = 0;
            }
            u64Tag = pHh(aKey, "", 0);
            if (u64Tag != 0x7035da75b9d54469ull) {
                fprintf(stderr, "libcgj: dyn highwayhash64 empty KAT fail\n");
                (void)dlclose(h);
                return 83;
            }
            if (pHh(NULL, "", 0) != u64Tag) {
                fprintf(stderr, "libcgj: dyn highwayhash64 null-key fail\n");
                (void)dlclose(h);
                return 84;
            }
        }
        if (pDef != NULL) {
            unsigned char aOut[64];
            size_t cbOut = sizeof(aOut);

            if (pDef((const unsigned char *)"", 0, aOut, &cbOut, 0) != 0 ||
                cbOut != 11u || aOut[0] != 0x78u || aOut[1] != 0x01u) {
                fprintf(stderr, "libcgj: dyn gj_deflate empty store fail\n");
                (void)dlclose(h);
                return 85;
            }
        }
        if (pTar != NULL) {
            unsigned char aHdr[512];
            char aName[256];
            unsigned long long u64Sz = 1;
            int nTf = -1;
            int j;

            if (pTar(NULL, aName, &u64Sz, &nTf) != -1) {
                fprintf(stderr, "libcgj: dyn gj_tar_header_parse NULL fail\n");
                (void)dlclose(h);
                return 86;
            }
            for (j = 0; j < 512; j++) {
                aHdr[j] = 0;
            }
            aHdr[0] = 's';
            aHdr[1] = 'm';
            aHdr[2] = 'o';
            aHdr[3] = 'k';
            aHdr[4] = 'e';
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
            aHdr[156] = '0';
            aHdr[257] = 'u';
            aHdr[258] = 's';
            aHdr[259] = 't';
            aHdr[260] = 'a';
            aHdr[261] = 'r';
            aHdr[263] = '0';
            aHdr[264] = '0';
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
            if (pTar(aHdr, aName, &u64Sz, &nTf) != 0 ||
                strcmp(aName, "smoke") != 0 || u64Sz != 0ull || nTf != '0') {
                fprintf(stderr, "libcgj: dyn gj_tar_header_parse KAT fail\n");
                (void)dlclose(h);
                return 87;
            }
        }
    }
    /*
     * Continuum soft gates (dlsym; skip if missing, hard fail if present
     * but wrong): gj_ws_header_parse, gj_lz4_decompress_safe, z85_encode
     */
    {
        typedef int (*ws_fn)(const unsigned char *, size_t, int *, int *,
                             int *, unsigned long long *, size_t *);
        typedef int (*lz4_fn)(const unsigned char *, size_t, unsigned char *,
                              size_t);
        typedef ssize_t (*z85_fn)(const unsigned char *, size_t, char *,
                                  size_t);

        ws_fn pWs = (ws_fn)dlsym(h, "gj_ws_header_parse");
        lz4_fn pLz4 = (lz4_fn)dlsym(h, "gj_lz4_decompress_safe");
        z85_fn pZ85 = (z85_fn)dlsym(h, "z85_encode");

        if (pWs != NULL) {
            const unsigned char aWs[] = {0x81u, 0x05u};
            int nFin = -1;
            int nOp = -1;
            int nMask = -1;
            unsigned long long uPay = 1;
            size_t cbHdr = 0;

            if (pWs(NULL, 2, &nFin, &nOp, &nMask, &uPay, &cbHdr) != -1) {
                fprintf(stderr, "libcgj: dyn gj_ws_header_parse NULL fail\n");
                (void)dlclose(h);
                return 88;
            }
            nFin = nOp = nMask = -1;
            uPay = 1;
            cbHdr = 0;
            if (pWs(aWs, sizeof(aWs), &nFin, &nOp, &nMask, &uPay, &cbHdr) !=
                    0 ||
                nFin != 1 || nOp != 1 || nMask != 0 || uPay != 5ull ||
                cbHdr != 2u) {
                fprintf(stderr, "libcgj: dyn gj_ws_header_parse KAT fail\n");
                (void)dlclose(h);
                return 89;
            }
        }
        if (pLz4 != NULL) {
            unsigned char aDst[16];
            const unsigned char aLz4[] = {0x50u, 'h', 'e', 'l', 'l', 'o'};
            int nOut;

            if (pLz4(NULL, 0, aDst, sizeof(aDst)) != -1) {
                fprintf(stderr,
                        "libcgj: dyn gj_lz4_decompress_safe NULL fail\n");
                (void)dlclose(h);
                return 90;
            }
            if (pLz4((const unsigned char *)"", 0, aDst, sizeof(aDst)) != 0) {
                fprintf(stderr,
                        "libcgj: dyn gj_lz4_decompress_safe empty fail\n");
                (void)dlclose(h);
                return 91;
            }
            nOut = pLz4(aLz4, sizeof(aLz4), aDst, sizeof(aDst));
            if (nOut != 5 || memcmp(aDst, "hello", 5) != 0) {
                fprintf(stderr,
                        "libcgj: dyn gj_lz4_decompress_safe KAT fail\n");
                (void)dlclose(h);
                return 92;
            }
        }
        if (pZ85 != NULL) {
            static const unsigned char aBin[] = {
                0x86u, 0x4Fu, 0xD2u, 0x6Fu, 0xB5u, 0x59u, 0xF7u, 0x5Bu};
            char aZ[16];
            ssize_t nEnc;

            nEnc = pZ85((const unsigned char *)"", 0, aZ, sizeof(aZ));
            if (nEnc != 0 || aZ[0] != '\0') {
                fprintf(stderr, "libcgj: dyn z85_encode empty fail\n");
                (void)dlclose(h);
                return 93;
            }
            nEnc = pZ85(aBin, sizeof(aBin), aZ, sizeof(aZ));
            if (nEnc != 10 || strcmp(aZ, "HelloWorld") != 0) {
                fprintf(stderr, "libcgj: dyn z85_encode KAT fail\n");
                (void)dlclose(h);
                return 94;
            }
        }
    }
    /*
     * Continuum soft gates (dlsym; skip if missing, hard fail if present
     * but wrong): gj_bitrev32, gj_gray_encode, gj_fletcher16
     */
    {
        typedef unsigned int (*bitrev_fn)(unsigned int);
        typedef unsigned int (*gray_fn)(unsigned int);
        typedef unsigned short (*f16_fn)(const void *, size_t);

        bitrev_fn pBit = (bitrev_fn)dlsym(h, "gj_bitrev32");
        gray_fn pGray = (gray_fn)dlsym(h, "gj_gray_encode");
        f16_fn pF16 = (f16_fn)dlsym(h, "gj_fletcher16");

        if (pBit != NULL) {
            if (pBit(0x00000001u) != 0x80000000u ||
                pBit(0x12345678u) != 0x1e6a2c48u || pBit(0u) != 0u) {
                fprintf(stderr, "libcgj: dyn gj_bitrev32 KAT fail\n");
                (void)dlclose(h);
                return 95;
            }
        }
        if (pGray != NULL) {
            if (pGray(0u) != 0u || pGray(2u) != 3u || pGray(7u) != 4u) {
                fprintf(stderr, "libcgj: dyn gj_gray_encode KAT fail\n");
                (void)dlclose(h);
                return 96;
            }
        }
        if (pF16 != NULL) {
            if (pF16(NULL, 1) != 0u || pF16((const void *)"", 0) != 0u) {
                fprintf(stderr, "libcgj: dyn gj_fletcher16 empty fail\n");
                (void)dlclose(h);
                return 97;
            }
            if (pF16((const void *)"\x01", 1) != 0x0101u ||
                pF16((const void *)"abc", 3) != 0x4c27u) {
                fprintf(stderr, "libcgj: dyn gj_fletcher16 KAT fail\n");
                (void)dlclose(h);
                return 98;
            }
        }
    }
    /*
     * Continuum soft gates (dlsym; skip if missing, hard fail if present
     * but wrong): gj_kmp_find, gj_gcd_u64, gj_modpow_u64
     */
    {
        typedef ssize_t (*kmp_fn)(const char *, const char *);
        typedef unsigned long long (*gcd_fn)(unsigned long long,
                                             unsigned long long);
        typedef unsigned long long (*modpow_fn)(unsigned long long,
                                                unsigned long long,
                                                unsigned long long);

        kmp_fn pKmp = (kmp_fn)dlsym(h, "gj_kmp_find");
        gcd_fn pGcd = (gcd_fn)dlsym(h, "gj_gcd_u64");
        modpow_fn pMod = (modpow_fn)dlsym(h, "gj_modpow_u64");

        if (pKmp != NULL) {
            if (pKmp(NULL, "a") != (ssize_t)-1 ||
                pKmp("a", NULL) != (ssize_t)-1) {
                fprintf(stderr, "libcgj: dyn gj_kmp_find NULL fail\n");
                (void)dlclose(h);
                return 99;
            }
            if (pKmp("hello", "") != (ssize_t)0 ||
                pKmp("", "") != (ssize_t)0) {
                fprintf(stderr, "libcgj: dyn gj_kmp_find empty fail\n");
                (void)dlclose(h);
                return 100;
            }
            if (pKmp("hello", "ll") != (ssize_t)2 ||
                pKmp("hello", "he") != (ssize_t)0 ||
                pKmp("hello", "xyz") != (ssize_t)-1 ||
                pKmp("aaa", "aa") != (ssize_t)0 ||
                pKmp("ababcabcabababd", "ababd") != (ssize_t)10) {
                fprintf(stderr, "libcgj: dyn gj_kmp_find KAT fail\n");
                (void)dlclose(h);
                return 101;
            }
        }
        if (pGcd != NULL) {
            if (pGcd(0ull, 0ull) != 0ull || pGcd(0ull, 7ull) != 7ull ||
                pGcd(7ull, 0ull) != 7ull) {
                fprintf(stderr, "libcgj: dyn gj_gcd_u64 zero fail\n");
                (void)dlclose(h);
                return 102;
            }
            if (pGcd(48ull, 18ull) != 6ull || pGcd(17ull, 13ull) != 1ull ||
                pGcd(1071ull, 462ull) != 21ull) {
                fprintf(stderr, "libcgj: dyn gj_gcd_u64 KAT fail\n");
                (void)dlclose(h);
                return 103;
            }
        }
        if (pMod != NULL) {
            if (pMod(2ull, 0ull, 7ull) != 1ull ||
                pMod(5ull, 0ull, 1ull) != 0ull ||
                pMod(3ull, 5ull, 1ull) != 0ull) {
                fprintf(stderr, "libcgj: dyn gj_modpow_u64 edge fail\n");
                (void)dlclose(h);
                return 104;
            }
            if (pMod(2ull, 10ull, 1000ull) != 24ull ||
                pMod(5ull, 3ull, 13ull) != 8ull ||
                pMod(2ull, 10ull, 0ull) != 1024ull ||
                pMod(0ull, 5ull, 7ull) != 0ull) {
                fprintf(stderr, "libcgj: dyn gj_modpow_u64 KAT fail\n");
                (void)dlclose(h);
                return 105;
            }
        }
    }
    /*
     * Continuum soft gates (dlsym; skip if missing, hard fail if present
     * but wrong): gj_fib_u64, gj_is_prime_u64, gj_edit_distance
     */
    {
        typedef unsigned long long (*fib_fn)(unsigned);
        typedef int (*prime_fn)(unsigned long long);
        typedef size_t (*edit_fn)(const char *, const char *);

        fib_fn pFib = (fib_fn)dlsym(h, "gj_fib_u64");
        prime_fn pPrime = (prime_fn)dlsym(h, "gj_is_prime_u64");
        edit_fn pEdit = (edit_fn)dlsym(h, "gj_edit_distance");

        if (pFib != NULL) {
            if (pFib(0u) != 0ull || pFib(1u) != 1ull || pFib(10u) != 55ull) {
                fprintf(stderr, "libcgj: dyn gj_fib_u64 KAT fail\n");
                (void)dlclose(h);
                return 106;
            }
        }
        if (pPrime != NULL) {
            if (pPrime(0ull) != 0 || pPrime(1ull) != 0 || pPrime(4ull) != 0) {
                fprintf(stderr, "libcgj: dyn gj_is_prime_u64 composite fail\n");
                (void)dlclose(h);
                return 107;
            }
            if (pPrime(2ull) != 1 || pPrime(3ull) != 1 || pPrime(17ull) != 1 ||
                pPrime(91ull) != 0) {
                fprintf(stderr, "libcgj: dyn gj_is_prime_u64 KAT fail\n");
                (void)dlclose(h);
                return 108;
            }
        }
        if (pEdit != NULL) {
            if (pEdit("", "") != (size_t)0) {
                fprintf(stderr, "libcgj: dyn gj_edit_distance empty fail\n");
                (void)dlclose(h);
                return 109;
            }
            if (pEdit("kitten", "sitting") != (size_t)3) {
                fprintf(stderr, "libcgj: dyn gj_edit_distance KAT fail\n");
                (void)dlclose(h);
                return 110;
            }
        }
    }
    /*
     * Continuum soft gates (dlsym; skip if missing, hard fail if present
     * but wrong): gj_ctz64, gj_popcnt64, gj_str_len
     */
    {
        typedef unsigned (*ctz_fn)(unsigned long long);
        typedef unsigned (*pop_fn)(unsigned long long);
        typedef size_t (*slen_fn)(const char *);

        ctz_fn pCtz = (ctz_fn)dlsym(h, "gj_ctz64");
        pop_fn pPop = (pop_fn)dlsym(h, "gj_popcnt64");
        slen_fn pSlen = (slen_fn)dlsym(h, "gj_str_len");

        if (pCtz != NULL) {
            if (pCtz(1ull) != 0u || pCtz(8ull) != 3u || pCtz(0ull) != 0u) {
                fprintf(stderr, "libcgj: dyn gj_ctz64 KAT fail\n");
                (void)dlclose(h);
                return 111;
            }
        }
        if (pPop != NULL) {
            if (pPop(0ull) != 0u || pPop(7ull) != 3u ||
                pPop(0xFFull) != 8u) {
                fprintf(stderr, "libcgj: dyn gj_popcnt64 KAT fail\n");
                (void)dlclose(h);
                return 112;
            }
        }
        if (pSlen != NULL) {
            if (pSlen(NULL) != (size_t)0 || pSlen("") != (size_t)0 ||
                pSlen("abc") != (size_t)3) {
                fprintf(stderr, "libcgj: dyn gj_str_len KAT fail\n");
                (void)dlclose(h);
                return 113;
            }
        }
    }
    /*
     * Continuum soft gates (dlsym; skip if missing, hard fail if present
     * but wrong): gj_batch_id, gj_is_leap_year, gj_bset_set/test
     */
    {
        typedef int (*batch_id_fn)(void);
        typedef int (*leap_fn)(unsigned);
        typedef void (*bset_set_fn)(unsigned long long *, size_t);
        typedef int (*bset_test_fn)(const unsigned long long *, size_t);

        batch_id_fn pBid = (batch_id_fn)dlsym(h, "gj_batch_id");
        leap_fn pLeap = (leap_fn)dlsym(h, "gj_is_leap_year");
        bset_set_fn pBsetSet = (bset_set_fn)dlsym(h, "gj_bset_set");
        bset_test_fn pBsetTest = (bset_test_fn)dlsym(h, "gj_bset_test");

        /* gj_batch_id: wired continuum id → 500 */
        if (pBid != NULL) {
            if (pBid() != 500) {
                fprintf(stderr, "libcgj: dyn gj_batch_id KAT fail\n");
                (void)dlclose(h);
                return 114;
            }
        }
        /* gj_is_leap_year: 2000=1, 1900=0, 2024=1 (proleptic Gregorian) */
        if (pLeap != NULL) {
            if (pLeap(2000u) != 1 || pLeap(1900u) != 0 || pLeap(2024u) != 1) {
                fprintf(stderr, "libcgj: dyn gj_is_leap_year KAT fail\n");
                (void)dlclose(h);
                return 115;
            }
        }
        /* gj_bset_set/test: set bit 3, test bit 3 == 1 */
        if (pBsetSet != NULL && pBsetTest != NULL) {
            unsigned long long aWords[1] = { 0ull };

            pBsetSet(aWords, (size_t)3);
            if (pBsetTest(aWords, (size_t)3) != 1) {
                fprintf(stderr, "libcgj: dyn gj_bset_set/test KAT fail\n");
                (void)dlclose(h);
                return 116;
            }
        }
    }
    /* batch46: PBKDF2-SHA512 + scrypt + bcrypt_pbkdf/argon2 stubs */
    if (dlsym(h, "pbkdf2_hmac_sha512") == NULL || dlsym(h, "scrypt") == NULL ||
        dlsym(h, "scrypt_kdf") == NULL || dlsym(h, "bcrypt_pbkdf") == NULL ||
        dlsym(h, "argon2id_hash_raw") == NULL ||
        dlsym(h, "argon2i_hash_raw") == NULL ||
        dlsym(h, "__libcgj_batch46_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch46: %s\n", dlerror());
        (void)dlclose(h);
        return 63;
    }
    /* batch47: fortify/chk surface (pre-existing) + marker */
    if (dlsym(h, "__readlinkat_chk") == NULL ||
        dlsym(h, "__getgroups_chk") == NULL ||
        dlsym(h, "__gethostname_chk") == NULL ||
        dlsym(h, "__getdomainname_chk") == NULL ||
        dlsym(h, "__getlogin_r_chk") == NULL ||
        dlsym(h, "__confstr_chk") == NULL ||
        dlsym(h, "__wctomb_chk") == NULL ||
        dlsym(h, "__mbsnrtowcs_chk") == NULL ||
        dlsym(h, "__wcsnrtombs_chk") == NULL ||
        dlsym(h, "__recv_chk") == NULL ||
        dlsym(h, "__recvfrom_chk") == NULL ||
        dlsym(h, "__read_chk") == NULL ||
        dlsym(h, "__pread_chk") == NULL ||
        dlsym(h, "__getwd_chk") == NULL ||
        dlsym(h, "__libcgj_batch47_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch47: %s\n", dlerror());
        (void)dlclose(h);
        return 64;
    }
    /* batch48: idna / nss lookup2 / nscd nl timestamp / res aliases */
    if (dlsym(h, "__idna_to_dns_encoding") == NULL ||
        dlsym(h, "__idna_from_dns_encoding") == NULL ||
        dlsym(h, "__nss_passwd_lookup2") == NULL ||
        dlsym(h, "__nss_group_lookup2") == NULL ||
        dlsym(h, "__nss_hosts_lookup2") == NULL ||
        dlsym(h, "__nscd_get_nl_timestamp") == NULL ||
        dlsym(h, "__res_ninit") == NULL || dlsym(h, "__res_iclose") == NULL ||
        dlsym(h, "__libcgj_batch48_marker") == NULL) {
        fprintf(stderr, "libcgj: dyn missing desktop batch48: %s\n", dlerror());
        (void)dlclose(h);
        return 65;
    }

    if (*pMark == 0) {
        (void)dlclose(h);
        return 3;
    }
    if (pStrcmp("ok", "ok") != 0) {
        (void)dlclose(h);
        return 4;
    }
    p = pMalloc(64);
    if (p == NULL) {
        (void)dlclose(h);
        return 5;
    }
    pFree(p);

    /* printf from libcgj would use our write() SYSCALL — fine on host */
    (void)pPrintf("libcgj: dyn smoke PASS path=%s marker=0x%llx\n", szPath,
                  (unsigned long long)*pMark);
    (void)dlclose(h);
    return 0;
}
