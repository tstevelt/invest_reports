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

#include	"rpt_macross.h"

int EachStock ()
{
	int		rv, Day, KeepIt;
	double	Percent;
	char	*Signal;

	rv = LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_DESCEND, CrossOverSince + 2 );
	if ( rv < CrossOverSince + 2 )
	{
		if ( Debug )
		{
			printf ( "%s only %d history records\n", xstock.xsticker, rv );
		}
		return ( 0 );
	}

	if ( HistoryArray[0].Average[FastAVG] < 0.01 ||  HistoryArray[0].Average[SlowAVG] < 0.01 )
	{
		return ( 0 );
	}

	if ( BuySellAll == 'S' )
	{
		if ( HistoryArray[0].Average[FastAVG] > HistoryArray[0].Average[SlowAVG] )
		{
			return ( 0 );
		}
	}
	else if ( BuySellAll == 'B' )
	{
		if ( HistoryArray[0].Average[FastAVG] < HistoryArray[0].Average[SlowAVG] )
		{
			return ( 0 );
		}
	}

	for ( Day = 1; Day < CrossOverSince; Day++ )
	{
		if ( Debug )
		{
			printf ( "%s %s Close %.2f Slow %.2f Fast %.2f\n",
				xstock.xsticker, HistoryArray[Day].Date, HistoryArray[Day].Close,  HistoryArray[Day].Average[FastAVG], HistoryArray[Day].Average[SlowAVG] );
		}

		KeepIt = 0;
		if ( BuySellAll == 'S' )
		{
			if ( HistoryArray[Day].Average[FastAVG] > HistoryArray[Day].Average[SlowAVG] )
			{
				KeepIt = 1;
				Signal = "Sell";

				if ( Debug )
				{
					printf ( "%s %s\n", xstock.xsticker, HistoryArray[Day].Date );
				}
			}
		}
		else if ( BuySellAll == 'B' )
		{
			if ( HistoryArray[Day].Average[FastAVG] < HistoryArray[Day].Average[SlowAVG] )
			{
				KeepIt = 1;
				Signal = "Buy";

				if ( Debug )
				{
					printf ( "%s %s\n", xstock.xsticker, HistoryArray[Day].Date );
				}
			}
		}
		else
		{
			if (( HistoryArray[0].Average[FastAVG]   < HistoryArray[0].Average[SlowAVG]   ) &&
				( HistoryArray[Day].Average[FastAVG] > HistoryArray[Day].Average[SlowAVG] ))
			{
				KeepIt = 1;
				Signal = "Sell";
			}
			else if (( HistoryArray[0].Average[FastAVG]   > HistoryArray[0].Average[SlowAVG]   ) &&
				     ( HistoryArray[Day].Average[FastAVG] < HistoryArray[Day].Average[SlowAVG] ))
			{
				KeepIt = 1;
				Signal = "Buy";
			}
		}
				
		if ( KeepIt )
		{
			Percent = 100.0 * ( HistoryArray[0].Close - HistoryArray[Day-1].Close ) / HistoryArray[Day-1].Close;

			fprintf ( fpOutput, "%-10.10s|%-30.30s|%s|%s|%.2f|%.2f|%.2f|%.2f|%.2f\n", 
					xstock.xsticker, 
					xstock.xsname, 
					Signal,
					HistoryArray[Day-1].Date, 
					HistoryArray[Day-1].Average[FastAVG], 
					HistoryArray[Day-1].Average[SlowAVG], 
					HistoryArray[Day-1].Close,
					HistoryArray[0].Close,
					Percent );
			return ( 0 );
		}
	}

	return ( 0 );
}
