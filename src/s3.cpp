#include "s3.h"
#include "representative_user_sampler.h"
using namespace std;
Person* S3::find(){
    const double beta = 0.8;
    Person * ret;
    double maxincr = -1;
    for(int i = 0;i < g->people.size();i++){
        auto & p = g->people[i];
        if(p.mark) continue;
        double incr = 0;
        for(auto& g_index: p.validvalue){
            Group& gr = g->groups[g_index];
            double topic_incr = gr.calc_increment(&p);
            incr += pow(gr.size(), 0.5) * (pow(gr.representive_sum + topic_incr, beta) - pow(gr.representive_sum, beta));
            //if(p.name == "Jiawei Han") cerr << topic_incr <<endl;
        }
        if(incr > maxincr)
            maxincr = incr, ret = &p;
    }
    //cerr << ret->name <<" "<<maxincr<<" "<< ret->validvalue.size() << endl;
    return ret;
}