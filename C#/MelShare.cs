// MIT License
//
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

using System;
using System.Text;
using System.Threading;

#if UNITY_2017_1_OR_NEWER

using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
using UnityEngine;

class MelShare
{
    Mutex mutex_;
    SharedMemory shm_;

    /// <summary>
    /// Constructor
    /// </summary>
    public MelShare(string name, long maxSize = 256)
    {
        try
        {
            shm_ = new SharedMemory(name);
            mutex_ = new Mutex(false, name + "_mutex");
            Debug.Log("Opened MelShare <" + name + ">");
        }
        catch
        {
            Debug.Log("Failed to open MelShare <" + name + ">");
        }
    }

    /// <summary>
    /// Destructor
    /// </summary>
    ~MelShare()
    {
        mutex_.Close();
    }

    /// <summary>
    /// Writes an array of doubles to the MelShare
    /// </summary>
    public void WriteData(double[] data)
    {
        uint size = (uint)(data.Length * sizeof(double));
        if ((size + sizeof(uint)) <= shm_.MaxSize)
        {
            mutex_.WaitOne();
            byte[] sizeBytes = BitConverter.GetBytes(size);
            Marshal.Copy(sizeBytes, 0, shm_.Address, 1);
            Marshal.Copy(data, 0, shm_.Offset(sizeof(uint)), data.Length);
            mutex_.ReleaseMutex();
        }
        else
            Debug.Log("MelShare <" + shm_.Name + "> failed to write data. Data is larger than max size of " + shm_.MaxSize + " bytes");
    }

    /// <summary>
    /// Reads an array of doubles from the MelShare.
    /// </summary>
    public double[] ReadData()
    {
        double[] data;
        mutex_.WaitOne();
        uint size = GetSize();
        if (size > 0)
        {
            data = new double[size / sizeof(double)];
            Marshal.Copy(shm_.Offset(sizeof(uint)), data, 0, data.Length);
        }
        else
            data = new double[0];
        mutex_.ReleaseMutex();
        return data;
    }

    /// <summary>
    /// Writes a string messsage to the MelShare.
    /// </summary>
    public void WriteMessage(string message)
    {
        message += Char.MinValue; // add null terminator
        uint size = (uint)(message.Length);
        if ((size + sizeof(uint)) <= shm_.MaxSize)
        {
            mutex_.WaitOne();
            byte[] sizeBytes = BitConverter.GetBytes(size);
            Marshal.Copy(sizeBytes, 0, shm_.Address, 1);
            byte[] stringBytes = Encoding.ASCII.GetBytes(message);
            Marshal.Copy(stringBytes, 0, shm_.Offset(sizeof(uint)), stringBytes.Length);
            mutex_.ReleaseMutex();
        }
        else
            Debug.Log("MelShare <" + shm_.Name + "> failed to write message. Message is larger than max size of " + shm_.MaxSize + " bytes");

    }

    /// <summary>
    /// Reads a string message from the MelShare.
    /// </summary>
    public string ReadMessage()
    {
        string message;
        mutex_.WaitOne();
        uint size = GetSize();
        if (size > 0)
        {
            byte[] messageBytes = new byte[size / sizeof(byte) - 1]; // strip null terminator
            Marshal.Copy(shm_.Offset(sizeof(uint)), messageBytes, 0, messageBytes.Length);
            message = Encoding.ASCII.GetString(messageBytes);
        }
        else
            message = "";
        mutex_.ReleaseMutex();
        return message;
    }

    /// <summary>
    /// Gets the number of bytes currently stored in the MelShare.
    /// </summary>
    private uint GetSize()
    {
        byte[] sizeBytes = new byte[4];
        Marshal.Copy(shm_.Address, sizeBytes, 0, 4);
        return BitConverter.ToUInt32(sizeBytes, 0);
    }

}

//==============================================================================
// UNITY EXAMPLE:
//==============================================================================
/*
using UnityEngine;

public class MelShareExample : MonoBehaviour
{

    public string id = "A";
    MelShare ms;

    // Use this for initialization
    void Start()
    {
        ms = new MelShare("melshare");
        if (id == "A")
        {
            ms.WriteMessage("Hello from C#/Unity! Please send me some data.");
            print("Press Enter after running B ...");
        }
        else if (id == "B")
        {
            print(ms.ReadMessage());
            ms.WriteData(new double[] { 7.0, 8.0, 9.0 });
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (id == "A" && Input.GetKeyDown(KeyCode.Return))
        {
            var data = ms.ReadData();
            foreach (double d in data)
                print(d);
        }
    }
}
*/

//==============================================================================
// SHARED MEMORY IMPLEMENTATION:
//==============================================================================

class SharedMemory
{
    string name_;
    uint maxSize_;
    SafeFileHandle map_;
    IntPtr buffer_;

    public string Name
    {
        get { return name_; }
    }

    public uint MaxSize
    {
        get { return maxSize_; }
    }

    public IntPtr Address
    {
        get { return buffer_; }
    }

    public SharedMemory(string name, uint maxSize = 256)
    {
        name_ = name;
        maxSize_ = maxSize;
        map_ = CreateFileMapping(INVALID_HANDLE_VALUE, IntPtr.Zero, PAGE_READWRITE, 0, maxSize, name);
        if (map_.IsInvalid)
            Debug.Log("Failed to create/open file mapping " + name);
        buffer_ = MapViewOfFile(map_, FILE_MAP_ALL_ACCESS, 0, 0, maxSize);
    }

    public IntPtr Offset(int offset)
    {
        return Offset(buffer_, offset);
    }

    public static unsafe IntPtr Offset(IntPtr pointer, int offset)
    {
        return (IntPtr)(unchecked(((byte*)pointer) + offset));
    }

    // TODO: Read & Write functions. Maybe uncessary with Marshall.Copy(...) available.

    const UInt32 STANDARD_RIGHTS_REQUIRED = 0x000F0000;
    const UInt32 SECTION_QUERY = 0x0001;
    const UInt32 SECTION_MAP_WRITE = 0x0002;
    const UInt32 SECTION_MAP_READ = 0x0004;
    const UInt32 SECTION_MAP_EXECUTE = 0x0008;
    const UInt32 SECTION_EXTEND_SIZE = 0x0010;
    const UInt32 SECTION_ALL_ACCESS = (
        STANDARD_RIGHTS_REQUIRED |
        SECTION_QUERY |
        SECTION_MAP_WRITE |
        SECTION_MAP_READ |
        SECTION_MAP_EXECUTE |
        SECTION_EXTEND_SIZE);
    const UInt32 FILE_MAP_ALL_ACCESS = SECTION_ALL_ACCESS;
    UInt32 PAGE_READWRITE = 0x00000004;
    IntPtr INVALID_HANDLE_VALUE = new IntPtr(-1);


    [DllImport("kernel32.dll", EntryPoint = "CreateFileMapping", SetLastError = true, CharSet = CharSet.Auto)]
    static extern SafeFileHandle CreateFileMapping(
        IntPtr hFile,
        IntPtr lpSecurityAttributes,
        UInt32 flProtect,
        UInt32 dwMaximumSizeHigh,
        UInt32 dwMaximumSizeLow,
        string lpName);

    [DllImport("kernel32.dll", EntryPoint = "OpenFileMapping", SetLastError = true, CharSet = CharSet.Auto)]
    static extern SafeFileHandle OpenFileMapping(
          uint dwDesiredAccess,
          bool bInheritHandle,
          string lpName);

    [DllImport("kernel32.dll", EntryPoint = "MapViewOfFile", SetLastError = true)]
    static extern IntPtr MapViewOfFile(
        SafeFileHandle hFileMappingObject,
        UInt32 dwDesiredAccess,
        UInt32 wfileOffsetHigh,
        UInt32 dwFileOffsetLow,
        UInt32 dwNumberOfBytesToMap);

    [DllImport("kernel32.dll", EntryPoint = "UnmapViewOfFile", SetLastError = true)]
    static extern bool UnmapViewOfFile(IntPtr lpBaseAddress);

    [DllImport("kernel32.dll", EntryPoint = "CopyMemory", SetLastError = false)]
    static extern void CopyMemory(IntPtr dest, IntPtr src, uint count);

}

#else

//==============================================================================
// MEMORY MAPPED FILES IMPLEMENTATION:
//==============================================================================

using System.IO.MemoryMappedFiles;

class MelShare
{
    MemoryMappedFile mmf;
    MemoryMappedViewAccessor accessor;
    Mutex mutex;

    /// <summary>
    /// Default constructor.
    /// </summary>
    public MelShare(string name, long maxSize = 256)
    {
        try
        {
            mmf = MemoryMappedFile.CreateOrOpen(name, maxSize);
            accessor = mmf.CreateViewAccessor();
            mutex = new Mutex(false, name + "_mutex");
        }
        catch
        {
            Console.WriteLine("Failed to open MelShare " + name);
        }
    }

    /// <summary>
    /// Writes a vector of doubles to the MelShare
    /// </summary>
    public void WriteData(double[] data)
    {
        try
        {
            mutex.WaitOne();
            uint size = (uint)(data.Length * sizeof(double));
            accessor.Write(0, size);
            for (int i = 0; i < data.Length; ++i)
                accessor.Write(sizeof(uint) + i * sizeof(double), data[i]);
            mutex.ReleaseMutex();
        }
        catch
        {

        }
    }

    /// <summary>
    /// Reads a vector of doubles from the MelShare.
    /// </summary>
    public double[] ReadData()
    {
        try
        {
            mutex.WaitOne();
            uint length = GetSize() / sizeof(double);
            double[] data = new double[length];
            for (int i = 0; i < length; ++i)
                data[i] = accessor.ReadDouble(sizeof(uint) + i * sizeof(double));
            mutex.ReleaseMutex();
            return data;
        }
        catch
        {
            return new double[0];
        }
    }

    /// <summary>
    /// Writes a string messsage to the MelShare.
    /// </summary>
    public void WriteMessage(string message)
    {
        try
        {
            mutex.WaitOne();
            message += Char.MinValue; // add null terminator
            uint size = (uint)(message.Length);
            accessor.Write(0, size);
            byte[] stringBytes = Encoding.ASCII.GetBytes(message);
            for (int i = 0; i < stringBytes.Length; ++i)
                accessor.Write(sizeof(uint) + i * sizeof(byte), stringBytes[i]);
            mutex.ReleaseMutex();
        }
        catch
        {

        }
    }

    /// <summary>
    /// Reads a string message from the MelShare.
    /// </summary>
    public string ReadMessage()
    {
        try
        {
            mutex.WaitOne();
            int length = (int)GetSize() / sizeof(byte);
            byte[] message = new byte[length - 1];
            for (int i = 0; i < length; ++i)
                message[i] = accessor.ReadByte(sizeof(uint) + i * sizeof(byte));
            mutex.ReleaseMutex();
            return Encoding.ASCII.GetString(message);
        }
        catch
        {
            return "";
        }
    }

    /// <summary>
    /// Gets the number of bytes currently stored in the MelShare.
    /// </summary>
    private uint GetSize()
    {
        return accessor.ReadUInt32(0);
    }
}

#endif
