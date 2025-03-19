#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <functional>
#include <string>
#include <unordered_map>
#include "types.hpp"

namespace handlers
{
  class MessageHandler
  {
    using handler_t = std::function< void(const types::Message&) >;
   public:
    MessageHandler():
      cmds_()
    {}
    virtual void addHandler(const std::string& cmd, handler_t handler) {};
   private:
    std::unordered_map< std::string, std::function< void(const types::Message&) > > cmds_;
  };

  class Handler: public MessageHandler
  {
   public:
    Handler():
      MessageHandler()
    {}
  };
}

#endif
