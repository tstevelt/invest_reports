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

int getdata_cashbal ()
{
	RECORD	Key, *Ptr;
	int		NeedSort;

	/*----------------------------------------------------------
		find (or add) element in array
	----------------------------------------------------------*/
	snprintf ( Key.Sector, sizeof(Key.Sector), "~Other Cash" );
	snprintf ( Key.Industry, sizeof(Key.Industry), "~Other Cash" );

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

	Ptr->Invest += xcashbal.xcbamount;
	Ptr->Market += xcashbal.xcbamount;

	if ( NeedSort )
	{
		qsort ( Array, Count, sizeof(RECORD), (int(*)()) cmprec );
	}

	return ( 0 );
}
