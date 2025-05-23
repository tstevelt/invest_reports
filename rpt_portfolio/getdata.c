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

#include	"rpt_portfolio.h"

extern	double	xstock_ttm_dividends;

static void InsertArray ( XSTOCK *ptrStock, XPORTFOLIO *ptrPortfolio, DATEVAL *BoughtDate, double *StockStartPrice )
{
	int		ndx;
	double	TotalShares, ExtendCost;

	TotalShares = ExtendCost = 0.0;
	for ( ndx = 0; ndx < MstarCount; ndx++ )
	{
		if ( strcmp ( ptrStock->xsticker, MstarArray[ndx].Ticker ) == 0 )
		{
			TotalShares = MstarArray[ndx].Shares;
			ExtendCost  = MstarArray[ndx].Shares * MstarArray[ndx].Cost;
			
			TotalShares += ptrPortfolio->xpshares;
			ExtendCost  += ptrPortfolio->xpshares * ptrPortfolio->xpprice;

			MstarArray[ndx].Shares = TotalShares;
			MstarArray[ndx].Cost   = ExtendCost / TotalShares;

			return;
		}
	}

	if ( MstarCount >= PAID_MEMBER_PORTFOLIO_LIMIT )
	{
		printf ( "Exceeds %d records\n", PAID_MEMBER_PORTFOLIO_LIMIT );
		exit ( 0 );
	}

	MstarArray[MstarCount].Type = ptrStock->xstype[0];
	strcpy ( MstarArray[MstarCount].Ticker, ptrStock->xsticker );
	memcpy ( &MstarArray[MstarCount].Date, BoughtDate, sizeof(DATEVAL) );
	MstarArray[MstarCount].Shares = ptrPortfolio->xpshares;
	MstarArray[MstarCount].Cost = *StockStartPrice;
	MstarCount++;


}

int EachHistory ()
{
	return ( 0 );
}

static long NoMilliseconds ( char *TimeString )
{
	char	TimeBuffer[16];
	long	rv;

	sprintf ( TimeBuffer, "%-10.10s", TimeString );
	rv = nsAtol ( TimeBuffer );
	return ( rv );
}

int getdata ()
{
	int		rv, HaveFundamentals, xl;
	double	StockStartPrice, StockYesterdayClose, StockPercent, StockLatestPrice;
	double	BenchmarkStartPrice, BenchmarkCurrentPrice, BenchmarkPercent;
	double	ComparedToBenchmark;
	DATEVAL	BoughtDateval;
	int		OwnedDays;
	double	OwnedYears;
	double	AnnualYield, CollectedDividends, TotalYield;
	double	AvgVolumeUp, AvgVolumeDown;
	double	AD_Ratio;
	double	CompareTo, Difference, Percent;
	char	cmdline[1024];
	//FILE	*pfp;
	char	*buffer;
	JSON_NAME_VALUE     *ptrNameValue;
	double	StockOpenPrice, StockClosePrice;
	long	StockOpenTime, StockCloseTime;
	char	StockOpenFlag, StockCloseFlag;
	double	PercentInvest, PercentValue;

	StockStartPrice = xportfolio.xpprice;

	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 1 )) != 1 )
	{
		fprintf ( stderr, "LoadStock %s returned %d\n", xportfolio.xpticker, rv );
		return ( 0 );
	}

	switch ( StockType )
	{
		case 'A':
			break;
		case 'S':
			switch ( xstock.xstype[0] )
			{
				case STYPE_STOCK:
				case STYPE_ADR:
				case STYPE_PREFER:
				case STYPE_OTHER:
					break;
				default:
					return ( 0 );
			}
			break;
		case 'E':
			switch ( xstock.xstype[0] )
			{
				case STYPE_ETF:
				case STYPE_REIT:
					break;
				default:
					return ( 0 );
			}
			break;
		case 'F':
			switch ( xstock.xstype[0] )
			{
				case STYPE_BOND:
					break;
				default:
					return ( 0 );
			}
			break;
		case 'C':
			switch ( xstock.xstype[0] )
			{
				case STYPE_CRYPTO:
					break;
				default:
					return ( 0 );
			}
			break;
	}

	if ( ReportStyle == STYLE_YOY )
	{
		switch ( xstock.xstype[0] )
		{
			case STYPE_BOND:
			case STYPE_CRYPTO:
				return ( 0 );
		}

		/*----------------------------------------------------------
			if stock owned over one year, get year ago price
		----------------------------------------------------------*/
#ifdef ONLY_HELD_FOR_YEAR_OR_MORE
		if ( nsStrcmp ( xportfolio.xpdate, ascYearAgo ) < 0 )
		{
			sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpticker, ascYearAgo );
			if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
			{
				fprintf ( stderr, 
					"LoadHistory returned %d, cannot find year ago price for %s %s<br>\n",
							rv, xportfolio.xpticker, xportfolio.xpdate );
				return ( 0 );
			}
			StockStartPrice = xhistory.xhclose;
		}
#else
		sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpticker, ascYearAgo );
		if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
		{
			fprintf ( stderr, 
				"LoadHistory returned %d, cannot find year ago price for %s %s<br>\n",
						rv, xportfolio.xpticker, xportfolio.xpdate );
			return ( 0 );
		}
		StockStartPrice = xhistory.xhclose;
#endif

	}

	/*----------------------------------------------------------
		load fundamentals
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Fticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 )) != 1 )
	{
		memset ( &xfundamental, '\0', sizeof(xfundamental) );
		HaveFundamentals = 0;
	}
	else
	{
		HaveFundamentals = 1;
	}

	switch ( xstock.xstype[0] )
	{
		case STYPE_BOND:
			AD_Ratio = 0.0;
			xstock_ttm_dividends = 0.0;
			xhistory.xhclose = 1000.0;
			break;
		default:
			/*----------------------------------------------------------
				set current to cost for CASH accounts.  tms 07/09/2022
			----------------------------------------------------------*/
			if ( xstock.xstype[0] == STYPE_CRYPTO || xstock.xslast[0] != '2'|| nsStrncmp ( xstock.xslast, "(null)", 6 ) == 0 )
			{
//				printf ( "Stock %s does not have any history\n", xportfolio.xpticker );
//				return ( 0 );

				AD_Ratio = 0.0;

				xhistory.xhclose = xportfolio.xpprice;
			}
			else
			{
				if ( CalcAccumDistrVolume ( &MySql, xstock.xsticker, xstock.xslast, 30, &AD_Ratio, &AvgVolumeUp, &AvgVolumeDown ) != 0  )
				{
					AD_Ratio = 0.0;
				}
				/*----------------------------------------------------------
					get stock current price
				----------------------------------------------------------*/
				sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpticker, xstock.xslast );
				if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
				{
					fprintf ( stderr, 
						"LoadHistory returned %d, cannot find stock price for %s %s<br>\n", rv, xportfolio.xpticker, xstock.xslast );
					return ( 0 );
				}
			}
			break;
	}

	StrToDatevalFmt ( xportfolio.xpdate, DATEFMT_YYYY_MM_DD, &BoughtDateval );

//	use today in order to handle bonds which don't have history.  tms 04/08/2022
//	StrToDatevalFmt ( xstock.xslast,     DATEFMT_YYYY_MM_DD, &dvToday );

	OwnedDays = (int) DateDiff ( &BoughtDateval, &dvToday );
	OwnedYears = (double) OwnedDays / 365.0;
	AnnualYield = 100.0 * xstock_ttm_dividends / xportfolio.xpprice;	

	CollectedDividends = LoadDividendSum ( &MySql, xportfolio.xpticker, xportfolio.xpdate );
	TotalYield = 100.0 * CollectedDividends / xportfolio.xpprice;

	if ( Debug )
	{
		printf ( "%s: owned %d days %.2f years, annual %.2f total %.2f\n", 
			xportfolio.xpticker, OwnedDays, OwnedYears, AnnualYield, TotalYield );
	}

	StockYesterdayClose = xhistory.xhclose;

	StockPercent = 100.0 * ( StockYesterdayClose - StockStartPrice ) / StockStartPrice;

	if ( ReportStyle == STYLE_ALERTS )
	{
		if (  xportfolio.xpalerttype[0] == ALERT_NONE )
		{
			return ( 0 );
		}

		/*----------------------------------------------------------
			compare current stock price to 50 day moving average
			when custom is implemented, the can use other # of days.
		----------------------------------------------------------*/
		sprintf ( WhereClause, "Aticker = '%s' and Adate = '%s'", xportfolio.xpticker, xstock.xslast );
		LoadAverage ( &MySql, WhereClause, &xaverage, 0 );

		if (  xportfolio.xpalerttype[0] == ALERT_BENCHMARK )
		{
			/*----------------------------------------------------------
				get benchmark start price
			----------------------------------------------------------*/
			sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpbenchmark, xportfolio.xpdate );
			if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
			{
				fprintf ( stderr, 
					"LoadHistory returned %d, cannot find benchmark (%s) price for %s %s<br>\n",
							rv, xportfolio.xpbenchmark, xportfolio.xpticker, xportfolio.xpdate );
				return ( 0 );
			}
			BenchmarkStartPrice = xhistory.xhclose;

#ifdef USE_PORTFOLIO_PRICE_THIS_IS_WRONG
			BenchmarkStartPrice = xportfolio.xpprice;
#endif

			/*----------------------------------------------------------
				get benchmark current price
			----------------------------------------------------------*/
			sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpbenchmark, xstock.xslast );
			if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
			{
				fprintf ( stderr, 
					"LoadHistory returned %d, cannot find benchmark (%s) price for %s %s<br>\n", 
							rv, xportfolio.xpbenchmark, xportfolio.xpticker, xstock.xslast );
				return ( 0 );
			}
			BenchmarkCurrentPrice = xhistory.xhclose;

			BenchmarkPercent = 100.0 * ( BenchmarkCurrentPrice - BenchmarkStartPrice ) / BenchmarkStartPrice;

if ( Debug )
{
	printf ( "%s: stock start %.2f current %.2f ret %.2f, benchmark start %.2f current %.2f ret %.2f\n", 
					xportfolio.xpticker,
					StockStartPrice, StockYesterdayClose, StockPercent,
					BenchmarkStartPrice, BenchmarkCurrentPrice, BenchmarkPercent );
}
			
			/*----------------------------------------------------------
				Stock + Div		=	18
				Benchmark		=	59
				Compared		=	-41
				Alert			= 	-10
			----------------------------------------------------------*/
			ComparedToBenchmark = (StockPercent + TotalYield) - BenchmarkPercent;
		}
		else
		{
			BenchmarkStartPrice = 0.0;
			BenchmarkCurrentPrice = 0.0;
			BenchmarkPercent = 0.0;
			ComparedToBenchmark = 0.0;
		}
		
	}

	ReportCount++;
	if ( ReportCount == 1 )
	{
		sprintf ( OutFileName, "%s/portfolio_%d.dat", TEMPDIR, getpid() );
		if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
		{
			fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
			exit ( 1 );
		}
	}

	switch ( ReportStyle )
	{
		case STYLE_BASE:
			PercentInvest = 100.0 * xportfolio.xpshares * StockStartPrice / TotalInvest;
			PercentValue  = 100.0 * xportfolio.xpshares * StockYesterdayClose / TotalValue;;
			fprintf ( fpOutput, "%s|", xportfolio.xpbroker );
			fprintf ( fpOutput, "%c|", xstock.xstype[0] );
			fprintf ( fpOutput, "%s|", xportfolio.xpticker );
			fprintf ( fpOutput, "%02d/%02d/%04d|", BoughtDateval.month, BoughtDateval.day, BoughtDateval.year4 );
			fprintf ( fpOutput, "%.2f|", xportfolio.xpshares );
			fprintf ( fpOutput, "%.2f|", StockStartPrice );
			fprintf ( fpOutput, "%.2f|", xportfolio.xpshares * StockStartPrice );
			fprintf ( fpOutput, "%.2f|", PercentInvest );
			fprintf ( fpOutput, "%.2f|", StockYesterdayClose );
			fprintf ( fpOutput, "%.2f|", xportfolio.xpshares * StockYesterdayClose );
			fprintf ( fpOutput, "%.2f|", PercentValue );
			fprintf ( fpOutput, "%.2f|", xportfolio.xpshares * (StockYesterdayClose-StockStartPrice) );
			fprintf ( fpOutput, "%.2f|", StockPercent );
			break;

		case STYLE_MSTAR:
			/*---------------------------------------------------------------------------
				Use array in order to combine duplicate symbols held in multiple accts.
			---------------------------------------------------------------------------*/
			InsertArray ( &xstock, &xportfolio, &BoughtDateval, &StockStartPrice );
			break;

		case STYLE_FUNDAMENTAL:
			fprintf ( fpOutput, "%s|", xportfolio.xpticker );
			fprintf ( fpOutput, "%s|", xstock.xsname );
			fprintf ( fpOutput, "%c|", xstock.xstype[0] );
			if ( HaveFundamentals == 0 )
			{
				fprintf ( fpOutput, "NONE|" );
				fprintf ( fpOutput, "|" );
				fprintf ( fpOutput, "|" );
				fprintf ( fpOutput, "|" );
				fprintf ( fpOutput, "|" );
				fprintf ( fpOutput, "|" );
				fprintf ( fpOutput, "|" );
				fprintf ( fpOutput, "|" );
				fprintf ( fpOutput, "|" );
			}
			else
			{
				fprintf ( fpOutput, "%.2f|", xfundamental.xfpettm );
				fprintf ( fpOutput, "%.2f|", xfundamental.xfpefwd );
				if ( TotalYield > 0 && xfundamental.xfyield == 0 )
				{
					fprintf ( fpOutput, "%.2f|", TotalYield );
				}
				else
				{
					fprintf ( fpOutput, "%.2f|", xfundamental.xfyield );
				}
				fprintf ( fpOutput, "%.2f|", xfundamental.xfgrow5 );
				fprintf ( fpOutput, "%.2f|", xfundamental.xfbeta );
				fprintf ( fpOutput, "%.2f|", xfundamental.xfquick );
				fprintf ( fpOutput, "%.2f|", xfundamental.xfdebteq );
				fprintf ( fpOutput, "%.2f|", xfundamental.xfvalue );
				fprintf ( fpOutput, "%.2f|", xstock.xsclose );
				fprintf ( fpOutput, "%.2f|", xfundamental.xftarget );
			}
			break;

		case STYLE_TODAY:
		case STYLE_OVERNIGHT:
			// StockLatestPrice = 123.0;
			// open
			// openTime
			// close
			// closeTime
			// latestPrice

			if ( xstock.xstype[0] == STYPE_BOND )
			{
				StockOpenPrice = StockClosePrice = StockLatestPrice = StockYesterdayClose = 1000.0;
			}
			else if ( xstock.xstype[0] == STYPE_CRYPTO )
			{
				StockOpenPrice = StockClosePrice = StockLatestPrice = StockYesterdayClose = xportfolio.xpprice;
			}
			else
			{
				xl = lastchr ( xportfolio.xpticker, nsStrlen ( xportfolio.xpticker ));
				xportfolio.xpticker[xl] = '\0';
				if ( UseTiingo )
				{
					/*---------------------------------------------------------------------------
						case "$OPTION" in
							'-quote' )
								# curl -s "https://api.tiingo.com/iex/$TICKER?token=$APISTR"  | JsonTree -
								curl -s "https://api.tiingo.com/iex/$TICKER?token=$APISTR"  | sed 's/,/\n/g'
								;;
					env_ApiURL, has /tiingo at the end
					---------------------------------------------------------------------------*/
					sprintf ( cmdline, "curl -s 'https://api.tiingo.com/iex/%s?token=%s' > %s",
						xportfolio.xpticker,
						env_ApiKey,
						PRICEFILE );
				}
				else
				{
					sprintf ( cmdline, "curl -s '%s/stock/%s/quote?format=json&token=%s' > %s",
						env_ApiURL,
						xportfolio.xpticker,
						env_ApiKey,
						PRICEFILE );
				}

				IEX_RateLimit ( 0 );

				/*----------------------------------------------------------
					fixit - still using curl system() instead of library.
				----------------------------------------------------------*/
				system ( cmdline );

				if (( buffer = JsonOpenFileAndRead ( PRICEFILE )) == NULL )
				{
					if ( Debug )
					{
						printf ( "Cannot open price file %s\n", PRICEFILE );
					}
					StockLatestPrice = 0.0;
				}

				if ( UseTiingo == 0 )
				{
					/*----------------------------------------------------------
						IEX dead
					----------------------------------------------------------*/
					if ( nsStrncmp ( buffer, "Unknown symbol", 14 ) == 0 )
					{
						if ( Debug )
						{
							printf ( "Unknown symbol %s<br>\n", xstock.xsticker );
							printf ( "%s<br>\n", cmdline );
						}
						StockLatestPrice = 0.0;
					}
					else
					{
						/*----------------------------------------------------------
							get open price, first check to see if "official"
							otherwise, get iexopen and set openflag
						----------------------------------------------------------*/
						StockOpenPrice = 0.0;
						StockOpenFlag = ' ';
						ptrNameValue = JsonScan ( buffer, "openTime", JSON_FIRST );
						if ( ptrNameValue != NULL && (StockOpenTime = NoMilliseconds ( ptrNameValue->Value )) > 0 )
						{
							ptrNameValue = JsonScan ( buffer, "open", JSON_FIRST );
							if ( ptrNameValue != NULL )
							{
								StockOpenPrice = nsAtof ( ptrNameValue->Value );
							}
						}
						else
						{
							ptrNameValue = JsonScan ( buffer, "iexOpen", JSON_FIRST );
							if ( ptrNameValue != NULL )
							{
								StockOpenPrice = nsAtof ( ptrNameValue->Value );
							}
							StockOpenFlag = 'X';

							ptrNameValue = JsonScan ( buffer, "iexOpenTime", JSON_FIRST );
							if ( ptrNameValue != NULL )
							{
								StockOpenTime = NoMilliseconds ( ptrNameValue->Value );
							}

						}

						/*----------------------------------------------------------
							get close price, first check to see if "official"
							otherwise, get iexclose and set closeflag
						----------------------------------------------------------*/
						StockClosePrice = 0.0;
						StockCloseFlag = ' ';
						ptrNameValue = JsonScan ( buffer, "closeTime", JSON_FIRST );
						if ( ptrNameValue != NULL && (StockCloseTime = NoMilliseconds ( ptrNameValue->Value )) > 0 )
						{
							ptrNameValue = JsonScan ( buffer, "close", JSON_FIRST );
							if ( ptrNameValue != NULL )
							{
								StockClosePrice = nsAtof ( ptrNameValue->Value );
							}
						}
						else
						{
							ptrNameValue = JsonScan ( buffer, "iexClose", JSON_FIRST );
							if ( ptrNameValue != NULL )
							{
								StockClosePrice = nsAtof ( ptrNameValue->Value );
							}
							StockCloseFlag = 'X';

							ptrNameValue = JsonScan ( buffer, "iexCloseTime", JSON_FIRST );
							if ( ptrNameValue != NULL )
							{
								StockCloseTime = NoMilliseconds ( ptrNameValue->Value );
							}
						}

						/*----------------------------------------------------------
							get latest price
						----------------------------------------------------------*/
						StockLatestPrice = 0.0;
						ptrNameValue = JsonScan ( buffer, "latestPrice", JSON_FIRST );
						if ( ptrNameValue != NULL )
						{
							StockLatestPrice = nsAtof ( ptrNameValue->Value );
						}

						/*----------------------------------------------------------
							free json buffer
						----------------------------------------------------------*/
						free ( buffer );
						
						if ( StockCloseTime > StockOpenTime && ReportStyle == STYLE_OVERNIGHT )
						{
							printf ( "%s - wrong time of day for this report.<br>\n", xportfolio.xpticker );
							printf ( "StockCloseTime %ld greater than StockOpenTime %ld<br>\n", StockCloseTime, StockOpenTime );
						}
					}
				}
				else if ( ReportStyle == STYLE_TODAY )
				{
					/*----------------------------------------------------------
						Tiingo
					----------------------------------------------------------*/
					if ( nsStrstr ( buffer, "Not Found" ) != NULL )
					{
						if ( Debug )
						{
							printf ( "Unknown symbol %s<br>\n", xstock.xsticker );
							printf ( "%s<br>\n", cmdline );
						}
						StockLatestPrice = 0.0;
					}
					else
					{
/*---------------------------------------------------------------------------
[{"ticker":"IBM"
"timestamp":"2024-06-24T10:28:57.053159184-04:00"
"lastSaleTimestamp":"2024-06-24T10:28:57.053159184-04:00"
"quoteTimestamp":"2024-06-24T10:28:50.950142514-04:00"
"open":174.69
"high":178.46
"low":174.31
"mid":179.55
"tngoLast":177.97
"last":177.97
"lastSize":10
"bidSize":400
"bidPrice":174.1
"askPrice":185.0
"askSize":162
"prevClose":172.46
"volume":39782}]
---------------------------------------------------------------------------*/
						StockOpenPrice = 0.0;
						StockOpenFlag = ' ';
						StockLatestPrice = 0.0;

						ptrNameValue = JsonScan ( buffer, "open", JSON_FIRST );
						if ( ptrNameValue != NULL )
						{
							StockOpenPrice = nsAtof ( ptrNameValue->Value );
						}

						/*----------------------------------------------------------
							get latest price
							"last" gets date instead of price
						----------------------------------------------------------*/
						ptrNameValue = JsonScan ( buffer, "tngoLast", JSON_FIRST );
						if ( ptrNameValue != NULL )
						{
							StockLatestPrice = nsAtof ( ptrNameValue->Value );
						}

						/*----------------------------------------------------------
							free json buffer
						----------------------------------------------------------*/
						free ( buffer );
					}
				}
				else
				{
					printf ( "OVERNIGHT not finished/supported at Tiingo.<br>\n" );
					return ( -1 );
				}
			}

			if ( ReportStyle == STYLE_TODAY )
			{
				if ( StockLatestPrice > 0.01 )
				{
					fprintf ( fpOutput, "%s|", xportfolio.xpticker );
				}
				else
				{
					StockLatestPrice = StockYesterdayClose;
					fprintf ( fpOutput, "%s ***|", xportfolio.xpticker );
				}
				fprintf ( fpOutput, "%s|", xstock.xsname );
				fprintf ( fpOutput, "%.2f|", StockLatestPrice );
				fprintf ( fpOutput, "%.2f|", StockLatestPrice - StockYesterdayClose );
				Percent = (StockLatestPrice - StockYesterdayClose) * 100.0 / StockYesterdayClose;
				fprintf ( fpOutput, "%.2f|", Percent );		/* % change */
				fprintf ( fpOutput, "%.2f|", xportfolio.xpshares );
				fprintf ( fpOutput, "%.2f|", StockLatestPrice * xportfolio.xpshares );
				fprintf ( fpOutput, "%.2f|", (StockLatestPrice - StockYesterdayClose) * xportfolio.xpshares );
			}
			else
			{
				double	Diff;

				if ( StockOpenPrice > 0.0 && StockClosePrice > 0.0 )
				{
					Diff = StockOpenPrice - StockClosePrice;
				}
				else
				{
					Diff = 0.0;
				}

				fprintf ( fpOutput, "%s|", xportfolio.xpticker );
				fprintf ( fpOutput, "%.2f|", StockClosePrice );
				fprintf ( fpOutput, "%c|",   StockCloseFlag );
				fprintf ( fpOutput, "%.2f|", StockOpenPrice );
				fprintf ( fpOutput, "%c|",   StockOpenFlag );
				fprintf ( fpOutput, "%.2f|", Diff );
				fprintf ( fpOutput, "%.2f|", xportfolio.xpshares );
				fprintf ( fpOutput, "%.2f|", Diff * xportfolio.xpshares );
			}
			break;

		case STYLE_FULL:
			fprintf ( fpOutput, "%s|", xportfolio.xpticker );
			fprintf ( fpOutput, "%s|", xportfolio.xpdate );
			fprintf ( fpOutput, "%.2f|", xportfolio.xpshares );
			fprintf ( fpOutput, "%.2f|", StockStartPrice );
			fprintf ( fpOutput, "%.2f|", xportfolio.xpshares * StockStartPrice );
			fprintf ( fpOutput, "%.2f|", StockYesterdayClose );
			fprintf ( fpOutput, "%.2f|", xportfolio.xpshares * StockYesterdayClose );
			fprintf ( fpOutput, "%.2f|", xportfolio.xpshares * (StockYesterdayClose-StockStartPrice) );
			fprintf ( fpOutput, "%.2f|", StockPercent );
			fprintf ( fpOutput, "%.2f|", AnnualYield );
			fprintf ( fpOutput, "%.2f|", TotalYield );
			fprintf ( fpOutput, "%.2f|", StockPercent + TotalYield );

			fprintf ( fpOutput, "%.2f|", AD_Ratio );

			if (( nsStrncmp ( xportfolio.xpoptexp, "(null)", 6 ) == 0 ) ||
				( nsStrncmp ( xportfolio.xpoptexp, "0000-0", 6 ) == 0 ))
			{
				fprintf ( fpOutput, "|" );
			}
			else
			{
				fprintf ( fpOutput, "%s|", xportfolio.xpoptexp );
			}
			fprintf ( fpOutput, "%s|", xportfolio.xpbroker );
			fprintf ( fpOutput, "%s|", xstock.xsdomfor );
			break;

		case STYLE_ALERTS:
			fprintf ( fpOutput, "%s|", xportfolio.xpbroker );
			fprintf ( fpOutput, "%s|", xportfolio.xpticker );
			fprintf ( fpOutput, "%s|", xportfolio.xpdate );
			fprintf ( fpOutput, "%.2f|", xportfolio.xpshares );
			fprintf ( fpOutput, "%.2f|", StockStartPrice );
			fprintf ( fpOutput, "%.2f|", StockYesterdayClose );
			fprintf ( fpOutput, "%.2f|", StockPercent );

			fprintf ( fpOutput, "%.2f|", StockPercent + TotalYield );

			if ( xportfolio.xpalerttype[0] == ALERT_BENCHMARK )
			{
				fprintf ( fpOutput, "%s (%s)|", AlertDescription(xportfolio.xpalerttype[0]), xportfolio.xpbenchmark );
			}
			else if ( xportfolio.xpalerttype[0] == ALERT_TRAIL_STOP )
			{
				fprintf ( fpOutput, "%s (%.2f%%)|", AlertDescription (  xportfolio.xpalerttype[0] ), xportfolio.xptrailpct );
			}
			else
			{
				fprintf ( fpOutput, "%s|", AlertDescription (  xportfolio.xpalerttype[0] ) );
			}
			switch ( xportfolio.xpalerttype[0] )
			{
				case ALERT_ABOVE_AMT:
				case ALERT_BELOW_AMT:
					fprintf ( fpOutput, "%.2f|", xportfolio.xpalertprice );
					CompareTo = xportfolio.xpalertprice;
					break;

				case ALERT_TRAIL_STOP:
					fprintf ( fpOutput, "%.2f|", xportfolio.xptrailvalue );
					CompareTo = xportfolio.xptrailvalue;
					break;

				case ALERT_BENCHMARK:
					fprintf ( fpOutput, "%.2f| |%.2f|", BenchmarkPercent, ComparedToBenchmark );
					CompareTo = 0.0;
					break;

				case ALERT_ABOVE_MA5:
				case ALERT_BELOW_MA5:
					fprintf ( fpOutput, "%.2f|", xaverage.xama5 );
					CompareTo = xaverage.xama5;
					break;

				case ALERT_ABOVE_MA10:
				case ALERT_BELOW_MA10:
					fprintf ( fpOutput, "%.2f|", xaverage.xama10 );
					CompareTo = xaverage.xama10;
					break;

				case ALERT_ABOVE_MA20:
				case ALERT_BELOW_MA20:
					fprintf ( fpOutput, "%.2f|", xaverage.xama20 );
					CompareTo = xaverage.xama20;
					break;

				case ALERT_ABOVE_MA50:
				case ALERT_BELOW_MA50:
					fprintf ( fpOutput, "%.2f|", xaverage.xama50 );
					CompareTo = xaverage.xama50;
					break;

				case ALERT_BELOW_MA100:
				case ALERT_ABOVE_MA100:
					fprintf ( fpOutput, "%.2f|", xaverage.xama100 );
					CompareTo = xaverage.xama100;
					break;

				case ALERT_ABOVE_MA200:
				case ALERT_BELOW_MA200:
					fprintf ( fpOutput, "%.2f|", xaverage.xama200 );
					CompareTo = xaverage.xama200;
					break;

				case ALERT_NONE:
				default:
					fprintf ( fpOutput, " |" );
					CompareTo = 0.0;
					break;
			}

			if ( CompareTo != 0.0 )
			{
				Difference = StockYesterdayClose - CompareTo;
				Percent = 100.0 * Difference / CompareTo;
				fprintf ( fpOutput, "%.2f|%.2f|", Difference, Percent );
			}
			else
			{
				fprintf ( fpOutput, " | |" );
			}
			break;

		case STYLE_YOY:
			fprintf ( fpOutput, "%c|", xstock.xstype[0] );
			fprintf ( fpOutput, "%s|", xportfolio.xpticker );
			fprintf ( fpOutput, "%s|", xstock.xsname );
			fprintf ( fpOutput, "%s|", xportfolio.xpdate );
			fprintf ( fpOutput, "%d|", OwnedDays );
			fprintf ( fpOutput, "%.2f|", StockStartPrice      * xportfolio.xpshares);
			fprintf ( fpOutput, "%.2f|", StockYesterdayClose  * xportfolio.xpshares);

			double Gain = (StockYesterdayClose - StockStartPrice) * xportfolio.xpshares;
			double ROI  = 100.0 * Gain / (StockStartPrice * xportfolio.xpshares);
			fprintf ( fpOutput, "%.2f|%.2f|", Gain, ROI );

			break;

		case STYLE_CUSTOM:
			break;
	}

	if ( ReportStyle != STYLE_MSTAR )
	{
		fprintf ( fpOutput, "\n" );
	}

	return ( 0 );
}
