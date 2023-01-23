//-----------------------------------------------------------------------------//
// Audio Playback - Spare time development for fun                             //
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

#include "audio_playback.hpp"
#include "audio_context.hpp"
#include "tools/double_buffer.hpp"

#include "miniaudio/miniaudio.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <numeric>
#include <type_traits>
#include <utility>

namespace audio
{
    namespace
    {
        void frame_playback(ma_device* device, void* output, const void* input, ma_uint32 frame_count)
        {
            (void)input;

            auto* samples = reinterpret_cast<sample_t*>(output);
            auto* instance = reinterpret_cast<audio_playback*>(device->pUserData);
            if (instance)
            {
                instance->on_playback(samples, static_cast<std::size_t>(frame_count));
            }
        }
    }

    audio_playback::audio_playback(std::shared_ptr<tools::double_buffer> buffer, std::shared_ptr<audio_context> context)
        : m_context(context)
        , m_buffer(buffer)
        , m_playback_device(std::make_unique<ma_device>())
    {
        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.pDeviceID = nullptr;

        if constexpr (std::is_same<float, sample_t>::value)
        {
            config.playback.format = ma_format_f32;
        }
        else if constexpr (std::is_same<std::int16_t, sample_t>::value)
        {
            config.playback.format = ma_format_s16;
        }
        else if constexpr (std::is_same<std::int32_t, sample_t>::value)
        {
            config.playback.format = ma_format_s32;
        }
        else
        {
            config.playback.format = ma_format_unknown;
        }

        config.playback.channels = audio_channels;
        config.sampleRate = audio_frequency;
        config.periodSizeInMilliseconds = audio_period_ms;
        config.dataCallback = frame_playback;
        config.pUserData = this;
        config.performanceProfile = ma_performance_profile_low_latency;

        if (MA_SUCCESS != ma_device_init(&(m_context->context()), &config, m_playback_device.get()))
        {
            std::cerr << "Failed to initialize playback device" << std::endl;
            return;
        }

        std::cout << "Playback Device " << m_playback_device->playback.name << std::endl;
    }

    audio_playback::~audio_playback() { ma_device_uninit(m_playback_device.get()); }

    void audio_playback::start()
    {
        if (MA_SUCCESS != ma_device_start(m_playback_device.get()))
        {
            std::cerr << "Failed to start playback device" << std::endl;
            return;
        }
    }

    void audio_playback::stop()
    {
        m_buffer->swap(); // wakeup

        if (MA_SUCCESS != ma_device_stop(m_playback_device.get()))
        {
            std::cerr << "Failed to stop playback device" << std::endl;
            return;
        }
    }

    void audio_playback::on_playback(sample_t* output, const std::size_t frames)
    {
        constexpr const auto timeout = std::chrono::duration<int, std::micro>(audio_period_ms * 1000);
        m_buffer->sync(timeout); // wait for an available frame

        const auto* input = reinterpret_cast<sample_t*>(m_buffer->front());

        const auto nb_samples_in_buffer = static_cast<std::size_t>(m_buffer->frame_size() / (audio_channels * sizeof(sample_t)));
        const auto nb_frames = (frames < nb_samples_in_buffer) ? frames : nb_samples_in_buffer;

        // compute attenuated sidetone (-6dB)
        if constexpr (std::is_same<float, sample_t>::value)
        {
            std::transform(input, input + nb_frames * audio_channels, output, [](sample_t v) { return 0.5f * v; });
        }
        else if constexpr (std::is_same<std::int16_t, sample_t>::value || std::is_same<std::int32_t, sample_t>::value)
        {
            std::transform(input, input + nb_frames * audio_channels, output, [](sample_t v) { return v / 2; });
        }

#if defined(DISPLAY_PROGRESS)
        std::cout << "-";
#endif
    }


}
