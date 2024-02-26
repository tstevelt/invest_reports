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

#include	"rpt_value.h"

static	COLUMN_HEADINGS	ColumnArray[] = 
{
	{ "TICKER",		"",			INIT_STRING_LEFT },
	{ "COMPANY",	"",			INIT_STRING_LEFT },
	{ "CURRENT",	"",			INIT_DOUBLE_RIGHT },
	{ "GRAHAM",		"VALUE",	INIT_DOUBLE_RIGHT },
	{ "B/H/S",		"",			INIT_STRING_CENTER },
	{ "MODERN",		"VALUE",	INIT_DOUBLE_RIGHT },
	{ "B/H/S",		"",			INIT_STRING_CENTER },
	{ "ANALYST",	"OPINION",	INIT_STRING_CENTER },
};

static	int				ColumnCount = sizeof(ColumnArray) / sizeof(COLUMN_HEADINGS);

void dorpt ( char *DataFilename )
{
	char	xbuffer[1024];
//	char	Subtitle[80];
	char	InfoOne[80];
//	char	InfoTwo[80];
#define		MAXTOKS		100
	char	*tokens[MAXTOKS];
	int		tokcnt;
	FILE	*fpData;

	ReportOptions.HeaderRows = 2;
	ReportOptions.Title = "INTRINSIC VALUE REPORT";
	switch ( StockIndex )
	{
		case 'D':
			ReportOptions.Subtitle = "DOW JONES INDUSTRIALS";
			break;
		case 'S':
			ReportOptions.Subtitle = "S&P 500";
			break;
		case 'N':
			ReportOptions.Subtitle = "NASDAQ";
			break;
		case '1':
			ReportOptions.Subtitle = "RUSSELL 1000";
			break;
		case '2':
			ReportOptions.Subtitle = "RUSSELL 2000";
			break;
		case '3':
			ReportOptions.Subtitle = "RUSSELL 3000";
			break;
		case 'P':
			ReportOptions.Subtitle = "PORTFOLIO";
			break;
		case 'W':
			ReportOptions.Subtitle = "WATCHLIST";
			break;
	}
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
	sprintf ( ReportOptions.OutputFilename,  "%s/%s_%d", TEMPDIR, "value", getpid() );
	strcat ( ReportOptions.OutputFilename, rptextension ( ReportOptions.Format ) );
	ReportOptions.IsStdout = 0;

	fpData = rptinit ( DataFilename, &ReportOptions, ColumnArray, ColumnCount );

	while ( fgets ( xbuffer, sizeof(xbuffer), fpData ) != (char *)0 )
	{
		tokcnt = GetTokensA ( xbuffer, "|\n\r", tokens, MAXTOKS );

		rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, imin(tokcnt,ColumnCount) );
	}

	rptNewLine ( &ReportOptions );

	ColumnArray[2].Options = INIT_STRING_LEFT;
	ColumnArray[3].Options = INIT_STRING_LEFT;
	ColumnArray[5].Options = INIT_STRING_LEFT;

	tokens[0] = "";
	tokens[1] = "";
	tokens[2] = "";
	tokens[3] = "";
	tokens[4] = "";
	tokens[5] = "";
	tokens[6] = "";
	tokens[7] = "";

	if ( ErrorCount )
	{
		sprintf ( InfoOne, "SKIPPED %d STOCKS W/ NEG GROWTH OR EARNINGS", ErrorCount );
		tokens[1] = InfoOne;
		rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, imin(tokcnt,ColumnCount) );
	}

	sprintf ( InfoOne, "BUY/HOLD/SELL range PLUS/MINUS %.2f percent", RangePercent );
	tokens[1] = InfoOne;
	rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, imin(tokcnt,ColumnCount) );

	sprintf ( InfoOne, "Corporate AAA Bond %.2f percent", CurrentYieldAAA );
	tokens[1] = InfoOne;
	rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, imin(tokcnt,ColumnCount) );

	rptclose ( &ReportOptions );
	
}
