// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "set_delodd.h"
#include "set_type_split_list.h"

namespace set
{

CDSSTRESS_SplitListSet(Set_DelOdd_LF, run_test_extract, key_thread, size_t)
    CDSSTRESS_SplitListIterableSet(Set_DelOdd_LF, run_test_extract, key_thread, size_t)


} // namespace set
