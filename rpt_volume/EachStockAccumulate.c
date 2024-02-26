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

#include	"rpt_volume.h"

// #define		DAYS		30
// #define		TRIGGER_PCT	25.0
// #define		MINVOL		250000

int EachStockAccumulate ()
{
	double	AvgVolumeUp, AvgVolumeDown, AD_Ratio;
	double	Percent;

	if ( CalcAccumDistrVolume ( &MySql, xstock.xsticker, xstock.xslast, NumberOfDays, 
							&AD_Ratio, &AvgVolumeUp, &AvgVolumeDown ) != 0  )
	{
		return ( 0 );
	}

	Percent = 100.0 * (AvgVolumeUp) / AvgVolumeDown;

	if ( AvgVolumeUp > MinimumVolume && (Debug == 1 || Percent > TopPercent) )
	{
		ReportCount++;
		if ( ReportCount == 1 )
		{
			sprintf ( OutFileName, "%s/volume_%d.dat", TEMPDIR, getpid() );
			if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
			{
				fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
				exit ( 1 );
			}
		}

		fprintf ( fpOutput, "%s|%s|%.0f|%.0f|%.2f|%.2f\n",
			xstock.xsticker,
			xstock.xsname,
			AvgVolumeUp,
			AvgVolumeDown,
			AD_Ratio,
			Percent );
	}

	return ( 0 );
}
