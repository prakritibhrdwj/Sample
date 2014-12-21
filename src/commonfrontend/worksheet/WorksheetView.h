/***************************************************************************
    File                 : WorksheetView.h
    Project              : LabPlot
    Description          : Worksheet view
    --------------------------------------------------------------------
    Copyright            : (C) 2009-2014 by Alexander Semke (alexander.semke@web.de)

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

#ifndef WORKSHEETVIEW_H
#define WORKSHEETVIEW_H

#include <QGraphicsView>
#include "backend/worksheet/Worksheet.h"

class QMenu;
class QToolBar;
class QToolButton;
class QWheelEvent;
class QTimeLine;

class AbstractAspect;
class WorksheetElement;
class WorksheetModel;

class WorksheetView : public QGraphicsView {
	Q_OBJECT

  public:
	explicit WorksheetView(Worksheet* worksheet);
	virtual ~WorksheetView();

	enum ExportFormat{Pdf, Eps, Svg, Png};
	enum GridStyle{NoGrid, LineGrid, DotGrid};
	enum ExportArea{ExportBoundingBox, ExportSelection, ExportWorksheet};

	struct GridSettings {
		GridStyle style;
		QColor color;
		int horizontalSpacing;
		int verticalSpacing;
		float opacity;
	};

	void setScene(QGraphicsScene*);
	void exportToFile(const QString&, const ExportFormat, const ExportArea, const bool, const int);

  private:
	enum MouseMode{SelectionMode, NavigationMode, ZoomSelectionMode};
	enum CartesianPlotActionMode{ApplyActionToSelection, ApplyActionToAll};

	void initActions();
	void initMenus();
	void processResize();
	void drawForeground(QPainter*, const QRectF&);
	void drawBackground(QPainter*, const QRectF&);
	void exportPaint(QPainter* painter, const QRectF& targetRect, const QRectF& sourceRect, const bool);

	//events
	void resizeEvent(QResizeEvent*);
	void contextMenuEvent(QContextMenuEvent*);
	void wheelEvent(QWheelEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);

	Worksheet* m_worksheet;
	WorksheetModel* m_model;
	MouseMode m_mouseMode;
	CartesianPlotActionMode m_cartesianPlotActionMode;
	bool m_selectionBandIsShown;
	QPoint m_selectionStart;
	QPoint m_selectionEnd;
	GridSettings m_gridSettings;
	QList<QGraphicsItem*> m_selectedItems;
	bool m_suppressSelectionChangedEvent;
	WorksheetElement* lastAddedWorksheetElement;
	QTimeLine* m_fadeInTimeLine;
	QTimeLine* m_fadeOutTimeLine;

	//Menus
	QMenu* m_addNewMenu;
	QMenu* m_addNewCartesianPlotMenu;
	QMenu* m_zoomMenu;
	QMenu* m_layoutMenu;
	QMenu* m_gridMenu;
	QMenu* m_viewMouseModeMenu;
	QMenu* m_cartesianPlotMenu;
	QMenu* m_cartesianPlotMouseModeMenu;
	QMenu* m_cartesianPlotAddNewMenu;
	QMenu* m_cartesianPlotZoomMenu;
	QMenu* m_cartesianPlotActionModeMenu;

	QToolButton* tbNewCartesianPlot;
	QToolButton* tbZoom;
	QAction* currentZoomAction;

	//Actions
	QAction* selectAllAction;
	QAction* deleteAction;

	QAction* zoomInViewAction;
	QAction* zoomOutViewAction;
	QAction* zoomOriginAction;
	QAction* zoomFitPageHeightAction;
	QAction* zoomFitPageWidthAction;
	QAction* zoomFitSelectionAction;

	QAction* navigationModeAction;
	QAction* zoomSelectionModeAction;
	QAction* selectionModeAction;

	QAction* addCartesianPlot1Action;
	QAction* addCartesianPlot2Action;
	QAction* addCartesianPlot3Action;
	QAction* addCartesianPlot4Action;
	QAction* addTextLabelAction;

	QAction* verticalLayoutAction;
	QAction* horizontalLayoutAction;
	QAction* gridLayoutAction;
	QAction* breakLayoutAction;

	QAction* noGridAction;
	QAction* denseLineGridAction;
	QAction* sparseLineGridAction;
	QAction* denseDotGridAction;
	QAction* sparseDotGridAction;
	QAction* customGridAction;
	QAction* snapToGridAction;

	//Actions for cartesian plots
	QAction* cartesianPlotApplyToSelectionAction;
	QAction* cartesianPlotApplyToAllAction;
	QAction* cartesianPlotSelectionModeAction;
	QAction* cartesianPlotZoomSelectionModeAction;
	QAction* cartesianPlotZoomXSelectionModeAction;
	QAction* cartesianPlotZoomYSelectionModeAction;

	QAction* addCurveAction;
	QAction* addEquationCurveAction;
	QAction* addFitCurveAction;
	QAction* addHorizontalAxisAction;
	QAction* addVerticalAxisAction;
	QAction* addLegendAction;

	QAction* scaleAutoXAction;
	QAction* scaleAutoYAction;
	QAction* scaleAutoAction;
	QAction* zoomInAction;
	QAction* zoomOutAction;
	QAction* zoomInXAction;
	QAction* zoomOutXAction;
	QAction* zoomInYAction;
	QAction* zoomOutYAction;
	QAction* shiftLeftXAction;
	QAction* shiftRightXAction;
	QAction* shiftUpYAction;
	QAction* shiftDownYAction;

  public slots:
	void createContextMenu(QMenu*) const;
	void fillToolBar(QToolBar*);
	void fillCartesianPlotToolBar(QToolBar*);
	void print(QPrinter*) const;
	void selectItem(QGraphicsItem*);

  private slots:
	void addNew(QAction*);
	void aspectAboutToBeRemoved(const AbstractAspect*);
	void selectAllElements();
	void deleteElement();

	void mouseModeChanged(QAction*);
	void useViewSizeRequested();
	void changeZoom(QAction*);
	void changeLayout(QAction*);
	void changeGrid(QAction*);
	void changeSnapToGrid();

	void deselectItem(QGraphicsItem*);
	void selectionChanged();
	void updateBackground();
	void layoutChanged(Worksheet::Layout);

	void fadeIn(qreal);
	void fadeOut(qreal);

	//SLOTs for cartesian plots
	void cartesianPlotActionModeChanged(QAction*);
	void cartesianPlotMouseModeChanged(QAction*);
	void addCurve();
	void addEquationCurve();
	void addFitCurve();
	void addLegend();
	void addHorizontalAxis();
	void addVerticalAxis();
	void scaleAuto();
	void scaleAutoX();
	void scaleAutoY();
	void zoomIn();
	void zoomOut();
	void zoomInX();
	void zoomOutX();
	void zoomInY();
	void zoomOutY();
	void shiftLeftX();
	void shiftRightX();
	void shiftUpY();
	void shiftDownY();

  signals:
	void statusInfo(const QString&);
};

#endif
