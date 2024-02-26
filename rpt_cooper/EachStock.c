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

#include	"rpt_cooper.h"

static int IsInsideDay ( int Index )
{
	if (( HistoryArray[Index].High <= HistoryArray[Index-1].High ) &&
		( HistoryArray[Index].Low  >= HistoryArray[Index-1].Low  ))
	{
		return ( 1 );
	}

	return ( 0 );
}

#define		DIRECTION_NONE		0
#define		DIRECTION_BUY		1
#define		DIRECTION_SELL		-1

static int IsOneEighty ( int Index )
{
	double		RangeLast, RangePrevious, PercentLast, PercentPrevious, Diff;

	RangeLast     = HistoryArray[Index].High   - HistoryArray[Index].Low;
	RangePrevious = HistoryArray[Index-1].High - HistoryArray[Index-1].Low;

	Diff = HistoryArray[Index].Close - HistoryArray[Index].Low;
	PercentLast = 100.0 * Diff / RangeLast;

	Diff = HistoryArray[Index-1].Close - HistoryArray[Index-1].Low;
	PercentPrevious = 100.0 * Diff / RangePrevious;
	
	if ( PercentPrevious < 25.0 && PercentLast > 75.0 )
	{
		return ( DIRECTION_BUY );
	}
	
	if ( PercentPrevious > 75.0 && PercentLast < 25.0 )
	{
		return ( DIRECTION_SELL );
	}

	return ( DIRECTION_NONE );
}

#define		RANGE_OPEN		1
#define		RANGE_CLOSE		2

static double RangePercent ( int Index, int Which )
{
	double	Range, Diff, Percent;

	Range   = HistoryArray[Index].High  - HistoryArray[Index].Low;
	switch ( Which )
	{
		case RANGE_OPEN:
			Diff = HistoryArray[Index].Open - HistoryArray[Index].Low;
			break;
		case RANGE_CLOSE:
			Diff = HistoryArray[Index].Close - HistoryArray[Index].Low;
			break;
		default:
			printf ( "RangePercent: Unknown 'which'\n" );
			exit ( 1 );
	}
    Percent = 100.0 * Diff / Range;

	return ( Percent );
}

int EachStock ()
{
	int		rv, ndx, LastIndex, Expected, X_Count, Inside_Count;
	double	PreviousNineDaysMaxRange;
	double	TwoMonthCalendarHigh;
	double	TwoMonthCalendarLow;
	double	TenTradeDayHigh;
	double	TenTradeDayLow;
	double	OpenPercent, ClosePercent;
	double	OrderPrice, StopPrice, StopPercent;
	double	*outMinusDI, *outPlusDI, *outADX;
	int		outMinusBegIndex, outMinusNBElement;
	int		outPlusBegIndex, outPlusNBElement;
	int		outAdxBegIndex, outAdxNBElement;

	Expected = MINIMUM_HISTORY_RECORDS;

	LoadHistoryArrays ( &MySql, xstock.xsticker, xstock.xslast, SORT_ASCEND, Expected );

	if ( HistoryCount < Expected )
	{
		return ( 0 );
	}

	LastIndex = HistoryCount - 1;

	/*----------------------------------------------------------
		Global parameters
	----------------------------------------------------------*/
	if (( HistoryArray[LastIndex].Close < MINIMUM_PRICE ) ||
		( HistoryArray[LastIndex].Close > MAXIMUM_PRICE ))
	{
		return ( 0 );
	}

	if (( HistoryArray[LastIndex].AvgVol < MINIMUM_AVERAGE_VOLUME ) ||
		( HistoryArray[LastIndex].AvgVol > MAXIMUM_AVERAGE_VOLUME ))
	{
		return ( 0 );
	}

	/*----------------------------------------------------------
		ADX -DI +DI
	----------------------------------------------------------*/
	if (( outADX = calloc ( Expected, sizeof(double) )) == NULL )
	{
		printf ( "calloc failed\n" );
		return ( 0 );
	}
	if (( outMinusDI = calloc ( Expected, sizeof(double) )) == NULL )
	{
		printf ( "calloc failed\n" );
		return ( 0 );
	}
	if (( outPlusDI = calloc ( Expected, sizeof(double) )) == NULL )
	{
		printf ( "calloc failed\n" );
		return ( 0 );
	}

	TA_MINUS_DI ( 0, Expected-1, HistoryHighArray, HistoryLowArray, HistoryCloseArray, 14, &outMinusBegIndex, &outMinusNBElement, outMinusDI );
	TA_PLUS_DI ( 0, Expected-1, HistoryHighArray, HistoryLowArray, HistoryCloseArray, 14, &outPlusBegIndex, &outPlusNBElement, outPlusDI );
	TA_ADX ( 0, Expected-1, HistoryHighArray, HistoryLowArray, HistoryCloseArray, 14, &outAdxBegIndex, &outAdxNBElement, outADX );

	if ( Debug )
	{
		printf ( "%s  -DI %.2f +DI %.2f ADX %.2f\n", 
			xstock.xsticker,
			outMinusDI[outMinusNBElement-1],
			outPlusDI[outPlusNBElement-1],
			outADX[outAdxNBElement-1] );
	}

	/*----------------------------------------------------------
		previous nine days max range
		two month calendar high and low
	----------------------------------------------------------*/
	PreviousNineDaysMaxRange = HistoryArray[LastIndex-1].High - HistoryArray[LastIndex-1].Low;
	TwoMonthCalendarHigh     = HistoryArray[LastIndex-1].High;
	TwoMonthCalendarLow      = HistoryArray[LastIndex-1].Low;
	TenTradeDayHigh          = HistoryArray[LastIndex-1].High;
	TenTradeDayLow           = HistoryArray[LastIndex-1].Low;

	for ( ndx = 2; ndx < 10; ndx++ )
	{
		if ( PreviousNineDaysMaxRange < HistoryArray[LastIndex-ndx].High - HistoryArray[LastIndex-ndx].Low )
		{
			PreviousNineDaysMaxRange = HistoryArray[LastIndex-ndx].High - HistoryArray[LastIndex-ndx].Low;
		}
		if ( TwoMonthCalendarHigh < HistoryArray[LastIndex-ndx].High )
		{
			TwoMonthCalendarHigh = HistoryArray[LastIndex-ndx].High;
		}
		if ( TwoMonthCalendarLow > HistoryArray[LastIndex-ndx].Low )
		{
			TwoMonthCalendarLow = HistoryArray[LastIndex-ndx].Low;
		}
		if ( TenTradeDayHigh < HistoryArray[LastIndex-ndx].High )
		{
			TenTradeDayHigh = HistoryArray[LastIndex-ndx].High;
		}
		if ( TenTradeDayLow > HistoryArray[LastIndex-ndx].Low )
		{
			TenTradeDayLow = HistoryArray[LastIndex-ndx].Low;
		}
	}

	for ( ; ndx < 44; ndx++ )
	{
		if ( TwoMonthCalendarHigh < HistoryArray[LastIndex-ndx].High )
		{
			TwoMonthCalendarHigh = HistoryArray[LastIndex-ndx].High;
		}
		if ( TwoMonthCalendarLow > HistoryArray[LastIndex-ndx].Low )
		{
			TwoMonthCalendarLow = HistoryArray[LastIndex-ndx].Low;
		}
	}

	/*----------------------------------------------------------
		Report Expansion Breakouts
	----------------------------------------------------------*/
	if ( PreviousNineDaysMaxRange < HistoryArray[LastIndex].High - HistoryArray[LastIndex].Low )
	{
		/*-----------------------------------------------------------------------------
			Cooper bases expansion stops on yesterday's close, apparently real time
		-----------------------------------------------------------------------------*/
		if ( TwoMonthCalendarHigh < HistoryArray[LastIndex].High )
		{
			OrderPrice = HistoryArray[LastIndex].High + 0.1;
			StopPrice = HistoryArray[LastIndex].Close - 1.0;
			StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
			TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

			fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|BUY Expansion Breakout\n", 
				xstock.xsticker, xstock.xsname, 
				HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

			ReportCount++;
		}
		else if ( TwoMonthCalendarLow > HistoryArray[LastIndex].Low )
		{
			OrderPrice = HistoryArray[LastIndex].Low - 0.1;
			StopPrice = HistoryArray[LastIndex].Close + 1.0;
			StopPercent = 100.0 * (StopPrice - OrderPrice) / OrderPrice;
			TwoMonthCalendarLow = HistoryArray[LastIndex].Low;

			fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|SELL Expansion Breakdown\n", 
				xstock.xsticker, xstock.xsname, 
				HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

			ReportCount++;
		}
	}

	/*----------------------------------------------------------
		Report Expansion Pivot
	----------------------------------------------------------*/
	if ( PreviousNineDaysMaxRange < HistoryArray[LastIndex].High - HistoryArray[LastIndex].Low )
	{
		if (( HistoryArray[LastIndex].Close > HistoryArray[LastIndex].Average[AVG_50] ) &&
			( HistoryArray[LastIndex].Open  < HistoryArray[LastIndex].Average[AVG_50] ))
		{
			OrderPrice = HistoryArray[LastIndex].High + 0.1;
			StopPrice = HistoryArray[LastIndex].Close - 1.0;
			StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
			TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

			fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|BUY Expansion Pivot BUY\n", 
				xstock.xsticker, xstock.xsname, 
				HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

			ReportCount++;
		}
		else if (( HistoryArray[LastIndex].Close < HistoryArray[LastIndex].Average[AVG_50] ) &&
			     ( HistoryArray[LastIndex].Open  > HistoryArray[LastIndex].Average[AVG_50] ))
		{
			OrderPrice = HistoryArray[LastIndex].Low - 0.1;
			StopPrice = HistoryArray[LastIndex].Close + 1.0;
			StopPercent = 100.0 * (StopPrice - OrderPrice) / OrderPrice;
			TwoMonthCalendarLow = HistoryArray[LastIndex].Low;

			fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|SELL Expansion Pivot SELL\n", 
				xstock.xsticker, xstock.xsname, 
				HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

			ReportCount++;
		}
	}

	/*----------------------------------------------------------
		1-2-3 pullback 
	----------------------------------------------------------*/
	if ( outADX[outAdxNBElement-1] > 30.0 )
	{
		X_Count = Inside_Count = 0;

		if ( outMinusDI[outMinusNBElement-1] < outPlusDI[outPlusNBElement-1] )
		{
			for ( ndx = 0; ndx < 3; ndx++ )
			{
				if ( HistoryArray[LastIndex-ndx].Low < HistoryArray[LastIndex-ndx-1].Low )
				{
					if ( Debug )
					{
						printf ( "X Low %.2f < Prev %.2f\n", HistoryArray[LastIndex-ndx].Low, HistoryArray[LastIndex-ndx-1].Low );
					}
					X_Count++;
				}
				else if ( IsInsideDay ( LastIndex-ndx ) == 1 )
				{
					if ( Debug )
					{
						printf ( "I ndx %d\n", LastIndex-ndx );
					}
					Inside_Count++;
				}
			}
			if (( X_Count == 3 ) || ( X_Count == 2 && Inside_Count == 1 ))
			{
				OrderPrice = HistoryArray[LastIndex].High + 0.1;
				StopPrice = HistoryArray[LastIndex].Low;
				StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
				TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

				fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|BUY 1-2-3 Pullback (%d-%d)\n", 
					xstock.xsticker, xstock.xsname, 
					HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent, X_Count, Inside_Count );

				ReportCount++;
			}
		}
		else if ( outMinusDI[outMinusNBElement-1] > outPlusDI[outPlusNBElement-1] )
		{
			for ( ndx = 0; ndx < 3; ndx++ )
			{
				if ( HistoryArray[LastIndex-ndx].High > HistoryArray[LastIndex-ndx-1].High )
				{
					X_Count++;
				}
				else if ( IsInsideDay ( LastIndex-ndx ) == 1 )
				{
					Inside_Count++;
				}
			}
			if (( X_Count == 3 ) || ( X_Count == 2 && Inside_Count == 1 ))
			{
				OrderPrice = HistoryArray[LastIndex].Low - 0.1;
				StopPrice = HistoryArray[LastIndex].High;
				StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
				TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

				fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|SELL 1-2-3 Pullback (%d-%d)\n", 
					xstock.xsticker, xstock.xsname, 
					HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent, X_Count, Inside_Count );

				ReportCount++;
			}
		}
	}

	/*----------------------------------------------------------
		180's
	----------------------------------------------------------*/
	if (( rv = IsOneEighty ( LastIndex )) != DIRECTION_NONE )
	{
		if (( rv == DIRECTION_BUY ) &&
			( HistoryArray[LastIndex].Average[AVG_10] > HistoryArray[LastIndex].Average[AVG_50] ) &&
			( HistoryArray[LastIndex].Close > HistoryArray[LastIndex].Average[AVG_10] ) &&
			( HistoryArray[LastIndex].Close > HistoryArray[LastIndex].Average[AVG_50] ))
		{
			OrderPrice = HistoryArray[LastIndex].High + 0.1;
			StopPrice = OrderPrice - 1.0;
			StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
			TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

			fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|BUY 180\n", 
				xstock.xsticker, xstock.xsname, 
				HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

			ReportCount++;
		}
		else if (( rv == DIRECTION_SELL ) &&
				 ( HistoryArray[LastIndex].Average[AVG_10] < HistoryArray[LastIndex].Average[AVG_50] ) &&
			     ( HistoryArray[LastIndex].Close < HistoryArray[LastIndex].Average[AVG_10] ) &&
			     ( HistoryArray[LastIndex].Close < HistoryArray[LastIndex].Average[AVG_50] ))
		{
			OrderPrice = HistoryArray[LastIndex].Low - 0.1;
			StopPrice = OrderPrice + 1.0;
			StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
			TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

			fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|SELL 180\n", 
				xstock.xsticker, xstock.xsname, 
				HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

			ReportCount++;
		}
	}

	/*----------------------------------------------------------
		Gilligan's Island
	----------------------------------------------------------*/
	if (( TwoMonthCalendarLow > HistoryArray[LastIndex].Low ) &&
		( HistoryArray[LastIndex].Open < HistoryArray[LastIndex-1].Low ) &&
		( RangePercent ( LastIndex, RANGE_CLOSE ) > 50.0 ) &&
		( HistoryArray[LastIndex].Close >= HistoryArray[LastIndex].Open ))
	{
		OrderPrice = HistoryArray[LastIndex].High + 0.1;
		StopPrice = OrderPrice - 1.0;
		StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
		TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

		fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|BUY Gilligan's Island\n", 
			xstock.xsticker, xstock.xsname, 
			HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

		ReportCount++;
	}
	else if (( TwoMonthCalendarHigh < HistoryArray[LastIndex].High ) &&
			 ( HistoryArray[LastIndex].Open > HistoryArray[LastIndex-1].High ) &&
			 ( RangePercent ( LastIndex, RANGE_CLOSE ) < 50.0 ) &&
			 ( HistoryArray[LastIndex].Close >= HistoryArray[LastIndex].Open ))
	{
		OrderPrice = HistoryArray[LastIndex].Low - 0.1;
		StopPrice = OrderPrice + 1.0;
		StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
		TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

		fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|SELL Gilligan's Island\n", 
			xstock.xsticker, xstock.xsname, 
			HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

		ReportCount++;
	}

	/*----------------------------------------------------------
		Boomer
	----------------------------------------------------------*/
	if (( IsInsideDay ( LastIndex ) == 1   ) && 
		( IsInsideDay ( LastIndex-1 ) == 1 ) &&
		( outADX[outAdxNBElement-1] > 30.0 ))
	{
		if ( outMinusDI[outMinusNBElement-1] < outPlusDI[outPlusNBElement-1] )
		{
			OrderPrice = HistoryArray[LastIndex].High + 0.1;
			StopPrice = HistoryArray[LastIndex].Low - 0.1;
			StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
			TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

			fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|BUY Boomer\n", 
				xstock.xsticker, xstock.xsname, 
				HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

			ReportCount++;
		}
		else if ( outMinusDI[outMinusNBElement-1] > outPlusDI[outPlusNBElement-1] )
		{
			OrderPrice = HistoryArray[LastIndex].Low - 0.1;
			StopPrice = HistoryArray[LastIndex].High + 0.1;
			StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
			TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

			fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|SELL Boomer\n", 
				xstock.xsticker, xstock.xsname, 
				HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

			ReportCount++;
		}
	}

	/*----------------------------------------------------------
		Lizard
	----------------------------------------------------------*/
	OpenPercent = RangePercent ( LastIndex, RANGE_OPEN );
	ClosePercent = RangePercent ( LastIndex, RANGE_CLOSE );
	if (( OpenPercent > 75.0 ) && 
		( ClosePercent > 75.0 ) &&
		( TenTradeDayLow > HistoryArray[LastIndex].Low ))
	{
		OrderPrice = HistoryArray[LastIndex].High + 0.1;
		StopPrice = OrderPrice - 1.0;
		StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
		TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

		fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|BUY Lizard\n", 
			xstock.xsticker, xstock.xsname, 
			HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

		ReportCount++;
	}
	else if (( OpenPercent < 25.0 ) && 
			 ( ClosePercent < 25.0 ) &&
			 ( TenTradeDayHigh < HistoryArray[LastIndex].High ))
	{
		OrderPrice = HistoryArray[LastIndex].Low - 0.1;
		StopPrice = OrderPrice + 1.0;
		StopPercent = 100.0 * ( OrderPrice - StopPrice ) / OrderPrice;
		TwoMonthCalendarHigh = HistoryArray[LastIndex].High;

		fprintf ( fpOutput, "%-10.10s|%-30.30s|%.2f|%ld|%.2f|%.2f|%.2f|SELL Lizard\n", 
			xstock.xsticker, xstock.xsname, 
			HistoryArray[LastIndex].Close, HistoryArray[LastIndex].Volume, OrderPrice, StopPrice, StopPercent );

		ReportCount++;
	}

	return ( 0 );
}
