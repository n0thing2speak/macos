/*
 * Copyright (c) 2005-2016 Apple Inc. All rights reserved.
 *
 * @APPLE_BSD_LICENSE_HEADER_START@
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @APPLE_BSD_LICENSE_HEADER_END@
 */

#ifdef USE_BSM_AUDIT

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/usr.bin/login/login_audit.c,v 1.2 2007/05/07 11:01:36 dwmalone Exp $");

#include <sys/types.h>

#include <bsm/libbsm.h>
#include <bsm/audit_uevents.h>
#include <bsm/audit_session.h>

#include <err.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "login.h"

/*
 * Audit data
 */
au_tid_addr_t tid;

/*
 * The following tokens are included in the audit record for a successful
 * login: header, subject, return.
 */
void
au_login_success(int fflag)
{
	token_t *tok;
	int aufd;
	auditinfo_addr_t auinfo;
	uid_t uid = pwd->pw_uid;
	gid_t gid = pwd->pw_gid;
	pid_t pid = getpid();
	long au_cond;

	/* Determine whether auditing is enabled. */
	if (auditon(A_GETCOND, &au_cond, sizeof(long)) < 0) {
		if (errno == ENOSYS)
			return;
		errx(1, "login: Could not determine audit condition");
	}

	/* Initialize with the current audit info. */
	if (getaudit_addr(&auinfo, sizeof(auinfo)) < 0) {
		err(1, "getaudit_addr");
	}
	auinfo.ai_auid = pwd->pw_uid;
	memcpy(&auinfo.ai_termid, &tid, sizeof(auinfo.ai_termid));

	/* Do the SessionCreate() equivalent. */
	if (!fflag) {
		auinfo.ai_asid = AU_ASSIGN_ASID;
		auinfo.ai_flags |= AU_SESSION_FLAG_HAS_TTY;
		auinfo.ai_flags |= AU_SESSION_FLAG_HAS_AUTHENTICATED;
	}

	if (au_cond != AUC_NOAUDIT) {
		/* Compute and set the user's preselection mask. */
		if (au_user_mask(pwd->pw_name, &auinfo.ai_mask) < 0) {
			errx(1, "login: Could not set audit mask\n");
		}
	}

	if (setaudit_addr(&auinfo, sizeof(auinfo)) < 0)
		err(1, "login: setaudit_addr failed");

	char *session = NULL;
	asprintf(&session, "%x", auinfo.ai_asid);
	if (NULL == session) {
		errx(1, "asprintf failed");
	}
	setenv("SECURITYSESSIONID", session, 1);
	free(session);

	/* If we are not auditing, don't cut an audit record; just return. */
	if (au_cond == AUC_NOAUDIT)
		return;

	if ((aufd = au_open()) == -1)
		errx(1,"login: Audit Error: au_open() failed");

	if ((tok = au_to_subject32_ex(uid, geteuid(), getegid(), uid, gid, pid,
	    pid, &tid)) == NULL)
		errx(1, "login: Audit Error: au_to_subject32() failed");
	au_write(aufd, tok);

	if ((tok = au_to_return32(0, 0)) == NULL)
		errx(1, "login: Audit Error: au_to_return32() failed");
	au_write(aufd, tok);

	if (au_close(aufd, 1, AUE_login) == -1)
		errx(1, "login: Audit Record was not committed.");
}

/*
 * The following tokens are included in the audit record for failed
 * login attempts: header, subject, text, return.
 */
void
au_login_fail(const char *errmsg, int na)
{
	token_t *tok;
	int aufd;
	long au_cond;
	uid_t uid;
	gid_t gid;
	pid_t pid = getpid();

	/* If we are not auditing, don't cut an audit record; just return. */
	if (auditon(A_GETCOND, &au_cond, sizeof(long)) < 0) {
		if (errno == ENOSYS)
			return;
		errx(1, "login: Could not determine audit condition");
	}
	if (au_cond == AUC_NOAUDIT)
		return;

	if ((aufd = au_open()) == -1)
		errx(1, "login: Audit Error: au_open() failed");

	if (na) {
		/*
		 * Non attributable event.  Assuming that login is not called
		 * within a user's session => auid,asid == -1.
		 */
		if ((tok = au_to_subject32_ex(-1, geteuid(), getegid(), -1, -1,
		    pid, -1, &tid)) == NULL)
			errx(1, "login: Audit Error: au_to_subject32() failed");
	} else {
		/* We know the subject -- so use its value instead. */
		uid = pwd->pw_uid;
		gid = pwd->pw_gid;
		if ((tok = au_to_subject32_ex(uid, geteuid(), getegid(), uid,
		    gid, pid, pid, &tid)) == NULL)
			errx(1, "login: Audit Error: au_to_subject32() failed");
	}
	au_write(aufd, tok);

	/* Include the error message. */
	if ((tok = au_to_text(errmsg)) == NULL)
		errx(1, "login: Audit Error: au_to_text() failed");
	au_write(aufd, tok);

	if ((tok = au_to_return32(1, errno)) == NULL)
		errx(1, "login: Audit Error: au_to_return32() failed");
	au_write(aufd, tok);

	if (au_close(aufd, 1, AUE_login) == -1)
		errx(1, "login: Audit Error: au_close() was not committed");
}

/*
 * The following tokens are included in the audit record for a logout:
 * header, subject, return.
 */
void
audit_logout(void)
{
	token_t *tok;
	int aufd;
	uid_t uid = pwd->pw_uid;
	gid_t gid = pwd->pw_gid;
	pid_t pid = getpid();
	long au_cond;

	/* If we are not auditing, don't cut an audit record; just return. */
	if (auditon(A_GETCOND, &au_cond, sizeof(long)) < 0) {
		if (errno == ENOSYS)
			return;
		errx(1, "login: Could not determine audit condition");
	}
	if (au_cond == AUC_NOAUDIT)
		return;

	if ((aufd = au_open()) == -1)
		errx(1, "login: Audit Error: au_open() failed");

	/* The subject that is created (euid, egid of the current process). */
	if ((tok = au_to_subject32_ex(uid, geteuid(), getegid(), uid, gid, pid,
	    pid, &tid)) == NULL)
		errx(1, "login: Audit Error: au_to_subject32() failed");
	au_write(aufd, tok);

	if ((tok = au_to_return32(0, 0)) == NULL)
		errx(1, "login: Audit Error: au_to_return32() failed");
	au_write(aufd, tok);

	if (au_close(aufd, 1, AUE_logout) == -1)
		errx(1, "login: Audit Record was not committed.");
}

#endif /* USE_BSM_AUDIT */
