// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/container/striped_map/std_hash_map.h>
#include "test_striped_map.h"

namespace
{
struct test_traits {
    typedef std::unordered_map<cds_test::striped_map_fixture::key_type, cds_test::striped_map_fixture::value_type,
                               cds_test::striped_map_fixture::hash2, cds_test::striped_map_fixture::equal_to>
        container_type;

    struct copy_policy {
        typedef container_type::iterator iterator;

        void operator()(container_type &m, iterator /*itInsert*/, iterator itWhat)
        {
            m.insert(std::make_pair(itWhat->first, itWhat->second));
        }
    };

    static bool const c_hasFindWith = false;
    static bool const c_hasEraseWith = false;
};

INSTANTIATE_TYPED_TEST_CASE_P(StdUnorderedMap, StripedMap, test_traits);
INSTANTIATE_TYPED_TEST_CASE_P(StdUnorderedMap, RefinableMap, test_traits);

} // namespace
