#pragma once
#include <vector>
#include <string>
struct Person{
public:
    std::vector<std::pair<Person*, double> > nebo; //neighbor, coauther/# paper of neighbor
    std::vector<int> validvalue; //index of values non-zero
    std::vector<double> max_representative_value, value; //value[d], frequency/sum of all frequencies
    int paper;
    bool mark;
    int read_state;
    std::string name;
    Person() : paper(0), mark(false), read_state(0){
    }
};
