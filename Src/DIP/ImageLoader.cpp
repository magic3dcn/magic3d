#include "ImageLoader.h"
#include "../Tool/ErrorCodes.h"
#include "../Tool/LogSystem.h"
#include "opencv2/opencv.hpp"

namespace MagicDIP
{
    ImageLoader::ImageLoader() :
        mImages(),
        mImageSize()
    {
    }

    ImageLoader::~ImageLoader()
    {
        Reset();
    }

    int ImageLoader::LoadImages(const std::vector<std::string>& imgFiles, ImageType it)
    {
        int imgCount = imgFiles.size();
        if (imgCount == 0)
        {
            return MAGIC_EMPTY_INPUT;
        }
        Reset();
        if (it == IT_Gray)
        {
            mImages.reserve(imgCount);
            mImageSize.reserve(imgCount * 2);
            for (int imgId = 0; imgId < imgCount; imgId++)
            {
                cv::Mat img = cv::imread(imgFiles.at(imgId));
                int imgH = img.rows;
                int imgW = img.cols;
                mImageSize.push_back(imgH);
                mImageSize.push_back(imgW);
                unsigned char* pImagePixels = new unsigned char[imgH * imgW];
                for (int hid = 0; hid < imgH; hid++)
                {
                    int baseIndex = hid * imgW;
                    for (int wid = 0; wid < imgW; wid++)
                    {
                        pImagePixels[baseIndex + wid] = img.ptr(hid, wid)[0];
                    }
                }
                img.release();
                mImages.push_back(pImagePixels);
            }
        }
        else if (it == IT_Color)
        {
            DebugLog << "Error: No implementation" << std::endl;
        }

        return MAGIC_NO_ERROR;
    }

    const unsigned char* ImageLoader::GetImage(int imgId) const
    {
        return mImages.at(imgId);
    }

    int ImageLoader::GetImageWidth(int imgId) const
    {
        return mImageSize.at(imgId * 2 + 1);
    }
     
    int ImageLoader::GetImageHeight(int imgId) const
    {
        return mImageSize.at(imgId * 2);
    }
     
    unsigned char ImageLoader::GetGrayImageValue(int imgId, int hid, int wid) const
    {
        return mImages.at(imgId)[hid * mImageSize.at(imgId * 2 + 1) + wid];
    }
            
    void ImageLoader::Reset(void)
    {
        for (std::vector<unsigned char* >::iterator itr = mImages.begin(); itr != mImages.end(); itr++)
        {
            if ((*itr) != NULL)
            {
                delete (*itr);
                (*itr) = NULL;
            }
        }
        mImages.clear();
        mImageSize.clear();
    }
}