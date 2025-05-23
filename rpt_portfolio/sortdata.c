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

void sortdata ( char *OutFileName )
{
	char	cmdline[1024];
	int		FldNo = 1;

	if ( ReportStyle == STYLE_FUNDAMENTAL )
	{
		snprintf ( cmdline, sizeof(cmdline), 
			"sort -t '|' -k 3,3 -k 1,1 -o %s %s", OutFileName, OutFileName );

		system ( cmdline );
		return;
	}
	switch ( SortBy )
	{
		case SORTBY_TICKER:
			/*----------------------------------------------------------
				Ticker is column one for all reports.
				Except BASE which starts with TYPE. Same sort applies
				Some reports start with Pbroker, then Pticker
			----------------------------------------------------------*/
			break;
		case SORTBY_GAIN:
			switch ( ReportStyle )
			{
				case STYLE_BASE:
					FldNo = 12;
					break;
				case STYLE_TODAY:
					// FldNo = 4;
					FldNo = 8;
					break;
				case STYLE_FULL:
					FldNo = 8;
					break;
				case STYLE_YOY:
					FldNo = 7;
					break;
				case STYLE_ALERTS:
				case STYLE_CUSTOM:
					break;
			}
			break;
		case SORTBY_RETURN:
			switch ( ReportStyle )
			{
				case STYLE_BASE:
					FldNo = 13;
					break;
				case STYLE_TODAY:
					FldNo = 5;
					break;
				case STYLE_ALERTS:
					FldNo = 8;
					break;
				case STYLE_FULL:
					FldNo = 9;
					break;
				case STYLE_YOY:
					FldNo = 8;
					break;
				case STYLE_CUSTOM:
					break;
			}
			break;
		case SORTBY_COMPARED:
			switch ( ReportStyle )
			{
				case STYLE_ALERTS:
					FldNo = 12;
					break;
				case STYLE_BASE:
				case STYLE_TODAY:
				case STYLE_FULL:
				case STYLE_CUSTOM:
					break;
			}
			break;
	}

	snprintf ( cmdline, sizeof(cmdline), "sort -n -t '|' -k %d,%d -o %s %s", FldNo, FldNo, OutFileName, OutFileName );

	system ( cmdline );
}
