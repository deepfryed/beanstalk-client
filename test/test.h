#pragma once

#include <cxxabi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>

#define TEST(suite, test) const char* _t_##suite##_##test(void)

static char _test_error[1024];
static int _test_failures = 0;

#define TEST_UNIT_ANSI_COLOR_PASS   (isatty(fileno(stdout)) ? "\033[0;32;49m" : "")
#define TEST_UNIT_ANSI_COLOR_FAIL   (isatty(fileno(stdout)) ? "\033[0;31;49m" : "")
#define TEST_UNIT_ANSI_COLOR_RESET  (isatty(fileno(stdout)) ? "\033[0m"       : "")

#define RUN(suite, test) \
  do { \
    const char *error = _t_##suite##_##test(); \
    if (error) { \
      fprintf(stderr, "%s[FAIL]%s %s %s\n%s", TEST_UNIT_ANSI_COLOR_FAIL, TEST_UNIT_ANSI_COLOR_RESET, #suite, #test, error); \
      _test_failures++; \
    } \
    else { \
      fprintf(stderr, "%s[PASS]%s %s %s\n", TEST_UNIT_ANSI_COLOR_PASS, TEST_UNIT_ANSI_COLOR_RESET, #suite, #test) ; \
    } \
  } while (0)

#define EXPECT_EQ(a, b, m) \
  do { \
    if ((a) != (b)) { \
      snprintf(_test_error, sizeof(_test_error), "  %s\n  [%s:%d] %s != %s\n", m, __FILE__, __LINE__, #a, #b); \
      return _test_error; \
    } \
  } while (0)

#define EXPECT_GT(a, b, m) \
  do { \
    if ((a) <= (b)) { \
      snprintf(_test_error, sizeof(_test_error), "  %s\n  [%s:%d] %s <= %s\n", m, __FILE__, __LINE__, #a, #b); \
      return _test_error; \
    } \
  } while (0)

#define EXPECT_LT(a, b, m) \
  do { \
    if ((a) >= (b)) { \
      snprintf(_test_error, sizeof(_test_error), "  %s\n  [%s:%d] %s >= %s\n", m, __FILE__, __LINE__, #a, #b); \
      return _test_error; \
    } \
  } while (0)

#define ASSERT_TRUE(a)      EXPECT_EQ(a, true, "")
#define ASSERT_FALSE(a)     EXPECT_EQ(a, false, "")
#define ASSERT_NO_THROW(a)  \
  do { \
    try { \
      a; \
    } \
    catch (std::exception const& got) { \
      int status; \
      char *klass = abi::__cxa_demangle(typeid(got).name(), 0, 0, &status); \
      snprintf(_test_error, sizeof(_test_error), \
        "  [%s:%d] %s raised exception ‘%s’\n", __FILE__, __LINE__, #a, klass ? klass : got.what()); \
      if (klass) free(klass); \
      return _test_error; \
    } \
  } while (0)

#define ASSERT_THROW(a, e)  \
  do { \
    bool raised = false; \
    try { \
      a; \
    } \
    catch (e) { \
      raised = true; \
    } \
    catch (std::exception const& got) { \
      int status; \
      char *klass = abi::__cxa_demangle(typeid(got).name(), 0, 0, &status); \
      snprintf(_test_error, sizeof(_test_error), \
        "  [%s:%d] %s did not raise exception ‘%s’ but raised ‘%s’\n", __FILE__, __LINE__, #a, #e, klass ? klass : got.what()); \
      if (klass) free(klass); \
      return _test_error; \
    } \
    if (!raised) { \
      snprintf(_test_error, sizeof(_test_error), "  [%s:%d] %s did not raise an exception\n", __FILE__, __LINE__, #a); \
      return _test_error; \
    } \
  } while (0)
