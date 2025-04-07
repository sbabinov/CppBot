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
    MessageHandler();
    void addHandler(const std::string& cmd, handler_t handler);
    void processMessage(const types::Message& msg) const;
   private:
    std::unordered_map< std::string, std::function< void(const types::Message&) > > cmds_;
  };
}

#endif
