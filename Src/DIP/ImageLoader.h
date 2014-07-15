#pragma once
#include <vector>
#include <string>

namespace MagicDIP
{
    class ImageLoader
    {
    public:
        enum ImageType
        {
            IT_Color,
            IT_Gray
        };

        ImageLoader();
        ~ImageLoader();

        int LoadImages(const std::vector<std::string>& imgFiles, ImageType it);
        const unsigned char* GetImage(int imgId) const;
        int GetImageWidth(int imgId) const;
        int GetImageHeight(int imgId) const;
        unsigned char GetGrayImageValue(int imgId, int hid, int wid) const;
        void Reset(void);

    private:
        std::vector<unsigned char* > mImages;
        std::vector<int> mImageSize;
    };
}
