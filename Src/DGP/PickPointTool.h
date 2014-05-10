#pragma once
#include "Mesh3D.h"
#include "PointCloud3D.h"
#include "../Math/Vector2.h"

namespace MagicDGP
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

        static int PickMeshVertexByPoint(const Mesh3D* pMesh, MagicMath::Vector2 mousePos, bool ignoreBack);
        static void PickMeshVertexByRectangle(const Mesh3D* pMesh, MagicMath::Vector2 pos0, 
            MagicMath::Vector2 pos1, std::vector<int>& pickIndex, bool ignoreBack);
        static void PickMeshVertexByCycle(const Mesh3D* pMesh, MagicMath::Vector2 centerPos, 
            double radius, std::vector<int>& pickIndex, bool ignoreBack);

        static int PickMeshVertexByPoint(const LightMesh3D* pMesh, MagicMath::Vector2 mousePos, bool ignoreBack);
        static void PickMeshVertexByRectangle(const LightMesh3D* pMesh, MagicMath::Vector2 pos0, 
            MagicMath::Vector2 pos1, std::vector<int>& pickIndex, bool ignoreBack);
        static void PickMeshVertexByCycle(const LightMesh3D* pMesh, MagicMath::Vector2 centerPos, 
            double radius, std::vector<int>& pickIndex, bool ignoreBack);

        static int PickPointSetByPoint(const Point3DSet* pPS, MagicMath::Vector2 mousePos, bool ignoreBack);
        static void PickPointSetByRectangle(const Point3DSet* pPS, MagicMath::Vector2 pos0, 
            MagicMath::Vector2 pos1, std::vector<int>& pickIndex, bool ignoreBack);
        static void PickPointSetByCycle(const Point3DSet* pPS, MagicMath::Vector2 centerPos, 
            double radius, std::vector<int>& pickIndex, bool ignoreBack);

        void SetPickParameter(PickMode pm, bool ignoreBack, LightMesh3D* pLightMesh, Mesh3D* pMesh, Point3DSet* pPS);
        void MousePressed(int mousePosX, int mousePosY);
        void MouseMoved(int mousePosX, int mousePosY);
        void MouseReleased(int mousePosX, int mousePosY);
        void GetPickMeshIndex(std::vector<int>& pickIndex);
        void GetPickLightMeshIndex(std::vector<int>& pickIndex);
        void GetPickPointsetIndex(std::vector<int>& pickIndex);

    private:
        void UpdateMarkObject(MagicMath::Vector2& pos0, MagicMath::Vector2& pos1);
        void ClearMarkObject();
        void Pick(MagicMath::Vector2& curPos);

    private:
        PickMode mPickMode;
        bool mIgnoreBack;
        MagicMath::Vector2 mMousePos;
        Mesh3D* mpMesh;
        LightMesh3D* mpLightMesh;
        Point3DSet* mpPointSet;
        std::vector<int> mPickMeshIndex;
        std::vector<int> mPickLightMeshIndex;
        std::vector<int> mPickPointsetIndex;
        bool mPickPressed;
    };
}