#include "FaceBeautificationApp.h"
#include "../Common/LogSystem.h"
#include "../Common/ToolKit.h"

namespace MagicApp
{
    FaceBeautificationApp::FaceBeautificationApp()
    {
    }

    FaceBeautificationApp::~FaceBeautificationApp()
    {
    }

    bool FaceBeautificationApp::Enter(void)
    {
        InfoLog << "Enter FaceBeautificationApp" << std::endl;
        mUI.Setup();
        SetupScene();

        return true;
    }

    bool FaceBeautificationApp::Update(float timeElapsed)
    {
        return true;
    }

    bool FaceBeautificationApp::Exit(void)
    {
        ShutdownScene();
        mUI.Shutdown();

        return true;
    }

    bool FaceBeautificationApp::MouseMoved( const OIS::MouseEvent &arg )
    {
        return true;
    }

    bool FaceBeautificationApp::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool FaceBeautificationApp::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
    {
        return true;
    }

    bool FaceBeautificationApp::KeyPressed( const OIS::KeyEvent &arg )
    {
        return true;
    }

    void FaceBeautificationApp::WindowResized( Ogre::RenderWindow* rw )
    {
        
    }

    void FaceBeautificationApp::SetupScene(void)
    {

    }

    void FaceBeautificationApp::ShutdownScene(void)
    {

    }

    bool FaceBeautificationApp::OpenImage(void)
    {
        std::string fileName;
        char filterName[] = "Image Files(*.*)\0*.*\0";
        if (MagicCore::ToolKit::FileOpenDlg(fileName, filterName))
        {
            mImage.release();
            mImage = cv::imread(fileName);
            if (mImage.data != NULL)
            {
                //mImage = ResizeToViewSuit(mImage);
                //SetDefaultParameter();
                ////mIsNewImage = true;
                //UpdateAuxiliaryData();
                //Display();
                //w = mImage.cols;
                //h = mImage.rows;
                
                return true;
            }
        }
        return false;
    }
}