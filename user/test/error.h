#pragma once

#include "prelude.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
	u32 line;
	const char *file;
	const char *func;
} SourceLocation;

#define HERE                                                                   \
	((SourceLocation){                                                     \
		.line = __LINE__,                                              \
		.file = __FILE__,                                              \
		.func = __func__,                                              \
	})

void SourceLocation_print(SourceLocation self, FILE *output, bool use_color);

INLINE const char *errno_name(int error)
{
#define e(name)                                                                \
	case name:                                                             \
		return #name

	switch (error) {
		e(EPERM);
		e(ENOENT);
		e(ESRCH);
		e(EINTR);
		e(EIO);
		e(ENXIO);
		e(E2BIG);
		e(ENOEXEC);
		e(EBADF);
		e(ECHILD);
		e(EAGAIN);
		e(ENOMEM);
		e(EACCES);
		e(EFAULT);
		e(ENOTBLK);
		e(EBUSY);
		e(EEXIST);
		e(EXDEV);
		e(ENODEV);
		e(ENOTDIR);
		e(EISDIR);
		e(EINVAL);
		e(ENFILE);
		e(EMFILE);
		e(ENOTTY);
		e(ETXTBSY);
		e(EFBIG);
		e(ENOSPC);
		e(ESPIPE);
		e(EROFS);
		e(EMLINK);
		e(EPIPE);
		e(EDOM);
		e(ERANGE);
		e(EDEADLK);
		e(ENAMETOOLONG);
		e(ENOLCK);
		e(ENOSYS);
		e(ENOTEMPTY);
		e(ELOOP);
		e(ENOMSG);
		e(EIDRM);
		e(ECHRNG);
		e(EL2NSYNC);
		e(EL3HLT);
		e(EL3RST);
		e(ELNRNG);
		e(EUNATCH);
		e(ENOCSI);
		e(EL2HLT);
		e(EBADE);
		e(EBADR);
		e(EXFULL);
		e(ENOANO);
		e(EBADRQC);
		e(EBADSLT);
		e(EBFONT);
		e(ENOSTR);
		e(ENODATA);
		e(ETIME);
		e(ENOSR);
		e(ENONET);
		e(ENOPKG);
		e(EREMOTE);
		e(ENOLINK);
		e(EADV);
		e(ESRMNT);
		e(ECOMM);
		e(EPROTO);
		e(EMULTIHOP);
		e(EDOTDOT);
		e(EBADMSG);
		e(EOVERFLOW);
		e(ENOTUNIQ);
		e(EBADFD);
		e(EREMCHG);
		e(ELIBACC);
		e(ELIBBAD);
		e(ELIBSCN);
		e(ELIBMAX);
		e(ELIBEXEC);
		e(EILSEQ);
		e(ERESTART);
		e(ESTRPIPE);
		e(EUSERS);
		e(ENOTSOCK);
		e(EDESTADDRREQ);
		e(EMSGSIZE);
		e(EPROTOTYPE);
		e(ENOPROTOOPT);
		e(EPROTONOSUPPORT);
		e(ESOCKTNOSUPPORT);
		e(EOPNOTSUPP);
		e(EPFNOSUPPORT);
		e(EAFNOSUPPORT);
		e(EADDRINUSE);
		e(EADDRNOTAVAIL);
		e(ENETDOWN);
		e(ENETUNREACH);
		e(ENETRESET);
		e(ECONNABORTED);
		e(ECONNRESET);
		e(ENOBUFS);
		e(EISCONN);
		e(ENOTCONN);
		e(ESHUTDOWN);
		e(ETOOMANYREFS);
		e(ETIMEDOUT);
		e(ECONNREFUSED);
		e(EHOSTDOWN);
		e(EHOSTUNREACH);
		e(EALREADY);
		e(EINPROGRESS);
		e(ESTALE);
		e(EUCLEAN);
		e(ENOTNAM);
		e(ENAVAIL);
		e(EISNAM);
		e(EREMOTEIO);
		e(EDQUOT);
		e(ENOMEDIUM);
		e(EMEDIUMTYPE);
		e(ECANCELED);
		e(ENOKEY);
		e(EKEYEXPIRED);
		e(EKEYREVOKED);
		e(EKEYREJECTED);
		e(EOWNERDEAD);
		e(ENOTRECOVERABLE);
		e(ERFKILL);
		e(EHWPOISON);
	case 0:
		return "ENONE";
	default:
		return "EUNKNOWN";
	}

#undef e
}

void check_eq(FILE *output, SourceLocation location, const void *actual,
	      const void *expected, bool (*eq)(const void *, const void *),
	      void (*print)(FILE *, const void *), const char *prefix);

void check_errno(FILE *output, SourceLocation location, int error);
