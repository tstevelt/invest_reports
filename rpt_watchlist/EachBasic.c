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

#include	"rpt_watchlist.h"

int EachBasic ()
{
	int		rv;
	double	CurrentPrice, PreviousPrice, PriceChangePercent, AlertDiff, AlertPercent, AlertPrice;

	sprintf ( WhereClause, "Sticker = '%s'", xwatchlist.xwticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 0 )) != 1 )
	{
		fprintf ( stderr, "LoadStock %s returned %d\n", xwatchlist.xwticker, rv );
		return ( 0 );
	}

	/*----------------------------------------------------------
		get stock current and previous price
	----------------------------------------------------------*/
	LoadHistoryArray ( &MySql,  xwatchlist.xwticker, xstock.xslast, SORT_DESCEND, 2 );
	switch ( HistoryCount )
	{
		case 0:
			fprintf ( stderr, "Cannot find price history for %s %s\n", xwatchlist.xwticker, xstock.xslast );
			return ( 0 );
		case 1:
			CurrentPrice = HistoryArray[0].Close;
			PreviousPrice = 0.0;
			break;
		case 2:
		default:
			CurrentPrice  = HistoryArray[0].Close;
			PreviousPrice = HistoryArray[1].Close;
			break;
	}
	PriceChangePercent = 100.0 * ( CurrentPrice - PreviousPrice ) / PreviousPrice;

	ReportCount++;
	if ( ReportCount == 1 )
	{
		sprintf ( OutFileName, "%s/watchlist_%d.dat", TEMPDIR, getpid() );
		if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
		{
			fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
			exit ( 1 );
		}
	}

	fprintf ( fpOutput, "%s|",   xwatchlist.xwticker );
	fprintf ( fpOutput, "%s|",   xstock.xsname );
	fprintf ( fpOutput, "%.2f|", PriceChangePercent );
	fprintf ( fpOutput, "%.2f|", CurrentPrice );

	switch ( xwatchlist.xwalerttype[0] )
	{
		case ALERT_ABOVE_MA5:
		case ALERT_BELOW_MA5:
			AlertPrice = HistoryArray[0].Average[AVG_5];
			break;
		case ALERT_ABOVE_MA10:
		case ALERT_BELOW_MA10:
			AlertPrice = HistoryArray[0].Average[AVG_10];
			break;
		case ALERT_ABOVE_MA20:
		case ALERT_BELOW_MA20:
			AlertPrice = HistoryArray[0].Average[AVG_20];
			break;
		case ALERT_ABOVE_MA50:
		case ALERT_BELOW_MA50:
			AlertPrice = HistoryArray[0].Average[AVG_50];
			break;
		case ALERT_ABOVE_MA100:
		case ALERT_BELOW_MA100:
			AlertPrice = HistoryArray[0].Average[AVG_100];
			break;
		case ALERT_ABOVE_MA200:
		case ALERT_BELOW_MA200:
			AlertPrice = HistoryArray[0].Average[AVG_200];
			break;
	}

	switch ( xwatchlist.xwalerttype[0] )
	{
		case ALERT_ABOVE_AMT:
			fprintf ( fpOutput, "%s|", AlertDescription(xwatchlist.xwalerttype[0]) );
			AlertDiff = xwatchlist.xwalertprice - CurrentPrice;
			AlertPercent = 100.0 * AlertDiff / xwatchlist.xwalertprice;
			fprintf ( fpOutput, "%.2f|", xwatchlist.xwalertprice );
			fprintf ( fpOutput, "%.2f|", AlertDiff );
			fprintf ( fpOutput, "%.2f|", AlertPercent );
			break;
		case ALERT_BELOW_AMT:
			fprintf ( fpOutput, "%s|", AlertDescription(xwatchlist.xwalerttype[0]) );
			AlertDiff = CurrentPrice - xwatchlist.xwalertprice;
			AlertPercent = 100.0 * AlertDiff / xwatchlist.xwalertprice;
			fprintf ( fpOutput, "%.2f|", xwatchlist.xwalertprice );
			fprintf ( fpOutput, "%.2f|", AlertDiff );
			fprintf ( fpOutput, "%.2f|", AlertPercent );
			break;
		case ALERT_ABOVE_MA5:
		case ALERT_ABOVE_MA10:
		case ALERT_ABOVE_MA20:
		case ALERT_ABOVE_MA50:
		case ALERT_ABOVE_MA100:
		case ALERT_ABOVE_MA200:
			fprintf ( fpOutput, "%s|", AlertDescription(xwatchlist.xwalerttype[0]) );
			AlertDiff = AlertPrice - CurrentPrice;
			AlertPercent = 100.0 * AlertDiff / AlertPrice;
			fprintf ( fpOutput, "%.2f|", AlertPrice );
			fprintf ( fpOutput, "%.2f|", AlertDiff );
			fprintf ( fpOutput, "%.2f|", AlertPercent );
			break;
		case ALERT_BELOW_MA5:
		case ALERT_BELOW_MA10:
		case ALERT_BELOW_MA20:
		case ALERT_BELOW_MA50:
		case ALERT_BELOW_MA100:
		case ALERT_BELOW_MA200:
			fprintf ( fpOutput, "%s|", AlertDescription(xwatchlist.xwalerttype[0]) );
			AlertDiff = CurrentPrice - AlertPrice;
			AlertPercent = 100.0 * AlertDiff / AlertPrice;
			fprintf ( fpOutput, "%.2f|", AlertPrice );
			fprintf ( fpOutput, "%.2f|", AlertDiff );
			fprintf ( fpOutput, "%.2f|", AlertPercent );
			break;
		default:
			fprintf ( fpOutput, " | | |" );
			break;
			
	}

	fprintf ( fpOutput, "\n" );

	return ( 0 );
}
