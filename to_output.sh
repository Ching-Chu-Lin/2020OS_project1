for file in OS_PJ1_Test/*
do
    output_fname=`echo $file | awk -F '/' '{print $NF}' | awk -F '.' '{print $1}'`
    output_fname="${output_fname}_stdout.txt"
    echo "./main.out < $file > output/$output_fname"
    ./main.out < $file > output/$output_fname
done
