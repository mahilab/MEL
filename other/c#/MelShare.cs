using System;
using System.Text;
using System.Threading;
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
            uint size = (uint)(message.Length + 1);
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
            byte[] message = new byte[length];
            for (int i = 0; i < length; ++i)
                message[i] = accessor.ReadByte(sizeof(uint) + i * sizeof(byte));
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