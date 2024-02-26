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

int cmprec ( RECORD *a, RECORD *b )
{
	int		rv;

	if ( a->Bucket < b->Bucket )
	{
		return ( -1 );
	}
	if ( a->Bucket > b->Bucket )
	{
		return ( 1 );
	}

	rv = nsStrcmp ( a->Broker, b->Broker );
	if ( rv < 0 )
	{
		return ( -1 );
	}
	if ( rv > 0 )
	{
		return ( 1 );
	}

	rv = nsStrcmp ( a->Ticker, b->Ticker );
	if ( rv < 0 )
	{
		return ( -1 );
	}
	if ( rv > 0 )
	{
		return ( 1 );
	}

	return ( 0 );
}
