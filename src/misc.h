///////////////////////////////////////////////////////////////////////////////
// misc.h


#ifndef _MISC_H
#  define _MISC_H

// See for more information about including windows.h
// https://learn.microsoft.com/en-gb/windows/win32/winprog/using-the-windows-headers

#define NOMINMAX

#  include <windows.h>
#  include <assert.h>

#  ifdef NDEBUG
#    define ASSERT(exp)		exp
#    define CHECK(cond, exp)	exp
#    define CHECK_TRUE(exp)	exp
#    define CHECK_FALSE(exp)	exp
#  else // NDEBUG
#    define ASSERT(exp)		assert(exp)
#    define CHECK(cond, exp)	assert(cond (exp))
#    define CHECK_TRUE(exp)	assert(!!(exp))
#    define CHECK_FALSE(exp)	assert(!(exp))
#  endif // NDEBUG

#endif // _MISC_H
