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
 * VICy Context
 */

#define STRINGIFY(x)    #x
#define TOSTRING(x)     STRINGIFY(x)
#define DO_EXPAND(x)    x ## 1
#define EXPAND(x)       DO_EXPAND(x)

#include <iostream>

#include <VICyMain/Context.hpp>

using namespace VICy::VICyMain;

void Context::showTryHelp()
{
    std::cout << "Try '" << m_nameShort << " --help' for more information." << std::endl;
}

void Context::setInformation()
{
    m_name = "VICy - Open Cognitive Assist System";
    m_nameShort = "VICy";

#if !defined(VICY_VERSION) || (EXPAND(VICY_VERSION) == 1)
    m_version = "v0.1.0.0";
#else
    return TOSTRING(VICY_VERSION);
#endif

    m_help = "[OPTION]\n"\
    "  -l , --longtermmemory=PATH        Set database (long-term memory) path\n"\
    "  -s , --shorttermmemory=PATH       Set snapshot (short-term memory) path\n"\
    "  -C , --controladdr=ADDR           Set control interface listen address\n"\
    "  -c , --controlport=PORT           Set control interface listen port\n"\
    "  -U , --useraddr=ADDR              Set user interface listen address\n"\
    "  -u , --userport=PORT              Set user interface listen port\n"\
    "  -D , --debuglevel=LEVEL           Set debug level\n"\
    "  -H , --help                       Show this text and exit\n"\
    "  -V , --version                    Show version and exit";

    m_license = "Copyright (C) 2026 Johannes Krottmayer <krotti83@proton.me>\n"\
    "\n"\
    "This program is free software: you can redistribute it and/or modify\n"\
    "it under the terms of the GNU General Public License as published by\n"\
    "the Free Software Foundation, either version 3 of the License, or\n"\
    "any later version.\n"\
    "\n"\
    "This program is distributed in the hope that it will be useful,\n"\
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"\
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"\
    "GNU General Public License for more details.\n"\
    "\n"\
    "You should have received a copy of the GNU General Public License\n"\
    "long with this program.  If not, see <https://www.gnu.org/licenses/>.";
}

Context::Context()
{
    setInformation();
    m_error = false;
}

Context::Context(int argc, char *argv[]) : Options(argc, argv)
{
    setInformation();
    m_error = false;
}

Context::~Context()
{
    // This destructor is currently a stub
}

const char *Context::GetName()
{
    return m_name.c_str();
}

const char *Context::GetVersion()
{
    return m_version.c_str();
}

const char *Context::GetHelp()
{
    return m_help.c_str();
}

const char *Context::GetLicense()
{
    return m_license.c_str();
}

int Context::Execute()
{
    if (Error()) {
        if (m_errorCLI) {
            showTryHelp();
            return -1;
        }
    }

    if (m_showHelp) {
        std::cout << "Usage: " << m_nameShort << " [OPTION]... [FILE]" << std::endl;
        std::cout << std::endl;
        std::cout << m_help << std::endl;
        return 0;
    }

    if (m_showVersion) {
        std::cout << m_name << " " << m_version << std::endl;
        std::cout << std::endl;
        std::cout << m_license << std::endl;
        return 0;
    }

    return 0;
}
