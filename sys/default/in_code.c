/* $Id: in_code.c,v 1.1.2.1 2005/12/30 17:37:29 amura Exp $ */
/*
 * Some special charactors of buffer internal code
 */

#include "config.h"
#include "def.h"
#include "in_code.h"
#include <stdarg.h>

int
wstrcmp(a, b)
register const NG_WCHAR_t *a;
register const NG_WCHAR_t *b;
{
    while (*a != NG_EOS) {
	int s = *a++ - *b++;
	if (s)
	    return s;
    }
    return *a - *b;
}

int
wstrncmp(a, b, n)
register const NG_WCHAR_t *a;
register const NG_WCHAR_t *b;
size_t n;
{
    while (*a != NG_EOS) {
	int s;
	if (n-- < 0)
	    return 0;
	s = *a++ - *b++;
	if (s)
	    return s;
    }
    return *a - *b;
}

int
wstrncmpa(a, b, n)
register const NG_WCHAR_t *a;
const char *b;
size_t n;
{
    register const unsigned char *bp = (const unsigned char *)b;
    while (*a != NG_EOS) {
	int s;
	if (n-- < 0)
	    return 0;
	s = *a++ - *bp++;
	if (s)
	    return s;
    }
    return *a - *bp;
}

size_t
wstrlen(s)
const NG_WCHAR_t *s;
{
    register const NG_WCHAR_t *p = s;
    while (*p++ != NG_EOS)
	/*NOP*/; 
    return p - s;
}

NG_WCHAR_t *
wstrcpy(dst, src)
NG_WCHAR_t *dst;
register const NG_WCHAR_t *src;
{
    register NG_WCHAR_t *p = dst;
    while (*src != NG_EOS)
	*p++ = *src++;
    *p = NG_EOS;
    return dst;
}

NG_WCHAR_t *
wstrcat(dst, src)
NG_WCHAR_t *dst;
register const NG_WCHAR_t *src;
{
    register NG_WCHAR_t *p = dst;
    while (*p++ != NG_EOS)
	/*NOP*/;
    while (*src != NG_EOS)
	*p++ = *src++;
    *p = NG_EOS;
    return dst;
}

size_t
wstrlcpy(dst, src, n)
register NG_WCHAR_t *dst;
const NG_WCHAR_t *src;
size_t n;
{
    size_t i;
    register const NG_WCHAR_t *p = src;
    for (i=n-1; i>0 && *p!=NG_EOS; i--)
	*dst++ = *p++;
    *dst = NG_EOS;
    while (*p++ != NG_EOS)
	/*NOP*/;
    return p - src;
}

size_t
wstrlcat(dst, src, n)
NG_WCHAR_t *dst;
const NG_WCHAR_t *src;
size_t n;
{
    size_t i;
    register const NG_WCHAR_t *psrc = src;
    register const NG_WCHAR_t *pdst = dst;
    while (*pdst++ != NG_EOS)
	/*NOP*/;
    for (i=n-(pdst-dst)-1; i>0 && *src != NG_EOS; i--)
	*dst++ = *src++;
    *dst = NG_EOS;
    while (*src++ != NG_EOS)
	/*NOP*/;
    return (pdst-dst) + (psrc-src);
}

size_t
wstrlcpya(dst, src, n)
NG_WCHAR_t *dst;
const char *src;
size_t n;
{
    size_t i;
    register const char *p = src;
    for (i=n-1; i>0 && *p!='\0'; i--)
	*dst++ = NG_WCODE(*p++);
    *dst = NG_EOS;
    while (*p++ != NG_EOS)
	/*NOP*/;
    return p - src;
}

/* XXX size_t wstrlcata _PRO((NG_WCHAR_t *, const char *, size_t)); */

size_t
strlcpyw(dst, src, n)
char *dst;
const NG_WCHAR_t *src;
size_t n;
{
    size_t i;
    register const NG_WCHAR_t *p = src;
    for (i=n-1; i>0 && *p!=NG_EOS; i--) {
	if (*p > 0x7f) {
	    *dst++ = NG_WCODE('?');
	    ++p;
	}
	else
	    *dst++ = *p++ & 0x7f;
    }
    *dst = NG_EOS;
    while (*p++ != NG_EOS)
	/*NOP*/;
    return p - src;
}

/* XXX size_t strlcatw _PRO((char *, const NG_WCHAR_t *, size_t)); */

int
watoi(str)
const NG_WCHAR_t *str;
{
    size_t len;
    char *tmp;
    
    len = wstrlen(str) + 1;
    tmp = (char *)alloca(len);
    strlcpyw(tmp, str, len);
    return atoi(tmp);
}

#ifdef SUPPORT_ANSI
size_t
wsnprintf(NG_WCHAR_t *buf, size_t size, const char *format, ...)
#else
size_t
wsnprintf(buf, size, format, ...)
NG_WCHAR_t *buf;
size_t size;
const char *format;
#endif
{
    va_list va;
    char *tmp = (char *)alloca(size);
    va_start(va, format);
    vsnprintf(tmp, size, format, va);
    va_end(va);
    return wstrlcpya(buf, tmp, size);
}
