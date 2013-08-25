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

    private:
        Vector3 mPosition;
        Vector3 mNormal;
        Edge3D* mpEdge;

    };

    class Face3D;
    class Edge3D
    {
    public:
        Edge3D();
        ~Edge3D();

    private:
        Vertex3D* mpVertex;
        Edge3D*   mpPair;
        Edge3D*   mpNext;
        Edge3D*   mpPre;
        Face3D*   mpFace;
        Vector3   mTexCord;
        Vector3   mNormal;
    };


    class Face3D
    {
    public:
        Face3D();
        ~Face3D();

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