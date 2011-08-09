/*
Binary Diagnostic Tests Comparator
Copyright (C) 2011 Damian Fijorek (damianfijorek@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <QObject>

const int ALL     = 0;
const int SENONLY = 1;
const int SPEONLY = 2;

class Params : public QObject
{
    Q_OBJECT
    
    int gold_standard;
    int case_to_calculate;
    double confidence_level;
    double pvalue;
    
public:
    explicit Params(QObject *parent = 0);
    
    int getGoldStandard() const
    {
        return gold_standard;
    }
    
    double getConfidenceLevel() const
    {
        return confidence_level;
    }
    
    double getPvalue() const
    {
        return pvalue;
    }
    
    int getCaseToCalculate() const
    {
        return case_to_calculate;
    }
    
    void setCaseToCalculate(int c)
    {
        this->case_to_calculate = c;
    }
    
signals:
    void paramsChanged();
    void gsChanged(int gs);
    
public slots:
    void setGoldStandard(int gs)
    {
        gold_standard = gs;
        
        emit gsChanged(gs);
        emit paramsChanged();
    }
    
    void setConfidenceLevel(double cl)
    {
        confidence_level = cl;
        
        emit paramsChanged();
    }
    
    void setPvalue(double cl)
    {
        pvalue = 1.0 - cl;
        
        emit paramsChanged();
    }

};

#endif // PARAMS_HPP
