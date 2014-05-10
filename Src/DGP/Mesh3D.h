#pragma once
#include "../Math/Vector3.h"
#include <vector>
#include <map>

namespace MagicDGP
{
    enum BoundaryType
    {
        BT_Inner = 0,
        BT_Boundary
    };

    class Edge3D;
    class Vertex3D
    {
    public:
        Vertex3D();
        Vertex3D(const MagicMath::Vector3& pos);
        Vertex3D(const MagicMath::Vector3& pos, const MagicMath::Vector3& nor);
        ~Vertex3D();

        MagicMath::Vector3 GetPosition() const;
        void    SetPosition(const MagicMath::Vector3& pos);
        MagicMath::Vector3 GetNormal() const;
        void    SetNormal(const MagicMath::Vector3& nor);
        MagicMath::Vector3 GetTexCord() const;
        void    SetTexCord(const MagicMath::Vector3& tex);
        MagicMath::Vector3 GetColor() const;
        void    SetColor(const MagicMath::Vector3& color);
        Edge3D* GetEdge();
        const Edge3D* GetEdge() const;
        void    SetEdge(Edge3D* pEdge);
        int     GetId() const;
        void    SetId(int id);
        void    SetValid(bool valid);
        bool    IsValid() const;
        void    SetBoundaryType(BoundaryType bt);
        BoundaryType GetBoundaryType() const;

    private:
        MagicMath::Vector3 mPosition;
        MagicMath::Vector3 mNormal;
        MagicMath::Vector3 mTexCord;
        MagicMath::Vector3 mColor;
        Edge3D* mpEdge;
        int     mId;
        bool    mValid;
        BoundaryType mBoundaryType;
    };

    class Face3D;
    class Edge3D
    {
    public:
        Edge3D();
        ~Edge3D();

        Vertex3D* GetVertex();
        const Vertex3D* GetVertex() const;
        void      SetVertex(Vertex3D* pVert);
        Edge3D*   GetPair();
        const Edge3D* GetPair() const;
        void      SetPair(Edge3D* pEdge);
        Edge3D*   GetNext();
        const Edge3D* GetNext() const;
        void      SetNext(Edge3D* pEdge);
        Edge3D*   GetPre();
        const Edge3D* GetPre() const;
        void      SetPre(Edge3D* pEdge);
        Face3D*   GetFace();
        const Face3D* GetFace() const;
        void      SetFace(Face3D* pFace);
        int       GetId() const;
        void      SetId(int id);
        void      SetValid(bool valid);
        bool      IsValid() const;
        void      SetBoundaryType(BoundaryType bt);
        BoundaryType GetBoundaryType() const;

    private:
        Vertex3D* mpVertex;
        Edge3D*   mpPair;
        Edge3D*   mpNext;
        Edge3D*   mpPre;
        Face3D*   mpFace;
        int       mId;
        bool      mValid;
        BoundaryType mBoundaryType;
    };


    class Face3D
    {
    public:
        Face3D();
        ~Face3D();

        Edge3D* GetEdge();
        const Edge3D* GetEdge() const;
        void    SetEdge(Edge3D* pEdge);
        MagicMath::Vector3 GetNormal() const;
        void    SetNormal(const MagicMath::Vector3& nor);
        void    CalArea();
        double    GetArea() const;
        int     GetId() const;
        void    SetId(int id);
        void    SetValid(bool valid);
        bool    IsValid() const;

    private:
        Edge3D* mpEdge;
        MagicMath::Vector3 mNormal;
        double    mArea;
        int     mId;
        bool    mValid;
    };

    class Mesh3D
    {
    public:
        Mesh3D();
        ~Mesh3D();

    public:
        std::vector<Vertex3D* >& GetVertexList();
        std::vector<Edge3D* >&   GetEdgeList();
        std::vector<Face3D* >&   GetFaceList();
        Vertex3D* GetVertex(int index);
        const Vertex3D* GetVertex(int index) const;
        Edge3D*   GetEdge(int index);
        const Edge3D* GetEdge(int index) const;
        Face3D*   GetFace(int index);
        const Face3D* GetFace(int index) const;
        int GetVertexNumber() const;
        int GetEdgeNumber() const;
        int GetFaceNumber() const;

        Vertex3D* InsertVertex(const MagicMath::Vector3& pos);
        Edge3D*   InsertEdge(Vertex3D* pVertStart, Vertex3D* pVertEnd);
        Face3D*   InsertFace(const std::vector<Vertex3D* >& vertList);

        void UnifyPosition(double size);
        void UpdateNormal();
        void UpdateBoundaryFlag();
        void GetBBox(MagicMath::Vector3& bboxMin, MagicMath::Vector3& bboxMax) const;
        void CalculateBBox();
        void CalculateFaceArea();
        void ClearData();

    private:
        std::vector<Vertex3D* > mVertexList;
        std::vector<Edge3D* >   mEdgeList;
        std::vector<Face3D* >   mFaceList;
        std::map<std::pair<Vertex3D*, Vertex3D*>, Edge3D* > mEdgeMap; //Only used in construct mesh
        MagicMath::Vector3 mBBoxMin, mBBoxMax;
    };

    struct FaceIndex
    {
        int mIndex[3];
    };

    class LightMesh3D
    {
    public:
        LightMesh3D();
        ~LightMesh3D();

        Vertex3D* GetVertex(int index);
        const Vertex3D* GetVertex(int index) const;
        const FaceIndex GetFace(int index) const;
        int GetVertexNumber() const;
        int GetFaceNumber() const;

        Vertex3D* InsertVertex(const MagicMath::Vector3& pos);
        void InsertFace(const FaceIndex& fi);

        void UnifyPosition(double size);
        void UpdateNormal();
        void ClearData();

    private:
        std::vector<Vertex3D*> mVertexList;
        std::vector<FaceIndex> mFaceList;
    };
}