#pragma once
#include "solver.h"
#include <cmath>
class S3 : public Solver{
    virtual Person* find();
    virtual std::string name(){ return "s3"; }
};