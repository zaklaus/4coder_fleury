#undef function

/*** Start of inlined file: plugin_win32.cpp ***/

/*** Start of inlined file: plugin.h ***/
#ifndef PLUGIN_H
#include <stddef.h>

/*** Start of inlined file: cee.h ***/
/* TODO:
 *  - could sizeof(**ptr) be used to enforce void ** checks:
 *      - #define free_ptr(ptr) free_ptr((sizeof(**ptr), ptr))
 *      - would this fail with a literal void ** rather than normal int **
 *      - usage: int *x = malloc(sizeof(int)); free_ptr(&x); // x now == 0
 *
 */

#ifndef CEE_H

// for data in global scope
#define global   static
// for data in function scope
#define persist  static
// TODO: change to "local"
// for functions
#define internal static
// for array function parameters
#define atleast static

// INTERNAL TO HEADER //
#define CEE_CAT1(a,b) a ## b
#define CEE_CAT2(a,b) CEE_CAT1(a,b)
#define CEE_CAT(a,b)  CEE_CAT2(a,b)

#define cee_n CEE_CAT(cee_internal_n_, __LINE__)
#define cee_a CEE_CAT(cee_internal_a_, __LINE__)
#define cee_b CEE_CAT(cee_internal_b_, __LINE__)

#define CEE_FIRST(a, ...) a
#define CEE_FIRST_ONLY(...) CEE_FIRST(__VA_ARGS__, DUMMY)
#define CEE_REST(a, ...) __VA_ARGS__

#define CEE_SECOND(a, b, ...) b
#define CEE_HAS_MULTIPLE(...) CEE_SECOND((__VA_ARGS__), (DUMMY1), DUMMY2)
CEE_REST(__VA_ARGS__ )

// compile-time assert:
/* #define ct_assert(e) struct { char compile_time_assert[(e) ? 1 : -1]; } */
#define ct_assert(e) typedef char CEE_CAT(compile_time_assertion, __LINE__)[(e) ? 1 : -1]

#if 1 // TYPES //

#define cast(type, data) ((type)(data))
#define ptr_cast(type, data) ((type)(uintptr_t)(data))

#include <stdint.h>

// bytes //
typedef uint8_t  U1;
typedef uint16_t U2;
typedef uint32_t U4;
typedef uint64_t U8;

typedef  int8_t  I1;
typedef  int16_t I2;
typedef  int32_t I4;
typedef  int64_t I8;

typedef  int8_t  S1;
typedef  int16_t S2;
typedef  int32_t S4;
typedef  int64_t S8;

typedef  int8_t  B1;
typedef  int16_t B2;
typedef  int32_t B4;
typedef  int64_t B8;

typedef float    F4;
typedef double   F8;

// bits //
typedef uint8_t  U8b;
typedef uint16_t U16b;
typedef uint32_t U32b;
typedef uint64_t U64b;

typedef  int8_t  I8b;
typedef  int16_t I16b;
typedef  int32_t I32b;
typedef  int64_t I64b;

typedef  int8_t  S8b;
typedef  int16_t S16b;
typedef  int32_t S32b;
typedef  int64_t S64b;

typedef  int8_t  B8b;
typedef  int16_t B16b;
typedef  int32_t B32b;
typedef  int64_t B64b;

typedef float    F32b;
typedef double   F64b;

// named //
typedef char          Char;
typedef unsigned char Byte;
typedef int           Bool;
typedef size_t        Size;
typedef uintptr_t     UPtr;
typedef intptr_t      SPtr;
typedef void *        Addr;

#define ARR_LEN_BITS(bits) ((bits / CHAR_BIT) + (bits % CHAR_BIT > 0))

// TODO: aligned versions

// TYPE CREATION //
// NOTE: if you don't add the brackets these just act as normal: e.g. struct MyType vs struct (MyType)
#define struct(s) struct s; typedef struct s s; struct s
#define union(s)  union  s; typedef union  s s; union  s
#define enum(s)   enum   s; typedef enum   s s; enum   s

// create a label as well as a case jump point
/* #define case(c) case c: case_## c */

// CASTING //
#define cast(t, value) ((t)(value))
// "bitcast"?
#define recast(ti, to, value) (((union { ti in, char b[sizeof(value)]; to out; }){value}).out)

#endif // TYPES

#if 1 // VALUES
#if 0 // ID VALUES
// 7 char string as numeric value
// NOTE: d7 had implementation
#define STR_ID_(s, n) (((s "\0\0\0\0\0\0\0\0")[n]) << n)
#define STR_ID1(s) STR_ID_(s,0)|STR_ID_(s,1)|STR_ID_(s,2)|STR_ID_(s,3)|STR_ID_(s,4)|STR_ID_(s,5)|STR_ID_(s,6) & ~(0xFF << 7)

/* #define STR__ID(s, n) (s[n] ? s[n] : 0) << 7-n */
#define STR__ID(s, n) (((s "\0\0\0\0\0\0\0\0")[n]) << (7-n))
#define STR_ID2(s) \
    STR__ID(s,0)| \
STR__ID(s,1) |\
STR__ID(s,2) |\
STR__ID(s,3) |\
STR__ID(s,4) |\
STR__ID(s,5) |\
STR__ID(s,6) &\
~0xFF
typedef union CeeStringId {
    U8   val;
    char str[8];
} CeeStringId;
#endif // ID VALUES

#define TYPE_IS_SIGNED(t) ((t)-1 < 0)
#define VAR_IS_SIGNED(v) ((((v)^(v))-1) < 0)

// INVALID VALUES //

#define MIN_SIGNED(type) ((type)1 << (sizeof(type) * 8 - 1))
#define MAX_UNSIGNED(type) (~(type)0)

#define IS_MIN_SIGNED(v) ((v) == ((((v)^(v))+1) << (sizeof(v) * 8 - 1)) )

// for unsigned: all bits set to 1
// TODO: for signed: MIN_SIGNED
// makes valid range for signed integers symmetrical
// if only unsigned integers are needed, only loses top value
// rather than losing half of available as when `value < 0` -> invalid

#define INVALID(type) (TYPE_IS_SIGNED(type) ? MIN_SIGNED(type) : MAX_UNSIGNED(type))
// e.g. memset(new_array, INVALID(char), sizeof(new_array));
#define is_invalid(value) (VAR_IS_SIGNED(value) ? IS_MIN_SIGNED(value) : !~(value))
#define is_valid(value) (VAR_IS_SIGNED(value)   ? ! IS_MIN_SIGNED(value) : !!~(value))

// SIZES //
#ifndef KB
#define KB(x) (1024ULL *   (x))
#define MB(x) (1024ULL * KB(x))
#define GB(x) (1024ULL * MB(x))
#define TB(x) (1024ULL * GB(x))
#endif // KB
#endif // VALUES

#if 1 // SCOPES
// TODO: not sure about naming here... see how they tend to be used in practice
#define scoped(start, end) for(char cee_n = ((start), 0); !cee_n++; end)
#define before_after(when, ...) for(char const *when = "before", *cee_n = ((__VA_ARGS__), when); cee_n; when = "after", cee_n = 0, __VA_ARGS__)
// NOTE: MUST use a type
#define scope(init, end) for(init, *cee_n = 0; !cee_n; ++cee_n, (end))
#define scope_var(init)  for(init, *cee_n = 0; !cee_n; ++cee_n)
#endif // SCOPES

#if 1 // ANNOTATION
#define NOTE(x,m) x
#define yes(m) 1
#define no(m)  0
#define nonull
// ALT for nonull
// #if DEBUG
// #define not_null(p) (assert(p && "argument is null"),p)
// #endif
//
// int my_func(nonull int *nums, int count) {...}
// #define my_func(a,b) my_func(not_null(a), b)

// to be redefined at the point of use as _:  _(a, V2_add, _(b, V2_sub, c))
// #define _(a,fn,b) INFIX_PRE(V2_,a,fn,b) => _(a, add, _(b, sub, c))
//                           vs normal use => V2_add(a, V2_sub(b, c))
#define INFIX(a, fn, b)                                   fn((a), (b))
#define INFIX_PRE(pre, a, fn, b)                   pre ## fn((a), (b))
#define INFIX_POST(post, a, fn, b)                fn ## post((a), (b))
#define INFIX_PREPOST(pre, post, a, fn, b) pre ## fn ## post((a), (b))

//#define ASSUME_  typedef char assumption_failed[-1];
#define ASSUME_CHECK_0 typedef char assumption_failed[-1];
#define ASSUME_CHECK_1
#define assume(a) CEE_CAT(ASSUME_CHECK_, ASSUME_## a)
#endif // ANNOTATION

#if 1 // LOOPS
#define array_len(...) (sizeof(__VA_ARGS__)/sizeof(*(__VA_ARGS__)))
#define countof(...) (sizeof(__VA_ARGS__)/sizeof(*(__VA_ARGS__)))
/* #ifndef  offsetof */
/* # define offsetof(st, m) (&((st *)0)->m) */
/* #endif */
/* #ifndef  alignof */
/* # define alignof(t) (&((struct{char c; t m} *)0)->m) */
/* #endif */
#define ARRAY__N(...)    (__VA_ARGS__), countof(__VA_ARGS__)
#define N__ARRAY(...)    countof(__VA_ARGS__), (__VA_ARGS__)
#define ARRAY__SIZE(...) (__VA_ARGS__), sizeof(__VA_ARGS__)
#define SIZE__ARRAY(...) sizeof(__VA_ARGS__), (__VA_ARGS__)
#define STR_LIT__SIZE(a) (a), sizeof(a)
#define SIZE__STR_LIT(a) sizeof(a), (a)
#define STR_LIT__LEN(a)  (a), (sizeof(a)-1)
#define LEN__STR_LIT(a)  (sizeof(a)-1), (a)

// TODO: typeof variants
// TODO: can define struct with all components of loop...
#define each
// NOTE: because this uses the auto-coercing void *, this will only work in C, not C++, this is required because it's unknown what level of pointer t is
#define all(t, a, ...) (t a = __VA_ARGS__, *cee_n = (void*)((char*)a + sizeof(__VA_ARGS__)); (void*)a < (void*)cee_n; ++a)
#define all_lit(t, a, ...) all(t*, a, (t[])__VA_ARGS__)
// TODO: use (unsigned) -1 to allow unsigned loops to zero with same format
//#define range(t, i, start, end) (t i = (start), cee_a = ((end) < (start) ? (t)-1 : +1), cee_n = (t)(end)-((end) < (start) ? (t)1 : 0); i != cee_n; i += cee_a)
#define range(  t, i, start, end) (t i = (start); i != (end); i += 1)
#define range_const(t, i, start, end) (t i = (start), cee_n = (end); i != cee_n; i += 1)
#define range_n(t, i, start, n, end) (t i = (start), n = (end); i != n; i += 1)
#define upto(t, i, end) (t i = 0; i < end; ++i)
#define repeat(n) for (Size cee_n = (n); cee_n-- > 0;)

#define until(e) while(! (e))
#endif // LOOPS

// JUMPS //
#define case_label(x) case x: case_## x
// case_label(3): dostuff(); break;
// case_label(4): dootherstuff(); goto case_3;
#define fallthrough

// FUNCTIONS //
// XXX: meh...
#define multiple_return(types, fn) struct fn##_result types fn
/*
 * multiple_return( {int a; int b},
 * my_function_name)() {
 * 		struct my_function_name_result result = {0};
 * 		return result;
 * }
 *
 * struct my_function_name_result result = my_function_name();
 */

#if 1  // BINARY
#define BITS_0000 0x0
#define BITS_0001 0x1
#define BITS_0010 0x2
#define BITS_0011 0x3
#define BITS_0100 0x4
#define BITS_0101 0x5
#define BITS_0110 0x6
#define BITS_0111 0x7
#define BITS_1000 0x8
#define BITS_1001 0x9
#define BITS_1010 0xa
#define BITS_1011 0xb
#define BITS_1100 0xc
#define BITS_1101 0xd
#define BITS_1110 0xe
#define BITS_1111 0xf

#define BITS_4(a)   BITS_##a
#define BITS_8(a,b)               ((BITS_##a <<  4) | BITS_##b)
#define BITS_16(a,b,c,d)          ((BITS_##a << 12) | (BITS_##b <<  8) | (BITS_##c << 4) | BITS_##d)
#define BITS_32(a,b,c,d, e,f,g,h) ((BITS_##a << 28) | (BITS_##b << 24) | (BITS_##c << 20) | (BITS_##d << 16) |\
                                   (BITS_##e << 12) | (BITS_##f <<  8) | (BITS_##g <<  4) | (BITS_##h))
#define BITS_64(a,b,c,d, e,f,g,h, i,j,k,l, m,n,o,p) (((long long)BITS_32(a,b,c,d, e,f,g,h) << 32) | BITS_32(i,j,k,l, m,n,o,p))

#if 0  // USAGE
enum { My_Bits = BITS_8(0010, 1100) };
#endif // USAGE
#endif // BINARY

// USEFUL CONSTANTS FOR QUASI-RANDOM LAYOUT
static const double Golden_Ratio     = 1.61803398874989484820458683436563;
static const double Phi1             = 1.61803398874989484820458683436563;
static const double Plastic_Constant = 1.32471795724474602596090885447809;
static const double Phi2             = 1.32471795724474602596090885447809;
static const double Phi3             = 1.220744084605759475361685349108831;

#if 0  // USAGE
// for use with `n` points in `d` dimensions:

g = Phi_d

a1 = 1.0/g
a2 = 1.0/(g*g)
a3 = 1.0/(g*g*g)
...

unused;
x[n] = modf(0.5+a1*n, &unused) // fractional part, i.e. between 0 & 1
y[n] = modf(0.5+a2*n, &unused)
z[n] = modf(0.5+a3*n, &unused)
...
#endif // USAGE

#define CEE_H
#endif//CEE_H

/*** End of inlined file: cee.h ***/


#define PLUGIN_PIPE_NAME L"\\\\.\\pipe\\whitebox_plugin"

//  TODO: SrcLoc & comparisons
// NOTE: all 4 should be valid at any time.
// if the cursor is just being used normally, current == initial
// current may be >, ==, or < initial
typedef struct Selection {
    // TODO: 2 `V2i`s?
    // TODO: naming
     Size current_line, current_clmn,
          initial_line, initial_clmn;
} Selection;

static inline Bool
is_selection(Selection const *selection)
{
    Bool result = (selection->current_line != selection->initial_line ||
                   selection->current_clmn != selection->initial_clmn);
    return result;
}

static inline Bool
selection_eq(Selection const *a, Selection const *b)
{
    Bool result = (a && b &&
                   a->current_line == b->current_line &&
                   a->current_clmn == b->current_clmn &&
                   a->initial_line == b->initial_line &&
                   a->initial_clmn == b->initial_clmn);
    return result;
}

static int
rng_contains_rng(unsigned parent_begin_line, unsigned short parent_begin_clmn,
                 unsigned parent_end_line,   unsigned short parent_end_clmn,
                 unsigned child_begin_line,  unsigned short child_begin_clmn,
                 unsigned child_end_line,    unsigned short child_end_clmn)

{
    int starts_after_parent = (child_begin_line  >  parent_begin_line ||
                               (child_begin_line == parent_begin_line &&
                                child_begin_clmn >= parent_begin_clmn));
    int ends_before_parent  = (child_end_line    <  parent_end_line ||
                               (child_end_line   == parent_end_line &&
                                child_end_clmn   <= parent_end_clmn));
    return starts_after_parent && ends_before_parent;
}

// TODO: should this be a string?
typedef enum PluginEditor {
    EDITOR_none,
    EDITOR_file,  // using a given file to parse location etc
    EDITOR_4coder,
    EDITOR_notepad_pp,
    EDITOR_vim,
    EDITOR_COUNT,
} PluginEditor;

typedef enum DirtyState {
    DIRTY_current              = 0x0,
    DIRTY_unsaved              = 0x1,
    DIRTY_unloaded             = 0x2,
    DIRTY_unsaved_and_unloaded = DIRTY_unsaved | DIRTY_unloaded,
} DirtyState;

typedef struct PluginDataOut { // from plugin to WhiteBox proper
    Selection  sel;
    U1         editor; // TODO: no need to communicate every time
    U1         dirty_state;
    char      *path;
} PluginDataOut;
enum {
     PluginDataOut_Fixed_Size = offsetof(PluginDataOut, path),
     Plugin_Path_Len = 260,
};
// typedef struct PluginIn { } PluginIn;
//
#define PLUGIN_H
#endif//PLUGIN_H

/*** End of inlined file: plugin.h ***/


/*** Start of inlined file: airhead.h ***/
/* TODO:
 * - add compile-time option for bounds-checking with custom behaviour on fail
 * - refcounting
 * - thread-safe/atomic versions
 * - alignment for SIMD
 * - minimize repeated arguments where possible
 *     - move to functions?
 * - consider adding a temp/register at a[-1]
 * - add flags to top capacity bits
 */

/*
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| Test | Name                      | Description                                                                                                              |
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
|      |                           |                                                                                                                          |
| ---- | Adding elements           | ------------------------------------------------------------------------------------------------------------------------ |
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| PASS | push(a,v)                 | append value to the end of the array, growing (or creating) the array if needed                                          |
| PASS | add(a,n)                  | add space for `n` more items on the array, growing (or creating) if needed. The items are considered used (in len)       |
| PASS | insert(a, i, v)           | inserts value into the array at existing index i, moving elements from position i upwards along 1 to make space          |
|      |                           |                                                                                                                          |
| TODO | changecap(a,n)            | change space to allow for `n` items on the array, growing (or creating) if needed                                        |
| TODO | expand(a,n)               | add space to allow for `n` items on the array, growing (or creating) if needed                                           |
|      |                           |                                                                                                                          |
| PASS | pusharr(a,b)              | appends the values from b to the end of a                                                                                |
| PASS | pushptr(a,ptr,n)          | appends n elements from ptr to the end of a                                                                              |
| TODO | pushstr(a,str)            | appends chars from str until a '\0' character is hit
| TODO | pushstrlit(a,str)         | pushes a string literal
| TODO | pushstrn(a,str,n)         | appends n elements from ptr to the end of a                                                                              |
| PASS | pusharray(a,arr)          | append array to the end of a for fixed-size arrays (where the number of elements can be determined at compile-time)      |
|      |                           |                                                                                                                          |
| TODO | insertarr(a, i, b)        | inserts value into the array at existing index i, moving elements from position i upwards along 1 to make space          |
| TODO | insertptr(a, i, v, n)     | inserts value into the array at existing index i, moving elements from position i upwards along 1 to make space          |
| TODO | insertarray(a, i, arr)    | inserts value into the array at existing index i, moving elements from position i upwards along 1 to make space          |
|      |                           |                                                                                                                          |
| ---- | Removing elements         | ------------------------------------------------------------------------------------------------------------------------ |
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| NONE | remove(a, i, n)           | removes the element at index i, shifting the higher elements down into its place TODO: this should have n                |
| NONE | pull(a,i)                 | remove 1 and return the value. This fails if wrapped in parens: `x = (pull(vals, i))`                                    |
| PASS | pop(a)                    | removes and returns the last element of the array                                                                        |
| NONE | shift(a)                  | removes and returns the first element of the array, shifting the other elements down into its place                      |
|      |                           |                                                                                                                          |
| PASS | clear(a)                  | set the length of array to 0                                                                                             |
|      |                           |                                                                                                                          |
| PASS | removeswap(a,i)           | a faster way to remove elements. Moves last element into index i. Does not preserve ordering.                            |
|      |                           |                                                                                                                          |
| PASS | resetlen(a,n)             | set length to a given value                                                                                              |
|      |                           |                                                                                                                          |
| PASS | free(a)                   | frees the array contents; also sets the array ptr back to NULL, causing access to fail. The array can be pushed to again |
| NONE | free2d(a)                 | frees 2d/jagged arrays - all of the subarrays are freed, then the containing array                                       |
| NONE | free2dt(ht2, a)           | as above, but for when the inner arrays have a different header type                                                     |
|      |                           |                                                                                                                          |
|      |                           |                                                                                                                          |
|      | / Accessing elements /    |                                                                                                                          |
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| PASS | bc(a,i)                   | checks that i is within the bounds of a                                                                                  |
| NONE | get(a,i)                  | expands to an l-value of the ith  element in the array, or asserts if i is out of bounds                                 |
| PASS | last(a)                   | expands to an l-value of the last element in the array                                                                   |
|      |                           |                                                                                                                          |
|      |                           |                                                                                                                          |
|      | Array header/metadata     |                                                                                                                          |
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| PASS | hdr(a)                    | 'returns' a pointer to the header, which you can then use like a normal struct                                           |
| PASS | len(a)                    | number of elements in the array. Safe to pass `NULL`.                                                                    |
| PASS | cap(a)                    | number of possible elements in the array before it needs to grow. Safe to pass `NULL`.                                   |
| ???? | count(a)                  | (alias for `ahd_len` (kept for compatibility with `stb_stretchybuffer.h`                                                 |
| PASS | _len(a)                   | number of elements in the array. Not safe to pass `NULL`.                                                                |
| PASS | _cap(a)                   | number of possible elements in the array before it needs to grow. Not safe to pass `NULL`.                               |
|      |                           |                                                                                                                          |
| PASS | size(a)                   | number of bytes in the array (excluding the header) for `len` elements                                                   |
| PASS | totalsize(a)              | number of bytes in the array (including the header)                                                                      |
| NONE | capsize(a)                | number of bytes in the array (excluding the header) for `cap` elements                                                   |
| NONE | totalcapsize(a)           | number of bytes in the array (including the header)                                                                      |
|      |                           |                                                                                                                          |
|      |                           |                                                                                                                          |
|      | Extracting array subsets  | Creates new array to be freed at some point                                                                              |
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| PASS | dup(a)                    | duplicate the array                                                                                                      |
| PASS | sub(a,f,n)                | duplicate subset of the array                                                                                            |
| TODO | slice()                   | window into array (elements will update along with original)                                                             |
|      |                           |                                                                                                                          |
| TODO | unique()                  |                                                                                                                          |
| TODO | uniqueadj()               | linear time simplification of above - only excludes adjacent elements: {a,b,b,c,b} -> {a,b,c,b}                          |
|      |                           |                                                                                                                          |
| TODO | union(a,b)                |                                                                                                                          |
| TODO | intersection(a,b)         |                                                                                                                          |
| TODO | difference(a,b)           |                                                                                                                          |
| TODO | symdiff(a,b)              |                                                                                                                          |
|      |                           |                                                                                                                          |
| TODO | shuffle(a)                |                                                                                                                          |
| TODO | sample(a,n)               |                                                                                                                          |
|      |                           |                                                                                                                          |
|      | / Rearranging array /     | Modify the array itself (nothing new created on the heap)                                                                |
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| PASS | sortu(a,mem,dir)          | Sort a in dir direction based on an unsigned integer member of each element (mem is the address of that)                 |
| PASS | sorts(a,mem,dir)          | Sort a in dir direction based on a signed integer member of each element (mem is the address of that)                    |
| PASS | sorti(a,mem,dir)          | Sort a in dir direction based on any integer member of each element (mem is the address of that)                         |
| PASS | sortf(a,mem,dir)          | Sort a in dir direction based on a floating point member of each element (mem is the address of that)                    |
| TODO | sortstr(a,mem,dir)        | Sort a in dir direction based on a string pointer member of each element (mem is the address of that)                    |
| TODO | sortchr(a,t,dir)          | Sort a in dir direction based on a character array member of each element (mem is the address of that)                   |
| PASS | reverse(a)                | reverse the order of the elements in the array                                                                           |
| PASS | rotr(a,n)                 | move each element to the next index. The last element moves to the first position                                        |
| TODO | rotl(a,n)                 | move each element to the previous index. The first element moves to the last position                                    |
|      |                           |                                                                                                                          |
|      |                           |                                                                                                                          |
|      | / Array processing /      |                                                                                                                          |
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| NONE | mapt(i,ta,a,va,tb,b,vb)   |                                                                                                                          |
| NONE | map(i,t,a,b,v)            |                                                                                                                          |
| NONE | mapfn(fn,a,b,udata)       |                                                                                                                          |
|      |                           |                                                                                                                          |
| NONE | reduce(a,ex)              |                                                                                                                          |
| NONE | reducefn(fn,a,acc,udata)  |                                                                                                                          |
|      |                           |                                                                                                                          |
| NONE | filter(i,t,v,a,b,tr)      |                                                                                                                          |
| NONE | reject(i,t,v,a,b,tr)      |                                                                                                                          |
| NONE | count(i,t,v,a,b,tr)       |                                                                                                                          |
| NONE | uncount(i,t,v,a,b,tr)     |                                                                                                                          |
| NONE |                           |                                                                                                                          |
| NONE | find(a,i,t,v,fnd,tr)      |                                                                                                                          |
| NONE | findi(a,i,t,v,tr)         |                                                                                                                          |
| NONE | findv(a,t,v,fnd,tr)       |                                                                                                                          |
|      |                           |                                                                                                                          |
|      | / Scoped memory /         |                                                                                                                          |
| ---- | ------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| PASS | exitscope                 |                                                                                                                          |
| PASS | scoped(t,a)               |                                                                                                                          |
| PASS | scoped_init(t,a,init)     |                                                                                                                          |
| NONE | scoped2d(t,a)             |                                                                                                                          |
| NONE | scoped2d_init(t,a,init)   |                                                                                                                          |
*************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AHD_INCLUDED

#if ! (defined(AHD_REALLOC) && defined(AHD_FREE))
#include <stdlib.h>
#define AHD_REALLOC(ptr, size) realloc(ptr, size)
#define AHD_FREE(ptr) free(ptr)
#endif/*stdlib*/

#if ! (defined(AHD_MEMCPY) && defined(AHD_MEMMOVE))
#include <string.h>
#define AHD_MEMCPY memcpy
#define AHD_MEMCMP memcmp
#define AHD_MEMMOVE memmove
#endif/*stdlib*/

#ifndef  AHD_VSNPRINTF
# define AHD_VSNPRINTF(...) vsnprintf(__VA_ARGS__)
#endif

#ifndef ahd_int
typedef unsigned long long ahd_int;
#endif

#ifndef ahd_enum
typedef int ahd_enum;
#endif

typedef struct ahd_arr {
	ahd_int cap;
	ahd_int len;
} ahd_arr;

// TODO:
typedef struct ahd_rc {
	ahd_int rc;
	void (*free)(void*);
} ahd_rc;

// for use with enums
typedef struct ahd_kind {
	ahd_enum kind;
} ahd_kind;

typedef struct ahd_ts {
	ahd_int lock;
	// TODO: anything else? signals?
} ahd_ts; // or mtx?

#define ahd_t(t) ahd_## t t

/* Usage: creating a custom header
 * typedef struct typesafe_refcounted_array { ahd_t(arr); ahd_t(rc); int my_custom_int; ahd_t(ts); } typesafe_refcounted_array;
 * int *Nums = 0;
 * ahd_push(typesafe_refcounted_array, Nums, 3);
 * #define tsrc_push(a,v) ahd_push(typesafe_refcounted_array, a, v)
 * tsrc_push(Nums, 62);
 */

#ifndef AHD_NO_DEFAULT_ARR /***************************************************/
/* Adding elements */
#define arr_push(a,v)               ahd_push(ahd_arr,a,v)
#define arr_add(a,n)                ahd_add(ahd_arr,a,n)
#define arr_insert(a,i,v)           ahd_insert(ahd_arr,a,i,v)

#define arr_append(a,src,len,size)  ahd_append(ahd_arr,a,src,len,size)
#define arr_concat(a,b)             ahd_concat(ahd_arr,a,b)
#define arr_pusharr(a,arr,n)        ahd_pusharr(ahd_arr,a,arr,n)
#define arr_pushptr(a,arr,n)        ahd_pushptr(ahd_arr,a,arr,n)
#define arr_pushstr(a,str)          ahd_pushstr(ahd_arr,a,str)
#define arr_pushstrlit(a,str)       ahd_pushstrlit(ahd_arr,a,str)
#define arr_pushstrn(a,str,n)       ahd_pushstr(ahd_arr,a,str,n)
#define arr_pusharray(a,arr)        ahd_pusharray(ahd_arr,a,arr)

/* Removing elements */
#define arr_pop(a)                  ahd_pop(ahd_arr,a)
#define arr_shift(a)                ahd_shift(ahd_arr,a)
#define arr_remove(a,i,n)           ahd_remove(ahd_arr,a,i,n)
#define arr_pull(a,i)               ahd_pull(ahd_arr,a,i)
#define arr_removeswap(a,i)         ahd_removeswap(ahd_arr,a,i)

#define arr_clear(a)                ahd_clear(ahd_arr,a)
#define arr_resetlen(a,n)           ahd_resetlen(ahd_arr,a,n)

#define arr_free(a)                 ahd_free(ahd_arr,a)
#define arr_free2dt(ht2,a)          ahd_free2dt(ahd_arr,ht2,a)
#define arr_free2d(a)               ahd_free2d(ahd_arr,a)

/* Accessing elements */
#define arr_bc(a,i)                 ahd_bc(ahd_arr,a,i)
#define arr_get(a,i)                ahd_get(ahd_arr,a,i)
#define arr_el(a,i)                 ahd_el(ahd_arr,a,i)
#define arr_last(a)                 ahd_last(ahd_arr,a)

/* Array header/metadata */
#define arr_hdr(a)                  ahd_hdr(ahd_arr,a)

#define arr_len(a)                  ahd_len(ahd_arr,a)
#define arr_cap(a)                  ahd_cap(ahd_arr,a)
#define arr_size(a)                 ahd_size(ahd_arr,a)
#define arr_totalsize(a)            ahd_totalsize(ahd_arr,a)
#define arr_capsize(a)              ahd_capsize(ahd_arr,a)
#define arr_totalcapsize(a)         ahd_totalcapsize(ahd_arr,a)

#define arr__len(a)                 ahd__len(ahd_arr,a)
#define arr__cap(a)                 ahd__cap(ahd_arr,a)
#define arr__size(a)                ahd__size(ahd_arr,a)
#define arr__totalsize(a)           ahd__totalsize(ahd_arr,a)
#define arr__capsize(a)             ahd__capsize(ahd_arr,a)
#define arr__totalcapsize(a)        ahd__totalcapsize(ahd_arr,a)

/* Extracting array subsets */
#define arr_dup(a)                  ahd_dup(ahd_arr,a)
#define arr_sub(a,f,n)              ahd_sub(ahd_arr,a,f,n)
#define arr_slice()                 ahd_slice()

#define arr_unique(a)               ahd_unique(ahd_arr,a)

#define arr_union(a,b)              ahd_union(ahd_arr,a,b)
#define arr_intersection(a,b)       ahd_intersection(ahd_arr,a,b)

/* Rearranging array */
#define arr_sorti(a,mem,dir)        ahd_sorti(ahd_arr,a,mem,dir)
#define arr_sortu(a,mem,dir)        ahd_sortu(ahd_arr,a,mem,dir)
#define arr_sortint(a,mem,dir)      ahd_sortint(ahd_arr,a,mem,dir)
#define arr_sortf(a,mem,dir)        ahd_sortf(ahd_arr,a,mem,dir)
#define arr_reverse(a)              ahd_reverse(ahd_arr,a)
#define arr_rotr(a, n)              ahd_rotr(ahd_arr,a,n)
#define arr_rotl(a, n)              ahd_rotl(ahd_arr,a,n)

#define arr_eq(a,b)              ahd_eq(ahd_arr,a,b)

/* Array processing */
#if 1
#define arr_each(a,i)                  ahd_each(ahd_arr,a,i)
#define arr_each_r(a,i)                ahd_each_r(ahd_arr,a,i)
#define arr_each_v(a,i,t,v)            ahd_each_v(ahd_arr,a,i,t,v)
#define arr_each_rv(a,i,t,v)           ahd_each_rv(ahd_arr,a,i,t,v)
#else // 0
#define arr_foreach(a,i)                  for ahd_each(ahd_arr,a,i)
#define arr_foreach_r(a,i)                for ahd_each_r(ahd_arr,a,i)
#define arr_foreach_v(a,i,t,v)            for ahd_each_v(ahd_arr,a,i,t,v)
#define arr_foreach_rv(a,i,t,v)           for ahd_each_rv(ahd_arr,a,i,t,v)
#endif // 0

#define arr_mapt(i,ta,a,va,tb,b,vb)    ahd_mapt(ahd_arr,i,ta,a,va,tb,b,vb)
#define arr_mapv(i,t,a,b,v)            ahd_mapv(ahd_arr,i,t,a,b,v)
#define arr_mapfn(fn,a,b,udata)        ahd_mapfn(ahd_arr,fn,a,b,udata)

#define arr_reduce(a,ex)               ahd_reduce(ahd_arr,a,ex)
#define arr_reducefn(fn,a,acc,udata)   ahd_reducefn(ahd_arr,fn,a,acc,udata)

#define arr_filter(i,t,v,a,b,tr)       ahd_filter(ahd_arr,i,t,v,a,b,tr)
#define arr_count(i,t,v,a,b,tr)        ahd_count(ahd_arr,i,t,v,a,b,tr)
#define arr_countx(a,i,x,tr)           ahd_countx(ahd_arr,a,i,x,tr)

#define arr_find(a,i,t,v,fnd,tr)       ahd_find(ahd_arr,a,i,t,v,fnd,tr)
#define arr_findi(a,i,t,v,tr)          ahd_findi(ahd_arr,a,i,t,v,tr)
#define arr_findv(a,t,v,fnd,tr)        ahd_findv(ahd_arr,a,t,v,fnd,tr)

/* Scoped memory */
#define arr_exitscope                  ahd_exitscope
#define arr_scoped(t,a)                ahd_scoped(ahd_arr,t,a)
#define arr_scoped2(t,a,b)             ahd_scoped2(ahd_arr,t,a,b)
#define arr_scoped_init(t,a,init)      ahd_scoped_init(ahd_arr,t,a,init)
#define arr_scoped2d(t,a)              ahd_scoped2d(ahd_arr,t,a)
#define arr_scoped2d_init(t,a,init)    ahd_scoped2d_init(ahd_arr,t,a,init)

#endif/*AHD_NO_DEFAULT_ARR*****************************************************/

/******************************************************************************/
/* Helper macros **************************************************************/
/******************************************************************************/
#define AHD_LN(x)  AHD_CAT(x, __LINE__)

#define AHD_CAT(a,b)  AHD_CAT2(a,b)
#define AHD_CAT2(a,b) AHD_CAT1(a,b)
#define AHD_CAT1(a,b) a##b

#define ahd_n_ln AHD_LN(ahd_n_)
#define ahd_i_ln AHD_LN(ahd_i_)

// TODO: used?
#define ahd_ptr2i(a,p)        (((unsigned char *)(p) - (unsigned char *)(a)) / sizeof(*(a)))

#define ahd_if(a,v)           ((a) ? (v) : 0)

#define ahd__data(ht,a) (a), sizeof(ht), sizeof(*(a))

/******************************************************************************/
/* Adding elements ************************************************************/
/******************************************************************************/
#define ahd_push(ht,a,v)      (ahd_maybegrow(ht,a,1), (a)[ahd__len(ht,a)++] = (v), ahd__len(ht,a)-1)
#define ahd_add(ht,a,n)       ((ahd_maybegrow(ht,a,n), ahd__len(ht,a)+=(n)) - (n))
#define ahd_concat(ht,a,b)        ahd__pushsize(ht, a, b,   ahd_len(ht, b),             ahd_size(ht,b))
#define ahd_pusharr(ht,a,arr,els) ahd__pushsize(ht, a, arr, els,                        (els)*sizeof(*(a)) )
#define ahd_pushptr(ht,a,ptr,els) ahd__pushsize(ht, a, ptr, els,                        (els)*sizeof(*(a)) )
#define ahd_pushstrlit(ht,a,str)  ahd__pushsize(ht, a, str, sizeof(str),                sizeof(str) )
#define ahd_pushstr(ht,a,str)     (*(char **)&(a) = ahd__pushstr(&(a), sizeof(ht), sizeof(*(a)), str, ~0))
#define ahd_pushstrn(ht,a,str,n)  (*(char **)&(a) = ahd__pushstr(&(a), sizeof(ht), sizeof(*(a)), str,  n))
#define ahd_pusharray(ht,a,arr)   ahd__pushsize(ht, a, arr, sizeof(arr)/sizeof(*(arr)), sizeof(arr))

#define ahd__pushsize(ht,a,src,len,size) (AHD_MEMMOVE((a)+ahd_add(ht,a,len), (src), (size)), ahd__len(ht,a)-(len))

#define ahd_insert(ht,a,i,v)  (ahd_add(ht,a,1),\
	                           AHD_MEMMOVE((a)+(i)+1, (a)+(i), (ahd__len(ht,a)-(i)-1) * sizeof(*(a)) ),\
	                           (a)[(i)] = (v), (i) )

/*TODO: #define ahd_push_ts(ht,a,v)   do{ \
	(ahd_maybegrow(ht,a,1), (a)[ahd__len(ht,a)++] = (v), ahd__len(ht,a)-1)} while(0)*/

#define ahd_needgrow(ht,a,n)  ((a)==0 || ahd__len(ht,a)+(n) >= ahd__cap(ht,a))
#define ahd_maybegrow(ht,a,n) ahd_if(ahd_needgrow(ht,a,(n)), ahd_grow(ht,a,n))
#define ahd_grow(ht,a,n)      (*((void **)&(a)) = ahd__grow(ahd_if(a, ahd_hdr(ht,a)), (n), \
			                  sizeof(*(a)), sizeof(ht)))

#if AHD_DEBUG // control whether callsite is recorded
#define AHD_DBG(fn, ...) fn##_dbg(__VA_ARGS__, int line, char const *file, char const *func, char const *call)

#define ahd__grow(...) ahd__grow_dbg(__VA_ARGS__, __LINE__, __FILE__, __func__, "ahd__grow("#__VA_ARGS__")")

#else //AHD_DEBUG
#define AHD_DBG(fn, ...) fn(__VA_ARGS__)

#define ahd__grow(...) ahd__grow(__VA_ARGS__)
#endif//AHD_DEBUG

// TODO: should this be arr ptr, rather than base?
static void *
AHD_DBG(ahd__grow, void *ptr, ahd_int inc, ahd_int itemsize, ahd_int headersize)//, int cap, int len)
// TODO: static int ahd__grow(void **ptr, ahd_int inc, ahd_int itemsize, ahd_int headersize)//, int cap, int len)
{
	ahd_arr *head      = (ahd_arr *)ptr;
	ahd_int dbl_cap    = ahd_if(ptr, head->cap  * 2);
	ahd_int min_needed = ahd_if(ptr, head->len) + inc;
	min_needed         = (min_needed > 64) ? min_needed : 64;
	ahd_int new_cap    = (dbl_cap > min_needed) ? dbl_cap   : min_needed; // could round to next highest pow2?
	head               = (ahd_arr *) AHD_REALLOC(ptr, itemsize * new_cap + headersize);
	if (head) {
		if (!ptr) {
			char *hdr_bytes = ((char *)&head->len) - 1,
				 *hdr_guard = ((char *)head) + headersize;
			while(++hdr_bytes < hdr_guard)
			{ *hdr_bytes = 0; } /* zero init for any size header */
		}
		head->cap = new_cap;
		return (char *)head + headersize;
	}
	else {
#ifdef AHD_BUFFER_OUT_OF_MEMORY
		AHD_BUFFER_OUT_OF_MEMORY ;
#endif
		return (char*)(uintptr_t)headersize; // try to force a NULL pointer exception later
	}
}

#if 1
static ahd_int
ahd__pushstr(char **arr, ahd_int hdr_size, ahd_int el_size, char *str, ahd_int n)
{
	ahd_arr *head = (ahd_arr *)(*arr - hdr_size);
	ahd_int len = head->len;
	ahd_int strLen = 0;
	char *str_ = str;
	while(*(str_++) && (!~n || n--)) {++strLen;}

	if(head->len + strLen + 1 >= head->cap)
	{ *arr = (char *)ahd__grow(*arr-hdr_size, strLen + 1, el_size, hdr_size); }

	AHD_MEMMOVE(*arr + len, str, strLen);

	*arr[len+strLen] = '\0';

	return len;
}
#endif

/******************************************************************************/
/* Removing elements **********************************************************/
/******************************************************************************/
#define ahd_pop(ht,a)         ((a)[--ahd__len(ht,a)])
#define ahd_shift(ht,a)       ahd_pull(ht,a,0)
#define ahd_clear(ht,a)       ahd_if(a, ahd__len(ht,a) = 0)
#define ahd_resetlen(ht,a,n)  (ahd_clear(ht,a), ahd_add(ht,a,n))
#define ahd_remove(ht,a,i,n)   AHD_MEMMOVE((a)+(i), (a)+(i)+(n), ((ahd__len(ht,a)-=(n))-(i)) * sizeof(*(a)) )
// NOTE: (similar to delix)
#define ahd_removeswap(ht,a,i) ((a)[i] = (a)[--ahd__len(ht,a)])
// Remove and return value
// this should work as expected as long as you don't wrap in parens: x = (ahd_pull(ahd_arr, arr, 3));
#define ahd_pull(ht,a,i)       (a)[i], ahd_remove(ht,a,i,1)

/******************************************************************************/
/* Array metadata *************************************************************/
/******************************************************************************/
#define ahd_len(ht,a)           ahd_if(a, ahd__len(ht,a))
#define ahd_cap(ht,a)           ahd_if(a, ahd__cap(ht,a))
#define ahd_size(ht,a)          ahd_if(a, ahd__size(ht,a))
#define ahd_capsize(ht,a)       ahd_if(a, ahd__capsize(ht,a))
#define ahd_totalsize(ht,a)     ahd_if(a, ahd__totalsize(ht,a))
#define ahd_totalcapsize(ht,a)  ahd_if(a, ahd__totalcapsize(ht,a))

#define ahd_last(ht,a)        ((a)[ahd__len(ht,a)-1])

#define ahd_hdr(ht, a)        ((ht *)(a) - 1)

// these don't check if array is non-NULL
#define ahd__len(ht,a)          (ahd_hdr(ht,a)->len)
#define ahd__cap(ht,a)          (ahd_hdr(ht,a)->cap)
#define ahd__size(ht,a)         (ahd__len(ht,a) * sizeof(*(a)) )
#define ahd__capsize(ht,a)      (ahd__cap(ht,a) * sizeof(*(a)) )
#define ahd__totalsize(ht,a)    (ahd__len(ht,a) * sizeof(*(a)) + sizeof(ht) )
#define ahd__totalcapsize(ht,a) (ahd__cap(ht,a) * sizeof(*(a)) + sizeof(ht) )

// sets the array back to NULL, so any attempts to access contents fail, or the array can be pushed to again
#define ahd_free(ht,a)        (ahd_if(a, (AHD_FREE(ahd_hdr(ht,a)),0)), (a) = 0)
#define ahd_free2dt(ht,ht2,a)    do { \
		for(ahd_int ahd_i_ln = 0; ahd_i_ln < ahd_len(ht,a); ++ahd_i_ln) \
		{ ahd_free(ht2,(a)[ahd_i_ln]); } \
        ahd_free(ht,a); \
	} while(0)
#define ahd_free2d(ht,a)     ahd_free2dt(ht,ht,a)
// 3D?

static void ahd__free2d(void *outer, ahd_int el_size_outer, ahd_int hdr_size_outer, ahd_int hdr_size_inner)
#ifdef AHD_IMPLEMENTATION
{
	char *p = (char *)outer;
	if(p) {
		ahd_arr *head = (ahd_arr *)(p - hdr_size_outer);
		ahd_int len = head->len;
		ahd_int i = 0;
		for(; i < len; ++i, p += el_size_outer) {
			char *p2 = *(char **)p;
			if(p2)
			{ AHD_FREE(p2 - hdr_size_inner); }
		}
		AHD_FREE(head);
	}
}
#else
;
#endif// AHD_IMPLEMENTATION

#define ahd_union(ht,a,b)
#define ahd_intersection(ht,a,b)

#define ahd_reverse(ht,a) ahd__reverse(ahd__data(ht,a))

static void ahd__memswap(void *el_a, void *el_b, ahd_int size) {
	char *a = (char *)el_a,
		 *b = (char *)el_b;
	for(;size--; ++a, ++b) {
		char t = *a;
		*a = *b;
		*b = t;
	}
}

static void ahd__reverse(void *arr, ahd_int hdr_size, ahd_int el_size)
{
	char *a = (char *)arr;
	ahd_int len = ((ahd_arr *)(a - hdr_size))->len;
	char *at  = a,
		 *mid = a + (len / 2) * el_size,
		 *ta  = a + (len - 1) * el_size;

	for(; at < mid; at += el_size, ta -= el_size)
	{ ahd__memswap(at, ta, el_size); }
}

/******************************************************************************/
/* Extracting array subsets****************************************************/
/******************************************************************************/
#define ahd_dup(ht,a)       ahd__dup(ahd__data(ht,a))

#define ahd_sub(ht,a,f,n)    ahd__sub(ahd__data(ht,a), f, n)
// TODO: refactor with similar fns
static void *
AHD_DBG(ahd__dup, void *arr, ahd_int hdr_size, ahd_int el_size) {
	ahd_arr *head          = (ahd_arr *)((char *)arr - hdr_size);
	ahd_int total_cap_size = hdr_size + head->cap * el_size;
	ahd_arr *new_head      = (ahd_arr *) AHD_REALLOC(0, total_cap_size);
	if (head)
	{ return (char *)AHD_MEMMOVE(new_head, head, total_cap_size) + hdr_size; }
	else {
#ifdef AHD_BUFFER_OUT_OF_MEMORY
		AHD_BUFFER_OUT_OF_MEMORY ;
#endif
		return (char*)(uintptr_t)hdr_size; // try to force a NULL pointer exception later
	}

}

static void *ahd__sub(void *arr, ahd_int hdr_size, ahd_int el_size, ahd_int first, ahd_int n) {
	char *new_arr = (char *)ahd__grow(0, n, el_size, hdr_size);
	ahd_arr *head = (ahd_arr *)(new_arr - hdr_size);
	head->len = n;
	AHD_MEMMOVE(new_arr, ((char *)arr + first*el_size), n*el_size);
	return new_arr;
}

#define ahd_slice()
// TODO: don't repeat i
#define ahd_unique(ht,a)

#define ahd_bc(ht,a,i) ahd__bc(ahd_len(ht,a), i)
static inline int ahd__bc(ahd_int len, ahd_int i) { return i >= 0 && i < len; }

#define ahd_get(ht,a,i) (a)[ahd__get(ahd__len(ht,a), i)]

#ifndef AHD_ASSERT
#define AHD_ASSERT(x) ((x) ? 1 : *(volatile int *)0 == 0)
#endif/*AHD_ASSERT*/

#ifndef AHD_BOUNDS_ASSERT
#define AHD_BOUNDS_ASSERT(x) AHD_ASSERT(x)
#endif/*AHD_BOUNDS_ASSERT*/

// negative numbers allow access from end
static inline ahd_int
ahd__get(ahd_int len, signed long long i) {
	ahd_int index = (ahd_int)((i >= 0) ? i
	                                   : len + i);
	AHD_BOUNDS_ASSERT(ahd__bc(len, index));
	return index;
}

/******************************************************************************/
/* Array element rearranging **************************************************/
/******************************************************************************/
#define ahd_is_signed(v) (~((v)^(v))<0)

#define ahd__sortx(x,ht,a,mem,dir) \
	ahd__sort##x(ahd__data(ht,a), mem, (ahd_int)sizeof(*(mem)), dir)

#define ahd_sorti(ht,a,mem,dir)   ahd__sortx(i,ht,a,mem,dir)
#define ahd_sortu(ht,a,mem,dir)   ahd__sortx(u,ht,a,mem,dir)
#define ahd_sortint(ht,a,mem,dir) ahd__sortx(int[ahd_is_signed(*(mem))],ht,a,mem,dir)
#define ahd_sortf(ht,a,mem,dir)   ahd__sortx(f,ht,a,mem,dir)

#define AHD_STACK_BUF_SIZE 256

#define ahd__max(a,b) ((a) >= (b) ? (a) : (b))
#define ahd__min(a,b) ((a) <  (b) ? (a) : (b))

#define ahd_rotr(ht,a,n) ahd__memrotr(a, sizeof(*(a)), ahd_len(ht,a), n)

/* TODO: add temp var ptr? */
static void ahd__memrotr(void *mem, ahd_int el_size, ahd_int len, ahd_int rot_n) {
	/* TODO: how does this compare to just allocating and freeing the memory */
	char buf[AHD_STACK_BUF_SIZE];
	ahd_int size           = el_size * rot_n,
	        n_bytes        = (len - rot_n)*el_size,
	        max_size       = ahd__min(size, sizeof(buf)),
			size_remaining = size,
			size_copied    = 0,
			cpy_size       = ahd__min(size_remaining, max_size);

	for(; size_remaining;
		size_remaining -= (cpy_size = ahd__min(size_remaining, max_size)),
		size_copied += cpy_size)
	{
		char *bytes = (char *)mem + size_copied;
		AHD_MEMMOVE(buf, bytes + n_bytes, cpy_size);
		AHD_MEMMOVE(bytes + cpy_size, bytes, n_bytes);
		AHD_MEMMOVE(bytes, buf, cpy_size);
	}
}

typedef enum ahd_sort_dir { ahd_ASC = 1, ahd_DESC = -1 } ahd_sort_dir;
typedef enum ahd_sort_type {
	/* sizeof char      == 1 << 0, */
	/* sizeof short     == 1 << 1, */
	/* sizeof int       == 1 << 2, */
	/* sizeof long long == 1 << 3, */
	ahd_INT  = 1 << 4,
	ahd_FLT  = 1 << 5,
	ahd_STR  = 1 << 6,
	ahd_CHR  = 1 << 7, /* for local string arrays */
	ahd_SIGNSHIFT = 8,
	ahd_SIGN = 1 << ahd_SIGNSHIFT,
} ahd_sort_type;

/* TODO: sort_ _int, _flt, _str, _chr, _fn*/

static int
ahd__sorti(void *array, ahd_int hdr_size, ahd_int el_size, void *member, ahd_int member_size, ahd_sort_dir dir)
{
	int result = 1;
	char *arr = (char *)array;
	ahd_arr *head = (ahd_arr *)(arr - hdr_size);
	ahd_int len = head->len,
			member_offset = (ahd_int)((char *)member - arr);
	char *guard = arr + len * el_size, *at, *a, *b;

	/* insertion sort */
	for(at = arr; at + el_size < guard; at += el_size)
	{
		for(a = at + member_offset; ; a -= el_size)
		{
			int cmp = 0;
			b = a + el_size;
			switch(member_size) {
				case sizeof (char): {
					char A = *(char *)a, B = *(char *)b;
					cmp = A > B ? 1 : A < B ? -1 : 0;
				} break;

				case sizeof (short): {
					short A = *(short *)a, B = *(short *)b;
					cmp = A > B ? 1 : A < B ? -1 : 0;
				} break;

				case sizeof (int): {
					int A = *(int *)a, B = *(int *)b;
					cmp = A > B ? 1 : A < B ? -1 : 0;
				} break;

				case sizeof (long long): {
					long long A = *(long long *)a, B = *(long long *)b;
					cmp = A > B ? 1 : A < B ? -1 : 0;
				} break;

				/* case sizeof (char *):               cmp = strcmp(*(char **)a, *(char **)b);          break; */
				/* case sizeof (char *):               cmp = strncmp((char *)a, (char *)b, str_len);          break; */

				default: result = 0;
			}

			if(cmp * dir > 0) {
				// TODO: speed up with memmove (don't swap all of them)
				ahd__memswap(a-member_offset, b-member_offset, el_size);
				if(a == arr) { break; }
			} else break;
		}
	}

	return result;
}

static int
ahd__sortu(void *array, ahd_int hdr_size, ahd_int el_size, void *member, ahd_int member_size, ahd_sort_dir dir)
{
	int result = 1;
	char *arr = (char *)array;
	ahd_arr *head = (ahd_arr *)(arr - hdr_size);
	ahd_int len = head->len,
			member_offset = (ahd_int)((char *)member - arr);
	char *guard = arr + len * el_size, *at, *a, *b;

	/* insertion sort */
	for(at = arr; at + el_size < guard; at += el_size)
	{
		for(a = at + member_offset; ; a -= el_size)
		{
			int cmp = 0;
			b = a + el_size;

			switch(member_size) {
				case sizeof (unsigned char): {
					unsigned char A = *(char *)a, B = *(char *)b;
					cmp = A > B ? 1 : A < B ? -1 : 0;
				} break;

				case sizeof (unsigned short): {
					unsigned short A = *(short *)a, B = *(short *)b;
					cmp = A > B ? 1 : A < B ? -1 : 0;
				} break;

				case sizeof (unsigned int): {
					unsigned int A = *(int *)a, B = *(int *)b;
					cmp = A > B ? 1 : A < B ? -1 : 0;
				} break;

				case sizeof (unsigned long long): {
					unsigned long long A = *(long long *)a, B = *(long long *)b;
					cmp = A > B ? 1 : A < B ? -1 : 0;
				} break;

				default: result = 0;
			}

			if(cmp * dir > 0) {
				// TODO: speed up with memmove (don't swap all of them)
				ahd__memswap(a-member_offset, b-member_offset, el_size);
				if(a == arr) { break; }
			} else break;
		}
	}

	return result;
}

/* USAGE: ahd__sortint[is_signed(val)](...) */
typedef int (*ahd__sort_int_t)(void *, ahd_int, ahd_int, void *, ahd_int, ahd_sort_dir);
ahd__sort_int_t ahd__sortint[2] = { ahd__sortu, ahd__sorti };

static int
ahd__sortf(void *array, ahd_int hdr_size, ahd_int el_size, void *member,  ahd_int member_size, ahd_sort_dir dir)
{
	int result = 1;
	char *arr = (char *)array;
	ahd_arr *head = (ahd_arr *)(arr - hdr_size);
	ahd_int len = head->len,
			member_offset = (ahd_int)((char *)member - arr);
	char *guard = arr + len * el_size, *at, *a, *b;
	double Dir = (double)dir;

	/* insertion sort */
	for(at = arr; at + el_size < guard; at += el_size) {
		for(a = at + member_offset; ; a -= el_size) {
			double cmp = 0.0;
			b = a + el_size;
			switch(member_size) {
				case sizeof (float):  cmp = (double)(*(float *)a  - *(float  *)b); break;
				case sizeof (double): cmp =          *(double *)a - *(double *)b;  break;
			}

			if(cmp * Dir > 0.0) {
				// TODO: speed up with memmove (don't swap all of them)
				ahd__memswap(a - member_offset, b - member_offset, el_size);
				if(a == arr) { break; }
			} else break;
		}
	}

	return result;
}

#define ahd_eq(ht,a,b) (sizeof(*(a))  == sizeof(*(b))  && \
                        ahd_len(ht,a) == ahd_len(ht,b) && \
                        AHD_MEMCMP((a), (b), ahd_size(ht,a)) == 0)

#define ahd_foronce(cond) ahd_n_ln = 0; cond && ! ahd_n_ln

#ifdef AHD_NO_DECLS
#define ahd_decl(t)
#else
#define ahd_decl(t) t
#endif

// TODO: regularize naming:
//  - _i => just index
//  - no suffix => struct with .v and .i
#define ahd_each(ht,a,i) \
	(ahd_decl(ahd_int) i = 0; i < ahd_len(ht,a); ++i)
#define ahd_each_r(ht,a,i) \
	(ahd_decl(ahd_int) i = ahd_len(ht,a); i-- != 0 ;)

#define ahd_each_v(ht,a,i,t,v) \
	(ahd_decl(ahd_int) i = 0, ahd_foronce(ahd_len(ht,a))++;) \
	for(t v = (a)[i]; i < ahd_len(ht,a); v = (a)[++i])
#define ahd_each_rv(ht,a,i,t,v) \
	(ahd_int i##_ = ahd_len(ht,a), i = i##_-1, ahd_foronce(ahd_len(ht,a))++;) \
	for(t v = (a)[i]; i##_ != 0; i = --i##_-1, v = (a)[ahd_if(i##_, i)])

// NOTE: may break if you push to it during iteration
#define ahd_all(t, v, a) \
    (t v = (a); v < (a) + ahd_len(a); ++v)

// statement
// should this create new array? map to 2nd array? change in place?
#define ahd_mapt(ht,i,ta,a,va,tb,b,vb) if(a) \
			for(ahd_decl(ahd_int) i = ((a) == (b) || ahd_resetlen(ht, b, ahd__len(ht,a)), 0), ahd_foronce(1);) \
			for(t vb = {0}; ! ahd_n_ln++; ) \
			for(t va = (a)[i]; i < ahd__len(ht,a); (b)[i] = vb, ++i, va = (a)[i])
// for when the type remains the same:
#define ahd_mapv(ht,a,b,i,t,v) if(a) \
			for(ahd_decl(ahd_int) i = (a) == (b) || (ahd_resetlen(ht, b, ahd__len(ht,a)), 0), ahd_foronce(1)++;) \
			for(t v = {0}; i < ahd__len(ht,a); (b)[i] = v, memset(&v,0,sizeof(v)), ++i)
// TODO ahd_map(ht,a,b)
/* Usage:
 * int *lengths = 0;
 * arr_mapt(i_str, char *, strings, str, int, lengths, len)
 * { len = strlen(str); }
 *
 * int *lengths = 0;
 * arr_mapt(i_str, strings, lengths, int, len)
 * { len = strlen(strings[i_str]); }

 * int *lengths = 0;
 * arr_map(i_str, strings, lengths)
 * { lengths[i_str] = strlen(strings[i_str]); }
 */

// expression
#define ahd_mapfn(ht,fn,a,b,udata) (ahd_resetlen(ht, b, ht__len(ht,a)), \
		                            ahd__map(a, sizeof(*(a)), b, sizeof(*(b)), sizeof(ht), ahd_len(a), fn, udata))

typedef struct ahd_any {
	char *base;
	ahd_int hdr_size;
	ahd_int el_size;
} ahd_any;

typedef struct ahd_loop_info {
	ahd_int i;
	ahd_int n;
	void *udata;
	ahd_any a, b;
} ahd_loop_info;
typedef int ahd_loopfn(ahd_loop_info *info, void *a, void *b);

static void ahd__map(void *arr_a, ahd_int el_size_a, void *arr_b, ahd_int el_size_b,
                     ahd_int hdr_size, ahd_int len, ahd_loopfn fn, void *udata)
{
	char *a = (char *)arr_a, *b = (char *)arr_b;
	ahd_loop_info info = { 0 };
	info.n = len,     info.udata = udata;
	info.a.base = a,  info.a.hdr_size = hdr_size, info.a.el_size = el_size_a;
	info.b.base = b,  info.b.hdr_size = hdr_size, info.b.el_size = el_size_b;

	for(; info.i < len; ++info.i, a += el_size_a, b += el_size_b)
	{ fn(&info, a, b); }
}
/* Usage:
 * struct str{ char *str; int len; }

 * int map_strwlen(ahd_loop_info info, void *a, void *b, void *udata) {
 *     info; // UNUSED
 *     char **str = a;
 *     struct str *val = b;
 *     struct str new_val = { *str, strlen(*str) };
 *     *val = new_val;
 *     return 0;
 * }
 * ...
 * arr_mapfn(map_strwlen, strings, strs, 0);
 */

#define ahd_reduce(ht,a,ex)
#define ahd_reducefn(ht,fn,a,acc,udata) ahd__reduce(acc, sizeof(acc), a, sizeof(*(a)), sizeof(ht), ahd_len(ht,a), fn, udata)

static void ahd__reduce(void *accum, ahd_int el_size_acc,
		         void *value, ahd_int el_size_val, ahd_int hdr_size,
		         ahd_int len, ahd_loopfn fn, void *udata)
{
	char *acc = (char *)accum,
	     *val = (char *)value;
	ahd_loop_info info = { 0 };
	info.n = len;
	info.udata = udata;
	info.a.base = acc,  info.a.el_size = el_size_acc,  info.a.hdr_size = 0;
	info.b.base = val,  info.b.el_size = el_size_val,  info.b.hdr_size = hdr_size;
	for(; info.i < len; ++info.i, val += el_size_val)
	{ fn(&info, acc, val); }
}
/* Usage:
 * int reduce_sumstrlen(ahd_loop_info info, void *acc, void *val) {
 *     info; // UNUSED
 *     char **str = val;
 *     int *sum_len = acc;
 *     *sum_len += strlen(*str);
 *     return 0;
 * }
 * ...
 * arr_reducefn(reduce_sumstrlen, strings, sum_len, 0);
 */

#define ahd_filterreject(ht,i,t,v,a,b,tr,reset,test) if(a) \
			for(ahd_decl(ahd_int) i = (ahd_clear(ht,b), 0), (reset), ahd_foronce(1)++;) \
			for(ahd_decl(t) v = (a)[i]; \
				i < ahd__len(a); \
				(test) && ahd_push(ht,b,(a)[i]), ++i, (reset))
#define ahd_filter(ht,i,t,v,a,b,tr) ahd_filterreject(ht,i,t,v,a,b,tr,tr=0, tr)
#define ahd_reject(ht,i,t,v,a,b,tr) ahd_filterreject(ht,i,t,v,a,b,tr,tr=1,!tr)
#define ahd_countx(ht,a,i,x,tr) \
	for(ahd_decl(ahd_int) i = (x) = 0; i < arr_len(a); (x) += !!(tr), ++i)

/* Usage:
 * obj_t *filtered_objects = 0;
 * arr_filter(objects, i, obj_t, obj, objects, filtered_objects, found)
 * { found = obj.is_valid; }
 * printf("Original length: %d, filtered length: %d", arr_len(objects), arr_len(filtered_objects));
 *
 * arr_filter(objects, i, objects, filtered_objects, found)
 * { found = objects[i].is_valid; }
 * printf("Original length: %d, filtered length: %d", arr_len(objects), arr_len(filtered_objects));
 */

/* #define ahd_reject(ht,i,t,a,b,tr) if(a) \ */
/* 			for(ahd_decl(ahd_int) tr = 0, i = (ahd_clear(ht,b), 0); \ */
/* 				i < ahd__len(a); \ */
/* 				!tr && ahd_push(ht,b,(a)[i]), ++i, tr = 1) */

#define ahd_filterfn()
static void ahd__filter(void *arr, void *out, ahd_int len, ahd_int el_size, ahd_int hdr_size, ahd_loopfn fn, void *udata) {
	char *a = (char *)arr;
	ahd_loop_info info = { 0 };
	info.n = len,     info.udata = udata;
	info.a.base = a,  info.a.el_size = el_size,  info.a.hdr_size = hdr_size;

	for(; info.i < len; ++info.i, a += el_size)
	{
		if(fn(&info, a, out))
		{
			//TODO
		}
	}
}

#define ahd_split()
#define ahd_join()
// NOTE: should be able to have the index and found bools as internally or externally scoped
// due to the way that comma op works (?)
#define ahd_find(ht,a,i,t,v,fnd,tr) if(a) \
			for(ahd_int AHD_LN(ahd_dummy), i = 0, tr = 0, (fnd) = 0; \
				i < ahd__len(ht,a) && !(fnd); \
				tr ? (ahd_if(fnd, (fnd)=&(a)[i]), i) : ++i)

#define ahd_findi(ht,a,i,t,v,tr)   ahd_find(ht, a, i,        t, v, 0,   tr)
#define ahd_findv(ht,a,t,v,fnd,tr) ahd_find(ht, a, ahd_i_ln, t, v, fnd, tr)

#define ahd_findlast(ht,a,i,t,v,fnd,tr)
/* Usage:
 * f32 *greater_than3;
 * ahd_find(ahd_arr, arr, _, f32, val, greater_than3, found)
 * { found = val > 3.f; }
 * if(greater_than3) { assert(*greater_than3 > 3.f); }
 *
 * int found, found_index;
 * ahd_find(ahd_arr, arr, found_index, f32, val, 0, found)
 * { found = val > 3.f && found_index % 2; }
 * if(found) { assert(arr[found_index] > 3.f); }

 * int found, i_obj;
 * my_findi(objects, i_obj, obj_t, obj, found)
 * { found = obj.is_valid; }
 * if(found) { assert(objects[i_obj].is_valid); }
 */

#define ahd_exitscope continue
// NOTE: adding to NULL ptr is technically UB
#define ahd_scope(init,end)            for(init, *ahd_n_ln = 0; ! ahd_n_ln++; end)
#define ahd_scoped_init(ht,t,a,init)   ahd_scope(t *a = (init), (ahd_free(ht,a),0))
#define ahd_scoped(ht,t,a)             ahd_scope(t *a = 0,      (ahd_free(ht,a),0))
#define ahd_scoped2d_init(ht,t,a,init) ahd_scope(t ** a = (init), (ahd_free2d(ht,a),0))
#define ahd_scoped2d(ht,t,a)           ahd_scope(t ** a = 0,      (ahd_free2d(ht,a),0))
/* Usage:
 * arr_scoped(char *, strings)
 * {
 *     arr_push(strings, "Hello");
 *     arr_push(strings, "World");
 *     printf(strings[0]);
 *     if(strings[1][0] == 'W')
 *     { ahd_exitscope; }
 *
 *     puts(strings[1]);
 * } // no leaks
 *
 * puts(Strings[1]); // fails at compile time - Strings has not been declared
 *
 * // stackable (don't semicolon)
 * ahd_scoped(ahd_arr, char *, Strings)
 * ahd_scoped(ahd_arr, int, Ints)
 *
 */

/* // TODO: make atomic */
/* static ahd_int ahd_rc_inc(ahd_rc *rc) */
/* { return ++rc->rc; } */

/* static ahd_int ahd_rc_dec(ahd_rc *rc, void *raw) { */
/* 	void (*freefn)(void*) = rc->free ? rc->free : AHD_FREE; */
/* 	if(--rc->rc == 0) */
/* 	{ */
/*         (rc->free */
/*          ? rc->free(raw) */
/*          : AHD_FREE(raw)); */
/*     } */
/*     return rc->rc; */
/* } */

#ifdef AHD_IMPLEMENTATION
#include <stdarg.h>
// TODO: variants:
//          - append to current string
//          - append a separate string (separated by '\0')
//          - rewrite
//          - from_offset (covers above 2)
static size_t
arr_vprintf(char *arr[], char const *fmt, va_list args)
{
    AHD_ASSERT(arr);

    size_t len             = arr_len(*arr);
    size_t zero_term       = !!(len > 0 && arr[0][len-1] == '\0');
    size_t cat_start       = len - zero_term;
    size_t chars_available = arr_cap(*arr) - cat_start;

    size_t chars_required = AHD_VSNPRINTF(NULL, 0, fmt, args) + 1; // zero terminator
    arr_add(*arr, chars_required - zero_term);

    {
        chars_available = arr_cap(*arr) - cat_start;
        size_t chars_printed = AHD_VSNPRINTF(*arr + cat_start, chars_available, fmt, args);
        AHD_ASSERT((chars_required - zero_term) <= chars_available && "didn't grow enough?");
    }

    if (*arr)
    {   arr_last(*arr) = '\0';   }

    return chars_required;
}

// TODO: appendf vs replacef vs print_at_offset_f
static int
arr_printf(char *arr[], char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int result = (int)arr_vprintf(arr, fmt, args);
    va_end(args);
    return result;
}

static int
arr_puts_n(const char *str, size_t str_len, char *arr[])
{
    AHD_ASSERT(arr);
    size_t chars_n   = str_len + 1; // zero terminator
    size_t len       = arr_len(*arr);
    int    zero_term = len > 0 && arr[0][len-1] == '\0';

    size_t offset  = arr_add(*arr, chars_n - zero_term) - zero_term;

    AHD_MEMMOVE(*arr + offset, str, str_len);
    arr_last(*arr) = '\0';

    return (int)chars_n;
}

static int
arr_puts(const char *str, char *arr[]) // TODO: restrict ptrs
{   return arr_puts_n(str, strlen(str), arr);   }

static int
arr_putc(int ch, char *arr[])
{
    size_t len       = arr_len(*arr);
    int    zero_term = len > 0 && arr[0][len-1] == '\0';

    if (zero_term)
    {   arr[0][len-1] = (char)ch;   } // overwrite the null terminator
    else
    {   arr_push(*arr, (char)ch);   } // append the new character

    arr_push(*arr, '\0');
    return ch;
}

static void
arr_rewind(char *arr[])
{   arr_clear(*arr);   }

typedef struct ArrFile {
    int  (*const printf)(char *arr[], char const *fmt, ...);
    int  (*const puts  )(char const *str, char *arr[]);
    int  (*const putc  )(int ch, char *arr[]);
    void (*const rewind)(char *arr[]);
} ArrFile;

static ArrFile const Arr_File = {
    arr_printf,
    arr_puts,
    arr_putc,
    arr_rewind,
};
#endif//AHD_IMPLEMENTATION

#define AHD_INCLUDED
#endif/*AHD_INCLUDED*/

#ifdef __cplusplus
}
#endif
/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

/*** End of inlined file: airhead.h ***/

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_DECORATE(name) s##name

/*** Start of inlined file: stb_sprintf.h ***/
// stb_sprintf - v1.09 - public domain snprintf() implementation
// originally by Jeff Roberts / RAD Game Tools, 2015/10/20
// http://github.com/nothings/stb
//
// allowed types:  sc uidBboXx p AaGgEef n
// lengths      :  hh h ll j z t I64 I32 I
//
// Contributors:
//    Fabian "ryg" Giesen (reformatting)
//
// Contributors (bugfixes):
//    github:d26435
//    github:trex78
//    github:account-login
//    Jari Komppa (SI suffixes)
//    Rohit Nirmal
//    Marcin Wojdyr
//    Leonard Ritter
//    Stefano Zanotti
//    Adam Allison
//    Arvid Gerstmann
//    Markus Kolb
//
// LICENSE:
//
//   See end of file for license information.

#ifndef STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_H_INCLUDE

/*
Single file sprintf replacement.

Originally written by Jeff Roberts at RAD Game Tools - 2015/10/20.
Hereby placed in public domain.

This is a full sprintf replacement that supports everything that
the C runtime sprintfs support, including float/double, 64-bit integers,
hex floats, field parameters (%*.*d stuff), length reads backs, etc.

Why would you need this if sprintf already exists?  Well, first off,
it's *much* faster (see below). It's also much smaller than the CRT
versions code-space-wise. We've also added some simple improvements
that are super handy (commas in thousands, callbacks at buffer full,
for example). Finally, the format strings for MSVC and GCC differ
for 64-bit integers (among other small things), so this lets you use
the same format strings in cross platform code.

It uses the standard single file trick of being both the header file
and the source itself. If you just include it normally, you just get
the header file function definitions. To get the code, you include
it from a C or C++ file and define STB_SPRINTF_IMPLEMENTATION first.

It only uses va_args macros from the C runtime to do it's work. It
does cast doubles to S64s and shifts and divides U64s, which does
drag in CRT code on most platforms.

It compiles to roughly 8K with float support, and 4K without.
As a comparison, when using MSVC static libs, calling sprintf drags
in 16K.

API:
====
int stbsp_sprintf( char * buf, char const * fmt, ... )
int stbsp_snprintf( char * buf, int count, char const * fmt, ... )
  Convert an arg list into a buffer.  stbsp_snprintf always returns
  a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintf( char * buf, char const * fmt, va_list va )
int stbsp_vsnprintf( char * buf, int count, char const * fmt, va_list va )
  Convert a va_list arg list into a buffer.  stbsp_vsnprintf always returns
  a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintfcb( STBSP_SPRINTFCB * callback, void * user, char * buf, char const * fmt, va_list va )
    typedef char * STBSP_SPRINTFCB( char const * buf, void * user, int len );
  Convert into a buffer, calling back every STB_SPRINTF_MIN chars.
  Your callback can then copy the chars out, print them or whatever.
  This function is actually the workhorse for everything else.
  The buffer you pass in must hold at least STB_SPRINTF_MIN characters.
    // you return the next buffer to use or 0 to stop converting

void stbsp_set_separators( char comma, char period )
  Set the comma and period characters to use.

FLOATS/DOUBLES:
===============
This code uses a internal float->ascii conversion method that uses
doubles with error correction (double-doubles, for ~105 bits of
precision).  This conversion is round-trip perfect - that is, an atof
of the values output here will give you the bit-exact double back.

One difference is that our insignificant digits will be different than
with MSVC or GCC (but they don't match each other either).  We also
don't attempt to find the minimum length matching float (pre-MSVC15
doesn't either).

If you don't need float or doubles at all, define STB_SPRINTF_NOFLOAT
and you'll save 4K of code space.

64-BIT INTS:
============
This library also supports 64-bit integers and you can use MSVC style or
GCC style indicators (%I64d or %lld).  It supports the C99 specifiers
for size_t and ptr_diff_t (%jd %zd) as well.

EXTRAS:
=======
Like some GCCs, for integers and floats, you can use a ' (single quote)
specifier and commas will be inserted on the thousands: "%'d" on 12345
would print 12,345.

For integers and floats, you can use a "$" specifier and the number
will be converted to float and then divided to get kilo, mega, giga or
tera and then printed, so "%$d" 1000 is "1.0 k", "%$.2d" 2536000 is
"2.53 M", etc. For byte values, use two $:s, like "%$$d" to turn
2536000 to "2.42 Mi". If you prefer JEDEC suffixes to SI ones, use three
$:s: "%$$$d" -> "2.42 M". To remove the space between the number and the
suffix, add "_" specifier: "%_$d" -> "2.53M".

In addition to octal and hexadecimal conversions, you can print
integers in binary: "%b" for 256 would print 100.

PERFORMANCE vs MSVC 2008 32-/64-bit (GCC is even slower than MSVC):
===================================================================
"%d" across all 32-bit ints (4.8x/4.0x faster than 32-/64-bit MSVC)
"%24d" across all 32-bit ints (4.5x/4.2x faster)
"%x" across all 32-bit ints (4.5x/3.8x faster)
"%08x" across all 32-bit ints (4.3x/3.8x faster)
"%f" across e-10 to e+10 floats (7.3x/6.0x faster)
"%e" across e-10 to e+10 floats (8.1x/6.0x faster)
"%g" across e-10 to e+10 floats (10.0x/7.1x faster)
"%f" for values near e-300 (7.9x/6.5x faster)
"%f" for values near e+300 (10.0x/9.1x faster)
"%e" for values near e-300 (10.1x/7.0x faster)
"%e" for values near e+300 (9.2x/6.0x faster)
"%.320f" for values near e-300 (12.6x/11.2x faster)
"%a" for random values (8.6x/4.3x faster)
"%I64d" for 64-bits with 32-bit values (4.8x/3.4x faster)
"%I64d" for 64-bits > 32-bit values (4.9x/5.5x faster)
"%s%s%s" for 64 char strings (7.1x/7.3x faster)
"...512 char string..." ( 35.0x/32.5x faster!)
*/

#if defined(__clang__)
 #if defined(__has_feature) && defined(__has_attribute)
  #if __has_feature(address_sanitizer)
   #if __has_attribute(__no_sanitize__)
    #define STBSP__ASAN __attribute__((__no_sanitize__("address")))
   #elif __has_attribute(__no_sanitize_address__)
    #define STBSP__ASAN __attribute__((__no_sanitize_address__))
   #elif __has_attribute(__no_address_safety_analysis__)
    #define STBSP__ASAN __attribute__((__no_address_safety_analysis__))
   #endif
  #endif
 #endif
#elif __GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
 #if __SANITIZE_ADDRESS__
  #define STBSP__ASAN __attribute__((__no_sanitize_address__))
 #endif
#endif

#ifndef STBSP__ASAN
#define STBSP__ASAN
#endif

#ifdef STB_SPRINTF_STATIC
#define STBSP__PUBLICDEC static
#define STBSP__PUBLICDEF static STBSP__ASAN
#else
#ifdef __cplusplus
#define STBSP__PUBLICDEC extern "C"
#define STBSP__PUBLICDEF extern "C" STBSP__ASAN
#else
#define STBSP__PUBLICDEC extern
#define STBSP__PUBLICDEF STBSP__ASAN
#endif
#endif

#include <stdarg.h> // for va_list()
#include <stddef.h> // size_t, ptrdiff_t

#ifndef STB_SPRINTF_MIN
#define STB_SPRINTF_MIN 512 // how many characters per callback
#endif
typedef char *STBSP_SPRINTFCB(const char *buf, void *user, int len);

#ifndef STB_SPRINTF_DECORATE
#define STB_SPRINTF_DECORATE(name) stbsp_##name // define this before including if you want to change the names
#endif

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsnprintf)(char *buf, int count, char const *fmt, va_list va);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...);

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char comma, char period);

#endif // STB_SPRINTF_H_INCLUDE

#ifdef STB_SPRINTF_IMPLEMENTATION

#include <stdlib.h> // for va_arg()

#define stbsp__uint32 unsigned int
#define stbsp__int32 signed int

#ifdef _MSC_VER
#define stbsp__uint64 unsigned __int64
#define stbsp__int64 signed __int64
#else
#define stbsp__uint64 unsigned long long
#define stbsp__int64 signed long long
#endif
#define stbsp__uint16 unsigned short

#ifndef stbsp__uintptr
#if defined(__ppc64__) || defined(__powerpc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64)
#define stbsp__uintptr stbsp__uint64
#else
#define stbsp__uintptr stbsp__uint32
#endif
#endif

#ifndef STB_SPRINTF_MSVC_MODE // used for MSVC2013 and earlier (MSVC2015 matches GCC)
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define STB_SPRINTF_MSVC_MODE
#endif
#endif

#ifdef STB_SPRINTF_NOUNALIGNED // define this before inclusion to force stbsp_sprintf to always use aligned accesses
#define STBSP__UNALIGNED(code)
#else
#define STBSP__UNALIGNED(code) code
#endif

#ifndef STB_SPRINTF_NOFLOAT
// internal float utility functions
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits);
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value);
#define STBSP__SPECIAL 0x7000
#endif

static char stbsp__period = '.';
static char stbsp__comma = ',';
static struct
{
   short temp; // force next field to be 2-byte aligned
   char pair[201];
} stbsp__digitpair =
{
  0,
   "00010203040506070809101112131415161718192021222324"
   "25262728293031323334353637383940414243444546474849"
   "50515253545556575859606162636465666768697071727374"
   "75767778798081828384858687888990919293949596979899"
};

STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char pcomma, char pperiod)
{
   stbsp__period = pperiod;
   stbsp__comma = pcomma;
}

#define STBSP__LEFTJUST 1
#define STBSP__LEADINGPLUS 2
#define STBSP__LEADINGSPACE 4
#define STBSP__LEADING_0X 8
#define STBSP__LEADINGZERO 16
#define STBSP__INTMAX 32
#define STBSP__TRIPLET_COMMA 64
#define STBSP__NEGATIVE 128
#define STBSP__METRIC_SUFFIX 256
#define STBSP__HALFWIDTH 512
#define STBSP__METRIC_NOSPACE 1024
#define STBSP__METRIC_1024 2048
#define STBSP__METRIC_JEDEC 4096

static void stbsp__lead_sign(stbsp__uint32 fl, char *sign)
{
   sign[0] = 0;
   if (fl & STBSP__NEGATIVE) {
      sign[0] = 1;
      sign[1] = '-';
   } else if (fl & STBSP__LEADINGSPACE) {
      sign[0] = 1;
      sign[1] = ' ';
   } else if (fl & STBSP__LEADINGPLUS) {
      sign[0] = 1;
      sign[1] = '+';
   }
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va)
{
   static char hex[] = "0123456789abcdefxp";
   static char hexu[] = "0123456789ABCDEFXP";
   char *bf;
   char const *f;
   int tlen = 0;

   bf = buf;
   f = fmt;
   for (;;) {
      stbsp__int32 fw, pr, tz;
      stbsp__uint32 fl;

      // macros for the callback buffer stuff
      #define stbsp__chk_cb_bufL(bytes)                        \
         {                                                     \
            int len = (int)(bf - buf);                         \
            if ((len + (bytes)) >= STB_SPRINTF_MIN) {          \
               tlen += len;                                    \
               if (0 == (bf = buf = callback(buf, user, len))) \
                  goto done;                                   \
            }                                                  \
         }
      #define stbsp__chk_cb_buf(bytes)    \
         {                                \
            if (callback) {               \
               stbsp__chk_cb_bufL(bytes); \
            }                             \
         }
      #define stbsp__flush_cb()                      \
         {                                           \
            stbsp__chk_cb_bufL(STB_SPRINTF_MIN - 1); \
         } // flush if there is even one byte in the buffer
      #define stbsp__cb_buf_clamp(cl, v)                \
         cl = v;                                        \
         if (callback) {                                \
            int lg = STB_SPRINTF_MIN - (int)(bf - buf); \
            if (cl > lg)                                \
               cl = lg;                                 \
         }

      // fast copy everything up to the next % (or end of string)
      for (;;) {
         while (((stbsp__uintptr)f) & 3) {
         schk1:
            if (f[0] == '%')
               goto scandd;
         schk2:
            if (f[0] == 0)
               goto endfmt;
            stbsp__chk_cb_buf(1);
            *bf++ = f[0];
            ++f;
         }
         for (;;) {
            // Check if the next 4 bytes contain %(0x25) or end of string.
            // Using the 'hasless' trick:
            // https://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
            stbsp__uint32 v, c;
            v = *(stbsp__uint32 *)f;
            c = (~v) & 0x80808080;
            if (((v ^ 0x25252525) - 0x01010101) & c)
               goto schk1;
            if ((v - 0x01010101) & c)
               goto schk2;
            if (callback)
               if ((STB_SPRINTF_MIN - (int)(bf - buf)) < 4)
                  goto schk1;
            #ifdef STB_SPRINTF_NOUNALIGNED
                if(((stbsp__uintptr)bf) & 3) {
                    bf[0] = f[0];
                    bf[1] = f[1];
                    bf[2] = f[2];
                    bf[3] = f[3];
                } else
            #endif
            {
                *(stbsp__uint32 *)bf = v;
            }
            bf += 4;
            f += 4;
         }
      }
   scandd:

      ++f;

      // ok, we have a percent, read the modifiers first
      fw = 0;
      pr = -1;
      fl = 0;
      tz = 0;

      // flags
      for (;;) {
         switch (f[0]) {
         // if we have left justify
         case '-':
            fl |= STBSP__LEFTJUST;
            ++f;
            continue;
         // if we have leading plus
         case '+':
            fl |= STBSP__LEADINGPLUS;
            ++f;
            continue;
         // if we have leading space
         case ' ':
            fl |= STBSP__LEADINGSPACE;
            ++f;
            continue;
         // if we have leading 0x
         case '#':
            fl |= STBSP__LEADING_0X;
            ++f;
            continue;
         // if we have thousand commas
         case '\'':
            fl |= STBSP__TRIPLET_COMMA;
            ++f;
            continue;
         // if we have kilo marker (none->kilo->kibi->jedec)
         case '$':
            if (fl & STBSP__METRIC_SUFFIX) {
               if (fl & STBSP__METRIC_1024) {
                  fl |= STBSP__METRIC_JEDEC;
               } else {
                  fl |= STBSP__METRIC_1024;
               }
            } else {
               fl |= STBSP__METRIC_SUFFIX;
            }
            ++f;
            continue;
         // if we don't want space between metric suffix and number
         case '_':
            fl |= STBSP__METRIC_NOSPACE;
            ++f;
            continue;
         // if we have leading zero
         case '0':
            fl |= STBSP__LEADINGZERO;
            ++f;
            goto flags_done;
         default: goto flags_done;
         }
      }
   flags_done:

      // get the field width
      if (f[0] == '*') {
         fw = va_arg(va, stbsp__uint32);
         ++f;
      } else {
         while ((f[0] >= '0') && (f[0] <= '9')) {
            fw = fw * 10 + f[0] - '0';
            f++;
         }
      }
      // get the precision
      if (f[0] == '.') {
         ++f;
         if (f[0] == '*') {
            pr = va_arg(va, stbsp__uint32);
            ++f;
         } else {
            pr = 0;
            while ((f[0] >= '0') && (f[0] <= '9')) {
               pr = pr * 10 + f[0] - '0';
               f++;
            }
         }
      }

      // handle integer size overrides
      switch (f[0]) {
      // are we halfwidth?
      case 'h':
         fl |= STBSP__HALFWIDTH;
         ++f;
         if (f[0] == 'h')
            ++f;  // QUARTERWIDTH
         break;
      // are we 64-bit (unix style)
      case 'l':
         fl |= ((sizeof(long) == 8) ? STBSP__INTMAX : 0);
         ++f;
         if (f[0] == 'l') {
            fl |= STBSP__INTMAX;
            ++f;
         }
         break;
      // are we 64-bit on intmax? (c99)
      case 'j':
         fl |= (sizeof(size_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      // are we 64-bit on size_t or ptrdiff_t? (c99)
      case 'z':
         fl |= (sizeof(ptrdiff_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      case 't':
         fl |= (sizeof(ptrdiff_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      // are we 64-bit (msft style)
      case 'I':
         if ((f[1] == '6') && (f[2] == '4')) {
            fl |= STBSP__INTMAX;
            f += 3;
         } else if ((f[1] == '3') && (f[2] == '2')) {
            f += 3;
         } else {
            fl |= ((sizeof(void *) == 8) ? STBSP__INTMAX : 0);
            ++f;
         }
         break;
      default: break;
      }

      // handle each replacement
      switch (f[0]) {
         #define STBSP__NUMSZ 512 // big enough for e308 (with commas) or e-307
         char num[STBSP__NUMSZ];
         char lead[8];
         char tail[8];
         char *s;
         char const *h;
         stbsp__uint32 l, n, cs;
         stbsp__uint64 n64;
#ifndef STB_SPRINTF_NOFLOAT
         double fv;
#endif
         stbsp__int32 dp;
         char const *sn;

      case 's':
         // get the string
         s = va_arg(va, char *);
         if (s == 0)
            s = (char *)"null";
         // get the length
         sn = s;
         for (;;) {
            if ((((stbsp__uintptr)sn) & 3) == 0)
               break;
         lchk:
            if (sn[0] == 0)
               goto ld;
            ++sn;
         }
         n = 0xffffffff;
         if (pr >= 0) {
            n = (stbsp__uint32)(sn - s);
            if (n >= (stbsp__uint32)pr)
               goto ld;
            n = ((stbsp__uint32)(pr - n)) >> 2;
         }
         while (n) {
            stbsp__uint32 v = *(stbsp__uint32 *)sn;
            if ((v - 0x01010101) & (~v) & 0x80808080UL)
               goto lchk;
            sn += 4;
            --n;
         }
         goto lchk;
      ld:

         l = (stbsp__uint32)(sn - s);
         // clamp to precision
         if (l > (stbsp__uint32)pr)
            l = pr;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         // copy the string in
         goto scopy;

      case 'c': // char
         // get the character
         s = num + STBSP__NUMSZ - 1;
         *s = (char)va_arg(va, int);
         l = 1;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;

      case 'n': // weird write-bytes specifier
      {
         int *d = va_arg(va, int *);
         *d = tlen + (int)(bf - buf);
      } break;

#ifdef STB_SPRINTF_NOFLOAT
      case 'A':              // float
      case 'a':              // hex float
      case 'G':              // float
      case 'g':              // float
      case 'E':              // float
      case 'e':              // float
      case 'f':              // float
         va_arg(va, double); // eat it
         s = (char *)"No float";
         l = 8;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;
#else
      case 'A': // hex float
      case 'a': // hex float
         h = (f[0] == 'A') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_parts((stbsp__int64 *)&n64, &dp, fv))
            fl |= STBSP__NEGATIVE;

         s = num + 64;

         stbsp__lead_sign(fl, lead);

         if (dp == -1023)
            dp = (n64) ? -1022 : 0;
         else
            n64 |= (((stbsp__uint64)1) << 52);
         n64 <<= (64 - 56);
         if (pr < 15)
            n64 += ((((stbsp__uint64)8) << 56) >> (pr * 4));
// add leading chars

#ifdef STB_SPRINTF_MSVC_MODE
         *s++ = '0';
         *s++ = 'x';
#else
         lead[1 + lead[0]] = '0';
         lead[2 + lead[0]] = 'x';
         lead[0] += 2;
#endif
         *s++ = h[(n64 >> 60) & 15];
         n64 <<= 4;
         if (pr)
            *s++ = stbsp__period;
         sn = s;

         // print the bits
         n = pr;
         if (n > 13)
            n = 13;
         if (pr > (stbsp__int32)n)
            tz = pr - n;
         pr = 0;
         while (n--) {
            *s++ = h[(n64 >> 60) & 15];
            n64 <<= 4;
         }

         // print the expo
         tail[1] = h[17];
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
         n = (dp >= 1000) ? 6 : ((dp >= 100) ? 5 : ((dp >= 10) ? 4 : 3));
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }

         dp = (int)(s - sn);
         l = (int)(s - (num + 64));
         s = num + 64;
         cs = 1 + (3 << 24);
         goto scopy;

      case 'G': // float
      case 'g': // float
         h = (f[0] == 'G') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6;
         else if (pr == 0)
            pr = 1; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, (pr - 1) | 0x80000000))
            fl |= STBSP__NEGATIVE;

         // clamp the precision and delete extra zeros after clamp
         n = pr;
         if (l > (stbsp__uint32)pr)
            l = pr;
         while ((l > 1) && (pr) && (sn[l - 1] == '0')) {
            --pr;
            --l;
         }

         // should we use %e
         if ((dp <= -4) || (dp > (stbsp__int32)n)) {
            if (pr > (stbsp__int32)l)
               pr = l - 1;
            else if (pr)
               --pr; // when using %e, there is one digit before the decimal
            goto doexpfromg;
         }
         // this is the insane action to get the pr to match %g semantics for %f
         if (dp > 0) {
            pr = (dp < (stbsp__int32)l) ? l - dp : 0;
         } else {
            pr = -dp + ((pr > (stbsp__int32)l) ? (stbsp__int32) l : pr);
         }
         goto dofloatfromg;

      case 'E': // float
      case 'e': // float
         h = (f[0] == 'E') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr | 0x80000000))
            fl |= STBSP__NEGATIVE;
      doexpfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;
         // handle leading chars
         *s++ = sn[0];

         if (pr)
            *s++ = stbsp__period;

         // handle after decimal
         if ((l - 1) > (stbsp__uint32)pr)
            l = pr + 1;
         for (n = 1; n < l; n++)
            *s++ = sn[n];
         // trailing zeros
         tz = pr - (l - 1);
         pr = 0;
         // dump expo
         tail[1] = h[0xe];
         dp -= 1;
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
#ifdef STB_SPRINTF_MSVC_MODE
         n = 5;
#else
         n = (dp >= 100) ? 5 : 4;
#endif
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }
         cs = 1 + (3 << 24); // how many tens
         goto flt_lead;

      case 'f': // float
         fv = va_arg(va, double);
      doafloat:
         // do kilos
         if (fl & STBSP__METRIC_SUFFIX) {
            double divisor;
            divisor = 1000.0f;
            if (fl & STBSP__METRIC_1024)
               divisor = 1024.0;
            while (fl < 0x4000000) {
               if ((fv < divisor) && (fv > -divisor))
                  break;
               fv /= divisor;
               fl += 0x1000000;
            }
         }
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr))
            fl |= STBSP__NEGATIVE;
      dofloatfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;

         // handle the three decimal varieties
         if (dp <= 0) {
            stbsp__int32 i;
            // handle 0.000*000xxxx
            *s++ = '0';
            if (pr)
               *s++ = stbsp__period;
            n = -dp;
            if ((stbsp__int32)n > pr)
               n = pr;
            i = n;
            while (i) {
               if ((((stbsp__uintptr)s) & 3) == 0)
                  break;
               *s++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)s = 0x30303030;
               s += 4;
               i -= 4;
            }
            while (i) {
               *s++ = '0';
               --i;
            }
            if ((stbsp__int32)(l + n) > pr)
               l = pr - n;
            i = l;
            while (i) {
               *s++ = *sn++;
               --i;
            }
            tz = pr - (n + l);
            cs = 1 + (3 << 24); // how many tens did we write (for commas below)
         } else {
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((600 - (stbsp__uint32)dp) % 3) : 0;
            if ((stbsp__uint32)dp >= l) {
               // handle xxxx000*000.0
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= l)
                        break;
                  }
               }
               if (n < (stbsp__uint32)dp) {
                  n = dp - n;
                  if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                     while (n) {
                        if ((((stbsp__uintptr)s) & 3) == 0)
                           break;
                        *s++ = '0';
                        --n;
                     }
                     while (n >= 4) {
                        *(stbsp__uint32 *)s = 0x30303030;
                        s += 4;
                        n -= 4;
                     }
                  }
                  while (n) {
                     if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                        cs = 0;
                        *s++ = stbsp__comma;
                     } else {
                        *s++ = '0';
                        --n;
                     }
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr) {
                  *s++ = stbsp__period;
                  tz = pr;
               }
            } else {
               // handle xxxxx.xxxx000*000
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= (stbsp__uint32)dp)
                        break;
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr)
                  *s++ = stbsp__period;
               if ((l - dp) > (stbsp__uint32)pr)
                  l = pr + dp;
               while (n < l) {
                  *s++ = sn[n];
                  ++n;
               }
               tz = pr - (l - dp);
            }
         }
         pr = 0;

         // handle k,m,g,t
         if (fl & STBSP__METRIC_SUFFIX) {
            char idx;
            idx = 1;
            if (fl & STBSP__METRIC_NOSPACE)
               idx = 0;
            tail[0] = idx;
            tail[1] = ' ';
            {
               if (fl >> 24) { // SI kilo is 'k', JEDEC and SI kibits are 'K'.
                  if (fl & STBSP__METRIC_1024)
                     tail[idx + 1] = "_KMGT"[fl >> 24];
                  else
                     tail[idx + 1] = "_kMGT"[fl >> 24];
                  idx++;
                  // If printing kibits and not in jedec, add the 'i'.
                  if (fl & STBSP__METRIC_1024 && !(fl & STBSP__METRIC_JEDEC)) {
                     tail[idx + 1] = 'i';
                     idx++;
                  }
                  tail[0] = idx;
               }
            }
         };

      flt_lead:
         // get the length that we copied
         l = (stbsp__uint32)(s - (num + 64));
         s = num + 64;
         goto scopy;
#endif

      case 'B': // upper binary
      case 'b': // lower binary
         h = (f[0] == 'B') ? hexu : hex;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[0xb];
         }
         l = (8 << 4) | (1 << 8);
         goto radixnum;

      case 'o': // octal
         h = hexu;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 1;
            lead[1] = '0';
         }
         l = (3 << 4) | (3 << 8);
         goto radixnum;

      case 'p': // pointer
         fl |= (sizeof(void *) == 8) ? STBSP__INTMAX : 0;
         pr = sizeof(void *) * 2;
         fl &= ~STBSP__LEADINGZERO; // 'p' only prints the pointer with zeros
                                    // fall through - to X

      case 'X': // upper hex
      case 'x': // lower hex
         h = (f[0] == 'X') ? hexu : hex;
         l = (4 << 4) | (4 << 8);
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[16];
         }
      radixnum:
         // get the number
         if (fl & STBSP__INTMAX)
            n64 = va_arg(va, stbsp__uint64);
         else
            n64 = va_arg(va, stbsp__uint32);

         s = num + STBSP__NUMSZ;
         dp = 0;
         // clear tail, and clear leading if value is zero
         tail[0] = 0;
         if (n64 == 0) {
            lead[0] = 0;
            if (pr == 0) {
               l = 0;
               cs = (((l >> 4) & 15)) << 24;
               goto scopy;
            }
         }
         // convert to string
         for (;;) {
            *--s = h[n64 & ((1 << (l >> 8)) - 1)];
            n64 >>= (l >> 8);
            if (!((n64) || ((stbsp__int32)((num + STBSP__NUMSZ) - s) < pr)))
               break;
            if (fl & STBSP__TRIPLET_COMMA) {
               ++l;
               if ((l & 15) == ((l >> 4) & 15)) {
                  l &= ~15;
                  *--s = stbsp__comma;
               }
            }
         };
         // get the tens and the comma pos
         cs = (stbsp__uint32)((num + STBSP__NUMSZ) - s) + ((((l >> 4) & 15)) << 24);
         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         // copy it
         goto scopy;

      case 'u': // unsigned
      case 'i':
      case 'd': // integer
         // get the integer and abs it
         if (fl & STBSP__INTMAX) {
            stbsp__int64 i64 = va_arg(va, stbsp__int64);
            n64 = (stbsp__uint64)i64;
            if ((f[0] != 'u') && (i64 < 0)) {
               n64 = (stbsp__uint64)-i64;
               fl |= STBSP__NEGATIVE;
            }
         } else {
            stbsp__int32 i = va_arg(va, stbsp__int32);
            n64 = (stbsp__uint32)i;
            if ((f[0] != 'u') && (i < 0)) {
               n64 = (stbsp__uint32)-i;
               fl |= STBSP__NEGATIVE;
            }
         }

#ifndef STB_SPRINTF_NOFLOAT
         if (fl & STBSP__METRIC_SUFFIX) {
            if (n64 < 1024)
               pr = 0;
            else if (pr == -1)
               pr = 1;
            fv = (double)(stbsp__int64)n64;
            goto doafloat;
         }
#endif

         // convert to string
         s = num + STBSP__NUMSZ;
         l = 0;

         for (;;) {
            // do in 32-bit chunks (avoid lots of 64-bit divides even with constant denominators)
            char *o = s - 8;
            if (n64 >= 100000000) {
               n = (stbsp__uint32)(n64 % 100000000);
               n64 /= 100000000;
            } else {
               n = (stbsp__uint32)n64;
               n64 = 0;
            }
            if ((fl & STBSP__TRIPLET_COMMA) == 0) {
               do {
                  s -= 2;
                  *(stbsp__uint16 *)s = *(stbsp__uint16 *)&stbsp__digitpair.pair[(n % 100) * 2];
                  n /= 100;
               } while (n);
            }
            while (n) {
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = (char)(n % 10) + '0';
                  n /= 10;
               }
            }
            if (n64 == 0) {
               if ((s[0] == '0') && (s != (num + STBSP__NUMSZ)))
                  ++s;
               break;
            }
            while (s != o)
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = '0';
               }
         }

         tail[0] = 0;
         stbsp__lead_sign(fl, lead);

         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         if (l == 0) {
            *--s = '0';
            l = 1;
         }
         cs = l + (3 << 24);
         if (pr < 0)
            pr = 0;

      scopy:
         // get fw=leading/trailing space, pr=leading zeros
         if (pr < (stbsp__int32)l)
            pr = l;
         n = pr + lead[0] + tail[0] + tz;
         if (fw < (stbsp__int32)n)
            fw = n;
         fw -= n;
         pr -= l;

         // handle right justify and leading zeros
         if ((fl & STBSP__LEFTJUST) == 0) {
            if (fl & STBSP__LEADINGZERO) // if leading zeros, everything is in pr
            {
               pr = (fw > pr) ? fw : pr;
               fw = 0;
            } else {
               fl &= ~STBSP__TRIPLET_COMMA; // if no leading zeros, then no commas
            }
         }

         // copy the spaces and/or zeros
         if (fw + pr) {
            stbsp__int32 i;
            stbsp__uint32 c;

            // copy leading spaces (or when doing %8.4d stuff)
            if ((fl & STBSP__LEFTJUST) == 0)
               while (fw > 0) {
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i) {
                     *bf++ = ' ';
                     --i;
                  }
                  stbsp__chk_cb_buf(1);
               }

            // copy leader
            sn = lead + 1;
            while (lead[0]) {
               stbsp__cb_buf_clamp(i, lead[0]);
               lead[0] -= (char)i;
               while (i) {
                  *bf++ = *sn++;
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }

            // copy leading zeros
            c = cs >> 24;
            cs &= 0xffffff;
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((stbsp__uint32)(c - ((pr + cs) % (c + 1)))) : 0;
            while (pr > 0) {
               stbsp__cb_buf_clamp(i, pr);
               pr -= i;
               if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = '0';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x30303030;
                     bf += 4;
                     i -= 4;
                  }
               }
               while (i) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (cs++ == c)) {
                     cs = 0;
                     *bf++ = stbsp__comma;
                  } else
                     *bf++ = '0';
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }
         }

         // copy leader if there is still one
         sn = lead + 1;
         while (lead[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, lead[0]);
            lead[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy the string
         n = l;
         while (n) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, n);
            n -= i;
            STBSP__UNALIGNED(while (i >= 4) {
               *(stbsp__uint32 volatile *)bf = *(stbsp__uint32 volatile *)s;
               bf += 4;
               s += 4;
               i -= 4;
            })
            while (i) {
               *bf++ = *s++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy trailing zeros
         while (tz) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tz);
            tz -= i;
            while (i) {
               if ((((stbsp__uintptr)bf) & 3) == 0)
                  break;
               *bf++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)bf = 0x30303030;
               bf += 4;
               i -= 4;
            }
            while (i) {
               *bf++ = '0';
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy tail if there is one
         sn = tail + 1;
         while (tail[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tail[0]);
            tail[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // handle the left justify
         if (fl & STBSP__LEFTJUST)
            if (fw > 0) {
               while (fw) {
                  stbsp__int32 i;
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i--)
                     *bf++ = ' ';
                  stbsp__chk_cb_buf(1);
               }
            }
         break;

      default: // unknown, just copy code
         s = num + STBSP__NUMSZ - 1;
         *s = f[0];
         l = 1;
         fw = fl = 0;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;
      }
      ++f;
   }
endfmt:

   if (!callback)
      *bf = 0;
   else
      stbsp__flush_cb();

done:
   return tlen + (int)(bf - buf);
}

// cleanup
#undef STBSP__LEFTJUST
#undef STBSP__LEADINGPLUS
#undef STBSP__LEADINGSPACE
#undef STBSP__LEADING_0X
#undef STBSP__LEADINGZERO
#undef STBSP__INTMAX
#undef STBSP__TRIPLET_COMMA
#undef STBSP__NEGATIVE
#undef STBSP__METRIC_SUFFIX
#undef STBSP__NUMSZ
#undef stbsp__chk_cb_bufL
#undef stbsp__chk_cb_buf
#undef stbsp__flush_cb
#undef stbsp__cb_buf_clamp

// ============================================================================
//   wrapper functions

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);
   result = STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
   va_end(va);
   return result;
}

typedef struct stbsp__context {
   char *buf;
   int count;
   int length;
   char tmp[STB_SPRINTF_MIN];
} stbsp__context;

static char *stbsp__clamp_callback(const char *buf, void *user, int len)
{
   stbsp__context *c = (stbsp__context *)user;
   c->length += len;

   if (len > c->count)
      len = c->count;

   if (len) {
      if (buf != c->buf) {
         const char *s, *se;
         char *d;
         d = c->buf;
         s = buf;
         se = buf + len;
         do {
            *d++ = *s++;
         } while (s < se);
      }
      c->buf += len;
      c->count -= len;
   }

   if (c->count <= 0)
      return c->tmp;
   return (c->count >= STB_SPRINTF_MIN) ? c->buf : c->tmp; // go direct into buffer if you can
}

static char * stbsp__count_clamp_callback( const char * buf, void * user, int len )
{
   stbsp__context * c = (stbsp__context*)user;
   (void) sizeof(buf);

   c->length += len;
   return c->tmp; // go direct into buffer if you can
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsnprintf )( char * buf, int count, char const * fmt, va_list va )
{
   stbsp__context c;

   if ( (count == 0) && !buf )
   {
      c.length = 0;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__count_clamp_callback, &c, c.tmp, fmt, va );
   }
   else
   {
      int l;

      c.buf = buf;
      c.count = count;
      c.length = 0;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__clamp_callback, &c, stbsp__clamp_callback(0,&c,0), fmt, va );

      // zero-terminate
      l = (int)( c.buf - buf );
      if ( l >= count ) // should never be greater, only equal (or less) than count
         l = count - 1;
      buf[l] = 0;
   }

   return c.length;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);

   result = STB_SPRINTF_DECORATE(vsnprintf)(buf, count, fmt, va);
   va_end(va);

   return result;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va)
{
   return STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
}

// =======================================================================
//   low level float utility functions

#ifndef STB_SPRINTF_NOFLOAT

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#define STBSP__COPYFP(dest, src)                   \
   {                                               \
      int cn;                                      \
      for (cn = 0; cn < 8; cn++)                   \
         ((char *)&dest)[cn] = ((char *)&src)[cn]; \
   }

// get float info
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value)
{
   double d;
   stbsp__int64 b = 0;

   // load value and round at the frac_digits
   d = value;

   STBSP__COPYFP(b, d);

   *bits = b & ((((stbsp__uint64)1) << 52) - 1);
   *expo = (stbsp__int32)(((b >> 52) & 2047) - 1023);

   return (stbsp__int32)((stbsp__uint64) b >> 63);
}

static double const stbsp__bot[23] = {
   1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005, 1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011,
   1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017, 1e+018, 1e+019, 1e+020, 1e+021, 1e+022
};
static double const stbsp__negbot[22] = {
   1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011,
   1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018, 1e-019, 1e-020, 1e-021, 1e-022
};
static double const stbsp__negboterr[22] = {
   -5.551115123125783e-018,  -2.0816681711721684e-019, -2.0816681711721686e-020, -4.7921736023859299e-021, -8.1803053914031305e-022, 4.5251888174113741e-023,
   4.5251888174113739e-024,  -2.0922560830128471e-025, -6.2281591457779853e-026, -3.6432197315497743e-027, 6.0503030718060191e-028,  2.0113352370744385e-029,
   -3.0373745563400371e-030, 1.1806906454401013e-032,  -7.7705399876661076e-032, 2.0902213275965398e-033,  -7.1542424054621921e-034, -7.1542424054621926e-035,
   2.4754073164739869e-036,  5.4846728545790429e-037,  9.2462547772103625e-038,  -4.8596774326570872e-039
};
static double const stbsp__top[13] = {
   1e+023, 1e+046, 1e+069, 1e+092, 1e+115, 1e+138, 1e+161, 1e+184, 1e+207, 1e+230, 1e+253, 1e+276, 1e+299
};
static double const stbsp__negtop[13] = {
   1e-023, 1e-046, 1e-069, 1e-092, 1e-115, 1e-138, 1e-161, 1e-184, 1e-207, 1e-230, 1e-253, 1e-276, 1e-299
};
static double const stbsp__toperr[13] = {
   8388608,
   6.8601809640529717e+028,
   -7.253143638152921e+052,
   -4.3377296974619174e+075,
   -1.5559416129466825e+098,
   -3.2841562489204913e+121,
   -3.7745893248228135e+144,
   -1.7356668416969134e+167,
   -3.8893577551088374e+190,
   -9.9566444326005119e+213,
   6.3641293062232429e+236,
   -5.2069140800249813e+259,
   -5.2504760255204387e+282
};
static double const stbsp__negtoperr[13] = {
   3.9565301985100693e-040,  -2.299904345391321e-063,  3.6506201437945798e-086,  1.1875228833981544e-109,
   -5.0644902316928607e-132, -6.7156837247865426e-155, -2.812077463003139e-178,  -5.7778912386589953e-201,
   7.4997100559334532e-224,  -4.6439668915134491e-247, -6.3691100762962136e-270, -9.436808465446358e-293,
   8.0970921678014997e-317
};

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000,
   100000000000,
   1000000000000,
   10000000000000,
   100000000000000,
   1000000000000000,
   10000000000000000,
   100000000000000000,
   1000000000000000000,
   10000000000000000000U
};
#define stbsp__tento19th ((stbsp__uint64)1000000000000000000)
#else
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000ULL,
   100000000000ULL,
   1000000000000ULL,
   10000000000000ULL,
   100000000000000ULL,
   1000000000000000ULL,
   10000000000000000ULL,
   100000000000000000ULL,
   1000000000000000000ULL,
   10000000000000000000ULL
};
#define stbsp__tento19th (1000000000000000000ULL)
#endif

#define stbsp__ddmulthi(oh, ol, xh, yh)                            \
   {                                                               \
      double ahi = 0, alo, bhi = 0, blo;                           \
      stbsp__int64 bt;                                             \
      oh = xh * yh;                                                \
      STBSP__COPYFP(bt, xh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(ahi, bt);                                      \
      alo = xh - ahi;                                              \
      STBSP__COPYFP(bt, yh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(bhi, bt);                                      \
      blo = yh - bhi;                                              \
      ol = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo; \
   }

#define stbsp__ddtoS64(ob, xh, xl)          \
   {                                        \
      double ahi = 0, alo, vh, t;           \
      ob = (stbsp__int64)ph;                \
      vh = (double)ob;                      \
      ahi = (xh - vh);                      \
      t = (ahi - xh);                       \
      alo = (xh - (ahi - t)) - (vh + t);    \
      ob += (stbsp__int64)(ahi + alo + xl); \
   }

#define stbsp__ddrenorm(oh, ol) \
   {                            \
      double s;                 \
      s = oh + ol;              \
      ol = ol - (s - oh);       \
      oh = s;                   \
   }

#define stbsp__ddmultlo(oh, ol, xh, xl, yh, yl) ol = ol + (xh * yl + xl * yh);

#define stbsp__ddmultlos(oh, ol, xh, yl) ol = ol + (xh * yl);

static void stbsp__raise_to_power10(double *ohi, double *olo, double d, stbsp__int32 power) // power can be -323 to +350
{
   double ph, pl;
   if ((power >= 0) && (power <= 22)) {
      stbsp__ddmulthi(ph, pl, d, stbsp__bot[power]);
   } else {
      stbsp__int32 e, et, eb;
      double p2h, p2l;

      e = power;
      if (power < 0)
         e = -e;
      et = (e * 0x2c9) >> 14; /* %23 */
      if (et > 13)
         et = 13;
      eb = e - (et * 23);

      ph = d;
      pl = 0.0;
      if (power < 0) {
         if (eb) {
            --eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__negbot[eb]);
            stbsp__ddmultlos(ph, pl, d, stbsp__negboterr[eb]);
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__negtop[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__negtop[et], stbsp__negtoperr[et]);
            ph = p2h;
            pl = p2l;
         }
      } else {
         if (eb) {
            e = eb;
            if (eb > 22)
               eb = 22;
            e -= eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__bot[eb]);
            if (e) {
               stbsp__ddrenorm(ph, pl);
               stbsp__ddmulthi(p2h, p2l, ph, stbsp__bot[e]);
               stbsp__ddmultlos(p2h, p2l, stbsp__bot[e], pl);
               ph = p2h;
               pl = p2l;
            }
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__top[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__top[et], stbsp__toperr[et]);
            ph = p2h;
            pl = p2l;
         }
      }
   }
   stbsp__ddrenorm(ph, pl);
   *ohi = ph;
   *olo = pl;
}

// given a float value, returns the significant bits in bits, and the position of the
//   decimal point in decimal_pos.  +/-INF and NAN are specified by special values
//   returned in the decimal_pos parameter.
// frac_digits is absolute normally, but if you want from first significant digits (got %g and %e), or in 0x80000000
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits)
{
   double d;
   stbsp__int64 bits = 0;
   stbsp__int32 expo, e, ng, tens;

   d = value;
   STBSP__COPYFP(bits, d);
   expo = (stbsp__int32)((bits >> 52) & 2047);
   ng = (stbsp__int32)((stbsp__uint64) bits >> 63);
   if (ng)
      d = -d;

   if (expo == 2047) // is nan or inf?
   {
      *start = (bits & ((((stbsp__uint64)1) << 52) - 1)) ? "NaN" : "Inf";
      *decimal_pos = STBSP__SPECIAL;
      *len = 3;
      return ng;
   }

   if (expo == 0) // is zero or denormal
   {
      if (((stbsp__uint64) bits << 1) == 0) // do zero
      {
         *decimal_pos = 1;
         *start = out;
         out[0] = '0';
         *len = 1;
         return ng;
      }
      // find the right expo for denormals
      {
         stbsp__int64 v = ((stbsp__uint64)1) << 51;
         while ((bits & v) == 0) {
            --expo;
            v >>= 1;
         }
      }
   }

   // find the decimal exponent as well as the decimal bits of the value
   {
      double ph, pl;

      // log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
      tens = expo - 1023;
      tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);

      // move the significant bits into position and stick them into an int
      stbsp__raise_to_power10(&ph, &pl, d, 18 - tens);

      // get full as much precision from double-double as possible
      stbsp__ddtoS64(bits, ph, pl);

      // check if we undershot
      if (((stbsp__uint64)bits) >= stbsp__tento19th)
         ++tens;
   }

   // now do the rounding in integer land
   frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
   if ((frac_digits < 24)) {
      stbsp__uint32 dg = 1;
      if ((stbsp__uint64)bits >= stbsp__powten[9])
         dg = 10;
      while ((stbsp__uint64)bits >= stbsp__powten[dg]) {
         ++dg;
         if (dg == 20)
            goto noround;
      }
      if (frac_digits < dg) {
         stbsp__uint64 r;
         // add 0.5 at the right position and round
         e = dg - frac_digits;
         if ((stbsp__uint32)e >= 24)
            goto noround;
         r = stbsp__powten[e];
         bits = bits + (r / 2);
         if ((stbsp__uint64)bits >= stbsp__powten[dg])
            ++tens;
         bits /= r;
      }
   noround:;
   }

   // kill long trailing runs of zeros
   if (bits) {
      stbsp__uint32 n;
      for (;;) {
         if (bits <= 0xffffffff)
            break;
         if (bits % 1000)
            goto donez;
         bits /= 1000;
      }
      n = (stbsp__uint32)bits;
      while ((n % 1000) == 0)
         n /= 1000;
      bits = n;
   donez:;
   }

   // convert to string
   out += 64;
   e = 0;
   for (;;) {
      stbsp__uint32 n;
      char *o = out - 8;
      // do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
      if (bits >= 100000000) {
         n = (stbsp__uint32)(bits % 100000000);
         bits /= 100000000;
      } else {
         n = (stbsp__uint32)bits;
         bits = 0;
      }
      while (n) {
         out -= 2;
         *(stbsp__uint16 *)out = *(stbsp__uint16 *)&stbsp__digitpair.pair[(n % 100) * 2];
         n /= 100;
         e += 2;
      }
      if (bits == 0) {
         if ((e) && (out[0] == '0')) {
            ++out;
            --e;
         }
         break;
      }
      while (out != o) {
         *--out = '0';
         ++e;
      }
   }

   *decimal_pos = tens;
   *start = out;
   *len = e;
   return ng;
}

#undef stbsp__ddmulthi
#undef stbsp__ddrenorm
#undef stbsp__ddmultlo
#undef stbsp__ddmultlos
#undef STBSP__SPECIAL
#undef STBSP__COPYFP

#endif // STB_SPRINTF_NOFLOAT

// clean up
#undef stbsp__uint16
#undef stbsp__uint32
#undef stbsp__int32
#undef stbsp__uint64
#undef stbsp__int64
#undef STBSP__UNALIGNED

#endif // STB_SPRINTF_IMPLEMENTATION

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

/*** End of inlined file: stb_sprintf.h ***/


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma comment(lib, "user32.lib")

#ifndef assert
# define assert(e) ((e) ? 1 : (__debugbreak(), 0))
#endif

struct PluginGlobals {
    HANDLE        pipe;
    PluginDataOut data;
    char         *buf;
} p = { INVALID_HANDLE_VALUE };

static inline Bool
wb_check_ok(Bool ok, Char const *fn_name)
{
    if (! ok)
    {
        char buf[512];
        ssnprintf(buf, sizeof(buf),
                 "Error connecting to WhiteBox: %s failed (code %lu).\n"
                 "I wish I could tell you more, but that's all I know.",
                 fn_name, GetLastError());
        MessageBoxA(NULL, buf, "WhiteBox Error", MB_OK);
    }
    return ok;
}

static inline Bool
wb_is_connected()
{   return p.pipe != INVALID_HANDLE_VALUE;   }

static Bool
wb_connect_to_whitebox(Bool is_noisy)
{
    Bool result = 0;

    while (! wb_is_connected()) // NOTE: skips if already set
    { // Try to open a named pipe; wait for it, if necessary.
        p.pipe = CreateFileW(PLUGIN_PIPE_NAME, GENERIC_READ | GENERIC_WRITE,
                             0,             // no sharing
                             NULL,          // default security attributes
                             OPEN_EXISTING, // opens existing pipe
                             0,             // default attributes
                             NULL);         // no template file

        if (wb_is_connected())
        { // The pipe connected; change to message-read mode.
            DWORD *max_collection_size = 0,
                  *collection_timeout  = 0;
            DWORD mode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
            if (! SetNamedPipeHandleState(p.pipe, &mode, max_collection_size, collection_timeout) &&
                is_noisy)
            {   wb_check_ok(0, "SetNamedPipeHandleState");   }
        }

        else
        { // Exit if an error other than ERROR_PIPE_BUSY occurs.
            // TODO: set some state so we don't keep trying and failing
            int err = GetLastError();

            if (err == ERROR_PIPE_BUSY) // acceptable error
            { // All pipe instances are busy, so wait for 5 seconds.
                if (! WaitNamedPipeW(PLUGIN_PIPE_NAME, 5 * 1000))
                {
                    if (is_noisy)
                    {   ::MessageBoxA(NULL, "Could not connect to WhiteBox: 5 second wait timed out.\nI wish I could tell you more, but I don't have any more information to tell you", "WhiteBox Plugin error", MB_OK);   }
                    goto end;
                }
                // else loop and try creating file again
            }

            else switch (err)
            { // respond to type of error
                case ERROR_FILE_NOT_FOUND:
                if (is_noisy)
                {
                    ::MessageBoxA(NULL, "Could not find a running instance of WhiteBox.\n"
                                  "Please make sure WhiteBox is open and try connecting again.",
                                  "Could not connect to WhiteBox", MB_OK);
                } goto end;

                default:
                if (is_noisy)
                {
                    char err_buf[256];
                    ssnprintf(err_buf, sizeof(err_buf), "Could not connect to WhiteBox: %d", err);
                    ::MessageBoxA(NULL, "Could not connect to WhiteBox.\n"
                                  "I'm not sure what the error is - please contact hi@whitebox.systems with the error code in the title of this message",
                                  err_buf, MB_OK);
                } goto end;
            }
        }
    }

    result = wb_is_connected();
end:;
    return result;
}

static Bool
wb_send_plugin_data(Selection sel, Char const *path, Size path_size, PluginEditor editor, DirtyState dirty_state)
{
    Bool result = 0;
    assert(path);

    if (wb_is_connected())
    {
        DWORD wrote_n = 0;

        { // update data values
            p.data.editor      = (U1)editor;
            p.data.dirty_state = (U1)dirty_state;
            p.data.sel         = sel;
            if (p.data.path != path) // NOTE: it makes sense for some plugins to use this as their buffer
            {
                arr_resetlen(p.data.path, path_size + 1);
                memcpy(p.data.path, path, path_size);
                p.data.path[path_size] = '\0';
            }
        }

        DWORD buf_path_o   = PluginDataOut_Fixed_Size;
        DWORD buf_path_len = cast(DWORD, strlen(p.data.path)) + 1;
        DWORD buf_size     = buf_path_o  + buf_path_len;
        arr_resetlen(p.buf, buf_size);
        memcpy(p.buf,              &p.data,     buf_path_o);
        memcpy(p.buf + buf_path_o, p.data.path, buf_path_len);

        // TODO: use TransactNamedPipe: https://docs.microsoft.com/en-us/windows/win32/api/namedpipeapi/nf-namedpipeapi-transactnamedpipe?redirectedfrom=MSDN
        // TODO IMPORTANT: don't freeze if messages don't get immediately responded to
        SetLastError(0);
        if (! WriteFile(p.pipe, p.buf, buf_size, &wrote_n, NULL))
        {
            int err = GetLastError();
            switch (err)
            {
                case ERROR_NO_DATA: // if whitebox is closed
                {
                    CloseHandle(p.pipe);
                    p.pipe = INVALID_HANDLE_VALUE;
                } break;

                default:
                wb_check_ok(0, "WriteFile");
            }
            goto end;
        }

        result = 1;
    }
end:
    return result;
}

static void
wb_disconnect_plugin()
{
    CloseHandle(p.pipe);
    p.pipe = INVALID_HANDLE_VALUE;
}

/*** End of inlined file: plugin_win32.cpp ***/


#define function static

struct wb {
    Buffer_ID buffer;

    Buffer_Cursor cursor;
    Buffer_Cursor marker;

    DirtyState dirty_state;

    String_Const_u8 file_name;

    Arena name_arena[1];
} wb = {};

function int
wb_4c_cursor_changed(Buffer_ID buffer, Buffer_Cursor cursor)
{
    int result = (buffer      != wb.buffer ||
                  cursor.line != wb.cursor.line ||
                  cursor.col  != wb.cursor.col);
    return result;
}

function void
wb_4c_tick(Application_Links *app, View_ID view)
{
    int buffer_changed = 0;
    int cursor_changed = 0;
    int marker_changed = 0;
    int dirty_changed  = 0;

    { // update buffer
        Buffer_ID buffer = view_get_buffer(app, view, Access_ReadVisible);
        buffer_changed   = buffer && buffer != wb.buffer;

        if (buffer_changed)
        {
            if (! wb.name_arena->chunk_size)
            {   wb.name_arena[0] = make_arena(get_thread_context(app)->allocator, KB(4));   }

            linalloc_clear(wb.name_arena);
            wb.file_name = push_buffer_file_name(app, wb.name_arena, buffer);
            if (wb.file_name.str) // NOTE: ignore scratch/command buffers
            {   wb.buffer = buffer;   }
            else
            {   buffer_changed = 0;   }
        }
    }

    { // update cursor
        i64           cursor_pos = view_get_cursor_pos(app, view);
        Buffer_Cursor cursor     = buffer_compute_cursor(app, wb.buffer, seek_pos(cursor_pos));
        cursor_changed = (cursor.line != wb.cursor.line ||
                          cursor.col  != wb.cursor.col);
        wb.cursor = cursor;
    }

    { // update marker
        i64           marker_pos = view_get_mark_pos(app, view);
        Buffer_Cursor marker     = buffer_compute_cursor(app, wb.buffer, seek_pos(marker_pos));
        marker_changed = (marker.line != wb.marker.line ||
                          marker.col  != wb.marker.col);
        wb.marker = marker;
    }

    { // update dirty_state
        Dirty_State dirty_state = buffer_get_dirty_state(app, wb.buffer);
        dirty_changed           = dirty_state != wb.dirty_state;
        wb.dirty_state          = (DirtyState)dirty_state;
    }

    // TODO: could either convey dirty_state to whitebox or only transmit when dirty_state
    if (buffer_changed ||
        cursor_changed ||
        marker_changed ||
        dirty_changed)
    {
        /* char buf[256]; */
        /* ssnprintf(buf, sizeof(buf), */
        /*           "Cursor moved to:\n" */
        /*           "file:   %s\n" */
        /*           "cursor: %lld, %lld\n" */
        /*           "marker: %lld, %lld\n" */
        /*           "dirty: %d, saved: %d" */
        /*           , wb.file_name.str */
        /*           , wb.cursor.line, wb.cursor.col */
        /*           , wb.marker.line, wb.marker.col */
        /*           , buffer_get_dirty_state(app, wb.buffer), wb.dirty_state); */

        /* MessageBoxA(0, buf, "Cursor changed", MB_OK); */

        Selection sel = { cast(Size, wb.cursor.line), cast(Size, wb.cursor.col),
                          cast(Size, wb.marker.line), cast(Size, wb.marker.col) };

        wb_send_plugin_data(sel, (char const *)wb.file_name.str, wb.file_name.size, EDITOR_4coder, wb.dirty_state);
    }
}

function void
wb_4c_default_tick(Application_Links *app, Frame_Info frame_info)
{
    default_tick(app, frame_info);
    View_ID view = get_active_view(app, Access_ReadVisible);
    wb_4c_tick(app, view);
}

#if 1  // COMMANDS

CUSTOM_COMMAND_SIG(whitebox_disconnect)
CUSTOM_DOC("Disconnects from a connected WhiteBox executable")
{
    if (wb_is_connected())
    {
        wb_disconnect_plugin();

        if (wb_is_connected())
        {   print_message(app, string_u8_litexpr("Failed to disconnect from WhiteBox\n"));   }
    }
    else
    {   print_message(app, string_u8_litexpr("Not connected to a WhiteBox instance\n"));   }
}

CUSTOM_COMMAND_SIG(whitebox_connect)
CUSTOM_DOC("Connects to a running WhiteBox executable")
{
    if (! wb_is_connected())
    {
        wb_connect_to_whitebox(1);

        if (! wb_is_connected())
        {   print_message(app, string_u8_litexpr("Failed to connect to WhiteBox. Is WhiteBox running?\n"));   }
    }
    else
    {   print_message(app, string_u8_litexpr("Already connected to a WhiteBox instance\n"));   }
}

CUSTOM_COMMAND_SIG(whitebox_connection_check)
CUSTOM_DOC("Query if currently connected to a WhiteBox instance")
{
    print_message(app, (wb_is_connected()
                        ? string_u8_litexpr("Is connected to WhiteBox\n")
                        : string_u8_litexpr("Is not connected to WhiteBox\n")));
}

#endif // COMMANDS

