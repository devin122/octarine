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

#include "list_test.hpp"

#include <gtest/gtest.h>
#include <klib/intrusive/List.hpp>

using namespace klib::intrusive;

struct TestStruct {
	ListNode list_info;
	ListNode other_list;
	int a;
};

typedef List<TestStruct, &TestStruct::list_info> MyList;
typedef List<TestStruct, &TestStruct::other_list> OtherList;

#define VALIDATE_LIST(LIST, TYPE, ELEMENTS...)                                 \
	{                                                                          \
		TYPE _expected[] = {ELEMENTS};                                         \
		test_collection(                                                       \
		    &_expected[0],                                                     \
		    &_expected[(sizeof(_expected) / sizeof(_expected[0])) - 1],        \
		    sizeof(_expected) / sizeof(_expected[0]), LIST.begin());           \
	}

#define CHECK_LIST(LIST, ELEMENTS...) VALIDATE_LIST(LIST, TestStruct*, ELEMENTS)

TEST(ListTest, NullInitialization) {
	// CHeck that list is initialized with null values
	MyList list;
	MyList::Iterator begin, end;
	TestStruct *head, *tail;

	ASSERT_EQ(list.head(), nullptr);
	ASSERT_EQ(list.tail(), nullptr);

	// ASSERT_EQ(*list.begin(), nullptr);
	// ASSERT_EQ(*list.end(), nullptr);
}

TEST(ListTest, InsertHead) {
	// Exercises the Xyz feature of Foo.
	MyList list;
	MyList::Iterator begin, end;

	TestStruct a, b, c, d;

	list.insert_head(&a);
	ASSERT_EQ(list.head(), &a);
	ASSERT_EQ(list.tail(), &a);
	CHECK_LIST(list, &a);

	list.insert_head(&b);
	ASSERT_EQ(list.head(), &b);
	ASSERT_EQ(list.tail(), &a);
	CHECK_LIST(list, &b, &a);

	list.insert_head(&c);
	ASSERT_EQ(list.head(), &c);
	ASSERT_EQ(list.tail(), &a);
	CHECK_LIST(list, &c, &b, &a);

	list.insert_head(&d);
	ASSERT_EQ(list.head(), &d);
	ASSERT_EQ(list.tail(), &a);
	CHECK_LIST(list, &d, &c, &b, &a);
}

TEST(ListTest, InsertTail) {
	// Exercises the Xyz feature of Foo.
	MyList list;
	MyList::Iterator begin, end;

	TestStruct a, b, c, d;

	list.insert_tail(&a);

	ASSERT_EQ(list.head(), &a);
	ASSERT_EQ(list.tail(), &a);
	CHECK_LIST(list, &a);

	list.insert_tail(&b);

	ASSERT_EQ(list.head(), &a);
	ASSERT_EQ(list.tail(), &b);
	CHECK_LIST(list, &a, &b);

	list.insert_tail(&c);
	ASSERT_EQ(list.head(), &a);
	ASSERT_EQ(list.tail(), &c);
	CHECK_LIST(list, &a, &b, &c);

	list.insert_tail(&d);
	ASSERT_EQ(list.head(), &a);
	ASSERT_EQ(list.tail(), &d);
	CHECK_LIST(list, &a, &b, &c, &d);
}

TEST(ListTest, MixedInsert) {
	TestStruct a, b, c, d, e, f, g, h;
	MyList list;

	list.insert_head(&d);
	ASSERT_EQ(list.head(), &d);
	ASSERT_EQ(list.tail(), &d);
	CHECK_LIST(list, &d);

	list.insert_tail(&e);
	ASSERT_EQ(list.head(), &d);
	ASSERT_EQ(list.tail(), &e);
	CHECK_LIST(list, &d, &e);

	list.insert_head(&c);
	ASSERT_EQ(list.head(), &c);
	ASSERT_EQ(list.tail(), &e);
	CHECK_LIST(list, &c, &d, &e);

	list.insert_tail(&f);
	ASSERT_EQ(list.head(), &c);
	ASSERT_EQ(list.tail(), &f);
	CHECK_LIST(list, &c, &d, &e, &f);

	list.insert_head(&b);
	ASSERT_EQ(list.head(), &b);
	ASSERT_EQ(list.tail(), &f);
	CHECK_LIST(list, &b, &c, &d, &e, &f);

	list.insert_tail(&g);
	ASSERT_EQ(list.head(), &b);
	ASSERT_EQ(list.tail(), &g);
	CHECK_LIST(list, &b, &c, &d, &e, &f, &g);

	list.insert_head(&a);
	ASSERT_EQ(list.head(), &a);
	ASSERT_EQ(list.tail(), &g);
	CHECK_LIST(list, &a, &b, &c, &d, &e, &f, &g);

	list.insert_tail(&h);
	ASSERT_EQ(list.head(), &a);
	ASSERT_EQ(list.tail(), &h);
	CHECK_LIST(list, &a, &b, &c, &d, &e, &f, &g, &h);
}

TEST(ListTest, InsertAfter) {
	TestStruct a, b, c, d;
	MyList list;

	list.insert_head(&a);
	CHECK_LIST(list, &a);

	list.insertAfter(list.begin(), &b);
	CHECK_LIST(list, &a, &b);

	list.insert_tail(&d);
	CHECK_LIST(list, &a, &b, &d);

	list.insertAfter(&b, &c);
	CHECK_LIST(list, &a, &b, &c, &d);
}

/* Test that an element can be in multiple lists at the same time */
TEST(ListTest, MultipleLists) {
	TestStruct a, b, c, d;
	MyList list1;
	OtherList list2;

	list1.insert_head(&a);
	list1.insert_head(&b);
	list1.insert_head(&c);
	list1.insert_head(&d);
	CHECK_LIST(list1, &d, &c, &b, &a);

	list2.insert_tail(&a);
	list2.insert_tail(&b);
	list2.insert_tail(&c);
	list2.insert_tail(&d);
	CHECK_LIST(list1, &d, &c, &b, &a);
	CHECK_LIST(list2, &a, &b, &c, &d);
}
