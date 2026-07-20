#include "SpaghettiEng/Assets/AssetManager.h"

namespace Spg
{
	 #if 0

    //Todo - this should be setup in Assets module
  void Application::SetAssetsPath()
  {
    //Todo - this will need to be changed.  Currently depends on the location of this source file, which won't work when app is 'deployed'
    fs::path this_file = fs::absolute(fs::path{__FILE__});
    fs::path assets_path = this_file.parent_path() / fs::path{"../../../Assets"};
    assets_path = fs::absolute(assets_path);
    if (!fs::exists(assets_path)) {
        SPG_ERROR("Assets path does not exist: {}", assets_path.string());
        return;
    }
    if (!fs::is_directory(assets_path)) {
        SPG_ERROR("Assets path is not a directory: {}", assets_path.string());
        return;
    }
    try {
        fs::current_path(assets_path);
    } catch (const fs::filesystem_error& e) {
        SPG_ERROR("Exception setting CWD to assets path. Msg: {} Error: {}", assets_path.string(), e.what());
        return;
    }
    SPG_INFO("Current working directory successfully set to: {}", fs::current_path().string());
  }

   #endif
}