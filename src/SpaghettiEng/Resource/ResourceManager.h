#pragma once

#include <filesystem>



/*
  {} []
*/


namespace Spg
{
  namespace fs = std::filesystem;

  // static class nor now - doesn't need to be registered with ServiceLocator
  class ResourceManager
  {
  public:
    static void Init();
  
  private:
    static void SetAssetsPath();

    static fs::path SearchTargetDirectoryParent(const std::string& target_directory);
    static fs::path SearchTargetDirectoryChildren(const std::string& target_directory);
    static fs::path GetExecutableDirectory();

    static fs::path s_assets_path;

  };
}