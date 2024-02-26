/*----------------------------------------------------------------------------
	Program : rpt_volume.c
	Author  : Tom Stevelt
	Date    : 01/21/2019
	Synopsis: Find increasing volume and price
				Available on the website screeners page.

	Who		Date		Modification
	---------------------------------------------------------------------

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
#include	"rpt_volume.h"

static		int		(*ptrFunc)();

static int EachWatchlist ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xwatchlist.xwticker );
	if ( Debug )
	{
		printf ( "LoadArrayW: %s\n", WhereClause );
	}
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		return ( 0 );
	}
	return ( ptrFunc () );
}

static int EachPortfolio ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if ( Debug )
	{
		printf ( "LoadArrayP: %s\n", WhereClause );
	}
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		return ( 0 );
	}
	return ( ptrFunc () );
}

static int EachTemp ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xtemp.xtticker );
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		return ( 0 );
	}
	return ( ptrFunc () );
}

int main ( int argc, char *argv[] )
{
	FILE	*ifp;
	char	xbuffer[80];
	char	*tokens[2];
	int		(*rptFunc)();
	char	*AttachArray[1];
	char	*Subject;
	int		FieldNumber;
	char	BodyFilename[128];
	FILE	*bfp;
	int		rv;

	getargs ( argc, argv );

	switch ( ReportType )
	{
		case 'I':
			ptrFunc = EachStockIncrease;
			rptFunc = dorpt_increase;
			Subject = "Increasing Volume Screener";
			FieldNumber = 8;
			break;
		case 'A':
			ptrFunc = EachStockAccumulate;
			rptFunc = dorpt_accumulate;
			Subject = "Accumulate/Distribute Volume Screener";
			FieldNumber = 6;
			break;
	}

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
		if (( ifp = fopen ( InputFileName, "r" )) == (FILE *)0 )
		{
			printf ( "Cannot open %s for input\n", InputFileName );
			exit ( 1 );
		}

		while ( fgets ( xbuffer, sizeof(xbuffer), ifp ) != (char *)0 )
		{
			if ( GetTokensA ( xbuffer, " ,|\t\r\n", tokens, 2 ) < 1 )
			{
				continue;
			}
			
			sprintf ( WhereClause, "Sticker = '%s'", tokens[0] );
			LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) ptrFunc, 1 );
		}

		nsFclose ( ifp );
	
		Portfolio = StockCount;
	}
	else if ( StockIndex == 'W' )
	{
		sprintf ( WhereClause, "Wmember = %ld", ReportMember );
		LoadWatchlistCB ( &MySql, WhereClause, "Wticker", &xwatchlist, (int(*)()) EachWatchlist, 0 );
	}
	else if ( StockIndex == 'P' )
	{
		sprintf ( WhereClause, "Pmember = %ld", ReportMember );
		LoadPortfolioCB ( &MySql, WhereClause, "Pticker", &xportfolio, (int(*)()) EachPortfolio, 0 );
	}
	else if ( StockIndex == 'Z' )
	{
		sprintf ( WhereClause, "Tmember = %ld", ReportMember );
		LoadTempCB ( &MySql, WhereClause, "Tticker", &xtemp, (int(*)()) EachTemp, 0 );
	}
	else if ( StockIndex == 'x' )
	{
		sprintf ( WhereClause, "Sticker = '%s'", Ticker );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) ptrFunc, 1 );
	}
	else
	{
		SetWhereClause ( WhereClause, StockIndex );
		strcat ( WhereClause, " and Slast is not null" );
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) ptrFunc, 1 );
	}

	if ( fpOutput != (FILE *)0 )
	{
		nsFclose ( fpOutput );
	}

	if ( ReportCount > 0 )
	{
		if ( ReportCount > 1 )
		{
			sortdata ( OutFileName, FieldNumber );
		}

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
				rptFunc ( OutFileName );
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
				switch ( ReportType )
				{
					case 'I':
						fprintf ( bfp, "Stocks included in %s\n", IndexName ( StockIndex ) );
						fprintf ( bfp, "Volume increase greater than %.2f%%\n", TopPercent );
						fprintf ( bfp, "Previous period trading days %d\n", NumberOfDaysPrevious );
						fprintf ( bfp, "Current period trading days %d\n", NumberOfDaysCurrent );
						fprintf ( bfp, "Minumum volume %d\n", MinimumVolume );
						break;
					case 'A':
						fprintf ( bfp, "Stocks included in %s\n", IndexName ( StockIndex ) );
						fprintf ( bfp, "Accumulating greater than %.2f%%\n", TopPercent );
						fprintf ( bfp, "Number of trading days %d\n", NumberOfDays );
						fprintf ( bfp, "Minumum volume %d\n", MinimumVolume );
						break;
				}
				nsFclose ( bfp );
				snprintf ( xbuffer, sizeof(xbuffer), "%s - %d stocks", Subject, ReportCount);
				AttachArray[0] = ReportOptions.OutputFilename;
				if ( ReportMember > 0L )
				{
					SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 0, BodyFilename, 1, AttachArray );
					printf ( "Sending %s to %s\n", ReportOptions.OutputFilename, xmember.xmemail );
				}
				else
				{
					/*----------------------------------------------------------
						this only used if run from command line
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
	}

	return ( 0 );
}
