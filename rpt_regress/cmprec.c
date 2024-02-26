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

int cmprec ( STOCK_RECORD *a, STOCK_RECORD *b )
{
	switch ( SortBy )
	{
		case SORTBY_RETURN:
			if ( a->Return < b->Return ) 
			{
				return ( 1 );
			}
			if ( a->Return > b->Return ) 
			{
				return ( -1 );
			}
			break;

		case SORTBY_CORREL:
			if ( a->CorrCoeff < b->CorrCoeff ) 
			{
				return ( 1 );
			}
			if ( a->CorrCoeff > b->CorrCoeff ) 
			{
				return ( -1 );
			}
			break;

		case SORTBY_VARIANCE:
			if ( a->Variance < b->Variance ) 
			{
				return ( -1 );
			}
			if ( a->Variance > b->Variance ) 
			{
				return ( 1 );
			}
			break;
	}

	if ( a->StdDev < b->StdDev ) 
	{
		return ( 1 );
	}
	if ( a->StdDev > b->StdDev ) 
	{
		return ( -1 );
	}
	return ( 0 );
}
