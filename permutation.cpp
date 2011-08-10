#include "permutation.hpp"

//! returns whether pair p1 is less than pair p2
bool permutation::pairLessThan(const pair &p1, const pair &p2)
{
    return p1.value < p2.value;
}

//! returns whether pair p1 is greater than pair p2
bool permutation::pairGreaterThan(const pair &p1, const pair &p2)
{
    return p1.value > p2.value;
}

Permutation::Permutation(int length)
{
    this->length = length;
    
    // initial permutation
    for (int i=0; i<length; i++)
    {
        permutation::pair p;
        
        p.index = i;
        
        list.append(p);
    }
}
