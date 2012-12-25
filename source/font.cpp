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

#include "font.h"

void create_text_obj(EFL_Core* core, EFL_Font *font, const char* path_font){
	font->obj = evas_object_text_add(get_main_object(core));
	evas_object_text_font_source_set(font->obj, path_font);
	font->type = EVAS_TEXT_STYLE_PLAIN;
	font->size = 30;
}


void create_text_obj(EFL_Core* core, EFL_Font *font, const char* path_font, int size){
	font->obj = evas_object_text_add(get_main_object(core));
	evas_object_text_font_source_set(font->obj, path_font);
	font->type = EVAS_TEXT_STYLE_PLAIN;
	font->size = size;
}

void create_text_obj(EFL_Core* core, EFL_Font *font, const char* path_font, Evas_Text_Style_Type type){
	font->obj = evas_object_text_add(get_main_object(core));
	evas_object_text_font_source_set(font->obj, path_font);
	font->type = type;
	font->size = 30;
}

void create_text_obj(EFL_Core* core, EFL_Font *font, const char* path_font, Evas_Text_Style_Type type, int size){
	font->obj = evas_object_text_add(get_main_object(core));
	evas_object_text_font_source_set(font->obj, path_font);
	font->type = type;
	font->size = size;
}

void write_text(EFL_Font *font, int x, int y, const char *message){
	evas_object_text_style_set(font->obj, font->type);
	evas_object_color_set(font->obj, 0,0,0,255);
	evas_object_text_font_set(font->obj, "font", font->size);
	evas_object_text_text_set(font->obj, message);
	evas_object_move(font->obj, x, y);
	evas_object_show(font->obj);
}

void write_text(EFL_Font *font, int x, int y, const char *message, EFL_Color color){
	evas_object_text_style_set(font->obj, font->type);
	evas_object_color_set(font->obj, color.r,color.g,color.b,color.a);
	evas_object_text_font_set(font->obj, "font", font->size);
	evas_object_text_text_set(font->obj, message);
	evas_object_move(font->obj, x, y);
	evas_object_show(font->obj);
}
