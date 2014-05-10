//#include "StdAfx.h"
#include "Parser.h"
#include <fstream>
#include <istream>
#include <ostream>
#include <vector>
#include <set>
#include <stdio.h>
#include "../Common/ToolKit.h"
#include "../Tool/LogSystem.h"

namespace MagicDGP
{
    Parser::Parser()
    {
    }

    Parser::~Parser()
    {
    }

    Point3DSet* Parser::ParsePointSet(std::string fileName)
    {
        size_t dotPos = fileName.rfind('.');
        if (dotPos == std::string::npos)
        {
            return NULL;
        }
        std::string extName = fileName.substr(dotPos + 1);
        if (extName == std::string("obj"))
        {
            return ParsePointSetByOBJ(fileName);
        }
        else if (extName == std::string("stl"))
        {
            return ParsePointSetBySTL(fileName);
        }
        else if (extName == std::string("off"))
        {
            return ParsePointSetByOFF(fileName);
        }
        else
        {
            return NULL;
        }
        
    }

    Point3DSet* Parser::ParsePointSetByOBJ(std::string fileName)
    {
        std::ifstream fin(fileName);
        std::vector<MagicMath::Vector3> posList;
        std::vector<MagicMath::Vector3> norList;
        const int maxSize = 512;
        char pLine[maxSize];
        while (fin.getline(pLine, maxSize))
        {
            if (pLine[0] == 'v')
            {
                if (pLine[1] == ' ' )
                {
                    char* tok = strtok(pLine, " ");
                    MagicMath::Vector3 pos;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        pos[i] = (double)atof(tok);
                    }
                    posList.push_back(pos);
                }
                else if (pLine[1] == 'n')
                {
                    char* tok = strtok(pLine, " ");
                    MagicMath::Vector3 nor;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        nor[i] = (double)atof(tok);
                    }
                    norList.push_back(nor);
                }
            }
        }
        fin.close();
        Point3DSet* pPSet = new Point3DSet;
        InfoLog << "Vertex number: " << posList.size() << " normal number: " << norList.size() << std::endl;
        if (norList.size() > 0)
        {
            for (int i = 0; i < posList.size(); i++)
            {
                Point3D* point = new Point3D(posList.at(i), norList.at(i), i);
                pPSet->InsertPoint(point);
            }
            pPSet->SetHasNormal(true);
        }
        else
        {
            for (int i = 0; i < posList.size(); i++)
            {
                Point3D* point = new Point3D(posList.at(i), i);
                pPSet->InsertPoint(point);
            }
        }
        return pPSet;
    }

    Point3DSet* Parser::ParsePointSetBySTL(std::string fileName)
    {
        //Judge whether it is a binary format.
        //If it is binary there are 80 char of comment, the number fn of faces and then exactly fn*4*3 bytes.
        DebugLog << "ParsePointSetBySTL file name: " << fileName.c_str() << std::endl;
        int stlLabelSize = 80;
        bool isBinary=false;
        FILE *fp = fopen(fileName.c_str(), "r");
        //Find size of file
        fseek(fp, 0, SEEK_END);
        int file_size = ftell(fp);
        int facenum;
        /* Check for binary or ASCII file */
        fseek(fp, stlLabelSize, SEEK_SET);
        fread(&facenum, sizeof(int), 1, fp);
        int expected_file_size = stlLabelSize + 4 + (sizeof(short)+sizeof(float) * 4) * facenum ;
        if(file_size ==  expected_file_size) isBinary = true;
        unsigned char tmpbuf[128];
        fread(tmpbuf,sizeof(tmpbuf),1,fp);
        for(unsigned int i = 0; i < sizeof(tmpbuf); i++)
        {
            if(tmpbuf[i] > 127)
            {
                isBinary=true;
                break;
            }
        }
        // Now we know if the stl file is ascii or binary.
        fclose(fp);
        //
        Point3DSet* pPointSet = NULL;
        if (isBinary)
        {
            FILE *fp = fopen(fileName.c_str(), "rb");
            if (fp == NULL)
            {
                DebugLog << "ParseMesh3dBySTL:: open failed in binary" << std::endl;
                return NULL;
            }
            int faceNum;
            fseek(fp, stlLabelSize, SEEK_SET);
            fread(&faceNum, sizeof(int), 1, fp);
            pPointSet = new Point3DSet;
            std::set<MagicMath::Vector3> vertPosSet;
            for (int fid = 0; fid < faceNum; fid++)
            {
                unsigned short attr;
                float xx, yy, zz;
                //read face normal
                fread(&xx, sizeof(xx), 1, fp);
                fread(&yy, sizeof(yy), 1, fp);
                fread(&zz, sizeof(zz), 1, fp);
                //read face position
                std::vector<Vertex3D* > vertList(3);
                for (int vid = 0; vid < 3; vid++)
                {
                    fread(&xx, sizeof(xx), 1, fp);
                    fread(&yy, sizeof(yy), 1, fp);
                    fread(&zz, sizeof(zz), 1, fp);
                    MagicMath::Vector3 pos(xx, yy, zz);
                    std::pair<std::set<MagicMath::Vector3>::iterator, bool> res = vertPosSet.insert( pos );
                    if (res.second)
                    {
                        Point3D* point = new Point3D(pos);
                        pPointSet->InsertPoint(point);
                    }
                }
                fread(&attr,sizeof(unsigned short),1,fp);
            }
            fclose(fp);
        }
        else
        {
            FILE *fp = fopen(fileName.c_str(), "r");
            if (fp == NULL)
            {
                DebugLog << "ParseMesh3dBySTL:: open failed in asic" << std::endl;
                return NULL;
            }
            //skip the first line of the file
            while (getc(fp) != '\n') 
            {//Skip the first line of the file 
            }
            //read file
            pPointSet = new Point3DSet;
            std::set<MagicMath::Vector3> vertPosSet;
            int currentVertId = 0;
            while (!feof(fp))
            {
                MagicMath::Vector3 faceNorm;
                float xx, yy, zz;
                int ret = fscanf(fp, "%*s %*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    continue;
                }
                ret = fscanf(fp, "%*s %*s");
                ret = fscanf(fp, "%*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    ErrorLog << "error: parse position error" << std::endl;
                    return pPointSet;
                }
                MagicMath::Vector3 pos0(xx, yy, zz);
                ret = fscanf(fp, "%*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    ErrorLog << "error: parse position error" << std::endl;
                    return pPointSet;
                }
                MagicMath::Vector3 pos1(xx, yy, zz);
                ret = fscanf(fp, "%*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    ErrorLog << "error: parse position error" << std::endl;
                    return pPointSet;
                }
                MagicMath::Vector3 pos2(xx, yy, zz);

                std::pair<std::set<MagicMath::Vector3>::iterator, bool> res = vertPosSet.insert( pos0 );
                if (res.second)
                {
                    Point3D* point = new Point3D(pos0);
                    pPointSet->InsertPoint(point);
                }
                res = vertPosSet.insert( pos1 );
                if (res.second)
                {
                    Point3D* point = new Point3D(pos1);
                    pPointSet->InsertPoint(point);
                }
                res = vertPosSet.insert( pos2 );
                if (res.second)
                {
                    Point3D* point = new Point3D(pos2);
                    pPointSet->InsertPoint(point);
                }
                ret = fscanf(fp, "%*s");
                ret = fscanf(fp, "%*s");
                if (feof(fp))
                {
                    break;
                }
            }
            fclose(fp);
        }
        InfoLog << "Import Point Number: " << pPointSet->GetPointNumber() << std::endl;
        return pPointSet;
    }

    Point3DSet* Parser::ParsePointSetByOFF(std::string fileName)
    {
        DebugLog << "ParsePointSetByOFF file name: " << fileName.c_str() << std::endl;
        std::ifstream fin(fileName);
        const int maxSize = 512;
        char pLine[maxSize];
        fin.getline(pLine, maxSize);
        int pointNum;
        fin >> pointNum;
        fin.getline(pLine, maxSize);
        Point3DSet* pPointSet = new Point3DSet;
        for (int vid = 0; vid < pointNum; vid++)
        {
            MagicMath::Vector3 pos;
            fin.getline(pLine, maxSize);
            char* tok = strtok(pLine, " ");
            pos[0] = (double)atof(tok);
            tok = strtok(NULL, " ");
            pos[1] = (double)atof(tok);
            tok = strtok(NULL, " ");
            pos[2] = (double)atof(tok);
            Point3D* point = new Point3D(pos);
            pPointSet->InsertPoint(point);
        }
        InfoLog << "Import Point Number: " << pPointSet->GetPointNumber() << std::endl;

        return pPointSet;
    }

    Mesh3D* Parser::ParseMesh3D(std::string fileName)
    {
        size_t dotPos = fileName.rfind('.');
        if (dotPos == std::string::npos)
        {
            return NULL;
        }
        std::string extName = fileName.substr(dotPos + 1);
        if (extName == std::string("obj"))
        {
            return ParseMesh3DByOBJ(fileName);
        }
        else if (extName == std::string("stl"))
        {
            return ParseMesh3dBySTL(fileName);
        }
        else if (extName == std::string("off"))
        {
            return ParseMesh3dByOFF(fileName);
        }
        else
        {
            return NULL;
        }
    }

    LightMesh3D* Parser::ParseLightMesh3D(std::string fileName)
    {
        size_t dotPos = fileName.rfind('.');
        if (dotPos == std::string::npos)
        {
            return NULL;
        }
        std::string extName = fileName.substr(dotPos + 1);
        if (extName == std::string("obj"))
        {
            return ParseLightMesh3DByOBJ(fileName);
        }
        else if (extName == std::string("stl"))
        {
            return ParseLightMesh3dBySTL(fileName);
        }
        else if (extName == std::string("off"))
        {
            return ParseLightMesh3dByOFF(fileName);
        }
        else
        {
            return NULL;
        }
    }

    Mesh3D* Parser::ParseMesh3DByOBJ(std::string fileName)
    {
        //float timeStart = MagicCore::ToolKit::GetTime();
        DebugLog << "ParseMesh3DByOBJ file name: " << fileName.c_str() << std::endl;
        std::ifstream fin(fileName);
        const int maxSize = 512;
        char pLine[maxSize];
        Mesh3D* pMesh = new Mesh3D;
        std::vector<MagicMath::Vector3> normalList;
        std::vector<MagicMath::Vector3> texcordList;
        while (fin.getline(pLine, maxSize))
        {
            if (pLine[0] == 'v')
            {
                if (pLine[1] == ' ' )
                {
                    char* tok = strtok(pLine, " ");
                    MagicMath::Vector3 pos;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        pos[i] = (double)atof(tok);
                    }
                    pMesh->InsertVertex(pos);
                }
                else if (pLine[1] == 'n')
                {
                    char* tok = strtok(pLine, " ");
                    MagicMath::Vector3 nor;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        nor[i] = (double)atof(tok);
                    }
                    normalList.push_back(nor);
                }
                else if (pLine[1] == 't')
                {
                    char* tok = strtok(pLine, " ");
                    MagicMath::Vector3 tex;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        tex[i] = (double)atof(tok);
                    }
                    texcordList.push_back(tex);
                }
            }
            else if (pLine[0] == 'f' && pLine[1] == ' ')
            {
                std::vector<Vertex3D* > vertList(3);
                char* tok = strtok(pLine, " ");
                for (int i = 0; i < 3; i++)
                {
                    tok = strtok(NULL, " ");
                    char temp[100];
                    strcpy(temp, tok);
                    temp[strcspn(temp, "/")] = 0;
                    int id = (int)strtol(temp, NULL, 10) - 1;
                    vertList.at(i) = pMesh->GetVertex(id);
                }
                pMesh->InsertFace(vertList);
            }
        }
        int vertNum = pMesh->GetVertexNumber();
        InfoLog << "Import Vertex Number: " << vertNum << " Face Number: " << pMesh->GetFaceNumber() << std::endl;
//        DebugLog << "Parse obj file time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
        if (texcordList.size() == vertNum)
        {
            for (int i = 0; i < vertNum; i++)
            {
                pMesh->GetVertex(i)->SetTexCord(texcordList.at(i));
            }
        }
        if (normalList.size() == vertNum)
        {
            for (int i = 0; i < vertNum; i++)
            {
                pMesh->GetVertex(i)->SetNormal(normalList.at(i));
            }
        }
        //DebugLog << "Parse total time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
        return pMesh;
    }

    Mesh3D* Parser::ParseMesh3dBySTL(std::string fileName)
    {
        //Judge whether it is a binary format.
        //If it is binary there are 80 char of comment, the number fn of faces and then exactly fn*4*3 bytes.
        DebugLog << "ParseMesh3dBySTL file name: " << fileName.c_str() << std::endl;
        int stlLabelSize = 80;
        bool isBinary=false;
        FILE *fp = fopen(fileName.c_str(), "r");
        //Find size of file
        fseek(fp, 0, SEEK_END);
        int file_size = ftell(fp);
        int facenum;
        /* Check for binary or ASCII file */
        fseek(fp, stlLabelSize, SEEK_SET);
        fread(&facenum, sizeof(int), 1, fp);
        int expected_file_size = stlLabelSize + 4 + (sizeof(short)+sizeof(float) * 4) * facenum ;
        if(file_size ==  expected_file_size) isBinary = true;
        unsigned char tmpbuf[128];
        fread(tmpbuf,sizeof(tmpbuf),1,fp);
        for(unsigned int i = 0; i < sizeof(tmpbuf); i++)
        {
            if(tmpbuf[i] > 127)
            {
                isBinary=true;
                break;
            }
        }
        // Now we know if the stl file is ascii or binary.
        fclose(fp);
        //
        Mesh3D* pMesh = NULL;
        if (isBinary)
        {
            FILE *fp = fopen(fileName.c_str(), "rb");
            if (fp == NULL)
            {
                DebugLog << "ParseMesh3dBySTL:: open failed in binary" << std::endl;
                return NULL;
            }
            int faceNum;
            fseek(fp, stlLabelSize, SEEK_SET);
            fread(&faceNum, sizeof(int), 1, fp);
            pMesh = new Mesh3D;
            std::map<MagicMath::Vector3, int> vertPosMap;
            int currentVertId = 0;
            for (int fid = 0; fid < faceNum; fid++)
            {
                unsigned short attr;
                float xx, yy, zz;
                //read face normal
                fread(&xx, sizeof(xx), 1, fp);
                fread(&yy, sizeof(yy), 1, fp);
                fread(&zz, sizeof(zz), 1, fp);
                //read face position
                MagicMath::Vector3 pos[3];
                std::set<MagicMath::Vector3> validVertex;
                for (int vid = 0; vid < 3; vid++)
                {
                    fread(&xx, sizeof(xx), 1, fp);
                    fread(&yy, sizeof(yy), 1, fp);
                    fread(&zz, sizeof(zz), 1, fp);
                    pos[vid] = MagicMath::Vector3(xx, yy, zz);
                    validVertex.insert(pos[vid]);
                }
                if (validVertex.size() != 3)
                {
                    InfoLog << "Degenerate triangle detect" << std::endl;
                }
                else
                {
                    std::vector<Vertex3D* > vertList(3);
                    for (int vid = 0; vid < 3; vid++)
                    {
                        /*fread(&xx, sizeof(xx), 1, fp);
                        fread(&yy, sizeof(yy), 1, fp);
                        fread(&zz, sizeof(zz), 1, fp);
                        MagicMath::Vector3 pos(xx, yy, zz);*/
                        std::pair<std::map<MagicMath::Vector3, int>::iterator, bool> mapRes 
                            = vertPosMap.insert( std::pair<MagicMath::Vector3, int>(pos[vid], currentVertId) );
                        if (mapRes.second)
                        {
                            vertList.at(vid) = pMesh->InsertVertex(pos[vid]);
                            currentVertId++;
                        }
                        else
                        {
                            vertList.at(vid) = pMesh->GetVertex(vertPosMap[pos[vid]]);
                        }
                    }
                    pMesh->InsertFace(vertList);
                }
                
                fread(&attr,sizeof(unsigned short),1,fp);
            }
            fclose(fp);
        }
        else
        {
            FILE *fp = fopen(fileName.c_str(), "r");
            if (fp == NULL)
            {
                DebugLog << "ParseMesh3dBySTL:: open failed in asic" << std::endl;
                return NULL;
            }
            //skip the first line of the file
            while (getc(fp) != '\n') 
            {//Skip the first line of the file 
            }
            //read file
            pMesh = new Mesh3D;
            std::map<MagicMath::Vector3, int> vertPosMap;
            int currentVertId = 0;
            while (!feof(fp))
            {
                MagicMath::Vector3 faceNorm;
                float xx, yy, zz;
                int ret = fscanf(fp, "%*s %*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    continue;
                }
                ret = fscanf(fp, "%*s %*s");
                ret = fscanf(fp, "%*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    ErrorLog << "error: parse position error" << std::endl;
                    return pMesh;
                }
                MagicMath::Vector3 pos0(xx, yy, zz);
                ret = fscanf(fp, "%*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    ErrorLog << "error: parse position error" << std::endl;
                    return pMesh;
                }
                MagicMath::Vector3 pos1(xx, yy, zz);
                ret = fscanf(fp, "%*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    ErrorLog << "error: parse position error" << std::endl;
                    return pMesh;
                }
                MagicMath::Vector3 pos2(xx, yy, zz);
                //judge whether it is a valid face.
                std::set<MagicMath::Vector3> validVertex;
                validVertex.insert(pos0);
                validVertex.insert(pos1);
                validVertex.insert(pos2);
                if (validVertex.size() != 3)
                {
                    InfoLog << "Degenerate triangle detect" << std::endl;
                }//
                else
                {
                    std::vector<Vertex3D* > vertList(3);
                    std::pair<std::map<MagicMath::Vector3, int>::iterator, bool> mapRes = vertPosMap.insert( std::pair<MagicMath::Vector3, int>(pos0, currentVertId) );
                    if (mapRes.second)
                    {
                        vertList.at(0) = pMesh->InsertVertex(pos0);
                        currentVertId++;
                    }
                    else
                    {
                        vertList.at(0) = pMesh->GetVertex(vertPosMap[pos0]);
                    }
                    mapRes = vertPosMap.insert( std::pair<MagicMath::Vector3, int>(pos1, currentVertId) );
                    if (mapRes.second)
                    {
                        vertList.at(1) = pMesh->InsertVertex(pos1);
                        currentVertId++;
                    }
                    else
                    {
                        vertList.at(1) = pMesh->GetVertex(vertPosMap[pos1]);
                    }
                    mapRes = vertPosMap.insert( std::pair<MagicMath::Vector3, int>(pos2, currentVertId) );
                    if (mapRes.second)
                    {
                        vertList.at(2) = pMesh->InsertVertex(pos2);
                        currentVertId++;
                    }
                    else
                    {
                        vertList.at(2) = pMesh->GetVertex(vertPosMap[pos2]);
                    }
                    pMesh->InsertFace(vertList);
                }   
                ret = fscanf(fp, "%*s");
                ret = fscanf(fp, "%*s");
                if (feof(fp))
                {
                    break;
                }
            }
            fclose(fp);
        }
        InfoLog << "Import Vertex Number: " << pMesh->GetVertexNumber() << " Face Number: " << pMesh->GetFaceNumber() << std::endl;
        return pMesh;
    }

    Mesh3D* Parser::ParseMesh3dByOFF(std::string fileName)
    {
        DebugLog << "ParseMesh3dByOFF file name: " << fileName.c_str() << std::endl;
        std::ifstream fin(fileName);
        const int maxSize = 512;
        char pLine[maxSize];
        fin.getline(pLine, maxSize);
        int vertNum, faceNum;
        fin >> vertNum >> faceNum;
        fin.getline(pLine, maxSize);
        Mesh3D* pMesh = new Mesh3D;
        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicMath::Vector3 pos;
            fin.getline(pLine, maxSize);
            char* tok = strtok(pLine, " ");
            pos[0] = (double)atof(tok);
            tok = strtok(NULL, " ");
            pos[1] = (double)atof(tok);
            tok = strtok(NULL, " ");
            pos[2] = (double)atof(tok);
            pMesh->InsertVertex(pos);
        }
        for (int fid = 0; fid < faceNum; fid++)
        {
            std::vector<Vertex3D* > vertList(3);
            fin.getline(pLine, maxSize);
            int kk;
            char* tok = strtok(pLine, " ");
            for (int j = 0; j < 3; j++)
            {
                tok = strtok(NULL, " ");
                kk = atoi(tok);
                vertList.at(j) = pMesh->GetVertex(kk);
            }
            pMesh->InsertFace(vertList);
        }
        InfoLog << "Import Vertex Number: " << vertNum << " Face Number: " << pMesh->GetFaceNumber() << std::endl;

        return pMesh;
    }

    LightMesh3D* Parser::ParseLightMesh3DByOBJ(std::string fileName)
    {
        //float timeStart = MagicCore::ToolKit::GetTime();
        DebugLog << "ParseMesh3DByOBJ file name: " << fileName.c_str() << std::endl;
        std::ifstream fin(fileName);
        const int maxSize = 512;
        char pLine[maxSize];
        LightMesh3D* pMesh = new LightMesh3D;
        std::vector<MagicMath::Vector3> normalList;
        std::vector<MagicMath::Vector3> texcordList;
        while (fin.getline(pLine, maxSize))
        {
            if (pLine[0] == 'v')
            {
                if (pLine[1] == ' ' )
                {
                    char* tok = strtok(pLine, " ");
                    MagicMath::Vector3 pos;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        pos[i] = (double)atof(tok);
                    }
                    pMesh->InsertVertex(pos);
                }
                else if (pLine[1] == 'n')
                {
                    char* tok = strtok(pLine, " ");
                    MagicMath::Vector3 nor;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        nor[i] = (double)atof(tok);
                    }
                    normalList.push_back(nor);
                }
                else if (pLine[1] == 't')
                {
                    char* tok = strtok(pLine, " ");
                    MagicMath::Vector3 tex;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        tex[i] = (double)atof(tok);
                    }
                    texcordList.push_back(tex);
                }
            }
            else if (pLine[0] == 'f' && pLine[1] == ' ')
            {
                FaceIndex faceIdx;
                char* tok = strtok(pLine, " ");
                for (int i = 0; i < 3; i++)
                {
                    tok = strtok(NULL, " ");
                    char temp[100];
                    strcpy(temp, tok);
                    temp[strcspn(temp, "/")] = 0;
                    faceIdx.mIndex[i] = (int)strtol(temp, NULL, 10) - 1;
                }
                pMesh->InsertFace(faceIdx);
            }
        }
        int vertNum = pMesh->GetVertexNumber();
        InfoLog << "Import Vertex Number: " << vertNum << " Face Number: " << pMesh->GetFaceNumber() << std::endl;
//        DebugLog << "Parse obj file time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
        if (texcordList.size() == vertNum)
        {
            for (int i = 0; i < vertNum; i++)
            {
                pMesh->GetVertex(i)->SetTexCord(texcordList.at(i));
            }
        }
        if (normalList.size() == vertNum)
        {
            for (int i = 0; i < vertNum; i++)
            {
                pMesh->GetVertex(i)->SetNormal(normalList.at(i));
            }
        }
        //DebugLog << "Parse total time: " << MagicCore::ToolKit::GetTime() - timeStart << std::endl;
        return pMesh;
    }

    LightMesh3D* Parser::ParseLightMesh3dBySTL(std::string fileName)
    {
        //Judge whether it is a binary format.
        //If it is binary there are 80 char of comment, the number fn of faces and then exactly fn*4*3 bytes.
        DebugLog << "ParseLightMesh3dBySTL file name: " << fileName.c_str() << std::endl;
        int stlLabelSize = 80;
        bool isBinary=false;
        FILE *fp = fopen(fileName.c_str(), "r");
        //Find size of file
        fseek(fp, 0, SEEK_END);
        int file_size = ftell(fp);
        int facenum;
        /* Check for binary or ASCII file */
        fseek(fp, stlLabelSize, SEEK_SET);
        fread(&facenum, sizeof(int), 1, fp);
        int expected_file_size = stlLabelSize + 4 + (sizeof(short)+sizeof(float) * 4) * facenum ;
        if(file_size ==  expected_file_size) isBinary = true;
        unsigned char tmpbuf[128];
        fread(tmpbuf,sizeof(tmpbuf),1,fp);
        for(unsigned int i = 0; i < sizeof(tmpbuf); i++)
        {
            if(tmpbuf[i] > 127)
            {
                isBinary=true;
                break;
            }
        }
        // Now we know if the stl file is ascii or binary.
        fclose(fp);
        //
        LightMesh3D* pMesh = NULL;
        if (isBinary)
        {
            FILE *fp = fopen(fileName.c_str(), "rb");
            if (fp == NULL)
            {
                DebugLog << "ParseMesh3dBySTL:: open failed in binary" << std::endl;
                return NULL;
            }
            int faceNum;
            fseek(fp, stlLabelSize, SEEK_SET);
            fread(&faceNum, sizeof(int), 1, fp);
            pMesh = new LightMesh3D;
            std::map<MagicMath::Vector3, int> vertPosMap;
            int currentVertId = 0;
            for (int fid = 0; fid < faceNum; fid++)
            {
                unsigned short attr;
                float xx, yy, zz;
                //read face normal
                fread(&xx, sizeof(xx), 1, fp);
                fread(&yy, sizeof(yy), 1, fp);
                fread(&zz, sizeof(zz), 1, fp);
                //read face position
                MagicMath::Vector3 pos[3];
                std::set<MagicMath::Vector3> validVertex;
                for (int vid = 0; vid < 3; vid++)
                {
                    fread(&xx, sizeof(xx), 1, fp);
                    fread(&yy, sizeof(yy), 1, fp);
                    fread(&zz, sizeof(zz), 1, fp);
                    pos[vid] = MagicMath::Vector3(xx, yy, zz);
                    validVertex.insert(pos[vid]);
                }
                if (validVertex.size() != 3)
                {
                    InfoLog << "Degenerate triangle detect" << std::endl;
                }
                else
                {
                    FaceIndex faceIdx;
                    for (int vid = 0; vid < 3; vid++)
                    {
                        std::pair<std::map<MagicMath::Vector3, int>::iterator, bool> mapRes 
                            = vertPosMap.insert( std::pair<MagicMath::Vector3, int>(pos[vid], currentVertId) );
                        if (mapRes.second)
                        {
                            Vertex3D* pNewVert = pMesh->InsertVertex(pos[vid]);
                            faceIdx.mIndex[vid] = pNewVert->GetId();
                            currentVertId++;
                        }
                        else
                        {
                            faceIdx.mIndex[vid] = vertPosMap[pos[vid]];
                        }
                    }
                    pMesh->InsertFace(faceIdx);
                }
                
                fread(&attr,sizeof(unsigned short),1,fp);
            }
            fclose(fp);
        }
        else
        {
            FILE *fp = fopen(fileName.c_str(), "r");
            if (fp == NULL)
            {
                DebugLog << "ParseMesh3dBySTL:: open failed in asic" << std::endl;
                return NULL;
            }
            //skip the first line of the file
            while (getc(fp) != '\n') 
            {//Skip the first line of the file 
            }
            //read file
            pMesh = new LightMesh3D;
            std::map<MagicMath::Vector3, int> vertPosMap;
            int currentVertId = 0;
            while (!feof(fp))
            {
                MagicMath::Vector3 faceNorm;
                float xx, yy, zz;
                int ret = fscanf(fp, "%*s %*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    continue;
                }
                ret = fscanf(fp, "%*s %*s");
                ret = fscanf(fp, "%*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    ErrorLog << "error: parse position error" << std::endl;
                    return pMesh;
                }
                MagicMath::Vector3 pos0(xx, yy, zz);
                ret = fscanf(fp, "%*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    ErrorLog << "error: parse position error" << std::endl;
                    return pMesh;
                }
                MagicMath::Vector3 pos1(xx, yy, zz);
                ret = fscanf(fp, "%*s %f %f %f\n", &xx, &yy, &zz);
                if (ret != 3)
                {
                    ErrorLog << "error: parse position error" << std::endl;
                    return pMesh;
                }
                MagicMath::Vector3 pos2(xx, yy, zz);
                //judge whether it is a valid face.
                std::set<MagicMath::Vector3> validVertex;
                validVertex.insert(pos0);
                validVertex.insert(pos1);
                validVertex.insert(pos2);
                if (validVertex.size() != 3)
                {
                    InfoLog << "Degenerate triangle detect" << std::endl;
                }//
                else
                {
                    FaceIndex faceIdx;
                    //std::vector<Vertex3D* > vertList(3);
                    std::pair<std::map<MagicMath::Vector3, int>::iterator, bool> mapRes = vertPosMap.insert( std::pair<MagicMath::Vector3, int>(pos0, currentVertId) );
                    if (mapRes.second)
                    {
                        Vertex3D* pNewVert = pMesh->InsertVertex(pos0);
                        faceIdx.mIndex[0] = pNewVert->GetId();
                        currentVertId++;
                    }
                    else
                    {
                        faceIdx.mIndex[0] = vertPosMap[pos0];
                    }
                    mapRes = vertPosMap.insert( std::pair<MagicMath::Vector3, int>(pos1, currentVertId) );
                    if (mapRes.second)
                    {
                        Vertex3D* pNewVert = pMesh->InsertVertex(pos1);
                        faceIdx.mIndex[1] = pNewVert->GetId();
                        currentVertId++;
                    }
                    else
                    {
                        faceIdx.mIndex[1] = vertPosMap[pos1];
                    }
                    mapRes = vertPosMap.insert( std::pair<MagicMath::Vector3, int>(pos2, currentVertId) );
                    if (mapRes.second)
                    {
                        Vertex3D* pNewVert = pMesh->InsertVertex(pos2);
                        faceIdx.mIndex[2] = pNewVert->GetId();
                        currentVertId++;
                    }
                    else
                    {
                        faceIdx.mIndex[2] = vertPosMap[pos2];
                    }
                    pMesh->InsertFace(faceIdx);
                }   
                ret = fscanf(fp, "%*s");
                ret = fscanf(fp, "%*s");
                if (feof(fp))
                {
                    break;
                }
            }
            fclose(fp);
        }
        InfoLog << "Import Vertex Number: " << pMesh->GetVertexNumber() << " Face Number: " << pMesh->GetFaceNumber() << std::endl;
        return pMesh;
    }

    LightMesh3D* Parser::ParseLightMesh3dByOFF(std::string fileName)
    {
        DebugLog << "ParseLightMesh3dByOFF file name: " << fileName.c_str() << std::endl;
        std::ifstream fin(fileName);
        const int maxSize = 512;
        char pLine[maxSize];
        fin.getline(pLine, maxSize);
        int vertNum, faceNum;
        fin >> vertNum >> faceNum;
        fin.getline(pLine, maxSize);
        LightMesh3D* pMesh = new LightMesh3D;
        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicMath::Vector3 pos;
            fin.getline(pLine, maxSize);
            char* tok = strtok(pLine, " ");
            pos[0] = (double)atof(tok);
            tok = strtok(NULL, " ");
            pos[1] = (double)atof(tok);
            tok = strtok(NULL, " ");
            pos[2] = (double)atof(tok);
            pMesh->InsertVertex(pos);
        }
        for (int fid = 0; fid < faceNum; fid++)
        {
            FaceIndex faceIdx;
            //std::vector<Vertex3D* > vertList(3);
            fin.getline(pLine, maxSize);
            char* tok = strtok(pLine, " ");
            for (int j = 0; j < 3; j++)
            {
                tok = strtok(NULL, " ");
                faceIdx.mIndex[j] = atoi(tok);
            }
            pMesh->InsertFace(faceIdx);
        }
        InfoLog << "Import Vertex Number: " << vertNum << " Face Number: " << pMesh->GetFaceNumber() << std::endl;

        return pMesh;
    }

    void Parser::ExportPointSet(std::string fileName, const Point3DSet* pPC)
    {
        size_t dotPos = fileName.rfind('.');
        if (dotPos != std::string::npos)
        {
            std::string extName = fileName.substr(dotPos + 1);
            if (extName == std::string("obj"))
            {
                ExportPointSetByOBJ(fileName, pPC);
            }
            else if (extName == std::string("ply"))
            {
                ExportPointSetByPLY(fileName, pPC);
            }
            else if (extName == std::string("off"))
            {
                ExportPointSetByOFF(fileName, pPC);
            }
            else
            {
                DebugLog << "Export point set failed: file name extension error!" << std::endl;
            }
        }
        else
        {
            DebugLog << "Export point set failed: file name error!" << std::endl;
        }
    }

    void Parser::ExportPointSetByOBJ(std::string fileName, const Point3DSet* pPC)
    {
        DebugLog << "Parser::ExportPointSetByOBJ" << std::endl;
        std::ofstream fout(fileName);
        int pcNum = pPC->GetPointNumber();
        if (pPC->HasNormal() == true)
        {
            for (int i = 0; i < pcNum; i++)
            {
                const Point3D* pPoint = pPC->GetPoint(i);
                MagicMath::Vector3 pos = pPoint->GetPosition();
                MagicMath::Vector3 nor = pPoint->GetNormal();
                fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
                fout << "vn " << nor[0] << " " << nor[1] << " " << nor[2] << std::endl;
            }
        }
        else
        {
            for (int i = 0; i < pcNum; i++)
            {
                const Point3D* pPoint = pPC->GetPoint(i);
                MagicMath::Vector3 pos = pPoint->GetPosition();
                fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
            }
        }
        fout.close();
    }

    void Parser::ExportPointSetByPLY(std::string fileName, const Point3DSet* pPC)
    {
        DebugLog << "Parser::ExportPointSetByPLY" << "\n";
        std::ofstream fout(fileName);
        int pcNum = pPC->GetPointNumber();
        fout << "ply" << std::endl;
        fout << "format ascii 1.0 " << "\n";
        fout << "comment just for a test" << "\n";
        fout << "element vertex " << pcNum << "\n";
        fout << "property float x" << "\n";
        fout << "property float y" << "\n";
        fout << "property float z" << "\n";
        fout << "element face " << 0 << "\n";
        fout << "property list uchar int vertex_indices" << "\n";
        fout << "end_header" << "\n";
        for (int i = 0; i < pcNum; i++)
        {
            MagicMath::Vector3 pos = pPC->GetPoint(i)->GetPosition();
            fout << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
        }
        fout.close();
    }

    void Parser::ExportPointSetByOFF(std::string fileName, const Point3DSet* pPC)
    {
        DebugLog << "Parser::ExportPointSetByOFF: " << fileName.c_str() << std::endl;
        std::ofstream fout(fileName);
        fout << "OFF\n";
        int pointNum = pPC->GetPointNumber();
        fout << pointNum << " " << 0 << " " << 0 << "\n";
        for (int vid = 0; vid < pointNum; vid++)
        {
            MagicMath::Vector3 pos = pPC->GetPoint(vid)->GetPosition();
            fout << pos[0] << "  " << pos[1] << " " << pos[2] << "\n"; 
        }
        fout.close();
    }

    void Parser::ExportMesh3D(std::string fileName, const Mesh3D* pMesh)
    {
        size_t dotPos = fileName.rfind('.');
        if (dotPos != std::string::npos)
        {
            std::string extName = fileName.substr(dotPos + 1);
            if (extName == std::string("obj"))
            {
                ExportMesh3DByOBJ(fileName, pMesh);
            }
            else if (extName == std::string("stl"))
            {
                ExportMesh3DBySTL(fileName, pMesh);
            }
            else if (extName == std::string("off"))
            {
                ExportMesh3DByOFF(fileName, pMesh);
            }
            else
            {
                DebugLog << "Export mesh failed: file name extension error!" << std::endl;
            }
        }
        else
        {
            DebugLog << "Export mesh failed: file name error!" << std::endl;
        }
    }

    void Parser::ExportLightMesh3D(std::string fileName, const LightMesh3D* pMesh)
    {
        size_t dotPos = fileName.rfind('.');
        if (dotPos != std::string::npos)
        {
            std::string extName = fileName.substr(dotPos + 1);
            if (extName == std::string("obj"))
            {
                ExportLightMesh3DByOBJ(fileName, pMesh);
            }
            else if (extName == std::string("stl"))
            {
                ExportLightMesh3DBySTL(fileName, pMesh);
            }
            else if (extName == std::string("off"))
            {
                ExportLightMesh3DByOFF(fileName, pMesh);
            }
            else
            {
                DebugLog << "Export mesh failed: file name extension error!" << std::endl;
            }
        }
        else
        {
            DebugLog << "Export mesh failed: file name error!" << std::endl;
        }
    }

    void Parser::ExportMesh3DByOBJ(std::string fileName, const Mesh3D* pMesh)
    {
        DebugLog << "Parser::ExportMesh3DByOBJ: " << fileName.c_str() << std::endl;
        std::ofstream fout(fileName);
        int vertNum = pMesh->GetVertexNumber();
        for (int i = 0; i < vertNum; i++)
        {
            MagicMath::Vector3 pos = pMesh->GetVertex(i)->GetPosition();
            fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
        }
        int faceNum = pMesh->GetFaceNumber();
        for (int i = 0; i < faceNum; i++)
        {
            const Edge3D* pEdge = pMesh->GetFace(i)->GetEdge();
            fout << "f " << pEdge->GetVertex()->GetId() + 1 << " " << pEdge->GetNext()->GetVertex()->GetId() + 1 
                << " " << pEdge->GetPre()->GetVertex()->GetId() + 1 << "\n"; 
        }
        fout.close();
    }

    void Parser::ExportMesh3DBySTL(std::string fileName, const Mesh3D* pMesh)
    {
        DebugLog << "Parser::ExportMesh3DBySTL: " << fileName.c_str() << std::endl;
        std::ofstream fout(fileName);
        fout << "solid magic3d" << std::endl;
        int faceNum = pMesh->GetFaceNumber();
        for (int fid = 0; fid < faceNum; fid++)
        {
            const Edge3D* pEdge = pMesh->GetFace(fid)->GetEdge();
            MagicMath::Vector3 pos0 = pEdge->GetVertex()->GetPosition();
            MagicMath::Vector3 pos1 = pEdge->GetNext()->GetVertex()->GetPosition();
            MagicMath::Vector3 pos2 = pEdge->GetPre()->GetVertex()->GetPosition();
            MagicMath::Vector3 nor = (pos1 - pos0).CrossProduct(pos2 - pos0);
            nor.Normalise();
            fout << "  facet normal " << nor[0] << " " << nor[1] << " " << nor[2] << "\n";
            fout << "    outer loop" << "\n";
            fout << "      vertex " << pos0[0] << " " << pos0[1] << " " << pos0[2] << "\n";
            fout << "      vertex " << pos1[0] << " " << pos1[1] << " " << pos1[2] << "\n"; 
            fout << "      vertex " << pos2[0] << " " << pos2[1] << " " << pos2[2] << "\n";
            fout << "    endloop" << "\n";
            fout << "  endfacet" << "\n";
        }
        fout << "endsolid magic3d" << std::endl;
        fout.close();
    }

    void Parser::ExportMesh3DByOFF(std::string fileName, const Mesh3D* pMesh)
    {
        DebugLog << "Parser::ExportMesh3DByOFF: " << fileName.c_str() << std::endl;
        std::ofstream fout(fileName);
        fout << "OFF\n";
        int vertNum = pMesh->GetVertexNumber();
        int faceNum = pMesh->GetFaceNumber();
        fout << vertNum << " " << faceNum << " " << 0 << "\n";
        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicMath::Vector3 pos = pMesh->GetVertex(vid)->GetPosition();
            fout << pos[0] << "  " << pos[1] << " " << pos[2] << "\n"; 
        }
        for (int fid = 0; fid < faceNum; fid++)
        {
            const Edge3D* pEdge = pMesh->GetFace(fid)->GetEdge();
            fout << 3 << " " << pEdge->GetVertex()->GetId() << " " << pEdge->GetNext()->GetVertex()->GetId() 
                << " " << pEdge->GetPre()->GetVertex()->GetId() << "\n";
        }
        fout.close();
    }

    void Parser::ExportLightMesh3DByOBJ(std::string fileName, const LightMesh3D* pMesh)
    {
        DebugLog << "Parser::ExportLightMesh3DByOBJ: " << fileName.c_str() << std::endl;
        std::ofstream fout(fileName);
        int vertNum = pMesh->GetVertexNumber();
        for (int i = 0; i < vertNum; i++)
        {
            MagicMath::Vector3 pos = pMesh->GetVertex(i)->GetPosition();
            fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
        }
        int faceNum = pMesh->GetFaceNumber();
        for (int i = 0; i < faceNum; i++)
        {
            FaceIndex faceIdx = pMesh->GetFace(i);
            fout << "f " << faceIdx.mIndex[0] + 1 << " " << faceIdx.mIndex[1] + 1 
                << " " << faceIdx.mIndex[2] + 1 << "\n"; 
        }
        fout.close();
    }

    void Parser::ExportLightMesh3DBySTL(std::string fileName, const LightMesh3D* pMesh)
    {
        DebugLog << "Parser::ExportLightMesh3DBySTL: " << fileName.c_str() << std::endl;
        std::ofstream fout(fileName);
        fout << "solid magic3d" << std::endl;
        int faceNum = pMesh->GetFaceNumber();
        for (int fid = 0; fid < faceNum; fid++)
        {
            FaceIndex faceIdx = pMesh->GetFace(fid);
            MagicMath::Vector3 pos0 = pMesh->GetVertex(faceIdx.mIndex[0])->GetPosition();
            MagicMath::Vector3 pos1 = pMesh->GetVertex(faceIdx.mIndex[1])->GetPosition();
            MagicMath::Vector3 pos2 = pMesh->GetVertex(faceIdx.mIndex[2])->GetPosition();
            MagicMath::Vector3 nor = (pos1 - pos0).CrossProduct(pos2 - pos0);
            nor.Normalise();
            fout << "  facet normal " << nor[0] << " " << nor[1] << " " << nor[2] << "\n";
            fout << "    outer loop" << "\n";
            fout << "      vertex " << pos0[0] << " " << pos0[1] << " " << pos0[2] << "\n";
            fout << "      vertex " << pos1[0] << " " << pos1[1] << " " << pos1[2] << "\n"; 
            fout << "      vertex " << pos2[0] << " " << pos2[1] << " " << pos2[2] << "\n";
            fout << "    endloop" << "\n";
            fout << "  endfacet" << "\n";
        }
        fout << "endsolid magic3d" << std::endl;
        fout.close();
    }

    void Parser::ExportLightMesh3DByOFF(std::string fileName, const LightMesh3D* pMesh)
    {
        DebugLog << "Parser::ExportLightMesh3DByOFF: " << fileName.c_str() << std::endl;
        std::ofstream fout(fileName);
        fout << "OFF\n";
        int vertNum = pMesh->GetVertexNumber();
        int faceNum = pMesh->GetFaceNumber();
        fout << vertNum << " " << faceNum << " " << 0 << "\n";
        for (int vid = 0; vid < vertNum; vid++)
        {
            MagicMath::Vector3 pos = pMesh->GetVertex(vid)->GetPosition();
            fout << pos[0] << "  " << pos[1] << " " << pos[2] << "\n"; 
        }
        for (int fid = 0; fid < faceNum; fid++)
        {
            FaceIndex faceIdx = pMesh->GetFace(fid);
            fout << 3 << " " << faceIdx.mIndex[0] << " " << faceIdx.mIndex[1] 
                << " " << faceIdx.mIndex[2] << "\n";
        }
        fout.close();
    }
}