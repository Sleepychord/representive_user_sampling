#pragma once
#include "solver.h"
class SSD : public Solver{
    virtual Person* find();
    virtual std::string name(){ return "ssd"; }
};