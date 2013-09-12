#pragma once
#include "../DGP/PointCloud3D.h"
#include "../DGP/Mesh3D.h"
#include "TimePoisson.h"
#include "MarchingCubes.h"
#include "Octree.h"
#include "SparseMatrix.h"
#include "CmdLineParser.h"
#include "PPolynomial.h"
#include "Ply.h"
#include "MemoryUsage.h"
#include "MultiGridOctreeData.h"

namespace MagicDependence
{
    class PoissonReconstruction
    {
    public:
        PoissonReconstruction();
        static MagicDGP::Mesh3D* PoissonRecon(int argc , char* argv[], const MagicDGP::Point3DSet* pPC);
        static MagicDGP::Mesh3D* SurfaceTrimmer(int argc , char* argv[], const MagicDGP::Mesh3D* pMesh);
        ~PoissonReconstruction();

    private:
        template< class Real >
        static void SmoothValues( std::vector< PlyValueVertex< Real > >& vertices , const std::vector< std::vector< int > >& polygons )
        {
        }
        static void SetConnectedComponents( const std::vector< std::vector< int > >& polygons , std::vector< std::vector< int > >& components );
        template< class Real >
        static void SplitPolygon
        (
            const std::vector< int >& polygon ,
            std::vector< PlyValueVertex< Real > >& vertices , 
            std::vector< std::vector< int > >* ltPolygons , std::vector< std::vector< int > >* gtPolygons ,
            std::vector< bool >* ltFlags , std::vector< bool >* gtFlags ,
            hash_map< long long , int >& vertexTable ,
            Real trimValue
        )
        {
        }
        template< class Real >
        static double PolygonArea( const std::vector< PlyValueVertex< Real > >& vertices , const std::vector< int >& polygon )
        {
        }
        template< class Real >
        static double TriangleArea( Point3D< Real > v1 , Point3D< Real > v2 , Point3D< Real > v3 )
        {
        }
        template< class Real >
        static Point3D< Real > CrossProduct( Point3D< Real > p1 , Point3D< Real > p2 )
        {
        }
        template< class Real >
        static void Triangulate( const std::vector< PlyValueVertex< Real > >& vertices , const std::vector< std::vector< int > >& polygons , std::vector< std::vector< int > >& triangles )
        {
        }
        template< class Vertex >
        static void RemoveHangingVertices( std::vector< Vertex >& vertices , std::vector< std::vector< int > >& polygons )
        {
        }
        template< class Real >
        static void ColorVertices( const std::vector< PlyValueVertex< Real > >& inVertices , std::vector< PlyColorVertex< Real > >& outVertices , float min , float max )
        {
        }
    };

}