/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#include <string>
#include <iostream>
#include <fstream>
#include <uxn/patl/aux_/perf_timer.hpp>
#include <set>

#include <uxn/patl/trie_map.hpp>
//#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/partial.hpp>
#include <uxn/patl/priority.hpp>

using namespace uxn;

int main(int /*argc*/,char *argv[])
{
#if 0
    typedef patl::trie_set<std::string,patl::bit_comparator<std::string,16> > ConcorSet;
    ConcorSet concor;
    std::ifstream input(argv[1]);
    if (!input.is_open())
        std::cerr<<"Unable to open input file!"<<std::endl;
    performance_timer tim;
    std::string str;
    while (input>>str)
        concor.insert(str);
    tim.finish();
    std::cout<<"Timer: "<<tim.get_seconds()<<" sec."<<std::endl;
    tim.start();
    for (ConcorSet::const_iterator it=concor.begin();it!=concor.end();++it)
        std::cout<<"'"<<*it<<"'"<<std::endl;
    tim.finish();
    std::cout<<"Timer: "<<tim.get_seconds()<<" sec."<<std::endl;
#endif
    typedef patl::trie_map<std::string, unsigned, 0, patl::reverse_bit_comparator<std::string> > ConcorMap;
    ConcorMap concor;
    std::ifstream input(argv[1]);
    if (!input.is_open())
    {
        std::cerr<<"Unable to open input file!"<<std::endl;
        return 0;
    }
    patl::aux::performance_timer tim;
    std::string str;
    while (input>>str)
        ++concor[str];
    tim.finish();
    std::cout<<"Timer: "<<tim.get_seconds()<<" sec."<<std::endl;
    ConcorMap concorClone=concor;
    const ConcorMap &cconc=concor;
    ConcorMap::const_iterator it;
#if 0
    tim.start();
    for (it=cconc.begin();it!=cconc.end();++it)
        std::cout<<"'"<<it->first<<"' -> "<<it->second<<std::endl;
    tim.finish();
    std::cout<<"Timer: "<<tim.get_seconds()<<" sec."<<std::endl;
#endif
    { // partial match iterator
        tim.start();
        //typedef patl::partial_match<ConcorMap,true> PartMatch;
        typedef patl::hamming_distance<ConcorMap,true> PartMatch;
        //const PartMatch pm("???ь");
        //const PartMatch pm("боковой",3);
        const PartMatch pm(cconc,2,"часовой");
        //const PartMatch pm(cconc,"??ден");
        ConcorMap::const_partimator<PartMatch> pend(cconc.end(pm));
        for (ConcorMap::const_partimator<PartMatch> pit=cconc.begin(pm)
            ;pit!=pend
            ;++pit)
            std::cout<<"'"<<pit->first<<"' -> "<<pit->second<<std::endl;
        tim.finish();
        std::cout<<"Timer: "<<tim.get_seconds()<<" sec."<<std::endl;
    }
    // equal_range example
    std::pair<ConcorMap::const_iterator,ConcorMap::const_iterator>
        pit=cconc.equal_range("орденоносцев",5*8);
    if (pit.first==pit.second)
        std::cout<<"Not found orden*"<<std::endl;
    for (ConcorMap::const_reverse_iterator rit=ConcorMap::const_reverse_iterator(pit.second)
        ;rit!=ConcorMap::const_reverse_iterator(pit.first)
        ;++rit)
        std::cout<<"'"<<rit->first<<"' -> "<<rit->second<<std::endl;
    // erase stress test
    tim.start();
#if 0
    unsigned amount=concor.size(),eraseSize=0;
    while (concor.size()>20)
    {
        unsigned size=rand()%concor.size()+1;
        eraseSize+=size;
        if (rand()<RAND_MAX/2)
        {
            for (unsigned i=0;i!=size;++i)
                concor.erase(concor.begin());
        }
        else
        {
            for (unsigned i=0;i!=size;++i)
                concor.erase(--concor.end());
        }
    }
#endif
    concor.erase("а",8);
    concor.erase("в",8);
    concor.erase("д",8);
    concor.erase("ж",8);
    concor.erase("и",8);
    concor.erase("л",8);
    concor.erase("н",8);
    concor.erase("п",8);
    concor.erase("с",8);
    concor.erase("у",8);
    concor.erase("х",8);
    concor.erase("ч",8);
    concor.erase("э",8);
    concor.erase("я",8);
    tim.finish();
    std::cout<<"Timer erase: "<<tim.get_seconds()<<" sec."<<std::endl;
    std::cout<<"*** After erase all:"<<std::endl;
    //std::cout<<"*** Size must be "<<amount<<" - "<<eraseSize<<" = "<<amount-eraseSize<<std::endl;
    unsigned size=0;
    for (it=cconc.begin();it!=cconc.end();++it,++size)
        std::cout<<"'"<<it->first<<"' -> "<<it->second<<std::endl;
    std::cout<<"*** Size: "<<concor.size()<<", true size: "<<size<<std::endl;
    // unique_priority_queue example
    patl::unique_priority_queue<int/*,std::set<int> */> pq;
    pq.push(-1);
    pq.push(22);
    pq.push(2147483647);
    std::cout<<pq.top()<<' ';
    pq.pop();
    std::cout<<pq.top()<<std::endl;
    pq.pop();
    pq.push(11);
    pq.push(0x80000000); // -2147483648
    pq.push(33);
    pq.pop();
    while (!pq.empty())
    {
        std::cout<<pq.top()<<' ';
        pq.pop();
    }
    std::cout<<std::endl;
    return 0;
}
