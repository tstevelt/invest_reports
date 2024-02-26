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

#include	"rpt_ad_hist.h"

static	COLUMN_HEADINGS	ColumnArray [] = 
{
	{ "DATE",		"",			INIT_STRING_LEFT },
	{ "NASDAQ",	"ACCUMULATE",	INIT_LONG_RIGHT },
	{ "NASDAQ",	"DISTRIBUTE",	INIT_LONG_RIGHT },
	{ "NASDAQ",	"PERCENT",		INIT_DOUBLE_RIGHT },
	{ "SP500",	"ACCUMULATE",	INIT_LONG_RIGHT },
	{ "SP500",	"DISTRIBUTE",	INIT_LONG_RIGHT },
	{ "SP500",	"PERCENT",		INIT_DOUBLE_RIGHT },
};

static  int		ColumnCount = sizeof(ColumnArray) / sizeof(COLUMN_HEADINGS);

void dorpt ()
{
	char	xbuffer[1024];
#define		MAXTOKS		8
	char	*tokens[MAXTOKS];
	int		tokcnt;
	FILE	*fpData;

	ReportOptions.HeaderRows = 2;
	ReportOptions.Title = "ACCUMULATE/DISTRIBUTE HISTORY";
	switch ( DateRange )
	{
		case DATE_RANGE_ONE_MONTH:
			ReportOptions.Subtitle = "Past One Month - Days in Past 6 Weeks";
			break;
		case DATE_RANGE_SIX_MONTHS:
			ReportOptions.Subtitle = "Past Six Months - Days in Past 6 Weeks";
			break;
		case DATE_RANGE_ONE_YEAR:
			ReportOptions.Subtitle = "Past Year - Days in Past 6 Weeks";
			break;
	}
	ReportOptions.BreakCount = 0;
//ReportOptions.CurrentLineNumber = 0;
//ReportOptions.CurrentPageNumber = 0;
	switch ( ReportOptions.Format )
	{
		case RPT_FORMAT_CSV:
		case RPT_FORMAT_EXCEL:
		case RPT_FORMAT_HTML:
			ReportOptions.LinesPerPage = 0;
			ReportOptions.WritePageNumbers = 0;
			break;
		case RPT_FORMAT_TEXT:
		case RPT_FORMAT_PDF_VIEW:
			ReportOptions.LinesPerPage = 60;
			ReportOptions.WritePageNumbers = 1;
			break;
	}
	sprintf ( ReportOptions.OutputFilename,  "%s/%s_%d", TEMPDIR, "ad_hist", getpid() );
	strcat ( ReportOptions.OutputFilename, rptextension ( ReportOptions.Format ) );
	ReportOptions.IsStdout = 0;

	fpData = rptinit ( OutFileName, &ReportOptions, ColumnArray, ColumnCount );

	while ( fgets ( xbuffer, sizeof(xbuffer), fpData ) != (char *)0 )
	{
		tokcnt = GetTokensA ( xbuffer, "|\n\r", tokens, MAXTOKS );

		rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, tokcnt );
	}

	rptclose ( &ReportOptions );
	
}
