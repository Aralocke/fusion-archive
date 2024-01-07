#!/usr/bin/env python

# Copyright 2015-2024 Daniel Weiner
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from datetime import datetime
import fnmatch
import os
import re
import sys

rootPath = os.path.realpath(os.path.join(__file__, os.pardir, os.pardir))
if not os.path.isfile(os.path.join(rootPath, 'CMakeLists.txt')):
    print('Cannot \'CMakeLists.txt\' in root path: {}'.format(rootPath))
    sys.exit(1)


# Paths which should be ignored by default.
DEFAULT_IGNORE_PATHS = ['*.git', '*external/libraries/*/*']
# Regular expression for C-style comments
REGEX_C_STYLE = r'/\*([\s\S]*?)\*/'
# Regular expression for # based comments used in Python or shell scripting
REGEX_HASH_STYLE = r'(#.*?)$'


def FindFiles(searchPath: str, ignoredPatterns: [list, None] = None) -> [list, list]:
    if ignoredPatterns is None:
        ignoredPatterns = []

    def IsIgnored(p):
        return any(fnmatch.fnmatch(p, pattern)
                   for pattern in ignoredPatterns)

    if IsIgnored(searchPath):
        return [], [searchPath]

    results = list()
    ignored = list()

    [root, dirs, files] = next(os.walk(searchPath))

    for f in files:
        filePath = os.path.join(root, f)
        if IsIgnored(filePath):
            ignored.append(filePath)
            continue
        results.append(filePath)
    for d in dirs:
        dirPath = str(os.path.join(root, d))
        res, ig = FindFiles(dirPath, ignoredPatterns)
        results.extend(res)
        ignored.extend(ig)

    results.sort()
    ignored.sort()

    return results, ignored


def LoadIgnore(ignoreFile: str) -> list:
    if not os.path.isfile(ignoreFile):
        return []

    patterns = list()
    with open(ignoreFile, 'rb') as handle:
        for line in handle:
            line = line.decode('utf-8').strip()
            if len(line) == 0 or line.startswith('#'):
                continue
            if '#' in line:
                line = line[0: line.find('#')].strip()
            if line.endswith('/'):
                line = line[0:-1]
            if not line.startswith('*'):
                line = '*{}'.format(line)
            patterns.append(line)

    return patterns + DEFAULT_IGNORE_PATHS


ignoredPatterns = LoadIgnore(os.path.join(rootPath, '.gitignore'))
sourceFiles, ignoredFiles = FindFiles(rootPath, ignoredPatterns)

print('Found {} source files'.format(len(sourceFiles)))
print('Found {} ignored paths'.format(len(ignoredFiles)))


def UpdateCopyright(sourceFile: str) -> bool:
    currentYear = datetime.now().year
    contentUpdated = False

    with open(sourceFile, 'rb') as handle:
        content = handle.read().decode('utf-8')

        matches = re.finditer(REGEX_C_STYLE, content)
        for match in matches:
            if 'Copyright' in match.group(1):
                updatedComment = re.sub(
                    r'(\b(19|20)\d{2}\b)-(\b(19|20)\d{2}\b)',
                    f"\\1-{currentYear}",
                    match.group(1))
                content = content.replace(match.group(1), updatedComment)
                contentUpdated = True

        matches = re.finditer(REGEX_HASH_STYLE, content, re.MULTILINE)
        for match in matches:
            if 'Copyright' in match.group(1):
                updatedComment = re.sub(
                    r'(\b(19|20)\d{2}\b)-(\b(19|20)\d{2}\b)',
                    f"\\1-{currentYear}",
                    match.group(1))
                content = content.replace(match.group(1), updatedComment)
                contentUpdated = True

    if contentUpdated:
        with open(sourceFile, 'wb') as handle:
            handle.write(content.encode('utf-8'))

    return contentUpdated


updates = 0
for sourceFile in sourceFiles:
    if UpdateCopyright(sourceFile):
        updates += 1

print('Update complete')
print('Updated \'{}\' files'.format(updates))
