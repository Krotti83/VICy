// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * VICy - Open Cognitive Assist System
 *
 * Copyright (C) 2026 Johannes Krottmayer <krotti83@proton.me>
 *
 * This file is part of VICy core modules.
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
 * VICy CLI Options classes
 */

#ifndef _VICY_VICYMAIN_OPTIONS_HPP
#define _VICY_VICYMAIN_OPTIONS_HPP

#include <VICyLibGen/GetOptions.hpp>

using namespace VICy::LibGen;

namespace VICy::VICyMain {
    class Options {
    public:
        Options();
        Options(int argc, char *argv[]);
        ~Options();

    protected:
        bool m_showHelp;
        bool m_showVersion;
        bool m_errorCLI;

        bool Error();

    private:
        bool m_error;
        bool m_default;
        GetOptions *m_options;

        bool parseOptions();
    };
}

#endif
