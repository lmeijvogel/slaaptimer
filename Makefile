CXX = g++
CXXFLAGS = -std=c++0x -g -Wall -Wextra -pedantic

TEST_FILES_O = obj/RtcAdapterTests.o

bin/program: bin/ main.cpp ILight.cpp FakeLight.cpp ILight.cpp LightController.cpp NCursesGui.cpp AlarmStateMachine.cpp LightStateMachine.cpp lib/TimeTools.cpp
	$(CXX) main.cpp -o $@ $(CXX_FLAGS) -lncurses

bin/:
	mkdir bin

test: bin bin/run_tests
	bin/run_tests

obj/:
	mkdir obj

obj/catch.o:  obj/ __tests__/suite.cpp
	$(CXX) __tests__/suite.cpp -c -o $@ $(CXXFLAGS)

obj/RtcAdapterTests.o: __tests__/RtcAdapterTests.cpp RtcAdapter.cpp
	$(CXX) __tests__/RtcAdapterTests.cpp -c -o $@ $(CXXFLAGS)

bin/run_tests: $(APP_FILES_O) $(TEST_FILES_O) obj/catch.o
	$(CXX) $(TEST_FILES_O) obj/catch.o -o $@ $(CXXFLAGS)
