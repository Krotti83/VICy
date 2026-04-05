// SPDX-License-Identifier: GPL-3.0-or-later
/**
 * VICy - Open Cognitive Assist System
 *
 * Copyright (C) 2026 Johannes Krottmayer <krotti83@proton.me>
 *
 * This file is part of VICy's library modules.
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

#include <cstring>
#include <sstream>
#include <ios>

#include <VICyLib/SHA256.hpp>

using namespace VICy::VICyLib;

#define ROR32(val, num)     (((val) >> (num)) | ((val) << (32 - (num))))
#define SHR32(val, num)     ((val) >> (num))

#define CH(x, y, z)         (((x) & (y)) ^ ((~(x)) & (z)))
#define MAJ(x, y, z)        (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SUM0(x)             (ROR32((x), 2) ^ ROR32((x), 13) ^ ROR32((x), 22))
#define SUM1(x)             (ROR32((x), 6) ^ ROR32((x), 11) ^ ROR32((x), 25))
#define SIG0(x)             (ROR32((x), 7) ^ ROR32((x), 18) ^ SHR32((x), 3))
#define SIG1(x)             (ROR32((x), 17) ^ ROR32((x), 19) ^ SHR32((x), 10))

#define SHA256_INIT_H1      0x6A09E667
#define SHA256_INIT_H2      0xBB67AE85
#define SHA256_INIT_H3      0x3C6EF372
#define SHA256_INIT_H4      0xA54FF53A
#define SHA256_INIT_H5      0x510E527F
#define SHA256_INIT_H6      0x9B05688C
#define SHA256_INIT_H7      0x1F83D9AB
#define SHA256_INIT_H8      0x5BE0CD19

static const unsigned long k[64] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
    0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
    0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
    0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
    0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
    0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
    0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
    0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
    0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};

SHA256::SHA256()
{
    m_done = false;
    init();
}

SHA256::~SHA256()
{
    // This is currently a stub.
}

SHA256::SHA256(void *buf, size_t len)
{
    m_done = false;
    init();
    m_done = true;
    sum(buf, len);
}

SHA256::SHA256(std::queue<unsigned char> buf)
{
    m_done = false;

    if (!m_done) {
        init();
    }

    m_done = true;
    sum(buf);
}

void SHA256::Calc(void *buf, size_t len)
{
    if (!m_done) {
        init();
    }

    m_done = true;
    sum(buf, len);
}

void SHA256::Calc(std::queue<unsigned char> buf)
{
    if (!m_done) {
        init();
    }

    m_done = true;
    sum(buf);
}

void SHA256::init()
{
    m_hashCurrent[0] = SHA256_INIT_H1;
    m_hashCurrent[1] = SHA256_INIT_H2;
    m_hashCurrent[2] = SHA256_INIT_H3;
    m_hashCurrent[3] = SHA256_INIT_H4;
    m_hashCurrent[4] = SHA256_INIT_H5;
    m_hashCurrent[5] = SHA256_INIT_H6;
    m_hashCurrent[6] = SHA256_INIT_H7;
    m_hashCurrent[7] = SHA256_INIT_H8;

    m_hashLast[0] = SHA256_INIT_H1;
    m_hashLast[1] = SHA256_INIT_H2;
    m_hashLast[2] = SHA256_INIT_H3;
    m_hashLast[3] = SHA256_INIT_H4;
    m_hashLast[4] = SHA256_INIT_H5;
    m_hashLast[5] = SHA256_INIT_H6;
    m_hashLast[6] = SHA256_INIT_H7;
    m_hashLast[7] = SHA256_INIT_H8;
}

void SHA256::transform(unsigned char msg[64])
{
    int i;
    int j = 0;
    unsigned long t1;
    unsigned long t2;
    unsigned long w[64];

    for (i = 0; i < 16; i++) {
        w[i] = static_cast<unsigned long>(msg[j++] << 24);
        w[i] |= static_cast<unsigned long>(msg[j++] << 16);
        w[i] |= static_cast<unsigned long>(msg[j++] << 8);
        w[i] |= static_cast<unsigned long>(msg[j++]);
    }

    for (i = 16; i < 64; i++) {
        w[i] = SIG1(w[i - 2]) + w[i - 7] + SIG0(w[i - 15]) + w[i - 16];
    }

    for (i = 0; i < 64; i++) {
        t1 = m_hashCurrent[7] +
        SUM1(m_hashCurrent[4]) +
        CH(m_hashCurrent[4], m_hashCurrent[5], m_hashCurrent[6]) + k[i] + w[i];
        t2 = SUM0(m_hashCurrent[0]) +
             MAJ(m_hashCurrent[0], m_hashCurrent[1], m_hashCurrent[2]);
        m_hashCurrent[7] = m_hashCurrent[6];
        m_hashCurrent[6] = m_hashCurrent[5];
        m_hashCurrent[5] = m_hashCurrent[4];
        m_hashCurrent[4] = m_hashCurrent[3] + t1;
        m_hashCurrent[3] = m_hashCurrent[2];
        m_hashCurrent[2] = m_hashCurrent[1];
        m_hashCurrent[1] = m_hashCurrent[0];
        m_hashCurrent[0] = t1 + t2;
    }

    m_hashCurrent[0] += m_hashLast[0];
    m_hashCurrent[1] += m_hashLast[1];
    m_hashCurrent[2] += m_hashLast[2];
    m_hashCurrent[3] += m_hashLast[3];
    m_hashCurrent[4] += m_hashLast[4];
    m_hashCurrent[5] += m_hashLast[5];
    m_hashCurrent[6] += m_hashLast[6];
    m_hashCurrent[7] += m_hashLast[7];

    m_hashLast[0] = m_hashCurrent[0];
    m_hashLast[1] = m_hashCurrent[1];
    m_hashLast[2] = m_hashCurrent[2];
    m_hashLast[3] = m_hashCurrent[3];
    m_hashLast[4] = m_hashCurrent[4];
    m_hashLast[5] = m_hashCurrent[5];
    m_hashLast[6] = m_hashCurrent[6];
    m_hashLast[7] = m_hashCurrent[7];
}

void SHA256::sum(void *buf, size_t len)
{
    int i;
    int p = 0;
    unsigned char msg[64];
    unsigned long int len_bit;
    long int num_blocks;
    int mod;
    unsigned char *ptr;

    ptr = static_cast<unsigned char *>(buf);

    num_blocks = len / 64;
    mod = len % 64;

    if (num_blocks) {
        for (i = 0; i < num_blocks; i++) {
            memcpy(msg, &ptr[p], 64);
            p += 64;
            transform(msg);
        }
    }

    if (mod < 56) {
        memset(msg, 0, 64);
        memcpy(msg, &ptr[p], mod);
        msg[mod] = 0x80;
    } else if (mod == 0){
        memset(msg, 0, 64);
        msg[0] = 0x80;
    } else {
        memset(msg, 0, 64);
        memcpy(msg, &ptr[p], mod);
        msg[mod] = 0x80;
        transform(msg);
        memset(msg, 0, 64);
    }

    len_bit = len * 8;
    msg[56] = static_cast<unsigned char>((len_bit >> 56) & 0xFF);
    msg[57] = static_cast<unsigned char>((len_bit >> 48) & 0xFF);
    msg[58] = static_cast<unsigned char>((len_bit >> 40) & 0xFF);
    msg[59] = static_cast<unsigned char>((len_bit >> 32) & 0xFF);
    msg[60] = static_cast<unsigned char>((len_bit >> 24) & 0xFF);
    msg[61] = static_cast<unsigned char>((len_bit >> 16) & 0xFF);
    msg[62] = static_cast<unsigned char>((len_bit >> 8) & 0xFF);
    msg[63] = static_cast<unsigned char>(len_bit & 0xFF);
    transform(msg);
}

void SHA256::sum(std::queue<unsigned char> buf)
{
    int i;
    int j;
    int p = 0;
    unsigned char msg[64];
    unsigned long int len_bit;
    long int num_blocks;
    int mod;

    num_blocks = buf.size() / 64;
    mod = buf.size() % 64;

    if (num_blocks) {
        for (i = 0; i < num_blocks; i++) {
            for (j = 0; j < 64; j++) {
                msg[j] = buf.front();
                buf.pop();
            }

            p += 64;
            transform(msg);
        }
    }

    if (mod < 56) {
        memset(msg, 0, 64);

        for (j = 0; j < mod; j++) {
            msg[j] = buf.front();
            buf.pop();
        }

        msg[mod] = 0x80;
    } else if (mod == 0){
        memset(msg, 0, 64);
        msg[0] = 0x80;
    } else {
        memset(msg, 0, 64);

        for (j = 0; j < mod; j++) {
            msg[j] = buf.front();
            buf.pop();
        }

        msg[mod] = 0x80;
        transform(msg);
        memset(msg, 0, 64);
    }

    len_bit = buf.size() * 8;
    msg[56] = static_cast<unsigned char>((len_bit >> 56) & 0xFF);
    msg[57] = static_cast<unsigned char>((len_bit >> 48) & 0xFF);
    msg[58] = static_cast<unsigned char>((len_bit >> 40) & 0xFF);
    msg[59] = static_cast<unsigned char>((len_bit >> 32) & 0xFF);
    msg[60] = static_cast<unsigned char>((len_bit >> 24) & 0xFF);
    msg[61] = static_cast<unsigned char>((len_bit >> 16) & 0xFF);
    msg[62] = static_cast<unsigned char>((len_bit >> 8) & 0xFF);
    msg[63] = static_cast<unsigned char>(len_bit & 0xFF);
    transform(msg);
}

void SHA256::hash()
{
    m_hash[0] = static_cast<unsigned char>((m_hashCurrent[0] >> 24) & 0xFF);
    m_hash[1] = static_cast<unsigned char>((m_hashCurrent[0] >> 16) & 0xFF);
    m_hash[2] = static_cast<unsigned char>((m_hashCurrent[0] >> 8) & 0xFF);
    m_hash[3] = static_cast<unsigned char>(m_hashCurrent[0] & 0xFF);

    m_hash[4] = static_cast<unsigned char>((m_hashCurrent[1] >> 24) & 0xFF);
    m_hash[5] = static_cast<unsigned char>((m_hashCurrent[1] >> 16) & 0xFF);
    m_hash[6] = static_cast<unsigned char>((m_hashCurrent[1] >> 8) & 0xFF);
    m_hash[7] = static_cast<unsigned char>(m_hashCurrent[1] & 0xFF);

    m_hash[8] = static_cast<unsigned char>((m_hashCurrent[2] >> 24) & 0xFF);
    m_hash[9] = static_cast<unsigned char>((m_hashCurrent[2] >> 16) & 0xFF);
    m_hash[10] = static_cast<unsigned char>((m_hashCurrent[2] >> 8) & 0xFF);
    m_hash[11] = static_cast<unsigned char>(m_hashCurrent[2] & 0xFF);

    m_hash[12] = static_cast<unsigned char>((m_hashCurrent[3] >> 24) & 0xFF);
    m_hash[13] = static_cast<unsigned char>((m_hashCurrent[3] >> 16) & 0xFF);
    m_hash[14] = static_cast<unsigned char>((m_hashCurrent[3] >> 8) & 0xFF);
    m_hash[15] = static_cast<unsigned char>(m_hashCurrent[3] & 0xFF);

    m_hash[16] = static_cast<unsigned char>((m_hashCurrent[4] >> 24) & 0xFF);
    m_hash[17] = static_cast<unsigned char>((m_hashCurrent[4] >> 16) & 0xFF);
    m_hash[18] = static_cast<unsigned char>((m_hashCurrent[4] >> 8) & 0xFF);
    m_hash[19] = static_cast<unsigned char>(m_hashCurrent[4] & 0xFF);

    m_hash[20] = static_cast<unsigned char>((m_hashCurrent[5] >> 24) & 0xFF);
    m_hash[21] = static_cast<unsigned char>((m_hashCurrent[5] >> 16) & 0xFF);
    m_hash[22] = static_cast<unsigned char>((m_hashCurrent[5] >> 8) & 0xFF);
    m_hash[23] = static_cast<unsigned char>(m_hashCurrent[5] & 0xFF);

    m_hash[24] = static_cast<unsigned char>((m_hashCurrent[6] >> 24) & 0xFF);
    m_hash[25] = static_cast<unsigned char>((m_hashCurrent[6] >> 16) & 0xFF);
    m_hash[26] = static_cast<unsigned char>((m_hashCurrent[6] >> 8) & 0xFF);
    m_hash[27] = static_cast<unsigned char>(m_hashCurrent[6] & 0xFF);

    m_hash[28] = static_cast<unsigned char>((m_hashCurrent[7] >> 24) & 0xFF);
    m_hash[29] = static_cast<unsigned char>((m_hashCurrent[7] >> 16) & 0xFF);
    m_hash[30] = static_cast<unsigned char>((m_hashCurrent[7] >> 8) & 0xFF);
    m_hash[31] = static_cast<unsigned char>(m_hashCurrent[7] & 0xFF);
}

std::array<unsigned char, 32> SHA256::GetHashBuffer()
{
    return m_hash;
}

std::string SHA256::GetHashSring()
{
    std::string ret;
    std::string tmp;
    std::stringstream ss;

    ret.clear();

    for (unsigned char &i : m_hash) {
        ss << std::hex << i;
        ss >> tmp;
        ret.append(tmp);
    }

    return ret;
}
