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

#include "calculator.hpp"

Calculator::Calculator(DataTable *data, Results *results, Params *params, QObject *parent) :
    QObject(parent)
{
    this->data = data;
    this->results = results;
    this->params = params;
}

Entry Calculator::confidenceInterval(const double y, const double n)
{
    double alpha = 1.0 - params->getConfidenceLevel();
    double est, low, upp;

    if (y==0.0)
    {
        low = 0.0;
    }
    else
    {
        boost::math::fisher_f low_f(2.0*y, 2.0*(n-y+1.0));
        low = 1.0 / (1.0 + (n - y + 1.0) / (y * boost::math::quantile(low_f, alpha/2.0)));
    }
    
    if (y==n)
    {
        upp = 1.0;
    }
    else
    {
        boost::math::fisher_f upp_f(2.0*(y+1.0), 2.0*(n-y));
        double upp = 1.0 / (1.0 + (n - y) / ((y + 1.0) * boost::math::quantile(upp_f, 1.0-alpha/2.0)));
    }
    
    est = y / n;

    QList<double> output;
    output << est << low << upp;
    
    return Entry(CI, output);
}

EntryList Calculator::confidenceIntervals(int column)
{
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    double d = 0.0;
    
    int gc = params->getGoldStandard();
    
    int n_rows = data->rowCount();
    
    for (int i=0; i<n_rows; i++)
    {
        if (!data->isActive(i))
        {
            continue;
        }
        
        QStringList row = data->row(i);
        
        QString row_symbol;
        row_symbol = "9" + row.at(gc) + row.at(column);
        
        switch (row_symbol.toInt())
        {
        case 911:
            a++;
            break;
        case 901:
            b++;
            break;
        case 910:
            c++;
            break;
        case 900:
            d++;
        }
    }
    
    EntryList output;
    
    if (results->toCalculate(ACC)) output.append(confidenceInterval(a+d, a+b+c+d)); // Diagnostic accuracy
    if (results->toCalculate(SEN)) output.append(confidenceInterval(  a, a+c    )); // Sensitivity
    if (results->toCalculate(SPE)) output.append(confidenceInterval(  d, b+d    )); // Specificity
    
    if (a<0.5)
    {
        a = 0.1;
    }
    if (b<0.5)
    {
        b = 0.1;
    }
    if (c<0.5)
    {
        c = 0.1;
    }
    if (d<0.5)
    {
        d = 0.1;
    }
    
    if (results->toCalculate(PPV)) output.append(confidenceInterval(  a, a+b    )); // Positive Predictive Value
    if (results->toCalculate(NPV)) output.append(confidenceInterval(  d, c+d    )); // Negative Predictive Value
    
    double q = 1.0 - ((1.0 - params->getConfidenceLevel()) / 2.0);
    double p1, p2;
    
    boost::math::normal normal;
    
    double z = boost::math::quantile(normal, q);
    
    if (results->toCalculate(LRP))
    {
        p1 = a / (a + c);
        p2 = b / (b + d);
        
        QList<double> lrpos;
        lrpos << p1 / p2;
        lrpos << exp(log(lrpos.at(EST)) - z * sqrt((1 - p1) / a + (1 - p2) / b));
        lrpos << exp(log(lrpos.at(EST)) + z * sqrt((1 - p1) / a + (1 - p2) / b));
        
        output.append(Entry(CI, lrpos));
    }
    
    if (results->toCalculate(LRN))
    {
        p1 = c / (a + c);
        p2 = d / (b + d);
        
        QList<double> lrneg;
        lrneg << p1 / p2;
        lrneg << exp(log(lrneg.at(EST)) - z * sqrt((1 - p1) / c + (1 - p2) / d));
        lrneg << exp(log(lrneg.at(EST)) + z * sqrt((1 - p1) / c + (1 - p2) / d));
        
        output.append(Entry(CI, lrneg));
    }
    
    return output;
}

void Calculator::pairwiseComparision(QList<QStringList> *input, ResultsTable *out_pv, ResultsTable *out_ci)
{
    int n_rows = input->length();
    int n_cols = data->columnCount();
    
    int m = n_cols - 1;
    
    int gc = params->getGoldStandard();
    
    double q = 1.0 - (params->getPvalue() / 2.0);
    
    boost::math::normal normal;
    boost::math::chi_squared chisq1(1);
    
    double z = boost::math::quantile(normal, q);
    
    QVector<double> row_sums(n_rows, 0.0);
    QVector<double> col_sums(n_cols, 0.0);
    
    double sum = 0.0;
    
    for (int i=0; i<n_rows; i++)
    {
        QStringList row = input->at(i);
        
        for (int j=0; j<n_cols; j++)
        {
            if (j==gc)
            {
                continue;
            }
            
            double value = row.at(j).toDouble();
            
            row_sums[i] += value;
            col_sums[j] += value;
            
            sum += value;
        }
    }
        
    if (m>2)
    {
        double cochran_Q;
        
        boost::math::chi_squared chisq2(m-1);
        
        double row_sum_square = 0.0;
        double col_sum_square = 0.0;
        
        for (int i=0; i<n_rows; i++)
        {
            row_sum_square += row_sums[i] * row_sums[i];
        }
        
        for (int j=0; j<n_cols; j++)
        {
            col_sum_square += col_sums[j] * col_sums[j];
        }
        
        double denom = (m - 1) * (m * col_sum_square - sum * sum);
        if (denom==0)
        {
            cochran_Q = 0.0;
        }
        else
        {
            cochran_Q = denom / (m * sum - row_sum_square);
        }
        
        double pvalue = 1.0 - boost::math::cdf(chisq2, cochran_Q);
        
        out_pv->info[0] = "Cochran's Q = " + QString::number(cochran_Q, 'f', 2);
        out_pv->info[1] = "p-value     = " + QString::number(pvalue, 'f', 4);
    }

    out_ci->info[0] = "Conf. level = " + QString::number(1.0 - params->getPvalue(), 'f', 4);
    
    for (int i=0; i<n_cols; i++)
    {
        if (i==gc)
        {
            continue;
        }
        
        EntryList row_pv;
        EntryList row_ci;
               
        for (int j=0; j<n_cols; j++)
        {
            if (j==gc)
            {
                continue;
            }
            
            if (i==j)
            {
                row_pv.append(Entry());
                row_ci.append(Entry());
                continue;
            }
            
            double a = 0.0;
            double b = 0.0;
            double c = 0.0;
            double d = 0.0;
            
            for (int k=0; k<n_rows; k++)
            {
                QStringList row = input->at(k);
                
                QString row_symbol;
                row_symbol = "9" + row.at(i) + row.at(j);
                
                switch (row_symbol.toInt())
                {
                case 911:
                    a++;
                    break;
                case 901:
                    b++;
                    break;
                case 910:
                    c++;
                    break;
                case 900:
                    d++;
                }
            }
            
            double n = a + b + c + d;
            
            double mc;
            
            if (b+c<1.0)
            {
                mc = 0.0;
            }
            else
            {
                mc = (abs(b - c) - 1.0);
                mc = mc * mc / (b + c);
            }
            
            QList<double> pval;
            pval << 1.0 - boost::math::cdf(chisq1, mc);
            
            row_pv.append(Entry(PV, pval));
            
            double p1 = col_sums[i] / n_rows;
            double p2 = col_sums[j] / n_rows;
            
            double std_err = sqrt(b + c - (b - c) * (b - c) / n) / n;
            
            QList<double> ci;

            double est = p1 - p2;

            double low = est - z * std_err - 1.0 / n;
            if (low<-1.0)
            {
                low = -1.0;
            }

            double upp = est + z * std_err + 1.0 / n;
            if (upp>1.0)
            {
                upp = 1.0;
            }

            ci << est << low << upp;
            
            row_ci.append(Entry(CI, ci));
        }
        
        out_pv->appendRow(row_pv);
        out_ci->appendRow(row_ci);
    }
}

void Calculator::pairwisePredictiveValue(const ResultsTable *ci_table, ResultsTable *out_ppv_pv, ResultsTable *out_npv_pv, ResultsTable *out_ppv_ci, ResultsTable *out_npv_ci)
{
    int n_rows = data->rowCount();
    int n_cols = data->columnCount();
    
    int gc = params->getGoldStandard();
    
    double q = 1.0 - (params->getPvalue() / 2.0);
    
    out_ppv_ci->info[0] = "Conf. level = " + QString::number(1.0 - params->getPvalue(), 'f', 4);
    out_npv_ci->info[0] = "Conf. level = " + QString::number(1.0 - params->getPvalue(), 'f', 4);
    
    boost::math::normal normal;
    
    double z = boost::math::quantile(normal, q);
    
    for (int i=0; i<n_cols; i++)
    {
        if (i==gc)
        {
            continue;
        }
        
        EntryList row_ppv_pv;
        EntryList row_npv_pv;
        EntryList row_ppv_ci;
        EntryList row_npv_ci;
        
        for (int j=0; j<n_cols; j++)
        {
            if (j==gc)
            {
                continue;
            }
            
            if (i==j)
            {
                row_ppv_pv.append(Entry());
                row_npv_pv.append(Entry());
                row_ppv_ci.append(Entry());
                row_npv_ci.append(Entry());
                continue;
            }
            
            QVector<double> n(9, 0.0);
            QVector<double> p(9, 0.0);
            
            for (int k=0; k<n_rows; k++)
            {
                QStringList row = data->row(k);
                
                QString row_symbol;
                row_symbol = "9" + row.at(gc) + row.at(i) + row.at(j);
                
                switch (row_symbol.toInt())
                {
                case 9011:
                    n[1]++;
                    break;
                case 9010:
                    n[2]++;
                    break;
                case 9001:
                    n[3]++;
                    break;
                case 9000:
                    n[4]++;
                    break;
                case 9111:
                    n[5]++;
                    break;
                case 9110:
                    n[6]++;
                    break;
                case 9101:
                    n[7]++;
                    break;
                case 9100:
                    n[8]++;
                }
            }
                
            for (int l=1; l<9; l++)
            {
                if (n[l]<0.5)
                {
                    n[l] = 0.1;
                }
                
                n[0] += n[l];
            }
            
            for (int l=1; l<9; l++)
            {                   
                p[l] = n[l] / n[0];
            }
            
            int ir = i<gc ? i : i-1;
            int jr = j<gc ? j : j-1;
            
            double ppvi = ci_table->row(ir).at(PPV).value.at(EST);
            double npvi = ci_table->row(ir).at(NPV).value.at(EST);
            double ppvj = ci_table->row(jr).at(PPV).value.at(EST);
            double npvj = ci_table->row(jr).at(NPV).value.at(EST);
            
            double rppv = ppvi / ppvj;
            double rnpv = npvi / npvj;
            
            double sigma2_log_rppv
                    = 1.0 / ((p[5] + p[7]) * (p[5] + p[6]))
                    * (p[6] * (1.0 - ppvj)
                       + p[5] * (ppvj - ppvi)
                       + 2.0 * (p[7] + p[3]) * ppvi * ppvj
                       + p[7] * (1.0 - 3.0 * ppvi));
            
            double sigma2_log_rnpv
                    = (npvj * (p[4] - p[3] - 2.0 * (p[4] + p[8]) * npvi) + p[2] + p[3] - npvi * (p[2] - p[4])) / ((p[2] + p[4]) * (p[3] + p[4]));
            
            double ppvu = log(rppv) / sqrt(sigma2_log_rppv / n[0]);
            double npvu = log(rnpv) / sqrt(sigma2_log_rnpv / n[0]);
            
            QList<double> ppv_pv;
            ppv_pv << 2.0 * (1.0 - boost::math::cdf(normal, fabs(ppvu)));
            row_ppv_pv.append(Entry(PV, ppv_pv));
            
            QList<double> npv_pv;
            npv_pv << 2.0 * (1.0 - boost::math::cdf(normal, fabs(npvu)));
            row_npv_pv.append(Entry(PV, npv_pv));
            
            QList<double> ppv_ci;
            double ppv_est = log(rppv);
            double ppv_std_err = sqrt(sigma2_log_rppv / n[0]);
            ppv_ci << exp(ppv_est);
            ppv_ci << exp(ppv_est - z * ppv_std_err);
            ppv_ci << exp(ppv_est + z * ppv_std_err);
            row_ppv_ci.append(Entry(CI, ppv_ci));
            
            QList<double> npv_ci;
            double npv_est = log(rnpv);
            double npv_std_err = sqrt(sigma2_log_rnpv / n[0]);
            npv_ci << exp(npv_est);
            npv_ci << exp(npv_est - z * npv_std_err);
            npv_ci << exp(npv_est + z * npv_std_err);
            row_npv_ci.append(Entry(CI, npv_ci));
        }
        
        out_ppv_pv->appendRow(row_ppv_pv);
        out_npv_pv->appendRow(row_npv_pv);
        out_ppv_ci->appendRow(row_ppv_ci);
        out_npv_ci->appendRow(row_npv_ci);
    }
}

void Calculator::pairwiseLikelihoodRatio(const ResultsTable *ci_table, ResultsTable *out_lrp_pv, ResultsTable *out_lrn_pv, ResultsTable *out_lrp_ci, ResultsTable *out_lrn_ci)
{
    int n_rows = data->rowCount();
    int n_cols = data->columnCount();
    
    int gc = params->getGoldStandard();
    
    double q = 1.0 - (params->getPvalue() / 2.0);
    
    out_lrp_ci->info[0] = "Conf. level = " + QString::number(1.0 - params->getPvalue(), 'f', 4);
    out_lrn_ci->info[0] = "Conf. level = " + QString::number(1.0 - params->getPvalue(), 'f', 4);
    
    boost::math::normal normal;
    
    double z = boost::math::quantile(normal, q);
    
    for (int i=0; i<n_cols; i++)
    {
        if (i==gc)
        {
            continue;
        }
        
        EntryList row_lrp_pv;
        EntryList row_lrn_pv;
        EntryList row_lrp_ci;
        EntryList row_lrn_ci;
        
        for (int j=0; j<n_cols; j++)
        {
            if (j==gc)
            {
                continue;
            }
            
            if (i==j)
            {
                row_lrp_pv.append(Entry());
                row_lrn_pv.append(Entry());
                row_lrp_ci.append(Entry());
                row_lrn_ci.append(Entry());
                continue;
            }
            
            QVector<double> n(9, 0.0);
            
            for (int k=0; k<n_rows; k++)
            {
                QStringList row = data->row(k);
                
                QString row_symbol;
                row_symbol = "9" + row.at(gc) + row.at(i) + row.at(j);
                
                switch (row_symbol.toInt())
                {
                case 9011:
                    n[1]++;
                    break;
                case 9010:
                    n[2]++;
                    break;
                case 9001:
                    n[3]++;
                    break;
                case 9000:
                    n[4]++;
                    break;
                case 9111:
                    n[5]++;
                    break;
                case 9110:
                    n[6]++;
                    break;
                case 9101:
                    n[7]++;
                    break;
                case 9100:
                    n[8]++;
                }
            }
                
            for (int l=1; l<9; l++)
            {
                if (n[l]<0.5)
                {
                    n[l] = 0.1;
                }
                
                n[0] += n[l];
            }
            
            double q11 = n[1] / n[0];
            double q10 = n[2] / n[0];
            double q01 = n[3] / n[0];
            double q00 = n[4] / n[0];
            
            double p11 = n[5] / n[0];
            double p10 = n[6] / n[0];
            double p01 = n[7] / n[0];
            double p00 = n[8] / n[0];
            
            int ir = i<gc ? i : i-1;
            int jr = j<gc ? j : j-1;
            
            double lrpi = ci_table->row(ir).at(LRP).value.at(EST);
            double lrni = ci_table->row(ir).at(LRN).value.at(EST);
            double lrpj = ci_table->row(jr).at(LRP).value.at(EST);
            double lrnj = ci_table->row(jr).at(LRN).value.at(EST);
            
            double rlrp = lrpi / lrpj;
            double rlrn = lrni / lrnj;
            
            double mp[8] = {
                0.0,
                -1.0 / (p01 + p11),
                1.0 / (p10 + p11),
                (p01 - p10) / ((p01 + p11) * (p10 + p11)),
                0.0,
                -1.0 / (q01 + q11),
                1.0 / (q10 + q11),
                (q01 - q10) / ((q01 + q11) * (q10 + q11))
            };
            
            double mn[8] = {
                (p10 - p01) / ((p00 + p01) * (p00 + p10)),
                1.0 / (p00 + p01),
                -1.0 / (p00 + p10),
                0.0,
                (q01 - q10) / ((q00 + q01) * (q00 + q10)),
                -1.0 / (q00 + q01),
                1.0 / (q00 + q10),
                0.0
            };
            
            double w[8] = {p00, p01, p10, p11, q00, q01, q10, q11};
            double var[8][8];
            
            for (int k=0; k<8; k++)
            {
                for (int l=0; l<8; l++)
                {
                    var[k][l] = -w[k] * w[l] / n[0];
                }
            }
            
            for (int k=0; k<8; k++)
            {
                var[k][k] = (1.0 - w[k]) * w[k] / n[0];
            }
            
            double vlrp = 0.0;
            double vlrn = 0.0;
            
            for (int k=0; k<8; k++)
            {
                for (int l=0; l<8; l++)
                {
                    vlrp += mp[k]*var[k][l]*mp[l];
                    vlrn += mn[k]*var[k][l]*mn[l];
                }
            }
            
            double lrpu = log(rlrp) / sqrt(vlrp);
            double lrnu = log(rlrn) / sqrt(vlrn);
            
            QList<double> lrp_pv;
            lrp_pv << 2.0 * (1.0 - boost::math::cdf(normal, fabs(lrpu)));
            row_lrp_pv.append(Entry(PV, lrp_pv));
            
            QList<double> lrn_pv;
            lrn_pv << 2.0 * (1.0 - boost::math::cdf(normal, fabs(lrnu)));;
            row_lrn_pv.append(Entry(PV, lrn_pv));
            
            QList<double> lrp_ci;
            double lrp_est = log(rlrp);
            double lrp_std_err = sqrt(vlrp);
            lrp_ci << exp(lrp_est);
            lrp_ci << exp(lrp_est - z * lrp_std_err);
            lrp_ci << exp(lrp_est + z * lrp_std_err);
            row_lrp_ci.append(Entry(CI, lrp_ci));
            
            QList<double> lrn_ci;
            double lrn_est = log(rlrn);
            double lrn_std_err = sqrt(vlrn);
            lrn_ci << exp(lrn_est);
            lrn_ci << exp(lrn_est - z * lrn_std_err);
            lrn_ci << exp(lrn_est + z * lrn_std_err);
            row_lrn_ci.append(Entry(CI, lrn_ci));
        }
        
        out_lrp_pv->appendRow(row_lrp_pv);
        out_lrn_pv->appendRow(row_lrn_pv);
        out_lrp_ci->appendRow(row_lrp_ci);
        out_lrn_ci->appendRow(row_lrn_ci);
    }
}

void Calculator::calculate()
{
    int n_rows = data->rowCount();
    int n_cols = data->columnCount();
    int gc = params->getGoldStandard();
    
    for (int i=0; i<n_cols; i++)
    {
        if (i==gc)
        {
            continue;
        }
        
        results->confidence_intervals.appendRow(confidenceIntervals(i));
    }
    
    results->confidence_intervals.info[0] = "Conf. level = " + QString::number(params->getConfidenceLevel(), 'f', 4);
    
    if (n_cols>2)
    {
        QList<QStringList> acc;
        QList<QStringList> sen;
        QList<QStringList> spe;
        
        for (int i=0; i<n_rows; i++)
        {
            if (!data->isActive(i))
            {
                continue;
            }
            
            acc.append(QStringList());
            
            QStringList row = data->row(i);
            
            if (row.at(gc)=="1")
            {
                sen.append(row);
            }
            else
            {
                spe.append(QStringList());
                for (int j=0; j<n_cols; j++)
                {
                    if (row.at(j)=="1")
                    {
                        spe.last().append("0");
                    }
                    else
                    {
                        spe.last().append("1");
                    }
                }
            }
            
            for (int j=0; j<n_cols; j++)
            {
                if (row.at(gc)==row.at(j))
                {
                    acc.last().append("1");
                }
                else
                {
                    acc.last().append("0");
                }
            }
        }
        
        if (results->toCalculate(ACC))
            pairwiseComparision(&acc, &results->pc_pv[ACC], &results->pc_ci[ACC]);
        if (results->toCalculate(SEN))
            pairwiseComparision(&sen, &results->pc_pv[SEN], &results->pc_ci[SEN]);
        if (results->toCalculate(SPE))
            pairwiseComparision(&spe, &results->pc_pv[SPE], &results->pc_ci[SPE]);
        
        if (results->toCalculate(PPV) || results->toCalculate(NPV))
            pairwisePredictiveValue(&results->confidence_intervals, &results->pc_pv[PPV], &results->pc_pv[NPV], &results->pc_ci[PPV], &results->pc_ci[NPV]);
        if (results->toCalculate(LRP) || results->toCalculate(LRN))
            pairwiseLikelihoodRatio(&results->confidence_intervals, &results->pc_pv[LRP], &results->pc_pv[LRN], &results->pc_ci[LRP], &results->pc_ci[LRN]);
    }
    
    results->buildHighlightTables();
    
    emit calculated(true);
}
