#ifndef STATE_HPP
#define STATE_HPP
#include <string>
#include <unordered_map>
#include <boost/any.hpp>

namespace states
{
  class State
  {};

  class StatesForm
  {
   public:
    using Data = std::unordered_map< State, boost::any >;
  };

  class Storage
  {
   public:
    friend class StateMachine;
    friend class StateContext;

    Storage();
    StatesForm::Data getData(size_t chatId);
    boost::any getData(size_t chatId, const State& state);
    void remove(size_t chatId);
   private:
    std::unordered_map< size_t, State > currentStates_;
    std::unordered_map< size_t, StatesForm::Data > data_;
  };

  class StateMachine
  {
   public:
    friend class StateContext;

    StateMachine(Storage* storage);
    void setState(size_t chatId, const State& state);
    State getState(size_t chatId);
   private:
    static State defaultState_;
    Storage* storage_;
  };

  class StateContext
  {
   public:
    StateContext(size_t chatId, StateMachine* stateMachine);
    void setState(const State& state);
    void resetState();
    StatesForm::Data getData();
    boost::any getData(const State& state);
   private:
    size_t chatId_;
    StateMachine* stateMachine_;
  };
}

namespace std
{
  template<>
  struct hash< states::State >
  {
    size_t operator()(const states::State &obj) const
    {
      return hash< const states::State* >()(&obj);
    }
  };
}

#endif
