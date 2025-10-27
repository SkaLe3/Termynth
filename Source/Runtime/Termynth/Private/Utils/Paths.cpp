#include "Utils/Paths.h"
#include "Platform/PlatformUtils.h"

fs::path Paths::EngineDir()
{
    return PlatformUtils::Get().EngineDir();
}

fs::path Paths::ProjectDir()
{
    return PlatformUtils::Get().ProjectDir();
}

fs::path Paths::EngineContentDir()
{
    return Paths::EngineDir() / "Content";
}

fs::path Paths::ProjectContentDir()
{
    return Paths::ProjectDir() / "Content";
}