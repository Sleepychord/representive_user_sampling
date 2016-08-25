#include "ssd.h"
#include "s3.h"
#include "representative_user_sampler.h"
using namespace std;
int main(){
    auto algorithm = new RepresentativeUserSampler("config.txt");
    algorithm->readData();
    algorithm->main(); 
}