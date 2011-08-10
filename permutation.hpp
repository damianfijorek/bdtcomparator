#ifndef PERMUTATION_HPP
#define PERMUTATION_HPP

#include <QList>

class Permutation
{
public:
    Permutation(int length);
    
    //! returns position of x after current permutation
    int at(int x) const
    {
        if (x<length)
        {
            return perm.at(x);
        }
        else
        {
            return -1;
        }
    }
    
private:
    //!length of permutation
    int length;
    
    //! permutation list
    QList<int> perm;
};

#endif // PERMUTATION_HPP
