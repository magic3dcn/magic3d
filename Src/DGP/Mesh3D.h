#pragma once
#include "Vector3.h"
#include <vector>

namespace MagicDGP
{
    class Edge3D;
    class Vertex3D
    {
    public:
        Vertex3D();
        ~Vertex3D();

        Vector3 GetPosition();
        void    SetPosition(const Vector3& pos);
        Vector3 GetNormal();
        void    SetNormal(const Vector3& nor);
        Vector3 GetTexCord();
        void    SetTexCord(const Vector3& tex);
        Edge3D* GetEdge();
        void    SetEdge(Edge3D* pEdge);

    private:
        Vector3 mPosition;
        Vector3 mNormal;
        Vector3 mTexCord;
        Edge3D* mpEdge;

    };

    class Face3D;
    class Edge3D
    {
    public:
        Edge3D();
        ~Edge3D();

        Vertex3D* GetVertex();
        void      SetVertex(Vertex3D* pVert);
        Edge3D*   GetPair();
        void      SetPair(Edge3D* pEdge);
        Edge3D*   GetNext();
        void      SetNext(Edge3D* pEdge);
        Edge3D*   GetPre();
        void      SetPre(Edge3D* pEdge);
        Face3D*   GetFace();
        void      SetFace(Face3D* pFace);

    private:
        Vertex3D* mpVertex;
        Edge3D*   mpPair;
        Edge3D*   mpNext;
        Edge3D*   mpPre;
        Face3D*   mpFace;
    };


    class Face3D
    {
    public:
        Face3D();
        ~Face3D();

        Edge3D* GetEdge();
        void    SetEdge(Edge3D* pEdge);
        Vector3 GetNormal();
        void    SetNormal(const Vector3& nor);
        Real    GetArea();

    private:
        Edge3D* mpEdge;
        Vector3 mNormal;
        Real    mArea;
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
        Edge3D*   GetEdge(int index);
        Face3D*   GetFace(int index);
        int GetVertexNumber() const;
        int GetEdgeNumber() const;
        int GetFaceNumber() const;
        Vertex3D* InsertVertex(const Vector3& pos);
        Edge3D*   InsertEdge(const Vertex3D* pVert1, const Vertex3D* pVert2);
        Face3D*   InsertFace(const std::vector<Vertex3D* >& vertList);
        void ClearData();

    private:
        std::vector<Vertex3D* > mVertexList;
        std::vector<Edge3D* >   mEdgeList;
        std::vector<Face3D* >   mFaceList;
    };
}