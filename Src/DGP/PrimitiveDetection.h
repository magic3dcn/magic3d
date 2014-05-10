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

    struct PrimitiveScore
    {
        double mScore[4];

        PrimitiveScore()
        {
            mScore[0] = 0;
            mScore[1] = 0;
            mScore[2] = 0;
            mScore[3] = 0;
        }
    };

    class ShapeCandidate
    {
    public:
        ShapeCandidate();
        virtual ~ShapeCandidate();
        virtual bool IsValidFromPatch(const Mesh3D* pMesh, std::vector<int>& supportVertex) = 0;
        virtual bool IsValid() = 0;
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag) = 0;
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag) = 0;
        virtual bool FitParameter(const Mesh3D* pMesh) = 0;
        virtual PrimitiveType GetType() = 0;
        bool IsRemoved();
        void SetRemoved(bool valid);
        int GetSupportNum();
        std::vector<int>& GetSupportVertex();
        void SetSupportVertex(const std::vector<int>& supportVertex);
        double GetSupportArea();
        void UpdateSupportArea(const Mesh3D* pMesh, std::vector<double>& vertWeightList);
        double GetScore();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<double>& vertWeightList) = 0;
        bool HasRefit() const;
    protected:
        std::vector<int> mSupportVertex;
        PrimitiveType mType;
        double mScore;
        bool mRemoved;
        double mSupportArea;
        bool mHasRefit;
    };

    class PlaneCandidate : public ShapeCandidate
    {
    public:
        PlaneCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1, const Vertex3D* pVert2);
        virtual ~PlaneCandidate();
        virtual bool IsValid();
        virtual bool IsValidFromPatch(const Mesh3D* pMesh, std::vector<int>& supportVertex);
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual bool FitParameter(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<double>& vertWeightList);
    public:
        const Vertex3D* mpVert0;
        const Vertex3D* mpVert1;
        const Vertex3D* mpVert2;
        MagicMath::Vector3 mCenter, mNormal;

    };

    class SphereCandidate : public ShapeCandidate
    {
    public:
        SphereCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1);
        virtual ~SphereCandidate();
        virtual bool IsValid();
        virtual bool IsValidFromPatch(const Mesh3D* pMesh, std::vector<int>& supportVertex);
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual bool FitParameter(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<double>& vertWeightList);
    public:
        const Vertex3D* mpVert0;
        const Vertex3D* mpVert1;
        MagicMath::Vector3 mCenter;
        double mRadius;
    };

    class CylinderCandidate : public ShapeCandidate
    {
    public:
        CylinderCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1);
        virtual ~CylinderCandidate();
        virtual bool IsValid();
        virtual bool IsValidFromPatch(const Mesh3D* pMesh, std::vector<int>& supportVertex);
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual bool FitParameter(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<double>& vertWeightList);
        void Rectify(const Mesh3D* pMesh);
    public:
        const Vertex3D* mpVert0;
        const Vertex3D* mpVert1;
        MagicMath::Vector3 mCenter;
        double mRadius;
        MagicMath::Vector3 mDir;
    };

    class ConeCandidate : public ShapeCandidate
    {
    public:
        ConeCandidate(const Vertex3D* pVert0, const Vertex3D* pVert1, const Vertex3D* pVert2);
        virtual ~ConeCandidate();
        virtual bool IsValid();
        virtual bool IsValidFromPatch(const Mesh3D* pMesh, std::vector<int>& supportVertex);
        virtual int CalSupportVertex(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual int Refitting(const Mesh3D* pMesh, std::vector<int>& resFlag);
        virtual bool FitParameter(const Mesh3D* pMesh);
        virtual PrimitiveType GetType();
        virtual void UpdateScore(const Mesh3D* pMesh, std::vector<double>& vertWeightList);
    public:
        const Vertex3D* mpVert0;
        const Vertex3D* mpVert1;
        const Vertex3D* mpVert2;
        MagicMath::Vector3 mApex;
        MagicMath::Vector3 mDir;
        double mAngle;
    };

    class PrimitiveDetection
    {
    public:
        PrimitiveDetection();
        ~PrimitiveDetection();

        static void Primitive2DDetection(Mesh3D* pMesh, std::vector<int>& res);
        static void Primitive2DSelection(Mesh3D* pMesh, std::vector<int>& res);
        static void Primitive2DDetectionEnhance(Mesh3D* pMesh, std::vector<int>& res);
        static void Primitive2DDetectionByScore(Mesh3D* pMesh, std::vector<int>& res);
        static ShapeCandidate* Primitive2DSelectionByVertex(Mesh3D* pMesh, int selectIndex, std::vector<int>& res);
        static ShapeCandidate* Primitive2DSelectionByVertexPatch(Mesh3D* pMesh, int selectIndex, std::vector<int>& res);
        static ShapeCandidate* Primitive2DSelectionByVertexSampling(Mesh3D* pMesh, int selectIndex, std::vector<int>& res);
    
    private:
        static void CalVertexWeight(Mesh3D* pMesh, std::vector<double>& vertWeightList);
        static bool SampleVertex(const Mesh3D* pMesh, std::vector<int>& res, std::vector<int>& sampleFlag, std::vector<double>& featureScores,
            std::vector<int>& sampleIndex, int sampleNum, double validProportion);
        static void SampleNeighborVertex(const Mesh3D* pMesh, std::vector<int>& neighborList, std::vector<int>& sampleNeigbors);
        static bool AddNewCandidates(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh, 
            std::vector<int>& res, std::vector<int>& sampleFlag, std::vector<double>& vertWeightList);
        static bool AddNewCandidatesEnhance(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh, 
            std::vector<int>& res, std::vector<int>& sampleFlag, std::vector<double>& vertWeightList, 
            std::vector<double>& featureScores, std::vector<int>& sampleIndex);
        static bool AddNewCandidatesByScore(std::vector<ShapeCandidate* >& candidates, const Mesh3D* pMesh, 
            std::vector<int>& res, std::vector<int>& sampleFlag, std::vector<double>& vertWeightList, 
            std::vector<double>& featureScores, std::vector<int>& sampleIndex);
        static bool IsCandidateAcceptable(int index, std::vector<ShapeCandidate* >& candidates);
        static void RemoveAcceptableCandidate(std::vector<ShapeCandidate* >& candidates, const std::vector<int>& res);
        static void ChosePotentials(std::vector<ShapeCandidate* >& candidates, std::vector<int>& potentials);
        static int ChoseBestCandidate(std::vector<ShapeCandidate* >& candidates);
        static bool UpdateAcceptableArea(Mesh3D* pMesh, std::vector<int>& res);
        static bool UpdateAcceptableAreaEnhance(Mesh3D* pMesh, std::vector<int>& res, double acceptScale);
        static bool UpdateAcceptableScore(Mesh3D* pMesh, std::vector<int>& res, double scoreScale);
        static void CalFeatureBoundary(Mesh3D* pMesh, std::vector<int>& features);
        static void CalFeatureScore(Mesh3D* pMesh, std::vector<int>& features, std::vector<double>& scores);
        static void CalScaleGradient(std::vector<double>& scaleField, std::vector<MagicMath::Vector3>& gradientField, 
            const MagicDGP::Mesh3D* pMesh);
        static void CalFeatureScoreByGradient(Mesh3D* pMesh, std::vector<int>& features, std::vector<double>& scores);
        static int RefitPotentials(std::vector<ShapeCandidate* >& candidates, std::vector<int>& potentials, std::map<double, int>& refitedPotentials,
            Mesh3D* pMesh, std::vector<int>& resFlag, std::vector<double>& vertWeightList);
    };

}