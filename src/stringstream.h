#ifndef STRINGSTREAM_H
#define STRINGSTREAM_H

#include <Arduino.h>


int hexDigitToInt(uint8_t c)
{
    if ((c >= '0') and (c <= '9'))
        return c - '0';
    
    if ((c >= 'A') and (c <= 'F'))
        return c - 'A' + 10;
    
    if ((c >= 'a') and (c <= 'f'))
        return c - 'a' + 10;

    return -1;
}

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