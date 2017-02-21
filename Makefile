CXXFLAGS=-O3
LDFLAGS=-O3 -Wl,--gc-sections -s

all: test.exe test_time.exe

%.obj: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

stpcpy_s.obj: stpcpy.S
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test_time.obj: test_time.cpp
	$(CXX) $(CXXFLAGS) -fwhole-program -c -o $@ $<

test_time.exe: stpcpy.obj stpcpy_s.obj test_time.obj
	$(CXX) -o $@ $^ $(LDFLAGS) 

test.exe: test.obj stpcpy_s.obj
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f test.exe test_time.exe stpcpy.obj stpcpy_s.obj test.obj test_time.obj