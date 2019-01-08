#ifndef STRINGSTREAM_H
#define STRINGSTREAM_H

#include <Arduino.h>


class StringStream : public Stream
{
public:
    StringStream(String &s) : string(s), position(0) {
      setTimeout(0);
    }

    unsigned int getPositon() const {return position;}
    void setPositon(unsigned int pos) {position = pos;}

    // Stream methods
    virtual int available() { return string.length() - position; }
    virtual int read() { return position < string.length() ? string[position++] : -1; }
    virtual int peek() { return position < string.length() ? string[position] : -1; }
    virtual void flush() { };
    // Print methods
    virtual size_t write(uint8_t c) { string += (char)c; return 1;};

    void discard(int chars)
    {
        if (chars > available())
            position = length;
        else
            position += chars;
    }

private:
    String &string;
    unsigned int length;
    unsigned int position;
};

#endif