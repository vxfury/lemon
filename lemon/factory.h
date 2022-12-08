#pragma once

#include <regex>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace lemon
{
#define INTERNAL_UNIQUE_NAME_EXPAND(prefix, num) prefix##num
#define INTERNAL_UNIQUE_NAME_LINE(prefix, num)   INTERNAL_UNIQUE_NAME_EXPAND(prefix, num)
#define LEMON_UNIQUE_NAME(prefix)                INTERNAL_UNIQUE_NAME_LINE(prefix, __LINE__)
} // namespace lemon

namespace lemon
{
template <typename Base, typename... Args>
class factory final : private std::unordered_map<std::string, std::function<Base *(Args...)>> {
  public:
    factory(factory const &) = delete;
    factory &operator=(factory const &) = delete;

    static factory &instance()
    {
        static factory instance;
        return instance;
    }

    inline void offer(const std::string &name, std::function<Base *(Args...)> func)
    {
        this->insert({name, std::move(func)});
    }

    inline void revoke(const std::string &name)
    {
        this->erase(name);
    }

    inline Base *fetch(const std::string &name, Args... args)
    {
        auto it = this->find(name);
        return it == this->end() ? nullptr : it->second.operator()(args...);
    }

    inline bool has(const std::string &name)
    {
        return this->find(name) != this->end();
    }

    inline std::vector<std::string> query(const std::string &filter)
    {
        std::vector<std::string> names;
        auto pattern = std::regex(filter);
        for (auto it = this->begin(); it != this->end(); ++it) {
            if (!filter.empty() && !std::regex_match(it->first, pattern)) {
                continue;
            }
            names.push_back(it->first);
        }

        return names;
    }

  private:
    factory() {}
};

template <typename Derived, typename Base, typename... Args>
class registrar {
  public:
    registrar(const std::string &name)
    {
        lemon::factory<Base, Args...>::instance().offer(name, [](Args... args) -> Base * {
            return new Derived(args...);
        });
    }

    registrar(registrar const &) = delete;
    registrar &operator=(registrar const &) = delete;
};

template <typename Base, typename... Args>
inline bool has(const std::string &name)
{
    return lemon::factory<Base, Args...>::instance().has(name);
}

template <typename Base, typename... Args>
inline std::vector<std::string> query(const std::string &filter)
{
    return lemon::factory<Base, Args...>::instance().query(filter);
}

template <typename Base, typename... Args>
inline std::shared_ptr<Base> fetch(const std::string &name, Args... args)
{
    return std::shared_ptr<Base>(lemon::factory<Base, Args...>::instance().fetch(name, args...));
}

#define LEMON_DEFINE_FACTORY(Base, ...)                                       \
    inline bool has(const std::string &name)                                  \
    {                                                                         \
        return lemon::has<Base, ##__VA_ARGS__>(name);                         \
    }                                                                         \
    inline std::vector<std::string> query(const std::string &filter = ".*")   \
    {                                                                         \
        return lemon::query<Base, ##__VA_ARGS__>(filter);                     \
    }                                                                         \
    template <typename... Args>                                               \
    inline std::shared_ptr<Base> fetch(const std::string &name, Args... args) \
    {                                                                         \
        return lemon::fetch<Base, ##__VA_ARGS__>(name, args...);              \
    }

#define LEMON_FACTORY_REGISTRAR(name, Derived, Base, ...) \
    static lemon::registrar<Derived, Base, ##__VA_ARGS__> __attribute__((used)) LEMON_UNIQUE_NAME(registrar_)(name);
} // namespace lemon
