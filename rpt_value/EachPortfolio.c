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

#include	"rpt_value.h"

int EachPortfolio ( XPORTFOLIO *ptr )
{
	char	StockWhereClause[80];

	sprintf ( StockWhereClause, "Sticker = '%s'", ptr->xpticker );
	if ( LoadStock ( &MySql, StockWhereClause, &xstock, 0, 0 ) == 0 )
	{
		return ( 0 );
	}

	switch ( xstock.xstype[0] )
	{
		case 'S':
		case 'A':
			EachStock();
			break;
		default:
			break;
	}

	return ( 0 );
}
