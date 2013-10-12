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

#include "core_util.h"
#include <fstream>
#include <string.h>
#include <sysmodule/sysmodule.h>
using namespace std;


static string *package_names = NULL;

void init_core_file(){
	ecore_file_init();
	netInitialize();
	sysModuleLoad(SYSMODULE_FS);
}

void stop_core_file(){
	netDeinitialize();	
	sysModuleUnload(SYSMODULE_FS);
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
		sprintf(path,"/dev_usb%03d/",usb_num);
		if(sysLv2FsOpenDir(path,&fd) == 0)
			goto close_dir;
	}
	return -1;
close_dir:
	return usb_num;
}

int file_rename(const char* old_file, const char* new_file){
	return sysLv2FsLink(old_file, new_file);
}

#define BLOCK_SIZE	0x1000

int file_copy(const char *source, const char *dest){
	s32 src = -1;
	s32 dst = -1;
	sysFSStat stats;

	char buffer[BLOCK_SIZE];
	u64  i;
	s32  ret;

	ret = sysLv2FsOpen(source, SYS_O_RDONLY, &src, 0, NULL, 0);
	if (ret)
		goto out;

	ret = sysLv2FsOpen(dest, SYS_O_WRONLY | SYS_O_CREAT | SYS_O_TRUNC, &dst, 0, NULL, 0);
	if (ret)
		goto out;

	sysLv2FsChmod(dest, S_IFMT | 0777);

	sysLv2FsFStat(src, &stats);

	for (i = 0; i < stats.st_size;) {
		u64 pos, read, written;

		sysLv2FsLSeek64(src, i, 0, &pos);
		sysLv2FsLSeek64(dst, i, 0, &pos);

		ret = sysLv2FsRead(src, buffer, sizeof(buffer), &read);
		if (ret || !read)
			break;

		ret = sysLv2FsWrite(dst, buffer, read, &written);
		if (ret || !written)
			break;

		i += written;
	}

out:
	if (src >= 0) sysLv2FsClose(src);
	if (dst >= 0) sysLv2FsClose(dst);

	return ret;
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


int get_size(const char *path){
	int length;
	fstream file;
	file.open(path,ios::in|ios::binary);
	if(!file.fail()){
		file.seekg(0, ios::end);
		length = file.tellg();
		file.close();
		return length;
	}
	return 0;
}

