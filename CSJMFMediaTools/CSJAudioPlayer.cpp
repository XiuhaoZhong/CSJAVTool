#include "CSJAudioPlayer.h"

#include "CSJAudioPlayerImpl/CSJAudioPlayerDSImpl.h"
#include "CSJAudioPlayerImpl/CSJAudioPlayerCAImpl.h"

CSJSharedAudioPlayer CSJAudioPlayer::getCSJAudioPlayer(CSJAudioPlayerTpye type /* = CSJAUDIOPLAYER_DSHOW */) {
    switch (type) {
    case CSJAUDIOPLAYER_DSHOW:
        return std::make_shared<CSJAudioPlayerDSImpl>();
    case CSJAUDIOPLAYER_CA:
        return std::make_shared<CSJAudioPlayerCAImpl>();
    default:
        return std::make_shared<CSJAudioPlayerDSImpl>();
    }

    return nullptr;
}