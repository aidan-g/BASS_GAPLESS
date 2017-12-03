using ManagedBass.Asio;
using NUnit.Framework;
using System;
using System.Threading;

namespace ManagedBass.Gapless.Asio.Test
{
    [TestFixture]
    public class Tests
    {
        [Test]
        public void Test001()
        {
            if (!Bass.Init(Bass.NoSoundDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassAsio.Init(0, AsioInitFlags.Thread))
            {
                Assert.Fail(string.Format("Failed to initialize ASIO: {0}", Enum.GetName(typeof(Errors), BassAsio.LastError)));
            }

            if (!BassGapless.Init() || !BassGaplessAsio.Init())
            {
                Assert.Fail("Failed to initialize GAPLESS.");
            }

            var sourceChannel1 = Bass.CreateStream(@"C:\Source\Prototypes\Resources\01 Botanical Dimensions.flac", 0, 0, BassFlags.Decode | BassFlags.Float);
            if (sourceChannel1 == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var sourceChannel2 = Bass.CreateStream(@"C:\Source\Prototypes\Resources\02 Outer Shpongolia.flac", 0, 0, BassFlags.Decode | BassFlags.Float);
            if (sourceChannel2 == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var channelInfo = default(ChannelInfo);
            if (!Bass.ChannelGetInfo(sourceChannel1, out channelInfo))
            {
                Assert.Fail(string.Format("Failed to get channel info: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassGapless.ChannelEnqueue(sourceChannel1))
            {
                Assert.Fail("Failed to add stream to gapless queue.");
            }

            if (!BassGapless.ChannelEnqueue(sourceChannel2))
            {
                Assert.Fail("Failed to add stream to gapless queue.");
            }

            var sourceChannelCount = default(int);
            var sourceChannels = BassGapless.GetChannels(out sourceChannelCount);

            if (sourceChannelCount != 2)
            {
                Assert.Fail("Gapless reports unexpected queued channel count.");
            }

            if (sourceChannels[0] != sourceChannel1 || sourceChannels[1] != sourceChannel2)
            {
                Assert.Fail("Gapless reports unexpected queued channel handles.");
            }

            {
                var ok = true;
                ok &= BassGaplessAsio.ChannelEnable(false, 0);
                ok &= BassAsio.ChannelJoin(false, 1, 0);
                ok &= BassAsio.ChannelSetFormat(false, 0, AsioSampleFormat.Float);
                ok &= BassAsio.ChannelSetRate(false, 0, channelInfo.Frequency);
                if (!ok)
                {
                    Assert.Fail("Failed to configure ASIO.");
                }
            }

            if (!BassAsio.Start())
            {
                Assert.Fail(string.Format("Failed to start ASIO: {0}", Enum.GetName(typeof(Errors), BassAsio.LastError)));
            }

            var channelLength = Bass.ChannelGetLength(sourceChannel1);
            var channelLengthSeconds = Bass.ChannelBytes2Seconds(sourceChannel1, channelLength);

            Bass.ChannelSetPosition(sourceChannel1, Bass.ChannelSeconds2Bytes(sourceChannel1, channelLengthSeconds - 10));

            do
            {
                var channelActive1 = Bass.ChannelIsActive(sourceChannel1);
                var channelActive2 = Bass.ChannelIsActive(sourceChannel2);
                if (channelActive1 == PlaybackState.Stopped && channelActive2 == PlaybackState.Stopped)
                {
                    break;
                }

                var channelPosition = Bass.ChannelGetPosition(sourceChannel2);
                var channelPositionSeconds = Bass.ChannelBytes2Seconds(sourceChannel2, channelPosition);

                if (channelPositionSeconds >= 10)
                {
                    break;
                }

                Thread.Sleep(1000);
            } while (true);

            if (BassGapless.ChannelRemove(sourceChannel1))
            {
                Assert.Fail("Queued gapless channel should have been removed.");
            }

            if (!BassGapless.ChannelRemove(sourceChannel2))
            {
                Assert.Fail("Failed to remove queued gapless channel.");
            }

            BassAsio.Stop();

            Bass.StreamFree(sourceChannel1);
            Bass.StreamFree(sourceChannel2);
            BassGapless.Free();
            BassGaplessAsio.Free();
            BassAsio.Free();
            Bass.Free();
        }

        [Test]
        public void Test002()
        {
            Assert.IsFalse(BassGapless.Free());
            Assert.IsTrue(BassGapless.Init());
            Assert.IsFalse(BassGapless.Init());
            Assert.IsTrue(BassGapless.Free());
            Assert.IsFalse(BassGapless.Free());
        }
    }
}
