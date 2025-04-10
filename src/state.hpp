#ifndef STATE_HPP
#define STATE_HPP
#include <string>
#include <unordered_map>
#include <boost/any.hpp>

namespace state
{
  class Data
  {};

  class StatesForm
  {
    Data data;
  };

  class State
  {
  };

  class StateContext
  {
   public:
    Data data;
    StateContext(size_t chatId):
      chatId_(chatId)
    {}
    void setState(const State& state);
   private:
    size_t chatId_;
  };
}

#endif
