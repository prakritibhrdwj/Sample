/***************************************************************************
    File                 : XYFitCurve.h
    Project              : LabPlot
    Description          : A xy-curve defined by a fit model
    --------------------------------------------------------------------
    Copyright            : (C) 2014 Alexander Semke (alexander.semke@web.de)

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

#ifndef XYFITCURVE_H
#define XYFITCURVE_H

#include "backend/worksheet/plots/cartesian/XYCurve.h"

class XYFitCurvePrivate;
class XYFitCurve: public XYCurve {
	Q_OBJECT

	public:
		enum ModelType {Polynomial, Power, Exponential, Fourier, Gaussian, Lorentz, Maxwell, Custom};
		enum WeightsType {WeightsFromColumn, WeightsFromErrorColumn};

		struct FitData {
			FitData() : modelType(Polynomial),
						weightsType(XYFitCurve::WeightsFromColumn),
						degree(1),
						maxIterations(500),
						eps(1e-4) {};

			ModelType modelType;
			WeightsType weightsType;
			int degree;
			QString model;
			QStringList paramNames;
			QVector<double> paramStartValues;

			int maxIterations;
			double eps;
		};

		struct FitResult {
			FitResult() : available(false) {};

			bool available;
			QString status;
			int iterations;
			double dof; //degrees of freedom
			double sse; //sum of squared errors (SSE) / residual sum of errors (RSS) / sum of sq. residuals (SSR) = \sum_i^n (Y_i-y_i)^2
			double mse; //mean squared error = 1/n \sum_i^n  (Y_i-y_i)^2
			double rmse; //root-mean squared error = \sqrt(mse)
			double mae; //mean absolute error = \sum_i^n |Y_i-y_i|
			double rms; //residual mean square = SSE/d.o.f.
			double rsd; //residual standard deviation = sqrt(SSE/d.o.f)
			double rsquared; //Coefficient of determination (R^2)
			QVector<double> paramValues;
			QVector<double> errorValues;
			QString solverOutput;
		};

		explicit XYFitCurve(const QString& name);
		virtual ~XYFitCurve();

		void recalculate();
		virtual QIcon icon() const;
		virtual void save(QXmlStreamWriter*) const;
		virtual bool load(XmlStreamReader*);

		POINTER_D_ACCESSOR_DECL(const AbstractColumn, xDataColumn, XDataColumn)
		POINTER_D_ACCESSOR_DECL(const AbstractColumn, yDataColumn, YDataColumn)
		POINTER_D_ACCESSOR_DECL(const AbstractColumn, weightsColumn, WeightsColumn)
		const QString& xDataColumnPath() const;
		const QString& yDataColumnPath() const;
		const QString& weightsColumnPath() const;

		CLASS_D_ACCESSOR_DECL(FitData, fitData, FitData)
		const FitResult& fitResult() const;

		typedef AbstractWorksheetElement BaseClass;
		typedef XYFitCurvePrivate Private;

	protected:
		XYFitCurve(const QString& name, XYFitCurvePrivate* dd);

	private:
		Q_DECLARE_PRIVATE(XYFitCurve)
		void init();

	signals:
		friend class XYFitCurveSetXDataColumnCmd;
		friend class XYFitCurveSetYDataColumnCmd;
		friend class XYFitCurveSetWeightsColumnCmd;
		void xDataColumnChanged(const AbstractColumn*);
		void yDataColumnChanged(const AbstractColumn*);
		void weightsColumnChanged(const AbstractColumn*);

		friend class XYFitCurveSetFitDataCmd;
		void fitDataChanged(const XYFitCurve::FitData&);
};

#endif