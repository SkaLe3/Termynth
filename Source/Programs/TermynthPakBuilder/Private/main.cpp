#include "PakBuilder.h"
#include "Core/Logger.h"

#include <iostream>

void PrintUsage()
{
    LOG_INFO("PAK Builder Tool");
    LOG_INFO("Usage:");
    LOG_INFO("  TermynthPakBuilder <base_path>:<content_dir> <output.pak>");
    //LOG_INFO("  TermynthPakBuilder -d <directory_list.txt> <output.pak>");
    LOG_INFO("  TermynthPakBuilder -f <file_list.txt> <output.pak>\n");
    LOG_INFO("Examples:");
    LOG_INFO("  TermynthPakBuilder Content Assets.pak");
    //LOG_INFO("  TermynthPakBuilder -d directories.txt Game.pak");
    LOG_INFO("  TermynthPakBuilder -f files.txt Game.pak\n");
    LOG_INFO("File list format (one per line):");
    LOG_INFO("  aset_path|disk_path");
    LOG_INFO("  Game/Textures/player.thtx|Content/Textures/player.thtx\n");
    LOG_INFO("Directory list format (one per line):");
    LOG_INFO("  base_path|disk_path");
    LOG_INFO("  Game|Content");
    LOG_INFO("  Engine|Engine/Termynth/Content");
}


int main(int argc, char* argv[])
{
    LOG_INIT_NOSTAMP(ELogLevel::Debug);

    if (argc < 3) 
    {
        PrintUsage();
        return 1;
    }

    PakBuilder builder;

    std::string outputPak = argv[argc - 1];

    if (std::string(argv[1]) == "-f")
    {
        // Load from file list
        if (argc < 4)
        {
            PrintUsage();
            return 1;
        }

        std::string fileListPath = argv[2];
        std::ifstream fileList(fileListPath);

        if (!fileList.is_open())
        {
            LOG_ERROR("Failed to open file list: " + fileListPath);
            return 1;
        }

        std::string line;
        while (std::getline(fileList, line))
        {
            if (line.empty() || line[0] == '#') continue;

            size_t sep = line.find('|');
            if (sep == std::string::npos)
            {
                LOG_ERROR("Invalid line in file list: " + line);
                continue;
            }

            std::string assetPath = line.substr(0, sep);
            std::string diskPath = line.substr(sep + 1);

            builder.AddFile(assetPath, diskPath);
        }
    }
    else if (std::string(argv[1]) == "-d")
    {

    }
    else
    {
        for (int32_t i = 1; i < argc - 1; i++)
        {
            // Add entire directory
            std::string contentDir = argv[i];

            if (contentDir.empty()) continue;
            size_t sep = contentDir.find(':');
            if (sep == std::string::npos)
            {
                LOG_ERROR("Invalid argument format: " + contentDir);
                continue;
            }

            std::string basePath = contentDir.substr(0, sep);
            std::string diskPath = contentDir.substr(sep + 1);

            if (!builder.AddDirectory(diskPath, basePath))
            {
                LOG_ERROR("Failed to add directory: " + diskPath);
                return 1;
            }
        }
    }
    if (!builder.Build(outputPak))
    {
        LOG_ERROR("Failed to build PAK file");
        return 1;
    }
    LOG_SHUTDOWN();
    return 0;
}