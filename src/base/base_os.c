#include <string.h>
#include "base.h"

#if OS_WINDOWS
#include <windowsx.h>
#include <windows.h>
#include <minwindef.h>
#include <time.h>
#include <errhandlingapi.h>
#include <stdio.h>
#include <winbase.h>
#include <wingdi.h>
#include <winuser.h>
#include <wingdi.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/wglext.h>

#define DEBUG_OS_KEYBOARD_KEY 0
#define EVENTLOGGING 1

#if EVENTLOGGING
LARGE_INTEGER OS_EVENT_LOG_START_TIME;
LARGE_INTEGER OS_EVENT_LOG_FREQUENCY;
CHAR OS_EVENT_LOG_PHASE[50];
FILE *OS_EVENT_LOG = 0;
S32 OS_EVENTLOGGING = 0;
S32 OS_EVENTLOGGING_MOUSE = 0;
S32 OS_EVENTLOGGING_KEYBOARD = 0;
#endif

V2_F32 OS_mousePosition = {};
V2_F32 OS_lastMousePosition = {};
V2_F32 OS_mouseMovement = {};

int OS_mousePositionChanged = 0;

S32 OS_Keyboard_keyPressed[OS_KEYBOARD_KEYS_LENGTH] = {0};
S32 OS_Keyboard_keyPressed_thisFrame[OS_KEYBOARD_KEYS_LENGTH] = {0};
S32 OS_Keyboard_keyHeld[OS_KEYBOARD_KEYS_LENGTH] = {0};
S32 OS_Keyboard_keyReleased[OS_KEYBOARD_KEYS_LENGTH] = {0};
S32 OS_Keyboard_delayedRelease[OS_KEYBOARD_KEYS_LENGTH] = {0};

S32 OS_Mouse_buttonState[OS_MOUSE_BUTTONS_LENGTH] = {0};
S32 OS_Mouse_buttonState_thisFrame[OS_MOUSE_BUTTONS_LENGTH] = {0};

void *OS_customData = 0;

struct OS_thread {
    OS_ThreadableFunction func;
    void **customData;
    HANDLE handle;
    U8 destroyData;
};

struct OS_Window {
    U32 quitRequested;
    HINSTANCE hInstance;
    HGLRC hRC;
    HDC hDC;
    HWND hwnd;
    U32 width;
    U32 height;
    U32 bitsPerPixel;
    U32 isFullScreen;
    RECT windowRect;
    WNDCLASS wndCls;
    
    
    OS_Callback_resize      resizeFunc;         /* Trigger this callback when the window is resized */
    OS_Callback_mouseMove   mouseMoveFunc;      /* Trigger this callback when the mouse is moved */
    OS_Callback_mouseScroll mouseScrollFunc;    /* Trigger this callback when mouse is scrolled */
    OS_Callback_mouseClick  mouseClickFunc;     /* Trigger this callback when mouse is clicked */
    OS_Callback_keyPress    keyPressFunc;       /* Trigger this callback when a key is pressed */
    
    // Not sure if need VVV
    RECT clientRect;
    RECT borderRect;
};

int OS_windowOpen = 1;
OS_Window OS_activeWindow = 0;

const char OS_Keyboard_keyToLowercase[OS_KEYBOARD_PRINTABLE_CHAR_END] = "  0123456789abcdefghijklmnopqrstuvwxyz0123456789*+.-./,.-=;/`[\\]'";
const char OS_Keyboard_keyToShift[OS_KEYBOARD_PRINTABLE_CHAR_END] = "  )!@#$%^&*(ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789*+.-./<>_+:?~{|}'";
const char OS_Keyboard_keyToUppercase[OS_KEYBOARD_PRINTABLE_CHAR_END] = "  0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789*+.-./,.-=;/`[\\]'";
const char OS_Keyboard_keyToShiftUppercase[OS_KEYBOARD_PRINTABLE_CHAR_END] = "  )!@#$%^&*(abcdefghijklmnopqrstuvwxyz0123456789*+.-./<>_+:?~{|}'";

int OS_keyboardMapping[] = {
    0,
    VK_SPACE,             //  SPACEBAR
    0x30,                 // 0 key
    0x31,                 // 1 key
    0x32,                 // 2 key
    0x33,                 // 3 key
    0x34,                 // 4 key
    0x35,                 // 5 key
    0x36,                 // 6 key
    0x37,                 // 7 key
    0x38,                // 8 key
    0x39,                // 9 key
    0x41,                // A key
    0x42,                // B key
    0x43,                // C key
    0x44,                // D key
    0x45,                // E key
    0x46,                // F key
    0x47,                // G key
    0x48,                // H key
    0x49,                // I key
    0x4A,                // J key
    0x4B,                // K key
    0x4C,                // L key
    0x4D,                // M key
    0x4E,                // N key
    0x4F,                // O key
    0x50,                // P key
    0x51,                // Q key
    0x52,                // R key
    0x53,                // S key
    0x54,                // T key
    0x55,                // U key
    0x56,                // V key
    0x57,                // W key
    0x58,                // X key
    0x59,                // Y key
    0x5A,                // Z key
    VK_NUMPAD0,          // Numeric keypad 0 key
    VK_NUMPAD1,          // Numeric keypad 1 key
    VK_NUMPAD2,          // Numeric keypad 2 key
    VK_NUMPAD3,          // Numeric keypad 3 key
    VK_NUMPAD4,          // Numeric keypad 4 key
    VK_NUMPAD5,          // Numeric keypad 5 key
    VK_NUMPAD6,          // Numeric keypad 6 key
    VK_NUMPAD7,          // Numeric keypad 7 key
    VK_NUMPAD8,          // Numeric keypad 8 key
    VK_NUMPAD9,          // Numeric keypad 9 key
    VK_MULTIPLY,         // Multiply key
    VK_ADD,              // Add key
    VK_SEPARATOR,        // Separator key
    VK_SUBTRACT,         // Subtract key
    VK_DECIMAL,          // Decimal key
    VK_DIVIDE,           // Divide key
    VK_OEM_COMMA,
    VK_OEM_PERIOD,
    VK_OEM_MINUS,
    VK_OEM_PLUS,
    VK_OEM_1,
    VK_OEM_2,
    VK_OEM_3,
    VK_OEM_4,
    VK_OEM_5,
    VK_OEM_6,
    VK_OEM_7,
    VK_BACK,        // Backspace
    VK_TAB,         // Tab
    VK_CLEAR,       // Clear
    VK_RETURN,      // Return
    VK_SHIFT,       // Shift
    VK_CONTROL,     // Ctrl
    VK_MENU,        // Alt
    VK_PAUSE,       // Pause
    VK_CAPITAL,     // Capslock
    VK_ESCAPE,      //Escape
    VK_PRIOR,       //  PAGE UP key
    VK_NEXT,        //  PAGE DOWN key
    VK_END,         //  END key
    VK_HOME,        //  HOME key
    VK_LEFT,        //  LEFT ARROW key
    VK_UP,          //  UP ARROW key
    VK_RIGHT,       //  RIGHT ARROW key
    VK_DOWN,        //  DOWN ARROW key
    VK_SELECT,      //  SELECT key
    VK_PRINT,       //  PRINT key
    VK_EXECUTE,     //  EXECUTE key
    VK_SNAPSHOT,    //  PRINT SCREEN key
    VK_INSERT,      //  INS key
    VK_DELETE,      //  DEL key
    VK_HELP,        //  HELP key
    VK_F1,                // F1 key
    VK_F2,                // F2 key
    VK_F3,                // F3 key
    VK_F4,                // F4 key
    VK_F5,                // F5 key
    VK_F6,                // F6 key
    VK_F7,                // F7 key
    VK_F8,                // F8 key
    VK_F9,                // F9 key
    VK_F10,               // F10 key
    VK_F11,               // F11 key
    VK_F12,               // F12 key
    VK_F13,               // F13 key
    VK_F14,               // F14 key
    VK_F15,               // F15 key
    VK_F16,               // F16 key
    VK_F17,               // F17 key
    VK_F18,               // F18 key
    VK_F19,               // F19 key
    VK_F20,               // F20 key
    VK_F21,               // F21 key
    VK_F22,               // F22 key
    VK_F23,              // F23 key
    VK_F24,              // F24 key
    VK_NUMLOCK,          // NUM LOCK key
    VK_SCROLL,           // SCROLL LOCK key
    VK_LSHIFT,           // Left SHIFT key
    VK_RSHIFT,           // Right SHIFT key
    VK_LCONTROL,         // Left CONTROL key
    VK_RCONTROL,         // Right CONTROL key
    VK_LMENU,            // Left ALT key
    VK_RMENU,            // Right ALT key
    VK_OEM_8,
    VK_OEM_102,
};

OS_keyboardKey OS_OSKeyToKeyboardKey[] = {
    [0x00]=0,
    [0x01]=0,
    [0x02]=0,
    [0x03]=0,
    [0x04]=0,
    [0x05]=0,
    [0x07]=0,
    [0x08]=OS_KEYBOARD_BACKSPACE,
    [0x09]=OS_KEYBOARD_TAB,
    [0x0A]=0,
    [0x0B]=0,
    [0x0C]=OS_KEYBOARD_CLEAR,
    [0x0D]=OS_KEYBOARD_ENTER,
    [0x0E]=0,
    [0x0F]=0,

    [0x10]=OS_KEYBOARD_SHIFT,
    [0x11]=OS_KEYBOARD_CTRL,
    [0x12]=OS_KEYBOARD_ALT,
    [0x13]=OS_KEYBOARD_PAUSE,
    [0x14]=OS_KEYBOARD_CAPS_LOCK,
    [0x15]=0,
    [0x16]=0,
    [0x17]=0,
    [0x18]=0,
    [0x19]=0,
    [0x1A]=0,
    [0x1B]=OS_KEYBOARD_ESCAPE,
    [0x1C]=0,
    [0x1D]=0,
    [0x1E]=0,
    [0x1F]=0,

    [0x20]=OS_KEYBOARD_SPACE,
    [0x21]=OS_KEYBOARD_PAGE_UP,
    [0x22]=OS_KEYBOARD_PAGE_DOWN,
    [0x23]=OS_KEYBOARD_END,
    [0x24]=OS_KEYBOARD_HOME,
    [0x25]=OS_KEYBOARD_LEFT_ARROW,
    [0x26]=OS_KEYBOARD_UP_ARROW,
    [0x27]=OS_KEYBOARD_RIGHT_ARROW,
    [0x28]=OS_KEYBOARD_DOWN_ARROW,
    [0x29]=OS_KEYBOARD_SELECT,
    [0x2A]=OS_KEYBOARD_PRINT,
    [0x2B]=OS_KEYBOARD_EXECUTE,
    [0x2C]=OS_KEYBOARD_PRINT_SCREEN,
    [0x2D]=OS_KEYBOARD_INSERT,
    [0x2E]=OS_KEYBOARD_DELETE,
    [0x2F]=OS_KEYBOARD_HELP,

    [0x30]=OS_KEYBOARD_0,
    [0x31]=OS_KEYBOARD_1,
    [0x32]=OS_KEYBOARD_2,
    [0x33]=OS_KEYBOARD_3,
    [0x34]=OS_KEYBOARD_4,
    [0x35]=OS_KEYBOARD_5,
    [0x36]=OS_KEYBOARD_6,
    [0x37]=OS_KEYBOARD_7,
    [0x38]=OS_KEYBOARD_8,
    [0x39]=OS_KEYBOARD_9,
    [0x3A]=0,
    [0x3B]=0,
    [0x3C]=0,
    [0x3D]=0,
    [0x3E]=0,
    [0x3F]=0,

    [0x40]=0,
    [0x41]=OS_KEYBOARD_A,
    [0x42]=OS_KEYBOARD_B,
    [0x43]=OS_KEYBOARD_C,
    [0x44]=OS_KEYBOARD_D,
    [0x45]=OS_KEYBOARD_E,
    [0x46]=OS_KEYBOARD_F,
    [0x47]=OS_KEYBOARD_G,
    [0x48]=OS_KEYBOARD_H,
    [0x49]=OS_KEYBOARD_I,
    [0x4A]=OS_KEYBOARD_J,
    [0x4B]=OS_KEYBOARD_K,
    [0x4C]=OS_KEYBOARD_L,
    [0x4D]=OS_KEYBOARD_M,
    [0x4E]=OS_KEYBOARD_N,
    [0x4F]=OS_KEYBOARD_O,
    [0x50]=OS_KEYBOARD_P,
    [0x51]=OS_KEYBOARD_Q,
    [0x52]=OS_KEYBOARD_R,
    [0x53]=OS_KEYBOARD_S,
    [0x54]=OS_KEYBOARD_T,
    [0x55]=OS_KEYBOARD_U,
    [0x56]=OS_KEYBOARD_V,
    [0x57]=OS_KEYBOARD_W,
    [0x58]=OS_KEYBOARD_X,
    [0x59]=OS_KEYBOARD_Y,
    [0x5A]=OS_KEYBOARD_Z,
    [0x5B]=0,
    [0x5C]=0,
    [0x5D]=0,
    [0x5E]=0,
    [0x5F]=0,

    [0x60]=OS_KEYBOARD_NUMPAD_0,
    [0x61]=OS_KEYBOARD_NUMPAD_1,
    [0x62]=OS_KEYBOARD_NUMPAD_2,
    [0x63]=OS_KEYBOARD_NUMPAD_3,
    [0x64]=OS_KEYBOARD_NUMPAD_4,
    [0x65]=OS_KEYBOARD_NUMPAD_5,
    [0x66]=OS_KEYBOARD_NUMPAD_6,
    [0x67]=OS_KEYBOARD_NUMPAD_7,
    [0x68]=OS_KEYBOARD_NUMPAD_8,
    [0x69]=OS_KEYBOARD_NUMPAD_9,
    [0x6A]=OS_KEYBOARD_NUMPAD_MULTIPLY,
    [0x6B]=OS_KEYBOARD_NUMPAD_ADD,
    [0x6C]=OS_KEYBOARD_NUMPAD_SEPARATOR,
    [0x6D]=OS_KEYBOARD_NUMPAD_SUBTRACT,
    [0x6E]=OS_KEYBOARD_NUMPAD_DECIMAL,
    [0x6F]=OS_KEYBOARD_NUMPAD_DIVIDE,

    [0x70]=OS_KEYBOARD_F1,
    [0x71]=OS_KEYBOARD_F2,
    [0x72]=OS_KEYBOARD_F3,
    [0x73]=OS_KEYBOARD_F4,
    [0x74]=OS_KEYBOARD_F5,
    [0x75]=OS_KEYBOARD_F6,
    [0x76]=OS_KEYBOARD_F7,
    [0x77]=OS_KEYBOARD_F8,
    [0x78]=OS_KEYBOARD_F9,
    [0x79]=OS_KEYBOARD_F10,
    [0x7A]=OS_KEYBOARD_F11,
    [0x7B]=OS_KEYBOARD_F12,
    [0x7C]=OS_KEYBOARD_F13,
    [0x7D]=OS_KEYBOARD_F14,
    [0x7E]=OS_KEYBOARD_F15,
    [0x7F]=OS_KEYBOARD_F16,

    [0x80]=OS_KEYBOARD_F17,
    [0x81]=OS_KEYBOARD_F18,
    [0x82]=OS_KEYBOARD_F19,
    [0x83]=OS_KEYBOARD_F20,
    [0x84]=OS_KEYBOARD_F21,
    [0x85]=OS_KEYBOARD_F22,
    [0x86]=OS_KEYBOARD_F23,
    [0x87]=OS_KEYBOARD_F24,
    [0x88]=0,
    [0x89]=0,
    [0x8A]=0,
    [0x8B]=0,
    [0x8C]=0,
    [0x8D]=0,
    [0x8E]=0,
    [0x8F]=0,

    [0x90]=OS_KEYBOARD_NUMLOCK,
    [0x91]=OS_KEYBOARD_SCROLL,
    [0x92]=0,
    [0x93]=0,
    [0x94]=0,
    [0x95]=0,
    [0x96]=0,
    [0x97]=0,
    [0x98]=0,
    [0x99]=0,
    [0x9A]=0,
    [0x9B]=0,
    [0x9C]=0,
    [0x9D]=0,
    [0x9E]=0,
    [0x9F]=0,

    [0xA0]=0,
    [0xA1]=0,
    [0xA2]=0,
    [0xA3]=0,
    [0xA4]=0,
    [0xA5]=0,
    [0xA6]=0,
    [0xA7]=0,
    [0xA8]=0,
    [0xA9]=0,
    [0xAA]=0,
    [0xAB]=0,
    [0xAC]=0,
    [0xAD]=0,
    [0xAE]=0,
    [0xAF]=0,

    [0xB0]=0,
    [0xB1]=0,
    [0xB2]=0,
    [0xB3]=0,
    [0xB4]=0,
    [0xB5]=0,
    [0xB6]=0,
    [0xB7]=0,
    [0xB8]=0,
    [0xB9]=0,
    [0xBA]=OS_KEYBOARD_SEMICOLON,
    [0xBB]=OS_KEYBOARD_PLUS,
    [0xBC]=OS_KEYBOARD_COMMA,
    [0xBD]=OS_KEYBOARD_MINUS,
    [0xBE]=OS_KEYBOARD_PERIOD,
    [0xBF]=OS_KEYBOARD_FORWARD_SLASH,

    [0xC0]=OS_KEYBOARD_TILDE,
    [0xC1]=0,
    [0xC2]=0,
    [0xC3]=0,
    [0xC4]=0,
    [0xC5]=0,
    [0xC6]=0,
    [0xC7]=0,
    [0xC8]=0,
    [0xC9]=0,
    [0xCA]=0,
    [0xCB]=0,
    [0xCC]=0,
    [0xCD]=0,
    [0xCE]=0,
    [0xCF]=0,

    [0xD0]=OS_KEYBOARD_TILDE,
    [0xD1]=0,
    [0xD2]=0,
    [0xD3]=0,
    [0xD4]=0,
    [0xD5]=0,
    [0xD6]=0,
    [0xD7]=0,
    [0xD8]=0,
    [0xD9]=0,
    [0xDA]=0,
    [0xDB]=OS_KEYBOARD_OEM_LEFT_BRACE,
    [0xDC]=OS_KEYBOARD_RIGHT_BRACE,
    [0xDD]=OS_KEYBOARD_BACKSLASH,
    [0xDE]=OS_KEYBOARD_QUOTE,
    [0xDF]=0,
};

const char *OS_mouseButtonNames[OS_MOUSE_BUTTONS_LENGTH] = {
    "MOUSE_LEFT",
    "MOUSE_MIDDLE",
    "MOUSE_RIGHT",
    "MOUSE_XBUTTON_1",
    "MOUSE_XBUTTON_2",
};

const char *OS_keyboardKeyNames[OS_KEYBOARD_KEYS_LENGTH] = {
    "OS_KEYBOARD_NULL",
    "OS_KEYBOARD_SPACE",
    "OS_KEYBOARD_0",
    "OS_KEYBOARD_1",
    "OS_KEYBOARD_2",
    "OS_KEYBOARD_3",
    "OS_KEYBOARD_4",
    "OS_KEYBOARD_5",
    "OS_KEYBOARD_6",
    "OS_KEYBOARD_7",
    "OS_KEYBOARD_8",
    "OS_KEYBOARD_9",
    "OS_KEYBOARD_A",
    "OS_KEYBOARD_B",
    "OS_KEYBOARD_C",
    "OS_KEYBOARD_D",
    "OS_KEYBOARD_E",
    "OS_KEYBOARD_F",
    "OS_KEYBOARD_G",
    "OS_KEYBOARD_H",
    "OS_KEYBOARD_I",
    "OS_KEYBOARD_J",
    "OS_KEYBOARD_K",
    "OS_KEYBOARD_L",
    "OS_KEYBOARD_M",
    "OS_KEYBOARD_N",
    "OS_KEYBOARD_O",
    "OS_KEYBOARD_P",
    "OS_KEYBOARD_Q",
    "OS_KEYBOARD_R",
    "OS_KEYBOARD_S",
    "OS_KEYBOARD_T",
    "OS_KEYBOARD_U",
    "OS_KEYBOARD_V",
    "OS_KEYBOARD_W",
    "OS_KEYBOARD_X",
    "OS_KEYBOARD_Y",
    "OS_KEYBOARD_Z",
    "OS_KEYBOARD_NUMPAD_0",
    "OS_KEYBOARD_NUMPAD_1",
    "OS_KEYBOARD_NUMPAD_2",
    "OS_KEYBOARD_NUMPAD_3",
    "OS_KEYBOARD_NUMPAD_4",
    "OS_KEYBOARD_NUMPAD_5",
    "OS_KEYBOARD_NUMPAD_6",
    "OS_KEYBOARD_NUMPAD_7",
    "OS_KEYBOARD_NUMPAD_8",
    "OS_KEYBOARD_NUMPAD_9",
    "OS_KEYBOARD_MULTIPLY",
    "OS_KEYBOARD_ADD",
    "OS_KEYBOARD_SEPARATOR",
    "OS_KEYBOARD_SUBTRACT",
    "OS_KEYBOARD_DECIMAL",
    "OS_KEYBOARD_DIVIDE",
    "OS_KEYBOARD_COMMA",
    "OS_KEYBOARD_PERIOD",
    "OS_KEYBOARD_MINUS",
    "OS_KEYBOARD_PLUS",
    "OS_KEYBOARD_SEMICOLON (OEM_1)",
    "OS_KEYBOARD_FORWARD_SLASH (OEM_2)",
    "OS_KEYBOARD_TILDE (OEM_3)",
    "OS_KEYBOARD_OEM_LEFT_BRACE (OEM_4)",
    "OS_KEYBOARD_BACKSLASH (OEM_5)",
    "OS_KEYBOARD_RIGHT_BRACE (OEM_6)",
    "OS_KEYBOARD_QUOTE (OEM_7)",
    "OS_KEYBOARD_BACKSPACE",
    "OS_KEYBOARD_TAB",
    "OS_KEYBOARD_CLEAR",
    "OS_KEYBOARD_ENTER",
    "OS_KEYBOARD_SHIFT",
    "OS_KEYBOARD_CTRL",
    "OS_KEYBOARD_ALT",
    "OS_KEYBOARD_PAUSE",
    "OS_KEYBOARD_CAPS_LOCK",
    "OS_KEYBOARD_ESCAPE",
    "OS_KEYBOARD_PAGE_UP",
    "OS_KEYBOARD_PAGE_DOWN",
    "OS_KEYBOARD_END",
    "OS_KEYBOARD_HOME",
    "OS_KEYBOARD_LEFT_ARROW",
    "OS_KEYBOARD_UP_ARROW",
    "OS_KEYBOARD_RIGHT_ARROW",
    "OS_KEYBOARD_DOWN_ARROW",
    "OS_KEYBOARD_SELECT",
    "OS_KEYBOARD_PRINT",
    "OS_KEYBOARD_EXECUTE",
    "OS_KEYBOARD_PRINT_SCREEN",
    "OS_KEYBOARD_INSERT",
    "OS_KEYBOARD_DELETE",
    "OS_KEYBOARD_HELP",
    "OS_KEYBOARD_F1",
    "OS_KEYBOARD_F2",
    "OS_KEYBOARD_F3",
    "OS_KEYBOARD_F4",
    "OS_KEYBOARD_F5",
    "OS_KEYBOARD_F6",
    "OS_KEYBOARD_F7",
    "OS_KEYBOARD_F8",
    "OS_KEYBOARD_F9",
    "OS_KEYBOARD_F10",
    "OS_KEYBOARD_F11",
    "OS_KEYBOARD_F12",
    "OS_KEYBOARD_F13",
    "OS_KEYBOARD_F14",
    "OS_KEYBOARD_F15",
    "OS_KEYBOARD_F16",
    "OS_KEYBOARD_F17",
    "OS_KEYBOARD_F18",
    "OS_KEYBOARD_F19",
    "OS_KEYBOARD_F20",
    "OS_KEYBOARD_F21",
    "OS_KEYBOARD_F22",
    "OS_KEYBOARD_F23",
    "OS_KEYBOARD_F24",
    "OS_KEYBOARD_NUMLOCK",
    "OS_KEYBOARD_SCROLL",
    "OS_KEYBOARD_LSHIFT",
    "OS_KEYBOARD_RSHIFT",
    "OS_KEYBOARD_LCONTROL",
    "OS_KEYBOARD_RCONTROL",
    "OS_KEYBOARD_LMENU",
    "OS_KEYBOARD_RMENU",
    "OS_KEYBOARD_OEM_8",
    "OS_KEYBOARD_OEM_102",
};

U64 HashString(const char *key) {
    U64 hash = 5381;
    S32 c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

U64 RollingHashString(char c, U64 hash) {
    if (hash == 0) hash = 5381;
    return ((hash << 5) + hash) + c;
}

char OS_Keyboard_keyToChar(OS_keyboardKey key) {
    if (key >= OS_KEYBOARD_FIRST_PRINTABLE_CHAR && key < OS_KEYBOARD_PRINTABLE_CHAR_END) {
        if ((OS_Keyboard_keyHeld[OS_KEYBOARD_SHIFT] || OS_Keyboard_keyPressed[OS_KEYBOARD_SHIFT] ) && OS_capsLockEnabled()) {
            return OS_Keyboard_keyToShiftUppercase[key];
        } else if (OS_Keyboard_keyHeld[OS_KEYBOARD_SHIFT] || OS_Keyboard_keyPressed[OS_KEYBOARD_SHIFT]) {
            return OS_Keyboard_keyToShift[key];
        } else if (OS_capsLockEnabled()) {
            return OS_Keyboard_keyToUppercase[key];
        } else {
            return OS_Keyboard_keyToLowercase[key];
        }
    }
    return 0;
}
struct OSLibrary {
    HMODULE module;
    char *filename;
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

const S32 *OS_getKeyboardPressed() { return OS_Keyboard_keyPressed; };
const S32 *OS_getKeyboardHeld() { return OS_Keyboard_keyHeld; };
const S32 *OS_getKeyboardReleased() { return OS_Keyboard_keyReleased; };
const V2_F32 *OS_getMousePosition() { return &OS_mousePosition; }
const V2_F32 *OS_getLastMousePosition() { return &OS_lastMousePosition; }
const V2_F32 *OS_getMouseMovement() { return &OS_mouseMovement; }

const S32 *OS_getMouseState() { return OS_Mouse_buttonState; }
const S32 *OS_getMouseState_thisFrame() { return OS_Mouse_buttonState_thisFrame; }


S32 OS_FS_fileExists(char *filename) {
    DWORD dwAttrib = GetFileAttributes(filename);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

List_t *OS_FS_Scandir(const char *folder, OS_SCANDIR_FILTER filter) {
    WIN32_FIND_DATA ffd;
    DWORD err = 0;
    char ifolder[256], ofolder[256];
    if (folder[strlen(folder)-1] == '\\' || folder[strlen(folder)-1] == '/') {
        sprintf(ifolder, "%s*", folder);
        sprintf(ofolder, "%s", folder);
    } else {
        sprintf(ifolder, "%s\\*", folder);
        sprintf(ofolder, "%s/", folder);
    }
    
    SetLastError(0);
    HANDLE hFind = FindFirstFile(ifolder, &ffd);
    List_t *files = 0;
    do {
        err = GetLastError();
        if (err) {
            if (err == ERROR_NO_MORE_FILES) {
                break;
            } else {
                char badPathMessage[150];
                sprintf(badPathMessage, "OS_FS_Scandir ERROR - bad path '%s'", ifolder);
                const Except_T e = {badPathMessage};
                RAISE(e);
                break;
            }
        }
        if (strlen(ffd.cFileName) == 1 && strcmp(".", ffd.cFileName) == 0) continue;
        if (strlen(ffd.cFileName) == 2 && strcmp("..", ffd.cFileName) == 0) continue;
        char *filename;
        NEWN(filename, strlen(ofolder)+strlen(ffd.cFileName)+1);
        sprintf(filename, "%s%s", ofolder, ffd.cFileName);
        files = List_append(files, filename,  (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? OS_DIR : OS_FILE);
    } while (FindNextFile(hFind, &ffd));
    
    FindClose(hFind);
    return files;
}

void OS_FS_setFileContents(const char *filename, void *data, U32 numBytes) {
    FILE *fp;
    errno_t err;
    if ((err = fopen_s(&fp, filename, "wb")) != 0) {
        printf("Could not open file: %s\n", filename);
        exit(-1);
    }
    fwrite(data, numBytes, 1, fp);
    fclose(fp);
}

char *OS_FS_getFileContents(const char *filename) {
    FILE *fp;
    errno_t err;
    if ((err = fopen_s(&fp, filename, "rb")) != 0) {
        printf("Could not open file: %s\n", filename);
        exit(-1);
    }
    char *buffer;
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    NEWN(buffer, fsize+1);
    fread(&buffer[0], fsize, 1, fp);
    buffer[fsize] = '\0';
    fclose(fp);
    return buffer;
}

void OS_FS_structLoad(const char *filename, void *dest, U32 numBytes) {
    char *dat = OS_FS_getFileContents(filename);
    memcpy(dest, dat, numBytes);
}

void OS_FS_structSave(const char *filename, void *src, U32 numBytes) {
    OS_FS_setFileContents(filename, src, numBytes);
}

U64 OS_currentTime() {
    return time(0);
}

U8 OS_capsLockEnabled() {
    return GetKeyState(VK_CAPITAL) & 0x0001;
}


struct OS_Timer {
    LARGE_INTEGER Frequency, StartingTime, EndingTime, ElapsedMicroseconds;
};

OS_Timer Timer_new() {
    OS_Timer t;
    NEW(t);
    QueryPerformanceFrequency(&t->Frequency);
    QueryPerformanceCounter(&t->StartingTime);
    return t;
}

void Timer_reset(OS_Timer t) {
    QueryPerformanceCounter(&t->StartingTime);
}

S64 Timer_getElapsedMicrosecondsAndLap(OS_Timer t) {
    QueryPerformanceCounter(&t->EndingTime);
    t->ElapsedMicroseconds.QuadPart = t->EndingTime.QuadPart - t->StartingTime.QuadPart;
    t->StartingTime.QuadPart = t->EndingTime.QuadPart;
    t->StartingTime = t->EndingTime;
    t->ElapsedMicroseconds.QuadPart *= 1000000;
    t->ElapsedMicroseconds.QuadPart /= t->Frequency.QuadPart;
    return t->ElapsedMicroseconds.QuadPart;
}

S64 Timer_dispose(OS_Timer *t) {
    float seconds = Timer_getElapsedMicrosecondsAndLap(*t);
    FREE(*t);
    t = 0;
    return seconds;
}

F64 microsecondsToSeconds(long long microseconds) {
    return microseconds / 1000000.f;
}

DWORD WINAPI RunThread(void *customData) {
    OS_thread ctd = customData;
    DWORD result = ctd->func(*ctd->customData);
    if (ctd->destroyData && ctd->customData) {
        FREE(*(ctd->customData));
        *(ctd->customData) = 0;
    }
    FREE(ctd);
    return result;
}

OSLibrary OS_Library(const char *name) {
    OSLibrary lib;
    NEW(lib);
    lib->module = LoadLibraryA(name);
    lib->filename = copyString(name);
    assert(lib->module);
    return lib;
}

int OS_Library_reload(OSLibrary lib) {
    // The library would not have recompiled if already loaded
    // our pattern will be [lib]_alt.dll
    char alternateFile[100];
    size_t dotpos = strlen(lib->filename)-4;
    char *f = "_alt.dll";
    memcpy(alternateFile, lib->filename, dotpos);
    memcpy(&alternateFile[dotpos], &f[0], 9);
    if (OS_FS_fileExists(alternateFile)) {
        FreeLibrary(lib->module);
        remove(lib->filename);
        rename(alternateFile, lib->filename);
        lib->module = LoadLibraryA(lib->filename);
        if (lib->module) {
            return 1;
        }
        return 0;
    }
    return 0;
}

void *OS_LibraryFunction(OSLibrary lib, char *functionName) {
    return GetProcAddress(lib->module, functionName);
}

void OS_runThreaded(OS_ThreadableFunction func, void *customData) {
    HANDLE handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, customData, 0, NULL);
    CloseHandle(handle);
}

char *copyString(const char *in) {
    if (!in) return 0;
    char *c;
    NEWN(c, strlen(in)+1);
    sprintf(c, "%s", in); 
    return c;
}

int OS_GraphicsWindow_close() {
    ChangeDisplaySettings(0, 0);
    ShowCursor(1);
    return 1;
}

U32 OS_Window_screenWidth(OS_Window window) {
    return window->width;
}

U32 OS_Window_screenHeight(OS_Window window) {
    return window->height;
}

void OS_Window_swapBuffers(OS_Window window) {
    SwapBuffers(window->hDC);
}

U32 OS_Window_quitRequested(OS_Window window) {
    return window->quitRequested;
}

void OS_Window_connect(
    OS_Window window,
    void *customDataIn,
    OS_Callback_resize resizeFunc,
    OS_Callback_mouseMove mouseMoveFunc,
    OS_Callback_mouseScroll mouseScrollFunc,
    OS_Callback_mouseClick mouseClickFunc,
    OS_Callback_keyPress keyPressFunc,
    S32 **keyPressedRef,
    S32 **keyHeldRef,
    S32 **keyReleasedRef
) {
    OS_customData = customDataIn;
    window->resizeFunc      = resizeFunc;     
    window->mouseMoveFunc   = mouseMoveFunc;  
    window->mouseScrollFunc = mouseScrollFunc;
    window->mouseClickFunc  = mouseClickFunc; 
    window->keyPressFunc    = keyPressFunc;

    *keyPressedRef = OS_Keyboard_keyPressed;
    *keyHeldRef = OS_Keyboard_keyHeld;
    *keyReleasedRef = OS_Keyboard_keyReleased;
}

void *GetAnyGLFuncAddress(const char *name)
{
  void *p = (void *)wglGetProcAddress(name);
  if(p == 0 ||
    (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
    (p == (void*)-1) )
  {
    HMODULE module = LoadLibraryA("opengl32.dll");
    p = (void *)GetProcAddress(module, name);
  }

  return p;
}
typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext, const int *attribList);
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

static void init_opengl_extensions() {
    WNDCLASSA window_class = {
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = DefWindowProcA,
        .hInstance = GetModuleHandle(0),
        .lpszClassName = "Dummy_WGL_djuasiodwa",
    };

    if (!RegisterClassA(&window_class)) {
        printf("Failed to register dummy OpenGL window.");
        exit(-1);
    }

    HWND dummy_window = CreateWindowExA(
        0,
        window_class.lpszClassName,
        "Dummy OpenGL Window",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        window_class.hInstance,
        0);

    if (!dummy_window) {
        printf("Failed to create dummy OpenGL window.");
        exit(-1);
    }

    HDC dummy_dc = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR dpfd = {
        .nSize = sizeof(dpfd),
        .nVersion = 1,
        .iPixelType = PFD_TYPE_RGBA,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .cColorBits = 32,
        .cAlphaBits = 8,
        .iLayerType = PFD_MAIN_PLANE,
        .cDepthBits = 24,
        .cStencilBits = 8,
    };

    int pixel_format = ChoosePixelFormat(dummy_dc, &dpfd);

    if (!pixel_format) {
        printf("Failed to find a suitable pixel format.");
        exit(-1);
    }

    if (!SetPixelFormat(dummy_dc, pixel_format, &dpfd)) {
        printf("Failed to set the pixel format.");
        exit(-1);
    }

    HGLRC dummy_context = wglCreateContext(dummy_dc);

    if (!dummy_context) {
        printf("Failed to create a dummy OpenGL rendering context.");
        exit(-1);
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context)) {
        printf("Failed to activate dummy OpenGL rendering context.");
        exit(-1);
    }

    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress("wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress("wglChoosePixelFormatARB");

    wglMakeCurrent(dummy_dc, 0);
    wglDeleteContext(dummy_context);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
}

HGLRC init_opengl(HDC real_dc) {
    init_opengl_extensions();
    
    // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        
        0
    };

    int pixel_format;
    UINT num_formats;
    wglChoosePixelFormatARB(real_dc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats) {
        printf("Failed to set the OpenGL 3.3 pixel format.");
        exit(-1);
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(real_dc, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(real_dc, pixel_format, &pfd)) {
        printf("Failed to set the OpenGL 3.3 pixel format.");
        exit(-1);
    }

    // Specify that we want to create an OpenGL 3.3 core profile context
    int gl33_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    HGLRC gl33_context = wglCreateContextAttribsARB(real_dc, 0, gl33_attribs);
    if (!gl33_context) {
        printf("Failed to create OpenGL 3.3 context.");
        exit(-1);
    }

    if (!wglMakeCurrent(real_dc, gl33_context)) {
        printf("Failed to activate OpenGL 3.3 rendering context.");
        exit(-1);
    }

    return gl33_context;
}

static HWND create_window(HINSTANCE inst) {
    WNDCLASSA window_class = {
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = WndProc,
        .hInstance = inst,
        .hCursor = LoadCursor(0, IDC_ARROW),
        .hbrBackground = 0,
        .lpszClassName = "WGL_fdjhsklf",
    };

    if (!RegisterClassA(&window_class)) {
        printf("Failed to register window.");
        exit(-1);
    }

    // Specify a desired width and height, then adjust the rect so the window's client area will be
    // that size.
    RECT rect = {
        .right = 1920,
        .bottom = 1080,
        .top = 0,
        .left = 0
    };
    // DWORD window_style = WS_OVERLAPPEDWINDOW;
    DWORD window_style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    DWORD window_extended_style = WS_EX_TOPMOST;
    // DWORD window_extended_style = WS_EX_OVERLAPPEDWINDOW;
    AdjustWindowRect(&rect, window_style, 0);

    HWND window = CreateWindowExA(
        window_extended_style,
        window_class.lpszClassName,
        "OpenGL",
        window_style,
        0,
        0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        0,
        0,
        inst,
        0);

    if (!window) {
        printf("Failed to create window.");
        exit(-1);
    }

    DEVMODE fullscreenSettings;
    int isChangeSuccessful;
    // RECT windowBoundary;

    EnumDisplaySettings(NULL, 0, &fullscreenSettings);
    fullscreenSettings.dmPelsWidth        = 1920;
    fullscreenSettings.dmPelsHeight       = 1080;
    fullscreenSettings.dmBitsPerPel       = 32;
    fullscreenSettings.dmDisplayFrequency = 60;
    fullscreenSettings.dmFields           = DM_PELSWIDTH |
                                            DM_PELSHEIGHT |
                                            DM_BITSPERPEL |
                                            DM_DISPLAYFREQUENCY;

    SetWindowLongPtr(window, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
    SetWindowLongPtr(window, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
    isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
    ShowWindow(window, SW_MAXIMIZE);
    SetForegroundWindow(window);
    SetFocus(window);


    // Test
    SetCapture(window);

    // Capture cursor
    RECT clipRect;
    GetClientRect(window, &clipRect);
    ClientToScreen(window, (POINT*) &clipRect.left);
    ClientToScreen(window, (POINT*) &clipRect.right);
    ClipCursor(&clipRect);


    return window;
}

void OS_Window_toFullScreen() {
 DEVMODE fullscreenSettings;
    int isChangeSuccessful;
    // RECT windowBoundary;

    EnumDisplaySettings(NULL, 0, &fullscreenSettings);
    fullscreenSettings.dmPelsWidth        = 1920;
    fullscreenSettings.dmPelsHeight       = 1080;
    fullscreenSettings.dmBitsPerPel       = 32;
    fullscreenSettings.dmDisplayFrequency = 60;
    fullscreenSettings.dmFields           = DM_PELSWIDTH |
                                            DM_PELSHEIGHT |
                                            DM_BITSPERPEL |
                                            DM_DISPLAYFREQUENCY;

    SetWindowLongPtr(OS_activeWindow->hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
    SetWindowLongPtr(OS_activeWindow->hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(OS_activeWindow->hwnd, HWND_TOPMOST, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
    isChangeSuccessful = ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
    ShowWindow(OS_activeWindow->hwnd, SW_MAXIMIZE);
}

OS_Window OS_OpenGLWindow(U32 width, U32 height, U32 bitsPerPixel, U32 isFullScreen) {
    HINSTANCE hInstance = GetModuleHandle(0);
    HWND window = create_window(hInstance);
    HDC gldc = GetDC(window);
    // HGLRC glrc = init_opengl(gldc);
    init_opengl(gldc);
    
    OS_Window w;
    NEW(w);
    w->quitRequested = 0;
    w->width = width;
    w->height = height;
    w->bitsPerPixel = bitsPerPixel;
    w->isFullScreen = isFullScreen;
    w->hInstance = GetModuleHandle(0);
    w->hDC = gldc;
    OS_activeWindow = w;
    
    ShowWindow(window, 1);
    UpdateWindow(window);
    OS_Window_changeCursor(OS_CURSOR_CROSSHAIR);
    // OS_Window_toFullScreen();
    // SetForegroundWindow(w->hwnd);
    // SetFocus(w->hwnd);
    
    w->resizeFunc      = 0;
    w->mouseMoveFunc   = 0;
    w->mouseScrollFunc = 0;
    w->mouseClickFunc  = 0;
    w->keyPressFunc    = 0;
    

    return w;
}

void OS_handleEvents() {
    OS_Window_handleEvents(OS_activeWindow);
}

void OS_triggerKeyUp(OS_Window window, OS_keyboardKey key) {
    OS_Keyboard_keyPressed[key] = 0;
    OS_Keyboard_keyHeld[key] = 0;
    OS_Keyboard_keyReleased[key] = 1;
    if (OS_activeWindow->keyPressFunc) {
        OS_activeWindow->keyPressFunc(OS_customData, key, 0);
    }
}

void OS_triggerMouseMove(OS_Window window, int mouseX, int mouseY) {
    OS_mousePosition.x = -1.0 + (float)mouseX / window->width * 2.0;
    OS_mousePosition.y = 1.0 - (float)mouseY / window->height * 2.0;
    if (window->mouseMoveFunc) {
        window->mouseMoveFunc(OS_customData, &OS_mousePosition);
    }
}

void OS_Window_handleEvents(OS_Window window) {
    MSG message;

    // Mouse state sanitation:
    //  - Reset mouse release so that we only process it on one frame.
    //  - Clear all "this frame" mouse state.
    for (S32 i = 0; i < OS_MOUSE_BUTTONS_LENGTH; i++) {
        OS_Mouse_buttonState_thisFrame[i] = 0;
        if (OS_Mouse_buttonState[i] == MOUSE_RELEASE) {
            OS_Mouse_buttonState[i] = MOUSE_IDLE;
        }
    }
    // Initialise all "last frame" presses to 0
    // Create events for all delayed key releases
    for (int i = 0; i < OS_KEYBOARD_KEYS_LENGTH; i++) {
        OS_Keyboard_keyPressed_thisFrame[i] = 0;
        OS_Keyboard_keyReleased[i] = 0;
        if (OS_Keyboard_delayedRelease[i]) {
            OS_triggerKeyUp(window, i);
            OS_Keyboard_delayedRelease[i] = 0;
        }
    }
    OS_mouseMovement.x = 0;
    OS_mouseMovement.y = 0;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            window->quitRequested = 1;
            break;
        }
        else {
            // TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    if (window->mouseMoveFunc && OS_mousePositionChanged) {
        window->mouseMoveFunc(OS_customData, &OS_mousePosition);
    }
    OS_mousePositionChanged = 0;
}

#define TMP_HANDLE_EVT(E) case E: printf("%s (%d)\n", #E, E); break
#define TMP_HANDLE_EVT_R(E) case E: printf("%s (%d)\n", #E, E); return 0

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {

        // WINDOW LIFECYCLE
        case WM_CLOSE: { DestroyWindow(hwnd); } break;
        case WM_DESTROY: { PostQuitMessage(0); } break;
        TMP_HANDLE_EVT(WM_KILLFOCUS);
        TMP_HANDLE_EVT(WM_SETFOCUS);
        TMP_HANDLE_EVT(WM_ACTIVATE);
        TMP_HANDLE_EVT(WM_NCHITTEST);
    
        TMP_HANDLE_EVT(WM_NCACTIVATE);
        TMP_HANDLE_EVT(WM_ACTIVATEAPP);
        TMP_HANDLE_EVT(WM_IME_SETCONTEXT);
        TMP_HANDLE_EVT(WM_IME_NOTIFY);
        
        // MOUSE ICON CURSOR
        TMP_HANDLE_EVT(WM_GETICON);
        TMP_HANDLE_EVT(WM_SETCURSOR);
        TMP_HANDLE_EVT(WM_NCMOUSEMOVE);
        TMP_HANDLE_EVT(WM_NCMOUSELEAVE);
    
        TMP_HANDLE_EVT(WM_WINDOWPOSCHANGING);
        TMP_HANDLE_EVT(WM_WINDOWPOSCHANGED);
        TMP_HANDLE_EVT(WM_SIZE);
        TMP_HANDLE_EVT(WM_PAINT);
        

        // MOUSE
        // S32 OS_Mouse_buttonClicked[OS_MOUSE_BUTTONS_LENGTH] = {0};
        // S32 OS_Mouse_buttonHeld[OS_MOUSE_BUTTONS_LENGTH] = {0};
        // S32 OS_Mouse_buttonReleased[OS_MOUSE_BUTTONS_LENGTH] = {0};
        // S32 OS_Mouse_buttonClicked_thisFrame[OS_MOUSE_BUTTONS_LENGTH] = {0};
        case WM_LBUTTONDOWN: { // 513
            OS_Mouse_buttonState[OS_MOUSE_LEFT]++;
            if (OS_Mouse_buttonState[OS_MOUSE_LEFT] > MOUSE_HOLD) {
                OS_Mouse_buttonState[OS_MOUSE_LEFT] = MOUSE_HOLD;
            }
            OS_Mouse_buttonState_thisFrame[OS_MOUSE_LEFT] = OS_Mouse_buttonState[OS_MOUSE_LEFT];

            if (OS_activeWindow->mouseClickFunc) {
                OS_activeWindow->mouseClickFunc(OS_customData, OS_MOUSE_LEFT, OS_Mouse_buttonState[OS_MOUSE_LEFT], 0, &OS_mousePosition);
            }
        } break;
        case WM_LBUTTONUP: { // 514
            OS_Mouse_buttonState[OS_MOUSE_LEFT] = OS_Mouse_buttonState_thisFrame[OS_MOUSE_LEFT] = MOUSE_RELEASE;
            if (OS_activeWindow->mouseClickFunc) {
                OS_activeWindow->mouseClickFunc(OS_customData, OS_MOUSE_LEFT, OS_Mouse_buttonState[OS_MOUSE_LEFT], 0, &OS_mousePosition);
            }
        } break;
        case WM_RBUTTONDOWN: {
            OS_Mouse_buttonState[OS_MOUSE_RIGHT]++;
            if (OS_Mouse_buttonState[OS_MOUSE_RIGHT] > MOUSE_HOLD) {
                OS_Mouse_buttonState[OS_MOUSE_RIGHT] = MOUSE_HOLD;
            }
            OS_Mouse_buttonState_thisFrame[OS_MOUSE_RIGHT] = OS_Mouse_buttonState[OS_MOUSE_RIGHT];

            if (OS_activeWindow->mouseClickFunc) {
                OS_activeWindow->mouseClickFunc(OS_customData, OS_MOUSE_RIGHT, OS_Mouse_buttonState[OS_MOUSE_RIGHT], 0, &OS_mousePosition);
            }

        } break;
        case WM_RBUTTONUP: {
            OS_Mouse_buttonState[OS_MOUSE_RIGHT] = OS_Mouse_buttonState_thisFrame[OS_MOUSE_RIGHT] = MOUSE_RELEASE;
            if (OS_activeWindow->mouseClickFunc) {
                OS_activeWindow->mouseClickFunc(OS_customData, OS_MOUSE_RIGHT, OS_Mouse_buttonState[OS_MOUSE_RIGHT], 0, &OS_mousePosition);
            }
        } break;
        case WM_MBUTTONDOWN: {
            if (++OS_Mouse_buttonState[OS_MOUSE_MIDDLE] > MOUSE_HOLD) {
                OS_Mouse_buttonState[OS_MOUSE_MIDDLE] = MOUSE_HOLD;
            }
            OS_Mouse_buttonState_thisFrame[OS_MOUSE_MIDDLE] = OS_Mouse_buttonState[OS_MOUSE_MIDDLE];

            if (OS_activeWindow->mouseClickFunc) {
                OS_activeWindow->mouseClickFunc(OS_customData, OS_MOUSE_MIDDLE, OS_Mouse_buttonState[OS_MOUSE_MIDDLE], 0, &OS_mousePosition);
            }
        } break;
        case WM_MBUTTONUP: {
            OS_Mouse_buttonState[OS_MOUSE_MIDDLE] = OS_Mouse_buttonState_thisFrame[OS_MOUSE_MIDDLE] = MOUSE_RELEASE;
            if (OS_activeWindow->mouseClickFunc) {
                OS_activeWindow->mouseClickFunc(OS_customData, OS_MOUSE_MIDDLE, OS_Mouse_buttonState[OS_MOUSE_MIDDLE], 0, &OS_mousePosition);
            }
        } break;
        case WM_XBUTTONDOWN: {
            if (++OS_Mouse_buttonState[OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam)] > MOUSE_HOLD) {
                OS_Mouse_buttonState[OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam)] = MOUSE_HOLD;
            }
            OS_Mouse_buttonState_thisFrame[OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam)] = OS_Mouse_buttonState[OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam)];

            if (OS_activeWindow->mouseClickFunc) {
                OS_activeWindow->mouseClickFunc(OS_customData, OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam), OS_Mouse_buttonState[OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam)], 0, &OS_mousePosition);
            }
        } break;
        case WM_XBUTTONUP: {
            OS_Mouse_buttonState[OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam)] = OS_Mouse_buttonState_thisFrame[OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam)] = MOUSE_RELEASE;
            if (OS_activeWindow->mouseClickFunc) {
                OS_activeWindow->mouseClickFunc(OS_customData, OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam), OS_Mouse_buttonState[OS_MOUSE_XBUTTON1 - 1 + GET_XBUTTON_WPARAM(wparam)], 0, &OS_mousePosition);
            }
        } break;
        

        case WM_MOUSEWHEEL: { // 522
            if (OS_activeWindow->mouseScrollFunc) {
                OS_activeWindow->mouseScrollFunc(OS_customData, 0, 0);
            }
        } break;
        case WM_CONTEXTMENU: { //123 (right click?)
            // if (OS_activeWindow->mouseClickFunc) {
                // OS_activeWindow->mouseClickFunc(OS_customData, OS_MOUSE_RIGHT_BUTTON, OS_MOUSE, 0, 0);
            // }
        }
    //     case WM_MOUSEACTIVATE: { //33
    //         printf("WM_MOUSEACTIVATE\n");
    //     } break;
        case WM_MOUSEMOVE: {
            // printf("WM_MOUSEMOVE\n");
            int mouseX = GET_X_LPARAM(lparam);
            int mouseY = GET_Y_LPARAM(lparam);
            // printf("%d %d\n", mouseX, mouseY);
            
            


            OS_lastMousePosition.x = OS_mousePosition.x;
            OS_lastMousePosition.y = OS_mousePosition.y;
            OS_mousePosition.x = -1.0 + (float)mouseX / OS_activeWindow->width * 2.0;
            OS_mousePosition.y = 1.0 - (float)mouseY / OS_activeWindow->height * 2.0;
            OS_mouseMovement.x = OS_mousePosition.x - OS_lastMousePosition.x;
            OS_mouseMovement.y = OS_lastMousePosition.y - OS_mousePosition.y;
            OS_mousePositionChanged = 1;
            return 0;
        } break;
	
        // // KEYBOARD HANDLING EVENTS
        case WM_KEYDOWN: {
            if (OS_OSKeyToKeyboardKey[wparam]) {
                OS_keyboardKey key = OS_OSKeyToKeyboardKey[wparam];
                if (!OS_Keyboard_keyPressed_thisFrame[key]) {
                    #if DEBUG_OS_KEYBOARD_KEY == 1
                    if (!OS_Keyboard_keyHeld[key] && !OS_Keyboard_keyPressed[key]) {
                        printf("Key Press: %s\n", OS_keyboardKeyNames[key]);
                    }
                    #endif
                    #if EVENTLOGGING == 1
                    if (!OS_Keyboard_keyHeld[key] && !OS_Keyboard_keyPressed[key]) {
                        char event[256];
                        sprintf(event, "PRESS %s", OS_keyboardKeyNames[key]);
                        OS_logEvent("BASE_OS", "KEYBOARD", event);
                    }
                    #endif
                    OS_Keyboard_keyHeld[key] = OS_Keyboard_keyPressed[key] || OS_Keyboard_keyHeld[key];
                    OS_Keyboard_keyPressed[key] = !OS_Keyboard_keyHeld[key];
                    #if DEBUG_OS_KEYBOARD_KEY == 1
                    if (!OS_Keyboard_keyHeld[key]) {
                        printf("Key Hold: %s\n", OS_keyboardKeyNames[key]);
                    }
                    #endif
                    #if EVENTLOGGING == 1
                    if (!OS_Keyboard_keyHeld[key]) {
                        char event[256];
                        sprintf(event, "HOLD %s", OS_keyboardKeyNames[key]);
                        OS_logEvent("BASE_OS", "KEYBOARD", event);
                    }
                    #endif
                }
                OS_Keyboard_keyPressed_thisFrame[key] = 1;
                if (OS_activeWindow->keyPressFunc) {
                    OS_activeWindow->keyPressFunc(OS_customData, key, 1);
                }
            }
        } break;
        case WM_KEYUP: {
            if (OS_OSKeyToKeyboardKey[wparam]) {
                OS_keyboardKey key = OS_OSKeyToKeyboardKey[wparam];
                // Key press and release would both be triggered this frame. Instead, push
                // the release to next frame
                if (OS_Keyboard_keyPressed_thisFrame[key]) {
                    OS_Keyboard_delayedRelease[key] = 1;
                } else {
                    #if DEBUG_OS_KEYBOARD_KEY == 1
                    printf("Key Release: %s\n", OS_keyboardKeyNames[key]);
                    #endif
                    #if EVENTLOGGING == 1
                    char event[256];
                    sprintf(event, "RELEASE %s", OS_keyboardKeyNames[key]);
                    OS_logEvent("BASE_OS", "KEYBOARD", event);
                    #endif
                    OS_Keyboard_keyPressed[key] = 0;
                    OS_Keyboard_keyHeld[key] = 0;
                    OS_Keyboard_keyReleased[key] = 1;
                    if (OS_activeWindow->keyPressFunc) {
                        OS_activeWindow->keyPressFunc(OS_customData, key, 0);
                    }
                    // OS_triggerKeyUp(OS_activeWindow, key);
                }   
            }
        } break;
        default:
            printf("Unhandled Message: %u\n", message);
            break;
    }
	return DefWindowProc(hwnd, message, wparam, lparam);
}

void OS_Window_changeCursor(OS_CursorType cursorType) {
    switch (cursorType) {
        case OS_CURSOR_ARROW: {
            HCURSOR cursor = LoadCursor(0, IDC_ARROW);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_CROSSHAIR: {
            HCURSOR cursor = LoadCursor(0, IDC_CROSS);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_HAND: {
            // HCURSOR cursor = LoadCursor(0, IDC_HAND);
            HCURSOR cursor = LoadCursor(NULL, IDC_HAND);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_HELP: {
            HCURSOR cursor = LoadCursor(0, IDC_HELP);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_IBEAM: {
            HCURSOR cursor = LoadCursor(0, IDC_IBEAM);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_NOTALLOWED: {
            HCURSOR cursor = LoadCursor(0, IDC_NO);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_RESIZE_ALL: {
            HCURSOR cursor = LoadCursor(0, IDC_SIZEALL);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_RESIZE_NORTH_SOUTH: {
            HCURSOR cursor = LoadCursor(0, IDC_SIZENS);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_RESIZE_NORTHEAST_SOUTHWEST: {
            HCURSOR cursor = LoadCursor(0, IDC_SIZENESW);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_RESIZE_NORTHWEST_SOUTHEAST: {
            HCURSOR cursor = LoadCursor(0, IDC_SIZENWSE);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_RESIZE_WEST_EAST: {
            HCURSOR cursor = LoadCursor(0, IDC_SIZEWE);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_UP_ARROW: {
            HCURSOR cursor = LoadCursor(0, IDC_UPARROW);
            SetCursor(cursor);
        } break;
        case OS_CURSOR_HOURGLASS: {
            HCURSOR cursor = LoadCursor(0, IDC_WAIT);
            SetCursor(cursor);
        } break;
    }
}

OSLibrary assetsLib;
Mem_alloc_func Asset;
Mem_alloc_func MEM_ALLOC_PTR;
Mem_calloc_func MEM_CALLOC_PTR;
Mem_free_func MEM_FREE_PTR;
Mem_resize_func MEM_RESIZE_PTR;
Mem_log_func MEM_LOG_PTR;
void (*Assets_initFunc)();
void *(*Assets_getFunc)(const char *assetName);
void (*Assets_putFunc)(const char *assetName, void *data);
U32 (*Assets_hasFunc)(const char *assetName);

void *Assets_get(const char *assetName) {
    printf("Checking for asset: %s\n", assetName);
    return Assets_getFunc(assetName);
}

void Assets_put(const char *assetName, void *data) {
    printf("Adding asset: %s\n", assetName);
    Assets_putFunc(assetName, data);
}

U32 Assets_has(const char *assetName) {
    U32 has = Assets_hasFunc(assetName);
    printf("Checking for asset: %s (exists: %s)\n", assetName, has ? "true" : "false");
    return has;
}

/**
 * @brief Assets has allocation functions that are used by the engine. Thus we have to bootstrap it
 * 
 */
void OS_init() {
    assetsLib = malloc(sizeof(*assetsLib));
    assetsLib->module = LoadLibraryA("assets.dll");
    assetsLib->filename = "assets.dll";
    assert(assetsLib->module);
    Assets_initFunc = OS_LibraryFunction(assetsLib, "ASSET_SETUP");
    Assets_getFunc = OS_LibraryFunction(assetsLib, "ASSET_GET");
    Assets_putFunc = OS_LibraryFunction(assetsLib, "ASSET_PUT");
    Assets_hasFunc = OS_LibraryFunction(assetsLib, "ASSET_HAS");
    MEM_ALLOC_PTR = (Mem_alloc_func)OS_LibraryFunction(assetsLib, "Mem_alloc");
    MEM_CALLOC_PTR = (Mem_calloc_func)OS_LibraryFunction(assetsLib, "Mem_calloc");
    MEM_FREE_PTR = (Mem_free_func)OS_LibraryFunction(assetsLib, "Mem_free");
    MEM_RESIZE_PTR = (Mem_resize_func)OS_LibraryFunction(assetsLib, "Mem_resize");
    MEM_LOG_PTR = (Mem_log_func)OS_LibraryFunction(assetsLib, "logMem");

#if EVENTLOGGING
    OS_EVENT_LOG = fopen("event.log", "w");
    char *header = "Time,Phase,System,Category,Event";
    fwrite(header, 1, strlen(header), OS_EVENT_LOG);
    OS_EVENTLOGGING = 1;
    OS_EVENTLOGGING_KEYBOARD = 1;
    OS_EVENTLOGGING_MOUSE = 1;
    QueryPerformanceFrequency(&OS_EVENT_LOG_FREQUENCY);
    QueryPerformanceCounter(&OS_EVENT_LOG_START_TIME);
    OS_EventLog_startPhase("Beginning");
#endif

    Assets_initFunc();
}

#if EVENTLOGGING

void OS_EventLog_startPhase(const char *phase) {
    if (OS_EVENTLOGGING) {
        memcpy(OS_EVENT_LOG_PHASE, phase, strlen(phase)+1);
    }
}

void OS_logEvent(const char *system, const char *category, const char *event) {
    if (OS_EVENTLOGGING) {
        LARGE_INTEGER CurrentTime;
        QueryPerformanceCounter(&CurrentTime);
        CurrentTime.QuadPart = CurrentTime.QuadPart - OS_EVENT_LOG_START_TIME.QuadPart;
        CurrentTime.QuadPart *= 1000000;
        CurrentTime.QuadPart /= OS_EVENT_LOG_FREQUENCY.QuadPart;
        double elapsedTimeInSeconds = (double)CurrentTime.QuadPart / 1000000.0;
        fprintf(OS_EVENT_LOG, "\n%f,%s,%s,%s,\"%s\"", elapsedTimeInSeconds, OS_EVENT_LOG_PHASE, system, category, event);
        fflush(OS_EVENT_LOG); //Performance?
    }
}
#endif


// struct IO_PrintHandler {
//     char token[256];
//     U64 hash;
//     void (*func)(const void *type, char *output);
// } IO_PrintHandlers[256] = {0};

// void IO_PrintHandler_new(const char *token, void (*func)(const void *type, char *output)) {
//     for (int i = 0; i < 256; i++) {
//         if (!IO_PrintHandlers[i].func) {
//             memcpy(IO_PrintHandlers[i].token, token, strlen(token)+1);
//             IO_PrintHandlers[i].func = func;
//             return;
//         }
//     }
//     assert(0);
// }

// void IO_print(char *string, ...) {
//     HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
//     DWORD written = 0;
//     char buffer[4096];
//     char *firstPos = buffer, *readptr = string, *writeptr = buffer; // Plan: Use math figure out how many characters were written
//     va_list args;
//     va_start(args, string);
    
//     while (readptr && *readptr) {
//         if (*readptr != '{') {
//             *writeptr++ = *readptr++;
//             continue;
//         }
//         readptr++;
//         if (*readptr == '{') {
//             *writeptr++ = *readptr++;
//             continue;
//         }
//         U64 hash = 0;
//         while (*readptr && *readptr != '}') {
//             hash = RollingHashString(*readptr, hash);
//         }
//         for (int i = 0; i < 256; i++) {
//             if (IO_PrintHandlers[i].func && IO_PrintHandlers[i].hash == hash) {
//                 IO_PrintHandlers[i].func(va_arg(args, void*), writeptr);
//                 writeptr += strlen(writeptr);
//                 break;
//             }
//         }  
//     }

//     WriteFile(out, string, strlen(string), &written, 0);
// }

void OS_initFromAssets(
    Mem_alloc_func allocFunc,
    Mem_calloc_func callocFunc,
    Mem_free_func freeFunc,
    Mem_resize_func resizeFunc,
    Mem_log_func logFunc
) {
    MEM_ALLOC_PTR = allocFunc;
    MEM_CALLOC_PTR = callocFunc;
    MEM_FREE_PTR = freeFunc;
    MEM_RESIZE_PTR = resizeFunc;
    MEM_LOG_PTR = logFunc;
}

void OS_cleanup() {

}

#endif
