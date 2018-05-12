#include <MEL/Utility/Options.hpp>
#include <string>

using namespace mel;

int main(int argc, char *argv[])
{
	Options options("chat.exe", "MEL Chat");
	options.add_options()
		("s,server","Server")
		("u,user","User Name",value<std::string>())
		("h,help","Help");
	auto input = options.parse(argc, argv);


	return 0;
}