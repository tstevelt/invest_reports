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

#include	"rpt_regress.h"

static double BuyPrice;
static double SellPrice;

int BreakOut ()
{
	return ( -1 );
}

int LoadArray ()
{
	if ( StockCount >= MAXSTOCKS )
	{
		printf ( "Exceeds MAXSTOCKS\n" );
		exit ( 1 );
	}

	if ( Debug )
	{
		printf ( "LoadArray: %-10.10s %-30.30s\n", xstock.xsticker, xstock.xsname );
	}

	snprintf ( StockArray[StockCount].xsticker, sizeof(StockArray[StockCount].xsticker), "%s", xstock.xsticker );
	snprintf ( StockArray[StockCount].xsname,   sizeof(StockArray[StockCount].xsname),   "%s", xstock.xsname   );

	BuyPrice = -1.0;
	SellPrice = -1.0;

	sprintf ( WhereClause, "Hticker = '%s' and Hdate >= '%s' and Hdate <= '%s'", xstock.xsticker, StartDate, EndDate );
	if ( LoadHistoryCB ( &MySql, WhereClause, NULL, &xhistory, (int(*)()) BreakOut, 0 ) < 1 )
	{
		return ( 0 );
	}

//	StockArray[StockCount].BuyPrice = BuyPrice;
//	StockArray[StockCount].SellPrice = SellPrice;
	StockCount++;

	return ( 0 );
}

int LoadArrayW ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xwatchlist.xwticker );
	if ( Debug )
	{
		printf ( "LoadArrayW: %s\n", WhereClause );
	}
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		return ( 0 );
	}
	return (LoadArray () );
}

int LoadArrayP ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if ( Debug )
	{
		printf ( "LoadArrayP: %s\n", WhereClause );
	}
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		return ( 0 );
	}
	return (LoadArray () );
}

int LoadArrayT ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xtemp.xtticker );
	if ( Debug )
	{
		printf ( "LoadArrayT: %s\n", WhereClause );
	}
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		return ( 0 );
	}
	return (LoadArray () );
}
