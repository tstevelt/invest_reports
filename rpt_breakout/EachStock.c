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

#include	"rpt_breakout.h"

int EachStock ()
{
	double	AveragePrice;
	double	PriceChange, PricePercent;
	double	VolumeChange, VolumePercent;

	LoadHistoryArray ( &MySql, xstock.xsticker, xstock.xslast, SORT_DESCEND, 2 );
	if ( HistoryCount < 2 )
	{
		return ( 0 );
	}

	if ( HistoryArray[1].AvgVol < MinimumAvgVol )
	{
		return ( 0 );
	}

	if ( HistoryArray[0].Close < HistoryArray[1].Close )
	{
		return ( 0 );
	}
	
	AveragePrice = HistoryArray[1].Average[AVG_10];

	PriceChange = HistoryArray[0].Close - AveragePrice;
	PricePercent = 100.0 * ( PriceChange ) / AveragePrice;

	VolumeChange = HistoryArray[0].Volume - HistoryArray[1].AvgVol;
	VolumePercent = 100.0 * ( VolumeChange ) /  HistoryArray[1].AvgVol;

	if ( VolumePercent > VolumeCutoff && PricePercent > PriceCutoff )
	{
		if ( Debug )
		{
			printf ( "%-10.10s %-30.30s avg %8.2f curr %8.2f pct %8.2f vol %12ld avg %12ld pct %8.2f\n", 
				xstock.xsticker, xstock.xsname, 
				AveragePrice, HistoryArray[0].Close, PricePercent, 
				HistoryArray[0].Volume, HistoryArray[1].AvgVol, VolumePercent );
		}
		else
		{
			fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%.2f|%.2f|%ld|%ld|%.2f\n", 
				xstock.xsticker, xstock.xsname, 
				AveragePrice, HistoryArray[0].Close, PricePercent, 
				HistoryArray[0].Volume, HistoryArray[1].AvgVol, VolumePercent );
		}

		ReportCount++;
	}
	
	return ( 0 );
}
