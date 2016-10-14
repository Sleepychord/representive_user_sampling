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
            break;
        }
    }
    if(need_read)
    {
        for(int index = 0;index < maxd;index++){
            Group& gr = groups[index];
            for(int i = gr.now;i < gr.now + B && i < gr.size();i++)
            {
                if(gr.ordered_member[i]->read_state == 0)
                    gr.ordered_member[i]->read_state = 1;
            }
        }
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
    clock_t start_time = clock();
    int pcnt = 0;
    while(readstr(temp) != EOF) {
    if((pcnt++) % 10000 == 0) cout<< "reading people " << pcnt <<endl;;
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
    tout<<"time of read people is "<<(clock() - start_time) * 1.0 / CLOCKS_PER_SEC <<endl;
    //--------read edges for the first time------------
    start_time = clock();
    pcnt = 0;
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
        else {
            fclose(fh);
            return true;
        }
    };
    tout<<"time of read edge first time is "<<(clock() - start_time) * 1.0 / CLOCKS_PER_SEC <<endl;
    ofstream fout("deepwalkdata.edgelist");
    if(partfile){
        do{
            cout << "reading file "<<filename<<endl;
            freopen(filename.c_str(), "r", stdin);
            int ch = '0';
            while(readstr(temp) != EOF){
                string strtmp = string(temp);
                auto ret1 = register_map.find(strtmp);
                int index1 = ret1->second;
                while((ch != EOF) && (ch != '\n')) {
                    ch = readstr(temp);
                    strtmp = string(temp);
                    auto ret2 = register_map.find(strtmp);
                    int index2 = ret2->second;
                    if(ret1 == register_map.end() || ret2 == register_map.end()) continue;
                    if(deepwalk)
                        fout << index1 <<" "<<index2 <<endl;
                    people[index1].paper += 1;
                    people[index2].paper += 1;
                }
            }    
        }while(nextfile());
    }else{
        cout << "reading file "<<filename<<endl;
        freopen(coauthor_file.c_str(), "r", stdin);
        do tmp = getc(stdin); while(tmp != '\n' && tmp != EOF);
        while(readstr(temp) != EOF) {
            string strtmp = string(temp);
            auto ret1 = register_map.find(strtmp);
            int index1 = ret1->second;
            readstr(temp);
            strtmp = string(temp);
            auto ret2 = register_map.find(strtmp);
            if(ret1 == register_map.end() || ret2 == register_map.end()) continue;
            int index2 = ret2->second;
            scanf("%d", &tmp);
            if(deepwalk)
                fout << index1 <<" "<<index2 <<endl;
            people[index1].paper += tmp;
            people[index2].paper += tmp;
            readstr(temp);
        }
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
    cout<< "normalization..."<<endl;
    for(int number = 0;number < people.size();number++){
        double svalue = 0;
        for(int j = 0;j < maxd;j++)
            svalue += people[number].value[j];
        for(int j = 0;j < maxd;j++)
        if(svalue != 0)
            people[number].value[j] /= svalue;
    }
    //----------sort for papar number----------
    cout<< "sorting"<<endl;
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
    clock_t start_time = clock();
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
        else {
            fclose(fh);
            return true;
        }
    };
    if(partfile){
        do{
            cout << "reading file "<<filename<<endl;
            freopen(filename.c_str(), "r", stdin);
            int ch = '0';
            while(readstr(temp) != EOF){
                string strtmp = string(temp);
                auto ret1 = register_map.find(strtmp);
                int index1 = ret1->second;
                while((ch != EOF) && (ch != '\n')) {
                    ch = readstr(temp);
                    strtmp = string(temp);
                    auto ret2 = register_map.find(strtmp);
                    int index2 = ret2->second;
                    if(ret1 == register_map.end() || ret2 == register_map.end()) continue;
                    if(people[index1].read_state < 2 && people[index2].read_state < 2 && people[index1].read_state + people[index2].read_state > 0)// new edge
                    {
                        people[index1].nebo.push_back(pair<Person*, double>(&people[index2], tmp * 1.0 / people[index2].paper));
                        people[index2].nebo.push_back(pair<Person*, double>(&people[index1], tmp * 1.0 / people[index1].paper));
                    }
                }
            }    
        }while(nextfile());
    }else{
        cout << "reading file "<<filename<<endl;
        freopen(coauthor_file.c_str(), "r", stdin);
        do tmp = getc(stdin); while(tmp != '\n' && tmp != EOF);
        while(readstr(temp) != EOF) {
            string strtmp = string(temp);
            auto ret1 = register_map.find(strtmp);
            int index1 = ret1->second;
            readstr(temp);
            strtmp = string(temp);
            auto ret2 = register_map.find(strtmp);
            if(ret1 == register_map.end() || ret2 == register_map.end()) continue;
            int index2 = ret2->second;
            scanf("%d", &tmp);
            if(people[index1].read_state < 2 && people[index2].read_state < 2 && people[index1].read_state + people[index2].read_state > 0)// new edge
            {
                people[index1].nebo.push_back(pair<Person*, double>(&people[index2], tmp * 1.0 / people[index2].paper));
                people[index2].nebo.push_back(pair<Person*, double>(&people[index1], tmp * 1.0 / people[index1].paper));
            }
            readstr(temp);
        }
    }
    for(auto & p: people)
    if(p.read_state == 1)
        p.read_state = 2;
    tout<<"time of read edge is "<<(clock() - start_time) * 1.0 / CLOCKS_PER_SEC <<endl;
}
void RepresentativeUserSampler::main(){
    tout << "begin main" <<endl;
    clock_t start_time = clock();
    ofstream fout(output_file);
    for(int u = 0;u < K;u++){
        cerr<<"finding "<<u<<endl;
        Person* p = solver->find();
        fout << p->name <<endl;
        choose(p);
    }
    tout <<"time of main is "<< (clock() - start_time) * 1.0 / CLOCKS_PER_SEC <<endl;
    tout.close();
    fout.close();
}