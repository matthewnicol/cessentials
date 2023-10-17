#ifndef BASE_H
#define BASE_H

#define PRODUCTION 0
#define DEBUG 1
#define MEMTRACE 1

#include <stdint.h>
#include <setjmp.h>  // Except
#include <stdarg.h>  // Assert
#include <stdlib.h>  // Mem
#include <string.h>  // Mem

// CLANG
#if defined(__clang__)
#define COMPILER_CLANG 1

// CLANG OS
#if defined(_WIN32)
#define OS_WINDOWS 1
#elif defined(__gnu_linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#error Could not detect OS
#endif

// CLANG ARCHITECTURE
#if defined(__amd64__)
#define ARCH_X64 1
#elif defined(__i386__)
#define ARCH_X86 1
#elif defined(__arm__)
#define ARCH_ARM 1
#elif defined(__aarch64__)
#define ARCH_ARM64 1
#else
#error Could not detect architecture
#endif

// CL
#elif defined (_MSC_VER)
#define COMPILER_CL 1

// CL OS
#if defined(_WIN32)
#define OS_WINDOWS 1
#elif defined(__gnu_linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#error Could not detect OS
#endif

// CL ARCHITECTURE
#if defined(_M_X64)
#define ARCH_X64 1
#elif defined(_M_IX86)
#define ARCH_X86 1
#elif defined(_M_ARM)
#define ARCH_ARM 1
#elif defined(__aarch64__)
#define ARCH_ARM64 1
#else
#error Could not detect architecture
#endif

// GCC
#elif defined(__GNUC__)

// GCC OS
#if defined(_WIN32)
#define OS_WINDOWS 1
#elif defined(__gnu_linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#error Could not detect OS
#endif

// GCC ARCHITECTURE
#if defined(__amd64__)
#define ARCH_X64 1
#elif defined(__i386__)
#define ARCH_X86 1
#elif defined(__arm__)
#define ARCH_ARM 1
#elif defined(__aarch64__)
#define ARCH_ARM64 1
#else
#error Could not detect architecture
#endif
#endif

#ifndef COMPILER_CLANG
#define COMPILER_CLANG 0
#endif

#ifndef COMPILER_GCC
#define COMPILER_GCC 0
#endif

#ifndef COMPILER_CL
#define COMPILER_CL 0
#endif

#ifndef OS_WINDOWS
#define OS_WINDOWS 0
#endif

#ifndef OS_LINUX
#define OS_LINUX 0
#endif

#ifndef OS_MAC
#define OS_MAC 0
#endif

#ifndef ARCH_X64
#define ARCH_X64 0
#endif

#ifndef ARCH_X86
#define ARCH_X86 0
#endif

#ifndef ARCH_ARM
#define ARCH_ARM 0
#endif

#ifndef ARCH_ARM64
#define ARCH_ARM64 0
#endif

// TODO: TEST
#if defined(__GNUC__) || defined(__clang__) // supports passing 0 arguments using gcc/clang non-standard features
#define VA_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define VA_NARGS(...) VA_NARGS_IMPL(_, ## __VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#else
#define VA_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N // 1 or more arguments only
#define VA_NARGS(...) VA_NARGS_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#endif
// END TODO

//
// TYPES
//

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int32_t S32;
typedef int64_t S64;
typedef float F32;
typedef double F64;

#define PTR_TO_INT(P) ((U64)((char*)P - (char*)0))
#define INT_TO_PTR(N) ((void*)((char*)0 + (N)))

#define STRUCT_MEMBER(T, M) (((T*)0)->M)
#define STRUCT_MEMBER_OFFSET(T, M) PTR_TO_INT(&STRUCT_MEMBER(T, M))

#define MIN(A, B) (((A) < (B)) ? (A) : (B))
#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#define CLAMP(A, X, B) (((X) < (A)) ? (A) : ((B) < (X)) ? (B) : (X))

#define COPY(FROM, TO) (memcpy(TO, FROM, sizeof(*FROM)));
#define COPYN(FROM, TO, N) if (N) (memcpy((TO), (FROM), sizeof((FROM)[0]) * (N)))

#define STRCOPY(FROM, TO) { for (char *outp = (TO), *inp = (FROM); (*outp++ = *inp++);) ; }

typedef U32 ItemID;

typedef union V2_U32 {
    struct { U32 x, y; };
    U32 v[2];
} V2_U32;

typedef union V2_S32 {
    struct { S32 x, y; };
    S32 v[2];
} V2_S32;

typedef union V3_U32 {
    struct { U32 x, y, z; };
    U32 v[3];
} V3_U32;

typedef union V3_S32 {
    struct { S32 x, y, z; };
    S32 v[3];
} V3_S32;

typedef union V4_U32 {
    struct { U32 x, y, z, w; };
    U32 v[3];
} V4_U32;

typedef union V4_S32 {
    struct { S32 x, y, z, w; };
    S32 v[4];
} V4_S32;

typedef union V2_F32 {
    struct { F32 x, y; };
    F32 v[2];
    F32 f[2];
} V2_F32;

typedef union V2_F64 {
    struct { F64 x, y; };
    F64 v[2];
} V2_F64;

typedef union V3_F32 {
    struct { F32 x, y, z; };
    struct { F32 r, g, b; };
    struct { F32 lo, mid, hi; };
    V2_F32 xy;
    F32 v[3];
    F32 f[3];
} V3_F32;

typedef union V3_F64 {
    struct { F64 x, y, z; };
    F64 v[3];
} V3_F64;

typedef union V4_F32 {
    struct { F32 x, y, z, w; };
    struct { F32 r, g, b, a; };
    F32 v[4];
    F32 f[4];
    V3_F32 xyz;
} V4_F32;

typedef union V4_F64 {
    struct { F64 x, y, z, w; };
    F64 v[4];
} V4_F64;

typedef union MAT3 {
    F32 v[16];
    struct {
        F32 x1, x2, x3,
            y1, y2, y3,
            z1, z2, z3;
    };
} MAT3;

typedef union MAT4 {
    F32 v[16];
    struct {
        V4_F32 right, up, forward, position;
    };
    struct {
        F32 x1, x2, x3, x4,
            y1, y2, y3, y4,
            z1, z2, z3, z4,
            w1, w2, w3, w4;
    };
    struct {
        F32 xx, xy, xz, xw,
            yx, yy, yz, yw,
            zx, zy, zz, zw,
            tx, ty, tz, tw;
    };
    struct {
        F32 c0r0, c0r1, c0r2, c0r3,
            c1r0, c1r1, c1r2, c1r3,
            c2r0, c2r1, c2r2, c2r3,
            c3r0, c3r1, c3r2, c3r3;
    };
    struct {
        F32 r0c1, r1c0, r2c0, r3c0,
            r0c2, r1c1, r2c1, r3c1,
            r0c3, r1c2, r2c2, r3c2,
            r0c4, r1c3, r2c3, r3c3;
    };
} MAT4;

typedef MAT4 Mat4_t;
typedef MAT3 Mat3_t;

typedef V4_F32 Vec4_t;
typedef V3_F32 Vec3_t;
typedef V2_F32 Vec2_t;

typedef V4_S32 IVec4_t;
typedef V3_S32 IVec3_t;
typedef V2_S32 IVec2_t;

extern const float EPSILON;
extern const float VEC3_EPSILON;
extern const float MAT4_EPSILON;
extern const float QUAT_EPSILON;



extern const Vec3_t V3010;
extern const Vec3_t V3100;
extern const Vec3_t V3001;
extern const Vec3_t V3111;
extern const Vec3_t V3N111;

#define V2_ADD(A, B, C) ((C)->x = (A)->x + (B)->x;\
    (C)->y = (A)->y + (B)->y)

#define V3_ADD(A, B, C) ((C)->x = (A)->x + (B)->x;\
    (C)->y = (A)->y + (B)->y;\
    (C)->z = (A)->z + (B)->z)

#define V4_ADD(A, B, C) ((C)->x = (A)->x + (B)->x;\
    (C)->y = (A)->y + (B)->y;\
    (C)->z = (A)->z + (B)->z;\
    (C)->w = (A)->w + b->w)

#define V4_COPY(A, B) (B)->x = (A)->x;\
    (B)->y = (A)->y;\
    (B)->z = (A)->z;\
    (B)->w = (A)->w

#define V3_COPY(A, B) (B)->x = (A)->x;\
    (B)->y = (A)->y;\
    (B)->z = (A)->z

#define V2_COPY(A, B) (B)->x = (A)->x;\
    (B)->y = (A)->y

#define V4_SET(A, X, Y, Z, W) (A)->x = X;\
    (A)->y = Y;\
    (A)->z = Z;\
    (A)->w = W

#define V3_SET(A, X, Y, Z)    (A)->x = X;\
    (A)->y = Y;\
    (A)->z = Z
#define V2_SET(A, X, Y)       (A)->x = X;\
    (A)->y = Y

// Fast comparison function for 2 vec2s that tests using an epsilon
#define V2_EQ(A, B) (fabsf((A)->x - (B)->x) < EPSILON &&\
    fabsf((A)->y - (B)->y) < EPSILON)

// Fast comparison function for 2 vec3s that tests using an epsilon
#define V3_EQ(A, B) (fabsf((A)->x - (B)->x) < EPSILON &&\
    fabsf((A)->y - (B)->y) < EPSILON &&\
    fabsf((A)->z - (B)->z) < EPSILON)

// Fast comparison function for 2 vec4s that tests using an epsilon
#define V4_EQ(A, B) (fabsf((A)->x - (B)->x) < EPSILON &&\
    fabsf((A)->y - (B)->y) < EPSILON &&\
    fabsf((A)->z - (B)->z) < EPSILON &&\
    fabsf((A)->w - (B)->w) < EPSILON) 

#define V4(A, B, C, D) ((V4_F32){A, B, C, D})
#define V3(A, B, C) ((V3_F32){A, B, C})
#define V2(A, B) ((V2_F32){A, B})

typedef struct List_t {
    void *payload;
    S32 tag;  // Possibly unused
    struct List_t *next;
} List_t;

List_t *List_list(void *x, ...);
List_t *List_append(List_t *list, void *payload, int tag);
int List_size(List_t *list);
List_t *List_build(char *file, int line, void *x, ...);
List_t *List_add(char *filename, int line, List_t *list, void *payload, int tag);
List_t *List_new(void *payload, int tag);
void List_dispose(List_t **list);
void List_orphan(List_t **list);
List_t *List_drop(List_t *l, void* payload);
List_t *List_popLeft(List_t **list);
List_t *List_popRight(List_t **list);

#define LIST_INIT(A, EL) \
        NEW(A);\
        A->payload = EL;\
        A->next = NULL;

#define LIST_PREPEND(A, EL, NEXT) \
        NEW(A);\
        A->payload = EL;\
        A->next = NEXT

// For custom list types
#define LIST_ADD(LIST_TYPE, A, EL) \
        if (!A) {\
            NEW(A);\
            A->payload = EL;\
            A->next = NULL;\
        } else {\
            LIST_TYPE *B = A;\
            while (B->next) B = B->next;\
            NEW(B->next);\
            B->next->payload = EL;\
            B->next->next = 0;\
        }
    

#define List_list(x, ...) List_build(__FILE__, __LINE__, x, __VA_ARGS__)
#define List_append(a, b, c) List_add(__FILE__, __LINE__, a, b, c)

typedef struct Buffer_t {
    U32 bytes_per_item;
    U32 stored;
    U32 capacity;
    void *buffer;
} Buffer_t;

Buffer_t *Buffer_new(size_t bytes_per_item, size_t capacity);
void Buffer_dispose(Buffer_t **buffer);
int Buffer_expand(Buffer_t *buffer, size_t nbytes);
int Buffer_write(Buffer_t *buffer, size_t n_items, const void *data);
void *Buffer_orphan(Buffer_t **buffer);

List_t *Buffer_pluck(Buffer_t *buffer, int items, int *indexes);
int Buffer_consume(Buffer_t *buffer, Buffer_t **other, int destroyOther);
int Buffer_index(Buffer_t *buffer, void *data);
void *Buffer_get(Buffer_t *buffer, ItemID item);

typedef struct IDRegistry {
    U32 numItems;
    U32 maxItems;
    U32 growBy;
    void **items;
} IDRegistry;

IDRegistry *Registry_new(int startSize, int growBy);
ItemID Registry_register(IDRegistry *registry, void *item);
void Registry_reregister(IDRegistry *registry, ItemID itemID, void *item);
void *Registry_get(IDRegistry *registry, ItemID item);
void Registry_remove(IDRegistry *registry, ItemID item, int destroy);
void Registry_dispose(IDRegistry **registry);

#define REGISTRY_LOOKUP(A, B, C) A = Registry_get(B, C);

/**
 * I know - seems gross - but this bundles forgettable & tedious things
 * - Skip holes in registry due to freed items; don't take null as 'stop'
 * - Compensate for sentinal value (0): add 1 to numItems, and skip 0
 * - Manage pointer, including saving value on break.
 */

#define REGISTRY_FOREACH_I(A, B, N) A; \
            for (U32 N = 1; N < B->numItems+1; N++) \
                if ((A = B->items[N])) \


typedef struct HashMap_t HashMap_t;
typedef struct Mapping_t {
    char *key;
    void *value;
} Mapping_t;

HashMap_t *HashMap_new_hop(char *file, int line);
HashMap_t *HashMap_newInt_hop(char *file, int line);
#define HashMap_new() HashMap_new_hop(__FILE__, __LINE__)
#define HashMap_newInt() HashMap_newInt_hop(__FILE__, __LINE__)
void HashMap_dispose(HashMap_t **map);
void HashMap_set(HashMap_t *map, const char *key, void *payload);
void *HashMap_pop(HashMap_t *map, const char *key);
void *HashMap_get(HashMap_t *map, const char *key);
void HashMap_setInt(HashMap_t *map, const char *key, int payload);
int HashMap_popInt(HashMap_t *map, const char *key);
int HashMap_getInt(HashMap_t *map, const char *key);

typedef struct MemHashMap_Mapping_t {
    void *key;
    void *value;
} MemHashMap_Mapping_t;

typedef struct MemHashMap_t MemHashMap_t;
MemHashMap_t *MemHashMap_new();
void *MemHashMap_get(MemHashMap_t *map, void *key);
void *MemHashMap_pop(MemHashMap_t *map, void *key);
void MemHashMap_set(MemHashMap_t *map, void *key, void *payload);


typedef struct Stack_t *Stack_t;
Stack_t Stack_new(void *top);
void *Stack_pop(Stack_t *stack);
void *Stack_top(Stack_t stack);
void Stack_push(Stack_t *stack, void *top);
int Stack_size(Stack_t stack);
int Stack_empty(Stack_t stack);

int strpos(char *haystack, char *needle);

#define NBASE_PARSER_RULE(...) VA_NARGS(__VA_ARGS__), (NBASE_Parser_match[]){__VA_ARGS__}

#define PARSER_COPY_TOKEN(tok, dest) \
    {\
        char *dp = dest, *sp = tok.string; \
        for (S32 i = 0; i < tok.length; i++) *dp++ = *sp++; \
        *dp = 0; \
    }

#define PARSER_DUPLICATE_TOKEN(tok, assignTo) \
    {\
        char *out; \
        NEWN(out, tok.length+1);\
        TOKCOPY(tok, out);\
        assignTo = out;\
    }

typedef enum NBASE_Parser_tokenType {
    TOKEN_NOMATCH,
    TOKEN_ANY,
    TOKEN_UNKNOWN,
    TOKEN_WHITESPACE,
    TOKEN_LINE_COMMENT,
    TOKEN_BLOCK_COMMENT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_DOUBLE_LARROW, 
    TOKEN_DOUBLE_RARROW, 
    TOKEN_OR,
    TOKEN_AND,
    TOKEN_BANG,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LARROW, 
    TOKEN_RARROW, 
    TOKEN_HYPHEN,
    TOKEN_EOF,
} NBASE_Parser_tokenType;

typedef struct NBASE_Parser_token {
    NBASE_Parser_tokenType type;
    char *string;
    U32 length;
    U32 col;
    U32 line;

    S32 matchNumber;        // The number of the match that produced this token
    S32 matchStringNumber;  // The number of the string that produced this token
} NBASE_Parser_token;

typedef struct NBASE_Parser_match {
    NBASE_Parser_tokenType type;
    char **matches;
    S32 numMatches;
    S32 optional;
} NBASE_Parser_match;

typedef struct NBASE_Parser_bookmark {
    char *location;
    U32 col;
    U32 line;
} NBASE_Parser_bookmark;

typedef struct NBASE_Parser {
    char *source;
    char *location;
    S32 col;
    S32 line;
    char filename[100];

    struct NBASE_Parser *subParser;

    NBASE_Parser_token pendingTokens[100];
    S32 numPendingTokens;

    S32 keepWhitespace;
    char *restoreLocation;
    U32 restoreCol;
    U32 restoreLine;
} NBASE_Parser;


NBASE_Parser_token NBASE_Parser_getToken(NBASE_Parser *p); /* debug */

NBASE_Parser *NBASE_Parser_new();
void NBASE_Parser_pushSource(NBASE_Parser *p, char *source);

void NBASE_Parser_skipWhitespace(NBASE_Parser *p);
void NBASE_Parser_keepWhitespace(NBASE_Parser *p);
S32 NBASE_Parser_EOF(NBASE_Parser *p);
S32 NBASE_ParserToken_matchesString(NBASE_Parser_token *token, const char *testMatch);
S32 NBASE_Parser_matchSeries(NBASE_Parser *p, S32 seriesLength, NBASE_Parser_match *series, S32 *matchLength, NBASE_Parser_token *out);

void NBASE_ParserToken_extractString(NBASE_Parser_token *token, char *dest);
char *NBASE_ParserToken_copyString(NBASE_Parser_token *token);

typedef enum Axis {
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
    AXIS_W,
} Axis;

enum Side {
    Side_Min,
    Side_Max,
};

enum OperatingSystem {
    OperatingSystem_None,
    OperatingSystem_Windows,
    OperatingSystem_Linux,
    OperatingSystem_Mac,
    OperatingSystem_COUNT,
};

enum Architecture {
    Architecture_None,
    Architecture_X64,
    Architecture_X86,
    Architecture_ARM,
    Architecture_ARM64,
    Architecture_COUNT,
};

// EXCEPT.C

typedef struct Except_T {
    const char *reason;
} Except_T;

// Exported Types
typedef struct Except_Frame Except_Frame;
struct Except_Frame {
    Except_Frame *prev;
    jmp_buf env;
    const char *file;
    int line;
    const Except_T *exception;
};

enum {
    Except_entered = 0,
    Except_raised,
    Except_handled,
    Except_finalized
};


// Exported Variables
extern Except_Frame *Except_stack;
extern const Except_T Assert_Failed;

// Exported Functions
void Except_raise(const Except_T *e, const char *file, int line);
void Except_raise_msg(const char *file, int line, const char *msg);

// Exported Macros
#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)
#define RAISE_MSG(e) Except_raise_msg(__FILE__, __LINE__, e)
#define RERAISE Except_raise(Except_frame.exception, Except_frame.file, Except_frame.line)

#define RETURN switch (Except_stack = Except_stack->prev, 0) default: return

#define TRY do { \
    volatile int Except_flag; \
    Except_Frame Except_frame; \
    Except_frame.prev = Except_stack; \
    Except_stack = &Except_frame; \
    Except_flag = setjmp(Except_frame.env); \
    if (Except_flag == Except_entered) {
#define EXCEPT(e) \
    if (Except_flag == Except_entered) \
        Except_stack = Except_stack->prev \
    } else if (Except_frame.exception == &(e)) { \
        Except_flag = Except_handled;

#define ELSE \
if (Except_flag == Except_entered) Except_stack = Except_stack->prev \
} else { \
Except_flag = Except_handled

#define FINALLY \
if (Except_flag == Except_entered) Except_stack = Except_stack->prev \
} { \
if (Except_flag == Except_entered) \
Except_flag = Except_finalized;

#define END_TRY \
    if (Except_flag == Except_entered) \
        Except_stack = Except_stack->prev \
    } \
    if (Except_flag == Except_raised) RERAISE; \
    } while (0)

// ASSERT.C

#undef assert
#ifdef NDEBUG
#define assert(e) ((void)0)
#else

extern void assert(int e);
extern void _assert_with_message(int e, char *file, int line, char *format, ...);

#define assert(e) ((void)((e) || (RAISE(Assert_Failed), 0)))
#define assert_message(e, m, ...) _assert_with_message(e, __FILE__, __LINE__, m, __VA_ARGS__)
#define ERROR_IF_FALSE(e, m, ...) _assert_with_message(((S64)e), __FILE__, __LINE__, m, __VA_ARGS__)

#endif

typedef void *(*Mem_alloc_func)(U32 nbytes, const char *file, S32 line);
typedef void *(*Mem_calloc_func)(U32 count, U32 nbytes, const char *file, S32 line);
typedef void  (*  Mem_free_func)(void *ptr, const char *file, S32 line);
typedef void *(*Mem_resize_func)(void *ptr, U32 nbytes, const char *file, S32 line);
typedef void (*Mem_log_func)(char *message);

extern Mem_alloc_func MEM_ALLOC_PTR;
extern Mem_calloc_func MEM_CALLOC_PTR;
extern Mem_free_func MEM_FREE_PTR;
extern Mem_resize_func MEM_RESIZE_PTR;
extern Mem_log_func MEM_LOG_PTR;

// extern void  *Mem_alloc(U32 nbytes, const char *file, S32 line);
// extern void *Mem_calloc(U32 count, U32 nbytes, const char *file, S32 line);
// extern void    Mem_free(void *ptr, const char *file, S32 line);
// extern void *Mem_resize(void *ptr, U32 nbytes, const char *file, S32 line);

/**
 * @brief Helpers for the various allocation functions that report file and line information.
 */

#define ALLOC(nbytes) MEM_ALLOC_PTR((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes) MEM_CALLOC_PTR((count), (nbytes), __FILE__, __LINE__)
#define FREE(ptr) ((void)(MEM_FREE_PTR(((void*)ptr), __FILE__, __LINE__), (ptr) = 0))
#define RESIZE(ptr, nbytes) ((ptr) = MEM_RESIZE_PTR((ptr), (nbytes), __FILE__, __LINE__))

#define MEMLOG(message) MEM_LOG_PTR(message)

/**
 * @brief New Allocations: Allocate space
 *
 *       NEW: Allocate memory for just one item
 *       NEWN: Allocate memory for N items
 *       NEWB: Allocate memory by specifying bytes literally
 *       NEWNV: Allocate memory for a pointer array
 *       NEWO: Allocate memory and fill with zeroes
 */

#define NEW(p) ((p) = ALLOC((U32)sizeof *(p)))
#define NEWN(p, n) ((p) = ALLOC(((U32)sizeof *(p)) * (n)))
#define NEWNV(p, n) ((p) = ALLOC(((U32)sizeof (void*) * (n))))
#define NEWB(p, s, n) ((p) = ALLOC(((U32)(s)) * (n)))
#define NEW0(p) ((p) = CALLOC(1, (U32)sizeof *(p)))

/**
 * @brief Hop Allocations: Report the file and location of the memory allocation to assist with memory leak tracing.
 *
 *       NEWHOP: Allocate memory for just one item
 *       NEWNHOP: Allocate memory for N items
 *       NEWBHOP: Allocate memory by specifying bytes literally
 */

#define NEWHOP(p, f, l) ((p) = MEM_ALLOC_PTR( (U32)sizeof *(p), (f), (l)) )
#define NEWNHOP(p, n, f, l) ((p) = MEM_ALLOC_PTR( (((U32)sizeof *(p)) * (n)), (f), (l)))
#define NEWBHOP(p, s, n, f, l) ((p) = MEM_ALLOC_PTR( ((U32)(s)) * (n), (f), (l)) )

#define Q_NEW(p) p; ((p) = ALLOC((U32)sizeof *(p)))
#define Q_NEWN(p, n) p; ((p) = ALLOC(((U32)sizeof *(p)) * (n)))
#define Q_NEWB(p, s, n) p; ((p) = ALLOC(((U32)(s)) * (n)))
#define Q_NEW0(p) p; ((p) = CALLOC(1, (U32)sizeof *(p)))

#define NEWP(A, B) A = (B*)malloc(sizeof(B))

typedef struct BST_t BST_t;
typedef void (*BST_applyFunc)(const char *key, void *value, void **customData);

BST_t *BST_new();
void BST_insert(BST_t *bst, const char *key, void *value);
void *BST_get(BST_t *bst, const char *key);
void BST_apply(BST_t *bst, BST_applyFunc applyFunc, void *customData);

// OS-SPECIFIC

typedef enum OS_SCANDIR_FILTER {
    OS_SCANDIR_FILES,
    OS_SCANDIR_DIRS,
    OS_SCANDIR_ALL,
} OS_SCANDIR_FILTER;

#define OS_FILE 0
#define OS_DIR 1

#if OS_WINDOWS
#include <windows.h>
#include <minwindef.h>
#endif

typedef enum OS_keyboardKey {
    OS_KEYBOARD_NULL,
    OS_KEYBOARD_SPACE,
    OS_KEYBOARD_0,
    OS_KEYBOARD_1,
    OS_KEYBOARD_2,
    OS_KEYBOARD_3,
    OS_KEYBOARD_4,
    OS_KEYBOARD_5,
    OS_KEYBOARD_6,
    OS_KEYBOARD_7,
    OS_KEYBOARD_8,
    OS_KEYBOARD_9,
    OS_KEYBOARD_A,
    OS_KEYBOARD_B,
    OS_KEYBOARD_C,
    OS_KEYBOARD_D,
    OS_KEYBOARD_E,
    OS_KEYBOARD_F,
    OS_KEYBOARD_G,
    OS_KEYBOARD_H,
    OS_KEYBOARD_I,
    OS_KEYBOARD_J,
    OS_KEYBOARD_K,
    OS_KEYBOARD_L,
    OS_KEYBOARD_M,
    OS_KEYBOARD_N,
    OS_KEYBOARD_O,
    OS_KEYBOARD_P,
    OS_KEYBOARD_Q,
    OS_KEYBOARD_R,
    OS_KEYBOARD_S,
    OS_KEYBOARD_T,
    OS_KEYBOARD_U,
    OS_KEYBOARD_V,
    OS_KEYBOARD_W,
    OS_KEYBOARD_X,
    OS_KEYBOARD_Y,
    OS_KEYBOARD_Z,
    OS_KEYBOARD_NUMPAD_0,
    OS_KEYBOARD_NUMPAD_1,
    OS_KEYBOARD_NUMPAD_2,
    OS_KEYBOARD_NUMPAD_3,
    OS_KEYBOARD_NUMPAD_4,
    OS_KEYBOARD_NUMPAD_5,
    OS_KEYBOARD_NUMPAD_6,
    OS_KEYBOARD_NUMPAD_7,
    OS_KEYBOARD_NUMPAD_8,
    OS_KEYBOARD_NUMPAD_9,
    OS_KEYBOARD_NUMPAD_MULTIPLY,
    OS_KEYBOARD_NUMPAD_ADD,
    OS_KEYBOARD_NUMPAD_SEPARATOR,
    OS_KEYBOARD_NUMPAD_SUBTRACT,
    OS_KEYBOARD_NUMPAD_DECIMAL,
    OS_KEYBOARD_NUMPAD_DIVIDE,
    OS_KEYBOARD_COMMA,
    OS_KEYBOARD_PERIOD,
    OS_KEYBOARD_MINUS,
    OS_KEYBOARD_PLUS,
    OS_KEYBOARD_SEMICOLON,
    OS_KEYBOARD_FORWARD_SLASH,
    OS_KEYBOARD_TILDE,
    OS_KEYBOARD_OEM_LEFT_BRACE,
    OS_KEYBOARD_BACKSLASH,
    OS_KEYBOARD_RIGHT_BRACE,
    OS_KEYBOARD_QUOTE,
    OS_KEYBOARD_BACKSPACE,
    OS_KEYBOARD_TAB,
    OS_KEYBOARD_CLEAR,
    OS_KEYBOARD_ENTER,
    OS_KEYBOARD_SHIFT,
    OS_KEYBOARD_CTRL,
    OS_KEYBOARD_ALT,
    OS_KEYBOARD_PAUSE,
    OS_KEYBOARD_CAPS_LOCK,
    OS_KEYBOARD_ESCAPE,
    OS_KEYBOARD_PAGE_UP,
    OS_KEYBOARD_PAGE_DOWN,
    OS_KEYBOARD_END,
    OS_KEYBOARD_HOME,
    OS_KEYBOARD_LEFT_ARROW,
    OS_KEYBOARD_UP_ARROW,
    OS_KEYBOARD_RIGHT_ARROW,
    OS_KEYBOARD_DOWN_ARROW,
    OS_KEYBOARD_SELECT,
    OS_KEYBOARD_PRINT,
    OS_KEYBOARD_EXECUTE,
    OS_KEYBOARD_PRINT_SCREEN,
    OS_KEYBOARD_INSERT,
    OS_KEYBOARD_DELETE,
    OS_KEYBOARD_HELP,
    OS_KEYBOARD_F1,
    OS_KEYBOARD_F2,
    OS_KEYBOARD_F3,
    OS_KEYBOARD_F4,
    OS_KEYBOARD_F5,
    OS_KEYBOARD_F6,
    OS_KEYBOARD_F7,
    OS_KEYBOARD_F8,
    OS_KEYBOARD_F9,
    OS_KEYBOARD_F10,
    OS_KEYBOARD_F11,
    OS_KEYBOARD_F12,
    OS_KEYBOARD_F13,
    OS_KEYBOARD_F14,
    OS_KEYBOARD_F15,
    OS_KEYBOARD_F16,
    OS_KEYBOARD_F17,
    OS_KEYBOARD_F18,
    OS_KEYBOARD_F19,
    OS_KEYBOARD_F20,
    OS_KEYBOARD_F21,
    OS_KEYBOARD_F22,
    OS_KEYBOARD_F23,
    OS_KEYBOARD_F24,
    OS_KEYBOARD_NUMLOCK,
    OS_KEYBOARD_SCROLL,
    OS_KEYBOARD_LSHIFT,
    OS_KEYBOARD_RSHIFT,
    OS_KEYBOARD_LCONTROL,
    OS_KEYBOARD_RCONTROL,
    OS_KEYBOARD_LMENU,
    OS_KEYBOARD_RMENU,
    OS_KEYBOARD_OEM_8,         // unknown
    OS_KEYBOARD_OEM_102,       // unknown
    OS_KEYBOARD_KEYS_LENGTH,
} OS_keyboardKey;

#define MOUSE_IDLE 0
#define MOUSE_DOWN 1
#define MOUSE_HOLD 2
#define MOUSE_RELEASE 3

typedef enum OS_mouseButton {
    OS_MOUSE_LEFT,
    OS_MOUSE_MIDDLE,
    OS_MOUSE_RIGHT,
    OS_MOUSE_XBUTTON1,
    OS_MOUSE_XBUTTON2,
    OS_MOUSE_BUTTONS_LENGTH,
} OS_mouseButton;

typedef enum OS_CursorType {
    OS_CURSOR_ARROW,
    OS_CURSOR_HAND,
    OS_CURSOR_CROSSHAIR,
    OS_CURSOR_HELP,
    OS_CURSOR_IBEAM,
    OS_CURSOR_NOTALLOWED,
    OS_CURSOR_RESIZE_ALL,
    OS_CURSOR_RESIZE_NORTH_SOUTH,
    OS_CURSOR_RESIZE_NORTHEAST_SOUTHWEST,
    OS_CURSOR_RESIZE_NORTHWEST_SOUTHEAST,
    OS_CURSOR_RESIZE_WEST_EAST,
    OS_CURSOR_UP_ARROW,
    OS_CURSOR_HOURGLASS
} OS_CursorType;

typedef void (*OS_Callback_resize)(void *customData, int width, int height);
typedef void (*OS_Callback_mouseMove)(void *customData, const V2_F32 *newPosition);
typedef void (*OS_Callback_mouseScroll)(void *customData, const V2_F32 *scrollOffset, const V2_F32 *mousePosition);
typedef void (*OS_Callback_mouseClick)(void *customData, int button, int action, int mods, const V2_F32 *mousePosition);
typedef void (*OS_Callback_keyPress)(void *customData, OS_keyboardKey key, int isDown);

#define OS_KEYBOARD_FIRST_PRINTABLE_CHAR OS_KEYBOARD_SPACE
#define OS_KEYBOARD_PRINTABLE_CHAR_END OS_KEYBOARD_BACKSPACE

extern const char OS_Keyboard_keyToLowercase[OS_KEYBOARD_PRINTABLE_CHAR_END];
extern const char OS_Keyboard_keyToShift[OS_KEYBOARD_PRINTABLE_CHAR_END];
extern const char OS_Keyboard_keyToUppercase[OS_KEYBOARD_PRINTABLE_CHAR_END];
extern const char OS_Keyboard_keyToShiftUppercase[OS_KEYBOARD_PRINTABLE_CHAR_END];

extern int OS_keyboardMapping[OS_KEYBOARD_KEYS_LENGTH];
extern const char *OS_keyboardKeyNames[OS_KEYBOARD_KEYS_LENGTH];

U64 HashString(const char *key);
U64 RollingHashString(char c, U64 hash);

const S32 *OS_getKeyboardPressed();
const S32 *OS_getKeyboardHeld();
const S32 *OS_getKeyboardReleased();
const V2_F32 *OS_getMousePosition();
const V2_F32 *OS_getLastMousePosition();
const V2_F32 *OS_getMouseMovement();
const S32 *OS_getMouseState();
const S32 *OS_getMouseState_thisFrame();

typedef struct OSLibrary *OSLibrary;
typedef struct OS_Timer *OS_Timer;
typedef struct OS_Window *OS_Window;

OSLibrary OS_Library(const char *name);
int OS_Library_reload(OSLibrary lib);
void *OS_LibraryFunction(OSLibrary, char *functionName);

OS_Timer Timer_new();
void Timer_reset(OS_Timer t);
S64 Timer_getElapsedMicrosecondsAndLap(OS_Timer t);
S64 Timer_dispose(OS_Timer *t);

U64 OS_currentTime();
List_t *OS_FS_Scandir(const char *folder, OS_SCANDIR_FILTER filter);
int OS_FS_fileExists(char *filename);
char *OS_FS_getFileContents(const char *filename);
void OS_FS_setFileContents(const char *filename, void *data, U32 numBytes);
void OS_FS_structLoad(const char *filename, void *dest, U32 numbytes);
void OS_FS_structSave(const char *filename, void *src, U32 numbytes);

void OS_EventLog_startPhase(const char *phase);
void OS_logEvent(const char *system, const char *category, const char *event);

#define OS_STRUCTLOAD(A, B) OS_FS_structLoad((A), (B), sizeof(*(B)))
#define OS_STRUCTSAVE(A, B) OS_FS_structSave((A), (B), sizeof(*(B)))

U8 OS_capsLockEnabled();

F64 microsecondsToSeconds(S64 microseconds);

typedef struct OS_thread *OS_thread;
typedef U32 (*OS_ThreadableFunction)(void *userData);
void OS_runThreaded(OS_ThreadableFunction func, void *customData);

// ---------------------------
// OS WINDOW
// ---------------------------
char OS_Keyboard_keyToChar(OS_keyboardKey key);

void OS_handleEvents();
void OS_Window_handleEvents(OS_Window window);
void OS_Window_swapBuffers(OS_Window window);
void OS_Window_connect(
    OS_Window window,
    void *customDataIn,
    OS_Callback_resize resizeFunc,
    OS_Callback_mouseMove mouseMoveFunc,
    OS_Callback_mouseScroll mouseScrollFunc,
    OS_Callback_mouseClick mouseClickFunc,
    OS_Callback_keyPress keyPressFunc,
    S32 **keyPressedRefOut,
    S32 **keyHeldRefOut,
    S32 **keyReleasedRefOut
);
U32 OS_Window_quitRequested(OS_Window window);

OS_Window OS_OpenGLWindow(U32 width, U32 height, U32 bitsPerPixel, U32 isFullScreen);
U32 OS_Window_screenWidth(OS_Window window);
U32 OS_Window_screenHeight(OS_Window window);
void OS_init();
void OS_cleanup();
char *copyString(const char *in);

void OS_initFromAssets(
    Mem_alloc_func allocFunc,
    Mem_calloc_func callocFunc,
    Mem_free_func freeFunc,
    Mem_resize_func resizeFunc,
    Mem_log_func logFunc
);

void OS_Window_changeCursor(OS_CursorType cursorType);

void Assets_init();
void *Assets_get(const char *assetName);
void Assets_put(const char *assetName, void *data);
U32 Assets_has(const char *assetName);

#endif
