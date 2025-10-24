#pragma once
#include "Core/Core.h"
#include <string>

class Paths
{
public:
    static fs::path EngineDir();
    static fs::path ProjectDir();

    static fs::path EngineContentDir();
    static fs::path ProjectContentDir();
};