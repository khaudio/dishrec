CXX = g++
CXXFLAGS = -fPIC -g -std=gnu++2a -Wall -Wextra -I$(INC) -L$(LIB) -D _DEBUG
LDFLAGS = -lpthread
SRC = $(PWD)/src
BUILD = $(PWD)/build
OUT = $(PWD)/bin
INC = $(PWD)/include
LIB = $(PWD)/lib
MKDIR = mkdir -p
NAME = dishrec_dev_test

.PHONY: clean run all output

output: $(BUILD)/main_dev_test.o | $(OUT)
	$(CXX) $(CXXFLAGS) $(BUILD)/* -o $(OUT)/$(NAME) $(LDFLAGS)

$(BUILD)/main_dev_test.o: $(BUILD)/RingBuffer.o $(BUILD)/WavHeader.o $(BUILD)/TimecodeBase.o $(BUILD)/BWFiXML.o | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/main_dev_test.cpp -o $(BUILD)/main_dev_test.o $(LDFLAGS)

$(BUILD)/RingBuffer.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/RingBuffer.cpp -o $(BUILD)/RingBuffer.o $(LDFLAGS)

$(BUILD)/WavHeader.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/WavHeader.cpp -o $(BUILD)/WavHeader.o $(LDFLAGS)

$(BUILD)/TimecodeBase.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/TimecodeBase.cpp -o $(BUILD)/TimecodeBase.o $(LDFLAGS)

$(BUILD)/BWFiXML.o: $(BUILD)/tinyxml2.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/BWFiXML.cpp -o $(BUILD)/BWFiXML.o $(LDFLAGS)

$(BUILD)/tinyxml2.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(LIB)/tinyxml2/tinyxml2.cpp -o $(BUILD)/tinyxml2.so $(LDFLAGS) -shared

$(OUT) $(BUILD):
	$(MKDIR) $@

run: output
	$(OUT)/$(NAME)

clean:
	rm -rf $(BUILD) $(OUT)
