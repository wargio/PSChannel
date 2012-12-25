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

#include "edje.h"

void edje_start(){
	edje_init();
}

void edje_stop(){
	edje_shutdown();
}

void load_edj_from_file(EFL_Core* core, EFL_Edj *edj, const char *path_edj, const char* main_label_name, int width, int height){
	edj->obj = edje_object_add(get_main_object(core));
	edje_object_file_set(edj->obj, path_edj, main_label_name);
	edj->width  = width;
	edj->height = height;
}

void override_text(EFL_Edj *edj, const char* text_label_name,const char* message){
	edje_object_part_text_set(edj->obj, text_label_name, message);
}

void draw_edj(EFL_Edj *edj, int x, int y){
	evas_object_move(edj->obj, x, y);
	evas_object_resize(edj->obj, edj->width, edj->height);
	evas_object_show(edj->obj);
}

void draw_edj(EFL_Edj *edj, int x, int y, int width, int height){
	evas_object_move(edj->obj, x, y);
	evas_object_resize(edj->obj, width, height);
	evas_object_show(edj->obj);
}

void move_edj(EFL_Edj *edj, int x, int y){
	evas_object_move(edj->obj, x, y);
}


void delete_edj(EFL_Edj *edj){
	delete_obj(edj->obj);
}

void show_edj(EFL_Edj *edj){
	show_obj(edj->obj);
}

void hide_edj(EFL_Edj *edj){
	hide_obj(edj->obj);
}
