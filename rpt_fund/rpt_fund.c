/*----------------------------------------------------------------------------
	Program : rpt_fund.c
	Author  : Tom Stevelt
	Date    : 08/29/2022
	Synopsis: Report of stocks which match criteria entered on the website
				either the Equity or ETF Fundamentals Screener.

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
#include	"rpt_fund.h"

int main ( int argc, char *argv[] )
{
	char	xbuffer[1024];
	char	*AttachArray[1];
	char	BodyFilename[128];
	FILE	*bfp;
	int		ndx;

	getargs ( argc, argv );

	StartMySQL ( &MySql, "invest" );

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
 
	getdata ();

	if ( ReportCount )
	{
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

				fprintf ( bfp, "<html><head></head><body><pre>\n" );

				fprintf ( bfp, "%d stocks, Tokcnt %d\n", ReportCount, Tokcnt );

				for ( ndx = 0; ndx < strlen(Statement2); ndx++ )
				{
					switch ( Statement2[ndx] )
					{
						case '<':
							fprintf ( bfp, "&lt;" );
							break;
						case '>':
							fprintf ( bfp, "&gt;" );
							break;
						case '=':
							fprintf ( bfp, "&#61;" );
							break;
						default:
							if (( strncmp ( &Statement2[ndx], "from", 4  ) == 0 ) ||
								( strncmp ( &Statement2[ndx], "where", 5 ) == 0 ) ||
								( strncmp ( &Statement2[ndx], "and", 3   ) == 0 ))
							{
								fputc ( '\n', bfp );
							}

							fputc ( Statement2[ndx], bfp );
							break;
					}
				}

				fprintf ( bfp, "</pre></body></html>\n" );
				nsFclose ( bfp );

				snprintf ( xbuffer, sizeof(xbuffer), "Fundamentals - %d stocks", ReportCount );
				AttachArray[0] = ReportOptions.OutputFilename;
				if ( ReportMember > 0L )
				{
					SendAttached ( REPORT_ADDRESS, xmember.xmemail, "", "", xbuffer, 1, BodyFilename, 1, AttachArray );
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
				if ( Debug )
				{
					printf ( "Not removing %s\n", BodyFilename );
					printf ( "Not removing %s\n", AttachArray[0] );
				}
				else
				{
					unlink ( BodyFilename );
					unlink ( AttachArray[0] );
				}
				break;

			case RPT_FORMAT_HTML:
				snprintf ( xbuffer, sizeof(xbuffer), "cat %s", ReportOptions.OutputFilename );
				system ( xbuffer );
				break;
				
		}
	}

	if ( Debug )
	{
		printf ( "Not removing %s\n", OutFileName );
		printf ( "Not removing %s\n", ReportOptions.OutputFilename );
	}
	else
	{
		unlink ( OutFileName );
		unlink ( ReportOptions.OutputFilename );
	}

	return ( 0 );
}
