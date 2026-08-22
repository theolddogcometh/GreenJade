# sshd_gj — **abandoned** (not product, not linked)

Moved out of `user/sshd/`. Not on `C_SRCS` / `S_SRCS`. Do not re-link.
Product SSH is OpenSSH-portable on the Linux ABI (`user/openssh/`).

**Was** a NATIVE lab probe. Product SSH is **OpenSSH-portable 10.5p1** +
**OpenSSL 3.5.7 LTS libcrypto** (Apache-2.0) on the **Linux ABI**
([`user/openssh/`](../openssh/README.md)). This tree is a **NATIVE**
`GJ_SYS_NET` probe (`u32Personality = 0`). **Freeze it** — no RFC 4254
deepen, no residual-lean deepen.

Dual DoD **B** is **OPEN** until host **interactive SSH login** via that
Linux-ABI `sshd`. Banner / PK_OK / **SUCCESS** / `true` ≠ login.

Lab wire for the probe (not Dual DoD B close):

```text
rtl8168_udx → kernel net_tcp → sshd.elf :22  (lab 10.200.125.50)
```

**Soft≠product.** **G-AC-1.** GOP dash isolate stays.

## License

| Item | Policy |
|------|--------|
| Lab probe (`sshd_gj.c`, `ssh_crypto.c`, `sshd.c`) | **MIT OR Apache-2.0** (SPDX dual) |
| Probe crypto | SHA-256/512 + X25519/ChaCha/Poly from RFCs; Ed25519 is SUPERCOP/OpenSSH `ed25519.c` (Public Domain) |
| **OpenSSH** | **BSD** (OpenBSD / portable) — **not GPL**. Same third-party class as `dash`. Allowed per [PROVENANCE.md](../../third_party/PROVENANCE.md). |
| Product SSH | **OpenSSH-portable 10.5p1** + **OpenSSL 3.5.7 LTS libcrypto** (Apache-2.0) in `third_party/`. LINUX-personality. Not more protocol in `sshd_gj.c`. |
| **Not** in tree | GPL / LGPL paste. OpenSSH is not that. |

See repository root `LICENSE` and `third_party/PROVENANCE.md`.

## Layout

| Path | Role |
|------|------|
| `src/sshd_gj.c` | Frozen NATIVE lab probe (TCP :22). Not product SSH. |
| `src/ssh_crypto.c` | Probe SHA-256, X25519, HMAC-SHA256, ChaCha20, Poly1305, host identity |
| `src/sshd.c` | Host POSIX listen + soft banner/KEXINIT smoke |

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/sshd.elf` | `make sshd-gj` | Frozen NATIVE probe (net door :22). Not the product daemon. |
| `build/sshd` | `make sshd` | Host POSIX smoke (default `127.0.0.1:2222`) |

Product daemon work is [`user/openssh/`](../openssh/README.md) (`make openssh-gj`).
It does not replace this embed.

## History (proven-class, not next work)

UDX NIC **L3 ARP + ping** is proven. The frozen probe then reached these
**proven-class** host rows. None of them is Dual DoD **B** close.

| Stamp | Proven-class (≠ interactive login) |
|-------|--------------------------------------|
| **0.1.169** | Host accepted `KEX_ECDH_REPLY` |
| **0.1.172** | NEWKEYS both directions |
| **0.1.173** | AEAD `packet_length%8` (fly) |
| **0.1.177** | Continue-list + **`Server accepts key`** / `USERAUTH_PK_OK` |
| **0.1.178** | **`USERAUTH_SUCCESS`** |
| **0.1.179** | `OPEN_CONFIRM` + `CHANNEL_SUCCESS` dropped (`true` hang) |
| **0.1.180** | exec-stub flush + stay-open (fly) |
| **0.1.183** | Host **`Sending command: true` PASS**; exec 124 |
| **0.1.184** | exec TX drain after each CHANNEL_SUCCESS / exit-status / EOF / CLOSE (packed, not host-probed) |

Channel / SUCCESS / `true` ≠ login. Do not deepen RFC 4254 in `sshd_gj.c`
to chase a session.

## Host path (`sshd.c`)

POSIX listen + soft SSH-2.0 identification + soft `SSH_MSG_KEXINIT` only.
Not product crypto (product crypto is OpenSSL 3.5.7 libcrypto). Probe
crypto for the NATIVE ELF is `sshd_gj.c` + `ssh_crypto.c`.

```
make sshd && ./build/sshd --once     # self-connect CI smoke (default)
./build/sshd --listen                # accept external clients (nc/ssh)
./build/sshd --port 2223 --once      # or env GJ_SSHD_PORT
printf 'SSH-2.0-GreenJade_test\r\n' | nc 127.0.0.1 2222   # with --listen
```

Host smoke markers (success, greppable):

```
sshd: authorized_keys soft PASS
sshd: listening 127.0.0.1:2222 (product host smoke)
sshd: client saw server banner PASS
sshd: peer banner PASS
sshd: kexinit soft PASS
sshd: client kexinit soft PASS
sshd: host path PASS
```

| Marker | Meaning |
|--------|---------|
| `authorized_keys soft PASS` | Soft parse of ed25519/rsa/ecdsa line shapes |
| `client saw server banner PASS` | Loopback client read `SSH-2.0-GreenJade_sshd` |
| `peer banner PASS` | Server accepted client identification (`SSH-…`) |
| `kexinit soft PASS` | Minimal binary `SSH_MSG_KEXINIT` sent |
| `client kexinit soft PASS` | Client drained soft KEXINIT (msg type 20) |
| `host path PASS` | Full host smoke green (exit 0) |

Failure lines use the same prefix with **FAIL**. Default mode is `--once`
(self-connect); use `--listen` for long-running external probing.

## Frozen probe (`sshd_gj.c` + `ssh_crypto.c`)

Implemented lab path (history of the freeze, not a deepen list):

1. **TCP** :22 via `net_tcp` (virtio-net IPv4 + loopback)
2. Banners (RFC 4253) — id `SSH-2.0-GreenJade_sshd`
3. **KEX** `curve25519-sha256` — X25519 + SHA-256 shared secret
4. Host identity Edwards `ssh-ed25519`; RFC 8032 sign of H (SUPERCOP PD)
5. `SSH_MSG_KEX_ECDH_REPLY` on wire (K_S ‖ Q_S ‖ sig) — host **0.1.169**
6. **NEWKEYS** both directions (host **0.1.172**); fly **0.1.173** AEAD pad
7. Session channel + shell MOTD (loopback self-smoke only)
8. Key derivation (RFC 4253 §7.2) → `chacha20-poly1305@openssh.com`
9. Encrypted SERVICE_ACCEPT after NEWKEYS
10. USERAUTH PK_OK / SUCCESS on eth (see History); channel/`true` after that
11. Daemon parks with listen held

QEMU hostfwd `2222→22` targets guest TCP :22. Lab NIC
(`10.200.125.50:22`) uses the eth accept/session path after park. Lab-host
SSH remains for serial/bench (operator path, not this probe). Soft≠product.

### Probe markers (`sshd_gj.c`)

Success path emits greppable **PASS** lines (order fixed). These are
inventory of the frozen probe, not Dual DoD B close and not a deepen
backlog.

```
sshd-gj: start TCP :22 full path
sshd-gj: TCP listening :22 listen=:22 product_net_owns_wire=1 product_path=UDX soft_listen_ne_host_banner=1
sshd-gj: peer banner PASS
sshd-gj: poly1305 selfcheck PASS
sshd-gj: hostkey sign PASS
sshd-gj: ECDH_REPLY PASS
sshd-gj: shared secret match PASS
sshd-gj: x25519 KEX shared PASS
sshd-gj: KEX PASS (curve25519-sha256 + hostkey)
sshd-gj: NEWKEYS exchange PASS
sshd-gj: service soft PASS
sshd-gj: channel+shell MOTD PASS
sshd-gj: key derivation PASS (ChaCha20+HMAC)
sshd-gj: poly1305 aead soft PASS
sshd-gj: encrypted channel PASS
sshd-gj: encrypted channel recv PASS
sshd-gj: post-KEX session PASS
sshd-gj: live path PASS (TCP+KEX+NEWKEYS+shell)
sshd-gj: daemon park (TCP :22 listen held)
sshd-gj: eth accept loop ready (yield+poll Dual DoD B)
```

The probe still prints residual-lean / door-first honesty lines
(`sshd-gj: soft residual lean …`, `soft residual lean deepen …`). Those
are frozen inventory. **Do not** treat them as next work.

External eth accept/session (after park):

```
sshd-gj: eth accept session
sshd-gj: eth banner PASS
sshd-gj: eth peer early close soft PASS   # host nc: read banner → close
sshd-gj: eth KEX start PASS                 # when peer stays connected
sshd-gj: eth session path PASS
sshd-gj: eth accept session PASS
sshd-gj: eth accept session done
```

Early host `nc` (connect → see `SSH-2.0-GreenJade_sshd` → close) is a
**session PASS** after server banner TX; no client banner/KEX required.
Banner ≠ login. Soft≠product.

| Marker | Meaning |
|--------|---------|
| `peer banner PASS` | RFC 4253 identification string exchange |
| `poly1305 selfcheck PASS` | RFC 8439 §2.5.2 Poly1305 test vector |
| `hostkey sign PASS` | Probe hostkey sign + verify of `H` |
| `ECDH_REPLY PASS` | SSH_MSG_KEX_ECDH_REPLY on wire + soft parse |
| `shared secret match PASS` | Server/client X25519 shared secrets equal |
| `x25519 KEX shared PASS` | X25519 shared secret from ECDH |
| `KEX PASS (…)` | Aggregate: hostkey + curve25519-sha256 KEX OK |
| `NEWKEYS exchange PASS` | SSH_MSG_NEWKEYS both directions + peer drain |
| `service soft PASS` | Soft SERVICE_REQUEST/ACCEPT (`ssh-userauth`) |
| `channel+shell MOTD PASS` | Session channel open + shell + MOTD |
| `key derivation PASS` | RFC 4253 §7.2 keys armed (ChaCha20 + HMAC) |
| `poly1305 aead soft PASS` | Poly1305 over session material (soft AEAD leg) |
| `encrypted channel PASS` | Encrypted CHANNEL_DATA after NEWKEYS |
| `encrypted channel recv PASS` | Client MAC verify + ChaCha20 decrypt of channel data |
| `post-KEX session PASS` | Full `do_kex_and_session` success |
| `live path PASS (…)` | Banner + session both green; daemon parks |
| `eth accept loop ready` | Parked; yield-first POLL+ACCEPT |
| `eth accept session` | External TCP peer accepted on :22 |
| `eth banner PASS` | Id `SSH-2.0-GreenJade_sshd` TX (proven-class; **not** Dual DoD B close) |
| `eth USERAUTH_PK_OK PASS` | RFC 4252 type 60 after publickey query (**proven** on 0.1.177; **not** login) |
| `eth USERAUTH_SUCCESS PASS` | RFC 4252 type 52 after signed type 50 (**proven** on 0.1.178; **not** login) |
| `eth peer early close soft PASS` | Peer FIN/EOF after banner (typical `nc`); session PASS |
| `eth KEX start PASS` | Server KEXINIT on wire |
| `eth session path PASS` | Soft session path green after banner/KEX (or early nc close) |
| `eth accept session PASS` | Accept + session handler returned OK |

**SUCCESS ≠ login.** `sshd-gj: eth USERAUTH_SUCCESS PASS` is proven-class
history on **0.1.178**, not Dual DoD B close.

Host `nc` on lab **10.200.125.50:22** seeing `SSH-2.0-GreenJade_sshd` is
**banner proven-class**. Dual DoD B **OPEN** until **interactive login**.
ICMP ≠ this DoD.

Failure lines use the same prefix with **FAIL** (e.g. `hostkey sign FAIL`,
`post-KEX session FAIL`, `live path FAIL`).

Kernel spawn markers (not from this directory):

```
sshd: live elf thr=…
sshd: live spawn PASS
```

Soft inventory (`scripts/gj-product-summary.sh`) greps `sshd: live spawn PASS`.
Hard/quick keys may also look for freestanding `live path PASS`.

## Config (rootfs)

```
/etc/ssh/sshd_config
/etc/ssh/authorized_keys   # hwtest ed25519 pubkey when staged
/usr/sbin/sshd             # lab probe ELF when staged (also /sbin/sshd)
```

That path is the frozen NATIVE probe, not OpenSSH-portable.

## Build

```
make sshd-gj    # → build/user/sshd.elf  (frozen NATIVE probe)
make sshd       # → build/sshd (host POSIX smoke)
make openssh-gj # product SSH glue; see user/openssh/README.md
```

## Kernel live spawn

Embedded via `kernel/proc/sshd_embed.S` and spawned as the **lab** `:22`
NATIVE probe in `kernel/main.c` (outside this directory). Mid-hop is kernel
`net_tcp`, not `netstackd` (that ELF is leftover smoke on `:7777`).
Boot log shows the kernel markers above; probe markers appear once the
ELF runs its TCP :22 self-smoke. This embed is **not** the product
daemon.

See also: [OpenSSH-portable](../openssh/README.md) (product SSH) ·
[ABI-first pivot](../../docs/ABI_FIRST_PIVOT.md).

---

**Project:** GreenJade · Soft≠product · Dual DoD A **OPEN** until host USB path · Dual DoD B **OPEN** until interactive SSH login. [root README](../../README.md). Support: [Patreon — TheOldDog](https://www.patreon.com/cw/TheOldDog).
