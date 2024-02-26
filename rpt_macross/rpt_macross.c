/*----------------------------------------------------------------------------
	Program : rpt_macross.c
	Author  : Tom Stevelt
	Date    : 03/18/2019
	Synopsis: Find tops or bottoms based on two ma crossing in past x days.
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

#define	MAIN
#include	"rpt_macross.h"

static int EachWatchlist ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xwatchlist.xwticker );
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		return ( 0 );
	}
	return ( EachStock () );
}

static int EachPortfolio ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xportfolio.xpticker );
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		return ( 0 );
	}
	return ( EachStock () );
}

static int EachTemp ()
{
	sprintf ( WhereClause, "Sticker = '%s'", xtemp.xtticker );
	if ( LoadStock ( &MySql, WhereClause, &xstock, 0, 0 ) < 1 )
	{
		return ( 0 );
	}
	return ( EachStock () );
}

int main ( int argc, char *argv[] )
{
	char	xbuffer[80];
	char	*AttachArray[1];
	char	BodyFilename[128];
	FILE	*bfp;
	int		rv;

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

	sprintf ( OutFileName, "%s/macross_%d.dat", TEMPDIR, getpid() );
	if (( fpOutput = fopen ( OutFileName, "w" )) == 0 )
	{
		printf ( "Cannot create %s for output\n", OutFileName );
		exit ( 1 );
	}

	if ( StockIndex == 'W' )
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
	else
	{
		SetWhereClause ( WhereClause, StockIndex );	
		LoadStockCB ( &MySql, WhereClause, NULL, &xstock, (int(*)()) EachStock, 0 );
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
			fprintf ( bfp, "Stock Universe: %s\n", IndexName(StockIndex) );
			nsFclose ( bfp );
			switch ( BuySellAll )
			{
				case 'B':
					snprintf ( xbuffer, sizeof(xbuffer), "MA Cross Over - Buy Signal" );
					break;
				case 'S':
					snprintf ( xbuffer, sizeof(xbuffer), "MA Cross Over - Sell Signal" );
					break;
				case 'A':
					snprintf ( xbuffer, sizeof(xbuffer), "MA Cross Over - Buy and Sell" );
					break;
			}
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
