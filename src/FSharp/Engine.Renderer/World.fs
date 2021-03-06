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

module Engine.Renderer.World

open System
open System.Diagnostics.Contracts
open Engine.Core
open Engine.Math
open Engine.Renderer.Core

/// Based on Q3: R_CullTriSurf
/// CullTriangleSurface
///
/// Returns true if the grid is completely culled away.
/// Also sets the clipped hint bit in tess
let cullTriangleSurface (surface: SurfaceTriangles) (r: Renderer) =
    ()