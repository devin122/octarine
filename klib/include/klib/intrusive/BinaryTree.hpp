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

#ifndef KLIB_INTRUSIVE_BINARY_TREE_HPP
#define KLIB_INTRUSIVE_BINARY_TREE_HPP

#include <klib.h>
#include <type_traits>
namespace intrusive {

#if 0

template <typename T, typename K>
struct DummyTraits {
	KLIB_FORCEINLINE static T* getLeft(const T*);
	KLIB_FORCEINLINE static void setLeft(T*, T*);
	KLIB_FORCEINLINE static T* getRight(const T*);
	KLIB_FORCEINLINE static void setRight(T*, T*);
	KLIB_FORCEINLINE static T* getParent(const T*);
	KLIB_FORCEINLINE static void setParent(T*, T*);
	KLIB_FORCEINLINE static T* getNext(const T*);

	using Key = K;
	using NodeInfoType = BinaryTreeNode;
};
#endif

template <typename T>
struct TreeBaseHook {
	T* parent = nullptr;
	T* left = nullptr;
	T* right = nullptr;
};

struct EmptyTraits {};
namespace impl {
template <typename T, typename Base>
struct MakeBinaryTreeTraits : public Base {
	// dummy base case, dont do anything
	// static_assert(false);
};

template <typename T, typename Base>
struct MakeBinaryTreeTraits<TreeBaseHook<T>, Base> : public Base {
	KLIB_FORCEINLINE static T* getLeft(const T* node) { return node->left; }
	KLIB_FORCEINLINE static void setLeft(T* node, T* val) { node->left = val; }
	KLIB_FORCEINLINE static T* getRight(const T* node) { return node->right; }
	KLIB_FORCEINLINE static void setRight(T* node, T* val) {
		node->right = val;
	}
	KLIB_FORCEINLINE static T* getParent(const T* node) { return node->parent; }
	KLIB_FORCEINLINE static void setParent(T* node, T* val) {
		node->parent = val;
	}
};

template <typename T>
struct DefaultComparator {
	KLIB_FORCEINLINE constexpr bool operator()(const T& a, const T& b) const {
		return a < b;
	}
};
} // namespace impl

template <typename T>
struct TreeMemberHook {};

template <typename T, typename _Key, typename _Traits,
          typename Compare = impl::DefaultComparator<_Key>>
class BinaryTree {
	// TODO: currently only support base hook
	static_assert(std::is_base_of<TreeBaseHook<T>, T>::value,
	              "Currently only TreeBaseHook<T> us supported");

  public:
	using Key = typename _Traits::Key;

  protected:
	T* root_ = nullptr;
	using Traits = _Traits;

	KLIB_FORCEINLINE void replaceInParent(T* node, T* replacement) {
		T* parent = Traits::getParent(node);
		if (parent) {
			bool is_lchild = Traits::isLeftChild(node);
			KLIB_ASSERT(is_lchild != Traits::isRightChild(node));
			if (is_lchild) {
				Traits::setLeft(parent, replacement);
			} else {
				Traits::setRight(parent, replacement);
			}
		} else {
			KLIB_ASSERT(root_ == node);
			root_ = replacement;
		}

		if (replacement) {
			Traits::setParent(replacement, parent);
		}
	}

	KLIB_FORCEINLINE void setLeftChild(T* node, T* leftChild) {
		Traits::setLeft(node, leftChild);
		if (leftChild) {
			Traits::setParent(leftChild, node);
		}
	}

	KLIB_FORCEINLINE void setRightChild(T* node, T* rightChild) {
		Traits::setRight(node, rightChild);
		if (rightChild) {
			Traits::setParent(rightChild, node);
		}
	}

	KLIB_FORCEINLINE static bool isLess(Key key, T* node) {
		return Traits::isLess(key, Traits::getKey(node));
	}

	KLIB_FORCEINLINE static bool isLess(T* node, Key key) {
		return Traits::isLess(Traits::getKey(node), key);
	}
	KLIB_FORCEINLINE static bool isLess(T* n1, T* n2) {
		return Traits::isLess(Traits::getKey(n1), Traits::getKey(n2));
	}

  public:
	T* getRoot() { return root_; }
	void insert(T* val) {
		Traits::setLeft(val, nullptr);
		Traits::setRight(val, nullptr);
		Traits::setParent(val, nullptr);
		if (root_ == nullptr) {
			root_ = val;
			return;
		}
		T* node = root_;
		Key valKey = Traits::getKey(val);

		while (true) {
			KLIB_ASSERT(node != nullptr);
			Key nodeKey = Traits::getKey(node);
			if (Traits::isLess(valKey, nodeKey)) {
				T* next = Traits::getLeft(node);
				if (next == nullptr) {
					setLeftChild(node, val);
					return;
				}
				node = next;
			} else if (Traits::isLess(nodeKey, valKey)) {
				T* next = Traits::getRight(node);
				if (next == nullptr) {
					setRightChild(node, val);
					return;
				}
				node = next;
			} else {
				// TODO maybe fail more gracefully
				KLIB_PANIC("Duplicate insertion");
			}
		}
	}

	T* find(Key key) const {
		T* node = root_;
		while (true) {
			if (node == nullptr) {
				return nullptr;
			}
			Key nodeKey = Traits::getKey(node);
			if (Traits::isLess(key, nodeKey)) {
				node = Traits::getLeft(node);
			} else if (Traits::isLess(nodeKey, key)) {
				node = Traits::getRight(node);
			} else {
				return node;
			}
		}
	}

	void remove(T* node) {
		T* parent = Traits::getParent(node);
		T* l_child = Traits::getLeft(node);
		T* r_child = Traits::getRight(node);
		if (l_child == nullptr) {
			replaceInParent(node, r_child);
		} else if (r_child == nullptr) {
			replaceInParent(node, l_child);
		} else {
			T* replacement = Traits::getNext(node);
			KLIB_ASSERT(!Traits::getLeft(node));
			KLIB_ASSERT(!Traits::getRight(node));
			replaceInParent(replacement, nullptr);
			// setLeftChild()
			Traits::setParent(replacement, parent);
			Traits::setLeft(replacement, l_child);
			if (r_child != replacement) {
				Traits::setRight(replacement, r_child);
			}
		}
	}
};

} // namespace intrusive
#endif
