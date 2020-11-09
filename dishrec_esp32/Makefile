CXX = g++ -pipe
CXXFLAGS = -std=gnu++2a -Wall -Wextra -O0 -D _DEBUG -g
LDFLAGS = $(INC) -L$(LIB) -L$(LIB)/ebur128 -L$(LIB)/tinyxml2
SRC = $(PWD)/src
BUILD = $(PWD)/build
OUT = $(PWD)/bin
LIB = $(PWD)/lib
INC := -I$(PWD)/include -I/usr/local/include -I$(LIB) -I$(LIB)/tinyxml2 -I$(LIB)/ebur128
MKDIR = mkdir -p
NAME = dishrec_dev_test

.PHONY: clean run all output

output: $(BUILD)/main_dev_test.o | $(OUT)
	$(CXX) $(CXXFLAGS) $(BUILD)/* -o $(OUT)/$(NAME) $(LDFLAGS)

$(BUILD)/main_dev_test.o: $(BUILD)/RingBuffer.so $(BUILD)/WavWriter.so $(BUILD)/AudioDataPad.o $(BUILD)/RMS.so $(BUILD)/Timer.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/main_dev_test.cpp -o $(BUILD)/main_dev_test.o $(LDFLAGS)

$(BUILD)/RingBuffer.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/RingBuffer.cpp -o $(BUILD)/RingBuffer.so $(LDFLAGS) -shared

$(BUILD)/WavWriter.so: $(BUILD)/BWFHeader.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/WavWriter.cpp -o $(BUILD)/WavWriter.so $(LDFLAGS) -shared

$(BUILD)/BWFHeader.so: $(BUILD)/BEXTChunk.so $(BUILD)/WavHeader.so $(BUILD)/Loudness.so $(BUILD)/iXML.o | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/BWFHeader.cpp -o $(BUILD)/BWFHeader.so $(LDFLAGS) -shared

$(BUILD)/Loudness.so: $(BUILD)/WavHeader.so $(BUILD)/ebur128.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/Loudness.cpp -o $(BUILD)/Loudness.so $(LDFLAGS) -lebur128.so

$(BUILD)/WavHeader.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/WavHeader.cpp -o $(BUILD)/WavHeader.so $(LDFLAGS) -shared

$(BUILD)/AudioDataPad.o: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/AudioDataPad.cpp -o $(BUILD)/AudioDataPad.o $(LDFLAGS)

$(BUILD)/iXML.o: $(BUILD)/tinyxml2.so $(BUILD)/TimecodeBase.so $(BUILD)/AudioUtils.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/iXML.cpp -o $(BUILD)/iXML.o $(LDFLAGS)

$(BUILD)/TimecodeBase.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/TimecodeBase.cpp -o $(BUILD)/TimecodeBase.so $(LDFLAGS)

$(BUILD)/BEXTChunk.so: $(BUILD)/AudioUtils.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/BEXTChunk.cpp -o $(BUILD)/BEXTChunk.so $(LDFLAGS) -shared

$(BUILD)/tinyxml2.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(LIB)/tinyxml2/tinyxml2.cpp -o $(BUILD)/tinyxml2.so $(LDFLAGS) -shared

$(BUILD)/ebur128.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(LIB)/ebur128/ebur128.c -o $(BUILD)/ebur128.so $(LDFLAGS) -shared

$(BUILD)/RMS.so: $(BUILD)/AudioUtils.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/RMS.cpp -o $(BUILD)/RMS.so $(LDFLAGS) -shared

$(BUILD)/AudioUtils.so: $(BUILD)/int_audio.so | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/AudioUtils.cpp -o $(BUILD)/AudioUtils.so $(LDFLAGS) -shared

$(BUILD)/Timer.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/Timer.cpp -o $(BUILD)/Timer.so $(LDFLAGS) -shared

$(BUILD)/int_audio.so: | $(BUILD)
	$(CXX) -c $(CXXFLAGS) $(SRC)/int_audio.cpp -o $(BUILD)/int_audio.so $(LDFLAGS) -shared

$(OUT) $(BUILD):
	$(MKDIR) $@

run: output
	$(OUT)/$(NAME)

clean:
	rm -rf $(BUILD) $(OUT)
