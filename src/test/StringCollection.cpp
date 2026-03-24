#include "mutiny/test/StringCollection.hpp"

namespace mu {
namespace tiny {
namespace test {
namespace {
size_t count_substr(const String& string, const String& substr)
{
  size_t num = 0;
  const char* str = string.c_str();
  const char* strpart = nullptr;
  if (*str) {
    strpart = strstr(str, substr.c_str());
  }
  while (*str && strpart) {
    str = strpart;
    str++;
    num++;
    strpart = strstr(str, substr.c_str());
  }
  return num;
}

} // namespace

StringCollection::StringCollection(String const& str, char delimiter)
{
  split_string(str, delimiter);
}

StringCollection::~StringCollection()
{
  delete[] collection_;
}

void StringCollection::split_string(String const& string, char d)
{
  auto delimiter = string_from(d);

  size_t num = count_substr(string, delimiter);
  size_t extra_end_token = (string_ends_with(string, delimiter)) ? 0 : 1U;
  allocate(num + extra_end_token);

  const char* str = string.c_str();
  const char* prev;
  for (size_t i = 0; i < num; ++i) {
    prev = str;
    str = strstr(str, delimiter.c_str()) + 1;
    (*this)[i] = String(prev).substr(0, size_t(str - prev));
  }
  if (extra_end_token) {
    (*this)[num] = str;
  }
}

void StringCollection::allocate(size_t size)
{
  delete[] collection_;

  size_ = size;
  collection_ = new String[size_];
}

size_t StringCollection::size() const
{
  return size_;
}

String& StringCollection::operator[](size_t index)
{
  if (index >= size_) {
    empty_ = "";
    return empty_;
  }

  return collection_[index];
}

} // namespace test
} // namespace tiny
} // namespace mu
