#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class File
{
    public:
      int iteration;
      int fl;
      float angle;
      string axiom, axiom2;
      string rule, rule2;
      string data1, data2, data3, data4, data5, data6;
      vector <string> Lines[6];
      
      int getIteration()
      {
        return iteration;
      }

      float getAngle()
      {
        return angle;
      }

      string getAxiom()
      {
        return axiom;
      }

      string getRule1()
      {

        return rule;
      }

      string getAxiom2()
      {
        return axiom2;
      }

      string getRule2()
      {

        return rule2;
      }

      File()
      {
        ifstream file;

        file.open("config/1.txt");

        if(file.is_open())
        {
          while(getline(file, data1))
          {
            Lines[0].push_back(data1);
          }
        }
        file.close();

        file.open("config/2.txt");

        if(file.is_open())
        {
          while(getline(file, data2))
          {
            Lines[1].push_back(data2);
          }
        }
        file.close();

        file.open("config/3.txt");

        if(file.is_open())
        {
          while(getline(file, data3))
          {
            Lines[2].push_back(data3);
          }
        }
        file.close();

        file.open("config/4.txt");

        if(file.is_open())
        {
          while(getline(file, data4))
          {
            Lines[3].push_back(data4);
          }
        }
        file.close();

        file.open("config/5.txt");

        if(file.is_open())
        {
          while(getline(file, data5))
          {
            Lines[4].push_back(data5);
          }
        }
        file.close();

        file.open("config/6.txt");

        if(file.is_open())
        {
          while(getline(file, data6))
          {
            Lines[5].push_back(data6);
          }
        }
        file.close();

        chooseFile(1);
      }

      void chooseFile(int choose)
      {
        fl = choose;

        if(fl  == 1)
        {
          iteration = ::atoi(Lines[0][0].c_str());
          angle = ::atof(Lines[0][1].c_str());
          axiom = Lines[0][2];
          rule = Lines[0][3];
          axiom2 = Lines[0][4];
          rule2 = Lines[0][5];
        }
        else if(fl == 2)
        {
          iteration = ::atoi(Lines[1][0].c_str());
          angle = ::atof(Lines[1][1].c_str());
          axiom = Lines[1][2];
          rule = Lines[1][3];
          axiom2 = Lines[1][4];
          rule2 = Lines[1][5];
        }
        else if(fl == 3)
        {
          iteration = ::atoi(Lines[2][0].c_str());
          angle = ::atof(Lines[2][1].c_str());
          axiom = Lines[2][2];
          rule = Lines[2][3];
          axiom2 = Lines[2][4];
          rule2 = Lines[2][5];
        }
        else if(fl == 4)
        {
          iteration = ::atoi(Lines[3][0].c_str());
          angle = ::atof(Lines[3][1].c_str());
          axiom = Lines[3][2];
          rule = Lines[3][3];
          axiom2 = Lines[3][4];
          rule2 = Lines[3][5];
        }
        else if(fl == 5)
        {
          iteration = ::atoi(Lines[4][0].c_str());
          angle = ::atof(Lines[4][1].c_str());
          axiom = Lines[4][2];
          rule = Lines[4][3];
          axiom2 = Lines[4][4];
          rule2 = Lines[4][5];

        }
        else if(fl == 6)
        {
          iteration = ::atoi(Lines[5][0].c_str());
          angle = ::atof(Lines[5][1].c_str());
          axiom = Lines[5][2];
          rule = Lines[5][3];
          axiom2 = Lines[5][4];
          rule2 = Lines[5][5];
        }
      }
};