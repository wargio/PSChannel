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

#ifndef __PLAYSTATION_CHANNEL_CURL_H__
#define __PLAYSTATION_CHANNEL_CURL_H__

#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <string>
#include "core_util.h"

#define IS_DOWNLOADING			2
#define DOWNLOAD_FAILED			1
#define DOWNLOAD_DONE			0

void download_file(const char* url, const char* dst, curl_progress_callback func, Ecore_File_Download_Completion_Cb cb);
void download_game_update(const char* url, const char* dst, curl_progress_callback func, Ecore_File_Download_Completion_Cb cb);
int get_download_status();
bool get_download_wr_status();
unsigned int get_size_status();

#endif
