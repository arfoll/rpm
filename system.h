/**
 * \file system.h
 *
 *  Some misc low-level API
 */

#ifndef	H_SYSTEM
#define	H_SYSTEM

#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

/* <unistd.h> should be included before any preprocessor test
   of _POSIX_VERSION.  */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#if !defined(__GLIBC__)
#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern char ** environ;
#endif /* __APPLE__ */
#endif
#endif

#if !defined(HAVE_STPCPY)
char * stpcpy(char * dest, const char * src);
#endif

#if !defined(HAVE_STPNCPY)
char * stpncpy(char * dest, const char * src, size_t n);
#endif

#if HAVE_SECURE_GETENV
#define	getenv(_s)	secure_getenv(_s)
#elif HAVE___SECURE_GETENV
#define	getenv(_s)	__secure_getenv(_s)
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/file.h>
#endif

#ifdef HAVE_DIRENT_H
# include <dirent.h>
# define NLENGTH(direct) (strlen((direct)->d_name))
#else /* not HAVE_DIRENT_H */
# define dirent direct
# define NLENGTH(direct) ((direct)->d_namlen)
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif /* HAVE_SYS_NDIR_H */
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif /* HAVE_SYS_DIR_H */
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif /* HAVE_NDIR_H */
#endif /* HAVE_DIRENT_H */

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef PATH_MAX
#ifdef _POSIX_PATH_MAX
#define PATH_MAX _POSIX_PATH_MAX
#elif defined MAXPATHLEN
#define PATH_MAX MAXPATHLEN
#else
#define PATH_MAX 256
#endif
#endif

#if defined(HAVE_FDATASYNC) && !HAVE_DECL_FDATASYNC
extern int fdatasync(int fildes);
#endif

#include "rpmio/rpmutil.h"
/* compatibility macros to avoid a mass-renaming all over the codebase */
#define xmalloc(_size) rmalloc((_size))
#define xcalloc(_nmemb, _size) rcalloc((_nmemb), (_size))
#define xrealloc(_ptr, _size) rrealloc((_ptr), (_size))
#define xstrdup(_str) rstrdup((_str))
#define _free(_ptr) rfree((_ptr))

/* To extract program's name: use calls (reimplemented or shipped with system):
   - void setprogname(const char *pn)
   - const char *getprogname(void)

   setprogname(*pn) must be the first call in main() in order to set the name
   as soon as possible. */
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__) || defined(__APPLE__)
/* `man setprogname' v. May 21, 2011 (NetBSD 6.1) says:
   Portable programs that wish to use getprogname() should call setprogname()
   in main(). In some systems (like NetBSD) it can be set only once and it is
   done before an execution of main() -- therefore calling it again has no
   effect.

   Getprogname and setprogname function calls appeared in NetBSD 1.6
   (released in 2002). So nothing to (re)implement for this platform. */
# include <stdlib.h> /* Make sure this header is included */
# define xsetprogname(pn) setprogname(pn)
# define xgetprogname(pn) getprogname(pn)
#elif defined(__GLIBC__) /* GNU LIBC ships with (const *char *) __progname */
# define xsetprogname(pn) /* No need to implement it in GNU LIBC. */
  extern const char *__progname;
# define xgetprogname(pn) __progname
#else /* Reimplement setprogname and getprogname */
# include "misc/rpmxprogname.h"
# define xsetprogname(pn) _rpmxsetprogname(pn)
# define xgetprogname() _rpmxgetprogname()
#endif

/* Take care of NLS matters.  */
#if ENABLE_NLS
# include <locale.h>
# include <libintl.h>
# define _(Text) dgettext (PACKAGE, Text)
#else
# define _(Text) Text
#endif

#define N_(Text) Text

/* ============== from misc/miscfn.h */

#include "misc/fnmatch.h"

//#include <dlfcn.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <direct.h>
#define EOPNOTSUPP ENOSYS
#define S_ISUID 0
#define S_ISGID 0
#define S_ISVTX 0
#define S_IWGRP S_IWUSR
#define S_IXGRP S_IXUSR
#define S_IXOTH S_IXUSR
typedef int uid_t;
typedef int gid_t;
typedef int nlink_t;
typedef int sigset_t; /* dummy */
#define chown(path, uid, gid) (((uid) != 0 && (gid) != 0) ? (errno = ENOSYS, -1) : 0)
#define chroot(path) (fprintf(stderr, "chroot(%s)\n", (path)), errno = ENOSYS, -1)
#define fchdir(fd) (fprintf(stderr, "fchdir(%d)\n", (fd)), errno = ENOSYS, -1)
#define getuid() 0
#define getgid() 0
#define lstat stat
#define mkdir(path, mode) _mkdir(path)
#define mkfifo(path, mode) (fprintf(stderr, "UNIMP: mkfifo(%s, %#o)\n", (path), (mode)), errno = ENOSYS, -1)
#define mknod(path, mode, dev) (fprintf(stderr, "UNIMP: mknod(%s,%#o,%x)\n", (path), (mode), (dev)), errno = ENOSYS, -1)
#define link(source, target) (fprintf(stderr, "UNIMP: link(%s,%s)\n", (source), (target)), errno = ENOSYS, -1)
#define readlink(path, buf, size) (errno = EINVAL, -1)
#define sleep(secs) Sleep((secs)*1000)
#define symlink(source, target) (fprintf(stderr, "UNIMP: symlink(%s,%s)\n", (source), (target)), errno = ENOSYS, -1)
char *realpath(const char *path, char resolved_path []);

#define IS_DIR_SEP(c) ((c) == '/' || (c) == '\\')
#define DIR_IS_ABSOLUTE(d) (IS_DIR_SEP((d)[0]) || (isalpha((d)[0]) && (d)[1] == ':' && IS_DIR_SEP((d)[2])))
#define PATH_SEP ';'
#define PATH_SEP_STRING ";"

#endif	/* H_SYSTEM */
