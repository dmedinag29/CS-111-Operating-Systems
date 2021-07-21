default:
	gcc -Wall -Wextra -pthread -g lab2_list.c -o lab2_list SortedList.c
tests: default
	rm -f lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv	
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=1 >> lab2b_list.csv
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=2 >> lab2b_list.csv
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=4 >> lab2b_list.csv
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=8 >> lab2b_list.csv
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=16 >> lab2b_list.csv
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=16 >> lab2b_list.csv
	./lab2_list --threads=1 --lists=4 --yield=id --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --yield=id --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --yield=id --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --yield=id --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --lists=4 --yield=id --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --lists=4 --yield=id --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --yield=id --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --yield=id --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --yield=id --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --lists=4 --yield=id --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --lists=4 --yield=id --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --yield=id --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --yield=id --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --yield=id --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --lists=4 --yield=id --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --lists=4 --yield=id --iterations=80 --sync=m  >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --yield=id --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --yield=id --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --yield=id --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --lists=4 --yield=id --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --lists=4 --yield=id --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --yield=id --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --yield=id --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --yield=id --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --lists=4 --yield=id --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --lists=4 --yield=id --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --yield=id --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --yield=id --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --yield=id --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --lists=4 --yield=id --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --lists=4 --yield=id --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --yield=id --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --yield=id --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --yield=id --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --lists=4 --yield=id --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --lists=4 --yield=id --iterations=80 --sync=s  >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --yield=id --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --yield=id --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --yield=id --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --lists=4 --yield=id --iterations=80 --sync=s >> lab2b_list.csv
	#./lab2_list --threads=1 --lists=1 --iterations=1000 --sync=m >> lab2b_list.csv
	#./lab2_list --threads=2 --lists=1  --iterations=1000 --sync=m >> lab2b_list.csv
	#./lab2_list --threads=4 --lists=1 --iterations=1000 --sync=m >> lab2b_list.csv
	#./lab2_list --threads=8 --lists=1 --iterations=1000 --sync=m >> lab2b_list.csv
	#./lab2_list --threads=12 --lists=1 --iterations=1000 --sync=m >> lab2b_list.csv
	#./lab2_list --threads=1 --lists=1 --iterations=1000 --sync=s >> lab2b_list.csv
	#./lab2_list --threads=2 --lists=1 --iterations=1000 --sync=s >> lab2b_list.csv
	#./lab2_list --threads=4 --lists=1 --iterations=1000 --sync=s >> lab2b_list.csv
	#./lab2_list --threads=8 --lists=1 --iterations=1000 --sync=s >> lab2b_list.csv
	#./lab2_list --threads=12 --lists=1 --iterations=1000 --sync=s >> lab2b_list.csv 
	./lab2_list --threads=1 --lists=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --lists=4  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --lists=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --lists=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --lists=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --lists=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --lists=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=1 --lists=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --lists=8  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --lists=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --lists=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --lists=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --lists=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --lists=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --lists=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --lists=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --lists=8 --iterations=1000 --sync=s >> lab2b_list.csv 
	./lab2_list --threads=1 --lists=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=2 --lists=16  --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=4 --lists=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=8 --lists=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=12 --lists=16 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=1 --lists=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=2 --lists=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=4 --lists=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=8 --lists=16 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=12 --lists=16 --iterations=1000 --sync=s >> lab2b_list.csv 

profile: default		
	rm -f ./raw.gperf profile.out
	LD_PRELOAD=/usr/lib64/libprofiler.so.0 CPUPROFILE=./raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperf >> profile.out
	pprof --list=thread_worker ./lab2_list ./raw.gperf >> profile.out
	rm -f ./raw.gperf

graphs: tests 
	gnuplot lab2_list.gp

clean: 
	rm -f lab2b-204971333.tar.gz lab2_list

dist: graphs profile 
	tar -cvzf lab2b-204971333.tar.gz lab2_list.c Makefile README \
	SortedList.h SortedList.c lab2b_list.csv lab2b_1.png \
	lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png profile.out lab2_list.gp
