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

#include "unzip.h"
#define BLOCK_SIZE		0x1000
#define MIN(a,b)		((a)<(b) ? (a) : (b))

int extract_zip(const char* fpath, const char* folder){
	struct zip *archive = zip_open(fpath,ZIP_CHECKCONS,NULL);
	int files = zip_get_num_files(archive);
	if(files>0){
		for(int i=0;i<files;i++){
			char path[MAXPATHLEN + 1];
			const char *filename = zip_get_name(archive,i,0);
			strcpy(path,folder);
			if(!filename)
				continue;
			if(filename[0]!='/')
				strcat(path,"/");
			strcat(path,filename);
			if(filename[strlen(filename) - 1]!='/'){
				struct zip_stat st;
				struct zip_file *zfd;
				if(zip_stat_index(archive,i,0,&st)) {
					fprintf(stderr,"Unable to access file \'%s\' in zip.\n",filename);
					continue;
				}
				zfd = zip_fopen_index(archive,i,0);
				if(!zfd){
					fprintf(stderr,"Unable to open file \'%s\' in zip.\n",filename);
					continue;
				}
				fstream file_out;
				file_out.open(path,ios::out|ios::binary);
				char buffer[BLOCK_SIZE];
				for(unsigned int j = 0, buf=BLOCK_SIZE; j < st.size;j+=BLOCK_SIZE){
					if(j+buf>st.size)
						buf = st.size-j;
					zip_fread(zfd,buffer,buf);
					file_out.write(buffer,buf);
				}
				zip_fclose(zfd);
			}else
				mkdir(path,0777);
		}
	}else
		return 0;
	
	return 1;
}
