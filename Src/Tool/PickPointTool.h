#pragma once
#include "OIS.h"
#include "../DGP/Mesh3D.h"
#include "../DGP/PointCloud3D.h"
#include "../DGP/Vector2.h"

namespace MagicTool
{
    enum PickMode
    {
        PM_Point = 0,
        PM_Rectangle,
        PM_Cycle
    };

    class PickPointTool
    {
    public:
        PickPointTool();
        ~PickPointTool();

        static int PickMeshVertexByPoint(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 mousePos, bool ignoreBack);
        static void PickMeshVertexByRectangle(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 pos0, 
            MagicDGP::Vector2 pos1, std::vector<int>& pickIndex, bool ignoreBack);
        static void PickMeshVertexByCycle(const MagicDGP::Mesh3D* pMesh, MagicDGP::Vector2 centerPos, 
            MagicDGP::Real radius, std::vector<int>& pickIndex, bool ignoreBack);

        static int PickMeshVertexByPoint(const MagicDGP::LightMesh3D* pMesh, MagicDGP::Vector2 mousePos, bool ignoreBack);
        static void PickMeshVertexByRectangle(const MagicDGP::LightMesh3D* pMesh, MagicDGP::Vector2 pos0, 
            MagicDGP::Vector2 pos1, std::vector<int>& pickIndex, bool ignoreBack);
        static void PickMeshVertexByCycle(const MagicDGP::LightMesh3D* pMesh, MagicDGP::Vector2 centerPos, 
            MagicDGP::Real radius, std::vector<int>& pickIndex, bool ignoreBack);

        static int PickPointSetByPoint(const MagicDGP::Point3DSet* pPS, MagicDGP::Vector2 mousePos, bool ignoreBack);
        static void PickPointSetByRectangle(const MagicDGP::Point3DSet* pPS, MagicDGP::Vector2 pos0, 
            MagicDGP::Vector2 pos1, std::vector<int>& pickIndex, bool ignoreBack);
        static void PickPointSetByCycle(const MagicDGP::Point3DSet* pPS, MagicDGP::Vector2 centerPos, 
            MagicDGP::Real radius, std::vector<int>& pickIndex, bool ignoreBack);

        void SetPickParameter(PickMode pm, bool ignoreBack, MagicDGP::LightMesh3D* pLightMesh, MagicDGP::Mesh3D* pMesh, MagicDGP::Point3DSet* pPS);
        void MousePressed(const OIS::MouseEvent& arg);
        void MouseMoved(const OIS::MouseEvent& arg);
        void MouseReleased(const OIS::MouseEvent& arg);
        void GetPickMeshIndex(std::vector<int>& pickIndex);
        void GetPickLightMeshIndex(std::vector<int>& pickIndex);
        void GetPickPointsetIndex(std::vector<int>& pickIndex);

    private:
        void UpdateMarkObject(MagicDGP::Vector2& pos0, MagicDGP::Vector2& pos1);
        void ClearMarkObject();
        void Pick(MagicDGP::Vector2& curPos);

    private:
        PickMode mPickMode;
        bool mIgnoreBack;
        MagicDGP::Vector2 mMousePos;
        MagicDGP::Mesh3D* mpMesh;
        MagicDGP::LightMesh3D* mpLightMesh;
        MagicDGP::Point3DSet* mpPointSet;
        std::vector<int> mPickMeshIndex;
        std::vector<int> mPickLightMeshIndex;
        std::vector<int> mPickPointsetIndex;
        bool mPickPressed;
    };
}