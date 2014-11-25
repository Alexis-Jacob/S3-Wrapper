#include "S3Wrapper.hpp"
#include <vector>


int main()
{
	S3Wrapper wrapper = S3Wrapper::getS3Wrapper();

	//wrapper.getFile<std::string>("a08BKEq_700b.jpg");
	for (int i = 0; i < 10;i++)
		wrapper.putBinaryData(13, "test.txt", "olleh world\n");

}