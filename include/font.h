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

#ifndef __PLAYSTATION_CHANNEL_FONT_H__
#define __PLAYSTATION_CHANNEL_FONT_H__

#include "core.h"
#include "color.h"

typedef struct {
	Evas_Object *obj;
	Evas_Text_Style_Type type;
	Evas_Font_Size size;
}EFL_Font;

void create_text_obj(EFL_Core* core, EFL_Font *font, const char* path_font);
void create_text_obj(EFL_Core* core, EFL_Font *font, const char* path_font, int size);
void create_text_obj(EFL_Core* core, EFL_Font *font, const char* path_font, Evas_Text_Style_Type type);
void create_text_obj(EFL_Core* core, EFL_Font *font, const char* path_font, Evas_Text_Style_Type type, int size);

void write_text(EFL_Font *font, int x, int y, const char *message);
void write_text(EFL_Font *font, int x, int y, const char *message, EFL_Color color);

#endif
