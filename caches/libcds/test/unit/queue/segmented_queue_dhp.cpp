// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_segmented_queue.h"

#include <cds/gc/dhp.h>
#include <cds/container/segmented_queue.h>

namespace
{
namespace cc = cds::container;
typedef cds::gc::DHP gc_type;


class SegmentedQueue_DHP : public cds_test::segmented_queue {
  protected:
    static const size_t c_QuasiFactor = 15;
    void SetUp()
    {
        typedef cc::SegmentedQueue<gc_type, int> queue_type;

        cds::gc::dhp::smr::construct(queue_type::c_nHazardPtrCount);
        cds::threading::Manager::attachThread();
    }

    void TearDown()
    {
        cds::threading::Manager::detachThread();
        cds::gc::dhp::smr::destruct();
    }
};

TEST_F(SegmentedQueue_DHP, defaulted)
{
    typedef cds::container::SegmentedQueue<gc_type, int> test_queue;

    test_queue q(c_QuasiFactor);
    ASSERT_EQ(q.quasi_factor(), cds::beans::ceil2(c_QuasiFactor));
    test(q);
}

TEST_F(SegmentedQueue_DHP, mutex)
{
    struct traits : public cds::container::segmented_queue::traits {
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::opt::v::random_shuffle_permutation<> permutation_generator;
    };
    typedef cds::container::SegmentedQueue<gc_type, int, traits> test_queue;

    test_queue q(c_QuasiFactor);
    ASSERT_EQ(q.quasi_factor(), cds::beans::ceil2(c_QuasiFactor));
    test(q);
}

TEST_F(SegmentedQueue_DHP, shuffle)
{
    struct traits : public cds::container::segmented_queue::traits {
        typedef cds::atomicity::item_counter item_counter;
        typedef cds::opt::v::random_shuffle_permutation<> permutation_generator;
    };
    typedef cds::container::SegmentedQueue<gc_type, int, traits> test_queue;

    test_queue q(c_QuasiFactor);
    ASSERT_EQ(q.quasi_factor(), cds::beans::ceil2(c_QuasiFactor));
    test(q);
}

TEST_F(SegmentedQueue_DHP, stat)
{
    struct traits : public cds::container::segmented_queue::make_traits<
                        cds::opt::item_counter<cds::atomicity::item_counter>,
                        cds::opt::permutation_generator<cds::opt::v::random_permutation<> >,
                        cds::opt::stat<cds::container::segmented_queue::stat<> > >::type {};
    typedef cds::container::SegmentedQueue<gc_type, int, traits> test_queue;

    test_queue q(c_QuasiFactor);
    ASSERT_EQ(q.quasi_factor(), cds::beans::ceil2(c_QuasiFactor));
    test(q);
}

TEST_F(SegmentedQueue_DHP, move)
{
    typedef cds::container::SegmentedQueue<gc_type, std::string> test_queue;

    test_queue q(c_QuasiFactor);
    ASSERT_EQ(q.quasi_factor(), cds::beans::ceil2(c_QuasiFactor));
    test_string(q);
}

TEST_F(SegmentedQueue_DHP, move_item_counting)
{
    struct traits : public cds::container::segmented_queue::traits {
        typedef cds::atomicity::item_counter item_counter;
    };
    typedef cds::container::SegmentedQueue<gc_type, std::string, traits> test_queue;

    test_queue q(c_QuasiFactor);
    ASSERT_EQ(q.quasi_factor(), cds::beans::ceil2(c_QuasiFactor));
    test_string(q);
}

} // namespace
