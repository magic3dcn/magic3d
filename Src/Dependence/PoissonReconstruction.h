#pragma once
#include "../DGP/PointCloud3D.h"
#include "../DGP/Mesh3D.h"
#include "Hash.h"

template< class Real >
class PlyValueVertex;
template< class Real >
struct Point3D;
template< class Real >
class PlyColorVertex;
namespace MagicDependence
{
    class PoissonReconstruction
    {
    public:
        PoissonReconstruction();
        static MagicDGP::Mesh3D* ScreenPoissonRecon(const MagicDGP::Point3DSet* pPC);
        static void PoissonRecon(int argc , char* argv[], const MagicDGP::Point3DSet* pPC, std::vector< PlyValueVertex< float > >& vertices, std::vector< std::vector< int > >& polygons);
        static MagicDGP::Mesh3D* SurfaceTrimmer(int argc , char* argv[], std::vector< PlyValueVertex< float > >& vertices, std::vector< std::vector< int > >& polygons);
        ~PoissonReconstruction();

    private:
        template< class Real >
        static void SmoothValues( std::vector< PlyValueVertex< Real > >& vertices , const std::vector< std::vector< int > >& polygons )
        {
            std::vector< int > count( vertices.size() );
            std::vector< Real > sums( vertices.size() , 0 );
            for( size_t i=0 ; i<polygons.size() ; i++ )
            {
                int sz = int(polygons[i].size());
                for( int j=0 ; j<sz ; j++ )
                {
                    int j1 = j , j2 = (j+1)%sz;
                    int v1 = polygons[i][j1] , v2 = polygons[i][j2];
                    count[v1]++ , count[v2]++;
                    sums[v1] += vertices[v2].value , sums[v2] += vertices[v1].value;
                }
            }
            for( size_t i=0 ; i<vertices.size() ; i++ ) vertices[i].value = ( sums[i] + vertices[i].value ) / ( count[i] + 1 );
        }

        static long long EdgeKey( int key1 , int key2 );

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
            int sz = int( polygon.size() );
            std::vector< bool > gt( sz );
            int gtCount = 0;
            for( int j=0 ; j<sz ; j++ )
            {
                gt[j] = ( vertices[ polygon[j] ].value>trimValue );
                if( gt[j] ) gtCount++;
            }
            if     ( gtCount==sz ){ if( gtPolygons ) gtPolygons->push_back( polygon ) ; if( gtFlags ) gtFlags->push_back( false ); }
            else if( gtCount==0  ){ if( ltPolygons ) ltPolygons->push_back( polygon ) ; if( ltFlags ) ltFlags->push_back( false ); }
            else
            {
                int start;
                for( start=0 ; start<sz ; start++ ) if( gt[start] && !gt[(start+sz-1)%sz] ) break;

                bool gtFlag = true;
                std::vector< int > poly;

                // Add the initial vertex
                {
                    int j1 = (start+int(sz)-1)%sz , j2 = start;
                    int v1 = polygon[j1] , v2 = polygon[j2];
                    int vIdx;
                    hash_map< long long , int >::iterator iter = vertexTable.find( EdgeKey( v1 , v2 ) );
                    if( iter==vertexTable.end() )
                    {
                        vertexTable[ EdgeKey( v1 , v2 ) ] = vIdx = int( vertices.size() );
                        vertices.push_back( InterpolateVertices( vertices[v1] , vertices[v2] , trimValue ) );
                    }
                    else vIdx = iter->second;
                    poly.push_back( vIdx );
                }

                for( int _j=0  ; _j<=sz ; _j++ )
                {
                    int j1 = (_j+start+sz-1)%sz , j2 = (_j+start)%sz;
                    int v1 = polygon[j1] , v2 = polygon[j2];
                    if( gt[j2]==gtFlag ) poly.push_back( v2 );
                    else
                    {
                        int vIdx;
                        hash_map< long long , int >::iterator iter = vertexTable.find( EdgeKey( v1 , v2 ) );
                        if( iter==vertexTable.end() )
                        {
                            vertexTable[ EdgeKey( v1 , v2 ) ] = vIdx = int( vertices.size() );
                            vertices.push_back( InterpolateVertices( vertices[v1] , vertices[v2] , trimValue ) );
                        }
                        else vIdx = iter->second;
                        poly.push_back( vIdx );
                        if( gtFlag ){ if( gtPolygons ) gtPolygons->push_back( poly ) ; if( ltFlags ) ltFlags->push_back( true ); }
                        else        { if( ltPolygons ) ltPolygons->push_back( poly ) ; if( gtFlags ) gtFlags->push_back( true ); }
                        poly.clear() , poly.push_back( vIdx ) , poly.push_back( v2 );
                        gtFlag = !gtFlag;
                    }
                }
            }
        }

        template< class Real >
        static double PolygonArea( const std::vector< PlyValueVertex< Real > >& vertices , const std::vector< int >& polygon )
        {
            if( polygon.size()<3 ) return 0.;
            else if( polygon.size()==3 ) return TriangleArea( vertices[polygon[0]].point , vertices[polygon[1]].point , vertices[polygon[2]].point );
            else
            {
                Point3D< Real > center;
                for( size_t i=0 ; i<polygon.size() ; i++ ) center += vertices[ polygon[i] ].point;
                center /= Real( polygon.size() );
                double area = 0;
                for( size_t i=0 ; i<polygon.size() ; i++ ) area += TriangleArea( center , vertices[ polygon[i] ].point , vertices[ polygon[ (i+1)%polygon.size() ] ].point );
                return area;
            }
        }

        template< class Real >
        static double TriangleArea( Point3D< Real > v1 , Point3D< Real > v2 , Point3D< Real > v3 )
        {
            Point3D< Real > n = CrossProduct( v2-v1 , v3-v1 );
            return sqrt( n[0]*n[0] + n[1]*n[1] + n[2]*n[2] ) / 2.;
        }

        template< class Real >
        static Point3D< Real > CrossProduct( Point3D< Real > p1 , Point3D< Real > p2 )
        {
            return Point3D< Real >( p1[1]*p2[2]-p1[2]*p2[1] , p1[2]*p2[0]-p1[0]*p2[2] , p1[0]*p1[1]-p1[1]*p2[0] );
        }

        template< class Real >
        static void Triangulate( const std::vector< PlyValueVertex< Real > >& vertices , const std::vector< std::vector< int > >& polygons , std::vector< std::vector< int > >& triangles )
        {
            triangles.clear();
            for( size_t i=0 ; i<polygons.size() ; i++ )
            if( polygons.size()>3 )
            {
                MinimalAreaTriangulation< Real > mat;
                std::vector< Point3D< Real > > _vertices( polygons[i].size() );
                std::vector< TriangleIndex > _triangles;
                for( int j=0 ; j<int( polygons[i].size() ) ; j++ ) _vertices[j] = vertices[ polygons[i][j] ].point;
                mat.GetTriangulation( _vertices , _triangles );

                // Add the triangles to the mesh
                size_t idx = triangles.size();
                triangles.resize( idx+_triangles.size() );
                for( int j=0 ; j<int(_triangles.size()) ; j++ )
                {
                    triangles[idx+j].resize(3);
                    for( int k=0 ; k<3 ; k++ ) triangles[idx+j][k] = polygons[i][ _triangles[j].idx[k] ];
                }
            }
            else if( polygons[i].size()==3 ) triangles.push_back( polygons[i] );
        }

        template< class Vertex >
        static void RemoveHangingVertices( std::vector< Vertex >& vertices , std::vector< std::vector< int > >& polygons )
        {
            hash_map< int , int > vMap;
            std::vector< bool > vertexFlags( vertices.size() , false );
            for( size_t i=0 ; i<polygons.size() ; i++ ) for( size_t j=0 ; j<polygons[i].size() ; j++ ) vertexFlags[ polygons[i][j] ] = true;
            int vCount = 0;
            for( int i=0 ; i<int(vertices.size()) ; i++ ) if( vertexFlags[i] ) vMap[i] = vCount++;
            for( size_t i=0 ; i<polygons.size() ; i++ ) for( size_t j=0 ; j<polygons[i].size() ; j++ ) polygons[i][j] = vMap[ polygons[i][j] ];

            std::vector< Vertex > _vertices( vCount );
            for( int i=0 ; i<int(vertices.size()) ; i++ ) if( vertexFlags[i] ) _vertices[ vMap[i] ] = vertices[i];
            vertices = _vertices;
        }

        template< class Real >
        static void ColorVertices( const std::vector< PlyValueVertex< Real > >& inVertices , std::vector< PlyColorVertex< Real > >& outVertices , float min , float max )
        {
            outVertices.resize( inVertices.size() );
            for( size_t i=0 ; i<inVertices.size() ; i++ )
            {
                outVertices[i].point = inVertices[i].point;
                float temp = ( inVertices[i].value-min ) / ( max - min );
                temp = std::max< float >( 0.f , std::min< float >( 1.f , temp ) );
                temp *= 255;
                outVertices[i].color[0] = outVertices[i].color[1] = outVertices[i].color[2] = (int)temp;
            }
        }

        template< class Real >
        static PlyValueVertex< Real > InterpolateVertices( const PlyValueVertex< Real >& v1 , const PlyValueVertex< Real >& v2 , const float& value )
        {
            if( v1.value==v2.value ) return (v1+v2)/Real(2.);
            PlyValueVertex< Real > v;

            Real dx = (v1.value-value)/(v1.value-v2.value);
            for( int i=0 ; i<3 ; i++ ) v.point.coords[i]=v1.point.coords[i]*(1.f-dx)+v2.point.coords[i]*dx;
            v.value=v1.value*(1.f-dx)+v2.value*dx;
            return v;
        }

    };

}