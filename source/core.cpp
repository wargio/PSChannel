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

void init_core(EFL_Core *core, uint16_t width, uint16_t height, const char *APP){
	ecore_evas_init();
	core->ee_core = ecore_evas_new(NULL, 0, 0, width, height, NULL);
//	ecore_evas_title_set(core->ee_core, APP);
	ecore_evas_show(core->ee_core);
	core->width  = width;
	core->height = height;
	
}

Evas *get_main_object(EFL_Core *core){
	return ecore_evas_get(core->ee_core);
}

void start_core_loop(){
	ecore_main_loop_begin();
}

void stop_core(EFL_Core *core){
	ecore_evas_free(core->ee_core);
	ecore_evas_shutdown();
}

void delete_obj(Evas_Object *obj){
	evas_object_del(obj);
}

void show_obj(Evas_Object *obj){
	evas_object_show(obj);
}

void hide_obj(Evas_Object *obj){
	evas_object_hide(obj);
}

void enable_callback(EFL_Core *core, Evas_Object *obj, Evas_Callback_Type type, Evas_Object_Event_Cb func){
	ecore_evas_object_associate(core->ee_core, obj, ECORE_EVAS_OBJECT_ASSOCIATE_BASE);
	evas_object_event_callback_add(obj, type, func, NULL);
	evas_object_focus_set(obj, EINA_TRUE);
}


void disable_callback(Evas_Object *obj, Evas_Callback_Type type, Evas_Object_Event_Cb func){
	evas_object_event_callback_del(obj,type, func);
}


