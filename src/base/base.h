#ifndef BASE_H
#define BASE_H

#define PRODUCTION 0
#define DEBUG 1
#define MEMTRACE 1

#include <stdint.h>
#include <setjmp.h> // Except
#include <stdarg.h> // Assert
#include <stdlib.h> // Mem
#include <string.h> // Mem

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

#define PTR_TO_INT(P) ((unsigned long long)((char*)p - (char*)0))
#define INT_TO_PTR(N) ((void*)((char*)0 + (n)))

#define STRUCT_MEMBER(T, M) (((T*)0)->M)
#define STRUCT_MEMBER_OFFSET(T,M) PTR_TO_INT(&STRUCT_MEMBER(T,M))

#define MIN(A,B) (((A) < (B)) ? (A) : (B))
#define MAX(A,B) (((A) > (B)) ? (A) : (B))
#define CLAMP(A,X,B) (((X) < (A)) ? (A) : ((B) < (X)) ? (B) : (X))

#define COPY(FROM, TO) (memcpy(TO, FROM, sizeof(*FROM)));
#define COPYN(FROM, TO, N) if (N) (memcpy((TO), (FROM), sizeof((FROM)[0])*(N)))

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

typedef U32 ItemID;

typedef union V2_U32 {
    struct {U32 x; U32 y;};
    U32 v[2];
} V2_U32;

typedef union V2_S32 {
    struct {S32 x; S32 y;};
    S32 v[2];
} V2_S32;

typedef union V3_U32 {
    struct {U32 x; U32 y; U32 z;};
    U32 v[3];
} V3_U32;

typedef union V3_S32 {
    struct {S32 x; S32 y; S32 z;};
    S32 v[3];
} V3_S32;

typedef union V4_U32 {
    struct {U32 x; U32 y; U32 z; U32 w;};
    U32 v[3];
} V4_U32;

typedef union V4_S32 {
    struct {S32 x; S32 y; S32 z; S32 w;};
    S32 v[4];
    F32 f[4];
} V4_S32;

typedef union V2_F32 {
    struct {F32 x; F32 y;};
    F32 v[2];
    F32 f[2];
} V2_F32;

typedef union V2_F64 {
    struct {F64 x; F64 y;};
    F64 v[2];
} V2_F64;

typedef union V3_F32 {
    struct {F32 x; F32 y; F32 z;};
    struct {F32 r; F32 g; F32 b;};
    struct {F32 lo; F32 mid; F32 hi;};
    V2_F32 xy;
    F32 v[3];
    F32 f[3];
} V3_F32;

typedef union V3_F64 {
    struct {F64 x; F64 y; F64 z;};
    F64 v[3];
} V3_F64;

typedef union V4_F32 {
    struct {F32 x; F32 y; F32 z; F32 w;};
    F32 v[4];
    F32 f[4];
    V3_F32 xyz;
} V4_F32;

typedef union V4_F64 {
    struct {F64 x; F64 y; F64 z; F64 w;};
    F64 v[4];
} V4_F64;

typedef union MAT3 {
    F32 v[16];
    struct {
        F32 x1; F32 x2; F32 x3;
        F32 y1; F32 y2; F32 y3;
        F32 z1; F32 z2; F32 z3;
    };

} MAT3;

typedef union MAT4 {
    
    F32 v[16];
    struct {
        V4_F32 right;
        V4_F32 up;
        V4_F32 forward;
        V4_F32 position;
    };
    struct { 
        F32 x1; F32 x2; F32 x3; F32 x4;
        F32 y1; F32 y2; F32 y3; F32 y4;
        F32 z1; F32 z2; F32 z3; F32 z4;
        F32 w1; F32 w2; F32 w3; F32 w4;
    };
    struct { 
        F32 xx; F32 xy; F32 xz; F32 xw;
        F32 yx; F32 yy; F32 yz; F32 yw;
        F32 zx; F32 zy; F32 zz; F32 zw;
        F32 tx; F32 ty; F32 tz; F32 tw;
    };
    struct {
        F32 c0r0; F32 c0r1; F32 c0r2; F32 c0r3;
        F32 c1r0; F32 c1r1; F32 c1r2; F32 c1r3;
        F32 c2r0; F32 c2r1; F32 c2r2; F32 c2r3;
        F32 c3r0; F32 c3r1; F32 c3r2; F32 c3r3;
    };
    struct {
        F32 r0c1; F32 r1c0; F32 r2c0; F32 r3c0;
        F32 r0c2; F32 r1c1; F32 r2c1; F32 r3c1;
        F32 r0c3; F32 r1c2; F32 r2c2; F32 r3c2;
        F32 r0c4; F32 r1c3; F32 r2c3; F32 r3c3;
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
#define V2_ADD(A, B, C) ((C)->x = (A)->x + (B)->x; (C)->y = (A)->y + (B)->y)
#define V3_ADD(A, B, C) ((C)->x = (A)->x + (B)->x; (C)->y = (A)->y + (B)->y; (C)->z = (A)->z + (B)->z)
#define V4_ADD(A, B, C) ((C)->x = (A)->x + (B)->x; (C)->y = (A)->y + (B)->y; (C)->z = (A)->z + (B)->z; (C)->w = (A)->w + b->w)

#define V4_COPY(A, B) (B)->x = (A)->x; (B)->y = (A)->y; (B)->z = (A)->z; (B)->w = (A)->w
#define V3_COPY(A, B) (B)->x = (A)->x; (B)->y = (A)->y; (B)->z = (A)->z
#define V2_COPY(A, B) (B)->x = (A)->x; (B)->y = (A)->y

#define V4_SET(A, X, Y, Z, W) (A)->x = X; (A)->y = Y; (A)->z = Z; (A)->w = W
#define V3_SET(A, X, Y, Z)    (A)->x = X; (A)->y = Y; (A)->z = Z
#define V2_SET(A, X, Y)       (A)->x = X; (A)->y = Y
typedef struct List_t {
    void *payload;
    S32 tag; // Possibly unused
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


// typedef char *String;

// size_t String_size(String string);
// String String_consume(String *string, String other);
// String String_copy(const String string);
// String String_new(const char *strIn);
// void String_dump(const String string);


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
    Except_entered=0, 
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

#define RETURN switch (Except_stack = Except_stack->prev,0) default: return

#define TRY do { \
volatile int Except_flag; \
Except_Frame Except_frame; \
Except_frame.prev = Except_stack; \
Except_stack = &Except_frame; \
Except_flag = setjmp(Except_frame.env); \
if (Except_flag == Except_entered) {

#define EXCEPT(e) \
if (Except_flag == Except_entered) Except_stack = Except_stack->prev \
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
if (Except_flag == Except_entered) Except_stack = Except_stack->prev \
} if (Except_flag == Except_raised) RERAISE; \
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

#endif

// MEM.C

#if MEMTRACE
extern void initMem();
extern void closeMem();
extern void logMem(char *message);
#endif

extern void *Mem_alloc (long nbytes, const char *file, int line);
extern void *Mem_calloc(long count, long nbytes, const char *file, int line);
extern void Mem_free(void *ptr, const char *file, int line);
extern void *Mem_resize(void *ptr, long nbytes, const char *file, int line);

/**
 * @brief Helpers for the various allocation functions that report file and line information.
 */

#define ALLOC(nbytes) Mem_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes) Mem_calloc((count), (nbytes), __FILE__, __LINE__)
#define FREE(ptr) ((void)(Mem_free(((void*)ptr), __FILE__, __LINE__), (ptr) = 0))
#define RESIZE(ptr, nbytes) ((ptr) = Mem_resize((ptr), (nbytes), __FILE__, __LINE__))


/**
 * @brief New Allocations: Allocate space
 *
 *       NEW: Allocate memory for just one item
 *       NEWN: Allocate memory for N items
 *       NEWB: Allocate memory by specifying bytes literally
 *       NEWNV: Allocate memory for a pointer array
 *       NEWO: Allocate memory and fill with zeroes
 */

#define NEW(p) ((p) = ALLOC((long)sizeof *(p)))
#define NEWN(p, n) ((p) = ALLOC(((long)sizeof *(p)) * (n)))
#define NEWNV(p, n) ((p) = ALLOC(((long)sizeof (void*) * (n))))
#define NEWB(p, s, n) ((p) = ALLOC(((long)(s)) * (n)))
#define NEW0(p) ((p) = CALLOC(1, (long)sizeof *(p)))

/**
 * @brief Hop Allocations: Report the file and location of the memory allocation to assist with memory leak tracing.
 * 
 *       NEWHOP: Allocate memory for just one item
 *       NEWNHOP: Allocate memory for N items
 *       NEWBHOP: Allocate memory by specifying bytes literally
 */

#define NEWHOP(p, f, l) ((p) = Mem_alloc( (long)sizeof *(p), (f), (l)) )
#define NEWNHOP(p, n, f, l) ((p) = Mem_alloc( (((long)sizeof *(p)) * (n)), (f), (l)))
#define NEWBHOP(p, s, n, f, l) ((p) = Mem_alloc( ((long)(s)) * (n), (f), (l)) )

#define Q_NEW(p) p; ((p) = ALLOC((long)sizeof *(p)))
#define Q_NEWN(p, n) p; ((p) = ALLOC(((long)sizeof *(p)) * (n)))
#define Q_NEWB(p, s, n) p; ((p) = ALLOC(((long)(s)) * (n)))
#define Q_NEW0(p) p; ((p) = CALLOC(1, (long)sizeof *(p)))

#define NEWP(A, B) A = (B*)malloc(sizeof(B))

// OS-SPECIFIC

#if OS_WINDOWS
#include <Windows.h>
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
    OS_KEYBOARD_OEM_8,         //unknown
    OS_KEYBOARD_OEM_102,       //unknown
    OS_KEYBOARD_KEYS_LENGTH,
} OS_keyboardKey;

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

typedef struct OSLibrary *OSLibrary;
typedef struct OS_Timer *OS_Timer;
typedef struct OS_Window *OS_Window;

OSLibrary OS_Library(const char *name);
int OS_Library_reload(OSLibrary lib);
void *OS_LibraryFunction(OSLibrary, char *functionName);

OS_Timer Timer_new();
void Timer_reset(OS_Timer t) ;
S64 Timer_getElapsedMicrosecondsAndLap(OS_Timer t);
S64 Timer_dispose(OS_Timer *t);

U64 OS_currentTime();
List_t *OS_FS_Scandir(char *folder);
char *OS_FS_getFileContents(char *filename);
U8 OS_capsLockEnabled();

F64 microsecondsToSeconds(long long microseconds);

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

#endif