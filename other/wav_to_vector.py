#!/usr/bin/env python

import collections
import sys
import wave

filename = sys.argv[1]
outfile = sys.argv[2]
samplesPerLine = 8
sampleWidthDataType = {
        8: 'uint8_t',
        16: 'int16_t',
        24: 'int24_t',
        32: 'float',
        64: 'double'
    }

with open(outfile, 'wb') as out:
    with wave.open(filename, 'rb') as wav:
        numFrames = wav.getnframes()
        width = wav.getsampwidth()
        datatype = sampleWidthDataType.get(width, 'int16_t')
        half = (2 ** ((width * 8) - 1)) - 1
        out.write(b''.join((
                b'#include <vector>\n\n',
                f'std::vector<{datatype}> '.encode('utf-8'),
                '{}'.format(outfile.rpartition('.')[0]).encode('utf-8'),
                b' = {\n',
                (b' ' * 8),
            )))
        index = 0
        lengthIndex = 0
        newline = False
        while index < numFrames:
            if newline:
                out.write(b' ' * 8)
                newline = False
            lengthIndex += 1
            if lengthIndex >= samplesPerLine:
                newline = True
                lengthIndex = 0
            frame = (int.from_bytes(
                    wav.readframes(1), byteorder='little'
                ) - half)
            byte = '{},{}'.format(
                    frame, '\n' if newline else ' ').encode('utf-8'
                )
            if index >= (numFrames + 1):
                byte.rstrip(b',')
            out.write(byte)
            index += 1
        out.write((b'\n' if lengthIndex else b'') + b'    };\n')
