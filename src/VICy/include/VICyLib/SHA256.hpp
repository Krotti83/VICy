// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * VICy - Open Cognitive Assist System
 *
 * Copyright (C) 2026 Johannes Krottmayer <krotti83@proton.me>
 *
 * This file is part of VICy's core library modules.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * VICy SHA256 Hash classes
 */

#ifndef _VICY_VICYLIB_SHA256_HPP
#define _VICY_VICYLIB_SHA256_HPP

#include <array>
#include <string>
#include <queue>
#include <vector>

namespace VICy::VICyLib {
    class SHA256 {
    public:
        SHA256();
        SHA256(const SHA256 &sha256);
        SHA256(void *buf, size_t len);
        SHA256(std::queue<unsigned char> buf);
        SHA256(std::vector<unsigned char> buf);
        ~SHA256();

        void Calc(void *buf, size_t len, bool discard = false);
        void Calc(std::queue<unsigned char> buf, bool discard = false);
        void Calc(std::vector<unsigned char> buf, bool discard = false);

        void ClearHash();

        bool ValidHash();

        std::array<unsigned char, 32> GetHashArray();
        std::vector<unsigned char> GetHashVector();
        std::string GetHashString();

        bool operator==(const SHA256 &sha256);
        bool operator!=(const SHA256 &sha256);
        SHA256& operator=(const SHA256 &sha256);

    private:
        void init();
        void transform(unsigned char msg[64]);
        void sum(void *buf, size_t len);
        void sum(std::queue<unsigned char> buf);
        void hash();

        unsigned int *m_hashCurrent;
        unsigned int *m_hashLast;
        std::array<unsigned char, 32> m_hash;
        bool m_initDone;
        bool m_sumDone;
        bool m_hashDone;
        bool m_validData;
    };
}

#endif
