//#include "StdAfx.h"
#include "Mesh3D.h"

namespace MagicDGP
{
    Vertex3D::Vertex3D() : 
        mColor(0.9, 0.9, 0.9),
        mpEdge(NULL),
        mId(-1)
    {
    }

    Vertex3D::Vertex3D(const Vector3& pos) :
        mPosition(pos), 
        mColor(0.9, 0.9, 0.9),
        mpEdge(NULL),
        mId(-1)
    {

    }

    Vertex3D::Vertex3D(const Vector3& pos, const Vector3& nor) : 
        mPosition(pos),
        mNormal(nor),
        mColor(0.9, 0.9, 0.9),
        mpEdge(NULL),
        mId(-1)
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

    Vector3 Vertex3D::GetColor() const
    {
        return mColor;
    }

    void Vertex3D::SetColor(const Vector3& color)
    {
        mColor = color;
    }

    Edge3D* Vertex3D::GetEdge()
    {
        return mpEdge;
    }

    const Edge3D* Vertex3D::GetEdge() const
    {
        return mpEdge;
    }

    void Vertex3D::SetEdge(Edge3D* pEdge)
    {
        mpEdge = pEdge;
    }

    int Vertex3D::GetId() const
    {
        return mId;
    }

    void Vertex3D::SetId(int id)
    {
        mId = id;
    }

    Edge3D::Edge3D() :
        mpVertex(NULL),
        mpPair(NULL),
        mpNext(NULL),
        mpPre(NULL),
        mpFace(NULL),
        mId(-1)
    {
    }

    Edge3D::~Edge3D()
    {
    }

    Vertex3D* Edge3D::GetVertex()
    {
        return mpVertex;
    }

    const Vertex3D* Edge3D::GetVertex() const
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

    const Edge3D* Edge3D::GetPair() const
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

    const Edge3D* Edge3D::GetNext() const
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

    const Edge3D* Edge3D::GetPre() const
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

    const Face3D* Edge3D::GetFace() const
    {
        return mpFace;
    }

    void Edge3D::SetFace(Face3D* pFace)
    {
        mpFace = pFace;
    }

    int Edge3D::GetId() const
    {
        return mId;
    }

    void Edge3D::SetId(int id)
    {
        mId = id;
    }

    Face3D::Face3D() : 
        mpEdge(NULL),
        mId(-1)
    {
    }

    Face3D::~Face3D()
    {
    }

    Edge3D* Face3D::GetEdge()
    {
        return mpEdge;
    }

    const Edge3D* Face3D::GetEdge() const
    {
        return mpEdge;
    }

    void Face3D::SetEdge(Edge3D* pEdge)
    {
        mpEdge = pEdge;
    }

    Vector3 Face3D::GetNormal() const
    {
        return mNormal;
    }

    void Face3D::SetNormal(const Vector3& nor)
    {
        mNormal = nor;
    }

    void Face3D::CalArea()
    {
        Vector3 pos0 = mpEdge->GetVertex()->GetPosition();
        Vector3 pos1 = mpEdge->GetNext()->GetVertex()->GetPosition();
        Vector3 pos2 = mpEdge->GetPre()->GetVertex()->GetPosition();
        Vector3 dir1 = pos1 - pos0;
        Vector3 dir2 = pos2 - pos0;
        mArea = (dir1.CrossProduct(dir2)).Length();
    }

    Real Face3D::GetArea() const
    {
        return mArea;
    }

    int Face3D::GetId() const
    {
        return mId;
    }

    void Face3D::SetId(int id)
    {
        mId = id;
    }

    Mesh3D::Mesh3D()
    {
    }

    Mesh3D::~Mesh3D()
    {
        ClearData();
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

    const Vertex3D* Mesh3D::GetVertex(int index) const
    {
        return mVertexList.at(index);
    }

    Edge3D* Mesh3D::GetEdge(int index)
    {
        return mEdgeList.at(index);
    }

    const Edge3D* Mesh3D::GetEdge(int index) const
    {
        return mEdgeList.at(index);
    }

    Face3D* Mesh3D::GetFace(int index)
    {
        return mFaceList.at(index);
    }

    const Face3D* Mesh3D::GetFace(int index) const
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
        pVert->SetId(mVertexList.size());
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
            pEdge->SetId(mEdgeList.size());
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
        std::vector<Edge3D* > innerEdgeList(3);
        for (int i = 0; i < 3; i++)
        {
            Edge3D* pEdge0 = InsertEdge(vertList.at(i), vertList.at((i + 1) % 3));
            pEdge0->SetFace(pFace);
            innerEdgeList.at(i) = pEdge0;
            Edge3D* pEdge1 = InsertEdge(vertList.at((i + 1) % 3), vertList.at(i));
            pEdge0->SetPair(pEdge1);
            pEdge1->SetPair(pEdge0);
        }

        for (int i = 0; i < 3; i++)
        {
            innerEdgeList.at(i)->SetNext(innerEdgeList.at((i + 1) % 3));
            innerEdgeList.at((i + 1) % 3)->SetPre(innerEdgeList.at(i));
        }
        pFace->SetId(mFaceList.size());
        pFace->SetEdge(innerEdgeList.at(0));
        mFaceList.push_back(pFace);

        return pFace;
    }

    void Mesh3D::UnifyPosition(Real size)
    {
        DebugLog << "Mesh3D::UnifyPosition" << std::endl;
        Vector3 posMin(10e10, 10e10, 10e10);
        Vector3 posMax(-10e10, -10e10, -10e10);
        for (std::vector<Vertex3D* >::iterator itr = mVertexList.begin(); itr != mVertexList.end(); ++itr)
        {
            Vector3 pos = (*itr)->GetPosition();
            posMin[0] = posMin[0] < pos[0] ? posMin[0] : pos[0];
            posMin[1] = posMin[1] < pos[1] ? posMin[1] : pos[1];
            posMin[2] = posMin[2] < pos[2] ? posMin[2] : pos[2];
            posMax[0] = posMax[0] > pos[0] ? posMax[0] : pos[0];
            posMax[1] = posMax[1] > pos[1] ? posMax[1] : pos[1];
            posMax[2] = posMax[2] > pos[2] ? posMax[2] : pos[2];
        }
        Vector3 scale3 = posMax - posMin;
        Real scaleMax = scale3[0];
        if (scaleMax < scale3[1])
        {
            scaleMax = scale3[1];
        }
        if (scaleMax < scale3[2])
        {
            scaleMax = scale3[2];
        }
        if (scaleMax > Epsilon)
        {
            Real scaleV = size / scaleMax;
            Vector3 centerPos = (posMin + posMax) / 2.0;
            for (std::vector<Vertex3D* >::iterator itr = mVertexList.begin(); itr != mVertexList.end(); ++itr)
            {
                (*itr)->SetPosition(((*itr)->GetPosition() - centerPos) * scaleV);
            }
        }
    }

    void Mesh3D::UpdateNormal()
    {
        DebugLog << "Update Mesh Normal" << std::endl;
        for (std::vector<Vertex3D* >::iterator itr = mVertexList.begin(); itr != mVertexList.end(); ++itr)
        {
            Vertex3D* pVert = *itr;
            Edge3D* pEdge = pVert->GetEdge();
            Vector3 nor(0, 0, 0);
            do
            {
                if (pEdge->GetFace() != NULL)
                {
                    Vertex3D* pOrigin = pEdge->GetPre()->GetVertex();
                    Vertex3D* pNext = pEdge->GetVertex();
                    Vertex3D* pPre = pEdge->GetNext()->GetVertex();
                    /*Vector3 faceNor = (pNext->GetPosition() - pOrigin->GetPosition()).CrossProduct(pPre->GetPosition() - pOrigin->GetPosition());
                    faceNor.Normalise();
                    nor += faceNor;*/
                    nor += (pNext->GetPosition() - pOrigin->GetPosition()).CrossProduct(pPre->GetPosition() - pOrigin->GetPosition());
                }
                pEdge = pEdge->GetPair()->GetNext();
            } while (pEdge != NULL && pEdge != pVert->GetEdge());
            Real norLen = nor.Normalise();
            if (norLen < Epsilon)
            {
                DebugLog << "normal lenth too small" << std::endl;
                nor[0] = 1.0;
            }
            pVert->SetNormal(nor);
        }
    }

    void Mesh3D::GetBBox(Vector3& bboxMin, Vector3& bboxMax) const
    {
        bboxMin = mBBoxMin;
        bboxMax = mBBoxMax;
    }

    void Mesh3D::CalculateBBox()
    {
        Vector3 posTemp = mVertexList.at(0)->GetPosition(); //mPointSet.at(0)->GetPosition();
        mBBoxMin[0] = mBBoxMax[0] = posTemp[0];
        mBBoxMin[1] = mBBoxMax[1] = posTemp[1];
        mBBoxMin[2] = mBBoxMax[2] = posTemp[2];
        int vertNum = mVertexList.size();
        for (int i = 0; i < vertNum; i++)
        {
            Vector3 pos = mVertexList.at(i)->GetPosition();
            for (int k = 0; k < 3; k++)
            {
                if (mBBoxMin[k] > pos[k])
                {
                    mBBoxMin[k] = pos[k];
                }
                if (mBBoxMax[k] < pos[k])
                {
                    mBBoxMax[k] = pos[k];
                }
            }
        }
        DebugLog << "BBoxMin: " << mBBoxMin[0] << " " << mBBoxMin[1] << " " << mBBoxMin[2] << " "
            << "BBoxMax: " << mBBoxMax[0] << " " << mBBoxMax[1] << " " << mBBoxMax[2] << std::endl;
    }

    void Mesh3D::CalculateFaceArea()
    {
        for (std::vector<Face3D* >::iterator itr = mFaceList.begin(); itr != mFaceList.end(); ++itr)
        {
            (*itr)->CalArea();
        }
    }

    void Mesh3D::ClearData()
    {
        for (std::vector<Vertex3D* >::iterator itr = mVertexList.begin(); itr != mVertexList.end(); ++itr)
        {
            if (*itr != NULL)
            {
                delete *itr;
                *itr = NULL;
            }
        }
        mVertexList.clear();
        for (std::vector<Edge3D* >::iterator itr = mEdgeList.begin(); itr != mEdgeList.end(); ++itr)
        {
            if (*itr != NULL)
            {
                delete *itr;
                *itr = NULL;
            }
        }
        mEdgeList.clear();
        for (std::vector<Face3D* >::iterator itr = mFaceList.begin(); itr != mFaceList.end(); ++itr)
        {
            if (*itr != NULL)
            {
                delete *itr;
                *itr = NULL;
            }
        }
        mFaceList.clear();
        mEdgeMap.clear();
    }
}