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

int gettotal ()
{
	int		rv;

	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 1 )) != 1 )
	{
		fprintf ( stderr, "LoadStock %s returned %d\n", xportfolio.xpticker, rv );
		return ( 0 );
	}

	switch ( xstock.xstype[0] )
	{
		case STYPE_BOND:
			xhistory.xhclose = 1000.0;
			break;
		case STYPE_CRYPTO:
			xhistory.xhclose = 1.0;
			break;
		default:
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
			break;
	}

	TotalInvest += xportfolio.xpshares * xportfolio.xpprice;
	TotalValue  += xportfolio.xpshares * xhistory.xhclose;
	
	return ( 0 );
}
