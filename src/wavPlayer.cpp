//============================================================================
// Name        : wavPlayer.cpp
// Author      : Pawe³ Po³eæ
// Version     :
// Copyright   : This is properity ot Pawe³ Po³eæ.
// Description : Hello World in C++, Ansi-style
//============================================================================

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <fstream>
#include <conio.h>

using namespace std;

class Wave {

public:
    Wave(const char * filename);
    Wave(Wave wave1, Wave wave2);
    ~Wave();
    void play(bool async=true);
    void print();
    bool isok();

    char * buffer;
    int buffSize;
private:
    bool ok;
    HINSTANCE HInstance;
};

Wave::Wave(const char * filename)
{
    ok = false;
    buffer = 0;
    HInstance = GetModuleHandle(0);


    ifstream infile(filename, ios::binary);

    if (!infile)
    {
         std::cout << "Wave::file error: "<< filename << std::endl;
        return;
    }

    infile.seekg (0, ios::end);   // get length of file
    int length = infile.tellg();
    buffer = new char[length];    // allocate memory
    buffSize = length;
    infile.seekg (0, ios::beg);   // position to start of file
    infile.read (buffer,length);  // read entire file

    infile.close();
    ok = true;
}

Wave::Wave(Wave wave1, Wave wave2)
{
	this->ok = false;
    this->buffer = 0;
    HInstance = GetModuleHandle(0);

    if (!(wave1.isok() && wave2.isok()))
    {
         std::cout << "Wave not ready to mix " << std::endl;
        return;
    }

    int length = max(wave1.buffSize, wave2.buffSize);
    this->buffer = new char[length];    // allocate memory
    this->buffSize = length;

    Wave *t = (wave1.buffSize > wave2.buffSize) ? &wave1 : &wave2;

    for (int i=0; i<44; ++i) {
    	this->buffer[i] = t->buffer[i];
    }

	unsigned w1, w2;
    for (int i=44; i<min(wave1.buffSize, wave2.buffSize); ++i) {
    	if (i&1) {
    		w1|=(unsigned)wave1.buffer[i] << 8;
    		w2|=(unsigned)wave2.buffer[i] << 8;

        	int w = max(w1, w2);//w1+w2/2;//max(w1, w2);
        	this->buffer[i-1]= w&0xFF;
        	this->buffer[i]  = (w>>8) & 0xFF;
    	} else {
    		w1=(unsigned)wave1.buffer[i];
    		w2=(unsigned)wave2.buffer[i];
    	}
    }

    for (int i=min(wave1.buffSize, wave2.buffSize); i<max(wave1.buffSize, wave2.buffSize); ++i) {
    	if (wave1.buffSize> wave2.buffSize) {
    		this->buffer[i] =wave1.buffer[i];
    	} else {
    		this->buffer[i] =wave2.buffer[i];
    	}
    }

    this->ok = true;
}

Wave::~Wave()
{
	PlaySound( NULL, 0, 0);
    //PlaySound( NULL, 0, 0); // STOP ANY PLAYING SOUND
    delete [] buffer;      // before deleting buffer.
}
void Wave::play(bool async)
{
    if (!ok)
        return;

    if (async)
        PlaySound(buffer, HInstance, SND_MEMORY | SND_ASYNC);
    else
        PlaySound(buffer, HInstance, SND_MEMORY);
}


void Wave::print() {
	for (int i=0; i<min(this->buffSize, 100); ++i) {
		cout<<hex<<(unsigned short)this->buffer[i] << ",\t";
		if ( (i&0xF) == 0xF) {
			cout<<endl;
		}
	}
	cout<<endl;
}


bool Wave::isok()
{
    return ok;
}

int main(int argc, char *argv[]) {

    std::cout << "Trying to play sound ...\n";

    Wave one("sound1.WAV");cout<<endl;
    std::cout << "S1\n";
    one.print();
    one.play(0);

    Wave two("sound2.WAV");cout<<endl;
    std::cout << "S2\n";
    two.print();
    two.play(0);

    Wave three(one, two);cout<<endl;
    std::cout << "S3\n";
    three.print();
    three.play(0);

    std::cout << "press key to exit";
    //getch();

    return 0;
}
