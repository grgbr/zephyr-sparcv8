/*
 *  Minimal SSL client, used for memory measurements.
 *  (meant to be used with config-suite-b.h or config-ccm-psk-tls1_2.h)
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#include <zephyr.h>
#if defined(CONFIG_STDOUT_CONSOLE)
#include <stdio.h>
#define PRINT           printf
#else
#include <misc/printk.h>
#define PRINT           printk
#endif

#include <errno.h>

#if !defined(CONFIG_MBEDTLS_CFG_FILE)
#include "mbedtls/config.h"
#else
#include CONFIG_MBEDTLS_CFG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_time_t       time_t
#define MBEDTLS_EXIT_SUCCESS EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE EXIT_FAILURE
#endif

#include <string.h>
#include "tcp.h"
#include "tcp_cfg.h"

#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#include "mbedtls/memory_buffer_alloc.h"
static unsigned char heap[20480];
#endif

/*
 * Hardcoded values for server host and port
 */
#define HOSTNAME "localhost"	/* for cert verification if enabled */

#define GET_REQUEST "GET / HTTP/1.0\r\n\r\n"

const char *pers = "mini_client";

#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
const unsigned char psk[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

const char psk_id[] = "Client_identity";
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
/* This is tests/data_files/test-ca2.crt, a CA using EC secp384r1 */
const unsigned char ca_cert[] = {
	0x30, 0x82, 0x02, 0x52, 0x30, 0x82, 0x01, 0xd7, 0xa0, 0x03, 0x02, 0x01,
	0x02, 0x02, 0x09, 0x00, 0xc1, 0x43, 0xe2, 0x7e, 0x62, 0x43, 0xcc, 0xe8,
	0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02,
	0x30, 0x3e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
	0x02, 0x4e, 0x4c, 0x31, 0x11, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x04, 0x0a,
	0x13, 0x08, 0x50, 0x6f, 0x6c, 0x61, 0x72, 0x53, 0x53, 0x4c, 0x31, 0x1c,
	0x30, 0x1a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x13, 0x50, 0x6f, 0x6c,
	0x61, 0x72, 0x73, 0x73, 0x6c, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x45,
	0x43, 0x20, 0x43, 0x41, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x33, 0x30, 0x39,
	0x32, 0x34, 0x31, 0x35, 0x34, 0x39, 0x34, 0x38, 0x5a, 0x17, 0x0d, 0x32,
	0x33, 0x30, 0x39, 0x32, 0x32, 0x31, 0x35, 0x34, 0x39, 0x34, 0x38, 0x5a,
	0x30, 0x3e, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
	0x02, 0x4e, 0x4c, 0x31, 0x11, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x04, 0x0a,
	0x13, 0x08, 0x50, 0x6f, 0x6c, 0x61, 0x72, 0x53, 0x53, 0x4c, 0x31, 0x1c,
	0x30, 0x1a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x13, 0x50, 0x6f, 0x6c,
	0x61, 0x72, 0x73, 0x73, 0x6c, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x45,
	0x43, 0x20, 0x43, 0x41, 0x30, 0x76, 0x30, 0x10, 0x06, 0x07, 0x2a, 0x86,
	0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x05, 0x2b, 0x81, 0x04, 0x00, 0x22,
	0x03, 0x62, 0x00, 0x04, 0xc3, 0xda, 0x2b, 0x34, 0x41, 0x37, 0x58, 0x2f,
	0x87, 0x56, 0xfe, 0xfc, 0x89, 0xba, 0x29, 0x43, 0x4b, 0x4e, 0xe0, 0x6e,
	0xc3, 0x0e, 0x57, 0x53, 0x33, 0x39, 0x58, 0xd4, 0x52, 0xb4, 0x91, 0x95,
	0x39, 0x0b, 0x23, 0xdf, 0x5f, 0x17, 0x24, 0x62, 0x48, 0xfc, 0x1a, 0x95,
	0x29, 0xce, 0x2c, 0x2d, 0x87, 0xc2, 0x88, 0x52, 0x80, 0xaf, 0xd6, 0x6a,
	0xab, 0x21, 0xdd, 0xb8, 0xd3, 0x1c, 0x6e, 0x58, 0xb8, 0xca, 0xe8, 0xb2,
	0x69, 0x8e, 0xf3, 0x41, 0xad, 0x29, 0xc3, 0xb4, 0x5f, 0x75, 0xa7, 0x47,
	0x6f, 0xd5, 0x19, 0x29, 0x55, 0x69, 0x9a, 0x53, 0x3b, 0x20, 0xb4, 0x66,
	0x16, 0x60, 0x33, 0x1e, 0xa3, 0x81, 0xa0, 0x30, 0x81, 0x9d, 0x30, 0x1d,
	0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x9d, 0x6d, 0x20,
	0x24, 0x49, 0x01, 0x3f, 0x2b, 0xcb, 0x78, 0xb5, 0x19, 0xbc, 0x7e, 0x24,
	0xc9, 0xdb, 0xfb, 0x36, 0x7c, 0x30, 0x6e, 0x06, 0x03, 0x55, 0x1d, 0x23,
	0x04, 0x67, 0x30, 0x65, 0x80, 0x14, 0x9d, 0x6d, 0x20, 0x24, 0x49, 0x01,
	0x3f, 0x2b, 0xcb, 0x78, 0xb5, 0x19, 0xbc, 0x7e, 0x24, 0xc9, 0xdb, 0xfb,
	0x36, 0x7c, 0xa1, 0x42, 0xa4, 0x40, 0x30, 0x3e, 0x31, 0x0b, 0x30, 0x09,
	0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x4e, 0x4c, 0x31, 0x11, 0x30,
	0x0f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x08, 0x50, 0x6f, 0x6c, 0x61,
	0x72, 0x53, 0x53, 0x4c, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x03, 0x55, 0x04,
	0x03, 0x13, 0x13, 0x50, 0x6f, 0x6c, 0x61, 0x72, 0x73, 0x73, 0x6c, 0x20,
	0x54, 0x65, 0x73, 0x74, 0x20, 0x45, 0x43, 0x20, 0x43, 0x41, 0x82, 0x09,
	0x00, 0xc1, 0x43, 0xe2, 0x7e, 0x62, 0x43, 0xcc, 0xe8, 0x30, 0x0c, 0x06,
	0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30,
	0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03,
	0x69, 0x00, 0x30, 0x66, 0x02, 0x31, 0x00, 0xc3, 0xb4, 0x62, 0x73, 0x56,
	0x28, 0x95, 0x00, 0x7d, 0x78, 0x12, 0x26, 0xd2, 0x71, 0x7b, 0x19, 0xf8,
	0x8a, 0x98, 0x3e, 0x92, 0xfe, 0x33, 0x9e, 0xe4, 0x79, 0xd2, 0xfe, 0x7a,
	0xb7, 0x87, 0x74, 0x3c, 0x2b, 0xb8, 0xd7, 0x69, 0x94, 0x0b, 0xa3, 0x67,
	0x77, 0xb8, 0xb3, 0xbe, 0xd1, 0x36, 0x32, 0x02, 0x31, 0x00, 0xfd, 0x67,
	0x9c, 0x94, 0x23, 0x67, 0xc0, 0x56, 0xba, 0x4b, 0x33, 0x15, 0x00, 0xc6,
	0xe3, 0xcc, 0x31, 0x08, 0x2c, 0x9c, 0x8b, 0xda, 0xa9, 0x75, 0x23, 0x2f,
	0xb8, 0x28, 0xe7, 0xf2, 0x9c, 0x14, 0x3a, 0x40, 0x01, 0x5c, 0xaf, 0x0c,
	0xb2, 0xcf, 0x74, 0x7f, 0x30, 0x9f, 0x08, 0x43, 0xad, 0x20,
};
#endif /* MBEDTLS_X509_CRT_PARSE_C */

enum exit_codes {
	exit_ok = 0,
	ctr_drbg_seed_failed,
	ssl_config_defaults_failed,
	ssl_setup_failed,
	hostname_failed,
	socket_failed,
	connect_failed,
	x509_crt_parse_failed,
	ssl_handshake_failed,
	ssl_write_failed,
};

static int entropy_source(void *data, unsigned char *output, size_t len,
			  size_t *olen)
{
	uint32_t seed;

	ARG_UNUSED(data);

	seed = sys_rand32_get();

	if (len > sizeof(seed)) {
		len = sizeof(seed);
	}

	memcpy(output, &seed, len);

	*olen = len;

	return 0;
}

void tls_client(void)
{
	int ret = exit_ok;
	struct tcp_context ctx;

	ctx.timeout = MBEDTLS_NETWORK_TIMEOUT;

#if defined(MBEDTLS_X509_CRT_PARSE_C)
	mbedtls_x509_crt ca;
#endif

	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_ctr_drbg_init(&ctr_drbg);

	mbedtls_platform_set_printf(PRINT);

	/*
	 * 0. Initialize and setup stuff
	 */
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
	mbedtls_x509_crt_init(&ca);
#endif

	mbedtls_printf("\n  . Seeding the random number generator...");
	mbedtls_entropy_init(&entropy);
	mbedtls_entropy_add_source(&entropy, entropy_source, NULL,
				   MBEDTLS_ENTROPY_MAX_GATHER,
				   MBEDTLS_ENTROPY_SOURCE_STRONG);

	if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
				  (const unsigned char *)pers,
				  strlen(pers)) != 0) {
		ret = ctr_drbg_seed_failed;
		mbedtls_printf
		    (" failed\n  ! mbedtls_ctr_drbg_seed returned -0x%x\n",
		     -ret);
		goto exit;
	}
	mbedtls_printf(" ok\n");

	mbedtls_printf("  . Setting up the SSL/TLS structure...");
	if (mbedtls_ssl_config_defaults(&conf,
					MBEDTLS_SSL_IS_CLIENT,
					MBEDTLS_SSL_TRANSPORT_STREAM,
					MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
		ret = ssl_config_defaults_failed;
		mbedtls_printf
		    (" failed\n  ! mbedtls_ssl_config_defaults returned -0x%x\n\n",
		     -ret);
		goto exit;
	}

	mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
	mbedtls_memory_buffer_alloc_init(heap, sizeof(heap));
#endif

#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
	mbedtls_ssl_conf_psk(&conf, psk, sizeof(psk),
			     (const unsigned char *)psk_id, sizeof(psk_id) - 1);
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
	if (mbedtls_x509_crt_parse_der(&ca, ca_cert, sizeof(ca_cert)) != 0) {
		ret = x509_crt_parse_failed;
		mbedtls_printf
		    (" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n",
		     -ret);
		goto exit;
	}

	mbedtls_ssl_conf_ca_chain(&conf, &ca, NULL);
	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#endif

	if (mbedtls_ssl_setup(&ssl, &conf) != 0) {
		ret = ssl_setup_failed;
		mbedtls_printf
		    (" failed\n  ! mbedtls_ssl_setup returned -0x%x\n\n", -ret);
		goto exit;
	}
#if defined(MBEDTLS_X509_CRT_PARSE_C)
	if (mbedtls_ssl_set_hostname(&ssl, HOSTNAME) != 0) {
		ret = hostname_failed;
		mbedtls_printf
		    (" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n",
		     ret);
		goto exit;
	}
#endif
	mbedtls_printf(" ok\n");

	/*
	 * 1. Start the connection
	 */

	mbedtls_printf("  . Connecting to tcp %s...", SERVER_ADDR);

	if (tcp_set_local_addr(&ctx, LOCAL_ADDR) != 0) {
		printk("tcp set_local_addr error\n");
		goto exit;
	}

	if (tcp_init(&ctx, SERVER_ADDR, SERVER_PORT) != 0) {
		ret = connect_failed;
		mbedtls_printf(" failed\n  ! tcp_init returned -0x%x\n\n",
			       -ret);
		goto exit;
	}

	mbedtls_printf(" ok\n");

	mbedtls_ssl_set_bio(&ssl, &ctx, tcp_tx, tcp_rx, NULL);

	mbedtls_printf("  . Performing the SSL/TLS handshake...");
	if (mbedtls_ssl_handshake(&ssl) != 0) {
		ret = ssl_handshake_failed;
		mbedtls_printf
		    (" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n",
		     -ret);
		goto exit;
	}
	mbedtls_printf(" ok\n");

	/*
	 * 2. Write the GET request and close the connection
	 */
	mbedtls_printf("  > Write to server:");
	if (mbedtls_ssl_write(&ssl, (const unsigned char *)GET_REQUEST,
			      sizeof(GET_REQUEST) - 1) <= 0) {
		ret = ssl_write_failed;
		mbedtls_printf
		    (" failed\n  ! mbedtls_ssl_write returned -0x%x\n\n", -ret);
		goto exit;
	}
	mbedtls_printf(" ok\n");

	mbedtls_printf("  . Closing the connection...");
	mbedtls_ssl_close_notify(&ssl);
	mbedtls_printf(" done\n");
exit:

	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
	mbedtls_x509_crt_free(&ca);
#endif

}

#define STACK_SIZE		8192
uint8_t stack[STACK_SIZE];

void main(void)
{
	k_thread_spawn(stack, STACK_SIZE, (k_thread_entry_t) tls_client,
		       NULL, NULL, NULL, K_PRIO_COOP(7), 0, 0);
}
