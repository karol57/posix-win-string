#include <cstdint>
#include <cstring>

char * stpcpy_bymem(char * dst, const char * src) noexcept {
  const size_t len = strlen (src);
  return (char *) memcpy(dst, src, len + 1) + len;
}

char * stpcpy_naive(char *dest, const char *src) noexcept {
  register char *d = dest;
  register const char *s = src;
  
  do
    *d++ = *s;
  while (*s++ != '\0');
  
  return d - 1;
}