#include "url.h"

static int ret = 0;
static unsigned int size_w =0;
static bool is_writing = false;

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	ret = IS_DOWNLOADING;
	is_writing = true;
	size_w +=nmemb;
	return written;
}

void download_file(const char* url, const char* dst, curl_progress_callback func, Ecore_File_Download_Completion_Cb cb){
	CURL *curl;
	CURLcode res;
	FILE *fileptr;
	curl = curl_easy_init();
	if(curl){
		fileptr = fopen(dst, "w");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fileptr);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "pschannel");
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, func);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, func);
 		curl_easy_setopt(curl, CURLOPT_COOKIE, "name=pschannel;");
//		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20); 
		size_w =0;
		ret = IS_DOWNLOADING;
		res = curl_easy_perform(curl);
 		
		fclose(fileptr);
		is_writing = false;
		if(res == CURLE_OK){
			ret = DOWNLOAD_DONE;
			if(cb!=NULL)
				cb(NULL, dst, 200);
		}else{
			ret = DOWNLOAD_FAILED;
			if(cb!=NULL)
				cb(NULL, dst, 1);
		}
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
}

void download_game_update(const char* url, const char* dst, curl_progress_callback func, Ecore_File_Download_Completion_Cb cb){
	CURL *curl;
	CURLcode res;
	FILE *fileptr;
	curl = curl_easy_init();
	if(curl){
		fileptr = fopen(dst, "w");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fileptr);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "PS3Update-agent/1.0.0 libhttp/1.0.0");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, func);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, func);
		size_w =0;
		ret = IS_DOWNLOADING;
		res = curl_easy_perform(curl);
		fclose(fileptr);
		is_writing = false;
		if(res == CURLE_OK){
			ret = DOWNLOAD_DONE;
			if(cb!=NULL)
				cb(NULL, dst, 200);
		}else{
			ret = DOWNLOAD_FAILED;
			if(cb!=NULL)
				cb(NULL, dst, 1);
		}
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
}

int get_download_status(){
	return ret;
}

unsigned int get_size_status(){
	return size_w;
}

bool get_download_wr_status(){
	return is_writing;
}

