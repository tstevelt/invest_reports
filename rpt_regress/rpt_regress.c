/*----------------------------------------------------------------------------
	Program : rpt_regress.c
	Author  : Tom Stevelt
	Date    : 01/21/2019
	Synopsis: Find best fit straight lines, output slope and std dev.
				Available on the website screeners page.

	Who		Date		Modification
	---------------------------------------------------------------------
	tms		02/11/2019	Added Watchlist and Portfolio options.
	tms		05/22/2019	Added Temp source and destination

----------------------------------------------------------------------------*/
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


#define		MAIN
#include	"rpt_regress.h"

int main ( int argc, char *argv[] )
{
	int		ndx, rv;
	double	Percent;
	FILE	*fp;
	char	xbuffer[80];
	char	*tokens[2];
	char	*AttachArray[1];
	char	BodyFilename[128];
	FILE	*bfp;

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

	if ( ReportMember > 0L )
	{
		sprintf ( WhereClause, "member.id = %ld", ReportMember );
		if ( LoadMember ( &MySql, WhereClause, &xmember, 0 ) < 1 )
		{
			printf ( "Unknown member id %ld\n", ReportMember );
			exit ( 1 );
		}
	}

	if ( StockIndex == 'F' )
	{
		if (( fp = fopen ( InputFileName, "r" )) == (FILE *)0 )
		{
			printf ( "Cannot open %s for input\n", InputFileName );
			exit ( 1 );
		}

		while ( fgets ( xbuffer, sizeof(xbuffer), fp ) != (char *)0 )
		{
			if ( GetTokensA ( xbuffer, " \t\r\n", tokens, 2 ) < 1 )
			{
				continue;
			}
			
			sprintf ( WhereClause, "Sticker = '%s'", tokens[0] );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)())LoadArray, 1 );
		}

		nsFclose ( fp );
	
		ReportCount = StockCount;
	}
	else if ( StockIndex == 'W' )
	{
		sprintf ( WhereClause, "Wmember = %ld", ReportMember );
		LoadWatchlistCB ( &MySql, WhereClause, "Wticker", &xwatchlist, (int(*)())LoadArrayW, 0 );
	}
	else if ( StockIndex == 'P' )
	{
		sprintf ( WhereClause, "Pmember = %ld", ReportMember );
		LoadPortfolioCB ( &MySql, WhereClause, "Pticker", &xportfolio, (int(*)())LoadArrayP, 0 );
	}
	else if ( StockIndex == 'Z' )
	{
		sprintf ( WhereClause, "Tmember = %ld", ReportMember );
		LoadTempCB ( &MySql, WhereClause, "Tticker", &xtemp, (int(*)())LoadArrayT, 0 );
	}
	else if ( StockIndex == 'x' )
	{
		sprintf ( WhereClause, "Sticker = '%s'", Ticker );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)())LoadArray, 0 );
	}
	else
	{
		SetWhereClause ( WhereClause, StockIndex );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)())LoadArray, 0 );
	}

	if ( Debug )
	{
		printf ( "Loaded                  %d stocks\n", StockCount );
		printf ( "Missing Buy Date        %d\n", MissingStartDate );
		printf ( "Missing Sell Date       %d\n", MissingEndDate );
		printf ( "Missing High Low Prices %d\n", MissingHighLowPrices );
	}

	if ( StockCount == 0 )
	{
		return ( 0 );
	}

	for ( ndx = 0; ndx < StockCount; ndx++ )
	{
		FindLine ( ndx );
	}

	qsort ( StockArray, StockCount, sizeof(STOCK_RECORD), (int(*)()) cmprec );

	sprintf ( OutFileName, "%s/regress_%d.dat", TEMPDIR, getpid() );
	if (( fpOutput = fopen ( OutFileName, "w" )) == 0 )
	{
		printf ( "Cannot create %s for output\n", OutFileName );
	}

	for ( ndx = 0; ndx < ReportCount && ndx < StockCount; ndx++ ) 
	{
		Percent = 100.0 * StockArray[ndx].StdDev / StockArray[ndx].CurrentPrice;

		fprintf ( fpOutput, "%s|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f\n", 
					StockArray[ndx].xsticker,
					StockArray[ndx].xsname,
					StockArray[ndx].Intercept,
					StockArray[ndx].Slope,
					StockArray[ndx].Return,
					StockArray[ndx].CorrCoeff,
					StockArray[ndx].StdDev,
					Percent,
					StockArray[ndx].Variance );
	}

	nsFclose ( fpOutput );

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_APPEND_W:
		case RPT_FORMAT_REPLACE_W:
			rv = ToWatchlist ( &MySql, OutFileName, "|", 1, ReportOptions.Format, ReportMember, 1 );
			if ( rv < 0 )
			{
				printf ( "ToWatchlist failed, rv %d\n", rv );
			}
			else if ( rv == 0 )
			{
				printf ( "Nothing added to watchlist\n" );
			}
			else
			{
				printf ( "Added %d stocks to watchlist\n", rv );
			}
			break;
		case RPT_FORMAT_TEMP:
			rv = ToTemp ( &MySql, OutFileName, "|", 1, ReportOptions.Format, ReportMember, 1 );
			if ( rv < 0 )
			{
				printf ( "ToTemp failed, rv %d\n", rv );
			}
			else if ( rv == 0 )
			{
				printf ( "Nothing added to temp file\n" );
			}
			else
			{
				printf ( "Added %d stocks to temp file\n", rv );
			}
			break;
		default:
			dorpt ();
			break;
	}


	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_PDF_VIEW:
		case RPT_FORMAT_TEXT:
			sprintf ( BodyFilename, "%s/body_%d.txt", TEMPDIR, getpid() );
			if (( bfp = fopen ( BodyFilename, "w" )) == (FILE *)0 )
			{
				printf ( "Cannot create %s for output\n", BodyFilename );
				break;
			}
			fprintf ( bfp, "Stocks included in %s\n", IndexName ( StockIndex ) );
			fprintf ( bfp, "Date range %s to %s\n", StartDate, EndDate );
			fprintf ( bfp, "Top count %d\n", ReportCount );
			nsFclose ( bfp );
			snprintf ( xbuffer, sizeof(xbuffer), "Regression Channel Screener - %d stocks", ReportCount );
			AttachArray[0] = ReportOptions.OutputFilename;
			if ( ReportMember > 0L )
			{
				SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 0, BodyFilename, 1, AttachArray );
				printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, xmember.xmemail );
			}
			else
			{
				/*----------------------------------------------------------
					this only used when run from command line
				----------------------------------------------------------*/
				SendAttached ( REPORT_ADDRESS, "tms@stevelt.com", "", "", xbuffer, 0, BodyFilename, 1, AttachArray );
				printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, "tms@stevelt.com" );
			}
			break;

		case RPT_FORMAT_HTML:
			snprintf ( xbuffer, sizeof(xbuffer), "cat %s", ReportOptions.OutputFilename );
			system ( xbuffer );
			break;
			
	}

	unlink ( OutFileName );
	unlink ( ReportOptions.OutputFilename );
										 
	return ( 0 );
}
