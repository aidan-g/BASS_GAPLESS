using NUnit.Framework;
using System;
using System.Linq;
using System.Threading;

namespace ManagedBass.Gapless.Test
{
    [TestFixture]
    public class Tests
    {
        [Test]
        public void Test001()
        {
            if (!Bass.Init(Bass.DefaultDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassGapless.Init())
            {
                Assert.Fail("Failed to initialize GAPLESS.");
            }

            if (!BassGapless.EnableEvents(e =>
            {
                //Nothing to do for now.
            }))
            {
                Assert.Fail("Failed to enable GAPLESS events.");
            }

            var sourceChannel1 = Bass.CreateStream(@"C:\Source\Prototypes\Resources\01 Botanical Dimensions.flac", 0, 0, BassFlags.Decode | BassFlags.Float);
            if (sourceChannel1 == -1)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var sourceChannel2 = Bass.CreateStream(@"C:\Source\Prototypes\Resources\02 Outer Shpongolia.flac", 0, 0, BassFlags.Decode | BassFlags.Float);
            if (sourceChannel2 == -1)
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

            var playbackChannel = BassGapless.StreamCreate(channelInfo.Frequency, channelInfo.Channels, BassFlags.Float);
            if (playbackChannel == -1)
            {
                Assert.Fail(string.Format("Failed to create playback stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!Bass.ChannelPlay(playbackChannel))
            {
                Assert.Fail(string.Format("Failed to play stream:  {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var channelLength = Bass.ChannelGetLength(sourceChannel1);
            var channelLengthSeconds = Bass.ChannelBytes2Seconds(sourceChannel1, channelLength);

            Bass.ChannelSetPosition(sourceChannel1, Bass.ChannelSeconds2Bytes(sourceChannel1, channelLengthSeconds - 10));

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

            if (BassGapless.ChannelRemove(sourceChannel1))
            {
                Assert.Fail("Queued gapless channel should have been removed.");
            }

            if (!BassGapless.ChannelRemove(sourceChannel2))
            {
                Assert.Fail("Failed to remove queued gapless channel.");
            }

            Bass.StreamFree(sourceChannel1);
            Bass.StreamFree(sourceChannel2);
            Bass.StreamFree(playbackChannel);
            BassGapless.Free();
            Bass.Free();
        }

        [Test]
        public void Test002()
        {
            try
            {
                if (!BassGapless.Init())
                {
                    Assert.Fail("Failed to initialize GAPLESS.");
                }

                var input = Enumerable.Range(0, 11).ToList();

                for (var a = 0; a < 11; a++)
                {
                    var success = BassGapless.ChannelEnqueue(input[a]);
                    if (a < 10)
                    {
                        Assert.IsTrue(success);
                    }
                    else
                    {
                        //Should only allow 10 channels.
                        Assert.IsFalse(success);
                    }
                }

                var count = default(int);
                var output = BassGapless.GetChannels(out count);
                Assert.AreEqual(10, count);
                for (var a = 0; a < 10; a++)
                {
                    Assert.AreEqual(input[a], output[a]);
                }
            }
            finally
            {
                BassGapless.Free();
            }
        }

        [Test]
        public void Test003()
        {
            try
            {
                if (!BassGapless.Init())
                {
                    Assert.Fail("Failed to initialize GAPLESS.");
                }

                var input = Enumerable.Range(0, 10).ToList();

                for (var a = 0; a < 10; a++)
                {
                    BassGapless.ChannelEnqueue(input[a]);
                }

                var channel1 = input[2];
                var channel2 = input[5];
                var channel3 = input[7];

                input.Remove(channel1);
                input.Remove(channel2);
                input.Remove(channel3);
                BassGapless.ChannelRemove(channel1);
                BassGapless.ChannelRemove(channel2);
                BassGapless.ChannelRemove(channel3);

                var count = default(int);
                var output = BassGapless.GetChannels(out count);
                Assert.AreEqual(7, count);
                for (var a = 0; a < 7; a++)
                {
                    Assert.AreEqual(input[a], output[a]);
                }
            }
            finally
            {
                BassGapless.Free();
            }
        }

        [Test]
        public void Test004()
        {
            Assert.IsFalse(BassGapless.Free());
            Assert.IsTrue(BassGapless.Init());
            Assert.IsFalse(BassGapless.Init());
            Assert.IsTrue(BassGapless.Free());
            Assert.IsFalse(BassGapless.Free());
        }
    }
}
