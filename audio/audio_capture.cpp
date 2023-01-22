//-----------------------------------------------------------------------------//
// Audio Capture - Spare time development for fun                              //
// (c) 2023 Laurent Lardinois https://be.linkedin.com/in/laurentlardinois      //
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

#include "audio_capture.hpp"
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
        void frame_capture(ma_device* device, void* output, const void* input, ma_uint32 frame_count)
        {
            (void)output;
            
            const auto* samples = reinterpret_cast<const sample_t*>(input);
            auto* instance = reinterpret_cast<audio_capture*>(device->pUserData);
            if (instance)
            {
                instance->on_capture(samples, static_cast<std::size_t>(frame_count));
            }
        }
    }

    audio_capture::audio_capture(std::shared_ptr<tools::double_buffer> buffer, std::shared_ptr<audio_context> context)
        : m_context(context)
        , m_buffer(buffer)
        , m_capture_device(std::make_unique<ma_device>())
    {
        ma_device_config config = ma_device_config_init(ma_device_type_capture);
        config.capture.pDeviceID = nullptr;

        if constexpr (std::is_same<float, sample_t>::value)
        {
            config.capture.format = ma_format_f32;
        }
        else if constexpr (std::is_same<std::int16_t, sample_t>::value)
        {
            config.capture.format = ma_format_s16;
        }
        else if constexpr (std::is_same<std::int32_t, sample_t>::value)
        {
            config.capture.format = ma_format_s32;
        }
        else
        {
            config.capture.format = ma_format_unknown;
        }

        config.capture.channels = audio_channels;
        config.sampleRate = audio_frequency;
        config.periodSizeInMilliseconds = audio_period_ms;
        config.dataCallback = frame_capture;
        config.pUserData = this;
        config.performanceProfile = ma_performance_profile_low_latency;

        if (MA_SUCCESS != ma_device_init(&(m_context->context()), &config, m_capture_device.get()))
        {
            std::cerr << "Failed to initialize capture device" << std::endl;
            return;
        }

        std::cout << "Capture Device " << m_capture_device->capture.name << std::endl;
    }

    audio_capture::~audio_capture() { ma_device_uninit(m_capture_device.get()); }

    void audio_capture::start()
    {
        if (MA_SUCCESS != ma_device_start(m_capture_device.get()))
        {
            std::cerr << "Failed to start capture device" << std::endl;
            return;
        }
    }

    void audio_capture::stop()
    {
        if (MA_SUCCESS != ma_device_stop(m_capture_device.get()))
        {
            std::cerr << "Failed to stop capture device" << std::endl;
            return;
        }
    }

    void audio_capture::on_capture(const sample_t* input, const std::size_t frames)
    {
        auto* output = reinterpret_cast<sample_t*>(m_buffer->back());

        const auto nb_samples_in_buffer = static_cast<std::size_t>(m_buffer->frame_size() / (audio_channels * sizeof(sample_t)));
        const auto nb_frames = (frames < nb_samples_in_buffer) ? frames : nb_samples_in_buffer;

        std::memcpy(output, input, nb_frames * audio_channels * sizeof(sample_t));

        m_buffer->swap();

#if defined(DISPLAY_PROGRESS)
        std::cout << ".";
#endif
    }
}
