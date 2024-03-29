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

#ifndef RESULTSTABLE_HPP
#define RESULTSTABLE_HPP

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include <QBrush>

#include "permutation.hpp"

const int EMPTY = -1;
const int PV  = 100;
const int CI  = 200;

const int EST = 0;
const int LOW = 1;
const int UPP = 2;

class Entry
{
public:
    Entry(int type = EMPTY, QList<double> value = QList<double>())
    {
        this->type = type;
        this->value = value;
    }
    
    int type;
    QList<double> value;
};

typedef QList<Entry> EntryList;
typedef QList<bool> BoolList;

const QString format = "%1 (%2;%3)";

class ResultsTable : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    explicit ResultsTable(Permutation *permutation, bool constant_columns = false, QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent=QModelIndex()) const
    {
        if (parent.isValid())
        {
            return 0;
        }
        else
        {
            return results.length();
        }
    }

    int columnCount(const QModelIndex &parent=QModelIndex()) const
    {
        if (parent.isValid())
        {
            return 0;
        }
        else
        {
            return horizontal_header.length();
        }
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        int row, col;
        
        row = permutation->at(index.row());
        
        if (constant_columns)
        {
            col = index.column();
        }
        else
        {
            col =  permutation->at(index.column());
        }
        
        switch (role)
        {
        case Qt::DisplayRole:
        {   
            Entry e = results.at(row).at(col);
            QString out;
            
            switch (e.type)
            {
            case PV:
                out = QString::number(e.value.first(), 'f', 4);
                break;
            case CI:
                out = format
                        .arg(e.value.at(0), 6, 'f', 3)
                        .arg(e.value.at(1), 6, 'f', 3)
                        .arg(e.value.at(2), 6, 'f', 3);
                break;
            default:
                out = "";
            }

            return QVariant(out);
        }
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignCenter);
        case Qt::BackgroundRole:
            if (higlight->at(row).at(col))
            {
                return QVariant(QBrush(QColor(170, 255, 170)));
            }
            else
            {
                return QVariant();
            }
        default:
            return QVariant();
        }
    }
    
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
        switch (role)
        {
        case Qt::DisplayRole:
            if (orientation==Qt::Horizontal)
            {
                if (constant_columns)
                {
                    return QVariant(horizontal_header.at(section));
                }
                else
                {
                    return QVariant(horizontal_header.at(permutation->at(section)));
                }
            }
            else
            {
                return QVariant(vertical_header.at(permutation->at(section)));
            }
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft);
        default:
            return QVariant();
        }
    }
    
    void appendRow(EntryList row, const QModelIndex &parent=QModelIndex())
    {
        int last = results.length();
        
        beginInsertRows(parent, last, last);
        
        results.append(row);
        
        endInsertRows();
    }
    
    void setHorizontalHeader(QStringList header)
    {
        horizontal_header = header;
    }
    
    void setVerticalHeader(QStringList header)
    {
        vertical_header = header;
    }
    
    EntryList row(int x) const
    {
        return results.at(x);
    }
    
    QList<double> column(int x, int type=EST) const
    {
        QList<double> column;
        
        for (int i=0; i<this->rowCount(); i++)
        {
            column.append(results.at(i).at(x).value.at(type));
        }
        
        return column;
    }
    
    QList<BoolList> *higlight;
    QString info[3];
    
signals:

public slots:
    void updateAll()
    {
        int top = 0;
        int bottom = this->rowCount() - 1;
        int left = 0;
        int right = this->columnCount() - 1;
        
        QModelIndex top_left = this->index(top, left);
        QModelIndex bottom_right = this->index(bottom, right);
        
        if (!constant_columns)
        {
            this->headerDataChanged(Qt::Horizontal, left, right);
        }
        
        this->headerDataChanged(Qt::Vertical, top, bottom);
        
        this->dataChanged(top_left, bottom_right);
    }
    
private:
    Permutation *permutation;
    
    QList<EntryList> results;
    QStringList vertical_header;
    QStringList horizontal_header;
    
    //! is the column order constant
    bool constant_columns;
    
};

#endif // RESULTSTABLE_HPP
