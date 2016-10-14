#pragma once
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <cassert>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "person.h"
#include "group.h"
#include "solver.h"
#include "ssd.h"
#include "s3.h"
using std::pair;
using std::vector;
class RepresentativeUserSampler{
public:
    std::ofstream tout;
    bool deepwalk, streaming;
    std::string author_file, coauthor_file, output_file;
    friend class Solver;
    vector<Group> groups;
    std::map<std::string, int> register_map;
    vector<Person> people;
    int K, B, d, maxd;
    int partfile;
    vector<int> order_list;
    Solver * solver;
    void choose(Person*);
    void readData();
    void readEdge();//use for streaming
    void main();
    int readstr(char *str);
    RepresentativeUserSampler(const char* filename): deepwalk(false), streaming(false){
        std::string s;
        std::ifstream fin(filename);
        tout.open("time.txt", std::ofstream::out);
        partfile = 0;
        while(fin >> s){
            int tmp = 0;
            if(s == "author_file") fin >> author_file;
            else if (s == "coauthor_file") fin >>coauthor_file;
            else if (s == "output_file") fin >> output_file;
            else if (s == "K") fin >> K;
            else if (s == "B") fin >> B;
            else if (s == "partfile") fin >> partfile;
            else if (s == "deepwalk") {fin >> tmp; if(tmp) deepwalk = true;}
            else if (s == "streaming"){fin >> tmp; if(tmp) streaming = true;}
            else if (s == "algorithm"){
                std::string al_name;
                fin >> al_name;
                if(al_name == "SSD")  setSolver(new SSD());
                else if(al_name == "S3") setSolver(new S3());
                else {
                    std::cerr<< "now such algorithm"<<std::endl;
                    throw ;
                }
            }
        }
    }
    void setSolver(Solver* s){
        s->g = this;
        solver = s;
    }
};