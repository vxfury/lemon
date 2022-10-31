#include "factory.h"
#include <iostream>

// for cmake: target_link_libraries(lib_a PUBLIC -Wl,-force_load fake_4)
// for bazel: alwayslink = true

namespace sms
{
class Sender {
  public:
    virtual ~Sender() {}

    virtual int Send() = 0;
};
DEFINE_EASY_FACTORY_DISPATCHER(sms::Sender)

#define SENDER_REGISTRAR(name, Impl) DESIGN_PATTERN_FACTORY_REGISTRAR(sms::Sender, Impl, name)
} // namespace sms

class Sender1 : public sms::Sender {
  public:
    Sender1() {}
    ~Sender1() {}
    virtual int Send() override
    {
        std::cout << "Do Sender1" << std::endl;
        return 0;
    }
};
SENDER_REGISTRAR("sender1", Sender1)

class Sender2 : public sms::Sender {
  public:
    Sender2() {}
    ~Sender2() {}
    virtual int Send() override
    {
        std::cout << "Do Sender2" << std::endl;
        return 0;
    }
};
SENDER_REGISTRAR("sender2", Sender2)

int main()
{
    auto names = sms::query(".*");

    for (auto const &name : names) {
        std::cout << "  " << name << std::endl;
    }

    auto v = sms::fetch("sender1");
    v->Send();

    return 0;
}
