//-----------------------------------------------------------------------------//
// Audio Capture - Spare time development for fun                              //
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

#if !defined(__AUDIO_CAPTURE_HPP__)
#define __AUDIO_CAPTURE_HPP__

#include "audio/audio_context.hpp"
#include "tools/non_copyable.hpp"

#include <cstdint>
#include <memory>

class ma_context;
class ma_device;

namespace tools
{
    class double_buffer;
}

namespace audio
{
    class audio_capture : public tools::non_copyable
    {
    public:
        audio_capture() = delete;
        explicit audio_capture(std::shared_ptr<tools::double_buffer> buffer, std::shared_ptr<audio_context> context);
        ~audio_capture();

        void start();
        void stop();

        void on_capture(const sample_t* input, const std::size_t frames);

    private:
        std::shared_ptr<audio_context> m_context;
        std::shared_ptr<tools::double_buffer> m_buffer;
        std::unique_ptr<ma_device> m_capture_device;
    };
}

#endif //  __AUDIO_CAPTURE_HPP__
