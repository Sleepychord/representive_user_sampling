#include "ssd.h"
#include "representative_user_sampler.h"
using namespace std;
Person* SSD::find(){
    double poorest_value = 1e10;
    int best_paper_num = 0;
    Person* ret = NULL;
    for(auto & group : g->groups)
    if(group.size())
    {
        double value = group.representive_sum / group.size();
        Person* p = group.get_next();
        if(p && (value < poorest_value || value < poorest_value + 1e-6 && p->paper > best_paper_num))
            poorest_value = value, ret = p, best_paper_num = p->paper;
    }
    return ret;
}