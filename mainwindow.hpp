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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>

#include <aboutdialog.hpp>

#include "params.hpp"
#include "datatable.hpp"
#include "results.hpp"
#include "resultstable.hpp"
#include "calculator.hpp"

const QStringList RESULTS = (QStringList()
                             << "Performance measures (point estimates and confidence intervals)"
                             << "Pairwise accuracy p-value"
                             << "Pairwise accuracy confidence intervals"
                             << "Pairwise sensitivity p-value"
                             << "Pairwise sensitivity confidence intervals"
                             << "Pairwise specificity p-value"
                             << "Pairwise specificity confidence intervals"
                             << "Positive Predictive Value p-value"
                             << "Positive Predictive Value confidence intervals"
                             << "Negative Predictive Value p-value"
                             << "Negative Predictive Value confidence intervals"
                             << "Likelihood ratio of a positive test p-value"
                             << "Likelihood ratio of a positive test confidence intervals"
                             << "Likelihood ratio of a negative test p-value"
                             << "Likelihood ratio of a negative test confidence intervals");

const QStringList SORT_BY = (QStringList() << "ACC" << "SEN" << "SPE" << "PPV" << "NPV" << "LRP" << "LRN");

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_Results_triggered();
    
    void clearData();
    
    void setResults(int id);
    void clearResults();
    void initResults();
    void mapResults();
    void updateResults();
    void sortResults(bool enabled);
    
    void caseToCalculate(int gs);
    
    void calculate();
    
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;

    AboutDialog dialog;
    
    Params *params;
    
    QString input_file;
    QString output_file;
    
    QString results_list[NRESTOT];
    int results_map[NRESTOT];
    
    DataTable *data;
    Results *results;
    ResultsTable *current_result;
    Calculator *calculator;
    
};

#endif // MAINWINDOW_H
