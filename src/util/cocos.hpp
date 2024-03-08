#pragma once
#include <cocos2d.h>

namespace util::cocos {
    // Loads the given images in separate threads, in parallel. Blocks the thread until all images have been loaded.
    // This will ONLY load .png images.
    void loadAssetsParallel(const std::vector<std::string>& images);

    // Caveat: does not include -hd.png or -uhd.png
    std::string fullPathForFilename(const std::string_view filename, size_t preferIdx = -1);
    size_t findSearchPathIdxForFile(const std::string_view filename);

    // returns ".png", "-hd.png" or "-uhd.png"
    const char* getTextureNameSuffix();

    // returns ".plist", "-hd.plist" or "-uhd.plist"
    const char* getTextureNameSuffixPlist();

    // transform a path, adding -hd or -uhd at the end depending on current tex quality
    std::string transformPathWithQuality(const std::string_view path, const std::string_view suffix);

    // transform a path, adding -hd or -uhd at the end depending on current tex quality
    std::string transformPathWithQualityPlist(const std::string_view path, const std::string_view suffix);
}
