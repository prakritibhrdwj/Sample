/***************************************************************************
    File                 : Transform.cpp
    Project              : LabPlot
    Description          : transformation for mapping between scene and
                           logical coordinates of Datapicker-image
    --------------------------------------------------------------------
    Copyright            : (C) 2015 by Ankit Wagadre (wagadre.ankit@gmail.com)
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

#include "Transform.h"
#include "math.h"

#define PI 3.14159265

Transform::Transform() {
}

bool Transform::mapTypeToCartesian(const DatapickerImage::ReferencePoints& axisPoints) {
	if (axisPoints.type == DatapickerImage::LogarithmicX) {
		for(int i=0; i<3; i++) {
			if (axisPoints.logicalPos[i].x() <= 0)
				return false;
			x[i] = log(axisPoints.logicalPos[i].x());
			y[i] = axisPoints.logicalPos[i].y();
			X[i] = axisPoints.scenePos[i].x();
			Y[i] = axisPoints.scenePos[i].y();
		}
	} else if (axisPoints.type == DatapickerImage::LogarithmicY) {
		for(int i=0; i<3; i++) {
			if (axisPoints.logicalPos[i].y() <= 0)
				return false;
			x[i] = axisPoints.logicalPos[i].x();
			y[i] = log(axisPoints.logicalPos[i].y());
			X[i] = axisPoints.scenePos[i].x();
			Y[i] = axisPoints.scenePos[i].y();
		}
	} else if (axisPoints.type == DatapickerImage::PolarInDegree) {
		for(int i=0; i<3; i++) {
			if (axisPoints.logicalPos[i].x() < 0)
				return false;
			x[i] = axisPoints.logicalPos[i].x()*cos(axisPoints.logicalPos[i].y()*PI / 180.0);
			y[i] = axisPoints.logicalPos[i].x()*sin(axisPoints.logicalPos[i].y()*PI / 180.0);
			X[i] = axisPoints.scenePos[i].x();
			Y[i] = axisPoints.scenePos[i].y();
		}
	} else if (axisPoints.type == DatapickerImage::PolarInRadians) {
		for(int i=0; i<3; i++) {
			if (axisPoints.logicalPos[i].x() < 0)
				return false;
			x[i] = axisPoints.logicalPos[i].x()*cos(axisPoints.logicalPos[i].y());
			y[i] = axisPoints.logicalPos[i].x()*sin(axisPoints.logicalPos[i].y());
			X[i] = axisPoints.scenePos[i].x();
			Y[i] = axisPoints.scenePos[i].y();
		}
	} else if (axisPoints.type == DatapickerImage::Ternary) {
		for(int i=0; i<3; i++) {
			x[i] = (2*axisPoints.logicalPos[i].y() + axisPoints.logicalPos[i].z())/(2*axisPoints.ternaryScale);
			y[i] = (sqrt(3)*axisPoints.logicalPos[i].z())/(2*axisPoints.ternaryScale);
			X[i] = axisPoints.scenePos[i].x();
			Y[i] = axisPoints.scenePos[i].y();
		}
	} else {
		for(int i=0; i<3; i++) {
			x[i] = axisPoints.logicalPos[i].x();
			y[i] = axisPoints.logicalPos[i].y();
			X[i] = axisPoints.scenePos[i].x();
			Y[i] = axisPoints.scenePos[i].y();
		}
	}

	return true;
}

QVector3D Transform::mapSceneToLogical(const QPointF& scenePoint, const DatapickerImage::ReferencePoints& axisPoints) {
	X[3] = scenePoint.x();
	Y[3] = scenePoint.y();

	if (mapTypeToCartesian(axisPoints)) {
		double tan;
		double sin;
		double cos;
		double scaleOfX;
		double scaleOfY;
		if (((Y[1] - Y[0])*(x[2] - x[0]) - (Y[2] - Y[0])*(x[1] - x[0]))!=0) {
			tan = ((X[1] - X[0])*(x[2] - x[0]) - (X[2] - X[0])*(x[1] - x[0]))/((Y[1] - Y[0])*(x[2] - x[0]) - (Y[2] - Y[0])*(x[1] - x[0]));
			sin = tan/sqrt(1 + tan*tan);
			cos = sqrt(1 - sin*sin);
		} else {
			sin=1;
			cos=0;
		}

		if((x[1] - x[0])!=0) {
			scaleOfX = (x[1] - x[0])/((X[1] - X[0])*cos - (Y[1] - Y[0])*sin);
		} else {
			scaleOfX = (x[2] - x[0])/((X[2] - X[0])*cos - (Y[2] - Y[0])*sin);
		}

		if ((y[1]-y[0])!=0) {
			scaleOfY = (y[1] - y[0])/((X[1] - X[0])*sin + (Y[1] - Y[0])*cos);
		} else {
			scaleOfY = (y[2] - y[0])/((X[2] - X[0])*sin + (Y[2] - Y[0])*cos);
		}

		x[3] = x[0] + (((X[3] - X[0])*cos - (Y[3] - Y[0])*sin)*scaleOfX);
		y[3] = y[0] + (((X[3] - X[0])*sin + (Y[3] - Y[0])*cos)*scaleOfY);
		return mapCartesianToType(QPointF(x[3], y[3]), axisPoints);
	}
	return QVector3D();
}

QVector3D Transform::mapSceneLengthToLogical(const QPointF& errorSpan, const DatapickerImage::ReferencePoints& axisPoints) {
	return mapSceneToLogical(errorSpan, axisPoints) - mapSceneToLogical(QPointF(0,0), axisPoints);
}

QVector3D Transform::mapCartesianToType(const QPointF& point, const DatapickerImage::ReferencePoints &axisPoints) const {
	if (axisPoints.type == DatapickerImage::LogarithmicX) {
		return QVector3D(exp(point.x()), point.y(), 0);
	} else if (axisPoints.type == DatapickerImage::LogarithmicY) {
		return QVector3D(point.x(), exp(point.y()), 0);
	} else if (axisPoints.type == DatapickerImage::PolarInDegree) {
		double r = sqrt(point.x()*point.x() + point.y()*point.y());
		double angle = atan(point.y()*180/(point.x()*PI));
		return QVector3D(r, angle, 0);
	} else if (axisPoints.type == DatapickerImage::PolarInRadians) {
		double r = sqrt(point.x()*point.x() + point.y()*point.y());
		double angle = atan(point.y()/point.x());
		return QVector3D(r, angle, 0);
	} else if (axisPoints.type == DatapickerImage::Ternary) {
		double c = (point.y()*2*axisPoints.ternaryScale)/sqrt(3);
		double b = (point.x()*2*axisPoints.ternaryScale - c)/2;
		double a = axisPoints.ternaryScale - b - c;
		return QVector3D(a, b, c);
	} else {
		return QVector3D(point.x(), point.y(), 0);
	}
}

