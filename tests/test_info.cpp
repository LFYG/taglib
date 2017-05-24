/***************************************************************************
    copyright           : (C) 2012 by Tsuda Kageyu
    email               : tsuda.kageyu@gmail.com
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

#include <infotag.h>
#include <boost/test/unit_test.hpp>
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestInfoTag)

BOOST_AUTO_TEST_CASE(testTitle)
{
  RIFF::Info::Tag tag;
  
  BOOST_CHECK(tag.title().isEmpty());
  tag.setTitle("Test title 1");
  tag.setFieldText("TEST", "Dummy Text");
  
  BOOST_CHECK_EQUAL(tag.title(), "Test title 1");
  
  const RIFF::Info::FieldListMap map = tag.fieldListMap();
  BOOST_CHECK_EQUAL(map["INAM"], "Test title 1");
  BOOST_CHECK_EQUAL(map["TEST"], "Dummy Text");
}

BOOST_AUTO_TEST_CASE(testNumericFields)
{
  RIFF::Info::Tag tag;
  
  BOOST_CHECK_EQUAL(tag.track(), 0);
  tag.setTrack(1234);
  BOOST_CHECK_EQUAL(tag.track(), 1234);
  BOOST_CHECK_EQUAL(tag.fieldText("IPRT"), "1234");
  
  BOOST_CHECK_EQUAL(tag.year(), 0);
  tag.setYear(1234);
  BOOST_CHECK_EQUAL(tag.year(), 1234);
  BOOST_CHECK_EQUAL(tag.fieldText("ICRD"), "1234");
}

BOOST_AUTO_TEST_SUITE_END()
