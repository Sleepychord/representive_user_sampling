#include "representative_user_sampler.h"
using namespace std;
void RepresentativeUserSampler::choose(Person* p){
    for(auto & index: p->validvalue){
        groups[index].calc_increment(p, true);
    }
    p->mark = true;
    bool need_read = false;
    for(auto & index: p->validvalue){
        groups[index].label_next();
        if(groups[index].get_next() && groups[index].get_next()->read_state == 0){
            need_read = true;
            Group& gr = groups[index];
            for(int i = gr.now;i < gr.now + B && i < gr.size();i++)
            {
                if(gr.ordered_member[i]->read_state == 0)
                    gr.ordered_member[i]->read_state = 1;
            }
        }
    }
    if(need_read)
    {
        cerr<< "need_read" <<endl;
        readEdge();
    }
}
int RepresentativeUserSampler::readstr(char *str) {
    int pos = 0, ch;
    str[0] = '\0';
    do {
        ch = getc(stdin);
        if(ch == '\n' || ch == EOF) return ch;
    } while(isspace(ch));
    do {
        str[pos++] = ch;
        ch = getc(stdin);
    } while(ch != '\n' && ch != '\t' && ch != EOF);
    str[pos] = '\0';
    return ch;
}

void RepresentativeUserSampler::readData(){
    int cnt = 0;
    freopen(author_file.c_str(), "r", stdin);
    char temp[256];
    int tmp;
    d = 0;
    while(readstr(temp) != '\n' && temp[0]) ++d;
    maxd = --d;
    if(deepwalk){
        maxd *= 1.3;
        if((maxd - d) % 2) maxd++;
    }
    for(int i = 0;i < maxd;i++)
        groups.push_back(Group(i));
    //-----------read people-------------------
    while(readstr(temp) != EOF) {
    people.push_back(Person());
    Person& r = people[people.size()-1];
    register_map.insert(pair<string, int > (string(temp), cnt++));
    r.name = string(temp);
    r.value = vector<double>(maxd, 0);
    r.max_representative_value = vector<double>(maxd, 0);
    for(int i = 0; i < d; ++i) {
        scanf("%d", &tmp);
        if(tmp) {
            r.validvalue.push_back(i);
            r.value[i] = tmp;
        }
    }
    readstr(temp);
    }
    //--------read edges for the first time------------
    freopen(coauthor_file.c_str(), "r", stdin);
    ofstream fout("deepwalkdata.edgelist");
    if(!partfile)
        do tmp = getc(stdin); while(tmp != '\n' && tmp != EOF);
    while(readstr(temp) != EOF) {
        int index1 = register_map.find(string(temp))->second;
        readstr(temp);
        int index2 = register_map.find(string(temp))->second;
        scanf("%d", &tmp);
        if(deepwalk)
            fout << index1 <<" "<<index2 <<endl;
        people[index1].paper += tmp;
        people[index2].paper += tmp;
        readstr(temp);
    }
    fout.close();
    //----------------deep walk-----------------
    if(deepwalk){
        char cmd[100];
        int d2 = maxd - d, walk_length = 50, window_size = 10, walk_num = 10;
        sprintf(cmd, "python deepwalk --format edgelist --input deepwalkdata.edgelist --output deepwalkdata.embeddings --representation-size %d --walk-length %d --window-size %d --number-walks %d", d2 , walk_length, window_size, walk_num);
        system(cmd);
        ifstream fin("./deepwalkdata.embeddings");
        int n,m;
        fin >>n >>m;
        assert(m == d2);
        for(int i = 0;i < n;i++)
        {
            int number;
            double tmp;
            fin >>number;
            double minv = 1e10, maxv = -1e10;
            for(int j = 0;j < m;j++){
                fin >> tmp;
                if(tmp < minv) minv = tmp;
                if(tmp > maxv) maxv = tmp;
                people[number].value.push_back(tmp);
            }
            for(int j = d;j < m + d;j++){
                if(maxv - minv >1e-6)
                    people[number].value[j] = (people[number].value[j] - minv) / (maxv - minv);
                else people[number].value[j] = 0;
                if(people[number].value[j] > 1e-6)
                    people[number].validvalue.push_back(j);
            }
        }
    }
    //-------normalization--------------------
    for(int number = 0;number < people.size();number++){
        double svalue = 0;
        for(int j = 0;j < maxd;j++)
            svalue += people[number].value[j];
        for(int j = 0;j < maxd;j++)
        if(svalue != 0)
            people[number].value[j] /= svalue;
    }
    //----------sort for papar number----------
    for(int i = 0;i < people.size();i++)
        order_list.push_back(i);
    sort(order_list.begin(), order_list.end(), [this](int x, int y){ return people[x].paper > people[y].paper; });
    for(int i = 0;i < people.size();i++)
    {
        Person& p = people[order_list[i]];
        for(auto & t : p.validvalue){
            groups[t].ordered_member.push_back(&p);
            if(!streaming || groups[t].ordered_member.size() <= B)
                p.read_state = 1;
        }
    }
    //----------read edge for the second time-----
    readEdge();
}
void RepresentativeUserSampler::readEdge()//use for streaming
{
    int tmp;
    char temp[256];
    int num = 0;
    string filename =(partfile) ? coauthor_file + string("part-00000") : coauthor_file;
    string copy_pre = coauthor_file;
    auto nextfile = [& filename, & num, copy_pre](){
        num ++;
        char nametmp[100];
        sprintf(nametmp, "part-%05d", num);
        filename = copy_pre + nametmp;
        FILE* fh = fopen(filename.c_str(),"r");
        if(fh == NULL) return false;
        else return true;
    };
    do{
        cout << "reading file "<<filename<<endl;
        freopen(filename.c_str(), "r", stdin);
        if(!partfile)
            do tmp = getc(stdin); while(tmp != '\n' && tmp != EOF);
        while(readstr(temp) != EOF) {
            int index1 = register_map.find(string(temp))->second;
            readstr(temp);
            int index2 = register_map.find(string(temp))->second;
            scanf("%d", &tmp);
            if(people[index1].read_state < 2 && people[index2].read_state < 2 && people[index1].read_state + people[index2].read_state > 0)// new edge
            {
                people[index1].nebo.push_back(pair<Person*, double>(&people[index2], tmp * 1.0 / people[index2].paper));
                people[index2].nebo.push_back(pair<Person*, double>(&people[index1], tmp * 1.0 / people[index1].paper));
            }
            readstr(temp);
        }
    }while(partfile && nextfile());
    for(auto & p: people)
    if(p.read_state == 1)
        p.read_state = 2;
}
void RepresentativeUserSampler::main(){
    freopen(output_file.c_str(),"w", stdout);
    for(int u = 0;u < K;u++){
        Person* p = solver->find();
        cout << p->name <<endl;
        choose(p);
    }
}