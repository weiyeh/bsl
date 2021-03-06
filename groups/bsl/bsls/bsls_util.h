// bsls_util.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLS_UTIL
#define INCLUDED_BSLS_UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide essential, low-level support for portable generic code.
//
//@CLASSES:
//  bsls::Util: utility class supplying essential, low-level functionality
//
//@MACROS:
//  BSLS_UTIL_ADDRESSOF(OBJ): address of 'OBJ', even if 'operator&' overloaded
//
//@DESCRIPTION: This component defines a utility 'struct', 'bsls::Util', that
// serves as a namespace for a suite of pure functions that supply essential
// low-level support for implementing portable generic facilities such as might
// be found in the C++ standard library.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Obtain the address of a 'class' that defines 'operator&'.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There are times, especially within low-level library functions, where it is
// necessary to obtain the address of an object even if that object's class
// overloads 'operator&' to return something other than the object's address.
//
// First, we create a special reference-like type that can refer to a single
// bit within a byte (inline implementations are provided in class scope for
// ease of exposition):
//..
//  class BitReference {
//
//      // DATA
//      char *d_byte_p;
//      int   d_bitpos;
//
//    public:
//      // CREATORS
//      BitReference(char *byteptr = 0, int bitpos = 0)
//      : d_byte_p(byteptr)
//      , d_bitpos(bitpos)
//      {
//      }
//
//      // ACCESSORS
//      operator bool() const { return (*d_byte_p >> d_bitpos) & 1; }
//
//      char *byteptr() const { return d_byte_p; }
//      int bitpos() const { return d_bitpos; }
//  };
//..
// Then, we create a pointer-like type that can point to a single bit:
//..
//  class BitPointer {
//
//      // DATA
//      char *d_byte_p;
//      int   d_bitpos;
//
//    public:
//      // CREATORS
//      BitPointer(char *byteptr = 0, int bitpos = 0)
//      : d_byte_p(byteptr)
//      , d_bitpos(bitpos)
//      {
//      }
//
//      // ACCESSORS
//      BitReference operator*() const
//      {
//          return BitReference(d_byte_p, d_bitpos);
//      }
//
//      // etc.
//  };
//..
// Next, we overload 'operator&' for 'BitReference' to return a 'BitPointer'
// instead of a raw pointer, completing the setup:
//..
//  inline BitPointer operator&(const BitReference& ref)
//  {
//      return BitPointer(ref.byteptr(), ref.bitpos());
//  }
//..
// Then, we note that there are times when it might be desirable to get the
// true address of a 'BitReference'.  Since the above overload prevents the
// obvious syntax from working, we use 'bsls::Util::addressOf' to accomplish
// this task.
//
// Next, we create a 'BitReference' object:
//..
//  char c[4];
//  BitReference br(c, 3);
//..
// Now, we invoke 'bsls::Util::addressOf' to obtain and save the address of
// 'br':
//..
//  BitReference *p = bsls::Util::addressOf(br);  // OK
//  // BitReference *p = &br;                     // Won't compile
//..
// Notice that the commented line illustrates canonical use of 'operator&' that
// would not compile in this example.
//
// Finally, we verify that address obtained is the correct one, running some
// sanity checks:
//..
//  assert(0 != p);
//  assert(c == p->byteptr());
//  assert(3 == p->bitpos());
//..

namespace BloombergLP {

namespace bsls {

                                 // ===========
                                 // struct Util
                                 // ===========

struct Util {
    // This 'struct' provides a namespace for essential low-level functions for
    // implementing portable generic facilities such as the C++ standard
    // library.

    // CLASS METHODS
    template <class BSLS_TYPE>
    static BSLS_TYPE *addressOf(BSLS_TYPE& obj);
        // Return the address of the specified 'obj', even if 'operator&' is
        // overloaded for objects of type 'BSLS_TYPE'.  Behavior is undefined
        // unless 'BSLS_TYPE' is an object type.  Note that this function
        // conforms to the C++11 definition for 'addressof' as specified in the
        // section [specialized.addressof] (20.6.12.1) of the C++11 standard,
        // except that function types, which are not object types, are
        // supported by 'std::addressof' in C++11.
};

}  // close package namespace

                                   // ======
                                   // MACROS
                                   // ======

// The following macros are private to the BDE implementation and not intended
// for widespread use.  They support the BDE STL decision for the standard
// containers to support types that overload 'operator&' only on the Microsoft
// platform.  This support is provided on Microsoft to enable containers
// holding the 'CComPtr' type from the Microsoft Foundation Class library
// (which overloads 'operator&'), but is not provided on UNIX platforms to
// avoid additional template bloat in the 'big' only to support a class design
// that is almost certainly an error.
#ifdef BSLS_PLATFORM_CMP_MSVC
#   define BSLS_UTIL_ADDRESSOF(OBJ) ::BloombergLP::bsls::Util::addressOf(OBJ)

#   if !defined(BDE_USE_ADDRESSOF)
#       define BDE_USE_ADDRESSOF
#   endif
#else
#   define BSLS_UTIL_ADDRESSOF(OBJ) (&(OBJ))
#endif

namespace bsls {

// This macro takes the address of an object by calling 'Util::addressOf' on
// Windows, and simply taking the address with the '&' operator on all other
// platforms.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CLASS METHODS
template <class BSLS_TYPE>
inline
BSLS_TYPE *Util::addressOf(BSLS_TYPE& obj)
{
    return static_cast<BSLS_TYPE *>(
        static_cast<void *>(
            const_cast<char *>(&reinterpret_cast<const volatile char&>(obj))));
}

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
