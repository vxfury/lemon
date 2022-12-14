// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_insdelfind.h"
#include "map_type_split_list.h"

namespace map
{

CDSSTRESS_SplitListMap_HP(Map_InsDelFind_LF, run_test, size_t, size_t)
    CDSSTRESS_SplitListIterableMap(Map_InsDelFind_LF, run_test, size_t, size_t)

} // namespace map
