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
  public:
    MessageHandler():
      cmds_()
    {}
    void addHandler(const std::string& cmd, std::function< void(const types::Message&) > handler);
  private:
    std::unordered_map< std::string, std::function< void(const types::Message&) > > cmds_;
  };
}

#endif
