#ifndef __PLAYSTATION_CHANNEL_CURL_H__
#define __PLAYSTATION_CHANNEL_CURL_H__

#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <string>
#include "core_download.h"

#define IS_DOWNLOADING			2
#define DOWNLOAD_FAILED			1
#define DOWNLOAD_DONE			0

void download_file(const char* url, const char* dst, curl_progress_callback func, Ecore_File_Download_Completion_Cb cb);
int get_download_status();
bool get_download_wr_status();

#endif
