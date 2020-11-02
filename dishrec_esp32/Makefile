CXX = g++ -pipe
CXXFLAGS = -std=gnu++2a -Wall -Wextra -O0 -D _DEBUG -g
LDFLAGS = $(INC) -L$(LIB) -L$(LIB)/ebur128 -L$(LIB)/tinyxml2
SRC = $(PWD)/src
BUILD = $(PWD)/build
OUT = $(PWD)/bin
LIB = $(PWD)/lib
INC := -I$(PWD)/include -I/usr/local/include
MKDIR = mkdir -p
NAME = dishrec_dev_test

.PHONY: clean run all output

output: $(BUILD)/main_dev_test.o | $(OUT)
	$(CXX) $(CXXFLAGS) $(MORECXXFLAGS) $(BUILD)/* -o $(OUT)/$(NAME) $(LDFLAGS)

$(BUILD)/main_dev_test.o: $(BUILD)/RingBuffer.o $(BUILD)/BWFHeader.o $(BUILD)/Loudness.o $(BUILD)/Timer.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/main_dev_test.cpp -o $(BUILD)/main_dev_test.o $(LDFLAGS)

$(BUILD)/Loudness.o: $(BUILD)/WavHeader.o $(BUILD)/libebur128.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/Loudness.cpp -o $(BUILD)/Loudness.o $(LDFLAGS) -lebur128.so

$(BUILD)/RingBuffer.o: $(BUILD)/RMS.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/RingBuffer.cpp -o $(BUILD)/RingBuffer.o $(LDFLAGS)

$(BUILD)/BWFHeader.o: $(BUILD)/WavHeader.o $(BUILD)/AudioDataPad.o $(BUILD)/iXML.o | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/BWFHeader.cpp -o $(BUILD)/BWFHeader.o $(LDFLAGS)

$(BUILD)/WavHeader.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/WavHeader.cpp -o $(BUILD)/WavHeader.o $(LDFLAGS)

$(BUILD)/AudioDataPad.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/AudioDataPad.cpp -o $(BUILD)/AudioDataPad.o $(LDFLAGS)

$(BUILD)/iXML.o: $(BUILD)/tinyxml2.so $(BUILD)/BEXTChunk.o $(BUILD)/TimecodeBase.o $(BUILD)/AudioUtils.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/iXML.cpp -o $(BUILD)/iXML.o $(LDFLAGS)

$(BUILD)/TimecodeBase.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/TimecodeBase.cpp -o $(BUILD)/TimecodeBase.o $(LDFLAGS)

$(BUILD)/BEXTChunk.o: $(BUILD)/AudioUtils.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/BEXTChunk.cpp -o $(BUILD)/BEXTChunk.o $(LDFLAGS)

$(BUILD)/tinyxml2.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(LIB)/tinyxml2/tinyxml2.cpp -o $(BUILD)/tinyxml2.o $(LDFLAGS) -shared

$(BUILD)/libebur128.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(LIB)/ebur128/ebur128.c -o $(BUILD)/libebur128.so $(LDFLAGS) -shared

$(BUILD)/RMS.so: $(BUILD)/AudioUtils.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/RMS.cpp -o $(BUILD)/RMS.so $(LDFLAGS) -shared

$(BUILD)/AudioUtils.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/AudioUtils.cpp -o $(BUILD)/AudioUtils.so $(LDFLAGS) -shared

$(BUILD)/Timer.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/Timer.cpp -o $(BUILD)/Timer.so $(LDFLAGS) -shared

$(OUT) $(BUILD):
	$(MKDIR) $@

run: output
	$(OUT)/$(NAME)

clean:
	rm -rf $(BUILD) $(OUT)
