/*!
  @file
  @brief Heaader contains handler classes using in bot.
  @author sbabinov92
  @version 1.0
  @date April 2025
  @warning The project is still in development
*/

#ifndef CPPBOT_HANDLERS_HPP
#define CPPBOT_HANDLERS_HPP

#include <functional>
#include <string>
#include <map>
#include <unordered_map>
#include <utility>
#include "types.hpp"
#include "states.hpp"

namespace handlers
{
  namespace detail
  {
    struct PairHasher
    {
      size_t operator()(const std::pair< std::string, states::State >& p) const
      {
        size_t h1 = std::hash< std::string >{}(p.first);
        size_t h2 = std::hash< states::State >{}(p.second);
        return h1 ^ (h2 << 1);
      }
    };
  }

  /*!
    @brief Handler class for processing text messages.
  */
  class MessageHandler
  {
    using handler_t = std::function< void(const types::Message&) >;
    using state_handler_t = std::function< void(const types::Message&, states::StateContext&) >;
   public:
    MessageHandler() = default;

    /*!
      @brief Method for adding a new handler of some command
      @param cmd Command
      @param handler Handler for message
    */
    void addHandler(const std::string& cmd, handler_t handler);

    /*!
      @brief Method for adding a new handler of messages in some state
      @param state State
      @param handler Handler for message
    */
    void addHandler(const states::State& state, state_handler_t handler);

    /*!
      @brief Method for adding a new handler of some command in some state
      @param cmd Command
      @param state State
      @param handler Handler for message
    */
    void addHandler(const std::string& cmd, const states::State& state, state_handler_t handler);

    void processMessage(const types::Message& msg, states::StateContext& state) const;
   private:
    std::unordered_map< std::string, handler_t > cmdHandlers_;
    std::unordered_map< states::State, state_handler_t > stateHandlers_;
    std::unordered_map< std::pair< std::string, states::State >, state_handler_t, detail::PairHasher > stateCmdHandlers_;
  };

  /*!
    @brief Handler class for processing callback queries.
  */
  class CallbackQueryHandler
  {
    using handler_t = std::function< void(const types::CallbackQuery&) >;
   public:
    CallbackQueryHandler() = default;

    /*!
      @brief Method for adding a new handler of callback query
      @param callData Data of button that was pressed
      @param handler Handler for query
      @param allowPartialMatch If pressed button data starts with {callData} and this param is true, handler will be called
    */
    void addHandler(const std::string& callData, handler_t handler, bool allowPartialMatch = false);

    void processCallbackQuery(const types::CallbackQuery& query) const;
   private:
    std::unordered_map< std::string, handler_t > handlers_;
    std::map< std::string, handler_t > partialMatchHandlers_;
  };
}

#endif
