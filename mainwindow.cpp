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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowState(Qt::WindowMaximized);
    
    ui->menuBar->setVisible(false);
    
    this->params = new Params();
    this->data = NULL;
    this->results = NULL;
    this->current_result = NULL;
    this->calculator = new Calculator(NULL, NULL, params);
    
    /* params */
    QObject::connect(params, SIGNAL(paramsChanged()), this, SLOT(clearResults()));
    
    /* Gold Standard */
    QObject::connect(ui->GScomboBox, SIGNAL(currentIndexChanged(int)), params, SLOT(setGoldStandard(int)));
    QObject::connect(params, SIGNAL(gsChanged(int)), ui->tableView, SLOT(reset()));
    QObject::connect(params, SIGNAL(gsChanged(int)), this, SLOT(caseToCalculate(int)));
    QObject::connect(params, SIGNAL(gsChanged(int)), ui->resultsComboBox, SLOT(clear()));
    
    /* Confidence Level */
    QObject::connect(ui->confidenceLevelSpinBox, SIGNAL(valueChanged(double)), params, SLOT(setConfidenceLevel(double)));
    
    /* Pairwise comparision Confidence Level */
    QObject::connect(ui->pcConfidenceLevelSpinBox, SIGNAL(valueChanged(double)), params, SLOT(setPvalue(double)));
        
    /* Calculator */
    QObject::connect(ui->actionCalculate, SIGNAL(triggered()), this, SLOT(calculate()));
    QObject::connect(calculator, SIGNAL(calculated(bool)), ui->actionSave_Results, SLOT(setEnabled(bool)));
    QObject::connect(calculator, SIGNAL(calculated(bool)), ui->resultsView, SLOT(reset()));
    QObject::connect(calculator, SIGNAL(calculated(bool)), ui->resultsView, SLOT(resizeColumnsToContents()));
    QObject::connect(calculator, SIGNAL(calculated(bool)), ui->actionCalculate, SLOT(setDisabled(bool)));
    
    /* Results */
    QObject::connect(ui->resultsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setResults(int)));
    
    /* Sort */
    QObject::connect(ui->sortGroupBox, SIGNAL(toggled(bool)), ui->sortComboBox, SLOT(setEnabled(bool)));
    QObject::connect(ui->sortGroupBox, SIGNAL(toggled(bool)), params, SLOT(setSorted(bool)));
    QObject::connect(ui->sortGroupBox, SIGNAL(toggled(bool)), this, SLOT(sortResults(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
    
    delete params;
    delete calculator;
    
    if (data!=NULL)
    {
        delete data;        
    }
    
    if (results!=NULL)
    {
        delete results;
    }
    
}

void MainWindow::on_actionOpen_triggered()
{
    input_file = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Tab-delimited text file (*.txt)"));
    if (input_file.isNull())
    {
        return;
    }
        
    output_file = QString();
    
    this->clearData();
    
    data = new DataTable(input_file, params);
    
    int cols = data->columnCount();
    
    if (cols==0 || cols==1)
    {
        QMessageBox msgBox;
        QString message = "Load at least 2 columns of data.";
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        
        this->clearData();
        
        return;
    }
    else if (cols==2)
    {
        ui->resultsComboBox->setCurrentIndex(0);
        ui->resultsComboBox->setEnabled(false);
    }
    else
    {
        ui->resultsComboBox->setEnabled(true);
    }
    
    calculator->setData(data);
    
    ui->tableView->setModel(data);
    ui->tableView->resizeColumnsToContents();
    
    ui->GScomboBox->addItems(data->getHeader());
    ui->GScomboBox->setEnabled(true);
    ui->GScomboBox->setCurrentIndex(0);
    
    this->initResults();
    
    ui->actionCalculate->setEnabled(true);
}

void MainWindow::on_actionSave_Results_triggered()
{
    if (output_file.isNull())
    {
        output_file = input_file;
        int i = output_file.length() - 4;
        output_file.insert(i, "_out");
    }
    
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Results"), output_file, tr("Tab-delimited text file (*.txt)"));
    if (filename.isNull())
    {
        return;
    }
    
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream output(&file);
        
        ResultsTable *res = NULL;
        
        int n = results->numberOfResults();
        if (data->columnCount()==2)
        {
            n=1;
        }
        
        for (int i=0; i<n; i++)
        {
            if (i>0)
            {
                int id = results_map[i] - 1;
                if (id%2==0)
                {
                    res = results->pc_pv[id/2];
                }
                else
                {
                    res = results->pc_ci[id/2];
                }
            }
            else
            {
                res = results->confidence_intervals;
            }
            
            output << results_list[i] << "\n";
            for (int j=0; j<3; j++)
            {
                if (!res->info[j].isEmpty())
                {
                    output << res->info[j] << "\n";
                }
            }
            
            QStringList row;
            row << "" << res->getHorizontalHeader();
            output << row.join("\t");
            output << "\n";
            
            int n_rows = res->rowCount();
            for (int j=0; j<n_rows; j++)
            {
                row.clear();
                row << res->getVerticalHeader().at(j) << res->getRow(j);
                output << row.join("\t");
                output << "\n";
            }
            output << "\n";
        }
        
        file.close();
    }
}

void MainWindow::clearData()
{
    if (data!=NULL)
    {
        data->disconnect();
        
        ui->tableView->setModel(NULL);
        ui->GScomboBox->clear();
        ui->GScomboBox->setEnabled(false);
        
        delete data;
        data = NULL;
    }
}

void MainWindow::setResults(int id)
{
    if (results!=NULL)
    {
        if (id>0)
        {
            id = results_map[id] - 1;
            if (id%2==0)
            {
                current_result = results->pc_pv[id/2];
            }
            else
            {
                current_result = results->pc_ci[id/2];
            }
        }
        else
        {
            current_result = results->confidence_intervals;
        }
        
        ui->resultsView->setModel(current_result);
        ui->resultsView->resizeColumnsToContents();
        
        ui->infoLabel1->setText(current_result->info[0]);
        ui->infoLabel2->setText(current_result->info[1]);
        ui->infoLabel3->setText(current_result->info[2]);
    }
}

void MainWindow::clearResults()
{
    if (results!=NULL)
    {
        ui->resultsView->setModel(NULL);
        ui->actionSave_Results->setEnabled(false);
        
        results->disconnect();
        current_result = NULL;
        
        delete results;
        
        results = new Results(data, params);
        
        if (ui->resultsComboBox->count()==0)
        {
            this->mapResults();
        }
        
        calculator->setResults(results);
        
        ui->infoLabel1->setText("");
        ui->infoLabel2->setText("");
        ui->infoLabel3->setText("");
        
        ui->actionCalculate->setEnabled(true);
        
        /* Sort criteria */
        QObject::connect(ui->sortComboBox, SIGNAL(currentIndexChanged(int)), results, SLOT(sortBy(int)));
        QObject::connect(results, SIGNAL(resultsChanged()), this, SLOT(updateResults()));
    }
}

void MainWindow::initResults()
{
    if (results!=NULL)
    {
        ui->resultsView->setModel(NULL);
        
        results->disconnect();
        current_result = NULL;
        
        delete results;
    }
    
    results = new Results(data, params);
    
    this->mapResults();
    
    calculator->setResults(results);
    
    ui->actionCalculate->setEnabled(true);
    
    /* Sort criteria */
    QObject::connect(ui->sortComboBox, SIGNAL(currentIndexChanged(int)), results, SLOT(sortBy(int)));
    QObject::connect(results, SIGNAL(resultsChanged()), this, SLOT(updateResults()));
}

void MainWindow::mapResults()
{
    ui->resultsComboBox->clear();
    ui->resultsComboBox->addItem(RESULTS[0]);
    results_list[0] = RESULTS[0];
    results_map[0] = 0;
    
    int r = 1;
    for (int i=1; i<NRESTOT; i++)
    {
        int id = i - 1;
        if (results->toCalculate(id/2))
        {
            results_list[r] = RESULTS[i];
            ui->resultsComboBox->addItem(RESULTS[i]);
            results_map[r] = i;
            r++;
        }
    }
    
    ui->sortComboBox->clear();
    
    for (int i=0; i<NRESULTS; i++)
    {
        if (results->toCalculate(i))
        {
            ui->sortComboBox->addItem(SORT_BY[i]);
        }
    }
}

void MainWindow::updateResults()
{
    if (current_result!=NULL)
    {
        current_result->updateAll();
    }
}

void MainWindow::sortResults(bool enabled)
{
    if (results!=NULL && results->areCalculated())
    {
        if (enabled)
        {
            int column = ui->sortComboBox->currentIndex();
            results->sortBy(column);
        }
        else
        {
            results->resetOrder();
        }
    }
}

void MainWindow::caseToCalculate(int gs)
{
    if (gs==-1)
    {
        return;
    }
    
    bool senonly = true;
    bool speonly = true;
    
    int n_rows = data->rowCount();
    
    for (int i=0; i<n_rows; i++)
    {
        if (!data->isActive(i))
        {
            continue;
        }
        
        if (data->row(i).at(gs)!="1")
        {
            senonly = false;
            break;
        }
    }
    
    if (senonly)
    {
        params->setCaseToCalculate(SENONLY);
        return;
    }
    
    for (int i=0; i<n_rows; i++)
    {
        if (!data->isActive(i))
        {
            continue;
        }
        
        if (data->row(i).at(gs)!="0")
        {
            speonly = false;
            break;
        }
    }
    
    if(speonly)
    {
        params->setCaseToCalculate(SPEONLY);
        return;
    }
    
    params->setCaseToCalculate(ALL);
    return;
}

void MainWindow::calculate()
{   
    calculator->calculate();
    
    ui->tabWidget->setCurrentIndex(1);
    this->sortResults(params->isSorted());
    this->setResults(ui->resultsComboBox->currentIndex());
}

void MainWindow::on_actionAbout_triggered()
{
    dialog.show();
}
