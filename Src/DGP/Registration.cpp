#include "StdAfx.h"
#include "Registration.h"

namespace MagicDGP
{
    Registration::Registration()
    {
    }

    Registration::~Registration()
    {
    }

    void Registration::ICPRegistrate(const Point3DSet* pRef, Point3DSet* pPC)
    {
        std::vector<int> sampleIndex;
        ICPSamplePoint(pPC, sampleIndex);
        std::vector<int> correspondIndex;
        ICPFindCorrespondance(pRef, pPC, sampleIndex, correspondIndex);
        ICPEnergyMinimization(pRef, pPC, sampleIndex, correspondIndex);
    }

    void Registration::ICPSamplePoint(const Point3DSet* pPC, std::vector<int>& sampleIndex)
    {

    }

    void Registration::ICPFindCorrespondance(const Point3DSet* pRef, const Point3DSet* pPC, std::vector<int>& sampleIndex,  std::vector<int>& correspondIndex)
    {

    }

    void Registration::ICPEnergyMinimization(const Point3DSet* pRef, Point3DSet* pPC, std::vector<int>& sampleIndex, std::vector<int>& correspondIndex)
    {

    }
}
