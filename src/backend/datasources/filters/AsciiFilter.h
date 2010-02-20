/***************************************************************************
File                 : AsciiFilter.h
Project              : LabPlot/SciDAVis
Description          : ASCII I/O-filter
--------------------------------------------------------------------
Copyright            		: (C) 2009 Alexander Semke
Email (use @ for *)  	: alexander.semke*web.de
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
#ifndef ASCIIFILTER_H
#define ASCIIFILTER_H

#include <QStringList>

#include "datasources/filters/AbstractFileFilter.h"
#include "datasources/FileDataSource.h"

class AsciiFilterPrivate;
class AsciiFilter : public QObject, public AbstractFileFilter{
  Q_OBJECT
  Q_INTERFACES(AbstractFileFilter)

  public:
	AsciiFilter();
	~AsciiFilter();

	static QStringList separatorCharacters();
	static QStringList commentCharacters();
	static QStringList predefinedFilters();

	static int columnNumber(const QString & fileName);
	static long lineNumber(const QString & fileName);

	void read(const QString & fileName, FileDataSource* dataSource);
	void write(const QString & fileName, FileDataSource* dataSource);

	void loadFilterSettings(const QString&);
	void saveFilterSettings(const QString&) const;

	void setTransposed(const bool);
	bool isTransposed() const;

	void setCommentCharacter(const QString&);
	QString commentCharacter() const;

	void setSeparatingCharacter(const QString&);
	QString separatingCharacter() const;

	void setAutoModeEnabled(const bool);
	bool isAutoModeEnabled() const;

	void setHeaderEnabled(const bool);
	bool isHeaderEnabled() const;

	void setVectorNames(const QString);
	QString vectorNames() const;

	void setEmptyLinesEnabled(const bool);
	bool emptyLinesEnabled() const;

	void setSimplifyWhitespacesEnabled(const bool);
	bool simplifyWhitespacesEnabled() const;

	void setStartRow(const int);
	int startRow() const;

	void setEndRow(const int);
	int endRow() const;

	void setStartColumn(const int);
	int startColumn() const;

	void setEndColumn(const int);
	int endColumn() const;

  private:
	AsciiFilterPrivate* const d;
};

#endif