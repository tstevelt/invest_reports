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

static	COLUMN_HEADINGS	ColumnArray[] = 
{
	{ "TICKER",		"",			INIT_STRING_LEFT },
	{ "COMPANY",	"",			INIT_STRING_LEFT },
	{ "CURRENT",	"PRICE",	INIT_DOUBLE_RIGHT },
	{ "$ CHANGE",	"",			INIT_DOUBLE_RIGHT },
	{ "% CHANGE",	"",			INIT_DOUBLE_RIGHT },
	{ "SHARES",		"HELD",		INIT_DOUBLE_RIGHT_TOTAL },
	{ "MARKET",		"VALUE",	INIT_DOUBLE_RIGHT_TOTAL },
	{ "GAIN/LOSS",	"TODAY",	INIT_DOUBLE_RIGHT_TOTAL },
};

static	int				ColumnCount = sizeof(ColumnArray) / sizeof(COLUMN_HEADINGS);

void dorpt_today ( char *DataFilename )
{
	char	xbuffer[1024];
#define		MAXTOKS		100
	char	*tokens[MAXTOKS];
	int		tokcnt;
	FILE	*fpData;

	ReportOptions.HeaderRows = 2;
	ReportOptions.Title = "PORTFOLIO REPORT";
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

	fpData = rptinit ( DataFilename, &ReportOptions, ColumnArray, ColumnCount );

	while ( fgets ( xbuffer, sizeof(xbuffer), fpData ) != (char *)0 )
	{
		tokcnt = GetTokensA ( xbuffer, "|\n\r", tokens, MAXTOKS );

		rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, imin(tokcnt,ColumnCount) );
	}

	rpttotal ( &ReportOptions, ColumnArray, ColumnCount, 0 );

	rptclose ( &ReportOptions );
	
}
