/*
 * Copyright (c) 2019, Devin Nakamura
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef KLIB_UTIL_HPP
#define KLIB_UTIL_HPP
#include <stdint.h>
#include <type_traits>
namespace klib {

template <typename T, typename U>
constexpr T ptr_cast(U* ptr) {
	static_assert(std::is_pointer<T>::value,
	              "ptr_cast can only cast to pointers");
	return static_cast<T>(static_cast<void*>(ptr));
}

template <typename T>
constexpr const char* toCharPtr(const T* ptr) {
	return static_cast<const char*>(static_cast<const void*>(ptr));
}

template <typename Parent, typename Member>
ptrdiff_t memberOffset(const Member Parent::*ptr) {

	const Parent* const parent = static_cast<const Parent*>(nullptr);
	const char* const member = toCharPtr(&(parent->*ptr));
	return ptrdiff_t(member - toCharPtr(parent));
}
} // namespace klib

#endif
