using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OpenWristScript : MonoBehaviour {

    [Header("OpenWrist State Vector")]
    public double[] state = new double[10];

    [Header("Inertial Frame References")]
    public GameObject frame0;
    public GameObject frame1;
    public GameObject frame2;
    public GameObject frame3;

    private Vector3 frame0Zero;
    private Vector3 frame1Zero;
    private Vector3 frame2Zero;
    private Vector3 frame3Zero;

    // Use this for initialization
    void Start () {
        frame0Zero = frame0.transform.localEulerAngles;
        frame1Zero = frame1.transform.localEulerAngles;
        frame2Zero = frame2.transform.localEulerAngles;
        frame3Zero = frame3.transform.localEulerAngles;
	}
	
	// Update is called once per frame
	void Update () {

        // read in state from MELShare
        MelShare.ReadMap("ow_state", state);

        // update frame z-axes
        frame1.transform.localEulerAngles = new Vector3(frame1Zero.x, frame1Zero.y, -(float)state[1]*Mathf.Rad2Deg);
        frame2.transform.localEulerAngles = new Vector3(frame2Zero.x, frame2Zero.y, -(float)state[2] * Mathf.Rad2Deg);
        frame3.transform.localEulerAngles = new Vector3(frame3Zero.x, frame3Zero.y, -(float)state[3] * Mathf.Rad2Deg);

    }
}
