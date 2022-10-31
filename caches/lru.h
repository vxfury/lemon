#pragma once

#include <unordered_map>
#include <list>
#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <list>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_map>

namespace cache
{
template <typename Key, typename Value>
class LRU {
  public:
    using pair_type = typename std::pair<Key, Value>;
    using list_iterator_type = typename std::list<pair_type>::iterator;

    LRU(size_t max_size) : _max_size(max_size) {}

    void put(const Key &key, const Value &value)
    {
        auto it = _cache_items_map.find(key);
        _cache_items_list.push_front(pair_type(key, value));
        if (it != _cache_items_map.end()) {
            _cache_items_list.erase(it->second);
            _cache_items_map.erase(it);
        }
        _cache_items_map[key] = _cache_items_list.begin();

        if (_cache_items_map.size() > _max_size) {
            auto last = _cache_items_list.end();
            last--;
            _cache_items_map.erase(last->first);
            _cache_items_list.pop_back();
        }
    }

    const Value &get(const Key &key)
    {
        auto it = _cache_items_map.find(key);
        if (it == _cache_items_map.end()) {
            throw std::range_error("There is no such key in cache");
        } else {
            _cache_items_list.splice(_cache_items_list.begin(), _cache_items_list, it->second);
            return it->second->second;
        }
    }

    bool exists(const Key &key) const
    {
        return _cache_items_map.find(key) != _cache_items_map.end();
    }

    size_t size() const
    {
        return _cache_items_map.size();
    }

  private:
    size_t _max_size;
    std::list<pair_type> _cache_items_list;
    std::unordered_map<Key, list_iterator_type> _cache_items_map;
};

#include <cassert>
#include <map>
#include <list>

template <typename K, typename V>
class lru_cache_using_std {
  public:
    typedef K key_type;
    typedef V value_type;

    typedef std::list<key_type> key_tracker_type;

    // 保存Key/Value数据，以及指向访问历史顺序的迭代器
    typedef std::map<key_type, std::pair<value_type, typename key_tracker_type::iterator> > key_to_value_type;

    lru_cache_using_std(value_type (*f)(const key_type &), size_t c) : fn_(f), capacity_(c)
    {
        assert(capacity_ != 0);
    }

    // 对外提供的唯一访问接口
    value_type operator()(const key_type &k)
    {
        const typename key_to_value_type::iterator it = key_to_value_.find(k);

        if (it == key_to_value_.end()) {
            // 如果Key/Value尚不存在，则根据Key获取Value，并存入Cache后返回
            const value_type v = fn_(k);
            insert(k, v);
            return v;
        } else {
            // 如果Key/Value已经存在，调整list顺序后访问
            key_tracker_.splice(key_tracker_.end(), key_tracker_, (*it).second.second);
            return (*it).second.first;
        }
    }

  private:
    // 插入新数据
    void insert(const key_type &k, const value_type &v)
    {
        assert(key_to_value_.find(k) == key_to_value_.end());

        // 当Cache满后，淘汰老数据
        if (key_to_value_.size() == capacity_) evict();

        typename key_tracker_type::iterator it = key_tracker_.insert(key_tracker_.end(), k);

        key_to_value_.insert(std::make_pair(k, std::make_pair(v, it)));
    }

    // 淘汰老数据
    void evict()
    {
        assert(!key_tracker_.empty());

        // list开头元素是最老的数据
        const typename key_to_value_type::iterator it = key_to_value_.find(key_tracker_.front());

        assert(it != key_to_value_.end());

        // 同时在map和list中删除
        key_to_value_.erase(it);
        key_tracker_.pop_front();
    }

    // 当Cache未命中时，由Key获取Value的函数。通常会访问一个更慢速的资源来获取Value值，比如网络或磁盘。
    value_type (*fn_)(const key_type &);

    size_t capacity_;

    key_tracker_type key_tracker_;
    key_to_value_type key_to_value_;
};
} // namespace cache
