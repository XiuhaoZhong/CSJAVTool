#include "CSJAudioPlayer.h"

#include "CSJAudioPlayerImpl/CSJAudioPlayerDSImpl.h"

CSJSharedAudioPlayer CSJAudioPlayer::getCSJAudioPlayer(CSJAudioPlayerTpye type /* = CSJAUDIOPLAYER_DSHOW */) {
    switch (type) {
    case CSJAUDIOPLAYER_DSHOW:
        return std::make_shared<CSJAudioPlayerDSImpl>();
    default:
        return std::make_shared<CSJAudioPlayerDSImpl>();
    }

    return nullptr;
}