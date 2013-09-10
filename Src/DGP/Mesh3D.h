#pragma once
#include "Vector3.h"
#include <vector>
#include <map>

namespace MagicDGP
{
    class Edge3D;
    class Vertex3D
    {
    public:
        Vertex3D();
        Vertex3D(const Vector3& pos);
        Vertex3D(const Vector3& pos, const Vector3& nor);
        ~Vertex3D();

        Vector3 GetPosition() const;
        void    SetPosition(const Vector3& pos);
        Vector3 GetNormal() const;
        void    SetNormal(const Vector3& nor);
        Vector3 GetTexCord() const;
        void    SetTexCord(const Vector3& tex);
        Vector3 GetColor() const;
        void    SetColor(const Vector3& color);
        Edge3D* GetEdge();
        const Edge3D* GetEdge() const;
        void    SetEdge(Edge3D* pEdge);
        int     GetId() const;
        void    SetId(int id);

    private:
        Vector3 mPosition;
        Vector3 mNormal;
        Vector3 mTexCord;
        Vector3 mColor;
        Edge3D* mpEdge;
        int     mId;
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

    private:
        Vertex3D* mpVertex;
        Edge3D*   mpPair;
        Edge3D*   mpNext;
        Edge3D*   mpPre;
        Face3D*   mpFace;
        int       mId;
    };


    class Face3D
    {
    public:
        Face3D();
        ~Face3D();

        Edge3D* GetEdge();
        const Edge3D* GetEdge() const;
        void    SetEdge(Edge3D* pEdge);
        Vector3 GetNormal() const;
        void    SetNormal(const Vector3& nor);
        Real    GetArea() const;
        int     GetId() const;
        void    SetId(int id);

    private:
        Edge3D* mpEdge;
        Vector3 mNormal;
        Real    mArea;
        int     mId;
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
        Vertex3D* InsertVertex(const Vector3& pos);
        Edge3D*   InsertEdge(Vertex3D* pVertStart, Vertex3D* pVertEnd);
        Face3D*   InsertFace(const std::vector<Vertex3D* >& vertList);
        void ClearData();

    private:
        std::vector<Vertex3D* > mVertexList;
        std::vector<Edge3D* >   mEdgeList;
        std::vector<Face3D* >   mFaceList;
        std::map<std::pair<Vertex3D*, Vertex3D*>, Edge3D* > mEdgeMap;
    };
}