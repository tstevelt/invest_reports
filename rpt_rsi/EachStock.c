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

#include	"rpt_rsi.h"

int EachStock ()
{
	sprintf	( WhereClause, "Aticker = '%s' and Adate = '%s'", xstock.xsticker, xstock.xslast );
	LoadAverage ( &MySql, WhereClause, &xaverage, 0 );

	if ( SelectStyle == SELECT_MINIMUM && xaverage.xarsi < MinimumRS )
	{
		return ( 0 );
	}
	else if ( SelectStyle == SELECT_MAXIMUM && xaverage.xarsi > MaximumRS )
	{
		return ( 0 );
	}

	if ( xaverage.xarsi > 0.0 )
	{
		switch ( GroupMode )
		{
			case GROUP_NONE:
			case GROUP_SECTOR:
				sprintf	( WhereClause, "id = '%ld'", xstock.xssector );
				if ( LoadSector ( &MySql, WhereClause, &xsector, 0 ) != 1 )
				{
					fprintf ( fpOutput, "Unknown|" );
				}
				else
				{
					fprintf ( fpOutput, "%.60s|", xsector.xsecname );
				}
				break;

			case GROUP_INDUSTRY:
				sprintf	( WhereClause, "industry.id = %ld", xstock.xsindustry );
				if ( LoadIndustry ( &MySql, WhereClause, &xindustry, 0 ) != 1 )
				{
					fprintf ( fpOutput, "Unknown|" );
				}
				else
				{
					fprintf ( fpOutput, "%.60s|", xindustry.xindname );
				}
				break;

			default:
				fprintf ( fpOutput, "WTF|" );
				break;
		}

		fprintf ( fpOutput, "%s|%s|", xstock.xsticker, xstock.xsname );

		fprintf ( fpOutput, "%.2f", xaverage.xarsi );

		fprintf ( fpOutput, "\n" );
	}

	return ( 0 );
}
