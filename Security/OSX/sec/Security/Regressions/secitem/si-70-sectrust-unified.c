/*
 * Copyright (c) 2013-2014 Apple Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */


//
//  si-70-sectrust-unified.c
//  regressions
//
//
//
#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#include <Security/SecCertificatePriv.h>
#include <utilities/array_size.h>
#include <utilities/SecCFWrappers.h>

#include "shared_regressions.h"
#include <test/testcert.h>

/* This is a minimal test case to ensure that the functionality of
 * <rdar://problem/11736016> TLF: SecTrust Unification is present
 * and working. Needs to be expanded and split up into separate
 * test case files in the future.
 */

/* SecPolicy: new in 7.0 */
//CFDictionaryRef SecPolicyCopyProperties(SecPolicyRef policyRef)
//OSStatus SecTrustSetPolicies(SecTrustRef trust, CFTypeRef policies) /* (this was SPI in 6.0) */
//SecPolicyRef SecPolicyCreateRevocation(CFIndex revocationFlags)
//SecPolicyRef SecPolicyCreateWithProperties(CFTypeRef policyIdentifier, CFDictionaryRef properties)

/* SecTrust new in 7.0 */
//OSStatus SecTrustCopyPolicies(SecTrustRef trust, CFArrayRef *policies)
//OSStatus SecTrustSetNetworkFetchAllowed(SecTrustRef trust, Boolean allowFetch)
//OSStatus SecTrustGetNetworkFetchAllowed(SecTrustRef trust, Boolean *allowFetch)
//OSStatus SecTrustCopyCustomAnchorCertificates(SecTrustRef trust, CFArrayRef *anchors)
//CFDictionaryRef SecTrustCopyResult(SecTrustRef trust)
//OSStatus SecTrustSetOCSPResponse(SecTrustRef trust, CFTypeRef responseData)


/* subject:/C=US/ST=California/L=Mountain View/O=Google LLC/CN=www.google.com */
/* issuer :/C=US/O=Google Trust Services/CN=Google Internet Authority G3 */
const uint8_t google_cert[]={
    0x30,0x82,0x03,0xC7,0x30,0x82,0x02,0xAF,0xA0,0x03,0x02,0x01,0x02,0x02,0x08,0x55,
    0x81,0x47,0xC4,0x26,0x8C,0x3F,0xC2,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
    0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x54,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,
    0x06,0x13,0x02,0x55,0x53,0x31,0x1E,0x30,0x1C,0x06,0x03,0x55,0x04,0x0A,0x13,0x15,
    0x47,0x6F,0x6F,0x67,0x6C,0x65,0x20,0x54,0x72,0x75,0x73,0x74,0x20,0x53,0x65,0x72,
    0x76,0x69,0x63,0x65,0x73,0x31,0x25,0x30,0x23,0x06,0x03,0x55,0x04,0x03,0x13,0x1C,
    0x47,0x6F,0x6F,0x67,0x6C,0x65,0x20,0x49,0x6E,0x74,0x65,0x72,0x6E,0x65,0x74,0x20,
    0x41,0x75,0x74,0x68,0x6F,0x72,0x69,0x74,0x79,0x20,0x47,0x33,0x30,0x1E,0x17,0x0D,
    0x31,0x38,0x30,0x35,0x30,0x38,0x31,0x34,0x34,0x37,0x34,0x33,0x5A,0x17,0x0D,0x31,
    0x38,0x30,0x37,0x33,0x31,0x31,0x33,0x32,0x37,0x30,0x30,0x5A,0x30,0x68,0x31,0x0B,
    0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x13,0x30,0x11,0x06,
    0x03,0x55,0x04,0x08,0x0C,0x0A,0x43,0x61,0x6C,0x69,0x66,0x6F,0x72,0x6E,0x69,0x61,
    0x31,0x16,0x30,0x14,0x06,0x03,0x55,0x04,0x07,0x0C,0x0D,0x4D,0x6F,0x75,0x6E,0x74,
    0x61,0x69,0x6E,0x20,0x56,0x69,0x65,0x77,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,
    0x0A,0x0C,0x0A,0x47,0x6F,0x6F,0x67,0x6C,0x65,0x20,0x4C,0x4C,0x43,0x31,0x17,0x30,
    0x15,0x06,0x03,0x55,0x04,0x03,0x0C,0x0E,0x77,0x77,0x77,0x2E,0x67,0x6F,0x6F,0x67,
    0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x30,0x59,0x30,0x13,0x06,0x07,0x2A,0x86,0x48,0xCE,
    0x3D,0x02,0x01,0x06,0x08,0x2A,0x86,0x48,0xCE,0x3D,0x03,0x01,0x07,0x03,0x42,0x00,
    0x04,0xDD,0x10,0xCB,0x4F,0xB1,0x49,0xF9,0xE8,0xC2,0x8E,0xB5,0xB9,0xC3,0x7D,0xCC,
    0x9D,0x94,0x3A,0x91,0x19,0x7C,0xA9,0xB3,0x78,0x81,0x21,0x01,0xC0,0x76,0x12,0xA9,
    0x84,0x65,0xDF,0xD3,0xE2,0x51,0xFF,0x17,0x9F,0x69,0x0F,0x0B,0xFA,0x04,0x0D,0xBA,
    0x35,0xBB,0xE8,0x1F,0x14,0x66,0xB7,0xC7,0xD7,0xFC,0xEB,0x10,0xD6,0xCD,0x79,0x8A,
    0x22,0xA3,0x82,0x01,0x52,0x30,0x82,0x01,0x4E,0x30,0x13,0x06,0x03,0x55,0x1D,0x25,
    0x04,0x0C,0x30,0x0A,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x03,0x01,0x30,0x0E,
    0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,0x04,0x04,0x03,0x02,0x07,0x80,0x30,0x19,
    0x06,0x03,0x55,0x1D,0x11,0x04,0x12,0x30,0x10,0x82,0x0E,0x77,0x77,0x77,0x2E,0x67,
    0x6F,0x6F,0x67,0x6C,0x65,0x2E,0x63,0x6F,0x6D,0x30,0x68,0x06,0x08,0x2B,0x06,0x01,
    0x05,0x05,0x07,0x01,0x01,0x04,0x5C,0x30,0x5A,0x30,0x2D,0x06,0x08,0x2B,0x06,0x01,
    0x05,0x05,0x07,0x30,0x02,0x86,0x21,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x70,0x6B,
    0x69,0x2E,0x67,0x6F,0x6F,0x67,0x2F,0x67,0x73,0x72,0x32,0x2F,0x47,0x54,0x53,0x47,
    0x49,0x41,0x47,0x33,0x2E,0x63,0x72,0x74,0x30,0x29,0x06,0x08,0x2B,0x06,0x01,0x05,
    0x05,0x07,0x30,0x01,0x86,0x1D,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x6F,0x63,0x73,
    0x70,0x2E,0x70,0x6B,0x69,0x2E,0x67,0x6F,0x6F,0x67,0x2F,0x47,0x54,0x53,0x47,0x49,
    0x41,0x47,0x33,0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,0x2B,0x53,
    0xE0,0x79,0xD4,0xFD,0xA4,0xD4,0xDF,0x18,0x6B,0xDD,0x80,0x4D,0x11,0x35,0xC7,0xB2,
    0x41,0xCC,0x30,0x0C,0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x02,0x30,0x00,
    0x30,0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x77,0xC2,0xB8,
    0x50,0x9A,0x67,0x76,0x76,0xB1,0x2D,0xC2,0x86,0xD0,0x83,0xA0,0x7E,0xA6,0x7E,0xBA,
    0x4B,0x30,0x21,0x06,0x03,0x55,0x1D,0x20,0x04,0x1A,0x30,0x18,0x30,0x0C,0x06,0x0A,
    0x2B,0x06,0x01,0x04,0x01,0xD6,0x79,0x02,0x05,0x03,0x30,0x08,0x06,0x06,0x67,0x81,
    0x0C,0x01,0x02,0x02,0x30,0x31,0x06,0x03,0x55,0x1D,0x1F,0x04,0x2A,0x30,0x28,0x30,
    0x26,0xA0,0x24,0xA0,0x22,0x86,0x20,0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x63,0x72,
    0x6C,0x2E,0x70,0x6B,0x69,0x2E,0x67,0x6F,0x6F,0x67,0x2F,0x47,0x54,0x53,0x47,0x49,
    0x41,0x47,0x33,0x2E,0x63,0x72,0x6C,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,
    0x0D,0x01,0x01,0x0B,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0x6E,0x85,0x02,0xC0,0xF0,
    0x15,0xBF,0xAF,0x4F,0x29,0x73,0x19,0x87,0x7F,0x30,0xB3,0x24,0xD1,0xEE,0xA7,0xDC,
    0x90,0x44,0x30,0xC1,0xA0,0x84,0x65,0x52,0x26,0xE6,0xAD,0x0D,0xCA,0x43,0xEE,0xB6,
    0x6B,0x37,0x9D,0xFF,0x97,0x80,0x09,0x85,0x58,0x46,0xEC,0xFF,0xF2,0x42,0x6A,0xBB,
    0xE6,0xA3,0xB4,0x9B,0x26,0x26,0xA8,0x53,0xA9,0xB9,0x95,0xB6,0x42,0x06,0x94,0xED,
    0x31,0xC5,0x33,0xF7,0x91,0x6A,0x90,0x4B,0xD2,0x8A,0x45,0xAE,0x3A,0xA0,0x10,0x27,
    0xAE,0xF4,0x9A,0xC9,0x5E,0x63,0x20,0xAD,0xF2,0xCB,0xDC,0x74,0xA8,0x83,0x32,0x56,
    0x6D,0xAA,0x6C,0xCA,0xBC,0xCC,0x71,0x23,0xD4,0xAC,0xA9,0xAE,0xEA,0x04,0xD6,0x75,
    0xE7,0xBF,0x18,0xC7,0x9C,0xCC,0x7B,0xE6,0x81,0x62,0xC6,0xFA,0x17,0xA8,0x82,0x2F,
    0xCC,0xE9,0xAC,0xEF,0x81,0xCC,0xAE,0x1A,0x1C,0x79,0x35,0x7B,0x54,0xFE,0x06,0x57,
    0x2F,0x58,0xD0,0x7C,0x4E,0x5A,0x75,0xAE,0xCC,0x31,0xD6,0x20,0xA6,0xB1,0xDA,0x39,
    0x9E,0x46,0x5B,0x15,0x76,0xF2,0x3E,0x2C,0xB1,0x5E,0xBF,0x7F,0x29,0xE3,0xBE,0xC6,
    0xF3,0xE5,0xEB,0xD5,0x91,0x48,0x84,0x41,0x7B,0xB6,0x3B,0x83,0xC6,0xCE,0x1B,0xE2,
    0x88,0x44,0x91,0x89,0x72,0x27,0xF9,0xD2,0x72,0x33,0xCF,0xC3,0xB2,0x52,0x38,0x65,
    0x17,0x14,0x00,0x4E,0x36,0x1C,0xC2,0xAD,0xBF,0x7F,0x3A,0x18,0xF7,0x52,0xFA,0x3B,
    0x86,0x18,0xF3,0x24,0x97,0xF7,0x35,0x58,0x48,0x0D,0x7D,0x93,0x18,0xA7,0x14,0x52,
    0x1A,0x19,0x9D,0xDB,0xD5,0xCC,0xA3,0xC5,0x48,0x6D,0x8A,
};

/* subject:/C=US/O=Google Trust Services/CN=Google Internet Authority G3 */
/* issuer :/OU=GlobalSign Root CA - R2/O=GlobalSign/CN=GlobalSign */
const uint8_t _GIAG3[]={
    0x30,0x82,0x04,0x5C,0x30,0x82,0x03,0x44,0xA0,0x03,0x02,0x01,0x02,0x02,0x0D,0x01,
    0xE3,0xA9,0x30,0x1C,0xFC,0x72,0x06,0x38,0x3F,0x9A,0x53,0x1D,0x30,0x0D,0x06,0x09,
    0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x4C,0x31,0x20,0x30,
    0x1E,0x06,0x03,0x55,0x04,0x0B,0x13,0x17,0x47,0x6C,0x6F,0x62,0x61,0x6C,0x53,0x69,
    0x67,0x6E,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,0x41,0x20,0x2D,0x20,0x52,0x32,0x31,
    0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x0A,0x13,0x0A,0x47,0x6C,0x6F,0x62,0x61,0x6C,
    0x53,0x69,0x67,0x6E,0x31,0x13,0x30,0x11,0x06,0x03,0x55,0x04,0x03,0x13,0x0A,0x47,
    0x6C,0x6F,0x62,0x61,0x6C,0x53,0x69,0x67,0x6E,0x30,0x1E,0x17,0x0D,0x31,0x37,0x30,
    0x36,0x31,0x35,0x30,0x30,0x30,0x30,0x34,0x32,0x5A,0x17,0x0D,0x32,0x31,0x31,0x32,
    0x31,0x35,0x30,0x30,0x30,0x30,0x34,0x32,0x5A,0x30,0x54,0x31,0x0B,0x30,0x09,0x06,
    0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x1E,0x30,0x1C,0x06,0x03,0x55,0x04,
    0x0A,0x13,0x15,0x47,0x6F,0x6F,0x67,0x6C,0x65,0x20,0x54,0x72,0x75,0x73,0x74,0x20,
    0x53,0x65,0x72,0x76,0x69,0x63,0x65,0x73,0x31,0x25,0x30,0x23,0x06,0x03,0x55,0x04,
    0x03,0x13,0x1C,0x47,0x6F,0x6F,0x67,0x6C,0x65,0x20,0x49,0x6E,0x74,0x65,0x72,0x6E,
    0x65,0x74,0x20,0x41,0x75,0x74,0x68,0x6F,0x72,0x69,0x74,0x79,0x20,0x47,0x33,0x30,
    0x82,0x01,0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,
    0x05,0x00,0x03,0x82,0x01,0x0F,0x00,0x30,0x82,0x01,0x0A,0x02,0x82,0x01,0x01,0x00,
    0xCA,0x52,0x4B,0xEA,0x1E,0xFF,0xCE,0x24,0x6B,0xA8,0xDA,0x72,0x18,0x68,0xD5,0x56,
    0x5D,0x0E,0x48,0x5A,0x2D,0x35,0x09,0x76,0x5A,0xCF,0xA4,0xC8,0x1C,0xB1,0xA9,0xFE,
    0x53,0x89,0xFB,0xAD,0x34,0xFF,0x88,0x5B,0x9F,0xBB,0xE7,0xE8,0x00,0x01,0xDC,0x35,
    0x73,0x75,0x03,0xAD,0xB3,0xB1,0xB9,0xA4,0x7D,0x2B,0x26,0x79,0xCE,0x15,0x40,0x0A,
    0xEF,0x51,0xB8,0x9F,0x32,0x8C,0x7C,0x70,0x86,0x52,0x4B,0x16,0xFE,0x6A,0x27,0x6B,
    0xE6,0x36,0x7A,0x62,0x50,0xD8,0xDF,0x9A,0x89,0xCC,0x09,0x29,0xEB,0x4F,0x29,0x14,
    0x88,0x80,0x0B,0x8F,0x38,0x1E,0x80,0x6A,0x18,0x7C,0x1D,0xBD,0x97,0x3B,0x78,0x7D,
    0x45,0x49,0x36,0x4F,0x41,0xCD,0xA2,0xE0,0x76,0x57,0x3C,0x68,0x31,0x79,0x64,0xC9,
    0x6E,0xD7,0x51,0x1E,0x66,0xC3,0xA2,0x64,0x2C,0x79,0xC0,0xE7,0x65,0xC3,0x56,0x84,
    0x53,0x5A,0x43,0x6D,0xCB,0x9A,0x02,0x20,0xD2,0xEF,0x1A,0x69,0xD1,0xB0,0x9D,0x73,
    0xA2,0xE0,0x2A,0x60,0x65,0x50,0x31,0xCF,0xFB,0xB3,0x2F,0xBF,0x11,0x88,0x40,0x2E,
    0xB5,0x49,0x10,0x0F,0x0A,0x6E,0xDC,0x97,0xFA,0xBF,0x2C,0x9F,0x05,0x39,0x0B,0x58,
    0x54,0xAF,0x06,0x96,0xE8,0xC5,0x8E,0x01,0x16,0xBC,0xA8,0x1A,0x4D,0x41,0xC5,0x93,
    0x91,0xA2,0x1E,0xA1,0x8B,0xF2,0xFE,0xC1,0x88,0x24,0x49,0xA3,0x47,0x4B,0xC5,0x13,
    0x01,0xDD,0xA7,0x57,0x12,0x69,0x62,0x2B,0xEB,0xFE,0x20,0xEF,0x69,0xFB,0x3A,0xA5,
    0xF0,0x7E,0x29,0xEE,0xED,0x96,0x16,0xF7,0xB1,0x1F,0xA0,0xE4,0x90,0x25,0xE0,0x33,
    0x02,0x03,0x01,0x00,0x01,0xA3,0x82,0x01,0x33,0x30,0x82,0x01,0x2F,0x30,0x0E,0x06,
    0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,0x04,0x04,0x03,0x02,0x01,0x86,0x30,0x1D,0x06,
    0x03,0x55,0x1D,0x25,0x04,0x16,0x30,0x14,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,
    0x03,0x01,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x03,0x02,0x30,0x12,0x06,0x03,
    0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x08,0x30,0x06,0x01,0x01,0xFF,0x02,0x01,0x00,
    0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,0x77,0xC2,0xB8,0x50,0x9A,
    0x67,0x76,0x76,0xB1,0x2D,0xC2,0x86,0xD0,0x83,0xA0,0x7E,0xA6,0x7E,0xBA,0x4B,0x30,
    0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x9B,0xE2,0x07,0x57,
    0x67,0x1C,0x1E,0xC0,0x6A,0x06,0xDE,0x59,0xB4,0x9A,0x2D,0xDF,0xDC,0x19,0x86,0x2E,
    0x30,0x35,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x01,0x01,0x04,0x29,0x30,0x27,
    0x30,0x25,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x30,0x01,0x86,0x19,0x68,0x74,
    0x74,0x70,0x3A,0x2F,0x2F,0x6F,0x63,0x73,0x70,0x2E,0x70,0x6B,0x69,0x2E,0x67,0x6F,
    0x6F,0x67,0x2F,0x67,0x73,0x72,0x32,0x30,0x32,0x06,0x03,0x55,0x1D,0x1F,0x04,0x2B,
    0x30,0x29,0x30,0x27,0xA0,0x25,0xA0,0x23,0x86,0x21,0x68,0x74,0x74,0x70,0x3A,0x2F,
    0x2F,0x63,0x72,0x6C,0x2E,0x70,0x6B,0x69,0x2E,0x67,0x6F,0x6F,0x67,0x2F,0x67,0x73,
    0x72,0x32,0x2F,0x67,0x73,0x72,0x32,0x2E,0x63,0x72,0x6C,0x30,0x3F,0x06,0x03,0x55,
    0x1D,0x20,0x04,0x38,0x30,0x36,0x30,0x34,0x06,0x06,0x67,0x81,0x0C,0x01,0x02,0x02,
    0x30,0x2A,0x30,0x28,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x02,0x01,0x16,0x1C,
    0x68,0x74,0x74,0x70,0x73,0x3A,0x2F,0x2F,0x70,0x6B,0x69,0x2E,0x67,0x6F,0x6F,0x67,
    0x2F,0x72,0x65,0x70,0x6F,0x73,0x69,0x74,0x6F,0x72,0x79,0x2F,0x30,0x0D,0x06,0x09,
    0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x03,0x82,0x01,0x01,0x00,
    0x1C,0xB7,0x89,0x96,0xE4,0x53,0xED,0xBB,0xEC,0xDB,0xA8,0x32,0x01,0x9F,0x2C,0xA3,
    0xCD,0x6D,0xAD,0x42,0x12,0x77,0xB3,0xB8,0xE6,0xC9,0x03,0x52,0x60,0x20,0x7B,0x57,
    0x27,0xC6,0x11,0xB5,0x3F,0x67,0x0D,0x99,0x2C,0x5B,0x5A,0xCA,0x22,0x0A,0xDD,0x9E,
    0xBB,0x1F,0x4B,0x48,0x3F,0x8F,0x02,0x3D,0x8B,0x21,0x84,0x45,0x1D,0x6D,0xF5,0xFF,
    0xAC,0x68,0x89,0xCD,0x64,0xE2,0xD6,0xD6,0x5E,0x40,0xC2,0x8E,0x2A,0xF7,0xEF,0x14,
    0xD3,0x36,0xA4,0x40,0x30,0xF5,0x32,0x15,0x15,0x92,0x76,0xFB,0x7E,0x9E,0x53,0xEA,
    0xC2,0x76,0xFC,0x39,0xAD,0x88,0xFE,0x66,0x92,0x26,0xE9,0x1C,0xC4,0x38,0xCD,0x49,
    0xFA,0x43,0x87,0xF0,0x5D,0xD6,0x56,0x4D,0x81,0xD7,0x7F,0xF1,0xC2,0xDD,0xB0,0x4D,
    0xFE,0xC3,0x2A,0x6E,0x7C,0x9F,0x6E,0x5C,0xED,0x62,0x42,0x99,0xE1,0xF7,0x36,0xEE,
    0x14,0x8C,0x2C,0x20,0xE3,0x46,0x97,0x5A,0x77,0x03,0xC0,0xA0,0xC6,0x4A,0x88,0xFD,
    0x40,0x22,0x87,0x72,0x5A,0x18,0xEA,0x9C,0xA5,0xC7,0x5A,0x08,0x8C,0xE4,0x05,0xA4,
    0x7D,0xB9,0x84,0x35,0x5F,0x89,0x36,0x56,0x0E,0x40,0x3D,0x12,0xE8,0xBB,0x35,0x72,
    0xED,0xAF,0x08,0x56,0x4E,0xB0,0xBB,0x2E,0xA9,0x9B,0xE4,0xFB,0x1D,0x3E,0x0B,0x63,
    0xC8,0x9B,0x4B,0x91,0x44,0x66,0x57,0xC0,0x14,0xB4,0x96,0xF0,0xDC,0x2C,0x57,0x3F,
    0x52,0x04,0xAD,0x95,0xAA,0x7D,0x4D,0xD0,0xF2,0x0C,0x9F,0x9C,0x40,0xE8,0xD6,0x55,
    0x73,0xBA,0x3C,0xDF,0x90,0xCB,0x00,0x5B,0x21,0x11,0x67,0xC2,0xED,0x32,0x1E,0xDE,
};


static void tests(void)
{
	SecTrustResultType trustResult;
	SecTrustRef trust = NULL;
    SecPolicyRef policy = NULL;
    CFArrayRef certs = NULL;
	CFDateRef date = NULL;

	const void *cert_google;
	isnt(cert_google = SecCertificateCreateWithBytes(NULL, google_cert,
        sizeof(google_cert)), NULL, "create cert_google");
    certs = CFArrayCreate(NULL, &cert_google, 1, NULL);

    bool server = true;
    policy = SecPolicyCreateSSL(server, CFSTR("www2.google.com")); // deliberate hostname mismatch
    ok_status(SecTrustCreateWithCertificates(certs, policy, &trust),
        "create trust for ssl server www2.google.com");
    CFReleaseSafe(certs);
    date = CFDateCreate(NULL, 548800000.0); /* May 23, 2018" */
    ok_status(SecTrustSetVerifyDate(trust, date), "set trust date to May 23, 2018");

	/* This test uses a root which is no longer in our trust store,
	 * so we need explicitly set it as a trusted anchor
	 */
	SecCertificateRef _anchor;
	isnt(_anchor = SecCertificateCreateWithBytes(NULL, _GIAG3, sizeof(_GIAG3)),
		NULL, "create root");
	const void *v_roots[] = { _anchor };
	CFArrayRef _anchors;
	isnt(_anchors = CFArrayCreate(NULL, v_roots, array_size(v_roots), NULL),
		NULL, "create anchors");
	SecTrustSetAnchorCertificates(trust, _anchors);

    ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate google trust");
    is_status(trustResult, kSecTrustResultRecoverableTrustFailure,
		"trust is kSecTrustResultRecoverableTrustFailure (hostname mismatch)");

	/* Test SecPolicyCreateRevocation */
	{
		/* FIXME need to do more than just call the function, but it's a start */
		SecPolicyRef revocation = SecPolicyCreateRevocation(kSecRevocationUseAnyAvailableMethod);
		isnt(revocation, NULL, "create revocation policy");
		CFReleaseSafe(revocation);
	}

	/* Test SecTrustCopyPolicies */
	{
		CFArrayRef policies = NULL;
		ok_status(SecTrustCopyPolicies(trust, &policies), "copy policies");
		is((policies && (CFArrayGetCount(policies) > 0)), true, "non-empty policies");
		CFReleaseSafe(policies);
	}

	/* Test SecTrustSetNetworkFetchAllowed */
	{
		Boolean curAllow, allow;
		ok_status(SecTrustGetNetworkFetchAllowed(trust, &curAllow));
		allow = !curAllow; /* flip it and see if the setting sticks */
		ok_status(SecTrustSetNetworkFetchAllowed(trust, allow));
		ok_status(SecTrustGetNetworkFetchAllowed(trust, &curAllow));
		is((allow == curAllow), true, "network fetch toggle");
	}

	/* Test setting OCSP response data */
	{
		CFDataRef resp = (CFDataRef) CFDataCreateMutable(NULL, 0);
		/* FIXME: use actual OCSPResponse DER blob */
		CFDataIncreaseLength((CFMutableDataRef)resp, 64); /* arbitrary length, zero-filled data */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonnull"
        // NULL passed as 'trust' newly generates a warning, we need to suppress it in order to compile
		is_status(SecTrustSetOCSPResponse(NULL, resp), errSecParam, "SecTrustSetOCSPResponse param 1 check OK");
#pragma clang diagnostic pop
		is_status(SecTrustSetOCSPResponse(trust, NULL), errSecSuccess, "SecTrustSetOCSPResponse param 2 check OK");
		is_status(SecTrustSetOCSPResponse(trust, resp), errSecSuccess, "SecTrustSetOCSPResponse OK");
        CFReleaseSafe(resp);
	}

	/* Test creation of a policy via SecPolicyCreateWithProperties */
	CFReleaseNull(policy);
	{
		const void *keys[] = { kSecPolicyName, kSecPolicyClient };
		const void *values[] = { CFSTR("www.google.com"), kCFBooleanFalse };
		CFDictionaryRef properties = CFDictionaryCreate(NULL, keys, values,
				array_size(keys),
				&kCFTypeDictionaryKeyCallBacks,
				&kCFTypeDictionaryValueCallBacks);
		policy = SecPolicyCreateWithProperties(kSecPolicyAppleSSL, properties);
		isnt(policy, NULL, "SecPolicyCreateWithProperties");
		CFReleaseSafe(properties);
	}
    if (!policy) { goto errOut; }

	/* Test introspection of a policy's properties via SecPolicyCopyProperties */
	{
		CFDictionaryRef properties = NULL;
		isnt(properties = SecPolicyCopyProperties(policy), NULL, "copy policy properties");
		CFTypeRef value = NULL;
		is(CFDictionaryGetValueIfPresent(properties, kSecPolicyName, (const void **)&value) &&
			kCFCompareEqualTo == CFStringCompare((CFStringRef)value, CFSTR("www.google.com"), 0),
        	true, "has policy name");
		is(CFDictionaryGetValueIfPresent(properties, kSecPolicyOid, (const void **)&value) &&
			CFEqual(value, kSecPolicyAppleSSL) , true, "has SSL policy");
        CFReleaseSafe(properties);
	}
	/* Test setting new policy on a trust via SecTrustSetPolicies */
	ok_status(SecTrustSetPolicies(trust, policy));
	/* Evaluation should now succeed, since our new policy has the correct hostname */
    ok_status(SecTrustEvaluate(trust, &trustResult), "evaluate google trust");
    is_status(trustResult, kSecTrustResultUnspecified,
		"trust is kSecTrustResultUnspecified");

	/* Make sure we can get the results */
	{
		CFDictionaryRef results = NULL;
		SecTrustResultType anotherResult = kSecTrustResultInvalid;
		ok_status(SecTrustGetTrustResult(trust, &anotherResult), "get trust result");
		is_status(trustResult, anotherResult, "trust is kSecTrustResultUnspecified");

		isnt(results = SecTrustCopyResult(trust), NULL, "copy trust results");
		CFReleaseSafe(results);
	}

errOut:
	CFReleaseSafe(trust);
	CFReleaseSafe(policy);
	CFReleaseSafe(date);
	CFReleaseSafe(cert_google);

	CFReleaseSafe(_anchor);
	CFReleaseSafe(_anchors);
}

int si_70_sectrust_unified(int argc, char *const *argv)
{
	plan_tests(27);
	tests();

	return 0;
}
