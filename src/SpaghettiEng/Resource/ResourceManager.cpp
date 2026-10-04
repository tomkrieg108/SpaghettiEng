#include "SpaghettiEng/Resource/ResourceManager.h"

#include "CoreLib/Core.h"

#if 1

#if defined(_WIN32)
#include <windows.h>
#include <vector>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <vector>
#endif

#endif

#if 0

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

#endif

#include <filesystem>
#include <vector>

/*
  {} []
*/

namespace Spg
{

  namespace fs = std::filesystem;

  //todo - might want to Initialize and store all resources here - shaders, models, textures etc.  Currently, for shaders, this is done in shader.cpp
  
  ResourceManager::ResourceManager()
  {
    Init();
  }

  void ResourceManager::Init()
  {
    SetAssetsPath();
  }

  void ResourceManager::SetAssetsPath()
  {
    auto exe_dir = GetExecutableDirectory();
    //SPG_INFO("Path to executable: {}", exe_dir.string());
    m_assets_path = SearchDown(exe_dir,"Assets");
    
    if(m_assets_path.string() == "")
      m_assets_path = SearchUp(exe_dir, "Assets");
      
    SPG_INFO("Path to Assets folder: {}", m_assets_path.string());
    SPG_ASSERT(m_assets_path.string() != "");
  }

  fs::path ResourceManager::SearchDown(fs::path current_dir, const std::string& target_dir)
  {
    // if(!fs::is_directory(current_dir))
    //   return fs::path("");

    //Todo - needs fixing and testing - check again!
   
    for(const auto& entry : fs::directory_iterator(current_dir))
    { 
      auto entry_name = entry.path().filename().string();
      
      if(entry.is_directory())
      {
        if(entry_name == target_dir)
          return entry.path();
        else
          return SearchDown(entry.path(), target_dir);  
      }
    }
    return fs::path("");
  }

  fs::path ResourceManager::SearchUp(fs::path current_dir, const std::string& target_directory)
  {
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
    return fs::path("");
  }


  fs::path ResourceManager::GetExecutableDirectory()
  {
#if defined(_WIN32)
    std::vector<wchar_t> buffer(MAX_PATH);
    DWORD size;
    while ((size = GetModuleFileNameW(NULL, buffer.data(), buffer.size())) == buffer.size() && 
           GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        buffer.resize(buffer.size() * 2);
    }
    buffer.resize(size);
    return std::filesystem::path(buffer.begin(), buffer.end()).parent_path();

#elif defined(__linux__)
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path();
    }

#elif defined(__APPLE__)
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