#pragma once
#include "person.h"
#include <string>
class RepresentativeUserSampler;
class Solver{
public:
    friend class RepresentativeUserSampler;
    virtual Person* find() = 0;
    virtual std::string name() = 0;
    RepresentativeUserSampler * g;
};