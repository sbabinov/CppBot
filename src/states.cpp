#include "state.hpp"

states::StateMachine::StateMachine(Storage* storage):
  storage_(storage)
{}

states::State states::StateMachine::getState(size_t chatId)
{
  return storage_->currentStates_[chatId];
}

void states::StateMachine::setState(size_t chatId, const State& state)
{
  storage_->currentStates_[chatId] = state;
}

states::Storage::Storage():
  currentStates_(),
  data_()
{}

boost::any states::Storage::getData(size_t chatId, const states::State& state)
{
  return data_[chatId][state];
}

states::StatesForm::Data states::Storage::getData(size_t chatId)
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

void states::StateContext::setState(const states::State& state)
{
  stateMachine_->storage_->currentStates_[chatId_] = state;
}

void states::StateContext::resetState()
{
  stateMachine_->storage_->currentStates_[chatId_] = stateMachine_->defaultState_;
}

states::StatesForm::Data states::StateContext::getData()
{
  return stateMachine_->storage_->getData(chatId_);
}

boost::any states::StateContext::getData(const states::State& state)
{
  return stateMachine_->storage_->getData(chatId_, state);
}
