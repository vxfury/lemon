// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_delodd.h"
#include "map_type_bronson_avltree.h"

namespace map
{

CDSSTRESS_BronsonAVLTreeMap(Map_DelOdd, run_test_extract, key_thread, size_t)

} // namespace map
