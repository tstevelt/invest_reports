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

int putdata ()
{
	double	TotalInvest, TotalMarket;
	double	PercentInvest, PercentMarket;
	int		ndx;

	ReportCount = 0;

	sprintf ( OutFileName, "%s/allocation_%d.dat", TEMPDIR, getpid() );
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

		switch ( RunMode )
		{
			case MODE_SECTOR:
				fprintf ( fpOutput, "%-30.30s|%12.2f|%6.2f|%12.2f|%6.2f\n",
					Array[ndx].Sector, Array[ndx].Invest, PercentInvest, Array[ndx].Market, PercentMarket );
				break;
			case MODE_INDUSTRY:
				fprintf ( fpOutput, "%-40.40s|%12.2f|%6.2f|%12.2f|%6.2f\n",
					Array[ndx].Industry, Array[ndx].Invest, PercentInvest, Array[ndx].Market, PercentMarket );
				break;
			case MODE_BOTH:
				fprintf ( fpOutput, "%-30.30s|%-40.40s|%12.2f|%6.2f|%12.2f|%6.2f\n",
					Array[ndx].Sector, Array[ndx].Industry, Array[ndx].Invest, PercentInvest, Array[ndx].Market, PercentMarket );
				break;
		}

		ReportCount++;
	}

	nsFclose ( fpOutput );

	return ( 0 );
}
