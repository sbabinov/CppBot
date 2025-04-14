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

void handlers::MessageHandler::addHandler(const states::State& state, handler_t handler)
{
  stateHandlers_[state] = handler;
}

void handlers::MessageHandler::addHandler(const std::string& cmd, const states::State& state, handler_t handler)
{
  stateCmdHandlers_[{cmd, state}] = handler;
}

void handlers::MessageHandler::processMessage(const types::Message& msg, states::StateContext& state) const
{
  std::string cmd = fetchCommand(msg);
  states::State currentState = state.current();
  bool isHandlerExists = false;
  if (currentState == states::StateMachine::DEFAULT_STATE)
  {
    cmdHandlers_.at(cmd)(msg, state);
    return;
  }
  try
  {
    const handlers::MessageHandler::handler_t& handler = stateCmdHandlers_.at({cmd, currentState});
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

void handlers::CallbackQueryHandler::addHandler(const std::string& callData, handler_t handler, bool allowPartialMatch)
{
  if (!allowPartialMatch)
  {
    handlers_[callData] = handler;
  }
  else
  {
    partialMatchHandlers_[callData] = handler;
  }
}

void handlers::CallbackQueryHandler::processCallbackQuery(const types::CallbackQuery& query) const
{
  try
  {
    handlers_.at(query.data)(query);
  }
  catch (const std::out_of_range&)
  {
    auto it = partialMatchHandlers_.cbegin();
    while (query.data[0] <= (*it).first[0])
    {
      if (((*it).first.size() >= query.data.size()) && ((*it).first.find(query.data) == 0))
      {
        (*it).second(query);
      }
    }
  }
}
