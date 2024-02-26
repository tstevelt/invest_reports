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

#include	"rpt_rsi.h"

void sortdata ()
{
	char	cmdline[1024];

	switch ( GroupMode )
	{
		case GROUP_NONE:
			if ( SelectStyle == SELECT_BOTTOM )
			{
				snprintf ( cmdline, sizeof(cmdline), 
					"sort -t '|'  -k 4,4n -o %s %s", OutFileName, OutFileName );
			}
			else
			{
				snprintf ( cmdline, sizeof(cmdline), 
					"sort -t '|'  -k 4,4nr -o %s %s", OutFileName, OutFileName );
			}
			break;
		default:
			if ( SelectStyle == SELECT_BOTTOM )
			{
				snprintf ( cmdline, sizeof(cmdline), 
					"sort -t '|' -k 1,1  -k 4,4n -o %s %s", OutFileName, OutFileName );
			}
			else
			{
				snprintf ( cmdline, sizeof(cmdline), 
					"sort -t '|' -k 1,1  -k 4,4nr -o %s %s", OutFileName, OutFileName );
			}
			break;
	}

	system ( cmdline );

}
