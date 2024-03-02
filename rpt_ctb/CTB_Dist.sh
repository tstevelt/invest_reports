#!/bin/sh
#     Invest report
# 
#     Copyright (C)  2019 - 2024 Tom Stevelt
# 
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU Affero General Public License as
#     published by the Free Software Foundation, either version 3 of the
#     License, or (at your option) any later version.
# 
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU Affero General Public License for more details.
# 
#     You should have received a copy of the GNU Affero General Public License
#     along with this program.  If not, see <https://www.gnu.org/licenses/>.

DATE='2023-09-25'
MAX=200

case "$1" in
	'A' )
		echo "select Actb from average where Actb > 0.0 and Actb < $MAX and Adate = '$DATE'" > script
		;;

	'S' )
		echo "select Actb from average, stock where Sticker = Aticker and Stype = 'S' and  Actb > 0.0 and Actb < $MAX and Adate = '$DATE'" > script
		;;

	'E' )
		echo "select Actb from average, stock where Sticker = Aticker and Stype = 'E' and  Actb > 0.0 and Actb < $MAX and Adate = '$DATE'" > script
		;;
	
	'5' )
		echo "select Actb from average, stock where Sticker = Aticker and Ssp500 = 'Y' and  Actb > 0.0 and Actb < $MAX and Adate = '$DATE'" > script
		;;
	
	* )
		echo "CTB_Dist.sh {A|S|E}"
		exit 1
		;;
esac


mysql -D invest < script | grep -v Actb | grep -v '\+' | sed 's/[| ]//g' > x1.csv

distchart -r 50 -in x1.csv | tee Dist_CTB_$1.csv

rm -r x1.csv script

