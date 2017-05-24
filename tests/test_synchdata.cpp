/***************************************************************************
    copyright           : (C) 2007 by Lukas Lalinsky
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

#include <id3v2synchdata.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestID3v2SynchData)

BOOST_AUTO_TEST_CASE(test1)
{
  const ByteVector v("\x00\x00\x00\x7F", 4);

  BOOST_CHECK_EQUAL(ID3v2::SynchData::toUInt(v), 127);
  BOOST_CHECK_EQUAL(ID3v2::SynchData::fromUInt(127), v);
}

BOOST_AUTO_TEST_CASE(test2)
{
  const ByteVector v("\x00\x00\x01\x00", 4);
  
  BOOST_CHECK_EQUAL(ID3v2::SynchData::toUInt(v), 128);
  BOOST_CHECK_EQUAL(ID3v2::SynchData::fromUInt(128), v);
}

BOOST_AUTO_TEST_CASE(test3)
{
  const ByteVector v("\x00\x00\x01\x01", 4);

  BOOST_CHECK_EQUAL(ID3v2::SynchData::toUInt(v), 129);
  BOOST_CHECK_EQUAL(ID3v2::SynchData::fromUInt(129), v);
}

BOOST_AUTO_TEST_CASE(testToUIntBroken)
{
  const ByteVector data1("\x00\x00\x00\xFF", 4);
  const ByteVector data2("\x00\x00\xFF\xFF", 4);

  BOOST_CHECK_EQUAL(ID3v2::SynchData::toUInt(data1), 255);
  BOOST_CHECK_EQUAL(ID3v2::SynchData::toUInt(data2), 65535);
}

BOOST_AUTO_TEST_CASE(testToUIntBrokenAndTooLarge)
{
  const ByteVector data("\x00\x00\x00\xFF\x00", 5);

  BOOST_CHECK_EQUAL(ID3v2::SynchData::toUInt(data), 255);
}

BOOST_AUTO_TEST_CASE(testDecode1)
{
  const ByteVector a("\xff\x00\x00", 3);
  BOOST_CHECK_EQUAL(ID3v2::SynchData::decode(a), ByteVector("\xff\x00", 2));
}

BOOST_AUTO_TEST_CASE(testDecode2)
{
  const ByteVector a("\xff\x44", 2);
  BOOST_CHECK_EQUAL(ID3v2::SynchData::decode(a), ByteVector("\xff\x44", 2));
}

BOOST_AUTO_TEST_CASE(testDecode3)
{
  const ByteVector a("\xff\xff\x00", 3);
  BOOST_CHECK_EQUAL(ID3v2::SynchData::decode(a), ByteVector("\xff\xff", 2));
}

BOOST_AUTO_TEST_CASE(testDecode4)
{
  const ByteVector a("\xff\xff\xff", 3);
  BOOST_CHECK_EQUAL(ID3v2::SynchData::decode(a), ByteVector("\xff\xff\xff", 3));
}

BOOST_AUTO_TEST_SUITE_END()
