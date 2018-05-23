using System;
using System.Runtime.InteropServices;

namespace ManagedBass.Gapless
{
    public static class BassGapless
    {
        const string DllName = "bass_gapless";

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_Init();

        /// <summary>
        /// Initialize.
        /// </summary>
        /// <returns></returns>
        public static bool Init()
        {
            return BASS_GAPLESS_Init();
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_Free();

        /// <summary>
        /// Free.
        /// </summary>
        /// <returns></returns>
        public static bool Free()
        {
            return BASS_GAPLESS_Free();
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_SetConfig(BassGaplessAttriubute Attrib, int Value);

        public static bool SetConfig(BassGaplessAttriubute Attrib, int Value)
        {
            return BASS_GAPLESS_SetConfig(Attrib, Value);
        }

        public static bool SetConfig(BassGaplessAttriubute Attrib, bool Value)
        {
            return BASS_GAPLESS_SetConfig(Attrib, Value ? 1 : 0);
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_GetConfig(BassGaplessAttriubute Attrib, out int Value);

        public static bool GetConfig(BassGaplessAttriubute Attrib, out int Value)
        {
            return BASS_GAPLESS_GetConfig(Attrib, out Value);
        }

        [DllImport(DllName)]
        static extern int BASS_GAPLESS_StreamCreate(int Frequency, int Channels, BassFlags Flags, IntPtr User = default(IntPtr));

        public static int StreamCreate(int Frequency, int Channels, BassFlags Flags, IntPtr User = default(IntPtr))
        {
            return BASS_GAPLESS_StreamCreate(Frequency, Channels, Flags, User);
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_ChannelEnqueue(int Handle);

        public static bool ChannelEnqueue(int Handle)
        {
            return BASS_GAPLESS_ChannelEnqueue(Handle);
        }

        [DllImport(DllName)]
        static extern IntPtr BASS_GAPLESS_GetChannels(out int Count);

        public static int[] GetChannels(out int Count)
        {
            var channels = BASS_GAPLESS_GetChannels(out Count);
            var result = new int[Count];
            Marshal.Copy(channels, result, 0, Count);
            return result;
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_ChannelRemove(int Handle);

        public static bool ChannelRemove(int Handle)
        {
            return BASS_GAPLESS_ChannelRemove(Handle);
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_EnableEvents(BassGaplessEventProcedure Handler);

        public static bool EnableEvents(BassGaplessEventProcedure Handler)
        {
            return BASS_GAPLESS_EnableEvents(Handler);
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_DisableEvents();

        public static bool DisableEvents()
        {
            return BASS_GAPLESS_DisableEvents();
        }
    }

    public enum BassGaplessAttriubute
    {
        None = 0,
        KeepAlive = 1,
        BlockingEvents = 2,
        RecycleStream = 4
    }

    public enum BassGaplessEventType
    {
        None = 0,
        Start = 1,
        Change = 2,
        End = 3
    }

    public struct BassGaplessEventArgs
    {
        public BassGaplessEventType EventType;
        public int Channel1;
        public int Channel2;
    }

    public delegate void BassGaplessEventProcedure(BassGaplessEventArgs e);
}
