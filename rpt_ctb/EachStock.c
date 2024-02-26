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

#include	"rpt_ctb.h"

int EachStock ()
{
	sprintf	( WhereClause, "Aticker = '%s' and Adate = '%s'", xstock.xsticker, xstock.xslast );
	LoadAverage ( &MySql, WhereClause, &xaverage, 0 );

	if ( SelectStyle == SELECT_MINIMUM && xaverage.xactb < MinimumRS )
	{
		return ( 0 );
	}
	else if ( SelectStyle == SELECT_MAXIMUM && xaverage.xactb > MaximumRS )
	{
		return ( 0 );
	}

	if ( xaverage.xactb > 0.0 )
	{
		fprintf ( fpOutput, "%s|%s|", xstock.xsticker, xstock.xsname );

		fprintf ( fpOutput, "%.2f", xaverage.xactb );

		fprintf ( fpOutput, "\n" );
	}

	return ( 0 );
}
