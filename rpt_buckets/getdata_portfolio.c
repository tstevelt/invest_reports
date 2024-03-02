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

#include	"rpt_buckets.h"

double  xstock_ttm_dividends;

int getdata_portfolio ()
{
	double	StockStartPrice, StockCurrentPrice;
	DATEVAL	dvPdate;
	int		rv;
	RECORD	Key, *Ptr;
	int		NeedSort;

	StockStartPrice = xportfolio.xpprice;

	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 1 )) != 1 )
	{
		if ( Debug )
		{
			printf ( "LoadStock %s returned %d\n", xportfolio.xpticker, rv );
		}
		return ( 0 );
	}

	switch ( xstock.xstype[0] )
	{
		case STYPE_BOND:
			xhistory.xhclose = 1000.0;
			break;
		case STYPE_CRYPTO:
			xhistory.xhclose = xportfolio.xpprice;
			break;
		default:
			if ( xstock.xslast == NULL || nsStrncmp ( xstock.xslast, "(null)", 6 ) == 0 )
			{
				if ( Debug )
				{
					printf ( "Stock %s does not have any history\n", xportfolio.xpticker );
				}
				return ( 0 );
			}
			/*----------------------------------------------------------
				get stock current price
			----------------------------------------------------------*/
			sprintf ( WhereClause, "Hticker = '%s' and Hdate = '%s'", xportfolio.xpticker, xstock.xslast );
			if (( rv = LoadHistory ( &MySql, WhereClause, &xhistory, 0 )) != 1 )
			{
				printf ( "LoadHistory returned %d, cannot find stock price for %s %s\n", rv, xportfolio.xpticker, xstock.xslast );
				return ( 0 );
			}
			break;
	}

	StockCurrentPrice = xhistory.xhclose;

//if ( strcmp ( xportfolio.xpticker, "HMC" ) == 0 )
//{
//	printf ( "Kilroy was here<br>\n" );
//}

	/*----------------------------------------------------------
		load fundamentals
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Fticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 )) != 1 )
	{
		memset ( &xfundamental, '\0', sizeof(xfundamental) );
	}

	/*----------------------------------------------------------
		find (or add) element in array
	----------------------------------------------------------*/
	Key.Bucket = xportfolio.xpbucket;
	snprintf ( Key.Broker, sizeof(Key.Broker), "%s", xportfolio.xpbroker );
	snprintf ( Key.Ticker, sizeof(Key.Ticker), "%s", xportfolio.xpticker );

	NeedSort = 1;
	if ( Count == 0 )
	{
		Ptr = NULL;
	}
	else
	{
		Ptr = bsearch ( &Key, Array, Count, sizeof(RECORD), (int(*)()) cmprec );
	}

	if ( Ptr == NULL )
	{
		if ( Count >= MAXRECS )
		{
			printf ( "Exceeds MAXRECS\n" );
			exit ( 1 );
		}

		Array[Count].Bucket = xportfolio.xpbucket;
		snprintf ( Array[Count].Broker, sizeof(Key.Broker), "%s", xportfolio.xpbroker );
		snprintf ( Array[Count].Ticker, sizeof(Key.Ticker), "%s", xportfolio.xpticker );
		snprintf ( Array[Count].Name,   sizeof(Key.Name),   "%s", xstock.xsname );

		if ( /* xfundamental.xfyield == 0.0 && */ xstock_ttm_dividends > 0.0 )
		{
			Array[Count].Yield = 100.0 * xstock_ttm_dividends / xstock.xsclose;
		}
		else
		{
			Array[Count].Yield = xfundamental.xfyield;
// printf ( "Using Fundamental Yield %.2f for %s<br>\n", xfundamental.xfyield, xportfolio.xpticker );
		}
		Array[Count].Type   = xstock.xstype[0];
		Array[Count].DomesticForeign  = xstock.xsdomfor[0];
		StrToDatevalFmt ( xportfolio.xpdate, DATEFMT_YYYY_MM_DD, &dvPdate );
		Array[Count].Days   = DateDiff ( &dvPdate, &dvToday );
		Array[Count].Invest = 0.0;
		Array[Count].Market = 0.0;
		Array[Count].Shares = 0.0;
		Ptr = &Array[Count];
		Count++;
		NeedSort = 1;
	}

	Ptr->Invest += (xportfolio.xpshares * StockStartPrice);
	Ptr->Market += (xportfolio.xpshares * StockCurrentPrice);
	Ptr->Shares +=  xportfolio.xpshares;

	if ( NeedSort )
	{
		qsort ( Array, Count, sizeof(RECORD), (int(*)()) cmprec );
	}

	return ( 0 );
}
