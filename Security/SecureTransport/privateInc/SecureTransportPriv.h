/*
 * Copyright (c) 2000-2001 Apple Computer, Inc. All Rights Reserved.
 * 
 * The contents of this file constitute Original Code as defined in and are
 * subject to the Apple Public Source License Version 1.2 (the 'License').
 * You may not use this file except in compliance with the License. Please obtain
 * a copy of the License at http://www.apple.com/publicsource and read it before
 * using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. Please see the License for the
 * specific language governing rights and limitations under the License.
 */


/*
	File:		SecureTransportPriv.h

	Contains:	Apple-private exported routines

	Copyright: (c) 1999 by Apple Computer, Inc., all rights reserved.

*/

#ifndef	_SECURE_TRANSPORT_PRIV_H_
#define _SECURE_TRANSPORT_PRIV_H_	1

#include <Security/SecureTransport.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The size of of client- and server-generated random numbers in hello messages. */
#define SSL_CLIENT_SRVR_RAND_SIZE		32

/* The size of the pre-master and master secrets. */
#define SSL_RSA_PREMASTER_SECRET_SIZE	48
#define SSL_MASTER_SECRET_SIZE			48

/*
 * For the following three functions, *size is the available 
 * buffer size on entry and the actual size of the data returned
 * on return. The above consts are for convenience.
 */
OSStatus SSLInternalMasterSecret(
   SSLContextRef context,
   void *secret,         // mallocd by caller, SSL_MASTER_SECRET_SIZE 
   size_t *secretSize);  // in/out   

OSStatus SSLInternalServerRandom(
   SSLContextRef context,
   void *rand, 			// mallocd by caller, SSL_CLIENT_SRVR_RAND_SIZE
   size_t *randSize);	// in/out   

OSStatus SSLInternalClientRandom(
   SSLContextRef context,
   void *rand,  		// mallocd by caller, SSL_CLIENT_SRVR_RAND_SIZE
   size_t *randSize);	// in/out   

OSStatus SSLInternal_PRF(
   SSLContextRef context,
   const void *secret,
   size_t secretLen,
   const void *label,
   size_t labelLen,
   const void *seed,
   size_t seedLen,
   void *out,   		// mallocd by caller, length >= outLen
   size_t outLen);

#ifdef __cplusplus
}
#endif

#endif	/* _SECURE_TRANSPORT_PRIV_H_ */
