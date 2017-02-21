// Warning: This test will allocate 2 * (test_count * strlen(lorem_ipsum) + 1)
//          bytes. For default values that is 2 * (10858063 * 445 + 1) bytes
//          ~= 9.0 GiB

// Warning: This test uses std::chrono::steady_clock, because on windows
//          platform steady_clock is more accurate than
//          high_resolution_clock

#include <cstdio>
#include <random>
#include <chrono>
#include <memory>
#include <cstring>

extern char * stpcpy_bymem(char * dst, const char * src) noexcept;

extern "C"
char * stpcpy_asm(char * dst, const char * src) noexcept;

extern
char * stpcpy_naive(char *dest, const char *src) noexcept;

static
void test_gnu(char * dest, const char * src, size_t count) {
  fprintf(stdout, "Testing stpcpy by strlen+memove (GNU libiberty libc) %zu times... ", count);
  const auto start_gnu = std::chrono::steady_clock::now();
  
  for (size_t i = 0; i < count; ++i)
    dest = stpcpy_bymem(dest, src);
  
  const auto stop_gnu = std::chrono::steady_clock::now();
  const std::chrono::duration<double> gnu_time = stop_gnu-start_gnu;
  fprintf(stdout, "Done %0.2f s\n", gnu_time.count());
}

static
void test_naive(char * dest, const char * src, size_t count) {
  fprintf(stdout, "Testing stpcpy naive way %zu times... ", count);
  const auto start_gnu = std::chrono::steady_clock::now();
  
  for (size_t i = 0; i < count; ++i)
    dest = stpcpy_naive(dest, src);
  
  const auto stop_gnu = std::chrono::steady_clock::now();
  const std::chrono::duration<double> gnu_time = stop_gnu-start_gnu;
  fprintf(stdout, "Done %0.2f s\n", gnu_time.count());
}

static
void test_asm(char * dest, const char * src, size_t count) {
  fprintf(stdout, "Testing stpcpy written in asm %zu times... ", count);
  const auto start_my = std::chrono::steady_clock::now();
  
  for (size_t i = 0; i < count; ++i)
    dest = stpcpy_asm(dest, src);
  
  const auto stop_my = std::chrono::steady_clock::now();
  const std::chrono::duration<double> my_time = stop_my-start_my;
  fprintf(stdout, "Done %0.2f s\n", my_time.count());
}

int main() {
  static constexpr char lorem_ipsum[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
  static constexpr size_t test_count = 10'858'063ull;
  static constexpr size_t out_len = test_count * strlen(lorem_ipsum);
  static constexpr size_t out_size = out_len + 1;
  
  ::std::unique_ptr<char[]> dest{ new char[out_size] };
  char * dest_array = dest.get();
  
  memset(dest_array, 'A', out_size);
  test_gnu(dest_array, lorem_ipsum, test_count);
  if (strncmp(dest_array, lorem_ipsum, strlen(lorem_ipsum)) != 0)
    puts("test_gnu: failed (bad copy)");
  if (strlen(dest_array) != out_len)
    puts("test_gnu: failed (bad length)");
  
  memset(dest_array, 'B', out_size);
  test_naive(dest_array, lorem_ipsum, test_count);
  if (strncmp(dest_array, lorem_ipsum, strlen(lorem_ipsum)) != 0)
    puts("test_naive: failed (bad copy)");
  if (strlen(dest_array) != out_len)
    puts("test_naive: failed (bad length)");
  
  memset(dest_array, 'C', out_size);
  test_asm(dest_array, lorem_ipsum, test_count);
  if (strncmp(dest_array, lorem_ipsum, strlen(lorem_ipsum)) != 0)
    puts("test_asm: failed (bad copy)");
  if (strlen(dest_array) != out_len)
    puts("test_asm: failed (bad length)");
  
  ::std::unique_ptr<char[]> dest2{ new char[out_size] };
  char * dest2_array = dest2.get();

  memset(dest2_array, 'D', out_size);
  test_gnu(dest2_array, dest_array, 1);
  if (strcmp(dest2_array, dest_array) != 0)
    puts("test_gnu_single: failed");
  
  memset(dest2_array, 'E', out_size);
  test_naive(dest2_array, dest_array, 1);
  if (strcmp(dest2_array, dest_array) != 0)
    puts("test_naive_single: failed");
  
  memset(dest2_array, 'F', out_size);
  test_asm(dest2_array, dest_array, 1);
  if (strcmp(dest2_array, dest_array) != 0)
    puts("test_asm_single: failed");
  
  return 0;
}