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


if [ "$1" = '' ]
then
	echo "RSI_Stats.sh  date"
	exit 1
fi


DATE=$1
EXTRA="Aticker like 'A%' and"

echo "$EXTRA  $DATE"

echo "select 'OVER SOLD', count(*) from average where $EXTRA Adate = '$DATE' and Arsi < 30.0" | mysql -D invest

echo "select 'NORMAL', count(*) from average where $EXTRA Adate = '$DATE' and Arsi >= 30.0 and Arsi <= 70.0" | mysql -D invest

echo "select 'OVER BOUGHT', count(*) from average where $EXTRA Adate = '$DATE' and Arsi > 70.0" | mysql -D invest
 

