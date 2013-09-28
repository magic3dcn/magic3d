//#include "StdAfx.h"
#include "Parser.h"
#include <fstream>
#include <istream>
#include <ostream>
#include <vector>
#include "DGPDefines.h"

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
        return ParsePointSetByOBJ(fileName);
    }

    Point3DSet* Parser::ParsePointSetByOBJ(std::string fileName)
    {
        MagicLog << "ParsePointSetByOBJ file name: " << fileName.c_str() << std::endl;
        std::ifstream fin(fileName);
        std::vector<Vector3> posList;
        std::vector<Vector3> norList;
        while (!fin.eof())
        {
            char pLine[512];
            if (fin.peek() == 'v')
            {
                char ch;
                fin.get(ch);
                fin.get(ch);
                if (ch == ' ')
                {
                    Vector3 posTemp;
                    fin >> posTemp[0] >> posTemp[1] >> posTemp[2];
                    posList.push_back(posTemp);
                }
                else if (ch == 'n')
                {
                    Vector3 norTemp;
                    fin >> norTemp[0] >> norTemp[1] >> norTemp[2];
                    norList.push_back(norTemp);
                }
                else
                {
                    fin.getline(pLine, 512);
                }
            }
            else
            {
                fin.getline(pLine, 512);
            }
        }
        fin.close();
        Point3DSet* pPSet = new Point3DSet;
        MagicLog << "Vertex number: " << posList.size() << " normal number: " << norList.size() << std::endl;
        if (norList.size() > 0)
        {
            MAGICASSERT(norList.size() == posList.size(), "Error: Positino size != Normal Size in OBJ file!");
            for (int i = 0; i < posList.size(); i++)
            {
                Point3D* point = new Point3D(posList.at(i), norList.at(i), i);
                pPSet->InsertPoint(point);
            }
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

    Mesh3D* Parser::ParseMesh3D(std::string fileName)
    {
        return ParseMesh3DByOBJ(fileName);
    }

    Mesh3D* Parser::ParseMesh3DByOBJ(std::string fileName)
    {
        MagicLog << "ParseMesh3DByOBJ file name: " << fileName.c_str() << std::endl;
        std::ifstream fin(fileName);
        const int maxSize = 512;
        char pLine[maxSize];
        Mesh3D* pMesh = new Mesh3D;
        std::vector<Vector3> normalList;
        std::vector<Vector3> texcordList;
        while (fin.getline(pLine, maxSize))
        {
            if (pLine[0] == 'v')
            {
                if (pLine[1] == ' ' )
                {
                    char* tok = strtok(pLine, " ");
                    Vector3 pos;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        pos[i] = (Real)atof(tok);
                    }
                    pMesh->InsertVertex(pos);
                }
                else if (pLine[1] == 'n')
                {
                    char* tok = strtok(pLine, " ");
                    Vector3 nor;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        nor[i] = (Real)atof(tok);
                    }
                    normalList.push_back(nor);
                }
                else if (pLine[1] == 't')
                {
                    char* tok = strtok(pLine, " ");
                    Vector3 tex;
                    for (int i = 0; i < 3; i++)
                    {
                        tok = strtok(NULL, " ");
                        tex[i] = (Real)atof(tok);
                    }
                    texcordList.push_back(tex);
                }
            }
            else if (pLine[0] == 'f' && pLine[1] == ' ')
            {
                std::vector<Vertex3D* > vertList;
                char* tok = strtok(pLine, " ");
                for (int i = 0; i < 3; i++)
                {
                    tok = strtok(NULL, " ");
                    char temp[100];
                    strcpy(temp, tok);
                    temp[strcspn(temp, "/")] = 0;
                    int id = (int)strtol(temp, NULL, 10) - 1;
                    Vertex3D* pVert = pMesh->GetVertex(id);
                    vertList.push_back(pVert);
                }
                if (vertList.size() == 3)
                {
                    pMesh->InsertFace(vertList);
                }
            }
        }
        int vertNum = pMesh->GetVertexNumber();
        MagicLog << "Import Vertex Number: " << vertNum << " Normal Number: " << normalList.size() << " Texture Number: " << texcordList.size() << std::endl;
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

        return pMesh;
    }

    void Parser::ExportPointSet(std::string fileName, const Point3DSet* pPC)
    {
        //ExportPointSetByPSR(fileName, pPC);
        ExportPointSetByOBJ(fileName, pPC);
        //ExportPointSetByPLY(fileName, pPC);
    }

    void Parser::ExportPointSetByPSR(std::string fileName, const Point3DSet* pPC)
    {
        MagicLog << "Parser::ExportPointSetByPSR" << std::endl;
        std::ofstream fout(fileName);
        int pcNum = pPC->GetPointNumber();
        for (int i = 0; i < pcNum; i++)
        {
            const Point3D* pPoint = pPC->GetPoint(i);
            Vector3 pos = pPoint->GetPosition();
            Vector3 nor = pPoint->GetNormal();
            fout << pos[0] << " " << pos[1] << " " << pos[2] << " " << nor[0] << " " << nor[1] << " " << nor[2] << std::endl;
        }
        fout.close();
    }

    void Parser::ExportPointSetByOBJ(std::string fileName, const Point3DSet* pPC)
    {
        MagicLog << "Parser::ExportPointSetByOBJ" << std::endl;
        std::ofstream fout(fileName);
        int pcNum = pPC->GetPointNumber();
        for (int i = 0; i < pcNum; i++)
        {
            const Point3D* pPoint = pPC->GetPoint(i);
            Vector3 pos = pPoint->GetPosition();
            Vector3 nor = pPoint->GetNormal();
            fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
            fout << "vn " << nor[0] << " " << nor[1] << " " << nor[2] << std::endl;
        }
        fout.close();
    }

    void Parser::ExportPointSetByPLY(std::string fileName, const Point3DSet* pPC)
    {
        MagicLog << "Parser::ExportPointSetByPLY" << std::endl;
        std::ofstream fout(fileName);
        int pcNum = pPC->GetPointNumber();
        fout << "ply" << std::endl;
        fout << "format ascii 1.0 " << std::endl;
        fout << "comment just for a test" << std::endl;
        fout << "element vertex " << pcNum << std::endl;
        fout << "property float x" << std::endl;
        fout << "property float y" << std::endl;
        fout << "property float z" << std::endl;
        fout << "element face " << 0 << std::endl;
        fout << "property list uchar int vertex_indices" << std::endl;
        fout << "end_header" << std::endl;
        for (int i = 0; i < pcNum; i++)
        {
            Vector3 pos = pPC->GetPoint(i)->GetPosition();
            fout << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
        }
        fout.close();
    }

    void Parser::ExportMesh3D(std::string fileName, const Mesh3D* pMesh)
    {
        //ExportMesh3DByOBJ(fileName, pMesh);
        ExportPointSetFromMeshByOBJ(fileName, pMesh);
    }

    void Parser::ExportPointSetFromMesh(std::string fileName, const Mesh3D* pMesh)
    {
        ExportPointSetFromMeshByOBJ(fileName, pMesh);
    }

    void Parser::ExportPointSetFromMeshByOBJ(std::string fileName, const Mesh3D* pMesh)
    {
        MagicLog << "Parser::ExportPointSetByOBJFromMesh" << std::endl;
        std::ofstream fout(fileName);
        int vertNum = pMesh->GetVertexNumber();
        for (int i = 0; i < vertNum; i++)
        {
            Vector3 pos = pMesh->GetVertex(i)->GetPosition();
            Vector3 nor = pMesh->GetVertex(i)->GetNormal();
            fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
            fout << "vn " << nor[0] << " " << nor[1] << " " << nor[2] << std::endl;
        }
        fout.close();
    }

    void Parser::ExportMesh3DByOBJ(std::string fileName, const Mesh3D* pMesh)
    {
        MagicLog << "Parser::ExportMesh3DByOBJ" << std::endl;
        std::ofstream fout(fileName);
        int vertNum = pMesh->GetVertexNumber();
        for (int i = 0; i < vertNum; i++)
        {
            Vector3 pos = pMesh->GetVertex(i)->GetPosition();
            fout << "v " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
        }
        int faceNum = pMesh->GetFaceNumber();
        for (int i = 0; i < faceNum; i++)
        {
            const Edge3D* pEdge = pMesh->GetFace(i)->GetEdge();
            fout << "f " << pEdge->GetVertex()->GetId() + 1 << " " << pEdge->GetNext()->GetVertex()->GetId() + 1 << " " << pEdge->GetPre()->GetVertex()->GetId() + 1 << std::endl; 
        }
        fout.close();
    }
}