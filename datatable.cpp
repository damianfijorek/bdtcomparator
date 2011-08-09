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

#include "datatable.hpp"

DataTable::DataTable(const QString &input, Params *params, QObject *parent) :
    QAbstractTableModel(parent)
{   
    this->params = params;
    
    QFile file(input);

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream data(&file);

        QStringList row = data.readLine().split("\t");

        header = row;

        int length = header.length();

        bool exit = false;
        int row_number = 1;
        int col_number = 0;
        QString value;

        while (!data.atEnd())
        {
            row = data.readLine().split("\t");
            active_rows.append(true);
            rows.append(row);

            for (int i=0; i<length; i++)
            {
                value = row.at(i).simplified();
                if (value!="0" && value!="1" && value!="")
                {
                    col_number = i + 1;
                    exit = true;
                    break;
                }
                
                if (value=="")
                {
                    active_rows.last() = false;
                }
            }

            if (exit)
            {
                header.clear();
                rows.clear();

                QMessageBox msgBox;
                QString message = "Ilegal character '%1' in iput at (row: %2; col: %3).";
                message = message.arg(value, QString::number(row_number), QString::number(col_number));
                msgBox.setText(message);
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.exec();

                break;
            }

            row_number++;
        }
        
        file.close();
    }
}
