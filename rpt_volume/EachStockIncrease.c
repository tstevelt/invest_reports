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

#include	"rpt_volume.h"

typedef struct
{
	char	Date[12];
	double	Close;
	long	Volume;
} RECORD;

//#define		PERIOD_ONE	45
//#define		PERIOD_TWO	5
//#define		TRIGGER_PCT	1.20
//#define		MINVOL		250000
#define		MAXDAYS		378
static	RECORD		Array[MAXDAYS];
static	int			Count;
static	int			TotalDays;

static int EachHistory ()
{
	if ( Count >= TotalDays )
	{
		return ( -1 );
	}

	if ( Debug )
	{
		printf ( "%3d %s %10.2f %10ld\n", Count, xhistory.xhdate, xhistory.xhclose, xhistory.xhvolume );
	}

	nsStrcpy ( Array[Count].Date, xhistory.xhdate );
	Array[Count].Close  = xhistory.xhclose;
	Array[Count].Volume = xhistory.xhvolume;
	Count++;

	return ( 0 );
}

int EachStockIncrease ()
{
	int		xd;
	double	AvgPricePrevious, AvgPriceCurrent;
	double	AvgVolumePrevious, AvgVolumeCurrent;
	double	PricePercent, VolumePercent;

	if ( NumberOfDaysPrevious + NumberOfDaysCurrent > MAXDAYS )
	{
		printf ( "Total days cannot exceed %d\n", MAXDAYS );
		return ( -1 );
	}

	TotalDays = NumberOfDaysPrevious + NumberOfDaysCurrent;

	Count = 0;

	sprintf ( WhereClause, "Hticker = '%s' and Hdate <= '%s'", xstock.xsticker , xstock.xslast );

	if ( LoadHistoryCB ( &MySql, WhereClause, "Hdate desc", &xhistory, (int(*)()) EachHistory, 0 ) < 1 )
	{
		return ( 0 );
	}

	if ( Count < TotalDays )
	{
		return ( 0 );
	}

	AvgPricePrevious = AvgPriceCurrent = AvgVolumePrevious = AvgVolumeCurrent = 0.0;

	for ( xd = 0; xd < NumberOfDaysCurrent; xd++ )
	{
		AvgPriceCurrent  += Array[xd].Close;
		AvgVolumeCurrent += Array[xd].Volume;
		if ( Debug )
		{
			printf ( "CURR: %3d %s %10ld %10.0f\n", xd,  Array[xd].Date, Array[xd].Volume, AvgVolumeCurrent );
		}
	}
	AvgPriceCurrent  /= (double) NumberOfDaysCurrent;
	AvgVolumeCurrent /= (double) NumberOfDaysCurrent;
	if ( Debug )
	{
		printf ( "AVG : %14.14s %11.0f\n", " ",   AvgVolumeCurrent );
	}

	for ( ; xd < TotalDays; xd++ )
	{
		AvgPricePrevious  += Array[xd].Close;
		AvgVolumePrevious += Array[xd].Volume;
		if ( Debug )
		{
			printf ( "PREV: %3d %s %10ld %10.0f\n", xd,  Array[xd].Date, Array[xd].Volume, AvgVolumePrevious );
		}
	}
	AvgPricePrevious  /= (double) NumberOfDaysPrevious;
	AvgVolumePrevious /= (double) NumberOfDaysPrevious;
	if ( Debug )
	{
		printf ( "AVG : %14.14s %11.0f\n", " ",   AvgVolumePrevious );
	}

	PricePercent = 100.0 * (AvgPriceCurrent) / AvgPricePrevious;
	VolumePercent = 100.0 * (AvgVolumeCurrent) / AvgVolumePrevious;

	if (( AvgVolumeCurrent >  MinimumVolume ) && 
		( PricePercent     >  100.0         ) &&
		( VolumePercent    >= TopPercent    ))
	{
		ReportCount++;
		if ( ReportCount == 1 )
		{
			sprintf ( OutFileName, "%s/volume_%d.dat", TEMPDIR, getpid() );
			if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
			{
				fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
				exit ( 1 );
			}
		}


		fprintf ( fpOutput, "%s|%s|%.2f|%.2f|%.2f|%.0f|%.0f|%.2f\n",
			xstock.xsticker,
			xstock.xsname,
			AvgPricePrevious,
			AvgPriceCurrent,
			PricePercent,
			AvgVolumePrevious,
			AvgVolumeCurrent,
			VolumePercent );
	}

	return ( 0 );
}
