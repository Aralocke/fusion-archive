/**
 * Copyright 2015-2022 Daniel Weiner
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

#include <Fusion/Endian.h>

namespace Fusion
{
template<> class LittleEndian<uint16_t>;
template<> class LittleEndian<uint32_t>;
template<> class LittleEndian<uint64_t>;

template<> class BigEndian<uint16_t>;
template<> class BigEndian<uint32_t>;
template<> class BigEndian<uint64_t>;
}  // namespace Fusion
