//-----------------------------------------------------------------------------//
// Audio Context - Spare time development for fun                              //
// (c) 2023 Laurent Lardinois https://be.linkedin.com/in/laurentlardinois      //
//                                                                             //
// https://github.com/type-one/SideTone                                        //
//                                                                             //
// This software is provided 'as-is', without any express or implied           //
// warranty.In no event will the authors be held liable for any damages        //
// arising from the use of this software.                                      //
//                                                                             //
// Permission is granted to anyone to use this software for any purpose,       //
// including commercial applications, and to alter itand redistribute it       //
// freely, subject to the following restrictions :                             //
//                                                                             //
// 1. The origin of this software must not be misrepresented; you must not     //
// claim that you wrote the original software.If you use this software         //
// in a product, an acknowledgment in the product documentation would be       //
// appreciated but is not required.                                            //
// 2. Altered source versions must be plainly marked as such, and must not be  //
// misrepresented as being the original software.                              //
// 3. This notice may not be removed or altered from any source distribution.  //
//-----------------------------------------------------------------------------//

#pragma once

#if !defined(__AUDIO_CONTEXT_HPP__)
#define __AUDIO_CONTEXT_HPP__

#include "tools/non_copyable.hpp"

#include <cstdint>
#include <memory>

class ma_context;

namespace audio
{
    // uncomment to display producer/consumer progress
    // #define DISPLAY_PROGRESS

    // some values tested on Win10/Corei7/AVX2
    // 16000, 20ms, 1
    // 44100, 16ms, 1
    // 44100, 20ms, 2
    // 48000, 50ms, 1
    // 22050, 20ms, 1 (in debug)
    // 22050, 12ms, 1 (in release)
    // 22050, 8ms, 1  (in release, SIMD/Most optimizations)
    // 22050, 14ms, 2
    // under VMware player 17 (Ubuntu 20 LTS) on Win10/Corei7/AVX2 host
    // 22050, 40ms, 1 (in debug)

    // parameters to tune
#if defined(_WIN32)
    constexpr const int audio_period_ms = 12; // more aggressive/lower latency
#else
    constexpr const int audio_period_ms = 40; // more conservative by default
#endif

    constexpr const bool audio_frame_sync = true; // sync producer/consumer
    constexpr const int audio_frequency = 22050;
    constexpr const int audio_frame_size = ((audio_frequency * audio_period_ms) / 1000);
    constexpr const int audio_channels = 1; // mono (1), stereo (2)
    using sample_t = std::int16_t;          // S16_LE
    // using sample_t = float; // F32

    class audio_context : public tools::non_copyable
    {
    public:
        audio_context();
        ~audio_context();

        ma_context& context();

    private:
        std::unique_ptr<ma_context> m_context;
    };
}

#endif //  __AUDIO_CONTEXT_HPP__
