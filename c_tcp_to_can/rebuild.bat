make clean
rm ./tcp_to_can.exe
rm ./*.txt
 make
 objdump -D -S tcp_to_can.exe > tcp_to_can.rasm
.\tcp_to_can.exe
 