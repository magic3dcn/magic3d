#include "StdAfx.h"
#include "MediaStream.h"
#include "../DGP/Vector3.h"
#include "../Common/LogSystem.h"
#include <vector>

namespace MagicApp
{
    MediaStream::MediaStream() : 
        mScanMode(CLOSE),
        mIsRecording(false)
    {
    }

    MediaStream::~MediaStream()
    {
    }

    void MediaStream::Init()
    {
        openni::Status rc = openni::OpenNI::initialize();
        if (rc != openni::STATUS_OK)
        {
            MagicLog << "OpenNI initialize failed: " << openni::OpenNI::getExtendedError() << std::endl;
        }
        else
        {
            MagicLog << "OpenNI initialize succeed" << std::endl;
        }
        rc  = mRecorder.create("./RecorderData.oni");
        if (rc != openni::STATUS_OK)
        {
            MagicLog << "Recorder create failed: " << openni::OpenNI::getExtendedError() << std::endl;
        }
    }

    void MediaStream::StartScanner()
    {
        if ((mScanMode == CLOSE) || (mScanMode == RECORDER))
        {
            if (mScanMode == RECORDER)
            {
                mDevice.close();
                mColorStream.destroy();
                mDepthStream.destroy();
            }
            MagicLog << "MediaStream::StartScanner open device" << std::endl;
            openni::Status rc = mDevice.open(openni::ANY_DEVICE);
            if (rc != openni::STATUS_OK)
            {
                MagicLog << "Devive open failed: " << openni::OpenNI::getExtendedError() << std::endl;
            }
            rc = mDepthStream.create(mDevice, openni::SENSOR_DEPTH);
            if (rc != openni::STATUS_OK)
            {
                MagicLog << "DepthStream create failed: " << openni::OpenNI::getExtendedError() << std::endl;
            }
            rc = mColorStream.create(mDevice, openni::SENSOR_COLOR);
            if (rc != openni::STATUS_OK)
            {
                MagicLog << "ColorStream create failed: " << openni::OpenNI::getExtendedError() << std::endl;
            }
            mScanMode = SCANNER;
            //temp
            if (mRecorder.isValid())
            {
                rc = mRecorder.attach(mColorStream, true);
                if (rc != openni::STATUS_OK)
                {
                    MagicLog << "ColorStream attach failed: " << openni::OpenNI::getExtendedError() << std::endl;
                }
                rc = mRecorder.attach(mDepthStream);
                if (rc != openni::STATUS_OK)
                {
                    MagicLog << "DepthStream attach failed: " << openni::OpenNI::getExtendedError() << std::endl;
                }
                MagicLog << "Recorder attach stream" << std::endl;
            }
            //
        }
        openni::Status rc = mDepthStream.start();
        if (rc != openni::STATUS_OK)
        {
            MagicLog << "DepthStream start failed: " << openni::OpenNI::getExtendedError() << std::endl;
        }
        rc = mColorStream.start();
        if (rc != openni::STATUS_OK)
        {
            MagicLog << "ColorStream start failed: " << openni::OpenNI::getExtendedError() << std::endl;
        }
    }

    void MediaStream::StopScanner()
    {
        mDepthStream.stop();
        mColorStream.stop();
    }

    void MediaStream::StartRecorder()
    {
        mRecorder.start();
    }

    void MediaStream::StopRecorder()
    {
        mRecorder.stop();
        mRecorder.destroy();
    }

    void MediaStream::StartRecordScanner()
    {
        if ((mScanMode == CLOSE) || (mScanMode == SCANNER))
        {
            if (mScanMode == SCANNER)
            {
                mDevice.close();
                mColorStream.destroy();
                mDepthStream.destroy();
            }
            openni::Status rc = mDevice.open("./ScanData.oni");
            if (rc != openni::STATUS_OK)
            {
                MagicLog << "Devive open by file failed: " << openni::OpenNI::getExtendedError() << std::endl;
            }
            rc = mDepthStream.create(mDevice, openni::SENSOR_DEPTH);
            if (rc != openni::STATUS_OK)
            {
                MagicLog << "DepthStream create failed: " << openni::OpenNI::getExtendedError() << std::endl;
            }
            rc = mColorStream.create(mDevice, openni::SENSOR_COLOR);
            if (rc != openni::STATUS_OK)
            {
                MagicLog << "ColorStream create failed: " << openni::OpenNI::getExtendedError() << std::endl;
            }
            mScanMode = RECORDER;
        }
        mDepthStream.start();
      //  ExportDepthFrameForTest();
        mColorStream.start();
    }

    void MediaStream::StopRecordScanner()
    {
        mDepthStream.stop();
        mColorStream.stop();
        StopRecorder();
    }

    void MediaStream::WaitStream(openni::VideoFrameRef* pColorFrame, openni::VideoFrameRef* pDepthFrame)
    {
        openni::VideoStream** pStreams = new openni::VideoStream*[2];
        pStreams[0] = &mColorStream;
        pStreams[1] = &mDepthStream;
        int changeIndex;
        openni::OpenNI::waitForAnyStream(pStreams, 2, &changeIndex);
        if (changeIndex == 0)
        {
            mColorStream.readFrame(pColorFrame);
        }
        else if (changeIndex == 1)
        {
            mDepthStream.readFrame(pDepthFrame);
        }
        delete []pStreams;
    }

    openni::VideoStream& MediaStream::GetDepthStream()
    {
        return mDepthStream;
    }

    openni::VideoStream& MediaStream::GetColorStream()
    {
        return mColorStream;
    }

    void MediaStream::ExportDepthFrameForTest()
    {
        openni::PlaybackControl* pPC = mDevice.getPlaybackControl();
        int frameNum = pPC->getNumberOfFrames(mDepthStream);
       // pPC->seek(mDepthStream, 90);
        for (int fi = 10; fi < frameNum - 10; fi++)
        {
            MagicLog << "Export file(" << frameNum << "): " << fi << std::endl;
            pPC->seek(mDepthStream, fi);
            int changeIndex;
            openni::VideoStream** pStreams = new openni::VideoStream*[1];
            pStreams[0] = &mDepthStream;
            openni::OpenNI::waitForAnyStream(pStreams, 1, &changeIndex);
            if (changeIndex != 0)
            {
                MagicLog << "hangeIndex != 0" << std::endl;
            }
            delete []pStreams;
            openni::VideoFrameRef depthFrame;
            mDepthStream.readFrame(&depthFrame);
            if (depthFrame.isValid())
            {
                MagicLog << "Valid" << std::endl;
                const openni::DepthPixel* pDepth = (const openni::DepthPixel*)depthFrame.getData();
                int resolutionX = depthFrame.getVideoMode().getResolutionX();
                int resolutionY = depthFrame.getVideoMode().getResolutionY();
                std::vector<MagicDGP::Vector3> posList;
                for(int y = 0; y < resolutionY; y++)  
                {  
                    for(int x = 0; x < resolutionX; x++)  
                    {
                        openni::DepthPixel depth = pDepth[y * resolutionX + x]; 
                        float rx, ry, rz;
                        openni::CoordinateConverter::convertDepthToWorld(mDepthStream, 
                            x, y, depth, &rx, &ry, &rz);
                        MagicDGP::Vector3 pos(-rx, ry, rz);
                        posList.push_back(pos);
                    }
                }
                char fileName[20];
                sprintf(fileName, "Scene_%d.obj", fi);
                std::ofstream fout(fileName);
                for (int j = 1; j < resolutionY - 1; j++)
                {
                    for (int i = 1; i < resolutionX - 1; i++)
                    {
                        MagicDGP::Vector3 dirX = posList.at(j * resolutionX + i + 1) - posList.at(j * resolutionX + i - 1);
                        MagicDGP::Vector3 dirY = posList.at((j + 1) * resolutionX + i) - posList.at((j - 1) * resolutionX + i);
                        MagicDGP::Vector3 nor = dirY.CrossProduct(dirX);
                        MagicDGP::Real len = nor.Normalise();
                        if (len > MagicDGP::Epsilon)
                        {
                            MagicDGP::Vector3 pos = posList.at(j * resolutionX + i);
                            if (pos.Length() > MagicDGP::Epsilon)
                            {
                                fout << "vn " << nor[0] << " " << nor[1] << " " << nor[2] << std::endl;
                                fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
                            }
                        }
                    }
                }
                fout.close();
            }
        }
    }
}