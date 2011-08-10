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

#ifndef RESULTS_HPP
#define RESULTS_HPP

#include <QObject>

#include "params.hpp"
#include "datatable.hpp"
#include "resultstable.hpp"

const int ACC = 0;
const int SEN = 1;
const int SPE = 2;
const int PPV = 3;
const int NPV = 4;
const int LRP = 5;
const int LRN = 6;

const int NRESULTS = 7;
const int NRESTOT  = 1 + NRESULTS * 2;

const int EST  = 0;
const int LOW  = 1;
const int UPP  = 2;

const QStringList HEADER = (QStringList()
                            << "Diagnostic accuracy"
                            << "Sensitivity"
                            << "Specificity"
                            << "Positive predictive value"
                            << "Negative predictive value"
                            << "Likelihood ratio of a positive test"
                            << "Likelihood ratio of a negative test");

class Results : public QObject
{
    Q_OBJECT
public:
    explicit Results(DataTable *data, Params *params, QObject *parent = 0);
    ~Results();
    
    QList<BoolList> ci_hl;
    ResultsTable* confidence_intervals;
    
    QList<BoolList> pc_hl[NRESULTS];
    ResultsTable* pc_pv[NRESULTS];
    ResultsTable* pc_ci[NRESULTS];
    
    void buildHighlightTable(ResultsTable *table, QList<BoolList> *hl_table);
    
    bool isAvailable(int x) const
    {
        return available_results[x];
    }
    
    bool toCalculate(int x) const
    {
        return available_results[x];
    }
    
    int numberOfResults() const
    {
        return n_results;
    }

signals:

public slots:
    void buildHighlightTables();
    
private:
    DataTable *data;
    
    Params *params;
    
    bool available_results[NRESULTS];
    int n_results;

};

#endif // RESULTS_HPP
