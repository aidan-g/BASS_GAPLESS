#include <stdio.h>
#include "../bass_gapless/bass_gapless.h"
#include "../bass_gapless/queue.h"

VOID CALLBACK event_handler(GS_EVENT_ARGS args) {
	printf("Gapless event: %d => %d => %d\n", args.event_type, args.channel_1, args.channel_2);
}

int main(int argc, char **argv) {
	int output_rate = 192000;
	DWORD source_channel_1;
	DWORD source_channel_2;
	DWORD* source_channels;
	DWORD source_channel_count;
	DWORD playback_channel;
	BASS_CHANNELINFO channel_info;
	QWORD channel_length;
	double channel_length_seconds;

	//Start BASS.
	if (!BASS_Init(-1, output_rate, 0, 0, NULL)) {
		printf("Failed to initialize BASS: %d\n", BASS_ErrorGetCode());
		return 1;
	}

	//Start GAPLESS.
	if (!BASS_GAPLESS_Init()) {
		printf("Failed to initialize GAPLESS.\n");
		return 1;
	}

	if (!BASS_GAPLESS_EnableEvents(event_handler)) {
		printf("Failed to enable GAPLESS events.\n");
		return 1;
	}

		//Create a DECODE stream for a crappy song.
		source_channel_1 = BASS_StreamCreateFile(FALSE, "D:\\Source\\Prototypes\\Resources\\01 Botanical Dimensions.flac", 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
	if (source_channel_1 == 0) {
		printf("Failed to create source stream: %d\n", BASS_ErrorGetCode());
		return 1;
	}

	//Create a DECODE stream for another crappy song.
	source_channel_2 = BASS_StreamCreateFile(FALSE, "D:\\Source\\Prototypes\\Resources\\02 Outer Shpongolia.flac", 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
	if (source_channel_2 == 0) {
		printf("Failed to create source stream: %d\n", BASS_ErrorGetCode());
		return 1;
	}

	//Add the source channels to the queue.
	if (!BASS_GAPLESS_ChannelEnqueue(source_channel_1)) {
		printf("Failed to add stream to gapless queue.\n");
		return 1;
	}
	if (!BASS_GAPLESS_ChannelEnqueue(source_channel_2)) {
		printf("Failed to add stream to gapless queue.\n");
		return 1;
	}

	source_channels = BASS_GAPLESS_GetChannels(&source_channel_count);
	if (source_channel_count != 2) {
		printf("Gapless reports unexpected queued channel count.\n");
		return 1;
	}

	if (source_channels[0] != source_channel_1 || source_channels[1] != source_channel_2) {
		printf("Gapless reports unexpected queued channel handles.\n");
		return 1;
	}

	//Get some channel info (let's assume both are the same).
	BASS_ChannelGetInfo(source_channel_1, &channel_info);

	//Create the gapless stream.
	playback_channel = BASS_GAPLESS_StreamCreate(channel_info.freq, channel_info.chans, BASS_SAMPLE_FLOAT, NULL);
	if (playback_channel == 0) {
		printf("Failed to create playback stream: %d\n", BASS_ErrorGetCode());
	}

	//Play the gapless stream.
	if (!BASS_ChannelPlay(playback_channel, FALSE)) {
		printf("Failed to play stream: %d\n", BASS_ErrorGetCode());
		return 1;
	}

	//Calculate the source length.
	channel_length = BASS_ChannelGetLength(source_channel_1, BASS_POS_BYTE);
	channel_length_seconds = BASS_ChannelBytes2Seconds(source_channel_1, channel_length);

	//Set the position ten seconds from the end so we can hear the change.
	BASS_ChannelSetPosition(source_channel_1, BASS_ChannelSeconds2Bytes(source_channel_1, channel_length_seconds - 10), BASS_POS_BYTE);

	do {
		QWORD channel_position;
		double channel_position_seconds;

		//Check the channel is active otherwise break.
		DWORD channel_active = BASS_ChannelIsActive(playback_channel);
		if (channel_active == BASS_ACTIVE_STOPPED) {
			break;
		}

		//Calculate the source position and write it out.
		channel_position = BASS_ChannelGetPosition(playback_channel, BASS_POS_BYTE);
		channel_position_seconds = BASS_ChannelBytes2Seconds(playback_channel, channel_position);
		printf("%d/%d\n", (int)channel_position_seconds, (int)channel_length_seconds);

		if (channel_position_seconds >= 20) {
			break;
		}

		Sleep(1000);
	} while (TRUE);

	if (BASS_GAPLESS_ChannelRemove(source_channel_1)) {
		printf("Queued gapless channel should have been removed.\n");
		return 1;
	}

	if (!BASS_GAPLESS_ChannelRemove(source_channel_2)) {
		printf("Failed to remove queued gapless channel.\n");
		return 1;
	}

	BASS_StreamFree(source_channel_1);
	BASS_StreamFree(source_channel_2);
	BASS_StreamFree(playback_channel);
	BASS_GAPLESS_Free();
	BASS_Free();
}