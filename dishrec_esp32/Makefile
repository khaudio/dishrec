CXX = g++
CXXFLAGS = -std=gnu++2a -Wall -Wextra -O0 -D _DEBUG -g
LDFLAGS = $(INC) -L$(LIB)
SRC = $(PWD)/src
BUILD = $(PWD)/build
OUT = $(PWD)/bin
LIB = $(PWD)/.pio/libdeps/esp-wrover-kit
REMOTE_LIBS := $(foreach dir, $(LIB), -I$(wildcard $(dir)/*))
INC := -I. -I/usr/local/include -I$(PWD)/include/ $(REMOTE_LIBS)
MKDIR = mkdir -p
NAME = dishrec_dev_test

.PHONY: clean run all output

output: $(BUILD)/main_dev_test.o | $(OUT)
	$(CXX) $(CXXFLAGS) $(BUILD)/* -o $(OUT)/$(NAME) $(LDFLAGS)

$(BUILD)/main_dev_test.o: $(BUILD)/RingBuffer.o $(BUILD)/WavHeader.o $(BUILD)/TimecodeBase.o $(BUILD)/iXML.o | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/main_dev_test.cpp -o $(BUILD)/main_dev_test.o $(LDFLAGS)

$(BUILD)/RingBuffer.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/RingBuffer.cpp -o $(BUILD)/RingBuffer.o $(LDFLAGS)

$(BUILD)/WavHeader.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/WavHeader.cpp -o $(BUILD)/WavHeader.o $(LDFLAGS)

$(BUILD)/TimecodeBase.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/TimecodeBase.cpp -o $(BUILD)/TimecodeBase.o $(LDFLAGS)

$(BUILD)/iXML.o: $(BUILD)/tinyxml2.o | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(BUILD)/tinyxml2.o $(SRC)/iXML.cpp -o $(BUILD)/iXML.o $(LDFLAGS)

$(BUILD)/tinyxml2.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(LIB)/tinyxml2/tinyxml2.cpp -o $(BUILD)/tinyxml2.o $(LDFLAGS)

$(OUT) $(BUILD):
	$(MKDIR) $@

run: output
	$(OUT)/$(NAME)

clean:
	rm -rf $(BUILD) $(OUT)
