// SPDX-LICENSE-IDENTIFIER: Apache-2.0
#include "command.h"

using namespace mmkv::protocol;

std::string mmkv::protocol::detail::command_strings[] = {
    "STRADD",      "STRDEL",
    "STRGET",      "STRSET",
    "STRLEN",      "STRAPPEND",
    "STRPOPBACK",  "LADD",
    "LAPPEND",     "LPREPEND",
    "LGETSIZE",    "LGETALL",
    "LGETRANGE",   "LPOPFRONT",
    "LPOPBACK",    "LDEL",
    "VADD",        "VDELM",
    "VDELMRANGE",  "VDELMRANGEBYWEIGHT",
    "VSIZE",       "VSIZEBYWEIGHT",
    "VWEIGHT",     "VORDER",
    "VRORDER",     "VRANGE",
    "VALL",        "VRANGEBYWEIGHT",
    "VRRANGE",     "VRRANGEBYWEIGHT",
    "MADD",        "MSET",
    "MGET",        "MGETS",
    "MDEL",        "MALL",
    "MFIELDS",     "MVALUES",
    "MSIZE",       "MEXISTS",
    "SADD",        "SDELM",
    "SSIZE",       "SALL",
    "SEXISTS",     "SAND",
    "SANDTO",      "SSUB",
    "SSUBTO",      "SOR",
    "SORTO",       "SANDSIZE",
    "SSUBSIZE",    "SORSIZE",
    "SRANDDELM",   "MEMORYSTAT",
    "EXPIREAT",    "EXPIREAFTER",
    "EXPIREMAT",   "EXPIREMAFTER",
    "PERSIST",     "EXPIRATION",
    "TTL",         "DEL",
    "RENAME",      "TYPE",
    "KEYALL",      "DELS",
    "DELALL",      "SHARD_JOIN",
    "SHARD_LEAVE",
};

static_assert(
    sizeof(mmkv::protocol::detail::command_strings) / sizeof(std::string) == COMMAND_NUM,
    "The size of command_strings must be equal to the number of the commands"
);
