headers = ./header/*.h
files = ./*cpp ./test/*.cpp ./definition/*.cxx
objects = ./lib/Nodes.a ./lib/NodesHelper.a
Ldir = -L/usr/lib/x86_64-linux-gnu
libs = -lboost_regex
def = ./definition/
G++ = g++ -std=c++1y -Og -Wall -pthread -Wl,--no-as-needed $(Ldir)
Gdbg = g++ -std=c++1y -Og -Wall -Wshadow -ggdb -pthread -Wl,--no-as-needed $(Ldir)
release = ./bin/main.out

.PHONEY: clean

./bin/main.out: $(files) $(objects) main.cpp
	$(G++) $(libs) $(objects) -o $(release) main.cpp

./lib/Nodes.a: $(headers) $(def)/Nodes.cxx
	$(G++) -o ./lib/Nodes.a -c $(def)/Nodes.cxx

./lib/NodesHelper.a: $(headers) $(def)/NodesHelper.cxx
	$(G++) -o ./lib/NodesHelper.a -c $(def)/NodesHelper.cxx

clean:
	\rm $(objects) && \rm $(release)