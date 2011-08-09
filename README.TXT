
 ABOUT
 =====

   The BDTcomparator (Binary Diagnostic Tests Comparator) facilitates
   the selection of the best performing binary classification model or
   binary diagnostic procedure from the many possible alternatives by
   comparing their predictions with a known output measured with the use
   of a system recognized as the gold standard.

   The program calculates the estimates of accuracy, sensitivity,
   specificity, predictive values and diagnostic likelihood ratios along
   with appropriate confidence intervals. Furthermore all pairwise
   comparisons with respect to above mentioned measures are calculated.

   The program is implemented in an object-oriented fashion using C++
   programming language and Boost Math library <http://www.boost.org/>.
   The Graphical User Interface was made with Qt cross-platform
   application framework <http://qt.nokia.com>.

   The input data has to be given in a tab-delimited text file
   (an example data-file 'example_dataset.txt' is available with
   program). The first line has to contain variables names, any
   subsequent line represents a row of data (valid values are either
   ones or zeros). The output is saved as a tab-delimited text file.

   Typical usage is as follows:
    - run bdtcomparator
    - open an input data file - the first icon in the top menu,
    - set options on the right side of the program window,
    - calculate the output - the second icon in the top menu,
    - in the output tab use the combo box to browse through results,
    - save the output to a text file - the third icon in the top menu.
    
    
 REQUIREMENTS
 ============

    - Qt4 SDK <http://qt.nokia.com/downloads>
    - Boost Math library <http://www.boost.org/>
    - for Windows platform MinGW <http://http://www.mingw.org/>
    
    
 BUILD
 =====
 
   Linux
   _____
 
   Edit bdtcomparator.pro file to match your environment, see qmake manual
   at <http://doc.qt.nokia.com/latest/qmake-manual.html>
 
   The QtCreator is recommended for building. Otherwise do the following:
     
   - make build directory
    
    $ mkdir bdtcomparator-linux-build
    $ cd bdtcomparator-linux-build
   
   - run qmake  
   
    $ qmake PRO_FILE -spec SPEC -r
    
     where PRO_FILE is a path to the bdtcomparator.pro file and SPEC
     is platform and compiler specific information i.e. for Linux x86
     with gcc compiler:
    
    $ qmake ../bdtcomparator-stable/bdtcomparator.pro -spec linux-g++-32 -r
    
   - run make
    
    $ make
   
   - run bdtcomparator
   
    $ ./bdtcomparator 


   Windows
   _______
 
   Edit bdtcomparator.pro file to match your environment, see qmake manual
   at <http://doc.qt.nokia.com/latest/qmake-manual.html>.
 
   The QtCreator is recommended for building. Otherwise do the following:
     
   - make build directory
    
    $ mkdir bdtcomparator-linux-build
    $ cd bdtcomparator-linux-build
   
   - run qmake  
   
    $ qmake PRO_FILE -spec SPEC -r
    
     where PRO_FILE is a path to the bdtcomparator.pro file and SPEC
     is platform and compiler specific information i.e. for Windows
     with gcc compiler:
    
    $ qmake ../bdtcomparator-stable/bdtcomparator.pro -spec win32-g++ -r
    
   - run mingw32-make
    
    $ mingw32-make
   
   - run bdtdcomparator.exe
   
    $ bdtcomparator.exe


 LICENSES
 ========

   Binary Diagnostic Tests Comparator v1.0
   _______________________________________

   Copyright (C) 2011 Kamil Fijorek (kamil.fijorek@uek.krakow.pl)
   Copyright (C) 2011 Damian Fijorek (damianfijorek@gmaial.com)
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.


   Qt GUI Toolkit
   ______________

   The Qt GUI Toolkit is Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
   Contact: Nokia Corporation (qt-info@nokia.com)

   You may use, distribute and copy the Qt GUI Toolkit under the terms of
   GNU Lesser General Public License version 2.1.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
   
