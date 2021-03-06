﻿using NUnit.Framework;
using System;
using System.IO;
using System.Linq;
using System.Threading;

namespace ManagedBass.Gapless.Test
{
    [TestFixture]
    public class Tests
    {
        const int BASS_STREAMPROC_ERR = -1;

        const int BASS_STREAMPROC_EMPTY = 0;

        const int BASS_STREAMPROC_END = -2147483648; //0x80000000;

        private static readonly string Location = Path.GetDirectoryName(typeof(Tests).Assembly.Location);

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

            var sourceChannel1 = Bass.CreateStream(Path.Combine(Location, "Media", "01 Botanical Dimensions.m4a"), 0, 0, BassFlags.Decode | BassFlags.Float);
            if (sourceChannel1 == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var sourceChannel2 = Bass.CreateStream(Path.Combine(Location, "Media", "02 Outer Shpongolia.m4a"), 0, 0, BassFlags.Decode | BassFlags.Float);
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

            var playbackChannel = BassGapless.StreamCreate(channelInfo.Frequency, channelInfo.Channels, BassFlags.Float);
            if (playbackChannel == 0)
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

            if (!BassGapless.DisableEvents())
            {
                Assert.Fail("Failed to disable GAPLESS events.");
            }

            Bass.StreamFree(sourceChannel1);
            Bass.StreamFree(sourceChannel2);
            Bass.StreamFree(playbackChannel);
            BassGapless.Free();
            Bass.Free();
        }

        /// <summary>
        /// MAX_GAPLESS_STREAMS (10) resampler channels are supported.
        /// </summary>
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

        /// <summary>
        /// Random channel removal is OK.
        /// </summary>
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

        /// <summary>
        /// Init/Free called out of sequence does not crash.
        /// </summary>
        [Test]
        public void Test004()
        {
            Assert.IsFalse(BassGapless.Free());
            Assert.IsTrue(BassGapless.Init());
            Assert.IsFalse(BassGapless.Init());
            Assert.IsTrue(BassGapless.Free());
            Assert.IsFalse(BassGapless.Free());
        }

        /// <summary>
        /// 1) Reading channel when queue is empty returns an error (-1).
        /// 2) When the queue is populated and the position reset everything continues to work.
        /// </summary>
        [Test]
        public void Test005()
        {
            try
            {
                if (!Bass.Init(Bass.NoSoundDevice))
                {
                    Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                var sourceChannel = Bass.CreateStream(Path.Combine(Location, "Media", "01 Botanical Dimensions.m4a"), 0, 0, BassFlags.Decode);
                if (sourceChannel == 0)
                {
                    Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                var channelInfo = default(ChannelInfo);
                if (!Bass.ChannelGetInfo(sourceChannel, out channelInfo))
                {
                    Assert.Fail(string.Format("Failed to get channel info: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                if (!BassGapless.Init())
                {
                    Assert.Fail("Failed to initialize GAPLESS.");
                }

                var playbackChannel = BassGapless.StreamCreate(channelInfo.Frequency, channelInfo.Channels, BassFlags.Decode);
                if (playbackChannel == 0)
                {
                    Assert.Fail(string.Format("Failed to create playback stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                {
                    byte[] buffer = new byte[1024];
                    int count = Bass.ChannelGetData(playbackChannel, buffer, buffer.Length);
                    Assert.AreEqual(-1, count);
                }

                if (!BassGapless.ChannelEnqueue(sourceChannel))
                {
                    Assert.Fail("Failed to add stream to gapless queue.");
                }

                Bass.ChannelSetPosition(playbackChannel, 0);

                {
                    byte[] buffer = new byte[1024];
                    int count = Bass.ChannelGetData(playbackChannel, buffer, buffer.Length);
                    Assert.AreEqual(buffer.Length, count);
                }

                Bass.StreamFree(sourceChannel);
                Bass.StreamFree(playbackChannel);
            }
            finally
            {
                BassGapless.Free();
                Bass.Free();
            }
        }

        /// <summary>
        /// Failure to read from source does not mark the stream as "complete" when <see cref="BassGaplessAttriubute.KeepAlive"/> is specified.
        /// </summary>
        /// <param name="err"></param>
        //[TestCase(BASS_STREAMPROC_ERR)] //-1 doesn't seem to be understood.
        [TestCase(BASS_STREAMPROC_EMPTY)]
        [TestCase(BASS_STREAMPROC_END)]
        public void Test006(int? err)
        {
            try
            {
                if (!Bass.Init(Bass.NoSoundDevice, 44100))
                {
                    Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                if (!BassGapless.Init())
                {
                    Assert.Fail("Failed to initialize GAPLESS.");
                }

                var channel = Bass.CreateStream(48000, 2, BassFlags.Decode, (handle, buffer, length, user) =>
                {
                    if (err.HasValue)
                    {
                        return err.Value;
                    }
                    return length;
                });

                var gapless = BassGapless.StreamCreate(48000, 2, BassFlags.Decode);
                BassGapless.ChannelEnqueue(channel);
                BassGapless.SetConfig(BassGaplessAttriubute.KeepAlive, true);

                {
                    var buffer = new byte[1024];
                    var length = Bass.ChannelGetData(gapless, buffer, buffer.Length);
                    Assert.AreEqual(0, length);
                }

                err = null;
                Bass.ChannelSetPosition(channel, 0);
                BassGapless.ChannelEnqueue(channel);

                {
                    var buffer = new byte[1024];
                    var length = Bass.ChannelGetData(gapless, buffer, buffer.Length);
                    Assert.AreEqual(buffer.Length, length);
                }

                Bass.StreamFree(channel);
                Bass.StreamFree(gapless);
            }
            finally
            {
                BassGapless.Free();
                Bass.Free();
            }
        }

        [Test]
        public void Test007()
        {

            try
            {
                if (!Bass.Init(Bass.NoSoundDevice, 44100))
                {
                    Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                if (!BassGapless.Init())
                {
                    Assert.Fail("Failed to initialize GAPLESS.");
                }

                BassGapless.SetConfig(BassGaplessAttriubute.KeepAlive, true);
                BassGapless.SetConfig(BassGaplessAttriubute.BlockingEvents, true);

                {
                    var value = default(int);
                    BassGapless.GetConfig(BassGaplessAttriubute.KeepAlive, out value);
                    Assert.AreEqual(Convert.ToInt32(true), value);
                }

                {
                    var value = default(int);
                    BassGapless.GetConfig(BassGaplessAttriubute.BlockingEvents, out value);
                    Assert.AreEqual(Convert.ToInt32(true), value);
                }
            }
            finally
            {
                BassGapless.Free();
                Bass.Free();
            }
        }
    }
}
