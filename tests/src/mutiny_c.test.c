#include "mu/tiny/test.h"

#include <stdbool.h>

static int setup_teardown_was_called_in_test_group_in_c = 0;
static int test_was_called_in_test_group_in_c = 0;

TEST_GROUP_SETUP(MutinyC)
{
  setup_teardown_was_called_in_test_group_in_c++;
}

TEST_GROUP_TEARDOWN(MutinyC)
{
  setup_teardown_was_called_in_test_group_in_c--;
  CHECK(test_was_called_in_test_group_in_c == 1);
  test_was_called_in_test_group_in_c--;
}

TEST(MutinyC, checkThatTheTestHasRun)
{
  test_was_called_in_test_group_in_c++;
}

SKIPPED_TEST(MutinyC, skipMacroForCFile)
{
  test_was_called_in_test_group_in_c++;
}

TEST(MutinyC, bool)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_BOOL(1, 1);
  CHECK_EQUAL_BOOL(1, 2);
}

TEST(MutinyC, boolText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_BOOL_TEXT(1, 1, "Text");
  CHECK_EQUAL_BOOL_TEXT(1, 2, "Text");
}

TEST(MutinyC, int)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_INT(2, 2);
}

TEST(MutinyC, intText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_INT_TEXT(2, 2, "Text");
}

TEST(MutinyC, unsignedInt)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_UINT(2, 2);
}

TEST(MutinyC, unsignedIntText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_UINT_TEXT(2, 2, "Text");
}

TEST(MutinyC, long)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_LONG(2, 2);
}

TEST(MutinyC, longText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_LONG_TEXT(2, 2, "Text");
}

TEST(MutinyC, ulong)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_ULONG(2, 2);
}

TEST(MutinyC, ulongText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_ULONG_TEXT(2, 2, "Text");
}

TEST(MutinyC, longLong)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_LONGLONG(2, 2);
}

TEST(MutinyC, longLongText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_LONGLONG_TEXT(2, 2, "Text");
}

TEST(MutinyC, uLongLong)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_ULONGLONG(2, 2);
}

TEST(MutinyC, uLongLongText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_ULONGLONG_TEXT(2, 2, "Text");
}

TEST(MutinyC, real)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_DOUBLE(1.0, 1.1, 0.5);
}

TEST(MutinyC, realText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_DOUBLE_TEXT(1.0, 1.1, 0.5, "Text");
}

TEST(MutinyC, char)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_CHAR('a', 'a');
}

TEST(MutinyC, charText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_CHAR_TEXT('a', 'a', "Text");
}

TEST(MutinyC, uByte)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_UBYTE(254, 254);
}

TEST(MutinyC, uByteText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_UBYTE_TEXT(254, 254, "Text");
}

TEST(MutinyC, sByte)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_SBYTE(-3, -3);
}

TEST(MutinyC, sbyteText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_SBYTE_TEXT(-3, -3, "Text");
}

TEST(MutinyC, string)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_STRING("Hello", "Hello");
}

TEST(MutinyC, stringText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_STRING_TEXT("Hello", "Hello", "Text");
}

TEST(MutinyC, pointer)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_POINTER(NULL, NULL);
}

TEST(MutinyC, pointerText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_POINTER_TEXT(NULL, NULL, "Text");
}

TEST(MutinyC, memcmp)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_MEMCMP("TEST", "TEST", 5);
}

TEST(MutinyC, memcmpText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_MEMCMP_TEXT("TEST", "TEST", 5, "Text");
}

TEST(MutinyC, genericBool)
{
  bool actual = true;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL(true, actual);
}

TEST(MutinyC, genericInt)
{
  int actual = 2;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL(2, actual);
}

TEST(MutinyC, genericIntText)
{
  int actual = 2;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_TEXT(2, actual, "Text");
}

TEST(MutinyC, genericUInt)
{
  unsigned int actual = 2;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL(2U, actual);
}

TEST(MutinyC, genericLong)
{
  long actual = 2;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL(2L, actual);
}

TEST(MutinyC, genericULong)
{
  unsigned long actual = 2;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL(2UL, actual);
}

TEST(MutinyC, genericLongLong)
{
  long long actual = 2;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL(2LL, actual);
}

TEST(MutinyC, genericULongLong)
{
  unsigned long long actual = 2;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL(2ULL, actual);
}

TEST(MutinyC, genericChar)
{
  char actual = 'a';
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL('a', actual);
}

TEST(MutinyC, genericUByte)
{
  unsigned char actual = 254;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL((unsigned char)254, actual);
}

TEST(MutinyC, genericSByte)
{
  signed char actual = -3;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL((signed char)-3, actual);
}

TEST(MutinyC, genericString)
{
  const char* actual = "Hello";
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL("Hello", actual);
}

TEST(MutinyC, genericPointer)
{
  int x = 0;
  int* actual = &x;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL(actual, actual);
}

XFAIL_TEST(MutinyC, genericIntFail)
{
  int actual = 3;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL(2, actual);
}

XFAIL_TEST(MutinyC, genericStringFail)
{
  const char* actual = "World";
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL("Hello", actual);
}

TEST(MutinyC, check)
{
  test_was_called_in_test_group_in_c++;
  CHECK(true);
}

TEST(MutinyC, checkText)
{
  test_was_called_in_test_group_in_c++;
  CHECK_TEXT(true, "Text");
}

XFAIL_TEST(MutinyC, boolFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_BOOL(1, 0);
}

XFAIL_TEST(MutinyC, boolTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_BOOL_TEXT(1, 0, "Text");
}

XFAIL_TEST(MutinyC, intFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_INT(2, 3);
}

XFAIL_TEST(MutinyC, intTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_INT_TEXT(2, 3, "Text");
}

XFAIL_TEST(MutinyC, unsignedIntFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_UINT(2, 3);
}

XFAIL_TEST(MutinyC, unsignedIntTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_UINT_TEXT(2, 3, "Text");
}

XFAIL_TEST(MutinyC, longFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_LONG(2, 3);
}

XFAIL_TEST(MutinyC, longTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_LONG_TEXT(2, 3, "Text");
}

XFAIL_TEST(MutinyC, ulongFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_ULONG(2, 3);
}

XFAIL_TEST(MutinyC, ulongTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_ULONG_TEXT(2, 3, "Text");
}

XFAIL_TEST(MutinyC, longLongFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_LONGLONG(2, 3);
}

XFAIL_TEST(MutinyC, longLongTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_LONGLONG_TEXT(2, 3, "Text");
}

XFAIL_TEST(MutinyC, uLongLongFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_ULONGLONG(2, 3);
}

XFAIL_TEST(MutinyC, uLongLongTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_ULONGLONG_TEXT(2, 3, "Text");
}

XFAIL_TEST(MutinyC, realFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_DOUBLE(1.0, 3.0, 0.5);
}

XFAIL_TEST(MutinyC, realTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_DOUBLE_TEXT(1.0, 3.0, 0.5, "Text");
}

XFAIL_TEST(MutinyC, charFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_CHAR('a', 'b');
}

XFAIL_TEST(MutinyC, charTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_CHAR_TEXT('a', 'b', "Text");
}

XFAIL_TEST(MutinyC, uByteFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_UBYTE(254, 253);
}

XFAIL_TEST(MutinyC, uByteTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_UBYTE_TEXT(254, 253, "Text");
}

XFAIL_TEST(MutinyC, sByteFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_SBYTE(-3, -4);
}

XFAIL_TEST(MutinyC, sbyteTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_SBYTE_TEXT(-3, -4, "Text");
}

XFAIL_TEST(MutinyC, stringFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_STRING("Hello", "World");
}

XFAIL_TEST(MutinyC, stringTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_STRING_TEXT("Hello", "World", "Text");
}

XFAIL_TEST(MutinyC, pointerFail)
{
  int x = 0;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_POINTER(NULL, &x);
}

XFAIL_TEST(MutinyC, pointerTextFail)
{
  int x = 0;
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_POINTER_TEXT(NULL, &x, "Text");
}

XFAIL_TEST(MutinyC, memcmpFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_MEMCMP("ABCD", "TEST", 4);
}

XFAIL_TEST(MutinyC, memcmpTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_EQUAL_MEMCMP_TEXT("ABCD", "TEST", 4, "Text");
}

XFAIL_TEST(MutinyC, failText)
{
  test_was_called_in_test_group_in_c++;
  FAIL_TEST("expected failure");
}

XFAIL_TEST(MutinyC, fail)
{
  test_was_called_in_test_group_in_c++;
  FAIL_TEST("");
}

XFAIL_TEST(MutinyC, checkFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK(false);
}

XFAIL_TEST(MutinyC, checkTextFail)
{
  test_was_called_in_test_group_in_c++;
  CHECK_TEXT(false, "Text");
}
