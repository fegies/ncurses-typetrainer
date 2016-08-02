#include "dirlist.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

namespace dirops
{

int lsdir(std::string dir, std::vector<std::string>& files)
{
	DIR * dp;
	struct dirent * dirp;
	if((dp = opendir(dir.c_str())) == NULL)
		return errno;
	
	while ((dirp = readdir(dp)) != NULL)
		files.push_back(std::string(dirp->d_name));
	closedir(dp);
	return 0;
}

int lstexts(std::vector<std::string>& files)
{
	int rs = lsdir("texts",files);
	 	if(rs != 0)
	 		return rs;

	for(int y = files.size()-1; y >= 0; --y)
	{
		if(files[y].at(0)=='.')
		{
			files.erase(files.begin()+y);
			continue;
		}
		std::string ext;
		if(files[y].length() > 5)
		{
			ext = files[y].substr(files[y].length()-5,std::string::npos);
			if(ext.compare(".prog")==0)
				files.erase(files.begin()+y);
		}
	}
	return 0;
}

}