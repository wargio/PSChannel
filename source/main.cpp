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

#include "core.h"
#include "font.h"
#include "image.h"
#include "edje.h"
#include "utils.h"
#include "pkgbubble.h"
#include "application.h"
#include "core_download.h"
#include "unzip.h"

#include "url.h" //while ecore_download is not working

#include <stdlib.h>
#include <Ecore_Psl1ght.h>
#include <sysmodule/sysmodule.h>
/*
 *  PSC Settings
 */

#define PSC_INFO_SCREEN_TIMER		(0.50)
static int REAL_WIDTH  = 1280;
static int REAL_HEIGHT = 720;

/*
 *  PSC Files
 */

#define PSC_MAIN_MENU_LANG_PATH		"/dev_hdd0/game/PSCHANNEL/Lang.txt"
#define CACHE_PATH			"/dev_hdd0/game/PSCHANNEL/CACHE"
#define MAINURL				"http://your_website/files.zip"

/*
 *  EDJ Files!
 */
 
#define PSC_INFO_SCREEN_PATH		"/dev_hdd0/game/PSCHANNEL/theme/info_screen.edj"
#define PSC_MAIN_MENU_BAR_PATH		"/dev_hdd0/game/PSCHANNEL/theme/main_menu_bar.edj"
#define PSC_MAIN_MENU_PATH		"/dev_hdd0/game/PSCHANNEL/theme/main_menu.edj"
#define PSC_SUB_MENU_PATH		"/dev_hdd0/game/PSCHANNEL/theme/sub_menu.edj"
#define PSC_OPTION_MENU_PATH		"/dev_hdd0/game/PSCHANNEL/theme/option_menu.edj"
#define PSC_DOWNLOAD_MASK_PATH		"/dev_hdd0/game/PSCHANNEL/theme/download.edj"
#define PSC_DOWNLOADPERC_MASK_PATH	"/dev_hdd0/game/PSCHANNEL/theme/download_perc.edj"

static Ecore_Timer *information_timer;
static EFL_Core core;

Evas_Object *main_layer;

static EFL_Edj info;
static EFL_Edj menu[3];
static PSC_App Applications[5];
static EFL_Edj bar;
static EFL_Edj download;
static EFL_Edj download_ask;
static EFL_Edj download_perc;
static Evas_Object *down;

static uint16_t Cursor_X[3],Cursor_Y[2];
static uint16_t Type_X=0,Type_Y=0;
static uint16_t Bar_X_Val = 0, Bar_Y_Val = 0;

static PSC_Application Apps[5];
static string Categories_Names[6];

static int x = 0;
static int y = 0;

static int category = 0;

static bool is_main_menu = true;
static int is_drawn = 1;

static int app=0;
static bool is_download = false;
static bool is_downloading = false;
static bool is_abort = false;
static bool pkg_is_installed = false;
static bool download_is_failed = false;
static bool is_running = false;
static bool is_asking_exit_to_xmb = false;
static int download_status = 0;

void download_main_is_finished(void *data, const char *file, int status){
	is_downloading = false;
	is_running = false;
	if(status != 200){
		override_text(&info, "information_download", "No internet connection");
		download_is_failed = true;
		ecore_main_loop_iterate();
		sleep(2);
		return ;
	}
	override_text(&info, "information_download", "Extracting..");
	ecore_main_loop_iterate();
	sleep(1);
	extract_zip(file,CACHE_PATH);
	file_delete(file);
	override_text(&info, "information_download", "Loading menu..");
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
	char perc[26];
	if(dltotal!=0){
		sprintf(perc,"Downloading.. %.2f %%", dlnow*100/dltotal);
		override_text(&info, "information_download", perc);
	}else
	if(is_abort){
		is_abort = false;
		return -1;
	}
	ecore_main_loop_iterate();
	return 0;
}
void download_is_finished(void *data, const char *file, int status){
	printf("\nDone (status: %d)\n", status);
	download_status = status;
	if(status == 200 && !is_abort){
		override_text(&download_perc, "d_perc_text", "Installing..");
		ecore_main_loop_iterate();
		switch(Apps[category].type_file[app+(y*3+x)]){
			case PSC_PACKAGE:
				Create_Bubble(file, Apps[category].name[app+(y*3+x)].c_str());
				pkg_is_installed = true;
				break;
			case PSC_THEME:
				Copy_Theme(file, Apps[category].name[app+(y*3+x)].c_str());
				break;
			case PSC_IMAGE:
			// /dev_hdd0/photo
				break;
			case PSC_VIDEO:
				break;
			default:
				break;
		}
	}
	override_text(&download_perc, "d_perc_text", "Deleting cache..");
	ecore_main_loop_iterate();
	file_delete(file);
	sleep(1);
	is_downloading = false;
 	hide_obj(down);
 	hide_edj(&download_perc);
	hide_edj(&download);
	is_download = false;
	if(!is_abort){
		is_asking_exit_to_xmb = true;
		show_edj(&download_ask);
	}else
		is_abort = false;
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

int download_func_curl(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow){
	is_downloading = true;
	char perc[10];
	if(dltotal!=0){
		sprintf(perc,"%.2f %%", dlnow*100/dltotal);
		override_text(&download_perc, "d_perc_text", perc);
		evas_object_resize(down, Bar_X_Val*dlnow/dltotal, Bar_Y_Val);
	}else if(get_download_wr_status()){
		sprintf(perc,"Downloading..");
		override_text(&download_perc, "d_perc_text", perc);
	}
	if(is_abort){
		return -1;
	}
	ecore_main_loop_iterate();
	return 0;
}

Eina_Bool delete_info(void *data){
	if(!is_downloading){
		ecore_timer_del(information_timer);
		delete_edj(&info);
		draw_edj(&menu[0], 0, 0);
		draw_edj(&menu[1], 0, 0);
		draw_edj(&menu[2], 0, 0);

		override_text(&bar, "menu_text", Categories_Names[y*3+x].c_str());
		override_text(&bar, "app_type_text", "");
		draw_edj(&bar, Cursor_X[x], Cursor_Y[y]);
		draw_edj(&download, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2, (REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
		draw_edj(&download_perc, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2, (REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
		draw_edj(&download_ask, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2, (REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
		hide_edj(&download);
		hide_edj(&download_ask);
		hide_edj(&download_perc);
	}	
	return EINA_TRUE;
}

static void user_menu_handler(void *data, Evas *e, Evas_Object *obj, void *event){
	Evas_Event_Key_Down *ev = (Evas_Event_Key_Down*) event;

//	if (strcmp (ev->keyname, "f") == 0)
//		ecore_evas_fullscreen_set (core.ee_core, !ecore_evas_fullscreen_get (core.ee_core));
//	else


	if (strcmp (ev->keyname, "Start") == 0){
		if(!is_download && !is_downloading && get_download_status()!=IS_DOWNLOADING)
			ecore_main_loop_quit();
	}else if (strcmp (ev->keyname, "Circle") == 0){
		if(is_main_menu);
		else if(is_asking_exit_to_xmb){
			is_asking_exit_to_xmb = false;
			is_abort = false;
			hide_edj(&download_ask);
		}else if(is_download){
			is_download = false;
			hide_obj(down);
			hide_edj(&download_perc);
			hide_edj(&download);
		}else if(is_downloading){
			is_abort = true;
	//		ecore_file_download_abort_all();
		}else if(!is_downloading && get_download_status()!=IS_DOWNLOADING){
			for(int i=0;i<Apps[category].n_elem && category !=5;i++)
				hide_obj(Applications[category].Image[i].obj);
			is_main_menu = true;
			is_drawn = category = x = y = 0;
			show_edj(&menu[0]);
			show_edj(&menu[1]);
			show_edj(&menu[2]);
			show_edj(&bar);
			app=0;
			override_text(&bar, "app_type_text", "");
		}else{
			is_downloading = false;
			is_download = false;
			evas_object_resize(down, 1, Bar_Y_Val);
		}
	}else if (strcmp (ev->keyname, "Left") == 0 && !is_download) 
		x--;
	else if (strcmp (ev->keyname, "Right") == 0 && !is_download)
		x++;
	else if (strcmp (ev->keyname, "Up") == 0 && !is_download){
		if(!y)
			app-=6;
		y=!y;
	}else if (strcmp (ev->keyname, "Down") == 0 && !is_download){
		if(y)
			app+=6;
		y=!y;
	}else if (strcmp (ev->keyname, "Cross") == 0 && !is_download){
		if(is_main_menu){
			category = y*3+x;
			app = x = y = 0;
			is_drawn = 2;
			hide_edj(&menu[2]); //main menu
			if(category==5)
				hide_edj(&menu[1]); // sub menu
		}else if(is_asking_exit_to_xmb){
			is_asking_exit_to_xmb = false;
			ecore_main_loop_quit();
		}else if(app+(y*3+x) < Apps[category].n_elem){
			override_text(&download, "d_name_text", Apps[category].name[app+(y*3+x)].c_str());
			override_text(&download_perc, "d_perc_text", " ");
			show_edj(&download);
			show_edj(&download_perc);
			show_obj(down);
			is_download = true;
		}
		is_main_menu = false;
	}else if (strcmp (ev->keyname, "Cross") == 0 && is_download && !is_downloading){
		override_text(&download, "d_name_text", Apps[category].name[app+(y*3+x)].c_str());
		int ret = 7;
		show_obj(down);
		show_edj(&download);
		show_edj(&download_perc);
		download_status = 0;
		char dst[100];
		sprintf(dst,"%s/%s",CACHE_PATH,Apps[category].pkg_file[app+(y*3+x)].c_str());
		if(file_exist(dst))
			file_delete(dst);
		download_file(Apps[category].link[app+(y*3+x)].c_str(), dst, download_func_curl, download_is_finished);
//		ecore_file_download(Apps[category].link[app+(y*3+x)].c_str(), dst, download_is_finished, download_func, NULL, NULL);
	 	
	}
	
	if(x<0)
		x = 2;
	else if(x>2)
		x = 0;
	if(app<0)
		app=0;

	if(category != 5){
		if(is_main_menu)
			override_text(&bar, "menu_text", Categories_Names[y*3+x].c_str());
		else{
			if(app > Apps[category].n_elem-1)
				app -=6;
			override_text(&bar, "menu_text", (app+(y*3+x) < Apps[category].n_elem) ? (Apps[category].name[app+(y*3+x)].c_str()) : "" );
			switch(Apps[category].type_file[app+(y*3+x)]){
			 		case PSC_PACKAGE:
						override_text(&bar, "app_type_text", "Package");
			 			break;
			 		case PSC_THEME:
						override_text(&bar, "app_type_text", "Theme");
			 			break;
			 		case PSC_IMAGE:
						override_text(&bar, "app_type_text", "Image");
			 			break;
			 		case PSC_VIDEO:
						override_text(&bar, "app_type_text", "Video");
			 			break;
			 		case PSC_NO_TYPE:
						override_text(&bar, "app_type_text", " ");
			 			break;
			 		default:
						override_text(&bar, "app_type_text", " ");
			 			break;
			}
			for(int i=0;i<Apps[category].n_elem;i++)
				hide_obj(Applications[category].Image[i].obj);
			for(int i=0;i<6 && i+app < Apps[category].n_elem;i++)
				show_obj(Applications[category].Image[app+i].obj);
		}
		move_edj(&bar, Cursor_X[x], Cursor_Y[y]);
	}else{
		hide_edj(&bar);
	}
}
 
static Eina_Bool on_exit_handler(void *data, int type, void *event){
	ecore_main_loop_quit();
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
	
	Cursor_Y[0] = 0.25972*REAL_HEIGHT;
	Cursor_Y[1] = 0.5445 *REAL_HEIGHT;
	
	Cursor_X[0] = 0.1*REAL_WIDTH;		// APPS      (APP 1) || MEDIA     (APP 4)
	Cursor_X[1] = 0.37421875*REAL_WIDTH;	// BMANAGERS (APP 2) || RETRO     (APP 5)
	Cursor_X[2] = 0.65*REAL_WIDTH;		// GAMES     (APP 3) || OPTIONS   (APP 6)


	
	load_edj_from_file(&core, &info, PSC_INFO_SCREEN_PATH, "information", REAL_WIDTH, REAL_HEIGHT);
	load_edj_from_file(&core, &menu[0], PSC_OPTION_MENU_PATH, "menu", REAL_WIDTH, REAL_HEIGHT); //OPTIONS
	load_edj_from_file(&core, &menu[1], PSC_SUB_MENU_PATH, "menu", REAL_WIDTH, REAL_HEIGHT);    //SUB MENU

	main_layer = evas_object_rectangle_add(get_main_object(&core));
	evas_object_resize(main_layer, REAL_WIDTH, REAL_HEIGHT);
	evas_object_color_set(main_layer, 0, 0, 0, 0);
	evas_object_show(main_layer);

	ecore_evas_object_associate(core.ee_core, main_layer, ECORE_EVAS_OBJECT_ASSOCIATE_BASE);
	override_text(&info, "information_download", " ");
	draw_edj(&info, 0, 0);

//-----------------------------------------------------------------------------------------------------

	dir_make(CACHE_PATH);
	if(file_exist(CACHE_PATH"/files.zip"))
		file_delete(CACHE_PATH"/files.zip");

	{
		char url[20];
		sprintf(url,"%s",MAINURL);
//		if(!ecore_file_download(url,CACHE_PATH"/files.zip", download_main_is_finished, download_main_func, NULL, NULL))
		download_file(url,CACHE_PATH"/files.zip", download_main_func_curl, download_main_is_finished);
		if(get_download_status()==DOWNLOAD_FAILED)
			goto end;
	}
	LoadStrings(PSC_MAIN_MENU_LANG_PATH, Categories_Names, 6);
	{
		string XML_files[5];
		LoadMainStringsFromXML(CACHE_PATH"/files.xml",XML_files);
		for(int i=0;i<5;i++){
			char tmp_char[250];
			sprintf(tmp_char,"%s/%s",CACHE_PATH,XML_files[i].c_str());
			LoadStringsFromXML(tmp_char,&Apps[i]);
			Applications[i].Image = new EFL_Image [Apps[i].n_elem];
			Applications[i].x = new int [Apps[i].n_elem];
			Applications[i].y = new int [Apps[i].n_elem];
			for(int j=0, k=0, h=0;j<Apps[i].n_elem;j++,(k>1) ? h=!h : h=h,(k>1) ? k=0 : k++ ){
				sprintf(tmp_char,"%s/%s",CACHE_PATH,Apps[i].img_file[j].c_str());
				create_image_obj(&core, &Applications[i].Image[j], tmp_char, REAL_WIDTH*0.25, REAL_HEIGHT*0.246);
				Applications[i].x[j] = Cursor_X[k];
				Applications[i].y[j] = Cursor_Y[h];
				draw_image(&Applications[i].Image[j], Applications[i].x[j], Applications[i].y[j]);
				hide_obj(Applications[i].Image[j].obj);
			}
		}
	}
//-----------------------------------------------------------------------------------------------------
	load_edj_from_file(&core, &menu[2], PSC_MAIN_MENU_PATH, "menu", REAL_WIDTH, REAL_HEIGHT);                                 //MAIN MENU
	load_edj_from_file(&core, &bar, PSC_MAIN_MENU_BAR_PATH, "main_menu_bar", REAL_WIDTH*0.25, REAL_HEIGHT*0.246);             //(Main/Sub) Menu bar
	load_edj_from_file(&core, &download, PSC_DOWNLOAD_MASK_PATH, "download_mask", REAL_WIDTH*0.3125, REAL_HEIGHT*0.3055);     //DOWNLOAD MENU

	override_text(&menu[2], "error_text", "");

	down = evas_object_rectangle_add(get_main_object(&core));
	evas_object_move(down, (REAL_WIDTH-(REAL_WIDTH*0.3125))/2 +REAL_WIDTH*0.03125,REAL_HEIGHT*0.08861 +(REAL_HEIGHT-(REAL_HEIGHT*0.3055))/2);
	Bar_X_Val = REAL_WIDTH*0.25;
	Bar_Y_Val = REAL_HEIGHT*0.058;
	evas_object_resize(down, 1, Bar_Y_Val);
	evas_object_color_set(down, 0, 0, 255, 255); //blue
	hide_obj(down);
	load_edj_from_file(&core, &download_perc, PSC_DOWNLOADPERC_MASK_PATH, "download_perc", REAL_WIDTH*0.3125, REAL_HEIGHT*0.3055);    //DOWNLOAD MENU
	load_edj_from_file(&core, &download_ask, PSC_DOWNLOAD_MASK_PATH, "download_mask_ask", REAL_WIDTH*0.3125, REAL_HEIGHT*0.3055);     //DOWNLOAD MENU ASK XMB

	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, on_exit_handler, NULL);
	information_timer = ecore_timer_add(PSC_INFO_SCREEN_TIMER, delete_info, NULL);
	enable_callback(&core, main_layer, EVAS_CALLBACK_KEY_DOWN , user_menu_handler);
	
	override_text(&menu[0], "ps3_version_edit", get_ps3_version());
	char space [100];
	sprintf(space,"%ld/%ld Bytes",get_ps3_free_space(),get_ps3_total_space());
	override_text(&menu[0], "ps3_hdd_free_space_edit", space);
#ifdef PSCHANNEL_VERSION
	sprintf(space,"%f",PSCHANNEL_VERSION);
	override_text(&menu[0], "software_version", space);
#else
	override_text(&menu[0], "software_version", " ");
#endif
	if(sysModuleIsLoaded(SYSMODULE_NET))
		override_text(&menu[2], "error_text", "SYSMODULE_NET NOT LOADED!!");



	start_core_loop();
	disable_callback(main_layer,EVAS_CALLBACK_KEY_DOWN, user_menu_handler);
	
end:
	dir_recursive_delete(CACHE_PATH);

	for(int i=0;i<3;i++)
		delete_edj(&menu[i]);
	for(int i=0;i<5;i++)
		for(int j=0;j<Apps[i].n_elem;j++)
			delete_obj(Applications[i].Image[j].obj);
	delete_edj(&bar);
	delete_edj(&download);
	delete_edj(&download_perc);
	delete_obj(down);


	edje_stop();
	stop_core_file();
	ecore_psl1ght_shutdown();
//	stop_core(&core);  not needed on ps3.

	if(pkg_is_installed)
		reboot_sys();
	

	return 0;
}
