namespace types
{
  class Message
  {
   public:
    Message(int chatId):
      chatId_(chatId)
    {}
   private:
    int chatId_;
  };
}
