/***************************************************************************
File                 : AsciiMatrixFilter.cpp
Project              : LabPlot
Description          : ASCII Matrix I/O-filter
--------------------------------------------------------------------
Copyright            : (C) 2015 Stefan Gerlach (stefan.gerlach@uni.kn)

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
#include "backend/datasources/filters/AsciiMatrixFilter.h"
#include "backend/datasources/filters/AsciiMatrixFilterPrivate.h"
#include "backend/datasources/FileDataSource.h"
#include "backend/core/column/Column.h"

#include <math.h>

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <KLocale>

 /*!
	\class AsciiMatrixFilter
	\brief Manages the import/export of data organized as matrix from/to an ASCII-file.

	\ingroup datasources
 */

AsciiMatrixFilter::AsciiMatrixFilter():AbstractFileFilter(), d(new AsciiMatrixFilterPrivate(this)){

}

AsciiMatrixFilter::~AsciiMatrixFilter(){
	delete d;
}

/*!
  reads the content of the file \c fileName to the data source \c dataSource.
*/
void AsciiMatrixFilter::read(const QString & fileName, AbstractDataSource* dataSource, AbstractFileFilter::ImportMode importMode){
	d->read(fileName, dataSource, importMode);
}


/*!
writes the content of the data source \c dataSource to the file \c fileName.
*/
void AsciiMatrixFilter::write(const QString & fileName, AbstractDataSource* dataSource){
 	d->write(fileName, dataSource);
}

/*!
  loads the predefined filter settings for \c filterName
*/
void AsciiMatrixFilter::loadFilterSettings(const QString& filterName){
	Q_UNUSED(filterName);
}

/*!
  saves the current settings as a new filter with the name \c filterName
*/
void AsciiMatrixFilter::saveFilterSettings(const QString& filterName) const{
	Q_UNUSED(filterName);
}

/*!
  returns the list with the names of all saved
  (system wide or user defined) filter settings.
*/
QStringList AsciiMatrixFilter::predefinedFilters(){
	return QStringList();
}

/*!
  returns the list of all predefined separator characters.
*/
QStringList AsciiMatrixFilter::separatorCharacters(){
	return (QStringList()<<"auto"<<"TAB"<<"SPACE"<<","<<";"<<":"
				  <<",TAB"<<";TAB"<<":TAB"
				  <<",SPACE"<<";SPACE"<<":SPACE");
}

/*!
returns the list of all predefined comment characters.
*/
QStringList AsciiMatrixFilter::commentCharacters(){
	return (QStringList()<<"#"<<"!"<<"//"<<"+"<<"c"<<":"<<";");
}

/*!
    returns the number of columns in the file \c fileName.
*/
int AsciiMatrixFilter::columnNumber(const QString & fileName){
	QString line;
	QStringList lineStringList;

	QFile file(fileName);
	if ( !file.exists() )
		return 0;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return 0;

	QTextStream in(&file);
	line = in.readLine();
	lineStringList = line.split( QRegExp("\\s+")); //TODO
	return lineStringList.size();
}


/*!
  returns the number of lines in the file \c fileName.
*/
long AsciiMatrixFilter::lineNumber(const QString & fileName){
	//TODO: compare the speed of this function with the speed of wc from GNU-coreutils.
	QFile file(fileName);
	if ( !file.exists() )
		return 0;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return 0;

	QTextStream in(&file);
	long rows=0;
	while (!in.atEnd()){
		in.readLine();
		rows++;
	}

	return rows;
}

void AsciiMatrixFilter::setTransposed(const bool b){
	d->transposed=b;
}

bool AsciiMatrixFilter::isTransposed() const{
	return d->transposed;
}

void AsciiMatrixFilter::setCommentCharacter(const QString& s){
	d->commentCharacter=s;
}

QString AsciiMatrixFilter::commentCharacter() const{
	return d->commentCharacter;
}

void AsciiMatrixFilter::setSeparatingCharacter(const QString& s){
	d->separatingCharacter=s;
}

QString AsciiMatrixFilter::separatingCharacter() const{
	return d->separatingCharacter;
}

void AsciiMatrixFilter::setAutoModeEnabled(bool b){
	d->autoModeEnabled=b;
}

bool AsciiMatrixFilter::isAutoModeEnabled() const{
	return d->autoModeEnabled;
}

void AsciiMatrixFilter::setSkipEmptyParts(bool b){
	d->skipEmptyParts=b;
}

bool AsciiMatrixFilter::skipEmptyParts() const{
	return d->skipEmptyParts;
}

void AsciiMatrixFilter::setSimplifyWhitespacesEnabled(bool b){
	d->simplifyWhitespacesEnabled=b;
}

bool AsciiMatrixFilter::simplifyWhitespacesEnabled() const{
	return d->simplifyWhitespacesEnabled;
}

void AsciiMatrixFilter::setStartRow(const int r){
	d->startRow=r;
}

int AsciiMatrixFilter::startRow() const{
	return d->startRow;
}

void AsciiMatrixFilter::setEndRow(const int r){
	d->endRow=r;
}

int AsciiMatrixFilter::endRow() const{
	return d->endRow;
}

void AsciiMatrixFilter::setStartColumn(const int c){
	d->startColumn=c;
}

int AsciiMatrixFilter::startColumn() const{
	return d->startColumn;
}

void AsciiMatrixFilter::setEndColumn(const int c){
	d->endColumn=c;
}

int AsciiMatrixFilter::endColumn() const{
	return d->endColumn;
}

//#####################################################################
//################### Private implementation ##########################
//#####################################################################
AsciiMatrixFilterPrivate::AsciiMatrixFilterPrivate(AsciiMatrixFilter* owner) : q(owner),
	commentCharacter("#"),
	separatingCharacter("auto"),
	autoModeEnabled(true),
	skipEmptyParts(false),
	simplifyWhitespacesEnabled(true),
	transposed(false),
	startRow(0),
	endRow(-1),
	startColumn(0),
	endColumn(-1) {
}

/*!
    reads the content of the file \c fileName to the data source \c dataSource.
    Uses the settings defined in the data source.
*/
void AsciiMatrixFilterPrivate::read(const QString & fileName, AbstractDataSource* dataSource, AbstractFileFilter::ImportMode mode){
	int currentRow=0; //indexes the position in the vector(column)
	int columnOffset=0; //indexes the "start column" in the spreadsheet. Starting from this column the data will be imported.
	QString line;
	QStringList lineStringList;

	QFile file(fileName);
	if ( !file.exists() )
		return;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        	return;

	QTextStream in(&file);


	//TODO implement
	// if (transposed)
	//...

	//skip rows, if required
	for (int i=0; i<startRow; i++){
        //if the number of rows to skip is bigger then the actual number
		//of the rows in the file, then quit the function.
		if( in.atEnd() ) {
			if (mode==AbstractFileFilter::Replace) {
				//file with no data to be imported. In replace-mode clear the spreadsheet
				this->clearDataSource(dataSource);
			}
			return;
		}

		in.readLine();
	}

	//parse the first row:
	//use the first row to determine the number of columns,
	//create the columns and use (optionaly) the first row to name them
	if( in.atEnd() ) {
		if (mode==AbstractFileFilter::Replace) {
			//file with no data to be imported. In replace-mode clear the spreadsheet
			this->clearDataSource(dataSource);
		}
		return;
	}

	line = in.readLine();
	if( simplifyWhitespacesEnabled)
		line = line.simplified();

	QString separator;
	if( separatingCharacter == "auto" ){
		QRegExp regExp("(\\s+)|(,\\s+)|(;\\s+)|(:\\s+)");
		lineStringList = line.split( regExp, QString::SplitBehavior(skipEmptyParts) );

		//determine the separator
		if (lineStringList.size()){
		int length1 = lineStringList.at(0).length();
			if (lineStringList.size()>1){
				int pos2 = line.indexOf(lineStringList.at(1));
				separator = line.mid(length1, pos2-length1);
			}else {
				separator = line.right(line.length()-length1);
			}
		}
	}else {
		separator = separatingCharacter.replace(QString("TAB"), QString("\t"), Qt::CaseInsensitive);
		separator = separatingCharacter.replace(QString("SPACE"), QString(" "), Qt::CaseInsensitive);
		lineStringList = line.split( separator, QString::SplitBehavior(skipEmptyParts) );
	}
// 	qDebug() << "separator '"<<separator << "'";

	if (endColumn == -1)
		endColumn = lineStringList.size()-1; //use the last available column index

	//make sure we have enough columns in the data source.
	//we need in total (endColumn-startColumn+1) columns.
	//Create new columns, if needed.
	Column * newColumn;
	dataSource->setUndoAware(false);
	if (mode==AbstractFileFilter::Append){
		columnOffset=dataSource->childCount<Column>();
		for ( int n=startColumn; n<=endColumn; n++ ){
			newColumn = new Column(QString(""), AbstractColumn::Numeric);
			newColumn->setUndoAware(false);
			dataSource->addChild(newColumn);
		}
	}else if (mode==AbstractFileFilter::Prepend){
		Column* firstColumn = dataSource->child<Column>(0);
		for ( int n=startColumn; n<=endColumn; n++ ){
			newColumn = new Column(QString(""), AbstractColumn::Numeric);
			newColumn->setUndoAware(false);
			dataSource->insertChildBefore(newColumn, firstColumn);
		}
	}else if (mode==AbstractFileFilter::Replace){
		//replace completely the previous content of the data source with the content to be imported.
		int columns = dataSource->childCount<Column>();

		if (columns>(endColumn-startColumn+1)){
			//there're more columns in the data source then required
			//-> remove the superfluous columns
			for(int i=0;i<columns-(endColumn-startColumn+1);i++) {
				dataSource->removeChild(dataSource->child<Column>(0));
			}

			//rename the columns, that are already available
			for (int i=0; i<endColumn-startColumn; i++){
				dataSource->child<Column>(i)->setUndoAware(false);
				dataSource->child<Column>(i)->setColumnMode( AbstractColumn::Numeric);
				dataSource->child<Column>(i)->setName(QString(""));
				dataSource->child<Column>(i)->setSuppressDataChangedSignal(true);
			}
		}else{
			//rename the columns, that are already available
			for (int i=0; i<columns; i++){
				dataSource->child<Column>(i)->setUndoAware(false);
				dataSource->child<Column>(i)->setColumnMode( AbstractColumn::Numeric);
				dataSource->child<Column>(i)->setName(QString(""));
				dataSource->child<Column>(i)->setSuppressDataChangedSignal(true);
			}

			//create additional columns if needed
			for(int i=0; i<=(endColumn-startColumn-columns); i++) {
				newColumn = new Column(QString(""), AbstractColumn::Numeric);
				newColumn->setUndoAware(false);
				dataSource->addChild(newColumn);
				dataSource->child<Column>(i)->setSuppressDataChangedSignal(true);
			}
		}
	}


	int numLines=AsciiMatrixFilter::lineNumber(fileName);
	int actualEndRow;
	if (endRow == -1)
		actualEndRow = numLines;
	else if (endRow > numLines-1)
		actualEndRow = numLines-1;
	else
		actualEndRow = endRow;

	numLines = actualEndRow-startRow;

	//resize the spreadsheet
	Spreadsheet* spreadsheet = dynamic_cast<Spreadsheet*>(dataSource);
	if (mode==AbstractFileFilter::Replace){
		spreadsheet->clear();
		spreadsheet->setRowCount(numLines);
	}else{
		if (spreadsheet->rowCount()<numLines)
			spreadsheet->setRowCount(numLines);
	}

	//pointers to the actual data containers
	QVector<QVector<double>*> dataPointers;
	for ( int n=startColumn; n<=endColumn; n++ ){
		QVector<double>* vector = static_cast<QVector<double>* >(dataSource->child<Column>(columnOffset+n-startColumn)->data());
		vector->reserve(numLines);
		vector->resize(numLines);
		dataPointers.push_back(vector);
	}

	//import the values in the first line, if they were not used as the header (as the names for the columns)
	for ( int n=startColumn; n<=endColumn; n++ ){
		if (n<lineStringList.size()) {
			bool isNumber;
			const double value = lineStringList.at(n).toDouble(&isNumber);
			isNumber ? dataPointers[n-startColumn]->operator[](0) = value : dataPointers[n-startColumn]->operator[](0) = NAN;
		} else {
			dataPointers[n]->operator[](0) = NAN;
		}
	}
	currentRow++;

	//first line in the file is parsed. Read the remainder of the file.
	bool isNumber;
	for (int i=0; i<numLines; i++){
		line = in.readLine();

		if( simplifyWhitespacesEnabled )
			line = line.simplified();

		//skip empty lines
		if (line.isEmpty())
			continue;

		if( line.startsWith(commentCharacter) == true ){
			currentRow++;
			continue;
		}

		lineStringList = line.split( separator, QString::SplitBehavior(skipEmptyParts) );

		// TODO : read strings (comments) or datetime too
		for ( int n=startColumn; n<=endColumn; n++ ){
			if (n<lineStringList.size()) {
				const double value = lineStringList.at(n).toDouble(&isNumber);
				isNumber ? dataPointers[n-startColumn]->operator[](currentRow) = value : dataPointers[n-startColumn]->operator[](currentRow) = NAN;
			} else {
				dataPointers[n-startColumn]->operator[](currentRow) = NAN;
			}
		}

		currentRow++;
		emit q->completed(100*currentRow/numLines);
	}

	//set the comments for each of the columns
	//TODO: generalize to different data types
	QString comment = i18np("numerical data, %1 element", "numerical data, %1 elements", currentRow+1);
	for ( int n=startColumn; n<=endColumn; n++ ){
		Column* column = spreadsheet->column(columnOffset+n-startColumn);
		column->setComment(comment);
		column->setUndoAware(true);
		if (mode==AbstractFileFilter::Replace) {
			column->setSuppressDataChangedSignal(false);
			column->setChanged();
		}
	}

	spreadsheet->setUndoAware(true);
}


void AsciiMatrixFilterPrivate::clearDataSource(AbstractDataSource* dataSource) const {
// 	Spreadsheet* spreadsheet = dynamic_cast<Spreadsheet*>(dataSource);
	int columns = dataSource->childCount<Column>();
	for (int i=0; i<columns; i++){
		dataSource->child<Column>(i)->setUndoAware(false);
		dataSource->child<Column>(i)->setSuppressDataChangedSignal(true);
		dataSource->child<Column>(i)->clear();
		dataSource->child<Column>(i)->setUndoAware(true);
		dataSource->child<Column>(i)->setSuppressDataChangedSignal(false);
		dataSource->child<Column>(i)->setChanged();
	}
}

/*!
    writes the content of \c dataSource to the file \c fileName.
*/
void AsciiMatrixFilterPrivate::write(const QString & fileName, AbstractDataSource* dataSource){
	Q_UNUSED(fileName);
	Q_UNUSED(dataSource);
	//TODO
}

//##############################################################################
//##################  Serialization/Deserialization  ###########################
//##############################################################################

/*!
  Saves as XML.
 */
void AsciiMatrixFilter::save(QXmlStreamWriter* writer) const {
	writer->writeStartElement( "asciiMatrixFilter" );
	writer->writeAttribute( "commentCharacter", d->commentCharacter );
	writer->writeAttribute( "separatingCharacter", d->separatingCharacter );
	writer->writeAttribute( "autoMode", QString::number(d->autoModeEnabled) );
	writer->writeAttribute( "skipEmptyParts", QString::number(d->skipEmptyParts) );
	writer->writeAttribute( "simplifyWhitespaces", QString::number(d->simplifyWhitespacesEnabled) );
	writer->writeAttribute( "transposed", QString::number(d->transposed) );
	writer->writeAttribute( "startRow", QString::number(d->startRow) );
	writer->writeAttribute( "endRow", QString::number(d->endRow) );
	writer->writeAttribute( "startColumn", QString::number(d->startColumn) );
	writer->writeAttribute( "endColumn", QString::number(d->endColumn) );
	writer->writeEndElement();
}

/*!
  Loads from XML.
*/
bool AsciiMatrixFilter::load(XmlStreamReader* reader) {
	if(!reader->isStartElement() || reader->name() != "asciiMatrixFilter"){
		reader->raiseError(i18n("no ascii filter element found"));
		return false;
	}

	QString attributeWarning = i18n("Attribute '%1' missing or empty, default value is used");
	QXmlStreamAttributes attribs = reader->attributes();

	QString str = attribs.value("commentCharacter").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'commentCharacter'"));
	else
		d->commentCharacter = str;

	str = attribs.value("separatingCharacter").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'separatingCharacter'"));
	else
		d->separatingCharacter = str;

	str = attribs.value("autoMode").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'autoMode'"));
	else
		d->autoModeEnabled = str.toInt();

	str = attribs.value("simplifyWhitespaces").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'simplifyWhitespaces'"));
	else
		d->simplifyWhitespacesEnabled = str.toInt();

	str = attribs.value("skipEmptyParts").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'skipEmptyParts'"));
	else
		d->skipEmptyParts = str.toInt();

	str = attribs.value("transposed").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'transposed'"));
	else
		d->transposed = str.toInt();

	str = attribs.value("startRow").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'startRow'"));
	else
		d->startRow = str.toInt();

	str = attribs.value("endRow").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'endRow'"));
	else
		d->endRow = str.toInt();

	str = attribs.value("startColumn").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'startColumn'"));
	else
		d->startColumn = str.toInt();

	str = attribs.value("endColumn").toString();
	if(str.isEmpty())
		reader->raiseWarning(attributeWarning.arg("'endColumn'"));
	else
		d->endColumn = str.toInt();

	return true;
}

// Q_EXPORT_PLUGIN2(ioasciifilter, AsciiMatrixFilter)
