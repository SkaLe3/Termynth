#pragma once

#include "Platform/Generic/PlatformUtilsGeneric.h"
#include "Input/Key.h"

class PlatformUtilsWindows : public PlatformUtilsGeneric
{
public:
    virtual void ClearScreen() override; 

    virtual int kbhitNonBlocking() override;
    virtual char getchNonBlocking() override;
    virtual void ReadExtendedKey(uint8& c, int32& ext) override;
    virtual bool GetAsyncKeyState(uint8 c) override;
    virtual int32 GetVirtualKeyCode(Key key) override;

    virtual void MoveCursorTopLeft() override;
    virtual void HideCursor() override;
    virtual void ShowCursor() override;

    static void Init();
};