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

#ifndef DATATABLE_HPP
#define DATATABLE_HPP

#include <QAbstractTableModel>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QMessageBox>

#include "params.hpp"

class DataTable : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DataTable(const QString &input, Params *params, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        if (parent.isValid())
        {
            return 0;
        }
        else
        {
            return rows.length();
        }
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
        if (parent.isValid())
        {
            return 0;
        }
        else
        {
            return header.length();
        }
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        switch (role)
        {
        case Qt::DisplayRole:
        {
            QString out = rows.at(index.row()).at(index.column());
            return QVariant(out);
        }
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignCenter);
        case Qt::BackgroundRole:
            if (active_rows.at(index.row())==false)
            {
                return QVariant(QBrush(QColor(255, 100, 100)));
            }
            else
            {
                if (params->getGoldStandard()==index.column())
                {
                    return QVariant(QBrush(QColor(255, 170, 0)));
                }
                else
                {
                    return QVariant();
                }
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
                return QVariant(header.at(section));
            }
            else
            {
                return QVariant(section+1);
            }
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft);
        default:
            return QVariant();
        }
    }
    
    QStringList getHeader() const
    {
        return header;
    }
    
    QStringList row(int x) const
    {
        return rows.at(x);
    }
    
    bool isActive(int x) const
    {
        return active_rows.at(x);
    }

signals:

public slots:

private:
    QStringList header;
    QList<bool> active_rows;
    QList<QStringList> rows;
    
    Params *params;
    
};

#endif // DATATABLE_HPP
