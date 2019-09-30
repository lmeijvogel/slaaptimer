CXX = g++
CXXFLAGS = -std=c++0x -g -Wall -Wextra -pedantic

TEST_FILES_O = obj/RtcAdapterTests.o

bin/program: bin/ src/main.cpp src/ILight.cpp src/FakeLight.cpp src/ILight.cpp src/LightController.cpp src/NCursesGui.cpp src/AlarmStateMachine.cpp src/LightStateMachine.cpp src/LightBlinker.cpp
	$(CXX) src/main.cpp -o $@ $(CXX_FLAGS) -lncurses

bin/:
	mkdir bin

test: bin bin/run_tests
	bin/run_tests

obj/:
	mkdir obj

obj/catch.o:  obj/ __tests__/suite.cpp
	$(CXX) __tests__/suite.cpp -c -o $@ $(CXXFLAGS)

obj/RtcAdapterTests.o: __tests__/RtcAdapterTests.cpp src/RtcAdapter.cpp
	$(CXX) __tests__/RtcAdapterTests.cpp -c -o $@ $(CXXFLAGS)

bin/run_tests: $(APP_FILES_O) $(TEST_FILES_O) obj/catch.o
	$(CXX) $(TEST_FILES_O) obj/catch.o -o $@ $(CXXFLAGS)
