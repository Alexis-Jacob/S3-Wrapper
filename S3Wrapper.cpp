#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include <libs3.h>


#include "S3Wrapper.hpp"

S3Status statusG;

static S3Wrapper *_wrapper = nullptr;

static std::vector<char> rep;

static S3Status getObjectDataCallback(int bufferSize, const char *buffer,
                                      void *callbackData)
{
	for (int i = 0; i < bufferSize;i++)
		rep.push_back(buffer[i]);
    return S3StatusOK;
}

static S3Status responsePropertiesCallback
    (const S3ResponseProperties *properties, void *callbackData)
{
    return S3StatusOK;
}

// static void responseCompleteCallback(S3Status status,
//                                      const S3ErrorDetails *error, 
//                                      void *callbackData)
// {
// 	statusG = status;
// }



// static int putObjectDataCallback(int bufferSize, char *buffer,
//                                  void *callbackData)
// {
// 	memmove(buffer, callbackData, bufferSize);
//     return bufferSize;
// }



// void     S3Wrapper::uploadFile(uint32_t size, std::string &file_name, const void *data)
// {
//     S3PutObjectHandler putObjectHandler =
//     {
//         { &responsePropertiesCallback, &responseCompleteCallback },
//         &putObjectDataCallback
//     };

//     do {
//         S3_put_object(&this->_bucketContext, file_name.c_str(), size, 0, 0,
//                       &putObjectHandler, const_cast<void*>(data));
//     } while (S3_status_is_retryable(statusG));
// }


// void     S3Wrapper::getFile(std::string &&name)
// {
//     S3GetObjectHandler getObjectHandler =
//     {
//         { &responsePropertiesCallback, &responseCompleteCallback },
//         &getObjectDataCallback
//     };

//     do {
//         S3_get_object(&this->_bucketContext, name.c_str(), nullptr, 0,
//     	0, nullptr, &getObjectHandler, nullptr);
//     } while (S3_status_is_retryable(statusG));  
// }

S3Wrapper::S3Wrapper()
{
	S3Status status;
    const char *hostname = "s3-eu-west-1.amazonaws.com"; //getenv("S3_HOSTNAME");
    
    if ((status = S3_initialize("s3", S3_INIT_ALL, hostname)) != S3StatusOK) {
        std::cerr <<  "Failed to initialize libs3: " << S3_get_status_name(status) << std::endl;
        exit(-1);
    }
    else
    	std::cout << "init done" << std::endl;

    const char *aws_id  = getenv("AWS_ACCESS_KEY_ID");
    const char *aws_key = getenv("AWS_SECRET_KEY");

    if ((aws_id == nullptr) || (aws_key == nullptr))
    	std::cerr << "Missing aws access" << std::endl;


    this->_bucketContext = {
        0,
        BUCKET_NAME,
        PROTOCOL,
        URI_STYLE,
        aws_id,
        aws_key
    };

}

S3Wrapper&  S3Wrapper::getS3Wrapper()
{
	if (_wrapper == nullptr)
		_wrapper = new S3Wrapper;
	return *(_wrapper);
}

S3Wrapper::~S3Wrapper()
{
}