#ifndef TYPES_HPP
#define TYPES_HPP

#include <nlohmann/json.hpp>

namespace types
{
  class Message
  {
   public:
    explicit Message(nlohmann::json data):
      chatId_(0)
    {}
   private:
    int chatId_;
  };
}

#endif
