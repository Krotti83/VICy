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

#include <VICyLibGen/GetOptions.hpp>

using namespace VICy::LibGen;

bool GetOptions::is_optShort(const std::string opt)
{
    if (opt.length() < 2)
        return false;
    
    if (opt[0] != '-')
        return false;
    
    if (opt[1] == '-')      /* Test if long option */
        return false;
    
    return true;
}

bool GetOptions::is_optShortSingle(const std::string opt)
{
    if (opt.length() != 2)
        return false;
    
    if (opt[0] != '-')
        return false;
    
    if (opt[1] == '-')      /* Test if long option */
        return false;
    
    return true;
}

bool GetOptions::is_optLong(const std::string opt)
{
    if (opt.length() < 3)
        return false;
    
    if (opt[0] != '-')
        return false;
    
    if (opt[1] != '-')
        return false;
    
    return true;
}

bool GetOptions::has_optLongArg(const std::string opt)
{
    size_t i;
    
    if (opt.length() < 5)
        return false;
    
    if (opt[0] != '-')
        return false;
    
    if (opt[1] != '-')
        return false;
    
    for (i = 3; i < opt.length(); i++) {
        if (opt[i] == '=') {
            if (i == (opt.length() - 1)) {
                return false;
            } else {
                return true;
            }
        }
    }
    
    return false;
}

std::vector<char> GetOptions::get_optShort(const std::string opt)
{
    size_t i;
    std::vector<char> ret;
    
    if (opt[0] != '-')
        return ret;
    
    if (opt[1] == '-')      /* Test if long option */
        return ret;
    
    for (i = 1; i < opt.length(); i++) {
        ret.push_back(opt[i]);
    }
    
    return ret;
}

std::string GetOptions::get_optLong(const std::string opt)
{
    size_t i;
    std::string ret;
    
    if (opt.length() < 3)
        return ret;
    
    if (opt[0] != '-')
        return ret;
    
    if (opt[1] != '-')
        return ret;
    
    for (i = 2; i < opt.length(); i++) {
        ret.push_back(opt[i]);
    }
    
    return ret;
}

std::string GetOptions::get_optLongWithArg(const std::string opt)
{
    size_t i;
    std::string ret;
    
    if (opt.length() < 3)
        return ret;
    
    if (opt[0] != '-')
        return ret;
    
    if (opt[1] != '-')
        return ret;
    
    for (i = 2; i < opt.length(); i++) {
        if (opt[i] == '=') {
            break;
        } else {
            ret.push_back(opt[i]);
        }
    }
    
    return ret;
}

struct GetOptions::opt_input* GetOptions::find(char opt_short)
{
    std::vector<struct opt_input *>::iterator it;
    
    if (m_optsIn.empty())
        return nullptr;
    
    for (it = m_optsIn.begin(); it < m_optsIn.end(); it++) {
        if ((*it)->optShort == opt_short)
            return (*it);
    }
    
    return nullptr;
}

struct GetOptions::opt_input* GetOptions::find(std::string opt_long)
{
    std::vector<struct opt_input *>::iterator it;
    
    if (m_optsIn.empty())
        return nullptr;
    
    for (it = m_optsIn.begin(); it < m_optsIn.end(); it++) {
        if ((*it)->optLong == opt_long)
            return (*it);
    }
    
    return nullptr;
}

struct GetOptions::opt_input* GetOptions::find(int index)
{
    std::vector<struct opt_input *>::iterator it;
    
    if (m_optsIn.empty())
        return nullptr;
    
    for (it = m_optsIn.begin(); it < m_optsIn.end(); it++) {
        if ((*it)->index == index)
            return (*it);
    }
    
    return nullptr;
}

std::string GetOptions::get_optLongArg(const std::string opt)
{
    size_t i, j;
    std::string ret;
    
    if (opt.length() < 5)
        return ret;
    
    if (opt[0] != '-')
        return ret;
    
    if (opt[1] != '-')
        return ret;
    
    for (i = 3; i < opt.length(); i++) {
        if (opt[i] == '=') {
            for (j = (i + 1); j < opt.length(); j++) {
                ret.push_back(opt[i]);
            }
        }
    }
    
    return ret;
}

GetOptions::GetOptions()
{
    m_rawOpts = nullptr;
    m_error = false;
}

GetOptions::GetOptions(int argc, char *argv[])
{
    std::vector<std::string>::iterator it;
    int i;
    
    m_rawOpts = new std::vector<std::string>;
    
    for (i = 0, it = m_rawOpts->begin(); i < argc; i++, it++) {
        m_rawOpts->push_back(std::string(argv[i]));
    }
    
    m_error = false;
}

GetOptions::GetOptions(const std::vector<std::string> args)
{
    m_rawOpts = new std::vector<std::string>;
    
    for (std::string s : args) {
        m_rawOpts->push_back(s);
    }
    
    m_error = false;
}

GetOptions::~GetOptions()
{
    if (m_rawOpts)
        delete m_rawOpts;
    
    if (!m_optsIn.empty()) {
        for (struct opt_input * s : m_optsIn) {
            delete s;
        }
    }
}

bool GetOptions::setRawOptions(int argc, char *argv[])
{
    std::vector<std::string>::iterator it;
    int i;
    
    if (m_rawOpts)
        return false;
    
    m_rawOpts = new std::vector<std::string>;
    
    for (i = 0, it = m_rawOpts->begin(); i < argc; i++, it++) {
        m_rawOpts->push_back(std::string(argv[i]));
    }
    
    return true;
}

bool GetOptions::setRawOptions(const std::vector<std::string> args)
{
    if (m_rawOpts)
        return false;
    
    m_rawOpts = new std::vector<std::string>;
    
    for (std::string s : args) {
        m_rawOpts->push_back(s);
    }
    
    return true;
}

int GetOptions::addOption(int index, enum GetOptionsArgumentsEnum type,
                          char opt_short)
{
    struct GetOptions::opt_input *o;
    
    if (!m_optsIn.empty()) {
        o = find(index);
        
        if (o)
            return -1;
        
        o = find(opt_short);
        
        if (o)
            return -1;
    }
    
    o = new struct GetOptions::opt_input;
    o->index = index;
    o->found = false;
    o->optShort = opt_short;
    o->type = type;
    m_optsIn.push_back(o);
    
    return index;
}

int GetOptions::addOption(int index, enum GetOptionsArgumentsEnum type,
                          std::string opt_long)
{
    struct GetOptions::opt_input *o;
    
    if (!m_optsIn.empty()) {
        o = find(index);
        
        if (o)
            return -1;
        
        o = find(opt_long);
        
        if (o)
            return -1;
    }
    
    o = new struct GetOptions::opt_input;
    o->index = index;
    o->found = false;
    o->optLong = opt_long;
    o->type = type;
    m_optsIn.push_back(o);
    
    return index;
}

int GetOptions::addOption(int index, enum GetOptionsArgumentsEnum type,
                          char opt_short, std::string opt_long)
{
    struct GetOptions::opt_input *o;
    
    if (!m_optsIn.empty()) {
        o = find(index);
        
        if (o)
            return -1;
        
        o = find(opt_long);
        
        if (o)
            return -1;
        
        o = find(opt_short);
        
        if (o)
            return -1;
    }
    
    o = new struct GetOptions::opt_input;
    o->index = index;
    o->found = false;
    o->optShort = opt_short;
    o->optLong = opt_long;
    o->type = type;
    m_optsIn.push_back(o);
    
    return index;
}

int GetOptions::nextOption()
{
    std::vector<struct opt_input *>::iterator it;
    
    for (it = m_optsIn.begin(); it < m_optsIn.end(); it++) {
        if ((*it)->found) {
            (*it)->found = false;
            return (*it)->index;
        }
    }
    
    return -1;
}

bool GetOptions::error()
{
    return m_error;
}

std::vector<std::string> GetOptions::getArguments(int index)
{
    std::vector<struct opt_input *>::iterator it;
    
    if (m_optsIn.empty())
        return std::vector<std::string>();
    
    for (it = m_optsIn.begin(); it < m_optsIn.end(); it++) {
        if ((*it)->index == index)
            return (*it)->args;
    }
    
    return std::vector<std::string>();
}

std::vector<std::string> GetOptions::getNoneOptions()
{
    return m_noneOpts;
}

std::string GetOptions::what()
{
    return m_what;
}

bool GetOptions::exec()
{
    std::vector<std::string>::iterator it;
    std::vector<char> opt_short;
    std::string opt_long;
    struct opt_input *in;
    bool next_arg;
    bool last_short;
    char last_opt_short = '\0';
    
    if (!m_rawOpts)
        return false;
    
    next_arg = false;
    last_short = false;
    
    for (it = m_rawOpts->begin(); it < m_rawOpts->end(); it++) {
        if (last_short) {
            if (next_arg) {
                if (is_optShort(*it) || is_optLong(*it)) {
                    m_error = true;
                    m_what = std::string("invalid argument '" + *it + "'");
                    return false;
                }
                
                in = find(last_opt_short);
                in->args.push_back(*it);
                next_arg = false;
                last_short = false;
                continue;
            }
        }
        
        if (is_optShort(*it)) {
            if (is_optShortSingle(*it)) {
                opt_short = get_optShort(*it);
            } else {
                opt_short = get_optShort(*it);
            }
            
            for (char c : opt_short) {
                last_opt_short = c;
                in = find(c);
                
                if (!in) {
                    m_error = true;
                    m_what = std::string("invalid argument '" + *it + "'");
                    return false;
                } else {
                    in->found = true;
                    
                    switch (in->type) {
                        case GetOptionsArgumentsEnum::Required:
                            next_arg = true;
                            break;
                        default:
                            next_arg = false;
                    }
                }
            }
            
            last_short = true;
        } else if (is_optLong(*it)) {
            if (has_optLongArg(*it)) {
                opt_long = get_optLongWithArg(*it);
                in = find(opt_long);
                
                if (!in) {
                    m_error = true;
                    m_what = std::string("invalid argument '" + *it + "'");
                    return false;
                }
                
                in->found = true;
                in->args.push_back(get_optLongArg(*it));
            } else {
                
                opt_long = get_optLong(*it);
                in = find(opt_long);
                
                if (!in) {
                    m_error = true;
                    m_what = std::string("invalid argument '" + *it + "'");
                    return false;
                }
                
                in->found = true;
            }
            
            last_short = false;
        } else {
            m_noneOpts.push_back(*it);
            last_short = false;
        }
    }
    
    if (next_arg) {
        m_error = true;
        m_what = std::string("missing argument");
        return false;
    }
    
    return true;
}
