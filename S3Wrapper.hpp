#ifndef __S3Wrapper__
#define __S3Wrapper__

#include <string>
#include <vector>
#include <iostream>

#include <libs3.h>
#include <cstring> //memmove

#include "Blob.hpp"

#define PROTOCOL    S3ProtocolHTTPS
#define URI_STYLE   S3UriStyleVirtualHost

class S3Wrapper;

template<class C>
class Wrapper {
	friend S3Wrapper;

	C        _data;
	S3Status _status;
	int		 _index;
	int      _contentSize;
public:
	Wrapper(C data, int contentSize) : _data(data), _index(0), _contentSize(contentSize) {} 
	Wrapper(): _index(0), _contentSize(0) {}

	static void responseCompleteCallback(S3Status status, const S3ErrorDetails *error, void *callbackData){
		if (status != S3StatusOK)
			std::cerr << S3_get_status_name(status) << " | " << error->message << " | " << std::endl;
		reinterpret_cast<Wrapper*>(callbackData)->_status = status;
	}

	static int putObjectDataCallback(int bufferSize, char *buffer, void *callbackData){
		Wrapper* self = reinterpret_cast<Wrapper*>(callbackData);
		if (bufferSize >= self->_contentSize){
			memmove(buffer, self->_data, self->_contentSize);
			return self->_contentSize;
		}
		memmove(buffer, self->_data, bufferSize);
		self->_contentSize -= bufferSize;
		self->_data = static_cast<char*>(self->_data) + bufferSize;
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


class S3Wrapper : public Blob{
	S3PutObjectHandler _putObjectHandler = {
		   { &WrapperP::responsePropertiesCallback, &WrapperP::responseCompleteCallback }, &WrapperP::putObjectDataCallback
		};
	
	S3GetObjectHandler _getObjectHandler = {
        { &WrapperG::responsePropertiesCallback, &WrapperG::responseCompleteCallback }, &WrapperG::getObjectDataCallback
    };
	S3BucketContext bucketContext;

public:
	~S3Wrapper();
	S3Wrapper(std::string & bucketName);

	virtual void putBinaryData(std::string name, const void * data, uint32_t size) const; //size * sizeof(T)
	virtual void  getFile(std::string name, std::vector<char>&) const;

};

#endif