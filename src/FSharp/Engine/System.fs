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

// Disable native interop warnings
#nowarn "9"
#nowarn "51"

namespace Engine.System

open System
open System.IO
open System.Text
open System.Runtime.InteropServices
open System.Threading
open System.Diagnostics
open System.Reflection
open Microsoft.FSharp.NativeInterop
open Engine.Input
open Engine.Common
open Engine.Network
open Engine.IO
open Engine.Command
open Engine.NativeInterop
open Engine.Fsi

module private Native =
    [<DllImport (LibEngine, CallingConvention = DefaultCallingConvention)>]
    extern int system_cpu_get_physical_core_count ()

    [<DllImport (LibQuake3, CallingConvention = DefaultCallingConvention)>]
    extern void Sys_CreateConsole ()

    [<DllImport (LibQuake3, CallingConvention = DefaultCallingConvention)>]
    extern void Sys_Milliseconds ()

    [<DllImport (LibQuake3, CallingConvention = DefaultCallingConvention)>]
    extern void Sys_InitStreamThread ()

    [<DllImport (LibQuake3, CallingConvention = DefaultCallingConvention)>]
    extern void Sys_ShowConsole (int level, bool quitOnClose)

/// <summary>
/// System
///
/// Note: Revisit to make purely functional.
/// </summary
module System =
    let fsi = FsiSession @"C:\Program Files (x86)\Microsoft SDKs\F#\3.0\Framework\v4.0\fsi.exe"
    let private stopwatch = new Stopwatch ()

    let private SetupUnhandledExceptions () =
        let errorFilename = "error.txt"
        let UnhandledException (sender: obj) (e: UnhandledExceptionEventArgs) =
            let exceptionObject = (e.ExceptionObject :?> Exception)
            let msg = sprintf "%s %s\n" (exceptionObject.ToString ()) (exceptionObject.Message)

            let innerMsg = 
                match exceptionObject.InnerException = null with
                | true -> ""
                | _ -> exceptionObject.InnerException.Message
            
            let fullMsg = sprintf "%s %s" msg innerMsg

            File.WriteAllText (errorFilename, fullMsg)

        File.Delete (errorFilename)
        AppDomain.CurrentDomain.UnhandledException.AddHandler (new UnhandledExceptionEventHandler (UnhandledException))

    let Sleep (milliseconds: int) =
        Thread.Sleep (milliseconds)

    let Milliseconds () =
        stopwatch.ElapsedMilliseconds

    let GetPhysicalCoreCount () =
        Native.system_cpu_get_physical_core_count ()

    let CheckIsFsiRunning () =
        fsi.IsRunning

    let WriteFsiLine line =
        match fsi.IsRunning with
        | false -> raise <| Exception "Fsi is not running"
        | _ ->
        fsi.WriteLine line

    let Init () =
        SetupUnhandledExceptions ()

        use io = new StandardIO ()

        io.RedirectOut Common.Printf

        // done before Com/Sys_Init since we need this for error output
        Native.Sys_CreateConsole ()

        // get the initial time base
        stopwatch.Start ()

        Native.Sys_InitStreamThread ()

        Common.Init ""
        Network.Init ()

//#if USE_FSI_SESSION
        Command.Add "fsi" (fun _ -> 
            match fsi.IsRunning with
            | true -> ()
            | _ ->
            fsi.Start () |> ignore
        )
//#endif

        // hide the early console since we've reached the point where we
        // have a working graphics subsystems
        match (Common.CheckIsDedicated (), Common.CheckIsViewLogEnabled ()) with
        | (false, false) -> Native.Sys_ShowConsole (0, false)
        | _ -> ()

        printfn "Working directory: %s" (QFile.GetCurrentDirectory ())

        // main game loop
        while true do
            // if not running as a game client, sleep a bit
            match Common.CheckIsDedicated () with
            | true -> Sleep (5)
            | _ -> ()

            // make sure mouse and joystick are only called once a frame
            Input.Frame ()

            // run the game
            Common.Frame ();
            
//#if USE_FSI_SESSION
            match fsi.IsRunning with
            | true ->
                match fsi.ReadOutput () with
                | "" -> ()
                | x -> printf "%s" x 

                match fsi.ReadError () with
                | "" -> ()
                | x -> printf "%s" x 
            | _ -> ()
//#endif
            // Flush standard out
            io.FlushOut ()
        ()
