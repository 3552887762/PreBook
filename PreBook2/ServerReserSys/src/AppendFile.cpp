
#include"../include/AppendFile.hpp"
#include<cassert>
#include<cstring>
#include<error.h>

namespace tulun
{

size_t AppendFile::write(const char *info,const size_t len)
{
    return fwrite_unlocked(info,sizeof(char),len,fp_);
    //return fwrite;
}
AppendFile::AppendFile(const string &filename)
    :fp_(nullptr), // e for O_CLOEXEC
    buffer_(new char[FILEBUFFSIZE]),
    writtenBytes_(0)
{
    fp_ = fopen(filename.c_str(),"w");
    cout<<"filename: "<<filename<<endl;
    assert(nullptr != fp_);
    setbuffer(fp_,buffer_.get(),FILEBUFFSIZE);
    //

}
AppendFile::~AppendFile()
{
    fclose(fp_);
}
void AppendFile::append(const string &info)
{
    append(info.c_str(),info.size());
}
void AppendFile::append(const char *info,const size_t len)
{
    size_t n = write(info,len);
    size_t remain = len - n;
    while(remain > 0)
    {
        size_t x = write(info + n,remain);
        if(x == 0)
        {
            int err = ferror(fp_);
            if(err)
            {
                fprintf(stderr,"appenFile::append() failed %s \n",strerror(err));
            }
            break;
            //return ;
        }
        n += x;
        remain = len - n;
    }
    writtenBytes_ += len;
}
void AppendFile::flush()
{
    fflush(fp_);
}
size_t AppendFile::writtenBytes() const 
{
    return writtenBytes_;
}

}