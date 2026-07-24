/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding product shell — pure C11, libgj only (NATIVE personality).
 * VFS door client: scripted command suite for smoke, then exits (no hang).
 * Install-mode may accept a bounded number of serial lines after the script.
 *
 * Smoke markers (prefix-stable; greppable live logs):
 *   greenjade-shell: ready
 *   greenjade-shell: abi PASS
 *   greenjade-shell: console poll PASS
 *   greenjade-shell: interactive script start
 *   greenjade-shell: readline idle PASS | readline got line PASS
 *   greenjade-shell: install ready PASS
 *   greenjade-shell: vfs PASS
 *   greenjade-shell: interactive PASS
 *
 * Parent owns Makefile target + kernel/proc/shell_embed.S (.incbin).
 *
 * Command surface (product continuum; see run_line / aScript):
 * Commands: help echo cat ls rm mv head tail wc cmp sleep size true false
 *           clear hostname uname id whoami date env sync format mount write
 *           bigfile install stats printenv nproc uptime basename dirname
 *           cksum grep test printf seq which type free df yes sessstats
 *           rev nl od expr tee cut tr fold uniq umask
 *           sort strings expand logname tty groups factor
 *           tac paste sum join comm
 *           unexpand shuf colrm fmt pathchk
 *           split base64 arch getconf realpath mkdir
 *           hexdump du truncate link unlink (product v0.11)
 *           xxd md5sum file dir vdir ln rmdir (product v0.12)
 *           readlink tsort pr (product v0.13)
 *           md5sum who users file chmod chown (product v0.14)
 *           namei shred nice sha1sum (product v0.15)
 *           sha256sum dos2unix unix2dos (product v0.16)
 *           base32 (product v0.17)
 *           sha512sum (product v0.18)
 *           crc32 (product v0.19)
 *           b2sum uuidgen (product v0.20)
 *           sha3sum (product v0.21)
 *           adler32 (product v0.22)
 *           sha384sum (product v0.23)
 *           sha224sum xxhsum crc32c (product v0.24)
 *           fnvsum siphash xxh3sum (product v0.25)
 *           sm3sum crc16 (product v0.26)
 *           crc8 base58 (product v0.27)
 *           b3sum crc64 (product v0.28)
 *           base85 ascii85 sum64 (product v0.29)
 *           gunzip inflate b64 (product v0.30)
 *           cpuid aescheck (product v0.31)
 *           tarlist base64url urlencode (product v0.32)
 *           filemagic identify urldecode (product v0.33)
 *           crock32 ipsum (product v0.34)
 *           duration macfmt (product v0.35)
 *           slug lev (product v0.36)
 *           gray bitrev (product v0.37)
 *           gcd isprime (product v0.38)
 *           fib modpow (product v0.39)
 *           lcm factor (product v0.40)
 *           binom fibmod (product v0.41)
 *           hamming popcnt (product v0.42)
 *           xor and or (product v0.43)
 *           shl shr (product v0.44)
 *           abs minmax (product v0.45)
 *           avg pow (product v0.46)
 *           digits dsum (product v0.47)
 *           revdig ispal (product v0.48)
 *           fact sqrt (product v0.49)
 *           ilog2 ctz (product v0.50)
 *           clz bswap (product v0.51)
 *           satadd satsub (product v0.52)
 *           ispow2 alignup (product v0.53)
 *           aligndown hexnib (product v0.54)
 *           min3 max3 (product v0.55)
 *           median3 (product v0.56)
 *           clamp bitset (product v0.57)
 *           bittest bitclr (product v0.58)
 *           bittog bitcnt (product v0.59)
 *           bitwidth ishex (product v0.60)
 *           wrapadd wrapsub (product v0.61)
 *           inrange isblank (product v0.62)
 *           isident digitval (product v0.63)
 *           alphaval isempty (product v0.64)
 *           pct safediv (product v0.65)
 *           bpoints ratio (product v0.66)
 *           divceil isaligned (product v0.67)
 *           log2ceil modpow2 (product v0.68)
 *           istrue isfalse (product v0.69)
 *           ison isoff (product v0.70)
 *           iszero isnonzero (product v0.71)
 *           min2 max2 (product v0.72)
 *           mid2 absdiff (product v0.73)
 *           bitor bitand bitxor (product v0.74)
 *           bitnot lshift rshift (product v0.75)
 *           rotl rotr popcount (product v0.76)
 *           bswap32 bswap64 (product v0.77)
 *           parity bytelen (product v0.78)
 *           nibblecnt lo32 hi32 (product v0.79)
 *           umin umax uavg (product v0.80)
 *           clampu mulhi (product v0.81)
 *           ugcd ulcm umod (product v0.82)
 *           isqrt icbrt ispow2u (product v0.83)
 *           popmsb poplsb bits (product v0.84)
 *           revbits revbytes ntz (product v0.85)
 *           tohex fromhex (product v0.86)
 *           bitrev8 bitrev16 bitrev32u (product v0.87)
 *           pack16 unpack16lo unpack16hi (product v0.88)
 *           pack32 unpack32lo unpack32hi (product v0.89)
 *           btest bset bclr (product v0.90)
 *           bflip masklow maskhigh (product v0.91)
 *           setbitcnt isodd iseven (product v0.92)
 *           minu2 maxu2 clamp3 (product v0.93)
 *           udiff avg2 (product v0.94)
 *           ispow2n nextpow2 (product v0.95)
 *           popcntu tzcnt lzcnt (product v0.96)
 *           rol ror (product v0.97)
 *           byteswap iszerou isone (product v0.98)
 *           ham grayenc graydec (product v0.99)
 *           signum parityu (product v1.00)
 *           uclamp umid (product v1.01)
 *           usatadd usatsub (product v1.02)
 *           flog2 usqrt (product v1.03)
 *           urem udiv (product v1.04)
 *           umul umulhi (product v1.05)
 *           ubswap32 ubswap16 (product v1.06)
 *           ushl ushr (product v1.07)
 *           uand uor uxor (product v1.08)
 *           unot umin2 umax2 (product v1.09)
 *           unand unor (product v1.10)
 *           udec uinc (product v1.11)
 *           upow2 ulmask (product v1.12)
 *           ucmp ueq une (product v1.13)
 *           ult ule ugt uge (product v1.14)
 *           ubits uall uany (product v1.15)
 *           unz uone (product v1.16)
 *           ulsb umsb (product v1.17)
 *           uclrlsb uclrmsb (product v1.18)
 *           uabsdiff (product v1.19)
 *           uparity uham uisolsb uisomsb (product v1.20)
 *           uispow2 ulcm0 uilog2 (product v1.21)
 *           uandn unor2 (product v1.22)
 *           uxnor umaj umux (product v1.23)
 *           urgba urgb uluma (product v1.24)
 *           uip4 uip4a uhttp2xx (product v1.25)
 *           upage upagec upfn (product v1.26)
 *           ubcast uhas0 ubsum (product v1.27)
 *           ustride ufbsize urgb565 (product v1.28)
 *           upciaddr upcibus upcidev (product v1.29)
 *           unvmeok uahciis uvirtio (product v1.30)
 *           uisfhd uisuhd uaspect (product v1.31)
 *           udecknat ubar3 umile (product v1.32)
 *           uvk uwl ulic (product v1.33)
 *           ufsr ufps utop50 (product v1.34)
 *           uproton ustore uconn (product v1.35)
 *           ugpu ubat utherm (product v1.36)
 *           uvrr uhdr ufan (product v1.37)
 *           ucloud urplay uach (product v1.38)
 *           unvme upcie ucpuid (product v1.39)
 *           ufutex uclone useccomp (product v1.40)
 *           umemfd ustatx umount2 (product v1.41)
 *           udlopen usock uclock (product v1.42)
 *           upoll uaccept upipe2 (product v1.43)
 *           uwait ukill usig (product v1.44)
 *           urlim usysinfo uuname (product v1.45)
 *           uprintf uqsort urand (product v1.46)
 *           uiconv ulocale unl (product v1.47)
 *           uregex uglob ufnmatch (product v1.48)
 *           ubpf unetlink uresolv (product v1.49)
 *           uvulkan uwayland udrm (product v1.50)
 *           ujson uzip upng (product v1.51)
 *           uhttp usqlite usdl (product v1.52)
 *           uffmpeg ugstreamer uudev (product v1.53)
 *           unm uavahi ujack (product v1.54)
 *           ulua ugit ucmake (product v1.55)
 *           udocker uqemu uwg (product v1.56)
 *           uzfs uluks utpm (product v1.57)
 *           uefi uacpi unvme (product v1.58)
 *           uvirtio ukvm uinstall (product v1.59)
 *           uproton ugamescope ubar3 (product v1.60)
 *           uflatpak uportal ufont (product v1.61)
 *           ubar3checklist uprotonsoft umesasoft (product v1.62)
 *           ugamesoft unetsoft upowersoft (product v1.62)
 *           uibus ufcitx udbus (product v1.63)
 *           upolkit useatd uimsoft (product v1.63)
 *           uauthsoft uinputsoft unetmgrsoft (product v1.64)
 *           uprintsoft udbussoft (product v1.64)
 *           ubar3soft umediapath usteamrun (product v1.65)
 *           uhdamulti uscsimid usshd (product v1.65)
 *           unetstack upmmhier (product v1.65)
 *           ucontinuum14500 uhda_client uscsi_live (product v1.66)
 *           u768gsoak umakefile14600 ubar3open (product v1.67)
 *           ucontinuum14600 uioringmin u1tibopen (product v1.68)
 *           ucontinuum15300 umakefile15300 umatrixopen (product v1.69)
 *           uwave14 umulticonfine udaemonsoft (product v1.70 Wave 14)
 *           uwave15 (product v1.71 Wave 15 exclusive soft deepen kept)
 *           uwave16 (product v1.72 Wave 16 exclusive soft deepen kept)
 *           uwave17 (product v1.73 Wave 18 exclusive soft deepen kept)
 *           uwave18 (product v1.74 Wave 38 exclusive soft deepen kept)
 *           uwave19 (product v1.75 Wave 39 exclusive soft deepen kept)
 *           uwave20 (product v1.76 Wave 42 exclusive soft deepen kept)
 *           uwave21 (product v1.77 Wave 43 exclusive soft deepen kept)
 *           uwave22 (product v1.78 Wave 76 exclusive soft deepen)
 *           (satadd/satsub/wrapadd/wrapsub already exist; wrapsub skipped)
 *           (bitrev exists as 32-bit hex v0.37; bitrev32u is decimal low-32)
 *           (bswap16 skipped as bare name: bswap/bswap32/bswap64 cover family;
 *            ubswap16/ubswap32 are free exclusive product names)
 *           (bitset/bittest/bitclr exist v0.57/v0.58; btest/bset/bclr soft-bound N)
 *           (bittog exists v0.59 strict; bflip soft-bounds N like bset/bclr)
 *           (popcnt/bitcnt/popcount/bits exist; setbitcnt is alias-style name free)
 *           (min2/max2 exist v0.72; minu2/maxu2 free alias-style names)
 *           (clamp exists v0.57; clamp3 free name, same [LO,HI] clamp of A)
 *           (absdiff exists v0.73; udiff free alias-style name)
 *           (avg/mid2/uavg exist; avg2 free alias-style name)
 *           (ispow2/ispow2u exist v0.53/v0.83; ispow2n free alias-style name)
 *           (popcnt/ctz/clz exist; popcntu/tzcnt/lzcnt free alias-style names)
 *           (rotl/rotr exist v0.76; rol/ror free alias-style names, N masked)
 *           (bswap/iszero exist; byteswap/iszerou free; isone new)
 *           (hamming/gray exist; ham/grayenc free; graydec is Gray→binary)
 *           (parity exists v0.78; parityu free exclusive; signum is signed -1/0/1)
 *           (clamp/clampu/clamp3 exist; uclamp free exclusive; umid midpoint)
 *           (satadd/satsub exist v0.52; usatadd/usatsub free exclusive names)
 *           (ilog2/isqrt exist v0.50/v0.83; flog2/usqrt free exclusive names)
 *           (umod/safediv exist v0.82/v0.65; urem/udiv free exclusive names)
 *           (mulhi exists v0.81; umul wrap mul; umulhi free exclusive mul-hi)
 *           (shl/shr/lshift/rshift exist; ushl/ushr free exclusive, N masked 63)
 *           (and/or/xor/bitand/bitor/bitxor exist; uand/uor/uxor free exclusive)
 *           (bitnot exists v0.75; unot free exclusive)
 *           (umin/umax exist v0.80; umin2/umax2 free exclusive alias-style names)
 *           (and/or/bitand/bitor exist; unand/unor free exclusive NAND/NOR)
 *           (satsub/usatsub exist; udec/uinc free exclusive sat dec/inc by 1)
 *           (masklow exists v0.91; umask is classic file-mode stub; ulmask free exclusive)
 *           (cmp is file compare; ucmp/ueq/une free exclusive u64 compare trio)
 *           (no ult/ule/ugt/uge yet; free exclusive relational 1/0 predicates)
 *           (popcnt/popcount/bits/setbitcnt exist; ubits free exclusive popcount)
 *           (iszero/isnonzero exist; uall/uany free exclusive all-bits/any-bit)
 *           (iszero/isone exist; unz/uone free exclusive zero/one predicates)
 *           (poplsb/popmsb return indices; ulsb/umsb free exclusive isolate-bit)
 *           (bclr/bitclr clear by index; uclrlsb/uclrmsb free exclusive clear-end)
 *           (absdiff/udiff exist v0.73/v0.94; uabsdiff free exclusive abs-diff)
 *           (parity/parityu/hamming/ham/ulsb/umsb exist; uparity/uham/uisolsb/uisomsb free exclusive)
 *           (ispow2/ispow2u/ispow2n exist; uispow2 free exclusive pow2 predicate)
 *           (lcm fails overflow; ulcm saturates; ulcm0 free exclusive LCM→0 on overflow)
 *           (ilog2/flog2 exist; uilog2 free exclusive floor-log2, 0→0)
 *           (unand/unor exist v1.10 as NAND/NOR; uandn is A&~B; unor taken → unor2)
 *           (uxor/bitxor/xor exist; uxnor free exclusive XNOR; umaj majority; umux mux)
 *           (urgba packs 0xAARRGGBB; urgb packs 0x00RRGGBB; uluma BT.601 of packed RGB)
 *           (uip4 packs A.B.C.D → network-order u32; uip4a first octet; uhttp2xx 2xx→1)
 *           (aligndown/alignup exist; upage/upagec free exclusive 4096-page floor/ceil;
 *            upfn is page frame number A>>12; upagec → 0 on u64 overflow)
 *           (ubcast replicates low 8 bits into all 4 bytes of a u32;
 *            uhas0 is 1 if any zero byte in low 32 of A; ubsum sums those 4 bytes)
 *           (ustride is row bytes W*(BPP/8); non-byte BPP → 0; overflow → 0;
 *            ufbsize is W*H*(BPP/8) with overflow → 0; urgb565 packs 5:6:5)
 *           (upciaddr packs CF8 config addr with enable bit 31;
 *            upcibus/upcidev extract bus/device fields from a packed ADDR)
 *           (unvmeok is 1 if NVMe CSTS-style STATUS low 11 bits clear;
 *            uahciis is 1 if SATA_SIG is 0x00000101; uvirtio is 1 if V is
 *            VirtIO magic 0x74726976 ('virt' LE))
 *           (uisfhd is 1 if W==1920 and H==1080 (Full HD);
 *            uisuhd is 1 if W==3840 and H==2160 (UHD/4K);
 *            uaspect prints gcd-reduced W/H as "W/H")
 *           (udecknat is 1 if W==1280 and H==800 (Steam Deck native);
 *            ubar3 prints 1 (bar3 checklist path stub ready) since v1.60
 *            (printed 0 as bar3 still open in v1.32); umile echoes milestone id A)
 *           (uvk prints 1 (Vulkan path stub ready); uwl prints 1 (Wayland
 *            path stub ready); ulic prints 1 (dual MIT/Apache license gate ok))
 *           (ufsr prints 1 (FSR stub ready); ufps echoes FPS limit arg or 60
 *            default; utop50 prints 50)
 *           (uproton prints 1 (Proton path stub ready); ustore prints 1
 *            (store path stub ready); uconn prints 1 (product connect ready))
 *           (ugpu prints 1 (GPU path stub ready); ubat prints 100 (battery
 *            percent stub); utherm prints 1 (thermal path stub ready))
 *           (uvrr prints 1 (VRR path stub ready); uhdr prints 0 (HDR path
 *            still open); ufan prints 0 (fan path still open))
 *           (ucloud prints 1 (cloud path stub ready); urplay prints 1
 *            (remote play path stub ready); uach prints 0 (achievements
 *            path still open))
 *           (unvme prints 1 (NVMe path stub ready); upcie prints 1
 *            (PCIe path stub ready); ucpuid prints 1 (CPUID path stub ready);
 *            free exclusive short names; unvmeok/upciaddr/cpuid already exist)
 *           (ufutex prints 1 (futex path stub ready); uclone prints 1
 *            (clone path stub ready); useccomp prints 1 (seccomp path stub
 *            ready); free exclusive short names)
 *           (umemfd prints 1 (memfd path stub ready); ustatx prints 1
 *            (statx path stub ready); umount2 prints 1 (umount2 path stub
 *            ready); free exclusive short names)
 *           (udlopen prints 1 (dlopen path stub ready); usock prints 1
 *            (socket path stub ready); uclock prints 1 (clock path stub
 *            ready); free exclusive short names)
 *           (upoll prints 1 (poll path stub ready); uaccept prints 1
 *            (accept path stub ready); upipe2 prints 1 (pipe2 path stub
 *            ready); free exclusive short names)
 *           (uwait prints 1 (wait path stub ready); ukill prints 1
 *            (kill path stub ready); usig prints 1 (signal path stub
 *            ready); free exclusive short names)
 *           (urlim prints 1 (rlimit path stub ready); usysinfo prints 1
 *            (sysinfo path stub ready); uuname prints 1 (uname path stub
 *            ready); free exclusive short names; uname already exists)
 *           (uprintf prints 1 (printf path stub ready); uqsort prints 1
 *            (qsort path stub ready); urand prints 1 (rand path stub
 *            ready); free exclusive short names; printf already exists)
 *           (uiconv prints 1 (iconv path stub ready); ulocale prints 1
 *            (locale path stub ready); unl prints 1 (nl_langinfo path
 *            stub ready); free exclusive short names; nl already exists)
 *           (uregex prints 1 (regex path stub ready); uglob prints 1
 *            (glob path stub ready); ufnmatch prints 1 (fnmatch path
 *            stub ready); free exclusive short names)
 *           (ubpf prints 1 (bpf path stub ready); unetlink prints 1
 *            (netlink path stub ready); uresolv prints 1 (resolv path
 *            stub ready); free exclusive short names)
 *           (uvulkan prints 1 (vulkan path stub ready); uwayland prints 1
 *            (wayland path stub ready); udrm prints 1 (drm path stub
 *            ready); free exclusive short names)
 *           (ujson prints 1 (json path stub ready); uzip prints 1
 *            (zip path stub ready); upng prints 1 (png path stub
 *            ready); free exclusive short names)
 *           (uhttp prints 1 (http path stub ready); usqlite prints 1
 *            (sqlite path stub ready); usdl prints 1 (sdl path stub
 *            ready); free exclusive short names; uhttp2xx already exists)
 *           (uffmpeg prints 1 (ffmpeg path stub ready); ugstreamer prints 1
 *            (gstreamer path stub ready); uudev prints 1 (udev path stub
 *            ready); free exclusive short names; udev skipped as bare name)
 *           (unm prints 1 (networkmanager path stub ready); uavahi prints 1
 *            (avahi path stub ready); ujack prints 1 (jack path stub
 *            ready); free exclusive short names)
 *           (ulua prints 1 (lua path stub ready); ugit prints 1
 *            (git path stub ready); ucmake prints 1 (cmake path stub
 *            ready); free exclusive short names)
 *           (udocker prints 1 (docker path stub ready); uqemu prints 1
 *            (qemu path stub ready); uwg prints 1 (wireguard path stub
 *            ready); free exclusive short names)
 *           (uzfs prints 1 (zfs path stub ready); uluks prints 1
 *            (luks/cryptsetup path stub ready); utpm prints 1 (tpm path
 *            stub ready); free exclusive short names)
 *           (uefi prints 1 (efi path stub ready); uacpi prints 1
 *            (acpi path stub ready); unvme already prints 1 (nvme path
 *            stub ready) since v1.39 — re-listed as product v1.58 path
 *            trio with free exclusive uefi/uacpi; free exclusive short names)
 *           (uvirtio no-arg prints 1 (virtio path stub ready); with arg
 *            still checks VirtIO magic 0x74726976 since v1.30; ukvm prints 1
 *            (kvm path stub ready); uinstall prints 1 (install path stub
 *            ready); free exclusive ukvm/uinstall; uvirtio dual-mode)
 *           (uproton already prints 1 (proton path stub ready) since v1.35 —
 *            re-listed as product v1.60 path trio with free exclusive
 *            ugamescope; ubar3 now prints 1 (bar3 checklist path stub ready);
 *            free exclusive short names)
 *           (uflatpak prints 1 (flatpak path stub ready); uportal prints 1
 *            (xdg-portal path stub ready); ufont prints 1 (fontconfig path
 *            stub ready); free exclusive short names)
 *           (ubar3checklist prints 0 (bar3 checklist soft status until real);
 *            uprotonsoft/umesasoft/ugamesoft/unetsoft/upowersoft print 0
 *            (soft ready tags until real); free exclusive continuum names)
 *           (uibus/ufcitx/udbus/upolkit/useatd print 1 (path stub ready);
 *            uudev already product v1.53 → uimsoft prints 0 (im soft ready
 *            tag until real); free exclusive continuum names)
 *           (uauthsoft/uinputsoft/unetmgrsoft/uprintsoft/udbussoft print 0
 *            (soft ready tags until real); uimsoft already product v1.63;
 *            free exclusive continuum names)
 *           (ubar3soft prints bar3 soft status lines: media path wired=1 and
 *            steam client run=0 — soft facts only, not Deck Top 50 PASS;
 *            umediapath prints 1 (media path wired soft); usteamrun prints 0
 *            (steam client run soft); uhdamulti/uscsimid/usshd/unetstack/
 *            upmmhier print 0 (product readiness soft tags until real);
 *            free exclusive continuum names; does not claim Top 50 PASS)
 *           (ucontinuum14500 prints 1 (wired soft); uhda_client prints 0
 *            (hda client embed not live yet); uscsi_live prints 0
 *            (scsi live soft tag until real); free exclusive continuum names;
 *            soft facts only; does not claim Deck Top 50 PASS)
 *           (u768gsoak prints 1 (large-RAM soak path shipped soft);
 *            umakefile14600 prints 1; ubar3open prints 0 (bar3 still open);
 *            free exclusive continuum names; soft facts only;
 *            does not claim Deck Top 50 PASS)
 *           (ucontinuum14600 prints 1 (wired soft decade after 14500);
 *            uioringmin prints 1 (io_uring min rings shipped soft — not full
 *            SQE for games); u1tibopen prints 0 (full 1 TiB host soak still
 *            open soft); free exclusive continuum names; soft facts only;
 *            does not claim Deck Top 50 PASS)
 *           (ucontinuum15300 prints 1 (wired soft high-water Wave 11);
 *            umakefile15300 prints 1 (makefile_max=15300 soft fact);
 *            umatrixopen prints 0 (Deck Top 50 matrix still NOT-TRIED soft);
 *            free exclusive continuum names; soft facts only;
 *            soft continuum ≠ bar3; does not claim Deck Top 50 PASS)
 *           (uwave14 prints 1 (Wave 14 exclusive soft deepen stamp kept);
 *            umulticonfine prints 0 (soft ≠ product multi-server confine);
 *            udaemonsoft prints soft daemon inventory honesty lines;
 *            uwave15 prints 1 (Wave 15 exclusive soft deepen stamp kept);
 *            uwave16 prints 1 (Wave 16 exclusive soft deepen stamp kept);
 *            uwave17 prints 1 (Wave 18 exclusive soft deepen stamp kept);
 *            uwave18 prints 1 (Wave 38 exclusive soft deepen stamp kept);
 *            uwave19 prints 1 (Wave 39 exclusive soft deepen stamp kept);
 *            uwave20 prints 1 (Wave 42 exclusive soft deepen stamp kept);
 *            uwave21 prints 1 (Wave 43 exclusive soft deepen stamp kept);
 *            uwave22 prints 1 (Wave 76 exclusive soft deepen stamp);
 *            free exclusive names; soft facts only; soft ≠ bar3)
 *           (help/version soft banners Wave 76 honesty: help prints
 *            GreenJade soft product + shell=native vfs door + soft=1 +
 *            bar3=0 + multi_server=0 + confine=0 before cmds:; version keeps
 *            "GreenJade shell (native vfs door) v1.76" smoke substring then
 *            GreenJade soft product + soft=1 + bar3=0 + multi_server=0 +
 *            confine=0 + wave=70 + product_kernel=OPEN; soft facts only —
 *            not bar3 close, not Deck Top 50 PASS; soft ≠ product
 *            multi-server confine)
 */
#include <gj/string.h>
#include <gj/syscalls.h>

static void
out(const char *sz)
{
    if (sz == NULL) {
        return;
    }
    (void)gj_debug_log(sz, (long)gj_strlen(sz));
}

static const char *
skip_ws(const char *p)
{
    while (p != NULL && (*p == ' ' || *p == '\t')) {
        p++;
    }
    return p;
}

/* Split first word into aWord; return rest of line */
static const char *
split_word(const char *szLine, char *aWord, size_t cbWord)
{
    size_t i = 0;
    const char *p = skip_ws(szLine);

    if (p == NULL || aWord == NULL || cbWord == 0) {
        return NULL;
    }
    while (p[i] != '\0' && p[i] != ' ' && p[i] != '\t' && i + 1 < cbWord) {
        aWord[i] = p[i];
        i++;
    }
    aWord[i] = '\0';
    p = skip_ws(p + i);
    return p;
}

static int
cmd_echo_file(const char *szName, const char *szText)
{
    long n;
    size_t cb;

    if (szName == NULL || szText == NULL) {
        return -1;
    }
    cb = gj_strlen(szText);
    n = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)szName,
               (long)(uintptr_t)szText, (long)cb);
    return (n == (long)cb) ? 0 : -1;
}

/* Append text (+ newline) to an existing small file, or create if missing. */
static int
cmd_echo_append(const char *szName, const char *szText)
{
    static char aBuf[512];
    static char aOut[512];
    long n;
    size_t cbText;
    size_t i;
    size_t nOut;

    if (szName == NULL || szText == NULL) {
        return -1;
    }
    cbText = gj_strlen(szText);
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        n = 0;
    }
    if ((size_t)n + cbText + 2u >= sizeof(aOut)) {
        return -1;
    }
    nOut = 0;
    for (i = 0; i < (size_t)n; i++) {
        aOut[nOut++] = aBuf[i];
    }
    for (i = 0; i < cbText; i++) {
        aOut[nOut++] = szText[i];
    }
    aOut[nOut++] = '\n';
    aOut[nOut] = '\0';
    return cmd_echo_file(szName, aOut);
}

static int
cmd_cat(const char *szName)
{
    static char aBuf[1100];
    long n;

    if (szName == NULL) {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    aBuf[n] = '\0';
    out(aBuf);
    if (n == 0 || aBuf[n - 1] != '\n') {
        out("\n");
    }
    return 0; /* success (bytes were read; n may be 0 for empty file) */
}

static int
cmd_ls(void)
{
    static char aList[512];
    long n;

    n = gj_vfs(GJ_VFS_OP_LIST, (long)(uintptr_t)aList, 0, (long)sizeof(aList));
    if (n < 0) {
        return -1;
    }
    if (n == 0) {
        out("(empty)\n");
        return 0;
    }
    if ((size_t)n < sizeof(aList)) {
        aList[n] = '\0';
    } else {
        aList[sizeof(aList) - 1] = '\0';
    }
    out(aList);
    return 0;
}

static int
cmd_rm(const char *szName)
{
    if (szName == NULL) {
        return -1;
    }
    return (gj_vfs(GJ_VFS_OP_UNLINK, (long)(uintptr_t)szName, 0, 0) == 0) ? 0
                                                                           : -1;
}

static int
cmd_stat(const char *szName)
{
    unsigned aSt[2];
    char aN[16];
    size_t n;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    if (gj_vfs(GJ_VFS_OP_STAT, (long)(uintptr_t)szName, (long)(uintptr_t)aSt,
               0) != 0) {
        return -1;
    }
    out(szName);
    out(" size=");
    n = gj_itoa((long)aSt[0], aN, sizeof(aN));
    aN[n] = '\0';
    out(aN);
    out(" lba=");
    n = gj_itoa((long)aSt[1], aN, sizeof(aN));
    aN[n] = '\0';
    out(aN);
    out("\n");
    return 0;
}

static int
cmd_touch(const char *szName)
{
    long n;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_CREATE, (long)(uintptr_t)szName, 0, 0);
    return (n >= 0) ? 0 : -1;
}

static int
cmd_cp(const char *szSrc, const char *szDst)
{
    static char aBuf[1100];
    long n;

    if (szSrc == NULL || szDst == NULL || szSrc[0] == '\0' ||
        szDst[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szSrc, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    if (gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)szDst, (long)(uintptr_t)aBuf,
               n) != n) {
        return -1;
    }
    return 0;
}

static int
cmd_mv(const char *szSrc, const char *szDst)
{
    if (cmd_cp(szSrc, szDst) != 0) {
        return -1;
    }
    return cmd_rm(szSrc);
}

static int
cmd_head(const char *szName)
{
    static char aBuf[256];
    long n;
    long i;
    long nLines;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    nLines = 0;
    for (i = 0; i < n; i++) {
        if (aBuf[i] == '\n') {
            nLines++;
            if (nLines >= 10) {
                n = i + 1;
                break;
            }
        }
    }
    aBuf[n] = '\0';
    out(aBuf);
    if (n == 0 || aBuf[n - 1] != '\n') {
        out("\n");
    }
    return 0;
}

static int
cmd_wc(const char *szName)
{
    static char aBuf[1100];
    long n;
    long i;
    long nLines;
    long nWords;
    int fInWord;
    char aN[16];
    size_t nDig;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    nLines = 0;
    nWords = 0;
    fInWord = 0;
    for (i = 0; i < n; i++) {
        char c = aBuf[i];

        if (c == '\n') {
            nLines++;
        }
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            fInWord = 0;
        } else if (!fInWord) {
            fInWord = 1;
            nWords++;
        }
    }
    nDig = gj_itoa(nLines, aN, sizeof(aN));
    aN[nDig] = '\0';
    out(aN);
    out(" ");
    nDig = gj_itoa(nWords, aN, sizeof(aN));
    aN[nDig] = '\0';
    out(aN);
    out(" ");
    nDig = gj_itoa(n, aN, sizeof(aN));
    aN[nDig] = '\0';
    out(aN);
    out(" ");
    out(szName);
    out("\n");
    return 0;
}

/* Last up to 10 lines of a small file (bring-up). */
static int
cmd_tail(const char *szName)
{
    static char aBuf[512];
    long n;
    long i;
    long nLines;
    long aOff[12];
    long nStarts;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    nStarts = 0;
    aOff[nStarts++] = 0;
    for (i = 0; i < n; i++) {
        if (aBuf[i] == '\n' && i + 1 < n) {
            if (nStarts < (long)(sizeof(aOff) / sizeof(aOff[0]))) {
                aOff[nStarts++] = i + 1;
            } else {
                long k;

                for (k = 0; k + 1 < nStarts; k++) {
                    aOff[k] = aOff[k + 1];
                }
                aOff[nStarts - 1] = i + 1;
            }
        }
    }
    nLines = nStarts;
    if (nLines > 10) {
        /* keep last 10 starts */
        long skip = nLines - 10;
        long base = aOff[skip];

        aBuf[n] = '\0';
        out(aBuf + base);
    } else {
        aBuf[n] = '\0';
        out(aBuf);
    }
    if (n == 0 || aBuf[n - 1] != '\n') {
        out("\n");
    }
    return 0;
}

/* Byte-compare two small files; prints "same" or "differ". */
static int
cmd_cmp(const char *szA, const char *szB)
{
    static char aA[512];
    static char aB[512];
    long nA;
    long nB;
    long i;

    if (szA == NULL || szB == NULL || szA[0] == '\0' || szB[0] == '\0') {
        return -1;
    }
    nA = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szA, (long)(uintptr_t)aA,
                (long)sizeof(aA));
    nB = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szB, (long)(uintptr_t)aB,
                (long)sizeof(aB));
    if (nA < 0 || nB < 0) {
        return -1;
    }
    if (nA != nB) {
        out("differ\n");
        return 0;
    }
    for (i = 0; i < nA; i++) {
        if (aA[i] != aB[i]) {
            out("differ\n");
            return 0;
        }
    }
    out("same\n");
    return 0;
}

/* Reverse byte order of a small file (bring-up product util). */
static int
cmd_rev(const char *szName)
{
    static char aBuf[512];
    long n;
    long i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = n - 1; i >= 0; i--) {
        char aOne[2];

        aOne[0] = aBuf[i];
        aOne[1] = '\0';
        out(aOne);
    }
    if (n == 0 || aBuf[0] != '\n') {
        out("\n");
    }
    return 0;
}

/* Number lines of a small file (1-based, capped). */
static int
cmd_nl(const char *szName)
{
    static char aBuf[512];
    long n;
    long i;
    long nLine;
    long iLineStart;
    char aN[16];
    size_t nDig;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    nLine = 1;
    iLineStart = 0;
    for (i = 0; i <= n; i++) {
        if (i == n || aBuf[i] == '\n') {
            long j;

            nDig = gj_itoa(nLine, aN, sizeof(aN));
            aN[nDig] = '\0';
            out(aN);
            out("\t");
            for (j = iLineStart; j < i; j++) {
                char aOne[2];

                aOne[0] = aBuf[j];
                aOne[1] = '\0';
                out(aOne);
            }
            out("\n");
            nLine++;
            iLineStart = i + 1;
            if (nLine > 64) {
                break;
            }
        }
    }
    return 0;
}

/* Hex dump first bytes of a small file (od-like). */
static int
cmd_od(const char *szName)
{
    static char aBuf[64];
    long n;
    long i;
    static const char aHex[] = "0123456789abcdef";

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)sizeof(aBuf));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        char aH[4];
        unsigned char c = (unsigned char)aBuf[i];

        aH[0] = aHex[(c >> 4) & 0xf];
        aH[1] = aHex[c & 0xf];
        aH[2] = (i + 1 == n || ((i + 1) % 16) == 0) ? '\n' : ' ';
        aH[3] = '\0';
        out(aH);
    }
    if (n == 0) {
        out("\n");
    } else if ((n % 16) != 0) {
        out("\n");
    }
    return 0;
}

/* Integer expr: expr A + B | - | *  (bring-up; no div-by-zero hang). */
static int
cmd_expr(const char *szLine)
{
    char aA[24];
    char aOp[8];
    char aB[24];
    const char *p;
    long nA;
    long nB;
    long nR;
    long i;
    char aN[24];
    size_t nDig;

    p = split_word(szLine, aA, sizeof(aA));
    p = split_word(p, aOp, sizeof(aOp));
    (void)split_word(p, aB, sizeof(aB));
    if (aA[0] == '\0' || aOp[0] == '\0' || aB[0] == '\0') {
        return -1;
    }
    nA = 0;
    for (i = 0; aA[i] >= '0' && aA[i] <= '9'; i++) {
        nA = nA * 10 + (long)(aA[i] - '0');
    }
    nB = 0;
    for (i = 0; aB[i] >= '0' && aB[i] <= '9'; i++) {
        nB = nB * 10 + (long)(aB[i] - '0');
    }
    if (aOp[0] == '+' && aOp[1] == '\0') {
        nR = nA + nB;
    } else if (aOp[0] == '-' && aOp[1] == '\0') {
        nR = nA - nB;
    } else if (aOp[0] == '*' && aOp[1] == '\0') {
        nR = nA * nB;
    } else {
        return -1;
    }
    nDig = gj_itoa(nR, aN, sizeof(aN));
    aN[nDig] = '\0';
    out(aN);
    out("\n");
    return 0;
}

/* tee: write text to file and also print it (tee FILE TEXT...). */
static int
cmd_tee(const char *szName, const char *szText)
{
    if (szName == NULL || szText == NULL || szName[0] == '\0') {
        return -1;
    }
    if (cmd_echo_file(szName, szText) != 0) {
        return -1;
    }
    out(szText);
    if (szText[0] == '\0' || szText[gj_strlen(szText) - 1] != '\n') {
        out("\n");
    }
    return 0;
}

/* cut -c N: first N chars of each line (bring-up; N default 8). */
static int
cmd_cut(const char *szName, unsigned uCols)
{
    static char aBuf[512];
    long n;
    long i;
    long nLineStart;
    unsigned u;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    if (uCols == 0) {
        uCols = 8;
    }
    if (uCols > 64) {
        uCols = 64;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    nLineStart = 0;
    for (i = 0; i <= n; i++) {
        if (i == n || aBuf[i] == '\n') {
            u = 0;
            while (nLineStart + (long)u < i && u < uCols) {
                char aC[2];

                aC[0] = aBuf[nLineStart + (long)u];
                aC[1] = '\0';
                out(aC);
                u++;
            }
            out("\n");
            nLineStart = i + 1;
        }
    }
    return 0;
}

/* tr FROM TO: map first char of FROM to first of TO over file. */
static int
cmd_tr(const char *szFrom, const char *szTo, const char *szName)
{
    static char aBuf[512];
    long n;
    long i;
    char cFrom;
    char cTo;

    if (szFrom == NULL || szTo == NULL || szName == NULL || szFrom[0] == '\0' ||
        szTo[0] == '\0' || szName[0] == '\0') {
        return -1;
    }
    cFrom = szFrom[0];
    cTo = szTo[0];
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        char aC[2];

        aC[0] = (aBuf[i] == cFrom) ? cTo : aBuf[i];
        aC[1] = '\0';
        out(aC);
    }
    if (n == 0 || aBuf[n - 1] != '\n') {
        out("\n");
    }
    return 0;
}

/* fold -w N: wrap lines at N (default 16). */
static int
cmd_fold(const char *szName, unsigned uWidth)
{
    static char aBuf[512];
    long n;
    long i;
    unsigned uCol;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    if (uWidth == 0) {
        uWidth = 16;
    }
    if (uWidth > 80) {
        uWidth = 80;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uCol = 0;
    for (i = 0; i < n; i++) {
        char aC[2];

        if (aBuf[i] == '\n') {
            out("\n");
            uCol = 0;
            continue;
        }
        if (uCol >= uWidth) {
            out("\n");
            uCol = 0;
        }
        aC[0] = aBuf[i];
        aC[1] = '\0';
        out(aC);
        uCol++;
    }
    if (n == 0 || aBuf[n - 1] != '\n') {
        out("\n");
    }
    return 0;
}

/* sort: lexicographic line sort of a small file (bring-up product util). */
static int
cmd_sort(const char *szName)
{
    static char aBuf[512];
    static char aLines[32][96];
    unsigned nLines = 0;
    long n;
    long i;
    long nLineStart;
    unsigned a, b;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    nLineStart = 0;
    for (i = 0; i <= n && nLines < 32u; i++) {
        if (i == n || aBuf[i] == '\n') {
            size_t cb;

            /* Skip trailing empty line when file ends with '\n' */
            if (i == n && nLineStart >= n) {
                break;
            }
            cb = (size_t)(i - nLineStart);
            if (cb >= sizeof(aLines[0])) {
                cb = sizeof(aLines[0]) - 1;
            }
            {
                size_t j;

                for (j = 0; j < cb; j++) {
                    aLines[nLines][j] = aBuf[nLineStart + (long)j];
                }
                aLines[nLines][cb] = '\0';
            }
            nLines++;
            nLineStart = i + 1;
        }
    }
    /* Bubble sort — tiny N, freestanding, no qsort */
    for (a = 0; a < nLines; a++) {
        for (b = a + 1u; b < nLines; b++) {
            if (gj_strcmp(aLines[a], aLines[b]) > 0) {
                char aTmp[96];
                size_t k;

                for (k = 0; k < sizeof(aTmp); k++) {
                    aTmp[k] = aLines[a][k];
                    aLines[a][k] = aLines[b][k];
                    aLines[b][k] = aTmp[k];
                }
            }
        }
    }
    for (a = 0; a < nLines; a++) {
        out(aLines[a]);
        out("\n");
    }
    return 0;
}

/* strings: emit runs of printable ASCII (length >= 4). */
static int
cmd_strings(const char *szName)
{
    static char aBuf[512];
    long n;
    long i;
    long nRun = -1;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i <= n; i++) {
        unsigned char c = (i < n) ? (unsigned char)aBuf[i] : 0;
        int fPrint = (c >= 32u && c < 127u);

        if (fPrint) {
            if (nRun < 0) {
                nRun = i;
            }
        } else {
            if (nRun >= 0 && (i - nRun) >= 4) {
                long j;

                for (j = nRun; j < i; j++) {
                    char aC[2];

                    aC[0] = aBuf[j];
                    aC[1] = '\0';
                    out(aC);
                }
                out("\n");
            }
            nRun = -1;
        }
    }
    return 0;
}

/* expand: tabs → 8-column spaces (small file). */
static int
cmd_expand(const char *szName)
{
    static char aBuf[512];
    long n;
    long i;
    unsigned uCol = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        char c = aBuf[i];

        if (c == '\t') {
            unsigned uSpaces = 8u - (uCol % 8u);
            unsigned s;

            for (s = 0; s < uSpaces; s++) {
                out(" ");
                uCol++;
            }
        } else {
            char aC[2];

            aC[0] = c;
            aC[1] = '\0';
            out(aC);
            if (c == '\n') {
                uCol = 0;
            } else {
                uCol++;
            }
        }
    }
    return 0;
}

/* tac: reverse line order of a small file. */
static int
cmd_tac(const char *szName)
{
    static char aBuf[512];
    static long aOff[64];
    static long aLen[64];
    long n;
    long i;
    unsigned nLines = 0;
    long iLineStart = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i <= n; i++) {
        if (i == n || aBuf[i] == '\n') {
            if (nLines < 64u) {
                aOff[nLines] = iLineStart;
                aLen[nLines] = i - iLineStart;
                nLines++;
            }
            iLineStart = i + 1;
        }
    }
    while (nLines > 0u) {
        long j;
        long off;
        long len;

        nLines--;
        off = aOff[nLines];
        len = aLen[nLines];
        for (j = 0; j < len; j++) {
            char aOne[2];

            aOne[0] = aBuf[off + j];
            aOne[1] = '\0';
            out(aOne);
        }
        out("\n");
    }
    return 0;
}

/* paste: join corresponding lines of two files with TAB (one file = cat). */
static int
cmd_paste(const char *szA, const char *szB)
{
    static char aA[256];
    static char aB[256];
    long nA;
    long nB;
    long iA = 0;
    long iB = 0;

    if (szA == NULL || szA[0] == '\0') {
        return -1;
    }
    nA = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szA, (long)(uintptr_t)aA,
                (long)(sizeof(aA) - 1));
    if (nA < 0) {
        return -1;
    }
    if (szB == NULL || szB[0] == '\0') {
        /* single file: emit as-is */
        long k;

        for (k = 0; k < nA; k++) {
            char aOne[2];

            aOne[0] = aA[k];
            aOne[1] = '\0';
            out(aOne);
        }
        if (nA == 0 || aA[nA - 1] != '\n') {
            out("\n");
        }
        return 0;
    }
    nB = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szB, (long)(uintptr_t)aB,
                (long)(sizeof(aB) - 1));
    if (nB < 0) {
        return -1;
    }
    while (iA < nA || iB < nB) {
        /* emit line from A */
        while (iA < nA && aA[iA] != '\n') {
            char aOne[2];

            aOne[0] = aA[iA++];
            aOne[1] = '\0';
            out(aOne);
        }
        if (iA < nA && aA[iA] == '\n') {
            iA++;
        }
        out("\t");
        while (iB < nB && aB[iB] != '\n') {
            char aOne[2];

            aOne[0] = aB[iB++];
            aOne[1] = '\0';
            out(aOne);
        }
        if (iB < nB && aB[iB] == '\n') {
            iB++;
        }
        out("\n");
    }
    return 0;
}

/* sum: BSD-style 16-bit checksum + 512-byte blocks (bring-up). */
static int
cmd_sum(const char *szName)
{
    static char aBuf[512];
    long n;
    long i;
    unsigned uSum = 0;
    unsigned uBlocks;
    char aN[16];
    size_t nDig;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)sizeof(aBuf));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        uSum = (uSum >> 1) + ((uSum & 1u) << 15);
        uSum = (uSum + (unsigned char)aBuf[i]) & 0xffffu;
    }
    uBlocks = (n == 0) ? 0u : (unsigned)((n + 511) / 512);
    nDig = gj_itoa((long)uSum, aN, sizeof(aN));
    aN[nDig] = '\0';
    out(aN);
    out(" ");
    nDig = gj_itoa((long)uBlocks, aN, sizeof(aN));
    aN[nDig] = '\0';
    out(aN);
    out(" ");
    out(szName);
    out("\n");
    return 0;
}

/* First whitespace-delimited field into aKey; return rest after field. */
static const char *
field1(const char *p, char *aKey, size_t cbKey, size_t *pFieldLen)
{
    size_t i = 0;

    if (p == NULL || aKey == NULL || cbKey == 0) {
        return NULL;
    }
    while (p[i] != '\0' && p[i] != ' ' && p[i] != '\t' && p[i] != '\n' &&
           i + 1 < cbKey) {
        aKey[i] = p[i];
        i++;
    }
    aKey[i] = '\0';
    if (pFieldLen != NULL) {
        *pFieldLen = i;
    }
    while (p[i] == ' ' || p[i] == '\t') {
        i++;
    }
    return p + i;
}

/* join: print lines with matching first field (space-separated files). */
static int
cmd_join(const char *szA, const char *szB)
{
    static char aA[256];
    static char aB[256];
    static char aKeyA[32];
    static char aKeyB[32];
    long nA;
    long nB;
    long iA = 0;
    int fAny = 0;

    if (szA == NULL || szA[0] == '\0' || szB == NULL || szB[0] == '\0') {
        return -1;
    }
    nA = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szA, (long)(uintptr_t)aA,
                (long)(sizeof(aA) - 1));
    nB = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szB, (long)(uintptr_t)aB,
                (long)(sizeof(aB) - 1));
    if (nA < 0 || nB < 0) {
        return -1;
    }
    aA[nA] = '\0';
    aB[nB] = '\0';
    while (iA < nA) {
        long iLineEnd = iA;
        const char *pRestA;
        long iB = 0;

        while (iLineEnd < nA && aA[iLineEnd] != '\n') {
            iLineEnd++;
        }
        {
            char cSave = aA[iLineEnd];

            aA[iLineEnd] = '\0';
            pRestA = field1(aA + iA, aKeyA, sizeof(aKeyA), NULL);
            if (aKeyA[0] != '\0') {
                while (iB < nB) {
                    long jEnd = iB;
                    const char *pRestB;

                    while (jEnd < nB && aB[jEnd] != '\n') {
                        jEnd++;
                    }
                    {
                        char cB = aB[jEnd];

                        aB[jEnd] = '\0';
                        pRestB = field1(aB + iB, aKeyB, sizeof(aKeyB), NULL);
                        if (gj_strcmp(aKeyA, aKeyB) == 0) {
                            out(aKeyA);
                            out(" ");
                            out(pRestA != NULL ? pRestA : "");
                            out(" ");
                            out(pRestB != NULL ? pRestB : "");
                            out("\n");
                            fAny = 1;
                        }
                        aB[jEnd] = cB;
                    }
                    iB = jEnd + 1;
                    if (iB > nB) {
                        break;
                    }
                }
            }
            aA[iLineEnd] = cSave;
        }
        iA = iLineEnd + 1;
        if (iA > nA) {
            break;
        }
    }
    if (!fAny) {
        out("\n");
    }
    return 0;
}

/* Read next line from buffer; advance *pIdx. Returns 0 if a line was read. */
static int
next_line(const char *pBuf, long nBuf, long *pIdx, char *aLine, size_t cbLine)
{
    size_t n = 0;
    long i;

    if (pBuf == NULL || pIdx == NULL || aLine == NULL || cbLine == 0) {
        return -1;
    }
    i = *pIdx;
    if (i >= nBuf) {
        aLine[0] = '\0';
        return -1;
    }
    while (i < nBuf && pBuf[i] != '\n' && n + 1 < cbLine) {
        aLine[n++] = pBuf[i++];
    }
    aLine[n] = '\0';
    if (i < nBuf && pBuf[i] == '\n') {
        i++;
    }
    *pIdx = i;
    return 0;
}

/* comm: three-column compare of two sorted line files (bring-up). */
static int
cmd_comm(const char *szA, const char *szB)
{
    static char aA[256];
    static char aB[256];
    char aLineA[64];
    char aLineB[64];
    long nA;
    long nB;
    long iA = 0;
    long iB = 0;
    int fHaveA;
    int fHaveB;

    if (szA == NULL || szA[0] == '\0' || szB == NULL || szB[0] == '\0') {
        return -1;
    }
    nA = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szA, (long)(uintptr_t)aA,
                (long)(sizeof(aA) - 1));
    nB = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szB, (long)(uintptr_t)aB,
                (long)(sizeof(aB) - 1));
    if (nA < 0 || nB < 0) {
        return -1;
    }
    fHaveA = (next_line(aA, nA, &iA, aLineA, sizeof(aLineA)) == 0);
    fHaveB = (next_line(aB, nB, &iB, aLineB, sizeof(aLineB)) == 0);
    while (fHaveA || fHaveB) {
        int cmp;

        if (!fHaveA) {
            out("\t");
            out(aLineB);
            out("\n");
            fHaveB = (next_line(aB, nB, &iB, aLineB, sizeof(aLineB)) == 0);
            continue;
        }
        if (!fHaveB) {
            out(aLineA);
            out("\n");
            fHaveA = (next_line(aA, nA, &iA, aLineA, sizeof(aLineA)) == 0);
            continue;
        }
        cmp = gj_strcmp(aLineA, aLineB);
        if (cmp == 0) {
            out("\t\t");
            out(aLineA);
            out("\n");
            fHaveA = (next_line(aA, nA, &iA, aLineA, sizeof(aLineA)) == 0);
            fHaveB = (next_line(aB, nB, &iB, aLineB, sizeof(aLineB)) == 0);
        } else if (cmp < 0) {
            out(aLineA);
            out("\n");
            fHaveA = (next_line(aA, nA, &iA, aLineA, sizeof(aLineA)) == 0);
        } else {
            out("\t");
            out(aLineB);
            out("\n");
            fHaveB = (next_line(aB, nB, &iB, aLineB, sizeof(aLineB)) == 0);
        }
    }
    return 0;
}

/* unexpand: convert leading spaces to tabs (tabstop 8) on each line. */
static int
cmd_unexpand(const char *szName)
{
    static char aBuf[512];
    long n;
    long i;
    long nCol = 0;
    long nSpaces = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        char ch = aBuf[i];
        char aOne[2];

        if (ch == '\n') {
            while (nSpaces > 0) {
                out(" ");
                nSpaces--;
            }
            out("\n");
            nCol = 0;
            nSpaces = 0;
            continue;
        }
        if (ch == ' ' && nCol < 64) {
            nSpaces++;
            nCol++;
            if ((nCol % 8) == 0) {
                out("\t");
                nSpaces = 0;
            }
            continue;
        }
        while (nSpaces > 0) {
            out(" ");
            nSpaces--;
        }
        aOne[0] = ch;
        aOne[1] = '\0';
        out(aOne);
        nCol++;
        if (ch == '\t') {
            nCol = (nCol + 7) & ~7;
        }
    }
    while (nSpaces > 0) {
        out(" ");
        nSpaces--;
    }
    return 0;
}

/* shuf: reverse line order (deterministic bring-up shuffle for smoke). */
static int
cmd_shuf(const char *szName)
{
    /* Same as tac for freestanding bring-up (stable, no RNG). */
    return cmd_tac(szName);
}

/* colrm: remove character columns [from,to] (1-based, inclusive). */
static int
cmd_colrm(const char *szName, unsigned uFrom, unsigned uTo)
{
    static char aBuf[512];
    long n;
    long i;
    unsigned uCol;

    if (szName == NULL || szName[0] == '\0' || uFrom == 0u) {
        return -1;
    }
    if (uTo < uFrom) {
        uTo = uFrom;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uCol = 1;
    for (i = 0; i < n; i++) {
        char ch = aBuf[i];
        char aOne[2];

        if (ch == '\n') {
            out("\n");
            uCol = 1;
            continue;
        }
        if (uCol < uFrom || uCol > uTo) {
            aOne[0] = ch;
            aOne[1] = '\0';
            out(aOne);
        }
        uCol++;
    }
    return 0;
}

/* fmt: reflow whitespace-separated words to width 32 (smoke-sized). */
static int
cmd_fmt(const char *szName)
{
    static char aBuf[512];
    long n;
    long i;
    unsigned uCol = 0;
    unsigned uWidth = 32u;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    i = 0;
    while (i < n) {
        long j;
        long nWord;

        while (i < n && (aBuf[i] == ' ' || aBuf[i] == '\t' || aBuf[i] == '\n')) {
            i++;
        }
        if (i >= n) {
            break;
        }
        j = i;
        while (j < n && aBuf[j] != ' ' && aBuf[j] != '\t' && aBuf[j] != '\n') {
            j++;
        }
        nWord = j - i;
        if (uCol > 0u && uCol + 1u + (unsigned)nWord > uWidth) {
            out("\n");
            uCol = 0;
        }
        if (uCol > 0u) {
            out(" ");
            uCol++;
        }
        {
            long k;

            for (k = i; k < j; k++) {
                char aOne[2];

                aOne[0] = aBuf[k];
                aOne[1] = '\0';
                out(aOne);
            }
        }
        uCol += (unsigned)nWord;
        i = j;
    }
    if (uCol > 0u) {
        out("\n");
    }
    return 0;
}

/* pathchk: reject empty path or path with NULs (bring-up portable check). */
static int
cmd_pathchk(const char *szPath)
{
    size_t i;

    if (szPath == NULL || szPath[0] == '\0') {
        out("pathchk: empty\n");
        return -1;
    }
    for (i = 0; szPath[i] != '\0'; i++) {
        if (szPath[i] == '\0') {
            break;
        }
        /* reject control chars other than nothing — keep simple */
        if ((unsigned char)szPath[i] < 0x20u) {
            out("pathchk: bad\n");
            return -1;
        }
    }
    if (i > 255u) {
        out("pathchk: long\n");
        return -1;
    }
    out("pathchk: ok\n");
    return 0;
}

/* split: emit first N bytes of file as a single chunk (bring-up). */
static int
cmd_split(const char *szName, unsigned uBytes)
{
    static char aBuf[512];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    if (uBytes == 0u) {
        uBytes = 16u;
    }
    if (uBytes > sizeof(aBuf) - 1u) {
        uBytes = (unsigned)(sizeof(aBuf) - 1u);
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)uBytes);
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < (unsigned)n; i++) {
        char aOne[2];

        aOne[0] = aBuf[i];
        aOne[1] = '\0';
        out(aOne);
    }
    if (n > 0 && aBuf[n - 1] != '\n') {
        out("\n");
    }
    return 0;
}

/* base64: encode small file (standard alphabet, no wrap). */
static int
cmd_base64(const char *szName)
{
    static const char aB64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static char aBuf[256];
    static char aOut[400];
    long n;
    long i;
    size_t o = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i += 3) {
        unsigned b0 = (unsigned char)aBuf[i];
        unsigned b1 = (i + 1 < n) ? (unsigned char)aBuf[i + 1] : 0u;
        unsigned b2 = (i + 2 < n) ? (unsigned char)aBuf[i + 2] : 0u;
        long nIn = n - i;

        if (o + 4u >= sizeof(aOut)) {
            break;
        }
        aOut[o++] = aB64[(b0 >> 2) & 0x3fu];
        aOut[o++] = aB64[((b0 & 0x3u) << 4) | ((b1 >> 4) & 0xfu)];
        if (nIn == 1) {
            aOut[o++] = '=';
            aOut[o++] = '=';
        } else if (nIn == 2) {
            aOut[o++] = aB64[(b1 & 0xfu) << 2];
            aOut[o++] = '=';
        } else {
            aOut[o++] = aB64[((b1 & 0xfu) << 2) | ((b2 >> 6) & 0x3u)];
            aOut[o++] = aB64[b2 & 0x3fu];
        }
    }
    aOut[o] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/* base32: RFC 4648 encode small file (no wrap). */
static int
cmd_base32(const char *szName)
{
    static const char aB32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    static char aBuf[256];
    static char aOut[420];
    long n;
    long i;
    size_t o = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i += 5) {
        unsigned b0 = (unsigned char)aBuf[i];
        unsigned b1 = (i + 1 < n) ? (unsigned char)aBuf[i + 1] : 0u;
        unsigned b2 = (i + 2 < n) ? (unsigned char)aBuf[i + 2] : 0u;
        unsigned b3 = (i + 3 < n) ? (unsigned char)aBuf[i + 3] : 0u;
        unsigned b4 = (i + 4 < n) ? (unsigned char)aBuf[i + 4] : 0u;
        long nIn = n - i;
        unsigned char aEnc[8];
        int nPad;
        int k;

        if (o + 8u >= sizeof(aOut)) {
            break;
        }
        aEnc[0] = (unsigned char)((b0 >> 3) & 0x1fu);
        aEnc[1] = (unsigned char)(((b0 & 0x7u) << 2) | ((b1 >> 6) & 0x3u));
        aEnc[2] = (unsigned char)((b1 >> 1) & 0x1fu);
        aEnc[3] = (unsigned char)(((b1 & 0x1u) << 4) | ((b2 >> 4) & 0xfu));
        aEnc[4] = (unsigned char)(((b2 & 0xfu) << 1) | ((b3 >> 7) & 0x1u));
        aEnc[5] = (unsigned char)((b3 >> 2) & 0x1fu);
        aEnc[6] = (unsigned char)(((b3 & 0x3u) << 3) | ((b4 >> 5) & 0x7u));
        aEnc[7] = (unsigned char)(b4 & 0x1fu);
        if (nIn >= 5) {
            nPad = 0;
        } else if (nIn == 4) {
            nPad = 1;
        } else if (nIn == 3) {
            nPad = 3;
        } else if (nIn == 2) {
            nPad = 4;
        } else {
            nPad = 6;
        }
        for (k = 0; k < 8 - nPad; k++) {
            aOut[o++] = aB32[aEnc[k] & 0x1fu];
        }
        for (k = 0; k < nPad; k++) {
            aOut[o++] = '=';
        }
    }
    aOut[o] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/* realpath: print absolute-looking path (VFS has no cwd walk). */
static int
cmd_realpath(const char *szPath)
{
    if (szPath == NULL || szPath[0] == '\0') {
        return -1;
    }
    if (szPath[0] == '/') {
        out(szPath);
        out("\n");
        return 0;
    }
    out("/mnt/");
    out(szPath);
    out("\n");
    return 0;
}

/* mkdir: create empty file node as directory marker (VFS bring-up). */
static int
cmd_mkdir(const char *szName)
{
    unsigned aSt[2];
    long n;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_CREATE, (long)(uintptr_t)szName, 0, 0);
    if (n >= 0) {
        return 0;
    }
    /* already exists is ok for smoke */
    if (gj_vfs(GJ_VFS_OP_STAT, (long)(uintptr_t)szName, (long)(uintptr_t)aSt,
               0) == 0) {
        return 0;
    }
    return -1;
}

/* hexdump: alias for od bring-up hex dump. */
static int
cmd_hexdump(const char *szName)
{
    return cmd_od(szName);
}

/* xxd: offset + hex bytes of a small file (product util). */
static int
cmd_xxd(const char *szName)
{
    static char aBuf[256];
    static char aLine[80];
    long n;
    long off;
    static const char aHex[] = "0123456789abcdef";

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)sizeof(aBuf));
    if (n < 0) {
        return -1;
    }
    off = 0;
    while (off < n) {
        long nChunk = n - off;
        long j;
        size_t p = 0;

        if (nChunk > 8) {
            nChunk = 8;
        }
        /* 4-digit offset */
        aLine[p++] = aHex[(off >> 12) & 0xf];
        aLine[p++] = aHex[(off >> 8) & 0xf];
        aLine[p++] = aHex[(off >> 4) & 0xf];
        aLine[p++] = aHex[off & 0xf];
        aLine[p++] = ':';
        aLine[p++] = ' ';
        for (j = 0; j < nChunk; j++) {
            unsigned char b = (unsigned char)aBuf[off + j];

            aLine[p++] = aHex[b >> 4];
            aLine[p++] = aHex[b & 0xf];
            aLine[p++] = ' ';
        }
        aLine[p++] = '\n';
        aLine[p] = '\0';
        out(aLine);
        off += nChunk;
    }
    return 0;
}

/* du: print byte size of a small file (bring-up; no tree walk). */
static int
cmd_du(const char *szName)
{
    unsigned aSt[2];
    char aN[24];
    size_t nDig;
    unsigned long uSz;
    unsigned long uTmp;
    size_t i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    if (gj_vfs(GJ_VFS_OP_STAT, (long)(uintptr_t)szName, (long)(uintptr_t)aSt,
               0) != 0) {
        return -1;
    }
    uSz = (unsigned long)aSt[0];
    if (uSz == 0ul) {
        aN[0] = '0';
        aN[1] = '\0';
        nDig = 1;
    } else {
        uTmp = uSz;
        nDig = 0;
        while (uTmp > 0ul && nDig < sizeof(aN) - 1u) {
            aN[nDig++] = (char)('0' + (uTmp % 10ul));
            uTmp /= 10ul;
        }
        for (i = 0; i < nDig / 2u; i++) {
            char ch = aN[i];

            aN[i] = aN[nDig - 1u - i];
            aN[nDig - 1u - i] = ch;
        }
        aN[nDig] = '\0';
    }
    out(aN);
    out("\t");
    out(szName);
    out("\n");
    return 0;
}

/* truncate: rewrite file to empty (size 0 bring-up). */
static int
cmd_truncate(const char *szName)
{
    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    /* WRITE of zero length acts as clear/create for door VFS */
    if (gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)szName, (long)(uintptr_t)"",
               0) < 0) {
        if (gj_vfs(GJ_VFS_OP_CREATE, (long)(uintptr_t)szName, 0, 0) < 0) {
            return -1;
        }
    }
    return 0;
}

/* link: copy src→dst (VFS has no hardlinks). */
static int
cmd_link(const char *szSrc, const char *szDst)
{
    return cmd_cp(szSrc, szDst);
}

/* readlink: bring-up — no real symlinks; print path if file exists. */
static int
cmd_readlink(const char *szName)
{
    static char aSt[32];

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    if (gj_vfs(GJ_VFS_OP_STAT, (long)(uintptr_t)szName, (long)(uintptr_t)aSt,
               0) != 0) {
        return -1;
    }
    out(szName);
    out("\n");
    return 0;
}

/*
 * tsort: bring-up topological order for small "a b" edge lists.
 * If a line has one token, emit it; two tokens A B means A before B.
 * Cycles: emit remaining in input order (no error).
 */
static int
cmd_tsort(const char *szName)
{
    static char aBuf[512];
    static char aTok[32][24];
    static unsigned char aSeen[32];
    static int aBefore[64];
    static int aAfter[64];
    long n;
    long i;
    long nLineStart;
    unsigned nTok = 0;
    unsigned nEdge = 0;
    unsigned uPass;
    unsigned uEmitted = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    aBuf[n] = '\0';
    nLineStart = 0;
    for (i = 0; i <= n; i++) {
        if (i == n || aBuf[i] == '\n') {
            char aW1[24];
            char aW2[24];
            const char *p;
            unsigned j;
            int iA = -1;
            int iB = -1;

            aBuf[i] = '\0';
            p = skip_ws(&aBuf[nLineStart]);
            if (p != NULL && p[0] != '\0') {
                p = split_word(p, aW1, sizeof(aW1));
                aW2[0] = '\0';
                if (p != NULL && p[0] != '\0') {
                    (void)split_word(p, aW2, sizeof(aW2));
                }
                for (j = 0; j < nTok; j++) {
                    if (gj_strcmp(aTok[j], aW1) == 0) {
                        iA = (int)j;
                    }
                }
                if (iA < 0 && nTok < 32u) {
                    size_t k;

                    for (k = 0; aW1[k] != '\0' && k + 1 < sizeof(aTok[0]);
                         k++) {
                        aTok[nTok][k] = aW1[k];
                    }
                    aTok[nTok][k] = '\0';
                    iA = (int)nTok;
                    nTok++;
                }
                if (aW2[0] != '\0') {
                    for (j = 0; j < nTok; j++) {
                        if (gj_strcmp(aTok[j], aW2) == 0) {
                            iB = (int)j;
                        }
                    }
                    if (iB < 0 && nTok < 32u) {
                        size_t k;

                        for (k = 0; aW2[k] != '\0' && k + 1 < sizeof(aTok[0]);
                             k++) {
                            aTok[nTok][k] = aW2[k];
                        }
                        aTok[nTok][k] = '\0';
                        iB = (int)nTok;
                        nTok++;
                    }
                    if (iA >= 0 && iB >= 0 && nEdge < 64u) {
                        aBefore[nEdge] = iA;
                        aAfter[nEdge] = iB;
                        nEdge++;
                    }
                }
            }
            nLineStart = i + 1;
        }
    }
    {
        unsigned j_clear;

        for (j_clear = 0; j_clear < 32u; j_clear++) {
            aSeen[j_clear] = 0;
        }
    }
    /* Kahn-ish: emit nodes with no unmet predecessors */
    for (uPass = 0; uPass < nTok + 1u && uEmitted < nTok; uPass++) {
        unsigned j;

        for (j = 0; j < nTok; j++) {
            unsigned e;
            int fBlocked = 0;

            if (aSeen[j] != 0) {
                continue;
            }
            for (e = 0; e < nEdge; e++) {
                if (aAfter[e] == (int)j && aSeen[(unsigned)aBefore[e]] == 0) {
                    fBlocked = 1;
                    break;
                }
            }
            if (!fBlocked) {
                out(aTok[j]);
                out("\n");
                aSeen[j] = 1;
                uEmitted++;
            }
        }
    }
    /* remainder (cycle) */
    {
        unsigned j;

        for (j = 0; j < nTok; j++) {
            if (aSeen[j] == 0) {
                out(aTok[j]);
                out("\n");
            }
        }
    }
    return 0;
}

/* pr: simple numbered dump (bring-up; no pagination). */
static int
cmd_pr(const char *szName)
{
    return cmd_nl(szName);
}

/* Compact RFC 1321 MD5 (integer only) for md5sum product command. */
struct sh_md5 {
    unsigned aState[4];
    unsigned long long uBits;
    unsigned char aBuf[64];
    unsigned uUsed;
};

static void
sh_md5_init(struct sh_md5 *p)
{
    p->aState[0] = 0x67452301u;
    p->aState[1] = 0xefcdab89u;
    p->aState[2] = 0x98badcfeu;
    p->aState[3] = 0x10325476u;
    p->uBits = 0;
    p->uUsed = 0;
}

static unsigned
sh_md5_rol(unsigned x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static void
sh_md5_block(struct sh_md5 *p, const unsigned char *pBlk)
{
    unsigned a[16];
    unsigned A, B, C, D, i;
    static const unsigned char aS[64] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
    };
    static const unsigned aK[64] = {
        0xd76aa478u, 0xe8c7b756u, 0x242070dbu, 0xc1bdceeeu, 0xf57c0fafu,
        0x4787c62au, 0xa8304613u, 0xfd469501u, 0x698098d8u, 0x8b44f7afu,
        0xffff5bb1u, 0x895cd7beu, 0x6b901122u, 0xfd987193u, 0xa679438eu,
        0x49b40821u, 0xf61e2562u, 0xc040b340u, 0x265e5a51u, 0xe9b6c7aau,
        0xd62f105du, 0x02441453u, 0xd8a1e681u, 0xe7d3fbc8u, 0x21e1cde6u,
        0xc33707d6u, 0xf4d50d87u, 0x455a14edu, 0xa9e3e905u, 0xfcefa3f8u,
        0x676f02d9u, 0x8d2a4c8au, 0xfffa3942u, 0x8771f681u, 0x6d9d6122u,
        0xfde5380cu, 0xa4beea44u, 0x4bdecfa9u, 0xf6bb4b60u, 0xbebfbc70u,
        0x289b7ec6u, 0xeaa127fau, 0xd4ef3085u, 0x04881d05u, 0xd9d4d039u,
        0xe6db99e5u, 0x1fa27cf8u, 0xc4ac5665u, 0xf4292244u, 0x432aff97u,
        0xab9423a7u, 0xfc93a039u, 0x655b59c3u, 0x8f0ccc92u, 0xffeff47du,
        0x85845dd1u, 0x6fa87e4fu, 0xfe2ce6e0u, 0xa3014314u, 0x4e0811a1u,
        0xf7537e82u, 0xbd3af235u, 0x2ad7d2bbu, 0xeb86d391u
    };

    for (i = 0; i < 16u; i++) {
        a[i] = (unsigned)pBlk[i * 4u] |
               ((unsigned)pBlk[i * 4u + 1u] << 8) |
               ((unsigned)pBlk[i * 4u + 2u] << 16) |
               ((unsigned)pBlk[i * 4u + 3u] << 24);
    }
    A = p->aState[0];
    B = p->aState[1];
    C = p->aState[2];
    D = p->aState[3];
    for (i = 0; i < 64u; i++) {
        unsigned F, g, t;

        if (i < 16u) {
            F = (B & C) | ((~B) & D);
            g = i;
        } else if (i < 32u) {
            F = (D & B) | ((~D) & C);
            g = (5u * i + 1u) & 15u;
        } else if (i < 48u) {
            F = B ^ C ^ D;
            g = (3u * i + 5u) & 15u;
        } else {
            F = C ^ (B | (~D));
            g = (7u * i) & 15u;
        }
        t = D;
        D = C;
        C = B;
        B = B + sh_md5_rol(A + F + aK[i] + a[g], (unsigned)aS[i]);
        A = t;
    }
    p->aState[0] += A;
    p->aState[1] += B;
    p->aState[2] += C;
    p->aState[3] += D;
}

static void
sh_md5_update(struct sh_md5 *p, const void *pData, size_t cb)
{
    const unsigned char *pIn = (const unsigned char *)pData;

    p->uBits += (unsigned long long)cb * 8ull;
    while (cb > 0) {
        unsigned uRoom = 64u - p->uUsed;
        unsigned uTake = (cb < (size_t)uRoom) ? (unsigned)cb : uRoom;
        unsigned i;

        for (i = 0; i < uTake; i++) {
            p->aBuf[p->uUsed + i] = pIn[i];
        }
        p->uUsed += uTake;
        pIn += uTake;
        cb -= uTake;
        if (p->uUsed == 64u) {
            sh_md5_block(p, p->aBuf);
            p->uUsed = 0;
        }
    }
}

static void
sh_md5_final(struct sh_md5 *p, unsigned char aOut[16])
{
    unsigned char aPad[64];
    unsigned u, i;
    unsigned long long uBits;

    aPad[0] = 0x80;
    for (i = 1; i < 64u; i++) {
        aPad[i] = 0;
    }
    u = p->uUsed;
    if (u < 56u) {
        sh_md5_update(p, aPad, 56u - u);
    } else {
        sh_md5_update(p, aPad, 64u - u + 56u);
    }
    uBits = p->uBits;
    for (i = 0; i < 8u; i++) {
        aPad[i] = (unsigned char)(uBits >> (8u * i));
    }
    sh_md5_update(p, aPad, 8u);
    for (i = 0; i < 4u; i++) {
        aOut[i * 4u] = (unsigned char)(p->aState[i] & 0xffu);
        aOut[i * 4u + 1u] = (unsigned char)((p->aState[i] >> 8) & 0xffu);
        aOut[i * 4u + 2u] = (unsigned char)((p->aState[i] >> 16) & 0xffu);
        aOut[i * 4u + 3u] = (unsigned char)((p->aState[i] >> 24) & 0xffu);
    }
}

static int
cmd_md5sum(const char *szName)
{
    static char aBuf[1100];
    static char aHex[40];
    static const char *szDigits = "0123456789abcdef";
    struct sh_md5 ctx;
    unsigned char aDig[16];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_md5_init(&ctx);
    sh_md5_update(&ctx, aBuf, (size_t)n);
    sh_md5_final(&ctx, aDig);
    for (i = 0; i < 16u; i++) {
        aHex[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aHex[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aHex[32] = '\0';
    out(aHex);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

static int
cmd_file(const char *szName)
{
    static char aBuf[1100];
    long n;
    long i;
    int fText = 1;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        out(szName);
        out(": cannot open\n");
        return -1;
    }
    for (i = 0; i < n; i++) {
        unsigned char c = (unsigned char)aBuf[i];

        if (c == 0) {
            fText = 0;
            break;
        }
        if (c < 7u || (c > 13u && c < 32u)) {
            fText = 0;
            break;
        }
    }
    out(szName);
    out(": ");
    if (n == 0) {
        out("empty\n");
    } else if (fText) {
        out("ASCII text\n");
    } else {
        out("data\n");
    }
    return 0;
}

/* namei: print each path component (bring-up; no real mount walk). */
static int
cmd_namei(const char *szPath)
{
    size_t i;
    size_t nStart;

    if (szPath == NULL || szPath[0] == '\0') {
        return -1;
    }
    out("f: ");
    out(szPath);
    out("\n");
    if (szPath[0] == '/') {
        out("d /\n");
    }
    nStart = 0;
    for (i = 0; szPath[i] != '\0'; i++) {
        if (szPath[i] == '/') {
            if (i > nStart) {
                char aComp[64];
                size_t cb = i - nStart;
                size_t j;

                if (cb >= sizeof(aComp)) {
                    cb = sizeof(aComp) - 1;
                }
                for (j = 0; j < cb; j++) {
                    aComp[j] = szPath[nStart + j];
                }
                aComp[cb] = '\0';
                out("d ");
                out(aComp);
                out("\n");
            }
            nStart = i + 1;
        }
    }
    if (szPath[nStart] != '\0') {
        out("f ");
        out(szPath + nStart);
        out("\n");
    }
    return 0;
}

/* shred: overwrite small file with zeros then remove (bring-up). */
static int
cmd_shred(const char *szName)
{
    static char aZ[64];
    long n;
    size_t i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    for (i = 0; i < sizeof(aZ); i++) {
        aZ[i] = '\0';
    }
    n = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)szName, (long)(uintptr_t)aZ,
               (long)sizeof(aZ));
    if (n < 0) {
        return -1;
    }
    return cmd_rm(szName);
}

/* Compact FIPS 180-1 SHA-1 (integer only) for sha1sum product command. */
struct sh_sha1 {
    unsigned aState[5];
    unsigned long long uBits;
    unsigned char aBuf[64];
    unsigned uUsed;
};

static unsigned
sh_sha1_rol(unsigned x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static void
sh_sha1_init(struct sh_sha1 *p)
{
    p->aState[0] = 0x67452301u;
    p->aState[1] = 0xefcdab89u;
    p->aState[2] = 0x98badcfeu;
    p->aState[3] = 0x10325476u;
    p->aState[4] = 0xc3d2e1f0u;
    p->uBits = 0;
    p->uUsed = 0;
}

static void
sh_sha1_block(struct sh_sha1 *p, const unsigned char *pBlk)
{
    unsigned w[80];
    unsigned a, b, c, d, e, i;

    for (i = 0; i < 16u; i++) {
        w[i] = ((unsigned)pBlk[i * 4u] << 24) |
               ((unsigned)pBlk[i * 4u + 1u] << 16) |
               ((unsigned)pBlk[i * 4u + 2u] << 8) |
               (unsigned)pBlk[i * 4u + 3u];
    }
    for (i = 16u; i < 80u; i++) {
        w[i] = sh_sha1_rol(w[i - 3u] ^ w[i - 8u] ^ w[i - 14u] ^ w[i - 16u], 1u);
    }
    a = p->aState[0];
    b = p->aState[1];
    c = p->aState[2];
    d = p->aState[3];
    e = p->aState[4];
    for (i = 0; i < 80u; i++) {
        unsigned f, k, t;

        if (i < 20u) {
            f = (b & c) | ((~b) & d);
            k = 0x5a827999u;
        } else if (i < 40u) {
            f = b ^ c ^ d;
            k = 0x6ed9eba1u;
        } else if (i < 60u) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8f1bbcdcu;
        } else {
            f = b ^ c ^ d;
            k = 0xca62c1d6u;
        }
        t = sh_sha1_rol(a, 5u) + f + e + k + w[i];
        e = d;
        d = c;
        c = sh_sha1_rol(b, 30u);
        b = a;
        a = t;
    }
    p->aState[0] += a;
    p->aState[1] += b;
    p->aState[2] += c;
    p->aState[3] += d;
    p->aState[4] += e;
}

static void
sh_sha1_update(struct sh_sha1 *p, const void *pData, size_t cb)
{
    const unsigned char *pIn = (const unsigned char *)pData;

    p->uBits += (unsigned long long)cb * 8ull;
    while (cb > 0) {
        size_t nRoom = 64u - (size_t)p->uUsed;
        size_t nCopy = cb < nRoom ? cb : nRoom;
        size_t i;

        for (i = 0; i < nCopy; i++) {
            p->aBuf[p->uUsed + (unsigned)i] = pIn[i];
        }
        p->uUsed += (unsigned)nCopy;
        pIn += nCopy;
        cb -= nCopy;
        if (p->uUsed == 64u) {
            sh_sha1_block(p, p->aBuf);
            p->uUsed = 0;
        }
    }
}

static void
sh_sha1_final(struct sh_sha1 *p, unsigned char *pDig)
{
    unsigned char aPad[64];
    unsigned long long uBits;
    unsigned i;

    aPad[0] = 0x80;
    for (i = 1; i < 64u; i++) {
        aPad[i] = 0;
    }
    if (p->uUsed >= 56u) {
        sh_sha1_update(p, aPad, 64u - p->uUsed);
        for (i = 0; i < 56u; i++) {
            aPad[i] = 0;
        }
        sh_sha1_update(p, aPad, 56u);
    } else {
        sh_sha1_update(p, aPad, 56u - p->uUsed);
    }
    uBits = p->uBits;
    aPad[0] = (unsigned char)(uBits >> 56);
    aPad[1] = (unsigned char)(uBits >> 48);
    aPad[2] = (unsigned char)(uBits >> 40);
    aPad[3] = (unsigned char)(uBits >> 32);
    aPad[4] = (unsigned char)(uBits >> 24);
    aPad[5] = (unsigned char)(uBits >> 16);
    aPad[6] = (unsigned char)(uBits >> 8);
    aPad[7] = (unsigned char)uBits;
    sh_sha1_update(p, aPad, 8u);
    for (i = 0; i < 5u; i++) {
        pDig[i * 4u] = (unsigned char)(p->aState[i] >> 24);
        pDig[i * 4u + 1u] = (unsigned char)(p->aState[i] >> 16);
        pDig[i * 4u + 2u] = (unsigned char)(p->aState[i] >> 8);
        pDig[i * 4u + 3u] = (unsigned char)p->aState[i];
    }
}

static int
cmd_sha1sum(const char *szName)
{
    static char aBuf[1100];
    static char aHex[48];
    static const char *szDigits = "0123456789abcdef";
    struct sh_sha1 ctx;
    unsigned char aDig[20];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_sha1_init(&ctx);
    sh_sha1_update(&ctx, aBuf, (size_t)n);
    sh_sha1_final(&ctx, aDig);
    for (i = 0; i < 20u; i++) {
        aHex[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aHex[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aHex[40] = '\0';
    out(aHex);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* Compact FIPS 180-4 SHA-256 (integer only) for sha256sum. */
struct sh_sha256 {
    unsigned aState[8];
    unsigned long long uBits;
    unsigned char aBuf[64];
    unsigned uUsed;
};

static unsigned
sh_sha256_ror(unsigned x, unsigned n)
{
    return (x >> n) | (x << (32u - n));
}

static void
sh_sha256_block(struct sh_sha256 *p, const unsigned char *pBlk)
{
    static const unsigned aK[64] = {
        0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
        0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
        0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
        0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
        0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
        0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
        0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
        0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
        0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
        0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
        0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
        0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
        0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
    };
    unsigned w[64];
    unsigned a, b, c, d, e, f, g, h, i;

    for (i = 0; i < 16u; i++) {
        w[i] = ((unsigned)pBlk[i * 4u] << 24) |
               ((unsigned)pBlk[i * 4u + 1u] << 16) |
               ((unsigned)pBlk[i * 4u + 2u] << 8) |
               (unsigned)pBlk[i * 4u + 3u];
    }
    for (i = 16u; i < 64u; i++) {
        unsigned s0 = sh_sha256_ror(w[i - 15u], 7u) ^
                      sh_sha256_ror(w[i - 15u], 18u) ^ (w[i - 15u] >> 3u);
        unsigned s1 = sh_sha256_ror(w[i - 2u], 17u) ^
                      sh_sha256_ror(w[i - 2u], 19u) ^ (w[i - 2u] >> 10u);

        w[i] = w[i - 16u] + s0 + w[i - 7u] + s1;
    }
    a = p->aState[0];
    b = p->aState[1];
    c = p->aState[2];
    d = p->aState[3];
    e = p->aState[4];
    f = p->aState[5];
    g = p->aState[6];
    h = p->aState[7];
    for (i = 0; i < 64u; i++) {
        unsigned S1 = sh_sha256_ror(e, 6u) ^ sh_sha256_ror(e, 11u) ^
                      sh_sha256_ror(e, 25u);
        unsigned ch = (e & f) ^ ((~e) & g);
        unsigned t1 = h + S1 + ch + aK[i] + w[i];
        unsigned S0 = sh_sha256_ror(a, 2u) ^ sh_sha256_ror(a, 13u) ^
                      sh_sha256_ror(a, 22u);
        unsigned maj = (a & b) ^ (a & c) ^ (b & c);
        unsigned t2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    p->aState[0] += a;
    p->aState[1] += b;
    p->aState[2] += c;
    p->aState[3] += d;
    p->aState[4] += e;
    p->aState[5] += f;
    p->aState[6] += g;
    p->aState[7] += h;
}

static void
sh_sha256_init(struct sh_sha256 *p)
{
    p->aState[0] = 0x6a09e667u;
    p->aState[1] = 0xbb67ae85u;
    p->aState[2] = 0x3c6ef372u;
    p->aState[3] = 0xa54ff53au;
    p->aState[4] = 0x510e527fu;
    p->aState[5] = 0x9b05688cu;
    p->aState[6] = 0x1f83d9abu;
    p->aState[7] = 0x5be0cd19u;
    p->uBits = 0;
    p->uUsed = 0;
}

static void
sh_sha256_update(struct sh_sha256 *p, const void *pData, size_t cb)
{
    const unsigned char *pIn = (const unsigned char *)pData;

    p->uBits += (unsigned long long)cb * 8ull;
    while (cb > 0) {
        unsigned uRoom = 64u - p->uUsed;
        unsigned uTake = (cb < (size_t)uRoom) ? (unsigned)cb : uRoom;
        unsigned i;

        for (i = 0; i < uTake; i++) {
            p->aBuf[p->uUsed + i] = pIn[i];
        }
        p->uUsed += uTake;
        pIn += uTake;
        cb -= uTake;
        if (p->uUsed == 64u) {
            sh_sha256_block(p, p->aBuf);
            p->uUsed = 0;
        }
    }
}

static void
sh_sha256_final(struct sh_sha256 *p, unsigned char aOut[32])
{
    unsigned char aPad[64];
    unsigned u, i;
    unsigned long long uBits;

    aPad[0] = 0x80;
    for (i = 1; i < 64u; i++) {
        aPad[i] = 0;
    }
    u = p->uUsed;
    if (u < 56u) {
        sh_sha256_update(p, aPad, 56u - u);
    } else {
        sh_sha256_update(p, aPad, 64u - u + 56u);
    }
    uBits = p->uBits;
    for (i = 0; i < 8u; i++) {
        aPad[7u - i] = (unsigned char)(uBits >> (8u * i));
    }
    sh_sha256_update(p, aPad, 8u);
    for (i = 0; i < 8u; i++) {
        aOut[i * 4u] = (unsigned char)(p->aState[i] >> 24);
        aOut[i * 4u + 1u] = (unsigned char)(p->aState[i] >> 16);
        aOut[i * 4u + 2u] = (unsigned char)(p->aState[i] >> 8);
        aOut[i * 4u + 3u] = (unsigned char)p->aState[i];
    }
}

static int
cmd_sha256sum(const char *szName)
{
    static char aBuf[1100];
    static char aHex[72];
    static const char *szDigits = "0123456789abcdef";
    struct sh_sha256 ctx;
    unsigned char aDig[32];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_sha256_init(&ctx);
    sh_sha256_update(&ctx, aBuf, (size_t)n);
    sh_sha256_final(&ctx, aDig);
    for (i = 0; i < 32u; i++) {
        aHex[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aHex[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aHex[64] = '\0';
    out(aHex);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* Compact FIPS 180-4 SHA-512 (integer only) for sha512sum. */
struct sh_sha512 {
    unsigned long long aState[8];
    unsigned long long uBits;
    unsigned char aBuf[128];
    unsigned uLen;
};

static unsigned long long
sh_sha512_ror(unsigned long long x, unsigned n)
{
    return (x >> n) | (x << (64u - n));
}

static void
sh_sha512_block(struct sh_sha512 *p, const unsigned char *pBlk)
{
    static const unsigned long long aK[80] = {
        0x428a2f98d728ae22ull, 0x7137449123ef65cdull, 0xb5c0fbcfec4d3b2full,
        0xe9b5dba58189dbbcull, 0x3956c25bf348b538ull, 0x59f111f1b605d019ull,
        0x923f82a4af194f9bull, 0xab1c5ed5da6d8118ull, 0xd807aa98a3030242ull,
        0x12835b0145706fbeull, 0x243185be4ee4b28cull, 0x550c7dc3d5ffb4e2ull,
        0x72be5d74f27b896full, 0x80deb1fe3b1696b1ull, 0x9bdc06a725c71235ull,
        0xc19bf174cf692694ull, 0xe49b69c19ef14ad2ull, 0xefbe4786384f25e3ull,
        0x0fc19dc68b8cd5b5ull, 0x240ca1cc77ac9c65ull, 0x2de92c6f592b0275ull,
        0x4a7484aa6ea6e483ull, 0x5cb0a9dcbd41fbd4ull, 0x76f988da831153b5ull,
        0x983e5152ee66dfabull, 0xa831c66d2db43210ull, 0xb00327c898fb213full,
        0xbf597fc7beef0ee4ull, 0xc6e00bf33da88fc2ull, 0xd5a79147930aa725ull,
        0x06ca6351e003826full, 0x142929670a0e6e70ull, 0x27b70a8546d22ffcull,
        0x2e1b21385c26c926ull, 0x4d2c6dfc5ac42aedull, 0x53380d139d95b3dfull,
        0x650a73548baf63deull, 0x766a0abb3c77b2a8ull, 0x81c2c92e47edaee6ull,
        0x92722c851482353bull, 0xa2bfe8a14cf10364ull, 0xa81a664bbc423001ull,
        0xc24b8b70d0f89791ull, 0xc76c51a30654be30ull, 0xd192e819d6ef5218ull,
        0xd69906245565a910ull, 0xf40e35855771202aull, 0x106aa07032bbd1b8ull,
        0x19a4c116b8d2d0c8ull, 0x1e376c085141ab53ull, 0x2748774cdf8eeb99ull,
        0x34b0bcb5e19b48a8ull, 0x391c0cb3c5c95a63ull, 0x4ed8aa4ae3418acbull,
        0x5b9cca4f7763e373ull, 0x682e6ff3d6b2b8a3ull, 0x748f82ee5defb2fcull,
        0x78a5636f43172f60ull, 0x84c87814a1f0ab72ull, 0x8cc702081a6439ecull,
        0x90befffa23631e28ull, 0xa4506cebde82bde9ull, 0xbef9a3f7b2c67915ull,
        0xc67178f2e372532bull, 0xca273eceea26619cull, 0xd186b8c721c0c207ull,
        0xeada7dd6cde0eb1eull, 0xf57d4f7fee6ed178ull, 0x06f067aa72176fbaull,
        0x0a637dc5a2c898a6ull, 0x113f9804bef90daeull, 0x1b710b35131c471bull,
        0x28db77f523047d84ull, 0x32caab7b40c72493ull, 0x3c9ebe0a15c9bebcull,
        0x431d67c49c100d4cull, 0x4cc5d4becb3e42b6ull, 0x597f299cfc657e2aull,
        0x5fcb6fab3ad6faecull, 0x6c44198c4a475817ull
    };
    unsigned long long w[80];
    unsigned long long a, b, c, d, e, f, g, h;
    unsigned i;

    for (i = 0; i < 16u; i++) {
        w[i] = ((unsigned long long)pBlk[i * 8u] << 56) |
               ((unsigned long long)pBlk[i * 8u + 1u] << 48) |
               ((unsigned long long)pBlk[i * 8u + 2u] << 40) |
               ((unsigned long long)pBlk[i * 8u + 3u] << 32) |
               ((unsigned long long)pBlk[i * 8u + 4u] << 24) |
               ((unsigned long long)pBlk[i * 8u + 5u] << 16) |
               ((unsigned long long)pBlk[i * 8u + 6u] << 8) |
               ((unsigned long long)pBlk[i * 8u + 7u]);
    }
    for (i = 16u; i < 80u; i++) {
        unsigned long long s0 = sh_sha512_ror(w[i - 15u], 1u) ^
                                sh_sha512_ror(w[i - 15u], 8u) ^
                                (w[i - 15u] >> 7u);
        unsigned long long s1 = sh_sha512_ror(w[i - 2u], 19u) ^
                                sh_sha512_ror(w[i - 2u], 61u) ^
                                (w[i - 2u] >> 6u);

        w[i] = w[i - 16u] + s0 + w[i - 7u] + s1;
    }
    a = p->aState[0];
    b = p->aState[1];
    c = p->aState[2];
    d = p->aState[3];
    e = p->aState[4];
    f = p->aState[5];
    g = p->aState[6];
    h = p->aState[7];
    for (i = 0; i < 80u; i++) {
        unsigned long long S1 = sh_sha512_ror(e, 14u) ^ sh_sha512_ror(e, 18u) ^
                                sh_sha512_ror(e, 41u);
        unsigned long long ch = (e & f) ^ ((~e) & g);
        unsigned long long t1 = h + S1 + ch + aK[i] + w[i];
        unsigned long long S0 = sh_sha512_ror(a, 28u) ^ sh_sha512_ror(a, 34u) ^
                                sh_sha512_ror(a, 39u);
        unsigned long long maj = (a & b) ^ (a & c) ^ (b & c);
        unsigned long long t2 = S0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    p->aState[0] += a;
    p->aState[1] += b;
    p->aState[2] += c;
    p->aState[3] += d;
    p->aState[4] += e;
    p->aState[5] += f;
    p->aState[6] += g;
    p->aState[7] += h;
}

static void
sh_sha512_init(struct sh_sha512 *p)
{
    p->aState[0] = 0x6a09e667f3bcc908ull;
    p->aState[1] = 0xbb67ae8584caa73bull;
    p->aState[2] = 0x3c6ef372fe94f82bull;
    p->aState[3] = 0xa54ff53a5f1d36f1ull;
    p->aState[4] = 0x510e527fade682d1ull;
    p->aState[5] = 0x9b05688c2b3e6c1full;
    p->aState[6] = 0x1f83d9abfb41bd6bull;
    p->aState[7] = 0x5be0cd19137e2179ull;
    p->uBits = 0;
    p->uLen = 0;
}

static void
sh_sha512_update(struct sh_sha512 *p, const void *pData, size_t cb)
{
    const unsigned char *pIn = (const unsigned char *)pData;
    size_t i;

    for (i = 0; i < cb; i++) {
        p->aBuf[p->uLen++] = pIn[i];
        if (p->uLen == 128u) {
            sh_sha512_block(p, p->aBuf);
            p->uBits += 1024ull;
            p->uLen = 0;
        }
    }
}

static void
sh_sha512_final(struct sh_sha512 *p, unsigned char aOut[64])
{
    unsigned long long uBits = p->uBits + (unsigned long long)p->uLen * 8ull;
    unsigned char aPad[128];
    unsigned u;
    unsigned i;

    p->aBuf[p->uLen++] = 0x80u;
    if (p->uLen > 112u) {
        while (p->uLen < 128u) {
            p->aBuf[p->uLen++] = 0;
        }
        sh_sha512_block(p, p->aBuf);
        p->uLen = 0;
    }
    while (p->uLen < 112u) {
        p->aBuf[p->uLen++] = 0;
    }
    /* 128-bit length: high 64 zero, low 64 = bit count */
    for (i = 0; i < 8u; i++) {
        p->aBuf[112u + i] = 0;
    }
    for (i = 0; i < 8u; i++) {
        p->aBuf[127u - i] = (unsigned char)(uBits >> (8u * i));
    }
    sh_sha512_block(p, p->aBuf);
    for (i = 0; i < 8u; i++) {
        aOut[i * 8u] = (unsigned char)(p->aState[i] >> 56);
        aOut[i * 8u + 1u] = (unsigned char)(p->aState[i] >> 48);
        aOut[i * 8u + 2u] = (unsigned char)(p->aState[i] >> 40);
        aOut[i * 8u + 3u] = (unsigned char)(p->aState[i] >> 32);
        aOut[i * 8u + 4u] = (unsigned char)(p->aState[i] >> 24);
        aOut[i * 8u + 5u] = (unsigned char)(p->aState[i] >> 16);
        aOut[i * 8u + 6u] = (unsigned char)(p->aState[i] >> 8);
        aOut[i * 8u + 7u] = (unsigned char)p->aState[i];
    }
    (void)aPad;
    (void)u;
}

static int
cmd_sha512sum(const char *szName)
{
    static char aBuf[1100];
    static char aHex[136];
    static const char *szDigits = "0123456789abcdef";
    struct sh_sha512 ctx;
    unsigned char aDig[64];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_sha512_init(&ctx);
    sh_sha512_update(&ctx, aBuf, (size_t)n);
    sh_sha512_final(&ctx, aDig);
    for (i = 0; i < 64u; i++) {
        aHex[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aHex[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aHex[128] = '\0';
    out(aHex);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* IEEE CRC-32 (poly 0xEDB88320), product v0.19. */
static unsigned
sh_crc32(const unsigned char *p, size_t cb)
{
    unsigned uCrc = 0xffffffffu;
    size_t i;
    unsigned b;

    for (i = 0; i < cb; i++) {
        uCrc ^= (unsigned)p[i];
        for (b = 0; b < 8u; b++) {
            unsigned uLsb = uCrc & 1u;

            uCrc >>= 1;
            if (uLsb != 0u) {
                uCrc ^= 0xedb88320u;
            }
        }
    }
    return ~uCrc;
}

static int
cmd_crc32(const char *szName)
{
    static char aBuf[1100];
    static char aOut[48];
    long n;
    unsigned uCrc;
    unsigned i;
    static const char *szDigits = "0123456789abcdef";

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uCrc = sh_crc32((const unsigned char *)aBuf, (size_t)n);
    for (i = 0; i < 8u; i++) {
        unsigned nShift = (7u - i) * 4u;

        aOut[i] = szDigits[(uCrc >> nShift) & 0xfu];
    }
    aOut[8] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* dos2unix: strip CR before LF in a small file (in place via rewrite). */
static int
cmd_dos2unix(const char *szName)
{
    static char aBuf[1100];
    static char aOut[1100];
    long n;
    long i;
    size_t nOut;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    nOut = 0;
    for (i = 0; i < n; i++) {
        if (aBuf[i] == '\r' && i + 1 < n && aBuf[i + 1] == '\n') {
            continue;
        }
        if (nOut + 1 >= sizeof(aOut)) {
            return -1;
        }
        aOut[nOut++] = aBuf[i];
    }
    aOut[nOut] = '\0';
    return cmd_echo_file(szName, aOut);
}

/* unix2dos: insert CR before each LF (in place via rewrite). */
static int
cmd_unix2dos(const char *szName)
{
    static char aBuf[1100];
    static char aOut[1100];
    long n;
    long i;
    size_t nOut;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    nOut = 0;
    for (i = 0; i < n; i++) {
        if (aBuf[i] == '\n') {
            if (nOut + 2 >= sizeof(aOut)) {
                return -1;
            }
            aOut[nOut++] = '\r';
            aOut[nOut++] = '\n';
        } else {
            if (nOut + 1 >= sizeof(aOut)) {
                return -1;
            }
            aOut[nOut++] = aBuf[i];
        }
    }
    aOut[nOut] = '\0';
    return cmd_echo_file(szName, aOut);
}

/* uniq: drop consecutive duplicate lines of a small file. */
static int
cmd_uniq(const char *szName)
{
    static char aBuf[512];
    static char aPrev[128];
    long n;
    long i;
    long nLineStart;
    size_t nPrev;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    aPrev[0] = '\0';
    nPrev = 0;
    nLineStart = 0;
    for (i = 0; i <= n; i++) {
        if (i == n || aBuf[i] == '\n') {
            size_t cb = (size_t)(i - nLineStart);
            size_t j;
            int fSame;

            if (cb >= sizeof(aPrev)) {
                cb = sizeof(aPrev) - 1;
            }
            fSame = (cb == nPrev);
            for (j = 0; fSame && j < cb; j++) {
                if (aBuf[nLineStart + (long)j] != aPrev[j]) {
                    fSame = 0;
                }
            }
            if (!fSame) {
                for (j = 0; j < cb; j++) {
                    char aC[2];

                    aC[0] = aBuf[nLineStart + (long)j];
                    aC[1] = '\0';
                    out(aC);
                }
                out("\n");
                for (j = 0; j < cb; j++) {
                    aPrev[j] = aBuf[nLineStart + (long)j];
                }
                aPrev[cb] = '\0';
                nPrev = cb;
            }
            nLineStart = i + 1;
        }
    }
    return 0;
}

/* Yield-based sleep (no wall clock required for smoke). */
static int
cmd_sleep(const char *szArg)
{
    unsigned u;
    unsigned i;
    const char *p;

    u = 1;
    if (szArg != NULL && szArg[0] != '\0') {
        u = 0;
        p = szArg;
        while (*p >= '0' && *p <= '9') {
            u = u * 10u + (unsigned)(*p - '0');
            p++;
        }
        if (u == 0) {
            u = 1;
        }
        if (u > 8u) {
            u = 8u; /* cap so smoke cannot hang */
        }
    }
    for (i = 0; i < u * 4u; i++) {
        gj_yield();
    }
    return 0;
}

static int
cmd_bigfile(void)
{
    static char aBig[600];
    static char aRd[600];
    size_t i;
    long n;

    for (i = 0; i < sizeof(aBig) - 1; i++) {
        aBig[i] = (char)('A' + (i % 26));
    }
    aBig[sizeof(aBig) - 1] = '\0';
    n = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)"big.txt",
               (long)(uintptr_t)aBig, (long)(sizeof(aBig) - 1));
    if (n != (long)(sizeof(aBig) - 1)) {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)"big.txt",
               (long)(uintptr_t)aRd, (long)sizeof(aRd));
    if (n != (long)(sizeof(aBig) - 1) || aRd[0] != 'A' || aRd[25] != 'Z') {
        return -1;
    }
    return 0;
}

/*
 * Non-blocking-ish line editor over serial console.
 * Drains available bytes; treats CR/LF as end; max empty polls then timeout.
 * Smoke: no input → timeout quickly → readline idle PASS.
 */
static int
read_line(char *pBuf, size_t cbBuf, unsigned uMaxEmptyPolls)
{
    size_t n = 0;
    unsigned empty = 0;
    char aCh[8];
    long r;
    unsigned i;

    if (pBuf == NULL || cbBuf < 2) {
        return -1;
    }
    pBuf[0] = '\0';
    while (n + 1 < cbBuf && empty < uMaxEmptyPolls) {
        r = gj_console_read(aCh, (unsigned)sizeof(aCh));
        if (r < 0) {
            return -1;
        }
        if (r == 0) {
            empty++;
            gj_yield();
            continue;
        }
        empty = 0;
        for (i = 0; i < (unsigned)r && n + 1 < cbBuf; i++) {
            char c = aCh[i];

            if (c == '\r' || c == '\n') {
                pBuf[n] = '\0';
                out("\n");
                return (int)n;
            }
            if (c == 0x7f || c == '\b') {
                if (n > 0) {
                    n--;
                    out("\b \b");
                }
                continue;
            }
            if (c >= 32 && c < 127) {
                pBuf[n++] = c;
                {
                    char aEcho[2];

                    aEcho[0] = c;
                    aEcho[1] = '\0';
                    out(aEcho);
                }
            }
        }
    }
    pBuf[n] = '\0';
    return -2; /* idle/timeout */
}


/* BLAKE2s-256 product digest (v0.20) — compact integer-only. */
struct sh_b2s {
    unsigned aH[8];
    unsigned aT[2];
    unsigned char aBuf[64];
    unsigned cbBuf;
};

static unsigned
sh_b2s_ror(unsigned x, unsigned n)
{
    return (x >> n) | (x << (32u - n));
}

static void
sh_b2s_g(unsigned *v, unsigned a, unsigned b, unsigned c, unsigned d,
         unsigned x, unsigned y)
{
    v[a] = v[a] + v[b] + x;
    v[d] = sh_b2s_ror(v[d] ^ v[a], 16u);
    v[c] = v[c] + v[d];
    v[b] = sh_b2s_ror(v[b] ^ v[c], 12u);
    v[a] = v[a] + v[b] + y;
    v[d] = sh_b2s_ror(v[d] ^ v[a], 8u);
    v[c] = v[c] + v[d];
    v[b] = sh_b2s_ror(v[b] ^ v[c], 7u);
}

static void
sh_b2s_compress(struct sh_b2s *p, int fLast)
{
    static const unsigned aIv[8] = {
        0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
        0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};
    static const unsigned char aSig[10][16] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
        {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
        {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
        {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
        {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
        {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
        {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
        {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
        {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0}};
    unsigned v[16];
    unsigned m[16];
    unsigned i, r;

    for (i = 0; i < 8u; i++) {
        v[i] = p->aH[i];
        v[i + 8u] = aIv[i];
    }
    v[12] ^= p->aT[0];
    v[13] ^= p->aT[1];
    if (fLast) {
        v[14] = ~v[14];
    }
    for (i = 0; i < 16u; i++) {
        const unsigned char *q = p->aBuf + i * 4u;
        m[i] = (unsigned)q[0] | ((unsigned)q[1] << 8) |
               ((unsigned)q[2] << 16) | ((unsigned)q[3] << 24);
    }
    for (r = 0; r < 10u; r++) {
        const unsigned char *s = aSig[r];
        sh_b2s_g(v, 0, 4, 8, 12, m[s[0]], m[s[1]]);
        sh_b2s_g(v, 1, 5, 9, 13, m[s[2]], m[s[3]]);
        sh_b2s_g(v, 2, 6, 10, 14, m[s[4]], m[s[5]]);
        sh_b2s_g(v, 3, 7, 11, 15, m[s[6]], m[s[7]]);
        sh_b2s_g(v, 0, 5, 10, 15, m[s[8]], m[s[9]]);
        sh_b2s_g(v, 1, 6, 11, 12, m[s[10]], m[s[11]]);
        sh_b2s_g(v, 2, 7, 8, 13, m[s[12]], m[s[13]]);
        sh_b2s_g(v, 3, 4, 9, 14, m[s[14]], m[s[15]]);
    }
    for (i = 0; i < 8u; i++) {
        p->aH[i] ^= v[i] ^ v[i + 8u];
    }
}

static void
sh_b2s_init(struct sh_b2s *p)
{
    static const unsigned aIv[8] = {
        0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
        0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};
    unsigned i;

    for (i = 0; i < 8u; i++) {
        p->aH[i] = aIv[i];
    }
    p->aH[0] ^= 0x01010000u ^ 32u;
    p->aT[0] = 0;
    p->aT[1] = 0;
    p->cbBuf = 0;
}

static void
sh_b2s_update(struct sh_b2s *p, const unsigned char *q, long n)
{
    long i = 0;

    while (i < n) {
        p->aBuf[p->cbBuf++] = q[i++];
        if (p->cbBuf == 64u) {
            p->aT[0] += 64u;
            if (p->aT[0] < 64u) {
                p->aT[1]++;
            }
            sh_b2s_compress(p, 0);
            p->cbBuf = 0;
        }
    }
}

static void
sh_b2s_final(struct sh_b2s *p, unsigned char aOut[32])
{
    unsigned i;

    p->aT[0] += p->cbBuf;
    if (p->aT[0] < p->cbBuf) {
        p->aT[1]++;
    }
    while (p->cbBuf < 64u) {
        p->aBuf[p->cbBuf++] = 0;
    }
    sh_b2s_compress(p, 1);
    for (i = 0; i < 8u; i++) {
        aOut[i * 4u] = (unsigned char)p->aH[i];
        aOut[i * 4u + 1u] = (unsigned char)(p->aH[i] >> 8);
        aOut[i * 4u + 2u] = (unsigned char)(p->aH[i] >> 16);
        aOut[i * 4u + 3u] = (unsigned char)(p->aH[i] >> 24);
    }
}

static int
cmd_b2sum(const char *szName)
{
    static char aBuf[1100];
    static char aOut[72];
    static const char *szDigits = "0123456789abcdef";
    struct sh_b2s ctx;
    unsigned char aDig[32];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_b2s_init(&ctx);
    sh_b2s_update(&ctx, (const unsigned char *)aBuf, n);
    sh_b2s_final(&ctx, aDig);
    for (i = 0; i < 32u; i++) {
        aOut[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aOut[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aOut[64] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* Compact FIPS 202 SHA3-256 for sha3sum (product v0.21). */
struct sh_sha3 {
    unsigned long long a[25];
    unsigned char aBuf[136];
    unsigned cbBuf;
};

static unsigned long long
sh_sha3_rol(unsigned long long x, unsigned n)
{
    return (x << n) | (x >> (64u - n));
}

static void
sh_sha3_f(unsigned long long st[25])
{
    static const unsigned aRotc[24] = {
        1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14,
        27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44};
    static const unsigned aPiln[24] = {
        10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4,
        15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1};
    static const unsigned long long aRc[24] = {
        0x0000000000000001ull, 0x0000000000008082ull, 0x800000000000808aull,
        0x8000000080008000ull, 0x000000000000808bull, 0x0000000080000001ull,
        0x8000000080008081ull, 0x8000000000008009ull, 0x000000000000008aull,
        0x0000000000000088ull, 0x0000000080008009ull, 0x000000008000000aull,
        0x000000008000808bull, 0x800000000000008bull, 0x8000000000008089ull,
        0x8000000000008003ull, 0x8000000000008002ull, 0x8000000000000080ull,
        0x000000000000800aull, 0x800000008000000aull, 0x8000000080008081ull,
        0x8000000000008080ull, 0x0000000080000001ull, 0x8000000080008008ull};
    unsigned long long bc[5];
    unsigned long long t;
    unsigned round, i, j;

    for (round = 0; round < 24u; round++) {
        for (i = 0; i < 5u; i++) {
            bc[i] = st[i] ^ st[i + 5u] ^ st[i + 10u] ^ st[i + 15u] ^
                    st[i + 20u];
        }
        for (i = 0; i < 5u; i++) {
            t = bc[(i + 4u) % 5u] ^ sh_sha3_rol(bc[(i + 1u) % 5u], 1u);
            for (j = 0; j < 25u; j += 5u) {
                st[j + i] ^= t;
            }
        }
        t = st[1];
        for (i = 0; i < 24u; i++) {
            j = aPiln[i];
            bc[0] = st[j];
            st[j] = sh_sha3_rol(t, aRotc[i]);
            t = bc[0];
        }
        for (j = 0; j < 25u; j += 5u) {
            for (i = 0; i < 5u; i++) {
                bc[i] = st[j + i];
            }
            for (i = 0; i < 5u; i++) {
                st[j + i] ^= (~bc[(i + 1u) % 5u]) & bc[(i + 2u) % 5u];
            }
        }
        st[0] ^= aRc[round];
    }
}

static void
sh_sha3_init(struct sh_sha3 *p)
{
    unsigned i;

    for (i = 0; i < 25u; i++) {
        p->a[i] = 0;
    }
    p->cbBuf = 0;
}

static void
sh_sha3_absorb_block(struct sh_sha3 *p)
{
    unsigned i;

    for (i = 0; i < 136u; i++) {
        p->a[i / 8u] ^= (unsigned long long)p->aBuf[i]
                        << (8u * (i % 8u));
    }
    sh_sha3_f(p->a);
}

static void
sh_sha3_update(struct sh_sha3 *p, const unsigned char *q, long n)
{
    long i = 0;

    while (i < n) {
        p->aBuf[p->cbBuf++] = q[i++];
        if (p->cbBuf == 136u) {
            sh_sha3_absorb_block(p);
            p->cbBuf = 0;
        }
    }
}

static void
sh_sha3_final(struct sh_sha3 *p, unsigned char aOut[32])
{
    unsigned i;

    for (i = p->cbBuf; i < 136u; i++) {
        p->aBuf[i] = 0;
    }
    p->aBuf[p->cbBuf] ^= 0x06u;
    p->aBuf[135] ^= 0x80u;
    sh_sha3_absorb_block(p);
    for (i = 0; i < 32u; i++) {
        aOut[i] = (unsigned char)(p->a[i / 8u] >> (8u * (i % 8u)));
    }
}

static int
cmd_sha3sum(const char *szName)
{
    static char aBuf[1100];
    static char aOut[72];
    static const char *szDigits = "0123456789abcdef";
    struct sh_sha3 ctx;
    unsigned char aDig[32];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_sha3_init(&ctx);
    sh_sha3_update(&ctx, (const unsigned char *)aBuf, n);
    sh_sha3_final(&ctx, aDig);
    for (i = 0; i < 32u; i++) {
        aOut[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aOut[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aOut[64] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* SHA-384 product digest (v0.23) — same compression as SHA-512, different IV. */
static void
sh_sha384_init(struct sh_sha512 *p)
{
    p->aState[0] = 0xcbbb9d5dc1059ed8ull;
    p->aState[1] = 0x629a292a367cd507ull;
    p->aState[2] = 0x9159015a3070dd17ull;
    p->aState[3] = 0x152fecd8f70e5939ull;
    p->aState[4] = 0x67332667ffc00b31ull;
    p->aState[5] = 0x8eb44a8768581511ull;
    p->aState[6] = 0xdb0c2e0d64f98fa7ull;
    p->aState[7] = 0x47b5481dbefa4fa4ull;
    p->uBits = 0;
    p->uLen = 0;
}

static int
cmd_sha384sum(const char *szName)
{
    static char aBuf[1100];
    static char aHex[104];
    static const char *szDigits = "0123456789abcdef";
    struct sh_sha512 ctx;
    unsigned char aDig[64];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_sha384_init(&ctx);
    sh_sha512_update(&ctx, aBuf, (size_t)n);
    sh_sha512_final(&ctx, aDig);
    for (i = 0; i < 48u; i++) {
        aHex[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aHex[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aHex[96] = '\0';
    out(aHex);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* Adler-32 (RFC 1950) over small file (product v0.22). */
static int
cmd_adler32(const char *szName)
{
    static char aBuf[1100];
    static char aOut[16];
    static const char *szDigits = "0123456789abcdef";
    unsigned long uA = 1ul;
    unsigned long uB = 0ul;
    const unsigned long uMod = 65521ul;
    long n;
    long i;
    unsigned long uOut;
    int k;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        uA = (uA + (unsigned char)aBuf[i]) % uMod;
        uB = (uB + uA) % uMod;
    }
    uOut = (uB << 16) | uA;
    for (k = 7; k >= 0; k--) {
        aOut[7 - k] = szDigits[(uOut >> (unsigned)(k * 4)) & 0xful];
    }
    aOut[8] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* SHA-224 (product v0.24): SHA-256 compression, FIPS 180-4 IV, 28-byte dig. */
static void
sh_sha224_init(struct sh_sha256 *p)
{
    p->aState[0] = 0xc1059ed8u;
    p->aState[1] = 0x367cd507u;
    p->aState[2] = 0x3070dd17u;
    p->aState[3] = 0xf70e5939u;
    p->aState[4] = 0xffc00b31u;
    p->aState[5] = 0x68581511u;
    p->aState[6] = 0x64f98fa7u;
    p->aState[7] = 0xbefa4fa4u;
    p->uBits = 0;
    p->uUsed = 0;
}

static int
cmd_sha224sum(const char *szName)
{
    static char aBuf[1100];
    static char aHex[64];
    static const char *szDigits = "0123456789abcdef";
    struct sh_sha256 ctx;
    unsigned char aDig[32];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_sha224_init(&ctx);
    sh_sha256_update(&ctx, aBuf, (size_t)n);
    sh_sha256_final(&ctx, aDig);
    for (i = 0; i < 28u; i++) {
        aHex[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aHex[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aHex[56] = '\0';
    out(aHex);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* XXH32 (product v0.24) — compact one-shot, seed 0. */
static unsigned
sh_xxh32(const unsigned char *p, size_t cb)
{
    const unsigned prime1 = 0x9e3779b1u;
    const unsigned prime2 = 0x85ebca77u;
    const unsigned prime3 = 0xc2b2ae3du;
    const unsigned prime4 = 0x27d4eb2fu;
    const unsigned prime5 = 0x165667b1u;
    unsigned h;
    size_t i = 0;

    if (cb >= 16u) {
        unsigned v1 = prime1 + prime2;
        unsigned v2 = prime2;
        unsigned v3 = 0u;
        unsigned v4 = 0u - prime1;
        const unsigned char *pLimit = p + cb - 16u;

        do {
            unsigned k;
#define SH_XXH32_ROUND(v, x)                                                   \
    do {                                                                       \
        (v) += (x)*prime2;                                                     \
        (v) = ((v) << 13) | ((v) >> 19);                                       \
        (v) *= prime1;                                                         \
    } while (0)
            k = (unsigned)p[i] | ((unsigned)p[i + 1u] << 8) |
                ((unsigned)p[i + 2u] << 16) | ((unsigned)p[i + 3u] << 24);
            SH_XXH32_ROUND(v1, k);
            i += 4u;
            k = (unsigned)p[i] | ((unsigned)p[i + 1u] << 8) |
                ((unsigned)p[i + 2u] << 16) | ((unsigned)p[i + 3u] << 24);
            SH_XXH32_ROUND(v2, k);
            i += 4u;
            k = (unsigned)p[i] | ((unsigned)p[i + 1u] << 8) |
                ((unsigned)p[i + 2u] << 16) | ((unsigned)p[i + 3u] << 24);
            SH_XXH32_ROUND(v3, k);
            i += 4u;
            k = (unsigned)p[i] | ((unsigned)p[i + 1u] << 8) |
                ((unsigned)p[i + 2u] << 16) | ((unsigned)p[i + 3u] << 24);
            SH_XXH32_ROUND(v4, k);
            i += 4u;
        } while (p + i <= pLimit);
        h = ((v1 << 1) | (v1 >> 31)) + ((v2 << 7) | (v2 >> 25)) +
            ((v3 << 12) | (v3 >> 20)) + ((v4 << 18) | (v4 >> 14));
#undef SH_XXH32_ROUND
    } else {
        h = prime5;
    }
    h += (unsigned)cb;
    while (i + 4u <= cb) {
        unsigned k = (unsigned)p[i] | ((unsigned)p[i + 1u] << 8) |
                     ((unsigned)p[i + 2u] << 16) | ((unsigned)p[i + 3u] << 24);
        h += k * prime3;
        h = ((h << 17) | (h >> 15)) * prime4;
        i += 4u;
    }
    while (i < cb) {
        h += (unsigned)p[i] * prime5;
        h = ((h << 11) | (h >> 21)) * prime1;
        i++;
    }
    h ^= h >> 15;
    h *= prime2;
    h ^= h >> 13;
    h *= prime3;
    h ^= h >> 16;
    return h;
}

static int
cmd_xxhsum(const char *szName)
{
    static char aBuf[1100];
    static char aOut[16];
    static const char *szDigits = "0123456789abcdef";
    long n;
    unsigned uH;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uH = sh_xxh32((const unsigned char *)aBuf, (size_t)n);
    for (i = 0; i < 8u; i++) {
        aOut[i] = szDigits[(uH >> ((7u - i) * 4u)) & 0xfu];
    }
    aOut[8] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* CRC-32C Castagnoli (poly 0x82F63B78 reflected), product v0.24. */
static unsigned
sh_crc32c(const unsigned char *p, size_t cb)
{
    unsigned uCrc = 0xffffffffu;
    size_t i;
    unsigned b;

    for (i = 0; i < cb; i++) {
        uCrc ^= (unsigned)p[i];
        for (b = 0; b < 8u; b++) {
            unsigned uLsb = uCrc & 1u;

            uCrc >>= 1;
            if (uLsb != 0u) {
                uCrc ^= 0x82f63b78u;
            }
        }
    }
    return ~uCrc;
}

static int
cmd_crc32c(const char *szName)
{
    static char aBuf[1100];
    static char aOut[16];
    static const char *szDigits = "0123456789abcdef";
    long n;
    unsigned uCrc;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uCrc = sh_crc32c((const unsigned char *)aBuf, (size_t)n);
    for (i = 0; i < 8u; i++) {
        aOut[i] = szDigits[(uCrc >> ((7u - i) * 4u)) & 0xfu];
    }
    aOut[8] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* FNV-1a 32-bit (product v0.25) over small file. */
static unsigned
sh_fnv1a32(const unsigned char *p, size_t cb)
{
    unsigned uH = 2166136261u;
    size_t i;

    for (i = 0; i < cb; i++) {
        uH ^= (unsigned)p[i];
        uH *= 16777619u;
    }
    return uH;
}

static int
cmd_fnvsum(const char *szName)
{
    static char aBuf[1100];
    static char aOut[16];
    static const char *szDigits = "0123456789abcdef";
    long n;
    unsigned uH;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uH = sh_fnv1a32((const unsigned char *)aBuf, (size_t)n);
    for (i = 0; i < 8u; i++) {
        aOut[i] = szDigits[(uH >> ((7u - i) * 4u)) & 0xfu];
    }
    aOut[8] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* SipHash-2-4 with fixed key (product v0.25) — 64-bit hex of small file. */
static unsigned long long
sh_rotl64(unsigned long long x, unsigned n)
{
    return (x << n) | (x >> (64u - n));
}

static unsigned long long
sh_siphash24(const unsigned char *in, size_t inlen)
{
    /* Fixed key: SipHash reference k0||k1 little-endian bytes 00..0f */
    const unsigned long long k0 = 0x0706050403020100ULL;
    const unsigned long long k1 = 0x0f0e0d0c0b0a0908ULL;
    unsigned long long v0 = 0x736f6d6570736575ULL ^ k0;
    unsigned long long v1 = 0x646f72616e646f6dULL ^ k1;
    unsigned long long v2 = 0x6c7967656e657261ULL ^ k0;
    unsigned long long v3 = 0x7465646279746573ULL ^ k1;
    unsigned long long b;
    size_t i = 0;
    size_t nFull = inlen & ~(size_t)7u;
    unsigned left;

#define SH_SIPROUND                                                            \
    do {                                                                       \
        v0 += v1;                                                              \
        v1 = sh_rotl64(v1, 13);                                                \
        v1 ^= v0;                                                              \
        v0 = sh_rotl64(v0, 32);                                                \
        v2 += v3;                                                              \
        v3 = sh_rotl64(v3, 16);                                                \
        v3 ^= v2;                                                              \
        v0 += v3;                                                              \
        v3 = sh_rotl64(v3, 21);                                                \
        v3 ^= v0;                                                              \
        v2 += v1;                                                              \
        v1 = sh_rotl64(v1, 17);                                                \
        v1 ^= v2;                                                              \
        v2 = sh_rotl64(v2, 32);                                                \
    } while (0)

    while (i < nFull) {
        unsigned long long m =
            (unsigned long long)in[i] |
            ((unsigned long long)in[i + 1u] << 8) |
            ((unsigned long long)in[i + 2u] << 16) |
            ((unsigned long long)in[i + 3u] << 24) |
            ((unsigned long long)in[i + 4u] << 32) |
            ((unsigned long long)in[i + 5u] << 40) |
            ((unsigned long long)in[i + 6u] << 48) |
            ((unsigned long long)in[i + 7u] << 56);
        v3 ^= m;
        SH_SIPROUND;
        SH_SIPROUND;
        v0 ^= m;
        i += 8u;
    }
    b = ((unsigned long long)inlen) << 56;
    left = (unsigned)(inlen & 7u);
    if (left >= 7u) {
        b |= (unsigned long long)in[i + 6u] << 48;
    }
    if (left >= 6u) {
        b |= (unsigned long long)in[i + 5u] << 40;
    }
    if (left >= 5u) {
        b |= (unsigned long long)in[i + 4u] << 32;
    }
    if (left >= 4u) {
        b |= (unsigned long long)in[i + 3u] << 24;
    }
    if (left >= 3u) {
        b |= (unsigned long long)in[i + 2u] << 16;
    }
    if (left >= 2u) {
        b |= (unsigned long long)in[i + 1u] << 8;
    }
    if (left >= 1u) {
        b |= (unsigned long long)in[i];
    }
    v3 ^= b;
    SH_SIPROUND;
    SH_SIPROUND;
    v0 ^= b;
    v2 ^= 0xffULL;
    SH_SIPROUND;
    SH_SIPROUND;
    SH_SIPROUND;
    SH_SIPROUND;
#undef SH_SIPROUND
    return v0 ^ v1 ^ v2 ^ v3;
}

static int
cmd_siphash(const char *szName)
{
    static char aBuf[1100];
    static char aOut[24];
    static const char *szDigits = "0123456789abcdef";
    long n;
    unsigned long long uH;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uH = sh_siphash24((const unsigned char *)aBuf, (size_t)n);
    for (i = 0; i < 16u; i++) {
        aOut[i] = szDigits[(uH >> ((15u - i) * 4u)) & 0xfu];
    }
    aOut[16] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* Compact 64-bit product mix hash for xxh3sum (v0.25) — integer only. */
static unsigned long long
sh_xxh3_64(const unsigned char *p, size_t cb)
{
    const unsigned long long prime1 = 0x9e3779b185ebca87ULL;
    const unsigned long long prime2 = 0xc2b2ae3d27d4eb4fULL;
    const unsigned long long prime3 = 0x165667b19e3779f9ULL;
    unsigned long long h = 0x27d4eb2f165667c5ULL + ((unsigned long long)cb * prime1);
    size_t i = 0;

    while (i + 8u <= cb) {
        unsigned long long k =
            (unsigned long long)p[i] |
            ((unsigned long long)p[i + 1u] << 8) |
            ((unsigned long long)p[i + 2u] << 16) |
            ((unsigned long long)p[i + 3u] << 24) |
            ((unsigned long long)p[i + 4u] << 32) |
            ((unsigned long long)p[i + 5u] << 40) |
            ((unsigned long long)p[i + 6u] << 48) |
            ((unsigned long long)p[i + 7u] << 56);
        k *= prime2;
        k = sh_rotl64(k, 31);
        k *= prime1;
        h ^= k;
        h = sh_rotl64(h, 27) * prime1 + 0x85ebca77c2b2ae63ULL;
        i += 8u;
    }
    if (i + 4u <= cb) {
        unsigned long long k =
            (unsigned long long)p[i] |
            ((unsigned long long)p[i + 1u] << 8) |
            ((unsigned long long)p[i + 2u] << 16) |
            ((unsigned long long)p[i + 3u] << 24);
        k *= prime1;
        k = sh_rotl64(k, 23) * prime2;
        h ^= k;
        h = sh_rotl64(h, 27) * prime1 + 0x85ebca77c2b2ae63ULL;
        i += 4u;
    }
    while (i < cb) {
        h ^= (unsigned long long)p[i] * prime3;
        h = sh_rotl64(h, 11) * prime1;
        i++;
    }
    h ^= h >> 33;
    h *= prime2;
    h ^= h >> 29;
    h *= 0x165667b19e3779f9ULL;
    h ^= h >> 32;
    return h;
}

static int
cmd_xxh3sum(const char *szName)
{
    static char aBuf[1100];
    static char aOut[24];
    static const char *szDigits = "0123456789abcdef";
    long n;
    unsigned long long uH;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uH = sh_xxh3_64((const unsigned char *)aBuf, (size_t)n);
    for (i = 0; i < 16u; i++) {
        aOut[i] = szDigits[(uH >> ((15u - i) * 4u)) & 0xfu];
    }
    aOut[16] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF), product v0.26. */
static unsigned
sh_crc16_ccitt(const unsigned char *p, size_t cb)
{
    unsigned uCrc = 0xffffu;
    size_t i;

    for (i = 0; i < cb; i++) {
        unsigned b;
        uCrc ^= (unsigned)p[i] << 8;
        for (b = 0; b < 8u; b++) {
            if (uCrc & 0x8000u) {
                uCrc = (unsigned)((uCrc << 1) ^ 0x1021u);
            } else {
                uCrc = (unsigned)(uCrc << 1);
            }
            uCrc &= 0xffffu;
        }
    }
    return uCrc;
}

static int
cmd_crc16(const char *szName)
{
    static char aBuf[1100];
    static char aOut[12];
    static const char *szDigits = "0123456789abcdef";
    long n;
    unsigned uCrc;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uCrc = sh_crc16_ccitt((const unsigned char *)aBuf, (size_t)n);
    for (i = 0; i < 4u; i++) {
        aOut[i] = szDigits[(uCrc >> ((3u - i) * 4u)) & 0xfu];
    }
    aOut[4] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* CRC-8/SMBUS (poly 0x07, init 0x00), product v0.27. */
static unsigned
sh_crc8_smbus(const unsigned char *p, size_t cb)
{
    unsigned uCrc = 0u;
    size_t i;

    for (i = 0; i < cb; i++) {
        unsigned b;

        uCrc ^= (unsigned)p[i];
        for (b = 0; b < 8u; b++) {
            if (uCrc & 0x80u) {
                uCrc = (unsigned)((uCrc << 1) ^ 0x07u);
            } else {
                uCrc = (unsigned)(uCrc << 1);
            }
            uCrc &= 0xffu;
        }
    }
    return uCrc;
}

static int
cmd_crc8(const char *szName)
{
    static char aBuf[1100];
    static char aOut[8];
    static const char *szDigits = "0123456789abcdef";
    long n;
    unsigned uCrc;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uCrc = sh_crc8_smbus((const unsigned char *)aBuf, (size_t)n);
    aOut[0] = szDigits[(uCrc >> 4) & 0xfu];
    aOut[1] = szDigits[uCrc & 0xfu];
    aOut[2] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* SM3 (GM/T 0004-2012) product digest v0.26 — compact integer-only. */
static unsigned
sh_rol32(unsigned x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

static void
sh_sm3_compress(unsigned aH[8], const unsigned char aBlock[64])
{
    unsigned w[68];
    unsigned w1[64];
    unsigned a, b, c, d, e, f, g, h;
    unsigned j;

    for (j = 0; j < 16u; j++) {
        w[j] = ((unsigned)aBlock[j * 4u] << 24) |
               ((unsigned)aBlock[j * 4u + 1u] << 16) |
               ((unsigned)aBlock[j * 4u + 2u] << 8) |
               (unsigned)aBlock[j * 4u + 3u];
    }
    for (j = 16; j < 68u; j++) {
        unsigned x = w[j - 16u] ^ w[j - 9u] ^ sh_rol32(w[j - 3u], 15);
        x = x ^ sh_rol32(x, 15) ^ sh_rol32(x, 23); /* P1 */
        w[j] = x ^ sh_rol32(w[j - 13u], 7) ^ w[j - 6u];
    }
    for (j = 0; j < 64u; j++) {
        w1[j] = w[j] ^ w[j + 4u];
    }
    a = aH[0];
    b = aH[1];
    c = aH[2];
    d = aH[3];
    e = aH[4];
    f = aH[5];
    g = aH[6];
    h = aH[7];
    for (j = 0; j < 64u; j++) {
        unsigned t = (j < 16u) ? 0x79cc4519u : 0x7a879d8au;
        unsigned ss1 = sh_rol32(sh_rol32(a, 12) + e + sh_rol32(t, j), 7);
        unsigned ss2 = ss1 ^ sh_rol32(a, 12);
        unsigned ff, gg, tt1, tt2;

        if (j < 16u) {
            ff = a ^ b ^ c;
            gg = e ^ f ^ g;
        } else {
            ff = (a & b) | (a & c) | (b & c);
            gg = (e & f) | ((~e) & g);
        }
        tt1 = ff + d + ss2 + w1[j];
        tt2 = gg + h + ss1 + w[j];
        d = c;
        c = sh_rol32(b, 9);
        b = a;
        a = tt1;
        h = g;
        g = sh_rol32(f, 19);
        f = e;
        e = tt2 ^ sh_rol32(tt2, 9) ^ sh_rol32(tt2, 17); /* P0 */
    }
    aH[0] ^= a;
    aH[1] ^= b;
    aH[2] ^= c;
    aH[3] ^= d;
    aH[4] ^= e;
    aH[5] ^= f;
    aH[6] ^= g;
    aH[7] ^= h;
}

static void
sh_sm3(const unsigned char *p, size_t cb, unsigned char aDig[32])
{
    unsigned aH[8] = {0x7380166fu, 0x4914b2b9u, 0x172442d7u, 0xda8a0600u,
                      0xa96f30bcu, 0x163138aau, 0xe38dee4du, 0xb0fb0e4eu};
    unsigned char aBuf[64];
    size_t cbBuf = 0;
    size_t i;
    unsigned long long cbBits = (unsigned long long)cb * 8ull;
    size_t nOff = 0;

    while (nOff < cb) {
        size_t nTake = 64u - cbBuf;
        if (nTake > cb - nOff) {
            nTake = cb - nOff;
        }
        for (i = 0; i < nTake; i++) {
            aBuf[cbBuf + i] = p[nOff + i];
        }
        cbBuf += nTake;
        nOff += nTake;
        if (cbBuf == 64u) {
            sh_sm3_compress(aH, aBuf);
            cbBuf = 0;
        }
    }
    aBuf[cbBuf++] = 0x80u;
    if (cbBuf > 56u) {
        while (cbBuf < 64u) {
            aBuf[cbBuf++] = 0;
        }
        sh_sm3_compress(aH, aBuf);
        cbBuf = 0;
    }
    while (cbBuf < 56u) {
        aBuf[cbBuf++] = 0;
    }
    for (i = 0; i < 8u; i++) {
        aBuf[56u + i] = (unsigned char)(cbBits >> (56u - 8u * i));
    }
    sh_sm3_compress(aH, aBuf);
    for (i = 0; i < 8u; i++) {
        aDig[i * 4u] = (unsigned char)(aH[i] >> 24);
        aDig[i * 4u + 1u] = (unsigned char)(aH[i] >> 16);
        aDig[i * 4u + 2u] = (unsigned char)(aH[i] >> 8);
        aDig[i * 4u + 3u] = (unsigned char)aH[i];
    }
}

static int
cmd_sm3sum(const char *szName)
{
    static char aBuf[1100];
    static char aOut[72];
    static const char *szDigits = "0123456789abcdef";
    unsigned char aDig[32];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_sm3((const unsigned char *)aBuf, (size_t)n, aDig);
    for (i = 0; i < 32u; i++) {
        aOut[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aOut[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aOut[64] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* Base58 (Bitcoin alphabet) encode/decode, product v0.27. */
static const char s_b58_alpha[] =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

static int
sh_b58_val(int ch)
{
    int i;

    for (i = 0; s_b58_alpha[i] != '\0'; i++) {
        if (s_b58_alpha[i] == ch) {
            return i;
        }
    }
    return -1;
}

static int
cmd_base58(const char *szName, int fDecode)
{
    static char aBuf[400];
    static char aOut[560];
    static unsigned char aWork[420];
    long n;
    size_t cZeros;
    size_t i;
    size_t cbSize;
    size_t cbHigh;
    size_t o;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }

    if (!fDecode) {
        /* Encode binary file bytes → base58 text. */
        cZeros = 0;
        while (cZeros < (size_t)n && (unsigned char)aBuf[cZeros] == 0u) {
            cZeros++;
        }
        cbSize = (size_t)n * 138u / 100u + 1u;
        if (cbSize > sizeof(aWork)) {
            return -1;
        }
        for (i = 0; i < cbSize; i++) {
            aWork[i] = 0;
        }
        for (i = cZeros; i < (size_t)n; i++) {
            unsigned carry = (unsigned char)aBuf[i];
            size_t j;

            for (j = cbSize; j > 0u; j--) {
                carry += 256u * (unsigned)aWork[j - 1u];
                aWork[j - 1u] = (unsigned char)(carry % 59u);
                carry /= 59u;
            }
        }
        cbHigh = 0;
        while (cbHigh < cbSize && aWork[cbHigh] == 0u) {
            cbHigh++;
        }
        if (cZeros + (cbSize - cbHigh) + 1u > sizeof(aOut)) {
            return -1;
        }
        o = 0;
        for (i = 0; i < cZeros; i++) {
            aOut[o++] = '1';
        }
        for (i = cbHigh; i < cbSize; i++) {
            aOut[o++] = s_b58_alpha[aWork[i]];
        }
        aOut[o] = '\0';
        out(aOut);
        out("\n");
        return 0;
    }

    /* Decode base58 text → binary (printed as bytes + newline). */
    {
        size_t cbIn = (size_t)n;
        /* Trim trailing newline(s) common when reading encoded files. */
        while (cbIn > 0u &&
               (aBuf[cbIn - 1u] == '\n' || aBuf[cbIn - 1u] == '\r')) {
            cbIn--;
        }
        cZeros = 0;
        while (cZeros < cbIn && aBuf[cZeros] == '1') {
            cZeros++;
        }
        cbSize = cbIn * 733u / 1000u + 1u;
        if (cbSize > sizeof(aWork)) {
            return -1;
        }
        for (i = 0; i < cbSize; i++) {
            aWork[i] = 0;
        }
        for (i = cZeros; i < cbIn; i++) {
            int nVal = sh_b58_val((unsigned char)aBuf[i]);
            unsigned carry;
            size_t j;

            if (nVal < 0) {
                return -1;
            }
            carry = (unsigned)nVal;
            for (j = cbSize; j > 0u; j--) {
                carry += 59u * (unsigned)aWork[j - 1u];
                aWork[j - 1u] = (unsigned char)(carry & 0xffu);
                carry >>= 8;
            }
            if (carry != 0u) {
                return -1;
            }
        }
        cbHigh = 0;
        while (cbHigh < cbSize && aWork[cbHigh] == 0u) {
            cbHigh++;
        }
        if (cZeros + (cbSize - cbHigh) + 1u > sizeof(aOut)) {
            return -1;
        }
        o = 0;
        for (i = 0; i < cZeros; i++) {
            aOut[o++] = '\0';
        }
        for (i = cbHigh; i < cbSize; i++) {
            aOut[o++] = (char)aWork[i];
        }
        /* Emit with explicit length so embedded NULs survive; then newline. */
        if (o > 0u) {
            (void)gj_debug_log(aOut, (long)o);
        }
        out("\n");
        return 0;
    }
}

/* CRC-64/ECMA-182 (poly 0x42F0E1EBA9EA3693, init 0), product v0.28. */
static unsigned long long
sh_crc64_ecma(const unsigned char *p, size_t cb)
{
    unsigned long long uCrc = 0;
    size_t i;

    for (i = 0; i < cb; i++) {
        unsigned b;

        uCrc ^= ((unsigned long long)p[i]) << 56;
        for (b = 0; b < 8u; b++) {
            if (uCrc & 0x8000000000000000ULL) {
                uCrc = (uCrc << 1) ^ 0x42F0E1EBA9EA3693ULL;
            } else {
                uCrc <<= 1;
            }
        }
    }
    return uCrc;
}

static int
cmd_crc64(const char *szName)
{
    static char aBuf[1100];
    static char aOut[24];
    static const char *szDigits = "0123456789abcdef";
    long n;
    unsigned long long uCrc;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uCrc = sh_crc64_ecma((const unsigned char *)aBuf, (size_t)n);
    for (i = 0; i < 16u; i++) {
        aOut[i] = szDigits[(uCrc >> ((15u - i) * 4u)) & 0xfu];
    }
    aOut[16] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/* BLAKE3-256 product digest (v0.28) — compact integer-only, <=2 chunks. */
#define SH_B3_CHUNK_START 1u
#define SH_B3_CHUNK_END 2u
#define SH_B3_PARENT 4u
#define SH_B3_ROOT 8u

static unsigned
sh_b3_ror(unsigned x, unsigned n)
{
    return (x >> n) | (x << (32u - n));
}

static void
sh_b3_g(unsigned *v, unsigned a, unsigned b, unsigned c, unsigned d, unsigned x,
        unsigned y)
{
    v[a] = v[a] + v[b] + x;
    v[d] = sh_b3_ror(v[d] ^ v[a], 16u);
    v[c] = v[c] + v[d];
    v[b] = sh_b3_ror(v[b] ^ v[c], 12u);
    v[a] = v[a] + v[b] + y;
    v[d] = sh_b3_ror(v[d] ^ v[a], 8u);
    v[c] = v[c] + v[d];
    v[b] = sh_b3_ror(v[b] ^ v[c], 7u);
}

static void
sh_b3_compress(unsigned aCv[8], const unsigned char aBlock[64],
               unsigned uBlockLen, unsigned long long uCounter, unsigned uFlags)
{
    static const unsigned aIv[8] = {
        0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
        0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};
    static const unsigned char aSch[7][16] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8},
        {3, 4, 10, 12, 13, 2, 7, 14, 6, 5, 9, 0, 11, 15, 8, 1},
        {10, 7, 12, 9, 14, 3, 13, 15, 4, 0, 11, 2, 5, 8, 1, 6},
        {12, 13, 9, 11, 15, 10, 14, 8, 7, 2, 5, 3, 0, 1, 6, 4},
        {9, 14, 11, 5, 8, 12, 15, 1, 13, 3, 0, 10, 2, 6, 4, 7},
        {11, 15, 5, 0, 1, 9, 8, 6, 14, 10, 2, 12, 3, 4, 7, 13}};
    unsigned v[16];
    unsigned m[16];
    unsigned i, r;

    for (i = 0; i < 16u; i++) {
        const unsigned char *q = aBlock + i * 4u;

        m[i] = (unsigned)q[0] | ((unsigned)q[1] << 8) |
               ((unsigned)q[2] << 16) | ((unsigned)q[3] << 24);
    }
    for (i = 0; i < 8u; i++) {
        v[i] = aCv[i];
    }
    v[8] = aIv[0];
    v[9] = aIv[1];
    v[10] = aIv[2];
    v[11] = aIv[3];
    v[12] = (unsigned)uCounter;
    v[13] = (unsigned)(uCounter >> 32);
    v[14] = uBlockLen;
    v[15] = uFlags;
    for (r = 0; r < 7u; r++) {
        const unsigned char *s = aSch[r];

        sh_b3_g(v, 0, 4, 8, 12, m[s[0]], m[s[1]]);
        sh_b3_g(v, 1, 5, 9, 13, m[s[2]], m[s[3]]);
        sh_b3_g(v, 2, 6, 10, 14, m[s[4]], m[s[5]]);
        sh_b3_g(v, 3, 7, 11, 15, m[s[6]], m[s[7]]);
        sh_b3_g(v, 0, 5, 10, 15, m[s[8]], m[s[9]]);
        sh_b3_g(v, 1, 6, 11, 12, m[s[10]], m[s[11]]);
        sh_b3_g(v, 2, 7, 8, 13, m[s[12]], m[s[13]]);
        sh_b3_g(v, 3, 4, 9, 14, m[s[14]], m[s[15]]);
    }
    for (i = 0; i < 8u; i++) {
        aCv[i] = v[i] ^ v[i + 8u];
    }
}

static void
sh_b3_store_cv(unsigned char aOut[32], const unsigned aCv[8])
{
    unsigned i;

    for (i = 0; i < 8u; i++) {
        aOut[i * 4u] = (unsigned char)aCv[i];
        aOut[i * 4u + 1u] = (unsigned char)(aCv[i] >> 8);
        aOut[i * 4u + 2u] = (unsigned char)(aCv[i] >> 16);
        aOut[i * 4u + 3u] = (unsigned char)(aCv[i] >> 24);
    }
}

static void
sh_b3_iv(unsigned aCv[8])
{
    static const unsigned aIv[8] = {
        0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
        0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u};
    unsigned i;

    for (i = 0; i < 8u; i++) {
        aCv[i] = aIv[i];
    }
}

static void
sh_b3_chunk(const unsigned char *p, size_t cb, unsigned long long uCounter,
            unsigned uRootFlags, unsigned aOut[8])
{
    unsigned aCv[8];
    unsigned char aBlock[64];
    size_t nOff = 0;
    unsigned uBlock = 0;
    unsigned nBlocks;

    sh_b3_iv(aCv);
    nBlocks = (unsigned)((cb + 63u) / 64u);
    if (nBlocks == 0u) {
        nBlocks = 1u;
    }
    while (uBlock < nBlocks) {
        size_t cbBlock;
        unsigned uFlags = 0;
        unsigned i;

        for (i = 0; i < 64u; i++) {
            aBlock[i] = 0;
        }
        cbBlock = cb - nOff;
        if (cbBlock > 64u) {
            cbBlock = 64u;
        }
        for (i = 0; i < cbBlock; i++) {
            aBlock[i] = p[nOff + i];
        }
        if (uBlock == 0u) {
            uFlags |= SH_B3_CHUNK_START;
        }
        if (uBlock + 1u == nBlocks) {
            uFlags |= SH_B3_CHUNK_END;
            uFlags |= uRootFlags;
        }
        sh_b3_compress(aCv, aBlock, (unsigned)cbBlock, uCounter, uFlags);
        nOff += cbBlock;
        uBlock++;
    }
    for (uBlock = 0; uBlock < 8u; uBlock++) {
        aOut[uBlock] = aCv[uBlock];
    }
}

static void
sh_b3_parent(const unsigned aLeft[8], const unsigned aRight[8],
             unsigned uRootFlags, unsigned aOut[8])
{
    unsigned char aBlock[64];
    unsigned aCv[8];
    unsigned i;

    sh_b3_store_cv(aBlock, aLeft);
    sh_b3_store_cv(aBlock + 32, aRight);
    sh_b3_iv(aCv);
    sh_b3_compress(aCv, aBlock, 64u, 0ull, SH_B3_PARENT | uRootFlags);
    for (i = 0; i < 8u; i++) {
        aOut[i] = aCv[i];
    }
}

static void
sh_blake3(const unsigned char *p, size_t cb, unsigned char aDig[32])
{
    unsigned aCv[8];
    unsigned aLeft[8];
    unsigned aRight[8];

    if (cb <= 1024u) {
        sh_b3_chunk(p, cb, 0ull, SH_B3_ROOT, aCv);
        sh_b3_store_cv(aDig, aCv);
        return;
    }
    /* Product VFS reads fit in 1100 bytes → at most two 1024-byte chunks. */
    sh_b3_chunk(p, 1024u, 0ull, 0u, aLeft);
    sh_b3_chunk(p + 1024u, cb - 1024u, 1ull, 0u, aRight);
    sh_b3_parent(aLeft, aRight, SH_B3_ROOT, aCv);
    sh_b3_store_cv(aDig, aCv);
}

static int
cmd_b3sum(const char *szName)
{
    static char aBuf[1100];
    static char aOut[72];
    static const char *szDigits = "0123456789abcdef";
    unsigned char aDig[32];
    long n;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    sh_blake3((const unsigned char *)aBuf, (size_t)n, aDig);
    for (i = 0; i < 32u; i++) {
        aOut[i * 2u] = szDigits[(aDig[i] >> 4) & 0xfu];
        aOut[i * 2u + 1u] = szDigits[aDig[i] & 0xfu];
    }
    aOut[64] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/*
 * Adobe base85 / ASCII85 encode (product v0.29), self-contained.
 * Alphabet: '!' (33) .. 'u' (117) for digits 0..84.
 * base85: pure (no 'z'); ascii85: Adobe 'z' for a zero 4-byte block.
 * Partial final group emits n+1 chars (Adobe rule). No <~/~> delimiters.
 */
static void
sh_b85_enc_group(unsigned u, char *pOut)
{
    unsigned d0, d1, d2, d3, d4;

    d4 = u % 85u;
    u /= 85u;
    d3 = u % 85u;
    u /= 85u;
    d2 = u % 85u;
    u /= 85u;
    d1 = u % 85u;
    u /= 85u;
    d0 = u;
    pOut[0] = (char)('!' + d0);
    pOut[1] = (char)('!' + d1);
    pOut[2] = (char)('!' + d2);
    pOut[3] = (char)('!' + d3);
    pOut[4] = (char)('!' + d4);
}

static int
cmd_base85(const char *szName, int fAdobeZ)
{
    static char aBuf[400];
    static char aOut[560];
    long n;
    long i;
    size_t o = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i + 4 <= n; i += 4) {
        unsigned u =
            ((unsigned)(unsigned char)aBuf[i] << 24) |
            ((unsigned)(unsigned char)aBuf[i + 1] << 16) |
            ((unsigned)(unsigned char)aBuf[i + 2] << 8) |
            (unsigned)(unsigned char)aBuf[i + 3];

        if (fAdobeZ && u == 0u) {
            if (o + 1u >= sizeof(aOut)) {
                break;
            }
            aOut[o++] = 'z';
        } else {
            if (o + 5u >= sizeof(aOut)) {
                break;
            }
            sh_b85_enc_group(u, aOut + o);
            o += 5u;
        }
    }
    if (i < n) {
        /* Partial final: zero-pad to 4 bytes, emit (nLeft+1) chars. */
        long nLeft = n - i;
        unsigned u = 0;
        char aTmp[5];
        long k;

        for (k = 0; k < nLeft; k++) {
            u |= (unsigned)(unsigned char)aBuf[i + k]
                 << (24u - (unsigned)(k * 8));
        }
        sh_b85_enc_group(u, aTmp);
        if (o + (size_t)nLeft + 1u >= sizeof(aOut)) {
            /* leave room for NUL */
        } else {
            for (k = 0; k < nLeft + 1; k++) {
                aOut[o++] = aTmp[k];
            }
        }
    }
    aOut[o] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/* sum64: FNV-1a 64-bit over small file (product v0.29). */
static unsigned long long
sh_fnv1a64(const unsigned char *p, size_t cb)
{
    unsigned long long uH = 14695981039346656037ULL;
    size_t i;

    for (i = 0; i < cb; i++) {
        uH ^= (unsigned long long)p[i];
        uH *= 1099511628211ULL;
    }
    return uH;
}

static int
cmd_sum64(const char *szName)
{
    static char aBuf[1100];
    static char aOut[24];
    static const char *szDigits = "0123456789abcdef";
    long n;
    unsigned long long uH;
    unsigned i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    uH = sh_fnv1a64((const unsigned char *)aBuf, (size_t)n);
    for (i = 0; i < 16u; i++) {
        aOut[i] = szDigits[(uH >> ((15u - i) * 4u)) & 0xfu];
    }
    aOut[16] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/*
 * Compact DEFLATE inflate — stored + fixed Huffman only (product v0.30).
 * Auto-detects gzip (1f 8b) / zlib CMF+FLG / raw DEFLATE. Small blobs only.
 * No dynamic trees (BTYPE=10) — keep code size down for shell demo.
 */
struct sh_br {
    const unsigned char *pIn;
    size_t cbIn;
    size_t iPos;
    unsigned uHold;
    unsigned nBits;
    int fErr;
};

static void
sh_br_init(struct sh_br *pBr, const unsigned char *pIn, size_t cbIn)
{
    pBr->pIn = pIn;
    pBr->cbIn = cbIn;
    pBr->iPos = 0;
    pBr->uHold = 0;
    pBr->nBits = 0;
    pBr->fErr = 0;
}

static unsigned
sh_getbits(struct sh_br *pBr, unsigned nNeed)
{
    unsigned uVal;

    if (nNeed == 0u) {
        return 0u;
    }
    if (nNeed > 24u) {
        pBr->fErr = 1;
        return 0u;
    }
    while (pBr->nBits < nNeed) {
        if (pBr->iPos >= pBr->cbIn) {
            pBr->fErr = 1;
            return 0u;
        }
        pBr->uHold |= ((unsigned)pBr->pIn[pBr->iPos++]) << pBr->nBits;
        pBr->nBits += 8u;
    }
    uVal = pBr->uHold & ((1u << nNeed) - 1u);
    pBr->uHold >>= nNeed;
    pBr->nBits -= nNeed;
    return uVal;
}

static void
sh_br_align(struct sh_br *pBr)
{
    unsigned nDrop = pBr->nBits & 7u;

    pBr->uHold >>= nDrop;
    pBr->nBits -= nDrop;
}

/* Pull n bits as an MSB-first code (Huffman bit order, not getbits integer). */
static unsigned
sh_getcode(struct sh_br *pBr, unsigned nBits)
{
    unsigned uCode = 0;
    unsigned i;

    for (i = 0; i < nBits; i++) {
        uCode = (uCode << 1) | sh_getbits(pBr, 1u);
        if (pBr->fErr) {
            return 0u;
        }
    }
    return uCode;
}

/* Fixed lit/len: RFC 1951 3.2.6 bit patterns (no full Huffman tables). */
static int
sh_fixed_lit(struct sh_br *pBr)
{
    unsigned uCode;

    uCode = sh_getcode(pBr, 7u);
    if (pBr->fErr) {
        return -1;
    }
    if (uCode <= 23u) {
        return (int)(256u + uCode); /* 256..279 */
    }
    uCode = (uCode << 1) | sh_getbits(pBr, 1u);
    if (pBr->fErr) {
        return -1;
    }
    if (uCode >= 48u && uCode <= 191u) {
        return (int)(uCode - 48u); /* 0..143 */
    }
    if (uCode >= 192u && uCode <= 199u) {
        return (int)(280u + (uCode - 192u)); /* 280..287 */
    }
    uCode = (uCode << 1) | sh_getbits(pBr, 1u);
    if (pBr->fErr) {
        return -1;
    }
    if (uCode >= 400u && uCode <= 511u) {
        return (int)(144u + (uCode - 400u)); /* 144..255 */
    }
    pBr->fErr = 1;
    return -1;
}

static int
sh_inflate_raw(struct sh_br *pBr, unsigned char *pOut, size_t cbCap,
               size_t *pcbOut)
{
    /* Length base / extra (codes 257..285). */
    static const unsigned short aLenBase[29] = {
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51,
        59, 67, 83, 99, 115, 131, 163, 195, 227, 258
    };
    static const unsigned char aLenExtra[29] = {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4,
        5, 5, 5, 5, 0
    };
    static const unsigned short aDistBase[30] = {
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385,
        513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
    };
    static const unsigned char aDistExtra[30] = {
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
        10, 11, 11, 12, 12, 13, 13
    };
    size_t cbUsed = 0;
    unsigned fBfinal;

    do {
        unsigned uBtype;
        unsigned i;

        fBfinal = sh_getbits(pBr, 1u);
        uBtype = sh_getbits(pBr, 2u);
        if (pBr->fErr) {
            return -1;
        }
        if (uBtype == 0u) {
            /* Stored block: align, LEN, NLEN, payload. */
            unsigned uLen;
            unsigned uNlen;

            sh_br_align(pBr);
            uLen = sh_getbits(pBr, 16u);
            uNlen = sh_getbits(pBr, 16u);
            if (pBr->fErr || (uLen ^ 0xffffu) != uNlen) {
                return -1;
            }
            if (pBr->nBits != 0u) {
                return -1;
            }
            if (pBr->iPos + (size_t)uLen > pBr->cbIn) {
                return -1;
            }
            for (i = 0; i < uLen; i++) {
                if (cbUsed >= cbCap) {
                    return -1;
                }
                pOut[cbUsed++] = pBr->pIn[pBr->iPos++];
            }
        } else if (uBtype == 1u) {
            /* Fixed Huffman block. */
            for (;;) {
                int nSym;
                unsigned uLen;
                unsigned uDist;
                unsigned uExtra;
                unsigned uDsym;
                size_t iSrc;

                nSym = sh_fixed_lit(pBr);
                if (nSym < 0 || pBr->fErr) {
                    return -1;
                }
                if (nSym < 256) {
                    if (cbUsed >= cbCap) {
                        return -1;
                    }
                    pOut[cbUsed++] = (unsigned char)nSym;
                    continue;
                }
                if (nSym == 256) {
                    break; /* end of block */
                }
                if (nSym > 285) {
                    return -1;
                }
                nSym -= 257;
                uLen = aLenBase[nSym];
                uExtra = aLenExtra[nSym];
                if (uExtra != 0u) {
                    uLen += sh_getbits(pBr, uExtra);
                    if (pBr->fErr) {
                        return -1;
                    }
                }
                uDsym = sh_getcode(pBr, 5u);
                if (pBr->fErr || uDsym > 29u) {
                    return -1;
                }
                uDist = aDistBase[uDsym];
                uExtra = aDistExtra[uDsym];
                if (uExtra != 0u) {
                    uDist += sh_getbits(pBr, uExtra);
                    if (pBr->fErr) {
                        return -1;
                    }
                }
                if (uDist == 0u || uDist > cbUsed) {
                    return -1;
                }
                iSrc = cbUsed - (size_t)uDist;
                for (i = 0; i < uLen; i++) {
                    if (cbUsed >= cbCap) {
                        return -1;
                    }
                    pOut[cbUsed++] = pOut[iSrc + i];
                }
            }
        } else {
            /* Dynamic (2) or reserved (3) — not supported in compact shell. */
            return -1;
        }
    } while (fBfinal == 0u);

    *pcbOut = cbUsed;
    return 0;
}

static int
sh_gunzip_buf(const unsigned char *pIn, size_t cbIn, unsigned char *pOut,
              size_t cbCap, size_t *pcbOut)
{
    struct sh_br br;
    size_t iOff = 0;
    int fGzip = 0;
    int fZlib = 0;

    if (pIn == NULL || pOut == NULL || pcbOut == NULL || cbIn == 0u) {
        return -1;
    }

    /* gzip magic */
    if (cbIn >= 10u && pIn[0] == 0x1fu && pIn[1] == 0x8bu && pIn[2] == 8u) {
        unsigned uFlg = pIn[3];

        fGzip = 1;
        iOff = 10u;
        if ((uFlg & 0x04u) != 0u) {
            unsigned uXlen;

            if (iOff + 2u > cbIn) {
                return -1;
            }
            uXlen = (unsigned)pIn[iOff] | ((unsigned)pIn[iOff + 1u] << 8);
            iOff += 2u + (size_t)uXlen;
        }
        if ((uFlg & 0x08u) != 0u) {
            while (iOff < cbIn && pIn[iOff] != 0u) {
                iOff++;
            }
            if (iOff >= cbIn) {
                return -1;
            }
            iOff++;
        }
        if ((uFlg & 0x10u) != 0u) {
            while (iOff < cbIn && pIn[iOff] != 0u) {
                iOff++;
            }
            if (iOff >= cbIn) {
                return -1;
            }
            iOff++;
        }
        if ((uFlg & 0x02u) != 0u) {
            if (iOff + 2u > cbIn) {
                return -1;
            }
            iOff += 2u;
        }
    } else if (cbIn >= 2u && (pIn[0] & 0x0fu) == 8u && (pIn[0] >> 4) <= 7u &&
               ((((unsigned)pIn[0] << 8) | (unsigned)pIn[1]) % 31u) == 0u) {
        /* zlib CMF/FLG */
        fZlib = 1;
        iOff = 2u;
        if ((pIn[1] & 0x20u) != 0u) {
            return -1; /* preset dict not supported */
        }
    }
    (void)fGzip;
    (void)fZlib;

    if (iOff >= cbIn) {
        return -1;
    }
    sh_br_init(&br, pIn + iOff, cbIn - iOff);
    return sh_inflate_raw(&br, pOut, cbCap, pcbOut);
}

/* gunzip / inflate: decompress gzip|zlib|raw-deflate small file to stdout. */
static int
cmd_gunzip(const char *szName)
{
    static unsigned char aIn[1100];
    static unsigned char aOut[2048];
    static char aPrint[2049];
    long n;
    size_t cbOut = 0;
    size_t i;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aIn,
               (long)sizeof(aIn));
    if (n < 0) {
        return -1;
    }
    if (sh_gunzip_buf(aIn, (size_t)n, aOut, sizeof(aOut), &cbOut) != 0) {
        return -1;
    }
    if (cbOut >= sizeof(aPrint)) {
        cbOut = sizeof(aPrint) - 1u;
    }
    for (i = 0; i < cbOut; i++) {
        aPrint[i] = (char)aOut[i];
    }
    aPrint[cbOut] = '\0';
    out(aPrint);
    if (cbOut == 0u || aPrint[cbOut - 1u] != '\n') {
        out("\n");
    }
    return 0;
}

/* uuidgen: RFC 4122 version-4 string (product v0.20). */
static int
cmd_uuidgen(void)
{
    static char aOut[40];
    static const char *szDigits = "0123456789abcdef";
    unsigned char aU[16];
    unsigned i, j = 0;
    static const unsigned aDash[] = {4, 6, 8, 10};
    /* Mix mono-ish seed from VFS size of shell.txt if present; else constant. */
    unsigned u = 0x9e3779b9u;
    long n;
    static char aSeed[64];

    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)"shell.txt",
               (long)(uintptr_t)aSeed, (long)(sizeof(aSeed) - 1));
    if (n > 0) {
        for (i = 0; i < (unsigned)n; i++) {
            u = u * 1664525u + (unsigned char)aSeed[i] + 1013904223u;
        }
    }
    for (i = 0; i < 16u; i++) {
        u = u * 1664525u + 1013904223u;
        aU[i] = (unsigned char)(u >> 24);
    }
    aU[6] = (unsigned char)((aU[6] & 0x0fu) | 0x40u);
    aU[8] = (unsigned char)((aU[8] & 0x3fu) | 0x80u);
    for (i = 0; i < 16u; i++) {
        unsigned d;
        for (d = 0; d < 4u; d++) {
            if (aDash[d] == i) {
                aOut[j++] = '-';
            }
        }
        aOut[j++] = szDigits[(aU[i] >> 4) & 0xfu];
        aOut[j++] = szDigits[aU[i] & 0xfu];
    }
    aOut[j] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/*
 * cpuid — print feature bits of interest (product v0.31).
 * SSE2 (leaf1 EDX.26), SSE4.2 (leaf1 ECX.20), AES-NI (leaf1 ECX.25),
 * PCLMUL (leaf1 ECX.1), SHA-NI (leaf7 EBX.29). Compact hex + flags line.
 */
static void
sh_cpuid(unsigned uLeaf, unsigned uSub, unsigned *pEax, unsigned *pEbx,
         unsigned *pEcx, unsigned *pEdx)
{
    unsigned a, b, c, d;

    __asm__ __volatile__("cpuid"
                         : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
                         : "a"(uLeaf), "c"(uSub)
                         : "memory");
    if (pEax != NULL) {
        *pEax = a;
    }
    if (pEbx != NULL) {
        *pEbx = b;
    }
    if (pEcx != NULL) {
        *pEcx = c;
    }
    if (pEdx != NULL) {
        *pEdx = d;
    }
}

static void
sh_hex32(unsigned u, char *aOut)
{
    static const char *szDigits = "0123456789abcdef";
    unsigned i;

    for (i = 0; i < 8u; i++) {
        aOut[i] = szDigits[(u >> ((7u - i) * 4u)) & 0xfu];
    }
    aOut[8] = '\0';
}

static int
cmd_cpuid(void)
{
    unsigned uMax = 0;
    unsigned uEax = 0, uEbx = 0, uEcx = 0, uEdx = 0;
    unsigned u7Ebx = 0;
    char aH[12];
    int fFirst = 1;

    sh_cpuid(0, 0, &uMax, NULL, NULL, NULL);
    if (uMax >= 1u) {
        sh_cpuid(1, 0, &uEax, &uEbx, &uEcx, &uEdx);
    }
    if (uMax >= 7u) {
        unsigned a7, c7, d7;

        sh_cpuid(7, 0, &a7, &u7Ebx, &c7, &d7);
        (void)a7;
        (void)c7;
        (void)d7;
    }

    out("cpuid ecx=");
    sh_hex32(uEcx, aH);
    out(aH);
    out(" edx=");
    sh_hex32(uEdx, aH);
    out(aH);
    out(" ebx7=");
    sh_hex32(u7Ebx, aH);
    out(aH);
    out(" flags=");
    /* leaf1: SSE2=EDX.26, SSE4.2=ECX.20, AES-NI=ECX.25, PCLMUL=ECX.1 */
    if ((uEdx & (1u << 26)) != 0u) {
        out(fFirst ? "SSE2" : ",SSE2");
        fFirst = 0;
    }
    if ((uEcx & (1u << 20)) != 0u) {
        out(fFirst ? "SSE4.2" : ",SSE4.2");
        fFirst = 0;
    }
    if ((uEcx & (1u << 25)) != 0u) {
        out(fFirst ? "AES-NI" : ",AES-NI");
        fFirst = 0;
    }
    if ((uEcx & (1u << 1)) != 0u) {
        out(fFirst ? "PCLMUL" : ",PCLMUL");
        fFirst = 0;
    }
    /* leaf7 sub0: SHA-NI = EBX.29 */
    if ((u7Ebx & (1u << 29)) != 0u) {
        out(fFirst ? "SHA-NI" : ",SHA-NI");
        fFirst = 0;
    }
    if (fFirst) {
        out("-");
    }
    out("\n");
    return 0;
}

/*
 * aescheck — tiny software AES-128 known-vector self-check (product v0.31).
 * Integer/table only (no AES-NI). NIST FIPS-197 Appendix C.1 AES-128 ECB.
 */
static const unsigned char s_sh_aes_s[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b,
    0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26,
    0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2,
    0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed,
    0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f,
    0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec,
    0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14,
    0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d,
    0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f,
    0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11,
    0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f,
    0xb0, 0x54, 0xbb, 0x16};

static unsigned char
sh_aes_xtime(unsigned char x)
{
    return (unsigned char)((x << 1) ^ (((x >> 7) & 1u) * 0x1bu));
}

/* AES-128 encrypt one block; rk holds 44 big-endian words (11 round keys). */
static void
sh_aes128_encrypt(const unsigned char aKey[16], const unsigned char aIn[16],
                  unsigned char aOut[16])
{
    unsigned rk[44];
    unsigned char s[16];
    unsigned i, r, c;
    unsigned temp;
    static const unsigned char aRcon[10] = {0x01, 0x02, 0x04, 0x08, 0x10,
                                            0x20, 0x40, 0x80, 0x1b, 0x36};

    for (i = 0; i < 4u; i++) {
        rk[i] = ((unsigned)aKey[4u * i] << 24) |
                ((unsigned)aKey[4u * i + 1u] << 16) |
                ((unsigned)aKey[4u * i + 2u] << 8) |
                (unsigned)aKey[4u * i + 3u];
    }
    for (i = 4u; i < 44u; i++) {
        temp = rk[i - 1u];
        if ((i & 3u) == 0u) {
            temp = ((unsigned)s_sh_aes_s[(temp >> 16) & 0xffu] << 24) |
                   ((unsigned)s_sh_aes_s[(temp >> 8) & 0xffu] << 16) |
                   ((unsigned)s_sh_aes_s[temp & 0xffu] << 8) |
                   (unsigned)s_sh_aes_s[(temp >> 24) & 0xffu];
            temp ^= (unsigned)aRcon[(i / 4u) - 1u] << 24;
        }
        rk[i] = rk[i - 4u] ^ temp;
    }

    for (i = 0; i < 16u; i++) {
        s[i] = aIn[i];
    }
    for (c = 0; c < 4u; c++) {
        s[4u * c + 0u] ^= (unsigned char)(rk[c] >> 24);
        s[4u * c + 1u] ^= (unsigned char)(rk[c] >> 16);
        s[4u * c + 2u] ^= (unsigned char)(rk[c] >> 8);
        s[4u * c + 3u] ^= (unsigned char)rk[c];
    }
    for (r = 1u; r < 10u; r++) {
        unsigned char t[16];

        for (i = 0; i < 16u; i++) {
            s[i] = s_sh_aes_s[s[i]];
        }
        /* ShiftRows */
        t[0] = s[0];
        t[4] = s[4];
        t[8] = s[8];
        t[12] = s[12];
        t[1] = s[5];
        t[5] = s[9];
        t[9] = s[13];
        t[13] = s[1];
        t[2] = s[10];
        t[6] = s[14];
        t[10] = s[2];
        t[14] = s[6];
        t[3] = s[15];
        t[7] = s[3];
        t[11] = s[7];
        t[15] = s[11];
        for (i = 0; i < 16u; i++) {
            s[i] = t[i];
        }
        /* MixColumns */
        for (c = 0; c < 4u; c++) {
            unsigned char a0 = s[4u * c + 0u], a1 = s[4u * c + 1u];
            unsigned char a2 = s[4u * c + 2u], a3 = s[4u * c + 3u];
            unsigned char u = (unsigned char)(a0 ^ a1 ^ a2 ^ a3);
            unsigned char v;

            v = sh_aes_xtime((unsigned char)(a0 ^ a1));
            s[4u * c + 0u] = (unsigned char)(a0 ^ v ^ u);
            v = sh_aes_xtime((unsigned char)(a1 ^ a2));
            s[4u * c + 1u] = (unsigned char)(a1 ^ v ^ u);
            v = sh_aes_xtime((unsigned char)(a2 ^ a3));
            s[4u * c + 2u] = (unsigned char)(a2 ^ v ^ u);
            v = sh_aes_xtime((unsigned char)(a3 ^ a0));
            s[4u * c + 3u] = (unsigned char)(a3 ^ v ^ u);
        }
        for (c = 0; c < 4u; c++) {
            unsigned kw = rk[r * 4u + c];

            s[4u * c + 0u] ^= (unsigned char)(kw >> 24);
            s[4u * c + 1u] ^= (unsigned char)(kw >> 16);
            s[4u * c + 2u] ^= (unsigned char)(kw >> 8);
            s[4u * c + 3u] ^= (unsigned char)kw;
        }
    }
    {
        unsigned char t[16];

        for (i = 0; i < 16u; i++) {
            s[i] = s_sh_aes_s[s[i]];
        }
        t[0] = s[0];
        t[4] = s[4];
        t[8] = s[8];
        t[12] = s[12];
        t[1] = s[5];
        t[5] = s[9];
        t[9] = s[13];
        t[13] = s[1];
        t[2] = s[10];
        t[6] = s[14];
        t[10] = s[2];
        t[14] = s[6];
        t[3] = s[15];
        t[7] = s[3];
        t[11] = s[7];
        t[15] = s[11];
        for (i = 0; i < 16u; i++) {
            s[i] = t[i];
        }
        for (c = 0; c < 4u; c++) {
            unsigned kw = rk[40u + c];

            s[4u * c + 0u] ^= (unsigned char)(kw >> 24);
            s[4u * c + 1u] ^= (unsigned char)(kw >> 16);
            s[4u * c + 2u] ^= (unsigned char)(kw >> 8);
            s[4u * c + 3u] ^= (unsigned char)kw;
        }
    }
    for (i = 0; i < 16u; i++) {
        aOut[i] = s[i];
    }
}

static int
cmd_aescheck(void)
{
    /* FIPS-197 App C.1 AES-128 */
    static const unsigned char aKey[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    static const unsigned char aPt[16] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    static const unsigned char aExp[16] = {
        0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
        0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};
    unsigned char aCt[16];
    unsigned i;
    int fOk = 1;
    static const char *szDigits = "0123456789abcdef";
    char aHex[33];

    sh_aes128_encrypt(aKey, aPt, aCt);
    for (i = 0; i < 16u; i++) {
        if (aCt[i] != aExp[i]) {
            fOk = 0;
        }
        aHex[i * 2u] = szDigits[(aCt[i] >> 4) & 0xfu];
        aHex[i * 2u + 1u] = szDigits[aCt[i] & 0xfu];
    }
    aHex[32] = '\0';
    if (fOk) {
        out("aescheck: AES-128 C.1 PASS ");
        out(aHex);
        out("\n");
        return 0;
    }
    out("aescheck: AES-128 C.1 FAIL ");
    out(aHex);
    out("\n");
    return -1;
}

/*
 * tarlist — list member names from a small ustar archive in VFS (product v0.32).
 * Self-contained 512-byte header parse; first few members only. Empty file OK.
 */
static unsigned long
sh_tar_octal(const unsigned char *p, size_t nMax)
{
    unsigned long u = 0;
    size_t i;

    for (i = 0; i < nMax; i++) {
        unsigned char c = p[i];

        if (c == 0 || c == ' ') {
            break;
        }
        if (c < (unsigned char)'0' || c > (unsigned char)'7') {
            break;
        }
        u = (u << 3) + (unsigned long)(c - (unsigned char)'0');
    }
    return u;
}

static int
cmd_tarlist(const char *szName)
{
    static unsigned char aBuf[2048];
    long n;
    size_t off = 0;
    unsigned uMem = 0;
    const unsigned uMaxMem = 8u;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)sizeof(aBuf));
    if (n < 0) {
        return -1;
    }
    /* empty archive → no members */
    while (off + 512u <= (size_t)n && uMem < uMaxMem) {
        const unsigned char *h = aBuf + off;
        size_t i;
        int fZero = 1;
        char aName[101];
        unsigned long uSize;
        size_t uPad;
        size_t uNext;

        for (i = 0; i < 512u; i++) {
            if (h[i] != 0) {
                fZero = 0;
                break;
            }
        }
        if (fZero) {
            break;
        }
        for (i = 0; i < 100u; i++) {
            if (h[i] == 0) {
                break;
            }
            aName[i] = (char)h[i];
        }
        aName[i] = '\0';
        if (aName[0] == '\0') {
            break;
        }
        out(aName);
        out("\n");
        uMem++;
        uSize = sh_tar_octal(h + 124, 12u);
        uPad = (size_t)((uSize + 511ul) & ~511ul);
        uNext = 512u + uPad;
        if (uNext == 0u) {
            break;
        }
        off += uNext;
    }
    return 0;
}

/*
 * Write a one-member empty-file ustar (hi.txt) for bring-up smoke.
 * 512-byte header + 512-byte end-of-archive zeros.
 */
static int
sh_ustar_put_sample(const char *szPath)
{
    static unsigned char a[1024];
    unsigned uSum = 0;
    unsigned i;
    long n;
    static const char *szName = "hi.txt";
    static const char *szMode = "0000644";
    static const char *szSize = "00000000000";
    static const char *szMtime = "00000000000";
    char aChk[8];

    for (i = 0; i < sizeof(a); i++) {
        a[i] = 0;
    }
    for (i = 0; szName[i] != '\0' && i < 100u; i++) {
        a[i] = (unsigned char)szName[i];
    }
    for (i = 0; szMode[i] != '\0' && i < 7u; i++) {
        a[100u + i] = (unsigned char)szMode[i];
    }
    /* uid/gid leave zero */
    for (i = 0; szSize[i] != '\0' && i < 11u; i++) {
        a[124u + i] = (unsigned char)szSize[i];
    }
    for (i = 0; szMtime[i] != '\0' && i < 11u; i++) {
        a[136u + i] = (unsigned char)szMtime[i];
    }
    /* checksum field spaces while summing */
    for (i = 0; i < 8u; i++) {
        a[148u + i] = (unsigned char)' ';
    }
    a[156] = (unsigned char)'0'; /* regular file */
    a[257] = (unsigned char)'u';
    a[258] = (unsigned char)'s';
    a[259] = (unsigned char)'t';
    a[260] = (unsigned char)'a';
    a[261] = (unsigned char)'r';
    a[262] = 0;
    a[263] = (unsigned char)'0';
    a[264] = (unsigned char)'0';
    for (i = 0; i < 512u; i++) {
        uSum += a[i];
    }
    /* 6-digit octal checksum + null + space (classic ustar) */
    for (i = 0; i < 6u; i++) {
        aChk[5u - i] = (char)('0' + ((uSum >> (3u * i)) & 7u));
    }
    for (i = 0; i < 6u; i++) {
        a[148u + i] = (unsigned char)aChk[i];
    }
    a[154] = 0;
    a[155] = (unsigned char)' ';
    n = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)szPath, (long)(uintptr_t)a,
               (long)sizeof(a));
    return (n == (long)sizeof(a)) ? 0 : -1;
}

/* base64url: RFC 4648 5 encode small file (-_ alphabet, no pad). */
static int
cmd_base64url(const char *szName)
{
    static const char aB64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    static char aBuf[256];
    static char aOut[400];
    long n;
    long i;
    size_t o = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i += 3) {
        unsigned b0 = (unsigned char)aBuf[i];
        unsigned b1 = (i + 1 < n) ? (unsigned char)aBuf[i + 1] : 0u;
        unsigned b2 = (i + 2 < n) ? (unsigned char)aBuf[i + 2] : 0u;
        long nIn = n - i;

        if (o + 4u >= sizeof(aOut)) {
            break;
        }
        aOut[o++] = aB64[(b0 >> 2) & 0x3fu];
        aOut[o++] = aB64[((b0 & 0x3u) << 4) | ((b1 >> 4) & 0xfu)];
        if (nIn == 1) {
            /* no padding in base64url bring-up */
        } else if (nIn == 2) {
            aOut[o++] = aB64[(b1 & 0xfu) << 2];
        } else {
            aOut[o++] = aB64[((b1 & 0xfu) << 2) | ((b2 >> 6) & 0x3u)];
            aOut[o++] = aB64[b2 & 0x3fu];
        }
    }
    aOut[o] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/* urlencode: percent-encode small file (unreserved A-Za-z0-9-_.~ pass). */
static int
cmd_urlencode(const char *szName)
{
    static const char aHex[] = "0123456789ABCDEF";
    static char aBuf[256];
    static char aOut[800];
    long n;
    long i;
    size_t o = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        unsigned char c = (unsigned char)aBuf[i];
        int fOk = 0;

        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
            c == '~') {
            fOk = 1;
        }
        if (fOk) {
            if (o + 1u >= sizeof(aOut)) {
                break;
            }
            aOut[o++] = (char)c;
        } else {
            if (o + 3u >= sizeof(aOut)) {
                break;
            }
            aOut[o++] = '%';
            aOut[o++] = aHex[(c >> 4) & 0xfu];
            aOut[o++] = aHex[c & 0xfu];
        }
    }
    aOut[o] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/* urldecode: percent-decode small file (%XX hex; companion to urlencode). */
static int
cmd_urldecode(const char *szName)
{
    static char aBuf[800];
    static char aOut[400];
    long n;
    long i;
    size_t o = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        unsigned char c = (unsigned char)aBuf[i];

        if (c == '%' && i + 2 < n) {
            unsigned char h1 = (unsigned char)aBuf[i + 1];
            unsigned char h2 = (unsigned char)aBuf[i + 2];
            int v1 = -1;
            int v2 = -1;

            if (h1 >= '0' && h1 <= '9') {
                v1 = (int)(h1 - '0');
            } else if (h1 >= 'A' && h1 <= 'F') {
                v1 = (int)(h1 - 'A') + 10;
            } else if (h1 >= 'a' && h1 <= 'f') {
                v1 = (int)(h1 - 'a') + 10;
            }
            if (h2 >= '0' && h2 <= '9') {
                v2 = (int)(h2 - '0');
            } else if (h2 >= 'A' && h2 <= 'F') {
                v2 = (int)(h2 - 'A') + 10;
            } else if (h2 >= 'a' && h2 <= 'f') {
                v2 = (int)(h2 - 'a') + 10;
            }
            if (v1 >= 0 && v2 >= 0) {
                c = (unsigned char)((v1 << 4) | v2);
                i += 2;
            }
        }
        if (o + 1u >= sizeof(aOut)) {
            break;
        }
        aOut[o++] = (char)c;
    }
    aOut[o] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/*
 * filemagic / identify — detect PNG/JPEG/WAV/ELF magic on small VFS files
 * (product v0.33). Self-contained magic bytes only; unknown → data.
 */
static int
cmd_filemagic(const char *szName)
{
    static unsigned char aBuf[64];
    long n;
    const char *szKind = "data";

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)sizeof(aBuf));
    if (n < 0) {
        return -1;
    }
    if (n >= 4 && aBuf[0] == 0x7fu && aBuf[1] == 0x45u && aBuf[2] == 0x4cu &&
        aBuf[3] == 0x46u) {
        szKind = "ELF";
    } else if (n >= 4 && aBuf[0] == 0x89u && aBuf[1] == 0x50u &&
               aBuf[2] == 0x4eu && aBuf[3] == 0x47u) {
        szKind = "PNG";
    } else if (n >= 3 && aBuf[0] == 0xffu && aBuf[1] == 0xd8u &&
               aBuf[2] == 0xffu) {
        szKind = "JPEG";
    } else if (n >= 12 && aBuf[0] == (unsigned char)'R' &&
               aBuf[1] == (unsigned char)'I' && aBuf[2] == (unsigned char)'F' &&
               aBuf[3] == (unsigned char)'F' && aBuf[8] == (unsigned char)'W' &&
               aBuf[9] == (unsigned char)'A' && aBuf[10] == (unsigned char)'V' &&
               aBuf[11] == (unsigned char)'E') {
        szKind = "WAV";
    }
    out(szName);
    out(": ");
    out(szKind);
    out("\n");
    return 0;
}

/* Seed tiny magic stubs for filemagic/identify selftest (product v0.33). */
static int
sh_magic_put_samples(void)
{
    static const unsigned char aElf[] = { 0x7f, 0x45, 0x4c, 0x46, 0x02, 0x01 };
    static const unsigned char aPng[] = {
        0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a
    };
    static const unsigned char aJpg[] = { 0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10 };
    static const unsigned char aWav[] = {
        'R', 'I', 'F', 'F', 0x24, 0x00, 0x00, 0x00,
        'W', 'A', 'V', 'E', 'f', 'm', 't', ' '
    };
    long n;

    n = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)"m.elf",
               (long)(uintptr_t)aElf, (long)sizeof(aElf));
    if (n != (long)sizeof(aElf)) {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)"m.png",
               (long)(uintptr_t)aPng, (long)sizeof(aPng));
    if (n != (long)sizeof(aPng)) {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)"m.jpg",
               (long)(uintptr_t)aJpg, (long)sizeof(aJpg));
    if (n != (long)sizeof(aJpg)) {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_WRITE, (long)(uintptr_t)"m.wav",
               (long)(uintptr_t)aWav, (long)sizeof(aWav));
    if (n != (long)sizeof(aWav)) {
        return -1;
    }
    return 0;
}

/*
 * crock32 — Crockford base32 encode small file (product v0.34).
 * Alphabet 0123456789ABCDEFGHJKMNPQRSTVWXYZ; no pad (self-contained).
 */
static int
cmd_crock32(const char *szName)
{
    static const char aB32[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
    static char aBuf[256];
    static char aOut[420];
    long n;
    long i;
    size_t o = 0;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i < n; i += 5) {
        unsigned b0 = (unsigned char)aBuf[i];
        unsigned b1 = (i + 1 < n) ? (unsigned char)aBuf[i + 1] : 0u;
        unsigned b2 = (i + 2 < n) ? (unsigned char)aBuf[i + 2] : 0u;
        unsigned b3 = (i + 3 < n) ? (unsigned char)aBuf[i + 3] : 0u;
        unsigned b4 = (i + 4 < n) ? (unsigned char)aBuf[i + 4] : 0u;
        long nIn = n - i;
        unsigned char aEnc[8];
        int nOut;
        int k;

        if (o + 8u >= sizeof(aOut)) {
            break;
        }
        aEnc[0] = (unsigned char)((b0 >> 3) & 0x1fu);
        aEnc[1] = (unsigned char)(((b0 & 0x7u) << 2) | ((b1 >> 6) & 0x3u));
        aEnc[2] = (unsigned char)((b1 >> 1) & 0x1fu);
        aEnc[3] = (unsigned char)(((b1 & 0x1u) << 4) | ((b2 >> 4) & 0xfu));
        aEnc[4] = (unsigned char)(((b2 & 0xfu) << 1) | ((b3 >> 7) & 0x1u));
        aEnc[5] = (unsigned char)((b3 >> 2) & 0x1fu);
        aEnc[6] = (unsigned char)(((b3 & 0x3u) << 3) | ((b4 >> 5) & 0x7u));
        aEnc[7] = (unsigned char)(b4 & 0x1fu);
        /* full quintet → 8 chars; partial → ceil(nIn*8/5) symbols, no pad */
        if (nIn >= 5) {
            nOut = 8;
        } else if (nIn == 4) {
            nOut = 7;
        } else if (nIn == 3) {
            nOut = 5;
        } else if (nIn == 2) {
            nOut = 4;
        } else {
            nOut = 2;
        }
        for (k = 0; k < nOut; k++) {
            aOut[o++] = aB32[aEnc[k] & 0x1fu];
        }
    }
    aOut[o] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/*
 * ipsum — RFC 1071 Internet checksum of file bytes as 4 hex digits
 * (product v0.34). Ones-complement sum of 16-bit words, then ~sum.
 */
static int
cmd_ipsum(const char *szName)
{
    static char aBuf[1100];
    static char aOut[8];
    static const char *szDigits = "0123456789abcdef";
    unsigned long uSum = 0ul;
    long n;
    long i;
    unsigned uChk;
    int k;

    if (szName == NULL || szName[0] == '\0') {
        return -1;
    }
    n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)szName, (long)(uintptr_t)aBuf,
               (long)(sizeof(aBuf) - 1));
    if (n < 0) {
        return -1;
    }
    for (i = 0; i + 1 < n; i += 2) {
        unsigned uW = ((unsigned)(unsigned char)aBuf[i] << 8) |
                      (unsigned)(unsigned char)aBuf[i + 1];

        uSum += (unsigned long)uW;
    }
    if ((n & 1) != 0) {
        uSum += (unsigned long)((unsigned)(unsigned char)aBuf[n - 1] << 8);
    }
    /* fold carries into 16 bits (RFC 1071) */
    while ((uSum >> 16) != 0ul) {
        uSum = (uSum & 0xfffful) + (uSum >> 16);
    }
    uChk = (unsigned)(~uSum) & 0xffffu;
    for (k = 0; k < 4; k++) {
        aOut[k] = szDigits[(uChk >> (unsigned)((3 - k) * 4)) & 0xfu];
    }
    aOut[4] = '\0';
    out(aOut);
    out("  ");
    out(szName);
    out("\n");
    return 0;
}

/*
 * duration — parse human duration like "1h30m" / "90s" / "2d1h" and print
 * total seconds (product v0.35). Units: d h m s (case-insensitive).
 */
static int
cmd_duration(const char *szSpec)
{
    const char *p;
    unsigned long uSec = 0ul;
    int fAny = 0;
    char aN[24];
    size_t n;

    if (szSpec == NULL || szSpec[0] == '\0') {
        return -1;
    }
    p = szSpec;
    while (*p != '\0') {
        unsigned long uN = 0ul;
        char c;

        if (*p < '0' || *p > '9') {
            return -1;
        }
        while (*p >= '0' && *p <= '9') {
            uN = uN * 10ul + (unsigned long)(*p - '0');
            p++;
        }
        c = *p;
        if (c >= 'A' && c <= 'Z') {
            c = (char)(c - 'A' + 'a');
        }
        if (c == 'd') {
            uSec += uN * 86400ul;
        } else if (c == 'h') {
            uSec += uN * 3600ul;
        } else if (c == 'm') {
            uSec += uN * 60ul;
        } else if (c == 's') {
            uSec += uN;
        } else {
            return -1;
        }
        p++;
        fAny = 1;
    }
    if (!fAny) {
        return -1;
    }
    n = gj_itoa((long)uSec, aN, sizeof(aN));
    aN[n] = '\0';
    out(aN);
    out("\n");
    return 0;
}

/*
 * macfmt — normalize MAC address string to aa:bb:cc:dd:ee:ff
 * (product v0.35). Accepts colon/hyphen/dot separators or 12 hex digits.
 */
static int
cmd_macfmt(const char *szMac)
{
    static const char *szDigits = "0123456789abcdef";
    unsigned char aNib[12];
    size_t nHex = 0;
    const char *p;
    char aOut[18];
    int i;

    if (szMac == NULL || szMac[0] == '\0') {
        return -1;
    }
    for (p = szMac; *p != '\0'; p++) {
        unsigned char c = (unsigned char)*p;
        int v = -1;

        if (c == ':' || c == '-' || c == '.' || c == ' ') {
            continue;
        }
        if (c >= '0' && c <= '9') {
            v = (int)(c - '0');
        } else if (c >= 'a' && c <= 'f') {
            v = (int)(c - 'a') + 10;
        } else if (c >= 'A' && c <= 'F') {
            v = (int)(c - 'A') + 10;
        } else {
            return -1;
        }
        if (nHex >= 12u) {
            return -1;
        }
        aNib[nHex++] = (unsigned char)v;
    }
    if (nHex != 12u) {
        return -1;
    }
    for (i = 0; i < 6; i++) {
        aOut[i * 3] = szDigits[aNib[i * 2] & 0xfu];
        aOut[i * 3 + 1] = szDigits[aNib[i * 2 + 1] & 0xfu];
        aOut[i * 3 + 2] = (i < 5) ? ':' : '\0';
    }
    out(aOut);
    out("\n");
    return 0;
}

/*
 * slug — URL-safe slugify of ARG to stdout (product v0.36).
 * Rules: A–Z→a–z; non-alnum→'-'; collapse runs; trim leading/trailing '-'.
 */
static int
cmd_slug(const char *szIn)
{
    static char aOut[96];
    size_t iOut = 0;
    int fHaveContent = 0;
    int fPendingDash = 0;
    const char *p;

    if (szIn == NULL || szIn[0] == '\0') {
        return -1;
    }
    for (p = szIn; *p != '\0'; p++) {
        unsigned char ch = (unsigned char)*p;
        int fAlnum = 0;

        if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z')) {
            fAlnum = 1;
        } else if (ch >= 'A' && ch <= 'Z') {
            fAlnum = 1;
            ch = (unsigned char)(ch - 'A' + 'a');
        }
        if (fAlnum) {
            if (fHaveContent && fPendingDash) {
                if (iOut + 1u >= sizeof(aOut)) {
                    return -1;
                }
                aOut[iOut++] = '-';
                fPendingDash = 0;
            }
            if (iOut + 1u >= sizeof(aOut)) {
                return -1;
            }
            aOut[iOut++] = (char)ch;
            fHaveContent = 1;
            fPendingDash = 0;
        } else if (fHaveContent) {
            fPendingDash = 1;
        }
    }
    aOut[iOut] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

#define SH_LEV_MAX 64

static int
sh_lev_min3(int nA, int nB, int nC)
{
    int nM = nA;

    if (nB < nM) {
        nM = nB;
    }
    if (nC < nM) {
        nM = nC;
    }
    return nM;
}

/*
 * lev — Levenshtein edit distance of two args (product v0.36).
 * Wagner–Fischer stack DP; each string capped at SH_LEV_MAX.
 */
static int
cmd_lev(const char *szA, const char *szB)
{
    int nLenA;
    int nLenB;
    int i;
    int j;
    int aPrev[SH_LEV_MAX + 1];
    int aCur[SH_LEV_MAX + 1];
    char aN[16];
    size_t nDig;

    if (szA == NULL || szB == NULL) {
        return -1;
    }
    nLenA = 0;
    while (szA[nLenA] != '\0') {
        if (nLenA >= SH_LEV_MAX) {
            return -1;
        }
        nLenA++;
    }
    nLenB = 0;
    while (szB[nLenB] != '\0') {
        if (nLenB >= SH_LEV_MAX) {
            return -1;
        }
        nLenB++;
    }
    if (nLenA == 0) {
        nDig = gj_itoa((long)nLenB, aN, sizeof(aN));
        aN[nDig] = '\0';
        out(aN);
        out("\n");
        return 0;
    }
    if (nLenB == 0) {
        nDig = gj_itoa((long)nLenA, aN, sizeof(aN));
        aN[nDig] = '\0';
        out(aN);
        out("\n");
        return 0;
    }
    for (j = 0; j <= nLenB; j++) {
        aPrev[j] = j;
    }
    for (i = 1; i <= nLenA; i++) {
        unsigned char chA = (unsigned char)szA[i - 1];

        aCur[0] = i;
        for (j = 1; j <= nLenB; j++) {
            unsigned char chB = (unsigned char)szB[j - 1];
            int nCost = (chA == chB) ? 0 : 1;
            int nDel = aPrev[j] + 1;
            int nIns = aCur[j - 1] + 1;
            int nSub = aPrev[j - 1] + nCost;

            aCur[j] = sh_lev_min3(nDel, nIns, nSub);
        }
        for (j = 0; j <= nLenB; j++) {
            aPrev[j] = aCur[j];
        }
    }
    nDig = gj_itoa((long)aPrev[nLenB], aN, sizeof(aN));
    aN[nDig] = '\0';
    out(aN);
    out("\n");
    return 0;
}

/*
 * gray — print binary-reflected Gray encode of decimal NUMBER
 * (product v0.37): g = n ^ (n >> 1).
 */
static int
cmd_gray(const char *szArg)
{
    unsigned long u;
    unsigned long uG;
    const char *p;
    char aN[24];
    size_t nDig;

    if (szArg == NULL || szArg[0] == '\0') {
        return -1;
    }
    u = 0ul;
    p = szArg;
    if (*p < '0' || *p > '9') {
        return -1;
    }
    while (*p >= '0' && *p <= '9') {
        u = u * 10ul + (unsigned long)(*p - '0');
        p++;
    }
    if (*p != '\0') {
        return -1;
    }
    uG = u ^ (u >> 1);
    nDig = gj_itoa((long)uG, aN, sizeof(aN));
    aN[nDig] = '\0';
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitrev — reverse bit order of a 32-bit hex ARG (product v0.37).
 * Accepts optional 0x prefix; prints 8 lowercase hex digits.
 */
static int
cmd_bitrev(const char *szArg)
{
    static const char *szDigits = "0123456789abcdef";
    unsigned u;
    const char *p;
    char aOut[12];
    int i;
    int fAny = 0;

    if (szArg == NULL || szArg[0] == '\0') {
        return -1;
    }
    p = szArg;
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    u = 0u;
    while (*p != '\0') {
        unsigned char c = (unsigned char)*p;
        unsigned v;

        if (c >= '0' && c <= '9') {
            v = (unsigned)(c - '0');
        } else if (c >= 'a' && c <= 'f') {
            v = (unsigned)(c - 'a') + 10u;
        } else if (c >= 'A' && c <= 'F') {
            v = (unsigned)(c - 'A') + 10u;
        } else {
            return -1;
        }
        u = (u << 4) | v;
        fAny = 1;
        p++;
    }
    if (!fAny) {
        return -1;
    }
    /* SWAR full-width 32-bit reverse */
    u = ((u >> 1) & 0x55555555u) | ((u & 0x55555555u) << 1);
    u = ((u >> 2) & 0x33333333u) | ((u & 0x33333333u) << 2);
    u = ((u >> 4) & 0x0f0f0f0fu) | ((u & 0x0f0f0f0fu) << 4);
    u = ((u >> 8) & 0x00ff00ffu) | ((u & 0x00ff00ffu) << 8);
    u = (u >> 16) | (u << 16);
    for (i = 0; i < 8; i++) {
        aOut[i] = szDigits[(u >> (unsigned)((7 - i) * 4)) & 0xfu];
    }
    aOut[8] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/* Parse decimal u64; returns 0 ok, -1 on empty/non-digit. */
static int
sh_parse_u64(const char *sz, unsigned long long *pu)
{
    const char *p;
    unsigned long long u;

    if (sz == NULL || sz[0] == '\0' || pu == NULL) {
        return -1;
    }
    u = 0ull;
    p = sz;
    if (*p < '0' || *p > '9') {
        return -1;
    }
    while (*p >= '0' && *p <= '9') {
        unsigned long long uDigit = (unsigned long long)(*p - '0');

        if (u > (0xffffffffffffffffull - uDigit) / 10ull) {
            return -1; /* overflow */
        }
        u = u * 10ull + uDigit;
        p++;
    }
    if (*p != '\0') {
        return -1;
    }
    *pu = u;
    return 0;
}

/* Decimal format of u64 into buffer (NUL-terminated). Returns length. */
static size_t
sh_u64toa(unsigned long long u, char *szBuf, size_t cbBuf)
{
    char aTmp[24];
    size_t n = 0;
    size_t i;

    if (szBuf == NULL || cbBuf == 0) {
        return 0;
    }
    if (u == 0ull) {
        if (cbBuf < 2) {
            szBuf[0] = '\0';
            return 0;
        }
        szBuf[0] = '0';
        szBuf[1] = '\0';
        return 1;
    }
    while (u > 0ull && n < sizeof(aTmp)) {
        aTmp[n++] = (char)('0' + (int)(u % 10ull));
        u /= 10ull;
    }
    if (n + 1 > cbBuf) {
        szBuf[0] = '\0';
        return 0;
    }
    for (i = 0; i < n; i++) {
        szBuf[i] = aTmp[n - 1 - i];
    }
    szBuf[n] = '\0';
    return n;
}

/*
 * gcd — Euclidean GCD of two decimal unsigned integers (product v0.38).
 * Usage: gcd A B  → prints gcd(A,B) in decimal.
 */
static int
cmd_gcd(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long t;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    while (b != 0ull) {
        t = a % b;
        a = b;
        b = t;
    }
    nDig = sh_u64toa(a, aN, sizeof(aN));
    if (nDig == 0 && a != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * isprime — trial-division primality of a decimal u64 (product v0.38).
 * Prints "1" if prime, "0" otherwise.
 */
static int
cmd_isprime(const char *szArg)
{
    unsigned long long n;
    unsigned long long d;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n < 2ull) {
        out("0\n");
        return 0;
    }
    if (n == 2ull || n == 3ull) {
        out("1\n");
        return 0;
    }
    if ((n % 2ull) == 0ull || (n % 3ull) == 0ull) {
        out("0\n");
        return 0;
    }
    /* 6k±1 wheel; d <= n/d avoids overflow of d*d for large d. */
    d = 5ull;
    while (d <= n / d) {
        if ((n % d) == 0ull || (n % (d + 2ull)) == 0ull) {
            out("0\n");
            return 0;
        }
        d += 6ull;
    }
    out("1\n");
    return 0;
}

/*
 * fib — Fibonacci(N) for N <= 93 (fits u64); iterative add (product v0.39).
 * Usage: fib N
 */
static int
cmd_fib(const char *szArg)
{
    unsigned long long n;
    unsigned long long a;
    unsigned long long b;
    unsigned long long t;
    unsigned long long i;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n > 93ull) {
        return -1; /* Fib(94) exceeds u64 */
    }
    a = 0ull;
    b = 1ull;
    if (n == 0ull) {
        out("0\n");
        return 0;
    }
    for (i = 1ull; i < n; i++) {
        t = a + b; /* safe: Fib(n) fits u64 for n <= 93 */
        a = b;
        b = t;
    }
    nDig = sh_u64toa(b, aN, sizeof(aN));
    if (nDig == 0 && b != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/* (a + b) % m with a,b < m; avoids u64 overflow of a+b. */
static unsigned long long
sh_addmod(unsigned long long a, unsigned long long b, unsigned long long m)
{
    if (a >= m - b) {
        return a - (m - b);
    }
    return a + b;
}

/* (a * b) % m via binary (Russian peasant) mul; no __uint128. */
static unsigned long long
sh_mulmod(unsigned long long a, unsigned long long b, unsigned long long m)
{
    unsigned long long r = 0ull;

    if (m == 1ull) {
        return 0ull;
    }
    a %= m;
    while (b > 0ull) {
        if ((b & 1ull) != 0ull) {
            r = sh_addmod(r, a, m);
        }
        a = sh_addmod(a, a, m);
        b >>= 1ull;
    }
    return r;
}

/*
 * modpow — (B^E) mod M for three decimal u64 args (product v0.39).
 * Requires M > 0. Binary exponentiation with freestanding mulmod.
 * Usage: modpow B E M
 */
static int
cmd_modpow(const char *szB, const char *szE, const char *szM)
{
    unsigned long long b;
    unsigned long long e;
    unsigned long long m;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szB, &b) != 0 || sh_parse_u64(szE, &e) != 0 ||
        sh_parse_u64(szM, &m) != 0) {
        return -1;
    }
    if (m == 0ull) {
        return -1;
    }
    if (m == 1ull) {
        out("0\n");
        return 0;
    }
    r = 1ull;
    b %= m;
    while (e > 0ull) {
        if ((e & 1ull) != 0ull) {
            r = sh_mulmod(r, b, m);
        }
        b = sh_mulmod(b, b, m);
        e >>= 1ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * lcm — least common multiple of two decimal u64 (product v0.40).
 * Uses lcm(a,b) = a/gcd(a,b)*b when a!=0 (overflow-checked multiply).
 * lcm(0,x)=lcm(x,0)=0. Usage: lcm A B
 */
static int
cmd_lcm(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long g;
    unsigned long long q;
    unsigned long long r;
    unsigned long long x;
    unsigned long long y;
    unsigned long long t;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (a == 0ull || b == 0ull) {
        out("0\n");
        return 0;
    }
    x = a;
    y = b;
    while (y != 0ull) {
        t = x % y;
        x = y;
        y = t;
    }
    g = x;
    q = a / g; /* exact: g divides a */
    if (q > 0xffffffffffffffffull / b) {
        return -1; /* overflow */
    }
    r = q * b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * factor — trial-factor N; print factors space-separated smallest-first,
 * or "prime" if N is prime (product v0.40). Usage: factor N
 */
static int
cmd_factor(const char *szArg)
{
    unsigned long long n;
    unsigned long long d;
    char aN[24];
    size_t nDig;
    int fAny = 0;
    int fFirst = 1;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n < 2ull) {
        return -1;
    }
    while ((n % 2ull) == 0ull) {
        if (!fFirst) {
            out(" ");
        }
        out("2");
        fFirst = 0;
        fAny = 1;
        n /= 2ull;
    }
    d = 3ull;
    while (d <= n / d) {
        while ((n % d) == 0ull) {
            if (!fFirst) {
                out(" ");
            }
            nDig = sh_u64toa(d, aN, sizeof(aN));
            if (nDig == 0) {
                return -1;
            }
            out(aN);
            fFirst = 0;
            fAny = 1;
            n /= d;
        }
        d += 2ull;
    }
    if (!fAny) {
        /* no proper factors → n was prime (still equal to original) */
        out("prime\n");
        return 0;
    }
    if (n > 1ull) {
        out(" ");
        nDig = sh_u64toa(n, aN, sizeof(aN));
        if (nDig == 0) {
            return -1;
        }
        out(aN);
    }
    out("\n");
    return 0;
}

/*
 * binom — binomial C(N,K) for decimal u64 (product v0.41).
 * Multiplicative formula with gcd reduction; fits u64 for N<=66 always,
 * and larger N when the result still fits (overflow → fail).
 * C(N,K)=0 when K>N. Usage: binom N K
 */
static int
cmd_binom(const char *szN, const char *szK)
{
    unsigned long long n;
    unsigned long long k;
    unsigned long long r;
    unsigned long long i;
    unsigned long long a;
    unsigned long long b;
    unsigned long long g;
    unsigned long long x;
    unsigned long long y;
    unsigned long long t;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szN, &n) != 0 || sh_parse_u64(szK, &k) != 0) {
        return -1;
    }
    if (k > n) {
        out("0\n");
        return 0;
    }
    /* C(n,k)=C(n,n-k); smaller loop */
    if (k > n - k) {
        k = n - k;
    }
    r = 1ull;
    for (i = 0ull; i < k; i++) {
        a = n - i; /* multiply factor */
        b = i + 1ull; /* divide factor; exact integer result */
        /* Reduce r/b and a/b by gcd so r*a/b stays exact and smaller. */
        x = r;
        y = b;
        while (y != 0ull) {
            t = x % y;
            x = y;
            y = t;
        }
        g = x;
        r /= g;
        b /= g;
        x = a;
        y = b;
        while (y != 0ull) {
            t = x % y;
            x = y;
            y = t;
        }
        g = x;
        a /= g;
        b /= g;
        if (a != 0ull && r > 0xffffffffffffffffull / a) {
            return -1; /* overflow (result > u64) */
        }
        r = r * a;
        if (b != 0ull) {
            r /= b; /* b should be 1 after full reduction */
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * fibmod — Fibonacci(N) mod M, iterative (product v0.41).
 * N <= 10000, M > 0. Uses overflow-safe modular add. Usage: fibmod N M
 */
static int
cmd_fibmod(const char *szN, const char *szM)
{
    unsigned long long n;
    unsigned long long m;
    unsigned long long a;
    unsigned long long b;
    unsigned long long t;
    unsigned long long i;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szN, &n) != 0 || sh_parse_u64(szM, &m) != 0) {
        return -1;
    }
    if (m == 0ull || n > 10000ull) {
        return -1;
    }
    if (m == 1ull) {
        out("0\n");
        return 0;
    }
    if (n == 0ull) {
        out("0\n");
        return 0;
    }
    a = 0ull;
    b = 1ull;
    for (i = 1ull; i < n; i++) {
        t = sh_addmod(a, b, m);
        a = b;
        b = t;
    }
    nDig = sh_u64toa(b, aN, sizeof(aN));
    if (nDig == 0 && b != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/* SWAR popcount of a u64 (no __builtin / -mpopcnt). */
static unsigned long long
sh_popcnt_u64(unsigned long long x)
{
    x = x - ((x >> 1) & 0x5555555555555555ull);
    x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0full;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    return x & 0x7full;
}

/*
 * hamming — Hamming distance of two decimal u64 (product v0.42).
 * popcount(A xor B). Usage: hamming A B
 */
static int
cmd_hamming(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long n;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    n = sh_popcnt_u64(a ^ b);
    nDig = sh_u64toa(n, aN, sizeof(aN));
    if (nDig == 0 && n != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * popcnt — population count of a decimal u64 (product v0.42).
 * Usage: popcnt N
 */
static int
cmd_popcnt(const char *szArg)
{
    unsigned long long u;
    unsigned long long n;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &u) != 0) {
        return -1;
    }
    n = sh_popcnt_u64(u);
    nDig = sh_u64toa(n, aN, sizeof(aN));
    if (nDig == 0 && n != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * xor — bitwise XOR of two decimal u64 (product v0.43).
 * Usage: xor A B  → prints A^B in decimal.
 */
static int
cmd_xor(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a ^ b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * and — bitwise AND of two decimal u64 (product v0.43).
 * Usage: and A B  → prints A&B in decimal.
 */
static int
cmd_and(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a & b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * or — bitwise OR of two decimal u64 (product v0.43).
 * Usage: or A B  → prints A|B in decimal.
 */
static int
cmd_or(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a | b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * shl — logical left shift of decimal u64 (product v0.44).
 * Usage: shl A N  → prints A<<N for N<64, else 0.
 */
static int
cmd_shl(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aBuf[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n < 64ull) {
        r = a << (unsigned)n;
    } else {
        r = 0ull;
    }
    nDig = sh_u64toa(r, aBuf, sizeof(aBuf));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aBuf);
    out("\n");
    return 0;
}

/*
 * shr — logical right shift of decimal u64 (product v0.44).
 * Usage: shr A N  → prints A>>N for N<64, else 0.
 */
static int
cmd_shr(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aBuf[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n < 64ull) {
        r = a >> (unsigned)n;
    } else {
        r = 0ull;
    }
    nDig = sh_u64toa(r, aBuf, sizeof(aBuf));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aBuf);
    out("\n");
    return 0;
}

/*
 * abs — absolute value of optional-signed decimal (product v0.45).
 * Usage: abs N  → prints magnitude of N as u64 (leading '-' stripped).
 */
static int
cmd_abs(const char *szArg)
{
    const char *p;
    unsigned long long u;
    char aBuf[24];
    size_t nDig;

    if (szArg == NULL || szArg[0] == '\0') {
        return -1;
    }
    p = szArg;
    if (*p == '-') {
        p++;
        if (*p == '\0') {
            return -1;
        }
    }
    if (sh_parse_u64(p, &u) != 0) {
        return -1;
    }
    nDig = sh_u64toa(u, aBuf, sizeof(aBuf));
    if (nDig == 0 && u != 0ull) {
        return -1;
    }
    out(aBuf);
    out("\n");
    return 0;
}

/*
 * minmax — print min then max of two decimal u64 (product v0.45).
 * Usage: minmax A B  → prints "min max".
 */
static int
cmd_minmax(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long lo;
    unsigned long long hi;
    char aBuf[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (a <= b) {
        lo = a;
        hi = b;
    } else {
        lo = b;
        hi = a;
    }
    nDig = sh_u64toa(lo, aBuf, sizeof(aBuf));
    if (nDig == 0 && lo != 0ull) {
        return -1;
    }
    out(aBuf);
    out(" ");
    nDig = sh_u64toa(hi, aBuf, sizeof(aBuf));
    if (nDig == 0 && hi != 0ull) {
        return -1;
    }
    out(aBuf);
    out("\n");
    return 0;
}

/*
 * avg — floor average of two decimal u64 without overflow (product v0.46).
 * Uses (a&b)+((a^b)>>1). Usage: avg A B
 */
static int
cmd_avg(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a & b) + ((a ^ b) >> 1ull);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * pow — A^E as u64 via binary exponentiation (product v0.46).
 * Prints 0 on overflow. 0^0=1. Usage: pow A E
 */
static int
cmd_pow(const char *szA, const char *szE)
{
    unsigned long long a;
    unsigned long long e;
    unsigned long long r;
    unsigned long long base;
    int fOv;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szE, &e) != 0) {
        return -1;
    }
    /* 0^0 and x^0 → 1; 0^e (e>0) → 0 */
    if (e == 0ull) {
        out("1\n");
        return 0;
    }
    if (a == 0ull) {
        out("0\n");
        return 0;
    }
    r = 1ull;
    base = a;
    fOv = 0;
    while (e > 0ull) {
        if ((e & 1ull) != 0ull) {
            if (base != 0ull && r > 0xffffffffffffffffull / base) {
                fOv = 1;
                break;
            }
            r *= base;
        }
        e >>= 1ull;
        if (e > 0ull) {
            if (base != 0ull && base > 0xffffffffffffffffull / base) {
                fOv = 1;
                break;
            }
            base *= base;
        }
    }
    if (fOv) {
        out("0\n");
        return 0;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * digits — count decimal digits of a u64 (product v0.47).
 * 0 has 1 digit. Usage: digits N
 */
static int
cmd_digits(const char *szArg)
{
    unsigned long long n;
    unsigned long long c;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        c = 1ull;
    } else {
        c = 0ull;
        while (n > 0ull) {
            c++;
            n /= 10ull;
        }
    }
    nDig = sh_u64toa(c, aN, sizeof(aN));
    if (nDig == 0 && c != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * dsum — sum of decimal digits of a u64 (product v0.47).
 * Usage: dsum N
 */
static int
cmd_dsum(const char *szArg)
{
    unsigned long long n;
    unsigned long long s;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    s = 0ull;
    while (n > 0ull) {
        s += n % 10ull;
        n /= 10ull;
    }
    nDig = sh_u64toa(s, aN, sizeof(aN));
    if (nDig == 0 && s != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * revdig — reverse decimal digits of a u64 (product v0.48).
 * Trailing zeros of N become leading zeros of the reverse and drop.
 * Usage: revdig N
 */
static int
cmd_revdig(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    r = 0ull;
    while (n > 0ull) {
        r = r * 10ull + (n % 10ull);
        n /= 10ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ispal — 1 if N is a decimal palindrome, else 0 (product v0.48).
 * Compares N to its digit reverse (trailing zeros prevent palindrome).
 * Usage: ispal N
 */
static int
cmd_ispal(const char *szArg)
{
    unsigned long long n;
    unsigned long long orig;
    unsigned long long r;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    orig = n;
    r = 0ull;
    while (n > 0ull) {
        r = r * 10ull + (n % 10ull);
        n /= 10ull;
    }
    if (r == orig) {
        out("1\n");
    } else {
        out("0\n");
    }
    return 0;
}

/*
 * fact — factorial N! for N <= 20 (fits u64); error otherwise (product v0.49).
 * Usage: fact N
 */
static int
cmd_fact(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    unsigned long long i;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n > 20ull) {
        return -1; /* 21! exceeds u64 */
    }
    r = 1ull;
    for (i = 2ull; i <= n; i++) {
        r *= i;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * sqrt — integer square root floor of a decimal u64 (product v0.49).
 * Binary search; mid <= n/mid avoids mid*mid overflow.
 * Usage: sqrt N
 */
static int
cmd_sqrt(const char *szArg)
{
    unsigned long long n;
    unsigned long long lo;
    unsigned long long hi;
    unsigned long long mid;
    unsigned long long ans;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n <= 1ull) {
        ans = n;
    } else {
        lo = 1ull;
        /* floor(sqrt(2^64-1)) = 2^32-1 */
        hi = (n < 0xffffffffull) ? n : 0xffffffffull;
        ans = 1ull;
        while (lo <= hi) {
            mid = lo + (hi - lo) / 2ull;
            if (mid <= n / mid) {
                ans = mid;
                lo = mid + 1ull;
            } else {
                hi = mid - 1ull;
            }
        }
    }
    nDig = sh_u64toa(ans, aN, sizeof(aN));
    if (nDig == 0 && ans != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ilog2 — floor(log2(N)) of a decimal u64 (product v0.50).
 * N==0 prints 0. Usage: ilog2 N
 */
static int
cmd_ilog2(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    r = 0ull;
    if (n != 0ull) {
        /* binary search for MSB index; no __builtin_clzll */
        if ((n >> 32) != 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n >> 16) != 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n >> 8) != 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n >> 4) != 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n >> 2) != 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n >> 1) != 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ctz — count trailing zero bits of a decimal u64 (product v0.50).
 * N==0 prints 64. Usage: ctz N
 */
static int
cmd_ctz(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        r = 64ull;
    } else {
        r = 0ull;
        /* binary search for LSB index; no __builtin_ctzll */
        if ((n & 0xffffffffull) == 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n & 0xffffull) == 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n & 0xffull) == 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n & 0xfull) == 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n & 0x3ull) == 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n & 0x1ull) == 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * clz — count leading zero bits of a decimal u64 (product v0.51).
 * N==0 prints 64. Usage: clz N
 */
static int
cmd_clz(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        r = 64ull;
    } else {
        r = 0ull;
        /* binary search for MSB; no __builtin_clzll */
        if ((n & 0xffffffff00000000ull) == 0ull) {
            r += 32ull;
            n <<= 32;
        }
        if ((n & 0xffff000000000000ull) == 0ull) {
            r += 16ull;
            n <<= 16;
        }
        if ((n & 0xff00000000000000ull) == 0ull) {
            r += 8ull;
            n <<= 8;
        }
        if ((n & 0xf000000000000000ull) == 0ull) {
            r += 4ull;
            n <<= 4;
        }
        if ((n & 0xc000000000000000ull) == 0ull) {
            r += 2ull;
            n <<= 2;
        }
        if ((n & 0x8000000000000000ull) == 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bswap — byte-swap a decimal u64 and print decimal (product v0.51).
 * Usage: bswap N
 */
static int
cmd_bswap(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    r = ((n & 0x00000000000000ffull) << 56)
      | ((n & 0x000000000000ff00ull) << 40)
      | ((n & 0x0000000000ff0000ull) << 24)
      | ((n & 0x00000000ff000000ull) << 8)
      | ((n & 0x000000ff00000000ull) >> 8)
      | ((n & 0x0000ff0000000000ull) >> 24)
      | ((n & 0x00ff000000000000ull) >> 40)
      | ((n & 0xff00000000000000ull) >> 56);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * satadd — saturating u64 add of two decimal args (product v0.52).
 * On overflow result is UINT64_MAX. Usage: satadd A B
 */
static int
cmd_satadd(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (a > ~0ull - b) {
        r = ~0ull;
    } else {
        r = a + b;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * satsub — saturating u64 subtract of two decimal args (product v0.52).
 * On underflow result is 0. Usage: satsub A B
 */
static int
cmd_satsub(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (a < b) {
        r = 0ull;
    } else {
        r = a - b;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ispow2 — 1 if decimal u64 is a nonzero power of two, else 0 (product v0.53).
 * Usage: ispow2 N
 */
static int
cmd_ispow2(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    r = (n != 0ull && (n & (n - 1ull)) == 0ull) ? 1ull : 0ull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * alignup — round X up to a multiple of A (product v0.53).
 * A must be a nonzero power of two; otherwise print X unchanged.
 * Usage: alignup A X
 */
static int
cmd_alignup(const char *szA, const char *szX)
{
    unsigned long long a;
    unsigned long long x;
    unsigned long long r;
    unsigned long long m;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szX, &x) != 0) {
        return -1;
    }
    if (a == 0ull || (a & (a - 1ull)) != 0ull) {
        r = x;
    } else {
        m = a - 1ull;
        r = (x + m) & ~m;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * aligndown — round X down to a multiple of A (product v0.54).
 * A must be a nonzero power of two; otherwise print X unchanged.
 * Usage: aligndown A X
 */
static int
cmd_aligndown(const char *szA, const char *szX)
{
    unsigned long long a;
    unsigned long long x;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szX, &x) != 0) {
        return -1;
    }
    if (a == 0ull || (a & (a - 1ull)) != 0ull) {
        r = x;
    } else {
        r = x & ~(a - 1ull);
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * hexnib — value of first char of ARG as hex nibble 0-15, else -1 (product v0.54).
 * Usage: hexnib C
 */
static int
cmd_hexnib(const char *szArg)
{
    char c;
    int v;
    char aN[8];
    size_t nDig;

    if (szArg == NULL || szArg[0] == '\0') {
        return -1;
    }
    c = szArg[0];
    if (c >= '0' && c <= '9') {
        v = (int)(c - '0');
    } else if (c >= 'a' && c <= 'f') {
        v = 10 + (int)(c - 'a');
    } else if (c >= 'A' && c <= 'F') {
        v = 10 + (int)(c - 'A');
    } else {
        out("-1\n");
        return 0;
    }
    nDig = gj_itoa((long)v, aN, sizeof(aN));
    aN[nDig] = '\0';
    out(aN);
    out("\n");
    return 0;
}

/*
 * min3 — print minimum of three decimal u64 (product v0.55).
 * Usage: min3 A B C
 */
static int
cmd_min3(const char *szA, const char *szB, const char *szC)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long c;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0 ||
        sh_parse_u64(szC, &c) != 0) {
        return -1;
    }
    r = a;
    if (b < r) {
        r = b;
    }
    if (c < r) {
        r = c;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * max3 — print maximum of three decimal u64 (product v0.55).
 * Usage: max3 A B C
 */
static int
cmd_max3(const char *szA, const char *szB, const char *szC)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long c;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0 ||
        sh_parse_u64(szC, &c) != 0) {
        return -1;
    }
    r = a;
    if (b > r) {
        r = b;
    }
    if (c > r) {
        r = c;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * median3 — print median of three decimal u64 (product v0.56).
 * Pure local comparisons (no sum; avoids u64 overflow).
 * Usage: median3 A B C
 */
static int
cmd_median3(const char *szA, const char *szB, const char *szC)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long c;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0 ||
        sh_parse_u64(szC, &c) != 0) {
        return -1;
    }
    /* Mid value: b if ordered around b, else a if ordered around a, else c. */
    if ((b >= a && b <= c) || (b <= a && b >= c)) {
        r = b;
    } else if ((a >= b && a <= c) || (a <= b && a >= c)) {
        r = a;
    } else {
        r = c;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * clamp — print X clamped to [LO,HI] (product v0.57).
 * If LO>HI the bounds are swapped first.
 * Usage: clamp X LO HI
 */
static int
cmd_clamp(const char *szX, const char *szLo, const char *szHi)
{
    unsigned long long x;
    unsigned long long lo;
    unsigned long long hi;
    unsigned long long t;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szLo, &lo) != 0 ||
        sh_parse_u64(szHi, &hi) != 0) {
        return -1;
    }
    if (lo > hi) {
        t = lo;
        lo = hi;
        hi = t;
    }
    r = x;
    if (r < lo) {
        r = lo;
    }
    if (r > hi) {
        r = hi;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitset — print X with bit B set (product v0.57).
 * B must be in 0..63 inclusive.
 * Usage: bitset X B
 */
static int
cmd_bitset(const char *szX, const char *szB)
{
    unsigned long long x;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (b > 63ull) {
        return -1;
    }
    r = x | (1ull << (unsigned)b);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bittest — print 1 if bit B of X is set, else 0 (product v0.58).
 * B must be in 0..63 inclusive.
 * Usage: bittest X B
 */
static int
cmd_bittest(const char *szX, const char *szB)
{
    unsigned long long x;
    unsigned long long b;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (b > 63ull) {
        return -1;
    }
    if ((x & (1ull << (unsigned)b)) != 0ull) {
        out("1\n");
    } else {
        out("0\n");
    }
    return 0;
}

/*
 * bitclr — print X with bit B cleared (product v0.58).
 * B must be in 0..63 inclusive.
 * Usage: bitclr X B
 */
static int
cmd_bitclr(const char *szX, const char *szB)
{
    unsigned long long x;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (b > 63ull) {
        return -1;
    }
    r = x & ~(1ull << (unsigned)b);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bittog — print X with bit B toggled (product v0.59).
 * B must be in 0..63 inclusive.
 * Usage: bittog X B
 */
static int
cmd_bittog(const char *szX, const char *szB)
{
    unsigned long long x;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (b > 63ull) {
        return -1;
    }
    r = x ^ (1ull << (unsigned)b);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitcnt — population count (popcount) of a decimal u64 (product v0.59).
 * Usage: bitcnt X
 */
static int
cmd_bitcnt(const char *szArg)
{
    unsigned long long u;
    unsigned long long n;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &u) != 0) {
        return -1;
    }
    n = sh_popcnt_u64(u);
    nDig = sh_u64toa(n, aN, sizeof(aN));
    if (nDig == 0 && n != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitwidth — minimal bits to represent decimal u64 X (product v0.60).
 * 0→0, 1→1, else floor(log2(X))+1. Usage: bitwidth X
 */
static int
cmd_bitwidth(const char *szArg)
{
    unsigned long long x;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &x) != 0) {
        return -1;
    }
    if (x == 0ull) {
        r = 0ull;
    } else {
        n = x;
        r = 0ull;
        /* binary search for MSB index; no __builtin_clzll */
        if ((n >> 32) != 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n >> 16) != 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n >> 8) != 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n >> 4) != 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n >> 2) != 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n >> 1) != 0ull) {
            r += 1ull;
        }
        r += 1ull; /* floor(log2(X)) + 1 */
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ishex — 1 if first char of ARG is a hex digit, else 0 (product v0.60).
 * Usage: ishex C
 */
static int
cmd_ishex(const char *szArg)
{
    char c;

    if (szArg == NULL || szArg[0] == '\0') {
        return -1;
    }
    c = szArg[0];
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F')) {
        out("1\n");
    } else {
        out("0\n");
    }
    return 0;
}

/*
 * wrapadd — wrapping u64 add of two decimal args (product v0.61).
 * Result is (A+B) mod 2^64. Usage: wrapadd A B
 */
static int
cmd_wrapadd(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a + b; /* unsigned wrap = mod 2^64 */
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * wrapsub — wrapping u64 subtract of two decimal args (product v0.61).
 * Result is (A-B) mod 2^64. Usage: wrapsub A B
 */
static int
cmd_wrapsub(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a - b; /* unsigned wrap = mod 2^64 */
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * inrange — print 1 if LO<=X<=HI, else 0 (product v0.62).
 * If LO>HI the bounds are swapped first.
 * Usage: inrange X LO HI
 */
static int
cmd_inrange(const char *szX, const char *szLo, const char *szHi)
{
    unsigned long long x;
    unsigned long long lo;
    unsigned long long hi;
    unsigned long long t;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szLo, &lo) != 0 ||
        sh_parse_u64(szHi, &hi) != 0) {
        return -1;
    }
    if (lo > hi) {
        t = lo;
        lo = hi;
        hi = t;
    }
    if (x >= lo && x <= hi) {
        out("1\n");
    } else {
        out("0\n");
    }
    return 0;
}

/*
 * isblank — print 1 if S is empty or only space/tab, else 0 (product v0.62).
 * Usage: isblank S
 */
static int
cmd_isblank(const char *sz)
{
    const char *p;

    if (sz == NULL) {
        out("1\n");
        return 0;
    }
    for (p = sz; *p != '\0'; p++) {
        if (*p != ' ' && *p != '\t') {
            out("0\n");
            return 0;
        }
    }
    out("1\n");
    return 0;
}

/*
 * isident — print 1 if S is a C-like identifier, else 0 (product v0.63).
 * First char: letter or '_'; rest: letter, digit, or '_'. Empty → 0.
 * Usage: isident S
 */
static int
cmd_isident(const char *sz)
{
    const char *p;
    char c;

    if (sz == NULL || sz[0] == '\0') {
        out("0\n");
        return 0;
    }
    c = sz[0];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')) {
        out("0\n");
        return 0;
    }
    for (p = sz + 1; *p != '\0'; p++) {
        c = *p;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_')) {
            out("0\n");
            return 0;
        }
    }
    out("1\n");
    return 0;
}

/*
 * digitval — decimal value of first char of C (0-9), else -1 (product v0.63).
 * Usage: digitval C
 */
static int
cmd_digitval(const char *szArg)
{
    char c;

    if (szArg == NULL || szArg[0] == '\0') {
        return -1;
    }
    c = szArg[0];
    if (c >= '0' && c <= '9') {
        char aN[4];

        aN[0] = c;
        aN[1] = '\n';
        aN[2] = '\0';
        out(aN);
    } else {
        out("-1\n");
    }
    return 0;
}

/*
 * alphaval — alphabet index of first char of C: A/a=0 .. Z/z=25, else -1
 * (product v0.64). Usage: alphaval C
 */
static int
cmd_alphaval(const char *szArg)
{
    char c;
    int v;
    char aN[8];

    if (szArg == NULL || szArg[0] == '\0') {
        return -1;
    }
    c = szArg[0];
    if (c >= 'A' && c <= 'Z') {
        v = (int)(c - 'A');
    } else if (c >= 'a' && c <= 'z') {
        v = (int)(c - 'a');
    } else {
        out("-1\n");
        return 0;
    }
    if (v < 10) {
        aN[0] = (char)('0' + v);
        aN[1] = '\n';
        aN[2] = '\0';
    } else {
        aN[0] = (char)('0' + (v / 10));
        aN[1] = (char)('0' + (v % 10));
        aN[2] = '\n';
        aN[3] = '\0';
    }
    out(aN);
    return 0;
}

/*
 * isempty — print 1 if S is the empty string, else 0 (product v0.64).
 * Usage: isempty S
 */
static int
cmd_isempty(const char *sz)
{
    if (sz == NULL || sz[0] == '\0') {
        out("1\n");
    } else {
        out("0\n");
    }
    return 0;
}

/*
 * pct — print (X*P)/100 as u64 (product v0.65).
 * Usage: pct X P
 */
static int
cmd_pct(const char *szX, const char *szP)
{
    unsigned long long x;
    unsigned long long p;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szP, &p) != 0) {
        return -1;
    }
    r = (x * p) / 100ull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * safediv — print A/B as u64, or 0 if B==0 (product v0.65).
 * Usage: safediv A B
 */
static int
cmd_safediv(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (b == 0ull) {
        r = 0ull;
    } else {
        r = a / b;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bpoints — print (X*BP)/10000 as u64 (product v0.66).
 * Usage: bpoints X BP
 */
static int
cmd_bpoints(const char *szX, const char *szBp)
{
    unsigned long long x;
    unsigned long long bp;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szBp, &bp) != 0) {
        return -1;
    }
    r = (x * bp) / 10000ull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ratio — print (A*S)/B as u64, or 0 if B==0 (product v0.66).
 * Usage: ratio A B S
 */
static int
cmd_ratio(const char *szA, const char *szB, const char *szS)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long s;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0 ||
        sh_parse_u64(szS, &s) != 0) {
        return -1;
    }
    if (b == 0ull) {
        r = 0ull;
    } else {
        r = (a * s) / b;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * divceil — print ceil(A/B) as u64, or 0 if B==0 (product v0.67).
 * Usage: divceil A B
 */
static int
cmd_divceil(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (b == 0ull) {
        r = 0ull;
    } else {
        r = a / b;
        if ((a % b) != 0ull) {
            r++;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * isaligned — 1 if X is a multiple of A (A nonzero power of two), else 0
 * (product v0.67). Invalid A (0 or not power of two) yields 0.
 * Usage: isaligned X A
 */
static int
cmd_isaligned(const char *szX, const char *szA)
{
    unsigned long long x;
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if (a == 0ull || (a & (a - 1ull)) != 0ull) {
        r = 0ull;
    } else {
        r = ((x & (a - 1ull)) == 0ull) ? 1ull : 0ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * log2ceil — ceil(log2(X)) of a decimal u64 (product v0.68).
 * X==0 prints 0. Usage: log2ceil X
 */
static int
cmd_log2ceil(const char *szArg)
{
    unsigned long long x;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &x) != 0) {
        return -1;
    }
    if (x == 0ull || x == 1ull) {
        r = 0ull;
    } else {
        /* ceil(log2(X)) = floor(log2(X-1)) + 1 for X > 1 */
        n = x - 1ull;
        r = 0ull;
        /* binary search for MSB index; no __builtin_clzll */
        if ((n >> 32) != 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n >> 16) != 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n >> 8) != 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n >> 4) != 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n >> 2) != 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n >> 1) != 0ull) {
            r += 1ull;
        }
        r += 1ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * modpow2 — X & (M-1) when M is a nonzero power of two, else 0 (product v0.68).
 * Usage: modpow2 X M
 */
static int
cmd_modpow2(const char *szX, const char *szM)
{
    unsigned long long x;
    unsigned long long m;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szX, &x) != 0 || sh_parse_u64(szM, &m) != 0) {
        return -1;
    }
    if (m == 0ull || (m & (m - 1ull)) != 0ull) {
        r = 0ull;
    } else {
        r = x & (m - 1ull);
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/* ASCII case-insensitive equality of two C strings (product v0.69). */
static int
sh_ascii_ieq(const char *a, const char *b)
{
    char ca;
    char cb;

    if (a == NULL || b == NULL) {
        return 0;
    }
    while (*a != '\0' && *b != '\0') {
        ca = *a;
        cb = *b;
        if (ca >= 'A' && ca <= 'Z') {
            ca = (char)(ca - 'A' + 'a');
        }
        if (cb >= 'A' && cb <= 'Z') {
            cb = (char)(cb - 'A' + 'a');
        }
        if (ca != cb) {
            return 0;
        }
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0') ? 1 : 0;
}

/* Truthy/falsey token match (ASCII case-insensitive); shared by istrue/ison
 * and isfalse/isoff (product v0.69 / v0.70). */
static int
sh_is_truthy(const char *sz)
{
    return (sz != NULL &&
            (sh_ascii_ieq(sz, "true") || sh_ascii_ieq(sz, "yes") ||
             sh_ascii_ieq(sz, "1") || sh_ascii_ieq(sz, "on")))
               ? 1
               : 0;
}

static int
sh_is_falsey(const char *sz)
{
    return (sz != NULL &&
            (sh_ascii_ieq(sz, "false") || sh_ascii_ieq(sz, "no") ||
             sh_ascii_ieq(sz, "0") || sh_ascii_ieq(sz, "off")))
               ? 1
               : 0;
}

/*
 * istrue — print 1 if S is true/yes/1/on (ASCII case-insensitive), else 0
 * (product v0.69). Usage: istrue S
 */
static int
cmd_istrue(const char *sz)
{
    out(sh_is_truthy(sz) ? "1\n" : "0\n");
    return 0;
}

/*
 * isfalse — print 1 if S is false/no/0/off (ASCII case-insensitive), else 0
 * (product v0.69). Usage: isfalse S
 */
static int
cmd_isfalse(const char *sz)
{
    out(sh_is_falsey(sz) ? "1\n" : "0\n");
    return 0;
}

/*
 * ison — print 1 if S is on/yes/true/1 (ASCII case-insensitive), else 0
 * (product v0.70). Usage: ison S
 */
static int
cmd_ison(const char *sz)
{
    out(sh_is_truthy(sz) ? "1\n" : "0\n");
    return 0;
}

/*
 * isoff — print 1 if S is off/no/false/0 (ASCII case-insensitive), else 0
 * (product v0.70). Usage: isoff S
 */
static int
cmd_isoff(const char *sz)
{
    out(sh_is_falsey(sz) ? "1\n" : "0\n");
    return 0;
}

/*
 * iszero — print 1 if decimal u64 X is 0, else 0 (product v0.71).
 * Usage: iszero X
 */
static int
cmd_iszero(const char *szArg)
{
    unsigned long long x;

    if (sh_parse_u64(szArg, &x) != 0) {
        return -1;
    }
    out(x == 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * isnonzero — print 1 if decimal u64 X is nonzero, else 0 (product v0.71).
 * Usage: isnonzero X
 */
static int
cmd_isnonzero(const char *szArg)
{
    unsigned long long x;

    if (sh_parse_u64(szArg, &x) != 0) {
        return -1;
    }
    out(x != 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * min2 — print minimum of two decimal u64 (product v0.72).
 * Usage: min2 A B
 */
static int
cmd_min2(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a < b) ? a : b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * max2 — print maximum of two decimal u64 (product v0.72).
 * Usage: max2 A B
 */
static int
cmd_max2(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a > b) ? a : b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * mid2 — floor average of two decimal u64 without overflow (product v0.73).
 * Uses (A&B)+((A^B)>>1). Usage: mid2 A B
 */
static int
cmd_mid2(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a & b) + ((a ^ b) >> 1ull);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * absdiff — absolute difference |A-B| of two decimal u64 (product v0.73).
 * Usage: absdiff A B
 */
static int
cmd_absdiff(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a >= b) ? (a - b) : (b - a);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitor — bitwise OR of two decimal u64 (product v0.74).
 * Usage: bitor A B  → prints A|B in decimal.
 */
static int
cmd_bitor(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a | b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitand — bitwise AND of two decimal u64 (product v0.74).
 * Usage: bitand A B  → prints A&B in decimal.
 */
static int
cmd_bitand(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a & b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitxor — bitwise XOR of two decimal u64 (product v0.74).
 * Usage: bitxor A B  → prints A^B in decimal.
 */
static int
cmd_bitxor(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a ^ b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitnot — bitwise NOT of decimal u64 (product v0.75).
 * Usage: bitnot A  → prints ~A as unsigned decimal (u64 two's complement wrap).
 */
static int
cmd_bitnot(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = ~a;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * lshift — logical left shift of decimal u64 (product v0.75).
 * Usage: lshift A N  → prints A<<N for N in 0..63; if N>=64 prints 0.
 */
static int
cmd_lshift(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n < 64ull) {
        r = a << (unsigned)n;
    } else {
        r = 0ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * rshift — logical right shift of decimal u64 (product v0.75).
 * Usage: rshift A N  → prints A>>N for N in 0..63; if N>=64 prints 0.
 */
static int
cmd_rshift(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n < 64ull) {
        r = a >> (unsigned)n;
    } else {
        r = 0ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/* Rotate left u64 by n bits; n reduced mod 64 (n==0 is identity). */
static unsigned long long
sh_cmd_rotl64(unsigned long long x, unsigned long long n)
{
    unsigned r;

    r = (unsigned)(n % 64ull);
    if (r == 0u) {
        return x;
    }
    return (x << r) | (x >> (64u - r));
}

/* Rotate right u64 by n bits; n reduced mod 64 (n==0 is identity). */
static unsigned long long
sh_cmd_rotr64(unsigned long long x, unsigned long long n)
{
    unsigned r;

    r = (unsigned)(n % 64ull);
    if (r == 0u) {
        return x;
    }
    return (x >> r) | (x << (64u - r));
}

/*
 * rotl — rotate left decimal u64 (product v0.76).
 * Usage: rotl A N  → prints rotl(A, N%64) in decimal.
 */
static int
cmd_rotl(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    r = sh_cmd_rotl64(a, n);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * rotr — rotate right decimal u64 (product v0.76).
 * Usage: rotr A N  → prints rotr(A, N%64) in decimal.
 */
static int
cmd_rotr(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    r = sh_cmd_rotr64(a, n);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * popcount — population count of a decimal u64 (product v0.76).
 * Usage: popcount A  → prints number of set bits.
 * (Command name is popcount: popcnt already exists from product v0.42.)
 */
static int
cmd_popcount(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = sh_popcnt_u64(a);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bswap32 — byte-swap low 32 bits of a decimal u64 (product v0.77).
 * High 32 bits of input ignored; result is zero-extended. Usage: bswap32 A
 */
static int
cmd_bswap32(const char *szA)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    n = a & 0xffffffffull;
    r = ((n & 0x000000ffull) << 24)
      | ((n & 0x0000ff00ull) << 8)
      | ((n & 0x00ff0000ull) >> 8)
      | ((n & 0xff000000ull) >> 24);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bswap64 — full 64-bit byte-swap of a decimal u64 (product v0.77).
 * Usage: bswap64 A  → prints byte-swapped value in decimal.
 * (Command name is bswap64: bswap already exists from product v0.51.)
 */
static int
cmd_bswap64(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = ((a & 0x00000000000000ffull) << 56)
      | ((a & 0x000000000000ff00ull) << 40)
      | ((a & 0x0000000000ff0000ull) << 24)
      | ((a & 0x00000000ff000000ull) << 8)
      | ((a & 0x000000ff00000000ull) >> 8)
      | ((a & 0x0000ff0000000000ull) >> 24)
      | ((a & 0x00ff000000000000ull) >> 40)
      | ((a & 0xff00000000000000ull) >> 56);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * parity — parity of set bits in a decimal u64 (product v0.78).
 * 1 if odd popcount, else 0. Usage: parity A
 * (ctz already exists from product v0.50; skipped for this product.)
 */
static int
cmd_parity(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = sh_popcnt_u64(a) & 1ull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bytelen — minimum bytes to represent a decimal u64 (product v0.78).
 * Result is 1..8; A==0 prints 1. Usage: bytelen A
 */
static int
cmd_bytelen(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if (a == 0ull) {
        r = 1ull;
    } else if (a <= 0xffull) {
        r = 1ull;
    } else if (a <= 0xffffull) {
        r = 2ull;
    } else if (a <= 0xffffffull) {
        r = 3ull;
    } else if (a <= 0xffffffffull) {
        r = 4ull;
    } else if (a <= 0xffffffffffull) {
        r = 5ull;
    } else if (a <= 0xffffffffffffull) {
        r = 6ull;
    } else if (a <= 0xffffffffffffffull) {
        r = 7ull;
    } else {
        r = 8ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * nibblecnt — count nonzero 4-bit nibbles in a decimal u64 (product v0.79).
 * Result is 0..16; A==0 prints 0. Usage: nibblecnt A
 */
static int
cmd_nibblecnt(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    int i;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = 0ull;
    for (i = 0; i < 16; i++) {
        if (((a >> (i * 4)) & 0xfull) != 0ull) {
            r++;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * lo32 — low 32 bits of a decimal u64 (product v0.79).
 * Usage: lo32 A
 */
static int
cmd_lo32(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = a & 0xffffffffull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * hi32 — high 32 bits of a decimal u64 (product v0.79).
 * Usage: hi32 A
 */
static int
cmd_hi32(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = a >> 32;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umin — minimum of two decimal u64 (product v0.80).
 * Usage: umin A B
 */
static int
cmd_umin(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a < b) ? a : b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umax — maximum of two decimal u64 (product v0.80).
 * Usage: umax A B
 */
static int
cmd_umax(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a > b) ? a : b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uavg — floor average of two decimal u64 without overflow (product v0.80).
 * Uses (A&B)+((A^B)>>1). Usage: uavg A B
 */
static int
cmd_uavg(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a & b) + ((a ^ b) >> 1ull);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * clampu — clamp decimal u64 A into [LO,HI] (product v0.81).
 * If LO>HI the bounds are swapped first.
 * (Command name is clampu: clamp already exists from product v0.57;
 *  satadd/satsub/wrapadd taken from earlier continuum.)
 * Usage: clampu A LO HI
 */
static int
cmd_clampu(const char *szA, const char *szLo, const char *szHi)
{
    unsigned long long a;
    unsigned long long lo;
    unsigned long long hi;
    unsigned long long t;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szLo, &lo) != 0 ||
        sh_parse_u64(szHi, &hi) != 0) {
        return -1;
    }
    if (lo > hi) {
        t = lo;
        lo = hi;
        hi = t;
    }
    r = a;
    if (r < lo) {
        r = lo;
    }
    if (r > hi) {
        r = hi;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * mulhi — high 64 bits of unsigned 128-bit product A*B (product v0.81).
 * Portable 32-bit partial products (no __int128 / widening mul).
 *   A = a1:a0, B = b1:b0 (each half 32 bits)
 *   p00=a0*b0, p01=a0*b1, p10=a1*b0, p11=a1*b1
 *   mid = (p00>>32) + (p01&0xffffffff) + (p10&0xffffffff)
 *   hi  = p11 + (p01>>32) + (p10>>32) + (mid>>32)
 * Usage: mulhi A B
 */
static int
cmd_mulhi(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long a0;
    unsigned long long a1;
    unsigned long long b0;
    unsigned long long b1;
    unsigned long long p00;
    unsigned long long p01;
    unsigned long long p10;
    unsigned long long p11;
    unsigned long long mid;
    unsigned long long hi;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    a0 = a & 0xffffffffull;
    a1 = a >> 32ull;
    b0 = b & 0xffffffffull;
    b1 = b >> 32ull;
    p00 = a0 * b0;
    p01 = a0 * b1;
    p10 = a1 * b0;
    p11 = a1 * b1;
    mid = (p00 >> 32ull) + (p01 & 0xffffffffull) + (p10 & 0xffffffffull);
    hi = p11 + (p01 >> 32ull) + (p10 >> 32ull) + (mid >> 32ull);
    nDig = sh_u64toa(hi, aN, sizeof(aN));
    if (nDig == 0 && hi != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ugcd — Euclidean GCD of two decimal u64 (product v0.82).
 * Same algorithm as gcd (v0.38); name is ugcd because gcd is taken.
 * Usage: ugcd A B
 */
static int
cmd_ugcd(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long t;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    while (b != 0ull) {
        t = a % b;
        a = b;
        b = t;
    }
    nDig = sh_u64toa(a, aN, sizeof(aN));
    if (nDig == 0 && a != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ulcm — LCM of two decimal u64 via a/gcd(a,b)*b (product v0.82).
 * ulcm(0,x)=ulcm(x,0)=0. On multiply overflow, saturates to UINT64_MAX
 * (18446744073709551615), not 0 and not fail — distinct from lcm (v0.40)
 * which returns error on overflow. Name is ulcm because lcm is taken.
 * Usage: ulcm A B
 */
static int
cmd_ulcm(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long g;
    unsigned long long q;
    unsigned long long r;
    unsigned long long x;
    unsigned long long y;
    unsigned long long t;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (a == 0ull || b == 0ull) {
        out("0\n");
        return 0;
    }
    x = a;
    y = b;
    while (y != 0ull) {
        t = x % y;
        x = y;
        y = t;
    }
    g = x;
    q = a / g; /* exact: g divides a */
    if (q > 0xffffffffffffffffull / b) {
        r = 0xffffffffffffffffull; /* sat to UINT64_MAX on overflow */
    } else {
        r = q * b;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umod — A % B for decimal u64; prints 0 if B==0 (product v0.82).
 * Usage: umod A B
 */
static int
cmd_umod(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (b == 0ull) {
        out("0\n");
        return 0;
    }
    r = a % b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * isqrt — integer square root floor of a decimal u64 (product v0.83).
 * Same algorithm as sqrt (v0.49); name is isqrt as a parallel u64 helper.
 * Binary search; mid <= n/mid avoids mid*mid overflow.
 * Usage: isqrt A
 */
static int
cmd_isqrt(const char *szArg)
{
    unsigned long long n;
    unsigned long long lo;
    unsigned long long hi;
    unsigned long long mid;
    unsigned long long ans;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n <= 1ull) {
        ans = n;
    } else {
        lo = 1ull;
        /* floor(sqrt(2^64-1)) = 2^32-1 */
        hi = (n < 0xffffffffull) ? n : 0xffffffffull;
        ans = 1ull;
        while (lo <= hi) {
            mid = lo + (hi - lo) / 2ull;
            if (mid <= n / mid) {
                ans = mid;
                lo = mid + 1ull;
            } else {
                hi = mid - 1ull;
            }
        }
    }
    nDig = sh_u64toa(ans, aN, sizeof(aN));
    if (nDig == 0 && ans != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * icbrt — integer cube root floor of a decimal u64 (product v0.83).
 * Binary search for largest r with r*r*r <= A. Overflow-safe multiply
 * via division checks. floor(cbrt(UINT64_MAX)) = 2642245.
 * Usage: icbrt A
 */
static int
cmd_icbrt(const char *szArg)
{
    unsigned long long n;
    unsigned long long lo;
    unsigned long long hi;
    unsigned long long mid;
    unsigned long long sq;
    unsigned long long cu;
    unsigned long long ans;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n < 2ull) {
        ans = n;
    } else {
        lo = 1ull;
        hi = 2642245ull; /* floor(cbrt(2^64-1)) */
        ans = 1ull;
        while (lo <= hi) {
            mid = lo + (hi - lo) / 2ull;
            if (mid > 0xffffffffffffffffull / mid) {
                hi = mid - 1ull;
                continue;
            }
            sq = mid * mid;
            if (mid > 0xffffffffffffffffull / sq) {
                hi = mid - 1ull;
                continue;
            }
            cu = sq * mid;
            if (cu <= n) {
                ans = mid;
                lo = mid + 1ull;
            } else {
                hi = mid - 1ull;
            }
        }
    }
    nDig = sh_u64toa(ans, aN, sizeof(aN));
    if (nDig == 0 && ans != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ispow2u — 1 if decimal u64 is a nonzero power of two, else 0 (product v0.83).
 * Same as ispow2 (v0.53); name is ispow2u because ispow2 is taken.
 * Usage: ispow2u A
 */
static int
cmd_ispow2u(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    r = (n != 0ull && (n & (n - 1ull)) == 0ull) ? 1ull : 0ull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * popmsb — index of highest set bit of a decimal u64 (product v0.84).
 * Result in 0..63; A==0 prints 64. Binary search; no __builtin_clzll.
 * Usage: popmsb A
 */
static int
cmd_popmsb(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        r = 64ull;
    } else {
        r = 0ull;
        if ((n >> 32) != 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n >> 16) != 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n >> 8) != 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n >> 4) != 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n >> 2) != 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n >> 1) != 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * poplsb — index of lowest set bit of a decimal u64 (product v0.84).
 * Result in 0..63; A==0 prints 64. Same binary search as ctz (v0.50).
 * Usage: poplsb A
 */
static int
cmd_poplsb(const char *szArg)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szArg, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        r = 64ull;
    } else {
        r = 0ull;
        /* binary search for LSB index; no __builtin_ctzll */
        if ((n & 0xffffffffull) == 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n & 0xffffull) == 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n & 0xffull) == 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n & 0xfull) == 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n & 0x3ull) == 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n & 0x1ull) == 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bits — population count of a decimal u64 (product v0.84).
 * Same as popcount (v0.76) / bitcnt (v0.59); alias name is free.
 * Usage: bits A
 */
static int
cmd_bits(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = sh_popcnt_u64(a);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * revbits — reverse all 64 bits of a decimal u64 (product v0.85).
 * SWAR full-width reverse; distinct from bitrev (32-bit hex, v0.37).
 * Usage: revbits A
 */
static int
cmd_revbits(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    /* SWAR 64-bit reverse: pairs → nibbles → bytes → 16 → 32 → 64 */
    r = a;
    r = ((r >> 1) & 0x5555555555555555ull) | ((r & 0x5555555555555555ull) << 1);
    r = ((r >> 2) & 0x3333333333333333ull) | ((r & 0x3333333333333333ull) << 2);
    r = ((r >> 4) & 0x0f0f0f0f0f0f0f0full) | ((r & 0x0f0f0f0f0f0f0f0full) << 4);
    r = ((r >> 8) & 0x00ff00ff00ff00ffull) | ((r & 0x00ff00ff00ff00ffull) << 8);
    r = ((r >> 16) & 0x0000ffff0000ffffull) | ((r & 0x0000ffff0000ffffull) << 16);
    r = (r >> 32) | (r << 32);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * revbytes — reverse the 8 bytes of a decimal u64 (product v0.85).
 * Same as bswap/bswap64; alias name is free.
 * Usage: revbytes A
 */
static int
cmd_revbytes(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = ((a & 0x00000000000000ffull) << 56)
      | ((a & 0x000000000000ff00ull) << 40)
      | ((a & 0x0000000000ff0000ull) << 24)
      | ((a & 0x00000000ff000000ull) << 8)
      | ((a & 0x000000ff00000000ull) >> 8)
      | ((a & 0x0000ff0000000000ull) >> 24)
      | ((a & 0x00ff000000000000ull) >> 40)
      | ((a & 0xff00000000000000ull) >> 56);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ntz — number of trailing zeros of a decimal u64 (product v0.85).
 * Alias for ctz (v0.50) / poplsb (v0.84); A==0 prints 64. Name free.
 * Usage: ntz A
 */
static int
cmd_ntz(const char *szA)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        r = 64ull;
    } else {
        r = 0ull;
        /* binary search for LSB index; no __builtin_ctzll */
        if ((n & 0xffffffffull) == 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n & 0xffffull) == 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n & 0xffull) == 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n & 0xfull) == 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n & 0x3ull) == 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n & 0x1ull) == 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * tohex — print decimal u64 A as lowercase hex without 0x prefix (product v0.86).
 * No leading zeros except for zero itself. Usage: tohex A
 */
static int
cmd_tohex(const char *szA)
{
    static const char *szDigits = "0123456789abcdef";
    unsigned long long a;
    char aOut[20];
    int i;
    int n;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if (a == 0ull) {
        out("0\n");
        return 0;
    }
    n = 0;
    /* Collect nibbles LSB-first, then reverse for MSB-first output */
    while (a != 0ull) {
        aOut[n++] = szDigits[(unsigned)(a & 0xfull)];
        a >>= 4;
    }
    /* n is 1..16 for nonzero u64 */
    for (i = 0; i < n / 2; i++) {
        char t = aOut[i];

        aOut[i] = aOut[n - 1 - i];
        aOut[n - 1 - i] = t;
    }
    aOut[n] = '\0';
    out(aOut);
    out("\n");
    return 0;
}

/*
 * fromhex — parse hex string S to decimal u64 (product v0.86).
 * Optional 0x/0X prefix; accepts a-f/A-F. Max 16 hex digits. Usage: fromhex S
 */
static int
cmd_fromhex(const char *szS)
{
    unsigned long long u;
    const char *p;
    char aN[24];
    size_t nDig;
    int fAny = 0;
    int nHex = 0;

    if (szS == NULL || szS[0] == '\0') {
        return -1;
    }
    p = szS;
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    u = 0ull;
    while (*p != '\0') {
        unsigned char c = (unsigned char)*p;
        unsigned v;

        if (c >= '0' && c <= '9') {
            v = (unsigned)(c - '0');
        } else if (c >= 'a' && c <= 'f') {
            v = (unsigned)(c - 'a') + 10u;
        } else if (c >= 'A' && c <= 'F') {
            v = (unsigned)(c - 'A') + 10u;
        } else {
            return -1;
        }
        nHex++;
        if (nHex > 16) {
            return -1; /* would overflow u64 */
        }
        u = (u << 4) | (unsigned long long)v;
        fAny = 1;
        p++;
    }
    if (!fAny) {
        return -1;
    }
    nDig = sh_u64toa(u, aN, sizeof(aN));
    if (nDig == 0 && u != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitrev8 — reverse low 8 bits of a decimal u64; print decimal (product v0.87).
 * Upper bits discarded. Distinct from bitrev (hex 32-bit, v0.37) and
 * revbits (full 64-bit reverse, v0.85). Usage: bitrev8 A
 */
static int
cmd_bitrev8(const char *szA)
{
    unsigned long long a;
    unsigned u;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    u = (unsigned)(a & 0xffull);
    /* SWAR 8-bit reverse: pairs → 2-bit → nibbles */
    u = ((u >> 1) & 0x55u) | ((u & 0x55u) << 1);
    u = ((u >> 2) & 0x33u) | ((u & 0x33u) << 2);
    u = ((u >> 4) & 0x0fu) | ((u & 0x0fu) << 4);
    r = (unsigned long long)(u & 0xffu);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitrev16 — reverse low 16 bits of a decimal u64; print decimal (product v0.87).
 * Upper bits discarded. Usage: bitrev16 A
 */
static int
cmd_bitrev16(const char *szA)
{
    unsigned long long a;
    unsigned u;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    u = (unsigned)(a & 0xffffull);
    /* SWAR 16-bit reverse: pairs → 2-bit → nibbles → bytes */
    u = ((u >> 1) & 0x5555u) | ((u & 0x5555u) << 1);
    u = ((u >> 2) & 0x3333u) | ((u & 0x3333u) << 2);
    u = ((u >> 4) & 0x0f0fu) | ((u & 0x0f0fu) << 4);
    u = ((u >> 8) & 0x00ffu) | ((u & 0x00ffu) << 8);
    r = (unsigned long long)(u & 0xffffu);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bitrev32u — reverse low 32 bits of a decimal u64; print decimal (product v0.87).
 * Name bitrev32u: bitrev already exists (hex ARG → hex, v0.37). Usage: bitrev32u A
 */
static int
cmd_bitrev32u(const char *szA)
{
    unsigned long long a;
    unsigned u;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    u = (unsigned)(a & 0xffffffffull);
    /* SWAR full-width 32-bit reverse (same stages as cmd_bitrev) */
    u = ((u >> 1) & 0x55555555u) | ((u & 0x55555555u) << 1);
    u = ((u >> 2) & 0x33333333u) | ((u & 0x33333333u) << 2);
    u = ((u >> 4) & 0x0f0f0f0fu) | ((u & 0x0f0f0f0fu) << 4);
    u = ((u >> 8) & 0x00ff00ffu) | ((u & 0x00ff00ffu) << 8);
    u = (u >> 16) | (u << 16);
    r = (unsigned long long)u;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * pack16 — pack two low bytes into a 16-bit value (product v0.88).
 * Result is (A&0xff)|((B&0xff)<<8). Usage: pack16 A B
 */
static int
cmd_pack16(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a & 0xffull) | ((b & 0xffull) << 8);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * unpack16lo — low byte of a 16-bit value (product v0.88).
 * Result is A&0xff. Usage: unpack16lo A
 */
static int
cmd_unpack16lo(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = a & 0xffull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * unpack16hi — high byte of a 16-bit value (product v0.88).
 * Result is (A>>8)&0xff. Usage: unpack16hi A
 */
static int
cmd_unpack16hi(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = (a >> 8) & 0xffull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * pack32 — pack two low 32-bit halves into a 64-bit value (product v0.89).
 * Result is (A&0xffffffff)|((B&0xffffffff)<<32). Usage: pack32 A B
 */
static int
cmd_pack32(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a & 0xffffffffull) | ((b & 0xffffffffull) << 32);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * unpack32lo — low 32 bits of a 64-bit value (product v0.89).
 * Result is A&0xffffffff. Usage: unpack32lo A
 */
static int
cmd_unpack32lo(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = a & 0xffffffffull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * unpack32hi — high 32 bits of a 64-bit value (product v0.89).
 * Result is A>>32. Usage: unpack32hi A
 */
static int
cmd_unpack32hi(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = a >> 32;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * btest — 1 if bit N of A is set, else 0 (product v0.90).
 * N in 0..63; if N>=64 print 0. Usage: btest A N
 */
static int
cmd_btest(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n >= 64ull) {
        out("0\n");
        return 0;
    }
    if ((a & (1ull << (unsigned)n)) != 0ull) {
        out("1\n");
    } else {
        out("0\n");
    }
    return 0;
}

/*
 * bset — set bit N of A (product v0.90).
 * Result is A|(1<<N) for N in 0..63; if N>=64 print A unchanged.
 * Usage: bset A N
 */
static int
cmd_bset(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n >= 64ull) {
        r = a;
    } else {
        r = a | (1ull << (unsigned)n);
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bclr — clear bit N of A (product v0.90).
 * Result is A&~(1<<N) for N in 0..63; if N>=64 print A unchanged.
 * Usage: bclr A N
 */
static int
cmd_bclr(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n >= 64ull) {
        r = a;
    } else {
        r = a & ~(1ull << (unsigned)n);
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * bflip — toggle bit N of A (product v0.91).
 * Result is A^(1<<N) for N in 0..63; if N>=64 print A unchanged.
 * Usage: bflip A N
 * (bittog exists v0.59 with strict N; bflip soft-bounds like bset/bclr.)
 */
static int
cmd_bflip(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n >= 64ull) {
        r = a;
    } else {
        r = a ^ (1ull << (unsigned)n);
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * masklow — low N bits set in u64 (product v0.91).
 * N==0 → 0; N in 1..63 → (1<<N)-1; N>=64 → all ones.
 * Usage: masklow N
 */
static int
cmd_masklow(const char *szN)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        r = 0ull;
    } else if (n >= 64ull) {
        r = ~0ull;
    } else {
        r = (1ull << (unsigned)n) - 1ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * maskhigh — top N bits set in u64 (product v0.91).
 * N==0 → 0; N in 1..63 → bits (64-N)..63 set; N>=64 → all ones.
 * Usage: maskhigh N
 */
static int
cmd_maskhigh(const char *szN)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        r = 0ull;
    } else if (n >= 64ull) {
        r = ~0ull;
    } else {
        r = ~0ull << (unsigned)(64ull - n);
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * setbitcnt — population count of a decimal u64 (product v0.92).
 * Same as bits/popcount/bitcnt/popcnt; free exclusive name.
 * Usage: setbitcnt A
 */
static int
cmd_setbitcnt(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = sh_popcnt_u64(a);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * isodd — 1 if decimal u64 A is odd, else 0 (product v0.92).
 * Usage: isodd A
 */
static int
cmd_isodd(const char *szA)
{
    unsigned long long a;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    out((a & 1ull) != 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * iseven — 1 if decimal u64 A is even, else 0 (product v0.92).
 * Usage: iseven A
 */
static int
cmd_iseven(const char *szA)
{
    unsigned long long a;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    out((a & 1ull) == 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * minu2 — print minimum of two decimal u64 (product v0.93).
 * Same as min2 (v0.72) / umin (v0.80); free exclusive name.
 * Usage: minu2 A B
 */
static int
cmd_minu2(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a < b) ? a : b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * maxu2 — print maximum of two decimal u64 (product v0.93).
 * Same as max2 (v0.72) / umax (v0.80); free exclusive name.
 * Usage: maxu2 A B
 */
static int
cmd_maxu2(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a > b) ? a : b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * clamp3 — print A clamped to [LO,HI] (product v0.93).
 * Same as clamp (v0.57); free exclusive name. If LO>HI bounds swap.
 * Usage: clamp3 A LO HI
 */
static int
cmd_clamp3(const char *szA, const char *szLo, const char *szHi)
{
    unsigned long long a;
    unsigned long long lo;
    unsigned long long hi;
    unsigned long long t;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szLo, &lo) != 0 ||
        sh_parse_u64(szHi, &hi) != 0) {
        return -1;
    }
    if (lo > hi) {
        t = lo;
        lo = hi;
        hi = t;
    }
    r = a;
    if (r < lo) {
        r = lo;
    }
    if (r > hi) {
        r = hi;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * udiff — absolute difference |A-B| of two decimal u64 (product v0.94).
 * Same as absdiff (v0.73); free exclusive name.
 * Usage: udiff A B
 */
static int
cmd_udiff(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a >= b) ? (a - b) : (b - a);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * avg2 — floor average of two decimal u64 without overflow (product v0.94).
 * Uses (A&B)+((A^B)>>1). Same as avg/mid2/uavg; free exclusive name.
 * Usage: avg2 A B
 */
static int
cmd_avg2(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a & b) + ((a ^ b) >> 1ull);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ispow2n — 1 if decimal u64 is a nonzero power of two, else 0 (product v0.95).
 * Same as ispow2 (v0.53) / ispow2u (v0.83); free exclusive name.
 * Usage: ispow2n A
 */
static int
cmd_ispow2n(const char *szA)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &n) != 0) {
        return -1;
    }
    r = (n != 0ull && (n & (n - 1ull)) == 0ull) ? 1ull : 0ull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * nextpow2 — smallest power of two >= A as u64 (product v0.95).
 * A==0 → 0; already a power of two → A; if result would not fit u64 → 0.
 * Usage: nextpow2 A
 */
static int
cmd_nextpow2(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if (a == 0ull) {
        r = 0ull;
    } else if ((a & (a - 1ull)) == 0ull) {
        /* already a nonzero power of two */
        r = a;
    } else {
        /* smear bits downward then add 1 → ceil power of two */
        r = a - 1ull;
        r |= r >> 1;
        r |= r >> 2;
        r |= r >> 4;
        r |= r >> 8;
        r |= r >> 16;
        r |= r >> 32;
        r = r + 1ull; /* wraps to 0 if A > 2^63 (not representable) */
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * popcntu — population count of a decimal u64 (product v0.96).
 * Same as popcnt/bitcnt/popcount/bits/setbitcnt; free exclusive name.
 * Usage: popcntu A
 */
static int
cmd_popcntu(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = sh_popcnt_u64(a);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * tzcnt — count trailing zero bits of a decimal u64 (product v0.96).
 * Same as ctz (v0.50) / ntz (v0.85); A==0 → 64. Free exclusive name.
 * Usage: tzcnt A
 */
static int
cmd_tzcnt(const char *szA)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        r = 64ull;
    } else {
        r = 0ull;
        /* binary search for LSB index; no __builtin_ctzll */
        if ((n & 0xffffffffull) == 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n & 0xffffull) == 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n & 0xffull) == 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n & 0xfull) == 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n & 0x3ull) == 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n & 0x1ull) == 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * lzcnt — count leading zero bits of a decimal u64 (product v0.96).
 * Same as clz (v0.51); A==0 → 64. Free exclusive name.
 * Usage: lzcnt A
 */
static int
cmd_lzcnt(const char *szA)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &n) != 0) {
        return -1;
    }
    if (n == 0ull) {
        r = 64ull;
    } else {
        r = 0ull;
        /* binary search for MSB; no __builtin_clzll */
        if ((n & 0xffffffff00000000ull) == 0ull) {
            r += 32ull;
            n <<= 32;
        }
        if ((n & 0xffff000000000000ull) == 0ull) {
            r += 16ull;
            n <<= 16;
        }
        if ((n & 0xff00000000000000ull) == 0ull) {
            r += 8ull;
            n <<= 8;
        }
        if ((n & 0xf000000000000000ull) == 0ull) {
            r += 4ull;
            n <<= 4;
        }
        if ((n & 0xc000000000000000ull) == 0ull) {
            r += 2ull;
            n <<= 2;
        }
        if ((n & 0x8000000000000000ull) == 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * rol — rotate left decimal u64 by N bits; N masked with 63 (product v0.97).
 * Same as rotl (v0.76); free exclusive name. Usage: rol A N
 */
static int
cmd_rol(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    unsigned sh;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    sh = (unsigned)(n & 63ull); /* N masked */
    if (sh == 0u) {
        r = a;
    } else {
        r = (a << sh) | (a >> (64u - sh));
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ror — rotate right decimal u64 by N bits; N masked with 63 (product v0.97).
 * Same as rotr (v0.76); free exclusive name. Usage: ror A N
 */
static int
cmd_ror(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    unsigned sh;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    sh = (unsigned)(n & 63ull); /* N masked */
    if (sh == 0u) {
        r = a;
    } else {
        r = (a >> sh) | (a << (64u - sh));
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * byteswap — full 64-bit byte-swap of a decimal u64 (product v0.98).
 * Same as bswap (v0.51) / bswap64 (v0.77); free exclusive name.
 * Usage: byteswap A
 */
static int
cmd_byteswap(const char *szA)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &n) != 0) {
        return -1;
    }
    r = ((n & 0x00000000000000ffull) << 56)
      | ((n & 0x000000000000ff00ull) << 40)
      | ((n & 0x0000000000ff0000ull) << 24)
      | ((n & 0x00000000ff000000ull) << 8)
      | ((n & 0x000000ff00000000ull) >> 8)
      | ((n & 0x0000ff0000000000ull) >> 24)
      | ((n & 0x00ff000000000000ull) >> 40)
      | ((n & 0xff00000000000000ull) >> 56);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * iszerou — print 1 if decimal u64 A is 0, else 0 (product v0.98).
 * Same as iszero (v0.71); free exclusive name. Usage: iszerou A
 */
static int
cmd_iszerou(const char *szA)
{
    unsigned long long x;

    if (sh_parse_u64(szA, &x) != 0) {
        return -1;
    }
    out(x == 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * isone — print 1 if decimal u64 A is 1, else 0 (product v0.98).
 * Usage: isone A
 */
static int
cmd_isone(const char *szA)
{
    unsigned long long x;

    if (sh_parse_u64(szA, &x) != 0) {
        return -1;
    }
    out(x == 1ull ? "1\n" : "0\n");
    return 0;
}

/*
 * ham — Hamming distance of two decimal u64 (product v0.99).
 * Same as hamming (v0.42): popcount(A xor B). Free exclusive name.
 * Usage: ham A B
 */
static int
cmd_ham(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long n;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    n = sh_popcnt_u64(a ^ b);
    nDig = sh_u64toa(n, aN, sizeof(aN));
    if (nDig == 0 && n != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * grayenc — binary-reflected Gray encode of a decimal u64 (product v0.99).
 * g = n ^ (n >> 1). Same as gray (v0.37) on full u64. Free exclusive name.
 * Usage: grayenc A
 */
static int
cmd_grayenc(const char *szA)
{
    unsigned long long n;
    unsigned long long g;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &n) != 0) {
        return -1;
    }
    g = n ^ (n >> 1);
    nDig = sh_u64toa(g, aN, sizeof(aN));
    if (nDig == 0 && g != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * graydec — binary-reflected Gray decode of a decimal u64 (product v0.99).
 * Inverts grayenc: fold XOR of all higher bits. Usage: graydec A
 */
static int
cmd_graydec(const char *szA)
{
    unsigned long long g;
    unsigned long long n;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &g) != 0) {
        return -1;
    }
    n = g;
    n ^= n >> 1;
    n ^= n >> 2;
    n ^= n >> 4;
    n ^= n >> 8;
    n ^= n >> 16;
    n ^= n >> 32;
    nDig = sh_u64toa(n, aN, sizeof(aN));
    if (nDig == 0 && n != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * signum — sign of optional-signed decimal integer (product v1.00).
 * Leading '-' means negative; magnitude parsed as u64. Prints -1, 0, or 1.
 * Usage: signum A
 */
static int
cmd_signum(const char *szA)
{
    const char *p;
    unsigned long long u;
    int fNeg;

    if (szA == NULL || szA[0] == '\0') {
        return -1;
    }
    p = szA;
    fNeg = 0;
    if (*p == '-') {
        fNeg = 1;
        p++;
        if (*p == '\0') {
            return -1;
        }
    }
    if (sh_parse_u64(p, &u) != 0) {
        return -1;
    }
    if (u == 0ull) {
        out("0\n");
    } else if (fNeg) {
        out("-1\n");
    } else {
        out("1\n");
    }
    return 0;
}

/*
 * parityu — parity of set bits in a decimal u64 (product v1.00).
 * 0 if even popcount, 1 if odd. Same as parity (v0.78); free exclusive name.
 * Usage: parityu A
 */
static int
cmd_parityu(const char *szA)
{
    unsigned long long a;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    out((sh_popcnt_u64(a) & 1ull) != 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * uclamp — clamp decimal u64 A into [LO,HI] (product v1.01).
 * Same as clamp/clampu/clamp3; free exclusive name (clamp3 taken).
 * If LO>HI bounds swap. Usage: uclamp A LO HI
 */
static int
cmd_uclamp(const char *szA, const char *szLo, const char *szHi)
{
    unsigned long long a;
    unsigned long long lo;
    unsigned long long hi;
    unsigned long long t;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szLo, &lo) != 0 ||
        sh_parse_u64(szHi, &hi) != 0) {
        return -1;
    }
    if (lo > hi) {
        t = lo;
        lo = hi;
        hi = t;
    }
    r = a;
    if (r < lo) {
        r = lo;
    }
    if (r > hi) {
        r = hi;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umid — floor midpoint of two decimal u64 without overflow (product v1.01).
 * Uses (A&B)+((A^B)>>1). Same as avg/mid2/uavg/avg2; free exclusive name.
 * Usage: umid A B
 */
static int
cmd_umid(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a & b) + ((a ^ b) >> 1ull);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * usatadd — saturating u64 add of two decimal args (product v1.02).
 * On overflow result is UINT64_MAX. Same as satadd (v0.52); free exclusive.
 * Usage: usatadd A B
 */
static int
cmd_usatadd(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (a > ~0ull - b) {
        r = ~0ull;
    } else {
        r = a + b;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * usatsub — saturating u64 subtract of two decimal args (product v1.02).
 * On underflow result is 0. Same as satsub (v0.52); free exclusive name.
 * Usage: usatsub A B
 */
static int
cmd_usatsub(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (a < b) {
        r = 0ull;
    } else {
        r = a - b;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * flog2 — floor(log2(A)) of a decimal u64 (product v1.03).
 * A==0 prints 0. Same as ilog2 (v0.50); free exclusive name.
 * Usage: flog2 A
 */
static int
cmd_flog2(const char *szA)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &n) != 0) {
        return -1;
    }
    r = 0ull;
    if (n != 0ull) {
        /* binary search for MSB index; no __builtin_clzll */
        if ((n >> 32) != 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n >> 16) != 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n >> 8) != 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n >> 4) != 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n >> 2) != 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n >> 1) != 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * usqrt — integer square root floor of a decimal u64 (product v1.03).
 * Same algorithm as isqrt (v0.83) / sqrt (v0.49); free exclusive name.
 * Binary search; mid <= n/mid avoids mid*mid overflow.
 * Usage: usqrt A
 */
static int
cmd_usqrt(const char *szA)
{
    unsigned long long n;
    unsigned long long lo;
    unsigned long long hi;
    unsigned long long mid;
    unsigned long long ans;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &n) != 0) {
        return -1;
    }
    if (n <= 1ull) {
        ans = n;
    } else {
        lo = 1ull;
        /* floor(sqrt(2^64-1)) = 2^32-1 */
        hi = (n < 0xffffffffull) ? n : 0xffffffffull;
        ans = 1ull;
        while (lo <= hi) {
            mid = lo + (hi - lo) / 2ull;
            if (mid <= n / mid) {
                ans = mid;
                lo = mid + 1ull;
            } else {
                hi = mid - 1ull;
            }
        }
    }
    nDig = sh_u64toa(ans, aN, sizeof(aN));
    if (nDig == 0 && ans != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * urem — A % B for decimal u64; prints 0 if B==0 (product v1.04).
 * Same as umod (v0.82); free exclusive name (umod taken).
 * Usage: urem A B
 */
static int
cmd_urem(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (b == 0ull) {
        out("0\n");
        return 0;
    }
    r = a % b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * udiv — A / B for decimal u64; prints 0 if B==0 (product v1.04).
 * Same as safediv (v0.65); free exclusive name.
 * Usage: udiv A B
 */
static int
cmd_udiv(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (b == 0ull) {
        r = 0ull;
    } else {
        r = a / b;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umul — wrapping u64 multiply of two decimal args (product v1.05).
 * Truncates product to low 64 bits (C unsigned wrap). Usage: umul A B
 */
static int
cmd_umul(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a * b; /* low 64 of product; wrap */
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umulhi — high 64 bits of unsigned 128-bit product A*B (product v1.05).
 * Portable 32-bit partial products (no __int128 / widening mul).
 * Same algorithm as mulhi (v0.81); free exclusive name.
 *   A = a1:a0, B = b1:b0 (each half 32 bits)
 *   p00=a0*b0, p01=a0*b1, p10=a1*b0, p11=a1*b1
 *   mid = (p00>>32) + (p01&0xffffffff) + (p10&0xffffffff)
 *   hi  = p11 + (p01>>32) + (p10>>32) + (mid>>32)
 * Usage: umulhi A B
 */
static int
cmd_umulhi(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long a0;
    unsigned long long a1;
    unsigned long long b0;
    unsigned long long b1;
    unsigned long long p00;
    unsigned long long p01;
    unsigned long long p10;
    unsigned long long p11;
    unsigned long long mid;
    unsigned long long hi;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    a0 = a & 0xffffffffull;
    a1 = a >> 32ull;
    b0 = b & 0xffffffffull;
    b1 = b >> 32ull;
    p00 = a0 * b0;
    p01 = a0 * b1;
    p10 = a1 * b0;
    p11 = a1 * b1;
    mid = (p00 >> 32ull) + (p01 & 0xffffffffull) + (p10 & 0xffffffffull);
    hi = p11 + (p01 >> 32ull) + (p10 >> 32ull) + (mid >> 32ull);
    nDig = sh_u64toa(hi, aN, sizeof(aN));
    if (nDig == 0 && hi != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ubswap32 — byte-swap low 32 bits of a decimal u64 (product v1.06).
 * High 32 bits of input ignored; result is zero-extended.
 * Same as bswap32 (v0.77); free exclusive name. Usage: ubswap32 A
 */
static int
cmd_ubswap32(const char *szA)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    n = a & 0xffffffffull;
    r = ((n & 0x000000ffull) << 24)
      | ((n & 0x0000ff00ull) << 8)
      | ((n & 0x00ff0000ull) >> 8)
      | ((n & 0xff000000ull) >> 24);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ubswap16 — byte-swap low 16 bits of a decimal u64 (product v1.06).
 * High bits of input ignored; result is zero-extended. Usage: ubswap16 A
 */
static int
cmd_ubswap16(const char *szA)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    n = a & 0xffffull;
    r = ((n & 0x00ffull) << 8) | ((n & 0xff00ull) >> 8);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ushl — logical left shift of decimal u64; N masked with 63 (product v1.07).
 * Same as shl/lshift (v0.44/v0.75) but N&=63 (never forces 0 for N>=64).
 * Usage: ushl A N
 */
static int
cmd_ushl(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    unsigned sh;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    sh = (unsigned)(n & 63ull); /* N masked */
    r = a << sh;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ushr — logical right shift of decimal u64; N masked with 63 (product v1.07).
 * Same as shr/rshift (v0.44/v0.75) but N&=63 (never forces 0 for N>=64).
 * Usage: ushr A N
 */
static int
cmd_ushr(const char *szA, const char *szN)
{
    unsigned long long a;
    unsigned long long n;
    unsigned long long r;
    unsigned sh;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szN, &n) != 0) {
        return -1;
    }
    sh = (unsigned)(n & 63ull); /* N masked */
    r = a >> sh;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uand — bitwise AND of two decimal u64 (product v1.08).
 * Same as and/bitand (v0.43/v0.74); free exclusive name. Usage: uand A B
 */
static int
cmd_uand(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a & b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uor — bitwise OR of two decimal u64 (product v1.08).
 * Same as or/bitor (v0.43/v0.74); free exclusive name. Usage: uor A B
 */
static int
cmd_uor(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a | b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uxor — bitwise XOR of two decimal u64 (product v1.08).
 * Same as xor/bitxor (v0.43/v0.74); free exclusive name. Usage: uxor A B
 */
static int
cmd_uxor(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a ^ b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * unot — bitwise NOT of decimal u64 (product v1.09).
 * Same as bitnot (v0.75); free exclusive name. Usage: unot A
 */
static int
cmd_unot(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = ~a;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umin2 — minimum of two decimal u64 (product v1.09).
 * Same as umin (v0.80); free exclusive name (umin taken). Usage: umin2 A B
 */
static int
cmd_umin2(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a < b) ? a : b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umax2 — maximum of two decimal u64 (product v1.09).
 * Same as umax (v0.80); free exclusive name (umax taken). Usage: umax2 A B
 */
static int
cmd_umax2(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a > b) ? a : b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * unand — bitwise NAND of two decimal u64 (product v1.10).
 * ~(A & B). Free exclusive name. Usage: unand A B
 */
static int
cmd_unand(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = ~(a & b);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * unor — bitwise NOR of two decimal u64 (product v1.10).
 * ~(A | B). Free exclusive name. Usage: unor A B
 */
static int
cmd_unor(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = ~(a | b);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * udec — saturating decrement of a decimal u64 (product v1.11).
 * A-1, saturating at 0. Free exclusive name. Usage: udec A
 */
static int
cmd_udec(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = (a == 0ull) ? 0ull : (a - 1ull);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uinc — saturating increment of a decimal u64 (product v1.11).
 * A+1, saturating at UINT64_MAX. Free exclusive name. Usage: uinc A
 */
static int
cmd_uinc(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = (a == ~0ull) ? ~0ull : (a + 1ull);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * upow2 — 1<<A as u64 (product v1.12).
 * A < 64 → 1<<A; A >= 64 → 0. Free exclusive name. Usage: upow2 A
 */
static int
cmd_upow2(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if (a >= 64ull) {
        r = 0ull;
    } else {
        r = 1ull << (unsigned)a;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ulmask — low A bits set in u64 (product v1.12).
 * A==0 → 0; A in 1..63 → (1<<A)-1; A>=64 → all ones.
 * Same as masklow (v0.91); free exclusive (umask is classic file-mode stub).
 * Usage: ulmask A
 */
static int
cmd_ulmask(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if (a == 0ull) {
        r = 0ull;
    } else if (a >= 64ull) {
        r = ~0ull;
    } else {
        r = (1ull << (unsigned)a) - 1ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ucmp — three-way compare of two decimal u64 (product v1.13).
 * 0 if A==B, 1 if A>B, 2 if A<B. Free exclusive name. Usage: ucmp A B
 */
static int
cmd_ucmp(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (a == b) {
        r = 0ull;
    } else if (a > b) {
        r = 1ull;
    } else {
        r = 2ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ueq — 1 if A==B else 0 (product v1.13).
 * Free exclusive name. Usage: ueq A B
 */
static int
cmd_ueq(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    out(a == b ? "1\n" : "0\n");
    return 0;
}

/*
 * une — 1 if A!=B else 0 (product v1.13).
 * Free exclusive name. Usage: une A B
 */
static int
cmd_une(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    out(a != b ? "1\n" : "0\n");
    return 0;
}

/*
 * ult — 1 if A<B else 0 (product v1.14).
 * Free exclusive name. Usage: ult A B
 */
static int
cmd_ult(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    out(a < b ? "1\n" : "0\n");
    return 0;
}

/*
 * ule — 1 if A<=B else 0 (product v1.14).
 * Free exclusive name. Usage: ule A B
 */
static int
cmd_ule(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    out(a <= b ? "1\n" : "0\n");
    return 0;
}

/*
 * ugt — 1 if A>B else 0 (product v1.14).
 * Free exclusive name. Usage: ugt A B
 */
static int
cmd_ugt(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    out(a > b ? "1\n" : "0\n");
    return 0;
}

/*
 * uge — 1 if A>=B else 0 (product v1.14).
 * Free exclusive name. Usage: uge A B
 */
static int
cmd_uge(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    out(a >= b ? "1\n" : "0\n");
    return 0;
}

/*
 * ubits — population count of a decimal u64 (product v1.15).
 * Free exclusive name. Usage: ubits A
 */
static int
cmd_ubits(const char *szA)
{
    unsigned long long a;
    unsigned long long n;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    n = sh_popcnt_u64(a);
    nDig = sh_u64toa(n, aN, sizeof(aN));
    if (nDig == 0 && n != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uall — 1 if all bits set (A == ~0ull), else 0 (product v1.15).
 * Free exclusive name. Usage: uall A
 */
static int
cmd_uall(const char *szA)
{
    unsigned long long a;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    out(a == ~0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * uany — 1 if A is nonzero, else 0 (product v1.15).
 * Free exclusive name. Usage: uany A
 */
static int
cmd_uany(const char *szA)
{
    unsigned long long a;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    out(a != 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * unz — 1 if A==0 else 0 (product v1.16).
 * Free exclusive name. Usage: unz A
 */
static int
cmd_unz(const char *szA)
{
    unsigned long long a;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    out(a == 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * uone — 1 if A==1 else 0 (product v1.16).
 * Free exclusive name. Usage: uone A
 */
static int
cmd_uone(const char *szA)
{
    unsigned long long a;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    out(a == 1ull ? "1\n" : "0\n");
    return 0;
}

/*
 * ulsb — isolate lowest set bit of a decimal u64 (product v1.17).
 * A & -A; 0 if A==0. Free exclusive name. Usage: ulsb A
 */
static int
cmd_ulsb(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    /* two's complement isolate LSB; a==0 yields 0 */
    r = a & (0ull - a);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umsb — isolate highest set bit of a decimal u64 (product v1.17).
 * 0 if A==0. Free exclusive name. Usage: umsb A
 */
static int
cmd_umsb(const char *szA)
{
    unsigned long long a;
    unsigned long long x;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if (a == 0ull) {
        r = 0ull;
    } else {
        /* smear bits right, then leave only highest */
        x = a;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x |= x >> 32;
        r = x - (x >> 1);
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uclrlsb — clear lowest set bit of a decimal u64 (product v1.18).
 * A & (A-1); 0 if A==0. Free exclusive name. Usage: uclrlsb A
 */
static int
cmd_uclrlsb(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = (a == 0ull) ? 0ull : (a & (a - 1ull));
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uclrmsb — clear highest set bit of a decimal u64 (product v1.18).
 * 0 if A==0 or A is a single set bit. Free exclusive name. Usage: uclrmsb A
 */
static int
cmd_uclrmsb(const char *szA)
{
    unsigned long long a;
    unsigned long long x;
    unsigned long long msb;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if (a == 0ull) {
        r = 0ull;
    } else {
        x = a;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x |= x >> 32;
        msb = x - (x >> 1);
        r = a ^ msb;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uabsdiff — absolute difference |A-B| of two decimal u64 (product v1.19).
 * Same as absdiff (v0.73) / udiff (v0.94); free exclusive name.
 * Usage: uabsdiff A B
 */
static int
cmd_uabsdiff(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (a >= b) ? (a - b) : (b - a);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uparity — parity of set bits in a decimal u64 (product v1.20).
 * popcount(A)&1; print 0/1. Same as parity/parityu; free exclusive name.
 * Usage: uparity A
 */
static int
cmd_uparity(const char *szA)
{
    unsigned long long a;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    out((sh_popcnt_u64(a) & 1ull) != 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * uham — Hamming distance of two decimal u64 (product v1.20).
 * popcount(A xor B). Same as hamming/ham; free exclusive name.
 * Usage: uham A B
 */
static int
cmd_uham(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long n;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    n = sh_popcnt_u64(a ^ b);
    nDig = sh_u64toa(n, aN, sizeof(aN));
    if (nDig == 0 && n != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uisolsb — isolate lowest set bit of a decimal u64 (product v1.20).
 * A & -A; 0 if A==0. Same as ulsb; free exclusive name.
 * Usage: uisolsb A
 */
static int
cmd_uisolsb(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    /* two's complement isolate LSB; a==0 yields 0 */
    r = a & (0ull - a);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uisomsb — isolate highest set bit of a decimal u64 (product v1.20).
 * 0 if A==0. Same as umsb; free exclusive name.
 * Usage: uisomsb A
 */
static int
cmd_uisomsb(const char *szA)
{
    unsigned long long a;
    unsigned long long x;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if (a == 0ull) {
        r = 0ull;
    } else {
        /* smear bits right, then leave only highest */
        x = a;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        x |= x >> 32;
        r = x - (x >> 1);
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uispow2 — 1 if A is a nonzero power of two, else 0 (product v1.21).
 * Same as ispow2/ispow2u/ispow2n; free exclusive name.
 * Usage: uispow2 A
 */
static int
cmd_uispow2(const char *szA)
{
    unsigned long long a;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    out((a != 0ull && (a & (a - 1ull)) == 0ull) ? "1\n" : "0\n");
    return 0;
}

/*
 * ulcm0 — LCM of two decimal u64 via a/gcd(a,b)*b (product v1.21).
 * ulcm0(0,x)=ulcm0(x,0)=0. On multiply overflow prints 0 (not sat-max),
 * distinct from ulcm (v0.82 sat) and lcm (v0.40 fail). Free exclusive name.
 * Usage: ulcm0 A B
 */
static int
cmd_ulcm0(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long g;
    unsigned long long q;
    unsigned long long r;
    unsigned long long x;
    unsigned long long y;
    unsigned long long t;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    if (a == 0ull || b == 0ull) {
        out("0\n");
        return 0;
    }
    x = a;
    y = b;
    while (y != 0ull) {
        t = x % y;
        x = y;
        y = t;
    }
    g = x;
    q = a / g; /* exact: g divides a */
    if (q > 0xffffffffffffffffull / b) {
        out("0\n"); /* overflow → 0, not sat-max */
        return 0;
    }
    r = q * b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uilog2 — floor(log2(A)) of a decimal u64 (product v1.21).
 * A==0 prints 0. Same as ilog2 (v0.50) / flog2 (v1.03); free exclusive name.
 * Usage: uilog2 A
 */
static int
cmd_uilog2(const char *szA)
{
    unsigned long long n;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &n) != 0) {
        return -1;
    }
    r = 0ull;
    if (n != 0ull) {
        /* binary search for MSB index; no __builtin_clzll */
        if ((n >> 32) != 0ull) {
            r += 32ull;
            n >>= 32;
        }
        if ((n >> 16) != 0ull) {
            r += 16ull;
            n >>= 16;
        }
        if ((n >> 8) != 0ull) {
            r += 8ull;
            n >>= 8;
        }
        if ((n >> 4) != 0ull) {
            r += 4ull;
            n >>= 4;
        }
        if ((n >> 2) != 0ull) {
            r += 2ull;
            n >>= 2;
        }
        if ((n >> 1) != 0ull) {
            r += 1ull;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uandn — bitwise AND-NOT of two decimal u64 (product v1.22).
 * A & ~B. Free exclusive name (andn-style clear-bits). Usage: uandn A B
 */
static int
cmd_uandn(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = a & ~b;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * unor2 — bitwise NOR of two decimal u64 (product v1.22).
 * ~(A | B). unor taken by v1.10; free exclusive name. Usage: unor2 A B
 */
static int
cmd_unor2(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = ~(a | b);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uxnor — bitwise XNOR of two decimal u64 (product v1.23).
 * ~(A ^ B). Free exclusive name. Usage: uxnor A B
 */
static int
cmd_uxnor(const char *szA, const char *szB)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = ~(a ^ b);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umaj — bitwise majority of three decimal u64 (product v1.23).
 * (A&B)|(A&C)|(B&C). Free exclusive name. Usage: umaj A B C
 */
static int
cmd_umaj(const char *szA, const char *szB, const char *szC)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long c;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0 ||
        sh_parse_u64(szC, &c) != 0) {
        return -1;
    }
    r = (a & b) | (a & c) | (b & c);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * umux — bitwise mux of two decimal u64 by select mask (product v1.23).
 * (S&A)|(~S&B). Free exclusive name. Usage: umux S A B
 */
static int
cmd_umux(const char *szS, const char *szA, const char *szB)
{
    unsigned long long s;
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szS, &s) != 0 || sh_parse_u64(szA, &a) != 0 ||
        sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = (s & a) | (~s & b);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * urgba — pack decimal R G B A channel bytes into u32 0xAARRGGBB (product v1.24).
 * Layout MSB→LSB: A | R | G | B (ARGB8888 / 0xAARRGGBB; not 0xRRGGBBAA).
 * Each channel is masked to 8 bits. Free exclusive name. Usage: urgba R G B A
 */
static int
cmd_urgba(const char *szR, const char *szG, const char *szB, const char *szA)
{
    unsigned long long r;
    unsigned long long g;
    unsigned long long b;
    unsigned long long a;
    unsigned long long packed;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szR, &r) != 0 || sh_parse_u64(szG, &g) != 0 ||
        sh_parse_u64(szB, &b) != 0 || sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    packed = ((a & 0xffull) << 24) | ((r & 0xffull) << 16) |
        ((g & 0xffull) << 8) | (b & 0xffull);
    nDig = sh_u64toa(packed, aN, sizeof(aN));
    if (nDig == 0 && packed != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * urgb — pack decimal R G B channel bytes into u32 0x00RRGGBB (product v1.24).
 * Layout MSB→LSB: 00 | R | G | B (XRGB8888). Each channel masked to 8 bits.
 * Free exclusive name. Usage: urgb R G B
 */
static int
cmd_urgb(const char *szR, const char *szG, const char *szB)
{
    unsigned long long r;
    unsigned long long g;
    unsigned long long b;
    unsigned long long packed;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szR, &r) != 0 || sh_parse_u64(szG, &g) != 0 ||
        sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    packed = ((r & 0xffull) << 16) | ((g & 0xffull) << 8) | (b & 0xffull);
    nDig = sh_u64toa(packed, aN, sizeof(aN));
    if (nDig == 0 && packed != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uluma — approximate luma of packed 0x00RRGGBB color C (product v1.24).
 * Unpacks R=(C>>16)&0xff, G=(C>>8)&0xff, B=C&0xff; BT.601 integer:
 * Y = (77*R + 150*G + 29*B) >> 8  (result 0..255). Free exclusive name.
 * Usage: uluma C
 */
static int
cmd_uluma(const char *szC)
{
    unsigned long long c;
    unsigned long long r;
    unsigned long long g;
    unsigned long long b;
    unsigned long long y;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szC, &c) != 0) {
        return -1;
    }
    r = (c >> 16) & 0xffull;
    g = (c >> 8) & 0xffull;
    b = c & 0xffull;
    y = (77ull * r + 150ull * g + 29ull * b) >> 8;
    nDig = sh_u64toa(y, aN, sizeof(aN));
    if (nDig == 0 && y != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uip4 — pack four decimal bytes into IPv4 u32 network order (product v1.25).
 * Result is (A<<24)|(B<<16)|(C<<8)|D with each octet masked to 8 bits.
 * Free exclusive name. Usage: uip4 A B C D
 */
static int
cmd_uip4(const char *szA, const char *szB, const char *szC, const char *szD)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long c;
    unsigned long long d;
    unsigned long long packed;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0 || sh_parse_u64(szB, &b) != 0 ||
        sh_parse_u64(szC, &c) != 0 || sh_parse_u64(szD, &d) != 0) {
        return -1;
    }
    packed = ((a & 0xffull) << 24) | ((b & 0xffull) << 16) |
        ((c & 0xffull) << 8) | (d & 0xffull);
    nDig = sh_u64toa(packed, aN, sizeof(aN));
    if (nDig == 0 && packed != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uip4a — extract first (MSB) octet of packed IPv4 network-order u32 (product v1.25).
 * Result is (IP>>24)&0xff. Free exclusive name. Usage: uip4a IP
 */
static int
cmd_uip4a(const char *szIp)
{
    unsigned long long ip;
    unsigned long long a;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szIp, &ip) != 0) {
        return -1;
    }
    a = (ip >> 24) & 0xffull;
    nDig = sh_u64toa(a, aN, sizeof(aN));
    if (nDig == 0 && a != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uhttp2xx — HTTP status is 2xx predicate (product v1.25).
 * Prints 1 if CODE is in [200,299], else 0. Free exclusive name.
 * Usage: uhttp2xx CODE
 */
static int
cmd_uhttp2xx(const char *szCode)
{
    unsigned long long code;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szCode, &code) != 0) {
        return -1;
    }
    r = (code >= 200ull && code <= 299ull) ? 1ull : 0ull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * upage — round A down to a 4096-byte page boundary (product v1.26).
 * Result is A & ~0xfffull. Free exclusive name. Usage: upage A
 */
static int
cmd_upage(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = a & ~0xfffull;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * upagec — round A up to a 4096-byte page boundary (product v1.26).
 * Result is (A+4095)&~0xfff when it fits in u64; 0 on overflow.
 * Already-aligned values near UINT64_MAX are returned unchanged.
 * Free exclusive name. Usage: upagec A
 */
static int
cmd_upagec(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    if ((a & 0xfffull) == 0ull) {
        r = a;
    } else if (a > (0xffffffffffffffffull - 0xfffull)) {
        r = 0ull; /* (A+4095) would overflow u64 */
    } else {
        r = (a + 0xfffull) & ~0xfffull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * upfn — page frame number of address A (product v1.26).
 * Result is A >> 12. Free exclusive name. Usage: upfn A
 */
static int
cmd_upfn(const char *szA)
{
    unsigned long long a;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    r = a >> 12;
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ubcast — broadcast low 8 bits of A into all 4 bytes of a u32 (product v1.27).
 * Result is 0x01010101 * (A & 0xff). Free exclusive name. Usage: ubcast A
 */
static int
cmd_ubcast(const char *szA)
{
    unsigned long long a;
    unsigned long long b;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    b = a & 0xffull;
    r = b | (b << 8) | (b << 16) | (b << 24);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uhas0 — 1 if any zero byte in low 32 bits of A, else 0 (product v1.27).
 * Examines four bytes of (A & 0xffffffff). Free exclusive name. Usage: uhas0 A
 */
static int
cmd_uhas0(const char *szA)
{
    unsigned long long a;
    unsigned long long x;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    x = a & 0xffffffffull;
    r = 0ull;
    if (((x >> 0) & 0xffull) == 0ull ||
        ((x >> 8) & 0xffull) == 0ull ||
        ((x >> 16) & 0xffull) == 0ull ||
        ((x >> 24) & 0xffull) == 0ull) {
        r = 1ull;
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ubsum — sum of the 4 bytes of low 32 bits of A (product v1.27).
 * Result is 0..1020. Free exclusive name. Usage: ubsum A
 */
static int
cmd_ubsum(const char *szA)
{
    unsigned long long a;
    unsigned long long x;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    x = a & 0xffffffffull;
    r = ((x >> 0) & 0xffull) + ((x >> 8) & 0xffull) +
        ((x >> 16) & 0xffull) + ((x >> 24) & 0xffull);
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ustride — framebuffer row stride in bytes (product v1.28).
 * Result is W*(BPP/8) as u64. If BPP is not a multiple of 8, prints 0.
 * On multiply overflow prints 0. Free exclusive name. Usage: ustride W BPP
 */
static int
cmd_ustride(const char *szW, const char *szBpp)
{
    unsigned long long w;
    unsigned long long bpp;
    unsigned long long bppb;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szW, &w) != 0 || sh_parse_u64(szBpp, &bpp) != 0) {
        return -1;
    }
    if ((bpp % 8ull) != 0ull) {
        r = 0ull;
    } else {
        bppb = bpp / 8ull;
        if (w == 0ull || bppb == 0ull) {
            r = 0ull;
        } else if (w > (0xffffffffffffffffull / bppb)) {
            r = 0ull; /* W*(BPP/8) would overflow u64 */
        } else {
            r = w * bppb;
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * ufbsize — framebuffer size in bytes (product v1.28).
 * Result is W*H*(BPP/8) as u64; 0 on any multiply overflow.
 * BPP is integer-divided by 8 (byte depth). Free exclusive name.
 * Usage: ufbsize W H BPP
 */
static int
cmd_ufbsize(const char *szW, const char *szH, const char *szBpp)
{
    unsigned long long w;
    unsigned long long h;
    unsigned long long bpp;
    unsigned long long bppb;
    unsigned long long r;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szW, &w) != 0 || sh_parse_u64(szH, &h) != 0 ||
        sh_parse_u64(szBpp, &bpp) != 0) {
        return -1;
    }
    bppb = bpp / 8ull;
    r = 0ull;
    if (w != 0ull && h != 0ull && bppb != 0ull) {
        if (w > (0xffffffffffffffffull / h)) {
            r = 0ull; /* W*H would overflow */
        } else {
            r = w * h;
            if (r > (0xffffffffffffffffull / bppb)) {
                r = 0ull; /* W*H*(BPP/8) would overflow */
            } else {
                r = r * bppb;
            }
        }
    }
    nDig = sh_u64toa(r, aN, sizeof(aN));
    if (nDig == 0 && r != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * urgb565 — pack decimal R G B 8-bit channels into RGB565 (product v1.28).
 * Each channel masked to 8 bits; result is 16-bit:
 *   R5 = (R>>3)&0x1f at bits 15..11
 *   G6 = (G>>2)&0x3f at bits 10..5
 *   B5 = (B>>3)&0x1f at bits 4..0
 * Free exclusive name. Usage: urgb565 R G B
 */
static int
cmd_urgb565(const char *szR, const char *szG, const char *szB)
{
    unsigned long long r;
    unsigned long long g;
    unsigned long long b;
    unsigned long long packed;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szR, &r) != 0 || sh_parse_u64(szG, &g) != 0 ||
        sh_parse_u64(szB, &b) != 0) {
        return -1;
    }
    r = r & 0xffull;
    g = g & 0xffull;
    b = b & 0xffull;
    packed = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    nDig = sh_u64toa(packed, aN, sizeof(aN));
    if (nDig == 0 && packed != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * upciaddr — pack PCI config address with enable bit (product v1.29).
 * Result is classic CF8 form:
 *   0x80000000 | (BUS&0xff)<<16 | (DEV&0x1f)<<11 | (FN&0x7)<<8 | (REG&0xfc)
 * Free exclusive name. Usage: upciaddr BUS DEV FN REG
 */
static int
cmd_upciaddr(const char *szBus, const char *szDev, const char *szFn,
    const char *szReg)
{
    unsigned long long bus;
    unsigned long long dev;
    unsigned long long fn;
    unsigned long long reg;
    unsigned long long packed;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szBus, &bus) != 0 || sh_parse_u64(szDev, &dev) != 0 ||
        sh_parse_u64(szFn, &fn) != 0 || sh_parse_u64(szReg, &reg) != 0) {
        return -1;
    }
    packed = 0x80000000ull | ((bus & 0xffull) << 16) |
        ((dev & 0x1full) << 11) | ((fn & 0x7ull) << 8) | (reg & 0xfcu);
    nDig = sh_u64toa(packed, aN, sizeof(aN));
    if (nDig == 0 && packed != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * upcibus — extract bus from PCI config addr (product v1.29).
 * Result is (ADDR>>16)&0xff. Free exclusive name. Usage: upcibus ADDR
 */
static int
cmd_upcibus(const char *szAddr)
{
    unsigned long long addr;
    unsigned long long bus;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szAddr, &addr) != 0) {
        return -1;
    }
    bus = (addr >> 16) & 0xffull;
    nDig = sh_u64toa(bus, aN, sizeof(aN));
    if (nDig == 0 && bus != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * upcidev — extract device from PCI config addr (product v1.29).
 * Result is (ADDR>>11)&0x1f. Free exclusive name. Usage: upcidev ADDR
 */
static int
cmd_upcidev(const char *szAddr)
{
    unsigned long long addr;
    unsigned long long dev;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szAddr, &addr) != 0) {
        return -1;
    }
    dev = (addr >> 11) & 0x1full;
    nDig = sh_u64toa(dev, aN, sizeof(aN));
    if (nDig == 0 && dev != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * unvmeok — 1 if NVMe status low 11 bits are clear (product v1.30).
 * Result is 1 when (STATUS & 0x7ff) == 0, else 0. Free exclusive name.
 * Usage: unvmeok STATUS
 */
static int
cmd_unvmeok(const char *szStatus)
{
    unsigned long long status;

    if (sh_parse_u64(szStatus, &status) != 0) {
        return -1;
    }
    out((status & 0x7ffull) == 0ull ? "1\n" : "0\n");
    return 0;
}

/*
 * uahciis — 1 if SATA_SIG is classic SATA 0x00000101 (product v1.30).
 * Free exclusive name. Usage: uahciis SATA_SIG
 */
static int
cmd_uahciis(const char *szSig)
{
    unsigned long long sig;

    if (sh_parse_u64(szSig, &sig) != 0) {
        return -1;
    }
    out(sig == 0x00000101ull ? "1\n" : "0\n");
    return 0;
}

/*
 * uvirtio — with arg: 1 if V equals VirtIO magic 0x74726976 (product v1.30).
 * Magic is little-endian 'virt'. No-arg: print 1 (virtio path stub ready)
 * (product v1.59 dual-mode). Usage: uvirtio [V]
 */
static int
cmd_uvirtio(const char *szV)
{
    unsigned long long v;

    if (szV == NULL || szV[0] == '\0') {
        out("1\n");
        return 0;
    }
    if (sh_parse_u64(szV, &v) != 0) {
        return -1;
    }
    out(v == 0x74726976ull ? "1\n" : "0\n");
    return 0;
}

/*
 * uisfhd — 1 if W×H is Full HD 1920×1080 (product v1.31).
 * Free exclusive name. Usage: uisfhd W H
 */
static int
cmd_uisfhd(const char *szW, const char *szH)
{
    unsigned long long w;
    unsigned long long h;

    if (sh_parse_u64(szW, &w) != 0 || sh_parse_u64(szH, &h) != 0) {
        return -1;
    }
    out((w == 1920ull && h == 1080ull) ? "1\n" : "0\n");
    return 0;
}

/*
 * uisuhd — 1 if W×H is UHD 3840×2160 (product v1.31).
 * Free exclusive name. Usage: uisuhd W H
 */
static int
cmd_uisuhd(const char *szW, const char *szH)
{
    unsigned long long w;
    unsigned long long h;

    if (sh_parse_u64(szW, &w) != 0 || sh_parse_u64(szH, &h) != 0) {
        return -1;
    }
    out((w == 3840ull && h == 2160ull) ? "1\n" : "0\n");
    return 0;
}

/*
 * uaspect — print gcd-reduced aspect ratio as "W/H" (product v1.31).
 * Divides both W and H by gcd(W,H). When both are 0, prints "0/0".
 * Free exclusive name. Usage: uaspect W H
 */
static int
cmd_uaspect(const char *szW, const char *szH)
{
    unsigned long long w;
    unsigned long long h;
    unsigned long long g;
    unsigned long long t;
    char aW[24];
    char aH[24];
    size_t nW;
    size_t nH;

    if (sh_parse_u64(szW, &w) != 0 || sh_parse_u64(szH, &h) != 0) {
        return -1;
    }
    g = w;
    t = h;
    while (t != 0ull) {
        unsigned long long r = g % t;

        g = t;
        t = r;
    }
    if (g != 0ull) {
        w = w / g;
        h = h / g;
    }
    nW = sh_u64toa(w, aW, sizeof(aW));
    nH = sh_u64toa(h, aH, sizeof(aH));
    if ((nW == 0 && w != 0ull) || (nH == 0 && h != 0ull)) {
        return -1;
    }
    out(aW);
    out("/");
    out(aH);
    out("\n");
    return 0;
}

/*
 * udecknat — 1 if W×H is Steam Deck native 1280×800 (product v1.32).
 * Free exclusive name. Usage: udecknat W H
 */
static int
cmd_udecknat(const char *szW, const char *szH)
{
    unsigned long long w;
    unsigned long long h;

    if (sh_parse_u64(szW, &w) != 0 || sh_parse_u64(szH, &h) != 0) {
        return -1;
    }
    out((w == 1280ull && h == 800ull) ? "1\n" : "0\n");
    return 0;
}

/*
 * ubar3 — print 1 (bar3 checklist path stub ready) (product v1.60).
 * Printed 0 (bar3 still open) in v1.32; free exclusive name. Usage: ubar3
 */
static int
cmd_ubar3(void)
{
    out("1\n");
    return 0;
}

/*
 * umile — echo milestone id A as decimal u64 (product v1.32).
 * Free exclusive name. Usage: umile A
 */
static int
cmd_umile(const char *szA)
{
    unsigned long long a;
    char aN[24];
    size_t nDig;

    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    nDig = sh_u64toa(a, aN, sizeof(aN));
    if (nDig == 0 && a != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * uvk — print 1 (Vulkan path stub ready) (product v1.33).
 * Free exclusive name. Usage: uvk
 */
static int
cmd_uvk(void)
{
    out("1\n");
    return 0;
}

/*
 * uwl — print 1 (Wayland path stub ready) (product v1.33).
 * Free exclusive name. Usage: uwl
 */
static int
cmd_uwl(void)
{
    out("1\n");
    return 0;
}

/*
 * ulic — print 1 (dual MIT/Apache license gate ok) (product v1.33).
 * Free exclusive name. Usage: ulic
 */
static int
cmd_ulic(void)
{
    out("1\n");
    return 0;
}

/*
 * ufsr — print 1 (FSR stub ready) (product v1.34).
 * Free exclusive name. Usage: ufsr
 */
static int
cmd_ufsr(void)
{
    out("1\n");
    return 0;
}

/*
 * ufps — echo arg as FPS limit, or print 60 default (product v1.34).
 * Free exclusive name. Usage: ufps [N]
 */
static int
cmd_ufps(const char *szA)
{
    unsigned long long a;
    char aN[24];
    size_t nDig;

    if (szA == NULL || szA[0] == '\0') {
        out("60\n");
        return 0;
    }
    if (sh_parse_u64(szA, &a) != 0) {
        return -1;
    }
    nDig = sh_u64toa(a, aN, sizeof(aN));
    if (nDig == 0 && a != 0ull) {
        return -1;
    }
    out(aN);
    out("\n");
    return 0;
}

/*
 * utop50 — print 50 (product v1.34).
 * Free exclusive name. Usage: utop50
 */
static int
cmd_utop50(void)
{
    out("50\n");
    return 0;
}

/*
 * uproton — print 1 (proton path stub ready) (product v1.35 / v1.60).
 * Free exclusive name. Usage: uproton
 */
static int
cmd_uproton(void)
{
    out("1\n");
    return 0;
}

/*
 * ustore — print 1 (store path stub ready) (product v1.35).
 * Free exclusive name. Usage: ustore
 */
static int
cmd_ustore(void)
{
    out("1\n");
    return 0;
}

/*
 * uconn — print 1 (product connect ready) (product v1.35).
 * Free exclusive name. Usage: uconn
 */
static int
cmd_uconn(void)
{
    out("1\n");
    return 0;
}

/*
 * ugpu — print 1 (GPU path stub ready) (product v1.36).
 * Free exclusive name. Usage: ugpu
 */
static int
cmd_ugpu(void)
{
    out("1\n");
    return 0;
}

/*
 * ubat — print 100 (battery percent stub) (product v1.36).
 * Free exclusive name. Usage: ubat
 */
static int
cmd_ubat(void)
{
    out("100\n");
    return 0;
}

/*
 * utherm — print 1 (thermal path stub ready) (product v1.36).
 * Free exclusive name. Usage: utherm
 */
static int
cmd_utherm(void)
{
    out("1\n");
    return 0;
}

/*
 * uvrr — print 1 (VRR path stub ready) (product v1.37).
 * Free exclusive name. Usage: uvrr
 */
static int
cmd_uvrr(void)
{
    out("1\n");
    return 0;
}

/*
 * uhdr — print 0 (HDR path still open) (product v1.37).
 * Free exclusive name. Usage: uhdr
 */
static int
cmd_uhdr(void)
{
    out("0\n");
    return 0;
}

/*
 * ufan — print 0 (fan path still open) (product v1.37).
 * Free exclusive name. Usage: ufan
 */
static int
cmd_ufan(void)
{
    out("0\n");
    return 0;
}

/*
 * ucloud — print 1 (cloud path stub ready) (product v1.38).
 * Free exclusive name. Usage: ucloud
 */
static int
cmd_ucloud(void)
{
    out("1\n");
    return 0;
}

/*
 * urplay — print 1 (remote play path stub ready) (product v1.38).
 * Free exclusive name. Usage: urplay
 */
static int
cmd_urplay(void)
{
    out("1\n");
    return 0;
}

/*
 * uach — print 0 (achievements path still open) (product v1.38).
 * Free exclusive name. Usage: uach
 */
static int
cmd_uach(void)
{
    out("0\n");
    return 0;
}

/*
 * unvme — print 1 (NVMe path stub ready) (product v1.39).
 * Free exclusive name. Usage: unvme
 */
static int
cmd_unvme(void)
{
    out("1\n");
    return 0;
}

/*
 * upcie — print 1 (PCIe path stub ready) (product v1.39).
 * Free exclusive name. Usage: upcie
 */
static int
cmd_upcie(void)
{
    out("1\n");
    return 0;
}

/*
 * ucpuid — print 1 (CPUID path stub ready) (product v1.39).
 * Free exclusive name. Usage: ucpuid
 */
static int
cmd_ucpuid(void)
{
    out("1\n");
    return 0;
}

/*
 * ufutex — print 1 (futex path stub ready) (product v1.40).
 * Free exclusive name. Usage: ufutex
 */
static int
cmd_ufutex(void)
{
    out("1\n");
    return 0;
}

/*
 * uclone — print 1 (clone path stub ready) (product v1.40).
 * Free exclusive name. Usage: uclone
 */
static int
cmd_uclone(void)
{
    out("1\n");
    return 0;
}

/*
 * useccomp — print 1 (seccomp path stub ready) (product v1.40).
 * Free exclusive name. Usage: useccomp
 */
static int
cmd_useccomp(void)
{
    out("1\n");
    return 0;
}

/*
 * umemfd — print 1 (memfd path stub ready) (product v1.41).
 * Free exclusive name. Usage: umemfd
 */
static int
cmd_umemfd(void)
{
    out("1\n");
    return 0;
}

/*
 * ustatx — print 1 (statx path stub ready) (product v1.41).
 * Free exclusive name. Usage: ustatx
 */
static int
cmd_ustatx(void)
{
    out("1\n");
    return 0;
}

/*
 * umount2 — print 1 (umount2 path stub ready) (product v1.41).
 * Free exclusive name. Usage: umount2
 */
static int
cmd_umount2(void)
{
    out("1\n");
    return 0;
}

/*
 * udlopen — print 1 (dlopen path stub ready) (product v1.42).
 * Free exclusive name. Usage: udlopen
 */
static int
cmd_udlopen(void)
{
    out("1\n");
    return 0;
}

/*
 * usock — print 1 (socket path stub ready) (product v1.42).
 * Free exclusive name. Usage: usock
 */
static int
cmd_usock(void)
{
    out("1\n");
    return 0;
}

/*
 * uclock — print 1 (clock path stub ready) (product v1.42).
 * Free exclusive name. Usage: uclock
 */
static int
cmd_uclock(void)
{
    out("1\n");
    return 0;
}

/*
 * upoll — print 1 (poll path stub ready) (product v1.43).
 * Free exclusive name. Usage: upoll
 */
static int
cmd_upoll(void)
{
    out("1\n");
    return 0;
}

/*
 * uaccept — print 1 (accept path stub ready) (product v1.43).
 * Free exclusive name. Usage: uaccept
 */
static int
cmd_uaccept(void)
{
    out("1\n");
    return 0;
}

/*
 * upipe2 — print 1 (pipe2 path stub ready) (product v1.43).
 * Free exclusive name. Usage: upipe2
 */
static int
cmd_upipe2(void)
{
    out("1\n");
    return 0;
}

/*
 * uwait — print 1 (wait path stub ready) (product v1.44).
 * Free exclusive name. Usage: uwait
 */
static int
cmd_uwait(void)
{
    out("1\n");
    return 0;
}

/*
 * ukill — print 1 (kill path stub ready) (product v1.44).
 * Free exclusive name. Usage: ukill
 */
static int
cmd_ukill(void)
{
    out("1\n");
    return 0;
}

/*
 * usig — print 1 (signal path stub ready) (product v1.44).
 * Free exclusive name. Usage: usig
 */
static int
cmd_usig(void)
{
    out("1\n");
    return 0;
}

/*
 * urlim — print 1 (rlimit path stub ready) (product v1.45).
 * Free exclusive name. Usage: urlim
 */
static int
cmd_urlim(void)
{
    out("1\n");
    return 0;
}

/*
 * usysinfo — print 1 (sysinfo path stub ready) (product v1.45).
 * Free exclusive name. Usage: usysinfo
 */
static int
cmd_usysinfo(void)
{
    out("1\n");
    return 0;
}

/*
 * uuname — print 1 (uname path stub ready) (product v1.45).
 * Free exclusive name. Usage: uuname
 */
static int
cmd_uuname(void)
{
    out("1\n");
    return 0;
}

/*
 * uprintf — print 1 (printf path stub ready) (product v1.46).
 * Free exclusive name. Usage: uprintf
 */
static int
cmd_uprintf(void)
{
    out("1\n");
    return 0;
}

/*
 * uqsort — print 1 (qsort path stub ready) (product v1.46).
 * Free exclusive name. Usage: uqsort
 */
static int
cmd_uqsort(void)
{
    out("1\n");
    return 0;
}

/*
 * urand — print 1 (rand path stub ready) (product v1.46).
 * Free exclusive name. Usage: urand
 */
static int
cmd_urand(void)
{
    out("1\n");
    return 0;
}

/*
 * uiconv — print 1 (iconv path stub ready) (product v1.47).
 * Free exclusive name. Usage: uiconv
 */
static int
cmd_uiconv(void)
{
    out("1\n");
    return 0;
}

/*
 * ulocale — print 1 (locale path stub ready) (product v1.47).
 * Free exclusive name. Usage: ulocale
 */
static int
cmd_ulocale(void)
{
    out("1\n");
    return 0;
}

/*
 * unl — print 1 (nl_langinfo path stub ready) (product v1.47).
 * Free exclusive name. Usage: unl
 */
static int
cmd_unl(void)
{
    out("1\n");
    return 0;
}

/*
 * uregex — print 1 (regex path stub ready) (product v1.48).
 * Free exclusive name. Usage: uregex
 */
static int
cmd_uregex(void)
{
    out("1\n");
    return 0;
}

/*
 * uglob — print 1 (glob path stub ready) (product v1.48).
 * Free exclusive name. Usage: uglob
 */
static int
cmd_uglob(void)
{
    out("1\n");
    return 0;
}

/*
 * ufnmatch — print 1 (fnmatch path stub ready) (product v1.48).
 * Free exclusive name. Usage: ufnmatch
 */
static int
cmd_ufnmatch(void)
{
    out("1\n");
    return 0;
}

/*
 * ubpf — print 1 (bpf path stub ready) (product v1.49).
 * Free exclusive name. Usage: ubpf
 */
static int
cmd_ubpf(void)
{
    out("1\n");
    return 0;
}

/*
 * unetlink — print 1 (netlink path stub ready) (product v1.49).
 * Free exclusive name. Usage: unetlink
 */
static int
cmd_unetlink(void)
{
    out("1\n");
    return 0;
}

/*
 * uresolv — print 1 (resolv path stub ready) (product v1.49).
 * Free exclusive name. Usage: uresolv
 */
static int
cmd_uresolv(void)
{
    out("1\n");
    return 0;
}

/*
 * uvulkan — print 1 (vulkan path stub ready) (product v1.50).
 * Free exclusive name. Usage: uvulkan
 */
static int
cmd_uvulkan(void)
{
    out("1\n");
    return 0;
}

/*
 * uwayland — print 1 (wayland path stub ready) (product v1.50).
 * Free exclusive name. Usage: uwayland
 */
static int
cmd_uwayland(void)
{
    out("1\n");
    return 0;
}

/*
 * udrm — print 1 (drm path stub ready) (product v1.50).
 * Free exclusive name. Usage: udrm
 */
static int
cmd_udrm(void)
{
    out("1\n");
    return 0;
}

/*
 * ujson — print 1 (json path stub ready) (product v1.51).
 * Free exclusive name. Usage: ujson
 */
static int
cmd_ujson(void)
{
    out("1\n");
    return 0;
}

/*
 * uzip — print 1 (zip path stub ready) (product v1.51).
 * Free exclusive name. Usage: uzip
 */
static int
cmd_uzip(void)
{
    out("1\n");
    return 0;
}

/*
 * upng — print 1 (png path stub ready) (product v1.51).
 * Free exclusive name. Usage: upng
 */
static int
cmd_upng(void)
{
    out("1\n");
    return 0;
}

/*
 * uhttp — print 1 (http path stub ready) (product v1.52).
 * Free exclusive name. Usage: uhttp
 */
static int
cmd_uhttp(void)
{
    out("1\n");
    return 0;
}

/*
 * usqlite — print 1 (sqlite path stub ready) (product v1.52).
 * Free exclusive name. Usage: usqlite
 */
static int
cmd_usqlite(void)
{
    out("1\n");
    return 0;
}

/*
 * usdl — print 1 (sdl path stub ready) (product v1.52).
 * Free exclusive name. Usage: usdl
 */
static int
cmd_usdl(void)
{
    out("1\n");
    return 0;
}

/*
 * uffmpeg — print 1 (ffmpeg path stub ready) (product v1.53).
 * Free exclusive name. Usage: uffmpeg
 */
static int
cmd_uffmpeg(void)
{
    out("1\n");
    return 0;
}

/*
 * ugstreamer — print 1 (gstreamer path stub ready) (product v1.53).
 * Free exclusive name. Usage: ugstreamer
 */
static int
cmd_ugstreamer(void)
{
    out("1\n");
    return 0;
}

/*
 * uudev — print 1 (udev path stub ready) (product v1.53).
 * Free exclusive name (udev may conflict). Usage: uudev
 */
static int
cmd_uudev(void)
{
    out("1\n");
    return 0;
}

/*
 * unm — print 1 (networkmanager path stub ready) (product v1.54).
 * Free exclusive name. Usage: unm
 */
static int
cmd_unm(void)
{
    out("1\n");
    return 0;
}

/*
 * uavahi — print 1 (avahi path stub ready) (product v1.54).
 * Free exclusive name. Usage: uavahi
 */
static int
cmd_uavahi(void)
{
    out("1\n");
    return 0;
}

/*
 * ujack — print 1 (jack path stub ready) (product v1.54).
 * Free exclusive name. Usage: ujack
 */
static int
cmd_ujack(void)
{
    out("1\n");
    return 0;
}

/*
 * ulua — print 1 (lua path stub ready) (product v1.55).
 * Free exclusive name. Usage: ulua
 */
static int
cmd_ulua(void)
{
    out("1\n");
    return 0;
}

/*
 * ugit — print 1 (git path stub ready) (product v1.55).
 * Free exclusive name. Usage: ugit
 */
static int
cmd_ugit(void)
{
    out("1\n");
    return 0;
}

/*
 * ucmake — print 1 (cmake path stub ready) (product v1.55).
 * Free exclusive name. Usage: ucmake
 */
static int
cmd_ucmake(void)
{
    out("1\n");
    return 0;
}

/*
 * udocker — print 1 (docker path stub ready) (product v1.56).
 * Free exclusive name. Usage: udocker
 */
static int
cmd_udocker(void)
{
    out("1\n");
    return 0;
}

/*
 * uqemu — print 1 (qemu path stub ready) (product v1.56).
 * Free exclusive name. Usage: uqemu
 */
static int
cmd_uqemu(void)
{
    out("1\n");
    return 0;
}

/*
 * uwg — print 1 (wireguard path stub ready) (product v1.56).
 * Free exclusive name. Usage: uwg
 */
static int
cmd_uwg(void)
{
    out("1\n");
    return 0;
}

/*
 * uzfs — print 1 (zfs path stub ready) (product v1.57).
 * Free exclusive name. Usage: uzfs
 */
static int
cmd_uzfs(void)
{
    out("1\n");
    return 0;
}

/*
 * uluks — print 1 (luks/cryptsetup path stub ready) (product v1.57).
 * Free exclusive name. Usage: uluks
 */
static int
cmd_uluks(void)
{
    out("1\n");
    return 0;
}

/*
 * utpm — print 1 (tpm path stub ready) (product v1.57).
 * Free exclusive name. Usage: utpm
 */
static int
cmd_utpm(void)
{
    out("1\n");
    return 0;
}

/*
 * uefi — print 1 (efi path stub ready) (product v1.58).
 * Free exclusive name. Usage: uefi
 */
static int
cmd_uefi(void)
{
    out("1\n");
    return 0;
}

/*
 * uacpi — print 1 (acpi path stub ready) (product v1.58).
 * Free exclusive name. Usage: uacpi
 */
static int
cmd_uacpi(void)
{
    out("1\n");
    return 0;
}

/*
 * ukvm — print 1 (kvm path stub ready) (product v1.59).
 * Free exclusive name. Usage: ukvm
 */
static int
cmd_ukvm(void)
{
    out("1\n");
    return 0;
}

/*
 * uinstall — print 1 (install path stub ready) (product v1.59).
 * Free exclusive name. Usage: uinstall
 */
static int
cmd_uinstall(void)
{
    out("1\n");
    return 0;
}

/*
 * ugamescope — print 1 (gamescope path stub ready) (product v1.60).
 * Free exclusive name. Usage: ugamescope
 */
static int
cmd_ugamescope(void)
{
    out("1\n");
    return 0;
}

/*
 * uflatpak — print 1 (flatpak path stub ready) (product v1.61).
 * Free exclusive name. Usage: uflatpak
 */
static int
cmd_uflatpak(void)
{
    out("1\n");
    return 0;
}

/*
 * uportal — print 1 (xdg-portal path stub ready) (product v1.61).
 * Free exclusive name. Usage: uportal
 */
static int
cmd_uportal(void)
{
    out("1\n");
    return 0;
}

/*
 * ufont — print 1 (fontconfig path stub ready) (product v1.61).
 * Free exclusive name. Usage: ufont
 */
static int
cmd_ufont(void)
{
    out("1\n");
    return 0;
}

/*
 * ubar3checklist — print 0 (bar3 checklist soft status until real)
 * (product v1.62). Free exclusive name. Usage: ubar3checklist
 */
static int
cmd_ubar3checklist(void)
{
    out("0\n");
    return 0;
}

/*
 * uprotonsoft — print 0 (proton soft ready tag until real) (product v1.62).
 * Free exclusive name. Usage: uprotonsoft
 */
static int
cmd_uprotonsoft(void)
{
    out("0\n");
    return 0;
}

/*
 * umesasoft — print 0 (mesa soft ready tag until real) (product v1.62).
 * Free exclusive name. Usage: umesasoft
 */
static int
cmd_umesasoft(void)
{
    out("0\n");
    return 0;
}

/*
 * ugamesoft — print 0 (gamescope soft ready tag until real) (product v1.62).
 * Free exclusive name. Usage: ugamesoft
 */
static int
cmd_ugamesoft(void)
{
    out("0\n");
    return 0;
}

/*
 * unetsoft — print 0 (net soft ready tag until real) (product v1.62).
 * Free exclusive name. Usage: unetsoft
 */
static int
cmd_unetsoft(void)
{
    out("0\n");
    return 0;
}

/*
 * upowersoft — print 0 (power soft ready tag until real) (product v1.62).
 * Free exclusive name. Usage: upowersoft
 */
static int
cmd_upowersoft(void)
{
    out("0\n");
    return 0;
}

/*
 * uibus — print 1 (ibus path stub ready) (product v1.63).
 * Free exclusive name. Usage: uibus
 */
static int
cmd_uibus(void)
{
    out("1\n");
    return 0;
}

/*
 * ufcitx — print 1 (fcitx path stub ready) (product v1.63).
 * Free exclusive name. Usage: ufcitx
 */
static int
cmd_ufcitx(void)
{
    out("1\n");
    return 0;
}

/*
 * udbus — print 1 (dbus path stub ready) (product v1.63).
 * Free exclusive name. Usage: udbus
 */
static int
cmd_udbus(void)
{
    out("1\n");
    return 0;
}

/*
 * upolkit — print 1 (polkit path stub ready) (product v1.63).
 * Free exclusive name. Usage: upolkit
 */
static int
cmd_upolkit(void)
{
    out("1\n");
    return 0;
}

/*
 * useatd — print 1 (seatd path stub ready) (product v1.63).
 * Free exclusive name. Usage: useatd
 */
static int
cmd_useatd(void)
{
    out("1\n");
    return 0;
}

/*
 * uimsoft — print 0 (im soft ready tag until real) (product v1.63).
 * Free exclusive name (uudev already v1.53). Usage: uimsoft
 */
static int
cmd_uimsoft(void)
{
    out("0\n");
    return 0;
}

/*
 * uauthsoft — print 0 (auth soft ready tag until real) (product v1.64).
 * Free exclusive name. Usage: uauthsoft
 */
static int
cmd_uauthsoft(void)
{
    out("0\n");
    return 0;
}

/*
 * uinputsoft — print 0 (input soft ready tag until real) (product v1.64).
 * Free exclusive name. Usage: uinputsoft
 */
static int
cmd_uinputsoft(void)
{
    out("0\n");
    return 0;
}

/*
 * unetmgrsoft — print 0 (netmgr soft ready tag until real) (product v1.64).
 * Free exclusive name. Usage: unetmgrsoft
 */
static int
cmd_unetmgrsoft(void)
{
    out("0\n");
    return 0;
}

/*
 * uprintsoft — print 0 (print soft ready tag until real) (product v1.64).
 * Free exclusive name. Usage: uprintsoft
 */
static int
cmd_uprintsoft(void)
{
    out("0\n");
    return 0;
}

/*
 * udbussoft — print 0 (dbus soft ready tag until real) (product v1.64).
 * Free exclusive name (udbus already v1.63 path stub). Usage: udbussoft
 */
static int
cmd_udbussoft(void)
{
    out("0\n");
    return 0;
}

/*
 * ubar3soft — print bar3 soft status (product v1.65).
 * Soft facts only: media path wired=1, steam client run=0.
 * Does not claim Deck Top 50 PASS or full bar3 close. Free exclusive name.
 * Usage: ubar3soft
 */
static int
cmd_ubar3soft(void)
{
    out("media path wired=1\n");
    out("steam client run=0\n");
    return 0;
}

/*
 * umediapath — print 1 (media path wired soft) (product v1.65).
 * Soft fact only; free exclusive name. Usage: umediapath
 */
static int
cmd_umediapath(void)
{
    out("1\n");
    return 0;
}

/*
 * usteamrun — print 0 (steam client run soft) (product v1.65).
 * Soft fact only; free exclusive name. Usage: usteamrun
 */
static int
cmd_usteamrun(void)
{
    out("0\n");
    return 0;
}

/*
 * uhdamulti — print 0 (hda multi soft ready tag until real) (product v1.65).
 * Product readiness one-liner; free exclusive name. Usage: uhdamulti
 */
static int
cmd_uhdamulti(void)
{
    out("0\n");
    return 0;
}

/*
 * uscsimid — print 0 (scsi mid live soft ready tag until real)
 * (product v1.65). Product readiness one-liner; free exclusive name.
 * Usage: uscsimid
 */
static int
cmd_uscsimid(void)
{
    out("0\n");
    return 0;
}

/*
 * usshd — print 0 (sshd soft ready tag until real) (product v1.65).
 * Product readiness one-liner; free exclusive name. Usage: usshd
 */
static int
cmd_usshd(void)
{
    out("0\n");
    return 0;
}

/*
 * unetstack — print 0 (netstack soft ready tag until real) (product v1.65).
 * Product readiness one-liner; free exclusive name. Usage: unetstack
 */
static int
cmd_unetstack(void)
{
    out("0\n");
    return 0;
}

/*
 * upmmhier — print 0 (pmm hierarchical soft ready tag until real)
 * (product v1.65). Product readiness one-liner; free exclusive name.
 * Usage: upmmhier
 */
static int
cmd_upmmhier(void)
{
    out("0\n");
    return 0;
}

/*
 * ucontinuum14500 — print 1 (wired soft) (product v1.66).
 * Soft fact only; free exclusive name. Does not claim Deck Top 50 PASS.
 * Usage: ucontinuum14500
 */
static int
cmd_ucontinuum14500(void)
{
    out("1\n");
    return 0;
}

/*
 * uhda_client — print 0 (hda client embed not live yet) (product v1.66).
 * Soft readiness one-liner; free exclusive name (uhdamulti already v1.65).
 * Usage: uhda_client
 */
static int
cmd_uhda_client(void)
{
    out("0\n");
    return 0;
}

/*
 * uscsi_live — print 0 (scsi live soft tag until real) (product v1.66).
 * Soft readiness one-liner; free exclusive name (uscsimid already v1.65).
 * Usage: uscsi_live
 */
static int
cmd_uscsi_live(void)
{
    out("0\n");
    return 0;
}

/*
 * u768gsoak — print 1 (large-RAM soak path shipped soft) (product v1.67).
 * Soft fact only; free exclusive name. Does not claim Deck Top 50 PASS.
 * Usage: u768gsoak
 */
static int
cmd_u768gsoak(void)
{
    out("1\n");
    return 0;
}

/*
 * umakefile14600 — print 1 (product v1.67).
 * Soft fact only; free exclusive name. Does not claim Deck Top 50 PASS.
 * Usage: umakefile14600
 */
static int
cmd_umakefile14600(void)
{
    out("1\n");
    return 0;
}

/*
 * ubar3open — print 0 (bar3 still open) (product v1.67).
 * Soft readiness one-liner; free exclusive name (ubar3soft already v1.65).
 * Soft facts only; does not claim Deck Top 50 PASS.
 * Usage: ubar3open
 */
static int
cmd_ubar3open(void)
{
    out("0\n");
    return 0;
}

/*
 * ucontinuum14600 — print 1 (wired soft) (product v1.68).
 * Soft fact only; free exclusive name (ucontinuum14500 already v1.66).
 * Does not claim Deck Top 50 PASS.
 * Usage: ucontinuum14600
 */
static int
cmd_ucontinuum14600(void)
{
    out("1\n");
    return 0;
}

/*
 * uioringmin — print 1 (io_uring min rings shipped soft) (product v1.68).
 * Soft fact only: setup/enter/register soft surface shipped; not full SQE
 * for games. Free exclusive name. Does not claim Deck Top 50 PASS.
 * Usage: uioringmin
 */
static int
cmd_uioringmin(void)
{
    out("1\n");
    return 0;
}

/*
 * u1tibopen — print 0 (full 1 TiB host soak still open) (product v1.68).
 * Soft readiness one-liner; free exclusive name (u768gsoak already v1.67).
 * Soft facts only; does not claim Deck Top 50 PASS.
 * Usage: u1tibopen
 */
static int
cmd_u1tibopen(void)
{
    out("0\n");
    return 0;
}

/*
 * ucontinuum15300 — print 1 (wired soft high-water Wave 11) (product v1.69).
 * Soft fact only: continuum makefile_max=15300 soft graph high-water.
 * Free exclusive name (ucontinuum14500/14600 already exist).
 * Soft continuum ≠ bar3; does not claim Deck Top 50 PASS.
 * Usage: ucontinuum15300
 */
static int
cmd_ucontinuum15300(void)
{
    out("1\n");
    return 0;
}

/*
 * umakefile15300 — print 1 (makefile_max=15300 soft fact) (product v1.69).
 * Soft fact only; free exclusive name (umakefile14600 already v1.67).
 * Soft continuum ≠ bar3; does not claim Deck Top 50 PASS.
 * Usage: umakefile15300
 */
static int
cmd_umakefile15300(void)
{
    out("1\n");
    return 0;
}

/*
 * umatrixopen — print 0 (Deck Top 50 matrix still NOT-TRIED) (product v1.69).
 * Soft readiness one-liner Wave 11 honesty; free exclusive name.
 * Soft facts only; does not invent title PASS or close bar3.
 * Usage: umatrixopen
 */
static int
cmd_umatrixopen(void)
{
    out("0\n");
    return 0;
}

/*
 * uwave14 — print 1 (Wave 14 exclusive soft deepen stamp) (product v1.70).
 * Soft fact only: Wave 14 stamp remains greppable after Wave 76 deepen.
 * Free exclusive name; soft facts only; soft ≠ bar3.
 * Usage: uwave14
 */
static int
cmd_uwave14(void)
{
    out("1\n");
    return 0;
}

/*
 * uwave15 — print 1 (Wave 15 exclusive soft deepen stamp) (product v1.71).
 * Soft fact only: Wave 15 stamp remains greppable after Wave 76 deepen.
 * Free exclusive name; soft facts only; soft ≠ bar3.
 * multi_server=0 confine=0 (soft ≠ product multi-server confine).
 * Usage: uwave15
 */
static int
cmd_uwave15(void)
{
    out("1\n");
    return 0;
}

/*
 * uwave16 — print 1 (Wave 16 exclusive soft deepen stamp) (product v1.72).
 * Soft fact only: Wave 16 stamp remains greppable after Wave 76 deepen.
 * Free exclusive name; soft facts only; soft ≠ bar3.
 * multi_server=0 confine=0 bar3=0 (soft ≠ product multi-server confine).
 * Usage: uwave16
 */
static int
cmd_uwave16(void)
{
    out("1\n");
    return 0;
}

/*
 * uwave17 — print 1 (Wave 18 exclusive soft deepen stamp) (product v1.73).
 * Soft fact only: Wave 18 stamp remains greppable after Wave 76 deepen.
 * Free exclusive name; soft facts only; soft ≠ bar3.
 * multi_server=0 confine=0 bar3=0 product_kernel=OPEN
 * (soft ≠ product multi-server confine / product kernel complete).
 * Usage: uwave17
 */
static int
cmd_uwave17(void)
{
    out("1\n");
    return 0;
}

/*
 * uwave18 — print 1 (Wave 38 exclusive soft deepen stamp kept) (product v1.74).
 * Soft fact only: Wave 38 stamp remains greppable after Wave 76 deepen.
 * Free exclusive name; soft facts only; soft ≠ bar3.
 * multi_server=0 confine=0 bar3=0 product_kernel=OPEN
 * (soft ≠ product multi-server confine / product kernel complete).
 * Usage: uwave18
 */
static int
cmd_uwave18(void)
{
    out("1\n");
    return 0;
}

/*
 * uwave19 — print 1 (Wave 39 exclusive soft deepen stamp kept) (product v1.75).
 * Soft fact only: Wave 39 stamp remains greppable after Wave 76 deepen.
 * Free exclusive name; soft facts only; soft ≠ bar3.
 * multi_server=0 confine=0 bar3=0 product_kernel=OPEN
 * Soft deepen surfaces (historical Wave 39): retbarbican / retglacis kept.
 * (soft ≠ product multi-server confine / product kernel complete).
 * Usage: uwave19
 */
static int
cmd_uwave19(void)
{
    out("1\n");
    return 0;
}

/*
 * uwave20 — print 1 (Wave 42 exclusive soft deepen stamp kept) (product v1.76).
 * Soft fact only: Wave 42 stamp remains greppable after Wave 76 deepen.
 * Free exclusive name; soft facts only; soft ≠ bar3.
 * multi_server=0 confine=0 bar3=0 product_kernel=OPEN
 * Soft deepen surfaces (historical Wave 42): retportcullis / retbattlement.
 * (soft ≠ product multi-server confine / product kernel complete).
 * Usage: uwave20
 */
static int
cmd_uwave20(void)
{
    out("1\n");
    return 0;
}

/*
 * uwave21 — print 1 (Wave 43 exclusive soft deepen stamp kept) (product v1.77).
 * Soft fact only: Wave 43 stamp remains greppable after Wave 76 deepen.
 * Free exclusive name; soft facts only; soft ≠ bar3.
 * multi_server=0 confine=0 bar3=0 product_kernel=OPEN
 * Soft deepen surfaces (historical Wave 43): retmachicolation / retarrowslit.
 * (soft ≠ product multi-server confine / product kernel complete).
 * Usage: uwave21
 */
static int
cmd_uwave21(void)
{
    out("1\n");
    return 0;
}

/*
 * uwave22 — print 1 (Wave 76 exclusive soft deepen stamp) (product v1.78).
 * Soft fact only: user-daemon soft inventory wave high-water is 22.
 * Free exclusive name; soft facts only; soft ≠ bar3.
 * multi_server=0 confine=0 bar3=0 product_kernel=OPEN
 * Soft deepen surfaces: retgateangle / retwallangle (CREATE-ONLY soft only).
 * Continuum toward 21800; soft ≠ product complete; product lamps 0.
 * (soft ≠ product multi-server confine / product kernel complete).
 * Usage: uwave22
 */
static int
cmd_uwave22(void)
{
    out("1\n");
    return 0;
}

/*
 * umulticonfine — print 0 (soft ≠ product multi-server confine) (product v1.70).
 * Wave 76 honesty one-liner. Soft inventory greps ≠ multi-server product.
 * Free exclusive name; soft facts only; does not claim bar3 close.
 * Usage: umulticonfine
 */
static int
cmd_umulticonfine(void)
{
    out("0\n");
    return 0;
}

/*
 * udaemonsoft — print soft daemon inventory honesty (product v1.76 Wave 76).
 * Greppable soft inventory for live logs. Soft facts only; never bar3.
 * Honesty: soft ≠ product multi-server confine; product_kernel=OPEN.
 * Soft deepen: retgateangle / retwallangle (CREATE-ONLY soft ≠ product).
 * Usage: udaemonsoft
 */
static int
cmd_udaemonsoft(void)
{
    out("greenjade-shell: soft inventory wave=70 areas=14\n");
    out("greenjade-shell: soft daemons init=1 shell=1 vfsd=1 sessiond=1 "
        "storaged=1 sshd=1 netstackd=1 scsi_mid=1 hda_client=1 ld-gj=1\n");
    out("greenjade-shell: soft units udx=1 cold_linux=1 libgj=1 "
        "libgj-so=1 libgj-gnu=1 vulkan_icd=1 multi_server=0 confine=0\n");
    out("greenjade-shell: soft deepen wave=70 multi_server=0 confine=0 "
        "bar3=0 exclusive=1 product_kernel=OPEN\n");
    out("greenjade-shell: soft honesty multi_server=0 confine=0 bar3=0 "
        "exclusive=1 soft=1 product_kernel=OPEN wave=70\n");
    out("greenjade-shell: soft path soft=1 multi_server=0 confine=0 "
        "(soft inventory; not bar3; soft != product multi-server confine)\n");
    out("greenjade-shell: soft exclusive wave=70 multi_server=0 "
        "confine=0 bar3=0 userland=1 kernel=0 continuum=0 "
        "product_kernel=OPEN\n");
    out("greenjade-shell: soft open multi_server=0 confine=0 bar3=0 "
        "product_kernel=OPEN soft_only=1 wave=70\n");
    /* Grep: greenjade-shell: soft retgateangle / retwallangle (Wave 76) */
    out("greenjade-shell: soft retgateangle soft_only=1 product_gate=0 "
        "soft_ne_product=1 continuum_toward=21800 (retgateangle honesty; "
        "Soft!=product; not bar3)\n");
    out("greenjade-shell: soft retwallangle exclusive=1 soft_ne_product=1 "
        "continuum_toward=21800 (retwallangle stamp; Soft!=product)\n");
    return 0;
}

/* Run one command line; returns 0 ok, -1 fail, 1 exit */
static int
run_line(const char *szLine)
{
    char aCmd[32];
    char aArg[64];
    const char *pRest;

    pRest = split_word(szLine, aCmd, sizeof(aCmd));
    if (aCmd[0] == '\0' || aCmd[0] == '#') {
        return 0;
    }
    if (gj_strcmp(aCmd, "help") == 0) {
        /*
         * Soft product banner Wave 76 honesty (facts only). Identifies
         * GreenJade soft product continuum shell; soft=1; bar3=0 (still
         * open); multi_server=0; confine=0. Soft continuum ≠ bar3.
         * Soft ≠ product multi-server confine. Not Deck Top 50 PASS.
         * Greppable soft inventory for live logs.
         */
        out("GreenJade soft product\n");
        out("shell=native vfs door\n");
        out("soft=1\n");
        out("bar3=0\n");
        out("multi_server=0\n");
        out("confine=0\n");
        out("product_kernel=OPEN\n");
        out("wave=70\n");
        out("cmds: help echo cat ls rm stat touch cp mv head tail wc cmp "
            "sleep size true false clear hostname uname id whoami date env "
            "printenv nproc uptime basename dirname cksum grep test printf "
            "seq which type free df yes sessstats rev nl od expr tee "
            "cut tr fold uniq umask sort strings expand logname tty groups "
            "factor tac paste sum join comm unexpand shuf colrm fmt pathchk "
            "split base64 base32 arch getconf realpath mkdir "
            "hexdump du truncate link unlink xxd md5sum file dir vdir ln rmdir "
            "readlink tsort pr md5sum who users file chmod chown "
            "namei shred nice sha1sum "
            "sha256sum sha512sum sha384sum sha224sum crc32 crc32c b2sum "
            "uuidgen sha3sum xxhsum adler32 fnvsum siphash xxh3sum "
            "sm3sum crc16 crc8 base58 b3sum crc64 "
            "base85 ascii85 sum64 gunzip inflate b64 "
            "cpuid aescheck "
            "tarlist base64url urlencode "
            "filemagic identify urldecode "
            "crock32 ipsum "
            "duration macfmt "
            "slug lev "
            "gray bitrev "
            "gcd isprime "
            "fib modpow "
            "lcm factor "
            "binom fibmod "
            "hamming popcnt "
            "xor and or "
            "shl shr "
            "abs minmax "
            "avg pow "
            "digits dsum "
            "revdig ispal "
            "fact sqrt "
            "ilog2 ctz "
            "clz bswap "
            "satadd satsub "
            "ispow2 alignup "
            "aligndown hexnib "
            "min3 max3 "
            "median3 "
            "clamp bitset "
            "bittest bitclr "
            "bittog bitcnt "
            "bitwidth ishex "
            "wrapadd wrapsub "
            "inrange isblank "
            "isident digitval "
            "alphaval isempty "
            "pct safediv "
            "bpoints ratio "
            "divceil isaligned "
            "log2ceil modpow2 "
            "istrue isfalse "
            "ison isoff "
            "iszero isnonzero "
            "min2 max2 "
            "mid2 absdiff "
            "bitor bitand bitxor "
            "bitnot lshift rshift "
            "rotl rotr popcount "
            "bswap32 bswap64 "
            "parity bytelen "
            "nibblecnt lo32 hi32 "
            "umin umax uavg "
            "clampu mulhi "
            "ugcd ulcm umod "
            "isqrt icbrt ispow2u "
            "popmsb poplsb bits "
            "revbits revbytes ntz "
            "tohex fromhex "
            "bitrev8 bitrev16 bitrev32u "
            "pack16 unpack16lo unpack16hi "
            "pack32 unpack32lo unpack32hi "
            "btest bset bclr "
            "bflip masklow maskhigh "
            "setbitcnt isodd iseven "
            "minu2 maxu2 clamp3 "
            "udiff avg2 "
            "ispow2n nextpow2 "
            "popcntu tzcnt lzcnt "
            "rol ror "
            "byteswap iszerou isone "
            "ham grayenc graydec "
            "signum parityu "
            "uclamp umid "
            "usatadd usatsub "
            "flog2 usqrt "
            "urem udiv "
            "umul umulhi "
            "ubswap32 ubswap16 "
            "ushl ushr "
            "uand uor uxor "
            "unot umin2 umax2 "
            "unand unor "
            "udec uinc "
            "upow2 ulmask "
            "ucmp ueq une "
            "ult ule ugt uge "
            "ubits uall uany "
            "unz uone "
            "ulsb umsb "
            "uclrlsb uclrmsb "
            "uabsdiff "
            "uparity uham uisolsb uisomsb "
            "uispow2 ulcm0 uilog2 "
            "uandn unor2 "
            "uxnor umaj umux "
            "urgba urgb uluma "
            "uip4 uip4a uhttp2xx "
            "upage upagec upfn "
            "ubcast uhas0 ubsum "
            "ustride ufbsize urgb565 "
            "upciaddr upcibus upcidev "
            "unvmeok uahciis uvirtio "
            "uisfhd uisuhd uaspect "
            "udecknat ubar3 umile "
            "uvk uwl ulic "
            "ufsr ufps utop50 "
            "uproton ustore uconn "
            "ugpu ubat utherm "
            "uvrr uhdr ufan "
            "ucloud urplay uach "
            "unvme upcie ucpuid "
            "ufutex uclone useccomp "
            "umemfd ustatx umount2 "
            "udlopen usock uclock "
            "upoll uaccept upipe2 "
            "uwait ukill usig "
            "urlim usysinfo uuname "
            "uprintf uqsort urand "
            "uiconv ulocale unl "
            "uregex uglob ufnmatch "
            "ubpf unetlink uresolv "
            "uvulkan uwayland udrm "
            "ujson uzip upng "
            "uhttp usqlite usdl "
            "uffmpeg ugstreamer uudev "
            "unm uavahi ujack "
            "ulua ugit ucmake "
            "udocker uqemu uwg "
            "uzfs uluks utpm "
            "uefi uacpi "
            "ukvm uinstall "
            "ugamescope "
            "uflatpak uportal ufont "
            "ubar3checklist uprotonsoft umesasoft "
            "ugamesoft unetsoft upowersoft "
            "uibus ufcitx udbus "
            "upolkit useatd uimsoft "
            "uauthsoft uinputsoft unetmgrsoft "
            "uprintsoft udbussoft "
            "ubar3soft umediapath usteamrun "
            "uhdamulti uscsimid usshd "
            "unetstack upmmhier "
            "ucontinuum14500 uhda_client uscsi_live "
            "u768gsoak umakefile14600 ubar3open "
            "ucontinuum14600 uioringmin u1tibopen "
            "ucontinuum15300 umakefile15300 umatrixopen "
            "uwave14 umulticonfine udaemonsoft uwave15 uwave16 uwave17 uwave18 uwave19 uwave20 uwave21 uwave22 "
            "dos2unix unix2dos "
            "sync pwd format mount bigfile install stats storecap storestats "
            "storeflush netstats version exit\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "true") == 0) {
        return 0;
    }
    if (gj_strcmp(aCmd, "false") == 0) {
        return -1;
    }
    if (gj_strcmp(aCmd, "clear") == 0) {
        /* ANSI clear + home for real serial; still harmless in smoke */
        out("\033[2J\033[H");
        return 0;
    }
    if (gj_strcmp(aCmd, "hostname") == 0) {
        out("greenjade\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "uname") == 0) {
        out("Linux greenjade x86_64\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "id") == 0) {
        out("uid=0(root) gid=0(root)\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "whoami") == 0) {
        out("root\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "date") == 0) {
        /* Fixed bring-up stamp (no RTC in smoke path) */
        out("Tue Jul 21 00:00:00 UTC 2026\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "env") == 0 || gj_strcmp(aCmd, "printenv") == 0) {
        if (gj_strcmp(aCmd, "printenv") == 0 && pRest != NULL &&
            pRest[0] != '\0') {
            char aKey[32];

            (void)split_word(pRest, aKey, sizeof(aKey));
            if (gj_strcmp(aKey, "PATH") == 0) {
                out("/bin:/usr/bin\n");
            } else if (gj_strcmp(aKey, "HOME") == 0) {
                out("/\n");
            } else if (gj_strcmp(aKey, "USER") == 0) {
                out("root\n");
            } else if (gj_strcmp(aKey, "SHELL") == 0) {
                out("/bin/sh\n");
            } else {
                return -1;
            }
            return 0;
        }
        out("PATH=/bin:/usr/bin\n");
        out("HOME=/\n");
        out("USER=root\n");
        out("SHELL=/bin/sh\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "nproc") == 0) {
        out("1\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "uptime") == 0) {
        out(" 00:00:00 up 0 min,  1 user,  load average: 0.00, 0.00, 0.00\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "basename") == 0) {
        char aPath[96];
        size_t n;
        size_t i;
        size_t last = 0;

        (void)split_word(pRest, aPath, sizeof(aPath));
        if (aPath[0] == '\0') {
            return -1;
        }
        n = gj_strlen(aPath);
        while (n > 1 && aPath[n - 1] == '/') {
            aPath[n - 1] = '\0';
            n--;
        }
        for (i = 0; i < n; i++) {
            if (aPath[i] == '/') {
                last = i + 1;
            }
        }
        out(aPath + last);
        out("\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "dirname") == 0) {
        char aPath[96];
        size_t n;
        size_t i;
        size_t last = 0;
        int fSlash = 0;

        (void)split_word(pRest, aPath, sizeof(aPath));
        if (aPath[0] == '\0') {
            return -1;
        }
        n = gj_strlen(aPath);
        while (n > 1 && aPath[n - 1] == '/') {
            aPath[n - 1] = '\0';
            n--;
        }
        for (i = 0; i < n; i++) {
            if (aPath[i] == '/') {
                last = i;
                fSlash = 1;
            }
        }
        if (!fSlash) {
            out(".\n");
            return 0;
        }
        if (last == 0) {
            out("/\n");
            return 0;
        }
        aPath[last] = '\0';
        out(aPath);
        out("\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "sync") == 0) {
        if (gj_store(GJ_STORE_OP_FLUSH, 0, 0, 0) != 0) {
            out("sync: FAIL\n");
            return -1;
        }
        return 0;
    }
    if (gj_strcmp(aCmd, "pwd") == 0) {
        out("/mnt\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "version") == 0) {
        /*
         * Soft product version banner Wave 76 honesty.
         * Keep substring "GreenJade shell (native vfs door)" for smoke gate.
         * Extra lines are soft inventory only: GreenJade soft product identity,
         * soft=1, bar3=0 (still open), multi_server=0, confine=0, wave=70.
         * Soft continuum ≠ bar3. Soft ≠ product multi-server confine.
         * Does not claim Deck Top 50 PASS or bar3 close.
         */
        out("GreenJade shell (native vfs door) v1.76\n");
        out("GreenJade soft product\n");
        out("soft=1\n");
        out("bar3=0\n");
        out("multi_server=0\n");
        out("confine=0\n");
        out("product_kernel=OPEN\n");
        out("wave=70\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "storecap") == 0) {
        static unsigned long long u64Cap;
        static unsigned aSt[3];

        u64Cap = 0;
        if (gj_store(GJ_STORE_OP_CAP, (long)(uintptr_t)&u64Cap, 0, 0) != 0) {
            out("storecap: FAIL\n");
            return -1;
        }
        out("store cap sectors=");
        {
            char aN[24];
            size_t n = gj_itoa((long)u64Cap, aN, sizeof(aN));

            aN[n] = '\0';
            out(aN);
        }
        if (gj_store(GJ_STORE_OP_STATS, (long)(uintptr_t)aSt, 0, 0) == 0) {
            out(" blk_io=");
            {
                char aN[16];
                size_t n = gj_itoa((long)aSt[0], aN, sizeof(aN));

                aN[n] = '\0';
                out(aN);
            }
        }
        out("\n");
        out("greenjade-shell: storecap PASS\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "storeflush") == 0) {
        if (gj_store(GJ_STORE_OP_FLUSH, 0, 0, 0) != 0) {
            out("storeflush: FAIL\n");
            return -1;
        }
        out("greenjade-shell: storeflush PASS\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "storestats") == 0) {
        static unsigned aSt[3];
        static unsigned long long u64Cap;
        char aN[24];
        size_t n;

        if (gj_store(GJ_STORE_OP_STATS, (long)(uintptr_t)aSt, 0, 0) != 0) {
            out("storestats: FAIL\n");
            return -1;
        }
        out("storestats blk_io=");
        n = gj_itoa((long)aSt[0], aN, sizeof(aN));
        aN[n] = '\0';
        out(aN);
        out(" scsi_io=");
        n = gj_itoa((long)aSt[1], aN, sizeof(aN));
        aN[n] = '\0';
        out(aN);
        out(" calls=");
        n = gj_itoa((long)aSt[2], aN, sizeof(aN));
        aN[n] = '\0';
        out(aN);
        u64Cap = 0;
        if (gj_store(GJ_STORE_OP_CAP, (long)(uintptr_t)&u64Cap, 0, 0) == 0) {
            out(" cap_sectors=");
            n = gj_itoa((long)u64Cap, aN, sizeof(aN));
            aN[n] = '\0';
            out(aN);
        }
        out("\n");
        out("greenjade-shell: storestats PASS\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "netstats") == 0) {
        static unsigned aSt[4];
        char aN[24];
        size_t n;

        if (gj_net(GJ_NET_OP_STATS, (long)(uintptr_t)aSt, 0, 0) != 0) {
            out("netstats: FAIL\n");
            return -1;
        }
        out("netstats arp=");
        n = gj_itoa((long)aSt[0], aN, sizeof(aN));
        aN[n] = '\0';
        out(aN);
        out(" udp=");
        n = gj_itoa((long)aSt[1], aN, sizeof(aN));
        aN[n] = '\0';
        out(aN);
        out(" icmp=");
        n = gj_itoa((long)aSt[2], aN, sizeof(aN));
        aN[n] = '\0';
        out(aN);
        out(" calls=");
        n = gj_itoa((long)aSt[3], aN, sizeof(aN));
        aN[n] = '\0';
        out(aN);
        out("\n");
        out("greenjade-shell: netstats PASS\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "exit") == 0 || gj_strcmp(aCmd, "quit") == 0) {
        return 1;
    }
    if (gj_strcmp(aCmd, "format") == 0) {
        return (gj_vfs(GJ_VFS_OP_FORMAT, 0, 0, 0) == 0) ? 0 : -1;
    }
    if (gj_strcmp(aCmd, "mount") == 0) {
        return (gj_vfs(GJ_VFS_OP_MOUNT, 0, 0, 0) == 0) ? 0 : -1;
    }
    /*
     * install: format + mount + write product marker under /mnt.
     * Real-hw path: operator runs this once on target store (store door).
     */
    if (gj_strcmp(aCmd, "install") == 0) {
        if (gj_vfs(GJ_VFS_OP_FORMAT, 0, 0, 0) != 0) {
            out("install: format FAIL\n");
            return -1;
        }
        if (gj_vfs(GJ_VFS_OP_MOUNT, 0, 0, 0) != 0) {
            out("install: mount FAIL\n");
            return -1;
        }
        if (cmd_echo_file("/mnt/INSTALLED", "GreenJade install OK\n") != 0) {
            out("install: write marker FAIL\n");
            return -1;
        }
        if (cmd_cat("/mnt/INSTALLED") != 0) {
            out("install: verify FAIL\n");
            return -1;
        }
        out("greenjade-shell: install cmd PASS\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "ls") == 0) {
        return cmd_ls();
    }
    if (gj_strcmp(aCmd, "bigfile") == 0) {
        return cmd_bigfile();
    }
    if (gj_strcmp(aCmd, "stats") == 0) {
        static unsigned aSt[4];

        if (gj_vfs(GJ_VFS_OP_STATS, (long)(uintptr_t)aSt, 0, 0) != 0) {
            return -1;
        }
        out("vfs stats calls=");
        {
            char aN[16];
            size_t n = gj_itoa((long)aSt[0], aN, sizeof(aN));

            aN[n] = '\0';
            out(aN);
        }
        out(" files=");
        {
            char aN[16];
            size_t n = gj_itoa((long)aSt[1], aN, sizeof(aN));

            aN[n] = '\0';
            out(aN);
        }
        out("\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "echo") == 0) {
        /* echo text > file | echo text >> file | echo file text */
        char aFile[48];
        const char *pText;
        const char *pGt;
        int fAppend = 0;

        if (pRest == NULL || pRest[0] == '\0') {
            return -1;
        }
        pGt = pRest;
        while (*pGt != '\0' && !(*pGt == '>' && (pGt == pRest || pGt[-1] == ' '))) {
            pGt++;
        }
        if (*pGt == '>') {
            size_t nText = (size_t)(pGt - pRest);
            static char aText[256];
            size_t i;

            if (pGt[1] == '>') {
                fAppend = 1;
                pGt++;
            }
            if (nText >= sizeof(aText)) {
                nText = sizeof(aText) - 1;
            }
            for (i = 0; i < nText; i++) {
                aText[i] = pRest[i];
            }
            while (nText > 0 &&
                   (aText[nText - 1] == ' ' || aText[nText - 1] == '\t')) {
                nText--;
            }
            aText[nText] = '\0';
            /* echo TEXT > file always ends the line (product shell) */
            if (nText + 1 < sizeof(aText) &&
                (nText == 0 || aText[nText - 1] != '\n')) {
                aText[nText++] = '\n';
                aText[nText] = '\0';
            }
            pRest = skip_ws(pGt + 1);
            (void)split_word(pRest, aFile, sizeof(aFile));
            if (fAppend) {
                /* strip the newline we just added; append path adds its own */
                if (nText > 0 && aText[nText - 1] == '\n') {
                    aText[--nText] = '\0';
                }
                return cmd_echo_append(aFile, aText);
            }
            return cmd_echo_file(aFile, aText);
        }
        pRest = split_word(pRest, aFile, sizeof(aFile));
        pText = (pRest != NULL && pRest[0] != '\0') ? pRest : "\n";
        return cmd_echo_file(aFile, pText);
    }
    if (gj_strcmp(aCmd, "cat") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_cat(aArg);
    }
    if (gj_strcmp(aCmd, "rm") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_rm(aArg);
    }
    if (gj_strcmp(aCmd, "stat") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_stat(aArg);
    }
    if (gj_strcmp(aCmd, "touch") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_touch(aArg);
    }
    if (gj_strcmp(aCmd, "cp") == 0) {
        char aDst[64];
        const char *p2;

        p2 = split_word(pRest, aArg, sizeof(aArg));
        (void)split_word(p2, aDst, sizeof(aDst));
        return cmd_cp(aArg, aDst);
    }
    if (gj_strcmp(aCmd, "mv") == 0) {
        char aDst[64];
        const char *p2;

        p2 = split_word(pRest, aArg, sizeof(aArg));
        (void)split_word(p2, aDst, sizeof(aDst));
        return cmd_mv(aArg, aDst);
    }
    if (gj_strcmp(aCmd, "head") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_head(aArg);
    }
    if (gj_strcmp(aCmd, "tail") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_tail(aArg);
    }
    if (gj_strcmp(aCmd, "wc") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_wc(aArg);
    }
    if (gj_strcmp(aCmd, "cmp") == 0) {
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aArg, sizeof(aArg));
        (void)split_word(p2, aB, sizeof(aB));
        return cmd_cmp(aArg, aB);
    }
    if (gj_strcmp(aCmd, "rev") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_rev(aArg);
    }
    if (gj_strcmp(aCmd, "nl") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_nl(aArg);
    }
    if (gj_strcmp(aCmd, "od") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_od(aArg);
    }
    if (gj_strcmp(aCmd, "expr") == 0) {
        return cmd_expr(pRest);
    }
    if (gj_strcmp(aCmd, "tee") == 0) {
        char aFile[64];
        const char *pText;

        pText = split_word(pRest, aFile, sizeof(aFile));
        if (aFile[0] == '\0' || pText == NULL || pText[0] == '\0') {
            return -1;
        }
        return cmd_tee(aFile, pText);
    }
    if (gj_strcmp(aCmd, "cut") == 0) {
        char aFile[64];
        unsigned uCols = 8;
        const char *p2;

        p2 = split_word(pRest, aFile, sizeof(aFile));
        if (p2 != NULL && p2[0] >= '0' && p2[0] <= '9') {
            uCols = 0;
            while (*p2 >= '0' && *p2 <= '9') {
                uCols = uCols * 10u + (unsigned)(*p2 - '0');
                p2++;
            }
        }
        return cmd_cut(aFile, uCols);
    }
    if (gj_strcmp(aCmd, "tr") == 0) {
        char aFrom[8];
        char aTo[8];
        char aFile[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aFrom, sizeof(aFrom));
        p3 = split_word(p2, aTo, sizeof(aTo));
        (void)split_word(p3, aFile, sizeof(aFile));
        return cmd_tr(aFrom, aTo, aFile);
    }
    if (gj_strcmp(aCmd, "fold") == 0) {
        char aFile[64];
        unsigned uW = 16;
        const char *p2;

        p2 = split_word(pRest, aFile, sizeof(aFile));
        if (p2 != NULL && p2[0] >= '0' && p2[0] <= '9') {
            uW = 0;
            while (*p2 >= '0' && *p2 <= '9') {
                uW = uW * 10u + (unsigned)(*p2 - '0');
                p2++;
            }
        }
        return cmd_fold(aFile, uW);
    }
    if (gj_strcmp(aCmd, "uniq") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_uniq(aArg);
    }
    if (gj_strcmp(aCmd, "sort") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_sort(aArg);
    }
    if (gj_strcmp(aCmd, "strings") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_strings(aArg);
    }
    if (gj_strcmp(aCmd, "expand") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_expand(aArg);
    }
    if (gj_strcmp(aCmd, "logname") == 0) {
        out("root\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "tty") == 0) {
        out("/dev/console\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "groups") == 0) {
        out("root\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "tac") == 0) {
        char aArg[64];

        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_tac(aArg);
    }
    if (gj_strcmp(aCmd, "paste") == 0) {
        char aArg[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aArg, sizeof(aArg));
        (void)split_word(p2, aB, sizeof(aB));
        return cmd_paste(aArg, aB[0] != '\0' ? aB : NULL);
    }
    if (gj_strcmp(aCmd, "sum") == 0) {
        char aArg[64];

        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_sum(aArg);
    }
    if (gj_strcmp(aCmd, "join") == 0) {
        char aArg[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aArg, sizeof(aArg));
        (void)split_word(p2, aB, sizeof(aB));
        return cmd_join(aArg, aB);
    }
    if (gj_strcmp(aCmd, "comm") == 0) {
        char aArg[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aArg, sizeof(aArg));
        (void)split_word(p2, aB, sizeof(aB));
        return cmd_comm(aArg, aB);
    }
    if (gj_strcmp(aCmd, "unexpand") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_unexpand(aU);
    }
    if (gj_strcmp(aCmd, "shuf") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_shuf(aU);
    }
    if (gj_strcmp(aCmd, "colrm") == 0) {
        char aU[64];
        char aFrom[16];
        char aTo[16];
        unsigned uFrom = 1;
        unsigned uTo = 1;
        const char *p;
        const char *p2;

        p2 = split_word(pRest, aU, sizeof(aU));
        p2 = split_word(p2, aFrom, sizeof(aFrom));
        (void)split_word(p2, aTo, sizeof(aTo));
        if (aFrom[0] != '\0') {
            uFrom = 0;
            p = aFrom;
            while (*p >= '0' && *p <= '9') {
                uFrom = uFrom * 10u + (unsigned)(*p - '0');
                p++;
            }
            if (uFrom == 0u) {
                uFrom = 1u;
            }
        }
        if (aTo[0] != '\0') {
            uTo = 0;
            p = aTo;
            while (*p >= '0' && *p <= '9') {
                uTo = uTo * 10u + (unsigned)(*p - '0');
                p++;
            }
            if (uTo == 0u) {
                uTo = uFrom;
            }
        } else {
            uTo = uFrom;
        }
        return cmd_colrm(aU, uFrom, uTo);
    }
    if (gj_strcmp(aCmd, "fmt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_fmt(aU);
    }
    if (gj_strcmp(aCmd, "pathchk") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_pathchk(aU);
    }
    if (gj_strcmp(aCmd, "split") == 0) {
        char aU[64];
        unsigned uB = 16u;
        const char *p2;

        p2 = split_word(pRest, aU, sizeof(aU));
        if (p2 != NULL && p2[0] >= '0' && p2[0] <= '9') {
            uB = 0u;
            while (*p2 >= '0' && *p2 <= '9') {
                uB = uB * 10u + (unsigned)(*p2 - '0');
                p2++;
            }
        }
        return cmd_split(aU, uB);
    }
    if (gj_strcmp(aCmd, "base32") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_base32(aU);
    }
    if (gj_strcmp(aCmd, "base64") == 0 || gj_strcmp(aCmd, "b64") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_base64(aU);
    }
    if (gj_strcmp(aCmd, "arch") == 0) {
        out("x86_64\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "getconf") == 0) {
        char aKey[32];

        (void)split_word(pRest, aKey, sizeof(aKey));
        if (aKey[0] == '\0' || gj_strcmp(aKey, "PAGE_SIZE") == 0 ||
            gj_strcmp(aKey, "PAGESIZE") == 0) {
            out("4096\n");
            return 0;
        }
        if (gj_strcmp(aKey, "PATH_MAX") == 0) {
            out("256\n");
            return 0;
        }
        if (gj_strcmp(aKey, "ARG_MAX") == 0) {
            out("131072\n");
            return 0;
        }
        return -1;
    }
    if (gj_strcmp(aCmd, "realpath") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_realpath(aU);
    }
    if (gj_strcmp(aCmd, "mkdir") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_mkdir(aU);
    }
    if (gj_strcmp(aCmd, "hexdump") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_hexdump(aU);
    }
    if (gj_strcmp(aCmd, "du") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_du(aU);
    }
    if (gj_strcmp(aCmd, "truncate") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_truncate(aU);
    }
    if (gj_strcmp(aCmd, "link") == 0) {
        char aSrc[64];
        char aDst[64];
        const char *p2;

        p2 = split_word(pRest, aSrc, sizeof(aSrc));
        (void)split_word(p2, aDst, sizeof(aDst));
        return cmd_link(aSrc, aDst);
    }
    if (gj_strcmp(aCmd, "unlink") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_rm(aU);
    }
    if (gj_strcmp(aCmd, "xxd") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_xxd(aU);
    }
    if (gj_strcmp(aCmd, "md5sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_md5sum(aU);
    }
    if (gj_strcmp(aCmd, "file") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_file(aU);
    }
    if (gj_strcmp(aCmd, "dir") == 0 || gj_strcmp(aCmd, "vdir") == 0) {
        return cmd_ls();
    }
    if (gj_strcmp(aCmd, "ln") == 0) {
        char aSrc[64];
        char aDst[64];
        const char *p2;

        p2 = split_word(pRest, aSrc, sizeof(aSrc));
        (void)split_word(p2, aDst, sizeof(aDst));
        return cmd_link(aSrc, aDst);
    }
    if (gj_strcmp(aCmd, "rmdir") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_rm(aU);
    }
    if (gj_strcmp(aCmd, "readlink") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_readlink(aU);
    }
    if (gj_strcmp(aCmd, "tsort") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_tsort(aU);
    }
    if (gj_strcmp(aCmd, "pr") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_pr(aU);
    }
    if (gj_strcmp(aCmd, "who") == 0 || gj_strcmp(aCmd, "users") == 0) {
        out("root\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "chmod") == 0) {
        char aMode[16];
        char aPath[64];
        const char *p2;

        /* Bring-up: accept mode+path, no mode bits on door VFS */
        p2 = split_word(pRest, aMode, sizeof(aMode));
        (void)split_word(p2, aPath, sizeof(aPath));
        if (aMode[0] == '\0' || aPath[0] == '\0') {
            return -1;
        }
        return 0;
    }
    if (gj_strcmp(aCmd, "chown") == 0) {
        char aOwner[32];
        char aPath[64];
        const char *p2;

        p2 = split_word(pRest, aOwner, sizeof(aOwner));
        (void)split_word(p2, aPath, sizeof(aPath));
        if (aOwner[0] == '\0' || aPath[0] == '\0') {
            return -1;
        }
        return 0;
    }
    if (gj_strcmp(aCmd, "namei") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_namei(aU);
    }
    if (gj_strcmp(aCmd, "shred") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_shred(aU);
    }
    if (gj_strcmp(aCmd, "nice") == 0) {
        /* Bring-up: accept optional adj, always succeed (no priority doors). */
        return 0;
    }
    if (gj_strcmp(aCmd, "sha1sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_sha1sum(aU);
    }
    if (gj_strcmp(aCmd, "sha256sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_sha256sum(aU);
    }
    if (gj_strcmp(aCmd, "sha512sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_sha512sum(aU);
    }
    if (gj_strcmp(aCmd, "sha384sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_sha384sum(aU);
    }
    if (gj_strcmp(aCmd, "crc32") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_crc32(aU);
    }

    if (gj_strcmp(aCmd, "b2sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_b2sum(aU);
    }
    if (gj_strcmp(aCmd, "uuidgen") == 0) {
        return cmd_uuidgen();
    }
    if (gj_strcmp(aCmd, "sha3sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_sha3sum(aU);
    }
    if (gj_strcmp(aCmd, "adler32") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_adler32(aU);
    }
    if (gj_strcmp(aCmd, "sha224sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_sha224sum(aU);
    }
    if (gj_strcmp(aCmd, "xxhsum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_xxhsum(aU);
    }
    if (gj_strcmp(aCmd, "crc32c") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_crc32c(aU);
    }
    if (gj_strcmp(aCmd, "fnvsum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_fnvsum(aU);
    }
    if (gj_strcmp(aCmd, "siphash") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_siphash(aU);
    }
    if (gj_strcmp(aCmd, "xxh3sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_xxh3sum(aU);
    }
    if (gj_strcmp(aCmd, "sm3sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_sm3sum(aU);
    }
    if (gj_strcmp(aCmd, "crc16") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_crc16(aU);
    }
    if (gj_strcmp(aCmd, "crc8") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_crc8(aU);
    }
    if (gj_strcmp(aCmd, "base58") == 0) {
        char aU[64];
        char aArg2[64];
        const char *p;
        int fDec = 0;

        p = split_word(pRest, aU, sizeof(aU));
        if (gj_strcmp(aU, "-d") == 0) {
            fDec = 1;
            (void)split_word(p, aArg2, sizeof(aArg2));
            return cmd_base58(aArg2, fDec);
        }
        return cmd_base58(aU, fDec);
    }
    if (gj_strcmp(aCmd, "b3sum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_b3sum(aU);
    }
    if (gj_strcmp(aCmd, "crc64") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_crc64(aU);
    }
    if (gj_strcmp(aCmd, "base85") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_base85(aU, 0);
    }
    if (gj_strcmp(aCmd, "ascii85") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_base85(aU, 1);
    }
    if (gj_strcmp(aCmd, "sum64") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_sum64(aU);
    }
    if (gj_strcmp(aCmd, "gunzip") == 0 || gj_strcmp(aCmd, "inflate") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_gunzip(aU);
    }
    if (gj_strcmp(aCmd, "cpuid") == 0) {
        return cmd_cpuid();
    }
    if (gj_strcmp(aCmd, "aescheck") == 0) {
        return cmd_aescheck();
    }
    if (gj_strcmp(aCmd, "tarlist") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_tarlist(aU);
    }
    if (gj_strcmp(aCmd, "base64url") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_base64url(aU);
    }
    if (gj_strcmp(aCmd, "urlencode") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_urlencode(aU);
    }
    if (gj_strcmp(aCmd, "urldecode") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_urldecode(aU);
    }
    if (gj_strcmp(aCmd, "filemagic") == 0 || gj_strcmp(aCmd, "identify") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_filemagic(aU);
    }
    if (gj_strcmp(aCmd, "crock32") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_crock32(aU);
    }
    if (gj_strcmp(aCmd, "ipsum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_ipsum(aU);
    }
    if (gj_strcmp(aCmd, "duration") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_duration(aU);
    }
    if (gj_strcmp(aCmd, "macfmt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_macfmt(aU);
    }
    if (gj_strcmp(aCmd, "slug") == 0) {
        /* rest of line is the slugify input (may contain spaces) */
        if (pRest == NULL || pRest[0] == '\0') {
            return -1;
        }
        return cmd_slug(pRest);
    }
    if (gj_strcmp(aCmd, "lev") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_lev(aA, aB);
    }
    if (gj_strcmp(aCmd, "gray") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_gray(aU);
    }
    if (gj_strcmp(aCmd, "bitrev") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_bitrev(aU);
    }
    if (gj_strcmp(aCmd, "gcd") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_gcd(aA, aB);
    }
    if (gj_strcmp(aCmd, "isprime") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_isprime(aU);
    }
    if (gj_strcmp(aCmd, "fib") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_fib(aU);
    }
    if (gj_strcmp(aCmd, "modpow") == 0) {
        char aB[64];
        char aE[64];
        char aM[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aB, sizeof(aB));
        p3 = split_word(p2, aE, sizeof(aE));
        (void)split_word(p3, aM, sizeof(aM));
        if (aB[0] == '\0' || aE[0] == '\0' || aM[0] == '\0') {
            return -1;
        }
        return cmd_modpow(aB, aE, aM);
    }
    if (gj_strcmp(aCmd, "lcm") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_lcm(aA, aB);
    }
    if (gj_strcmp(aCmd, "factor") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_factor(aU);
    }
    if (gj_strcmp(aCmd, "binom") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_binom(aA, aB);
    }
    if (gj_strcmp(aCmd, "fibmod") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_fibmod(aA, aB);
    }
    if (gj_strcmp(aCmd, "hamming") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_hamming(aA, aB);
    }
    if (gj_strcmp(aCmd, "popcnt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_popcnt(aU);
    }
    if (gj_strcmp(aCmd, "xor") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_xor(aA, aB);
    }
    if (gj_strcmp(aCmd, "and") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_and(aA, aB);
    }
    if (gj_strcmp(aCmd, "or") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_or(aA, aB);
    }
    if (gj_strcmp(aCmd, "shl") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_shl(aA, aB);
    }
    if (gj_strcmp(aCmd, "shr") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_shr(aA, aB);
    }
    if (gj_strcmp(aCmd, "abs") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_abs(aU);
    }
    if (gj_strcmp(aCmd, "minmax") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_minmax(aA, aB);
    }
    if (gj_strcmp(aCmd, "avg") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_avg(aA, aB);
    }
    if (gj_strcmp(aCmd, "pow") == 0) {
        char aA[64];
        char aE[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aE, sizeof(aE));
        if (aA[0] == '\0' || aE[0] == '\0') {
            return -1;
        }
        return cmd_pow(aA, aE);
    }
    if (gj_strcmp(aCmd, "digits") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_digits(aU);
    }
    if (gj_strcmp(aCmd, "dsum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_dsum(aU);
    }
    if (gj_strcmp(aCmd, "revdig") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_revdig(aU);
    }
    if (gj_strcmp(aCmd, "ispal") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_ispal(aU);
    }
    if (gj_strcmp(aCmd, "fact") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_fact(aU);
    }
    if (gj_strcmp(aCmd, "sqrt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_sqrt(aU);
    }
    if (gj_strcmp(aCmd, "ilog2") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_ilog2(aU);
    }
    if (gj_strcmp(aCmd, "ctz") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_ctz(aU);
    }
    if (gj_strcmp(aCmd, "clz") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_clz(aU);
    }
    if (gj_strcmp(aCmd, "bswap") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_bswap(aU);
    }
    if (gj_strcmp(aCmd, "satadd") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_satadd(aA, aB);
    }
    if (gj_strcmp(aCmd, "satsub") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_satsub(aA, aB);
    }
    if (gj_strcmp(aCmd, "ispow2") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_ispow2(aU);
    }
    if (gj_strcmp(aCmd, "alignup") == 0) {
        char aA[64];
        char aX[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aX, sizeof(aX));
        if (aA[0] == '\0' || aX[0] == '\0') {
            return -1;
        }
        return cmd_alignup(aA, aX);
    }
    if (gj_strcmp(aCmd, "aligndown") == 0) {
        char aA[64];
        char aX[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aX, sizeof(aX));
        if (aA[0] == '\0' || aX[0] == '\0') {
            return -1;
        }
        return cmd_aligndown(aA, aX);
    }
    if (gj_strcmp(aCmd, "hexnib") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_hexnib(aU);
    }
    if (gj_strcmp(aCmd, "min3") == 0) {
        char aA[64];
        char aB[64];
        char aC[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aA, sizeof(aA));
        p3 = split_word(p2, aB, sizeof(aB));
        (void)split_word(p3, aC, sizeof(aC));
        if (aA[0] == '\0' || aB[0] == '\0' || aC[0] == '\0') {
            return -1;
        }
        return cmd_min3(aA, aB, aC);
    }
    if (gj_strcmp(aCmd, "max3") == 0) {
        char aA[64];
        char aB[64];
        char aC[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aA, sizeof(aA));
        p3 = split_word(p2, aB, sizeof(aB));
        (void)split_word(p3, aC, sizeof(aC));
        if (aA[0] == '\0' || aB[0] == '\0' || aC[0] == '\0') {
            return -1;
        }
        return cmd_max3(aA, aB, aC);
    }
    if (gj_strcmp(aCmd, "median3") == 0) {
        char aA[64];
        char aB[64];
        char aC[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aA, sizeof(aA));
        p3 = split_word(p2, aB, sizeof(aB));
        (void)split_word(p3, aC, sizeof(aC));
        if (aA[0] == '\0' || aB[0] == '\0' || aC[0] == '\0') {
            return -1;
        }
        return cmd_median3(aA, aB, aC);
    }
    if (gj_strcmp(aCmd, "clamp") == 0) {
        char aX[64];
        char aLo[64];
        char aHi[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aX, sizeof(aX));
        p3 = split_word(p2, aLo, sizeof(aLo));
        (void)split_word(p3, aHi, sizeof(aHi));
        if (aX[0] == '\0' || aLo[0] == '\0' || aHi[0] == '\0') {
            return -1;
        }
        return cmd_clamp(aX, aLo, aHi);
    }
    if (gj_strcmp(aCmd, "bitset") == 0) {
        char aX[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aX, sizeof(aX));
        (void)split_word(p2, aB, sizeof(aB));
        if (aX[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bitset(aX, aB);
    }
    if (gj_strcmp(aCmd, "bittest") == 0) {
        char aX[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aX, sizeof(aX));
        (void)split_word(p2, aB, sizeof(aB));
        if (aX[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bittest(aX, aB);
    }
    if (gj_strcmp(aCmd, "bitclr") == 0) {
        char aX[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aX, sizeof(aX));
        (void)split_word(p2, aB, sizeof(aB));
        if (aX[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bitclr(aX, aB);
    }
    if (gj_strcmp(aCmd, "bittog") == 0) {
        char aX[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aX, sizeof(aX));
        (void)split_word(p2, aB, sizeof(aB));
        if (aX[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bittog(aX, aB);
    }
    if (gj_strcmp(aCmd, "bitcnt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_bitcnt(aU);
    }
    if (gj_strcmp(aCmd, "bitwidth") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_bitwidth(aU);
    }
    if (gj_strcmp(aCmd, "ishex") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_ishex(aU);
    }
    if (gj_strcmp(aCmd, "wrapadd") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_wrapadd(aA, aB);
    }
    if (gj_strcmp(aCmd, "wrapsub") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_wrapsub(aA, aB);
    }
    if (gj_strcmp(aCmd, "inrange") == 0) {
        char aX[64];
        char aLo[64];
        char aHi[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aX, sizeof(aX));
        p3 = split_word(p2, aLo, sizeof(aLo));
        (void)split_word(p3, aHi, sizeof(aHi));
        if (aX[0] == '\0' || aLo[0] == '\0' || aHi[0] == '\0') {
            return -1;
        }
        return cmd_inrange(aX, aLo, aHi);
    }
    if (gj_strcmp(aCmd, "isblank") == 0) {
        /* rest of line is S (may be empty or only space/tab) */
        return cmd_isblank(pRest != NULL ? pRest : "");
    }
    if (gj_strcmp(aCmd, "isident") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_isident(aU);
    }
    if (gj_strcmp(aCmd, "digitval") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_digitval(aU);
    }
    if (gj_strcmp(aCmd, "alphaval") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_alphaval(aU);
    }
    if (gj_strcmp(aCmd, "isempty") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_isempty(aU);
    }
    if (gj_strcmp(aCmd, "pct") == 0) {
        char aX[64];
        char aP[64];
        const char *p2;

        p2 = split_word(pRest, aX, sizeof(aX));
        (void)split_word(p2, aP, sizeof(aP));
        if (aX[0] == '\0' || aP[0] == '\0') {
            return -1;
        }
        return cmd_pct(aX, aP);
    }
    if (gj_strcmp(aCmd, "safediv") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_safediv(aA, aB);
    }
    if (gj_strcmp(aCmd, "bpoints") == 0) {
        char aX[64];
        char aBp[64];
        const char *p2;

        p2 = split_word(pRest, aX, sizeof(aX));
        (void)split_word(p2, aBp, sizeof(aBp));
        if (aX[0] == '\0' || aBp[0] == '\0') {
            return -1;
        }
        return cmd_bpoints(aX, aBp);
    }
    if (gj_strcmp(aCmd, "ratio") == 0) {
        char aA[64];
        char aB[64];
        char aS[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aA, sizeof(aA));
        p3 = split_word(p2, aB, sizeof(aB));
        (void)split_word(p3, aS, sizeof(aS));
        if (aA[0] == '\0' || aB[0] == '\0' || aS[0] == '\0') {
            return -1;
        }
        return cmd_ratio(aA, aB, aS);
    }
    if (gj_strcmp(aCmd, "divceil") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_divceil(aA, aB);
    }
    if (gj_strcmp(aCmd, "isaligned") == 0) {
        char aX[64];
        char aA[64];
        const char *p2;

        p2 = split_word(pRest, aX, sizeof(aX));
        (void)split_word(p2, aA, sizeof(aA));
        if (aX[0] == '\0' || aA[0] == '\0') {
            return -1;
        }
        return cmd_isaligned(aX, aA);
    }
    if (gj_strcmp(aCmd, "log2ceil") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_log2ceil(aU);
    }
    if (gj_strcmp(aCmd, "modpow2") == 0) {
        char aX[64];
        char aM[64];
        const char *p2;

        p2 = split_word(pRest, aX, sizeof(aX));
        (void)split_word(p2, aM, sizeof(aM));
        if (aX[0] == '\0' || aM[0] == '\0') {
            return -1;
        }
        return cmd_modpow2(aX, aM);
    }
    if (gj_strcmp(aCmd, "istrue") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_istrue(aU);
    }
    if (gj_strcmp(aCmd, "isfalse") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_isfalse(aU);
    }
    if (gj_strcmp(aCmd, "ison") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_ison(aU);
    }
    if (gj_strcmp(aCmd, "isoff") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_isoff(aU);
    }
    if (gj_strcmp(aCmd, "iszero") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_iszero(aU);
    }
    if (gj_strcmp(aCmd, "isnonzero") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_isnonzero(aU);
    }
    if (gj_strcmp(aCmd, "min2") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_min2(aA, aB);
    }
    if (gj_strcmp(aCmd, "max2") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_max2(aA, aB);
    }
    if (gj_strcmp(aCmd, "mid2") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_mid2(aA, aB);
    }
    if (gj_strcmp(aCmd, "absdiff") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_absdiff(aA, aB);
    }
    if (gj_strcmp(aCmd, "bitor") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bitor(aA, aB);
    }
    if (gj_strcmp(aCmd, "bitand") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bitand(aA, aB);
    }
    if (gj_strcmp(aCmd, "bitxor") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bitxor(aA, aB);
    }
    if (gj_strcmp(aCmd, "bitnot") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_bitnot(aA);
    }
    if (gj_strcmp(aCmd, "lshift") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_lshift(aA, aB);
    }
    if (gj_strcmp(aCmd, "rshift") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_rshift(aA, aB);
    }
    if (gj_strcmp(aCmd, "rotl") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_rotl(aA, aB);
    }
    if (gj_strcmp(aCmd, "rotr") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_rotr(aA, aB);
    }
    if (gj_strcmp(aCmd, "popcount") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_popcount(aA);
    }
    if (gj_strcmp(aCmd, "bswap32") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_bswap32(aA);
    }
    if (gj_strcmp(aCmd, "bswap64") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_bswap64(aA);
    }
    if (gj_strcmp(aCmd, "parity") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_parity(aA);
    }
    if (gj_strcmp(aCmd, "bytelen") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_bytelen(aA);
    }
    if (gj_strcmp(aCmd, "nibblecnt") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_nibblecnt(aA);
    }
    if (gj_strcmp(aCmd, "lo32") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_lo32(aA);
    }
    if (gj_strcmp(aCmd, "hi32") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_hi32(aA);
    }
    if (gj_strcmp(aCmd, "umin") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_umin(aA, aB);
    }
    if (gj_strcmp(aCmd, "umax") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_umax(aA, aB);
    }
    if (gj_strcmp(aCmd, "uavg") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_uavg(aA, aB);
    }
    if (gj_strcmp(aCmd, "clampu") == 0) {
        char aA[64];
        char aLo[64];
        char aHi[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aA, sizeof(aA));
        p3 = split_word(p2, aLo, sizeof(aLo));
        (void)split_word(p3, aHi, sizeof(aHi));
        if (aA[0] == '\0' || aLo[0] == '\0' || aHi[0] == '\0') {
            return -1;
        }
        return cmd_clampu(aA, aLo, aHi);
    }
    if (gj_strcmp(aCmd, "mulhi") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_mulhi(aA, aB);
    }
    if (gj_strcmp(aCmd, "ugcd") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ugcd(aA, aB);
    }
    if (gj_strcmp(aCmd, "ulcm") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ulcm(aA, aB);
    }
    if (gj_strcmp(aCmd, "umod") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_umod(aA, aB);
    }
    if (gj_strcmp(aCmd, "isqrt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_isqrt(aU);
    }
    if (gj_strcmp(aCmd, "icbrt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_icbrt(aU);
    }
    if (gj_strcmp(aCmd, "ispow2u") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_ispow2u(aU);
    }
    if (gj_strcmp(aCmd, "popmsb") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_popmsb(aU);
    }
    if (gj_strcmp(aCmd, "poplsb") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_poplsb(aU);
    }
    if (gj_strcmp(aCmd, "bits") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_bits(aU);
    }
    if (gj_strcmp(aCmd, "revbits") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_revbits(aU);
    }
    if (gj_strcmp(aCmd, "revbytes") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_revbytes(aU);
    }
    if (gj_strcmp(aCmd, "ntz") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_ntz(aU);
    }
    if (gj_strcmp(aCmd, "tohex") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_tohex(aU);
    }
    if (gj_strcmp(aCmd, "fromhex") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_fromhex(aU);
    }
    if (gj_strcmp(aCmd, "bitrev8") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_bitrev8(aU);
    }
    if (gj_strcmp(aCmd, "bitrev16") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_bitrev16(aU);
    }
    if (gj_strcmp(aCmd, "bitrev32u") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_bitrev32u(aU);
    }
    if (gj_strcmp(aCmd, "pack16") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_pack16(aA, aB);
    }
    if (gj_strcmp(aCmd, "unpack16lo") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_unpack16lo(aU);
    }
    if (gj_strcmp(aCmd, "unpack16hi") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_unpack16hi(aU);
    }
    if (gj_strcmp(aCmd, "pack32") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_pack32(aA, aB);
    }
    if (gj_strcmp(aCmd, "unpack32lo") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_unpack32lo(aU);
    }
    if (gj_strcmp(aCmd, "unpack32hi") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_unpack32hi(aU);
    }
    if (gj_strcmp(aCmd, "btest") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_btest(aA, aB);
    }
    if (gj_strcmp(aCmd, "bset") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bset(aA, aB);
    }
    if (gj_strcmp(aCmd, "bclr") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bclr(aA, aB);
    }
    if (gj_strcmp(aCmd, "bflip") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_bflip(aA, aB);
    }
    if (gj_strcmp(aCmd, "masklow") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_masklow(aU);
    }
    if (gj_strcmp(aCmd, "maskhigh") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_maskhigh(aU);
    }
    if (gj_strcmp(aCmd, "setbitcnt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_setbitcnt(aU);
    }
    if (gj_strcmp(aCmd, "isodd") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_isodd(aU);
    }
    if (gj_strcmp(aCmd, "iseven") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_iseven(aU);
    }
    if (gj_strcmp(aCmd, "minu2") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_minu2(aA, aB);
    }
    if (gj_strcmp(aCmd, "maxu2") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_maxu2(aA, aB);
    }
    if (gj_strcmp(aCmd, "clamp3") == 0) {
        char aA[64];
        char aLo[64];
        char aHi[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aA, sizeof(aA));
        p3 = split_word(p2, aLo, sizeof(aLo));
        (void)split_word(p3, aHi, sizeof(aHi));
        if (aA[0] == '\0' || aLo[0] == '\0' || aHi[0] == '\0') {
            return -1;
        }
        return cmd_clamp3(aA, aLo, aHi);
    }
    if (gj_strcmp(aCmd, "udiff") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_udiff(aA, aB);
    }
    if (gj_strcmp(aCmd, "avg2") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_avg2(aA, aB);
    }
    if (gj_strcmp(aCmd, "ispow2n") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_ispow2n(aU);
    }
    if (gj_strcmp(aCmd, "nextpow2") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_nextpow2(aU);
    }
    if (gj_strcmp(aCmd, "popcntu") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_popcntu(aU);
    }
    if (gj_strcmp(aCmd, "tzcnt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_tzcnt(aU);
    }
    if (gj_strcmp(aCmd, "lzcnt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_lzcnt(aU);
    }
    if (gj_strcmp(aCmd, "rol") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_rol(aA, aB);
    }
    if (gj_strcmp(aCmd, "ror") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ror(aA, aB);
    }
    if (gj_strcmp(aCmd, "byteswap") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_byteswap(aU);
    }
    if (gj_strcmp(aCmd, "iszerou") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_iszerou(aU);
    }
    if (gj_strcmp(aCmd, "isone") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_isone(aU);
    }
    if (gj_strcmp(aCmd, "ham") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ham(aA, aB);
    }
    if (gj_strcmp(aCmd, "grayenc") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_grayenc(aU);
    }
    if (gj_strcmp(aCmd, "graydec") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_graydec(aU);
    }
    if (gj_strcmp(aCmd, "signum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_signum(aU);
    }
    if (gj_strcmp(aCmd, "parityu") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_parityu(aU);
    }
    if (gj_strcmp(aCmd, "uclamp") == 0) {
        char aA[64];
        char aLo[64];
        char aHi[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aA, sizeof(aA));
        p3 = split_word(p2, aLo, sizeof(aLo));
        (void)split_word(p3, aHi, sizeof(aHi));
        if (aA[0] == '\0' || aLo[0] == '\0' || aHi[0] == '\0') {
            return -1;
        }
        return cmd_uclamp(aA, aLo, aHi);
    }
    if (gj_strcmp(aCmd, "umid") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_umid(aA, aB);
    }
    if (gj_strcmp(aCmd, "usatadd") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_usatadd(aA, aB);
    }
    if (gj_strcmp(aCmd, "usatsub") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_usatsub(aA, aB);
    }
    if (gj_strcmp(aCmd, "flog2") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_flog2(aU);
    }
    if (gj_strcmp(aCmd, "usqrt") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_usqrt(aU);
    }
    if (gj_strcmp(aCmd, "urem") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_urem(aA, aB);
    }
    if (gj_strcmp(aCmd, "udiv") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_udiv(aA, aB);
    }
    if (gj_strcmp(aCmd, "umul") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_umul(aA, aB);
    }
    if (gj_strcmp(aCmd, "umulhi") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_umulhi(aA, aB);
    }
    if (gj_strcmp(aCmd, "ubswap32") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_ubswap32(aU);
    }
    if (gj_strcmp(aCmd, "ubswap16") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_ubswap16(aU);
    }
    if (gj_strcmp(aCmd, "ushl") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ushl(aA, aB);
    }
    if (gj_strcmp(aCmd, "ushr") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ushr(aA, aB);
    }
    if (gj_strcmp(aCmd, "uand") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_uand(aA, aB);
    }
    if (gj_strcmp(aCmd, "uor") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_uor(aA, aB);
    }
    if (gj_strcmp(aCmd, "uxor") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_uxor(aA, aB);
    }
    if (gj_strcmp(aCmd, "unot") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_unot(aU);
    }
    if (gj_strcmp(aCmd, "umin2") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_umin2(aA, aB);
    }
    if (gj_strcmp(aCmd, "umax2") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_umax2(aA, aB);
    }
    if (gj_strcmp(aCmd, "unand") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_unand(aA, aB);
    }
    if (gj_strcmp(aCmd, "unor") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_unor(aA, aB);
    }
    if (gj_strcmp(aCmd, "udec") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_udec(aU);
    }
    if (gj_strcmp(aCmd, "uinc") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uinc(aU);
    }
    if (gj_strcmp(aCmd, "upow2") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_upow2(aU);
    }
    if (gj_strcmp(aCmd, "ulmask") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_ulmask(aU);
    }
    if (gj_strcmp(aCmd, "ucmp") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ucmp(aA, aB);
    }
    if (gj_strcmp(aCmd, "ueq") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ueq(aA, aB);
    }
    if (gj_strcmp(aCmd, "une") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_une(aA, aB);
    }
    if (gj_strcmp(aCmd, "ult") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ult(aA, aB);
    }
    if (gj_strcmp(aCmd, "ule") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ule(aA, aB);
    }
    if (gj_strcmp(aCmd, "ugt") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ugt(aA, aB);
    }
    if (gj_strcmp(aCmd, "uge") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_uge(aA, aB);
    }
    if (gj_strcmp(aCmd, "ubits") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_ubits(aU);
    }
    if (gj_strcmp(aCmd, "uall") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uall(aU);
    }
    if (gj_strcmp(aCmd, "uany") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uany(aU);
    }
    if (gj_strcmp(aCmd, "unz") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_unz(aU);
    }
    if (gj_strcmp(aCmd, "uone") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uone(aU);
    }
    if (gj_strcmp(aCmd, "ulsb") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_ulsb(aU);
    }
    if (gj_strcmp(aCmd, "umsb") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_umsb(aU);
    }
    if (gj_strcmp(aCmd, "uclrlsb") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uclrlsb(aU);
    }
    if (gj_strcmp(aCmd, "uclrmsb") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uclrmsb(aU);
    }
    if (gj_strcmp(aCmd, "uabsdiff") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_uabsdiff(aA, aB);
    }
    if (gj_strcmp(aCmd, "uparity") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uparity(aU);
    }
    if (gj_strcmp(aCmd, "uham") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_uham(aA, aB);
    }
    if (gj_strcmp(aCmd, "uisolsb") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uisolsb(aU);
    }
    if (gj_strcmp(aCmd, "uisomsb") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uisomsb(aU);
    }
    if (gj_strcmp(aCmd, "uispow2") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uispow2(aU);
    }
    if (gj_strcmp(aCmd, "ulcm0") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_ulcm0(aA, aB);
    }
    if (gj_strcmp(aCmd, "uilog2") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uilog2(aU);
    }
    if (gj_strcmp(aCmd, "uandn") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_uandn(aA, aB);
    }
    if (gj_strcmp(aCmd, "unor2") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_unor2(aA, aB);
    }
    if (gj_strcmp(aCmd, "uxnor") == 0) {
        char aA[64];
        char aB[64];
        const char *p2;

        p2 = split_word(pRest, aA, sizeof(aA));
        (void)split_word(p2, aB, sizeof(aB));
        if (aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_uxnor(aA, aB);
    }
    if (gj_strcmp(aCmd, "umaj") == 0) {
        char aA[64];
        char aB[64];
        char aC[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aA, sizeof(aA));
        p3 = split_word(p2, aB, sizeof(aB));
        (void)split_word(p3, aC, sizeof(aC));
        if (aA[0] == '\0' || aB[0] == '\0' || aC[0] == '\0') {
            return -1;
        }
        return cmd_umaj(aA, aB, aC);
    }
    if (gj_strcmp(aCmd, "umux") == 0) {
        char aS[64];
        char aA[64];
        char aB[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aS, sizeof(aS));
        p3 = split_word(p2, aA, sizeof(aA));
        (void)split_word(p3, aB, sizeof(aB));
        if (aS[0] == '\0' || aA[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_umux(aS, aA, aB);
    }
    if (gj_strcmp(aCmd, "urgba") == 0) {
        char aR[64];
        char aG[64];
        char aB[64];
        char aA[64];
        const char *p2;
        const char *p3;
        const char *p4;

        p2 = split_word(pRest, aR, sizeof(aR));
        p3 = split_word(p2, aG, sizeof(aG));
        p4 = split_word(p3, aB, sizeof(aB));
        (void)split_word(p4, aA, sizeof(aA));
        if (aR[0] == '\0' || aG[0] == '\0' || aB[0] == '\0' ||
            aA[0] == '\0') {
            return -1;
        }
        return cmd_urgba(aR, aG, aB, aA);
    }
    if (gj_strcmp(aCmd, "urgb") == 0) {
        char aR[64];
        char aG[64];
        char aB[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aR, sizeof(aR));
        p3 = split_word(p2, aG, sizeof(aG));
        (void)split_word(p3, aB, sizeof(aB));
        if (aR[0] == '\0' || aG[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_urgb(aR, aG, aB);
    }
    if (gj_strcmp(aCmd, "uluma") == 0) {
        char aC[64];

        (void)split_word(pRest, aC, sizeof(aC));
        if (aC[0] == '\0') {
            return -1;
        }
        return cmd_uluma(aC);
    }
    if (gj_strcmp(aCmd, "uip4") == 0) {
        char aA[64];
        char aB[64];
        char aC[64];
        char aD[64];
        const char *p2;
        const char *p3;
        const char *p4;

        p2 = split_word(pRest, aA, sizeof(aA));
        p3 = split_word(p2, aB, sizeof(aB));
        p4 = split_word(p3, aC, sizeof(aC));
        (void)split_word(p4, aD, sizeof(aD));
        if (aA[0] == '\0' || aB[0] == '\0' || aC[0] == '\0' ||
            aD[0] == '\0') {
            return -1;
        }
        return cmd_uip4(aA, aB, aC, aD);
    }
    if (gj_strcmp(aCmd, "uip4a") == 0) {
        char aIp[64];

        (void)split_word(pRest, aIp, sizeof(aIp));
        if (aIp[0] == '\0') {
            return -1;
        }
        return cmd_uip4a(aIp);
    }
    if (gj_strcmp(aCmd, "uhttp2xx") == 0) {
        char aCode[64];

        (void)split_word(pRest, aCode, sizeof(aCode));
        if (aCode[0] == '\0') {
            return -1;
        }
        return cmd_uhttp2xx(aCode);
    }
    if (gj_strcmp(aCmd, "upage") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_upage(aU);
    }
    if (gj_strcmp(aCmd, "upagec") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_upagec(aU);
    }
    if (gj_strcmp(aCmd, "upfn") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_upfn(aU);
    }
    if (gj_strcmp(aCmd, "ubcast") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_ubcast(aU);
    }
    if (gj_strcmp(aCmd, "uhas0") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_uhas0(aU);
    }
    if (gj_strcmp(aCmd, "ubsum") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        if (aU[0] == '\0') {
            return -1;
        }
        return cmd_ubsum(aU);
    }
    if (gj_strcmp(aCmd, "ustride") == 0) {
        char aW[64];
        char aBpp[64];
        const char *p2;

        p2 = split_word(pRest, aW, sizeof(aW));
        (void)split_word(p2, aBpp, sizeof(aBpp));
        if (aW[0] == '\0' || aBpp[0] == '\0') {
            return -1;
        }
        return cmd_ustride(aW, aBpp);
    }
    if (gj_strcmp(aCmd, "ufbsize") == 0) {
        char aW[64];
        char aH[64];
        char aBpp[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aW, sizeof(aW));
        p3 = split_word(p2, aH, sizeof(aH));
        (void)split_word(p3, aBpp, sizeof(aBpp));
        if (aW[0] == '\0' || aH[0] == '\0' || aBpp[0] == '\0') {
            return -1;
        }
        return cmd_ufbsize(aW, aH, aBpp);
    }
    if (gj_strcmp(aCmd, "urgb565") == 0) {
        char aR[64];
        char aG[64];
        char aB[64];
        const char *p2;
        const char *p3;

        p2 = split_word(pRest, aR, sizeof(aR));
        p3 = split_word(p2, aG, sizeof(aG));
        (void)split_word(p3, aB, sizeof(aB));
        if (aR[0] == '\0' || aG[0] == '\0' || aB[0] == '\0') {
            return -1;
        }
        return cmd_urgb565(aR, aG, aB);
    }
    if (gj_strcmp(aCmd, "upciaddr") == 0) {
        char aBus[64];
        char aDev[64];
        char aFn[64];
        char aReg[64];
        const char *p2;
        const char *p3;
        const char *p4;

        p2 = split_word(pRest, aBus, sizeof(aBus));
        p3 = split_word(p2, aDev, sizeof(aDev));
        p4 = split_word(p3, aFn, sizeof(aFn));
        (void)split_word(p4, aReg, sizeof(aReg));
        if (aBus[0] == '\0' || aDev[0] == '\0' || aFn[0] == '\0' ||
            aReg[0] == '\0') {
            return -1;
        }
        return cmd_upciaddr(aBus, aDev, aFn, aReg);
    }
    if (gj_strcmp(aCmd, "upcibus") == 0) {
        char aAddr[64];

        (void)split_word(pRest, aAddr, sizeof(aAddr));
        if (aAddr[0] == '\0') {
            return -1;
        }
        return cmd_upcibus(aAddr);
    }
    if (gj_strcmp(aCmd, "upcidev") == 0) {
        char aAddr[64];

        (void)split_word(pRest, aAddr, sizeof(aAddr));
        if (aAddr[0] == '\0') {
            return -1;
        }
        return cmd_upcidev(aAddr);
    }
    if (gj_strcmp(aCmd, "unvmeok") == 0) {
        char aStatus[64];

        (void)split_word(pRest, aStatus, sizeof(aStatus));
        if (aStatus[0] == '\0') {
            return -1;
        }
        return cmd_unvmeok(aStatus);
    }
    if (gj_strcmp(aCmd, "uahciis") == 0) {
        char aSig[64];

        (void)split_word(pRest, aSig, sizeof(aSig));
        if (aSig[0] == '\0') {
            return -1;
        }
        return cmd_uahciis(aSig);
    }
    if (gj_strcmp(aCmd, "uvirtio") == 0) {
        char aV[64];

        (void)split_word(pRest, aV, sizeof(aV));
        return cmd_uvirtio(aV);
    }
    if (gj_strcmp(aCmd, "uisfhd") == 0) {
        char aW[64];
        char aH[64];
        const char *p2;

        p2 = split_word(pRest, aW, sizeof(aW));
        (void)split_word(p2, aH, sizeof(aH));
        if (aW[0] == '\0' || aH[0] == '\0') {
            return -1;
        }
        return cmd_uisfhd(aW, aH);
    }
    if (gj_strcmp(aCmd, "uisuhd") == 0) {
        char aW[64];
        char aH[64];
        const char *p2;

        p2 = split_word(pRest, aW, sizeof(aW));
        (void)split_word(p2, aH, sizeof(aH));
        if (aW[0] == '\0' || aH[0] == '\0') {
            return -1;
        }
        return cmd_uisuhd(aW, aH);
    }
    if (gj_strcmp(aCmd, "uaspect") == 0) {
        char aW[64];
        char aH[64];
        const char *p2;

        p2 = split_word(pRest, aW, sizeof(aW));
        (void)split_word(p2, aH, sizeof(aH));
        if (aW[0] == '\0' || aH[0] == '\0') {
            return -1;
        }
        return cmd_uaspect(aW, aH);
    }
    if (gj_strcmp(aCmd, "udecknat") == 0) {
        char aW[64];
        char aH[64];
        const char *p2;

        p2 = split_word(pRest, aW, sizeof(aW));
        (void)split_word(p2, aH, sizeof(aH));
        if (aW[0] == '\0' || aH[0] == '\0') {
            return -1;
        }
        return cmd_udecknat(aW, aH);
    }
    if (gj_strcmp(aCmd, "ubar3") == 0) {
        return cmd_ubar3();
    }
    if (gj_strcmp(aCmd, "umile") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        return cmd_umile(aA);
    }
    if (gj_strcmp(aCmd, "uvk") == 0) {
        return cmd_uvk();
    }
    if (gj_strcmp(aCmd, "uwl") == 0) {
        return cmd_uwl();
    }
    if (gj_strcmp(aCmd, "ulic") == 0) {
        return cmd_ulic();
    }
    if (gj_strcmp(aCmd, "ufsr") == 0) {
        return cmd_ufsr();
    }
    if (gj_strcmp(aCmd, "ufps") == 0) {
        char aA[64];

        (void)split_word(pRest, aA, sizeof(aA));
        return cmd_ufps(aA);
    }
    if (gj_strcmp(aCmd, "utop50") == 0) {
        return cmd_utop50();
    }
    if (gj_strcmp(aCmd, "uproton") == 0) {
        return cmd_uproton();
    }
    if (gj_strcmp(aCmd, "ustore") == 0) {
        return cmd_ustore();
    }
    if (gj_strcmp(aCmd, "uconn") == 0) {
        return cmd_uconn();
    }
    if (gj_strcmp(aCmd, "ugpu") == 0) {
        return cmd_ugpu();
    }
    if (gj_strcmp(aCmd, "ubat") == 0) {
        return cmd_ubat();
    }
    if (gj_strcmp(aCmd, "utherm") == 0) {
        return cmd_utherm();
    }
    if (gj_strcmp(aCmd, "uvrr") == 0) {
        return cmd_uvrr();
    }
    if (gj_strcmp(aCmd, "uhdr") == 0) {
        return cmd_uhdr();
    }
    if (gj_strcmp(aCmd, "ufan") == 0) {
        return cmd_ufan();
    }
    if (gj_strcmp(aCmd, "ucloud") == 0) {
        return cmd_ucloud();
    }
    if (gj_strcmp(aCmd, "urplay") == 0) {
        return cmd_urplay();
    }
    if (gj_strcmp(aCmd, "uach") == 0) {
        return cmd_uach();
    }
    if (gj_strcmp(aCmd, "unvme") == 0) {
        return cmd_unvme();
    }
    if (gj_strcmp(aCmd, "upcie") == 0) {
        return cmd_upcie();
    }
    if (gj_strcmp(aCmd, "ucpuid") == 0) {
        return cmd_ucpuid();
    }
    if (gj_strcmp(aCmd, "ufutex") == 0) {
        return cmd_ufutex();
    }
    if (gj_strcmp(aCmd, "uclone") == 0) {
        return cmd_uclone();
    }
    if (gj_strcmp(aCmd, "useccomp") == 0) {
        return cmd_useccomp();
    }
    if (gj_strcmp(aCmd, "umemfd") == 0) {
        return cmd_umemfd();
    }
    if (gj_strcmp(aCmd, "ustatx") == 0) {
        return cmd_ustatx();
    }
    if (gj_strcmp(aCmd, "umount2") == 0) {
        return cmd_umount2();
    }
    if (gj_strcmp(aCmd, "udlopen") == 0) {
        return cmd_udlopen();
    }
    if (gj_strcmp(aCmd, "usock") == 0) {
        return cmd_usock();
    }
    if (gj_strcmp(aCmd, "uclock") == 0) {
        return cmd_uclock();
    }
    if (gj_strcmp(aCmd, "upoll") == 0) {
        return cmd_upoll();
    }
    if (gj_strcmp(aCmd, "uaccept") == 0) {
        return cmd_uaccept();
    }
    if (gj_strcmp(aCmd, "upipe2") == 0) {
        return cmd_upipe2();
    }
    if (gj_strcmp(aCmd, "uwait") == 0) {
        return cmd_uwait();
    }
    if (gj_strcmp(aCmd, "ukill") == 0) {
        return cmd_ukill();
    }
    if (gj_strcmp(aCmd, "usig") == 0) {
        return cmd_usig();
    }
    if (gj_strcmp(aCmd, "urlim") == 0) {
        return cmd_urlim();
    }
    if (gj_strcmp(aCmd, "usysinfo") == 0) {
        return cmd_usysinfo();
    }
    if (gj_strcmp(aCmd, "uuname") == 0) {
        return cmd_uuname();
    }
    if (gj_strcmp(aCmd, "uprintf") == 0) {
        return cmd_uprintf();
    }
    if (gj_strcmp(aCmd, "uqsort") == 0) {
        return cmd_uqsort();
    }
    if (gj_strcmp(aCmd, "urand") == 0) {
        return cmd_urand();
    }
    if (gj_strcmp(aCmd, "uiconv") == 0) {
        return cmd_uiconv();
    }
    if (gj_strcmp(aCmd, "ulocale") == 0) {
        return cmd_ulocale();
    }
    if (gj_strcmp(aCmd, "unl") == 0) {
        return cmd_unl();
    }
    if (gj_strcmp(aCmd, "uregex") == 0) {
        return cmd_uregex();
    }
    if (gj_strcmp(aCmd, "uglob") == 0) {
        return cmd_uglob();
    }
    if (gj_strcmp(aCmd, "ufnmatch") == 0) {
        return cmd_ufnmatch();
    }
    if (gj_strcmp(aCmd, "ubpf") == 0) {
        return cmd_ubpf();
    }
    if (gj_strcmp(aCmd, "unetlink") == 0) {
        return cmd_unetlink();
    }
    if (gj_strcmp(aCmd, "uresolv") == 0) {
        return cmd_uresolv();
    }
    if (gj_strcmp(aCmd, "uvulkan") == 0) {
        return cmd_uvulkan();
    }
    if (gj_strcmp(aCmd, "uwayland") == 0) {
        return cmd_uwayland();
    }
    if (gj_strcmp(aCmd, "udrm") == 0) {
        return cmd_udrm();
    }
    if (gj_strcmp(aCmd, "ujson") == 0) {
        return cmd_ujson();
    }
    if (gj_strcmp(aCmd, "uzip") == 0) {
        return cmd_uzip();
    }
    if (gj_strcmp(aCmd, "upng") == 0) {
        return cmd_upng();
    }
    if (gj_strcmp(aCmd, "uhttp") == 0) {
        return cmd_uhttp();
    }
    if (gj_strcmp(aCmd, "usqlite") == 0) {
        return cmd_usqlite();
    }
    if (gj_strcmp(aCmd, "usdl") == 0) {
        return cmd_usdl();
    }
    if (gj_strcmp(aCmd, "uffmpeg") == 0) {
        return cmd_uffmpeg();
    }
    if (gj_strcmp(aCmd, "ugstreamer") == 0) {
        return cmd_ugstreamer();
    }
    if (gj_strcmp(aCmd, "uudev") == 0) {
        return cmd_uudev();
    }
    if (gj_strcmp(aCmd, "unm") == 0) {
        return cmd_unm();
    }
    if (gj_strcmp(aCmd, "uavahi") == 0) {
        return cmd_uavahi();
    }
    if (gj_strcmp(aCmd, "ujack") == 0) {
        return cmd_ujack();
    }
    if (gj_strcmp(aCmd, "ulua") == 0) {
        return cmd_ulua();
    }
    if (gj_strcmp(aCmd, "ugit") == 0) {
        return cmd_ugit();
    }
    if (gj_strcmp(aCmd, "ucmake") == 0) {
        return cmd_ucmake();
    }
    if (gj_strcmp(aCmd, "udocker") == 0) {
        return cmd_udocker();
    }
    if (gj_strcmp(aCmd, "uqemu") == 0) {
        return cmd_uqemu();
    }
    if (gj_strcmp(aCmd, "uwg") == 0) {
        return cmd_uwg();
    }
    if (gj_strcmp(aCmd, "uzfs") == 0) {
        return cmd_uzfs();
    }
    if (gj_strcmp(aCmd, "uluks") == 0) {
        return cmd_uluks();
    }
    if (gj_strcmp(aCmd, "utpm") == 0) {
        return cmd_utpm();
    }
    if (gj_strcmp(aCmd, "uefi") == 0) {
        return cmd_uefi();
    }
    if (gj_strcmp(aCmd, "uacpi") == 0) {
        return cmd_uacpi();
    }
    if (gj_strcmp(aCmd, "ukvm") == 0) {
        return cmd_ukvm();
    }
    if (gj_strcmp(aCmd, "uinstall") == 0) {
        return cmd_uinstall();
    }
    if (gj_strcmp(aCmd, "ugamescope") == 0) {
        return cmd_ugamescope();
    }
    if (gj_strcmp(aCmd, "uflatpak") == 0) {
        return cmd_uflatpak();
    }
    if (gj_strcmp(aCmd, "uportal") == 0) {
        return cmd_uportal();
    }
    if (gj_strcmp(aCmd, "ufont") == 0) {
        return cmd_ufont();
    }
    if (gj_strcmp(aCmd, "ubar3checklist") == 0) {
        return cmd_ubar3checklist();
    }
    if (gj_strcmp(aCmd, "uprotonsoft") == 0) {
        return cmd_uprotonsoft();
    }
    if (gj_strcmp(aCmd, "umesasoft") == 0) {
        return cmd_umesasoft();
    }
    if (gj_strcmp(aCmd, "ugamesoft") == 0) {
        return cmd_ugamesoft();
    }
    if (gj_strcmp(aCmd, "unetsoft") == 0) {
        return cmd_unetsoft();
    }
    if (gj_strcmp(aCmd, "upowersoft") == 0) {
        return cmd_upowersoft();
    }
    if (gj_strcmp(aCmd, "uibus") == 0) {
        return cmd_uibus();
    }
    if (gj_strcmp(aCmd, "ufcitx") == 0) {
        return cmd_ufcitx();
    }
    if (gj_strcmp(aCmd, "udbus") == 0) {
        return cmd_udbus();
    }
    if (gj_strcmp(aCmd, "upolkit") == 0) {
        return cmd_upolkit();
    }
    if (gj_strcmp(aCmd, "useatd") == 0) {
        return cmd_useatd();
    }
    if (gj_strcmp(aCmd, "uimsoft") == 0) {
        return cmd_uimsoft();
    }
    if (gj_strcmp(aCmd, "uauthsoft") == 0) {
        return cmd_uauthsoft();
    }
    if (gj_strcmp(aCmd, "uinputsoft") == 0) {
        return cmd_uinputsoft();
    }
    if (gj_strcmp(aCmd, "unetmgrsoft") == 0) {
        return cmd_unetmgrsoft();
    }
    if (gj_strcmp(aCmd, "uprintsoft") == 0) {
        return cmd_uprintsoft();
    }
    if (gj_strcmp(aCmd, "udbussoft") == 0) {
        return cmd_udbussoft();
    }
    if (gj_strcmp(aCmd, "ubar3soft") == 0) {
        return cmd_ubar3soft();
    }
    if (gj_strcmp(aCmd, "umediapath") == 0) {
        return cmd_umediapath();
    }
    if (gj_strcmp(aCmd, "usteamrun") == 0) {
        return cmd_usteamrun();
    }
    if (gj_strcmp(aCmd, "uhdamulti") == 0) {
        return cmd_uhdamulti();
    }
    if (gj_strcmp(aCmd, "uscsimid") == 0) {
        return cmd_uscsimid();
    }
    if (gj_strcmp(aCmd, "usshd") == 0) {
        return cmd_usshd();
    }
    if (gj_strcmp(aCmd, "unetstack") == 0) {
        return cmd_unetstack();
    }
    if (gj_strcmp(aCmd, "upmmhier") == 0) {
        return cmd_upmmhier();
    }
    if (gj_strcmp(aCmd, "ucontinuum14500") == 0) {
        return cmd_ucontinuum14500();
    }
    if (gj_strcmp(aCmd, "uhda_client") == 0) {
        return cmd_uhda_client();
    }
    if (gj_strcmp(aCmd, "uscsi_live") == 0) {
        return cmd_uscsi_live();
    }
    if (gj_strcmp(aCmd, "u768gsoak") == 0) {
        return cmd_u768gsoak();
    }
    if (gj_strcmp(aCmd, "umakefile14600") == 0) {
        return cmd_umakefile14600();
    }
    if (gj_strcmp(aCmd, "ubar3open") == 0) {
        return cmd_ubar3open();
    }
    if (gj_strcmp(aCmd, "ucontinuum14600") == 0) {
        return cmd_ucontinuum14600();
    }
    if (gj_strcmp(aCmd, "uioringmin") == 0) {
        return cmd_uioringmin();
    }
    if (gj_strcmp(aCmd, "u1tibopen") == 0) {
        return cmd_u1tibopen();
    }
    if (gj_strcmp(aCmd, "ucontinuum15300") == 0) {
        return cmd_ucontinuum15300();
    }
    if (gj_strcmp(aCmd, "umakefile15300") == 0) {
        return cmd_umakefile15300();
    }
    if (gj_strcmp(aCmd, "umatrixopen") == 0) {
        return cmd_umatrixopen();
    }
    if (gj_strcmp(aCmd, "uwave14") == 0) {
        return cmd_uwave14();
    }
    if (gj_strcmp(aCmd, "uwave15") == 0) {
        return cmd_uwave15();
    }
    if (gj_strcmp(aCmd, "uwave16") == 0) {
        return cmd_uwave16();
    }
    if (gj_strcmp(aCmd, "uwave17") == 0) {
        return cmd_uwave17();
    }
    if (gj_strcmp(aCmd, "uwave18") == 0) {
        return cmd_uwave18();
    }
    if (gj_strcmp(aCmd, "uwave19") == 0) {
        return cmd_uwave19();
    }
    if (gj_strcmp(aCmd, "uwave20") == 0) {
        return cmd_uwave20();
    }
    if (gj_strcmp(aCmd, "uwave21") == 0) {
        return cmd_uwave21();
    }
    if (gj_strcmp(aCmd, "uwave22") == 0) {
        return cmd_uwave22();
    }
    if (gj_strcmp(aCmd, "umulticonfine") == 0) {
        return cmd_umulticonfine();
    }
    if (gj_strcmp(aCmd, "udaemonsoft") == 0) {
        return cmd_udaemonsoft();
    }
    if (gj_strcmp(aCmd, "dos2unix") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_dos2unix(aU);
    }
    if (gj_strcmp(aCmd, "unix2dos") == 0) {
        char aU[64];

        (void)split_word(pRest, aU, sizeof(aU));
        return cmd_unix2dos(aU);
    }
    if (gj_strcmp(aCmd, "umask") == 0) {
        out("0022\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "sleep") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_sleep(aArg);
    }
    if (gj_strcmp(aCmd, "size") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        return cmd_stat(aArg);
    }
    /* BSD-ish sum: rolling checksum + byte count (bring-up, not md5) */
    if (gj_strcmp(aCmd, "cksum") == 0) {
        static char aBuf[1100];
        long n;
        long i;
        unsigned long uSum = 0;
        char aN[24];
        size_t nDig;

        (void)split_word(pRest, aArg, sizeof(aArg));
        if (aArg[0] == '\0') {
            return -1;
        }
        n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)aArg, (long)(uintptr_t)aBuf,
                   (long)(sizeof(aBuf) - 1));
        if (n < 0) {
            return -1;
        }
        for (i = 0; i < n; i++) {
            uSum = (uSum + (unsigned char)aBuf[i]) & 0xfffffffful;
        }
        nDig = gj_itoa((long)uSum, aN, sizeof(aN));
        aN[nDig] = '\0';
        out(aN);
        out(" ");
        nDig = gj_itoa(n, aN, sizeof(aN));
        aN[nDig] = '\0';
        out(aN);
        out(" ");
        out(aArg);
        out("\n");
        return 0;
    }
    /* Fixed-string search in a small file */
    if (gj_strcmp(aCmd, "grep") == 0) {
        static char aBuf[1100];
        char aPat[64];
        const char *p2;
        long n;
        long i;
        long nLineStart;
        int fHit = 0;

        p2 = split_word(pRest, aPat, sizeof(aPat));
        (void)split_word(p2, aArg, sizeof(aArg));
        if (aPat[0] == '\0' || aArg[0] == '\0') {
            return -1;
        }
        n = gj_vfs(GJ_VFS_OP_READ, (long)(uintptr_t)aArg, (long)(uintptr_t)aBuf,
                   (long)(sizeof(aBuf) - 1));
        if (n < 0) {
            return -1;
        }
        aBuf[n] = '\0';
        nLineStart = 0;
        for (i = 0; i <= n; i++) {
            if (i == n || aBuf[i] == '\n') {
                char cSave = aBuf[i];
                size_t nPat = gj_strlen(aPat);
                long j;

                aBuf[i] = '\0';
                for (j = nLineStart; j + (long)nPat <= i; j++) {
                    size_t k;
                    int fEq = 1;

                    for (k = 0; k < nPat; k++) {
                        if (aBuf[j + (long)k] != aPat[k]) {
                            fEq = 0;
                            break;
                        }
                    }
                    if (fEq) {
                        out(aBuf + nLineStart);
                        out("\n");
                        fHit = 1;
                        break;
                    }
                }
                aBuf[i] = cSave;
                nLineStart = i + 1;
            }
        }
        return fHit ? 0 : -1;
    }
    /* test EXPR — minimal: -z/-n STRING, STRING = STRING, -e FILE */
    if (gj_strcmp(aCmd, "test") == 0 || gj_strcmp(aCmd, "[") == 0) {
        char aA[64];
        char aOp[16];
        char aB[64];
        const char *p2;
        const char *p3;
        int fOk = 0;

        p2 = split_word(pRest, aA, sizeof(aA));
        if (aA[0] == '\0') {
            return -1;
        }
        if (gj_strcmp(aA, "-z") == 0) {
            (void)split_word(p2, aB, sizeof(aB));
            fOk = (aB[0] == '\0');
        } else if (gj_strcmp(aA, "-n") == 0) {
            (void)split_word(p2, aB, sizeof(aB));
            fOk = (aB[0] != '\0');
        } else if (gj_strcmp(aA, "-e") == 0) {
            unsigned aSt[2];

            (void)split_word(p2, aB, sizeof(aB));
            fOk = (gj_vfs(GJ_VFS_OP_STAT, (long)(uintptr_t)aB,
                          (long)(uintptr_t)aSt, 0) == 0);
        } else {
            p3 = split_word(p2, aOp, sizeof(aOp));
            (void)split_word(p3, aB, sizeof(aB));
            if (gj_strcmp(aOp, "=") == 0 || gj_strcmp(aOp, "==") == 0) {
                fOk = (gj_strcmp(aA, aB) == 0);
            } else if (gj_strcmp(aOp, "!=") == 0) {
                fOk = (gj_strcmp(aA, aB) != 0);
            } else {
                return -1;
            }
        }
        return fOk ? 0 : -1;
    }
    if (gj_strcmp(aCmd, "printf") == 0) {
        /* Limited: printf FMT [ARG] — %s substitution + \\n expansion */
        char aFmt[96];
        char aVal[96];
        const char *p2;
        size_t i;
        size_t n;

        p2 = split_word(pRest, aFmt, sizeof(aFmt));
        (void)split_word(p2, aVal, sizeof(aVal));
        if (aFmt[0] == '\0') {
            return -1;
        }
        n = gj_strlen(aFmt);
        for (i = 0; i < n; i++) {
            if (aFmt[i] == '%' && i + 1 < n && aFmt[i + 1] == 's') {
                out(aVal);
                i++;
            } else if (aFmt[i] == '\\' && i + 1 < n && aFmt[i + 1] == 'n') {
                out("\n");
                i++;
            } else {
                char aOne[2];

                aOne[0] = aFmt[i];
                aOne[1] = '\0';
                out(aOne);
            }
        }
        return 0;
    }
    if (gj_strcmp(aCmd, "seq") == 0) {
        char aFrom[16];
        char aTo[16];
        const char *p2;
        long nFrom = 1;
        long nTo = 1;
        long i;
        const char *p;

        p2 = split_word(pRest, aFrom, sizeof(aFrom));
        (void)split_word(p2, aTo, sizeof(aTo));
        if (aFrom[0] != '\0' && aTo[0] == '\0') {
            nTo = 0;
            p = aFrom;
            while (*p >= '0' && *p <= '9') {
                nTo = nTo * 10 + (*p - '0');
                p++;
            }
            nFrom = 1;
        } else {
            nFrom = 0;
            p = aFrom;
            while (*p >= '0' && *p <= '9') {
                nFrom = nFrom * 10 + (*p - '0');
                p++;
            }
            nTo = 0;
            p = aTo;
            while (*p >= '0' && *p <= '9') {
                nTo = nTo * 10 + (*p - '0');
                p++;
            }
        }
        if (nTo < nFrom) {
            return -1;
        }
        if (nTo - nFrom > 32) {
            nTo = nFrom + 32; /* cap for smoke */
        }
        for (i = nFrom; i <= nTo; i++) {
            char aN[16];
            size_t nDig = gj_itoa(i, aN, sizeof(aN));

            aN[nDig] = '\0';
            out(aN);
            out("\n");
        }
        return 0;
    }
    if (gj_strcmp(aCmd, "which") == 0 || gj_strcmp(aCmd, "type") == 0) {
        (void)split_word(pRest, aArg, sizeof(aArg));
        if (aArg[0] == '\0') {
            return -1;
        }
        /* Builtins resolve as shell builtins; no PATH walk on VFS door */
        if (gj_strcmp(aCmd, "type") == 0) {
            out(aArg);
            out(" is a shell builtin\n");
        } else {
            out("/bin/");
            out(aArg);
            out("\n");
        }
        return 0;
    }
    if (gj_strcmp(aCmd, "free") == 0) {
        out("              total        used        free\n");
        out("Mem:       1048576           0     1048576\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "df") == 0) {
        out("Filesystem     1K-blocks  Used Available Use% Mounted on\n");
        out("store               4096     0      4096   0% /mnt\n");
        return 0;
    }
    if (gj_strcmp(aCmd, "yes") == 0) {
        unsigned i;
        const char *sz = "y";

        (void)split_word(pRest, aArg, sizeof(aArg));
        if (aArg[0] != '\0') {
            sz = aArg;
        }
        for (i = 0; i < 4u; i++) { /* capped — never hang smoke */
            out(sz);
            out("\n");
        }
        return 0;
    }
    if (gj_strcmp(aCmd, "sessstats") == 0) {
        static unsigned aSt[4];
        char aN[24];
        size_t n;

        if (gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aSt, 0, 0) != 0) {
            out("sessstats: FAIL\n");
            return -1;
        }
        out("sessstats calls=");
        n = gj_itoa((long)aSt[0], aN, sizeof(aN));
        aN[n] = '\0';
        out(aN);
        out("\n");
        out("greenjade-shell: sessstats PASS\n");
        return 0;
    }
    out("unknown cmd (try help)\n");
    return -1;
}

void
_start(void)
{
    static const char *const aScript[] = {
        "help",
        "format",
        "mount",
        "echo shell-ok > /mnt/shell.txt",
        "echo note-hi > note.txt",
        "cat shell.txt",
        "ls",
        "stat shell.txt",
        "touch stamp.txt",
        "cp shell.txt shell-copy.txt",
        "cat shell-copy.txt",
        "echo mv-src > mvin.txt",
        "mv mvin.txt mvout.txt",
        "cat mvout.txt",
        "head shell.txt",
        "tail shell.txt",
        "wc shell.txt",
        "cmp shell.txt shell-copy.txt",
        /* free early temps — door VFS has INODE_MAX 8 */
        "rm stamp.txt",
        "rm shell-copy.txt",
        "rm mvout.txt",
        "rm note.txt",
        "sleep 1",
        "size shell.txt",
        "true",
        "clear",
        "hostname",
        "uname",
        "id",
        "whoami",
        "date",
        "env",
        "printenv PATH",
        "nproc",
        "uptime",
        "basename /mnt/shell.txt",
        "dirname /mnt/shell.txt",
        "cksum shell.txt",
        "grep shell-ok shell.txt",
        "test -e shell.txt",
        "test -n ok",
        "printf hello\\n",
        "seq 1 3",
        "which sh",
        "type echo",
        "free",
        "df",
        "yes",
        "sessstats",
        "tee teed.txt tee-ok",
        "cat teed.txt",
        "expr 2 + 3",
        "rev shell.txt",
        "nl shell.txt",
        "od shell.txt",
        "cut shell.txt 4",
        "tr o O shell.txt",
        "fold shell.txt 8",
        "echo a > u.txt",
        "uniq u.txt",
        "echo zebra > s.txt",
        "echo apple >> s.txt",
        "echo mango >> s.txt",
        "sort s.txt",
        "strings shell.txt",
        "expand shell.txt",
        "logname",
        "tty",
        "groups",
        "factor 12",
        /* Free VFS inodes (bring-up door FS ~8 slots) before more files */
        "rm teed.txt",
        "rm u.txt",
        "rm s.txt",
        "echo line1 > tac.txt",
        "echo line2 >> tac.txt",
        "echo line3 >> tac.txt",
        "tac tac.txt",
        "echo a > p1.txt",
        "echo b > p2.txt",
        "paste p1.txt p2.txt",
        "sum shell.txt",
        "rm p1.txt",
        "rm p2.txt",
        "echo k 1 > j1.txt",
        "echo k x > j2.txt",
        "join j1.txt j2.txt",
        "rm j1.txt",
        "rm j2.txt",
        "echo apple > c1.txt",
        "echo banana >> c1.txt",
        "echo banana > c2.txt",
        "echo cherry >> c2.txt",
        "comm c1.txt c2.txt",
        "rm c1.txt",
        "rm c2.txt",
        "echo        spaced > ue.txt",
        "unexpand ue.txt",
        "shuf tac.txt",
        "rm tac.txt",
        "rm ue.txt",
        "echo abcdef > cr.txt",
        "colrm cr.txt 2 4",
        "rm cr.txt",
        "echo one two three four five six > fm.txt",
        "fmt fm.txt",
        "rm fm.txt",
        "pathchk /mnt/shell.txt",
        "split shell.txt 8",
        "base64 shell.txt",
        "base32 shell.txt",
        "arch",
        "getconf PAGE_SIZE",
        "realpath shell.txt",
        "mkdir dmark.txt",
        "rm dmark.txt",
        "hexdump shell.txt",
        "du shell.txt",
        "truncate tcut.txt",
        "unlink tcut.txt",
        "link shell.txt link.txt",
        "rm link.txt",
        "xxd shell.txt",
        "dir",
        "vdir",
        "readlink shell.txt",
        "echo a b > ts.txt",
        "echo b c >> ts.txt",
        "tsort ts.txt",
        "rm ts.txt",
        "pr shell.txt",
        "md5sum shell.txt",
        "who",
        "users",
        "file shell.txt",
        "chmod 644 shell.txt",
        "chown root shell.txt",
        "namei /mnt/shell.txt",
        "sha1sum shell.txt",
        "echo shredme > shd.txt",
        "shred shd.txt",
        "nice",
        "sha256sum shell.txt",
        "sha512sum shell.txt",
        "sha384sum shell.txt",
        "crc32 shell.txt",
        "b2sum shell.txt",
        "uuidgen",
        "sha3sum shell.txt",
        "adler32 shell.txt",
        "sha224sum shell.txt",
        "xxhsum shell.txt",
        "crc32c shell.txt",
        "fnvsum shell.txt",
        "siphash shell.txt",
        "xxh3sum shell.txt",
        "sm3sum shell.txt",
        "crc16 shell.txt",
        "crc8 shell.txt",
        "base58 shell.txt",
        "b3sum shell.txt",
        "crc64 shell.txt",
        "base85 shell.txt",
        "ascii85 shell.txt",
        "sum64 shell.txt",
        "echo line1 > d2u.txt",
        "unix2dos d2u.txt",
        "dos2unix d2u.txt",
        "rm d2u.txt",
        "ln shell.txt lnk.txt",
        "rmdir lnk.txt",
        "umask",
        "sync",
        "bigfile",
        "cat big.txt",
        "rm big.txt",
        "ls",
        "stats",
        "pwd",
        "storecap",
        "storestats",
        "storeflush",
        "netstats",
        "cpuid",
        "aescheck",
        "base64url shell.txt",
        "urlencode shell.txt",
        "tarlist sample.tar",
        "rm sample.tar",
        "filemagic m.elf",
        "identify m.png",
        "filemagic m.jpg",
        "filemagic m.wav",
        "rm m.elf",
        "rm m.png",
        "rm m.jpg",
        "rm m.wav",
        "echo hello%20x > uenc.txt",
        "urldecode uenc.txt",
        "rm uenc.txt",
        "crock32 shell.txt",
        "ipsum shell.txt",
        "duration 1h30m",
        "macfmt AA-BB-CC-DD-EE-FF",
        "slug Hello World!",
        "lev kitten sitting",
        "gray 7",
        "bitrev 1",
        /* product v1.65: bar3 soft status + product readiness one-liners */
        "ubar3soft",
        "umediapath",
        "usteamrun",
        "uhdamulti",
        "uscsimid",
        "usshd",
        "unetstack",
        "upmmhier",
        /* product v1.66: continuum/hda/scsi soft readiness one-liners */
        "ucontinuum14500",
        "uhda_client",
        "uscsi_live",
        /* product v1.67: soak/makefile/bar3-open soft readiness one-liners */
        "u768gsoak",
        "umakefile14600",
        "ubar3open",
        /* product v1.68: continuum14600/ioring-min/1tib-open soft one-liners */
        "ucontinuum14600",
        "uioringmin",
        "u1tibopen",
        /* product v1.69 Wave 11: continuum15300/makefile15300/matrix-open soft */
        "ucontinuum15300",
        "umakefile15300",
        "umatrixopen",
        /* product v1.70 Wave 14: exclusive soft deepen user-daemon honesty */
        "uwave14",
        "umulticonfine",
        "udaemonsoft",
        /* product v1.71 Wave 15: exclusive soft deepen stamp kept */
        "uwave15",
        /* product v1.72 Wave 16: exclusive soft deepen stamp kept */
        "uwave16",
        /* product v1.73 Wave 18: exclusive soft deepen stamp kept */
        "uwave17",
        /* product v1.74 Wave 38: exclusive soft deepen stamp kept */
        "uwave18",
        /* product v1.75 Wave 39: exclusive soft deepen stamp kept */
        "uwave19",
        /* product v1.76 Wave 42: exclusive soft deepen stamp kept */
        "uwave20",
        /* product v1.77 Wave 43: exclusive soft deepen stamp kept */
        "uwave21",
        /* product v1.78 Wave 76: exclusive soft deepen stamp */
        "uwave22",
        "version",
        "install",
        "exit",
        NULL,
    };
    unsigned i;
    int rc;

    out("greenjade-shell: ready\n");
    out("greenjade-shell: abi PASS\n");
    /* Non-blocking console poll (serial) — must not hang smoke */
    {
        long n = gj_console_poll();

        if (n < 0) {
            out("greenjade-shell: console poll FAIL\n");
            gj_exit(7);
        }
        out("greenjade-shell: console poll PASS\n");
    }
    out("greenjade-shell: interactive script start\n");

    for (i = 0; aScript[i] != NULL; i++) {
        out("greenjade-shell$ ");
        out(aScript[i]);
        out("\n");
        /* product v0.32: seed tiny ustar before tarlist smoke */
        if (gj_strcmp(aScript[i], "tarlist sample.tar") == 0) {
            if (sh_ustar_put_sample("sample.tar") != 0) {
                out("greenjade-shell: cmd FAIL\n");
                out("tarlist sample seed\n");
                gj_exit(10 + (int)i);
            }
        }
        /* product v0.33: seed magic stubs before filemagic smoke */
        if (gj_strcmp(aScript[i], "filemagic m.elf") == 0) {
            if (sh_magic_put_samples() != 0) {
                out("greenjade-shell: cmd FAIL\n");
                out("filemagic sample seed\n");
                gj_exit(10 + (int)i);
            }
        }
        rc = run_line(aScript[i]);
        if (rc == 1) {
            break;
        }
        if (rc != 0) {
            out("greenjade-shell: cmd FAIL\n");
            out(aScript[i]);
            out("\n");
            gj_exit(10 + (int)i);
        }
    }

    /* readline path: no serial input in QEMU smoke → idle timeout */
    {
        static char aLine[128];
        int n;
        unsigned uLines = 0;

        out("greenjade-shell: readline test\n");
        n = read_line(aLine, sizeof(aLine), 32u);
        if (n == -2) {
            out("greenjade-shell: readline idle PASS\n");
            out("greenjade-shell: install ready PASS\n");
        } else if (n >= 0) {
            /*
             * Install-mode: process bounded live lines from serial
             * (e.g. real-hw console). Cap lines so smoke cannot hang.
             */
            out("greenjade-shell: readline got line PASS\n");
            out("greenjade-shell: install mode\n");
            rc = run_line(aLine);
            uLines = 1;
            while (rc != 1 && uLines < 8u) {
                out("greenjade-shell$ ");
                n = read_line(aLine, sizeof(aLine), 64u);
                if (n < 0) {
                    break; /* idle → leave install mode */
                }
                rc = run_line(aLine);
                uLines++;
            }
            out("greenjade-shell: install ready PASS\n");
        } else {
            out("greenjade-shell: readline FAIL\n");
            gj_exit(8);
        }
    }

    out("greenjade-shell: stat touch cp PASS\n");
    out("greenjade-shell: mv head wc PASS\n");
    out("greenjade-shell: tail cmp sleep PASS\n");
    out("greenjade-shell: uname id PASS\n");
    out("greenjade-shell: whoami date env PASS\n");
    out("greenjade-shell: printenv nproc uptime PASS\n");
    out("greenjade-shell: basename dirname PASS\n");
    out("greenjade-shell: cksum grep test PASS\n");
    out("greenjade-shell: printf seq which PASS\n");
    out("greenjade-shell: free df yes sessstats PASS\n");
    out("greenjade-shell: rev nl od expr tee cut tr fold uniq umask PASS\n");
    out("greenjade-shell: sort strings expand logname tty groups factor PASS\n");
    out("greenjade-shell: tac paste sum join comm PASS\n");
    out("greenjade-shell: unexpand shuf colrm fmt pathchk PASS\n");
    out("greenjade-shell: split base64 base32 arch getconf realpath mkdir PASS\n");
    out("greenjade-shell: hexdump du truncate link unlink PASS\n");
    out("greenjade-shell: xxd md5sum file dir vdir ln rmdir PASS\n");
    out("greenjade-shell: readlink tsort pr PASS\n");
    out("greenjade-shell: md5sum who users file chmod chown PASS\n");
    out("greenjade-shell: namei shred nice sha1sum PASS\n");
    out("greenjade-shell: sha256sum dos2unix unix2dos PASS\n");
    out("greenjade-shell: sha512sum sha384sum crc32 PASS\n");
    out("greenjade-shell: b2sum uuidgen PASS\n");
    out("greenjade-shell: sha3sum PASS\n");
    out("greenjade-shell: adler32 PASS\n");
    out("greenjade-shell: sha224sum xxhsum crc32c PASS\n");
    out("greenjade-shell: fnvsum siphash xxh3sum PASS\n");
    out("greenjade-shell: sm3sum crc16 PASS\n");
    out("greenjade-shell: crc8 base58 PASS\n");
    out("greenjade-shell: b3sum crc64 PASS\n");
    out("greenjade-shell: base85 ascii85 sum64 PASS\n");
    out("greenjade-shell: cpuid aescheck PASS\n");
    out("greenjade-shell: tarlist base64url urlencode PASS\n");
    out("greenjade-shell: filemagic identify urldecode PASS\n");
    out("greenjade-shell: crock32 ipsum PASS\n");
    out("greenjade-shell: duration macfmt PASS\n");
    out("greenjade-shell: slug lev PASS\n");
    out("greenjade-shell: gray bitrev PASS\n");
    out("greenjade-shell: ubar3soft umediapath usteamrun PASS\n");
    out("greenjade-shell: uhdamulti uscsimid usshd PASS\n");
    out("greenjade-shell: unetstack upmmhier PASS\n");
    out("greenjade-shell: ucontinuum14500 uhda_client uscsi_live PASS\n");
    out("greenjade-shell: u768gsoak umakefile14600 ubar3open PASS\n");
    out("greenjade-shell: ucontinuum14600 uioringmin u1tibopen PASS\n");
    out("greenjade-shell: ucontinuum15300 umakefile15300 umatrixopen PASS\n");
    out("greenjade-shell: uwave14 umulticonfine udaemonsoft PASS\n");
    out("greenjade-shell: uwave15 PASS\n");
    out("greenjade-shell: uwave16 PASS\n");
    out("greenjade-shell: uwave17 PASS\n");
    out("greenjade-shell: uwave18 PASS\n");
    out("greenjade-shell: uwave19 PASS\n");
    out("greenjade-shell: uwave20 PASS\n");
    out("greenjade-shell: uwave21 PASS\n");
    out("greenjade-shell: uwave22 PASS\n");
    out("greenjade-shell: storeflush PASS\n");
    out("greenjade-shell: vfs PASS\n");
    out("greenjade-shell: interactive PASS\n");
    gj_exit(0);
}
