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

#include	"rpt_watchlist.h"


int EachFundamental ()
{
	int		rv, HaveFundamentals;

	sprintf ( WhereClause, "Sticker = '%s'", xwatchlist.xwticker );
	if (( rv = LoadStock ( &MySql, WhereClause, &xstock, 0, 0 )) != 1 )
	{
		fprintf ( stderr, "LoadStock %s returned %d\n", xwatchlist.xwticker, rv );
		return ( 0 );
	}

	/*----------------------------------------------------------
		load fundamentals
	----------------------------------------------------------*/
	sprintf ( WhereClause, "Fticker = '%s'", xwatchlist.xwticker );
	if (( rv = LoadFundamental ( &MySql, WhereClause, &xfundamental, 0 )) != 1 )
	{
		memset ( &xfundamental, '\0', sizeof(xfundamental) );
		HaveFundamentals = 0;
	}
	else
	{
		HaveFundamentals = 1;
	}

	ReportCount++;
	if ( ReportCount == 1 )
	{
		sprintf ( OutFileName, "%s/watchlist_%d.dat", TEMPDIR, getpid() );
		if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
		{
			fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
			exit ( 1 );
		}
	}

	fprintf ( fpOutput, "%s|", xwatchlist.xwticker );
	fprintf ( fpOutput, "%s|", xstock.xsname );
	fprintf ( fpOutput, "%c|", xstock.xstype[0] );
	if ( HaveFundamentals == 0 )
	{
		fprintf ( fpOutput, "NONE|" );
		fprintf ( fpOutput, "|" );
		fprintf ( fpOutput, "|" );
		fprintf ( fpOutput, "|" );
		fprintf ( fpOutput, "|" );
		fprintf ( fpOutput, "|" );
		fprintf ( fpOutput, "|" );
		fprintf ( fpOutput, "|" );
		fprintf ( fpOutput, "|" );
	}
	else
	{
		fprintf ( fpOutput, "%.2f|", xfundamental.xfpettm );
		fprintf ( fpOutput, "%.2f|", xfundamental.xfyield );
		fprintf ( fpOutput, "%.2f|", xfundamental.xfgrow5 );
		fprintf ( fpOutput, "%.2f|", xfundamental.xfbeta );
		fprintf ( fpOutput, "%.2f|", xfundamental.xfquick );
		fprintf ( fpOutput, "%.2f|", xfundamental.xfdebteq );
		fprintf ( fpOutput, "%.2f|", xfundamental.xfvalue );
		fprintf ( fpOutput, "%.2f|", xstock.xsclose );
		fprintf ( fpOutput, "%.2f|", xfundamental.xftarget );
	}

	fprintf ( fpOutput, "\n" );

	return ( 0 );
}
