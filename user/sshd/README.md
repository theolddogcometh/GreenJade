# sshd (product)

GreenJade product SSH daemon — freestanding ELF + host smoke.

**Product markers** (greppable): `sshd-gj: … PASS`, `sshd: … PASS`,
`sshd: live spawn PASS` (kernel). See tables below.

## License

| Item | Policy |
|------|--------|
| Product code (`sshd_gj.c`, `ssh_crypto.c`, `sshd.c`) | **MIT OR Apache-2.0** (SPDX dual) |
| Crypto | Clean-room pure C from published RFCs (FIPS 180-4, RFC 7748, 2104, 8439, 4253) |
| **Not** in tree | GPL / LGPL OpenSSH source, or any GPL paste |
| Future optional backends | May vend **BSD** OpenSSH or **MIT** Dropbear under project third-party policy; prefer clean-room adapters first |

See repository root `LICENSE` and `third_party/PROVENANCE.md`.

## Layout

| Path | Role |
|------|------|
| `src/sshd_gj.c` | Freestanding product daemon (TCP :22, full KEX/session smoke) |
| `src/ssh_crypto.c` | SHA-256, X25519, HMAC-SHA256, ChaCha20, Poly1305, host identity |
| `src/sshd.c` | Host POSIX listen + soft banner/KEXINIT smoke |

## Status

| Binary | Build | Role |
|--------|-------|------|
| `build/user/sshd.elf` | `make sshd-gj` | Freestanding product daemon (net door, port **22**, **on by default**) |
| `build/sshd` | `make sshd` | Host POSIX smoke (default `127.0.0.1:2222`) |

### Host path (`sshd.c`)

POSIX listen + soft SSH-2.0 identification + soft `SSH_MSG_KEXINIT` only.
**Not** production crypto (that is `sshd_gj.c` + `ssh_crypto.c`).

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

### Freestanding path (`sshd_gj.c` + `ssh_crypto.c`)

1. **TCP** :22 via `net_tcp` (virtio-net IPv4 + loopback; retransmit/window/MSS)
2. Banners (RFC 4253) over TCP — product id `SSH-2.0-GreenJade_sshd`
3. **KEX** `curve25519-sha256` — X25519 + SHA-256 shared secret
4. **Production host identity** — seeded host key (not demo `0x41…` scalars); HMAC-SHA256 signature of exchange hash `H`
5. **SSH_MSG_KEX_ECDH_REPLY** on wire (K_S ‖ Q_S ‖ sig) + dual shared-secret match
6. **NEWKEYS** both directions (peer drain) + soft **SERVICE_REQUEST/ACCEPT** (`ssh-userauth`)
7. **Session channel** + shell MOTD (client drains cleartext)
8. Key derivation (RFC 4253 §7.2) → ChaCha20 + HMAC-SHA256 integrity
9. **Encrypted CHANNEL_DATA** send + recv (constant-time MAC verify + decrypt)
10. **Poly1305** soft AEAD leg (RFC 8439 §2.5.2 vector + post-keys tag)
11. Daemon parks with listen held (eth ACCEPT on a slow tick)

QEMU hostfwd `2222→22` targets guest TCP :22. Lab-host SSH remains for serial/bench (operator path, not product crypto).

### Freestanding product markers (`sshd_gj.c`)

Success path emits greppable **PASS** lines (order fixed). KEX milestones are
explicit so boot logs distinguish crypto KEX from post-KEX session work:

```
sshd-gj: start TCP :22 full path
sshd-gj: TCP listening :22 (virtio + loopback)
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
```

| Marker | Meaning |
|--------|---------|
| `peer banner PASS` | RFC 4253 identification string exchange |
| `poly1305 selfcheck PASS` | RFC 8439 §2.5.2 Poly1305 test vector |
| `hostkey sign PASS` | Product hostkey sign + verify of `H` |
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

**Primary product line for agents/scripts:**  
`sshd-gj: live path PASS (TCP+KEX+NEWKEYS+shell)`

Failure lines use the same prefix with **FAIL** (e.g. KEX path:
`hostkey sign FAIL`, `post-KEX session FAIL`, `live path FAIL`).

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
/usr/sbin/sshd             # freestanding ELF (also /sbin/sshd)
```

## Build

```
make sshd-gj    # → build/user/sshd.elf
make sshd       # → build/sshd (host POSIX smoke)
```

## Kernel live spawn

Embedded via `kernel/proc/sshd_embed.S` and spawned after `netstackd` in
`kernel/main.c` (outside this directory). Boot log shows the kernel markers
above; freestanding markers appear once the ELF runs its TCP :22 self-smoke.
