/*----------------------------------------------------------------------------
	Program : rpt_ad_hist.c
	Author  : Tom Stevelt
	Date    : 03/14/2019
	Synopsis: Accumulate / Distribute History report.  Available on 
				website screeners page.

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
#include	"rpt_ad_hist.h"

static int DebugMain = 0;

int main ( int argc, char *argv[] )
{
	char	xbuffer[80];
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

	sprintf ( OutFileName, "%s/ad_hist_%d.dat", TEMPDIR, getpid() );
	if (( fpOutput = fopen ( OutFileName, "w" )) == 0 )
	{
		printf ( "Cannot create %s for output\n", OutFileName );
		exit ( 1 );
	}

	switch ( DateRange )
	{
		case DATE_RANGE_ONE_MONTH:
			sprintf ( WhereClause, "MKTdate >= date_sub( now(),interval 1 month)" );
			break;
		case DATE_RANGE_SIX_MONTHS:
			sprintf ( WhereClause, "MKTdate >= date_sub( now(),interval 6 month)" );
			break;
		case DATE_RANGE_ONE_YEAR:
			sprintf ( WhereClause, "MKTdate >= date_sub( now(),interval 12 month)" );
			break;
	}

	LoadMarketCB ( &MySql, WhereClause, "MKTdate desc, MKTindex", &xmarket, (int(*)()) EachMarket, 0 );

	nsFclose ( fpOutput );

	dorpt ();

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
			switch ( DateRange )
			{
				case DATE_RANGE_ONE_MONTH:
					fprintf ( bfp, "Past One Month\n" );
					break;
				case DATE_RANGE_SIX_MONTHS:
					fprintf ( bfp, "Past Six Months\n" );
					break;
				case DATE_RANGE_ONE_YEAR:
					fprintf ( bfp, "Past Year\n" );
					break;
			}
			nsFclose ( bfp );
			snprintf ( xbuffer, sizeof(xbuffer), "Accumulate/Distribute History" );
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
			
	}

	if ( DebugMain == 0 )
	{
		unlink ( OutFileName );
		unlink ( ReportOptions.OutputFilename );
	}
										 
	return ( 0 );
}
