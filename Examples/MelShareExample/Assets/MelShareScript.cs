using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System.Text;

public class MelShareScript : MonoBehaviour {
    // Create our data buffers and make them public so they can be seen in the inspector

    public byte[] myBytes = new byte[3];
    public int[] myInts = new int[5];
    public double[] myDoubles = new double[10];
    public int result = 1;
    
    // Use this for initialization
    void Start () {

        print("map0:");
        result = MelShare.ReadMap("map0", myBytes);
        foreach (byte b in myBytes)
            print((char)b);

        print("map1:");
        result = MelShare.ReadMap("map1", myInts);
        foreach (int i in myInts)
            print(i);

        print("map2:");
        result = MelShare.ReadMap("map2", myDoubles);
        foreach (double d in myDoubles)
            print(d);

        myBytes[0] = (byte)'x';
        myBytes[1] = (byte)'y';
        myBytes[2] = (byte)'z';
        result = MelShare.WriteMap("map0", myBytes);

        for (int i = 0; i < myDoubles.Length; i++)
            myDoubles[i] *= 2;
        result = MelShare.WriteMap("map1", myDoubles);

        myInts = new int[] { 1, 1, 2, 3, 5, 8, 13 };
        result = MelShare.WriteMap("map2", myInts);	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
