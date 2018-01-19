# BASS_GAPLESS
A gapless playback plugin for BASS with .NET bindings.

bass.dll is required for native projects.
ManagedBass is required for .NET projects.

A simple example;

```c
#include <stdio.h>
#include "../bass_gapless/bass_gapless.h"
#include "../bass_gapless/queue.h"

int main(int argc, char **argv) {
	int output_rate = 192000;
	DWORD source_channel_1;
	DWORD source_channel_2;
	DWORD playback_channel;
	BASS_CHANNELINFO channel_info;

	if (!BASS_Init(-1, output_rate, 0, 0, NULL)) {
		return 1;
	}

	if (!BASS_GAPLESS_Init()) {
		return 1;
	}

	if (!BASS_GAPLESS_EnableEvents(event_handler)) {
		printf("Failed to enable GAPLESS events.\n");
		return 1;
	}

	source_channel_1 = BASS_StreamCreateFile(FALSE, "D:\\Source\\Prototypes\\Resources\\01 Botanical Dimensions.flac", 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
	if (source_channel_1 == 0) {
		return 1;
	}

	source_channel_2 = BASS_StreamCreateFile(FALSE, "D:\\Source\\Prototypes\\Resources\\02 Outer Shpongolia.flac", 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
	if (source_channel_2 == 0) {
		return 1;
	}

	if (!BASS_GAPLESS_ChannelEnqueue(source_channel_1)) {
		return 1;
	}
	if (!BASS_GAPLESS_ChannelEnqueue(source_channel_2)) {
		return 1;
	}

	BASS_ChannelGetInfo(source_channel_1, &channel_info);

	playback_channel = BASS_GAPLESS_StreamCreate(channel_info.freq, channel_info.chans, BASS_SAMPLE_FLOAT, NULL);
	if (playback_channel == 0) {
		return 1;
	}

	if (!BASS_ChannelPlay(playback_channel, FALSE)) {
		return 1;
	}

	do {
		DWORD channel_active = BASS_ChannelIsActive(playback_channel);
		if (channel_active == BASS_ACTIVE_STOPPED) {
			break;
		}

		Sleep(1000);
	} while (TRUE);

	BASS_StreamFree(source_channel_1);
	BASS_StreamFree(source_channel_2);
	BASS_StreamFree(playback_channel);
	BASS_GAPLESS_Free();
	BASS_Free();
}
```
