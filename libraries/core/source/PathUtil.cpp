/**
 * Copyright 2015-2024 Daniel Weiner
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#include <Fusion/PathUtil.h>

#include <Fusion/Ascii.h>
#include <Fusion/Assert.h>
#include <Fusion/Path.h>
#include <Fusion/StringUtil.h>
#include <Fusion/Uuid.h>

#include <algorithm>

namespace Fusion
{
static const constexpr std::string_view kPathDot = ".";
static const constexpr std::string_view kPathDoubleDot = "..";
static const constexpr std::string_view kPathTilde = "~";

#if FUSION_PLATFORM_WINDOWS
static bool UseDevicePath(std::string_view path)
{
    if (path.size() < 2 || path[1] != Path::SeparatorDrive)
    {
        // Not a win32 absolute path
        return false;
    }
    if (path.size() >= 248)
    {
        // Long Path
        return true;
    }

    for (;;)
    {
        size_t separator = path.find(Path::SeparatorUnix);
        std::string_view segment = path.substr(0, separator);

        FUSION_ASSERT(!segment.empty());

        if (segment != kPathDoubleDot)
        {
            if (segment.front() == ' ' || segment.back() == ' '
                || segment.back() == '.')
            {
                return true;
            }
        }
        if (separator == std::string_view::npos)
        {
            break;
        }

        path.remove_prefix(separator + 1);
    }

    return false;
}
#endif

std::string PathUtil::Basename(std::string_view path)
{
    std::string normalized = Normalize(path);

    if (IsNormalizedRoot(normalized))
    {
        return {};
    }

    size_t separator = normalized.rfind(Path::SeparatorUnix);
    if (separator == std::string::npos)
    {
        return normalized;
    }

    normalized.erase(0, separator + 1);
    return normalized;
}

std::string_view PathUtil::BasenameView(std::string_view path)
{
    if (path.empty() || IsNormalizedRoot(path))
    {
        return {};
    }

    size_t separator = path.rfind(Path::SeparatorUnix);
    if (separator == std::string_view::npos)
    {
        return path;
    }

    return path.substr(separator + 1);
}

std::string PathUtil::Dirname(std::string_view path)
{
    std::string normalized = Normalize(path);

    if (IsNormalizedRoot(normalized))
    {
        return normalized;
    }

    size_t separator = normalized.rfind(Path::SeparatorUnix);
    if (separator == std::string::npos)
    {
        return {};
    }

    normalized.resize(separator);

    if (normalized.empty() || IsNormalizedRoot(normalized))
    {
        normalized += Path::SeparatorUnix;
    }

    return normalized;
}

std::string_view PathUtil::DirnameView(std::string_view path)
{
    if (path.empty() || IsNormalizedRoot(path))
    {
        return path;
    }

    size_t separator = path.rfind(Path::SeparatorUnix);
    if (separator == std::string_view::npos)
    {
        return {};
    }

    return path.substr(0, separator);
}

std::string PathUtil::Extension(std::string_view file)
{
    std::string basename = Basename(file);

    if (basename.empty())
    {
        return {};
    }

    size_t dot = basename.rfind('.');
    if (dot == std::string::npos)
    {
        return {};
    }

    basename.erase(0, dot + 1);
    return basename;
}

std::string_view PathUtil::ExtensionView(std::string_view file)
{
    std::string_view basename = BasenameView(file);

    if (basename.empty())
    {
        return {};
    }

    size_t dot = basename.rfind('.');
    if (dot == std::string_view::npos)
    {
        return {};
    }

    return basename.substr(dot + 1);
}

bool PathUtil::IsAbsolute(std::string_view path)
{
    if (!path.empty())
    {
        if (path[0] == Path::SeparatorUnix)
        {
            return true;
        }

#if FUSION_PLATFORM_WINDOWS
        if (path[0] == Path::SeparatorWindows)
        {
            return true;
        }
        if (path.size() >= 2 && path[1] == Path::SeparatorDrive)
        {
            return true;
        }
#endif
    }

    return false;
}

bool PathUtil::IsNormalizedRoot(std::string_view path)
{
    if (path.size() == 1 && path[0] == Path::SeparatorUnix)
    {
        return true;
    }

#if FUSION_PLATFORM_WINDOWS
    if (path.size() >= 2 && path[1] == Path::SeparatorDrive)
    {
        if (path.size() == 2)
        {
            // Windows Drive Root
            return true;
        }
        if (path.size() == 3)
        {
            // Windows absolute path from the root
            return IsSeparator(path[2]);
        }
    }

    if (path.size() >= 2
        && path[0] == Path::SeparatorUnix
        && path[1] == Path::SeparatorUnix)
    {
        size_t separator = path.find(Path::SeparatorUnix, 2);
        if (separator == std::string_view::npos)
        {
            return true;
        }

        separator = path.find(Path::Separator, separator + 1);

        return separator == std::string_view::npos
            || separator == path.size() - 1;
    }
#endif

    return false;
}

bool PathUtil::IsRelative(
    std::string_view child,
    std::string_view parent)
{
    std::string p = Normalize(parent);
    std::string c = Normalize(child);

    if (p.size() >= c.size() || !c.starts_with(p))
    {
        return false;
    }

    return c[p.size()] == Path::SeparatorUnix || IsNormalizedRoot(p);
}

bool PathUtil::IsSeparator(char c)
{
    return c == Path::SeparatorUnix
        || c == Path::SeparatorWindows;
}

bool PathUtil::IsSpecialCharacter(char c)
{
    return c == Path::SeparatorUnix
        || c == Path::SeparatorWindows
        || c == Path::SeparatorDrive
        || c == '~';
}

std::string PathUtil::Join(
    std::string_view left,
    std::string_view right)
{
    std::string path(left);
    return Join(path, right);
}

std::string& PathUtil::Join(
    std::string& existing,
    std::string_view segment)
{
    if (IsAbsolute(segment) || existing.empty())
    {
        existing.assign(segment);
    }
    else if (!segment.empty())
    {
        existing.reserve(existing.size() + segment.size() + 1);

        if (existing.back() != Path::SeparatorUnix
            && existing.back() != Path::SeparatorWindows)
        {
            existing += Path::Separator;
        }

        existing.append(segment);
    }

    std::ignore = Normalize(existing);
    return existing;
}

std::string PathUtil::LongPath(std::string_view path)
{
    std::string longPath(path);
    std::ignore = LongPath(longPath);

    return longPath;
}

std::string& PathUtil::LongPath(std::string& path)
{
    std::ignore = Normalize(path);

#if FUSION_PLATFORM_WINDOWS
    // See https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file#namespaces
    // for information about what this is doing.
    constexpr static const std::string_view prefix = "\\\\?\\";

    if (UseDevicePath(path))
    {
        // Windows-only, these paths must be using windows path separators
        // rathed than the normalized '/'.

        std::replace(begin(path), end(path),
            Path::SeparatorUnix,
            Path::SeparatorWindows);

        // The prefix gets inserted at the beginning of the path to notify
        // the OS that it is using object paths.

        path.insert(0, prefix);
    }
#endif

    return path;
}

std::string PathUtil::Normalize(std::string_view path)
{
    if (path.empty())
    {
        return {};
    }

    std::string p(path);
    std::ignore = Normalize(p);

    return p;
}

std::string& PathUtil::Normalize(std::string& path)
{
    using namespace std::string_view_literals;

    if (path.empty())
    {
        return path;
    }

    // Convert any windows style paths to posix styled

    std::ignore = Normcase(path);

    // Purge any trailing slashes from the path.

    while (path.size() > 1 && path.back() == Path::SeparatorUnix)
    {
        path.pop_back();
    }

    // Handle the first special case - Path is '.' or '.. - we return the
    // current working directory joined to the remaining segments.
    if (path[0] == '.')
    {
        if (path.size() == 1)
        {
            path = GetCwdPath();
            return path;
        }
        else if (path.size() >= 2
            && (path[1] == Path::SeparatorUnix || path[1] == '.'))
        {
            // This also handles the '..' case

            std::string cwd(GetCwdPath());
            std::ignore = Join(cwd, path);

            path = std::move(cwd);
            return path;
        }
    }

    // The second special case - Path begins with a '~' indicating the HOME directory.

    if (path[0] == '~')
    {
        if (path.size() == 1)
        {
            path = GetHomePath();
            return path;
        }
        else
        {
            std::string home(GetHomePath());
            std::ignore = Join(home, path);

            path = std::move(home);
            return path;
        }
    }

    // Track how long the path prefix is.
    size_t prefixOffset = 0;

    if (path[0] == Path::SeparatorUnix)
    {
        // The path starts with a separator, so we don't want to purge that
        // out of what gets returned.
        prefixOffset++;

#if FUSION_PLATFORM_WINDOWS
        if (path.size() >= 2 && path[1] == Path::SeparatorUnix)
        {
            // On windows you can specify a path starting with a double-separator '//'
            // which gets treated like a resource locator (URL).
            prefixOffset++;
        }
#endif
    }

    // this is the pointer which points to the current "end" of the processed path string.
    // this value tracks the number of characters which have been normalized within the string.
    size_t currentOffset = prefixOffset;

    // This is absolutely a poor-man's do-while loop.

    for (;;)
    {
        size_t separator = path.find_first_of(Path::SeparatorUnix, currentOffset);
        size_t segmentLength = std::min(separator - currentOffset, path.size() - currentOffset);
        size_t segmentOffset = 0;

        if (path[currentOffset + segmentLength] == Path::SeparatorUnix)
        {
            segmentOffset++;
        }

        // Create a view of the segment we're looking at. If the segment has no
        // separators this could be a single string.

        FUSION_ASSERT(segmentLength <= path.size());
        std::string_view segment{ path.c_str() + currentOffset, segmentLength };

        // ------>

        if (segment.empty() || segment == kPathDot || segment == kPathTilde)
        {
            // We eat the segment here. The only value we need is the 'segmentOffset'
            // which in most cases should only be the single separator.
            path.erase(currentOffset, segment.size() + segmentOffset);
        }
        else if (segment == kPathDoubleDot)
        {
            std::string_view root{ path.c_str(), currentOffset };

            FUSION_ASSERT(root.size() >= prefixOffset);
            FUSION_ASSERT(currentOffset >= prefixOffset);

            if (IsNormalizedRoot(root))
            {
                // We are at a normalized root path now. We can't go back any further
                // so we eat this path segment and remove the separator if one exists.
                path.erase(currentOffset, segment.size() + segmentOffset);
            }
            else if (root.size() == prefixOffset)
            {
                // This essentially means we have a '..' segment but cannot go back
                // any further because we've hit our prefix.
                prefixOffset += segment.size() + segmentOffset;
                currentOffset += segment.size() + segmentOffset;
            }
            else if (size_t prevSeparator = path.rfind(Path::SeparatorUnix, currentOffset - 2);
                prevSeparator != std::string::npos)
            {
                size_t index = prevSeparator + 1;  // skip the separator and leave as the left most
                size_t count = (currentOffset - index) + segment.size() + segmentOffset;

                path.erase(index, count);
                currentOffset = index;
            }
            else
            {
                size_t count = (currentOffset - prefixOffset) + segment.size() + segmentOffset;

                path.erase(prefixOffset, count);
                currentOffset = prefixOffset;
            }
        }
        else
        {
            // The segment is good so we move the currentOffset pointer forward
            // inclusive of a separator if one exists.
            currentOffset += segment.size() + segmentOffset;
        }

        // <------

        FUSION_ASSERT(currentOffset >= prefixOffset);
        FUSION_ASSERT(path.size() >= prefixOffset);

        // Consume any trailing separators after we find one.
        // Note: we cannot use the 'separator' variable because we could have deleted
        //       data from the string above. We have to use our calculated offset instead.

        size_t end = currentOffset;
        while (end < path.size() && path[end] == Path::SeparatorUnix)
        {
            ++end;
        }
        FUSION_ASSERT(end >= currentOffset);
        path.erase(currentOffset, end - currentOffset);

        // This is the primary break condition.
        // If we hit this, it means we've processed all segments in the path and do not
        // have to check anything else.

        if (separator == std::string::npos
            || currentOffset == path.size())
        {
            break;
        }
    }

    // Trim the trailing slash if one exists and this isn't a root path.

    if (!path.empty() && !IsNormalizedRoot(path)
        && path.back() == Path::SeparatorUnix)
    {
        path.pop_back();
    }

    if (IsNormalizedRoot(path))
    {
        if (path.back() != Path::SeparatorUnix)
        {
            path.push_back(Path::SeparatorUnix);
        }
        return path;
    }

    return path;
}

std::string PathUtil::Normcase(std::string_view path)
{
    std::string p(path);
    std::ignore = Normcase(p);

    return p;
}

std::string& PathUtil::Normcase(std::string& path)
{
    for (char& c : path)
    {
        if (c == Path::SeparatorWindows)
        {
            c = Path::SeparatorUnix;
        }
#if FUSION_PLATFORM_WINDOWS
        else
        {
            c = char(Ascii::ToLower(uint8_t(c)));
        }
#endif
    }

    return path;
}

std::string PathUtil::RelPath(
    std::string_view path,
    std::string_view parent)
{
    if (path.empty())
    {
        return "";
    }
    else if (IsAbsolute(path))
    {
        return Normalize(path);
    }
    else if (parent.empty())
    {
        if (std::string cwd = GetCwdPath(); !cwd.empty())
        {
            return Join(cwd, path);
        }
    }

    std::string p(parent);
    std::ignore = Join(p, path);

    return p;
}
}  // namespace Fusion
