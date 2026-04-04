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
 * VICy Context classes
 */

#ifndef _VICY_VICYMAIN_CONTEXT_HPP
#define _VICY_VICYMAIN_CONTEXT_HPP

#include <string>

#include <VICyMain/Options.hpp>

using namespace VICy::VICyMain;

namespace VICy::VICyMain {
    class Context : public Options {
    public:
        Context();
        Context(int argc, char *argv[]);
        ~Context();

        const char *GetName();
        const char *GetVersion();
        const char *GetLicense();
        const char *GetHelp();

        int Execute();

    private:
        std::string m_name;
        std::string m_nameShort;
        std::string m_version;
        std::string m_license;
        std::string m_help;
        bool m_error;

        void setInformation();
        void showTryHelp();
    };
}

#endif
