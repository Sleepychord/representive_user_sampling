#pragma once
#include <vector>
#include "person.h"
#include <cassert>
#include <iostream>
using std::vector;
class Group{
    Group():representive_sum(0), now(0){}
public:
    vector<Person*> ordered_member;
    int size(){return ordered_member.size();}
    double representive_sum;
    Group(int _topic):representive_sum(0), now(0), topic(_topic){}
    int now, topic;
    Person* get_next()
    {
        return now < size() ? ordered_member[now] : NULL;
    }
    void label_next(){
        while(now < size() && (ordered_member[now]->mark || calc_increment(ordered_member[now]) < 1e-6))
            now++;
    }
    double calc_increment(Person * p, bool update = false){
        assert(p->value[topic] > 1e-6) ;
        assert(!p->mark);
        double ret = 1 - p->max_representative_value[topic];
        if(update) p->max_representative_value[topic] = 1;
        for(auto & e: p->nebo){
            if(e.first->value[topic] < 1e-6 || e.first->mark) continue;
            double new_value = p->value[topic] * e.second;
            // if(p->name == "Jiawei Han") std::cerr << p->value[topic] <<" "<<e.second<<std::endl;
            if(new_value > e.first->max_representative_value[topic]){
                ret += new_value - e.first->max_representative_value[topic];
                if(update) e.first->max_representative_value[topic] = new_value;
            }
        }
        if(update) representive_sum += ret;
        return ret;
    }
    
};