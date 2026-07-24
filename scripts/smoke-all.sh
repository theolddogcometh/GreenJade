#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Full bring-up smoke: Multiboot kernel + OVMF EFI + host packaging gates.
#
# HARD GATES (exit 1 on miss) — do not soften:
#   need_mb "…" Multiboot serial markers (product bar)
#   OVMF hard: GJ-EFI on serial
#   host ICD / sessiond / netstackd / storaged PASS
#   stage-esp / stage-rootfs / libcgj / install-img / license
#
# Soft deepen (inventory only — never fails the smoke):
#   soft_mb / soft_efi   optional serial companions
#   TRAP #UD count       volume signal for agents
#   product-summary      scripts/gj-product-summary.sh when present
#   live-iso artifact    presence only (build via make live-iso separately)
#   nvme CAP             already soft (RHEL QEMU splits)
#   continuum high-water via product-summary (Wave 115: toward M=25700;
#     greppable 25700 when scan ≥25700, prior tip 25600 when ≥25600 — not a hard gate;
#     ≠ bar3/Top50; soft deepen retpresentangle/retvsyncangle CREATE-ONLY only;
#     soft ≠ product complete; product lamps 0; do not hardcode false max)
#
# Contrast:
#   scripts/gj-quick-keys.sh      hard presence keys (subset)
#   scripts/gj-product-summary.sh soft inventory always exit 0
#   scripts/gj-soak-large-ram.sh  soft large-RAM path (not required here)
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

echo "== make =="
make -j"$(nproc)" greenjade.efi

echo "== Multiboot (GJ_SMP=2) =="
log_mb="${TMPDIR:-/tmp}/gj-smoke-mb.$$.log"
# PE32 int80 surface + live ELF smokes need more wall time than early M0
GJ_SMP=2 timeout 90 ./scripts/run-qemu.sh build/greenjade.elf >"$log_mb" 2>&1 || true
need_mb() {
    if ! grep -a -q "$1" "$log_mb"; then
        echo "FAIL Multiboot: missing /$1/" >&2
        grep -a -E "TRAP|FATAL|fail" "$log_mb" | head -5 || true
        exit 1
    fi
    echo "  ok: $1"
}
# Soft Multiboot companion — report only; never fails smoke-all.
soft_mb() {
    if grep -a -q "$1" "$log_mb" 2>/dev/null; then
        echo "  soft ok: $1"
    else
        echo "  soft miss: $1"
    fi
}
need_mb "M0 OK"
need_mb "x2apic: supported="
need_mb "ap_run ok"
need_mb "AP schedule ready"
need_mb "INQUIRY ok"
need_mb "greenjade-init"
need_mb "winesrv: A0 demo PASS"
need_mb "eventfd xfer"
need_mb "vk_icd: queue_present ok"
need_mb "compositor: present"
need_mb "session_door: PASS"
need_mb "net_door: PASS"
need_mb "net_door: socket path PASS"
need_mb "net_door: ownership PASS"
need_mb "net_door: virtio queue PASS"
need_mb "net_door: ring map PASS"
need_mb "net_door: avail push PASS"
need_mb "net_door: user ring PASS"
need_mb "store_door: PASS"
need_mb "store_door: ownership PASS"
need_mb "store_door: virtio queue PASS"
need_mb "store_door: ring export PASS"
need_mb "netstackd-gj: avail push PASS"
need_mb "netstackd-gj: user ring PASS"
need_mb "pci: MSI-X probe PASS"
need_mb "pci: MSI-X table PASS"
need_mb "notify: MSI-X IRQ PASS"
need_mb "notify: MSI-X HW path PASS"
need_mb "iommu: probe PASS"
need_mb "iommu: enforce PASS"
need_mb "iommu: vtd tables PASS"
need_mb "wow64: path PASS"
need_mb "store_door: ring map PASS"
need_mb "storaged-gj: ring map PASS"
need_mb "linux: clock_getres PASS"
need_mb "pe32: parse PASS"
need_mb "pe32: section map PASS"
need_mb "pe32: user map PASS"
need_mb "pe32: spawn PASS"
need_mb "pe32: wait4 reap PASS"
need_mb "linux: pread64 PASS"
need_mb "linux: pwrite64 PASS"
need_mb "linux: getdents64 PASS"
need_mb "linux: dup PASS"
need_mb "linux: readlink PASS"
need_mb "linux: wait4 PASS"
need_mb "linux: kill PASS"
need_mb "linux: uname PASS"
need_mb "linux: getuid PASS"
need_mb "linux: access PASS"
need_mb "linux: fstat PASS"
need_mb "linux: prlimit64 PASS"
need_mb "pe32: wow64 trampoline PASS"
need_mb "pe32: wow64 cs32 selector PASS"
need_mb "pe32: compat frame PASS"
need_mb "linux: getgroups PASS"
need_mb "linux: set_tid_address PASS"
need_mb "iommu: vtd identity grant PASS"
need_mb "iommu: vtd soft-only"
need_mb "platform: info PASS"
need_mb "hda: BDL kick PASS"
need_mb "hda: HW CORB path PASS"
need_mb "store_door: rw lba1"
need_mb "pmm: soak PASS"
need_mb "timer: preemption quantum PASS"
need_mb "hda: MMIO CORB path PASS"
need_mb "vk_icd: swapchain present ok"
need_mb "vk: khronos path PASS"
need_mb "vk: QueuePresentKHR"
need_mb "vk: render path PASS"
need_mb "vk: QueueSubmit"
need_mb "vk: multi-frame PASS"
need_mb "vk: spirv path PASS"
need_mb "vk: texture path PASS"
need_mb "vk: uv path PASS"
need_mb "hda: stream path PASS"
need_mb "hda: CORB/BDL path PASS"
need_mb "gj: HDA_STREAM syscall PASS"
need_mb "file_lock: count="
need_mb "session_door: PRESENT_FB PASS"
need_mb "session_door: ownership PASS"
need_mb "session_door: RELEASE PASS"
need_mb "sessiond: live spawn PASS"
need_mb "sessiond-gj: ownership path PASS"
need_mb "netstackd: live spawn PASS"
need_mb "netstackd-gj: live path PASS"
need_mb "storaged: live spawn PASS"
need_mb "vfsd: live spawn PASS"
need_mb "vfsd-gj: block mount PASS"
need_mb "vfsd-gj: file path PASS"
need_mb "vfsd-gj: multi-client door PASS"
need_mb "vfsd-gj: live path PASS"
need_mb "vfs_door: multi-client PASS"
need_mb "vfs_door: openfd PASS"
need_mb "shell: live spawn PASS"
need_mb "greenjade-shell: vfs PASS"
need_mb "greenjade-shell: interactive PASS"
need_mb "greenjade-shell: console poll PASS"
need_mb "greenjade-shell: readline idle PASS"
need_mb "greenjade-shell: install ready PASS"
need_mb "greenjade-shell: install cmd PASS"
need_mb "greenjade-shell: stat touch cp PASS"
need_mb "greenjade-shell: storecap PASS"
need_mb "greenjade-shell: storestats PASS"
need_mb "greenjade-shell: storeflush PASS"
need_mb "greenjade-shell: netstats PASS"
need_mb "greenjade-shell: whoami date env PASS"
need_mb "GreenJade shell"
need_mb "vfs_door: FORMAT PASS"
need_mb "nvme: probe"
# Soft CAP only when QEMU provides -device nvme (optional on RHEL splits)
if grep -a -q "nvme: CAP=" "$log_mb" 2>/dev/null; then
    echo "  ok: nvme: CAP= (soft)"
elif grep -a -q "nvme: probe none" "$log_mb" 2>/dev/null; then
    echo "  ok: nvme: probe none (no emulated ctrl)"
else
    echo "  ok: nvme: probe (present)"
fi
need_mb "ahci: probe"
need_mb "ahci: HBA CAP="
need_mb "usb: probe"
need_mb "ps2: status="
need_mb "console: poll="
need_mb "storaged-gj: live path PASS"
need_mb "hda: CORB DMA engine PASS"
need_mb "linux: poll PASS"
need_mb "linux: ioctl PASS"
need_mb "linux: chdir PASS"
need_mb "linux: writev PASS"
need_mb "linux: readv PASS"
need_mb "linux: clock_nanosleep PASS"
need_mb "pe32: i386 soft-exec PASS"
need_mb "iommu: vtd TE soft-arm PASS"
need_mb "iommu: vtd TE path PASS"
need_mb "except: port smoke PASS"
need_mb "linux: mincore PASS"
need_mb "linux: msync PASS"
need_mb "process: death exit="
need_mb "process: as_destroy"
need_mb "linux: sysinfo PASS"
need_mb "linux: times PASS"
need_mb "linux: getrusage PASS"
need_mb "linux: getpriority PASS"
need_mb "linux: ftruncate PASS"
need_mb "linux: rename PASS"
need_mb "linux: accept PASS"
need_mb "linux: mremap PASS"
need_mb "linux: getsockopt PASS"
need_mb "linux: getsockname PASS"
need_mb "linux: setsockopt PASS"
need_mb "linux: shutdown PASS"
need_mb "linux: rt_sigprocmask PASS"
need_mb "linux: rt_sigaction PASS"
need_mb "linux: F_DUPFD PASS"
need_mb "linux: symlink PASS"
need_mb "linux: utimensat PASS"
need_mb "linux: select PASS"
need_mb "pe32: compat gdt PASS"
need_mb "linux: pipe2 PASS"
need_mb "linux: link PASS"
need_mb "linux: alarm PASS"
need_mb "linux: pause PASS"
need_mb "linux: fchmod PASS"
need_mb "linux: rmdir PASS"
need_mb "linux: fallocate PASS"
need_mb "linux: sendfile PASS"
need_mb "linux: prctl PASS"
need_mb "linux: robust_list PASS"
need_mb "linux: waitid PASS"
need_mb "linux: eventfd2 PASS"
need_mb "linux: epoll PASS"
need_mb "linux: statx PASS"
need_mb "linux: fork PASS"
need_mb "linux: execve PASS"
need_mb "elf: PT_INTERP probe PASS"
need_mb "elf: DT_NEEDED probe PASS"
need_mb "linux: execve INTERP PASS"
need_mb "linux: dynlink path PASS"
need_mb "linux: execve DT_NEEDED resolve PASS"
need_mb "linux: execve auxv PASS"
need_mb "linux: execve RELA PASS"
need_mb "linux: execve SYM PASS"
need_mb "elf: GLOB_DAT/JUMP_SLOT PASS"
need_mb "elf: hash resolve"
need_mb "linux: execve handoff PASS"
need_mb "linux: execve INTERP embed"
need_mb "ld-gj: handoff PASS"
need_mb "ld-gj: AT_ENTRY ready"
need_mb "elf: stack auxv va="
need_mb "linux: execve INTERP-first entry="
need_mb "linux: INTERP-first ready entry="
need_mb "linux: execve INTERP-first live thr="
need_mb "linux: INTERP-first live ring3 thr="
need_mb "elf: gnu-hash resolve"
need_mb "ld-gj: multi-SO"
need_mb "ld-gj: hash/sym"
need_mb "linux: execve SO map PASS"
need_mb "process: linux_fork clone pages="
need_mb "linux: fork clone path PASS"
need_mb "linux: fork COW"
need_mb "vmm: COW break"
need_mb "vmm: as_destroy leaf="
need_mb "priv="
need_mb "vmm: low-band map 0x800000 PASS"
need_mb "linux: clone PASS"
need_mb "linux: timerfd PASS"
need_mb "linux: signalfd PASS"
need_mb "pe32: enter prep PASS"
need_mb "process: cnode_clear slots="
need_mb "linux: creds_mlock PASS"
need_mb "linux: process_vm PASS"
need_mb "linux: pidfd PASS"
need_mb "linux: membarrier_rseq PASS"
need_mb "linux: close_range PASS"
need_mb "linux: affinity PASS"
need_mb "linux: cap_getcpu PASS"
need_mb "linux: sync PASS"
need_mb "linux: inotify PASS"
need_mb "linux: copy_file_range PASS"
need_mb "linux: renameat2 PASS"
need_mb "pe32: hw ready PASS"
need_mb "pe32: soft-iretq PASS"
need_mb "pe32: hw enter PASS"
need_mb "pe32: int80 exit PASS"
need_mb "pe32: int80 getpid+exit PASS"
need_mb "pe32: int80 multi PASS"
need_mb "pe32: int80 mmap2 PASS"
need_mb "pe32: int80 pipe_sock PASS"
need_mb "pe32: int80 fstat64 PASS"
need_mb "pe32: int80 getrandom PASS"
need_mb "pe32: int80 mmap_fixed PASS"
need_mb "pe32: int80 clock_gettime PASS"
need_mb "pe32: int80 uname PASS"
need_mb "pe32: int80 clone PASS"
need_mb "pe32: int80 futex PASS"
need_mb "pe32: int80 tls PASS"
need_mb "pe32: vfs load PASS"
need_mb "pe32: int80 nanosleep PASS"
need_mb "pe32: int80 gettid PASS"
need_mb "pe32: int80 time PASS"
need_mb "pe32: int80 vfs_io PASS"
need_mb "pe32: int80 gettimeofday PASS"
need_mb "pe32: int80 getcwd_sysinfo PASS"
need_mb "pe32: int80 brk PASS"
need_mb "pe32: int80 access PASS"
need_mb "pe32: int80 rename PASS"
need_mb "pe32: int80 pgrp PASS"
need_mb "pe32: int80 getrlimit PASS"
need_mb "pe32: int80 wait4 PASS"
need_mb "pe32: int80 kill PASS"
need_mb "pe32: int80 fsync PASS"
need_mb "pe32: int80 poll PASS"
need_mb "pe32: int80 select PASS"
need_mb "pe32: int80 pipe_rw PASS"
need_mb "pe32: int80 eventfd PASS"
need_mb "pe32: int80 getdents PASS"
need_mb "pe32: int80 link PASS"
need_mb "pe32: int80 clock_getres PASS"
need_mb "pe32: int80 symlink PASS"
need_mb "pe32: int80 pipe2 PASS"
need_mb "pe32: int80 pread_pwrite PASS"
need_mb "pe32: int80 timerfd PASS"
need_mb "pe32: int80 epoll PASS"
need_mb "pe32: int80 inotify PASS"
need_mb "pe32: int80 sendfile PASS"
need_mb "pe32: int80 memfd PASS"
need_mb "pe32: int80 execve PASS"
need_mb "pe32: int80 clone_vm PASS"
need_mb "pe32: int80 vfork_wait PASS"
need_mb "pe32: int80 fork_wait PASS"
need_mb "iommu: vtd TE live-ready PASS"
need_mb "linux: renameat_link PASS"
need_mb "linux: clock_seccomp PASS"
need_mb "linux: fanotify PASS"
need_mb "linux: pidfd_uring PASS"
need_mb "linux: sched_kcmp PASS"
need_mb "linux: mempolicy PASS"
need_mb "linux: posix_timer PASS"
need_mb "linux: gettimeofday PASS"
need_mb "linux: tkill_sched PASS"
need_mb "linux: xattr_readlinkat PASS"
need_mb "linux: dup3 PASS"
need_mb "linux: statfs_openat2 PASS"
need_mb "linux: getrlimit PASS"
need_mb "linux: pkey_execveat PASS"
need_mb "linux: aio_epoll PASS"
need_mb "linux: mount PASS"
need_mb "linux: getsid_pers PASS"

# --- soft Multiboot companions (never fail smoke-all) ---------------------
echo "== Multiboot soft companions =="
soft_mb "sshd: live spawn PASS"
soft_mb "scsi_mid: live spawn PASS"
soft_mb "hda: multi-stream mixer PASS"
soft_mb "TCP multi-segment PASS"
soft_mb "hda_client: live spawn PASS"
soft_mb "sshd-gj: live path PASS"
soft_mb "scsi_mid-gj: READY PASS"
soft_mb "linux: io_uring min rings PASS"
soft_mb "pmm: soak_tib PASS"
soft_mb "pmm: soak_tib SKIP"
soft_mb "cap: mint"
soft_mb "door: timeout"
# TRAP #UD volume (soft — agents care about count; hard smoke stays green either way)
ud_n=$(grep -a -E 'TRAP[[:space:]]+#UD' "$log_mb" 2>/dev/null | wc -l | tr -d ' ' || true)
case "$ud_n" in
    '' | *[!0-9]*) ud_n=0 ;;
esac
echo "  soft info: TRAP #UD count=$ud_n (Multiboot)"
# Soft product-summary panel when helper is present
# (Wave 111: continuum high-water side panel notes toward M=25300; greppable
#  24800 when scan ≥24800, prior tip 24700 when ≥24700 — hard gates stay hard; soft graph ≠ product bar)
if [ -x scripts/gj-product-summary.sh ] || [ -f scripts/gj-product-summary.sh ]; then
    echo "  --- gj-product-summary (soft; wave 111 continuum side panel) ---"
    bash scripts/gj-product-summary.sh "$log_mb" 2>/dev/null || true
fi

echo "== OVMF EFI =="
# Ensure Multiboot kernel is staged as KERNEL.ELF on ESP
make -j"$(nproc)" build/greenjade.elf >/dev/null
log_efi="${TMPDIR:-/tmp}/gj-smoke-efi.$$.log"
timeout 45 ./scripts/run-ovmf.sh build/GreenJade.efi >"$log_efi" 2>&1 || true
# HARD gate: GJ-EFI must appear (firmware path alive).
if ! grep -a -q "GJ-EFI" "$log_efi"; then
    echo "FAIL OVMF: missing GJ-EFI" >&2
    tr -cd '\11\12\15\40-\176\n' <"$log_efi" | tail -30
    exit 1
fi
echo "  ok: GJ-EFI"
# Soft OVMF companions — deepen inventory; never hard-fail smoke here.
soft_efi() {
    if grep -a -q "$1" "$log_efi" 2>/dev/null; then
        echo "  soft ok: $1"
    else
        echo "  soft miss: $1"
    fi
}
soft_efi "GJ-OVMF"
soft_efi "GJ-EFI: OVMF"
soft_efi "KERNEL.ELF loaded"
soft_efi "ExitBootServices ok"
soft_efi "source=UEFI"
soft_efi "M0 OK"
soft_efi "GJ-EFI: GOP soft PASS"
soft_efi "GJ-EFI: GOP soft SKIP"
soft_efi "GJ-EFI: memmap soft PASS"
soft_efi "GJ-EFI: memmap soft REJECT"
soft_efi "GJ-EFI: handoff soft PASS"
soft_efi "GJ-EFI: handoff soft PARTIAL"
soft_efi "GJ-EFI: jump kmain_uefi"
soft_efi "run-ovmf: staged EFI/GREENJADE/KERNEL.ELF"

echo "== host Vulkan ICD =="
make vulkan-icd-host
if ! ./build/icd_host_smoke | tee /tmp/gj-icd-host.log | grep -q "icd_host_smoke: PASS"; then
    echo "FAIL host ICD smoke" >&2
    cat /tmp/gj-icd-host.log | tail -20
    exit 1
fi
echo "  ok: icd_host_smoke PASS"

echo "== host sessiond =="
make sessiond
if ! ./build/sessiond | tee /tmp/gj-sessiond.log | grep -q "sessiond: PASS"; then
    echo "FAIL sessiond smoke" >&2
    cat /tmp/gj-sessiond.log | tail -20
    exit 1
fi
echo "  ok: sessiond PASS"

echo "== freestanding server ELFs =="
make sessiond-gj netstackd-gj storaged-gj
for f in sessiond netstackd storaged; do
    if [ ! -f "build/user/${f}.elf" ]; then
        echo "FAIL ${f}.elf missing" >&2
        exit 1
    fi
    echo "  ok: ${f}.elf built"
done

echo "== host netstackd =="
make netstackd
if ! ./build/netstackd | tee /tmp/gj-netstackd.log | grep -q "netstackd: PASS"; then
    echo "FAIL netstackd smoke" >&2
    cat /tmp/gj-netstackd.log | tail -20
    exit 1
fi
echo "  ok: netstackd PASS"

echo "== host storaged =="
make storaged
if ! ./build/storaged | tee /tmp/gj-storaged.log | grep -q "storaged: PASS"; then
    echo "FAIL storaged smoke" >&2
    cat /tmp/gj-storaged.log | tail -20
    exit 1
fi
echo "  ok: storaged PASS"

echo "== stage-esp (install path) =="
make stage-esp
if ! ./scripts/stage-esp.sh build/esp 2>&1 | tee /tmp/gj-stage-esp.log | grep -q "stage-esp: PASS"; then
    echo "FAIL stage-esp" >&2
    cat /tmp/gj-stage-esp.log | tail -20
    exit 1
fi
echo "  ok: stage-esp PASS"
if [ ! -f build/esp/EFI/BOOT/BOOTX64.EFI ] || [ ! -f build/esp/EFI/GREENJADE/KERNEL.ELF ]; then
    echo "FAIL stage-esp tree incomplete" >&2
    exit 1
fi
echo "  ok: ESP BOOTX64 + KERNEL.ELF"
if [ ! -f build/esp/EFI/GREENJADE/user/init.elf ]; then
    echo "FAIL stage-esp missing user/init.elf" >&2
    exit 1
fi
echo "  ok: ESP user/init.elf"
if [ -f build/esp/EFI/GREENJADE/user/shell.elf ]; then
    echo "  ok: ESP user/shell.elf"
fi

echo "== freestanding vfsd =="
make vfsd-gj
if [ ! -f build/user/vfsd.elf ]; then
    echo "FAIL vfsd.elf missing" >&2
    exit 1
fi
echo "  ok: vfsd.elf built"

echo "== product DT_NEEDED SOs (libgj-so / libgj-gnu) =="
make libgj-so libgj-gnu
for so in libgj-so.so.1 libgj-gnu.so.1; do
    if [ ! -f "build/user/$so" ]; then
        echo "FAIL missing build/user/$so" >&2
        exit 1
    fi
    if ! file "build/user/$so" | grep -q "ELF"; then
        echo "FAIL $so is not ELF" >&2
        file "build/user/$so"
        exit 1
    fi
done
if ! nm -D build/user/libgj-so.so.1 2>/dev/null | grep -q "gj_so_export"; then
    echo "FAIL libgj-so.so.1 missing gj_so_export" >&2
    exit 1
fi
if ! nm -D build/user/libgj-gnu.so.1 2>/dev/null | grep -q "gj_gnu_export"; then
    echo "FAIL libgj-gnu.so.1 missing gj_gnu_export" >&2
    exit 1
fi
echo "  ok: product SO ELFs + exports"

echo "== stage-rootfs =="
chmod +x scripts/stage-rootfs.sh
if ! ./scripts/stage-rootfs.sh build/rootfs 2>&1 | tee /tmp/gj-rootfs.log | grep -q "stage-rootfs: PASS"; then
    echo "FAIL stage-rootfs" >&2
    cat /tmp/gj-rootfs.log | tail -20
    exit 1
fi
if [ ! -f build/rootfs/sbin/init ] || [ ! -f build/rootfs/bin/sh ]; then
    echo "FAIL stage-rootfs incomplete" >&2
    exit 1
fi
echo "  ok: stage-rootfs PASS"
echo "  ok: rootfs sbin/init + bin/sh"
if [ -f build/rootfs/lib/ld-gj.so.1 ] || [ -f build/user/ld-gj.so.1 ]; then
    echo "  ok: rootfs lib/ld-gj.so.1"
else
    echo "FAIL rootfs missing ld-gj.so.1" >&2
    exit 1
fi
for so in libgj-so.so.1 libgj-gnu.so.1; do
    if [ ! -f "build/rootfs/lib/$so" ] || ! file "build/rootfs/lib/$so" | grep -q "ELF"; then
        echo "FAIL rootfs lib/$so not ELF" >&2
        file "build/rootfs/lib/$so" 2>/dev/null || true
        exit 1
    fi
done
echo "  ok: rootfs libgj-so + libgj-gnu ELF"
if [ -f build/rootfs/usr/bin/vfsd ]; then
    echo "  ok: rootfs usr/bin/vfsd"
else
    echo "FAIL rootfs missing vfsd" >&2
    exit 1
fi

echo "== libcgj (clean-room glibc-shaped libc) =="
make libcgj
if [ ! -f build/user/libc.so.6 ]; then
    echo "FAIL libcgj missing libc.so.6" >&2
    exit 1
fi
# Must be ELF (not text stub)
if ! file build/user/libc.so.6 | grep -q "ELF"; then
    echo "FAIL libc.so.6 is not ELF" >&2
    file build/user/libc.so.6
    exit 1
fi
echo "  ok: libc.so.6 ELF"
# Host freestanding smoke (glibc-shaped API via SYSCALL)
if ! ./build/cgj_smoke 2>&1 | tee /tmp/gj-cgj-smoke.log | grep -q "libcgj: smoke PASS"; then
    echo "FAIL cgj_smoke" >&2
    cat /tmp/gj-cgj-smoke.log
    exit 1
fi
echo "  ok: libcgj host smoke PASS"
# Dynamic link against product libc.so.6 (DT_NEEDED path)
if [ ! -x build/cgj_dyn_smoke ]; then
    echo "FAIL cgj_dyn_smoke missing" >&2
    exit 1
fi
if ! ./build/cgj_dyn_smoke 2>&1 | tee /tmp/gj-cgj-dyn-smoke.log | grep -q "libcgj: dyn smoke PASS"; then
    echo "FAIL cgj_dyn_smoke" >&2
    cat /tmp/gj-cgj-dyn-smoke.log
    exit 1
fi
echo "  ok: libcgj dyn smoke PASS"
# Export surface: core symbols apps expect from libc.so.6
for sym in malloc free printf write mmap fork clock_gettime strtol \
           pthread_mutex_lock __tls_get_addr __errno_location __libc_start_main \
           setvbuf fflush memfd_create getenv setenv poll select getopt \
           getopt_long getrandom getentropy strcasecmp basename socket socketpair \
           opendir ioctl eventfd fnmatch realpath ftruncate sysconf \
           confstr strsignal gethostname scandir alphasort fallocate \
           aligned_alloc posix_memalign sched_yield pread pwrite mkstemp mkdtemp clock_getres \
           readv writev sync clearenv getopt_long_only \
           posix_fallocate getpagesize strndup dlopen dlsym dlclose dlerror \
           strnlen stpcpy memmem strtok_r strsep strtoll strtoull atoll llabs \
           strchrnul timerfd_create timerfd_settime timerfd_gettime \
           signalfd sendfile ppoll epoll_create1 epoll_ctl epoll_wait \
           inet_pton inet_ntop flock lockf getaddrinfo freeaddrinfo \
           gai_strerror if_nametoindex if_indextoname getifaddrs freeifaddrs \
           openlog syslog closelog glob globfree res_init \
           err warn ftw nftw madvise msync nl_langinfo iconv_open \
           backtrace strftime gmtime_r localtime_r strptime timegm mktime tzset \
           setsid sysinfo getloadavg daemon getrusage getrlimit setrlimit \
           getpriority setpriority nice prlimit times waitid \
           geteuid getegid getresuid setresuid getgroups utimensat utimes \
           chmod fchmod chown fchown umask statfs statvfs \
           fdatasync pathconf fpathconf eaccess syncfs \
           readlinkat unlinkat renameat mkdirat linkat copy_file_range; do
    if ! nm -D --with-symbol-versions build/user/libc.so.6 2>/dev/null | \
            grep -E " (T|D|B) $sym(@@GLIBC_|$)" >/dev/null && \
       ! nm -D build/user/libc.so.6 2>/dev/null | grep -qE " [TDB] $sym\$"; then
        echo "FAIL libc.so.6 missing export: $sym" >&2
        nm -D --with-symbol-versions build/user/libc.so.6 2>/dev/null | head -50
        exit 1
    fi
done
echo "  ok: libc.so.6 core exports"
# GNU symbol versions (host-glibc-shaped)
if ! readelf -V build/user/libc.so.6 2>/dev/null | grep -q "GLIBC_2.2.5"; then
    echo "FAIL libc.so.6 missing GLIBC_2.2.5 version node" >&2
    readelf -V build/user/libc.so.6 2>/dev/null | head -40
    exit 1
fi
echo "  ok: libc.so.6 GLIBC_2.* versions"
if [ -f build/rootfs/lib/libc.so.6 ] && file build/rootfs/lib/libc.so.6 | grep -q "ELF"; then
    echo "  ok: rootfs lib/libc.so.6 ELF"
else
    echo "FAIL rootfs libc.so.6 not ELF" >&2
    exit 1
fi

echo "== install-img (GPT ESP) =="
if ! make install-img 2>&1 | tee /tmp/gj-install-img.log | grep -q "make-install-img: PASS"; then
    echo "FAIL install-img" >&2
    cat /tmp/gj-install-img.log | tail -20
    exit 1
fi
if [ ! -f build/greenjade-install.img ]; then
    echo "FAIL install-img missing" >&2
    exit 1
fi
echo "  ok: install-img PASS"

echo "== license =="
make license

# --- soft host artifact companions (never fail; packaging is hard above) -
echo "== soft host artifacts =="
for art in \
    build/greenjade.elf \
    build/GreenJade.efi \
    build/greenjade-install.img \
    build/greenjade-live.iso \
    build/user/sshd.elf \
    build/user/scsi_mid.elf \
    build/user/libc.so.6 \
    build/user/libgj-so.so.1 \
    build/user/libgj-gnu.so.1
do
    if [ -f "$art" ]; then
        echo "  soft ok: artifact $art"
    else
        echo "  soft miss: artifact $art"
    fi
done
# live-iso is a separate make target; soft presence only here
if [ -f build/greenjade-live.iso ]; then
    echo "  soft ok: live-iso present (run: ./scripts/run-live-iso.sh)"
else
    echo "  soft miss: live-iso (make live-iso — not required by smoke-all)"
fi

echo "smoke-all: PASS"
rm -f "$log_mb" "$log_efi"
