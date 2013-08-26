#include "StdAfx.h"
#include "MediaStream.h"
#include "../Common/LogSystem.h"

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
}