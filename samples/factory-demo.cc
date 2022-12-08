#include "factory.h"
#include <iostream>

namespace protocol
{
class Sender {
  public:
    Sender(int argc, char **argv) {}
    virtual ~Sender() {}

    virtual int Send() = 0;
};
LEMON_DEFINE_FACTORY(protocol::Sender, int, char **)
#define SENDER_REGISTRAR(name, Impl) LEMON_FACTORY_REGISTRAR(name, Impl, protocol::Sender, int, char **)
} // namespace protocol

class Sender1 : public protocol::Sender {
  public:
    Sender1(int argc, char **argv) : protocol::Sender(argc, argv) {}
    ~Sender1() {}
    virtual int Send() override
    {
        std::cout << "Do Sender1" << std::endl;
        return 0;
    }
};
SENDER_REGISTRAR("sender1", Sender1)

class Sender2 : public protocol::Sender {
  public:
    Sender2(int argc, char **argv) : protocol::Sender(argc, argv) {}
    ~Sender2() {}
    virtual int Send() override
    {
        std::cout << "Do Sender2" << std::endl;
        return 0;
    }
};
SENDER_REGISTRAR("sender2", Sender2)

int main(int argc, char **argv)
{
    std::cout << "List Of Senders:" << std::endl;
    for (auto const &name : protocol::query(".*")) {
        std::cout << "  " << name << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Dispatch:" << std::endl;
    if (auto v = protocol::fetch("sender1", argc, argv); v.get() != nullptr) {
        v->Send();
    }
    std::cout << std::endl;

    return 0;
}
