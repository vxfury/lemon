// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_COMPILER_GCC_PPC64_BACKOFF_H
#define CDSLIB_COMPILER_GCC_PPC64_BACKOFF_H

//@cond none

namespace cds
{
namespace backoff
{
namespace gcc
{
namespace ppc64
{

#define CDS_backoff_hint_defined
static inline void backoff_hint()
{
    // Provide a hint that performance will probably be improved
    // if shared resources dedicated to the executing processor are released for use by other processors
    asm volatile("or 27,27,27   # yield");
}
} // namespace ppc64
} // namespace gcc

namespace platform
{
using namespace gcc::ppc64;
}
} // namespace backoff
} // namespace cds

//@endcond
#endif // #ifndef CDSLIB_COMPILER_GCC_PPC64_BACKOFF_H
