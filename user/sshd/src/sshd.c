/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host-linked product sshd smoke (POSIX sockets).
 * Dual MIT OR Apache-2.0. Clean-room pure C — not an OpenSSH/GPL paste.
 *
 * Listens on 127.0.0.1:2222 by default (override: --port or GJ_SSHD_PORT).
 * Soft path only: SSH-2.0 identification + minimal SSH_MSG_KEXINIT.
 * Full crypto / TCP+KEX+NEWKEYS lives in sshd_gj.c + ssh_crypto.c.
 *
 *   make sshd && ./build/sshd --once
 *   ./build/sshd --listen   # then: printf 'SSH-2.0-GreenJade_test\r\n' | nc 127.0.0.1 2222
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static const char g_szBanner[] = "SSH-2.0-GreenJade_sshd\r\n";
static const char g_szClientBanner[] = "SSH-2.0-GreenJade_smoke\r\n";

static int
banner_is_ssh(const char *szBuf, ssize_t cb)
{
	return cb >= 4 && szBuf[0] == 'S' && szBuf[1] == 'S' &&
	       szBuf[2] == 'H' && szBuf[3] == '-';
}

static int
ak_line_ok(const char *szLine)
{
	if (szLine == NULL || szLine[0] == '\0' || szLine[0] == '#') {
		return 0;
	}
	return strncmp(szLine, "ssh-ed25519 ", 12) == 0 ||
	       strncmp(szLine, "ssh-rsa ", 8) == 0 ||
	       strncmp(szLine, "ecdsa-sha2-nistp256 ", 20) == 0;
}

static ssize_t
send_kexinit_soft(int hFd)
{
	unsigned char aPkt[64];
	unsigned iByte;
	unsigned nPad = 4;
	unsigned cbPayload = 1 + 16 + 4;
	unsigned cbPacket;

	while ((1 + cbPayload + nPad) % 8 != 0) {
		nPad++;
	}
	cbPacket = 1 + cbPayload + nPad;
	aPkt[0] = (unsigned char)((cbPacket >> 24) & 0xff);
	aPkt[1] = (unsigned char)((cbPacket >> 16) & 0xff);
	aPkt[2] = (unsigned char)((cbPacket >> 8) & 0xff);
	aPkt[3] = (unsigned char)(cbPacket & 0xff);
	aPkt[4] = (unsigned char)nPad;
	aPkt[5] = 20; /* SSH_MSG_KEXINIT (RFC 4253) */
	for (iByte = 0; iByte < 16; iByte++) {
		aPkt[6 + iByte] = (unsigned char)(0xa0 + iByte);
	}
	aPkt[22] = aPkt[23] = aPkt[24] = aPkt[25] = 0;
	for (iByte = 0; iByte < nPad; iByte++) {
		aPkt[26 + iByte] = (unsigned char)(iByte + 1);
	}
	return send(hFd, aPkt, 4 + cbPacket, 0);
}

/*
 * External-client path: server already accepted hCli.
 * Peer must send its identification string (blocking recv).
 */
static int
serve_peer(int hCli, int fVerbose)
{
	char aRbuf[256];
	ssize_t cb;

	if (send(hCli, g_szBanner, strlen(g_szBanner), 0) < 0) {
		perror("send banner");
		return -1;
	}
	cb = recv(hCli, aRbuf, sizeof(aRbuf) - 1, 0);
	if (cb > 0) {
		aRbuf[cb] = '\0';
		if (banner_is_ssh(aRbuf, cb)) {
			if (fVerbose) {
				printf("sshd: peer banner PASS\n");
			}
		} else {
			printf("sshd: peer banner unexpected\n");
			return -1;
		}
	} else {
		printf("sshd: peer banner FAIL (no data)\n");
		return -1;
	}
	if (send_kexinit_soft(hCli) <= 0) {
		printf("sshd: kexinit soft FAIL\n");
		return -1;
	}
	if (fVerbose) {
		printf("sshd: kexinit soft PASS\n");
	}
	return 0;
}

static int
bind_listen(int nPort, int *pHSrv)
{
	int hSrv;
	int nOne = 1;
	struct sockaddr_in sinAddr;

	hSrv = socket(AF_INET, SOCK_STREAM, 0);
	if (hSrv < 0) {
		perror("socket");
		return -1;
	}
	(void)setsockopt(hSrv, SOL_SOCKET, SO_REUSEADDR, &nOne, sizeof(nOne));
	memset(&sinAddr, 0, sizeof(sinAddr));
	sinAddr.sin_family = AF_INET;
	sinAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sinAddr.sin_port = htons((uint16_t)nPort);
	if (bind(hSrv, (struct sockaddr *)&sinAddr, sizeof(sinAddr)) < 0) {
		perror("bind");
		close(hSrv);
		return -1;
	}
	if (listen(hSrv, 4) < 0) {
		perror("listen");
		close(hSrv);
		return -1;
	}
	*pHSrv = hSrv;
	printf("sshd: listening 127.0.0.1:%d (product host smoke)\n", nPort);
	fflush(stdout);
	return 0;
}

/*
 * Self-connect smoke: listen, connect loopback client, accept, exchange.
 * Order is interleaved so neither half blocks forever on the other.
 * Returns 0 on full host path PASS.
 */
static int
run_once_self(int nPort)
{
	int hSrv = -1;
	int hCliSrv = -1;
	int hCliLocal = -1;
	struct sockaddr_in sinAddr;
	struct sockaddr_in sinPeer;
	socklen_t cbPeer;
	char aSrvBuf[256];
	char aCliBuf[256];
	ssize_t cb;

	if (bind_listen(nPort, &hSrv) != 0) {
		return 1;
	}

	memset(&sinAddr, 0, sizeof(sinAddr));
	sinAddr.sin_family = AF_INET;
	sinAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sinAddr.sin_port = htons((uint16_t)nPort);

	hCliLocal = socket(AF_INET, SOCK_STREAM, 0);
	if (hCliLocal < 0) {
		perror("socket client");
		close(hSrv);
		return 1;
	}
	if (connect(hCliLocal, (struct sockaddr *)&sinAddr, sizeof(sinAddr)) <
	    0) {
		perror("connect self");
		close(hCliLocal);
		close(hSrv);
		return 1;
	}

	cbPeer = sizeof(sinPeer);
	hCliSrv = accept(hSrv, (struct sockaddr *)&sinPeer, &cbPeer);
	if (hCliSrv < 0) {
		perror("accept");
		close(hCliLocal);
		close(hSrv);
		return 1;
	}

	/* 1. Server identification */
	if (send(hCliSrv, g_szBanner, strlen(g_szBanner), 0) < 0) {
		perror("send banner");
		goto fail;
	}

	/* 2. Client reads server banner (TCP-buffered; no deadlock) */
	cb = recv(hCliLocal, aCliBuf, sizeof(aCliBuf) - 1, 0);
	if (cb <= 0 || !banner_is_ssh(aCliBuf, cb)) {
		printf("sshd: client saw server banner FAIL\n");
		goto fail;
	}
	aCliBuf[cb] = '\0';
	printf("sshd: client saw server banner PASS\n");

	/* 3. Client identification (same shape as freestanding smoke client) */
	if (send(hCliLocal, g_szClientBanner, strlen(g_szClientBanner), 0) <
	    0) {
		perror("send client banner");
		goto fail;
	}

	/* 4. Server reads peer banner */
	cb = recv(hCliSrv, aSrvBuf, sizeof(aSrvBuf) - 1, 0);
	if (cb <= 0 || !banner_is_ssh(aSrvBuf, cb)) {
		printf("sshd: peer banner FAIL\n");
		goto fail;
	}
	aSrvBuf[cb] = '\0';
	printf("sshd: peer banner PASS\n");

	/* 5. Soft KEXINIT from server */
	if (send_kexinit_soft(hCliSrv) <= 0) {
		printf("sshd: kexinit soft FAIL\n");
		goto fail;
	}
	printf("sshd: kexinit soft PASS\n");

	/* 6. Client drains soft KEXINIT (optional shape check: type byte) */
	cb = recv(hCliLocal, aCliBuf, sizeof(aCliBuf), 0);
	if (cb < 6 || aCliBuf[5] != 20) {
		printf("sshd: client kexinit soft FAIL\n");
		goto fail;
	}
	printf("sshd: client kexinit soft PASS\n");

	close(hCliSrv);
	close(hCliLocal);
	close(hSrv);
	printf("sshd: host path PASS\n");
	return 0;

fail:
	if (hCliSrv >= 0) {
		close(hCliSrv);
	}
	if (hCliLocal >= 0) {
		close(hCliLocal);
	}
	if (hSrv >= 0) {
		close(hSrv);
	}
	return 1;
}

static int
run_listen_loop(int nPort)
{
	int hSrv = -1;
	int hCli = -1;
	struct sockaddr_in sinPeer;
	socklen_t cbPeer;

	if (bind_listen(nPort, &hSrv) != 0) {
		return 1;
	}

	for (;;) {
		cbPeer = sizeof(sinPeer);
		hCli = accept(hSrv, (struct sockaddr *)&sinPeer, &cbPeer);
		if (hCli < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("accept");
			break;
		}
		(void)serve_peer(hCli, 1);
		close(hCli);
	}
	close(hSrv);
	return 1;
}

static void
usage(const char *szArg0)
{
	fprintf(stderr,
		"Usage: %s [--once] [--port N] [--listen]\n"
		"  Host product sshd smoke (POSIX). Default port 2222.\n"
		"  --once    self-connect loopback smoke (default)\n"
		"  --listen  stay listening; accept external clients (nc/ssh)\n"
		"  --port N  bind port (or env GJ_SSHD_PORT)\n",
		szArg0 != NULL ? szArg0 : "sshd");
}

int
main(int argc, char **argv)
{
	const char *szEnv;
	int nPort = 2222;
	int fListen = 0;
	int iArg;
	long nParsed;

	for (iArg = 1; iArg < argc; iArg++) {
		if (strcmp(argv[iArg], "--once") == 0) {
			fListen = 0;
		} else if (strcmp(argv[iArg], "--listen") == 0) {
			fListen = 1;
		} else if (strcmp(argv[iArg], "--port") == 0 &&
			   iArg + 1 < argc) {
			nParsed = strtol(argv[++iArg], NULL, 10);
			if (nParsed < 1 || nParsed > 65535) {
				fprintf(stderr, "sshd: bad --port\n");
				return 1;
			}
			nPort = (int)nParsed;
		} else if (strcmp(argv[iArg], "-h") == 0 ||
			   strcmp(argv[iArg], "--help") == 0) {
			usage(argv[0]);
			return 0;
		} else {
			fprintf(stderr, "sshd: unknown arg %s\n", argv[iArg]);
			usage(argv[0]);
			return 1;
		}
	}
	szEnv = getenv("GJ_SSHD_PORT");
	if (szEnv != NULL && szEnv[0] != '\0') {
		nParsed = strtol(szEnv, NULL, 10);
		if (nParsed < 1 || nParsed > 65535) {
			fprintf(stderr, "sshd: bad GJ_SSHD_PORT\n");
			return 1;
		}
		nPort = (int)nParsed;
	}

	if (!ak_line_ok("ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAItest") ||
	    !ak_line_ok("ssh-rsa AAAAB3NzaC1yc2Etest") ||
	    !ak_line_ok("ecdsa-sha2-nistp256 AAAAEtest") ||
	    ak_line_ok("# comment") || ak_line_ok("") || ak_line_ok(NULL)) {
		fprintf(stderr, "sshd: authorized_keys soft FAIL\n");
		return 1;
	}
	printf("sshd: authorized_keys soft PASS\n");

	/* Grep: sshd: soft inventory (Wave 82 exclusive deepen; host smoke) */
/* Wave 82 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retmantleangle continuum_toward=22400 soft_ne_product=1
 *   greppable: soft retaegisangle exclusive=1 continuum_toward=22400
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

	printf("sshd: soft inventory authorized_keys=1 kexinit=soft wave=70 "
	       "multi_server=0 confine=0\n");
	printf("sshd: soft deepen product_kernel=OPEN wave=70 areas=1 multi_server=0 confine=0 "
	       "bar3=0\n");
	printf("sshd: soft honesty multi_server=0 confine=0 bar3=0 "
	       "exclusive=1 soft=1 product_kernel=OPEN wave=70\n");

	if (fListen) {
		return run_listen_loop(nPort);
	}
	return run_once_self(nPort);
}
