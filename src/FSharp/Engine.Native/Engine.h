/*
Copyright (C) 2013 William F. Smith

This program is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

This program is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Derivative of Quake III Arena source:
Copyright (C) 1999-2005 Id Software, Inc.
*/

#ifndef __ENGINE_H__
#define __ENGINE_H__

#if defined(_WIN32)
#	define ENGINE_IMPORT __declspec(dllimport)
#	define ENGINE_EXPORT	__declspec(dllexport)
#	define ENGINE_DECL __cdecl
#elif defined(__GNUC__)
#	define ENGINE_EXPORT __attribute__((visibility("default")))
#	define ENGINE_IMPORT
#	define ENGINE_DECL __attribute__((cdecl))
#else
#	error Compiler not supported.
#endif

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vector4_t;

typedef struct {
	float values[4][4];
} matrix16_t;

#endif /* __ENGINE_H__ */