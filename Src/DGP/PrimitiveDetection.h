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
        Cone, 
        Blend, 
        Other
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
        void UpdateSupportArea(const Mesh3D* pMesh, std::vector<Real>& vertWeightList);
        Real GetScore();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<Real>& vertWeightList) = 0;
        bool HasRefit() const;
    protected:
        std::vector<int> mSupportVertex;
        PrimitiveType mType;
        Real mScore;
        bool mRemoved;
        Real mSupportArea;
        bool mHasRefit;
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
        static void Primitive2DSelection(Mesh3D* pMesh, std::vector<int>& res);
        static void Primitive2DDetectionEnhance(Mesh3D* pMesh, std::vector<int>& res);
        static void Primitive2DSelectionByVertex(Mesh3D* pMesh, int selectIndex, std::vector<int>& res);
    
    private:
        static void CalVertexWeight(Mesh3D* pMesh, std::vector<Real>& vertWeightList);
        static bool AddNewCandidates(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh, 
            std::vector<int>& res, std::vector<int>& sampleFlag, std::vector<Real>& vertWeightList);
        static bool AddNewCandidatesEnhance(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh, 
            std::vector<int>& res, std::vector<int>& sampleFlag, std::vector<Real>& vertWeightList, 
            std::vector<Real>& featureScores, bool isSmallSampled);
        static bool IsCandidateAcceptable(int index, std::vector<ShapeCandidate* >& candidates);
        static void RemoveAcceptableCandidate(std::vector<ShapeCandidate* >& candidates, const std::vector<int>& res);
        static void ChosePotentials(std::vector<ShapeCandidate* >& candidates, std::vector<int>& potentials);
        static int ChoseBestCandidate(std::vector<ShapeCandidate* >& candidates);
        static bool UpdateAcceptableArea(Mesh3D* pMesh, std::vector<int>& res);
        static bool UpdateAcceptableAreaEnhance(Mesh3D* pMesh, std::vector<int>& res, bool isFirst);
        static void CalFeatureBoundary(Mesh3D* pMesh, std::vector<int>& features);
        static void CalFeatureScore(Mesh3D* pMesh, std::vector<int>& features, std::vector<Real>& scores);
        static int RefitPotentials(std::vector<ShapeCandidate* >& candidates, std::vector<int>& potentials, std::map<Real, int>& refitedPotentials,
            Mesh3D* pMesh, std::vector<int>& resFlag, std::vector<Real>& vertWeightList);
    };

}