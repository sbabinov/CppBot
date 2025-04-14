#include "states.hpp"

states::State::State():
  id_(lastId_++)
{}
size_t states::State::lastId_ = 0;

bool states::State::operator==(const states::State& other) const
{
  return this->id_ == other.id_;
}

states::StateMachine::StateMachine(Storage* storage):
  storage_(storage)
{}

states::State states::StateMachine::getState(size_t chatId) const
{
  try
  {
    return storage_->currentStates_.at(chatId);
  }
  catch(const std::out_of_range&)
  {
    return states::StateMachine::DEFAULT_STATE;
  }
}

void states::StateMachine::setState(size_t chatId, const State& state) const
{
  storage_->currentStates_[chatId] = state;
}

const states::State states::StateMachine::DEFAULT_STATE = {};

states::Storage::Storage():
  currentStates_(),
  data_()
{}

boost::any& states::Storage::data(size_t chatId, const states::State& state)
{
  return data_[chatId][state];
}

states::StatesForm::Data& states::Storage::data(size_t chatId)
{
  return data_[chatId];
}

void states::Storage::remove(size_t chatId)
{
  data_[chatId].clear();
}

states::StateContext::StateContext(size_t chatId, states::StateMachine* stateMachine):
  chatId_(chatId),
  stateMachine_(stateMachine)
{}

states::State states::StateContext::current()
{
  return stateMachine_->getState(chatId_);
}

void states::StateContext::setState(const states::State& state) const
{
  stateMachine_->storage_->currentStates_[chatId_] = state;
}

void states::StateContext::resetState()
{
  stateMachine_->storage_->currentStates_[chatId_] = states::StateMachine::DEFAULT_STATE;
}

states::StatesForm::Data& states::StateContext::data()
{
  return stateMachine_->storage_->data(chatId_);
}

boost::any& states::StateContext::data(const states::State& state)
{
  return stateMachine_->storage_->data(chatId_, state);
}
