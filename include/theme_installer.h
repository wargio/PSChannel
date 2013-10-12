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

#ifndef __PLAYSTATION_CHANNEL_THEME_INSTALLER_H__
#define __PLAYSTATION_CHANNEL_THEME_INSTALLER_H__

#include "unzip.h"
#include "core_util.h"

#define FILE_MISSING		(2)
#define INSTALLED		(1)
#define INSTALL_FAILED		(1)
#define RESTORED		(1)
#define RESTORE_FAILED		(-1)

const char *get_missing_file();
int install_theme(const char* path_zip, const char* dir_to_install);
int create_config_file(const char *path, const char *theme_path);
int restore_original_theme(const char* path, const char* theme_dir);
const char* get_config_file(const char *path);
int check_files(const char *paths);

#endif
