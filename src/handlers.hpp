#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <functional>
#include <string>
#include <map>
#include <unordered_map>
#include <utility>
#include "types.hpp"
#include "state.hpp"

namespace handlers
{
  class MessageHandler
  {
    using handler_t = std::function< void(const types::Message&) >;
   public:
    MessageHandler();
    void addHandler(const std::string& cmd, handler_t handler);
    void addHandler(const state::State& state, handler_t handler);
    void addHandler(const std::string& cmd, const state::State& state, handler_t handler);
    void processMessage(const types::Message& msg) const;
   private:
    std::unordered_map< std::string, handler_t > cmds_;
    std::unordered_map< state::State, handler_t > states_;
    std::unordered_map< std::pair< std::string, state::State >, handler_t > stateCmds_;
  };
}

#endif
