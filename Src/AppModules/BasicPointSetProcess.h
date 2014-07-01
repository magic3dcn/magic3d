#pragma once
#include "MagicObject.h"
#include <string>
#include <vector>

namespace MagicDGP
{
    class Point3DSet;
}

namespace MagicApp
{
    class BasicPointSetProcess : public MagicObject
    {
    public:
        BasicPointSetProcess();
        ~BasicPointSetProcess();

        bool LoadPointSet(const std::string& fileName);
        const MagicDGP::Point3DSet* GetPointSet(void) const;
        MagicDGP::Point3DSet* GetPointSet(void);
        void SavePointSet(const std::string& fileName);

        void PointSetRegistration(const std::string& path, const std::vector<int>& pointIndex, const std::vector<double>& boundingBox);

    private:
        void Clear(void);

    private:
        MagicDGP::Point3DSet* mpPointSet;
    };
}
