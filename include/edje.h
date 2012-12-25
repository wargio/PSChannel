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

#ifndef __PLAYSTATION_CHANNEL_EDJE_H__
#define __PLAYSTATION_CHANNEL_EDJE_H__

#include "core.h"

#include <Edje.h>

typedef struct{
	Evas_Object *obj;
	uint16_t width;
	uint16_t height;
}EFL_Edj;


void edje_start();
void edje_stop();

void load_edj_from_file(EFL_Core* core, EFL_Edj *edj, const char *path_edj, const char* main_label_name, int height, int width);
void override_text(EFL_Edj *edj, const char* text_label_name,const char* message);
void draw_edj(EFL_Edj *edj, int x, int y);
void draw_edj(EFL_Edj *edj, int x, int y, int width, int height);
void delete_edj(EFL_Edj *edj);
void show_edj(EFL_Edj *edj);
void hide_edj(EFL_Edj *edj);
void move_edj(EFL_Edj *edj, int x, int y);

#endif
