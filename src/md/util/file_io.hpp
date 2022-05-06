#pragma once
 

#include <string>


namespace clientmap { 
 
int writeFile(const std::string& aPath, const std::string& aData);
int readFile(const std::string &aPath,std::string& aFileData);

int readTxtFile(const std::string& aPath,std::string& aFileData);
int deleteFile(const std::string& filename);
int copyFile(const std::string& destination, const std::string& source);
 
} // namespace clientmap
