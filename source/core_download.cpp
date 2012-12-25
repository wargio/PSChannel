/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 This program was created by Grazioli Giovanni Dante <wargio@libero.it>.

*/

#include "core_download.h"
static string *package_names = NULL;

void init_core_file(){
	ecore_file_init();
	netInitialize();
}

void stop_core_file(){
	netDeinitialize();	
	ecore_file_shutdown();
}

int file_exist(const char* file){
	FILE *f = NULL;
	f = fopen(file,"rb");
	if(f == NULL){
		fclose(f);
		return 0;
	}else{
		fclose(f);
		return 1;
	}
//	return ecore_file_exists(file);
}

int file_delete(const char* file){
//	ecore_file_unlink(file)
	return sysLv2FsUnlink(file);
}

int dir_delete(const char* file){
//	ecore_file_unlink(file)
	return sysLv2FsRmdir(file);
}

int dir_recursive_delete(const char* file){
	sysFSDirent entry;
	int fd;
	u64 read;
	if(sysLv2FsOpenDir(file,&fd) == 0){
		while(!sysLv2FsReadDir(fd,&entry,&read) && strlen(entry.d_name)>0){
			char path[100];
			sprintf(path,"%s/%s",file,entry.d_name);
			sysLv2FsUnlink(path);
		}
		sysLv2FsCloseDir(fd);
		return sysLv2FsRmdir(file);
	}
	return -1;
}


int dir_exist(const char* file){
	s32 fd;
	return !(int)sysLv2FsOpenDir(file, &fd);
}

int dir_make(const char* file){
	return sysLv2FsMkdir(file, S_IRWXO | S_IRWXU | S_IRWXG | S_IFDIR);
}

int find_usb(){
	int usb_num;
	char path[15];
	s32 fd;
	for(usb_num=0;usb_num<10;usb_num++){
		sprintf(path,"/dev_usb00%d/",usb_num);
		if(sysLv2FsOpenDir(path,&fd) == 0)
			goto close_dir;
	}
	return -1;
close_dir:
	return usb_num;
}

string *get_package_list(const char *file){
	string package_names1[1024];
	
	if(package_names!=NULL)
		delete [] package_names;

	sysFSDirent entry;
	int fd;
	u64 read;
	int num_pkg=0;
	sysLv2FsOpenDir(file,&fd);
	while(!sysLv2FsReadDir(fd,&entry,&read) && strlen(entry.d_name)>0 && num_pkg < 1024){
		if (!strcmp(entry.d_name, ".") || !strcmp(entry.d_name, "..") || entry.d_type != 2)
			continue;
		if(entry.d_name[strlen(entry.d_name)-3] == 'p' &&
		   entry.d_name[strlen(entry.d_name)-2] == 'k' &&
		   entry.d_name[strlen(entry.d_name)-1] == 'g')
		{
			package_names1[num_pkg] = entry.d_name;
			num_pkg++;
		}
	}
	sysLv2FsCloseDir(fd);
	if(num_pkg>0){
		package_names = new string [num_pkg];
		--num_pkg;
		do{
			package_names[num_pkg] = package_names1[num_pkg];
			--num_pkg;
		}while(num_pkg>0);
		return package_names;
	}
}

