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
    }
}
