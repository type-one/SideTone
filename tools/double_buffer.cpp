//-----------------------------------------------------------------------------//
// Helper - Spare time development for fun                                     //
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

#include "double_buffer.hpp"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <vector>

namespace tools
{
    double_buffer::double_buffer(std::size_t frame_size, bool sync)
        : m_sync_enabled(sync)
        , m_frame_sync(true)
    {
        m_internal_buffer.resize(frame_size << 1);
        std::fill(m_internal_buffer.begin(), m_internal_buffer.end(), 0);

        m_buffers[0] = m_internal_buffer.data();
        m_buffers[1] = m_buffers[0] + frame_size;
    }

    std::size_t double_buffer::frame_size() const { return m_internal_buffer.size() >> 1; }

    std::uint8_t* double_buffer::front()
    {
        unsigned long index = m_index.load();
        return m_buffers[index & 1];
    }

    std::uint8_t* double_buffer::back()
    {
        unsigned long index = m_index.load();
        return m_buffers[(index + 1) & 1];
    }

    void double_buffer::swap()
    {
        m_index.fetch_add(1);
        if (m_sync_enabled)
        {
            m_frame_sync.signal();
        }
    }

    void double_buffer::sync()
    {
        if (m_sync_enabled)
        {
            m_frame_sync.wait_for_signal();
        }
    }

    void double_buffer::sync(const std::chrono::duration<int, std::micro> timeout)
    {
        if (m_sync_enabled)
        {
            m_frame_sync.wait_for_signal(timeout);
        }
    }
}
