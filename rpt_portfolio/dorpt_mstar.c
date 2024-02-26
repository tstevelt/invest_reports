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

#include	"rpt_portfolio.h"

void dorpt_mstar ( char *DataFilename )
{
	char	xbuffer[1024];
#define		MAXTOKS		100
	char	*tokens[MAXTOKS];
	FILE	*fpData;
	FILE	*fpCSV;
	double	CashTotal, BondTotal;

	ReportOptions.HeaderRows = 1;
	ReportOptions.Title = "MORNINGSTAR REPORT";
	ReportOptions.Subtitle = "";
	ReportOptions.BreakCount = 0;

	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_HTML:
		case RPT_FORMAT_EMAIL:
			ReportOptions.LinesPerPage = 0;
			ReportOptions.WritePageNumbers = 0;
			break;
		case RPT_FORMAT_TEXT:
		case RPT_FORMAT_PDF_VIEW:
			ReportOptions.LinesPerPage = 42;
			ReportOptions.WritePageNumbers = 1;
			break;
	}
	
	sprintf ( ReportOptions.OutputFilename,  "%s/%s_%d", TEMPDIR, "portfolio", getpid() );
	strcat ( ReportOptions.OutputFilename, rptextension ( ReportOptions.Format ) );
	ReportOptions.IsStdout = 0;

	fpData = fopen ( DataFilename, "r" );
	fpCSV  = fopen ( ReportOptions.OutputFilename, "w" );

	if ( ReportOptions.Format == RPT_FORMAT_HTML )
	{
		fprintf ( fpCSV, "<pre>\n" );
	}
	CashTotal = BondTotal = 0.0;
	while ( fgets ( xbuffer, sizeof(xbuffer), fpData ) != (char *)0 )
	{
		GetTokensA ( xbuffer, "|\n\r", tokens, MAXTOKS );

		switch ( tokens[0][0] )
		{
			case 'C':
				CashTotal += atof ( tokens[3] ) * atof ( tokens[4] );
				break;
			case 'B':
				BondTotal += atof ( tokens[3] ) * atof ( tokens[4] );
				break;
			default:
				fprintf ( fpCSV, "%s,%s,%s,%s\r\n", tokens[1],  tokens[2],  tokens[3],  tokens[4] );
				break;
		}
	}
	if ( BondTotal > 0.0 )
	{
		fprintf ( fpCSV, "**BONDS**,VARIOUS,%.2f\r\n", BondTotal  );
	}
	if ( CashTotal > 0.0 )
	{
		fprintf ( fpCSV, "**CASH**,VARIOUS,%.2f\r\n", CashTotal  );
	}
	if ( ReportOptions.Format == RPT_FORMAT_HTML )
	{
		fprintf ( fpCSV, "</pre>\n" );
	}

	fclose ( fpCSV );
}
