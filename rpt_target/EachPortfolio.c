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

int EachPortfolio ()
{
	int		rv;
	char	AllocType;
	int		ndx;
	BRKR_RECORD		*ptr, Key;
	int		NeedSort;

	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 1 )) != 1 )
	{
		if ( Debug )
		{
			printf ( "LoadStock %s returned %d\n", xportfolio.xpticker, rv );
		}
		return ( 0 );
	}

	if (( AllocType = MapStype() ) == 0 )
	{
		printf ( "Cannot determine allocation type for %s\n", xstock.xsticker );
		exit ( 1 );
	}

	/*---------------------------------------------------------------------------
		load SummaryArray.  Elements already exist.
	---------------------------------------------------------------------------*/
	for ( ndx = 0; ndx < SummaryCount; ndx++ )
	{
		if ( AllocType == SummaryArray[ndx].AllocationType )
		{
			SummaryArray[ndx].CurrentInvest += xportfolio.xpshares * xportfolio.xpprice;
			switch ( AllocType )
			{
				case CASH:
				case BOND_AND_CD:
					SummaryArray[ndx].CurrentMarket += xportfolio.xpshares * xportfolio.xpprice;
					break;
				default:
					SummaryArray[ndx].CurrentMarket += xportfolio.xpshares * xstock.xsclose;
					break;
			}

			if ( Debug )
			{
				printf ( "%c %10.10s %-30.30s\n", AllocType, xstock.xsticker, xstock.xsname );
			}

			break;
		}
	}
	if ( ndx == SummaryCount )
	{
		printf ( "No target specified for allocation type %d for %s\n", AllocType, xstock.xsticker );
		exit ( 1 );
	}

	/*---------------------------------------------------------------------------
		Load Broker Array.  Add elements as needed.
	---------------------------------------------------------------------------*/
	NeedSort = 0;
	sprintf ( Key.Broker, "%s", xportfolio.xpbroker );
	Key.AllocationType = AllocType;
	if ( BrokerCount > 0 )
	{
		ptr = bsearch ( &Key, BrokerArray, BrokerCount, sizeof(BRKR_RECORD), (int(*)()) cmprec );
	}
	else
	{
		ptr = NULL;
	}

	if ( ptr == NULL )
	{
		sprintf ( BrokerArray[BrokerCount].Broker, "%s", xportfolio.xpbroker );
		BrokerArray[BrokerCount].AllocationType = AllocType;
		ptr = &BrokerArray[BrokerCount];
		BrokerCount++;
		NeedSort = 1;
	}

	switch ( AllocType )
	{
		case CASH:
		case BOND_AND_CD:
			ptr->Current += xportfolio.xpshares * xportfolio.xpprice;
			break;
		default:
			ptr->Current += xportfolio.xpshares * xstock.xsclose;
			break;
	}
		
	if ( NeedSort )
	{
		qsort ( BrokerArray, BrokerCount, sizeof(BRKR_RECORD), (int(*)()) cmprec );
	}

	return ( 0 );
}
