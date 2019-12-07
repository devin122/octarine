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

#ifndef KLIB_INTRUSIVE_LIST_HPP
#define KLIB_INTRUSIVE_LIST_HPP

//#include <klib.h>
#include <klib/util.hpp>

namespace klib {
namespace intrusive {

struct ListNode {
	ListNode* prev = nullptr;
	ListNode* next = nullptr;
	// constexpr ListNode()
};

template <typename T, ListNode T::*list_data>
class ListIterator {
	ListNode* node_;

	// TODO this shouldnt be duplicated. Need a smarter way of factoring this
	T* itemFromNode(ListNode* node) const {
		char* temp = ptr_cast<char*>(node) - memberOffset(list_data);
		return ptr_cast<T*>(temp);
	}

  public:
	constexpr ListIterator() : node_(nullptr) {}

	constexpr ListIterator(ListNode* node) : node_(node) {}

	ListIterator& operator++() {
		node_ = node_->next;
		return *this;
	}

	bool operator==(ListIterator<T, list_data>& other) const {
		return node_ == other.node_;
	}
	bool operator!=(ListIterator<T, list_data>& other) const {
		return node_ != other.node_;
	}

	ListIterator& operator=(ListIterator<T, list_data> other) {
		node_ = other.node_;
		return *this;
	}

	T* operator*() const { return itemFromNode(node_); }

	T* operator->() const { return itemFromNode(node_); }
};

template <typename T, ListNode T::*list_data>
class List {
	friend class ListIterator<T, list_data>;
	ListNode header_{.prev = &header_, .next = &header_};

	T* itemFromNode(ListNode* node) const {
		char* temp = ptr_cast<char*>(node) - memberOffset(list_data);
		return ptr_cast<T*>(temp);
	}

	void insertBetween(ListNode* before, ListNode* after, ListNode* newValue) {
		// KLIB_ASSERT(before != nullptr);
		// KLIB_ASSERT(before->next == after);
		// KLIB_ASSERT(after->prev == before);

		before->next = newValue;
		newValue->prev = before;

		newValue->next = after;
		after->prev = newValue;
	}

	void insertAfter(ListNode* existing, ListNode* newData) {
		ListNode* after = existing->next;

		existing->next = newData;
		newData->prev = existing;

		newData->next = after;
		after->prev = newData;
	}

	void remove(ListNode* node) {
		// KLIB_ASSERT(node != &header_);
		ListNode* before = node->prev;
		ListNode* after = node->next;
		before->next = after;
		after->prev = before;
	}

  public:
	typedef ListIterator<T, list_data> Iterator;
	// constexpr List(){}

	// void insertAfter(T* existing, T* newElement) {}

	void insert_head(T* elem) {
		insertBetween(&header_, header_.next, &(elem->*list_data));
	}

	void insert_tail(T* elem) {
		insertBetween(header_.prev, &header_, &(elem->*list_data));
	}

	T* remove_head() {
		ListNode* node = header_.next;
		if (node != &header_) {
			remove(node);
			return itemFromNode(node);
		} else {
			return nullptr;
		}
	}

	void remove(T* elem) { remove(&(elem->*list_data)); }

	T* head() {
		if (header_.next == &header_) {
			return nullptr;
		}
		return itemFromNode(header_.next);
	}

	T* tail() {
		if (header_.next == &header_) {
			return nullptr;
		}
		return itemFromNode(header_.prev);
	}

	Iterator begin() { return Iterator(header_.next); }

	Iterator end() { return Iterator(header_); }

	void insertAfter(T* existing, T* value) {
		insertAfter(&(existing->*list_data), &(value->*list_data));
	}

	void insertAfter(const Iterator it, T* value) { insertAfter(*it, value); }
};

} // namespace intrusive
} // namespace klib

#endif