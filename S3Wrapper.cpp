#include <iostream>
#include <fstream>

#include <libs3.h>

#include <cstring>
#include <cstdio>

#include "S3Wrapper.hpp"

S3Wrapper::S3Wrapper(std::string & bucketName)
{
    static bool S3_initialized = false;

    if (S3_initialized == false) {
        S3Status status;
        const char *hostname = "s3-eu-west-1.amazonaws.com"; //getenv("S3_HOSTNAME");
        
        if ((status = S3_initialize("s3", S3_INIT_ALL, hostname)) != S3StatusOK) {
            std::cerr <<  "Failed to initialize libs3: " << S3_get_status_name(status) << std::endl;
            exit(-1);
        }
        else
            S3_initialized = true;
    }


    const char *awsId  = getenv("AWS_ACCESS_KEY_ID");
    const char *awsKey = getenv("AWS_SECRET_KEY");

    if ((awsId == nullptr) || (awsKey == nullptr))
    	std::cerr << "Missing aws access" << std::endl;


    this->bucketContext.hostName = nullptr;
    this->bucketContext.bucketName = bucketName.c_str();
    this->bucketContext.protocol = PROTOCOL;
    this->bucketContext.uriStyle = URI_STYLE;
    this->bucketContext.accessKeyId = awsId;
    this->bucketContext.secretAccessKey = awsKey;
}

S3Wrapper::~S3Wrapper()
{
}


void S3Wrapper::putBinaryData(std::string name, const void * data, uint32_t size) const {
     // can be put in a thread =)
    WrapperP wrapper(const_cast<void*>( static_cast<void const*>(data)), size);
    do {
         S3_put_object(&bucketContext, name.c_str(), size, 0, 0,
                       &_putObjectHandler, &wrapper);
     } while (S3_status_is_retryable(wrapper._status));
}

void  S3Wrapper::getFile(std::string name, std::vector<char>&rep) const {
    WrapperG wrapper;
    do {
        S3_get_object(&bucketContext, name.c_str(), nullptr, 0,
     0, nullptr, &_getObjectHandler, &wrapper);
    } while (S3_status_is_retryable(wrapper._status));
    rep = wrapper._data;
}