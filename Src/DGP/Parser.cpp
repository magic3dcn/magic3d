#include "StdAfx.h"
#include "Parser.h"
#include <fstream>
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
        MagicLog << "file name: " << fileName.c_str() << std::endl;
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
}