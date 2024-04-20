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

#include	"rpt_porthist.h"

int getdata ()
{
	static	double	TwoYearHigh = 0.0;

	double	TotalValue = xporthist.xphcash + xporthist.xphbonds + xporthist.xphequity;
	double	ROI = 100.0 * (TotalValue-xporthist.xphinvest) / xporthist.xphinvest;

	ReportCount++;
	if ( ReportCount == 1 )
	{
		sprintf ( OutFileName, "%s/porthist_%d.dat", TEMPDIR, getpid() );
		if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
		{
			fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
			exit ( 1 );
		}

		TwoYearHigh = TotalValue;
	}

	if ( TwoYearHigh < TotalValue )
	{
		TwoYearHigh = TotalValue;
	}

	double	DrawDown = 100.0 * (TotalValue - TwoYearHigh) / TwoYearHigh;

	fprintf ( fpOutput, "%s|",   xporthist.xphdate );
	fprintf ( fpOutput, "%.2f|", xporthist.xphinvest );
	fprintf ( fpOutput, "%.2f|", xporthist.xphcash );
	fprintf ( fpOutput, "%.2f|", xporthist.xphbonds );
	fprintf ( fpOutput, "%.2f|", xporthist.xphequity );
	fprintf ( fpOutput, "%.2f|", TotalValue );
	fprintf ( fpOutput, "%.2f|", ROI );
	fprintf ( fpOutput, "%.2f|", 100.0 * xporthist.xphcash / TotalValue );
	fprintf ( fpOutput, "%.2f|", 100.0 * xporthist.xphbonds / TotalValue );
	fprintf ( fpOutput, "%.2f|", 100.0 * xporthist.xphequity / TotalValue );
	fprintf ( fpOutput, "%.2f|", TwoYearHigh );
	fprintf ( fpOutput, "%.2f|", DrawDown );
	fprintf ( fpOutput, "\n" );

	return ( 0 );
}
