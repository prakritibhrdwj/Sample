/***************************************************************************
    File                 : ImportDialog.cc
    Project              : LabPlot
    Description          : import file data dialog
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Alexander Semke (alexander.semke@web.de)
    Copyright            : (C) 2008-2015 by Stefan Gerlach (stefan.gerlach@uni.kn)

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

#include "ImportFileDialog.h"
#include "ImportFileWidget.h"
#include "backend/datasources/FileDataSource.h"
#include "backend/datasources/filters/AbstractFileFilter.h"
#include "backend/spreadsheet/Spreadsheet.h"
#include "backend/matrix/Matrix.h"
#include "backend/core/Workbook.h"
#include "commonfrontend/widgets/TreeViewComboBox.h"

#include <kmessagebox.h>
#include <KInputDialog>
#include <QProgressBar>
#include <QStatusBar>
#include <QDir>
#include <QInputDialog>
#include <QToolButton>
#include <KMenu>
#include <QDebug>


/*!
	\class ImportFileDialog
	\brief Dialog for importing data from a file. Embeddes \c ImportFileWidget and provides the standard buttons.

	\ingroup kdefrontend
 */

ImportFileDialog::ImportFileDialog(QWidget* parent) : KDialog(parent), cbPosition(0), m_optionsShown(false), m_newDataContainerMenu(new KMenu(this)) {
	mainWidget = new QWidget(this);
	vLayout = new QVBoxLayout(mainWidget);
	vLayout->setSpacing(0);
	vLayout->setContentsMargins(0,0,0,0);

	importFileWidget = new ImportFileWidget( mainWidget );
	vLayout->addWidget(importFileWidget);

	setMainWidget( mainWidget );

	setButtons( KDialog::Ok | KDialog::User1 | KDialog::Cancel );

	KConfigGroup conf(KSharedConfig::openConfig(),"ImportFileDialog");
	m_optionsShown = conf.readEntry("ShowOptions", false);
	if (m_optionsShown){
		setButtonText(KDialog::User1,i18n("Hide Options"));
	} else {
		setButtonText(KDialog::User1,i18n("Show Options"));
	}
	importFileWidget->showOptions(m_optionsShown);

	connect(this,SIGNAL(user1Clicked()), this, SLOT(toggleOptions()));

	setCaption(i18n("Import Data to Spreadsheet or Matrix"));
	setWindowIcon(KIcon("document-import-database"));
	resize( QSize(500,0).expandedTo(minimumSize()) );
}

ImportFileDialog::~ImportFileDialog(){
	KConfigGroup conf(KSharedConfig::openConfig(),"ImportFileDialog");
	conf.writeEntry("ShowOptions", m_optionsShown);
	if (cbPosition)
		conf.writeEntry("Position", cbPosition->currentIndex());
}

/*!
	creates widgets for the frame "Add-To" and sets the current model in the combobox to \c model.
 */
void ImportFileDialog::setModel(std::auto_ptr<QAbstractItemModel> model){
	m_model = model;

	//Frame for the "Add To"-Stuff
	frameAddTo = new QGroupBox(this);
	frameAddTo->setTitle(i18n("Import To"));
	QGridLayout *grid = new QGridLayout(frameAddTo);
	grid->addWidget( new QLabel(i18n("Data container"),  frameAddTo),0,0 );

	cbAddTo = new TreeViewComboBox(frameAddTo);
	cbAddTo->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
	QList<const char *> list;
	list<<"Folder"<<"Spreadsheet"<<"Matrix"<<"Workbook";
	cbAddTo->setTopLevelClasses(list);
	grid->addWidget(cbAddTo,0,1);
	connect( cbAddTo, SIGNAL(currentModelIndexChanged(QModelIndex)), this, SLOT(currentAddToIndexChanged(QModelIndex)) );

	list.clear();
	list<<"Spreadsheet"<<"Matrix"<<"Workbook";
	cbAddTo->setSelectableClasses(list);

	tbNewDataContainer = new QToolButton(frameAddTo);
	tbNewDataContainer->setIcon( KIcon("list-add") );
	grid->addWidget( tbNewDataContainer,0,2);
	connect(tbNewDataContainer, SIGNAL(clicked(bool)), this, SLOT(newDataContainerMenu()));
	connect(m_newDataContainerMenu, SIGNAL(triggered(QAction*)), this, SLOT(newDataContainer(QAction*)));

	//grid->addItem( new QSpacerItem(50,10, QSizePolicy::Preferred, QSizePolicy::Fixed) );

	lPosition = new QLabel(i18n("Position"), frameAddTo);
	lPosition->setEnabled(false);
	grid->addWidget(lPosition,1,0);

	cbPosition = new QComboBox(frameAddTo);
	cbPosition->setEnabled(false);
	cbPosition->addItem(i18n("Append"));
	cbPosition->addItem(i18n("Prepend"));
	cbPosition->addItem(i18n("Replace"));
	KConfigGroup conf(KSharedConfig::openConfig(),"ImportFileDialog");
	cbPosition->setCurrentIndex( conf.readEntry("Position", 0) );

	cbPosition->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	grid->addWidget(cbPosition,1,1);

	vLayout->addWidget(frameAddTo);
	cbAddTo->setModel(m_model.get());

	//hide the data-source related widgets
	//TODO: disable for file data sources
	importFileWidget->hideDataSource();

	//ok is only available if a valid spreadsheet was selected
	enableButtonOk(false);
}

void ImportFileDialog::updateModel(std::auto_ptr<QAbstractItemModel> model){
	m_model = model;
	cbAddTo->setModel(m_model.get());
}

void ImportFileDialog::setCurrentIndex(const QModelIndex& index){
	cbAddTo->setCurrentModelIndex(index);
	this->currentAddToIndexChanged(index);
}

/*!
  triggers data import to the file data source \c source
*/
void ImportFileDialog::importToFileDataSource(FileDataSource* source, QStatusBar* statusBar) const {
	importFileWidget->saveSettings(source);

	//show a progress bar in the status bar
	QProgressBar* progressBar = new QProgressBar();
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	connect(source->filter(), SIGNAL(completed(int)), progressBar, SLOT(setValue(int)));

	statusBar->clearMessage();
	statusBar->addWidget(progressBar, 1);
	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

	QTime timer;
	timer.start();
	source->read();
	statusBar->showMessage( i18n("File data source created in %1 seconds.").arg((float)timer.elapsed()/1000) );

	QApplication::restoreOverrideCursor();
	statusBar->removeWidget(progressBar);
}

/*!
  triggers data import to the currently selected data container
*/
void ImportFileDialog::importTo(QStatusBar* statusBar) const {
	AbstractAspect * aspect = static_cast<AbstractAspect *>(cbAddTo->currentModelIndex().internalPointer());
	QString fileName = importFileWidget->fileName();
	AbstractFileFilter* filter = importFileWidget->currentFileFilter();
	AbstractFileFilter::ImportMode mode = AbstractFileFilter::ImportMode(cbPosition->currentIndex());

	//show a progress bar in the status bar
	QProgressBar* progressBar = new QProgressBar();
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	connect(filter, SIGNAL(completed(int)), progressBar, SLOT(setValue(int)));

	statusBar->clearMessage();
	statusBar->addWidget(progressBar, 1);

	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
	QApplication::processEvents(QEventLoop::AllEvents, 100);

	QTime timer;
	timer.start();
	if(aspect->inherits("Matrix")) {
		Matrix* matrix = qobject_cast<Matrix*>(aspect);
		filter->read(fileName, matrix, mode);
	}
	else if (aspect->inherits("Spreadsheet")) {
		Spreadsheet* spreadsheet = qobject_cast<Spreadsheet*>(aspect);
		filter->read(fileName, spreadsheet, mode);
	}
	else if (aspect->inherits("Workbook")) {
		Workbook* workbook = qobject_cast<Workbook*>(aspect);

		//TODO: handle multiple data sets/variables
		switch(importFileWidget->currentFileType()) {
		case FileDataSource::HDF: {
			QStringList names = importFileWidget->selectedHDFNames();
			qDebug()<<"	Importing HDF data sets "<<names;
			//TODO: see NetCDF

			break;
		}
		case FileDataSource::NETCDF: {
			QStringList names = importFileWidget->selectedNetCDFNames();
			qDebug()<<"	Importing NetCDF variables "<<names;
			int nrNames = names.size();
			//TODO: resize workbook to handle all variables: use mode
			switch(mode) {
			case AbstractFileFilter::Replace: {
				QList<AbstractAspect*> sheets = workbook->children<AbstractAspect>();
				qDebug()<<"	workbook nr sheets ="<<sheets.size();
				break;
			}
			case AbstractFileFilter::Append:
				break;
			case AbstractFileFilter::Prepend:
				break;
			default:
				qDebug()<<"	unknown AbstractFileFilter mode";
			}

			break;
		}
		default: {
			break;
		}
		}

		// use active spreadsheet/matrix if present, else new spreadsheet
		Spreadsheet* spreadsheet = workbook->currentSpreadsheet();
		Matrix* matrix = workbook->currentMatrix();
		if(spreadsheet != NULL)
			filter->read(fileName, spreadsheet, mode);
		else if (matrix != NULL)
			filter->read(fileName, matrix, mode);
		else {
			spreadsheet = new Spreadsheet(0, i18n("Spreadsheet"));
			workbook->addChild(spreadsheet);
			filter->read(fileName, spreadsheet, mode);
		}
	}
	statusBar->showMessage( i18n("File %1 imported in %2 seconds.").arg(fileName).arg((float)timer.elapsed()/1000) );

	QApplication::restoreOverrideCursor();
	statusBar->removeWidget(progressBar);
	delete filter;
}

void ImportFileDialog::toggleOptions(){
	importFileWidget->showOptions(!m_optionsShown);
	m_optionsShown = !m_optionsShown;

	if (m_optionsShown)
		setButtonText(KDialog::User1,i18n("Hide Options"));
	else
		setButtonText(KDialog::User1,i18n("Show Options"));

	//resize the dialog
	mainWidget->resize(layout()->minimumSize());
	layout()->activate();
 	resize( QSize(this->width(),0).expandedTo(minimumSize()) );
}

void ImportFileDialog::currentAddToIndexChanged(QModelIndex index){
	AbstractAspect* aspect = static_cast<AbstractAspect *>(index.internalPointer());

	if (aspect) {
		lPosition->setEnabled(true);
		cbPosition->setEnabled(true);
		enableButtonOk(true);
	} else{
		lPosition->setEnabled(false);
		cbPosition->setEnabled(false);
		enableButtonOk(false);
	}
}

void ImportFileDialog::newDataContainer(QAction *action){
	QString path = importFileWidget->fileName();
	QString name=path.right( path.length()-path.lastIndexOf(QDir::separator())-1 );

	QString type = action->iconText().split(' ')[1];
	if (name.isEmpty())
		name = action->iconText();

	bool ok;
	// child widgets can't have own icons
	QInputDialog* dlg = new QInputDialog(this);
	name = dlg->getText(this, i18n("Add %1", action->iconText()), i18n("%1 name:", type), QLineEdit::Normal, name, &ok);
	if (ok) {
		if( action->iconText() == i18n("new Workbook"))
			emit newWorkbookRequested(name);
		else if( action->iconText() == i18n("new Spreadsheet"))
			emit newSpreadsheetRequested(name);
		else if( action->iconText() == i18n("new Matrix"))
			emit newMatrixRequested(name);
	}

	delete dlg;
}

void ImportFileDialog::newDataContainerMenu() {
        m_newDataContainerMenu->clear();

        m_newDataContainerMenu->addAction( QIcon(KIcon("tab-new-background")), i18n("new Workbook") );
        m_newDataContainerMenu->addAction( QIcon(KIcon("insert-table")), i18n("new Spreadsheet") );
        m_newDataContainerMenu->addAction( QIcon(KIcon("resource-calendar-insert")), i18n("new Matrix") );

        m_newDataContainerMenu->exec( tbNewDataContainer->mapToGlobal(tbNewDataContainer->rect().bottomLeft()));
}
