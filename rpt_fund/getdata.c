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

#include	"rpt_fund.h"

static int EachRow ( DBY_QUERY *Query )
{
	ReportCount++;
	if ( ReportCount == 1 )
	{
		sprintf ( OutFileName, "%s/rpt_fund_%d.dat", TEMPDIR, getpid() );
		if (( fpOutput = fopen ( OutFileName, "w" )) == (FILE *) 0 )
		{
			fprintf ( stderr, "Cannot create %s for output\n", OutFileName );
			exit ( 1 );
		}
	}

// printf ( "Number Fields %d<br>\n",  Query->NumFields );

	for ( int ndx = 0; ndx < Query->NumFields; ndx++ )
	{
		if ( MungeData && ndx == 1 )
		{
			fprintf ( fpOutput, "%s%s", ndx ? "|" : "",  strMunge(Query->EachRow[ndx]) );
		}
		else
		{
			fprintf ( fpOutput, "%s%s", ndx ? "|" : "",  Query->EachRow[ndx] );
		}
	}
	fprintf ( fpOutput, "\n" );

	return ( 0 );
}

int getdata ()
{
	FILE	*fp;
	int		ndx;

	ReportCount = StockCount = 0;

	if (( fp = fopen ( InputFileName, "r" )) == 0 )
	{
		printf ( "Cannot open %s\n", InputFileName );
		return ( 0 );
	}
	
	fread ( Statement, sizeof(Statement), 1, fp );
	nsStrcpy ( Statement2, Statement );

printf ( "%s\n", Statement );

	nsFclose ( fp );

	if ( Debug )
	{
		printf ( "Not removing %s\n", InputFileName );
	}
	else
	{
		unlink ( InputFileName );
	}

	StockCount = dbySelectCB ( "rpt_fund", &MySql, Statement, EachRow, LogFileName );

	nsFclose ( fpOutput );

	/*----------------------------------------------------------
		construct column array
select Fticker, Sname,Fvalue,Fyield from stock, fundamental where Sticker = Fticker and ( Sdj = 'Y' or Ssp500 = 'Y') and Fvalue > 3.000000 and Fvalue < 8.000000 and Fyield > 3.000000 and Fyield < 25.000000;

select Fticker, Sname,Fmstar from stock, fundamental where Sticker = Fticker and Stype = 'E' and Fmstar >= 3.000000 and Fmstar <= 5.000000; 
	----------------------------------------------------------*/

	Tokcnt = GetTokensD ( Statement, " ,", Tokens, MAXTOKS );

	ColumnArray[0].Row1 = FundamentalHuman(Tokens[1]);
	ColumnArray[0].Row2 = NULL;
	ColumnArray[0].Options = INIT_STRING_LEFT;
	ColumnArray[1].Row1 = FundamentalHuman(Tokens[2]);
	ColumnArray[1].Row2 = NULL;
	ColumnArray[1].Options = INIT_STRING_LEFT;
//	ColumnArray[2].Row1 = FundamentalHuman(Tokens[3]);
//	ColumnArray[2].Row2 = NULL;
//	ColumnArray[2].Options = INIT_STRING_LEFT;
	ColumnCount = 2;

	for ( ndx = 3; ndx < Tokcnt; ndx++ )
	{
		if ( nsStrcmp ( Tokens[ndx], "from" ) == 0 )
		{
			break;
		}
		ColumnArray[ColumnCount].Row1 = FundamentalHuman(Tokens[ndx]);
		ColumnArray[ColumnCount].Row2 = NULL;
		ColumnArray[ColumnCount].Options = INIT_STRING_RIGHT;
		ColumnCount++;
	}

	return ( StockCount );
}
