#!/bin/bash
# exam
mkdir mydir
chmod  a+rwx mydir
echo '2023' > myfile
mv moveme ./mydir/
cp copyme ./mydir/copied
cat readme
gcc bad.c 2>err.txt
mkdir gen
if [ $# -eq 0 ]
then
	a=10
else
	a=$1
fi
i=1
while [ $i -le $a ]
do
	touch ./gen/$i.txt
	i=$[$i+1]
done
