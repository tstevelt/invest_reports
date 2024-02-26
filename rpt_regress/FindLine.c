/*------------------------------------------------------------------------------------
	should have read this first!
		https://www.varsitytutors.com/hotmath/hotmath_help/topics/line-of-best-fit
	and this from stackflow!
		https://stackoverflow.com/questions/5083465/fast-efficient-least-squares-fit-algorithm-in-c
		Thanks to Mark Lakata - released as public domain.
	std dev calc thanks to:
		https://www.mathsisfun.com/data/standard-deviation-formulas.html
------------------------------------------------------------------------------------*/
//     Invest report
// 
//     Copyright (C)  2019 - 2024 Tom Stevelt
// 
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Affero General Public License as
//     published by the Free Software Foundation, either version 3 of the
//     License, or (at your option) any later version.
// 
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU Affero General Public License for more details.
// 
//     You should have received a copy of the GNU Affero General Public License
//     along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include	"rpt_regress.h"

#define		MAXIMUMHISTORYRECORDS		1200
static	double	MyHistoryArray[MAXIMUMHISTORYRECORDS];
static	int		MyHistoryCount;

inline static double sqr(double x) {
    return x*x;
}

static int linreg(int n, const double y[], double* m, double* b, double* r)
{
    double   sumx = 0.0;                      /* sum of x     */
    double   sumx2 = 0.0;                     /* sum of x**2  */
    double   sumxy = 0.0;                     /* sum of x * y */
    double   sumy = 0.0;                      /* sum of y     */
    double   sumy2 = 0.0;                     /* sum of y**2  */
	int		i;

    for (i=0;i<n;i++){ 
        sumx  += (double)i;       
        sumx2 += sqr((double)i);  
        sumxy += (double)i * y[i];
        sumy  += y[i];      
        sumy2 += sqr(y[i]); 
    } 

    double denom = (n * sumx2 - sqr(sumx));
    if (denom == 0) {
        // singular matrix. can't solve the problem.
        *m = 0;
        *b = 0;
        if (r) *r = 0;
            return 1;
    }

    *m = (n * sumxy  -  sumx * sumy) / denom;
    *b = (sumy * sumx2  -  sumx * sumxy) / denom;
    if (r!=NULL) {
        *r = (sumxy - sumx * sumy / n) /    /* compute correlation coeff */
              sqrt((sumx2 - sqr(sumx)/n) *
              (sumy2 - sqr(sumy)/n));
    }

    return 0; 
}

int EachHistory ()
{
	if ( MyHistoryCount >= MAXIMUMHISTORYRECORDS )
	{
		printf ( "%d Exceeds MAXIMUMHISTORYRECORDS %d\n", MyHistoryCount, MAXIMUMHISTORYRECORDS );
		printf ( "%s\n", WhereClause );
		exit ( 1 );
	}

	MyHistoryArray[MyHistoryCount++] = xhistory.xhclose;

	if ( Debug )
	{
		printf ( "%3d|%s|%10.2f|%10ld\n", MyHistoryCount, xhistory.xhdate, xhistory.xhclose, xhistory.xhvolume );
	}

	return ( 0 );
}

void FindLine ( int ndx )
{
	int		rv, xh;
//	double	FirstHistory = 0.0;
//	double	LastHistory = 0.0;
	double	Slope;
	double	Bias;
	double	CorrCoeff;
	double	StdDev;
	double	Value, Diff;
	double	MeanAvg;

	MyHistoryCount = 0;

	sprintf ( WhereClause, "Hticker = '%s' and Hdate >= '%s' and Hdate <= '%s'", StockArray[ndx].xsticker, StartDate, EndDate );

	if (( rv = LoadHistoryCB ( &MySql, WhereClause, "Hdate", &xhistory, (int(*)()) EachHistory, 0 )) < 2 )
	{
		return;
	}

	/*----------------------------------------------------------
		find best straight line
	----------------------------------------------------------*/
	if ( linreg ( MyHistoryCount, MyHistoryArray, &Slope, &Bias, &CorrCoeff ) == 0 )
	{
		StockArray[ndx].Intercept = Bias;
		StockArray[ndx].Slope     = Slope;
		StockArray[ndx].CorrCoeff = CorrCoeff;
	}

	/*----------------------------------------------------------
		find standard deviation
	----------------------------------------------------------*/
	MeanAvg = 0.0;
	for ( xh = 0; xh < MyHistoryCount; xh++ )
	{
		MeanAvg = MeanAvg + MyHistoryArray[xh];
	}
	MeanAvg = MeanAvg / (double)MyHistoryCount;

	StdDev = 0.0;
	for ( xh = 0; xh < MyHistoryCount; xh++ )
	{
		Diff = MyHistoryArray[xh] - MeanAvg;

		StdDev = StdDev + (Diff * Diff);
	}
	StdDev = StdDev / (double)xh;
	StockArray[ndx].StdDev = sqrt ( StdDev );
	StdDev =  StockArray[ndx].StdDev;

	if ( Debug )
	{
		printf ( "slope %.2f intercept %.2f correlation %.2f std-dev %.2f\n", Slope, Bias, CorrCoeff, StdDev );
	}

	/*---------------------------------------------------------------
		return on investment, based on end points of straight line
	----------------------------------------------------------------*/
	Value = Slope * (double) (MyHistoryCount-1) + Bias;
	StockArray[ndx].Return = 100.0 * (Value - Bias) / Bias;

	if ( Debug )
	{
		printf ( "line start %.2f  line end %.2f  return %.2f\n", Bias, Value, StockArray[ndx].Return );
	}

	/*----------------------------------------------------------
		number of std-dev last price is above or below end of line.
	----------------------------------------------------------*/
	Diff = MyHistoryArray[(MyHistoryCount-1)] - Value;
	StockArray[ndx].Variance = Diff / StdDev;

	StockArray[ndx].CurrentPrice = MyHistoryArray[(MyHistoryCount-1)];

	if ( nsStrcmp ( StockArray[ndx].xsticker, "????" ) == 0 )
	{
		printf ( "%s line %.2f last %.2f std-dev %.2f diff %.2f var %.2f\n",
			StockArray[ndx].xsticker,
			Value,
			MyHistoryArray[(MyHistoryCount-1)],
			StdDev,
			Diff,
			StockArray[ndx].Variance );
	}

}
