#include "permutation.hpp"

Permutation::Permutation(int length)
{
    this->length = length;
    
    // initial permutation
    for (int i=0; i<length; i++)
    {
        perm.append(i);
    }
}
