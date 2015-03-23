#include "DisjointSet.h"

// Creates a new disjoint set which points to itself (parent)
int DisjointSet::MakeSet(){
    NextSet++;
    Graph[NextSet] = NextSet; // Incremented before use
    return NextSet; // Let the user know which set ID has been assigned
}

// Returns the set ID of the most parent set
int DisjointSet::Find(int x){
    int ID = Graph[x];
    if(x != ID){
        ID = Find(ID); // Recurse
    }
    return ID;
}

// Merge two sets together, setting the parent to be the lowest set, return this value
int DisjointSet::Union(int a, int b){
    // Merge two sets, setting to the lowest set ID
    int aID = Find(a); // Find the lowest ID of each set
    int bID = Find(b);

    // We want to merge into the lowest ID
    int nID;
    if(aID < bID){
        nID = aID;
    }else{
        nID = bID;
    }

    // Make sets a,b part of nID
    Graph[a] = nID;
    Graph[b] = nID;

    // Return new merged set
    return nID;
}

/*
    Example Usage:
    DisjointSet MappingSet;
    int a = MappingSet.MakeSet();
    int b = MappingSet.MakeSet();
    int c = MappingSet.MakeSet();
    int d = MappingSet.Union(b,c);
    int e = MappingSet.MakeSet();
    std::cout << MappingSet.Find(a) << std::endl;
    std::cout << MappingSet.Find(b) << std::endl;
    std::cout << MappingSet.Find(c) << std::endl;
    std::cout << MappingSet.Find(d) << std::endl;
    std::cout << MappingSet.Find(e) << std::endl;
*/


