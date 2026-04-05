#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
    while (s[len] != '\0') {
        len++;
    }
    return len;
}

char *strcpy(char *dst, const char *src) {
  char *ret = dst;
    while ((*dst++ = *src++) != '\0');
    return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    for ( ; i < n; i++) {
        dst[i] = '\0';
    }
    return dst;
}

char *strcat(char *dst, const char *src) {
  char *ret = dst;
    while (*dst != '\0') {
        dst++;
    }
    while ((*dst++ = *src++) != '\0');
    return ret;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 != '\0' && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if (n == 0) return 0;
    while (n > 1 && *s1 != '\0' && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void *memset(void *s, int c, size_t n) {
  unsigned char *p = (unsigned char *)s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char *d = (char *)dst;
    const char *s = (const char *)src;

    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else if (d > s) {
        d += n;
        s += n;
        while (n--) {
            *(--d) = *(--s);
        }
    }
    return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *d = (char *)out;
    const char *s = (const char *)in;
    while (n--) {
        *d++ = *s++;
    }
    return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

#endif
