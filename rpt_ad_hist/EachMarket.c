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

#include	"rpt_ad_hist.h"

int EachMarket ()
{
	double	Ratio;

	// printf ( "%s %c %d %d\n", xmarket.xmktdate, xmarket.xmktindex[0], xmarket.xmktaccum, xmarket.xmktdist );

	Ratio = 100.0 * (double) xmarket.xmktaccum / (double)(xmarket.xmktaccum + xmarket.xmktdist);
	switch ( xmarket.xmktindex[0] )
	{
		case 'N':
			fprintf ( fpOutput, "%s|%d|%d|%.2f|", 
				xmarket.xmktdate, xmarket.xmktaccum, xmarket.xmktdist, Ratio );
			break;
		case 'S':
			fprintf ( fpOutput, "%d|%d|%.2f\n", 
				xmarket.xmktaccum, xmarket.xmktdist, Ratio );
			break;
		default:
			break;
	}


	return ( 0 );
}
