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

#ifndef __PLAYSTATION_CHANNEL_IMAGE_H__
#define __PLAYSTATION_CHANNEL_IMAGE_H__

#include "core.h"


typedef struct {
	Evas_Object *obj;
	uint16_t width;
	uint16_t height;
}EFL_Image;

typedef struct{
	EFL_Image *Image;
	int *x;
	int *y;
}PSC_App;


void create_image_obj(EFL_Core *core, EFL_Image *img, const char* path_image, int img_width, int img_height);
void draw_image(EFL_Image *img, int x, int y);
void draw_image(EFL_Image *img, int x, int y, int resize_w, int resize_h);
void move_image(EFL_Image *img, int x, int y);
void resize_image(EFL_Image *img, int x, int y, int resize_w, int resize_h);
void resize_image(EFL_Image *img, int x, int y);

#endif
