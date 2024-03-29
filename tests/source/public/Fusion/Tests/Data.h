#pragma once

#include <Fusion/Types.h>

#include <array>
#include <string_view>

using namespace std::string_view_literals;

namespace Fusion
{
#pragma pack(push, 1)
struct PackedHeader
{
    uint32_t length;
    uint64_t fnv;
};
#pragma pack(pop)

constexpr size_t PACKED_HEADER_SIZE = sizeof(PackedHeader);

#pragma pack(push, 1)
struct PackedBlock
{
    PackedHeader header;
    std::string_view data;
};
#pragma pack(pop)

constexpr size_t PACKED_BLOCK_SIZE = sizeof(PackedBlock);

#pragma pack(push, 1)
struct PackedFooter
{
    uint8_t hash[32];
};
#pragma pack(pop)

constexpr size_t PACKED_FOOTER_SIZE = sizeof(PackedFooter);

enum class Endian : uint8_t { BIG, LITTLE };

#pragma pack(push, 1)
template<size_t LENGTH, size_t BLOCKS>
struct Archive
{
    Endian endian;
    PackedHeader header;
    std::array<PackedBlock, BLOCKS> blocks;
    PackedFooter footer;
    std::array<uint8_t, LENGTH> data;
    std::array<uint8_t, 16U> md5;
};
#pragma pack(pop)

constexpr Archive<148U, 2U> PACKED_LE_ARCHIVE_A = {
    Endian::LITTLE,
    { 104U, 13365379659788549137ULL },
    {
        {
            {
                { 16U, 1703058405058535391 },
                ":_bMWVwo3M?M;GQd"sv
            },
            {
                { 64U, 3117432569895302438 },
                "9vn5K0F2NSpPI=lZlinYwmV`tB^`ZKDRxZhzI^is39_uTbrwQl?TBl34Qj3B8`ja"sv
            }
        },

    },
    {
        0xbd, 0x99, 0x4f, 0xb, 0x17, 0x87, 0x51, 0xc9, 0x51, 0xf6,
        0xd5, 0xc0, 0x3, 0xf6, 0xa8, 0x70, 0x68, 0x7c, 0x8e, 0xf1,
        0xef, 0xfd, 0x68, 0x6e, 0xe8, 0x14, 0x43, 0xb, 0x14, 0x77,
        0x88, 0xf8
    },
    {
        0x68, 0x0, 0x0, 0x0, 0x11, 0x7c, 0xed, 0xdd, 0xfc, 0x5d, 0x7b,
        0xb9, 0x10, 0x0, 0x0, 0x0, 0xdf, 0xdf, 0x41, 0xa1, 0x98, 0x7a,
        0xa2, 0x17, 0x3a, 0x5f, 0x62, 0x4d, 0x57, 0x56, 0x77, 0x6f,
        0x33, 0x4d, 0x3f, 0x4d, 0x3b, 0x47, 0x51, 0x64, 0x40, 0x0,
        0x0, 0x0, 0x26, 0xb5, 0xd7, 0xcb, 0x67, 0x58, 0x43, 0x2b,
        0x39, 0x76, 0x6e, 0x35, 0x4b, 0x30, 0x46, 0x32, 0x4e, 0x53,
        0x70, 0x50, 0x49, 0x3d, 0x6c, 0x5a, 0x6c, 0x69, 0x6e, 0x59,
        0x77, 0x6d, 0x56, 0x60, 0x74, 0x42, 0x5e, 0x60, 0x5a, 0x4b,
        0x44, 0x52, 0x78, 0x5a, 0x68, 0x7a, 0x49, 0x5e, 0x69, 0x73,
        0x33, 0x39, 0x5f, 0x75, 0x54, 0x62, 0x72, 0x77, 0x51, 0x6c,
        0x3f, 0x54, 0x42, 0x6c, 0x33, 0x34, 0x51, 0x6a, 0x33, 0x42,
        0x38, 0x60, 0x6a, 0x61, 0xbd, 0x99, 0x4f, 0xb, 0x17, 0x87,
        0x51, 0xc9, 0x51, 0xf6, 0xd5, 0xc0, 0x3, 0xf6, 0xa8, 0x70,
        0x68, 0x7c, 0x8e, 0xf1, 0xef, 0xfd, 0x68, 0x6e, 0xe8, 0x14,
        0x43, 0xb, 0x14, 0x77, 0x88, 0xf8
    },
    {
        0xca, 0x2f, 0x18, 0xd6, 0x3d, 0x03, 0xe3, 0xa5, 0xe1, 0x1c,
        0x0d, 0x7d, 0xd6, 0x05, 0xda, 0xf1
    }
};

constexpr Archive<268U, 4U> PACKED_LE_ARCHIVE_B = {
    Endian::LITTLE,
    { 224U, 12711104704854368431ULL },
    {
        {
            {
                { 16U, 2951903635486610874ULL },
                "1^?`0dJ4MnC>g:7m"sv
            },
            {
                { 16U, 11571691789115503866ULL },
                "cPvcGloPLQU9^T[O"sv
            },
            {
                { 16U, 9758457335488186095ULL },
                "@W@sV`LiXR8q>mun"sv
            },
            {
                { 128U, 10851745974528468074ULL },
                "uvVX55kb3]LXM]VppXF7d>NsDdnBw=SwaFRl=1Mtp5BZJinjRKbgd"
                "?G<BTOJz8si54Zl3dQgHMX9kO2C<C0ib1MJ0tmt85Szn?DQ[kAVt3"
                "zt[T?hc;t6[uRo7[j@K8EK"sv
            }
        },

    },
    {
        0x0, 0xa, 0x64, 0x0, 0xea, 0x66, 0xae, 0x6, 0x41, 0x5f, 0x2c,
        0x11, 0x30, 0x3b, 0x1d, 0xfe, 0xba, 0xc5, 0x93, 0x8d, 0x76,
        0xcd, 0xa8, 0x67, 0x2b, 0xd, 0xf5, 0x34, 0x50, 0x29, 0x23,
        0x8c
    },
    {
        0xe0, 0x0, 0x0, 0x0, 0xaf, 0x20, 0xa4, 0xfb, 0x61, 0xea, 0x66,
        0xb0, 0x10, 0x0, 0x0, 0x0, 0xba, 0x15, 0x5d, 0x74, 0xb7, 0x44,
        0xf7, 0x28, 0x31, 0x5e, 0x3f, 0x60, 0x30, 0x64, 0x4a, 0x34,
        0x4d, 0x6e, 0x43, 0x3e, 0x67, 0x3a, 0x37, 0x6d, 0x10, 0x0,
        0x0, 0x0, 0xfa, 0xb4, 0x88, 0x6c, 0x5f, 0xe8, 0x96, 0xa0,
        0x63, 0x50, 0x76, 0x63, 0x47, 0x6c, 0x6f, 0x50, 0x4c, 0x51,
        0x55, 0x39, 0x5e, 0x54, 0x5b, 0x4f, 0x10, 0x0, 0x0, 0x0, 0xef,
        0x6, 0x94, 0x7d, 0x3e, 0x1, 0x6d, 0x87, 0x40, 0x57, 0x40,
        0x73, 0x56, 0x60, 0x4c, 0x69, 0x58, 0x52, 0x38, 0x71, 0x3e,
        0x6d, 0x75, 0x6e, 0x80, 0x0, 0x0, 0x0, 0x6a, 0x64, 0x8b, 0x4,
        0x78, 0x25, 0x99, 0x96, 0x75, 0x76, 0x56, 0x58, 0x35, 0x35,
        0x6b, 0x62, 0x33, 0x5d, 0x4c, 0x58, 0x4d, 0x5d, 0x56, 0x70,
        0x70, 0x58, 0x46, 0x37, 0x64, 0x3e, 0x4e, 0x73, 0x44, 0x64,
        0x6e, 0x42, 0x77, 0x3d, 0x53, 0x77, 0x61, 0x46, 0x52, 0x6c,
        0x3d, 0x31, 0x4d, 0x74, 0x70, 0x35, 0x42, 0x5a, 0x4a, 0x69,
        0x6e, 0x6a, 0x52, 0x4b, 0x62, 0x67, 0x64, 0x3f, 0x47, 0x3c,
        0x42, 0x54, 0x4f, 0x4a, 0x7a, 0x38, 0x73, 0x69, 0x35, 0x34,
        0x5a, 0x6c, 0x33, 0x64, 0x51, 0x67, 0x48, 0x4d, 0x58, 0x39,
        0x6b, 0x4f, 0x32, 0x43, 0x3c, 0x43, 0x30, 0x69, 0x62, 0x31,
        0x4d, 0x4a, 0x30, 0x74, 0x6d, 0x74, 0x38, 0x35, 0x53, 0x7a,
        0x6e, 0x3f, 0x44, 0x51, 0x5b, 0x6b, 0x41, 0x56, 0x74, 0x33,
        0x7a, 0x74, 0x5b, 0x54, 0x3f, 0x68, 0x63, 0x3b, 0x74, 0x36,
        0x5b, 0x75, 0x52, 0x6f, 0x37, 0x5b, 0x6a, 0x40, 0x4b, 0x38,
        0x45, 0x4b, 0x0, 0xa, 0x64, 0x0, 0xea, 0x66, 0xae, 0x6, 0x41,
        0x5f, 0x2c, 0x11, 0x30, 0x3b, 0x1d, 0xfe, 0xba, 0xc5, 0x93,
        0x8d, 0x76, 0xcd, 0xa8, 0x67, 0x2b, 0xd, 0xf5, 0x34, 0x50,
        0x29, 0x23, 0x8c
    },
    {
        0x28, 0xc4, 0x7c, 0x61, 0x94, 0x93, 0xba, 0xf4, 0xb9, 0xf6,
        0x5d, 0x1b, 0x0f, 0x02, 0x6f, 0xbd
    }
};

constexpr Archive<332U, 4U> PACKED_BE_ARCHIVE_A = {
    Endian::BIG,
    { 288U, 3480413050526611944 },
    {
        {
            {
                { 16U, 17700588918963278457ULL },
                "2ym03Y1JJE<W]Rs5"sv
            },
            {
                { 32U, 10325975735857289239ULL },
                "W^FcbrR85j3V8LYI79Vsr6ExYk1=wfv3"sv
            },
            {
                { 64U, 17775993930945113356ULL },
                "DD<hkA3s0RcWs8=^Pev]SrIyvL8[IvVSp5C7ULf>iJAp54soOz1mf>"
                "xf;WkvatZQ"sv
            },
            {
                { 128U, 16088070824317950816ULL },
                "xyZrbgjpx<A:`v1_B=uUFMZRRF@wfth@6aXXrWANLMk;QLKvmJ4@JQM"
                "N:QQFDAJt7MDzEB_O6lBUS7Ll3Wf?_VzIcXiuXh8upMJ0<9PAP6boJn"
                "GZVX:qv[Z5yX0X>Vnc"sv
            }
        },

    },
    {
        0x3d, 0x3f, 0xb7, 0x32, 0xbb, 0xde, 0x5, 0x96, 0xa1, 0xc4, 0x1f,
        0xf, 0xd4, 0x82, 0x1b, 0x8b, 0x6b, 0x59, 0xa3, 0x70, 0x67, 0xc1,
        0xd6, 0xae, 0xe4, 0x10, 0x4f, 0x7d, 0xfa, 0x8a, 0x7f, 0x58
    },
    {
        0x0, 0x0, 0x1, 0x20, 0x30, 0x4c, 0xe9, 0x3a, 0xbc, 0x45, 0xb9,
        0xe8, 0x0, 0x0, 0x0, 0x10, 0xf5, 0xa5, 0x1f, 0xd7, 0xe, 0x5,
        0x92, 0x79, 0x32, 0x79, 0x6d, 0x30, 0x33, 0x59, 0x31, 0x4a,
        0x4a, 0x45, 0x3c, 0x57, 0x5d, 0x52, 0x73, 0x35, 0x0, 0x0, 0x0,
        0x20, 0x8f, 0x4d, 0x3c, 0x38, 0xef, 0x13, 0x98, 0x17, 0x57,
        0x5e, 0x46, 0x63, 0x62, 0x72, 0x52, 0x38, 0x35, 0x6a, 0x33,
        0x56, 0x38, 0x4c, 0x59, 0x49, 0x37, 0x39, 0x56, 0x73, 0x72,
        0x36, 0x45, 0x78, 0x59, 0x6b, 0x31, 0x3d, 0x77, 0x66, 0x76,
        0x33, 0x0, 0x0, 0x0, 0x40, 0xf6, 0xb1, 0x4, 0x4c, 0x87, 0x7b,
        0xad, 0xc, 0x44, 0x44, 0x3c, 0x68, 0x6b, 0x41, 0x33, 0x73, 0x30,
        0x52, 0x63, 0x57, 0x73, 0x38, 0x3d, 0x5e, 0x50, 0x65, 0x76,
        0x5d, 0x53, 0x72, 0x49, 0x79, 0x76, 0x4c, 0x38, 0x5b, 0x49,
        0x76, 0x56, 0x53, 0x70, 0x35, 0x43, 0x37, 0x55, 0x4c, 0x66,
        0x3e, 0x69, 0x4a, 0x41, 0x70, 0x35, 0x34, 0x73, 0x6f, 0x4f,
        0x7a, 0x31, 0x6d, 0x66, 0x3e, 0x78, 0x66, 0x3b, 0x57, 0x6b,
        0x76, 0x61, 0x74, 0x5a, 0x51, 0x0, 0x0, 0x0, 0x80, 0xdf, 0x44,
        0x4f, 0x2d, 0x2c, 0xcc, 0x1b, 0x60, 0x78, 0x79, 0x5a, 0x72,
        0x62, 0x67, 0x6a, 0x70, 0x78, 0x3c, 0x41, 0x3a, 0x60, 0x76,
        0x31, 0x5f, 0x42, 0x3d, 0x75, 0x55, 0x46, 0x4d, 0x5a, 0x52,
        0x52, 0x46, 0x40, 0x77, 0x66, 0x74, 0x68, 0x40, 0x36, 0x61,
        0x58, 0x58, 0x72, 0x57, 0x41, 0x4e, 0x4c, 0x4d, 0x6b, 0x3b,
        0x51, 0x4c, 0x4b, 0x76, 0x6d, 0x4a, 0x34, 0x40, 0x4a, 0x51,
        0x4d, 0x4e, 0x3a, 0x51, 0x51, 0x46, 0x44, 0x41, 0x4a, 0x74,
        0x37, 0x4d, 0x44, 0x7a, 0x45, 0x42, 0x5f, 0x4f, 0x36, 0x6c,
        0x42, 0x55, 0x53, 0x37, 0x4c, 0x6c, 0x33, 0x57, 0x66, 0x3f,
        0x5f, 0x56, 0x7a, 0x49, 0x63, 0x58, 0x69, 0x75, 0x58, 0x68,
        0x38, 0x75, 0x70, 0x4d, 0x4a, 0x30, 0x3c, 0x39, 0x50, 0x41,
        0x50, 0x36, 0x62, 0x6f, 0x4a, 0x6e, 0x47, 0x5a, 0x56, 0x58,
        0x3a, 0x71, 0x76, 0x5b, 0x5a, 0x35, 0x79, 0x58, 0x30, 0x58,
        0x3e, 0x56, 0x6e, 0x63, 0x3d, 0x3f, 0xb7, 0x32, 0xbb, 0xde,
        0x5, 0x96, 0xa1, 0xc4, 0x1f, 0xf, 0xd4, 0x82, 0x1b, 0x8b,
        0x6b, 0x59, 0xa3, 0x70, 0x67, 0xc1, 0xd6, 0xae, 0xe4, 0x10,
        0x4f, 0x7d, 0xfa, 0x8a, 0x7f, 0x58
    },
    {
        0xe3, 0x50, 0xaa, 0x0b, 0x70, 0x6e, 0x54, 0xad, 0xb4, 0x65,
        0x13, 0x1f, 0x07, 0x5e, 0x8f, 0x7c
    }
};

constexpr Archive<185U, 4U> PACKED_BE_ARCHIVE_B = {
    Endian::BIG,
    { 141U, 15995481264334489815ULL },
    {
        {
            {
                { 28U, 7554681275412056585ULL },
                "hvNjF=52dMndGIOTq@r26KaW_?[k"sv
            },
            {
                { 29U, 17741205489215442276ULL },
                "pg7w=;1lPvYoi2][y<NQPe0KWjywK"sv
            },
            {
                { 11U, 3839549448827234375ULL },
                "rRXQSCD@ri@"sv
            },
            {
                { 25U, 4103546144003522086ULL },
                "krykLQj2qDY4^?@oy>[GW^IV^"sv
            }
        },

    },
    {
        0xc5, 0xf4, 0xf, 0xb4, 0x7b, 0x6d, 0x65, 0x68, 0xc3, 0xe, 0x56,
        0xe8, 0xfe, 0xbc, 0x61, 0xea, 0xc3, 0x50, 0xec, 0x8e, 0x46,
        0xe8, 0x40, 0x37, 0xdc, 0xb5, 0x51, 0xa1, 0xc5, 0x67, 0xdb,
        0x5b
    },
    {
        0x0, 0x0, 0x0, 0x8d, 0xdd, 0xfb, 0x5d, 0x76, 0x54, 0x12, 0xc8,
        0xd7, 0x0, 0x0, 0x0, 0x1c, 0x68, 0xd7, 0x9e, 0x97, 0xf4, 0x3b,
        0xa, 0x9, 0x68, 0x76, 0x4e, 0x6a, 0x46, 0x3d, 0x35, 0x32, 0x64,
        0x4d, 0x6e, 0x64, 0x47, 0x49, 0x4f, 0x54, 0x71, 0x40, 0x72,
        0x32, 0x36, 0x4b, 0x61, 0x57, 0x5f, 0x3f, 0x5b, 0x6b, 0x0, 0x0,
        0x0, 0x1d, 0xf6, 0x35, 0x6c, 0x65, 0x3f, 0xe5, 0x8d, 0x64, 0x70,
        0x67, 0x37, 0x77, 0x3d, 0x3b, 0x31, 0x6c, 0x50, 0x76, 0x59,
        0x6f, 0x69, 0x32, 0x5d, 0x5b, 0x79, 0x3c, 0x4e, 0x51, 0x50,
        0x65, 0x30, 0x4b, 0x57, 0x6a, 0x79, 0x77, 0x4b, 0x0, 0x0, 0x0,
        0xb, 0x35, 0x48, 0xd1, 0xe1, 0x82, 0xe4, 0xe8, 0x47, 0x72,
        0x52, 0x58, 0x51, 0x53, 0x43, 0x44, 0x40, 0x72, 0x69, 0x40,
        0x0, 0x0, 0x0, 0x19, 0x38, 0xf2, 0xb9, 0x79, 0xf8, 0xc0, 0xfe,
        0x26, 0x6b, 0x72, 0x79, 0x6b, 0x4c, 0x51, 0x6a, 0x32, 0x71,
        0x44, 0x59, 0x34, 0x5e, 0x3f, 0x40, 0x6f, 0x79, 0x3e, 0x5b,
        0x47, 0x57, 0x5e, 0x49, 0x56, 0x5e, 0xc5, 0xf4, 0xf, 0xb4, 0x7b,
        0x6d, 0x65, 0x68, 0xc3, 0xe, 0x56, 0xe8, 0xfe, 0xbc, 0x61, 0xea,
        0xc3, 0x50, 0xec, 0x8e, 0x46, 0xe8, 0x40, 0x37, 0xdc, 0xb5,
        0x51, 0xa1, 0xc5, 0x67, 0xdb, 0x5b
    },
    {
        0x83, 0xad, 0x7d, 0x23, 0x5d, 0xd2, 0x22, 0xe0, 0x2c, 0xb4,
        0x49, 0x09, 0x26, 0xb3, 0xc2, 0x19
    }
};

}  // namespace Fusion
