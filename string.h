#ifndef _LINUX_STRING_H_
#define _LINUX_STRING_H_

#include <stddef.h>	/* for NULL */

extern char * ___strtok;
extern char * strpbrk(const char *, const char *);
extern char * strtok(char *, const char *);
extern char * strsep(char **, const char *);
extern size_t strspn(const char *, const char *);

#ifndef __HAVE_ARCH_STRCPY
extern char * strcpy(char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRNCPY
extern char * strncpy(char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRCAT
extern char * strcat(char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRNCAT
extern char * strncat(char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRCMP
extern int strcmp(const char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRNCMP
extern int strncmp(const char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRNICMP
extern int strnicmp(const char *, const char *, size_t);
#endif
#ifndef __HAVE_ARCH_STRCHR
extern char * strchr(const char *, int);
#endif
#ifndef __HAVE_ARCH_STRRCHR
extern char * strrchr(const char *, int);
#endif
#ifndef __HAVE_ARCH_STRSTR
extern char * strstr(const char *, const char *);
#endif
#ifndef __HAVE_ARCH_STRLEN
extern size_t strlen(const char *);
#endif
#ifndef __HAVE_ARCH_STRNLEN
extern size_t strnlen(const char *, size_t);
#endif

#define __HAVE_ARCH_STRDUP
#ifndef __HAVE_ARCH_STRDUP
extern char * strdup(const char *);
#endif

#ifndef __HAVE_ARCH_STRSWAB
extern char * strswab(const char *);
#endif

#ifndef __HAVE_ARCH_MEMSET
extern void * memset(void *, int, size_t);
#endif
#ifndef __HAVE_ARCH_MEMCPY
extern void * memcpy(void *, const void *, u16_t);
#endif
#ifndef __HAVE_ARCH_MEMMOVE
extern void * memmove(void *, const void *, size_t);
#endif
#ifndef __HAVE_ARCH_MEMSCAN
extern void * memscan(void *, int, size_t);
#endif
#ifndef __HAVE_ARCH_MEMCMP
extern int memcmp(const void *, const void *, size_t);
#endif
#ifndef __HAVE_ARCH_MEMCHR
extern void * memchr(const void *, int, size_t);
#endif

#endif /* _LINUX_STRING_H_ */
