using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Text;

// Evan Pezent | epezent@rice.edu
// 08/2017

//-----------------------------------------------------------------------------
// MELSHARE ERROR CODES
//-----------------------------------------------------------------------------
// >0 = successful read/write, return value is current map size
//  0   map size is zero
// -1 = failed to open shared memory map
// -2 = failed to open mutex
// -3 = wait on mutex abandoned
// -4 = wait on mutex timed out
// -5 = wait on mutex failed
// -6 = failed to release mutex
// -7 = failed to close mutex handle
//-----------------------------------------------------------------------------

public class MelShare {

    //-------------------------------------------------------------------------
    // API FUNCTIONS (USE THESE)
    //-------------------------------------------------------------------------

    public static int ReadMap(string name, byte[] buffer) { return ReadByteMap(name, buffer, buffer.Length); }
    public static int ReadMap(string name, int[] buffer) { return ReadIntMap(name, buffer, buffer.Length); }
    public static int ReadMap(string name, float[] buffer) { return ReadFloatMap(name, buffer, buffer.Length); }
    public static int ReadMap(string name, double[] buffer) { return ReadDoubleMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, byte[] buffer) { return WriteByteMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, int[] buffer) { return WriteIntMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, float[] buffer) { return WriteFloatMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, double[] buffer) { return WriteDoubleMap(name, buffer, buffer.Length); }

    public static string ReadMessage(string name)
    {
        int messageSize = GetMapSize(name);
        if (messageSize > 0)
        {
            byte[] messageChars = new byte[messageSize];
            ReadMap(name, messageChars);
            return Encoding.Default.GetString(messageChars);
        } else
        {
            return "";
        }
    }

    public static void WriteMessage(string name, string message)
    {
        byte[] messageChars = Encoding.Default.GetBytes(message);
        WriteMap(name, messageChars);
    }

    //-------------------------------------------------------------------------
    // DLL IMPORTS (DON'T USE THESE)
    //-------------------------------------------------------------------------

    [DllImport("MELShare", EntryPoint = "get_map_size", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int GetMapSize(string name);

    [DllImport("MELShare", EntryPoint = "read_char_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int ReadByteMap(string name, byte[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "read_int_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int ReadIntMap(string name, int[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "read_float_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int ReadFloatMap(string name, float[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "read_double_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int ReadDoubleMap(string name, double[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "write_char_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int WriteByteMap(string name, byte[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "write_int_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int WriteIntMap(string name, int[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "write_float_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int WriteFloatMap(string name, float[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "write_double_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    private static extern int WriteDoubleMap(string name, double[] buffer, int size);

}
