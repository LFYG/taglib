/***************************************************************************
    copyright           : (C) 2011 by Lukas Lalinsky
    email               : lukas@oxygene.sk
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include <tbytevectorstream.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestByteVectorStream)

BOOST_AUTO_TEST_CASE(testInitialData)
{
  ByteVectorStream stream("abcd");
  BOOST_CHECK_EQUAL(*stream.data(), "abcd");
}

BOOST_AUTO_TEST_CASE(testWriteBlock)
{
  ByteVectorStream stream("abcd");
  stream.seek(1);
  stream.writeBlock("xx");
  BOOST_CHECK_EQUAL(*stream.data(), "axxd");
}

BOOST_AUTO_TEST_CASE(testWriteBlockResize)
{
  ByteVectorStream stream("abcd");
  stream.seek(3);
  stream.writeBlock("xx");
  BOOST_CHECK_EQUAL(*stream.data(), "abcxx");
  stream.seek(5);
  stream.writeBlock("yy");
  BOOST_CHECK_EQUAL(*stream.data(), "abcxxyy");
}

BOOST_AUTO_TEST_CASE(testReadBlock)
{
  ByteVectorStream stream("abcd");
  BOOST_CHECK_EQUAL(stream.readBlock(1), "a");
  BOOST_CHECK_EQUAL(stream.readBlock(2), "bc");
  BOOST_CHECK_EQUAL(stream.readBlock(3), "d");
  BOOST_CHECK_EQUAL(stream.readBlock(3), "");
}

BOOST_AUTO_TEST_CASE(testRemoveBlock)
{
  ByteVectorStream stream("abcd");
  stream.removeBlock(1, 1);
  BOOST_CHECK_EQUAL(*stream.data(), "acd");
  stream.removeBlock(0, 2);
  BOOST_CHECK_EQUAL(*stream.data(), "d");
  stream.removeBlock(0, 2);
  BOOST_CHECK_EQUAL(*stream.data(), "");
}

BOOST_AUTO_TEST_CASE(testInsert)
{
  ByteVectorStream stream("abcd");
  stream.insert("xx", 1, 1);
  BOOST_CHECK_EQUAL(*stream.data(), "axxcd");
  stream.insert("yy", 0, 2);
  BOOST_CHECK_EQUAL(*stream.data(), "yyxcd");
  stream.insert("foa", 3, 2);
  BOOST_CHECK_EQUAL(*stream.data(), "yyxfoa");
  stream.insert("123", 3, 0);
  BOOST_CHECK_EQUAL(*stream.data(), "yyx123foa");
}

BOOST_AUTO_TEST_CASE(testSeekEnd)
{
  ByteVectorStream stream("abcdefghijklmnopqrstuvwxyz");
  BOOST_CHECK_EQUAL(stream.length(), 26);
  
  stream.seek(-4, IOStream::End);
  BOOST_CHECK_EQUAL(stream.readBlock(1), "w");
  
  stream.seek(-25, IOStream::End);
  BOOST_CHECK_EQUAL(stream.readBlock(1), "b");
}

BOOST_AUTO_TEST_SUITE_END()
