#include "mutiny/test/Filter.hpp"

namespace mu {
namespace tiny {
namespace test {

Filter::Filter()
  : strict_matching_(false)
  , invert_matching_(false)
  , next_(nullptr)
{
}

Filter::Filter(const String& filter)
  : strict_matching_(false)
  , invert_matching_(false)
  , next_(nullptr)
{
  filter_ = filter;
}

Filter::Filter(const char* filter)
  : strict_matching_(false)
  , invert_matching_(false)
  , next_(nullptr)
{
  filter_ = filter;
}

Filter* Filter::add(Filter* filter)
{
  next_ = filter;
  return this;
}

Filter* Filter::get_next() const
{
  return next_;
}

void Filter::strict_matching()
{
  strict_matching_ = true;
}

void Filter::invert_matching()
{
  invert_matching_ = true;
}

bool Filter::match(const String& name) const
{
  bool matches = false;

  if (strict_matching_)
    matches = name == filter_;
  else
    matches = string_contains(name, filter_);

  return invert_matching_ ? !matches : matches;
}

bool Filter::operator==(const Filter& filter) const
{
  return (
      filter_ == filter.filter_ &&
      strict_matching_ == filter.strict_matching_ &&
      invert_matching_ == filter.invert_matching_
  );
}

bool Filter::operator!=(const Filter& filter) const
{
  return !(filter == *this);
}

String Filter::as_string() const
{
  String text_filter = string_from_format("Filter: \"%s\"", filter_.c_str());
  if (strict_matching_ && invert_matching_)
    text_filter += MUTINY_STR(" with strict, invert matching");
  else if (strict_matching_)
    text_filter += MUTINY_STR(" with strict matching");
  else if (invert_matching_)
    text_filter += MUTINY_STR(" with invert matching");

  return text_filter;
}

String string_from(const Filter& filter)
{
  return filter.as_string();
}

} // namespace test
} // namespace tiny
} // namespace mu
