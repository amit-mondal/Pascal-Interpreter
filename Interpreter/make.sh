rm ./*~
g++ ./*.cpp -std=c++14 -g -fsanitize=address
./a.out -f test.pascal $*
