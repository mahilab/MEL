using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Text;

public class MelShare {

    // Import MELShare.dll (note that C++ char* array is a byte[] array in C#)

    [DllImport("MELShare", EntryPoint = "read_char_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int ReadByteMap(string name, byte[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "read_int_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int ReadIntMap(string name, int[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "read_double_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int ReadDoubleMap(string name, double[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "write_char_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int WriteByteMap(string name, byte[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "write_int_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int WriteIntMap(string name, int[] buffer, int size);

    [DllImport("MELShare", EntryPoint = "write_double_map", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int WriteDoubleMap(string name, double[] buffer, int size);

    // Wrap our imported functions into nice, overloaded functions

    public static int ReadMap(string name, byte[] buffer) { return ReadByteMap(name, buffer, buffer.Length); }
    public static int ReadMap(string name, int[] buffer) { return ReadIntMap(name, buffer, buffer.Length); }
    public static int ReadMap(string name, double[] buffer) { return ReadDoubleMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, byte[] buffer) { return WriteByteMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, int[] buffer) { return WriteIntMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, double[] buffer) { return WriteDoubleMap(name, buffer, buffer.Length); }

}
