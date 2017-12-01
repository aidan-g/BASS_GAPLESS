using System;
using System.Runtime.InteropServices;

namespace ManagedBass.Gapless.Asio
{
    public static class BassGaplessAsio
    {
        const string DllName = "bass_gapless_asio";

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_ASIO_Init();

        /// <summary>
        /// Initialize.
        /// </summary>
        /// <returns></returns>
        public static bool Init()
        {
            return BASS_GAPLESS_ASIO_Init();
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_ASIO_Free();

        /// <summary>
        /// Free.
        /// </summary>
        /// <returns></returns>
        public static bool Free()
        {
            return BASS_GAPLESS_ASIO_Free();
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_ASIO_ChannelEnable(bool Input, int Channel, IntPtr User = default(IntPtr));

        public static bool ChannelEnable(bool Input, int Channel, IntPtr User = default(IntPtr))
        {
            return BASS_GAPLESS_ASIO_ChannelEnable(Input, Channel, User);
        }
    }
}
