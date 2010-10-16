/***************************************************************************
    File                 : LineSymbolCurveDock.h
    Project            : LabPlot
    --------------------------------------------------------------------
    Copyright         : (C) 2010 Alexander Semke (alexander.semke*web.de)
							(replace * with @ in the email addresses)
    Description      : widget for worksheet properties
                           
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

#ifndef LINESYMBOLCURVEDOCK_H
#define LINESYMBOLCURVEDOCK_H

#include <QList>
#include "ui_linesymbolcurvedock.h"

class QTextEdit;
class LineSymbolCurve;
class TreeViewComboBox;
class CurveSymbolFactory;

class LineSymbolCurveDock: public QWidget{
	Q_OBJECT
	
public:
	LineSymbolCurveDock(QWidget *parent);
	void setModel(QAbstractItemModel * model);
	void setCurves(QList<LineSymbolCurve*>);
	
private:
	Ui::LineSymbolCurveDock ui;
	QList<LineSymbolCurve*> m_curvesList;
	bool m_initializing;
	
	QGridLayout *gridLayout;
    QLabel *lName;
    QLineEdit *leName;
    QLabel *lComment;
    QTextEdit *teComment;
    QSpacerItem *verticalSpacer;
	QLabel* lXColumn;
	QLabel* lYColumn;
	TreeViewComboBox* cbXColumn;
	TreeViewComboBox* cbYColumn;
	
	CurveSymbolFactory *symbolFactory;
	
	void updateSymbolStyles();
	void updateSymbolFillingStyles();
	void updateSymbolBorderStyles();
	
	void resizeEvent(QResizeEvent *);
	
private slots:
  	void symbolStyleChanged(int);
	void symbolSizeChanged(int);
	void symbolRotationChanged(int);
	void symbolOpacityChanged(int);
	void symbolFillingStyleChanged(int);
	void symbolFillingColorChanged(const QColor&);
	void symbolBorderStyleChanged(int);
	void symbolBorderColorChanged(const QColor&);
	void symbolBorderWidthChanged(int);
	
	void retranslateUi();
};

#endif
