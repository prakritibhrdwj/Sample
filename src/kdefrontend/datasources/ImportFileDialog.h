/***************************************************************************
    File                 : ImportFileDialog.h
    Project              : LabPlot
    Description          : import data dialog
    --------------------------------------------------------------------
    Copyright            : (C) 2008 by Stefan Gerlach
    Email (use @ for *)  : stefan.gerlach*uni-konstanz.de, alexander.semke*web.de

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef IMPORTFILEDIALOG_H
#define IMPORTFILEDIALOG_H

#include <QtGui>
#include <KDialog>
class ImportFileWidget;
class FileDataSource;
class TreeViewComboBox;

class ImportFileDialog: public KDialog {
  Q_OBJECT

  public:
	ImportFileDialog(QWidget*);
	void saveSettings(FileDataSource*) const;
	void setModel(QAbstractItemModel * model);
	void setCurrentIndex(const QModelIndex&);
	
  private:
	QVBoxLayout* vLayout;
	ImportFileWidget *importFileWidget;
	QFrame* frameAddTo;
	TreeViewComboBox* cbAddTo;
	QWidget* mainWidget;
	
  private slots:
	void toggleOptions();
};

#endif //IMPORTFILEDIALOG_H