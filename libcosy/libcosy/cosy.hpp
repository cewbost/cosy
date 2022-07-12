#pragma once

#include <iosfwd>
#include <string>

#include <libcosy/export.hpp>

namespace cosy
{
  // Print a greeting for the specified name into the specified
  // stream. Throw std::invalid_argument if the name is empty.
  //
  LIBCOSY_SYMEXPORT void
  say_hello (std::ostream&, const std::string& name);
}
