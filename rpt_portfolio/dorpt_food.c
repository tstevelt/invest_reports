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

#include	"rpthist.h"

static	COLUMN_HEADINGS	ColumnArray [] = 
{
	{ "amount",		"",	INIT_DOUBLE_RIGHT },
	{ "serving",	"",	INIT_STRING_LEFT },
	{ "name",		"",	INIT_STRING_LEFT },
	{ "calorie",	"",	INIT_LONG_RIGHT_TOTAL },
	{ "carb",		"",	INIT_LONG_RIGHT_TOTAL },
	{ "protein",	"",	INIT_LONG_RIGHT_TOTAL },
	{ "fat",		"",	INIT_LONG_RIGHT_TOTAL },
	{ "sodium",		"",	INIT_LONG_RIGHT_TOTAL },
	{ "fiber",		"",	INIT_DOUBLE_RIGHT_TOTAL },
	{ "pro/cal",	"",	INIT_DOUBLE_RIGHT },
};

static  int		ColumnCount = sizeof(ColumnArray) / sizeof(COLUMN_HEADINGS);

void dorpt_food ()
{
	char	xbuffer[1024];
#define		MAXTOKS		12
	char	*tokens[MAXTOKS];
	int		tokcnt;
	char	Title[80];
	long	ReportTotals[6];

	sprintf ( Title, "Nutrition History List" );

	memset ( ReportTotals, '\0', sizeof(ReportTotals) );

	ReportOptions.HeaderRows = 1;
	ReportOptions.Title = Title;
	ReportOptions.Subtitle = "";
	ReportOptions.BreakCount = 1;
	// ReportOptions.Format = ReportFormat;
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
		case RPT_FORMAT_PDF_EMAIL:
			ReportOptions.LinesPerPage = 60;
			ReportOptions.WritePageNumbers = 1;
			break;
	}
	sprintf ( ReportOptions.OutputFilename,  "%s/%s_%d", TEMPDIR, "rpthist", getpid() );
	strcat ( ReportOptions.OutputFilename, rptextension ( ReportOptions.Format ) );
	ReportOptions.IsStdout = 0;

	fpData = rptinit ( fnData, &ReportOptions, ColumnArray, ColumnCount );

	while ( fgets ( xbuffer, sizeof(xbuffer), fpData ) != (char *)0 )
	{
		tokcnt = GetTokensA ( xbuffer, "|\n\r", tokens, MAXTOKS );

		rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, tokcnt );
	}
	nsFclose ( fpData );

	ReportTotals[1] = ColumnArray[3].Total.longTotal[1];
	ReportTotals[2] = ColumnArray[4].Total.longTotal[1];
	ReportTotals[3] = ColumnArray[5].Total.longTotal[1];
	ReportTotals[4] = ColumnArray[6].Total.longTotal[1];
	ReportTotals[5] = ColumnArray[7].Total.longTotal[1];
	ReportTotals[6] = ColumnArray[8].Total.doubleTotal[1];

	rpttotal ( &ReportOptions, ColumnArray, ColumnCount, 0 );

	rpttotal ( &ReportOptions, ColumnArray, ColumnCount, 1 );

	ReportOptions.BreakCount = 0;
	
	sprintf ( xbuffer, "%s|%s|%s|%d|%d|%d|%d|%d|%d|%.2f",
		// tokens[0], tokens[1],
		"TARGET", " ", " ",
		Duration * CookieMember.xmcalorie,
		Duration * CookieMember.xmcarb,
		Duration * CookieMember.xmprotein,
		Duration * CookieMember.xmfat,
		Duration * CookieMember.xmsodium,
		Duration * CookieMember.xmfiber,
		1000.0 * CookieMember.xmprotein / (double) CookieMember.xmcalorie);

	tokcnt = GetTokensA ( xbuffer, "|\n\r", tokens, MAXTOKS );

	rptline ( &ReportOptions, ColumnArray, ColumnCount, tokens, tokcnt );

	sprintf ( xbuffer, "%s|%s|%s|%.0f|%.0f|%.0f|%.0f|%.0f|%.0f| ",
		// tokens[0], tokens[1],
		"PERCENT", " ", " ",
		100.0 * ReportTotals[1] / (Duration * CookieMember.xmcalorie),
		100.0 * ReportTotals[2] / (Duration * CookieMember.xmcarb   ),
		100.0 * ReportTotals[3] / (Duration * CookieMember.xmprotein),
		100.0 * ReportTotals[4] / (Duration * CookieMember.xmfat    ),
		100.0 * ReportTotals[5] / (Duration * CookieMember.xmsodium ),
		100.0 * ReportTotals[6] / (Duration * CookieMember.xmfiber  ) );

	tokcnt = GetTokensA ( xbuffer, "|\n\r", tokens, MAXTOKS );

	rptline ( &ReportOptions, ColumnArray, ColumnCount - 1, tokens, tokcnt );


	rptclose ( &ReportOptions );
}
