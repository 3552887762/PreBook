
#ifndef _APPENDFILE_H
#define _APPENDFILE_H
//C++
#include<string>
#include<iostream>
#include<memory>
using namespace std;
// C 
#include<stdio.h>
// not thread safe
namespace tulun
{
    class AppendFile
    {
    private:
        static const int FILEBUFFSIZE = 64*1024;// 64k
        FILE *fp_;
        //char buffer_[FILEBUFFSIZE]; // 64K;//8192
        std::unique_ptr<char[]> buffer_;
        //char *buffer_; // malloc(64*1024*sizeof(char));
        size_t writtenBytes_;  // 
        size_t write(const char *info,const size_t len);
    public:
        AppendFile(const string &filename);
        ~AppendFile();
        void append(const string &info);
        void append(const char *info,const size_t len);
        void flush();
        size_t writtenBytes() const ;
    };
}

#endif 
