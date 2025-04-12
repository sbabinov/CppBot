#include "handlers.hpp"
#include <string>
#include <utility>

std::string fetchCommand(const types::Message& msg)
{
  std::string cmd;
  for (size_t i = 0; (i < msg.text.size()) && (msg.text[i] != ' '); ++i)
  {
    cmd += msg.text[i];
  }
  return std::move(cmd);
}

void handlers::MessageHandler::addHandler(const std::string& cmd, handler_t handler)
{
  cmdHandlers_[cmd] = handler;
}

void handlers::MessageHandler::addHandler(const states::State& state, state_handler_t handler)
{
  stateHandlers_[state] = handler;
}

void handlers::MessageHandler::addHandler(const std::string& cmd, const states::State& state, state_handler_t handler)
{
  stateCmdHandlers_[{cmd, state}] = handler;
}

void handlers::MessageHandler::processMessage(const types::Message& msg, states::StateContext& state) const
{
  std::string cmd = fetchCommand(msg);
  states::State currentState = state.current();
  bool isHandlerExists = false;
  if (currentState == states::DEFAULT_STATE)
  {
    cmdHandlers_.at(cmd)(msg);
    return;
  }
  try
  {
    const handlers::MessageHandler::state_handler_t& handler = stateCmdHandlers_.at({cmd, currentState});
    isHandlerExists = true;
    handler(msg, state);
  }
  catch(const std::out_of_range&)
  {
    if (!isHandlerExists)
    {
      stateHandlers_.at(currentState)(msg, state);
    }
  }
}
