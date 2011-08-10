#ifndef PERMUTATION_HPP
#define PERMUTATION_HPP

#include <QList>
#include <QtAlgorithms>

const int ASCENDING  = 0;
const int DESCENDING = 1;

namespace permutation
{
    struct pair
    {
        double value;
        int index;
    };
    
    bool pairLessThan(const pair &p1, const pair &p2);
    bool pairGreaterThan(const pair &p1, const pair &p2);
}

class Permutation
{
public:
    Permutation(int length);
    
    //! returns position of x after current permutation
    int at(int x) const
    {
        if (x<length)
        {
            return list.at(x).index;
        }
        else
        {
            return -1;
        }
    }
    
    //! sorts the list of values and builds permutation
    void sort(QList<double> *value, int order=DESCENDING)
    {
        for (int i=0; i<length; i++)
        {
            list[i].value = value->at(i);
            list[i].index = i;
        }
        
        switch (order)
        {
        case DESCENDING:
            qSort(list.begin(), list.end(), permutation::pairGreaterThan);
            break;
        case ASCENDING:
            qSort(list.begin(), list.end(), permutation::pairLessThan);
        }
    }
    
private:
    //!length of permutation
    int length;
       
    //! list of pairs
    QList<permutation::pair> list;
};

#endif // PERMUTATION_HPP
