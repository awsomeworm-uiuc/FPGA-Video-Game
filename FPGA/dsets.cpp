/* Your code here! */
#include "dsets.h"

#include <vector>

void DisjointSets::addelements(int num){
    for(int i = 0; i < num; i++){
        sets_.push_back(-1);
    }
}

int DisjointSets::find(int elem){
    if(sets_[elem] < 0){
        return elem;
    }
    else{
        sets_[elem] = find(sets_[elem]);
        return sets_[elem];
    }
}

void DisjointSets::setunion(int a, int b){
    int root_1 = find(a);
    int root_2 = find(b);

    //check if in same set
    if(root_1 == root_2){
        return;
    }

    if(size(root_1) < size(root_2)){
        sets_[root_2] += sets_[root_1];
        sets_[root_1] = root_2;
    }
    else{
        sets_[root_1] += sets_[root_2];
        sets_[root_2] = root_1;
    }
}

int DisjointSets::size(int elem){
    return -(sets_[find(elem)]);
}