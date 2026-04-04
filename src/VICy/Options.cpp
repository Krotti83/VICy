// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * VICy - Open Cognitive Assist System
 *
 * Copyright (C) 2026 Johannes Krottmayer <krotti83@proton.me>
 *
 * This file is part of VICy's core modules.
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
 * VICy CLI Options
 */

#include <iostream>

#include <VICyMain/Options.hpp>

using namespace VICy::VICyMain;

Options::Options()
{
    m_default = true;
    m_options = nullptr;
    m_error = false;
}

Options::Options(int argc, char *argv[])
{
    m_default = false;
    m_options = new GetOptions(argc, argv);
    parseOptions();
}

Options::~Options()
{
    if (m_default != true) {
        if (m_options != nullptr) {
            delete m_options;
        }
    }
}

bool Options::Error()
{
    return m_error;
}

bool Options::parseOptions()
{
    int ret;
    int ret_index;

    if (!m_options) {
        return false;
    }

    ret_index = m_options->addOption(0, GetOptionsArgumentsEnum::No, 'H', "help");

    if (ret_index == -1) {
        m_error = true;
        return false;
    }

    ret_index = m_options->addOption(1, GetOptionsArgumentsEnum::No, 'V', "version");

    if (ret_index == -1) {
        m_error = true;
        return false;
    }

    ret_index = m_options->addOption(2, GetOptionsArgumentsEnum::Required, 'C', "controladdr");

    if (ret_index == -1) {
        m_error = true;
        return false;
    }

    ret_index = m_options->addOption(3, GetOptionsArgumentsEnum::Required, 'c', "controlport");

    if (ret_index == -1) {
        m_error = true;
        return false;
    }

    ret_index = m_options->addOption(4, GetOptionsArgumentsEnum::Required, 'U', "useraddr");

    if (ret_index == -1) {
        m_error = true;
        return false;
    }

    ret_index = m_options->addOption(5, GetOptionsArgumentsEnum::Required, 'u', "userport");

    if (ret_index == -1) {
        m_error = true;
        return false;
    }

    if (m_options->error()) {
        std::cout << m_options->what() << std::endl;
        std::cout << std::endl;
        m_errorCLI = true;
        return false;
    }

    m_options->exec();

    while ((ret = m_options->nextOption()) != -1) {
        switch (ret) {
            case 0:
                m_showHelp = true;
                return true;
            case 1:
                m_showVersion = true;
                return true;
            default:
                break;
        }
    }

    return true;
}
