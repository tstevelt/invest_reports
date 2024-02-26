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

#include	"rpt_target.h"

int EachStock ()
{
	int		rv;

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
		case STYPE_ETF:
		case STYPE_PREFER:
		case STYPE_STOCK:
		case STYPE_ADR:
		case STYPE_REIT:
			if ( MatchDomFor == xstock.xsdomfor[0] )
			{
				fprintf (  fpOutput, "%-10.10s %-10.10s %-30.30s %12.2f\n", 
					xportfolio.xpbroker, xstock.xsticker, xstock.xsname, xportfolio.xpshares * xstock.xsclose);
			}
			break;

		default:
			return ( 0 );
	}

	return ( 0 );
}
