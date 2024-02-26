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

#include	"rpt_allocation.h"

int getdata_portfolio ()
{
	double	StockStartPrice, StockCurrentPrice;
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
		case 'C':
			StockCurrentPrice = StockStartPrice;
			snprintf ( Key.Sector, sizeof(Key.Sector), "~Cash" );
			snprintf ( Key.Industry, sizeof(Key.Industry), "~Cash" );
			break;
		case 'B':
			StockCurrentPrice = 1000.0;
			snprintf ( Key.Sector, sizeof(Key.Sector), "~Bonds" );
			snprintf ( Key.Industry, sizeof(Key.Industry), "~Bonds" );
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
			StockCurrentPrice = xhistory.xhclose;

			if (  xstock.xstype[0] == 'E' )
			{
				snprintf ( Key.Sector, sizeof(Key.Sector), "~Funds" );
				snprintf ( Key.Industry, sizeof(Key.Industry), "~Funds" );
			}
			else
			{
				/*----------------------------------------------------------
					get sector and industry
				----------------------------------------------------------*/
				sprintf ( WhereClause, "industry.id = %ld", xstock.xsindustry );
				if ( LoadIndustry ( &MySql, WhereClause, &xindustry, 0 ) != 1 )
				{
					xindustry.xid = 0;
					sprintf ( xindustry.xindname, "Unknown" );
				}

				sprintf ( WhereClause, "sector.id = '%ld'", xstock.xssector );
				if ( LoadSector ( &MySql, WhereClause, &xsector, 0 ) != 1 )
				{
					xsector.xid = 0;
					sprintf ( xsector.xsecname, "Unknown" );
				}

				/*----------------------------------------------------------
					find (or add) element in array
				----------------------------------------------------------*/
				snprintf ( Key.Sector, sizeof(Key.Sector), "%s", xsector.xsecname );
				snprintf ( Key.Industry, sizeof(Key.Industry), "%s", xindustry.xindname );
			}

			break;
	}

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

		snprintf ( Array[Count].Sector, sizeof(Key.Sector), "%s", Key.Sector );
		snprintf ( Array[Count].Industry, sizeof(Key.Industry), "%s", Key.Industry );
		Array[Count].Invest = 0.0;
		Array[Count].Market = 0.0;
		Ptr = &Array[Count];
		Count++;
		NeedSort = 1;
	}

	Ptr->Invest += (xportfolio.xpshares * StockStartPrice);
	Ptr->Market += (xportfolio.xpshares * StockCurrentPrice);

	if ( NeedSort )
	{
		qsort ( Array, Count, sizeof(RECORD), (int(*)()) cmprec );
	}

	return ( 0 );
}
