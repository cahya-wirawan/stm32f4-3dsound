/*
 STM32F4-3DSound - Copyright (C) 2015 Cahya Wirawan.
 
 This file is part of STM32F4-3DSound
 
 STM32F4-3DSound is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 STM32F4-3DSound is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file      cwMems.h
 * @brief     Mems macros and structures
 * @details
 */

#ifndef CW_MEMS_H_
#define CW_MEMS_H_

#define CW_MEMS_MAX_VALUE 1024

void cwMemsInit(void);
float cwMemsGetAzimuth(void);
void cwMemsGetPosition(float *elevation, float *azimuth);

#endif /* CW_MEMS_H_ */
