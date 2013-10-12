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

#include "debug_stuff.h"


#include "core.h"
#include "font.h"
#include "image.h"
#include "edje.h"
#include "utils.h"
#include "pkgbubble.h"
#include "application.h"
#include "core_util.h"
#include "unzip.h"
#include "aes_stuff.h"
#include "theme_installer.h"

#include "url.h" //while ecore_download is not working

#include <stdlib.h>
#include <Ecore_Psl1ght.h>
#include <sysmodule/sysmodule.h>
#include <sysutil/sysutil.h>
#include <rsx/gcm_sys.h>

/*
 *  PSC Files
 */
#define PSC_LANG_CONFIG			"/dev_hdd0/game/PSCHANNEL/Lang.cfg"

#define PSC_MAIN_MENU_CATEGORIES_PATH	"/dev_hdd0/game/PSCHANNEL/Categories.txt"
#define PSC_MAIN_MENU_CAT_CUSTOM_PATH	"/dev_hdd0/game/PSCHANNEL/Custom_Categories.txt"

#define PSC_MAIN_MENU_LANG_PATH		"/dev_hdd0/game/PSCHANNEL/Lang.txt"
#define PSC_MAIN_MENU_LANG_CUSTOM_PATH	"/dev_hdd0/game/PSCHANNEL/Custom_Lang.txt"

#define CACHE_PATH			"/dev_hdd0/game/PSCHANNEL/CACHE"

#define PSC_ORIGINAL_PATH		"/dev_hdd0/game/PSCHANNEL/USRDIR/theme"
#define PSC_CUSTOM_PATH			"/dev_hdd0/game/PSCHANNEL/USRDIR/custom_theme"

/*
 *  EDJ Files!
 */

#define PSC_THEME_DIR_PATH		"/dev_hdd0/game/PSCHANNEL/USRDIR"
 
#define PSC_INFO_SCREEN_PATH		"info_screen.edj"
#define PSC_MAIN_MENU_BAR_PATH		"main_menu_bar.edj"
#define PSC_MAIN_MENU_PATH		"main_menu.edj"
#define PSC_SUB_MENU_PATH		"sub_menu.edj"
#define PSC_OPTION_MENU_PATH		"option_menu.edj"
#define PSC_DOWNLOAD_MASK_PATH		"download.edj"
#define PSC_DOWNLOADPERC_MASK_PATH	"download_perc.edj"
#define PSC_ERROR_MASK_PATH		"error.edj"

/*
 *  PSC Settings
 */
#define SUBMENU_0			0
#define SUBMENU_1			1
#define PSC_INFO_SCREEN_TIMER		(0.50)

static int REAL_WIDTH  = 1280;
static int REAL_HEIGHT = 720;
static char THEME_DIR [250] = PSC_ORIGINAL_PATH;

/*
 *  PSC Globals
 */

static Ecore_Timer *information_timer;
static EFL_Core core;

Evas_Object *main_layer;

static EFL_Edj info;
static EFL_Edj menu[4];
static PSC_App Applications[6];
static PSN_Update *Games;
static EFL_Edj bar[2];
static EFL_Edj download;
static EFL_Edj download_ask;
static EFL_Edj download_perc;
static EFL_Edj error_dialog;
static Evas_Object *down;

static uint16_t Cursor_X[3],Cursor_Y[2];
static uint16_t Type_X=0,Type_Y=0;
static uint16_t Bar_X_Val = 0, Bar_Y_Val = 0;

static PSC_Application Apps[5];
static string Categories_Names[6];
static string Lang[32];

static int x = 0;
static int y = 0;

static int pkg=0;
static int category = 0;
static int  games_on_hdd = 0;

static bool is_main_menu = true;
static int is_drawn = 1;

static int app=0;
static bool updates_already_downloaded = false;
static bool is_downloading = false;
static bool is_asking_to_download = false;
static bool is_abort = false;
static bool pkg_is_installed = false;
static bool download_is_failed = false;
static bool is_running = false;
static bool is_error = false;
static bool restart = false;
static bool is_asking_exit_to_xmb = false;
static bool is_option_menu = false;
static bool is_sub_menu = false;
static bool skip_update = true;
static bool draw_only = false;
static bool submenu_reset = false;
static bool game_updates_down = false;
static bool xmb = false;
static bool psc_running = false;

static int download_status = 0;
static u8  submenu_type = SUBMENU_0;

static string download_file_txt = "";
static unsigned int file_size = 0;


#define LANG_MAX		8
#define LANG_STRINGS_MAX	26
static int current_lang = 0;

const  char*Language[LANG_MAX]={"English",
				"Portugues",
				"Espanol",
				"Italiano",
				"Francais",
				"Deutsch",
				"Pусский",
				"Nederlands"
				};

const  char*Lang_XX[LANG_MAX]= {"EN",
				"PT",
				"SP",
				"IT",
				"FR",
				"DE",
				"RU",
				"NL"
				};

char t[12];
const char* PS3_OS_TEXT(float x){
	snprintf(t,12,"PS3 FW %.2f",x);
	return (const char*)t;
}

void check_hdd(const char* path){
	if(games_on_hdd!=0){
		delete [] Games;
		games_on_hdd = 0;
	}
	sysFSDirent entry;
	int fd;
	u64 read;
	if(sysLv2FsOpenDir(path,&fd) == 0){
		string *tmp = new string[512];
		while(!sysLv2FsReadDir(fd,&entry,&read) && strlen(entry.d_name)>0 && games_on_hdd<512){
			if(strcmp (entry.d_name, ".") != 0 && strcmp (entry.d_name, "..") != 0)
				if(checkUpdate(entry.d_name, PSC_NP_ENVIRONMENT))
					tmp[games_on_hdd++] = entry.d_name;
		}
		sysLv2FsCloseDir(fd);
		Games = new PSN_Update[games_on_hdd];
		for(int j=0;j<games_on_hdd;j++){
			Games[j].name = tmp[j].c_str();
		}
		delete [] tmp;
	}
}
void DownloadUpdates(){
	if(games_on_hdd>0 && !updates_already_downloaded){
		char tmp_char[250];
	
		string title_id;
		Applications[5].Image = new EFL_Image [games_on_hdd];
		Applications[5].x     = new int       [games_on_hdd];
		Applications[5].y     = new int       [games_on_hdd];
		for(int j=0, k=0, h=0;j<games_on_hdd;j++){
			title_id = Games[j].name.c_str();
			sprintf(tmp_char,Lang[4].c_str(), j*100.f / games_on_hdd);
			override_text(&menu[3], "error_text", tmp_char);
			draw_only = true;
			ecore_main_loop_iterate();
			draw_only = false;
			if(updateFinder(title_id.c_str(), PSC_NP_ENVIRONMENT, &Games[j])){
				Games[j].loaded = true;
				char* image = new char[150];
				sprintf(image,"/dev_hdd0/game/%s/ICON0.PNG",title_id.c_str());
				Games[j].img_file = image;
				
				delete [] image;
				create_image_obj(&core, &Applications[5].Image[j], Games[j].img_file.c_str(), REAL_WIDTH*0.25, REAL_HEIGHT*0.246); //320x176 <- 144x80
				Applications[5].x[j] = Cursor_X[k];
				Applications[5].y[j] = Cursor_Y[h];
				draw_image(&Applications[5].Image[j], Applications[5].x[j], Applications[5].y[j]);
				evas_object_stack_below(Applications[5].Image[j].obj, bar[0].obj);
				hide_obj(Applications[5].Image[j].obj);
				(k>1) ? h=!h : h=h;
				(k>1) ? k=0 : k++;
			}else
				Games[j].loaded = false;
		}
		override_text(&menu[3], "error_text", " ");
		updates_already_downloaded = true;
	}
}

void download_main_is_finished(void *data, const char *file, int status){
	is_downloading = false;
	is_running = false;
	if(status != 200){
		override_text(&info, "information_download", Lang[3].c_str());
		download_is_failed = true;
		ecore_main_loop_iterate();
		sleep(2);
		return ;
	}

	override_text(&info, "information_download", download_file_txt.c_str());
	ecore_main_loop_iterate();
}

int download_main_func(void *data, const char *file, long int dltotal, long int dlnow, long int ultotal, long int ulnow){
	is_downloading = true;
	if(dltotal!=0){
		char perc[26];
		sprintf(perc,"Downloading.. %ld %%", dlnow*100/dltotal);
		override_text(&info, "information_download", perc);
	}
	return ECORE_FILE_PROGRESS_CONTINUE;
}

int download_main_func_curl(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow){
	is_downloading = true;
	draw_only = true;
	char perc[26];
	if(dltotal!=0){
		sprintf(perc,Lang[4].c_str(), dlnow*100/dltotal);
		override_text(&info, "information_download", perc);
	}else
	if(is_abort){
		is_abort = false;
		is_asking_to_download = false;
		is_downloading = false;
		is_asking_exit_to_xmb = false;
		return -1;
	}
	ecore_main_loop_iterate();
	draw_only = false;
	return 0;
}

int download_update_curl(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow){
	char perc[50];
	draw_only = true;
	if(get_download_wr_status()){
		sprintf(perc,Lang[5].c_str(), get_size_status());
		override_text(&error_dialog, "downloading_text", perc);
	}
	ecore_main_loop_iterate();
	draw_only = false;
	return 0;
}

void download_is_finished(void *data, const char *file, int status){
//	printf("\nDone (status: %d)\n", status);
	download_status = status;
	if(status == 200 && !is_abort){
		override_text(&download_perc, "d_perc_text",Lang[6].c_str());
		ecore_main_loop_iterate();
		if(category!=5){
			int type;
			const char* name;
			if(submenu_type == SUBMENU_0){
				type = Apps[category].type_file[app+(y*3+x)];
				name = Apps[category].pkg[app+(y*3+x)].name[pkg].c_str();
			}else{
				type = Apps[category].type_file[app+(y*2+x)];
				name = Apps[category].pkg[app+(y*2+x)].name[pkg].c_str();
			}
			switch(type){
				case PSC_PACKAGE:
					Create_Bubble(file, name);
					pkg_is_installed = true;
					break;
				case PSC_THEME:
					Copy_Theme(file, name);
					break;
				case PSC_IMAGE:
				// /dev_hdd0/photo
					break;
				case PSC_VIDEO:
					break;
				default:
					break;
			}
		}else if(category==5){
			if(submenu_type == SUBMENU_0){
				Create_Bubble(file, Games[app+(y*3+x)].name.c_str());
			}else{
				Create_Bubble(file, Games[app+(y*2+x)].name.c_str());
			}
			pkg_is_installed = true;
		}
	}
	override_text(&download_perc, "d_perc_text", Lang[7].c_str());
	ecore_main_loop_iterate();
	file_delete(file);
	sleep(1);
	is_downloading = false;
 	hide_obj(down);
 	hide_edj(&download_perc);
	hide_edj(&download);
	if(!is_abort){
		is_asking_to_download = false;
		is_asking_exit_to_xmb = true;
		show_edj(&download_ask);
//		is_error=true;
	}else{
		is_abort = false;
		is_asking_to_download = false;
		is_asking_exit_to_xmb = false;
		is_downloading = false;
	}
	evas_object_resize(down, 0, Bar_Y_Val);
}

int download_func(void *data, const char *file, long int dltotal, long int dlnow, long int ultotal, long int ulnow){
	is_downloading = true;
	if(dltotal!=0){
		char perc[10];
		sprintf(perc,"%ld %%", dlnow*100/dltotal);
		override_text(&download_perc, "d_perc_text", perc);
		evas_object_resize(down, Bar_X_Val*dlnow/dltotal, Bar_Y_Val);
		evas_object_show(down);
	}
	return ECORE_FILE_PROGRESS_CONTINUE;
}

void set_message(const char *msg){
	override_text(&menu[3], "error_text", msg);
	draw_only = true;
	ecore_main_loop_iterate();
	draw_only = false;
}

int download_func_curl(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow){
	is_downloading = true;
	dltotal=file_size;
	char perc[10];
	if(dltotal!=0){
		dlnow = get_size_status();
		sprintf(perc,"%.2f %%", dlnow*100/dltotal);
		override_text(&download_perc, "d_perc_text", perc);
		evas_object_resize(down, Bar_X_Val*dlnow/dltotal, Bar_Y_Val);
	}else if(get_download_wr_status()){
		sprintf(perc,Lang[2].c_str());
		override_text(&download_perc, "d_perc_text", perc);
	}
	if(is_abort){
		return -1;
	}
	ecore_main_loop_iterate();
	return 0;
}

int check_file(const char *paths){
	char files[150];
	sprintf(files,"%s/main_menu_bar.edj",paths);
	if(!file_exist(files))
		return -1;
	sprintf(files,"%s/main_menu.edj",paths);
	if(!file_exist(files))
		return -1;
	sprintf(files,"%s/sub_menu.edj",paths);
	if(!file_exist(files))
		return -1;
	sprintf(files,"%s/option_menu.edj",paths);
	if(!file_exist(files))
		return -1;
	sprintf(files,"%s/download.edj",paths);
	if(!file_exist(files))
		return -1;
	sprintf(files,"%s/download_perc.edj",paths);
	if(!file_exist(files))
		return -1;
	sprintf(files,"%s/error.edj",paths);
	if(!file_exist(files))
		return -1;

	return 1;
}

Eina_Bool delete_info(void *data){
	if(!is_downloading){
		ecore_timer_del(information_timer);
		delete_edj(&info);
		draw_edj(&menu[0], 0, 0);
		draw_edj(&menu[1], 0, 0);
		draw_edj(&menu[2], 0, 0);
		draw_edj(&menu[3], 0, 0);
		hide_edj(&menu[2]);

		override_text(&bar[0], "menu_text", Categories_Names[y*3+x].c_str());
		override_text(&bar[0], "app_type_text", "");
		override_text(&bar[0], "app_version_text", "");
		override_text(&bar[1], "app_name_text",/* (app+(y*2+x) < Apps[category].n_elem) ? (Apps[category].name[app+(y*2+x)].c_str()) :*/ "" );
		override_text(&bar[1], "app_type_text",/* (app+(y*2+x) < Apps[category].n_elem) ? (Apps[category].version[app+(y*2+x)].c_str()) :*/ "" );
		override_text(&bar[1], "app_version_text",/* Apps[category].author[app+(y*2+x)].c_str() */ "");
		draw_edj(&bar[0], Cursor_X[x], Cursor_Y[y]);
		draw_edj(&bar[1], Cursor_X[x], Cursor_Y[y]);
		draw_edj(&download, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2, (REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
		draw_edj(&download_perc, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2, (REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
		draw_edj(&download_ask, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2, (REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
		draw_edj(&error_dialog, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2, (REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
		hide_edj(&bar[1]);
		hide_edj(&download);
		hide_edj(&download_ask);
		hide_edj(&download_perc);
		hide_edj(&error_dialog);
	}	
	return EINA_TRUE;
}

void draw_submenu(){
	if(app > games_on_hdd && category==5)
		app -=6;
	else if(app > Apps[category].n_elem-1 && category!=5)
		app -=6;
	
	char page[20];

	if(submenu_type == SUBMENU_0){
		if(submenu_reset){
			submenu_reset = false;
			Cursor_Y[0] = 0.25972*REAL_HEIGHT;
			Cursor_Y[1] = 0.5445 *REAL_HEIGHT;
	
			Cursor_X[0] = 0.1*REAL_WIDTH;		// APPS      (APP 1) || SYSTEM    (APP 4)
			Cursor_X[1] = 0.37421875*REAL_WIDTH;	// BMANAGERS (APP 2) || MEDIA     (APP 5)
			Cursor_X[2] = 0.65*REAL_WIDTH;		// GAMES     (APP 3) || OPTIONS   (APP 6)
			for(int CATEGORY = 0; CATEGORY < 5; CATEGORY++){
				for(int j=0, k=0, h=0;j<Apps[CATEGORY].n_elem && Apps[CATEGORY].downloaded;j++,(k>1) ? h=!h : h=h,(k>1) ? k=0 : k++ ){
					Applications[CATEGORY].x[j] = Cursor_X[k];
					Applications[CATEGORY].y[j] = Cursor_Y[h];
					resize_image(&Applications[CATEGORY].Image[j], Applications[CATEGORY].x[j], Applications[CATEGORY].y[j], REAL_WIDTH*0.25, REAL_HEIGHT*0.246); // 320x176 <- 144x80 0.1125 0.1125
				}
			}
			
			for(int j=0, k=0, h=0; j<games_on_hdd && Games[j].loaded; j++,(k>1) ? h=!h : h=h,(k>1) ? k=0 : k++ ){
				Applications[5].x[j] = Cursor_X[k];
				Applications[5].y[j] = Cursor_Y[h];
				resize_image(&Applications[5].Image[j], Applications[5].x[j], Applications[5].y[j], REAL_WIDTH*0.25, REAL_HEIGHT*0.246); // 320x176 <- 144x80 0.1125 0.1125
			}
			
			show_edj(&bar[0]);
			hide_edj(&bar[1]);
			show_edj(&menu[1]);
			hide_edj(&menu[2]);
			if(y>1) y = 1;
		}
		if(category!=5){
			sprintf(page,"%d/%d",app+(y*3+x)+1,Apps[category].n_elem);
			override_text(&menu[1], "page_of", page);
			override_text(&bar[0], "menu_text", (app+(y*3+x) < Apps[category].n_elem) ? (Apps[category].name[app+(y*3+x)].c_str()) : "" );
			override_text(&bar[0], "app_type_text", (app+(y*3+x) < Apps[category].n_elem) ? (Apps[category].version[app+(y*3+x)].c_str()) : "" );
			override_text(&bar[0], "app_version_text", Apps[category].author[app+(y*3+x)].c_str());
		}else{
			sprintf(page,"%d/%d",app+(y*3+x)+1,games_on_hdd);
			override_text(&menu[1], "page_of", page);
			override_text(&bar[0], "menu_text", (app+(y*3+x) < games_on_hdd) ? (Games[app+(y*3+x)].name.c_str()) : "" );
			override_text(&bar[0], "app_type_text", (app+(y*3+x) < games_on_hdd) ? (Games[app+(y*3+x)].version[Games[app+(y*3+x)].pkgs-1].c_str()) : "" );
			override_text(&bar[0], "app_version_text", (app+(y*3+x) < games_on_hdd) ? PS3_OS_TEXT(Games[app+(y*3+x)].min_ver[Games[app+(y*3+x)].pkgs-1]) : "" );
		}
		
		if(category!=5){
			for(int i=0;i<Apps[category].n_elem;i++)
				hide_obj(Applications[category].Image[i].obj);
			for(int i=0;i<6 && i+app < Apps[category].n_elem;i++)
				show_obj(Applications[category].Image[app+i].obj);
		}else{
			for(int i=0;i<games_on_hdd;i++)
				hide_obj(Applications[category].Image[i].obj);
			for(int i=0;i<6 && i+app < games_on_hdd;i++)
				show_obj(Applications[category].Image[app+i].obj);
		}
		
		move_edj(&bar[0], Cursor_X[x], Cursor_Y[y]);
		
		if(is_asking_to_download && category!=5){
			override_text(&download, "d_name_text", Apps[category].pkg[app+(y*3+x)].name[pkg].c_str());
			char dst[100];
			if(Apps[category].pkg[app+(y*3+x)].size[pkg]>1000000000)
				sprintf(dst,"%.2f Gb",1.*Apps[category].pkg[app+(y*3+x)].size[pkg]/1000000000);
			else if(Apps[category].pkg[app+(y*3+x)].size[pkg]>1000000)
				sprintf(dst,"%.2f Mb",1.*Apps[category].pkg[app+(y*3+x)].size[pkg]/1000000);
			else if(Apps[category].pkg[app+(y*3+x)].size[pkg]>1000)
				sprintf(dst,"%.2f Kb",1.*Apps[category].pkg[app+(y*3+x)].size[pkg]/1000);
			else
				sprintf(dst,"%d Bytes",1.*Apps[category].pkg[app+(y*3+x)].size[pkg]);
			override_text(&download_perc, "d_perc_text", dst);
		}else if(is_asking_to_download && category==5){
			override_text(&download, "d_name_text", Games[app+(y*3+x)].name.c_str());
			char dst[100];
			
			     if(Games[app+(y*3+x)].size[pkg]>1000000000)
				sprintf(dst,"%.2f Gb",1.*Games[app+(y*3+x)].size[pkg]/1000000000);
			else if(Games[app+(y*3+x)].size[pkg]>1000000)
				sprintf(dst,"%.2f Mb",1.*Games[app+(y*3+x)].size[pkg]/1000000);
			else if(Games[app+(y*3+x)].size[pkg]>1000)
				sprintf(dst,"%.2f Kb",1.*Games[app+(y*3+x)].size[pkg]/1000);
			else
				sprintf(dst,"%d Bytes",1.*Games[app+(y*3+x)].size[pkg]);
			override_text(&download_perc, "d_perc_text", dst);
		}
	}else if(submenu_type == SUBMENU_1){
		if(submenu_reset){
			submenu_reset = false;
			Cursor_Y[0] = 0.121875*REAL_WIDTH;
			Cursor_Y[1] = 0.51328125*REAL_WIDTH;
	
			Cursor_X[0] = 0.307*REAL_HEIGHT;	// APPS      (APP 1) || SYSTEM    (APP 4)
			Cursor_X[1] = 0.480*REAL_HEIGHT;	// BMANAGERS (APP 2) || MEDIA     (APP 5)
			Cursor_X[2] = 0.651389*REAL_HEIGHT;	// GAMES     (APP 3) || OPTIONS   (APP 6)
			for(int CATEGORY = 0; CATEGORY < 5; CATEGORY++){
				for(int j=0, k=0, h=0;j<Apps[CATEGORY].n_elem && Apps[CATEGORY].downloaded;j++){
					Applications[CATEGORY].x[j] = Cursor_Y[h];
					Applications[CATEGORY].y[j] = Cursor_X[k];
					resize_image(&Applications[CATEGORY].Image[j], Applications[CATEGORY].x[j], Applications[CATEGORY].y[j], REAL_WIDTH*0.1125, REAL_HEIGHT*0.112);
					h=!h;
					if(h==0)
						++k;
					if(k>2) k=0;
					
					// 320x176 <- 144x80 0.1125 0.1125
				}
				for(int j=0, k=0, h=0; j<games_on_hdd && Games[j].loaded; j++){
					Applications[5].x[j] = Cursor_Y[h];
					Applications[5].y[j] = Cursor_X[k];
					resize_image(&Applications[5].Image[j], Applications[5].x[j], Applications[5].y[j], REAL_WIDTH*0.1125, REAL_HEIGHT*0.112);
					h=!h;
					if(h==0)
						++k;
					if(k>2) k=0;
				}
			}
			hide_edj(&bar[0]);
			show_edj(&bar[1]);
			hide_edj(&menu[1]);
			show_edj(&menu[2]);
			if(x>1) x = 1;
		}


		if(category!=5){
			sprintf(page,"%d/%d",app+(y*2+x)+1,Apps[category].n_elem);
			override_text(&menu[2], "page_of", page);
		
			override_text(&menu[2], "app1-name", (app < Apps[category].n_elem) ? (Apps[category].name[app].c_str()) : "" );
			override_text(&menu[2], "app1-version", (app < Apps[category].n_elem) ? (Apps[category].version[app].c_str()) : "" );
			override_text(&menu[2], "app1-author", (app < Apps[category].n_elem) ? (Apps[category].author[app].c_str()) : "" );
		
			override_text(&menu[2], "app2-name", (app+1 < Apps[category].n_elem) ? (Apps[category].name[app+1].c_str()) : "" );
			override_text(&menu[2], "app2-version", (app+1 < Apps[category].n_elem) ? (Apps[category].version[app+1].c_str()) : "" );
			override_text(&menu[2], "app2-author", (app+1 < Apps[category].n_elem) ? (Apps[category].author[app+1].c_str()) : "" );
		
			override_text(&menu[2], "app3-name", (app+2 < Apps[category].n_elem) ? (Apps[category].name[app+2].c_str()) : "" );
			override_text(&menu[2], "app3-version", (app+2 < Apps[category].n_elem) ? (Apps[category].version[app+2].c_str()) : "" );
			override_text(&menu[2], "app3-author", (app+2 < Apps[category].n_elem) ? (Apps[category].author[app+2].c_str()) : "" );
		
			override_text(&menu[2], "app4-name", (app+3 < Apps[category].n_elem) ? (Apps[category].name[app+3].c_str()) : "" );
			override_text(&menu[2], "app4-version", (app+3 < Apps[category].n_elem) ? (Apps[category].version[app+3].c_str()) : "" );
			override_text(&menu[2], "app4-author", (app+3 < Apps[category].n_elem) ? (Apps[category].author[app+3].c_str()) : "" );
		
			override_text(&menu[2], "app5-name", (app+4 < Apps[category].n_elem) ? (Apps[category].name[app+4].c_str()) : "" );
			override_text(&menu[2], "app5-version", (app+4 < Apps[category].n_elem) ? (Apps[category].version[app+4].c_str()) : "" );
			override_text(&menu[2], "app5-author", (app+4 < Apps[category].n_elem) ? (Apps[category].author[app+4].c_str()) : "" );
		
			override_text(&menu[2], "app6-name", (app+5 < Apps[category].n_elem) ? (Apps[category].name[app+5].c_str()) : "" );
			override_text(&menu[2], "app6-version", (app+5 < Apps[category].n_elem) ? (Apps[category].version[app+5].c_str()) : "" );
			override_text(&menu[2], "app6-author", (app+5 < Apps[category].n_elem) ? (Apps[category].author[app+5].c_str()) : "" );
		}else{
			sprintf(page,"%d/%d",app+(y*2+x)+1,games_on_hdd);
			override_text(&menu[2], "page_of", page);
			
			override_text(&menu[2], "app1-name", (app < games_on_hdd) ? (Games[app].name.c_str()) : "" );
			override_text(&menu[2], "app1-version", (app < games_on_hdd) ? (Games[app].version[Games[app].pkgs-1].c_str()) : "" );
			override_text(&menu[2], "app1-author", (app < games_on_hdd) ? PS3_OS_TEXT(Games[app].min_ver[Games[app].pkgs-1]) : "" );
		
			override_text(&menu[2], "app2-name", (app+1 < games_on_hdd) ? (Games[app+1].name.c_str()) : "" );
			override_text(&menu[2], "app2-version", (app+1 < games_on_hdd) ? (Games[app+1].version[Games[app+1].pkgs-1].c_str()) : "" );
			override_text(&menu[2], "app2-author", (app+1 < games_on_hdd) ? PS3_OS_TEXT(Games[app+1].min_ver[Games[app+1].pkgs-1]) : "" );
		
			override_text(&menu[2], "app3-name", (app+2 < games_on_hdd) ? (Games[app+2].name.c_str()) : "" );
			override_text(&menu[2], "app3-version", (app+2 < games_on_hdd) ? (Games[app+2].version[Games[app+2].pkgs-1].c_str()) : "" );
			override_text(&menu[2], "app3-author", (app+2 < games_on_hdd) ? PS3_OS_TEXT(Games[app+2].min_ver[Games[app+2].pkgs-1]) : "" );
		
			override_text(&menu[2], "app4-name", (app+3 < games_on_hdd) ? (Games[app+3].name.c_str()) : "" );
			override_text(&menu[2], "app4-version", (app+3 < games_on_hdd) ? (Games[app+3].version[Games[app+3].pkgs-1].c_str()) : "" );
			override_text(&menu[2], "app4-author", (app+3 < games_on_hdd) ? PS3_OS_TEXT(Games[app+3].min_ver[Games[app+3].pkgs-1]) : "" );
		
			override_text(&menu[2], "app5-name", (app+4 < games_on_hdd) ? (Games[app+4].name.c_str()) : "" );
			override_text(&menu[2], "app5-version", (app+4 < games_on_hdd) ? (Games[app+4].version[Games[app+4].pkgs-1].c_str()) : "" );
			override_text(&menu[2], "app5-author", (app+4 < games_on_hdd) ? PS3_OS_TEXT(Games[app+4].min_ver[Games[app+4].pkgs-1]) : "" );
		
			override_text(&menu[2], "app6-name", (app+5 < games_on_hdd) ? (Games[app+5].name.c_str()) : "" );
			override_text(&menu[2], "app6-version", (app+5 < games_on_hdd) ? (Games[app+5].version[Games[app+5].pkgs-1].c_str()) : "" );
			override_text(&menu[2], "app6-author", (app+5 < games_on_hdd) ? PS3_OS_TEXT(Games[app+5].min_ver[Games[app+5].pkgs-1]) : "" );
		}
		
		if(category!=5){
			for(int i=0;i<Apps[category].n_elem;i++)
				hide_obj(Applications[category].Image[i].obj);
			for(int i=0;i<6 && i+app < Apps[category].n_elem;i++)
				show_obj(Applications[category].Image[app+i].obj);
		}else{
			for(int i=0;i<games_on_hdd;i++)
				hide_obj(Applications[category].Image[i].obj);
			for(int i=0;i<6 && i+app < games_on_hdd;i++)
				show_obj(Applications[category].Image[app+i].obj);
		}
			
		move_edj(&bar[1], Cursor_Y[x], Cursor_X[y]);
		if(is_asking_to_download && category!=5){
			override_text(&download, "d_name_text", Apps[category].pkg[app+(y*2+x)].name[pkg].c_str());
			char dst[100];
			if(Apps[category].pkg[app+(y*2+x)].size[pkg]>1000000000)
				sprintf(dst,"%.2f Gb",1.*Apps[category].pkg[app+(y*2+x)].size[pkg]/1000000000);
			else if(Apps[category].pkg[app+(y*2+x)].size[pkg]>1000000)
				sprintf(dst,"%.2f Mb",1.*Apps[category].pkg[app+(y*2+x)].size[pkg]/1000000);
			else if(Apps[category].pkg[app+(y*2+x)].size[pkg]>1000)
				sprintf(dst,"%.2f Kb",1.*Apps[category].pkg[app+(y*2+x)].size[pkg]/1000);
			else
				sprintf(dst,"%d Bytes",1.*Apps[category].pkg[app+(y*2+x)].size[pkg]);
			override_text(&download_perc, "d_perc_text", dst);
		}else if(is_asking_to_download && category==5){
			override_text(&download, "d_name_text", Games[app+(y*2+x)].name.c_str());
			char dst[100];
			
			     if(Games[app+(y*2+x)].size[pkg]>1000000000)
				sprintf(dst,"%.2f Gb",1.*Games[app+(y*2+x)].size[pkg]/1000000000);
			else if(Games[app+(y*2+x)].size[pkg]>1000000)
				sprintf(dst,"%.2f Mb",1.*Games[app+(y*2+x)].size[pkg]/1000000);
			else if(Games[app+(y*2+x)].size[pkg]>1000)
				sprintf(dst,"%.2f Kb",1.*Games[app+(y*2+x)].size[pkg]/1000);
			else
				sprintf(dst,"%d Bytes",1.*Games[app+(y*2+x)].size[pkg]);
			override_text(&download_perc, "d_perc_text", dst);
		}
	}
	
}

static void user_menu_handler(void *data, Evas *e, Evas_Object *obj, void *event){
	Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*) event;
//	printf("press: %s               \n",ev->keyname);
	debugPrintf("press: %s\n",ev->keyname);
	if(draw_only)
		return;
		
//	if (strcmp (ev->keyname, "f") == 0)
//		ecore_evas_fullscreen_set (core.ee_core, !ecore_evas_fullscreen_get (core.ee_core));
/*
	if (strcmp (ev->keyname, "Select")==0){
		debugPrintf("\n\n\n\n\n\n\n\n\n");
		debugPrintf("is_downloading        %s\n",(is_downloading)? " VERO" : " FALSO");
		debugPrintf("is_asking_to_download %s\n",(is_asking_to_download)? " VERO" : " FALSO");
		debugPrintf("is_abort              %s\n",(is_abort)? " VERO" : " FALSO");
		debugPrintf("pkg_is_installed      %s\n",(pkg_is_installed)? " VERO" : " FALSO");
		debugPrintf("download_is_failed    %s\n",(download_is_failed)? " VERO" : " FALSO");
		debugPrintf("is_running            %s\n",(is_running)? " VERO" : " FALSO");
		debugPrintf("is_error              %s\n",(is_error)? " VERO" : " FALSO");
		debugPrintf("restart               %s\n",(restart)? " VERO" : " FALSO");
		debugPrintf("is_asking_exit_to_xmb %s\n",(is_asking_exit_to_xmb)? " VERO" : " FALSO");
		debugPrintf("is_option_menu        %s\n",(is_option_menu)? " VERO" : " FALSO");
		debugPrintf("is_sub_menu           %s\n",(is_sub_menu)? " VERO" : " FALSO");
		debugPrintf("skip_update           %s\n",(skip_update)? " VERO" : " FALSO");
		debugPrintf("draw_only             %s\n",(draw_only)? " VERO" : " FALSO");
		debugPrintf("submenu_reset         %s\n",(submenu_reset)? " VERO" : " FALSO");	
	}
	*/
	if (strcmp (ev->keyname, "Start") == 0){
		if(!is_downloading){
			ecore_main_loop_quit();
			psc_running = false;
		}
	}
	else if (strcmp (ev->keyname, "Circle") == 0 && !is_error){
		if(is_main_menu){
		}else if(is_sub_menu && is_downloading  && !is_asking_exit_to_xmb){
			is_downloading = false;
			is_asking_exit_to_xmb = false;
			is_abort = true;
			evas_object_resize(down, 0, Bar_Y_Val);
		}else if(is_asking_to_download && !is_downloading ){
			is_asking_to_download = false;
			is_asking_exit_to_xmb = false;
			is_downloading = false;
			is_abort = false;
			hide_edj(&download_ask);
			hide_edj(&download);
			hide_edj(&download_perc);
			hide_obj(down);
		}else if(is_asking_exit_to_xmb && !is_downloading){
			is_asking_to_download = false;
			is_asking_exit_to_xmb = false;
			is_downloading = false;
			is_abort = false;
			hide_edj(&download_ask);
			hide_edj(&download);
			hide_edj(&download_perc);
			hide_obj(down);
		}else if(is_option_menu && !is_asking_exit_to_xmb){
			is_main_menu = true;
			is_option_menu = false;
			hide_edj(&bar[0]);
			hide_edj(&bar[1]);
			show_edj(&menu[1+submenu_type]);
			show_edj(&menu[3]);
			show_edj(&bar[0]);
			app=0;
			category=0;
			x = y = 0;
		}else if(is_sub_menu && get_download_status()!=IS_DOWNLOADING && !is_asking_exit_to_xmb){
			is_sub_menu = false;
			is_main_menu = true;
			hide_edj(&bar[0]);
			hide_edj(&bar[1]);
			if(category!=5){
				for(int i=0;i<Apps[category].n_elem;i++)
					hide_obj(Applications[category].Image[i].obj);
			}else{
				for(int i=0;i<games_on_hdd;i++)
					hide_obj(Applications[category].Image[i].obj);
			}
			app = is_drawn = category = x = y = 0;
			show_edj(&menu[0]);
			show_edj(&menu[1+submenu_type]);
			show_edj(&menu[3]);
			show_edj(&bar[0]);
			override_text(&bar[0], "app_type_text", "");
			override_text(&bar[0], "app_version_text", "");
			Cursor_Y[0] = 0.25972*REAL_HEIGHT;
			Cursor_Y[1] = 0.5445 *REAL_HEIGHT;
	
			Cursor_X[0] = 0.1*REAL_WIDTH;		// APPS      (APP 1) || SYSTEM    (APP 4)
			Cursor_X[1] = 0.37421875*REAL_WIDTH;	// BMANAGERS (APP 2) || MEDIA     (APP 5)
			Cursor_X[2] = 0.65*REAL_WIDTH;		// GAMES     (APP 3) || OPTIONS   (APP 6)
		}else if(is_sub_menu && !is_error && !is_asking_exit_to_xmb){
			is_sub_menu = false;
			is_main_menu = true;
			hide_edj(&bar[0]);
			hide_edj(&bar[1]);
			if(category!=5){
				for(int i=0;i<Apps[category].n_elem;i++)
					hide_obj(Applications[category].Image[i].obj);
			}else{
				for(int i=0;i<games_on_hdd;i++)
					hide_obj(Applications[category].Image[i].obj);
			}
			app = is_drawn = category = x = y = 0;
			show_edj(&menu[0]);
			show_edj(&menu[1+submenu_type]);
			show_edj(&menu[3]);
			show_edj(&bar[0]);
			override_text(&bar[0], "app_type_text", "");
			override_text(&bar[0], "app_version_text", "");
		}
	}else if (strcmp (ev->keyname, "Right") == 0 && !is_asking_to_download && is_main_menu && !is_downloading && !is_asking_exit_to_xmb){
		x++;
		if(x>2)
			x = 0;
	}else if (strcmp (ev->keyname, "Left") == 0 && !is_asking_to_download && is_main_menu && !is_downloading && !is_asking_exit_to_xmb){
		x--;
		if(x<0)
			x = 2;
	}else if (strcmp (ev->keyname, "Left") == 0 && !is_asking_to_download && is_sub_menu && !is_downloading && !is_asking_exit_to_xmb){
		switch(submenu_type){
			case SUBMENU_0:
				x--;
				if(x<0)
					x = 2;
				break;
			case SUBMENU_1:
				if(x>1)
					x = 0;
				x=!x;
				break;
		}
	}else if (strcmp (ev->keyname, "Right") == 0 && !is_asking_to_download && is_sub_menu && !is_downloading && !is_asking_exit_to_xmb){
		switch(submenu_type){
			case SUBMENU_0:
				x++;
				if(x>2)
					x = 0;
				break;
			case SUBMENU_1:
				if(x>1)
					x = 1;
				x=!x;
				break;
		}
	}else if (strcmp (ev->keyname, "Left") == 0 && is_asking_to_download && !is_downloading && !is_asking_exit_to_xmb){ //choose pkg to download
		pkg--;
		if(pkg<0)
			switch(submenu_type){
				case SUBMENU_0:
					if(category!=5)
						pkg = Apps[category].pkg[app+(y*3+x)].pkg_num-1;
					else
						pkg = Games[app+(y*3+x)].pkgs-1;
					break;
				case SUBMENU_1:
					if(category!=5)
						pkg = Apps[category].pkg[app+(y*2+x)].pkg_num-1;
					else
						pkg = Games[app+(y*2+x)].pkgs-1;
					break;
			}
	}else if (strcmp (ev->keyname, "Right") == 0 && is_asking_to_download && !is_downloading && !is_asking_exit_to_xmb){ //choose pkg to download
		pkg++;
		switch(submenu_type){
			case SUBMENU_0:
				if(pkg > Games[app+(y*3+x)].pkgs-1 && category==5)
					pkg = 0;
				else if(pkg > Apps[category].pkg[app+(y*3+x)].pkg_num-1 && category!=5)
					pkg = 0;
				break;
			case SUBMENU_1:
				if(pkg > Games[app+(y*2+x)].pkgs-1 && category==5)
					pkg = 0;
				else if(pkg > Apps[category].pkg[app+(y*2+x)].pkg_num-1 && category!=5)
					pkg = 0;
				break;
		}
		
	}else if (strcmp (ev->keyname, "Up") == 0 && !is_asking_to_download && is_main_menu && !is_downloading && !is_asking_exit_to_xmb){
		y=!y;
	}else if (strcmp (ev->keyname, "Down") == 0 && !is_asking_to_download && is_main_menu && !is_downloading && !is_asking_exit_to_xmb){
		y=!y;
	}else if (strcmp (ev->keyname, "Up") == 0 && !is_asking_to_download && is_sub_menu && !is_downloading && !is_asking_exit_to_xmb){
		switch(submenu_type){
			case SUBMENU_0:
				if(y>1) y=1;
				if(!y)
					app-=6;
				y=!y;
				break;
			case SUBMENU_1:
				if(y==0)
					app-=6;
				y--;
				if(y<0)
					y=2;
				break;
		}

	}else if (strcmp (ev->keyname, "Down") == 0 && !is_asking_to_download && is_sub_menu && !is_downloading && !is_asking_exit_to_xmb){
		switch(submenu_type){
			case SUBMENU_0:
				if(y>1) y = 1;
				if(y)
					app+=6;
				y=!y;
				break;
			case SUBMENU_1:
				if(y==2)
					app+=6;
				y++;
				if(y>2)
					y=0;
				break;
		}

	}else if (strcmp (ev->keyname, "Cross") == 0 && !is_error && !is_asking_to_download){
		if(is_main_menu && !is_asking_exit_to_xmb){
			category = y*3+x;
			app = x = y = 0;
			is_drawn = 2;
			//--------------------------------------------------------------------------------------------------------------------------------------------
			if(category != 5){
				if(!Apps[category].downloaded){
					char tmp_char[250];
					sprintf(tmp_char, Lang[1].c_str(), Categories_Names[category].c_str());
					override_text(&menu[3], "error_text", tmp_char);
					Applications[category].Image = new EFL_Image [Apps[category].n_elem];
					Applications[category].x = new int [Apps[category].n_elem];
					Applications[category].y = new int [Apps[category].n_elem];
					draw_only = true;
					ecore_main_loop_iterate();
					BrewologyDownloadImages(&Apps[category], tmp_char, set_message);
					for(int j=0, k=0, h=0;j<Apps[category].n_elem;j++,(k>1) ? h=!h : h=h,(k>1) ? k=0 : k++ ){
						sprintf(tmp_char,"%s/%s",CACHE_PATH,Apps[category].img_file[j].c_str());
						//printf("%s\n",tmp_char);
						create_image_obj(&core, &Applications[category].Image[j], tmp_char, REAL_WIDTH*0.25, REAL_HEIGHT*0.246); //320x176 <- 144x80
						Applications[category].x[j] = Cursor_X[k];
						Applications[category].y[j] = Cursor_Y[h];
						draw_image(&Applications[category].Image[j], Applications[category].x[j], Applications[category].y[j]);
						evas_object_stack_below(Applications[category].Image[j].obj, bar[0].obj);
						hide_obj(Applications[category].Image[j].obj);
						submenu_reset = true;
					}
					draw_only = false;
					is_downloading = false;
					is_main_menu = false;
					//override_text(&menu[3], "error_text", Lang[9].c_str());
					override_text(&menu[3], "error_text", " ");
				}
			}else if(!game_updates_down && category == 5){
				DownloadUpdates();
			}
			//--------------------------------------------------------------------------------------------------------------------------------------------
			hide_edj(&menu[3]); //main menu
			is_sub_menu    = true;
			submenu_reset  = true;
			is_option_menu = false;
			is_main_menu   = false;
		}else if(is_asking_exit_to_xmb){
			is_option_menu = false;
			is_sub_menu = false;
			is_asking_exit_to_xmb = false;
			ecore_main_loop_quit();
		}else if(is_sub_menu && category!=5 && !is_asking_exit_to_xmb){
			if(app+(y*3+x) < Apps[category].n_elem && submenu_type == SUBMENU_0){
				pkg = 0;
				show_edj(&download);
				show_edj(&download_perc);
				show_obj(down);
				is_asking_to_download = true;
				is_asking_exit_to_xmb = false;
				is_downloading = false;
				is_abort = false;
				is_error = false;
			}else if(app+(y*2+x) < Apps[category].n_elem && submenu_type == SUBMENU_1){
				pkg = 0;
				show_edj(&download);
				show_edj(&download_perc);
				show_obj(down);
				is_asking_to_download = true;
				is_asking_exit_to_xmb = false;
				is_downloading = false;
				is_abort = false;
				is_error = false;
			}
		}else if(is_sub_menu && category==5 && !is_asking_exit_to_xmb){
			if(app+(y*3+x) < games_on_hdd && submenu_type == SUBMENU_0){
				pkg = 0;
				show_edj(&download);
				show_edj(&download_perc);
				show_obj(down);
				is_asking_to_download = true;
				is_asking_exit_to_xmb = false;
				is_downloading = false;
				is_abort = false;
				is_error = false;
			}else if(app+(y*2+x) < games_on_hdd && submenu_type == SUBMENU_1){
				pkg = 0;
				show_edj(&download);
				show_edj(&download_perc);
				show_obj(down);
				is_asking_to_download = true;
				is_asking_exit_to_xmb = false;
				is_downloading = false;
				is_abort = false;
				is_error = false;
			}
		}
	}else if (strcmp (ev->keyname, "Cross") == 0 && !is_downloading && !is_error && is_asking_to_download && category<5){
		if(submenu_type == SUBMENU_0){
			int ret = 7;
			show_obj(down);
			show_edj(&download);
			show_edj(&download_perc);
			download_status = 0;
			char dst[100];
			sprintf(dst,"%s/%s",CACHE_PATH,Apps[category].pkg[app+(y*3+x)].name[pkg].c_str());
			if(file_exist(dst))
				file_delete(dst);
			file_size = Apps[category].pkg[app+(y*3+x)].size[pkg];
			download_file(Apps[category].pkg[app+(y*3+x)].link[pkg].c_str(), dst, download_func_curl, download_is_finished);
		 	if(get_download_status()==DOWNLOAD_FAILED)
			 	override_text(&download_ask, "d_name_text", Lang[22].c_str());
			else
				override_text(&download_ask, "d_name_text", Lang[21].c_str());
		}else if(submenu_type == SUBMENU_1){
			int ret = 7;
			show_obj(down);
			show_edj(&download);
			show_edj(&download_perc);
			download_status = 0;
			char dst[100];
			sprintf(dst,"%s/%s",CACHE_PATH,Apps[category].pkg[app+(y*2+x)].name[pkg].c_str());
			if(file_exist(dst))
				file_delete(dst);
			file_size = Apps[category].pkg[app+(y*2+x)].size[pkg];
			download_file(Apps[category].pkg[app+(y*2+x)].link[pkg].c_str(), dst, download_func_curl, download_is_finished);
		 	if(get_download_status()==DOWNLOAD_FAILED)
			 	override_text(&download_ask, "d_name_text", Lang[22].c_str());
			else
				override_text(&download_ask, "d_name_text", Lang[21].c_str());
		}
	}else if (strcmp (ev->keyname, "Cross") == 0 && !is_downloading && !is_error && is_asking_to_download && category == 5){
		if(submenu_type == SUBMENU_0){
			int ret = 7;
			show_obj(down);
			show_edj(&download);
			show_edj(&download_perc);
			download_status = 0;
			char dst[100];
			sprintf(dst,"%s/update.pkg",CACHE_PATH);
			if(file_exist(dst))
				file_delete(dst);
			file_size = Games[app+(y*3+x)].size[pkg];
			download_file(Games[app+(y*3+x)].link[pkg].c_str(), dst, download_func_curl, download_is_finished);
		 	if(get_download_status()==DOWNLOAD_FAILED)
			 	override_text(&download_ask, "d_name_text", Lang[22].c_str());
			else
				override_text(&download_ask, "d_name_text", Lang[21].c_str());
		}else if(submenu_type == SUBMENU_1){
			int ret = 7;
			show_obj(down);
			show_edj(&download);
			show_edj(&download_perc);
			download_status = 0;
			char dst[100];
			sprintf(dst,"%s/update.pkg",CACHE_PATH);
			if(file_exist(dst))
				file_delete(dst);
			file_size = Games[app+(y*2+x)].size[pkg];
			download_file(Games[app+(y*2+x)].link[pkg].c_str(), dst, download_func_curl, download_is_finished);
		 	if(get_download_status()==DOWNLOAD_FAILED)
			 	override_text(&download_ask, "d_name_text", Lang[22].c_str());
			else
				override_text(&download_ask, "d_name_text", Lang[21].c_str());
		}
	}else if (strcmp (ev->keyname, "Square") == 0 && !is_option_menu && !is_downloading && !is_error && !is_asking_to_download && !is_asking_exit_to_xmb){ //Square
		if(is_sub_menu){
			submenu_reset = true;
			switch(submenu_type){
				case SUBMENU_0:
					submenu_type = SUBMENU_1;
					break;
				case SUBMENU_1:
					submenu_type = SUBMENU_0;
					break;
			}
		}else if (is_main_menu){ //Square
			is_option_menu = true;
			is_sub_menu = false;
			is_main_menu = false;
			hide_edj(&menu[1]); // sub menu
			hide_edj(&menu[2]); // sub menu
			hide_edj(&menu[3]); //main menu
			
		}
	}else if (strcmp (ev->keyname, "Triangle") == 0 && is_option_menu){ //Triangle
		++current_lang;
		current_lang = (current_lang<LANG_MAX)? current_lang : 0;
		char src[256];
		snprintf(src,256,PSC_THEME_DIR_PATH"/Translations/Lang_%s.txt",Lang_XX[current_lang]);
		file_copy(src, PSC_MAIN_MENU_LANG_CUSTOM_PATH);

		snprintf(src,256,PSC_THEME_DIR_PATH"/Translations/Categories_%s.txt",Lang_XX[current_lang]);
		file_copy(src, PSC_MAIN_MENU_CAT_CUSTOM_PATH);

		if(file_exist(PSC_MAIN_MENU_CAT_CUSTOM_PATH) && file_exist(PSC_MAIN_MENU_LANG_CUSTOM_PATH))
			SaveConfig(PSC_LANG_CONFIG, &current_lang);

		LoadStrings(PSC_MAIN_MENU_LANG_CUSTOM_PATH, Lang, LANG_STRINGS_MAX);
		LoadStrings(PSC_MAIN_MENU_CAT_CUSTOM_PATH, Categories_Names, 6);
		
		// Main Menu Override
		override_text(&menu[3], "square_text", Lang[10].c_str());
		override_text(&menu[3], "cross_text",  Lang[12].c_str());
		override_text(&menu[3], "circle_text", Lang[13].c_str());
		override_text(&menu[3], "arrows_text", Lang[14].c_str());
		override_text(&menu[3], "start_text",  Lang[15].c_str());

		//Sub Menu Override
		override_text(&menu[1], "square_text", Lang[11].c_str());
		override_text(&menu[1], "cross_text",  Lang[12].c_str());
		override_text(&menu[1], "circle_text", Lang[13].c_str());
		override_text(&menu[1], "arrows_text", Lang[14].c_str());
		override_text(&menu[1], "start_text",  Lang[15].c_str());
		override_text(&menu[2], "square_text", Lang[11].c_str());
		override_text(&menu[2], "cross_text",  Lang[12].c_str());
		override_text(&menu[2], "circle_text", Lang[13].c_str());
		override_text(&menu[2], "arrows_text", Lang[14].c_str());
		override_text(&menu[2], "start_text",  Lang[15].c_str());

		//Option Menu Override
		override_text(&menu[0], "circle_text",        Lang[13].c_str());
		override_text(&menu[0], "triangle_text",      Lang[24].c_str());
		override_text(&menu[0], "ps3_hdd_free_space", Lang[16].c_str());
		override_text(&menu[0], "ps3_user",           Lang[17].c_str());
		override_text(&menu[0], "theme_name",         Lang[18].c_str());
	
		//Download Mask Override

		override_text(&download, "d_cross_text",      Lang[19].c_str()); //Download
		override_text(&download, "d_circle_text",     Lang[13].c_str()); //Back
		override_text(&download_ask, "d_cross_text",  Lang[20].c_str()); //Exit to XMB
		override_text(&download_ask, "d_circle_text", Lang[13].c_str()); //Back
		override_text(&download_ask, "d_name_text",   Lang[22].c_str()); //Download Completed!
		override_text(&download_ask, "d_name_text2",  Lang[23].c_str()); //Download Failed!
		
		draw_only = true;
		ecore_main_loop_iterate();
		draw_only = false;
	}
	
	

	if(app<0)
		app=0;

	if(is_main_menu){
		override_text(&bar[0], "menu_text", Categories_Names[y*3+x].c_str());
		override_text(&bar[0], "app_version_text"," ");
		override_text(&bar[0], "app_type_text", " ");
		move_edj(&bar[0], Cursor_X[x], Cursor_Y[y]);
	}else if(is_sub_menu){
		draw_submenu();
	}
	if(is_option_menu){
		hide_edj(&bar[0]);
		hide_edj(&bar[1]);
	}else if(is_main_menu){
		hide_edj(&bar[1]);
		show_edj(&bar[0]);
	}


}
 
static Eina_Bool on_exit_handler(void *data, int type, void *event){
	ecore_main_loop_quit();
	psc_running = false;
	return 1;
}

 
static Eina_Bool on_ps_btn_handler(void *data, int type, void *event){
	return 1;
}

int main(int argc, char *argv[]){
	/*
	 * Here i need to get the TV resolution
	 */
	ecore_psl1ght_optimal_screen_resolution_get(&REAL_WIDTH, &REAL_HEIGHT);
	if(REAL_HEIGHT > 720){
		REAL_WIDTH  = 1280;
		REAL_HEIGHT = 720;
	}
	
	init_core(&core, REAL_WIDTH, REAL_HEIGHT, "Playstation Channel");
	edje_start();
	init_core_file();
	debugInit();
	
	
	if(file_exist(PSC_LANG_CONFIG))
		LoadConfig(PSC_LANG_CONFIG, &current_lang);
	if(file_exist(PSC_MAIN_MENU_CAT_CUSTOM_PATH))
		LoadStrings(PSC_MAIN_MENU_CAT_CUSTOM_PATH, Categories_Names, 6);
	else
		LoadStrings(PSC_MAIN_MENU_CATEGORIES_PATH, Categories_Names, 6);

	if(file_exist(PSC_MAIN_MENU_LANG_CUSTOM_PATH))
		LoadStrings(PSC_MAIN_MENU_LANG_CUSTOM_PATH, Lang, 31);
	else
		LoadStrings(PSC_MAIN_MENU_LANG_PATH, Lang, 31);
	
	if(dir_exist(PSC_CUSTOM_PATH))
		strcpy(THEME_DIR, PSC_CUSTOM_PATH);
	else
		strcpy(THEME_DIR, PSC_ORIGINAL_PATH);
	
	
	Cursor_Y[0] = 0.25972*REAL_HEIGHT;
	Cursor_Y[1] = 0.5445 *REAL_HEIGHT;
	
	Cursor_X[0] = 0.1*REAL_WIDTH;		// APPS      (APP 1) || SYSTEM    (APP 4)
	Cursor_X[1] = 0.37421875*REAL_WIDTH;	// BMANAGERS (APP 2) || MEDIA     (APP 5)
	Cursor_X[2] = 0.65*REAL_WIDTH;		// GAMES     (APP 3) || OPTIONS   (APP 6)

	string THEME_TMP;
	
	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_INFO_SCREEN_PATH;
	load_edj_from_file(&core, &info, THEME_TMP.c_str(), "information", REAL_WIDTH, REAL_HEIGHT);
	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_OPTION_MENU_PATH;
	load_edj_from_file(&core, &menu[0], THEME_TMP.c_str(), "menu", REAL_WIDTH, REAL_HEIGHT); //OPTIONS
	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_SUB_MENU_PATH;
	load_edj_from_file(&core, &menu[1], THEME_TMP.c_str(), "menu", REAL_WIDTH, REAL_HEIGHT);    //SUB MENU
	load_edj_from_file(&core, &menu[2], THEME_TMP.c_str(), "menu2", REAL_WIDTH, REAL_HEIGHT);    //SUB MENU

	main_layer = evas_object_rectangle_add(get_main_object(&core));
	evas_object_resize(main_layer, REAL_WIDTH, REAL_HEIGHT);
	evas_object_color_set(main_layer, 0, 0, 0, 0);
	evas_object_show(main_layer);

	ecore_evas_object_associate(core.ee_core, main_layer, ECORE_EVAS_OBJECT_ASSOCIATE_BASE);
	override_text(&info, "information_download", " ");
	draw_edj(&info, 0, 0);
	ecore_main_loop_iterate();
//-----------------------------------------------------------------------------------------------------
	dir_make(CACHE_PATH);
	{
		
		string XML_files[5];
		{
			char url[255];
#ifdef PSCHANNEL_VERSION
			
			download_file_txt = "Syncing with Brewology.com ..";
			sprintf(url,"www.your.website.com/new&v=%.2f",PSCHANNEL_VERSION);
			download_file(url,CACHE_PATH"/new_encrypted.xml", download_main_func_curl, download_main_is_finished);
			if(download_is_failed) goto end;
			download_file_txt = Lang[0];
			download_file_txt += " 0%";
			sprintf(url,"www.your.website.com/apps&v=%.2f",PSCHANNEL_VERSION);
			download_file(url,CACHE_PATH"/apps_encrypted.xml", download_main_func_curl, download_main_is_finished);
			if(download_is_failed) goto end;
			download_file_txt = Lang[0];
			download_file_txt += " 25%";
			sprintf(url,"www.your.website.com/games&v=%.2f",PSCHANNEL_VERSION);
			download_file(url,CACHE_PATH"/games_encrypted.xml", download_main_func_curl, download_main_is_finished);
			if(download_is_failed) goto end;
			download_file_txt = Lang[0];
			download_file_txt += " 50%";
			sprintf(url,"www.your.website.com/dev&v=%.2f",PSCHANNEL_VERSION);
			download_file(url,CACHE_PATH"/dev_encrypted.xml", download_main_func_curl, download_main_is_finished);
			if(download_is_failed) goto end;
			download_file_txt = Lang[0];
			download_file_txt += " 75%";
			sprintf(url,"www.your.website.com/emulators&v=%.2f",PSCHANNEL_VERSION);
			download_file(url,CACHE_PATH"/emulators_encrypted.xml", download_main_func_curl, download_main_is_finished);
			if(download_is_failed) goto end;
#endif
			XML_files[0] = "new.xml";
			XML_files[1] = "apps.xml";
			XML_files[2] = "games.xml";
			XML_files[3] = "dev.xml";
			XML_files[4] = "emulators.xml";

			decrypt_xml(CACHE_PATH"/new_encrypted.xml",       CACHE_PATH"/new.xml");
			decrypt_xml(CACHE_PATH"/apps_encrypted.xml",      CACHE_PATH"/apps.xml");
			decrypt_xml(CACHE_PATH"/games_encrypted.xml",     CACHE_PATH"/games.xml");
			decrypt_xml(CACHE_PATH"/dev_encrypted.xml",       CACHE_PATH"/dev.xml");
			decrypt_xml(CACHE_PATH"/emulators_encrypted.xml", CACHE_PATH"/emulators.xml");

			if(get_download_status()==DOWNLOAD_FAILED)
				goto end;
			
			download_file_txt =  "Setting Language to ";
			download_file_txt += Language[current_lang];
			download_file_txt += " and checking HDD";
			override_text(&info, "information_download", download_file_txt.c_str());
			draw_only = true;
			ecore_main_loop_iterate();
			draw_only = false;
			check_hdd("/dev_hdd0/game/");
		}
			for(int i=0;i<5;i++){
				char tmp_char[250];
				sprintf(tmp_char,Lang[8].c_str(),i*20);
				override_text(&info, "information_download", tmp_char);
				ecore_main_loop_iterate();
				sprintf(tmp_char,"%s/%s",CACHE_PATH,XML_files[i].c_str());
				if(BrewologyLoadStringsFromXML(tmp_char, &Apps[i])==PSC_OLDVERSION){
					skip_update = false;
					goto update;
				}else
					skip_update = true;
			}
		draw_only = true;
		ecore_main_loop_iterate();
		draw_only = false;
	}

//-----------------------------------------------------------------------------------------------------

	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_MAIN_MENU_PATH;
	load_edj_from_file(&core, &menu[3], THEME_TMP.c_str(), "menu", REAL_WIDTH, REAL_HEIGHT);   //MAIN MENU
	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_MAIN_MENU_BAR_PATH;
	load_edj_from_file(&core, &bar[0], THEME_TMP.c_str(), "main_menu_bar", REAL_WIDTH*0.25, REAL_HEIGHT*0.246); //(Main/Sub) Menu bar
	load_edj_from_file(&core, &bar[1], THEME_TMP.c_str(), "sub_menu_bar2", REAL_WIDTH*0.36484375, REAL_HEIGHT*0.111111111); //(Sub) Menu bar
	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_DOWNLOAD_MASK_PATH;
	load_edj_from_file(&core, &download, THEME_TMP.c_str(), "download_mask", REAL_WIDTH*0.3125, REAL_HEIGHT*0.3055);     //DOWNLOAD MENU

	override_text(&menu[3], "error_text", "");

	down = evas_object_rectangle_add(get_main_object(&core));
	evas_object_move(down, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2 +REAL_WIDTH*0.03125,REAL_HEIGHT*0.08861 +(REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
	Bar_X_Val = REAL_WIDTH*0.25;
	Bar_Y_Val = REAL_HEIGHT*0.058;
	evas_object_resize(down, 0, Bar_Y_Val);
	evas_object_color_set(down, 135, 173, 252, 255); //blue
	hide_obj(down);
	
	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_DOWNLOADPERC_MASK_PATH;
	load_edj_from_file(&core, &download_perc, THEME_TMP.c_str(), "download_perc", REAL_WIDTH*0.3125, REAL_HEIGHT*0.3055);		//DOWNLOAD MENU
	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_DOWNLOAD_MASK_PATH;
	load_edj_from_file(&core, &download_ask, THEME_TMP.c_str(), "download_mask_ask", REAL_WIDTH*0.3125, REAL_HEIGHT*0.3055);	//ASK to download
	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_ERROR_MASK_PATH;
	load_edj_from_file(&core, &error_dialog, THEME_TMP.c_str(), "error_message", REAL_WIDTH*0.3125, REAL_HEIGHT*0.3055);		//Error Message

	override_text(&info, "information_download", Lang[9].c_str());
	ecore_main_loop_iterate();

	ecore_event_handler_add(ECORE_PSL1GHT_EVENT_QUIT, on_exit_handler, NULL);
	ecore_event_handler_add(ECORE_PSL1GHT_EVENT_LOST_FOCUS, on_ps_btn_handler, NULL);
	ecore_event_handler_add(ECORE_PSL1GHT_EVENT_GOT_FOCUS, on_ps_btn_handler, NULL);
	ecore_event_handler_add(ECORE_PSL1GHT_EVENT_EXPOSE, on_ps_btn_handler, NULL);
	
	information_timer = ecore_timer_add(PSC_INFO_SCREEN_TIMER, delete_info, NULL);
	enable_callback(&core, main_layer, EVAS_CALLBACK_KEY_DOWN , user_menu_handler);
	
	override_text(&menu[0], "ps3_version_edit", get_ps3_version());
	char space [100];
	if(get_ps3_free_space()>1000000000)
		sprintf(space,"%ld Gigabytes",get_ps3_free_space()/1000000000);
	else if(get_ps3_free_space()>1000000)
		sprintf(space,"%ld Megabytes",get_ps3_free_space()/1000000);
	else if(get_ps3_free_space()>1000)
		sprintf(space,"%ld Kilobytes",get_ps3_free_space()/1000);
	else
		sprintf(space,"%ld Bytes",get_ps3_free_space());
	override_text(&menu[0], "ps3_hdd_free_space_edit", space);
#ifdef PSCHANNEL_VERSION
	sprintf(space,"%f",PSCHANNEL_VERSION);
	override_text(&menu[0], "software_version", space);
#else
	override_text(&menu[0], "software_version", "No version set");
#endif


// Main Menu Override
	override_text(&menu[3], "square_text", Lang[10].c_str());
	override_text(&menu[3], "cross_text",  Lang[12].c_str());
	override_text(&menu[3], "circle_text", Lang[13].c_str());
	override_text(&menu[3], "arrows_text", Lang[14].c_str());
	override_text(&menu[3], "start_text",  Lang[15].c_str());
//Sub Menu Override
	override_text(&menu[1], "square_text", Lang[11].c_str());
	override_text(&menu[1], "cross_text",  Lang[12].c_str());
	override_text(&menu[1], "circle_text", Lang[13].c_str());
	override_text(&menu[1], "arrows_text", Lang[14].c_str());
	override_text(&menu[1], "start_text",  Lang[15].c_str());
	override_text(&menu[2], "square_text", Lang[11].c_str());
	override_text(&menu[2], "cross_text",  Lang[12].c_str());
	override_text(&menu[2], "circle_text", Lang[13].c_str());
	override_text(&menu[2], "arrows_text", Lang[14].c_str());
	override_text(&menu[2], "start_text",  Lang[15].c_str());
//Option Menu Override
	override_text(&menu[0], "circle_text",        Lang[13].c_str());
	override_text(&menu[0], "triangle_text",      Lang[24].c_str());
	override_text(&menu[0], "ps3_hdd_free_space", Lang[16].c_str());
	override_text(&menu[0], "ps3_user",           Lang[17].c_str());
	override_text(&menu[0], "theme_name",         Lang[18].c_str());
	
//Download Mask Override

	override_text(&download, "d_cross_text",      Lang[19].c_str()); //Download
	override_text(&download, "d_circle_text",     Lang[13].c_str()); //Back
	override_text(&download_ask, "d_cross_text",  Lang[20].c_str()); //Exit to XMB
	override_text(&download_ask, "d_circle_text", Lang[13].c_str()); //Back
	override_text(&download_ask, "d_name_text",   Lang[22].c_str()); //Download Completed!
	override_text(&download_ask, "d_name_text2",  Lang[23].c_str()); //Download Failed!

	xmb = false;
	//gcmSetFlipHandler(flip);

	if(skip_update)
		goto main_loop;

update:
	delete_edj(&info);
	enable_callback(&core, main_layer, EVAS_CALLBACK_KEY_DOWN , user_menu_handler);

	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_ERROR_MASK_PATH;
	load_edj_from_file(&core, &info, THEME_TMP.c_str(), "update_screen", REAL_WIDTH, REAL_HEIGHT);
	draw_edj(&info, 0,0);
	THEME_TMP = THEME_DIR+(string)"/"+(string)PSC_ERROR_MASK_PATH;
	load_edj_from_file(&core, &error_dialog, THEME_TMP.c_str(), "update_message", REAL_WIDTH*0.3125, REAL_HEIGHT*0.3055);
	override_text(&error_dialog, "error_text", get_update_message());
	override_text(&error_dialog, "downloading_text", " ");
	draw_edj(&error_dialog, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2, (REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
	show_edj(&error_dialog);
	ecore_main_loop_iterate();
	ecore_main_loop_iterate();
	sleep(2);
	download_file("www.your.website.com/latest/pschannel.pkg", CACHE_PATH"/package.pkg", download_update_curl, NULL);
	Create_Bubble(CACHE_PATH"/package.pkg", "PSChannel Latest Update");
	file_delete(PSC_MAIN_MENU_CATEGORIES_PATH);
	file_delete(PSC_MAIN_MENU_LANG_PATH);
	dir_recursive_delete(CACHE_PATH"/..");
	pkg_is_installed = true;
	goto end;

main_loop:
	psc_running = true;
	while(psc_running){
		ecore_psl1ght_poll_events();
		ecore_main_loop_iterate();
	}
	disable_callback(main_layer,EVAS_CALLBACK_KEY_DOWN, user_menu_handler);
	
end:
	dir_recursive_delete(CACHE_PATH);
	if(skip_update){
		for(int i=0;i<3;i++)
			delete_edj(&menu[i]);
		for(int i=0;i<5;i++){
			for(int j=0;j<Apps[i].n_elem && Apps[i].downloaded;j++)
				delete_obj(Applications[i].Image[j].obj);
			FreePSCApplication(&Apps[i]);
		}
		delete_edj(&bar[0]);
		delete_edj(&bar[1]);
		delete_edj(&download);
		delete_edj(&download_ask);
		delete_edj(&download_perc);
		delete_edj(&error_dialog);
		delete_obj(down);
		delete [] Games;
	}

	edje_stop();
	stop_core_file();
	ecore_psl1ght_shutdown();

	if(pkg_is_installed /*&& atof(get_ps3_version())<3.56 */)
		reboot_sys();
	

	return 0;
}
