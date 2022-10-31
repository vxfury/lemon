// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_set_nogc.h"

#include <cds/intrusive/lazy_list_nogc.h>
#include <cds/intrusive/split_list_nogc.h>
#include <cds/intrusive/free_list.h>

#include <mutex>

namespace
{
namespace ci = cds::intrusive;
typedef cds::gc::nogc gc_type;

class IntrusiveSplitListLazySet_NoGC : public cds_test::intrusive_set_nogc {
  protected:
    typedef cds_test::intrusive_set_nogc base_class;

  protected:
    typedef typename base_class::base_int_item<ci::split_list::node<ci::lazy_list::node<gc_type>>> base_item_type;
    typedef typename base_class::base_int_item<ci::split_list::node<ci::lazy_list::node<gc_type, std::mutex>>>
        base_mutex_item_type;
    typedef typename base_class::member_int_item<ci::split_list::node<ci::lazy_list::node<gc_type>>> member_item_type;
    typedef typename base_class::member_int_item<ci::split_list::node<ci::lazy_list::node<gc_type, std::mutex>>>
        member_mutex_item_type;

    // void SetUp()
    //{}

    // void TearDown()
    //{}
};


TEST_F(IntrusiveSplitListLazySet_NoGC, base_cmp)
{
    typedef ci::LazyList<
        gc_type, base_item_type,
        ci::lazy_list::make_traits<ci::opt::hook<ci::lazy_list::base_hook<ci::opt::gc<gc_type>>>,
                                   ci::opt::compare<cmp<base_item_type>>, ci::opt::disposer<mock_disposer>,
                                   ci::opt::back_off<cds::backoff::pause>>::type>
        bucket_type;

    typedef ci::SplitListSet<gc_type, bucket_type, ci::split_list::make_traits<ci::opt::hash<hash_int>>::type> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, base_less)
{
    typedef ci::LazyList<
        gc_type, base_item_type,
        ci::lazy_list::make_traits<ci::opt::hook<ci::lazy_list::base_hook<ci::opt::gc<gc_type>>>,
                                   ci::opt::less<less<base_item_type>>, ci::opt::disposer<mock_disposer>>::type>
        bucket_type;

    typedef ci::SplitListSet<
        gc_type, bucket_type,
        ci::split_list::make_traits<ci::opt::hash<hash_int>, ci::opt::item_counter<cds::atomicity::item_counter>>::type>
        set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, base_cmpmix)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::base_hook<ci::opt::gc<gc_type>> hook;
        typedef base_class::less<base_item_type> less;
        typedef cmp<base_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, base_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
        typedef ci::split_list::stat<> stat;
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, base_mutex)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::base_hook<ci::opt::gc<gc_type>, ci::opt::lock_type<std::mutex>> hook;
        typedef base_class::less<base_mutex_item_type> less;
        typedef cmp<base_mutex_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, base_mutex_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
        typedef cds::backoff::empty back_off;
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, base_static_bucket_table)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::base_hook<ci::opt::gc<gc_type>> hook;
        typedef base_class::less<base_item_type> less;
        typedef cmp<base_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, base_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
        typedef ci::split_list::stat<> stat;
        enum { dynamic_bucket_table = false };
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, base_static_bucket_table_free_list)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::base_hook<ci::opt::gc<gc_type>> hook;
        typedef base_class::less<base_item_type> less;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, base_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
        typedef ci::split_list::stat<> stat;
        enum { dynamic_bucket_table = false };
        typedef ci::FreeList free_list;
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, base_free_list)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::base_hook<ci::opt::gc<gc_type>> hook;
        typedef cmp<base_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, base_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
        typedef ci::split_list::stat<> stat;
        typedef ci::FreeList free_list;
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, member_cmp)
{
    typedef ci::LazyList<
        gc_type, member_item_type,
        ci::lazy_list::make_traits<
            ci::opt::hook<ci::lazy_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>>>,
            ci::opt::compare<cmp<member_item_type>>, ci::opt::disposer<mock_disposer>>::type>
        bucket_type;

    typedef ci::SplitListSet<gc_type, bucket_type, ci::split_list::make_traits<ci::opt::hash<hash_int>>::type> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, member_less)
{
    typedef ci::LazyList<
        gc_type, member_item_type,
        ci::lazy_list::make_traits<
            ci::opt::hook<ci::lazy_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>>>,
            ci::opt::less<less<member_item_type>>, ci::opt::disposer<mock_disposer>>::type>
        bucket_type;

    typedef ci::SplitListSet<
        gc_type, bucket_type,
        ci::split_list::make_traits<ci::opt::hash<hash_int>, ci::opt::back_off<cds::backoff::pause>>::type>
        set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, member_cmpmix)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>> hook;
        typedef base_class::less<member_item_type> less;
        typedef cmp<member_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, member_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, member_mutex)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::member_hook<offsetof(member_mutex_item_type, hMember), ci::opt::gc<gc_type>,
                                           ci::opt::lock_type<std::mutex>>
            hook;
        typedef base_class::less<member_mutex_item_type> less;
        typedef cmp<member_mutex_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, member_mutex_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, member_static_bucket_table)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>> hook;
        typedef base_class::less<member_item_type> less;
        typedef cmp<member_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, member_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
        enum { dynamic_bucket_table = false };
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, member_static_bucket_table_free_list)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>> hook;
        typedef base_class::less<member_item_type> less;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, member_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
        enum { dynamic_bucket_table = false };
        typedef ci::FreeList free_list;
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveSplitListLazySet_NoGC, member_free_list)
{
    struct list_traits : public ci::lazy_list::traits {
        typedef ci::lazy_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>> hook;
        typedef cmp<member_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::LazyList<gc_type, member_item_type, list_traits> bucket_type;

    struct set_traits : public ci::split_list::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
        typedef ci::FreeList free_list;
    };
    typedef ci::SplitListSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

} // namespace
