﻿(*
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
*)

module GL

// Disable native interop warnings
#nowarn "9"
#nowarn "51"

open System
open System.Runtime.InteropServices
open Microsoft.FSharp.NativeInterop

[<Literal>]
let LibOpenGL = "opengl32.dll"

[<Literal>]
let OpenGLCallingConvention = CallingConvention.Cdecl

//

[<Literal>]
let GL_EQUAL = 0x0202

[<Literal>]
let GL_LEQUAL = 0x0202

[<Literal>]
let GL_MODELVIEW = 0x1700

[<Literal>]
let GL_PROJECTION = 0x1701

// BlendingFactorDest
[<Literal>]
let GL_ZERO = 0

[<Literal>]
let GL_ONE = 1

[<Literal>]
let GL_SRC_COLOR = 0x0300

[<Literal>]
let GL_ONE_MINUS_SRC_COLOR = 0x0301

[<Literal>]
let GL_SRC_ALPHA = 0x0302

[<Literal>]
let GL_ONE_MINUS_SRC_ALPHA = 0x0303

[<Literal>]
let GL_DST_ALPHA = 0x0304

[<Literal>]
let GL_ONE_MINUS_DST_ALPHA = 0x0305

[<Literal>]
let GL_DST_COLOR = 0x0306

[<Literal>]
let GL_ONE_MINUS_DST_COLOR = 0x0307

[<Literal>]
let GL_SRC_ALPHA_SATURATE = 0x0308

[<Literal>]
let GL_BLEND = 0x0BE2

type GLenum = uint32
type GLint = int
type GLsizei = int
type GLfloat = single

let inline GLenum a = uint32 a

//

[<DllImport (LibOpenGL, CallingConvention = OpenGLCallingConvention)>]
extern void glMatrixMode (GLenum mode)

[<DllImport (LibOpenGL, CallingConvention = OpenGLCallingConvention)>]
extern void glLoadMatrixf (GLfloat *m)

[<DllImport (LibOpenGL, CallingConvention = OpenGLCallingConvention)>]
extern void glViewport (GLint x, GLint y, GLsizei width, GLsizei height)

[<DllImport (LibOpenGL, CallingConvention = OpenGLCallingConvention)>]
extern void glScissor (GLint x, GLint y, GLsizei width, GLsizei height)

[<DllImport (LibOpenGL, CallingConvention = OpenGLCallingConvention)>]
extern void glFinish ()

[<DllImport (LibOpenGL, CallingConvention = OpenGLCallingConvention)>]
extern void glDepthFunc (GLenum func)

[<DllImport (LibOpenGL, CallingConvention = OpenGLCallingConvention)>]
extern void glEnable (GLenum cap)

[<DllImport (LibOpenGL, CallingConvention = OpenGLCallingConvention)>]
extern void glDisable (GLenum cap)

[<DllImport (LibOpenGL, CallingConvention = OpenGLCallingConvention)>]
extern void glBlendFunc (GLenum sfactor, GLenum dfactor)