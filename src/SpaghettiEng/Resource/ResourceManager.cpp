#include "SpaghettiEng/Resource/ResourceManager.h"

#include "CoreLib/Core.h"

// #if defined(_WIN32)
// #include <windows.h>
// #include <vector>
// #elif defined(__linux__)
// #include <unistd.h>
// #include <limits.h>
// #elif defined(__APPLE__)
// #include <mach-o/dyld.h>
// #include <vector>
// #endif

#if defined(OS_WINDOWS)
#include <windows.h>
#include <vector>
#elif defined(OS_LINUX)
#include <unistd.h>
#include <limits.h>
#elif defined(OS_IOS)
#include <mach-o/dyld.h>
#include <vector>
#endif

#include <filesystem>
#include <vector>

/*
  {} []
*/

namespace Spg
{

  namespace fs = std::filesystem;

  fs::path ResourceManager::s_assets_path;

  void ResourceManager::Init()
  {
    auto exe_dir = GetExecutableDirectory();
    SPG_INFO("Path to executable: {}", exe_dir.string());
    // SetAssetsPath();
  }

  void ResourceManager::SetAssetsPath()
  {

  }

  fs::path ResourceManager::SearchTargetDirectoryChildren(const std::string& target_directory)
  {
    // Container to hold the paths of child directories
    std::vector<fs::path> child_directories;

    fs::path current_dir = GetExecutableDirectory();
    for(const auto& entry : fs::directory_iterator(current_dir))
    { 
      //if(fs::is_directory(entry.status()) && entry.) 
    }
  }

  fs::path ResourceManager::SearchTargetDirectoryParent(const std::string& target_directory)
  {
    fs::path current_dir = GetExecutableDirectory();

    // Loop until we reach the root path (root's parent path is itself)
    while (current_dir.has_parent_path() && current_dir != current_dir.parent_path()) {
        fs::path potential_path = current_dir / target_directory;
        
        // Check if the path exists AND is actually a directory
        if (fs::exists(potential_path) && fs::is_directory(potential_path)) {
            // Return the absolute, canonicalized path (resolves symlinks and dots)
            return fs::canonical(potential_path); 
        }
        
        // Move up one level
        current_dir = current_dir.parent_path();
    }
    
    // Return an empty path if it was never found
    SPG_ASSERT(false);
    return "";
  }


  fs::path ResourceManager::GetExecutableDirectory()
  {
#if defined(OS_WINDOWS)
    std::vector<wchar_t> buffer(MAX_PATH);
    DWORD size;
    while ((size = GetModuleFileNameW(NULL, buffer.data(), buffer.size())) == buffer.size() && 
           GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        buffer.resize(buffer.size() * 2);
    }
    buffer.resize(size);
    return std::filesystem::path(buffer.begin(), buffer.end()).parent_path();

#elif defined(OS_LINUX)
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path();
    }

#elif defined(OS_IOS)
    std::vector<char> buffer(PATH_MAX);
    uint32_t size = buffer.size();
    if (_NSGetExecutablePath(buffer.data(), &size) == -1) {
        buffer.resize(size);
        _NSGetExecutablePath(buffer.data(), &size);
    }
    return std::filesystem::canonical(buffer.data()).parent_path();
#endif

    SPG_ASSERT(false);
    return "";
  }

}