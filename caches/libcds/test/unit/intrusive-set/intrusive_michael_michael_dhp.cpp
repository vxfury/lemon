// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_set_hp.h"

#include <cds/intrusive/michael_list_dhp.h>
#include <cds/intrusive/michael_set.h>

namespace
{
namespace ci = cds::intrusive;
typedef cds::gc::DHP gc_type;

class IntrusiveMichaelSet_DHP : public cds_test::intrusive_set_hp {
  protected:
    typedef cds_test::intrusive_set_hp base_class;

  protected:
    typedef typename base_class::base_int_item<ci::michael_list::node<gc_type>> base_item_type;
    typedef typename base_class::member_int_item<ci::michael_list::node<gc_type>> member_item_type;

    void SetUp()
    {
        struct list_traits : public ci::michael_list::traits {
            typedef ci::michael_list::base_hook<ci::opt::gc<gc_type>> hook;
        };
        typedef ci::MichaelList<gc_type, base_item_type, list_traits> list_type;
        typedef ci::MichaelHashSet<gc_type, list_type> set_type;

        cds::gc::dhp::smr::construct(set_type::c_nHazardPtrCount);
        cds::threading::Manager::attachThread();
    }

    void TearDown()
    {
        cds::threading::Manager::detachThread();
        cds::gc::dhp::smr::destruct();
    }
};


TEST_F(IntrusiveMichaelSet_DHP, base_cmp)
{
    typedef ci::MichaelList<
        gc_type, base_item_type,
        ci::michael_list::make_traits<ci::opt::hook<ci::michael_list::base_hook<ci::opt::gc<gc_type>>>,
                                      ci::opt::compare<cmp<base_item_type>>, ci::opt::disposer<mock_disposer>>::type>
        bucket_type;

    typedef ci::MichaelHashSet<gc_type, bucket_type, ci::michael_set::make_traits<ci::opt::hash<hash_int>>::type>
        set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveMichaelSet_DHP, base_less)
{
    typedef ci::MichaelList<
        gc_type, base_item_type,
        ci::michael_list::make_traits<ci::opt::hook<ci::michael_list::base_hook<ci::opt::gc<gc_type>>>,
                                      ci::opt::less<less<base_item_type>>, ci::opt::disposer<mock_disposer>>::type>
        bucket_type;

    typedef ci::MichaelHashSet<gc_type, bucket_type, ci::michael_set::make_traits<ci::opt::hash<hash_int>>::type>
        set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveMichaelSet_DHP, base_cmpmix)
{
    struct list_traits : public ci::michael_list::traits {
        typedef ci::michael_list::base_hook<ci::opt::gc<gc_type>> hook;
        typedef base_class::less<base_item_type> less;
        typedef cmp<base_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList<gc_type, base_item_type, list_traits> bucket_type;

    struct set_traits : public ci::michael_set::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
    };
    typedef ci::MichaelHashSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveMichaelSet_DHP, base_stat)
{
    struct list_traits : public ci::michael_list::traits {
        typedef ci::michael_list::base_hook<ci::opt::gc<gc_type>> hook;
        typedef base_class::less<base_item_type> less;
        typedef cmp<base_item_type> compare;
        typedef mock_disposer disposer;
        typedef ci::michael_list::stat<> stat;
    };
    typedef ci::MichaelList<gc_type, base_item_type, list_traits> bucket_type;

    struct set_traits : public ci::michael_set::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
    };
    typedef ci::MichaelHashSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
    EXPECT_GE(s.statistics().m_nInsertSuccess, 0u);
}

TEST_F(IntrusiveMichaelSet_DHP, base_wrapped_stat)
{
    struct list_traits : public ci::michael_list::traits {
        typedef ci::michael_list::base_hook<ci::opt::gc<gc_type>> hook;
        typedef base_class::less<base_item_type> less;
        typedef cmp<base_item_type> compare;
        typedef mock_disposer disposer;
        typedef ci::michael_list::wrapped_stat<> stat;
    };
    typedef ci::MichaelList<gc_type, base_item_type, list_traits> bucket_type;

    struct set_traits : public ci::michael_set::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
    };
    typedef ci::MichaelHashSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
    EXPECT_GE(s.statistics().m_nInsertSuccess, 0u);
}

TEST_F(IntrusiveMichaelSet_DHP, member_cmp)
{
    typedef ci::MichaelList<
        gc_type, member_item_type,
        ci::michael_list::make_traits<
            ci::opt::hook<ci::michael_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>>>,
            ci::opt::compare<cmp<member_item_type>>, ci::opt::disposer<mock_disposer>>::type>
        bucket_type;

    typedef ci::MichaelHashSet<gc_type, bucket_type, ci::michael_set::make_traits<ci::opt::hash<hash_int>>::type>
        set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveMichaelSet_DHP, member_less)
{
    typedef ci::MichaelList<
        gc_type, member_item_type,
        ci::michael_list::make_traits<
            ci::opt::hook<ci::michael_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>>>,
            ci::opt::less<less<member_item_type>>, ci::opt::disposer<mock_disposer>>::type>
        bucket_type;

    typedef ci::MichaelHashSet<gc_type, bucket_type, ci::michael_set::make_traits<ci::opt::hash<hash_int>>::type>
        set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveMichaelSet_DHP, member_cmpmix)
{
    struct list_traits : public ci::michael_list::traits {
        typedef ci::michael_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>> hook;
        typedef base_class::less<member_item_type> less;
        typedef cmp<member_item_type> compare;
        typedef mock_disposer disposer;
    };
    typedef ci::MichaelList<gc_type, member_item_type, list_traits> bucket_type;

    struct set_traits : public ci::michael_set::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
    };
    typedef ci::MichaelHashSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
}

TEST_F(IntrusiveMichaelSet_DHP, member_stat)
{
    struct list_traits : public ci::michael_list::traits {
        typedef ci::michael_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>> hook;
        typedef base_class::less<member_item_type> less;
        typedef cmp<member_item_type> compare;
        typedef mock_disposer disposer;
        typedef ci::michael_list::stat<> stat;
    };
    typedef ci::MichaelList<gc_type, member_item_type, list_traits> bucket_type;

    struct set_traits : public ci::michael_set::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
    };
    typedef ci::MichaelHashSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
    EXPECT_GE(s.statistics().m_nInsertSuccess, 0u);
}

TEST_F(IntrusiveMichaelSet_DHP, member_wrapped_stat)
{
    struct list_traits : public ci::michael_list::traits {
        typedef ci::michael_list::member_hook<offsetof(member_item_type, hMember), ci::opt::gc<gc_type>> hook;
        typedef base_class::less<member_item_type> less;
        typedef cmp<member_item_type> compare;
        typedef mock_disposer disposer;
        typedef ci::michael_list::wrapped_stat<> stat;
    };
    typedef ci::MichaelList<gc_type, member_item_type, list_traits> bucket_type;

    struct set_traits : public ci::michael_set::traits {
        typedef hash_int hash;
        typedef simple_item_counter item_counter;
    };
    typedef ci::MichaelHashSet<gc_type, bucket_type, set_traits> set_type;

    set_type s(kSize, 2);
    test(s);
    EXPECT_GE(s.statistics().m_nInsertSuccess, 0u);
}

} // namespace
