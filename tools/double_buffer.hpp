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

#pragma once

#if !defined(__DOUBLE_BUFFER_HPP__)
#define __DOUBLE_BUFFER_HPP__

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <vector>

#include "non_copyable.hpp"
#include "sync_object.hpp"

namespace tools
{
    class double_buffer : public non_copyable
    {
    public:
        explicit double_buffer(std::size_t frame_size, bool sync = true);
        ~double_buffer() = default;

        double_buffer() = delete;

        std::uint8_t* front();
        std::uint8_t* back();
        void swap();
        void sync();
        void sync(const std::chrono::duration<int, std::micro> timeout);

        std::size_t frame_size() const;

    private:
        std::vector<std::uint8_t> m_internal_buffer;
        std::array<std::uint8_t*, 2> m_buffers;
        std::atomic_ulong m_index;

        bool m_sync_enabled;
        sync_object m_frame_sync;
    };
}

#endif //  __DOUBLE_BUFFER_HPP__
