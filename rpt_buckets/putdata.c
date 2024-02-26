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

int putdata ()
{
	double	TotalInvest, TotalMarket;
	double	PercentInvest, PercentMarket, PercentGain;
	int		ndx;

	ReportCount = 0;

	sprintf ( OutFileName, "%s/buckets_%d.dat", TEMPDIR, getpid() );
	if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
	{
		printf ( "Cannot create %s for output\n", OutFileName );
		exit ( 1 );
	}

	TotalInvest = TotalMarket = 0.0;
	for ( ndx = 0; ndx < Count; ndx++ )
	{
		TotalInvest += Array[ndx].Invest;
		TotalMarket += Array[ndx].Market;
	}

	for ( ndx = 0; ndx < Count; ndx++ )
	{
		PercentInvest = 100.0 * Array[ndx].Invest / TotalInvest;
		PercentMarket = 100.0 * Array[ndx].Market / TotalMarket;
		PercentGain   = 100.0 * (Array[ndx].Market - Array[ndx].Invest) / Array[ndx].Invest;
		fprintf ( fpOutput, "%d|%-10.10s|%-12.12s|%-30.30s|%c|%c|%d|%12.2f|%12.2f|%6.2f|%12.2f|%6.2f|%12.2f|%6.2f|%6.2f\n",
			Array[ndx].Bucket, 
			Array[ndx].Broker, 
			Array[ndx].Ticker, 
			Array[ndx].Name, 
			Array[ndx].Type, 
			Array[ndx].DomesticForeign, 
			Array[ndx].Days, 
			Array[ndx].Shares, 
			Array[ndx].Invest, PercentInvest, 
			Array[ndx].Market, PercentMarket, 
			Array[ndx].Market - Array[ndx].Invest,
			PercentGain,
			Array[ndx].Yield );

		ReportCount++;
	}

	nsFclose ( fpOutput );

	return ( 0 );
}
