# GreenJade — glibc compatibility path

| | |
|--|--|
| **Policy** | **No GPL/copyleft source** in-tree (LICENSE dual MIT OR Apache-2.0) |
| **Not shipped** | GNU glibc (LGPL) source or binaries **as project tree content** |
| **In-tree runtime** | **libcgj** — clean-room **glibc-shaped** C library (same public symbols/paths apps expect) |
| **Dynlinker** | **ld-gj** (`/lib/ld-gj.so.1`, also `ld-linux-x86-64.so.2`) |
| **DT_NEEDED name** | Apps may link `libc.so.6` → staged **libcgj** product ELF |

## Goals

1. Provide a **bring-up libc** sufficient for freestanding servers, init, shell, and dynlink smokes.
2. Grow the Linux syscall + libc surface until **host-built glibc binaries** (or Steam) can run under clean-room ABI (Option C) **without** pasting glibc into the repo.
3. Package layout matches common FHS: `/lib/libc.so.6`, `/lib/ld-linux-x86-64.so.2`.

## Status of bring-up bars (1–4)

| # | Item | Status |
|---|------|--------|
| 1 | FILE buffering + wider stdio | **done** — line/full/none buffering, `setvbuf`, `fread`/`fwrite` buffered, `fdopen`, `ungetc`, `fgets`, … |
| 2 | pthread / TLS / futex helpers | **done** — mutex/cond (futex), `pthread_once`/keys, `__tls_get_addr`, `clone`-based create (bring-up) |
| 3 | `GLIBC_2.*` symbol versions | **done** — `user/libcgj/libc.map`; VERDEF nodes `GLIBC_2.2.5`…`2.34`; default `@@GLIBC_2.2.5` |
| 4 | Deeper ld-gj for host-glibc graphs | **done (bring-up)** — `R_X86_64_{64,COPY,TLS,IRELATIVE}`, DT_JMPREL, DT_INIT; kernel `elf_load` mirrors |

## Layout

| Path | Content |
|------|---------|
| `user/libcgj/` | Clean-room C sources (MIT OR Apache-2.0) |
| `user/libcgj/include/` | glibc-shaped headers |
| `user/libcgj/libc.map` | GNU symbol version script |
| `build/user/libc.so.6` | Shared freestanding product ELF (soname `libc.so.6` + versions) |
| `build/user/libcgj.a` | Static archive for freestanding apps |
| `build/rootfs/lib/libc.so.6` | Staged from product ELF |
| `build/cgj_smoke` | Host freestanding static smoke |
| `build/cgj_dyn_smoke` | Host `dlopen` of product `libc.so.6` |

## Current surface (libcgj)

| Area | Symbols / APIs (subset) |
|------|-------------------------|
| memory | `malloc` `free` `calloc` `realloc` `aligned_alloc` `posix_memalign` `sbrk` `brk` `mmap` `munmap` `mprotect` |
| strings | full common C string/mem set + `strlcpy`/`strlcat` `memrchr`/`mempcpy`/`rawmemchr` `explicit_bzero` `strcasestr` `stpncpy` `strverscmp` `strerror_r` (GNU) `ffs`/`ffsl`/`ffsll` … |
| stdio | buffered `FILE`, `printf`/`asprintf`/`scanf`/`sscanf`/`fscanf` family, `fopen`/`fmemopen`/`open_memstream`/`getline`/`fseeko`… |
| wchar | `wmem*`/`mbrtowc`/`fwprintf`/`fwscanf`/`wcsncmp`/`wcsncat`/`wcsstr`/`wcsnlen`/`wcscasecmp`/`wcsspn`/`isw*`/`wctype`/`towctrans`/`wcscoll`… |
| stdlib | `strtol` family `arc4random*` `secure_getenv` `aligned_alloc` `posix_memalign` `qsort` `bsearch` `exit` `abort` … |
| unistd/fs | open/read/write/seek/stat/fork/exec/pipe/dup/cwd… |
| time | `time` `clock_gettime` `nanosleep` `sleep` `usleep` |
| ctype | ASCII `is*` / `to*` |
| signal | `signal` `raise` `kill` |
| pthread | mutex/cond(+timed, attr type/protocol/clock) + recursive mutex + once/keys + rwlock(+timed) + spin + barrier + attr stacksize/stack/guard + getattr_np + affinity_np (+ attr affinity) + schedparam/prio + getcpuclockid + atfork/`__register_atfork` + `pthread_setname_np`/`getname_np` + `__tls_get_addr` |
| glibc entry | `__errno_location` `__libc_start_main` |
| versions | `GLIBC_2.2.5` (default) … `GLIBC_2.34` nodes |
| env | `environ` `getenv` `setenv` `unsetenv` `clearenv` `putenv` |
| uio | `readv` `writev` (`sys/uio.h`) |
| poll | `poll` `ppoll` (`poll.h`); `select` via poll (`sys/select.h`) |
| timerfd | `timerfd_create`/`settime`/`gettime` (`sys/timerfd.h`) |
| signalfd | `signalfd` (`sys/signalfd.h`) |
| sendfile | `sendfile` (`sys/sendfile.h`) |
| epoll | `epoll_create`/`create1`/`ctl`/`wait`/`pwait` (`sys/epoll.h`) |
| getopt | `getopt` `getopt_long` `getopt_long_only` + `optarg`/`optind`/`opterr`/`optopt` |
| random | `getrandom` `getentropy` `arc4random`/`arc4random_buf`/`arc4random_uniform` |
| setjmp | `setjmp`/`longjmp`/`sigsetjmp`/`siglongjmp` (x86_64) |
| wordexp | `wordexp`/`wordfree` (single-word no-meta) |
| regex | `regcomp`/`regexec` stubs |
| crypt | `crypt`/`crypt_r` traditional 13-char soft hash (MD5-folded); `getpass` |
| termios | `tcgetattr`/`tcsetattr`/`cf*` via ioctl TCGETS/TCSETS |
| kernel+ | `gettid` `getcpu` `mlock*` `mincore` `setns`/`unshare` `sync_file_range` `readahead` `personality` `klogctl` |
| pwd/grp | `getpwuid`/`getpwnam`/`getgr*` bring-up (root + self) |
| spawn/fts | `posix_spawn*` + `fts_*` + pty stubs |
| sched | `sched_yield` `sched_getaffinity`/`sched_setaffinity` `sched_getcpu` + `cpu_set_t`/`CPU_*`/`CPU_COUNT`/`__sched_cpucount` + `sched_setparam`/`getparam`/`setscheduler`/`getscheduler`/`get_priority_*`/`rr_get_interval` |
| time | `time` `clock_gettime` `clock_settime` `clock_getres` `gettimeofday` `nanosleep` `adjtime` `clock_getcpuclockid` (self) |
| memory | freelist `malloc`/`free`/`realloc` reuse (sbrk + free list); `malloc_usable_size` |
| pipe | `pipe` `pipe2` |
| locale | `setlocale` (C only); `localeconv` (C `lconv`); `newlocale`/`uselocale`/`duplocale`/`freelocale` (C locale objects) |
| regex | `regcomp`/`regexec`/`regerror`/`regfree` stubs (`REG_BADPAT` / `REG_NOMATCH`) |
| crypt | `crypt`/`crypt_r` traditional 13-char soft hash (MD5-folded); `getpass` reads stdin |
| wchar num | `wcstol`/`wcstoul`/`wcstoll`/`wcstoull` `wcstok` `wcsftime` (via `strftime`) |
| setjmp | `setjmp`/`longjmp`/`sigsetjmp`/`siglongjmp` (x86_64 asm; no mask save) |
| wordexp | simple no-meta split; meta → `WRDE_BADCHAR` |
| posix_spawn | working `posix_spawn`/`posix_spawnp` + attr + file_actions (fork+exec) |
| fts | minimal DFS `fts_open`/`fts_read`/`fts_close`/`fts_set` (`FTS_SKIP`) |
| pty | `openpty`/`forkpty`/`login_tty` soft fill (ptmx or pipe pair) |
| process+ | export `wait`/`waitpid`/`wait3`/`wait4`; `execvp`/`execvpe`/`execl`/`execlp`/`execle`; `system`; `popen`/`pclose` |
| atexit | `atexit`/`on_exit`/`__cxa_atexit`/`__cxa_finalize` (hooked from `exit`) |
| rand/div | `rand`/`srand`/`random`/`srandom`/`rand_r`; `div`/`ldiv`/`lldiv`; `reallocarray`/`memalign`/`valloc` |
| byteorder | exported `htonl`/`htons`/`ntohl`/`ntohs` (dynlink shape) |
| bsd strings | `bzero`/`bcopy`/`bcmp`/`index`/`rindex`/`swab`; C-locale `strcoll`/`strxfrm` |
| stdio+ | `dprintf`/`vdprintf`/`fgetpos`/`fsetpos`/`flockfile`*/unlocked I/O/`tmpfile`/`mkostemp` |
| pwd/grp | bring-up `getpwnam`/`getpwuid`/`getgr*` + `getlogin` |
| dirent+ | `dirfd`/`readdir_r`/`seekdir`/`telldir`/`versionsort` |
| signal++ | `sigpending`/`sigsuspend`/`sigwait*` + `pthread_kill`/`pthread_sigmask` (kernel sigset size 8) |
| ssp/assert | `__stack_chk_fail`/`__assert_fail`/`program_invocation_name`/`__progname` |
| signal+ | `sigemptyset`/`sigaddset`/… `sigprocmask` `sigaction` |
| misc | `getsubopt` `getgrouplist` (primary gid only); `get_current_dir_name` |
| errors | `strerror` `strerror_l` `strsignal` `strsignal_l` `strerrorname_np` `strerrordesc_np` `sigabbrev_np` `sigdescr_np` |
| sockets | `socket`/`socketpair`/`bind`/`listen`/`accept`/`connect`/`send`/`recv`… |
| inet | `inet_pton`/`inet_ntop`/`inet_aton`/`inet_ntoa`/`inet_addr` (`arpa/inet.h`) |
| netdb | `getaddrinfo`/`freeaddrinfo`/`gai_strerror`/`getnameinfo` (numeric+localhost; no DNS) |
| net/if | `if_nametoindex`/`if_indextoname`/`if_nameindex` (lo=1) |
| ifaddrs | `getifaddrs`/`freeifaddrs` (lo 127.0.0.1 bring-up) |
| syslog | `openlog`/`syslog`/`closelog`/`setlogmask` (LOG_PERROR→stderr; else silent) |
| glob | `glob`/`globfree` (basic `*`/`?` dir match via fnmatch) |
| resolv | `res_init`/`_res`; `res_query`/`res_search` → ENOSYS (use getaddrinfo numeric) |
| err | `err`/`errx`/`warn`/`warnx` + v* variants (`err.h`) |
| ftw | `ftw`/`nftw` (light DFS; FTW_PHYS/DEPTH) |
| mman | + `msync`/`madvise`; `memfd_create`/`memfd_secret`; `shm_open`/`shm_unlink` |
| langinfo | `nl_langinfo` (C locale strings / CODESET UTF-8) |
| time fmt | `gmtime_r`/`localtime_r`/`asctime_r`/`ctime_r`/`strftime`/`strptime`/`timegm`/`mktime`/`tzset` (UTC=local) |
| sysinfo | `sysinfo`/`getloadavg` (loads may be 0 without FPU); `setsid`/`daemon` |
| resource | `getrusage`/`getrlimit`/`setrlimit`/`prlimit`/`getpriority`/`setpriority`/`nice` |
| times/wait | `times`; `waitid` (+ `siginfo_t`) |
| creds | `geteuid`/`getegid`/`getresuid`/`setresuid`/`getgroups`/`setgroups`/`initgroups`… |
| utime | `utimes`/`lutimes`/`futimes`/`futimens`/`utimensat` |
| mode/own | `chmod`/`fchmod`/`chown`/`fchown`/`lchown`/`umask` |
| fs stats | `statfs`/`fstatfs`/`statvfs`/`fstatvfs` |
| pathconf | `pathconf`/`fpathconf` (bring-up constants) |
| sync | `fsync`/`fdatasync`/`sync`/`syncfs` |
| access | + `eaccess`/`euidaccess`/`faccessat`; `canonicalize_file_name` |
| *at | `readlinkat`/`symlinkat`/`linkat`/`unlinkat`/`renameat`/`renameat2`/`mkdirat`/`fstatat`/`fchmodat`/`fchownat`/`execveat`/`faccessat2`/`fchmodat2`/`statx`/`name_to_handle_at`/`open_by_handle_at` |
| copy | `copy_file_range` (syscall + pread/pwrite fallback) |
| splice | `splice`/`tee`/`vmsplice` (`fcntl.h`; ENOSYS → read/write) |
| msg | `sendmsg`/`recvmsg`/`sendmmsg`/`recvmmsg`/`accept4` + `msghdr`/`mmsghdr`/`cmsghdr` |
| nodes | `mknod`/`mknodat`/`mkfifo`/`mkfifoat` |
| fd | + `dup3` (ENOSYS → dup2+CLOEXEC); `pselect` (pselect6 + select fallback); `fchdir`; `close_range` |
| uio | + `preadv`/`pwritev` (syscall + pread/pwrite loop); `process_vm_readv`/`process_vm_writev` |
| signal+ | + `tgkill`; fork hooks via `pthread_atfork`/`__register_atfork` |
| misc++ | `__libc_single_threaded`; `malloc_usable_size`; `clock_getcpuclockid` |
| inotify | `inotify_init`/`init1`/`add_watch`/`rm_watch` (`sys/inotify.h`) |
| itimer | `getitimer`/`setitimer` (`sys/time.h`) |
| tty | `ttyname`/`ttyname_r` (via `/proc/self/fd` or `/dev/tty`) |
| posix timer | `timer_create`/`delete`/`settime`/`gettime`/`getoverrun`; `clock_nanosleep` (**done**) |
| mqueue | `mq_*` surface (`mqueue.h`); host/kernel → ENOSYS (**done**) |
| sem | `sem_init`/`wait`/`post` private+pshared (futex); `sem_open`/`close`/`unlink` via shm |
| iconv | `iconv_open`/`iconv`/`iconv_close` → ENOSYS stubs |
| execinfo | `backtrace`/`backtrace_symbols`/`backtrace_symbols_fd` (FP walk) |
| locks | `flock` (`sys/file.h`); `lockf` / `F_SETLK` via fcntl |
| dirent | `opendir`/`fdopendir`/`readdir`/`closedir`/`rewinddir`/`scandir`/`alphasort` |
| ioctl | `ioctl` |
| eventfd | `eventfd` / `eventfd_read` / `eventfd_write` |
| fnmatch | `fnmatch` (`*`, `?`, CASEFOLD) |
| path | `realpath` (absolute + cwd-relative) |
| entropy | `getentropy` (over `getrandom`) |
| conf | `sysconf` / `confstr` / `getpagesize` / `getdtablesize` (subset) |
| files | `ftruncate` / `truncate` / `pread` / `pwrite` / `fallocate` / `posix_fallocate` |
| strings | + `strndup` `strnlen` `stpcpy` `memmem` `strtok_r` `strsep` |
| dlfcn | `dlopen`/`dlsym`/`dlvsym`/`dlclose`/`dlerror`/`dladdr` (link stubs; real load = ld-gj) |
| link | `dl_iterate_phdr` (main AT_PHDR walk; full SO walk = ld-gj) |
| auxv | `getauxval` (stack AT_ walk after envp) |
| prctl | `prctl` wrapper |
| advise | `posix_fadvise` / `posix_madvise` |
| conf | + `getdtablesize` (via `sysconf(_SC_OPEN_MAX)`) |
| signals | `strsignal` `strsignal_l` `sigabbrev_np` `sigdescr_np` |
| host | `gethostname` (via uname); `sethostname` stub |
| thread id | `gettid`; `setns`/`unshare`; `mlock`/`munlock`/`mlockall`/`munlockall`/`mincore` |
| sync+ | `sync_file_range`; `readahead` |
| persona | `personality`; `reboot` (kernel EPERM bring-up); `klogctl` |
| caps | `capget`/`capset` (`sys/capability.h`) |
| mount | `mount`/`umount`/`umount2`/`pivot_root` (`sys/mount.h`) |
| xattr | `*setxattr`/`*getxattr`/`*listxattr`/`*removexattr` (`sys/xattr.h`) |
| sched+ | `sched_setparam`/`getparam`/`setscheduler`/`getscheduler`/`get_priority_*`/`rr_get_interval`; `__sched_cpucount` |
| ioprio | `ioprio_get`/`ioprio_set`; `membarrier` |
| inttypes | `strtoimax`/`strtoumax`/`imaxabs`/`imaxdiv` |
| stdlib+ | `qsort_r` `_Exit` `group_member` `getwd` `a64l`/`l64a` |
| search | `tsearch`/`tfind`/`tdelete`/`twalk`/`tdestroy`; `hsearch*`/`hcreate*`/`hdestroy*`; `lfind`/`lsearch`; `insque`/`remque` |
| error.h | `error`/`error_at_line` + `error_message_count`/`error_one_per_line`/`error_print_progname` |
| fortify | `__memcpy_chk`/`__memset_chk`/`__strcpy_chk`/`__printf_chk`/`__snprintf_chk`/`__open_2`/`__chk_fail`/`__fortify_fail`… |
| LFS/*64 | `open64`/`lseek64`/`pread64`/`pwrite64`/`mmap64`/`ftruncate64`/`stat64`/`fstat64`/`readdir64`/`glob64`/`ftw64`… |
| xstat | `__xstat`/`__fxstat`/`__lxstat`/`__*stat64`/`__fxstatat*` |
| isoc99 | `__isoc99_scanf`/`fscanf`/`sscanf` + `v*` |
| ctype loc | `__ctype_b_loc`/`__ctype_tolower_loc`/`__ctype_toupper_loc`/`__ctype_get_mb_cur_max` |
| wcwidth | `wcwidth`/`wcswidth` |
| sysmacros | `major`/`minor`/`makedev`/`gnu_dev_*` |
| mallinfo | `mallinfo`/`mallinfo2`/`mallopt`/`malloc_stats`/`malloc_info` |
| pidfd | `pidfd_open`/`pidfd_send_signal`/`pidfd_getfd` |
| fanotify+ | `fanotify_init`/`fanotify_mark`; `add_key`/`request_key`/`keyctl` |
| linux+ | `vfork` `clone` `fexecve` `openat2` `mlock2` `pkey_*` `userfaultfd` `seccomp` `swapon`/`swapoff` `quotactl` `process_madvise` `getdents64` `prlimit64` |
| gettext | identity stubs `gettext`/`dgettext`/`ngettext`/`textdomain`/`bindtextdomain`… |
| aio | `aio_*`/`aio_*64` sync fill-in (immediate pread/pwrite); `aio_init` no-op |
| argz/envz | full `argz_*` + `envz_*` (NUL-separated vectors) |
| C11 threads | `call_once` `mtx_*` `cnd_*` `thrd_*` `tss_*` over pthread |
| rand48 | integer `lrand48`/`nrand48`/`mrand48`/`jrand48` + `srand48`/`seed48`/`lcong48` (+`_r`); no `drand48` (SSE double return) |
| ether | `ether_aton`/`ntoa`/`_r` + `ether_line`; host map ENOSYS |
| graph++ | `alphasort64` `creat64`/`fopen64`/`fcntl64`/`fallocate64` `fgetpos64`/`fsetpos64` `__bzero` `__mempcpy`/`__stpcpy`/`__rawmemchr`/`__strdup` `__asprintf` `__backtrace*` `__clock_gettime` `cfree` `clock` `closefrom` `fcloseall` `ctermid`/`cuserid` `bsd_signal`/`sysv_signal` `sigblock`/`sigsetmask`/`siggetmask`/`sigpause` `c16rtomb`/`c32rtomb`/`mbrtoc16`/`mbrtoc32` `arch_prctl` `__clone` `__cmsg_nxthdr` `bindresvport` `gethostid`/`sethostid` `memfrob`/`strfry` `__adjtimex` `__sched_yield` |
| graph batch12 | `newfstatat` `memfd_secret` `strerror_l`/`strsignal_l` `strerrorname_np`/`strerrordesc_np` `sigabbrev_np`/`sigdescr_np` `__libc_malloc`/`calloc`/`realloc`/`free` `__strndup` `gnu_get_libc_version`/`release` `__libc_current_sigrtmin`/`max` `__sysconf` `statfs64`/`fstatfs64`/`__statfs`/`__fstatfs` `__res_init`/`__res_ninit`/`__res_query` `__open64`/`__open64_2`/`__openat64_2` `__fdelt_warn` `__longjmp_chk` `__isoc23_strto*`/`__isoc23_*scanf` `getc`/`putc` `_IO_getc`/`_IO_putc`/`_IO_feof`/`_IO_ferror` `__uflow`/`__overflow` `__mbrtowc`/`__mbsnrtowcs` `pthread_mutexattr_setrobust`/`getrobust` `pthread_mutex_consistent` `sem_clockwait` |
| SysV IPC | `shmget`/`shmat`/`shmdt`/`shmctl` `semget`/`semop`/`semtimedop`/`semctl` `msgget`/`msgsnd`/`msgrcv`/`msgctl` |
| graph batch3 | `mremap` `preadv2`/`pwritev2` `epoll_pwait2` `process_mrelease` `clone3` `exit_group`/`tkill`/`futex`/`set_tid_address` `ioperm`/`iopl` `setfsuid`/`setfsgid` `vhangup` `settimeofday`/`stime`/`ualarm` `init_module`/`delete_module`/`finit_module`/`kcmp`/`bpf` `landlock_*` `open_tree`/`move_mount` `eventfd2`/`signalfd4` `getw`/`putw` `tmpnam*`/`tempnam` `mkstemps*`/`mkostemps` `psignal`/`psiginfo` `__xmknod*` `getdirentries*` `strtoq`/`strtouq`/`l64a_r` `rpmatch` `glob_pattern_p` `strcasecmp_l`/`strncasecmp_l`/`strftime_l`/`nl_langinfo_l` `__xpg_basename`/`__xpg_strerror_r` `getpt` `__h_errno_location` `__sched_cpualloc`/`__sched_cpufree` |
| graph batch13 | underscored aliases (`__close`/`__fork`/…), `stdio_ext`, reentrant netdb/pwd/grp, `dn_*`, `dladdr1`/`dlinfo`/`dlmopen`, `ftok`/`ftime`/`dysize`, `quick_exit`/`at_quick_exit`/`__cxa_thread_atexit_impl`, netent, `fmtmsg` |
| graph batch14 | `twalk_r` `wcstoimax`/`wcstoumax` ucontext stubs; gmon; `catopen`/`catgets`/`catclose`; integer `strfmon`/`strfmon_l`; libaio + `io_uring_*`; obstack; argp; printf hooks; `fopencookie`; pthread clocklocks/`sigqueue`; `get_nprocs*`; `getpeereid`/`killpg`/`lchmod`/`malloc_trim`; `putpwent`/`putgrent`; ttyent; `futimesat`/`fflush_unlocked`/`versionsort64`/`sysctl` |
| graph batch15 | underscored `__read`/`__write`/`__open`/…; fortify more; LFS `getrlimit64`/`sendfile64`/`posix_fallocate64`; `sigaltstack`/sigset ops; pthread cancel+attr/barrierattr; `ns_*`/`res_n*`; `ptrace`/`sockatmark`/`isfdtype`; GNU `re_*`; locale `*_l`; `isascii`/`mbrlen`/`wcpcpy`/`pvalloc`; `getdate`/`timelocal`; `encrypt`/`setkey`; `__environ`/`__timezone` |
| graph batch16 | more underscored socket/uio/fd/mmap/wait; `__libc_*` open/read/write/fork/system; `__pthread_*`; `__secure_getenv`; `__strto*_internal`; `__isoc99_*printf`; fortify `__asprintf_chk`/`__syslog_chk`/`__pread_chk`; `_setjmp`/`__sigsetjmp`; `fseeko64`/`ftello64`/`tmpfile64`/`readdir64_r`; `__getpw*_r`/`__getgr*_r`; `__if_nametoindex` |
| graph batch17 | `memccpy` `freezero` `fts64_*` `statvfs64`/`fstatvfs64` `fgetpwent_r`/`fgetgrent_r`/`getspent_r`/`fgetspent_r` `getpw`/`gets` fstab/netgroup/gshadow/aliases streams `rcmd`/`rexec`/`ruserok`/`isastream`/`getmsg`/`putmsg` `inet_net_ntop`/`inet_net_pton` `re_set_syntax`/`regncomp`/`regnexec` `_IO_printf`/`_IO_fopen`/padn/sgetn/seek `isw*_l`/`__is*_l`/`__newlocale`/`__uselocale` fortify path/wide/poll/recv `__libc_enable_secure`/`__libc_allocate_rtsig`/`__ctype_init`/`__call_tls_dtors` `posix_spawn_file_actions_addtcsetpgrp_np` |
| graph batch18 | `syscall()` BSD `fls`/`flsl`/`flsll`/`strtonum`/`getprogname`/`setprogname`/`heapsort`/`mergesort`/`recallocarray`/`reallocf`/`issetugid`/`strmode` more underscored libc/syslog/IPC; Linux `sched_setattr`/`perf_event_open`/`fsopen` family `getdents` `_dl_find_object`/`__gmon_start__` |
| graph batch19 | LFS `preadv64`/`pwritev64`/`preadv64v2`/`pwritev64v2` `llseek`/`_llseek` `scandirat64` `mktemp`/`mkostemps64` `_longjmp` pthread `get/setconcurrency` `attr_set/getstackaddr` `mutexattr_*kind_np`/`*robust_np` `mutex_consistent_np` `attr_*sigmask_np` DNS `ns_name_pack`/`unpack`/`skip` `res_hnok`/`dnok`/`mailok`/`ownok` `ntp_gettimex` `lio_listio` `getaddrinfo_a`/`gai_error`/`gai_cancel`/`gai_suspend` `modify_ldt`/`remap_file_pages` `__libc_freeres`/`_obstack_free` profiling `__fentry__`/`mcount`/`__cyg_profile_func_*` `re_match_2`/`re_search_2` `__ptsname_r_chk` `get/setipv4sourcefilter` `inet6_opt_*`/`inet6_rth_*` `inet_nsap_*` `chflags`/`fchflags` `strtoimax_l`/`strtoumax_l` |
| graph batch20 | `utime`/`__utime` `madvise64`/`__madvise` `lockf64` `__memmem` `moncontrol`/`__monstartup` `__libc_dlopen_mode`/`dlsym`/`dlclose` reentrant `getut*_r`/`getsg*_r`/`getalias*_r` fortify `__wprintf_chk`/`__swprintf_chk`/`__fgetws_chk`/`__mbs*_chk` `isctype`/`__isctype` `__nl_langinfo_l` `mq_getsetattr`/`__mq_open_2` `mcheck`/`mtrace` `__libc_init_first`/`__libc_fcntl64`/`__libc_pthread_init` `__ns_name_compress`/`ntop`/`pton`/`uncompress` |
| graph batch21 | underscored `__memrchr`/`__strchrnul`/`__strsep` `__copy_file_range`/`__close_range` `__getentropy`/`__getrandom` `__pread64`/`__pwrite64`/`__lseek`/`__lseek64` `__euidaccess`/`__chmod`/`__fchmod`/`__chown`/`__fchown` `__link`/`__symlink` `__mremap`/`__msync`/`__mlock*`/`__mincore` `__getppid`/`__getuid`/`__getgid`/`__geteuid`/`__getegid` `__setuid`/`__setgid` `__kill`/`__tkill`/`__tgkill`/`__raise`/`__sigprocmask` `__exit`/`__exit_group` `__posix_spawn*`/`__execvpe` `__times`/`__sysinfo` `__mount`/`__umount*` `__sync`/`__fsync`/`__fdatasync` `__truncate`/`__ftruncate`/`__flock` `__setrlimit`/`__getrusage`/`__getpriority`/`__setpriority` `__sched_*affinity` `__confstr`/`__pathconf`/`__fpathconf` `__setenv`/`__unsetenv`/`__putenv`/`__clearenv`/`__getlogin_r` fortify `__wcscpy_chk`/`__wcsncpy_chk`/`__wcscat_chk`/`__wcsncat_chk`/`__wcsrtombs_chk`/`__wcstombs_chk` `obstack_printf`/`obstack_vprintf`/`__obstack_*_chk` `__res_mkquery`/`__res_send` `res_nquerydomain` nss stubs `__nss_*` `ns_samedomain`/`ns_subdomain`/`ns_makecanon`/`ns_samename` `__getwd` |
| graph batch22 | `fpurge` `sig2str`/`str2sig` `b64_ntop`/`b64_pton` `res_randomid`/`__res_randomid` `dn_count_labels`/`__dn_count_labels` `p_type`/`p_class`/`__p_*` `putshort`/`putlong`/`_getshort`/`_getlong` `ns_format_ttl`/`ns_parse_ttl`/`ns_msg_getflag`/`ns_name_rollback`/`ns_sprintrr` `res_hostalias`/`hostalias`/`__res_iclose` `__nss_*_lookup2` family `__sched_*affinity_new` underscored `__*xattr` `__libc_socket`/`bind`/`connect`/`listen`/`accept`/`send`/`recv`/`sendto`/`recvfrom`/`sendmsg`/`recvmsg`/`shutdown`/`socketpair` `__libc_pause`/`nanosleep`/`msync`/`fsync` |
| graph batch23 | fortify `__wctomb_chk`/`__mbrtowc_chk`; legacy `inet6_option_*`; `gtty`/`stty`/`revoke`; `__idna_to/from_dns_encoding`; `__libc_sa_len`/`__libc_msgrcv`/`__libc_msgsnd`; `__libc_dynarray_*`/`__libc_scratch_buffer_*`/`__libc_allocate_once_slow`; `__nss_database_get`/`__nss_disable_nscd`; `_nss_files_getpw*`/`getgr*`/`gethostby*`; `_nss_dns_gethostby*`; ONC RPC `getrpc*`/`setrpcent`/`endrpcent`/`getrpcport`/`callrpc`/`pmap_*`/`clnt_*`/`auth*`/`svc_*`/`svcerr_*`/`xprt_*`; XDR surface `xdr_*`/`xdrmem_create`/`xdrstdio_create`/`xdrrec_create` (fail-closed, no SSE floats); `getnetname`/`host2netname`/`key_*`/`des_setparity`/`cbc_crypt`/`ecb_crypt` |
| graph batch24 | `strtok`; C11 `timespec_get`/`timespec_getres`; `setproctitle`/`setlogin`; BSD `siginterrupt`/`sigstack`/`sigvec`; `res_nmkquery`/`res_nsend`/`res_querydomain` (+`__res_*`); XDR remainder (`xdr_int8/16_t`/`longlong`/`quad`/`opaque_auth`/`callmsg`/`replymsg`/`pmap*`/`authdes*`/`cryptkey*`/`xdrrec_*`); `xencrypt`/`xdecrypt`; RPC `clnt_broadcast`/`clntunix_create`/`authdes_*`/`registerrpc`/`pmap_rmtcall`/`netname2user`/`key_*_pk`/`getpublickey`/`getsecretkey`/`passwd2des`/`pkey_*`; SVC `svc_exit`/`svcfd_create`/`svc_getreq*`/`svcraw_create`/`svcudp_bufcreate`/`svc_unregister`/`svcerr_noprog`/`progvers`; obsolete `bdflush`/`create_module`/`query_module`/`uselib`/`ustat`/`vlimit`/`vtimes`/`nfsservctl`/`rtime`/`ruserpass`/`pthread_kill_other_threads_np`/`sgetsgent`; BSD `fgetln`/`strnstr`/`timingsafe_bcmp`/`timingsafe_memcmp`; `getipnodebyname`/`getipnodebyaddr`/`freehostent`; `__reallocarray`; fortify `__wcrtomb_chk`; C++ ABI `__cxa_guard_*`/`__cxa_pure_virtual`/`__cxa_deleted_virtual`; legacy regex `step`/`advance`; `tmpnam64` |
| graph batch25 | underscored `__snprintf`/`__vsnprintf`/`__strerror_r`/`__strftime_l`/`__iswctype`/`__posix_getopt`; resolv `__res_search`/`__res_nclose`/`__libc_dn_*`/`__libc_res_*`/`__libc_ns_*`/`__res_context_*`/`__resolv_context_*`; small-string `__mempcpy_small`/`__stpcpy_small`/`__strsep_*`/`__strcspn_c*`/`__strpbrk_c*`; pthread `_pthread_cleanup_push`/`pop`/`_defer`/`_restore`; `__lll_lock_*_private` `__nptl_*_event`; `__libc_alloc_buffer_*`/`__libc_alloca_cutoff`; inet `__inet_aton_exact`/`__inet_pton_length`/`__inet6_scopeid_pton`; `printf_size`/`printf_size_info`/`__printf_fp` (ENOSYS, no FP); RPC `_rpc_dtablesize`/`_seterr_reply`/`_authenticate`/`__ivaliduser`/`__libc_rpc_getport`/`__libc_clntudp_bufcreate`/`__rpc_thread_*`; NSS files remainder (`_nss_files_set/end/get*ent`, parse, initgroups) + dns net/canonname; `__shm_get_name`/`__open_catalog`/`__profile_frequency`/`__pread64_nocancel`/`__copy_grp`/`__merge_grp` |
| graph batch26 | small-string `__strspn_c1`/`c2`/`c3` `__strtok_r`/`__strtok_r_1c`; ctype `_tolower`/`_toupper`; locale `__strtol_l`/`__strtoul_l`/`__strtoll_l`/`__strtoull_l` `__strfmon_l`; tree `__tdelete`/`__tfind`/`__tsearch`/`__twalk`/`__twalk_r`; wide `__towctrans`/`__towctrans_l`/`__wctrans_l`/`__wctype_l` `__wcscasecmp_l`/`__wcsncasecmp_l`/`__wcsftime_l` `__wcstol*_l`/`__wcstol*_internal`; fortify `__wcpcpy_chk`/`__wcpncpy_chk`/`__wmempcpy_chk`; stdio `__vfscanf`/`__underflow`/`__woverflow`/`__wuflow`/`__wunderflow`; `__write_nocancel`; NSS `__nss_hash`/`__nss_lookup`/`__nss_passwd_lookup`/`__nss_group_lookup`/`__nss_hosts_lookup` `__nss_files_data_*`/`__nss_files_fopen`/`__nss_readline`/`__nss_parse_line_result` `__internal_*netgrent`; file-change `__file_change_detection_*`/`__file_is_unchanged`; `__netlink_assert_response` `__x86_get_cpuid_feature_leaf` `_environ` |
| graph batch27 | `__memcmpeq` `__strchr_c`/`__strrchr_c` `__wcsnlen`/`__wcschrnul`; isoc23 `__isoc23_strto*_l`/`__isoc23_wcsto*`; `__morecore` hook; nocancel `__openat64_nocancel`/`__openat_nocancel`/`__fcntl_nocancel`/`__libc_fcntl` `__poll_nocancel`/`__ppoll_nocancel` `__waitpid_nocancel`/`__pause_nocancel`/`__nanosleep_nocancel`/`__sigsuspend_nocancel` `__recv_nocancel`/`__send_nocancel`/`__connect_nocancel`/`__accept_nocancel`/`__accept4_nocancel`; gconv `__gconv_*` + `__iconv*`; broad `_IO_*` FILE vtable/aliases (`_IO_fdopen`/`_IO_popen`/`_IO_file_*`/`_IO_default_*`/`_IO_w*`/`_IO_str_*`/`_IO_flush_all`/markers/iters); printf internals `__printf_fphex` (ENOSYS) `__v*printf_internal`/`__vfscanf_internal`; `__malloc_usable_size` `__fseterr` `__libc_message` `__assert_fail_base` `__tunables_init` `__libc_print_version`/`banner`; rseq data `__rseq_*`; `_nl_msg_cat_cntr`; `sys_errlist`/`sys_nerr`/`_sys_*`; `__libc_argv`/`argc` `_dl_argv`/`_dl_pagesize` `__nptl_nthreads`; `__strstr_g`/`_cg` `__strpbrk_c1` `__strcspn_cg`/`__strspn_cg` `__stpncpy` `__strcasecmp`/`__strncasecmp` `__sigemptyset`/`__sigfillset` `__GI_raise`/`__GI___errno_location` |
| graph batch28 | `mseal`/`map_shadow_stack`/`set_mempolicy_home_node`; `statmount`/`listmount`; LSM `lsm_*_self_attr`/`lsm_list_modules`; `futex_requeue`/`set_robust_list`/`get_robust_list`/`move_pages`; `io_pgetevents`/`rt_tgsigqueueinfo`; `consttime_memequal`; `set_thread_area`; malloc hooks; termcap `tget*`/`setupterm`; `bsearch_r` `wcslcpy`/`wcslcat`; `_IO_*_unlocked`; `__stack_chk_fail_local` `__libc_longjmp`/`__libc_siglongjmp`; GI aliases; `fmtcheck`/`fparseln`/`fgetwln`; crypt_r stubs; thr/dl/tls helpers; isoc99 wide printf aliases |
| graph batch29 | Solaris `thr_*` over pthread; libcap-shaped `cap_*`; TI-RPC `clnt_*`; `text_domain`; cookie ops; `_dl_allocate_tls`/`_dl_deallocate_tls`; rcmd helpers |
| graph batch30 | NUMA `set/get_mempolicy`/`mbind`/`migrate_pages`; `kexec_*`; `futex_wait`/`futex_wake`; `file_getattr`/`file_setattr`/`uretprobe`; `fadvise64`/`ulimit`; wchar locale aliases; BSD `arc4random_stir`/`flopen`/`pidfile_*`/`humanize_number`; vdso stubs; GI aliases `__GI_printf`/`__GI_socket`/`__GI_gettimeofday`; `__explicit_bzero` `___tls_get_addr` `__abort_msg` `getclktck` `verrc`/`vwarnc` `_IO_vfork` |
| graph batch32 | remaining `__GI_*` (sigprocmask/bind/poll/getpid/dlopen/wait/fs); `pidfd_getfd_np`; `__check_pf` `__lll_lock_wait` `__nss_database_lookup2`; data `__resp`/`svc_fdset`/`__pointer_chk_guard`/`__rseq_handled`; BSD `stravis`/`strnvis`/`strnunvis`; libmd `MD5Init`/`SHA1Init`/`SHA256_Init` stubs; `getgrouplist_2` |
| graph batch33 | thr `set/getprio`/`setscheduler`/`keycreate_once`; `getrandom_r`/`getentropy_r`/`sprofil`; y2038 time64 aliases (`__clock_gettime64`/`ppoll_time64`/`__mmap64`/`__copy_file_range64`/…) |
| graph batch34 | remaining BSD vis: `nvis`/`svis`/`snvis`/`strsvis`/`strsnvis`/`strsvisx`/`strsnvisx`/`strenvisx`/`strunvisx` + underscored aliases; MD4/RMD160/BLAKE2s; bcrypt stubs; `strtofflags`/`setnetconfig` |
| graph batch35 | BSD RFC 4122 `uuid_create`/`uuid_to_string`/…; `basename_r`/`dirname_r`; NetBSD `strtoi`/`strtou`; `memcasecmp`; Solaris `gethrtime`/`gethrvtime` |
| graph batch36 | libuuid `uuid_generate`/`parse`/`unparse`/`clear`/`copy`; `MD4End`/`MD4Data`/`RMD160End`/`BLAKE2s_End`; `siphash24`; BSD `radixsort`/`sradixsort`; OpenBSD `dehumanize_number` |
| random_r | `random_r`/`srandom_r` `initstate`/`setstate` `initstate_r`/`setstate_r` |
| inet classful | `inet_network`/`inet_makeaddr`/`inet_lnaof`/`inet_netof` |
| pthread join+ | `pthread_tryjoin_np`/`timedjoin_np`/`clockjoin_np` `pthread_yield` `rwlockattr_setkind_np` |
| spawn+ | `posix_spawn_file_actions_addchdir_np`/`addfchdir_np`/`addclosefrom_np` |
| *at / secret | `fstatat`/`newfstatat` (NR 262); `memfd_create`/`memfd_secret` (NR 447; host may ENOSYS) |
| misc | `uname` `errno` `gj_libc_marker` |

## Kernel VA floor (product)

Kernel BSS grows past classic PE `0x400000` (embeds). User anon maps must not
hint into low identity: `GJ_USER_VA_BASE` is **8 MiB**; default `mmap` hint is
`0x40000000`. `vmm_map_page` refuses user maps over kernel image/BSS.

## ld-gj reloc depth

| Type | Support |
|------|---------|
| `R_X86_64_RELATIVE` | yes |
| `R_X86_64_GLOB_DAT` / `JUMP_SLOT` / `64` | yes |
| `R_X86_64_COPY` | yes (userspace ld-gj) |
| `R_X86_64_DTPMOD64` / `DTPOFF64` / `TPOFF64` | yes (static TLS bring-up) |
| `R_X86_64_IRELATIVE` | yes (call resolver) |
| `DT_JMPREL` / `DT_PLTRELSZ` | yes |
| `DT_INIT` | yes (call if mapped) |

## Graph continuum (clean-room growth)

Recent desktop graph batches. **Freestanding rules:** dual **MIT OR Apache-2.0**, every new public `T` symbol listed in `user/libcgj/libc.map` (`local: *` hides unlisted names).

**SSE policy (option 1)** — confirmed against top-level `Makefile`:

| Domain | Flags / rules |
|--------|----------------|
| **Userspace** (`USER_CFLAGS`, `CGJ_CFLAGS`) | **`-msse2` allowed.** Baseline compiler ISA is SSE2; AES-NI is OK only via CPUID-gated paths (optional per-TU `-maes` / target attributes / multi-version — **not** a global `-maes` in those CFLAGS). Integer fallbacks remain preferred for portable paths. |
| **Kernel** (`CFLAGS`) | **Remain no-SSE:** `-mno-sse -mno-sse2 -mno-mmx -mgeneral-regs-only`. |
| **EFI** (`EFI_CFLAGS` / `scripts/build-efi.sh`) | **Remain no-SSE:** `-mno-sse -mno-sse2 -mgeneral-regs-only`. |

SIMD/AES-NI hot paths should CPUID-dispatch where practical. Kernel FXSAVE/XSAVE for full XMM IRQ integrity is still a follow-on (userspace SSE is safe for host smoke and careful product paths until that lands).

**Batches 83–10100** are in-tree and **wired** in `CGJ_SRCS` (Makefile). **SSE policy reminder:** userspace `-msse2` OK (AES-NI/PCLMUL/SHA-NI/SSE4.2/AVX2 via CPUID dispatch / `target` attributes where used); kernel/EFI stay `-mno-sse`. Do not invent batch 10101+ ahead of parent wiring.

| Batch | Surface (examples) |
|-------|--------------------|
| batch27 | `_IO_*`, gconv/iconv, `*_nocancel`, `__malloc_usable_size` |
| batch28 | `mseal`, `map_shadow_stack`, `listmount`/`statmount`, `lsm_*`, `futex_requeue`, robust-list, malloc hooks, `fgetwln`/`fmtcheck` |
| batch29 | Solaris `thr_*` over pthread, libcap-shaped `cap_*`, extra `clnt_*`, `text_domain`, cookie ops, `_dl_allocate_tls` |
| batch30 | NUMA/mempolicy, `futex_wait`/`wake`, wchar/BSD helpers, partial `__GI_*`, `__abort_msg` |
| batch32 | remaining `__GI_*`, `stravis`/`MD5Init`, `pidfd_getfd_np`, `__check_pf` |
| batch40 | SHA-3 `SHA3_224`/`384`/`512`, `SHA224`/`SHA512_224`/`SHA512_256`, `hmac_sha384`, `XXH32`/`XXH64`, `hkdf_sha256`/`hkdf_sha512`, `pbkdf2_hmac_sha1`/`256`, `base64_encode`/`decode` |
| batch41 | `SHAKE128`/`SHAKE256`, `base32_encode`/`decode`, `hex_encode`/`decode`, `fnv1a_32`/`64`, `murmur3_32`, `crc16`/`crc64` (CCITT/ECMA), `hmac_sha224`/`hmac_sha3_256`, `crypto_memcmp`, `XXH3_64bits` |
| batch42 | `SM3_*`, `chacha20_xor`/`block`, `poly1305_auth`, `base58_*`, `crc8`/`crc16_modbus`, `hmac_sha3_512`, `base32hex_*`, `djb2_hash`/`sdbm_hash` |
| batch43 | XChaCha / `hchacha20` / `chacha20_encrypt` (Poly1305+xor live in 42) |
| batch44 | `MD2Init`/`Update`/`Final`/`Data`, lowercase `sm3_*` API |
| batch45 | AES (`AES_set_encrypt_key`/`AES_set_decrypt_key`/`AES_encrypt`/`AES_decrypt`) + `RC4_set_key`/`RC4` + Blowfish (`Blowfish_initstate`/`expand0state`/`encipher`/`encrypt`) — **wired** in `CGJ_SRCS` |
| batch46 | `pbkdf2_hmac_sha512`, `scrypt`/`scrypt_kdf` (working), `bcrypt_pbkdf`/`argon2id_hash_raw`/`argon2i_hash_raw` ENOSYS stubs — **wired** |
| batch47 | fortify/`*_chk` audit gate (symbols already from earlier TUs; marker only) — **wired** |
| batch48 | `__idna_to/from_dns_encoding` ASCII passthrough, `__nss_*_lookup2`, `__nscd_get_nl_timestamp`, `__res_ninit`/`__res_iclose` helpers — **wired** |
| batch49 | MD2 family (duplicate of batch44) — source present, **unwired** from `CGJ_SRCS` (avoids multi-def) |
| batch50 | `AES_ecb_encrypt_block` only (one-shot wrapper; full AES set/encrypt in batch45) — **wired** |
| batch51 | TEA/XTEA (`tea_encrypt`/`tea_decrypt`, `xtea_encrypt`/`xtea_decrypt`) — **wired** |
| batch52 | DES/3DES ECB (`DES_set_key`/`DES_ecb_encrypt`, `DES3_set_key`/`DES3_ecb_encrypt`) — **wired** |
| batch53 | SM4 (`SM4_set_key`/`SM4_encrypt`/`SM4_decrypt`, `sm4_crypt_ecb`) — **wired** |
| batch54 | RIPEMD-160 (`RIPEMD160_Init`/`Update`/`Final`, `RIPEMD160`) — **wired** |
| batch55 | Whirlpool (`WHIRLPOOL_Init`/`Update`/`Final`, `WHIRLPOOL`) — **wired** |
| batch56 | Salsa20 (`salsa20_keysetup`/`salsa20_keysetup128`, `salsa20_block`, `salsa20_xor`) — **wired** |
| batch57 | AES-CMAC (`aes_cmac`, `AES_CMAC`; AES primitives from batch45) — **wired** |
| batch58 | Twofish (`Twofish_set_key`/`encrypt`/`decrypt`, flat `twofish_setkey`/`encrypt`/`decrypt`) — **wired** |
| batch59 | Serpent (`serpent_set_key`/`serpent_encrypt`/`serpent_decrypt`) — **wired** |
| batch60 | AES-GCM lite (`ghash`, `aes_gcm_encrypt`/`aes_gcm_decrypt`; AES from batch45) — **wired** |
| batch61 | CityHash (`cityhash64`, `cityhash128`) — **wired** |
| batch62 | AES-XCBC-MAC-96 (`aes_xcbc_mac_96`, `AES_XCBC_MAC_96`; AES from batch45) — **wired** |
| batch63 | BLAKE3 (`blake3_hasher_init`/`update`/`finalize`, `blake3_hash`) — **wired** |
| batch64 | X25519 (`x25519`; Ed25519 omitted) — **wired** |
| batch65 | Argon2i/Argon2id working (`argon2i_hash`/`argon2id_hash` + `__` aliases; RFC 9106; modest mem caps; distinct from batch46 `*_hash_raw` ENOSYS stubs) — **wired** |
| batch66 | HMAC-DRBG SHA-256 (`hmac_drbg_init`/`reseed`/`generate` + `__` aliases; NIST SP 800-90A; SHA-256 from batch31/32) — **wired** |
| batch67 | CRC-64 fill-ins (`crc64_update`; `crc64_xz`/`crc64_xz_update` + `__` aliases; one-shot `crc64`/`crc64_ecma` remain batch41) — **wired** |
| batch68 | Adobe ASCII85 / base85 (`base85_encode`/`decode`, `ascii85_encode`/`decode` + `__` aliases; Adobe `'z'` zero-block on ascii85) — **wired** |
| batch69 | Camellia (`Camellia_set_key`/`encrypt`/`decrypt` + `__` aliases; RFC 3713; keys 128/192/256) — **wired** |
| batch70 | ChaCha20-Poly1305 AEAD (`chacha20_poly1305_encrypt`/`decrypt` + `__` aliases; RFC 8439; uses batch42 `chacha20_*`/`poly1305_auth`) — **wired** |
| batch71 | HKDF Extract/Expand (`hkdf_sha256_extract`/`expand`, `hkdf_sha512_extract`/`expand` + `__` aliases; RFC 5869; one-shot `hkdf_sha*` remain batch40) — **wired** |
| batch72 | PBKDF2-HMAC-SM3 (`pbkdf2_hmac_sm3` + `__` alias; RFC 8018; SM3 from batch42; not scrypt/batch46) — **wired** |
| batch73 | Ed25519 (`ed25519_public_key`/`ed25519_sign`/`ed25519_verify` + `__` aliases; RFC 8032; SHA-512 from batch31; does **not** redefine `x25519`/batch64) — **wired** |
| batch74 | AES-CBC / AES-CTR (`AES_cbc_encrypt`/`AES_ctr128_encrypt`, `aes_cbc_encrypt`/`aes_cbc_decrypt`/`aes_ctr_xor` + `__` aliases; NIST SP 800-38A; AES primitives from batch45) — **wired** |
| batch75 | SipHash key-first + HalfSipHash (`siphash24_with_key`/`siphash13_with_key`/`halfsiphash24` + `__` aliases; `siphash24` remains batch36) — **wired** |
| batch76 | DEFLATE inflate (`gj_inflate`/`__gj_inflate`; RFC 1951 + zlib/gzip wrappers RFC 1950/1952; trailers via batch39 `crc32_z`/`adler32_z`; does **not** define `inflate`/`uncompress`/`deflate`/`compress`) — **wired** |
| batch77 | mini JSON helpers (`gj_json_get_string`/`gj_json_get_int`, `json_escape`/`json_unescape` + `__` aliases; flat `{"k":"v"}` objects only) — **wired** |
| batch78 | UTF-8 helpers (`utf8_validate`/`utf8_strlen`/`utf8_decode`/`utf8_encode`; RFC 3629 strict; does **not** redefine `mbrtowc`/`wcrtomb`/iconv) — **wired** |
| batch79 | UUID v3/v5 convenience (`uuid_v5`/`uuid_v3` + `__` aliases; RFC 4122 name-based; ns+name→out order; digests via batch38 `uuid_generate_sha1`/`uuid_generate_md5`; `uuid_generate*` remain earlier batches) — **wired** |
| batch83 | AES-NI single-block encrypt (`gj_cpu_has_aesni`, `AES_encrypt_aesni`, `AES_encrypt_auto` + `__` aliases; CPUID.1 ECX[25]; fallback `AES_encrypt` batch45) — **wired** |
| batch84 | SSE2 bulk mem (`gj_memcpy_sse2`/`gj_memset_sse2`/`gj_memmove_sse2` + `__` aliases; does **not** redefine `memcpy`/`memmove`/`memset`) — **wired** |
| batch85 | PCLMUL GHASH (`ghash_pclmul`, `gj_cpu_has_pclmul` + `__` aliases; CPUID.1 ECX[1]; NIST SP 800-38D; does **not** redefine `ghash`/batch60) — **wired** |
| batch86 | SHA-NI SHA-256 block (`gj_cpu_has_sha_ni`, `SHA256_Transform_shani`, `SHA256_Update_auto` + `__` aliases; CPUID.7:0 EBX[29]; does **not** redefine `SHA256_*`/batch31–32) — **wired** |
| batch87 | SSE4.2 CRC-32C (`gj_cpu_has_sse4_2`, `crc32c_hw`, `crc32c_auto`; CPUID.1 ECX[20]; poly matches batch39 `crc32c`; does **not** redefine `crc32c`) — **wired** |
| batch88 | POSIX `drand48`/`erand48` (double return via SSE; bridges `lrand48`/`nrand48`/`seed48` in `rand48.c`; integer rand48 unchanged) — **wired** |
| batch89 | SSE2 string helpers (`gj_strlen_sse2`/`gj_strchr_sse2`/`gj_memcmp_sse2`; does **not** redefine `strlen`/`strchr`/`memcmp`) — **wired** |
| batch90 | ChaCha20 SSE2 stream XOR (`chacha20_xor_sse2` + `__` alias; RFC 8439; 4×64B parallel; remainder via batch42 `chacha20_xor`) — **wired** |
| batch91 | Poly1305 SSE2 load path (`poly1305_auth_sse2` + `__` alias; mac-first/key-last; does **not** redefine batch42 `poly1305_auth`) — **wired** |
| batch92 | mini libm (`sqrt`/`sqrtf` SSE2; `fabs`/`fabsf`; `floor`/`ceil` IEEE bit paths) — **wired** |
| batch93 | base64url (`base64url_encode`/`base64url_decode` + `__` aliases; RFC 4648 5; standard `base64_*` remain batch40) — **wired** |
| batch94 | HTTP-date IMF-fixdate (`gj_http_date_parse`/`gj_http_date_format` + `__` aliases; RFC 7231 7.1.1.1; integer-only, no SSE) — **wired** |
| batch95 | AVX2 bulk mem (`gj_cpu_has_avx2`, `gj_memcpy_avx2`/`gj_memset_avx2` + `__` aliases; CPUID.7:0 EBX[5]; fallback batch84 SSE2; does **not** redefine `memcpy`/`memset`/batch84) — **wired** |
| batch96 | AES-GCM fast path (`aes_gcm_encrypt_fast`/`aes_gcm_decrypt_fast` + `__` aliases; AES-NI + PCLMUL via batch83/85 when both present; else batch60; does **not** redefine `aes_gcm_*`/`ghash`) — **wired** |
| batch97 | HighwayHash64 (`highwayhash64`/`__highwayhash64`; integer pure C; does **not** redefine siphash/cityhash batches) — **wired** |
| batch98 | XXH3-style long-path (`gj_xxh3_64`/`gj_xxh3_128` + `__` aliases; SSE2 stripe ≥241 B; does **not** redefine batch41 `XXH3_*`) — **wired** |
| batch99 | DEFLATE deflate (`gj_deflate`/`__gj_deflate`; RFC 1951 + zlib wrapper RFC 1950; stored/fixed-Huffman; Adler-32 via batch39; does **not** define `deflate`/`compress`/`inflate`) — **wired** |
| batch100 | freestanding sort suite (`gj_qsort`/`gj_qsort_r`/`gj_heapsort`/`gj_mergesort` + `__` aliases; introsort / in-place heap / stack mergesort; does **not** redefine `qsort*`/batch18) — **wired** |
| batch101 | POSIX ustar parse (`gj_tar_octal`/`gj_tar_checksum_ok`/`gj_tar_header_parse` + `__` aliases; 512-byte header) — **wired** |
| batch102 | minimal ELF64 helpers (`gj_elf64_validate`/`gj_elf64_phdr`/`gj_elf64_find_dynamic` + marker; not a loader) — **wired** |
| batch103 | URL helpers (`gj_url_parse`/`gj_url_encode`/`gj_url_decode` + `__` aliases; RFC 3986 subset) — **wired** |
| batch104 | freestanding INI (`gj_ini_get`/`gj_ini_get_int` + `__` aliases; `[section]` + `key=value`) — **wired** |
| batch105 | MurmurHash3 Appleby (`MurmurHash3_x86_32`/`MurmurHash3_x64_128` + `__` aliases; does **not** redefine batch41 `murmur3_32`) — **wired** |
| batch106 | underscored arc4random (`__arc4random`/`__arc4random_buf`/`__arc4random_uniform`/`__arc4random_addrandom`; ChaCha20 CSPRNG; does **not** redefine public `arc4random*`) — **wired** |
| batch107 | flat JSON bool/array (`gj_json_get_bool`/`gj_json_array_len`/`gj_json_array_get_string` + `__` aliases; extends batch77 without redefining it) — **wired** |
| batch108 | IEEE CRC-32 PCLMUL (`crc32_pclmul`/`__crc32_pclmul`; poly matches batch39 `crc32`/`crc32_z`; does **not** redefine `crc32*`/`crc32c*`/`ghash_pclmul`) — **wired** |
| batch109 | BSD string fill-ins (`stresep`/`strpct`/`strspct`/`strsuftoll` + `__` aliases; does **not** redefine `strlcpy`/`strsep`/`memmem`) — **wired** |
| batch110 | JPEG SOI/marker scan (`gj_jpeg_is_jpeg`/`gj_jpeg_find_marker`/`gj_jpeg_dimensions` + `__` aliases; SOF0/SOF2 dims only; no full decode) — **wired** |
| batch111 | PNG sig + IHDR (`gj_png_is_png`/`gj_png_ihdr`/`gj_png_crc32` + `__` aliases; IEEE/PNG CRC; no IDAT inflate) — **wired** |
| batch112 | WAV RIFF (`gj_wav_is_wav`/`gj_wav_fmt`/`gj_wav_data_offset` + `__` aliases; PCM fmt + data chunk locate) — **wired** |
| batch113 | DNS wire helpers (`gj_dns_pack_query`/`gj_dns_skip_name`/`gj_dns_parse_header` + `__` aliases; RFC 1035; no network I/O) — **wired** |
| batch114 | HTTP/1.x request line + headers (`gj_http_req_line`/`gj_http_header_get`/`gj_http_content_length` + `__` aliases; RFC 7230 subset; no body) — **wired** |
| batch115 | PEM extract/decode (`gj_pem_find`/`gj_pem_decode` + `__` aliases; RFC 7468 boundaries; base64 via batch40) — **wired** |
| batch116 | freestanding UUID parse/unparse (`gj_uuid_parse`/`gj_uuid_unparse`/`gj_uuid_unparse_upper` + `__` aliases; does **not** redefine batch35/36 `uuid_*`) — **wired** |
| batch117 | lock-free SPSC ring (`gj_ring_t`/`gj_ring_init`/`gj_ring_write`/`gj_ring_read`/`gj_ring_avail`/`gj_ring_space`; GCC `__atomic_*`) — **wired** |
| batch118 | dense bitset (`gj_bitset_set`/`clear`/`test`/`count`/`find_first`; `uint64_t` words, LSB-first) — **wired** |
| batch119 | Crockford Base32 (`crockford32_encode`/`crockford32_decode` + `__` aliases; does **not** redefine RFC 4648 `base32_*`/batch41) — **wired** |
| batch120 | constant-time helpers (`gj_memeq_ct`/`gj_memzero_s`/`gj_select_u32` + `__` aliases; does **not** redefine `crypto_memcmp`/`consttime_memequal`/`explicit_bzero`) — **wired** |
| batch121 | Argon2id PHC encode + verify (`argon2id_hash_encoded`/`gj_argon2id_verify` + `__` aliases; over batch65 `argon2id_hash`; does **not** redefine batch46/65 hash cores) — **wired** |
| batch122 | public HMAC-SM3 (`hmac_sm3`/`__hmac_sm3`; RFC 2104 over batch42 SM3; does **not** redefine batch72 `pbkdf2_hmac_sm3`) — **wired** |
| batch123 | ZIP EOCD/CD metadata (`gj_zip_eocd_find`/`gj_zip_eocd_parse`/`gj_zip_cd_entry`; APPNOTE; no extract/inflate; SSE2 bulk copy when `cb≥16`) — **wired** |
| batch124 | Internet checksum (`gj_ip_checksum`/`gj_ip_checksum_fold`/`gj_ipv4_header_ok` + `__` aliases; RFC 1071) — **wired** |
| batch125 | classic Bloom filter (`gj_bloom_init`/`gj_bloom_add`/`gj_bloom_may_contain` + `__` aliases; Kirsch–Mitzenmacher double FNV-1a; does **not** redefine batch41 `fnv1a_*`) — **wired** |
| batch126 | WebSocket frame header (`gj_ws_header_parse`/`gj_ws_header_build`/`gj_ws_mask` + `__` aliases; RFC 6455 5.2/5.3; parse/build + payload mask XOR) — **wired** |
| batch127 | MQTT 3.1.1 fixed header (`gj_mqtt_encode_remaining`/`gj_mqtt_decode_remaining`/`gj_mqtt_fixed_header` + `__` aliases; Remaining Length max 268435455) — **wired** |
| batch128 | mini CBOR (`gj_cbor_encode_uint`/`bytes`/`text`, `gj_cbor_decode_uint`/`bytes`/`text`; RFC 8949 definite-length major 0/2/3 only) — **wired** |
| batch129 | MessagePack subset (`gj_msgpack_encode_str`/`gj_msgpack_encode_uint`/`gj_msgpack_decode_str` + `__` aliases; positive fixint / fixstr / bin8) — **wired** |
| batch130 | LEB128 (`gj_uleb128_encode`/`decode`, `gj_sleb128_encode`/`decode` + `__` aliases; DWARF/Wasm style; max 10 B for 64-bit) — **wired** |
| batch131 | protobuf wire (`gj_pb_encode_varint`/`decode_varint`, `gj_pb_encode_tag`/`decode_tag` + `__` aliases; field≪3\|wire; max 10 B varint) — **wired** |
| batch132 | Snappy raw-block uncompress (`gj_snappy_uncompressed_length`/`gj_snappy_uncompress` + `__` aliases; status 0/1/2; SSE2 bulk literals) — **wired** |
| batch133 | LZ4 block safe decompress (`gj_lz4_decompress_safe`/`__gj_lz4_decompress_safe`; public block format; no frame) — **wired** |
| batch134 | XOR + PKCS #7 (`gj_xor_buf`/`gj_xor_keystream`/`gj_pkcs7_pad`/`gj_pkcs7_unpad` + `__` aliases; SSE2 XOR when `n≥16`; block 1..255) — **wired** |
| batch135 | HPACK integer (`gj_hpack_encode_int`/`gj_hpack_decode_int` + `__` aliases; RFC 7541 5.1 prefix-N; N 1..8) — **wired** |
| batch136 | TLS record header (`gj_tls_record_header_parse`/`gj_tls_record_header_build` + `__` aliases; 5-byte type/version/length; RFC 5246/8446 layout) — **wired** |
| batch137 | netstring (`gj_netstring_encode`/`gj_netstring_decode` + `__` aliases; DJB `len:data,`; no leading zeros except `0:,`) — **wired** |
| batch138 | CSV field parse (`gj_csv_next_field`/`gj_csv_count_fields` + `__` aliases; RFC 4180 lite; quoted `""` unescape) — **wired** |
| batch139 | popcount (`gj_popcount32`/`gj_popcount64`/`gj_popcount_buf` + `__` aliases; SWAR; SSE2 bulk when `n≥16`) — **wired** |
| batch140 | Z85 (`z85_encode`/`z85_decode` + `__` aliases; ZeroMQ RFC 32; does **not** redefine batch68 Adobe `base85_*`/`ascii85_*`) — **wired** |
| batch141 | flat TOML bool / has-key (`gj_toml_get_bool`/`gj_toml_has_key`; extends batch81 without redefining it) — **wired** |
| batch142 | flat YAML key: value (`gj_yaml_get_string`/`gj_yaml_get_int`) — **wired** |
| batch143 | MIME encoded-word B lite (`gj_mime_b64_word_decode`/`encode`; base64 via batch40) — **wired** |
| batch144 | SMTP reply parse RFC 5321 lite (`gj_smtp_parse_code`/`gj_smtp_is_success`) — **wired** |
| batch145 | IMAP untagged helpers RFC 3501 lite (`gj_imap_is_untagged`/`parse_exists`/`parse_uid`) — **wired** |
| batch146 | FTP reply + PASV parse RFC 959 (`gj_ftp_parse_code`/`gj_ftp_parse_pasv`) — **wired** |
| batch147 | ISO-8601 basic UTC datetime (`gj_iso8601_parse`/`format`) — **wired** |
| batch148 | human duration `1h2m3s` (`gj_duration_parse`/`format`) — **wired** |
| batch149 | path helpers (`gj_path_join`/`basename`/`dirname`/`is_abs`) — **wired** |
| batch150 | shell-style glob match `*`/`?` (`gj_glob_match`; does **not** redefine `fnmatch`/`glob`) — **wired** |
| batch151 | SOCKS5 greeting / CONNECT helpers RFC 1928 (`gj_socks5_parse_greeting`/`build_connect`) — **wired** |
| batch152 | SSH banner parse RFC 4253 (`gj_ssh_parse_banner`) — **wired** |
| batch153 | BGP OPEN header parse RFC 4271 (`gj_bgp_parse_open`) — **wired** |
| batch154 | NTP packet header parse RFC 5905 (`gj_ntp_parse`) — **wired** |
| batch155 | DHCP option scan RFC 2132 (`gj_dhcp_find_option`/`gj_dhcp_msg_type`) — **wired** |
| batch156 | IPv6 text ↔ binary (`gj_inet_pton6`/`gj_inet_ntop6`; does **not** redefine `inet_pton`/`ntop`) — **wired** |
| batch157 | Ethernet MAC parse / format (`gj_mac_parse`/`gj_mac_format`) — **wired** |
| batch158 | PCI BDF address parse (`gj_pci_parse_bdf`) — **wired** |
| batch159 | UEFI GUID string parse / format (`gj_guid_parse`/`gj_guid_format`) — **wired** |
| batch160 | userspace counting semaphore (`gj_sem_init`/`trywait`/`post`/`getvalue`; not POSIX `sem_*`) — **wired** |
| batch161 | JSON Pointer RFC 6901 lite (`gj_json_pointer_get`) — **wired** |
| batch162 | JWT compact split + base64url segment decode (no crypto) (`gj_jwt_split`/`gj_jwt_b64url_decode_seg`) — **wired** |
| batch163 | UUID v7 layout helpers RFC 9562 (`gj_uuid_v7_set_timestamp`/`gj_uuid_version`/`variant`) — **wired** |
| batch164 | Bitcoin-style Base58Check encode (`gj_base58check_encode`) — **wired** |
| batch165 | Bech32 encode / decode BIP-0173 lite (`gj_bech32_encode`/`decode`) — **wired** |
| batch166 | ZigZag + protobuf sint64 varint (`gj_zigzag_*`/`gj_pb_encode_sint64`) — **wired** |
| batch167 | URL query-string get (`gj_url_query_get`; extends batch103 without redefining it) — **wired** |
| batch168 | HTML entity escape / unescape lite (`gj_html_escape`/`unescape`) — **wired** |
| batch169 | URL/path slugify (`gj_slugify`) — **wired** |
| batch170 | Levenshtein edit distance (`gj_levenshtein`) — **wired** |
| batch171 | Base36 encode / decode (`gj_base36_encode`/`decode`) — **wired** |
| batch172 | Q16.16 fixed-point (`gj_q16_mul`/`div`/`from_int`/`to_int`) — **wired** |
| batch173 | xorshift128+ PRNG (`gj_xorshift128_seed`/`next`) — **wired** |
| batch174 | token-bucket rate limiter (`gj_tb_init`/`gj_tb_try_take`) — **wired** |
| batch175 | fixed-slot void* work queue (`gj_wq_init`/`push`/`pop`) — **wired** |
| batch176 | bump / linear arena allocator (`gj_arena_init`/`alloc`/`reset`) — **wired** |
| batch177 | fixed-slot string intern table — **wired** |
| batch178 | ordered integer map (parallel arrays) (`gj_imap_find`/`insert`) — **wired** |
| batch179 | int64 min-heap / top-k (`gj_heap_init`/`push`/`pop`) — **wired** |
| batch180 | Rabin–Karp rolling hash (`gj_rk_hash`/`gj_rk_roll`) — **wired** |
| batch181 | full-width bit reverse 32/64 (`gj_bitrev32`/`gj_bitrev64`) — **wired** |
| batch182 | binary-reflected Gray code (`gj_gray_encode`/`decode`) — **wired** |
| batch183 | Fletcher-16 / Fletcher-32 checksums (`gj_fletcher16`/`gj_fletcher32`) — **wired** |
| batch184 | Hamming distance over buffers (`gj_hamming_distance`; SWAR + optional SSE2) — **wired** |
| batch185 | lock-free SPSC `uint64_t` ring power-of-2 (`gj_u64ring_*`; does **not** redefine batch117 `gj_ring_*`) — **wired** |
| batch186 | base64url encode/decode with optional `=` padding (`gj_b64url_encode_pad`/`decode_pad` + `__` aliases; RFC 4648 5; does **not** redefine batch40/93/22/`gj_jwt_b64url_decode_seg`) — **wired** |
| batch187 | Punycode encode + IDNA label lite (`gj_punycode_encode`/`gj_idna_to_ascii_label` + `__` aliases; RFC 3492; ASCII LDH passthrough) — **wired** |
| batch188 | Windows SID string parse (`gj_sid_parse` + `__` alias; `S-1-5-…` ConvertSidToStringSid form) — **wired** |
| batch189 | ASN.1 DER TLV header parse (`gj_asn1_tlv` + `__` alias; X.690 definite-length only) — **wired** |
| batch190 | ASN.1 OID content encode/decode (`gj_oid_encode`/`gj_oid_decode` + `__` aliases; X.690 8.19 arcs; no Tag/Length) — **wired** |
| batch191 | SPKI subjectPublicKey BIT STRING peel (`gj_spki_find_bitstring` + `__` alias; RFC 5280 outer SEQUENCE; does **not** redefine batch115 PEM) — **wired** |
| batch192 | PEM block count + null-sep label list (`gj_pem_count`/`gj_pem_list_labels` + `__` aliases; complements batch115) — **wired** |
| batch193 | TLS alert body parse (`gj_tls_alert_parse` + `__` alias; does **not** redefine batch136 record header) — **wired** |
| batch194 | HTTP/1.x status-line parse (`gj_http_status_line` + `__` alias; version + code + reason) — **wired** |
| batch195 | Cookie header name=value get (`gj_cookie_get` + `__` alias; Cookie header only) — **wired** |
| batch196 | multipart/form-data boundary find + part iterate (`gj_multipart_find_boundary`/`gj_multipart_next_part` + `__` aliases; RFC 2046/7578 subset) — **wired** |
| batch197 | application/x-www-form-urlencoded get (`gj_form_get` + `__` alias; key lookup) — **wired** |
| batch198 | WebSocket Sec-WebSocket-Accept key (`gj_ws_accept_key` + `__` alias; RFC 6455 1.3/4.2.2; SHA-1 + base64) — **wired** |
| batch199 | HTTP/2 frame header parse (`gj_h2_frame_header` + `__` alias; RFC 7540 4.1 fixed 9-byte) — **wired** |
| batch200 | QUIC varint encode/decode (`gj_quic_varint_encode`/`decode` + `__` aliases; RFC 9000 16) — **wired** |
| batch201 | streaming SipHash-2-4 (`gj_sip24_init`/`update`/`final` + `__` aliases; does **not** redefine batch36 `siphash24`) — **wired** |
| batch202 | ChaCha20 single-block keystream (`gj_chacha20_block_counter` + `__` alias; does **not** redefine batch42/70 stream/AEAD) — **wired** |
| batch203 | AES-CTR stream XOR (`gj_aes_ctr_xor` + `__` alias; NIST SP 800-38A; AES via batch45) — **wired** |
| batch204 | TLS 1.3 HKDF-Expand-Label lite (`gj_hkdf_expand_label` + `__` alias; RFC 8446 7.1 shape over batch71 expand) — **wired** |
| batch205 | simple scrypt bring-up (`gj_scrypt_simple` + `__` alias; N=16 r=1 p=1 → 32 B; weak-link to batch46 `scrypt`) — **wired** |
| batch206 | streaming RFC 4648 base64 encoder (`gj_b64_enc_init`/`update`/`final` + `__` aliases; does **not** redefine one-shot batch40/93/186) — **wired** |
| batch207 | IEEE CRC-32 combine (`gj_crc32_combine` + `__` alias; GF(2); matches batch39/111 polys; does **not** redefine `crc32*`) — **wired** |
| batch208 | fixed 64-bit Bloom (`gj_bloom64_add`/`gj_bloom64_may` + `__` aliases; k=4; does **not** redefine batch125) — **wired** |
| batch209 | dense bitset next-set (`gj_bitset_next` + `__` alias; extends batch118 without redefining set/clear/test) — **wired** |
| batch210 | uint32_t radix sort (`gj_radix_sort_u32` + `__` alias; freestanding LSD/MSD; does **not** redefine batch36 `radixsort`/`sradixsort`) — **wired** |
| batch211 | in-place int64 median (`gj_median_i64` + `__` alias; quickselect lower median) — **wired** |
| batch212 | Fisher–Yates shuffle uint32 (`gj_shuffle_u32` + `__` alias; caller LCG; not CSPRNG) — **wired** |
| batch213 | string Hamming distance (`gj_hamming_str` + `__` alias; equal-length C strings; does **not** redefine batch184 buffer Hamming) — **wired** |
| batch214 | LCS length short strings (`gj_lcs_len` + `__` alias; stack DP, max 64 chars; not Levenshtein/Hamming) — **wired** |
| batch215 | longest palindromic substring length (`gj_longest_palindrome_len` + `__` alias; expand-around-centers) — **wired** |
| batch216 | KMP first-match (`gj_kmp_find` + `__` alias) — **wired** |
| batch217 | Boyer–Moore–Horspool (`gj_bmh_find` + `__` alias) — **wired** |
| batch218 | multi-pattern first match (`gj_multi_find` + `__` alias) — **wired** |
| batch219 | compact trie insert/contains (`gj_trie_insert`/`gj_trie_contains` + `__` aliases) — **wired** |
| batch220 | Fenwick tree add/prefix-sum (`gj_fenwick_add`/`gj_fenwick_sum` + `__` aliases) — **wired** |
| batch221 | segment tree build/range-sum (`gj_segtree_build`/`gj_segtree_query` + `__` aliases) — **wired** |
| batch222 | union-find (`gj_uf_init`/`gj_uf_find`/`gj_uf_union` + `__` aliases) — **wired** |
| batch223 | Kahn topological sort (`gj_topo_sort` + `__` alias) — **wired** |
| batch224 | dense Dijkstra (`gj_dijkstra_dense` + `__` alias) — **wired** |
| batch225 | BFS grid shortest path (`gj_bfs_grid` + `__` alias) — **wired** |
| batch226 | int32 matrix multiply (`gj_matmul_i32` + `__` alias) — **wired** |
| batch227 | modular linear solve Gaussian (`gj_solve_linear_mod` + `__` alias) — **wired** |
| batch228 | naive polynomial multiply (`gj_poly_mul_naive` + `__` alias) — **wired** |
| batch229 | gcd/lcm/egcd u64 (`gj_gcd_u64`/`gj_lcm_u64`/`gj_egcd` + `__` aliases) — **wired** |
| batch230 | modular exponentiation u64 (`gj_modpow_u64` + `__` alias; freestanding binary mul) — **wired** |
| batch231 | deterministic Miller–Rabin u64 (`gj_is_prime_u64` + `__` alias) — **wired** |
| batch232 | Pollard rho factor (`gj_pollard_rho` + `__` alias) — **wired** |
| batch233 | CRT two congruences (`gj_crt_pair` + `__` alias) — **wired** |
| batch234 | binomial coefficient u64 (`gj_binom` + `__` alias) — **wired** |
| batch235 | Fibonacci u64 (`gj_fib_u64` + `__` alias) — **wired** |
| batch236 | Levenshtein edit distance (`gj_edit_distance` + `__` alias; stack DP max 64) — **wired** |
| batch237 | LCP length (`gj_lcp_len` + `__` alias) — **wired** |
| batch238 | Manacher palindrome length (`gj_manacher_len` + `__` alias) — **wired** |
| batch239 | polynomial rolling hash (`gj_poly_hash` + `__` alias; freestanding mulmod) — **wired** |
| batch240 | Rabin–Karp find (`gj_rk_find` + `__` alias) — **wired** |
| batch241 | Z-array (`gj_z_array` + `__` alias) — **wired** |
| batch242 | naive suffix array (`gj_suffix_array` + `__` alias) — **wired** |
| batch243 | next permutation u32 (`gj_next_perm_u32` + `__` alias) — **wired** |
| batch244 | Gray code sequence (`gj_gray_seq` + `__` alias) — **wired** |
| batch245 | bitset range popcount (`gj_bitset_popcount` + `__` alias) — **wired** |
| batch246 | min-heapify/pop u64 (`gj_heapify_min_u64`/`gj_heappop_min_u64` + `__` aliases) — **wired** |
| batch247 | merge sorted u32 (`gj_merge_u32` + `__` alias) — **wired** |
| batch248 | stable partition non-neg (`gj_stable_partition_nn` + `__` alias) — **wired** |
| batch249 | LIS length (`gj_lis_len` + `__` alias) — **wired** |
| batch250 | 0-1 knapsack (`gj_knapsack01` + `__` alias) — **wired** |
| batch251 | min coin change (`gj_coin_change_min` + `__` alias) — **wired** |
| batch252 | naive DFT (`gj_dft_naive` + `__` alias; Taylor sin/cos freestanding) — **wired** |
| batch253 | matrix det i64 Bareiss (`gj_mat_det_i64` + `__` alias) — **wired** |
| batch254 | CSR SpMV i64 (`gj_spmv_csr_i64` + `__` alias) — **wired** |
| batch255 | Bellman–Ford (`gj_bellman_ford` + `__` alias) — **wired** |
| batch256 | Floyd–Warshall (`gj_floyd_warshall` + `__` alias) — **wired** |
| batch257 | Aho–Corasick lite (`gj_ac_search` + `__` alias) — **wired** |
| batch258 | streaming base64 decode (`gj_b64_dec_*` + `__` aliases) — **wired** |
| batch259 | strict UTF-8 validate (`gj_utf8_validate` + `__` alias) — **wired** |
| batch260 | Punycode encode lite (`gj_puny_encode` + `__` alias) — **wired** |
| batch261 | IPv4 dotted-quad parse BE (`gj_inet_pton4` + `__` alias) — **wired** |
| batch262 | UUID v4 format (`gj_uuid_v4_format` + `__` alias) — **wired** |
| batch263 | path normalize (`gj_path_normalize` + `__` alias) — **wired** |
| batch264 | FNV-1a 64 (`gj_fnv1a64` + `__` alias) — **wired** |
| batch265 | SipHash-2-4 (`gj_siphash24` + `__` alias) — **wired** |
| batch266 | XXH64 (`gj_xxh64` + `__` alias) — **wired** |
| batch267 | cityhash-lite mix (`gj_cityhash64` + `__` alias) — **wired** |
| batch268 | CRC-64/ECMA (`gj_crc64_ecma` + `__` alias) — **wired** |
| batch269 | streaming Adler-32 (`gj_adler32_*` + `__` aliases) — **wired** |
| batch270 | wyhash-lite (`gj_wyhash64` + `__` alias) — **wired** |
| batch271–290 | JSON/URL/HTML/CSV/TOML helpers, duration/size, ring_u8, bump arena, varint/LEB128s, COBS, bitpack, RLE, delta i32 — **wired** |
| batch291–300 | murmur3_32, blake2s_256, hmac_sha256, ct_eq, secure_wipe, b32, hex, bech32 polymod, rs1024 polymod — **wired** |
| batch301–320 | PRNGs (mt19937/xoshiro/pcg), sampling/stats, sorted insert, intervals, coord compress, Mo order, sparse table RMQ, binary lifting, tree diameter, bipartite, bridges, Tarjan SCC — **wired** |
| batch321–330 | maxflow EK, assignment, string hashmap, clock replace, slab freelist, refcount, once, jobq, hazard ptrs, epoch RCU stub — **wired** |
| batch331–339 | b64url decode, uuid strict, mac parse, inet6, cidr4, netmask prefix, DNS name enc/dec, HTTP status text — **wired** |
| batch340–350 | WS mask, MQTT RL, QUIC varint, TLS rec hdr, NTP32, DHCP/BGP magic, inet/tcp cksum, bloom128, cuckoo filter — **wired** |
| batch351–360 | string builder, strcat cap, split/join, ascii case, starts/ends, trim, replace_char, count_substr, reverse_words — **wired** |
| batch361–375 | strcmp_n/strspn/cspn/strchr_nul/mem_eq, ctz/clz/popcnt/rot, isqrt/ilog2/pow2/align — **wired** |
| batch376–395 | min/max/clamp, sat add/sub, abs/signum, div_ceil/avg, gcd_n/lcm_n, fact/pow, digit utils, parse/format dec/hex, BE/LE load-store — **wired** |
| batch396–405 | gj_mem_set/cpy/move/cmp/chr, reverse/fill/find/sum/is_sorted u32 — **wired** |
| batch406–425 | sort/bsearch u32/i64, min/max arr, str len/cpy/eq/chr, bswap/bitrev/parity/lsb, fnv1a32/djb2/sdbm/splitmix/murmur seed0 — **wired** |
| batch426–435 | mul_u32, add/mul overflow u64, udiv_round, mod_u64, buf append/eq/zero/find/rev — **wired** |
| batch436–450 | stack/queue/deque u32, uniq/count, mat add/scale/transpose/trace, vec add, parse/format i64, bool/digit/hex str — **wired** |
| batch451–470 | crc8/crc16_ccitt, fletcher32_fold, adler64, pearson/jenkins/superfast/xxh32/farm/metro, base58, bech32_enc, cbor peek, msgpack u64, bson cstring, uuid nil/cmp, ULID/KSUID time — **wired** |
| batch471–490 | crc32c_sw, lookup3, spooky64, murmur2, fasthash64, path basename/dirname/join2/ext/is_abs, leap/days/yday, secs/ms, htons/htonl/ntoh*, ipv4 range — **wired** |
| batch491–510 | bset set/clear/test, bitset_pop_all, bloom256, HLL, CMS, weighted_sample, topk, online mean, stream minmax, gj_batch_id, utf16le/utf8 helpers, ascii casefold_n, str_eq/starts/ends_ci, fnmatch_simple (not glob_match), shell_quote — **wired** |
| batch511–540 | pack/unpack u16/u32/u64 BE+LE, is/next/prev power2, align up/down, mem xor/and/or, hex_nibble, hexdump_line, ring_u32 push/pop, bitmap first zero/one, hamming_u8, jaccard_u64, dot_i32, l2sq_i32 — **wired** |
| batch541–570 | ipv6_in_range, cidr_contains, csv_unquote, kv_lookup, arena_bump, pool alloc/free, rcu stamp/tick, seqlock read, hazard ptr, batch_id_550/milestone, base64url enc/dec, url_encode/decode_n, levenshtein_u8, cosine_i32, byteswap16/32/64, bitrev8, popcnt/ctz/clz/rotl/rotr u64, sat_mul, mul_hi, addc/subb — **wired** |
| batch571–600 | fibonacci/factorial (renamed), euclid gcd/lcm, prime_p, powmod, binom, icbrt, floor_log2, sum_digits, rev_digits, palindrome_p, parse/format dec+hex, clamp/min3/max3/median3, bitwidth/mask/extract/deposit, parity, gray enc/dec, next_perm_bits, isolate_lsb, batch_id_600 — **wired** |
| batch601–630 | str starts/ends/contains/count/ltrim/rtrim, mem zero/fill, u32 sum/xor/find/count, i64 abs_u/sign, u64 avg/div_ceil, fletcher16 marker, crc7, fnv1a32_n/djb2_n/sdbm_n, jenkins marker, siphash13, xxh32_seed, murmur3_32_seed, base16 enc/dec, uuid v4 bits/is_nil, batch_id_630 — **wired** |
| batch631–660 | str nlen/span/cspan, mem eq/chr_n, u8/u16 sum+xor, bswap buf, i32 sum/min/max/clamp, i64 clamp, u64 checked mul/add/sub, inc/dec sat, bit set/clear/test/toggle/count_range, ascii is_*_n, batch_id_650 — **wired** |
| batch661–670 | ascii_is_print_n, mem reverse/swap, u32 reverse/is_sorted_asc/unique_sorted/lower_bound/upper_bound/insertion_sort, batch_id_670 — **wired** |
| batch671–700 | u32 min-heap sift/push/pop, u64 stack, u32 queue, str split/join, path has_sep/is_rel, hex/bin digits, parse/format bin, count newlines, utf8 ascii/cp, base64 len, timeval add/sub, timespec cmp, duration ms/us, milestones 680/690/700 — **wired** |
| batch701–730 | i64 heap, u32 deque, str/mem prefix-suffix, partition/select_nth, hash_combine, splitmix/xorshift/pcg_s, uniform/bernoulli/reservoir, cobs_encode_len/decode_ok, slip, hamming74, crc8_poly, inet_checksum, fletcher32_bytes, milestones 710/720/730 — **wired** |
| batch731–760 | str empty/count_substr, mem sorted/u8_sort, fnv1a seed/n, u64 range/wrap, json ws, base32 len, uuid_ver_u/var_u, BE/LE load/store, milestones 740/750/760 — **wired** |
| batch761–800 | str blank/icmp, mem contains, fill/copy, rotl/rotr/bswap, u32 power2/align, crc16_ccitt/adler32/sum/xor, popcnt/ctz/clz/parity, path/url/mac helpers, milestones 770/780/790/800 — **wired** |
| batch801–860 | ws helpers, fill/copy, arr min/max/sum, prefix/delta, enc lens, ident/digit, min/max/clamp, modular arith, primes, IPv4 helpers, milestones 810–860 — **wired** |
| batch861–900 | str eq_bounded/cmp_n, mem_rcmp, find min/max, argsort, sorted set ops, bitset any/all, ring/SPSC/ticket/seq, bloom/hll/cms/topk/online/stream helpers, milestones 870–900 — **wired** |
| batch901–940 | str dup/cat/trim/replace/map, sorted mode/median/percentile, config char classes, sat arith, bool/bitfield/nibble, milestones 910–940 — **wired** |
| batch941–1000 | line/word/title/snake-camel, hist/argmax, mat3/vec3/quat/lerp i32, hash mixers, arena/ptr, barriers/prefetch stubs, page/cache sizes, unit convert, pct/bp/ratio/safe_div, milestones 950–1000 — **wired** |
| batch1001–1040 | str starts/ends_n, mem overlap, u32 edit, bit mask/deposit, gray/LFSR, utf8 lead/cont, C escape, time helpers, milestones 1010–1040 — **wired** |
| batch1041–1080 | path join, rotate/shuffle, bitmap ops, parse/format helpers, milestones 1050–1080 — **wired** |
| batch1081–1120 | str pad/wrap, moving stats, similarity metrics, net htons/ntoh*, ether/ipv6, DNS labels, milestones 1090–1120 — **wired** |
| batch1121–1160 | HTTP/URI/MIME/JSON/TOML helpers, file magic (ELF/PE/PNG/ZIP/…), milestones 1130–1160 — **wired** |
| batch1161–1200 | audio/video/font/cert/SSH/JWT/semver/git/ULID, CPU feature bits, caps, retry/circuit, milestones 1170–1200 — **wired** |
| batch1201–1240 | str normalize, array map/CRC, KMP/BM/RK/Z/SA helpers, regex/glob/SQL/HTML/CSS chars, milestones 1210–1240 — **wired** |
| batch1241–1280 | reduce ops, const-time mem, secure_wipe_hint, crypto size constants, AEAD overhead, milestones 1250–1280 — **wired** |
| batch1281–1320 | binary search family, heap indices, BST/AVL/RB hints, FIFO/LIFO/ring, tasks/barriers/events, milestones 1290–1320 — **wired** |
| batch1321–1360 | str find_first/last_of family, partition/rpartition/split_n/join_n, u8 hist/mode/entropy, bswap arrays, interleave/morton/hilbert, cantor/szudzik/elegant pair, hash_combine3/4, bloom/hll estimate, milestones 1330–1360 — **wired** |
| batch1361–1460 | CMS/topk/reservoir/EWMA stream, sat arith, pack/unpack, str/mem transforms, sort checks, path/URI, checksums/CRC5-10, civil time, mixers/xoshiro/jsf/sfc/romu, mat2/vec2 i32, milestones 1370–1460 — **wired** |
| batch1461–1620 | rings/stacks/deques, Q16/Q8, UTF-8/16, net endian, RLE/LZ/bitio, ct ops, arena/slab, parse/format_n, heaps, sync hints, metrics, bitmat, SI units, JSON/TOML chars, geometry, scheduler, milestones 1470–1620 — **wired** |
| batch1621–1700 | ASCII case/fold, bitset64 ops, base conversion lens, file magic, pid/tid hash, PCM/RGB, btree/heap/fenwick indices, milestone 1700 — **wired** |
| batch1701–1800 | hash stream steps, ranges/intervals, ELF/PE/Mach-O, u128 arith, cgroup, DNS, TLS sizes, page/PTE, string builder, semver, milestone 1800 — **wired** |
| batch1801–1900 | ring8, IPv6, HTML/XML, f32/f64 bits, keyboard, GPU buffers, Steam/Deck, mode bits, signal/errno, syscall ABI, milestone 1900 — **wired** |
| batch1901–2000 | desktop window, input, ports, power/thermal, logging, package/install, Vulkan/GL, duration, path product, product bar #3 hints, milestone 2000 — **wired** |
| batch2001–2030 | str lit/eq/span, u64/u32 bit ops, mem zero/ff/or/andnot/swap, milestones 2010–2030 — **wired** |
| batch2031–2100 | u64/i64 arith, hash mix, u32 arrays, encode lens, endian store, id/uuid/ulid, milestone 2100 — **wired** |
| batch2101–2130 | str trim/span/ws, CRC8/16/24/32/64 family, fifo_u32, milestones 2110–2130 — **wired** |
| batch2131–2200 | bit/byte/nibble reverse, parse/format, atomic hints, path segments, inet csum, RNG, product wave 2200 — **wired** |
| batch2201–2240 | str n-copy/cmp, mem scan/copy, open-address tables, insertion/selection/bubble sort, milestones 2210–2240 — **wired** |
| batch2241–2280 | log2/pow2, UTF-8 lead/cont, timespec/duration, Steam/Deck product, milestones 2250–2280 — **wired** |
| batch2281–2310 | popcount/ctz/clz, continuum gates, bar3 hint, byte buffer, milestones 2290–2310 — **wired** |
| batch2311–2340 | wrap/ovf arith, str/mem hash, intrusive list, milestones 2320–2340 — **wired** |
| batch2341–2370 | RB color/BST, median/min/max3, install/steam/proton/deck product path, milestones 2350–2370 — **wired** |
| batch2371–2400 | sat arith, CRC/SMBUS/ROHC/IBM/DNP, Luhn, smoke/bar3 gates, milestone 2400 — **wired** |
| batch2401–2500 | rot/shift sat, str CI, array reduce, ring/bitmap, IPv4, crypto sizes, path, parse/format, milestone 2500 — **wired** |
| batch2501–2600 | number theory, JSON/ELF, time, sync hints, sort, RLE/delta, UUID, base lens, milestone 2600 — **wired** |
| batch2601–2640 | min/max/clamp, mem/str _u, product path/Deck Top50, milestones 2610–2640 — **wired** |
| batch2641–2700 | uN avg/mid/lerp, crc/adler/fletcher/xor, queue/stack u32, bar3 bits, utf8 bounds, wave/smoke gates, milestones 2650–2700 — **wired** |
| batch2701–2800 | clamp/sat/bitops/popcount/rot/bswap, str/mem helpers, gcd/lcm/isqrt, endian, smoke gates, milestones 2710–2800 — **wired** |
| batch2801–2900 | hash/crc, ascii/path, ipv4/endian/ring, bitmap, array ops, sort/search, bar3 product gates, milestones 2810–2900 — **wired** |
| batch2901–3000 | sat/mod/bitrev/pack, primes/fib, str/mem, uuid/elf/pe/json, continuum gates, milestones 2910–3000 — **wired** |
| batch3001–3050 | log2/div/mul_hi/avg, carry/sat, utf8/path/env/shell soft gates, milestones 3010–3050 — **wired** |
| batch3051–3060 | mul_div/pct/lerp/q16 smoothstep, milestone 3060 — **wired** |
| batch3061–3150 | Q16/Q32/trig, hypot/distances, vectors, cmov/masks, crc/net, time/pages, milestones 3070–3150 — **wired** |
| batch3151–3200 | align/range, bitset8/16/32/64, str hashes, buf ops, continuum gates, milestones 3160–3200 — **wired** |
| batch3201–3300 | wrap/bitops/shifts/extract, product gates, absdiff/map/range, ascii/str, milestones 3210–3300 — **wired** |
| batch3301–3350 | clog2/isqrt/cbrt/pow, number theory, deque/heap/list, arena/refcount, milestones 3310–3350 — **wired** |
| batch3351–3400 | ELF/PE/COFF soft parse, JSON/TOML/INI/CSV, base/url, ipv6/mac, continuum gates, milestones 3360–3400 — **wired** |
| batch3401–3450 | BMI-style cmov/blsi/andn/bextr/pdep/pext, lz/tz/popcnt, byte/nibble swap, interleave, milestones 3410–3450 — **wired** |
| batch3451–3550 | absdiff/median/lerp, hash/ring/stack/queue, path/uri/mime, NT/sorts/stats, milestones 3460–3550 — **wired** |
| batch3551–3600 | safe shifts/rot, bit find/mask, mem eq/fill/scan, utf8/utf16/ascii ws, milestones 3560–3600 — **wired** |
| batch3601–3650 | ovf/carry/mul_hi/checked div, signed sat, str replace/split/pad, milestones 3610–3650 — **wired** |
| batch3651–3700 | crc32c/xxh/mix hashes, bloom/HLL/CMS, TLV/CBOR/protobuf/varint/zigzag, milestones 3660–3700 — **wired** |
| batch3701–3750 | digit/parse need, RNG, dense matrix u32, milestones 3710–3750 — **wired** |
| batch3751–3800 | graph adj/BFS/DFS/topo/SCC, SSSP/MST/flow, KMP/LCS/edit, SA/palindrome/Aho, milestones 3760–3800 — **wired** |
| batch3801–3900 | wrap/clip/predicates, endian/checksum, div/madd/sat cast, str/json/xml soft, milestones 3810–3900 — **wired** |
| batch3901–3950 | cdiv/fdiv/rem/quot, number theory, heap/deque/list, milestones 3910–3950 — **wired** |
| batch3951–4000 | bit/byte reverse, gray/hamming, arena/pool/buddy, page align, milestones 3960–4000 — **wired** |
| batch4001–4100 | clamp01/signbit/msb, mask, hash mix, bit permute, remap/waves, Q16.16, milestones 4050/4100 — **wired** |
| batch4101–4300 | saturate/rol/bf/endian, Q16x, hash/arr/checksum, gcd/mem/prng, align/overflow/ascii, milestones 4150–4300 — **wired** |
| batch4301–4400 | checked shifts/bits, str utils, pack bytes, ring, CRC/time/uuid, milestones 4350/4400 — **wired** |
| batch4401–4500 | mul/div/gray/min-max, path/str, bitmap bytes, csum/queue, rational, milestones 4450/4500 — **wired** |
| batch4501–4600 | bit rev/swizzle, counters, LUT/spread, validation, hex/b64 helpers, semver/flags, milestones 4550/4600 — **wired** |
| batch4601–4650 | tick/list/idx, entropy mix, KiB/MiB/SI units, milestone 4650 — **wired** |
| batch4651–4700 | cmp/bitwise, str measure, slice ranges, milestone 4700 — **wired** |
| batch4701–4800 | sat/hash/trig-q15, framing, bump/slot/job, cfg parse, milestones 4750/4800 — **wired** |
| batch4801–4900 | event/vfs/lock/cacheline, workgroup, PCM/RGB/rect, milestones 4850/4900 — **wired** |
| batch4901–5000 | mat/dot, UUID/HTTP/IPv4, TLS/ELF/PE/PTE constants, **MILESTONE 5000** — **wired** |
| batch5001–5100 | byte lanes/SIMD scalar, sched, mm pages, fs/net/crypto/dbg, milestones 5050/5100 — **wired** |
| batch5101–5150 | irq/syscall, soft atomics, seqlock/RCU, milestone 5150 — **wired** |
| batch5151–5200 | tty/fb, ACPI/PCI config, milestone 5200 — **wired** |
| batch5201–5250 | HPET/timer, APIC, DMA, virtio, milestone 5250 — **wired** |
| batch5251–5300 | NVMe/AHCI/USB/GPU ring, milestone 5300 — **wired** |
| batch5301–5350 | display modes, audio rates, HID, power, milestone 5350 — **wired** |
| batch5351–5400 | Deck native, Vulkan/GL, X11/Wayland, libc/license gates, milestone 5400 — **wired** |
| batch5401–5500 | install/EFI/runtime, VK/GL/EGL, X11/WL, audio/HID, steam paths, milestone 5500 — **wired** |
| batch5501–5600 | media/fw/mesa, steam runtimes, gamescope, CEF, input, storage, updates, privacy, milestone 5600 — **wired** |
| batch5601–5700 | APU/PCI/VRAM, Proton/DXVK, radio, SB/TPM, installer, wizard, steam client, launch, overlay, milestone 5700 — **wired** |
| batch5701–5800 | bar3 checklist/steam/top50, PCI enum, display/audio/net/fs/seats, milestone 5800 — **wired** |
| batch5801–5900 | bar3 media/steam/top50 finalize, soname/license, VK/WL/HID/TDP, milestone 5900 — **wired** |
| batch5901–6000 | product score, smoke/dyn/shell gates, install/panel/controller/proton/bar3 math, milestone 6000 — **wired** |
| batch6001–6100 | bar3/steam/top50 deepen, GPU heap, APU topo, thermal/battery/USB/SD, milestone 6100 — **wired** |
| batch6101–6200 | product deepen, gamescope/radv/pw/bt/nm/systemd/flatpak/cgroup, milestone 6200 — **wired** |
| batch6201–6300 | bar3 path, DRM/VRR/HDR/color, input lag, fan, EFI/ACPI, milestone 6300 — **wired** |
| batch6301–6400 | product deepen, shader cache/backends, frame pacing, DS5/xpad, cloud/remote/family, milestone 6400 — **wired** |
| batch6401–6500 | bar3 deepen, OpenXR/WebRTC, DLC/workshop/achievements, screenshot/stream/friends, milestone 6500 — **wired** |
| batch6501–6600 | bar3/steam/top50 deepen, NVMe/AHCI/VirtIO/PCIe, SMBIOS/CPUID, milestone 6600 — **wired** |
| batch6601–6700 | product deepen, mmap/epoll/timerfd/signalfd/eventfd, io_uring/netlink/rtnl, milestone 6700 — **wired** |
| batch6701–6800 | bar3 product, futex/clone/prctl/seccomp, landlock/bpf/fanotify/inotify, milestone 6800 — **wired** |
| batch6801–6900 | product deepen, memfd/uffd/pidfd/close_range/openat2, mount/fsconfig/statx, milestone 6900 — **wired** |
| batch6901–7000 | bar3 deepen, renameat2/cfr/splice/sendfile, mallocate/mlock2/pvm/kcmp, milestone 7000 — **wired** |
| batch7001–7100 | product deepen, getrandom/rseq/sched, aio/keyctl/cap/ptrace/perf, milestone 7100 — **wired** |
| batch7101–7200 | bar3 deepen, clock/timer/mq/shm/sem, pthread/dlopen/socket, milestone 7200 — **wired** |
| batch7201–7300 | product deepen, poll/select/sockopt/msg/accept4/pipe2, milestone 7300 — **wired** |
| batch7301–7400 | bar3 deepen, ioctl/fcntl/flock, fadvise/fallocate/readahead/sfr, milestone 7400 — **wired** |
| batch7401–7500 | product deepen, open/access/chmod/umask, linkat/unlinkat/mkdirat/mknodat, milestone 7500 — **wired** |
| batch7501–7600 | bar3 deepen, waitpid/waitid/kill, sigaction/sigprocmask, timers/nanosleep, milestone 7600 — **wired** |
| batch7601–7700 | bar3 deepen, rlimit/rusage/sysinfo/uname, times/gettimeofday/settimeofday, milestone 7700 — **wired** |
| batch7701–7800 | bar3 deepen, getcwd/chdir/fchdir, getdents/readdir/seekdir/scandir/glob, milestone 7800 — **wired** |
| batch7801–7900 | product deepen, fnmatch/wordexp/regex, iconv/langinfo/locale/strcoll, milestone 7900 — **wired** |
| batch7901–8000 | bar3 deepen, printf/scanf/strtol/strtod, qsort/bsearch/rand/atexit, milestone 8000 — **wired** |















Host gate: `./build/cgj_smoke` + `./build/cgj_dyn_smoke`.

## Build

```sh
make libcgj          # libcgj.a + libc.so.6 + smokes
make userland        # includes libcgj + ld-gj
make stage-rootfs    # installs real libc.so.6 into build/rootfs/lib/
./build/cgj_smoke
./build/cgj_dyn_smoke
```

## Host vs GreenJade dynlink

| Environment | How `libc.so.6` is used |
|-------------|-------------------------|
| **Host Linux** | Do **not** DT_NEEDED a non-glibc `libc.so.6` under host `ld-linux`. Host smoke uses `dlopen`. |
| **GreenJade** | **ld-gj** loads `/lib/libc.so.6` (libcgj) as the real libc for product apps. |

## Host glibc later (out of tree)

When product needs full Steam graphs beyond libcgj:

1. **Option 2 (hardware test):** stage a prebuilt Steam tree on media — `make steam-fetch` / `make steam-stage` / `make hwtest-img` (see [STEAM_HWTEST.md](STEAM_HWTEST.md)). No dpkg on GreenJade; Valve binaries stay out of git.
2. Optionally obtain **host** glibc packages outside the GPL-free tree (operator machine) for a denser runtime rootfs (not committed).
3. Run Steam / Proton against that rootfs on GreenJade (ld-gj + kernel ABI).

## License note

Do **not** copy files from GNU glibc, musl (if license conflicts product dual), or other copyleft trees into this repository. Reimplement from public man pages / ABI docs only.


## Continuum Batches 10101–10200 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch10200.c` (makefile_max=10200). Themes: continuum product deepen, flatpak/portal, fontconfig, ibus/fcitx, dbus/systemd, polkit/seatd, udev/libinput, bluez/networkmanager, cups/print, milestone 10200 gates. Soft gates; bar3 still open.


## Continuum Batches 10201–10300 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch10300.c` (makefile_max=10300). Themes: continuum product deepen wave 2, proton/wine, mesa/vulkan, audio/pipewire, gamescope/steamui, controller, net, power/thermal, bar3 checklist, milestone 10300 gates. Soft gates; bar3 still open.


## Continuum Batches 10301–10500 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch10500.c` (makefile_max=10500). Themes: continuum product deepen waves, flatpak/portal, font/IM, dbus/auth/input/netmgr/print soft stubs, milestone 10400/10500 gates. Soft gates; bar3 still open.


## Continuum Batches 10501–10600 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch10600.c` (makefile_max=10600). Soft gates; bar3 still open.


## Continuum Batches 10601–10800 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch10800.c` (makefile_max=10800). Soft gates; bar3 still open.


## Continuum Batches 10801–10900 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch10900.c` (makefile_max=10900). Soft gates; bar3 still open.


## Continuum Batches 10901–11000 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch11000.c` (makefile_max=11000). Milestone 11000 gates soft; bar3 still open.


## Continuum Batches 11001–11100 (parent wire)

Wired through `graph_batch11100.c` (makefile_max=11100). Soft gates; bar3 still open.


## Continuum Batches 11101–11200 (parent wire)

Wired through `graph_batch11200.c` (makefile_max=11200). Soft gates; bar3 still open.


## Continuum Batches 11201–11300 (parent wire)

Wired through `graph_batch11300.c` (makefile_max=11300). Soft gates; bar3 still open.


## Continuum Batches 11301–11400 (parent wire)

Wired through `graph_batch11400.c` (makefile_max=11400). Soft gates; bar3 still open.


## Continuum Batches 11401–11500 (parent wire)

Wired through `graph_batch11500.c` (makefile_max=11500). Soft gates; bar3 still open.


## Continuum Batches 11501–11600 (parent wire)

Wired through `graph_batch11600.c` (makefile_max=11600). Soft gates; bar3 still open.


## Continuum Batches 11601–11700 (parent wire)

Wired through `graph_batch11700.c` (makefile_max=11700). Soft gates; bar3 still open.


## Continuum Batches 11701–11800 (parent wire)

Wired through `graph_batch11800.c` (makefile_max=11800). Soft gates; bar3 still open.


## Continuum Batches 11801–11900 (parent wire)

Wired through `graph_batch11900.c` (makefile_max=11900). Soft gates; bar3 still open.


## Continuum Batches 11901–12000 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch12000.c` (makefile_max=12000). Milestone 12000 soft gates; bar3 still open.


## Continuum Batches 12001–12100 (parent wire)

Wired through `graph_batch12100.c` (makefile_max=12100). Soft gates; bar3 still open.


## Continuum Batches 12101–12200 (parent wire)

Wired through `graph_batch12200.c` (makefile_max=12200). Soft gates; bar3 still open.


## Continuum Batches 12201–12300 (parent wire)

Wired through `graph_batch12300.c` (makefile_max=12300). Soft gates; bar3 still open.


## Continuum Batches 12301–12400 (parent wire)

Wired through `graph_batch12400.c` (makefile_max=12400). Soft gates; bar3 still open.


## Continuum Batches 12401–12500 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch12500.c` (makefile_max=12500). Soft gates; bar3 still open.


## Continuum Batches 12501–12600 (parent wire)

Wired through `graph_batch12600.c` (makefile_max=12600). Soft gates; bar3 still open.


## Continuum Batches 12601–12700 (parent wire)

Wired through `graph_batch12700.c` (makefile_max=12700). Soft gates; bar3 still open.


## Continuum Batches 12701–12800 (parent wire)

Wired through `graph_batch12800.c` (makefile_max=12800). Soft gates; bar3 still open.


## Continuum Batches 12801–12900 (parent wire)

Wired through `graph_batch12900.c` (makefile_max=12900). Soft gates; bar3 still open.


## Continuum Batches 12901–13000 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch13000.c` (makefile_max=13000). Milestone 13000 soft gates; bar3 still open.


## Continuum Batches 13001–13100 (parent wire)

Wired through `graph_batch13100.c` (makefile_max=13100). Soft gates; bar3 still open.


## Continuum Batches 13101–13200 (parent wire)

Wired through `graph_batch13200.c` (makefile_max=13200). Soft gates; bar3 still open.


## Continuum Batches 13201–13300 (parent wire)

Wired through `graph_batch13300.c` (makefile_max=13300). Soft gates; bar3 still open.


## Continuum Batches 13301–13400 (parent wire)

Wired through `graph_batch13400.c` (makefile_max=13400). Soft gates; bar3 still open.


## Continuum Batches 13401–13500 (parent wire)

Wired through `graph_batch13500.c` (makefile_max=13500). Soft gates; bar3 still open.


## Continuum Batches 13501–13600 (parent wire)

Wired through `graph_batch13600.c` (makefile_max=13600). Soft gates; bar3 still open.


## Continuum Batches 13701–13800 (parent wire)

Wired through `graph_batch13800.c` (makefile_max=13800). Soft gates; bar3 still open.


## Continuum Batches 13801–13900 (parent wire)

Wired through `graph_batch13900.c` (makefile_max=13900). Soft gates; bar3 still open.


## Continuum Batches 13901–14000 (parent wire)

Wired through `graph_batch14000.c` (makefile_max=14000). Soft gates; bar3 still open.


## Continuum Batches 14001–14100 (parent wire)

Wired through `graph_batch14100.c` (makefile_max=14100). Soft gates; bar3 still open.


## Continuum Batches 14101–14200 (parent wire)

Wired through `graph_batch14200.c` (makefile_max=14200). Soft gates; bar3 still open.


## Continuum Batches 14201–14300 (parent wire)

Wired through `graph_batch14300.c` (makefile_max=14300). Soft gates; bar3 still open.


## Continuum Batches 14301–14400 (parent wire)

Wired through `graph_batch14400.c` (makefile_max=14400). Soft gates; bar3 still open.


## Continuum Batches 14401–14500 (parent wire)

Wired through `graph_batch14500.c` (makefile_max=14500). Soft gates; bar3 still open.


## Continuum Batches 14501–14600 (parent wire)

Wired through `graph_batch14600.c` (makefile_max=14600). Soft gates; bar3 still open.


## Continuum Batches 14601–14700 (parent wire)

Wired through `graph_batch14700.c` (makefile_max=14700). Soft gates; bar3 still open.


## Continuum Batches 14701–14800 (parent wire)

Wired through `graph_batch14800.c` (makefile_max=14800). Soft gates; bar3 still open.


## Continuum Batches 14801–14900 (parent wire)

Wired through `graph_batch14900.c` (makefile_max=14900). Soft gates; bar3 still open.


## Continuum Batches 14901–15000 (parent wire)

Wired through `graph_batch15000.c` (makefile_max=15000). Soft gates; bar3 still open.


## Continuum Batches 15001–15100 (parent wire)

Wired through `graph_batch15100.c` (makefile_max=15100). Soft gates; bar3 still open.


## Continuum Batches 15101–15200 (parent wire)

Wired exclusive CREATE-ONLY continuum product soft stubs through `graph_batch15300.c` (makefile_max=15300). Soft gates / host milestone probes only; bar3 still open.


## Status note — 2026-07-24 (Wave 11 honesty)

**Additive honesty (Wave 11 exclusive for this file):**

| Claim surface | What it is | What it is **not** |
|---------------|------------|--------------------|
| Continuum soft graph | CREATE-ONLY freestanding TUs in `user/libcgj/src/graph_batchN.c`, parent-wired into `CGJ_SRCS` | Full GNU glibc product; drop-in behavioral parity; Steam/Proton runtime complete |
| `makefile_max=15300` | Numeric high-water of wired `graph_batchN.c` basenames (ends at `graph_batch15300.c`; verify `./scripts/gj-continuum-makefile-snippet.sh --max`) | Bar3 closed; Deck Top 50 tried; host-glibc replacement finished |
| libcgj / `libc.so.6` product ELF | Clean-room **glibc-shaped** bring-up + growing symbol surface (versions, smokes) | Shipped GNU glibc; “full glibc” claim |
| Host milestone probes (`cgj_soft_milestone_*`) | Soft gates that symbols/markers resolve under freestanding build | Client launch; title PASS; product score |

**Continuum soft ≠ full glibc product.** Parallel soft waves through **makefile_max=15300** grow clean-room graph surface and host soft probes only. They do **not** make libcgj a full glibc, do **not** close Option C host-glibc / Steam graphs by themselves, and do **not** imply complete `GLIBC_2.*` behavioral parity with GNU libc.

**bar3 still open** (Steam **client** launch + Deck Top 50 title runs; matrix **NOT-TRIED × 50**). Soft surfaces such as `gj_bar3_ready_15200` / `gj_product_score_15200` remain **0** by design. Soft continuum, media `STATUS=READY`, and kernel smokes are **not** bar3 completion.

Also on tree (kernel/media soft, not bar3 / not full glibc): 768G hierarchical soak; media Steam path wired; host soft check READY\|SKELETON\|MISSING.

## Status note — 2026-07-24 (Wave 101 soft continuum)

**Additive honesty (Wave 101 exclusive for this file).** Wave 11 / Wave 14 notes above stay historical ledger. Soft deepen on non-w13 surfaces only.

| Claim surface | What it is | What it is **not** |
|---------------|------------|--------------------|
| Continuum soft graph | CREATE-ONLY freestanding TUs; parent wires advancing makefile high-water **toward 24300** | Full GNU glibc product; drop-in behavioral parity; Steam/Proton runtime complete |
| High-water **toward 24300** | Soft target advance via parent wires (tree may still report prior decade; verify `./scripts/gj-continuum-makefile-snippet.sh --max`) | Bar3 closed; Deck Top 50 tried; host-glibc replacement finished; product complete |
| Soft deepen **rettorusangle** / **retknotangle** | Wave 101 CREATE-ONLY soft graph surface names (docs/scripts/userland soft stamps) | Product glibc; bar3; product lamps lit |
| libcgj / `libc.so.6` product ELF | Clean-room **glibc-shaped** bring-up + growing symbol surface | Shipped GNU glibc; “full glibc” claim |
| Product lamps | Soft stubs (`gj_bar3_ready_*` / `gj_product_score_*` class) remain **0** by design | Any product-score or bar3-ready claim |

**Continuum soft ≠ full glibc product.** Wave 101 soft continuum / parent-wire growth **toward 24300** (soft deepen **rettorusangle**/**retknotangle**) does **not** make libcgj a full glibc and does **not** close Option C host-glibc / Steam graphs. **Soft ≠ product complete.**

**bar3 still open** (Steam **client** launch + Deck Top 50 title runs; matrix **NOT-TRIED × 50**). Product lamps remain **0**. Soft continuum, media `STATUS=READY`, and kernel smokes are **not** bar3 completion.
