#pragma once

#include "Platform/Generic/PlatformUtilsGeneric.h"


class PlatformUtilsLinux : public PlatformUtilsGeneric
{
public:
    virtual void InitInput() override;
    virtual void RestoreInput() override;
    virtual int32 kbhitNonBlocking() override;
    virtual char getchNonBlocking() override;
    virtual void ReadExtendedKey(uint8& c, int32& ext) override;

    static void Init();
};