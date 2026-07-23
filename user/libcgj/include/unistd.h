/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped unistd (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

ssize_t read(int nFd, void *pBuf, size_t cb);
ssize_t write(int nFd, const void *pBuf, size_t cb);
ssize_t pread(int nFd, void *pBuf, size_t cb, off_t off);
ssize_t pwrite(int nFd, const void *pBuf, size_t cb, off_t off);
int     close(int nFd);
int     fsync(int nFd);
int     fdatasync(int nFd);
int     sync(void);
int     syncfs(int nFd);
off_t   lseek(int nFd, off_t off, int nWhence);
long    pathconf(const char *szPath, int nName);
long    fpathconf(int nFd, int nName);
int     euidaccess(const char *szPath, int nMode);
int     eaccess(const char *szPath, int nMode);
int     faccessat(int nDfd, const char *szPath, int nMode, int nFlags);
int     faccessat2(int nDfd, const char *szPath, int nMode, int nFlags);
char   *canonicalize_file_name(const char *szPath);
pid_t   getpid(void);
pid_t   getppid(void);
pid_t   gettid(void);
int     setns(int nFd, int nNstype);
int     unshare(int nFlags);
int     sync_file_range(int nFd, off_t off, off_t cb, unsigned uFlags);
ssize_t readahead(int nFd, off_t off, size_t cb);
int     personality(unsigned long uPersona);
int     reboot(int nMagic, int nMagic2, int nCmd, void *pArg);
int     klogctl(int nType, char *szBuf, int nLen);
int     acct(const char *szFilename);
int     close_range(unsigned uFirst, unsigned uLast, int nFlags);
int     execveat(int nDfd, const char *szPath, char *const aArgv[],
                 char *const aEnvp[], int nFlags);
int     ioprio_set(int nWhich, int nWho, int nIoprio);
int     ioprio_get(int nWhich, int nWho);
int     membarrier(int nCmd, unsigned uFlags, int nCpuId);
uid_t   getuid(void);
gid_t   getgid(void);
uid_t   geteuid(void);
gid_t   getegid(void);
int     setuid(uid_t uid);
int     setgid(gid_t gid);
int     seteuid(uid_t euid);
int     setegid(gid_t egid);
int     setreuid(uid_t ruid, uid_t euid);
int     setregid(gid_t rgid, gid_t egid);
int     getresuid(uid_t *pRuid, uid_t *pEuid, uid_t *pSuid);
int     getresgid(gid_t *pRgid, gid_t *pEgid, gid_t *pSgid);
int     setresuid(uid_t ruid, uid_t euid, uid_t suid);
int     setresgid(gid_t rgid, gid_t egid, gid_t sgid);
int     getgroups(int nSize, gid_t *pList);
int     setgroups(size_t nSize, const gid_t *pList);
int     initgroups(const char *szUser, gid_t gid);
int     getgrouplist(const char *szUser, gid_t gid, gid_t *pGroups,
                     int *pNgids);
void   *sbrk(intptr_t i64Delta);
int     brk(void *p);
int     isatty(int nFd);
unsigned int sleep(unsigned int u32Sec);
int     usleep(unsigned int u32Usec);
int     pipe(int aFd[2]);
int     pipe2(int aFd[2], int nFlags);
int     dup(int nFd);
int     dup2(int nOld, int nNew);
int     dup3(int nOld, int nNew, int nFlags);
pid_t   fork(void);
int     execve(const char *szPath, char *const aArgv[], char *const aEnvp[]);
/* execveat declared above with *at helpers */
int     execv(const char *szPath, char *const aArgv[]);
int     execvp(const char *szFile, char *const aArgv[]);
int     execvpe(const char *szFile, char *const aArgv[], char *const aEnvp[]);
int     execl(const char *szPath, const char *szArg0, ...);
int     execlp(const char *szFile, const char *szArg0, ...);
int     execle(const char *szPath, const char *szArg0, ...);
int     access(const char *szPath, int nMode);
char   *get_current_dir_name(void);
char   *getlogin(void);
int     getlogin_r(char *szBuf, size_t cb);
int     unlink(const char *szPath);
int     unlinkat(int nDfd, const char *szPath, int nFlags);
int     rmdir(const char *szPath);
int     chdir(const char *szPath);
int     fchdir(int nFd);
char   *ttyname(int nFd);
int     ttyname_r(int nFd, char *szBuf, size_t cb);
mode_t  umask(mode_t mask);
int     chown(const char *szPath, uid_t uid, gid_t gid);
int     fchown(int nFd, uid_t uid, gid_t gid);
int     lchown(const char *szPath, uid_t uid, gid_t gid);
int     fchownat(int nDfd, const char *szPath, uid_t uid, gid_t gid,
                 int nFlags);
char   *getcwd(char *szBuf, size_t cb);
ssize_t readlink(const char *szPath, char *szBuf, size_t cb);
ssize_t readlinkat(int nDfd, const char *szPath, char *szBuf, size_t cb);
int     symlink(const char *szTarget, const char *szLink);
int     symlinkat(const char *szTarget, int nNewdfd, const char *szLink);
int     link(const char *szOld, const char *szNew);
int     linkat(int nOlddfd, const char *szOld, int nNewdfd, const char *szNew,
               int nFlags);
int     rename(const char *szOld, const char *szNew);
int     renameat(int nOlddfd, const char *szOld, int nNewdfd, const char *szNew);
int     renameat2(int nOlddfd, const char *szOld, int nNewdfd, const char *szNew,
                  unsigned uFlags);
ssize_t copy_file_range(int nFdin, off_t *pOffIn, int nFdout, off_t *pOffOut,
                        size_t cbLen, unsigned uFlags);
void    _exit(int nCode) __attribute__((noreturn));
unsigned int alarm(unsigned int u32Sec);
int     pause(void);
ssize_t getrandom(void *pBuf, size_t cb, unsigned int u32Flags);
int     getentropy(void *pBuf, size_t cb);
int     ftruncate(int nFd, off_t cbLen);
int     truncate(const char *szPath, off_t cbLen);
/* lockf declared in fcntl.h as well for apps that include either */
#ifndef F_ULOCK
int     lockf(int nFd, int nCmd, off_t cbLen);
#endif
long    sysconf(int nName);
long    __sysconf(int nName);
int     __libc_current_sigrtmin(void);
int     __libc_current_sigrtmax(void);
size_t  confstr(int nName, char *szBuf, size_t cb);
int     gethostname(char *szName, size_t cb);
long    gethostid(void);
int     sethostid(long nId);
int     closefrom(int nLowfd);
int     arch_prctl(int nCode, unsigned long uAddr);
int     __arch_prctl(int nCode, unsigned long uAddr);
int     clone(int (*fn)(void *), void *pStack, int nFlags, void *pArg, ...);
int     __clone(int (*fn)(void *), void *pStack, int nFlags, void *pArg, ...);
int     sethostname(const char *szName, size_t cb);
int     sched_yield(void);
int     getpagesize(void);
int     prctl(int nOption, unsigned long a2, unsigned long a3, unsigned long a4,
              unsigned long a5);
int     getdtablesize(void);
int     daemon(int nNochdir, int nNoclose);
pid_t   setsid(void);
pid_t   getsid(pid_t pid);
pid_t   getpgid(pid_t pid);
pid_t   getpgrp(void);
int     setpgid(pid_t pid, pid_t pgid);
int     setpgrp(void);
int     chroot(const char *szPath);
char   *getpass(const char *szPrompt);
char   *getusershell(void);
void    setusershell(void);
void    endusershell(void);
int     getdomainname(char *szName, size_t cb);
int     setdomainname(const char *szName, size_t cb);
pid_t   tcgetpgrp(int nFd);
int     tcsetpgrp(int nFd, pid_t pgrp);

/* sysconf names (glibc-shaped subset) */
#define _SC_PAGESIZE           30
#define _SC_PAGE_SIZE          _SC_PAGESIZE
#define _SC_CLK_TCK            2
#define _SC_NPROCESSORS_ONLN   84
#define _SC_NPROCESSORS_CONF   83
#define _SC_OPEN_MAX           4
#define _SC_ARG_MAX            0
#define _SC_LINE_MAX           43
#define _SC_HOST_NAME_MAX      180
#define _CS_PATH               0
#define _CS_GNU_LIBC_VERSION   2

/* pathconf / fpathconf names (glibc-shaped subset) */
#define _PC_LINK_MAX           0
#define _PC_MAX_CANON          1
#define _PC_MAX_INPUT          2
#define _PC_NAME_MAX           3
#define _PC_PATH_MAX           4
#define _PC_PIPE_BUF           5
#define _PC_CHOWN_RESTRICTED   6
#define _PC_NO_TRUNC           7
#define _PC_VDISABLE           8
#define _PC_SYNC_IO            9
#define _PC_ASYNC_IO          10
#define _PC_PRIO_IO           11
#define _PC_SOCK_MAXBUF       12
#define _PC_FILESIZEBITS      13
#define _PC_REC_INCR_XFER_SIZE 14
#define _PC_REC_MAX_XFER_SIZE 15
#define _PC_REC_MIN_XFER_SIZE 16
#define _PC_REC_XFER_ALIGN    17
#define _PC_ALLOC_SIZE_MIN    18
#define _PC_SYMLINK_MAX       19
#define _PC_2_SYMLINKS        20

#define AT_EACCESS 0x200

int     group_member(gid_t gid);
char   *getwd(char *szBuf);
int     killpg(int nPgrp, int nSig);
int     getpeereid(int nFd, uid_t *pUid, gid_t *pGid);
int     lchmod(const char *szPath, mode_t mode);
struct timeval;
int     futimesat(int nDfd, const char *szPath, const struct timeval *aTv);
int     sysctl(int *pName, int nLen, void *pOld, size_t *pcbOld, void *pNew,
               size_t cbNew);
int     pidfd_open(pid_t pid, unsigned int uFlags);
int     pidfd_send_signal(int nPidfd, int nSig, void *pInfo, unsigned int uFlags);
int     pidfd_getfd(int nPidfd, int nTargetFd, unsigned int uFlags);

/* LFS aliases (identity on x86_64) */
int     open64(const char *szPath, int nFlags, ...);
int     __open64(const char *szPath, int nFlags, ...);
int     __open64_2(const char *szPath, int nFlags);
int     __openat64_2(int nDfd, const char *szPath, int nFlags);
off_t   lseek64(int nFd, off_t off, int nWhence);
ssize_t pread64(int nFd, void *pBuf, size_t cb, off_t off);
ssize_t pwrite64(int nFd, const void *pBuf, size_t cb, off_t off);
int     ftruncate64(int nFd, off_t cbLen);
int     truncate64(const char *szPath, off_t cbLen);

pid_t   vfork(void);
int     fexecve(int nFd, char *const aArgv[], char *const aEnvp[]);
ssize_t getdents64(int nFd, void *pDirp, size_t cb);
int     getdents(int nFd, void *pDirp, unsigned cb);
int     issetugid(void);
void    strmode(mode_t mode, char *szBuf);
long    syscall(long nNr, ...);

#ifdef __cplusplus
}
#endif

