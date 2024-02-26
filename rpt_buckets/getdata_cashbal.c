/*----------+------------------+------+-----+---------+----------------+
| Field     | Type             | Null | Key | Default | Extra          |
+-----------+------------------+------+-----+---------+----------------+
| id        | int(11) unsigned | NO   | PRI | NULL    | auto_increment |
| CBmember  | int(11) unsigned | NO   |     | NULL    |                |
| CBacct    | varchar(30)      | NO   |     |         |                |
| CBdate    | date             | NO   |     | NULL    |                |
| CBamount  | double           | NO   |     | NULL    |                |
| CBcomment | varchar(60)      | YES  |     | NULL    |                |
+-----------+------------------+------+-----+---------+---------------*/
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

#include	"rpt_buckets.h"

int getdata_cashbal ()
{
	RECORD	Key, *Ptr;
	int		NeedSort;

	/*----------------------------------------------------------
		find (or add) element in array
	----------------------------------------------------------*/
	Key.Bucket = 1;
	// fixit
	snprintf ( Key.Broker, sizeof(Key.Broker), xcashbal.xcbacct );
	snprintf ( Key.Ticker, sizeof(Key.Ticker), "Cash" );

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

		Array[Count].Bucket = 1;
		// fixit
		snprintf ( Array[Count].Ticker, sizeof(Key.Broker), "Cash" );
		snprintf ( Array[Count].Broker, sizeof(Key.Ticker), xcashbal.xcbacct );
		snprintf ( Array[Count].Name,   sizeof(Key.Name),   xcashbal.xcbcomment );
		Array[Count].Type   = 'C';
		Array[Count].DomesticForeign = 'D';
		Array[Count].Days   = 0;
		Array[Count].Invest = 0.0;
		Array[Count].Market = 0.0;
		Array[Count].Shares = 0.0;
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
