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

    class ShapeCandidate
    {
    public:
        ShapeCandidate();
        virtual ~ShapeCandidate();
        virtual bool IsValid() = 0;
        virtual int CalSupportVertex(const Mesh3D* pMesh) = 0;
        virtual PrimitiveType GetType() = 0;
        int GetSupportNum();
        std::vector<int>& GetSupportVertex();
    private:
        std::vector<int> mSupportVertex;
        PrimitiveType mType;
    };

    class PlaneCandidate : public ShapeCandidate
    {
    public:
        PlaneCandidate(const Vector3& pos0, const Vector3& pos1, const Vector3& pos2, const Vector3& nor0, const Vector3& nor1, const Vector3& nor2);
        virtual ~PlaneCandidate();
        virtual bool IsValid();
        virtual int CalSupportVertex(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
    private:
        Vector3 mPos0, mPos1, mPos2;
        Vector3 mNor0, mNor1, mNor2;
    };

    class SphereCandidate : public ShapeCandidate
    {
    public:
        SphereCandidate(const Vector3& pos0, const Vector3& pos1, const Vector3& nor0, const Vector3& nor1);
        virtual ~SphereCandidate();
        virtual bool IsValid();
        virtual int CalSupportVertex(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
    private:
        Vector3 mPos0, mPos1;
        Vector3 mNor0, mNor1;
    };

    class CylinderCandidate : public ShapeCandidate
    {
    public:
        CylinderCandidate(const Vector3& pos0, const Vector3& pos1, const Vector3& nor0, const Vector3& nor1);
        virtual ~CylinderCandidate();
        virtual bool IsValid();
        virtual int CalSupportVertex(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
    private:
        Vector3 mPos0, mPos1;
        Vector3 mNor0, mNor1;
    };

    class ConeCandidate : public ShapeCandidate
    {
    public:
        ConeCandidate(const Vector3& pos0, const Vector3& pos1, const Vector3& pos2, const Vector3& nor0, const Vector3& nor1, const Vector3& nor2);
        virtual ~ConeCandidate();
        virtual bool IsValid();
        virtual int CalSupportVertex(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
    private:
        Vector3 mPos0, mPos1, mPos2;
        Vector3 mNor0, mNor1, mNor2;
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