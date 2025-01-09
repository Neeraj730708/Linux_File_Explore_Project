#include "header.cpp"
using namespace std;

int main(){
	path p = current_path();
	home=p;
	printDir(p);
	return 0;
}