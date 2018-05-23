using ManagedBass.Cd;
using NUnit.Framework;
using System;
using System.Threading;
using System.Linq;

namespace ManagedBass.Gapless.Cd.Test
{
    [TestFixture]
    public class Tests
    {
        /// <summary>
        /// A basic end to end test.
        /// </summary>
        [Test]
        public void Test001()
        {
            if (!Bass.Init(Bass.DefaultDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassGapless.Init() || !BassGaplessCd.Init() || !BassGaplessCd.Enable(0, BassFlags.Decode | BassFlags.Float))
            {
                Assert.Fail("Failed to initialize GAPLESS.");
            }

            BassGapless.SetConfig(BassGaplessAttriubute.RecycleStream, true);

            var sourceChannel = BassCd.CreateStream(0, 0, BassFlags.Decode | BassFlags.Float);
            if (sourceChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var channelInfo = default(ChannelInfo);
            if (!Bass.ChannelGetInfo(sourceChannel, out channelInfo))
            {
                Assert.Fail(string.Format("Failed to get channel info: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassGapless.ChannelEnqueue(sourceChannel))
            {
                Assert.Fail("Failed to add stream to gapless queue.");
            }

            var playbackChannel = BassGapless.StreamCreate(channelInfo.Frequency, channelInfo.Channels, BassFlags.Float);
            if (playbackChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create playback stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!Bass.ChannelPlay(playbackChannel))
            {
                Assert.Fail(string.Format("Failed to play stream:  {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var channelLength = Bass.ChannelGetLength(sourceChannel);
            var channelLengthSeconds = Bass.ChannelBytes2Seconds(sourceChannel, channelLength);

            Bass.ChannelSetPosition(sourceChannel, Bass.ChannelSeconds2Bytes(sourceChannel, channelLengthSeconds - 10));

            do
            {
                var channelActive = Bass.ChannelIsActive(playbackChannel);
                if (channelActive == PlaybackState.Stopped)
                {
                    break;
                }

                var channelPosition = Bass.ChannelGetPosition(playbackChannel);
                var channelPositionSeconds = Bass.ChannelBytes2Seconds(playbackChannel, channelPosition);

                if (channelPositionSeconds >= 20)
                {
                    break;
                }

                Thread.Sleep(1000);
            } while (true);

            if (!BassGapless.ChannelRemove(sourceChannel))
            {
                Assert.Fail("Failed to remove queued gapless channel.");
            }

            BassGaplessCd.Disable();

            Bass.StreamFree(sourceChannel);
            Bass.StreamFree(playbackChannel);

            BassGapless.Free();
            BassGaplessCd.Free();
            Bass.Free();
        }

        /// <summary>
        /// Init/Free called out of sequence does not crash.
        /// </summary>
        [Test]
        public void Test002()
        {
            Assert.IsFalse(BassGaplessCd.Free());
            Assert.IsTrue(BassGaplessCd.Init());
            Assert.IsFalse(BassGaplessCd.Init());
            Assert.IsTrue(BassGaplessCd.Free());
            Assert.IsFalse(BassGaplessCd.Free());
        }

        [Test]
        public void Test003()
        {
            try
            {
                if (!BassGaplessCd.Init())
                {
                    Assert.Fail("Failed to initialize GAPLESS.");
                }

                BassGaplessCd.Enable(0, BassFlags.Default);
                BassGaplessCd.Disable();

            }
            finally
            {
                BassGaplessCd.Free();
            }
        }
    }
}
