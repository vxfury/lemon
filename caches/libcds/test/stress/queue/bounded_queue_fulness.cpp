// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "queue_type.h"


/*
    Bounded queue test.
    The test checks the behaviour of bounded queue when it is almost full.
    Many algorithms says the queue is full when it is not, and vice versa.
*/
namespace
{

static size_t s_nThreadCount = 8;
static size_t s_nQueueSize = 1024;
static size_t s_nPassCount = 1000000;

class bounded_queue_fulness : public cds_test::stress_fixture {
    typedef cds_test::stress_fixture base_class;

  protected:
    template <class Queue>
    class Strain : public cds_test::thread {
        typedef cds_test::thread base_class;

      public:
        Queue &m_Queue;
        size_t m_nPushError = 0;
        size_t m_nPopError = 0;

      public:
        Strain(cds_test::thread_pool &pool, Queue &q) : base_class(pool), m_Queue(q) {}

        Strain(Strain &src) : base_class(src), m_Queue(src.m_Queue) {}

        virtual thread *clone()
        {
            return new Strain(*this);
        }

        virtual void test()
        {
            for (size_t i = 0; i < s_nPassCount; ++i) {
                if (!m_Queue.push(i)) ++m_nPushError;
                size_t item;
                if (!m_Queue.pop(item)) ++m_nPopError;
            }
        }
    };

  public:
    static void SetUpTestCase()
    {
        cds_test::config const &cfg = get_config("bounded_queue_fulness");

        s_nThreadCount = cfg.get_size_t("ThreadCount", s_nThreadCount);
        s_nQueueSize = cfg.get_size_t("QueueSize", s_nQueueSize);
        s_nPassCount = cfg.get_size_t("PassCount", s_nPassCount);

        if (s_nThreadCount == 0u) s_nThreadCount = 1;
        if (s_nQueueSize == 0u) s_nQueueSize = 1024;
        if (s_nPassCount == 0u) s_nPassCount = 1;
    }

    // static void TearDownTestCase();

  protected:
    template <class Queue>
    void analyze(Queue &q)
    {
        cds_test::thread_pool &pool = get_pool();

        size_t nPushError = 0;
        size_t nPopError = 0;
        for (size_t i = 0; i < pool.size(); ++i) {
            Strain<Queue> &strain = static_cast<Strain<Queue> &>(pool.get(i));
            nPushError += strain.m_nPushError;
            nPopError += strain.m_nPopError;
        }
        EXPECT_TRUE(!q.empty());
        EXPECT_EQ(nPushError, 0u);
        EXPECT_EQ(nPopError, 0u);
    }

    template <class Queue>
    void test(Queue &q)
    {
        cds_test::thread_pool &pool = get_pool();
        pool.add(new Strain<Queue>(pool, q), s_nThreadCount);

        size_t nSize = q.capacity() - s_nThreadCount;
        for (size_t i = 0; i < nSize; ++i) q.push(i);

        propout() << std::make_pair("thread_count", s_nThreadCount) << std::make_pair("push_count", s_nQueueSize)
                  << std::make_pair("pass_count", s_nPassCount);

        std::chrono::milliseconds duration = pool.run();
        propout() << std::make_pair("duration", duration);

        analyze(q);

        propout() << q.statistics();
    }
};

#undef CDSSTRESS_Queue_F
#define CDSSTRESS_Queue_F(test_fixture, type_name)          \
    TEST_F(test_fixture, type_name)                         \
    {                                                       \
        typedef queue::Types<size_t>::type_name queue_type; \
        queue_type queue(s_nQueueSize);                     \
        test(queue);                                        \
    }

CDSSTRESS_VyukovQueue(bounded_queue_fulness)

#undef CDSSTRESS_Queue_F

} // namespace
