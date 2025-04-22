/*!
  @file
  @brief Header contains classes for managing states.
  @author sbabinov92
  @version 1.0
  @date April 2025
  @warning The project is still in development
*/

#ifndef CPPBOT_STATES_HPP
#define CPPBOT_STATES_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <boost/any.hpp>

namespace states
{
  /*!
    @brief Class for creating your states.
  */
  class State
  {
   public:
    friend struct std::hash< State >;

    State();
    bool operator==(const State& other) const;
    bool operator!=(const State& other) const;
   private:
    size_t id_;
    static size_t lastId_;
  };

  /*!
    @brief Class for creating your state forms.
  */
  class StatesForm
  {
   public:
    using Data = std::unordered_map< State, boost::any >;
  };

  /*!
    @brief Class storing states data.
  */
  class Storage
  {
   public:
    friend class StateMachine;
    friend class StateContext;

    Storage();

    /*!
      @brief Method allows to get and change current states data.
      @param chatId Chat id
      @return Hash map where key is a state object, value is boost::any
    */
    StatesForm::Data& data(size_t chatId);

    /*!
      @brief Method allows to get and change data of certain state.
      @param chatId Chat id
      @param state State for gain access to it's data
      @return boost::any
    */
    boost::any& data(size_t chatId, const State& state);

    /*!
      @brief Method allows to delete all states data of certain user.
      @param chatId Chat id
    */
    void remove(size_t chatId);
   private:
    std::unordered_map< size_t, State > currentStates_;
    std::unordered_map< size_t, StatesForm::Data > data_;
  };

  /*!
    @brief Class for managing states.
  */
  class StateMachine
  {
   public:
    friend class StateContext;

    static const State DEFAULT_STATE;

    /*!
      @param storage Shared pointer to Storage object
    */
    StateMachine(std::shared_ptr< Storage > storage);

    /*!
      @brief Method allows to set a certain state for user.
      @param chatId Chat id
      @param state State to set
    */
    void setState(size_t chatId, const State& state) const;

    /*!
      @brief Method allows to get current state of user
      @param chatId Chat id
      @return User current state
    */
    State getState(size_t chatId) const;
   private:
    std::shared_ptr< Storage > storage_;
  };

  /*!
    @brief Class allows client to interact with states of certain user flexibly.
  */
  class StateContext
  {
   public:
    /*!
      @param chatId Chat id
      @param stateMachine Pointer to a StateMachine object
    */
    StateContext(size_t chatId, StateMachine* stateMachine);

    /*!
      @brief Method allows to get current user state.
      @param chatId Chat id
      @return Current user state
    */
    State current();

    /*!
      @brief Method allows to set certain state for user
      @param state State to set
    */
    void setState(const State& state) const;

    /// @example states_using.cpp

    /*!
      @brief Method allows to reset current user state to default.
    */
    void resetState();

    /*!
      @brief Method allows to get and change current states data.
      @return Hash map where key is a state object, value is boost::any
    */
    StatesForm::Data& data();

    /*!
      @brief Method allows to get and change data of certain state.
      @param state State for gain access to it's data
      @return boost::any
    */
    boost::any& data(const State& state);
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
    size_t operator()(const states::State& obj) const
    {
      return obj.id_;
    }
  };
}

#endif
