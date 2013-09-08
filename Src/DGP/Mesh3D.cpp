#include "StdAfx.h"
#include "Mesh3D.h"

namespace MagicDGP
{
    Vertex3D::Vertex3D() : 
        mpEdge(NULL)
    {
    }

    Vertex3D::Vertex3D(const Vector3& pos) :
        mPosition(pos), 
        mpEdge(NULL)
    {

    }

    Vertex3D::Vertex3D(const Vector3& pos, const Vector3& nor) : 
        mPosition(pos),
        mNormal(nor),
        mpEdge(NULL)
    {

    }

    Vertex3D::~Vertex3D()
    {
    }

    Vector3 Vertex3D::GetPosition() const
    {
        return mPosition;
    }

    void Vertex3D::SetPosition(const Vector3& pos)
    {
        mPosition = pos;
    }

    Vector3 Vertex3D::GetNormal() const
    {
        return mNormal;
    }

    void Vertex3D::SetNormal(const Vector3& nor)
    {
        mNormal = nor;
    }

    Vector3 Vertex3D::GetTexCord() const
    {
        return mTexCord;
    }

    void Vertex3D::SetTexCord(const Vector3& tex)
    {
        mTexCord = tex;
    }

    Edge3D* Vertex3D::GetEdge() const
    {
        return mpEdge;
    }

    void Vertex3D::SetEdge(Edge3D* pEdge)
    {
        mpEdge = pEdge;
    }

    Edge3D::Edge3D()
    {
    }

    Edge3D::~Edge3D()
    {
    }

    Vertex3D* Edge3D::GetVertex()
    {
        return mpVertex;
    }

    void Edge3D::SetVertex(Vertex3D* pVert)
    {
        mpVertex = pVert;
    }

    Edge3D* Edge3D::GetPair()
    {
        return mpPair;
    }

    void Edge3D::SetPair(Edge3D* pEdge)
    {
        mpPair = pEdge;
    }

    Edge3D* Edge3D::GetNext()
    {
        return mpNext;
    }

    void Edge3D::SetNext(Edge3D* pEdge)
    {
        mpNext = pEdge;
    }

    Edge3D* Edge3D::GetPre()
    {
        return mpPre;
    }

    void Edge3D::SetPre(Edge3D* pEdge)
    {
        mpPre = pEdge;
    }

    Face3D* Edge3D::GetFace()
    {
        return mpFace;
    }
    void Edge3D::SetFace(Face3D* pFace)
    {
        mpFace = pFace;
    }

    Face3D::Face3D()
    {
    }

    Face3D::~Face3D()
    {
    }

    Edge3D* Face3D::GetEdge()
    {
        return mpEdge;
    }

    void Face3D::SetEdge(Edge3D* pEdge)
    {
        mpEdge = pEdge;
    }

    Vector3 Face3D::GetNormal()
    {
        return mNormal;
    }

    void Face3D::SetNormal(const Vector3& nor)
    {
        mNormal = nor;
    }

    Real Face3D::GetArea()
    {
        return mArea;
    }

    Mesh3D::Mesh3D()
    {
    }

    Mesh3D::~Mesh3D()
    {
    }

    std::vector<Vertex3D* >& Mesh3D::GetVertexList()
    {
        return mVertexList;
    }

    std::vector<Edge3D* >& Mesh3D::GetEdgeList()
    {
        return mEdgeList;
    }

    std::vector<Face3D* >& Mesh3D::GetFaceList()
    {
        return mFaceList;
    }

    Vertex3D* Mesh3D::GetVertex(int index)
    {
        return mVertexList.at(index);
    }

    Edge3D* Mesh3D::GetEdge(int index)
    {
        return mEdgeList.at(index);
    }

    Face3D* Mesh3D::GetFace(int index)
    {
        return mFaceList.at(index);
    }

    int Mesh3D::GetVertexNumber() const
    {
        return mVertexList.size();
    }

    int Mesh3D::GetEdgeNumber() const
    {
        return mEdgeList.size();
    }

    int Mesh3D::GetFaceNumber() const
    {
        return mFaceList.size();
    }

    Vertex3D* Mesh3D::InsertVertex(const Vector3& pos)
    {
        Vertex3D* pVert = new Vertex3D(pos);
        mVertexList.push_back(pVert);
        return pVert;
    }

    Edge3D* Mesh3D::InsertEdge(Vertex3D* pVertStart, Vertex3D* pVertEnd)
    {
        if (mEdgeMap[std::pair<Vertex3D*, Vertex3D* >(pVertStart, pVertEnd)] != NULL)
        {
            return mEdgeMap[std::pair<Vertex3D*, Vertex3D* >(pVertStart, pVertEnd)];
        }
        else
        {
            Edge3D* pEdge = new Edge3D;
            pEdge->SetVertex(pVertEnd);
            pVertStart->SetEdge(pEdge);
            mEdgeMap[std::pair<Vertex3D*, Vertex3D* >(pVertStart, pVertEnd)] = pEdge;
            mEdgeList.push_back(pEdge);
            
            return pEdge;
        }
    }

    Face3D* Mesh3D::InsertFace(const std::vector<Vertex3D* >& vertList)
    {
        //Trianglar mesh
        if (vertList.size() != 3)
        {
            return NULL;
        }
        Face3D* pFace = new Face3D;
        std::vector<Edge3D* > innerEdgeList;
        for (int i = 0; i < 3; i++)
        {
            Edge3D* pEdge0 = InsertEdge(vertList.at(i), vertList.at((i + 1) % 3));
            pEdge0->SetFace(pFace);
            innerEdgeList.push_back(pEdge0);
            Edge3D* pEdge1 = InsertEdge(vertList.at((i + 1) % 3), vertList.at(i));
            pEdge0->SetPair(pEdge1);
            pEdge1->SetPair(pEdge0);
        }

        for (int i = 0; i < 3; i++)
        {
            innerEdgeList.at(i)->SetNext(innerEdgeList.at((i + 1) % 3));
            innerEdgeList.at((i + 1) % 3)->SetPre(innerEdgeList.at(i));
        }
        mFaceList.push_back(pFace);

        return pFace;
    }

    void Mesh3D::ClearData()
    {
    }
}