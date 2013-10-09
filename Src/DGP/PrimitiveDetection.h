#pragma once 
#include "../Dependence/RansacPrimitiveShapes.h"

namespace MagicDGP
{
    enum PrimitiveType
    {
        None = 0,
        Plane, 
        Sphere,
        Cylinder,
        Cone
    };

    class PrimitiveParameters
    {
    public:
        PrimitiveParameters();
        ~PrimitiveParameters();

    public:
        static Real mMaxCylinderRadius;
        static Real mMaxSphereRadius;
    };

    class ShapeCandidate
    {
    public:
        ShapeCandidate();
        virtual ~ShapeCandidate();
        virtual bool IsValid() = 0;
        virtual int CalSupportVertex(const Mesh3D* pMesh) = 0;
        virtual int Refitting(const Mesh3D* pMesh) = 0;
        virtual PrimitiveType GetType() = 0;
        int GetSupportNum();
        std::vector<int>& GetSupportVertex();
    protected:
        std::vector<int> mSupportVertex;
        PrimitiveType mType;
    };

    class PlaneCandidate : public ShapeCandidate
    {
    public:
        PlaneCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1, const Vertex3D* pVert2);
        virtual ~PlaneCandidate();
        virtual bool IsValid();
        virtual int CalSupportVertex(const Mesh3D* pMesh);
        virtual int Refitting(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
    private:
        const Vertex3D* mpVert0;
        const Vertex3D* mpVert1;
        const Vertex3D* mpVert2;
        Vector3 mCenter, mNormal;

    };

    class SphereCandidate : public ShapeCandidate
    {
    public:
        SphereCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1);
        virtual ~SphereCandidate();
        virtual bool IsValid();
        virtual int CalSupportVertex(const Mesh3D* pMesh);
        virtual int Refitting(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
    private:
        const Vertex3D* mpVert0;
        const Vertex3D* mpVert1;
        Vector3 mCenter;
        Real mRadius;
    };

    class CylinderCandidate : public ShapeCandidate
    {
    public:
        CylinderCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1);
        virtual ~CylinderCandidate();
        virtual bool IsValid();
        virtual int CalSupportVertex(const Mesh3D* pMesh);
        virtual int Refitting(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
    private:
        const Vertex3D* mpVert0;
        const Vertex3D* mpVert1;
        Vector3 mCenter;
        Real mRadius;
        Vector3 mDir;
    };

    class ConeCandidate : public ShapeCandidate
    {
    public:
        ConeCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1, const Vertex3D* pVert2);
        virtual ~ConeCandidate();
        virtual bool IsValid();
        virtual int CalSupportVertex(const Mesh3D* pMesh);
        virtual int Refitting(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
    private:
        const Vertex3D* mpVert0;
        const Vertex3D* mpVert1;
        const Vertex3D* mpVert2;
        Vector3 mApex;
        Vector3 mDir;
        Real mAngle;
    };

    class PrimitiveDetection
    {
    public:
        PrimitiveDetection();
        ~PrimitiveDetection();

        static void Primitive2DDetectionFromRansac(const Mesh3D* pMesh, std::vector<int>& res);
        static void Primitive2DDetection(const Mesh3D* pMesh, std::vector<int>& res);
        static void Primitive2DDetectionTest(const Mesh3D* pMesh, std::vector<int>& res);
    
    private:
        static void AddShapeCandidates(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh, std::vector<int>& validFlags);
        static void FindAllShapeCandidates(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh);
        static int ChoseBestCandidate(std::vector<ShapeCandidate* >& candidates);
        static bool IsCandidateAcceptable(int index, std::vector<ShapeCandidate* >& candidates);
        static void RemoveAcceptableCandidate(int index, std::vector<ShapeCandidate* >& candidates);

    private:

    };

}