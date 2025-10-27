#pragma once

#include "Platform/Generic/PlatformUtilsGeneric.h"
#include <string>

class PlatformUtilsLinux : public PlatformUtilsGeneric
{
public:
    virtual void InitInput() override;
    virtual void RestoreInput() override;
    virtual int32 kbhitNonBlocking() override;
    virtual char getchNonBlocking() override;
    virtual void ReadExtendedKey(uint8& c, int32& ext) override;
    virtual bool GetAsyncKeyState(uint8 c) override;
    virtual int32 GetVirtualKeyCode(Key key) override;
private:
    std::string FindKeyboardDevicePath();
public:
    static void Init();
};