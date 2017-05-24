/***************************************************************************
    copyright           : (C) 2012 by Michael Helmling
    email               : helmling@mathematik.uni-kl.de
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

#include <tpropertymap.h>
#include <id3v1tag.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestPropertyMap)

BOOST_AUTO_TEST_CASE(testInvalidKeys)
{
  PropertyMap map1;
  BOOST_CHECK(map1.isEmpty());
  map1[L"\x00c4\x00d6\x00dc"].append("test");
  BOOST_CHECK_EQUAL(map1.size(), 1);
  
  PropertyMap map2;
  map2[L"\x00c4\x00d6\x00dc"].append("test");
  BOOST_CHECK(map1 == map2);
  BOOST_CHECK(map1.contains(map2));
  
  map2["ARTIST"] = String("Test Artist");
  BOOST_CHECK(map1 != map2);
  BOOST_CHECK(map2.contains(map1));
  
  map2[L"\x00c4\x00d6\x00dc"].append("test 2");
  BOOST_CHECK(!map2.contains(map1));
}

BOOST_AUTO_TEST_CASE(testGetSet)
{
  ID3v1::Tag tag;
  
  tag.setTitle("Test Title");
  tag.setArtist("Test Artist");
  tag.setAlbum("Test Album");
  tag.setYear(2015);
  tag.setTrack(10);
  
  {
    PropertyMap prop = tag.properties();
    BOOST_CHECK_EQUAL(prop["TITLE"      ].front(), "Test Title");
    BOOST_CHECK_EQUAL(prop["ARTIST"     ].front(), "Test Artist");
    BOOST_CHECK_EQUAL(prop["ALBUM"      ].front(), "Test Album");
    BOOST_CHECK_EQUAL(prop["DATE"       ].front(), "2015");
    BOOST_CHECK_EQUAL(prop["TRACKNUMBER"].front(), "10");
  
    prop["TITLE"      ].front() = "Test Title 2";
    prop["ARTIST"     ].front() = "Test Artist 2";
    prop["TRACKNUMBER"].front() = "5";
  
    tag.setProperties(prop);
  }
  
  BOOST_CHECK_EQUAL(tag.title(), "Test Title 2");
  BOOST_CHECK_EQUAL(tag.artist(), "Test Artist 2");
  BOOST_CHECK_EQUAL(tag.track(), 5);
  
  tag.setProperties(PropertyMap());
  
  BOOST_CHECK_EQUAL(tag.title(), "");
  BOOST_CHECK_EQUAL(tag.artist(), "");
  BOOST_CHECK_EQUAL(tag.track(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
