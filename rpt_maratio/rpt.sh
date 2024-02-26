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


rm -f x1 x2 x3

echo "select Aticker, count(*) from average, stock where Sticker = Aticker and Srussell = '2' and Adate > '2020-12-01' and Ama200 > 0 and Ama100 > 0
 group by Aticker ;" | mysql | awk -F '|' '{print $2, $3}' > x1

echo "select Aticker, count(*) from average, stock where Sticker = Aticker and Srussell = '2' and Adate > '2020-12-01' and Ama200 > 0 and Ama100 > Ama200
 group by Aticker ;" | mysql | awk -F '|' '{print $2, $3}' > x2

join x1 x2 > x3


cat x3 | awk '{printf("%1 %.2f\n", $1, $3 / $2 );}' > x4


