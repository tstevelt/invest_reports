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

#include	"rpt_dicross.h"

#ifdef DONT_FIGHT_THE_MARKET
static int cmpmarket ( MARKET_RECORD *a, MARKET_RECORD *b )
{
	return ( nsStrcmp ( a->Date, b->Date ) );
}
#endif

static int PortfolioCount;
static double SellShares;

static int EachPortfolio ( DBY_QUERY *Query )
{
	if ( Query == NULL || Query->EachRow == NULL || Query->EachRow[0] == NULL || Query->EachRow[1] == NULL )
	{
		PortfolioCount = 0;
		SellShares = 0.0;
	}
	else
	{
		PortfolioCount = nsAtoi ( Query->EachRow[0] );
		SellShares     = nsAtof ( Query->EachRow[1] );
	}
	return ( 0 );
}

int EachStock ()
{
	int		Expected, ndx, offset;
	double	*outMinusDI, *outPlusDI, *outADX;
	int		outMinusBegIndex, outMinusNBElement;
	int		outPlusBegIndex, outPlusNBElement;
	int		outAdxBegIndex, outAdxNBElement;
#ifdef DONT_FIGHT_THE_MARKET
	MARKET_RECORD	Key, *Ptr;
#endif
	int		BuyItNow, SellItNow, Shares;
	double	Extended;

	if ( Debug > 1 )
	{
		printf ( "Loading history %s\n", xstock.xsticker );
	}

	CountVisited++;
	
	/*----------------------------------------------------------
		now load history for this stock
xxx	int rv;
xxx	rv = TA_MINUS_DI_Lookback ( 14 );
xxx	printf ( "Minus lookback %d\n", rv );
xxx	rv = TA_PLUS_DI_Lookback ( 14 );
xxx	printf ( "Plus lookback %d\n", rv );
xxx	rv = TA_ADX_Lookback ( 14 );
xxx	printf ( "ADX lookback %d\n", rv );
	----------------------------------------------------------*/
	HistoryCount = 0;
	memset ( HistoryArray, '\0', sizeof(HistoryArray) );
	Expected = CrossOverSince + 1 + 100;
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

	offset = outAdxBegIndex - outMinusBegIndex;

	/*---------------------------------------------------------------------------
		outMinusBegIndex 14
		outMinusNBElement 238
		outPlusBegIndex 14
		outPlusNBElement 238
		outAdxBegIndex 27
		outAdxNBElement 225
		DATE,CLOSE,ADX
		2018-07-18,86.58,7.292
		2018-07-19,87.00,8.153
		2018-07-20,86.15,8.952
	---------------------------------------------------------------------------*/
#ifdef DEBUG
	printf ( "outMinusBegIndex %d\n", outMinusBegIndex );
	printf ( "outMinusNBElement %d\n", outMinusNBElement );
	printf ( "outPlusBegIndex %d\n", outPlusBegIndex );
	printf ( "outPlusNBElement %d\n", outPlusNBElement );
	printf ( "outAdxBegIndex %d\n", outAdxBegIndex );
	printf ( "outAdxNBElement %d\n", outAdxNBElement );
	printf ( "DATE,CLOSE,-DI,+DI,ADX\n" );
#endif

	if ( Debug )
	{
		for ( ndx = 0; ndx < outMinusNBElement; ndx++ )
		{
			printf ( "-DI %.2f +DI %.2f\n", outMinusDI[ndx], outPlusDI[ndx] );
		}
		for ( ndx = 0; ndx < outAdxNBElement; ndx++ )
		{
			printf ( "ADX %.2f\n", outADX[ndx] );
		}
	}
	else
	{
		BuyItNow = SellItNow = 0;
		for ( ndx = 0; ndx < CrossOverSince; ndx++ )
		{
#ifdef SIMPLE_CROSSOVER 
			/*----------------------------------------------------------
				if Minus yesterday was above and today is below, buy it
			----------------------------------------------------------*/
			if (( outMinusDI[outMinusNBElement-2-ndx]  > outPlusDI[outMinusNBElement-2-ndx] ) &&
				( outMinusDI[outMinusNBElement-1-ndx] <= outPlusDI[outMinusNBElement-1-ndx] ))
			{
				BuyItNow = 1;
			}
#endif
			/*----------------------------------------------------------
				if Minus yesterday was below and today is above, sell it
			----------------------------------------------------------*/
			if (( outMinusDI[outMinusNBElement-2-ndx]  < outPlusDI[outMinusNBElement-2-ndx] ) &&
				( outMinusDI[outMinusNBElement-1-ndx] >= outPlusDI[outMinusNBElement-1-ndx] ))
			{
				SellItNow = 1;
				SellShares = 0.0;
				PortfolioCount = 0;
				if ( ReportMember )
				{
					sprintf ( Statement,
						"select count(*), sum(Pshares) from portfolio where Pmember = %ld and Pticker = '%s'", 
								ReportMember, xstock.xsticker );

					if ( dbySelectCB ( "invest", &MySql, Statement, (int(*)())EachPortfolio, LogFileName ) != 1 )
					{
						printf ( "dbySelectCB error\n" );
						exit ( 1 );
					}

					if ( PortfolioCount == 0 )
					{
						SellItNow = 0;
					}
				}
			}

			if ( BuyItNow )
			{
				if ( MinimumADX < outADX[outMinusNBElement-1-ndx-offset] )
				{
					Shares = (int) (BuyAmount / HistoryCloseArray[HistoryCount-1]);
#ifdef DEBUG					
					printf ( "TICKER %s %s BUY -DI %.2f +DI %.2f ADX %.2f\n",
						xstock.xsticker, 
						xstock.xsname, 
						outMinusDI[outMinusNBElement-1-ndx], 
						outPlusDI[outMinusNBElement-1-ndx],
						outADX[outMinusNBElement-1-ndx-offset] );

					printf ( "       %d SHARES AT $%.2f PER SHARE\n", Shares, HistoryCloseArray[HistoryCount-1] );
#else
					Extended = Shares * HistoryCloseArray[HistoryCount-1];

					fprintf ( fpOutput, "BUY|%s|%d|%.2f|%.2f|%.2f|%.2f|%.2f\n", 
							xstock.xsticker, Shares, HistoryCloseArray[HistoryCount-1], Extended,
							outMinusDI[outMinusNBElement-1-ndx], 
							outPlusDI[outMinusNBElement-1-ndx],
							outADX[outMinusNBElement-1-ndx-offset] );
#endif

				}
				break;
			}
			else if ( SellItNow )
			{

				if ( SellItNow )
				{
#ifdef DEBUG					
					printf ( "TICKER %s %s SELL -DI %.2f +DI %.2f ADX %.2f\n",
						xstock.xsticker, 
						xstock.xsname, 
						outMinusDI[outMinusNBElement-1-ndx], 
						outPlusDI[outMinusNBElement-1-ndx],
						outADX[outMinusNBElement-1-ndx-offset] );
#else
					Extended = SellShares * HistoryCloseArray[HistoryCount-1];

					fprintf ( fpOutput, "SELL|%s|%.4f|%.2f|%.2f|%.2f|%.2f|%.2f\n", 
							xstock.xsticker, 0.0 - SellShares, HistoryCloseArray[HistoryCount-1], 0.0 - Extended,
							outMinusDI[outMinusNBElement-1-ndx], 
							outPlusDI[outMinusNBElement-1-ndx],
							outADX[outMinusNBElement-1-ndx-offset] );
#endif
				}

				break;
			}
		}
	}

#ifdef STUFF

#ifdef SIMPLE_CROSSOVER 
	if (( outMinusDI[ndx+offset-1] > outPlusDI[ndx+offset-1] ) &&
		( outMinusDI[ndx+offset]   < outPlusDI[ndx+offset]   ) &&
		( CurrentShares == 0 ))
	{
		BuyItNow = 1;
	}
#endif

#ifdef MA_CONFIRM 
	if (( outMinusDI[ndx+offset-1] > outPlusDI[ndx+offset-1] ) &&
		( outMinusDI[ndx+offset]   < outPlusDI[ndx+offset]   ) &&
		( HistoryArray[ndx+outAdxBegIndex].Average[AVG_5] > HistoryArray[ndx+outAdxBegIndex].Average[AVG_10] ) &&
		( CurrentShares == 0 ))
	{
		BuyItNow = 1;
	}
#endif

#ifdef TOUGH_NICKLE
	if (( outMinusDI[ndx+offset] < outPlusDI[ndx+offset] ) &&
		( outADX[ndx-1]          < outADX[ndx]           ) &&
		( CurrentShares         == 0                     ))
	{
		BuyItNow = 1;
	}
#endif

#ifdef DONT_FIGHT_THE_MARKET
	sprintf ( Key.Date, HistoryArray[ndx+outAdxBegIndex].Date );
	Ptr = bsearch ( &Key, MarketArray, MarketCount, sizeof(MARKET_RECORD), (int(*)()) cmpmarket );

	if (( outMinusDI[ndx+offset-1] > outPlusDI[ndx+offset-1] ) &&
		( outMinusDI[ndx+offset]   < outPlusDI[ndx+offset]   ) &&
		( Ptr                     != (MARKET_RECORD *)0      ) &&
		( CurrentShares           == 0                       ))
	{
		BuyItNow = 1;
	}
#endif

#endif


	return ( 0 );
}
