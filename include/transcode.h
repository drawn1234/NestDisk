#ifndef TRANSCODE_H
#define TRANSCODE_H
#include <string>
#include <cstdlib>
#include <iostream>
class transCode
{
public:
    transCode();
     bool transcode(const std::string& inputFile, const std::string& outputFile);
};

#endif // TRANSCODE_H
