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

#include	"rpt_adx.h"

int EachStock ()
{
	int		Expected;
	double	*outMinusDI, *outPlusDI, *outADX;
	int		outMinusBegIndex, outMinusNBElement;
	int		outPlusBegIndex, outPlusNBElement;
	int		outAdxBegIndex, outAdxNBElement;
	double	DiDiff;
	char	*Signal;

	if ( Debug > 1 )
	{
		printf ( "Loading history %s\n", xstock.xsticker );
	}

	CountVisited++;
	
	/*----------------------------------------------------------
		now load history for this stock
	----------------------------------------------------------*/
	HistoryCount = 0;
	memset ( HistoryArray, '\0', sizeof(HistoryArray) );
	Expected = 101;
	LoadHistoryArrays ( &MySql, xstock.xsticker, xstock.xslast, SORT_ASCEND, Expected );

	if ( HistoryCount < Expected )
	{
		if ( Debug )
		{
			printf ( "Did not load expected count %d, loaded %d\n", Expected, HistoryCount );
		}
		return ( 0 );
	}

	if (( outADX = calloc ( Expected, sizeof(double) )) == NULL )
	{
		printf ( "calloc failed\n" );
		return ( 0 );
	}
	if (( outMinusDI = calloc ( Expected, sizeof(double) )) == NULL )
	{
		printf ( "calloc failed\n" );
		free ( outADX );
		return ( 0 );
	}
	if (( outPlusDI = calloc ( Expected, sizeof(double) )) == NULL )
	{
		printf ( "calloc failed\n" );
		free ( outADX );
		free ( outMinusDI );
		return ( 0 );
	}

	TA_MINUS_DI ( 0, Expected-1, HistoryHighArray, HistoryLowArray, HistoryCloseArray, 14, &outMinusBegIndex, &outMinusNBElement, outMinusDI );
	TA_PLUS_DI ( 0, Expected-1, HistoryHighArray, HistoryLowArray, HistoryCloseArray, 14, &outPlusBegIndex, &outPlusNBElement, outPlusDI );
	TA_ADX ( 0, Expected-1, HistoryHighArray, HistoryLowArray, HistoryCloseArray, 14, &outAdxBegIndex, &outAdxNBElement, outADX );

//	offset = outAdxBegIndex - outMinusBegIndex;

	if ( outADX[outAdxNBElement-1] < MinimumADX )
	{
		free ( outADX );
		free ( outMinusDI );
		free ( outPlusDI );
		return ( 0 );
	}

	if ( outMinusDI[outMinusNBElement-1] > outPlusDI[outMinusNBElement-1] )
	{
		DiDiff = outMinusDI[outMinusNBElement-1] - outPlusDI[outMinusNBElement-1];
		Signal = "Sell";
		if ( BuySellAll == 'B' )
		{
			free ( outADX );
			free ( outMinusDI );
			free ( outPlusDI );
			return ( 0 );
		}
	}
	else
	{
		DiDiff = outPlusDI[outMinusNBElement-1] - outMinusDI[outMinusNBElement-1];
		Signal = "Buy";
		if ( BuySellAll == 'S' )
		{
			free ( outADX );
			free ( outMinusDI );
			free ( outPlusDI );
			return ( 0 );
		}
	}
	if ( DiDiff < MinimumDI )
	{
		free ( outADX );
		free ( outMinusDI );
		free ( outPlusDI );
		return ( 0 );
	}

	if ( Debug )
	{
		printf ( "%s %s -DI %.2f +DI %.2f  ADX %.2f\n", 
				xstock.xsticker, Signal, outMinusDI[outMinusNBElement-1], outPlusDI[outMinusNBElement-1],  outADX[outAdxNBElement-1] );
	}
	else
	{
		fprintf ( fpOutput, "%s|%s|%.2f|%s|%.2f|%.2f|%.2f|%.2f\n", 
				xstock.xsticker, 
				xstock.xsname,
				HistoryCloseArray[HistoryCount-1],
				Signal,
				outMinusDI[outMinusNBElement-1], 
				outPlusDI[outMinusNBElement-1],
				outADX[outAdxNBElement-1],
				xaverage.xarsi );
	}

	free ( outADX );
	free ( outMinusDI );
	free ( outPlusDI );
	return ( 0 );
}
