using System.Runtime.InteropServices;

namespace ManagedBass.Gapless.Cd
{
    public static class BassGaplessCd
    {
        const string DllName = "bass_gapless_cd";

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_CD_Init();

        /// <summary>
        /// Initialize.
        /// </summary>
        /// <returns></returns>
        public static bool Init()
        {
            return BASS_GAPLESS_CD_Init();
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_CD_Free();

        /// <summary>
        /// Free.
        /// </summary>
        /// <returns></returns>
        public static bool Free()
        {
            return BASS_GAPLESS_CD_Free();
        }


        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_CD_Enable(int Drive, BassFlags Flags);

        /// <summary>
        /// Enable.
        /// </summary>
        /// <returns></returns>
        public static bool Enable(int Drive, BassFlags Flags)
        {
            return BASS_GAPLESS_CD_Enable(Drive, Flags);
        }

        [DllImport(DllName)]
        static extern bool BASS_GAPLESS_CD_Disable();

        /// <summary>
        /// Disable.
        /// </summary>
        /// <returns></returns>
        public static bool Disable()
        {
            return BASS_GAPLESS_CD_Disable();
        }
    }
}
