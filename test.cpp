#include <cstdio>
#include <cstring>

extern "C"
char * stpcpy_asm(char * dst, const char * src) noexcept;

void my_puts(const char * buffer, FILE * stream) {
  char ch;
  while (ch = *buffer) {
    if ((ch >= ' ' && ch <= '~') || ch == '\n')
      fputc(ch, stream);
    else
      fprintf(stream, "\\x%02hhX", (unsigned char)ch);
    ++buffer;
  }
}

constexpr size_t hbsasize = 2 * 129 + 1; // Double range from 0xFF to 0x79 + null character
static const char (&hbsarray)[hbsasize] = []() -> char(&)[hbsasize] {
  static char array[hbsasize];
  unsigned cur_ch = 0xFF;
  for (char & ch : (char(&)[hbsasize - 1])array) {
    ch = cur_ch--;
    if (ch == 0x7F)
      cur_ch = 0xFF;
  }
  array[hbsasize - 1] = '\0';
  return array;
}();

int main() {
  constexpr size_t buffer_size = 1024;
  constexpr size_t guard_pos = 6+8+3+1+446+17+17+17+17+16+10+21+142+(hbsasize-1)+1;
  static_assert(buffer_size > guard_pos);
  static char buffer[buffer_size];
  
  constexpr char guard_char = '\xFD';
  buffer[guard_pos] = guard_char;
  
  char * ptr = buffer;
  
  auto check_empty = [&ptr]() {
    if (ptr != buffer)
      fputs("ptr = stpcpy(ptr, \"\") failed: bad result\n", stderr);
    if (*buffer != '\0')
      fputs("ptr = stpcpy(ptr, \"\") failed: no null terminator\n", stderr);
    puts("ptr = stpcpy(ptr, \"\") succeded");
  };
  
  // Warning: While changing test strings remeber to change guard_pos too (and buffer_size when needed).
  
  ptr = stpcpy_asm(ptr, ""); check_empty();
  ptr = stpcpy_asm(ptr, "a"+1); check_empty();
  ptr = stpcpy_asm(ptr, "ab"+2); check_empty();
  ptr = stpcpy_asm(ptr, "abc"+3); check_empty();
  ptr = stpcpy_asm(ptr, "abcd"+4); check_empty();
  ptr = stpcpy_asm(ptr, "abcde"+5); check_empty();
  ptr = stpcpy_asm(ptr, "abcdef"+6); check_empty();
  ptr = stpcpy_asm(ptr, "abcdefg"+7); check_empty();
  
  ptr = stpcpy_asm(ptr, "Stpcpy");   // 6 chars
  ptr = stpcpy_asm(ptr, " testing"); // 8 chars
  ptr = stpcpy_asm(ptr, "..."); // 3 chars
  ptr = stpcpy_asm(ptr, "\n"); // 1 char
  ptr = stpcpy_asm(ptr, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\n"); // 446 chars
  
  ptr = stpcpy_asm(ptr, "1Alignment test 1\n"+1); // 17 chars
  ptr = stpcpy_asm(ptr, "12Alignment test 2\n"+2); // 17 chars
  ptr = stpcpy_asm(ptr, "123Alignment test 3\n"+3); // 17 chars
  ptr = stpcpy_asm(ptr, "1234Alignment test 4\n"+4); // 17 chars
  ptr = stpcpy_asm(ptr, "12345Alignment test 5"+5); // 16 chars
  ptr = stpcpy_asm(ptr, "123456 and... 6?"+6); // 10 chars
  ptr = stpcpy_asm(ptr, "1234567\nAlignment test 7 :D\n"+7); // 21 chars
  
  ptr = stpcpy_asm(ptr, "Previous implementation had problems with non-ascii characters (codepoint >= 0x80), so characters from 0xFF to 0x79 will be tested (2 times).\n"); // 142 chars
  ptr = stpcpy_asm(ptr, hbsarray); // (hbsasize-1) chars
  
  if (buffer[guard_pos] != guard_char)
    fputs("Guard test failed...", stderr);
  if (ptr != &buffer[guard_pos] + 1)
    fputs("Ptr to '\\0' doesn't point at last character", stderr);
  if (*ptr != '\0')
    fputs("Ptr to '\\0' doesn't point at '\\0'", stderr);
  my_puts(buffer, stdout);
  
  return 0;
}