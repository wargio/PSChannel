#include "theme_installer.h"
#include <string>

static string file_missing = "";

const char *get_missing_file(){
	return file_missing.c_str();
}

int create_config_file(const char *path, const char *theme_path){
	if(file_exist(path))
		file_delete(path);
	
	fstream config;
	config.open(path, ios::out);
	config << theme_path << endl;
	config.close();
	
}

const char* get_config_file(const char *path){
	fstream config;
	config.open(path, ios::in);
	string str = "";
	getline(config,str,'\n');
	config.close();
	return str.c_str();
}

int check_files(const char *paths){
	char files[200];
	sprintf(files,"%s/download.edj",paths);
	if(!file_exist(files)){
		file_missing = "download.edj";
		return -1;
	}
	sprintf(files,"%s/error.edj",paths);
	if(!file_exist(files)){
		file_missing = "error.edj";
		return -1;
	}
	sprintf(files,"%s/info_screen.edj",paths);
	if(!file_exist(files)){
		file_missing = "/info_screen.edj";
		return -1;
	}
	sprintf(files,"%s/main_menu.edj",paths);
	if(!file_exist(files)){
		file_missing = "/main_menu.edj";
		return -1;
	}
	sprintf(files,"%s/sub_menu.edj",paths);
	if(!file_exist(files)){
		file_missing = "/sub_menu.edj";
		return -1;
	}
	sprintf(files,"%s/download_perc.edj",paths);
	if(!file_exist(files)){
		file_missing = "/download_perc.edj";
		return -1;
	}
	sprintf(files,"%s/main_menu_bar.edj",paths);
	if(!file_exist(files)){
		file_missing = "/main_menu_bar.edj";
		return -1;
	}
	sprintf(files,"%s/option_menu.edj",paths);
	if(!file_exist(files)){
		file_missing = "/option_menu.edj";
		return -1;
	}

	return 1;
}

int install_theme(const char* path_zip, const char* dir_to_install){
	extract_zip(path_zip, dir_to_install);

	if(check_files(dir_to_install)<0){
		dir_recursive_delete(dir_to_install);
		return FILE_MISSING;
	}
	return 1;
	
}

int restore_original_theme(const char* path, const char* theme_dir){
	create_config_file(path, theme_dir);
}


