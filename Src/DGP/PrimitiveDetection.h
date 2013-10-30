#pragma once 
#include "Mesh3D.h"

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
        static int mMinInitSupportNum;
        static int mMinSupportNum;
        static Real mMinSupportArea;
        static int mSampleBreakNum;
        static int mSampleBreakDelta;
        static Real mAcceptableArea;
        static Real mAcceptableAreaDelta;
        static Real mMaxAngleDeviation;
        static Real mMaxDistDeviation;
        static Real mMaxCylinderRadiusScale;
        static Real mMaxSphereRadiusScale;
        static Real mMaxCylinderRadius;
        static Real mMaxSphereRadius;
        static Real mMinConeAngle;
        static Real mMaxConeAngle;
        static Real mMaxConeAngleDeviation;
        static Real mBaseScore;
    };

    class ShapeCandidate
    {
    public:
        ShapeCandidate();
        virtual ~ShapeCandidate();
        virtual bool IsValid() = 0;
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag) = 0;
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag) = 0;
        virtual PrimitiveType GetType() = 0;
        bool IsRemoved();
        void SetRemoved(bool valid);
        int GetSupportNum();
        std::vector<int>& GetSupportVertex();
        void SetSupportVertex(const std::vector<int>& supportVertex);
        Real GetSupportArea();
        void UpdateSupportArea(const Mesh3D* pMesh);
        Real GetScore();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList) = 0;
    protected:
        std::vector<int> mSupportVertex;
        PrimitiveType mType;
        Real mScore;
        bool mRemoved;
        Real mSupportArea;
    };

    class PlaneCandidate : public ShapeCandidate
    {
    public:
        PlaneCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1, const Vertex3D* pVert2);
        virtual ~PlaneCandidate();
        virtual bool IsValid();
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual PrimitiveType GetType();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList);
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
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual PrimitiveType GetType();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList);
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
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual PrimitiveType GetType();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList);
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
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual PrimitiveType GetType();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList);
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

        static void Primitive2DDetection(Mesh3D* pMesh, std::vector<int>& res);
    
    private:
        static void CalVertexWeight(Mesh3D* pMesh, std::vector<Real>& vertWeightList);
        static bool FindNewCandidates(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh, 
            std::vector<int>& res, std::vector<int>& sampleFlag, std::vector<Real>& vertWeightList);
        static int ChoseBestCandidate(std::vector<ShapeCandidate* >& candidates);
        static bool IsCandidateAcceptable(int index, std::vector<ShapeCandidate* >& candidates);
        static void RemoveAcceptableCandidate(std::vector<ShapeCandidate* >& candidates, const std::vector<int>& res);

    };

}