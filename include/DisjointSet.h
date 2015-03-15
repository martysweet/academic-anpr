#ifndef DISJOINTSET_H
#define DISJOINTSET_H

#include <map>

class DisjointSet
{
    public:
        /** Default constructor */
        DisjointSet();
        ~DisjointSet();
        int MakeSet();
        int Union(int a, int b);
        int Find(int x);
    protected:
    private:
        std::map<int,int> Graph;
        int NextSet = 0; // Value is incremented before every use

};

#endif // DISJOINTSET_H
