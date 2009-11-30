/* See md5.c for explanation and copyright information.  */

/*
 * $FreeBSD: src/contrib/cvs/lib/md5.h,v 1.2 1999/12/11 15:10:02 peter Exp $
 */

#ifndef MD5_H
#define MD5_H

#include <stdint.h>

/* Unlike previous versions of this code, uint32 need not be exactly
   32 bits, merely 32 bits or more.  Choosing a data type which is 32
   bits instead of 64 is not important; speed is considerably more
   important.  */

struct ldesk_MD5Context {
	uint32_t buf[4];
	uint32_t bits[2];
	uint8_t in[64];
};

void ldesk_MD5Init(struct ldesk_MD5Context *context);
void ldesk_MD5Update(struct ldesk_MD5Context *context, const uint8_t *buf, size_t len);
void ldesk_MD5Final(uint8_t digest[16], struct ldesk_MD5Context *context);
void ldesk_MD5Transform(uint32_t buf[4], const uint8_t in[64]);

#endif /* !MD5_H */
