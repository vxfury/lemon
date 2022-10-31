
#pragma once

#include <nstd/Atomic.h>
#include <nstd/Memory.h>

template <typename T>
class LockFreeQueue {
  public:
    explicit LockFreeQueue(usize capacity)
    {
        _capacityMask = capacity - 1;
        for (usize i = 1; i <= sizeof(void *) * 4; i <<= 1) _capacityMask |= _capacityMask >> i;
        _capacity = _capacityMask + 1;

        _queue = (Node *)Memory::alloc(sizeof(Node) * _capacity);
        for (usize i = 0; i < _capacity; ++i) {
            _queue[i].tail = i;
            _queue[i].head = -1;
        }

        _tail = 0;
        _head = 0;
    }

    ~LockFreeQueue()
    {
        for (usize i = _head; i != _tail; ++i) (&_queue[i & _capacityMask].data)->~T();

        Memory::free(_queue);
    }

    usize capacity() const
    {
        return _capacity;
    }

    usize size() const
    {
        usize head = Atomic::load(_head);
        return _tail - head;
    }

    bool push(const T &data)
    {
        Node *node;
        usize next, tail = _tail;
        for (;; tail = next) {
            node = &_queue[tail & _capacityMask];
            if (Atomic::load(node->tail) != tail) return false;
            if ((next = Atomic::compareAndSwap(_tail, tail, tail + 1)) == tail) break;
        }
        new (&node->data) T(data);
        Atomic::store(node->head, tail);
        return true;
    }

    bool pop(T &result)
    {
        Node *node;
        usize next, head = _head;
        for (;; head = next) {
            node = &_queue[head & _capacityMask];
            if (Atomic::load(node->head) != head) return false;
            if ((next = Atomic::compareAndSwap(_head, head, head + 1)) == head) break;
        }
        result = node->data;
        (&node->data)->~T();
        Atomic::store(node->tail, head + _capacity);
        return true;
    }

  private:
    struct Node {
        T data;
        usize tail;
        usize head;
    };

  private:
    usize _capacityMask;
    Node *_queue;
    usize _capacity;
    char cacheLinePad1[64];
    usize _tail;
    char cacheLinePad2[64];
    usize _head;
    char cacheLinePad3[64];
};
