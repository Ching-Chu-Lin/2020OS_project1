for file in ../OS_PJ1_Test/*
do
	echo "sudo dmesg -c"
	sudo dmesg -c

	var=`echo $file | awk -F'/' '{print $NF}' | awk -F'.' '{print $1}'`
	var1=${var}_stdout.txt
	var2=${var}_dmesg.txt

	echo "sudo ./main.out < $file > output/$var1"
	sudo ./main.out < $file > output/$var1

	echo "dmesg | grep Project1 > output/$var2"
	dmesg | grep Project1 > output/$var2

done
