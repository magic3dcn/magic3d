#include "stdafx.h"
#include "PoissonReconstruction.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <Psapi.h>

#include "TimePoisson.h"
#include "MarchingCubes.h"
#include "Octree.h"
#include "SparseMatrix.h"
#include "CmdLineParser.h"
#include "PPolynomial.h"
#include "Ply.h"
#include "MemoryUsage.h"

#include "omp.h"
#include <stdarg.h>
#include "MultiGridOctreeData.h"

namespace MagicDependence
{
    PoissonReconstruction::PoissonReconstruction()
    {
    }

    PoissonReconstruction::~PoissonReconstruction()
    {
    }

    MagicDGP::Mesh3D* PoissonReconstruction::PoissonRecon(int argc , char* argv[], const MagicDGP::Point3DSet* pPC)
    {
        cmdLineString
            In( "in" ) ,
            Out( "out" ) ,
            VoxelGrid( "voxel" ) ,
            XForm( "xForm" );

        cmdLineReadable
            Performance( "performance" ) ,
            ShowResidual( "showResidual" ) ,
            NoComments( "noComments" ) ,
            PolygonMesh( "polygonMesh" ) ,
            Confidence( "confidence" ) ,
            NonManifold( "nonManifold" ) ,
            ASCII( "ascii" ) ,
            Density( "density" ) ,
            Verbose( "verbose" );

        cmdLineInt
            Depth( "depth" , 8 ) ,
            SolverDivide( "solverDivide" , 8 ) ,
            IsoDivide( "isoDivide" , 8 ) ,
            KernelDepth( "kernelDepth" ) ,
            AdaptiveExponent( "adaptiveExp" , 1 ) ,
            MinIters( "minIters" , 24 ) ,
            FixedIters( "iters" , -1 ) ,
            VoxelDepth( "voxelDepth" , -1 ) ,
            MinDepth( "minDepth" , 5 ) ,
            MaxSolveDepth( "maxSolveDepth" ) ,
            BoundaryType( "boundary" , 1 ) ,
            Threads( "threads" , omp_get_num_procs() );

        cmdLineFloat
            SamplesPerNode( "samplesPerNode" , 1.f ) ,
            Scale( "scale" , 1.1f ) ,
            SolverAccuracy( "accuracy" , float(1e-3) ) ,
            PointWeight( "pointWeight" , 4.f );

        cmdLineReadable* params[] =
        {
            &In , &Depth , &Out , &XForm ,
            &SolverDivide , &IsoDivide , &Scale , &Verbose , &SolverAccuracy , &NoComments ,
            &KernelDepth , &SamplesPerNode , &Confidence , &NonManifold , &PolygonMesh , &ASCII , &ShowResidual , &MinIters , &FixedIters , &VoxelDepth ,
            &PointWeight , &VoxelGrid , &Threads , &MinDepth , &MaxSolveDepth ,
            &AdaptiveExponent , &BoundaryType ,
            &Density
        };

        cmdLineParse( argc-1 , &argv[1] , sizeof(params)/sizeof(cmdLineReadable*) , params , 1 );
        /*if( Density.set ) 
            return Execute< 2 , PlyValueVertex< Real > , true  >(argc , argv, pPC);
        else       
            return Execute< 2 ,      PlyVertex< Real > , false >(argc , argv, pPC);*/
        //Execute
        int i;
        int paramNum = sizeof(params)/sizeof(cmdLineReadable*);
        int commentNum=0;
        char **comments;

        comments = new char*[paramNum + 7];
        for( i=0 ; i<paramNum+7 ; i++ ) comments[i] = new char[1024];

        //if( Verbose.set ) echoStdout=1;

        XForm4x4< Real > xForm , iXForm;
        if( XForm.set )
        {
            FILE* fp = fopen( XForm.value , "r" );
            if( !fp )
            {
                fprintf( stderr , "[WARNING] Could not read x-form from: %s\n" , XForm.value );
                xForm = XForm4x4< Real >::Identity();
            }
            else
            {
                for( int i=0 ; i<4 ; i++ ) for( int j=0 ; j<4 ; j++ ) fscanf( fp , " %f " , &xForm( i , j ) );
                fclose( fp );
            }
        }
        else xForm = XForm4x4< Real >::Identity();
        iXForm = xForm.inverse();

        //DumpOutput2( comments[commentNum++] , "Running Screened Poisson Reconstruction (Version 5.0)\n" , Degree );
        //char str[1024];
        //for( int i=0 ; i<paramNum ; i++ )
        //    if( params[i]->set )
        //    {
        //        params[i]->writeValue( str );
        //        if( strlen( str ) ) DumpOutput2( comments[commentNum++] , "\t--%s %s\n" , params[i]->name , str );
        //        else                DumpOutput2( comments[commentNum++] , "\t--%s\n" , params[i]->name );
        //    }

        double t;
        double tt=Time();
        Real isoValue = 0;

        //Octree< Degree , OutputDensity > tree;
        Octree< 2 , true > tree;
        tree.threads = Threads.value;
        //if( !In.set )
        //{
        //    ShowUsage(argv[0]);
        //    return 0;
        //}
        if( !MaxSolveDepth.set ) MaxSolveDepth.value = Depth.value;
        if( SolverDivide.value<MinDepth.value )
        {
            fprintf( stderr , "[WARNING] %s must be at least as large as %s: %d>=%d\n" , SolverDivide.name , MinDepth.name , SolverDivide.value , MinDepth.value );
            SolverDivide.value = MinDepth.value;
        }
        if( IsoDivide.value<MinDepth.value )
        {
	        fprintf( stderr , "[WARNING] %s must be at least as large as %s: %d>=%d\n" , IsoDivide.name , MinDepth.name , IsoDivide.value , IsoDivide.value );
	        IsoDivide.value = MinDepth.value;
        }
	
        OctNode< TreeNodeData< true > , Real >::SetAllocator( MEMORY_ALLOCATOR_BLOCK_SIZE );

        t=Time();
        int kernelDepth = KernelDepth.set ?  KernelDepth.value : Depth.value-2;

        tree.setBSplineData( Depth.value , BoundaryType.value );
        //if( kernelDepth>Depth.value )
        //{
        //    fprintf( stderr,"[ERROR] %s can't be greater than %s: %d <= %d\n" , KernelDepth.name , Depth.name , KernelDepth.value , Depth.value );
        //    return EXIT_FAILURE;
        //}

        double maxMemoryUsage;
        t=Time() , tree.maxMemoryUsage=0;
        int pointCount = tree.setTree( In.value , Depth.value , MinDepth.value , kernelDepth , Real(SamplesPerNode.value) , Scale.value , Confidence.set , PointWeight.value , AdaptiveExponent.value , xForm );
        tree.ClipTree();
        tree.finalize( IsoDivide.value );

        /*DumpOutput2( comments[commentNum++] , "#             Tree set in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
        DumpOutput( "Input Points: %d\n" , pointCount );
        DumpOutput( "Leaves/Nodes: %d/%d\n" , tree.tree.leaves() , tree.tree.nodes() );
        DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage() )/(1<<20) );*/

        maxMemoryUsage = tree.maxMemoryUsage;
        t=Time() , tree.maxMemoryUsage=0;
        tree.SetLaplacianConstraints();
        /*DumpOutput2( comments[commentNum++] , "#      Constraints set in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
        DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage())/(1<<20) );*/
        maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );

        t=Time() , tree.maxMemoryUsage=0;
        tree.LaplacianMatrixIteration( SolverDivide.value, ShowResidual.set , MinIters.value , SolverAccuracy.value , MaxSolveDepth.value , FixedIters.value );
        /*DumpOutput2( comments[commentNum++] , "# Linear system solved in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
        DumpOutput( "Memory Usage: %.3f MB\n" , float( MemoryInfo::Usage() )/(1<<20) );*/
        maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );

        CoredFileMeshData< PlyValueVertex< Real > > mesh;

        if( Verbose.set ) tree.maxMemoryUsage=0;
        t=Time();
        isoValue = tree.GetIsoValue();
        //DumpOutput( "Got average in: %f\n" , Time()-t );
        //DumpOutput( "Iso-Value: %e\n" , isoValue );

        if( VoxelGrid.set )
        {
            double t = Time();
            FILE* fp = fopen( VoxelGrid.value , "wb" );
            if( !fp ) fprintf( stderr , "Failed to open voxel file for writing: %s\n" , VoxelGrid.value );
            else
            {
                int res;
                Pointer( Real ) values = tree.GetSolutionGrid( res , isoValue , VoxelDepth.value );
                fwrite( &res , sizeof(int) , 1 , fp );
                if( sizeof(Real)==sizeof(float) ) fwrite( values , sizeof(float) , res*res*res , fp );
                else
                {
                    float *fValues = new float[res*res*res];
                    for( int i=0 ; i<res*res*res ; i++ ) fValues[i] = float( values[i] );
                    fwrite( fValues , sizeof(float) , res*res*res , fp );
                    delete[] fValues;
                }
                fclose( fp );
                DeletePointer( values );
            }
            //DumpOutput( "Got voxel grid in: %f\n" , Time()-t );
        }

        if( Out.set )
        {
            t = Time() , tree.maxMemoryUsage = 0;
            tree.GetMCIsoTriangles( isoValue , IsoDivide.value , &mesh , 0 , 1 , !NonManifold.set , PolygonMesh.set );
            //if( PolygonMesh.set ) DumpOutput2( comments[commentNum++] , "#         Got polygons in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
            //else                  DumpOutput2( comments[commentNum++] , "#        Got triangles in: %9.1f (s), %9.1f (MB)\n" , Time()-t , tree.maxMemoryUsage );
            maxMemoryUsage = std::max< double >( maxMemoryUsage , tree.maxMemoryUsage );
            //DumpOutput2( comments[commentNum++],"#             Total Solve: %9.1f (s), %9.1f (MB)\n" , Time()-tt , maxMemoryUsage );

            if( NoComments.set )
            {
                if( ASCII.set ) PlyWritePolygons( Out.value , &mesh , PLY_ASCII         , NULL , 0 , iXForm );
                else            PlyWritePolygons( Out.value , &mesh , PLY_BINARY_NATIVE , NULL , 0 , iXForm );
            }
            else
            {
                if( ASCII.set ) PlyWritePolygons( Out.value , &mesh , PLY_ASCII         , comments , commentNum , iXForm );
                else            PlyWritePolygons( Out.value , &mesh , PLY_BINARY_NATIVE , comments , commentNum , iXForm );
            }
        }

        return NULL;
    }

    MagicDGP::Mesh3D* PoissonReconstruction::SurfaceTrimmer(int argc , char* argv[], const MagicDGP::Mesh3D* pMesh)
    {
        //cmdLineString In( "in" ) , Out( "out" );
        //cmdLineInt Smooth( "smooth" , 5 );
        //cmdLineFloat Trim( "trim" ) , IslandAreaRatio( "aRatio" , 0.001f );
        //cmdLineFloatArray< 2 > ColorRange( "color" );
        //cmdLineReadable PolygonMesh( "polygonMesh" );

        //cmdLineReadable* params[] =
        //{
        //    &In , &Out , &Trim , &PolygonMesh , &ColorRange , &Smooth , &IslandAreaRatio
        //};

        //int paramNum = sizeof(params)/sizeof(cmdLineReadable*);
        //cmdLineParse( argc-1 , &argv[1] , paramNum , params , 0 );

        //float min , max;
        //std::vector< PlyValueVertex< float > > vertices;
        //std::vector< std::vector< int > > polygons;

        //int ft , commentNum = paramNum+2;
        //char** comments;
        //bool readFlags[ PlyValueVertex< float >::Components ];
        //PlyReadPolygons( In.value , vertices , polygons , PlyValueVertex< float >::Properties , PlyValueVertex< float >::Components , ft , &comments , &commentNum , readFlags );
        ////if( !readFlags[3] ){ fprintf( stderr , "[ERROR] vertices do not have value flag\n" ) ; return EXIT_FAILURE; }

        //for( int i=0 ; i<Smooth.value ; i++ ) SmoothValues( vertices , polygons );

        //min = max = vertices[0].value;
        //for( size_t i=0 ; i<vertices.size() ; i++ ) min = std::min< float >( min , vertices[i].value ) , max = std::max< float >( max , vertices[i].value );
        //printf( "Value Range: [%f,%f]\n" , min , max );


        //if( Trim.set )
        //{
        //    hash_map< long long , int > vertexTable;
        //    std::vector< std::vector< int > > ltPolygons , gtPolygons;
        //    std::vector< bool > ltFlags , gtFlags;

        //    for( int i=0 ; i<paramNum+2 ; i++ ) comments[i+commentNum]=new char[1024];
        //    sprintf( comments[commentNum++] , "Running Surface Trimmer (V5)" );
        //    if(              In.set ) sprintf(comments[commentNum++],"\t--%s %s" , In.name , In.value );
        //    if(             Out.set ) sprintf(comments[commentNum++],"\t--%s %s" , Out.name , Out.value );
        //    if(            Trim.set ) sprintf(comments[commentNum++],"\t--%s %f" , Trim.name , Trim.value );
        //    if(          Smooth.set ) sprintf(comments[commentNum++],"\t--%s %d" , Smooth.name , Smooth.value );
        //    if( IslandAreaRatio.set ) sprintf(comments[commentNum++],"\t--%s %f" , IslandAreaRatio.name , IslandAreaRatio.value );
        //    if(     PolygonMesh.set ) sprintf(comments[commentNum++],"\t--%s" , PolygonMesh.name );

        //    double t=Time();
        //    for( size_t i=0 ; i<polygons.size() ; i++ ) SplitPolygon( polygons[i] , vertices , &ltPolygons , &gtPolygons , &ltFlags , &gtFlags , vertexTable , Trim.value );
        //    if( IslandAreaRatio.value>0 )
        //    {
        //        std::vector< std::vector< int > > _ltPolygons , _gtPolygons;
        //        std::vector< std::vector< int > > ltComponents , gtComponents;
        //        SetConnectedComponents( ltPolygons , ltComponents );
        //        SetConnectedComponents( gtPolygons , gtComponents );
        //        std::vector< double > ltAreas( ltComponents.size() , 0. ) , gtAreas( gtComponents.size() , 0. );
        //        std::vector< bool > ltComponentFlags( ltComponents.size() , false ) , gtComponentFlags( gtComponents.size() , false );
        //        double area = 0.;
        //        for( size_t i=0 ; i<ltComponents.size() ; i++ )
        //        {
        //            for( size_t j=0 ; j<ltComponents[i].size() ; j++ )
        //            {
        //                ltAreas[i] += PolygonArea( vertices , ltPolygons[ ltComponents[i][j] ] );
        //                ltComponentFlags[i] = ( ltComponentFlags[i] || ltFlags[ ltComponents[i][j] ] );
        //            }
        //            area += ltAreas[i];
        //        }
        //        for( size_t i=0 ; i<gtComponents.size() ; i++ )
        //        {
        //            for( size_t j=0 ; j<gtComponents[i].size() ; j++ )
        //            {
        //                gtAreas[i] += PolygonArea( vertices , gtPolygons[ gtComponents[i][j] ] );
        //                gtComponentFlags[i] = ( gtComponentFlags[i] || gtFlags[ gtComponents[i][j] ] );
        //            }
        //            area += gtAreas[i];
        //        }
        //        for( size_t i=0 ; i<ltComponents.size() ; i++ )
        //        {
        //            if( ltAreas[i]<area*IslandAreaRatio.value && ltComponentFlags[i] ) for( size_t j=0 ; j<ltComponents[i].size() ; j++ ) _gtPolygons.push_back( ltPolygons[ ltComponents[i][j] ] );
        //            else                                                               for( size_t j=0 ; j<ltComponents[i].size() ; j++ ) _ltPolygons.push_back( ltPolygons[ ltComponents[i][j] ] );
        //        }
        //        for( size_t i=0 ; i<gtComponents.size() ; i++ )
        //        {
        //            if( gtAreas[i]<area*IslandAreaRatio.value && gtComponentFlags[i] ) for( size_t j=0 ; j<gtComponents[i].size() ; j++ ) _ltPolygons.push_back( gtPolygons[ gtComponents[i][j] ] );
        //            else                                                               for( size_t j=0 ; j<gtComponents[i].size() ; j++ ) _gtPolygons.push_back( gtPolygons[ gtComponents[i][j] ] );
        //        }
        //        ltPolygons = _ltPolygons , gtPolygons = _gtPolygons;
        //    }
        //    if( !PolygonMesh.set )
        //    {
        //        {
        //            std::vector< std::vector< int > > polys = ltPolygons;
        //            Triangulate( vertices , ltPolygons , polys ) , ltPolygons = polys;
        //        }
        //        {
        //            std::vector< std::vector< int > > polys = gtPolygons;
        //            Triangulate( vertices , gtPolygons , polys ) , gtPolygons = polys;
        //        }
        //    }

        //    RemoveHangingVertices( vertices , gtPolygons );
        //    sprintf( comments[commentNum++] , "#Trimmed In: %9.1f (s)" , Time()-t );
        //    if( Out.set ) PlyWritePolygons( Out.value , vertices , gtPolygons , PlyValueVertex< float >::Properties , PlyValueVertex< float >::Components , ft , comments , commentNum );
        //}
        //else
        //{
        //    if( ColorRange.set ) min = ColorRange.values[0] , max = ColorRange.values[1];
        //    std::vector< PlyColorVertex< float > > outVertices;
        //    ColorVertices( vertices , outVertices , min , max );
        //    if( Out.set ) PlyWritePolygons( Out.value , outVertices , polygons , PlyColorVertex< float >::Properties , PlyColorVertex< float >::Components , ft , comments , commentNum );
        //}

        return NULL;
    }

    //void PoissonReconstruction::SetConnectedComponents( const std::vector< std::vector< int > >& polygons , std::vector< std::vector< int > >& components )
    //{

    //}

}