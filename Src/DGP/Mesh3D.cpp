#include "StdAfx.h"
#include "Mesh3D.h"

namespace MagicDGP
{
    Vertex3D::Vertex3D()
    {
    }

    Vertex3D::~Vertex3D()
    {
    }

    Edge3D::Edge3D()
    {
    }

    Edge3D::~Edge3D()
    {
    }

    Face3D::Face3D()
    {
    }

    Face3D::~Face3D()
    {
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
        return NULL;
    }

    Edge3D* Mesh3D::InsertEdge(const Vertex3D* pVert1, const Vertex3D* pVert2)
    {
        return NULL;
    }

    Face3D* Mesh3D::InsertFace(const std::vector<Vertex3D* >& vertList)
    {
        return NULL;
    }

    void Mesh3D::ClearData()
    {
    }
}