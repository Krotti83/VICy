// SPDX-License-Identifier: ISC
/**
 * 
 * VICy - Open Cognitive Assist System
 * 
 * This file is part of VICy's generic libraries.
 * 
 * Copyright (c) 2026 Johannes Krottmayer <krotti83@proton.me>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/**
 * VICy CLI Parser
 */

#ifndef _VICY_LIBGEN_GETOPTIONS_HPP
#define _VICY_LIBGEN_GETOPTIONS_HPP

#include <string>
#include <vector>

namespace VICy::LibGen {
    enum class GetOptionsArgumentsEnum {
        No,                 /* No arguments required */
        Required,           /* Argument required */
        Optional            /* Optional argument */
    };
    
    class GetOptions {
    public:
        GetOptions();
        GetOptions(int argc, char *argv[]);
        GetOptions(const std::vector<std::string> args);
        ~GetOptions();
        
        bool setRawOptions(int argc, char *argv[]);
        bool setRawOptions(const std::vector<std::string> args);
        int addOption(int index, enum GetOptionsArgumentsEnum type, char opt_short);
        int addOption(int index, enum GetOptionsArgumentsEnum type, std::string opt_long);
        int addOption(int index, enum GetOptionsArgumentsEnum type, char opt_short, std::string opt_long);
        bool exec();
        bool error();
        std::string what();
        int nextOption();
        std::vector<std::string> getArguments(int index);
        std::vector<std::string> getNoneOptions();
        
    private:
        struct opt_input {
            int index;
            char optShort;
            std::string optLong;
            enum GetOptionsArgumentsEnum type;
            std::vector<std::string> args;
            bool found;
        };
        
        std::vector<std::string> *m_rawOpts;
        std::vector<struct opt_input *> m_optsIn;
        std::vector<std::string> m_noneOpts;
        bool m_error;
        std::string m_what;
        
        bool is_optShort(const std::string opt);
        bool is_optShortSingle(const std::string opt);
        bool is_optLong(const std::string opt);
        bool has_optLongArg(const std::string opt);
        std::string get_optLongArg(const std::string opt);
        struct opt_input *find(char opt_short);
        struct opt_input *find(std::string opt_long);
        struct opt_input *find(int index);
        std::vector<char> get_optShort(const std::string opt);
        std::string get_optLong(const std::string opt);
        std::string get_optLongWithArg(const std::string opt);
        
    };
}

#endif
