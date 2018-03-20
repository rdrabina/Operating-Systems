A1=2500
S1=4
A2=5000
S2=4
A3=2500
S3=512
A4=5000
S4=512
A5=1000
S5=4096
A6=2000
S6=4096
A7=500
S7=8192
A8=1000
S8=8192
all: ec.o
	./ec generate records.txt $(A1) $(S1) >> results.txt
	./ec sort records.txt $(A1) $(S1) sys >> results.txt
	./ec sort records.txt $(A1) $(S1) lib >> results.txt
	./ec copy records.txt records_copy_sys.txt $(A1) $(S1) sys >> results.txt
	./ec copy records.txt records_copy_lib.txt $(A1) $(S1) lib >> results.txt
	./ec generate records.txt $(A2) $(S2) >> results.txt
	./ec sort records.txt $(A2) $(S2) sys >> results.txt
	./ec sort records.txt $(A2) $(S2) lib >> results.txt
	./ec copy records.txt records_copy_sys.txt $(A2) $(S2) sys >> results.txt
	./ec copy records.txt records_copy_lib.txt $(A2) $(S2) lib >> results.txt
	./ec generate records.txt $(A3) $(S3) >> results.txt
	./ec sort records.txt $(A3) $(S3) sys >> results.txt
	./ec sort records.txt $(A3) $(S3) lib >> results.txt
	./ec copy records.txt records_copy_sys.txt $(A3) $(S3) sys >> results.txt
	./ec copy records.txt records_copy_lib.txt $(A3) $(S3) lib >> results.txt
	./ec generate records.txt $(A4) $(S4) >> results.txt
	./ec sort records.txt $(A4) $(S4) sys >> results.txt
	./ec sort records.txt $(A4) $(S4) lib >> results.txt
	./ec copy records.txt records_copy_sys.txt $(A4) $(S4) sys >> results.txt
	./ec copy records.txt records_copy_lib.txt $(A4) $(S4) lib >> results.txt
	./ec generate records.txt $(A5) $(S5) >> results.txt
	./ec sort records.txt $(A5) $(S5) sys >> results.txt
	./ec sort records.txt $(A5) $(S5) lib >> results.txt
	./ec copy records.txt records_copy_sys.txt $(A5) $(S5) sys >> results.txt
	./ec copy records.txt records_copy_lib.txt $(A5) $(S5) lib >> results.txt
	./ec generate records.txt $(A6) $(S6) >> results.txt
	./ec sort records.txt $(A6) $(S6) sys >> results.txt
	./ec sort records.txt $(A6) $(S6) lib >> results.txt
	./ec copy records.txt records_copy_sys.txt $(A6) $(S6) sys >> results.txt
	./ec copy records.txt records_copy_lib.txt $(A6) $(S6) lib >> results.txt
	./ec generate records.txt $(A7) $(S7) >> results.txt
	./ec sort records.txt $(A7) $(S7) sys >> results.txt
	./ec sort records.txt $(A7) $(S7) lib >> results.txt
	./ec copy records.txt records_copy_sys.txt $(A7) $(S7) sys >> results.txt
	./ec copy records.txt records_copy_lib.txt $(A7) $(S7) lib >> results.txt
	./ec generate records.txt $(A8) $(S8) >> results.txt
	./ec sort records.txt $(A8) $(S8) sys >> results.txt
	./ec sort records.txt $(A8) $(S8) lib >> results.txt
	./ec copy records.txt records_copy_sys.txt $(A8) $(S8) sys >> results.txt
	./ec copy records.txt records_copy_lib.txt $(A8) $(S8) lib >> results.txt

ec.o: Efficiency_comparator.c
	gcc -std=c99 -Wall Efficiency_comparator.c -o ec

clean: 
	rm *.txt 
