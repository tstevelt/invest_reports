/*----------------------------------------------------------------------------
	Program : rpt_dicross.c
	Author  : Tom Stevelt
	Date    : 06/12/2019
	Synopsis: Report crossover of MinusDI and PlusDI
				Available on the website screeners page.

	http://ta-lib.org/d_api/d_api.html

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
#include	"rpt_dicross.h"

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

#ifdef DONT_FIGHT_THE_MARKET
static int EachMarket ()
{
	if ( MarketCount >= MAXHISTRECS )
	{
		printf ( "Exceeds MAXHISTRECS\n" );
		exit ( 1 );
	}

	sprintf ( MarketArray[MarketCount].Date, xmarket.xmktdate );
	MarketArray[MarketCount].Ratio = (double) xmarket.xmktaccum / (double) ( xmarket.xmktaccum + xmarket.xmktdist);
	MarketCount++;

	return ( 0 );
}
#endif

int main ( int argc, char *argv[] )
{
 	char	xbuffer[80];
 	char	*AttachArray[1];
 	char	BodyFilename[128];
 	FILE	*bfp;
#ifdef DONT_FIGHT_THE_MARKET
	char	MarketWhereClause[128];
#endif
	int		rv;

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

#ifdef DONT_FIGHT_THE_MARKET
	sprintf ( MarketWhereClause, "MKTaccum >= MKTdist" );
 	if ( LoadMarketCB ( &MySql, MarketWhereClause, "MKTdate", &xmarket, (int(*)()) EachMarket, 0 ) < 1 )
	{
		printf ( "No market records loaded\n" );
	}
#endif
 	if ( ReportMember > 0L )
 	{
		char	MyWhereClause[128];

 		sprintf ( MyWhereClause, "member.id = %ld", ReportMember );
 		if ( LoadMember ( &MySql, MyWhereClause, &xmember, 0 ) < 1 )
 		{
 			printf ( "Unknown member id %ld\n", ReportMember );
 			exit ( 1 );
 		}
 	}
 
 	sprintf ( OutFileName, "%s/dicross_%d.dat", TEMPDIR, getpid() );
 	if (( fpOutput = fopen ( OutFileName, "w" )) == 0 )
 	{
 		printf ( "Cannot create %s for output\n", OutFileName );
 		exit ( 1 );
 	}
 

	TA_Initialize ();

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
		/*----------------------------------------------------------
			WhereClause is set in getargs().
		----------------------------------------------------------*/
		LoadStockCB ( &MySql, WhereClause, "Sticker", &xstock, (int(*)()) EachStock, 0 );
	}

	nsFclose ( fpOutput );

	sortdata ();

	dorpt ();

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_APPEND_W:
		case RPT_FORMAT_REPLACE_W:
			rv = ToWatchlist ( &MySql, ReportOptions.OutputFilename, "|", 1, ReportOptions.Format, ReportMember, 1 );
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
			rv = ToTemp ( &MySql, ReportOptions.OutputFilename, "|", 1, ReportOptions.Format, ReportMember, 1 );
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
			if ( ReportMember == 0 )
			{
 				snprintf ( xbuffer, sizeof(xbuffer), "DI Cross" );
			}
			else
			{
 				snprintf ( xbuffer, sizeof(xbuffer), "DI Cross %ld %s", ReportMember, xmember.xmname );
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
			if ( nsStrlen ( ReportOptions.OutputFilename ) == 0 )
			{
				printf ( "Missing report file name\n" );
			}
			else
			{
 				snprintf ( xbuffer, sizeof(xbuffer), "cat %s", ReportOptions.OutputFilename );
 				system ( xbuffer );
			}
 			
 	}

	unlink ( OutFileName );
	unlink ( ReportOptions.OutputFilename );

	return ( 0 );
}
