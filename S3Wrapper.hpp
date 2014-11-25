#ifndef __S3Wrapper__
#define __S3Wrapper__

#include <functional>

#include <string>
#include <vector>

#include <libs3.h>
#include <cstring> //memmove


#define BUCKET_NAME "engine-test"
#define PROTOCOL    S3ProtocolHTTPS
#define URI_STYLE   S3UriStyleVirtualHost

class S3Wrapper;

template<class C>
class Wrapper {
	friend S3Wrapper;

	C        _data;
	S3Status _status;
public:
	Wrapper(C data) : _data(data){}
	Wrapper(){}

	static void responseCompleteCallback(S3Status status, const S3ErrorDetails *error, void *callbackData){
		reinterpret_cast<Wrapper*>(callbackData)->_status = status;
	}

	static int putObjectDataCallback(int bufferSize, char *buffer, void *callbackData){

		memmove(buffer, reinterpret_cast<Wrapper*>(callbackData)->_data, bufferSize);
	    return bufferSize;
	}

	static S3Status responsePropertiesCallback(const S3ResponseProperties *properties, void *callbackData){
    	return S3StatusOK;
	}

	static S3Status getObjectDataCallback(int bufferSize, const char *buffer, void *callbackData){
	for (int i = 0; i < bufferSize;i++)
		reinterpret_cast<Wrapper*>(callbackData)->_data.push_back(buffer[i]);
    return S3StatusOK;
	}
};

using WrapperP = Wrapper<void*>;
using WrapperG = Wrapper<std::vector<char>>;


class S3Wrapper {
	S3PutObjectHandler _putObjectHandler = {
		   { &WrapperP::responsePropertiesCallback, &WrapperP::responseCompleteCallback }, &WrapperP::putObjectDataCallback
		};
	
	S3GetObjectHandler _getObjectHandler = {
        { &WrapperG::responsePropertiesCallback, &WrapperG::responseCompleteCallback }, &WrapperG::getObjectDataCallback
    };

	S3Wrapper();
	S3BucketContext _bucketContext;
public:
	~S3Wrapper();
	static S3Wrapper   &getS3Wrapper();

	template<typename T> 
	void putBinaryData(uint32_t size, std::string name, T* data){
		WrapperP wrapper(const_cast<void*>( static_cast<void const*>(data)));
		do {
		    S3_put_object(&_bucketContext, name.c_str(), size, 0, 0,
		                  &_putObjectHandler, &wrapper);
		} while (S3_status_is_retryable(wrapper._status));
	}

	template<typename T>
	T   getFile(std::string name){
		WrapperG wrapper;
	    do {
	        S3_get_object(&_bucketContext, name.c_str(), nullptr, 0,
	    	0, nullptr, &_getObjectHandler, &wrapper);
	    } while (S3_status_is_retryable(wrapper._status));
	    return T(wrapper._data.data());
	}

};

#endif