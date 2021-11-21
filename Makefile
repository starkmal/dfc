all: dfc

OBJS = parser.o  \
       codegen.o \
       main.o    \
       tokens.o

CPPFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs all`
clean:
	$(RM) -rf parser.cpp parser.hpp dfc tokens.cpp $(OBJS)

parser.cpp: parser.y
	bison -d -o $@ $^
	
parser.hpp: parser.cpp

tokens.cpp: tokens.l parser.hpp
	flex -o $@ $^

%.o: %.cpp
	clang++ -c $(CPPFLAGS) -o $@ $<

dfc: $(OBJS)
	clang++ -o $@ $(OBJS) $(CPPFLAGS)

test: dfc test.txt
	cat test.txt | ./dfc