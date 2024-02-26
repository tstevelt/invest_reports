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

int MapStype ()
{
	switch ( xstock.xstype[0] )
	{
		case STYPE_CRYPTO:
			return ( CASH );

		case STYPE_BOND:
			return ( BOND_AND_CD );

		case STYPE_ETF:
		case STYPE_PREFER:
		case STYPE_STOCK:
		case STYPE_ADR:
		case STYPE_REIT:
			if ( xstock.xsdomfor[0] == 'D' )
			{
				return ( DOMESTIC_EQUITY );
			}
			else
			{
				return ( FOREIGN_EQUITY );
			}

		case STYPE_INDEX:
		case STYPE_OTHER:
			printf ( "Unknown stock type %c in portfolio, %s\n", xstock.xstype[0], xstock.xsticker );
			exit ( 1 );
	}
	return ( 0 );
}
