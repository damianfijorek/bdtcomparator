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

#include "results.hpp"

Results::Results(DataTable *data, Params *params, QObject *parent) :
    QObject(parent)
{
    this->data = data;
    this->params = params;
    
    // initialize permutation of tests
    permutation = new Permutation(data->numberOfTests());
    
    this->calculated = false;
    
    switch (params->getCaseToCalculate())
    {
    case ALL:
        for (int i=0; i<NRESULTS; i++)
        {
            available_results[i] = true;
        }
        n_results = NRESTOT;
        break;
    case SENONLY:
        for (int i=0; i<NRESULTS; i++)
        {
            available_results[i] = false;
        }
        available_results[SEN] = true;
        n_results = 3; 
        break;
    case SPEONLY:
        for (int i=0; i<NRESULTS; i++)
        {
            available_results[i] = false;
        }
        available_results[SPE] = true;
        n_results = 3;
        break;
    }
    
    QStringList header;
    for (int i=0; i<NRESULTS; i++)
    {
        if (available_results[i])
        {
            header << HEADER[i];
        }
    }
    
    confidence_intervals = new ResultsTable(permutation, true);
    confidence_intervals->setHorizontalHeader(header);
    
    header = data->getHeader();
    header.removeAt(params->getGoldStandard());
    
    confidence_intervals->setVerticalHeader(header);
    confidence_intervals->higlight = &ci_hl;
    
    for (int i=0; i<NRESULTS; i++)
    {
        pc_pv[i] = new ResultsTable(permutation);
        
        pc_pv[i]->setHorizontalHeader(header);
        pc_pv[i]->setVerticalHeader(header);
        pc_pv[i]->higlight = &pc_hl[i];
        
        pc_ci[i] = new ResultsTable(permutation);
        
        pc_ci[i]->setHorizontalHeader(header);
        pc_ci[i]->setVerticalHeader(header);
        pc_ci[i]->higlight = &pc_hl[i];
    }
}

Results::~Results()
{
    delete confidence_intervals;
    for (int i=0; i<NRESULTS; i++)
    {
        delete pc_pv[i];
        delete pc_ci[i];
    }
    
    delete permutation;
}

void Results::buildHighlightTable(ResultsTable *table, QList<BoolList> *hl_table)
{
    int n_rows = table->rowCount();
    int n_cols = table->columnCount();
    
    hl_table->clear();
    
    double pvalue = params->getPvalue();
    
    for (int i=0; i<n_rows; i++)
    {
        hl_table->append(BoolList());
        
        EntryList row = table->row(i);
        
        for (int j=0; j<n_cols; j++)
        {
            Entry e = row.at(j);
            
            if (e.type==PV)
            {
                if (e.value.at(EST) < pvalue)
                {
                    hl_table->last().append(true);
                }
                else
                {
                    hl_table->last().append(false);
                }
            }
            else
            {
                hl_table->last().append(false);
            }
        }
    }
}

void Results::buildHighlightTables()
{
    buildHighlightTable(confidence_intervals, &ci_hl);
    for (int i=0; i<NRESULTS; i++)
    {
        buildHighlightTable(pc_pv[i], &pc_hl[i]);
    }
}

void Results::sortBy(int id)
{
    if (this->calculated)
    {
        QList<double> column = confidence_intervals->column(id);
        
        permutation->sort(&column);
        
        emit resultsChanged();
    }
    else
    {
        return;
    }
}
