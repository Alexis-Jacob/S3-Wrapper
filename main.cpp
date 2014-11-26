#include "S3Wrapper.hpp"
#include <vector>
#include <thread> 
#include <string>
#include <functional>
#include <iostream>

void test(int i, Blob *wrapper)
{
	std::string tmp_name = "test" + std::to_string(i) + ".txt";
	std::string tmp_val  = "test n°" + std::to_string(i) + ".txt";
	wrapper->putBinaryData(tmp_name, tmp_val.c_str(), tmp_val.size());
}

 int main()
{
	std::string bucketName("engine-test"); 
	Blob * wrapper = new S3Wrapper(bucketName);

try{
	test(5, wrapper);
	}
	catch (const char *e)
	{
		std::cout << e << std::endl;
	}

	std::vector<std::thread*> v;
	for (int i = 0; i < 10;i++) {
		try {
			auto t = new std::thread(std::bind(test, i, wrapper));
			v.push_back(t);
		}
		catch (const std::exception & e) {
			std::cout << e.what()<< std::endl;
		}
	}

	for (auto thread : v){
			thread->join();
			delete thread;
		}

	std::vector<char> rep;
	wrapper->getFile("test5.txt", rep);
    std::cout << rep.data() << std::endl;
	S3_deinitialize();
}