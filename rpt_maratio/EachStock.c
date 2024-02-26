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

#include	"rpt_maratio.h"

int EachStock ()
{
	int		rv, Day, AboveCount;
	double	Percent;

	rv = LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_DESCEND, CrossOverSince + 2 );
	if ( rv < CrossOverSince + 2 )
	{
		if ( Debug )
		{
			printf ( "%s only %d history records\n", xstock.xsticker, rv );
		}
		return ( 0 );
	}

AboveCount = 0;
	for ( Day = 1; Day < CrossOverSince; Day++ )
	{
		if ( Debug )
		{
			printf ( "%s %s Close %.2f Slow %.2f Fast %.2f\n",
				xstock.xsticker, HistoryArray[Day].Date, HistoryArray[Day].Close,  HistoryArray[Day].Average[FastAVG], HistoryArray[Day].Average[SlowAVG] );
		}

		if ( HistoryArray[Day].Average[FastAVG] > HistoryArray[Day].Average[SlowAVG] )
		{
			AboveCount++;
		}
	}
				
	Percent = 100.0 * ( (double) AboveCount / (double) Day );

	if ( Percent >= MinimumPercent )
	{
		fprintf ( fpOutput, "%-10.10s|%-30.30s|%d|%d|%.2f\n", 
					xstock.xsticker, 
					xstock.xsname, 
					Day,
					AboveCount,
					Percent );
	}

	return ( 0 );
}
