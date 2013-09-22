#include "RansacPrimitiveShapes.h"
#include <RansacShapeDetector.h>
#include <PlanePrimitiveShapeConstructor.h>
#include <CylinderPrimitiveShapeConstructor.h>
#include <SpherePrimitiveShapeConstructor.h>
#include <ConePrimitiveShapeConstructor.h>
#include <TorusPrimitiveShapeConstructor.h>
#include "flann/flann.h"

namespace MagicDependence
{
    RansacPrimitiveShapes::RansacPrimitiveShapes()
    {
    }

    RansacPrimitiveShapes::~RansacPrimitiveShapes()
    {
    }

    void RansacPrimitiveShapes::MeshPrimitiveDetection(const MagicDGP::Mesh3D* pMesh, std::vector<int>& res)
    {
        RansacShapeDetector::Options ransacOptions;
        float bbxScale = 2.f;
        //ransacOptions.m_epsilon = .01f * pc.getScale(); // set distance threshold to .01f of bounding box width
        ransacOptions.m_epsilon = .007f * bbxScale;
        // NOTE: Internally the distance threshold is taken as 3 * ransacOptions.m_epsilon!!!
        //ransacOptions.m_bitmapEpsilon = .02f * pc.getScale(); // set bitmap resolution to .02f of bounding box width
        ransacOptions.m_bitmapEpsilon = .02f * bbxScale;
        // NOTE: This threshold is NOT multiplied internally!
        //ransacOptions.m_normalThresh = .9f; // this is the cos of the maximal normal deviation
        ransacOptions.m_normalThresh = .9848f; //cos(10 degree)
        ransacOptions.m_minSupport = 200; // this is the minimal numer of points required for a primitive
        ransacOptions.m_probability = .001f; // this is the "probability" with which a primitive is overlooked

        RansacShapeDetector detector(ransacOptions); // the detector object

        // set which primitives are to be detected by adding the respective constructors
        detector.Add(new PlanePrimitiveShapeConstructor());
        detector.Add(new SpherePrimitiveShapeConstructor());
        detector.Add(new CylinderPrimitiveShapeConstructor());
        detector.Add(new ConePrimitiveShapeConstructor());
        //detector.Add(new TorusPrimitiveShapeConstructor());

        PointCloud pc;
        int vertNum = pMesh->GetVertexNumber();
        for (int i = 0; i < vertNum; i++)
        {
            const MagicDGP::Vertex3D* pVert = pMesh->GetVertex(i);
            MagicDGP::Vector3 pos = pVert->GetPosition();
            MagicDGP::Vector3 nor = pVert->GetNormal();
            Vec3f pos_(pos[0], pos[1], pos[2]);
            Vec3f nor_(nor[0], nor[1], nor[2]);
            Point vert(pos_, nor_);
            pc.push_back(vert);
        }

        MiscLib::Vector< std::pair< MiscLib::RefCountPtr< PrimitiveShape >, size_t > > shapes; // stores the detected shapes
        size_t remaining = detector.Detect(pc, 0, pc.size(), &shapes); // run detection

        //copy result
        int dim = 3;
        int refNum = vertNum;
        float* dataSet = new float[refNum * dim];
        for (int i = 0; i < refNum; i++)
        {
            MagicDGP::Vector3 pos = pMesh->GetVertex(i)->GetPosition();// pRef->GetPoint(i)->GetPosition();
            dataSet[dim * i + 0] = pos[0];
            dataSet[dim * i + 1] = pos[1];
            dataSet[dim * i + 2] = pos[2];
        }
        int searchNum = 0;
        for (int i = 0; i < shapes.size(); i++)
        {
            searchNum += shapes.at(i).second;
        }
        float* searchSet = new float[searchNum * dim];
        for (int i = 0; i < searchNum; i++)
        {
            //Vector3 pos = pOrigin->GetPoint(sampleIndex.at(i))->GetPosition();
            Point pos = pc.at(vertNum - i - 1);
            searchSet[dim * i + 0] = pos[0];
            searchSet[dim * i + 1] = pos[1];
            searchSet[dim * i + 2] = pos[2];
        }
        int nn = 1;
        int* pIndex = new int[searchNum * nn];
        float* pDist = new float[searchNum * nn];
        FLANNParameters searchPara;
        searchPara = DEFAULT_FLANN_PARAMETERS;
        searchPara.algorithm = FLANN_INDEX_KDTREE;
        searchPara.trees = 8;
        searchPara.log_level = FLANN_LOG_INFO;
        searchPara.checks = 64;
        float speedup;
        flann_index_t indexId = flann_build_index(dataSet, refNum, dim, &speedup, &searchPara);
        flann_find_nearest_neighbors_index(indexId, searchSet, searchNum, pIndex, pDist, nn, &searchPara);
        flann_free_index(indexId, &searchPara);
        delete []dataSet;
        delete []searchSet;

        res.clear();
        res = std::vector<int>(vertNum, 0);
        int remainSize = vertNum;
        int reverseIndex = 0;
        for (int i = 0; i < shapes.size(); i++)
        {
            int curSize = shapes.at(i).second;
            int primitiveType = shapes.at(i).first->Identifier() + 1;
            for (int j = remainSize - curSize; j < remainSize; j++)
            {
                res.at(pIndex[reverseIndex]) = primitiveType;
                reverseIndex++;
            }
        }

        delete []pIndex;
        delete []pDist;

    }
}