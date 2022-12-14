// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "set_insdelfind.h"
#include "set_type_feldman_hashset.h"

namespace set
{

CDSSTRESS_FeldmanHashSet_fixed_RCU(Set_InsDelFind, run_test, size_t, size_t)

} // namespace set
