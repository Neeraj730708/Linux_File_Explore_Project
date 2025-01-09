#include "header.cpp"
void printDir(path p);
void scrollDir(vector<directory_entry> &dir, int first,int last,int curr);

string perms_linux(perms p){
	ostringstream ss;
	// here owner_read is constant value which will be and with perms to check the result is 0 or not if it is not 0 then it will give "R" otherwise - means read are not valid in this way we check all type of constant and we concatinate result in this way permission is decided. Here permsi is perms is class and owner_read is a constant
	ss << ((p & perms::owner_read) != perms::none ? "r" : "-") 
		<< ((p & perms::owner_write) != perms::none ? "w" : "-")
		<< ((p & perms::owner_exec) != perms::none ? "x" : "-")
		<< ((p & perms::group_read) != perms::none ? "r" : "-")
		<< ((p & perms::group_write) != perms::none ? "w" : "-")
		<< ((p & perms::group_exec) != perms::none ? "x" : "-")
		<< ((p & perms::others_read) != perms::none ? "r" : "-")
		<< ((p & perms::others_write) != perms::none ? "w" : "-")
		<< ((p & perms::others_exec) != perms::none ? "x" : "-");
	return ss.str();
}

string file_size(double size) {
	ostringstream ss;
	string PreFix = "B"; // here size will be sent in B in larges no so how below it will go we will assiogn more size to it.
	if (size > 1000) {
		size /= 1000;
		PreFix = "KB";
	} if (size > 1000) {
		size /= 1000;
		PreFix = "MB";
	} if (size > 1000) {
		size /= 1000;
		PreFix = "GB";
	} if (size > 1000) {
		size /= 1000;
		PreFix = "TB";
	}
	ss.precision(2);
	ss << fixed << size << " " << PreFix;
	return ss.str();
}
vector<directory_entry> dir;
void updateDirEntry(path p,int &first,int &last,int curr){
	current_path(p);	
	dir={directory_entry("."),directory_entry("..")};
	directory_iterator d_itr(p);
	for(auto e:d_itr){
		dir.push_back(e);
	}
	first=0;
	last=dir.size();
	if(last>25) 
		last=25;
	curr=1;
}	
	
void print_dirent(directory_entry const& d,bool color=false){
        ostringstream out;
        string filename=d.path().filename().c_str();
        filename=(filename.size()>24)?(filename.substr(0,21)+".. "):filename;
		out << filename<< string(24 - out.str().length(), ' ');

		if(!d.is_directory()) out << file_size(d.file_size());
		out << string(40 - out.str().length(), ' ');

		out << ((d.is_directory()) ? 'd' : '-');

		out << perms_linux(d.status().permissions())<< string(56 - out.str().length(), ' ');

		struct stat fileStat;
        stat(d.path().filename().c_str(), &fileStat);
        //Unsigned It type User ID to user name and Group ID to group name
        struct passwd *pw = getpwuid(fileStat.st_uid);
		out << pw->pw_name << string(72 - out.str().length(), ' ');//username fetch here
		struct group *gp = getgrgid(fileStat.st_gid);
		out << gp->gr_name << string(88 - out.str().length(), ' ');// group name fetch here
		out << ctime(&fileStat.st_mtime);
		string entry=out.str();
		if(color)entry="\033[0;31m"+entry+"\033[0m";
		cout << entry;
}
stack<string> backStk;
stack<string> forwStk;
void scrollDir(vector<directory_entry> &dir, int first,int last,int curr){
	path p;
    struct termios initialrsettings, newrsettings;
    tcgetattr(fileno(stdin), &initialrsettings);
    //switch to canonical mode and echo mode
    newrsettings = initialrsettings;
    newrsettings.c_lflag &= ~ICANON;
	newrsettings.c_lflag &= ~ECHO;
	tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings);
    while(true){
        cout << "\033c" ;// it clearing the screen
        ostringstream out;
        out << "File Name\t\tFile Size\tPermission\tUserID\t\tGroupID\t\tLast Modified\n" ;
	    out << string(112,'-');
	    cout << out.str() << "\n";			
        for (int i=first;i<last;i++){
        if(i==curr)print_dirent(dir[i],true);
        else print_dirent(dir[i]);
        }
        char key=cin.get();
		if(key==104){
			updateDirEntry(home,first,last,curr);
		}
        if (key==27){
            cin.get();
            char c=cin.get();
            if (c=='A') {
                curr=curr-1;
                if(curr<first){
                    if(curr>=0){
                        first--;
                        last--;
                    }else{
                        last=dir.size();
                        curr=last-1;
                        first=last-25;
                        if(first<0)first=0;
                    }
                }
            }
            else if (c=='B') {
                curr=curr+1;
                if(curr>=last){
                    if(last<dir.size()){
                            last++;
                            first++;
                    }
                    else {
                        curr=0;
                        first=0;
                        last=dir.size();
                        if(last>25)last=25;
                    }
                }
            }
			else if(c=='D'){ //68 // left arrow // back
			if(!backStk.empty()){
				p = current_path();				
				forwStk.push(p);
				string bk=backStk.top();
				backStk.pop();
				updateDirEntry(bk,first,last,curr);
				}
			}
			else if(c=='C'){ //67 // right arrow //forward
				if(!forwStk.empty()){
				p = current_path();				
				backStk.push(p);
				string fw=forwStk.top();
				forwStk.pop();
				updateDirEntry(fw,first,last,curr);
				}
			}			
        }		
        else if (key==10){
            if(dir[curr].is_directory()){
                if(curr==0)continue;
                if(curr==1)break;
				p = current_path();		// new value of current path will be fetched here		
				backStk.push(p);
				while(!forwStk.empty()){ // clearing the forward stack as we are selecting the one folder
					forwStk.pop();
				}
				updateDirEntry(dir[curr].path(),first,last,curr);
            }
			else if(dir[curr].is_regular_file()){
				openFile(dir[curr].path().filename().string());
			}
        }
        else break;
    }
    tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);// for restoring the initial by pasing initial setting
}

void printDir(path p){
	p=absolute(p);
	current_path(p);// it setting the current path to new path whatver path came in printDir
	directory_iterator d_itr(p);	
	dir={directory_entry("."),directory_entry("..")};
	for (auto e : d_itr){
        //skip hidden files
        if((e.status().permissions() & perms::owner_read) == perms::none)continue;
		dir.push_back(e);
	}			
    int first=0,last=dir.size();
    if(last>25)last=25;
    int curr=1;
    scrollDir(dir,first,last,curr);
}


/*code to make the program wait  till you press enter again
	cout<<p<<" durectory of current one after d_itr "<<endl;
				while(1){
					if(cin.get()==10){
						break;
					}
				}
*/