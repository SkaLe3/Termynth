#pragma once
#include "Core/Core.h"
#include "Input/Key.h"

class PlatformUtilsGeneric
{
public:
    PlatformUtilsGeneric();
    virtual ~PlatformUtilsGeneric();
public:
    virtual void ClearScreen();
    void ResetTerminal();

    std::string MakeUtf8(const uint8* value);
    std::string Utf8FromChar16(char16_t c);

    virtual void EnableVirtualTerminalProcessing() {}

    /** Cursor */
    void MoveCursor(int32 x, int32 y);
    virtual void MoveCursorTopLeft();
    virtual void HideCursor();
    virtual void ShowCursor();

    /** Input */
    virtual void InitInput() {}
    virtual void RestoreInput() {}
    virtual int kbhitNonBlocking() { return 0; }
    virtual char getchNonBlocking() { return ' '; }
    virtual void ReadExtendedKey(uint8& c, int32& ext) { (void)c; (void)ext;}
    virtual bool GetAsyncKeyState(uint8 c) { (void)c; return false;}
    virtual int32 GetVirtualKeyCode(Key key) {(void)key; return 0;}

    /** Paths */
    fs::path EngineDir();
    fs::path ProjectDir();

    static PlatformUtilsGeneric& Get();
    static void Init();
protected:
    static PlatformUtilsGeneric* s_Instance;
};