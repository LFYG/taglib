/***************************************************************************
    copyright           : (C) 2010 by Lukas Lalinsky
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

#include <apetag.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestAPETag)

BOOST_AUTO_TEST_CASE(testIsEmpty1)
{
  APE::Tag tag;
  BOOST_CHECK(tag.isEmpty());
  tag.addValue("COMPOSER", "Mike Oldfield");
  BOOST_CHECK(!tag.isEmpty());
}

BOOST_AUTO_TEST_CASE(testIsEmpty2)
{
  APE::Tag tag;
  BOOST_CHECK(tag.isEmpty());
  tag.setArtist("Mike Oldfield");
  BOOST_CHECK(!tag.isEmpty());
}

BOOST_AUTO_TEST_CASE(testPropertyInterface1)
{
  APE::Tag tag;
  PropertyMap dict = tag.properties();
  BOOST_CHECK(dict.isEmpty());
  dict["ARTIST"] = String("artist 1");
  dict["ARTIST"].append("artist 2");
  dict["TRACKNUMBER"].append("17");
  tag.setProperties(dict);
  BOOST_CHECK_EQUAL(tag.itemListMap()["TRACK"].values()[0], "17");
  BOOST_CHECK_EQUAL(tag.itemListMap()["ARTIST"].values().size(), 2);
  BOOST_CHECK_EQUAL(tag.artist(), "artist 1 artist 2");
  BOOST_CHECK_EQUAL(tag.track(), 17);
}

BOOST_AUTO_TEST_CASE(testPropertyInterface2)
{
  APE::Tag tag;
  APE::Item item1 = APE::Item("TRACK", "17");
  tag.setItem("TRACK", item1);
  
  APE::Item item2 = APE::Item();
  item2.setType(APE::Item::Binary);
  tag.setItem("TESTBINARY", item2);
  
  PropertyMap properties = tag.properties();
  BOOST_CHECK_EQUAL(properties.unsupportedData().size(), 1);
  BOOST_CHECK(properties.contains("TRACKNUMBER"));
  BOOST_CHECK(!properties.contains("TRACK"));
  BOOST_CHECK(tag.itemListMap().contains("TESTBINARY"));
  
  tag.removeUnsupportedProperties(properties.unsupportedData());
  BOOST_CHECK(!tag.itemListMap().contains("TESTBINARY"));
  
  APE::Item item3 = APE::Item("TRACKNUMBER", "29");
  tag.setItem("TRACKNUMBER", item3);
  properties = tag.properties();
  BOOST_CHECK_EQUAL(properties["TRACKNUMBER"].size(), 2);
  BOOST_CHECK_EQUAL(properties["TRACKNUMBER"][0], "17");
  BOOST_CHECK_EQUAL(properties["TRACKNUMBER"][1], "29");
}

BOOST_AUTO_TEST_CASE(testInvalidKeys)
{
  PropertyMap properties;
  properties["A"] = String("invalid key: one character");
  properties["MP+"] = String("invalid key: forbidden string");
  properties[L"\x1234\x3456"] = String("invalid key: Unicode");
  properties["A B~C"] = String("valid key: space and tilde");
  properties["ARTIST"] = String("valid key: normal one");
  
  APE::Tag tag;
  const PropertyMap unsuccessful = tag.setProperties(properties);
  BOOST_CHECK_EQUAL(unsuccessful.size(), 3);
  BOOST_CHECK(unsuccessful.contains("A"));
  BOOST_CHECK(unsuccessful.contains("MP+"));
  BOOST_CHECK(unsuccessful.contains(L"\x1234\x3456"));
  
  BOOST_CHECK_EQUAL(tag.itemListMap().size(), 2);
  tag.addValue("VALID KEY", "Test Value 1");
  tag.addValue("INVALID KEY \x7f", "Test Value 2");
  tag.addValue(L"INVALID KEY \x1234\x3456", "Test Value 3");
  BOOST_CHECK_EQUAL(tag.itemListMap().size(), 3);
}

BOOST_AUTO_TEST_CASE(testTextBinary)
{
  APE::Item item = APE::Item("DUMMY", "Test Text");
  BOOST_CHECK_EQUAL(item.toString(), "Test Text");
  BOOST_CHECK(item.binaryData().isEmpty());
  
  ByteVector data("Test Data");
  item.setBinaryData(data);
  BOOST_CHECK(item.values().isEmpty());
  BOOST_CHECK(item.toString().isEmpty());
  BOOST_CHECK_EQUAL(item.binaryData(), data);
  
  item.setValue("Test Text 2");
  BOOST_CHECK_EQUAL(item.toString(), "Test Text 2");
  BOOST_CHECK(item.binaryData().isEmpty());
}

BOOST_AUTO_TEST_SUITE_END()
