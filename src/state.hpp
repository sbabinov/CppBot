#ifndef STATE_HPP
#define STATE_HPP
#include <unordered_map>
#include <boost/any.hpp>

namespace state
{
  struct State
  {
    std::unordered_map< std::string, boost::any > data;
  };
}

#endif
