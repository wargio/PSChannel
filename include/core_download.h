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

#ifndef __PLAYSTATION_CHANNEL_DOWNLOAD_CORE_H__
#define __PLAYSTATION_CHANNEL_DOWNLOAD_CORE_H__

#include "core.h"
#include <Ecore_File.h>
#include <net/net.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <lv2/sysfs.h>
#include <string>

using namespace std;

void init_core_file();

void stop_core_file();

int file_exist(const char* file);
int file_delete(const char* file);
int dir_delete(const char* file);
int dir_exist(const char* file);
int dir_make(const char* file);
int dir_recursive_delete(const char* file);

int find_usb();
string *get_package_list(const char *file);

#endif
