#include "includes.h"

#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#endif

void prepare_logdir()
{
    spdlog::drop_all();
#ifdef _WIN32
    system("if not exist logs mkdir logs");
    system("del /F /Q logs\\*");
#else
    auto rv = system("mkdir -p logs");
    if (rv != 0)
    {
        throw std::runtime_error("Failed to mkdir -p logs");
    }
    rv = system("rm -f logs/*");
    if (rv != 0)
    {
        throw std::runtime_error("Failed to rm -f logs/*");
    }
#endif
}

std::string file_contents(const std::string &filename)
{
    std::ifstream ifs(filename);
    if (!ifs)
    {
        throw std::runtime_error("Failed open file ");
    }
    return std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
}

std::size_t count_lines(const std::string &filename)
{
    std::ifstream ifs(filename);
    if (!ifs)
    {
        throw std::runtime_error("Failed open file ");
    }

    std::string line;
    size_t counter = 0;
    while (std::getline(ifs, line))
        counter++;
    return counter;
}

std::size_t get_filesize(const std::string &filename)
{
    std::ifstream ifs(filename, std::ifstream::ate | std::ifstream::binary);
    if (!ifs)
    {
        throw std::runtime_error("Failed open file ");
    }

    return static_cast<std::size_t>(ifs.tellg());
}

// source: https://stackoverflow.com/a/2072890/192001
bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size())
    {
        return false;
    }
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

#ifdef _WIN32
// Based on: https://stackoverflow.com/a/37416569/192001
std::size_t count_files(const std::string &folder)
{
    size_t counter = 0;
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Start iterating over the files in the path directory.
    hFind = ::FindFirstFileA((folder + "\\*").c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do // Managed to locate and create an handle to that folder.
        {
             if (ffd.cFileName[0] != '.')
                counter++;            
        } while (::FindNextFile(hFind, &ffd) == TRUE);
        ::FindClose(hFind);
    }
    else
    {
        throw std::runtime_error("Failed open folder " + folder);
    }

    return counter;
}
#else
// Based on: https://stackoverflow.com/a/2802255/192001
std::size_t count_files(const std::string &folder)
{
    size_t counter = 0;
    DIR *dp = opendir(folder.c_str());
    if (dp == nullptr)
    {
        throw std::runtime_error("Failed open folder " + folder);
    }

    struct dirent *ep;
    while (ep = readdir(dp))
    {
        if (ep->d_name[0] != '.')
            counter++;
    }
    (void)closedir(dp);
    return counter;
}
#endif
