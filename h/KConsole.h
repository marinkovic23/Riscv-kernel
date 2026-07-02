//
// Created by os on 2/22/26.
//

#ifndef __KCONSOLE_H__
#define __KCONSOLE_H__


class Buffer;

class KConsole {

public:

    static void init();

    static char getc();

    static void putc(char);
    static void putDirect(char);

protected:
private:
    static Buffer* inputBuffer;
    static Buffer* outputBuffer;

    friend class RiscV;
};




#endif