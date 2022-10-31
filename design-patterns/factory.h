#pragma once

#include <regex>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace design_pattern
{
#define INTERNAL_UNIQUE_NAME_EXPAND(prefix, num) prefix##num
#define INTERNAL_UNIQUE_NAME(prefix, num)        INTERNAL_UNIQUE_NAME_EXPAND(prefix, num)
#define DESIGN_PATTERN_UNIQUE_NAME(prefix)       INTERNAL_UNIQUE_NAME(prefix, __COUNTER__)
} // namespace design_pattern

namespace design_pattern
{
template <typename T>
class factory : private std::unordered_map<std::string, std::function<T *(void)>> {
  public:
    factory(factory const &) = delete;
    factory &operator=(factory const &) = delete;

    static factory *instance()
    {
        static factory instance;
        return &instance;
    }

    void offer(const std::string &name, std::function<T *(void)> func)
    {
        this->operator[](name) = std::move(func);
    }

    T *fetch(const std::string &name)
    {
        auto it = this->find(name);
        return it == this->end() ? nullptr : it->second.operator()();
    }

    bool has(const std::string &name)
    {
        return this->find(name) != this->end();
    }

    std::vector<std::string> query(const std::string &filter)
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

template <typename T, typename V>
class registrar {
  public:
    registrar(const std::string &name)
    {
        factory<T>::instance()->offer(name, [](void) -> T * {
            return new V();
        });
    }

    template <typename... Args>
    registrar(const std::string &name, Args... args)
    {
        factory<T>::instance()->offer(name, [=](void) -> T * {
            return new V(args...);
        });
    }
};

#define DESIGN_PATTERN_FACTORY_REGISTRAR(T, V, name, ...) \
    static design_pattern::registrar<T, V> DESIGN_PATTERN_UNIQUE_NAME(registrar_)(name, ##__VA_ARGS__);

template <typename T>
static bool has(const std::string &name)
{
    return design_pattern::factory<T>::instance()->has(name);
}

template <typename T>
static std::vector<std::string> query(const std::string &filter = ".*")
{
    return design_pattern::factory<T>::instance()->query(filter);
}

template <typename T>
static inline std::shared_ptr<T> fetch(const std::string &name)
{
    return std::shared_ptr<T>(design_pattern::factory<T>::instance()->fetch(name));
}

#define DEFINE_EASY_FACTORY_DISPATCHER(T)                                   \
    static bool has(const std::string &name)                                \
    {                                                                       \
        return design_pattern::has<T>(name);                                \
    }                                                                       \
                                                                            \
    static std::vector<std::string> query(const std::string &filter = ".*") \
    {                                                                       \
        return design_pattern::query<T>(filter);                            \
    }                                                                       \
                                                                            \
    static inline std::shared_ptr<T> fetch(const std::string &name)         \
    {                                                                       \
        return design_pattern::fetch<T>(name);                              \
    }

} // namespace design_pattern
