for file in OS_PJ1_Test/*
do
	var=`echo $file | awk -F'/' '{print $NF}' | awk -F'.' '{print $1}'`
	var=${var}_stdout.txt

	echo "sudo ./main < $file > outputs/$var"
	sudo ./main.out < $file > output/$var

done
