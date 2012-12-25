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

#ifndef __PLAYSTATION_CHANNEL_CORE_H__
#define __PLAYSTATION_CHANNEL_CORE_H__

#include <Ecore.h>
#include <Ecore_Psl1ght.h>
#include <Ecore_Evas.h>
#include <stdint.h>

typedef struct {
	Ecore_Evas *ee_core;
	uint16_t width;
	uint16_t height;
}EFL_Core;


void init_core(EFL_Core *core, uint16_t width, uint16_t height, const char *APP);
void start_core_loop();
void stop_core(EFL_Core *core);

Evas *get_main_object(EFL_Core *core);
void delete_obj(Evas_Object *obj);
void show_obj(Evas_Object *obj);
void hide_obj(Evas_Object *obj);

void enable_callback(EFL_Core *core, Evas_Object *obj, Evas_Callback_Type type, Evas_Object_Event_Cb func);
void disable_callback(Evas_Object *obj, Evas_Callback_Type type, Evas_Object_Event_Cb func);

#endif
