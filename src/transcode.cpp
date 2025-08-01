#include "transcode.h"

transCode::transCode()
{

}
bool transCode::transcode(const std::string& inputFile, const std::string& outputFile) {
    // 构建 FFmpeg 命令
    //需要转成 H.264 和 AAC 以满足客户端的需求使用这个
    std::string command = "ffmpeg -i " + inputFile + " -c:v libx264 -c:a aac -strict -2 -start_number 0 -hls_time 10 -hls_list_size 0 -f hls " + outputFile;
    //直接复制编码, 可以快速实现文件切分 ts文件
    //std::string command = "ffmpeg -i " + inputFile + " -codec: copy -start_number 0 -hls_time 10 -hls_list_size 0 -f hls " + outputFile;
    // 调用 FFmpeg 进行转码
    std::cout<<command<<std::endl;
    int result = std::system(command.c_str());
    // 检查转码是否成功
    if (result == 0) {
        std::cout << "Transcoding completed successfully." << std::endl;
        return true;
    } else {
        std::cerr << "Transcoding failed with error code: " << result << std::endl;
        return false;
    }
}


