using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Text;

public class MelShareScript : MonoBehaviour {

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

    public static int ReadMap(string name, byte[] buffer) { return ReadByteMap(name, buffer, buffer.Length); }
    public static int ReadMap(string name, int[] buffer) { return ReadIntMap(name, buffer, buffer.Length); }
    public static int ReadMap(string name, double[] buffer) { return ReadDoubleMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, byte[] buffer) { return WriteByteMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, int[] buffer) { return WriteIntMap(name, buffer, buffer.Length); }
    public static int WriteMap(string name, double[] buffer) { return WriteDoubleMap(name, buffer, buffer.Length); }

    // Use this for initialization
    void Start () {
        byte[] myBytes = new byte[3];
        int[] myInts = new int[5];
        double[] myDoubles = new double[10];
        int result = 1;

        print("map0:");
        result = ReadMap("map0", myBytes);
        foreach (byte b in myBytes)
            print((char)b);

        print("map1:");
        result = ReadMap("map1", myInts);
        foreach (int i in myInts)
            print(i);

        print("map2:");
        result = ReadMap("map2", myDoubles);
        foreach (double d in myDoubles)
            print(d);

        myBytes[0] = (byte)'x';
        myBytes[1] = (byte)'y';
        myBytes[2] = (byte)'z';
        result = WriteMap("map0", myBytes);

        for (int i = 0; i < myDoubles.Length; i++)
            myDoubles[i] *= 2;
        result = WriteMap("map1", myDoubles);

        myInts = new int[] { 1, 1, 2, 3, 5, 8, 13 };
        result = WriteMap("map2", myInts);
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
