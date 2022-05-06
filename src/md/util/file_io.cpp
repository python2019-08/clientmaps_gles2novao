#include <cstdio>
#include<stdio.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <cassert>
#include <md/util/logtool.hpp>
#include <md/util/file_io.hpp>

namespace clientmap { 
 
int write_file(const std::string &filename, const std::string &data) 
{
    FILE *fd = NULL;
#ifdef OsID_WINDOWS
    int err = fopen_s(&fd, "data2", "w+");
    if (err == 0) {
        printf("The file 'data2' was opened\n");
    } 
#else
    FILE *fd = fopen(filename.c_str(), "wbe");
#endif
    if (fd) {
        fwrite(data.data(), sizeof(std::string::value_type), data.size(), fd);
        fclose(fd);
    } else {
        //throw std::runtime_error(std::string("Failed to open file ") + filename);
        return -10;
    }
    return 0;
}
 

int writeFile(const std::string& aPath, const std::string& aData) 
{
    std::ofstream file(aPath, std::ios::binary);
    if (file.good()) {
        file << aData;
        //file.write(aData.c_str(), aData.size());//---
        return 0;
    }
    return -10;
}
#if  0
std::string open_tile(std::string const& path)
{
    std::ifstream stream(path.c_str(), 
        std::ios_base::in | std::ios_base::binary);
    if (!stream.is_open())
    {
        throw std::runtime_error("could not open: '" + path + "'");
    }
    std::string message( std::istreambuf_iterator<char>(stream.rdbuf()), 
                         (std::istreambuf_iterator<char>())
                       );
    stream.close();
    return message;
}
#endif
int readFile(const std::string &aFilePath,
    std::string& aFileData
    )
{
    std::ifstream file(aFilePath, std::ios::binary);
    if (file.good()) 
    {
        std::stringstream data;
        data << file.rdbuf();
        aFileData = data.str();
        return 0;
    }
    else {
        //assert(file.good());
        Log::Debug("(^@^) readFile(%s) failed!\n", aFilePath.c_str() );
    }
	
    return -100;
}


int readTxtFile(const std::string& aFilePath,
    std::string& aFileData
)
{
    std::ifstream stream(aFilePath.c_str());
    if (stream.good()) 
    { 
        std::stringstream buffer;
        buffer << stream.rdbuf();
        stream.close();

        aFileData = buffer.str();
        return 0;
    }
    return  -100;

}

int deleteFile(const std::string& filename) 
{
    const int ret = std::remove(filename.c_str());
    //if (ret != 0 && errno != ENOENT) { 
    if (ret != 0) {
        //throw std::runtime_error(std::string("Could not delete file ")
        //                        + filename);
        return -10;
    }
}

int copyFile(const std::string& destination, const std::string& source) 
{
    std::ifstream src(source, std::ios::binary);
    if (!src.good()) { 
        //throw std::runtime_error("Cannot read file " + source);
        return -10;
    }
    
    std::ofstream dst(destination, std::ios::binary);
    if (!dst.good()) 
    { 
        //throw std::runtime_error("Cannot write file " + destination);
        return -20;
    }

    dst << src.rdbuf();
    return 0;
}

int copy_file(const std::string& sourcePath, const std::string& destPath) 
{
    std::string  contents;
    int ret = readFile(sourcePath, contents);
    if (ret <0) {
        return -10;
    }

    return writeFile(destPath, contents);
}
 
} // namespace clientmap
