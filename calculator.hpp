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

#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include <QObject>
#include <QVector>

#include <boost/math/distributions/fisher_f.hpp>
#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/chi_squared.hpp>

#include "params.hpp"
#include "datatable.hpp"
#include "results.hpp"
#include "resultstable.hpp"

class Calculator : public QObject
{
    Q_OBJECT
public:
    explicit Calculator(DataTable *data, Results *results, Params *params, QObject *parent = 0);
    
    Entry confidenceInterval(double y, double n);
    EntryList confidenceIntervals(int column);
    void pairwiseComparision(QList<QStringList> *input, ResultsTable *out_pv, ResultsTable *out_ci);
    void pairwisePredictiveValue(const ResultsTable *ci_table, ResultsTable *out_ppv_pv, ResultsTable *out_npv_pv, ResultsTable *out_ppv_ci, ResultsTable *out_npv_ci);
    void pairwiseLikelihoodRatio(const ResultsTable *ci_table, ResultsTable *out_lrp_pv, ResultsTable *out_lrn_pv, ResultsTable *out_lrp_ci, ResultsTable *out_lrn_ci);
    
    void setData(DataTable *data)
    {
        this->data = data;
    }
    
    void setResults(Results *results)
    {
        this->results = results;
    }
    
signals:
    void calculated(bool);

public slots:
    void calculate();

private:
    DataTable *data;
    Results *results;
    Params *params;

};

#endif // CALCULATOR_HPP
